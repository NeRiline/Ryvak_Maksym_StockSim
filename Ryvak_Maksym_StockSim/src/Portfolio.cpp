#include "Portfolio.h"
#include <algorithm>
#include <iostream>
#include <iomanip>

Portfolio::Portfolio(const string& ownerName, double initialCash)
    : cashBalance(initialCash), ownerName(ownerName) {}

void Portfolio::buyShares(const string& ticker, int shares, double price, const string& date) {
    if (shares <= 0 || price <= 0.0) {
        cout << "Invalid buy order.\n";
        return;
    }

    double totalCost = price * shares;
    if (totalCost > cashBalance) {
        cout << "Insufficient cash for this purchase.\n";
        return;
    }

    cashBalance -= totalCost;

    for (size_t i = 0; i < holdings.size(); ++i) {
        if (holdings[i].ticker == ticker) {
            double oldTotalCost = holdings[i].avgCostBasis * holdings[i].shares;
            double newTotalCost = oldTotalCost + totalCost;
            holdings[i].shares += shares;
            holdings[i].avgCostBasis = newTotalCost / holdings[i].shares;
            holdings[i].currentPrice = price;

            TradeRecord record{ticker, date, price, shares, "BUY", totalCost};
            tradeHistory.push(record);
            return;
        }
    }

    Position pos{ticker, shares, price, price};
    holdings.push_back(pos);

    TradeRecord record{ticker, date, price, shares, "BUY", totalCost};
    tradeHistory.push(record);
}

void Portfolio::sellShares(const string& ticker, int shares, double price, const string& date) {
    if (shares <= 0 || price <= 0.0) {
        cout << "Invalid sell order.\n";
        return;
    }

    for (size_t i = 0; i < holdings.size(); ++i) {
        if (holdings[i].ticker == ticker) {
            if (shares > holdings[i].shares) {
                cout << "Not enough shares to sell.\n";
                return;
            }

            double proceeds = price * shares;
            cashBalance += proceeds;
            holdings[i].shares -= shares;
            holdings[i].currentPrice = price;

            if (holdings[i].shares == 0) {
                holdings.erase(holdings.begin() + static_cast<long>(i));
            }

            TradeRecord record{ticker, date, price, shares, "SELL", -proceeds};
            tradeHistory.push(record);
            return;
        }
    }

    cout << "Position not found for ticker: " << ticker << "\n";
}

void Portfolio::undoLastTrade() {
    if (tradeHistory.isEmpty()) {
        cout << "No trades to undo.\n";
        return;
    }

    TradeRecord record = tradeHistory.pop();
    if (record.action == "BUY") {
        // Reverse a buy by selling the same shares at the same price.
        double proceeds = record.price * record.shares;
        cashBalance += proceeds;

        for (size_t i = 0; i < holdings.size(); ++i) {
            if (holdings[i].ticker == record.ticker) {
                int newShares = holdings[i].shares - record.shares;
                if (newShares <= 0) {
                    holdings.erase(holdings.begin() + static_cast<long>(i));
                } else {
                    double totalCostAfter = holdings[i].avgCostBasis * holdings[i].shares;
                    double totalCostBefore = totalCostAfter - (record.price * record.shares);
                    holdings[i].shares = newShares;
                    holdings[i].avgCostBasis = totalCostBefore / newShares;
                }
                cout << "Undid BUY of " << record.shares << " shares of " << record.ticker << ".\n";
                return;
            }
        }
    } else if (record.action == "SELL") {
        // Reverse a sell by buying the shares back at the same price.
        double totalCost = record.price * record.shares;
        if (totalCost > cashBalance) {
            cout << "Insufficient cash to undo sell trade.\n";
            return;
        }
        cashBalance -= totalCost;

        for (size_t i = 0; i < holdings.size(); ++i) {
            if (holdings[i].ticker == record.ticker) {
                double oldTotalCost = holdings[i].avgCostBasis * holdings[i].shares;
                double newTotalCost = oldTotalCost + totalCost;
                holdings[i].shares += record.shares;
                holdings[i].avgCostBasis = newTotalCost / holdings[i].shares;
                holdings[i].currentPrice = record.price;
                cout << "Undid SELL of " << record.shares << " shares of " << record.ticker << ".\n";
                return;
            }
        }

        Position pos{record.ticker, record.shares, record.price, record.price};
        holdings.push_back(pos);
        cout << "Undid SELL of " << record.shares << " shares of " << record.ticker << ".\n";
    }
}

