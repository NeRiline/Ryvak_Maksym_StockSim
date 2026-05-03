#ifndef PRICE_HISTORY_H
#define PRICE_HISTORY_H

#include "PriceNode.h"
#include <string>
using namespace std;

// Doubly-linked list of PriceNode items representing a stock's trading history.
// head -> oldest date, tail -> most recent date (chronological order)
//
// Course modules covered: Linked List, Iterators
class PriceHistory {
private:
    PriceNode* head;
    PriceNode* tail;
    int size;

public:
    PriceHistory();
    ~PriceHistory();  // must free all nodes

    // Appends a new PriceNode to the tail (most-recent end)
    void append(const string& date, double open, double high,
                double low, double close, long volume);

    // Linear search from head to tail — returns nullptr if not found
    PriceNode* findByDate(const string& date) const;

    // Print all nodes whose date falls within [startDate, endDate] (inclusive)
    void printRange(const string& startDate, const string& endDate) const;

    // Returns the head (oldest) node pointer — read only
    PriceNode* getHead() const;

    // Returns the tail (newest) node pointer — read only
    PriceNode* getTail() const;

    int getSize() const;

    // ---------------------------------------------------------------
    // Forward Iterator — traverses head -> tail (oldest to newest)
    // ---------------------------------------------------------------
    class Iterator {
    private:
        PriceNode* current;
    public:
        Iterator(PriceNode* node);
        Iterator& operator++();                   // move to next (more recent)
        PriceNode& operator*();                   // dereference to current node
        bool operator!=(const Iterator& other) const;
    };

    Iterator begin() const;   // points to head
    Iterator end()   const;   // points to nullptr (past tail)

    // ---------------------------------------------------------------
    // Reverse Iterator — traverses tail -> head (newest to oldest)
    // Used by MomentumStrategy to scan recent history backwards
    // ---------------------------------------------------------------
    class ReverseIterator {
    private:
        PriceNode* current;
    public:
        ReverseIterator(PriceNode* node);
        ReverseIterator& operator++();            // move to prev (older)
        PriceNode& operator*();
        bool operator!=(const ReverseIterator& other) const;
    };

    ReverseIterator rbegin() const;  // points to tail
    ReverseIterator rend()   const;  // points to nullptr (past head)
};

#endif // PRICE_HISTORY_H
