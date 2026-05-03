#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

// Fixed-size circular queue (ring buffer) for computing N-day moving averages.
//
// When full, enqueue() overwrites the oldest value automatically — the "window"
// slides forward. This is the natural structure for a rolling average.
//
// Example: CircularQueue(50) tracks the last 50 closing prices.
//          Once 50 prices have been enqueued, getAverage() returns the 50-day MA.
//          Each subsequent enqueue drops the oldest price and adds the newest.
//
// Course module: Queues (array-based circular buffer variant)
class CircularQueue {
private:
    double* buffer;    // heap-allocated array of size 'capacity'
    int     capacity;  // maximum number of elements
    int     head;      // index of the oldest element (next to dequeue)
    int     tail;      // index where the next enqueue will write
    int     count;     // number of elements currently stored

public:
    // Allocates a buffer of the given capacity
    CircularQueue(int capacity);
    ~CircularQueue();

    // Add a value. If the queue is full, the oldest value is overwritten
    // (head advances, count stays at capacity).
    void enqueue(double value);

    // Remove and return the oldest value.
    // Precondition: !isEmpty()
    double dequeue();

    // Return the oldest value without removing it.
    // Precondition: !isEmpty()
    double peek() const;

    // Compute sum of all stored values divided by count. O(n).
    // Returns 0.0 if isEmpty().
    double getAverage() const;

    bool isFull()  const;
    bool isEmpty() const;
    int  getCount()    const;
    int  getCapacity() const;
};

#endif // CIRCULAR_QUEUE_H
