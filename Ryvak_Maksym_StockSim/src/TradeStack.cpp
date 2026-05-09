#include "TradeStack.h"
#include <iostream>
#include <iomanip>

TradeStack::TradeStack() : top(nullptr), size(0) {}

TradeStack::~TradeStack() {
    while (top) {
        StackNode* next = top->next;
        delete top;
        top = next;
    }
    size = 0;
}

void TradeStack::push(const TradeRecord& record) {
    StackNode* node = new StackNode{record, top};
    top = node;
    size++;
}

TradeRecord TradeStack::pop() {
    if (isEmpty()) {
        return TradeRecord();
    }
    StackNode* node = top;
    TradeRecord record = node->data;
    top = node->next;
    delete node;
    size--;
    return record;
}

TradeRecord TradeStack::peek() const {
    if (isEmpty()) {
        return TradeRecord();
    }
    return top->data;
}

bool TradeStack::isEmpty() const { return size == 0; }

int TradeStack::getSize() const { return size; }

void TradeStack::printAll() const {
    if (isEmpty()) {
        cout << "No trades recorded.\n";
        return;
    }

    StackNode* current = top;
    while (current) {
        const TradeRecord& rec = current->data;
        cout << rec.date << "  " << rec.ticker
             << "  " << rec.action
             << "  " << rec.shares << " @ " << fixed << setprecision(2) << rec.price
             << "  Total: " << rec.totalCost << "\n";
        current = current->next;
    }
}
