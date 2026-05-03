#ifndef ORDER_QUEUE_H
#define ORDER_QUEUE_H

#include <string>
using namespace std;

// A pending trade order waiting to be executed.
struct Order {
    string ticker;
    string type;          // "MARKET" or "LIMIT"
    string side;          // "BUY" or "SELL"
    double targetPrice;   // for LIMIT orders; use 0.0 for MARKET orders
    int    shares;
    string submittedDate;
};

// FIFO queue of pending Order items.
// Implemented as a singly-linked list of QueueNodes (no STL).
//
// Course module: Queues (linked-list based)
class OrderQueue {
private:
    struct QueueNode {
        Order      data;
        QueueNode* next;
    };

    QueueNode* front;  // dequeue from here (oldest order)
    QueueNode* back;   // enqueue here    (newest order)
    int        size;

public:
    OrderQueue();
    ~OrderQueue();  // must free all QueueNodes

    // Add an order to the back of the queue.
    void enqueue(const Order& order);

    // Remove and return the order at the front.
    // Precondition: !isEmpty()
    Order dequeue();

    // Return the front order without removing it.
    // Precondition: !isEmpty()
    Order peek() const;

    bool isEmpty() const;
    int  getSize() const;

    // Print all pending orders front to back.
    void printAll() const;
};

#endif // ORDER_QUEUE_H
