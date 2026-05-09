#include "Stock.h"
#include "CSVParser.h"
#include <iostream>
#include <iomanip>

Stock::Stock(const string& ticker, const string& name, const string& sector)
    : FinancialAsset(ticker, name, sector), history(nullptr) {}

Stock::~Stock() {
    delete history;
    history = nullptr;
}

bool Stock::loadFromCSV(const string& filename) {
    PriceHistory* loaded = CSVParser::loadHistory(filename);
    if (!loaded) {
        return false;
    }

    delete history;
    history = loaded;

    cout << ticker << ": loaded " << history->getSize() << " trading days.\n";
    return true;
}

PriceHistory* Stock::getHistory() const {
    return history;
}

double Stock::getPriceOnDate(const string& date) const {
    if (!history) {
        return -1.0;
    }
    PriceNode* node = history->findByDate(date);
    return node ? node->close : -1.0;
}

double Stock::getYearStartPrice(int year) const {
    if (!history) {
        return -1.0;
    }
    for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) {
        PriceNode& node = *it;
        if (CSVParser::extractYear(node.date) == year) {
            return node.close;
        }
    }
    return -1.0;
}

double Stock::getYearEndPrice(int year) const {
    if (!history) {
        return -1.0;
    }
    for (PriceHistory::ReverseIterator it = history->rbegin(); it != history->rend(); ++it) {
        PriceNode& node = *it;
        if (CSVParser::extractYear(node.date) == year) {
            return node.close;
        }
    }
    return -1.0;
}

double Stock::calculateAnnualReturn(int year) const {
    double startPrice = getYearStartPrice(year);
    double endPrice = getYearEndPrice(year);
    if (startPrice <= 0.0 || endPrice <= 0.0) {
        return 0.0;
    }
    return (endPrice - startPrice) / startPrice * 100.0;
}

void Stock::printSummary() const {
    cout << "Ticker: " << ticker << "\n";
    cout << "Name  : " << name << "\n";
    cout << "Sector: " << sector << "\n";

    if (!history || history->getSize() == 0) {
        cout << "No price history loaded.\n";
        return;
    }

    PriceNode* head = history->getHead();
    PriceNode* tail = history->getTail();
    double overallReturn = 0.0;
    if (head && tail && head->close > 0.0) {
        overallReturn = (tail->close - head->close) / head->close * 100.0;
    }

    cout << "Date Range: " << head->date << " to " << tail->date << "\n";
    cout << "Trading Days: " << history->getSize() << "\n";
    cout << "Overall Return: " << fixed << setprecision(2) << overallReturn << "%\n";
}

string Stock::getType() const {
    return "Stock";
}
