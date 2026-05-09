#include "MomentumStrategy.h"
#include "CSVParser.h"

using namespace std;

MomentumStrategy::MomentumStrategy(double momentumThreshold, int lookbackDays)
    : momentumThreshold(momentumThreshold), lookbackDays(lookbackDays) {}

SimResult MomentumStrategy::backtest(PriceHistory* history,
                                    double monthlyCapital,
                                    int startYear,
                                    int endYear) {
    SimResult result{};
    result.strategyName = getName();

    if (!history || monthlyCapital <= 0.0) {
        return result;
    }

    double shares = 0.0;
    double cash = 0.0;
    double totalContributed = 0.0;
    int totalTrades = 0;
    vector<double> portfolioValues;

    int lastYear = -1;
    int lastMonth = -1;
    double lastPrice = 0.0;

    for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) {
        PriceNode& node = *it;
        int year = CSVParser::extractYear(node.date);
        int month = CSVParser::extractMonth(node.date);

        if (year < startYear || year > endYear) {
            continue;
        }

        if (year != lastYear || month != lastMonth) {
            cash += monthlyCapital;
            totalContributed += monthlyCapital;

            PriceHistory::ReverseIterator rit(&node);
            int steps = 0;
            while (rit != history->rend() && steps < lookbackDays) {
                ++rit;
                steps++;
            }

            if (rit != history->rend()) {
                PriceNode& past = *rit;
                double pastPrice = past.close;
                if (pastPrice > 0.0) {
                    double trailingReturn = (node.close - pastPrice) / pastPrice * 100.0;

                    if (trailingReturn > momentumThreshold) {
                        if (cash > 0.0) {
                            shares += cash / node.close;
                            cash = 0.0;
                            totalTrades++;
                        }
                    } else if (trailingReturn < 0.0) {
                        if (shares > 0.0) {
                            cash += shares * node.close;
                            shares = 0.0;
                            totalTrades++;
                        }
                    }
                }
            }

            lastYear = year;
            lastMonth = month;
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

string MomentumStrategy::getName() const {
    return "6-Month Momentum";
}
