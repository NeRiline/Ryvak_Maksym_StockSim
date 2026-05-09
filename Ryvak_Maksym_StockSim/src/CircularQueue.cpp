#include "CircularQueue.h"

CircularQueue::CircularQueue(int capacity)
    : buffer(nullptr), capacity(capacity), head(0), tail(0), count(0) {
    if (capacity > 0) {
        buffer = new double[capacity];
    }
}

CircularQueue::~CircularQueue() {
    delete[] buffer;
    buffer = nullptr;
    capacity = 0;
    head = 0;
    tail = 0;
    count = 0;
}

void CircularQueue::enqueue(double value) {
    if (capacity <= 0) {
        return;
    }
    if (isFull()) {
        buffer[tail] = value;
        tail = (tail + 1) % capacity;
        head = (head + 1) % capacity;
        return;
    }

    buffer[tail] = value;
    tail = (tail + 1) % capacity;
    count++;
}

double CircularQueue::dequeue() {
    if (isEmpty()) {
        return 0.0;
    }
    double value = buffer[head];
    head = (head + 1) % capacity;
    count--;
    if (count == 0) {
        tail = head;
    }
    return value;
}

double CircularQueue::peek() const {
    if (isEmpty()) {
        return 0.0;
    }
    return buffer[head];
}

double CircularQueue::getAverage() const {
    if (isEmpty()) {
        return 0.0;
    }
    double sum = 0.0;
    for (int i = 0; i < count; ++i) {
        int idx = (head + i) % capacity;
        sum += buffer[idx];
    }
    return sum / static_cast<double>(count);
}

bool CircularQueue::isFull() const { return count == capacity; }

bool CircularQueue::isEmpty() const { return count == 0; }

int CircularQueue::getCount() const { return count; }

int CircularQueue::getCapacity() const { return capacity; }
