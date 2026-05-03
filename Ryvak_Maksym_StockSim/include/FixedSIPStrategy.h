#ifndef FIXED_SIP_STRATEGY_H
#define FIXED_SIP_STRATEGY_H

#include "TradingStrategy.h"

// Fixed Systematic Investment Plan (SIP): invest a fixed dollar amount
// on the first available trading day of every calendar month, regardless
// of market conditions. This is the baseline strategy.
//
// Backtest logic:
//   For each month in [startYear, endYear]:
//     Find the first PriceNode whose month matches.
//     Buy (monthlyCapital / closePrice) fractional shares at that close price.
//     Record the portfolio value snapshot for drawdown calculation.
//
// Course module: OOP (concrete derived class, override)
class FixedSIPStrategy : public TradingStrategy {
public:
    FixedSIPStrategy() {}

    SimResult backtest(PriceHistory* history,
                       double monthlyCapital,
                       int startYear,
                       int endYear) override;

    string getName() const override;
};

#endif // FIXED_SIP_STRATEGY_H
