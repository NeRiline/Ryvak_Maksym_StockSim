#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include "OrderQueue.h"
#include "TradeStack.h"
#include <string>
#include <vector>
using namespace std;

// Represents an investor's portfolio of stock positions.
// Holdings are stored in a std::vector (allowed per course rules).
// Pending orders use OrderQueue (FIFO); trade history uses TradeStack (LIFO/undo).
//
// Course modules: Vectors, Queues, Stacks, OOP integration
class Portfolio {
public:
    struct Position {
        string ticker;
        int    shares;
        double avgCostBasis;   // weighted average purchase price per share
        double currentPrice;   // last known market price (set manually or from data)
    };

private:
    vector<Position> holdings;   // std::vector allowed here
    OrderQueue       pendingOrders;
    TradeStack       tradeHistory;
    double           cashBalance;
    string           ownerName;

public:
    Portfolio(const string& ownerName, double initialCash);

    // --- Trading operations ---

    // Buy 'shares' of 'ticker' at 'price' on 'date'.
    // Deducts from cashBalance. Updates or creates a Position.
    // Pushes a TradeRecord onto tradeHistory.
    void buyShares(const string& ticker, int shares, double price, const string& date);

    // Sell 'shares' of 'ticker' at 'price' on 'date'.
    // Adds to cashBalance. Reduces position (remove if shares reach 0).
    // Pushes a TradeRecord onto tradeHistory. Does nothing if position not found.
    void sellShares(const string& ticker, int shares, double price, const string& date);

    // Reverses the most recent trade by popping the TradeStack and
    // performing the opposite transaction (buy→sell, sell→buy).
    // Restores cashBalance to its pre-trade state.
    void undoLastTrade();

    // Add a pending order to the OrderQueue.
    void queueOrder(const Order& order);

    // Dequeue the front Order and execute it if conditions are met:
    //   MARKET order → execute immediately at currentPrice
    //   LIMIT BUY    → execute if currentPrice <= order.targetPrice
    //   LIMIT SELL   → execute if currentPrice >= order.targetPrice
    // Prints "Order skipped" if LIMIT conditions are not met (order is discarded).
    void executeNextOrder(double currentPrice, const string& date);

    // --- Portfolio queries ---

    // Returns the total market value of all positions (sum of shares * currentPrice).
    double getTotalMarketValue() const;

    // Returns total portfolio value: getTotalMarketValue() + cashBalance.
    double getTotalValue() const;

    // Returns total unrealized return across all positions as a percentage:
    //   (totalMarketValue - totalCostBasis) / totalCostBasis * 100
    double getTotalUnrealizedReturn() const;

    double getCashBalance() const;

    // Updates the currentPrice field of a position (called during backtesting).
    void updatePrice(const string& ticker, double newPrice);

    // --- Sorting (uses std::sort with custom comparators) ---
    void sortHoldingsByUnrealizedReturn();  // descending: best performer first
    void sortHoldingsByTicker();            // alphabetical ascending

    // --- Display ---
    void printHoldings()     const;
    void printTradeHistory() const;   // delegates to tradeHistory.printAll()
    void printPendingOrders() const;  // delegates to pendingOrders.printAll()
};

#endif // PORTFOLIO_H
