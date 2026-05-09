#include "ETF.h"
#include "CSVParser.h"
#include <iostream>
#include <iomanip>
#include <cmath>

ETF::ETF(const string& ticker, const string& name, const string& sector, double expenseRatio)
    : Stock(ticker, name, sector), expenseRatio(expenseRatio) {}

ETF::~ETF() {}

double ETF::getExpenseRatio() const { return expenseRatio; }

void ETF::setExpenseRatio(double ratio) { expenseRatio = ratio; }

double ETF::calculate10YearCAGR() const {
    if (!history || history->getSize() == 0) {
        return 0.0;
    }
    PriceNode* tail = history->getTail();
    if (!tail) {
        return 0.0;
    }
    int endYear = CSVParser::extractYear(tail->date);
    int startYear = endYear - 10;

    double startPrice = getYearStartPrice(startYear);
    double endPrice = getYearEndPrice(endYear);
    if (startPrice <= 0.0 || endPrice <= 0.0) {
        return 0.0;
    }

    double cagr = pow(endPrice / startPrice, 1.0 / 10.0) - 1.0;
    return cagr * 100.0;
}

void ETF::printSummary() const {
    Stock::printSummary();
    cout << "Expense Ratio: " << fixed << setprecision(2) << (expenseRatio * 100.0) << "%\n";
    cout << "10-Year CAGR : " << fixed << setprecision(2) << calculate10YearCAGR() << "%\n";
}

string ETF::getType() const {
    return "ETF";
}
