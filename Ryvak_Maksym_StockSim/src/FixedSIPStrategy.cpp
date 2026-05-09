#include "FixedSIPStrategy.h"
#include "CSVParser.h"

using namespace std;

SimResult FixedSIPStrategy::backtest(PriceHistory* history,
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

            if (cash > 0.0) {
                shares += cash / node.close;
                cash = 0.0;
                totalTrades++;
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

string FixedSIPStrategy::getName() const {
    return "Fixed SIP";
}
