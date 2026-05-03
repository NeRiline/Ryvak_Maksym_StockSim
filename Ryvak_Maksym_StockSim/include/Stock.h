#ifndef STOCK_H
#define STOCK_H

#include "FinancialAsset.h"
#include "PriceHistory.h"
#include <string>
using namespace std;

// Represents a single publicly traded stock (e.g., AAPL, TSLA).
// Inherits from FinancialAsset and owns a PriceHistory linked list.
//
// Course modules covered: OOP (inheritance, polymorphism), Linked List
class Stock : public FinancialAsset {
protected:
    PriceHistory* history;  // owns this — must delete in destructor

public:
    Stock(const string& ticker, const string& name, const string& sector);
    virtual ~Stock();  // must delete history

    // Loads price data from a Yahoo Finance CSV file.
    // Delegates parsing to CSVParser::loadHistory().
    // Should print the number of trading days loaded.
    bool loadFromCSV(const string& filename);

    // Returns raw pointer to the price history (does NOT transfer ownership)
    PriceHistory* getHistory() const;

    // Returns the closing price on the given date ("YYYY-MM-DD").
    // Returns -1.0 if the date is not found in the history.
    double getPriceOnDate(const string& date) const;

    // Returns the first trading day's close price in the given year.
    // Returns -1.0 if no data exists for that year.
    double getYearStartPrice(int year) const;

    // Returns the last trading day's close price in the given year.
    // Returns -1.0 if no data exists for that year.
    double getYearEndPrice(int year) const;

    // --- FinancialAsset overrides ---

    // Computes: (yearEndPrice - yearStartPrice) / yearStartPrice * 100
    // Returns 0.0 if data is unavailable for the given year.
    double calculateAnnualReturn(int year) const override;

    // Prints: ticker, full name, sector, date range loaded,
    //         total trading days, and overall return (first to last price).
    void printSummary() const override;

    // Returns "Stock"
    string getType() const override;
};

#endif // STOCK_H
