#include "DynamicSIPStrategy.h"
#include "CSVParser.h"
#include <iomanip> // For output formatting
#include <sstream> // For parameter formatting
#include <vector>

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

            double planned = monthlyCapital;
            if (dipPct >= dipThreshold) {
                planned = monthlyCapital * multiplier;
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