void Portfolio::queueOrder(const Order& order) {
    pendingOrders.enqueue(order);
}

void Portfolio::executeNextOrder(double currentPrice, const string& date) {
    if (pendingOrders.isEmpty()) {
        cout << "No pending orders to execute.\n";
        return;
    }

    Order order = pendingOrders.dequeue();
    bool execute = false;

    if (order.type == "MARKET") {
        execute = true;
    } else if (order.type == "LIMIT") {
        if (order.side == "BUY" && currentPrice <= order.targetPrice) {
            execute = true;
        } else if (order.side == "SELL" && currentPrice >= order.targetPrice) {
            execute = true;
        }
    }

    if (!execute) {
        cout << "Order skipped (conditions not met).\n";
        return;
    }

    if (order.side == "BUY") {
        buyShares(order.ticker, order.shares, currentPrice, date);
    } else if (order.side == "SELL") {
        sellShares(order.ticker, order.shares, currentPrice, date);
    }
}

double Portfolio::getTotalMarketValue() const {
    double total = 0.0;
    for (size_t i = 0; i < holdings.size(); ++i) {
        total += holdings[i].shares * holdings[i].currentPrice;
    }
    return total;
}

double Portfolio::getTotalValue() const {
    return getTotalMarketValue() + cashBalance;
}

double Portfolio::getTotalUnrealizedReturn() const {
    double totalCost = 0.0;
    for (size_t i = 0; i < holdings.size(); ++i) {
        totalCost += holdings[i].shares * holdings[i].avgCostBasis;
    }
    if (totalCost <= 0.0) {
        return 0.0;
    }
    double marketValue = getTotalMarketValue();
    return (marketValue - totalCost) / totalCost * 100.0;
}

double Portfolio::getCashBalance() const { return cashBalance; }

void Portfolio::updatePrice(const string& ticker, double newPrice) {
    for (size_t i = 0; i < holdings.size(); ++i) {
        if (holdings[i].ticker == ticker) {
            holdings[i].currentPrice = newPrice;
            return;
        }
    }
}

void Portfolio::sortHoldingsByUnrealizedReturn() {
    sort(holdings.begin(), holdings.end(), [](const Position& a, const Position& b) {
        double retA = (a.currentPrice - a.avgCostBasis) / a.avgCostBasis;
        double retB = (b.currentPrice - b.avgCostBasis) / b.avgCostBasis;
        return retA > retB;
    });
}

void Portfolio::sortHoldingsByTicker() {
    sort(holdings.begin(), holdings.end(), [](const Position& a, const Position& b) {
        return a.ticker < b.ticker;
    });
}

void Portfolio::printHoldings() const {
    cout << "\nPortfolio Holdings for " << ownerName << "\n";
    cout << "Cash Balance: $" << fixed << setprecision(2) << cashBalance << "\n";
    if (holdings.empty()) {
        cout << "No holdings.\n";
        return;
    }

    cout << left << setw(8) << "Ticker"
         << right << setw(10) << "Shares"
         << setw(14) << "Avg Cost"
         << setw(14) << "Price"
         << setw(12) << "Unrlzd%" << "\n";

    for (size_t i = 0; i < holdings.size(); ++i) {
        const Position& p = holdings[i];
        double unrealized = 0.0;
        if (p.avgCostBasis > 0.0) {
            unrealized = (p.currentPrice - p.avgCostBasis) / p.avgCostBasis * 100.0;
        }
        cout << left << setw(8) << p.ticker
             << right << setw(10) << p.shares
             << setw(14) << fixed << setprecision(2) << p.avgCostBasis
             << setw(14) << p.currentPrice
             << setw(12) << fixed << setprecision(2) << unrealized << "\n";
    }

    cout << "Total Market Value: $" << fixed << setprecision(2) << getTotalMarketValue() << "\n";
    cout << "Total Portfolio Value: $" << fixed << setprecision(2) << getTotalValue() << "\n";
    cout << "Total Unrealized Return: " << fixed << setprecision(2) << getTotalUnrealizedReturn() << "%\n";
}

void Portfolio::printTradeHistory() const {
    tradeHistory.printAll();
}

void Portfolio::printPendingOrders() const {
    pendingOrders.printAll();
}
