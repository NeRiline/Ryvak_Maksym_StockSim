#ifndef ETF_H
#define ETF_H

#include "Stock.h"
#include <string>
using namespace std;

// Represents an Exchange-Traded Fund (e.g., SPY tracking the S&P 500).
// Inherits from Stock — uses the same PriceHistory infrastructure.
// Adds expense ratio and overrides printSummary with ETF-specific stats.
//
// Course module: OOP (multi-level inheritance, override)
class ETF : public Stock {
private:
    double expenseRatio;  // annual fee as a decimal (e.g., 0.0003 for 0.03%)

public:
    ETF(const string& ticker, const string& name, const string& sector, double expenseRatio);
    ~ETF() override;

    double getExpenseRatio() const;
    void   setExpenseRatio(double ratio);

    // Computes compound annual growth rate over the last 10 years of available data.
    // CAGR formula: (endValue / startValue)^(1/years) - 1
    // Returns 0.0 if insufficient data.
    double calculate10YearCAGR() const;

    // --- FinancialAsset overrides ---

    // Prints everything Stock::printSummary prints, plus:
    //   Expense Ratio: 0.03%
    //   10-Year CAGR : 12.4%
    void printSummary() const override;

    // Returns "ETF"
    string getType() const override;
};

#endif // ETF_H
