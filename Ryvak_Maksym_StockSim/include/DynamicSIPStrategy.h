#ifndef DYNAMIC_SIP_STRATEGY_H
#define DYNAMIC_SIP_STRATEGY_H

#include "TradingStrategy.h"

// Dynamic SIP: adjusts monthly investment amount based on market conditions.
// The core research question: does investing more during dips outperform Fixed SIP?
//
// Rules (all comparisons against the 12-month rolling high/low):
//   - If current price is down >= dipThreshold% from 12-month high:
//       invest multiplier * monthlyCapital  (buy the dip)
//   - If current price is up >= rallyThreshold% from 12-month low:
//       invest 0.5 * monthlyCapital         (reduce exposure on rally)
//   - Otherwise: invest monthlyCapital      (normal month)
//
// Budget constraint: annual capital deployed must equal 12 * monthlyCapital.
//   If the strategy over-deploys one month, it should reduce a future month.
//   Hint: track cumulative deployed vs. budget and cap monthly spend accordingly.
//
// Course module: OOP (concrete derived class), open-ended algorithm design
class DynamicSIPStrategy : public TradingStrategy {
private:
    double dipThreshold;    // % drop from 12-month high that triggers increased investment
    double rallyThreshold;  // % rise from 12-month low that triggers reduced investment
    double multiplier;      // how many times monthlyCapital to invest on a dip (e.g., 2.0)

public:
    DynamicSIPStrategy(double dipThreshold, double rallyThreshold, double multiplier);

    SimResult backtest(PriceHistory* history,
                       double monthlyCapital,
                       int startYear,
                       int endYear) override;

    string getName() const override;

    // Accessors (used by the parameter sweep bonus)
    double getDipThreshold()   const;
    double getRallyThreshold() const;
    double getMultiplier()     const;
};

#endif // DYNAMIC_SIP_STRATEGY_H
