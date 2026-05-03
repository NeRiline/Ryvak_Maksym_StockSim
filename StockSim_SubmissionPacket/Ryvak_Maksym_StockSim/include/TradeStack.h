#ifndef TRADE_STACK_H
#define TRADE_STACK_H

#include <string>
using namespace std;

// Record of a single buy or sell transaction.
struct TradeRecord {
    string ticker;
    string date;
    double price;      // price per share at execution
    int    shares;
    string action;     // "BUY" or "SELL"
    double totalCost;  // price * shares (positive = cash spent, negative = cash received)
};

// LIFO stack of TradeRecord items. Supports undo of the most recent trade.
// Implemented using a singly-linked list of StackNodes (no arrays/STL).
//
// Course module: Stacks
class TradeStack {
private:
    struct StackNode {
        TradeRecord data;
        StackNode*  next;
    };

    StackNode* top;   // points to the most recently pushed record
    int        size;

public:
    TradeStack();
    ~TradeStack();  // must free all StackNodes

    // Push a trade record onto the top of the stack.
    void push(const TradeRecord& record);

    // Remove and return the top record.
    // Precondition: !isEmpty()
    TradeRecord pop();

    // Return the top record without removing it.
    // Precondition: !isEmpty()
    TradeRecord peek() const;

    bool isEmpty() const;
    int  getSize() const;

    // Print all records from top (most recent) to bottom (oldest).
    void printAll() const;
};

#endif // TRADE_STACK_H
