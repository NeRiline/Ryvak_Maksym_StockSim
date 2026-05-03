#ifndef MOMENTUM_STRATEGY_H
#define MOMENTUM_STRATEGY_H

#include "TradingStrategy.h"

// 6-Month Momentum Strategy: invest when recent price trend is positive,
// stay in cash when the trend turns negative.
//
// Backtest logic (evaluated at the start of each month):
//   1. Use the ReverseIterator on PriceHistory to scan backward ~126 trading days
//      (approx. 6 months) from the current date.
//   2. Compute 6-month trailing return: (current_price - price_6mo_ago) / price_6mo_ago * 100
//   3. If trailing return > momentumThreshold: buy / stay invested (invest monthlyCapital)
//   4. If trailing return < 0: sell all holdings, hold cash
//   5. Otherwise (0 to threshold): hold current position, no new investment
//
// The ReverseIterator integration is the key requirement here.
//
// Course modules: OOP (derived class), Iterators (ReverseIterator in practice)
class MomentumStrategy : public TradingStrategy {
private:
    double momentumThreshold;  // minimum trailing return to trigger a buy (e.g., 5.0 for 5%)
    int    lookbackDays;       // trading days to look back (default: 126 ≈ 6 months)

public:
    MomentumStrategy(double momentumThreshold = 5.0, int lookbackDays = 126);

    SimResult backtest(PriceHistory* history,
                       double monthlyCapital,
                       int startYear,
                       int endYear) override;

    string getName() const override;
};

#endif // MOMENTUM_STRATEGY_H
