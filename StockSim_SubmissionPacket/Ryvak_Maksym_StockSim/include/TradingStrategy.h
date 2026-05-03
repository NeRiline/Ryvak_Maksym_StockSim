#ifndef TRADING_STRATEGY_H
#define TRADING_STRATEGY_H

#include "PriceHistory.h"
#include <string>
#include <vector>
using namespace std;

// Holds the results of a single strategy backtest run.
struct SimResult {
    string strategyName;
    double finalValue;      // final portfolio value at end of simulation
    double totalInvested;   // total cash put into the market
    double totalReturn;     // (finalValue - totalInvested) / totalInvested * 100
    double cagr;            // Compound Annual Growth Rate (%)
    double maxDrawdown;     // worst peak-to-trough decline (%)
    int    totalTrades;     // number of buy/sell executions
};

// Abstract base class for all investment strategy simulators.
// Each derived class implements a different algorithm on the same PriceHistory data.
//
// Course module: OOP (abstract class, polymorphism)
class TradingStrategy {
public:
    virtual ~TradingStrategy() {}

    // Run a full backtest over [startYear, endYear] using the given price history.
    // monthlyCapital: dollars invested per month (used by SIP-style strategies).
    // Returns a SimResult with all computed statistics.
    virtual SimResult backtest(PriceHistory* history,
                               double monthlyCapital,
                               int startYear,
                               int endYear) = 0;

    // Returns a human-readable name for the strategy (e.g., "Fixed SIP").
    virtual string getName() const = 0;

    // Prints a formatted SimResult to stdout.
    // Provided (non-virtual) — all strategies share the same output format.
    void printResult(const SimResult& result) const;

protected:
    // Helper: CAGR = (endVal / startVal)^(1/years) - 1, returned as a percentage.
    // Returns 0.0 if years <= 0 or startVal <= 0.
    double calculateCAGR(double startVal, double endVal, int years) const;

    // Helper: given a time series of portfolio values (one per month or day),
    // returns the maximum peak-to-trough drawdown as a positive percentage.
    double calculateMaxDrawdown(const vector<double>& portfolioValues) const;
};

#endif // TRADING_STRATEGY_H
