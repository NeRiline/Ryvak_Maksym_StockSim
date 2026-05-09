#include "DynamicSIPStrategy.h"
#include "CSVParser.h"
#include <iomanip> // For output formatting
#include <sstream> // For parameter formatting
#include <vector>
#include <cmath>
#include <algorithm> // For std::max and std::min, which are used for calculating drawdown and other metrics

using namespace std;

DynamicSIPStrategy::DynamicSIPStrategy(double dipThreshold, double rallyThreshold, double multiplier)
    : dipThreshold(dipThreshold), rallyThreshold(rallyThreshold), multiplier(multiplier) {}

SimResult DynamicSIPStrategy::backtest(PriceHistory* history,
                                      double monthlyCapital,
                                      int startYear,
                                      int endYear) {
    SimResult result{};
    result.strategyName = getName();

    if (!history || monthlyCapital <= 0.0) {
        return result;
    }

    struct YearInfo {
        int year;
        int lastMonth;
    };

    vector<YearInfo> yearInfo;
    int lastYear = -1;
    int lastMonth = -1;
    for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) {
        PriceNode& node = *it;
        int year = CSVParser::extractYear(node.date);
        int month = CSVParser::extractMonth(node.date);
        if (year < startYear || year > endYear) {
            continue;
        }
        if (year != lastYear) {
            if (lastYear != -1) {
                yearInfo.push_back(YearInfo{lastYear, lastMonth});
            }
            lastYear = year;
            lastMonth = month;
        } else {
            lastMonth = month;
        }
    }
    if (lastYear != -1) {
        yearInfo.push_back(YearInfo{lastYear, lastMonth});
    }

    auto lastMonthForYear = [&yearInfo](int year) -> int {
        for (size_t i = 0; i < yearInfo.size(); ++i) {
            if (yearInfo[i].year == year) {
                return yearInfo[i].lastMonth;
            }
        }
        return -1;
    };

    struct RollingWindow {
        vector<double> values;
        int capacity;
        int count;
        int head;

        explicit RollingWindow(int cap)
            : values(cap, 0.0), capacity(cap), count(0), head(0) {}

        void push(double value) {
            values[head] = value;
            head = (head + 1) % capacity;
            if (count < capacity) {
                ++count;
            }
        }

        bool empty() const {
            return count == 0;
        }

        void minMax(double& high, double& low) const {
            int oldest = (head - count + capacity) % capacity;
            high = values[oldest];
            low = values[oldest];
            for (int i = 1; i < count; ++i) {
                int idx = (oldest + i) % capacity;
                double value = values[idx];
                if (value > high) {
                    high = value;
                }
                if (value < low) {
                    low = value;
                }
            }
        }
    };

    RollingWindow trailingCloses(252);

    double shares = 0.0;
    double cash = 0.0;
    double totalContributed = 0.0;
    int totalTrades = 0;
    vector<double> portfolioValues;

    int currentYear = -1;
    int currentMonth = -1;
    double lastPrice = 0.0;

    for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) {
        PriceNode& node = *it;
        trailingCloses.push(node.close);

        int year = CSVParser::extractYear(node.date);
        int month = CSVParser::extractMonth(node.date);
        if (year < startYear || year > endYear) {
            continue;
        }

        if (year != currentYear || month != currentMonth) {
            cash += monthlyCapital;
            totalContributed += monthlyCapital;

            double high = 0.0;
            double low = 0.0;
            if (!trailingCloses.empty()) {
                trailingCloses.minMax(high, low);
            }

            double dipPct = 0.0;
            double rallyPct = 0.0;
            if (high > 0.0) {
                dipPct = (high - node.close) / high * 100.0;
            }
            if (low > 0.0) {
                rallyPct = (node.close - low) / low * 100.0;
            }

            // --- Trend filter and adaptive multiplier ---
            // Compute simple indicators from the trailing window when available
            auto getNthFromLast = [&](int n) -> double {
                // n = 0 => most recent (current pushed value), n = 1 => 1-day ago, etc.
                if (trailingCloses.count <= n) return 0.0;
                int idx = (trailingCloses.head - 1 - n + trailingCloses.capacity) % trailingCloses.capacity;
                return trailingCloses.values[idx];
            };

            auto computeMA = [&](int days) -> double {
                if (trailingCloses.count < days) return 0.0;
                double sum = 0.0;
                for (int i = 0; i < days; ++i) {
                    sum += getNthFromLast(i);
                }
                return sum / days;
            };

            // 50-day and 200-day moving averages (approx trading days)
            double ma50 = computeMA(50);
            double ma200 = computeMA(200);

            // 6-month return (~126 trading days)
            double sixMonthPrice = getNthFromLast(126);
            double sixMonthReturn = 0.0;
            if (sixMonthPrice > 0.0) {
                sixMonthReturn = (node.close - sixMonthPrice) / sixMonthPrice * 100.0;
            }

            // Volatility: stddev of daily returns over last 63 days (~quarter)
            double vol = 0.0; // annualized volatility
            int volWindow = 63;
            if (trailingCloses.count > volWindow) {
                double sumR = 0.0;
                double sumR2 = 0.0;
                int actual = 0;
                double prev = getNthFromLast(volWindow);
                for (int i = volWindow - 1; i >= 0; --i) {
                    double today = getNthFromLast(i);
                    if (prev > 0.0) {
                        double r = (today - prev) / prev;
                        sumR += r;
                        sumR2 += r * r;
                        ++actual;
                    }
                    prev = today;
                }
                if (actual > 1) {
                    double mean = sumR / actual;
                    double variance = (sumR2 - actual * mean * mean) / (actual - 1);
                    if (variance < 0.0) variance = 0.0;
                    double dailyStd = sqrt(variance);
                    vol = dailyStd * sqrt(252.0);
                }
            }

            // Determine broader trend: positive if ANY of these are true
            bool positiveTrend = false;
            if (ma200 > 0.0 && node.close > ma200) positiveTrend = true;
            if (ma50 > 0.0 && ma200 > 0.0 && ma50 > ma200) positiveTrend = true;
            if (sixMonthReturn > 0.0) positiveTrend = true;

            // Long downtrend detection: is 200-day MA falling vs 30 days ago?
            bool longDowntrend = false;
            if (trailingCloses.count > 230) {
                // compare current 200MA to 200MA 30 days ago
                // compute 200MA 30 days ago by averaging values from 230..259 ago (rough proxy)
                double sumOld = 0.0;
                int countOld = 0;
                for (int i = 230; i < 230 + 30 && i < trailingCloses.count; ++i) {
                    sumOld += getNthFromLast(i);
                    ++countOld;
                }
                if (countOld == 30) {
                    double ma200_30 = sumOld / countOld; // rough proxy
                    if (ma200 > 0.0 && ma200 < ma200_30) longDowntrend = true;
                }
            }

            double planned = monthlyCapital;

            if (dipPct >= dipThreshold) {
                // Only apply aggressive multiplier when the broader trend is positive
                if (!positiveTrend) {
                    // trend negative: fall back to fixed SIP (no extra multiplier)
                    planned = monthlyCapital;
                } else {
                    // adaptive multiplier: scale with dip depth and temper with volatility
                    double base = multiplier;

                    // Dip depth factor (how deep relative to threshold)
                    double depthRatio = 1.0;
                    if (dipThreshold > 0.0) {
                        depthRatio = dipPct / dipThreshold;
                    } else {
                        depthRatio = 1.0;
                    }
                    if (depthRatio < 1.0) depthRatio = 1.0;
                    if (depthRatio > 3.0) depthRatio = 3.0;

                    double depthScale = 1.0 + 0.5 * (depthRatio - 1.0); // modest scaling

                    // Volatility scaling: prefer moderate volatility, reduce when extreme
                    double volScale = 1.0;
                    if (vol <= 0.15) volScale = 1.2;        // low vol => slightly more aggressive
                    else if (vol <= 0.30) volScale = 1.0;   // normal
                    else if (vol <= 0.50) volScale = 0.8;   // high vol => reduce
                    else volScale = 0.6;                    // extreme vol => conservative

                    double adaptive = base * depthScale * volScale;

                    // Cap adaptive multiplier in long downtrend
                    if (longDowntrend) {
                        adaptive = min(adaptive, base * 1.5);
                    }

                    // Absolute caps
                    if (adaptive < 1.0) adaptive = 1.0;
                    if (adaptive > 6.0) adaptive = 6.0;

                    planned = monthlyCapital * adaptive;
                }
            } else if (rallyPct >= rallyThreshold) {
                planned = monthlyCapital * 0.5;
            }

            int lastMonthInYear = lastMonthForYear(year);
            double invest = planned;
            if (month == lastMonthInYear) {
                invest = cash;
            } else if (invest > cash) {
                invest = cash;
            }

            if (invest > 0.0) {
                shares += invest / node.close;
                cash -= invest;
                totalTrades++;
            }

            currentYear = year;
            currentMonth = month;
        }

        lastPrice = node.close;
        portfolioValues.push_back(cash + shares * node.close);
    }

    double finalValue = cash + shares * lastPrice;
    result.finalValue = finalValue;
    result.totalInvested = totalContributed;
    if (totalContributed > 0.0) {
        result.totalReturn = (finalValue - totalContributed) / totalContributed * 100.0;
    }
    int years = endYear - startYear + 1;
    result.cagr = calculateCAGR(totalContributed, finalValue, years);
    result.maxDrawdown = calculateMaxDrawdown(portfolioValues);
    result.totalTrades = totalTrades;
    return result;
}

string DynamicSIPStrategy::getName() const {
    ostringstream out;
    out << fixed << setprecision(2)
        << "Dynamic SIP (dip=" << dipThreshold
        << "%, rally=" << rallyThreshold
        << "%, mult=" << multiplier << ")";
    return out.str();
}

double DynamicSIPStrategy::getDipThreshold() const { return dipThreshold; }

double DynamicSIPStrategy::getRallyThreshold() const { return rallyThreshold; }

double DynamicSIPStrategy::getMultiplier() const { return multiplier; }
