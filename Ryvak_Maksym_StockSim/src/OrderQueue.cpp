#include "OrderQueue.h"
#include <iostream>
#include <iomanip>

OrderQueue::OrderQueue() : front(nullptr), back(nullptr), size(0) {}

OrderQueue::~OrderQueue() {
    while (front) {
        QueueNode* next = front->next;
        delete front;
        front = next;
    }
    back = nullptr;
    size = 0;
}

void OrderQueue::enqueue(const Order& order) {
    QueueNode* node = new QueueNode{order, nullptr};
    if (!back) {
        front = node;
        back = node;
    } else {
        back->next = node;
        back = node;
    }
    size++;
}

Order OrderQueue::dequeue() {
    if (isEmpty()) {
        return Order();
    }
    QueueNode* node = front;
    Order order = node->data;
    front = node->next;
    if (!front) {
        back = nullptr;
    }
    delete node;
    size--;
    return order;
}

Order OrderQueue::peek() const {
    if (isEmpty()) {
        return Order();
    }
    return front->data;
}

bool OrderQueue::isEmpty() const { return size == 0; }

int OrderQueue::getSize() const { return size; }

void OrderQueue::printAll() const {
    if (isEmpty()) {
        cout << "No pending orders.\n";
        return;
    }

    QueueNode* current = front;
    while (current) {
        const Order& order = current->data;
        cout << order.submittedDate << "  " << order.ticker
             << "  " << order.side << " " << order.type
             << "  " << order.shares;
        if (order.type == "LIMIT") {
            cout << " @ " << fixed << setprecision(2) << order.targetPrice;
        }
        cout << "\n";
        current = current->next;
    }
}
