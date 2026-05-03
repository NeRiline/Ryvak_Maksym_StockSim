#ifndef GOLDEN_CROSS_STRATEGY_H
#define GOLDEN_CROSS_STRATEGY_H

#include "TradingStrategy.h"
#include "CircularQueue.h"

// Golden Cross / Death Cross strategy using two moving averages:
//   - Buy signal:  50-day MA crosses ABOVE 200-day MA ("Golden Cross")
//   - Sell signal: 50-day MA crosses BELOW 200-day MA ("Death Cross")
//
// Backtest logic (iterate day by day):
//   1. Enqueue today's close into both ma50Queue and ma200Queue.
//   2. Both queues must be full before generating signals
//      (need at least 200 trading days of data).
//   3. Detect crossover by comparing prev_ma50 vs prev_ma200 with curr_ma50 vs curr_ma200.
//   4. On Golden Cross: invest all available cash at today's close.
//   5. On Death Cross:  sell all shares at today's close, hold cash.
//
// Uses CircularQueue for BOTH moving averages — this is the key integration point.
//
// Course modules: OOP (derived class), Queues (CircularQueue in practice)
class GoldenCrossStrategy : public TradingStrategy {
private:
    int shortWindow;   // default: 50
    int longWindow;    // default: 200

public:
    GoldenCrossStrategy(int shortWindow = 50, int longWindow = 200);

    SimResult backtest(PriceHistory* history,
                       double monthlyCapital,
                       int startYear,
                       int endYear) override;

    string getName() const override;
};

#endif // GOLDEN_CROSS_STRATEGY_H
