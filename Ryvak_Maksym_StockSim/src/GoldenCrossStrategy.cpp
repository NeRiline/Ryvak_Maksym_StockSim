#include "GoldenCrossStrategy.h"
#include "CSVParser.h"

using namespace std;

GoldenCrossStrategy::GoldenCrossStrategy(int shortWindow, int longWindow)
    : shortWindow(shortWindow), longWindow(longWindow) {}

SimResult GoldenCrossStrategy::backtest(PriceHistory* history,
                                       double monthlyCapital,
                                       int startYear,
                                       int endYear) {
    SimResult result{};
    result.strategyName = getName();

    if (!history || monthlyCapital <= 0.0) {
        return result;
    }

    CircularQueue maShort(shortWindow);
    CircularQueue maLong(longWindow);

    double prevShort = 0.0;
    double prevLong = 0.0;
    bool hasPrev = false;

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

        maShort.enqueue(node.close);
        maLong.enqueue(node.close);

        bool inRange = (year >= startYear && year <= endYear);
        if (inRange) {
            if (year != lastYear || month != lastMonth) {
                cash += monthlyCapital;
                totalContributed += monthlyCapital;
                lastYear = year;
                lastMonth = month;
            }
        }

        if (maShort.isFull() && maLong.isFull()) {
            double currShort = maShort.getAverage();
            double currLong = maLong.getAverage();

            if (hasPrev && inRange) {
                if (prevShort <= prevLong && currShort > currLong) {
                    if (cash > 0.0) {
                        shares += cash / node.close;
                        cash = 0.0;
                        totalTrades++;
                    }
                } else if (prevShort >= prevLong && currShort < currLong) {
                    if (shares > 0.0) {
                        cash += shares * node.close;
                        shares = 0.0;
                        totalTrades++;
                    }
                }
            }

            prevShort = currShort;
            prevLong = currLong;
            hasPrev = true;
        }

        if (inRange) {
            lastPrice = node.close;
            portfolioValues.push_back(cash + shares * node.close);
        }
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

string GoldenCrossStrategy::getName() const {
    return "Golden Cross (50/200 MA)";
}
