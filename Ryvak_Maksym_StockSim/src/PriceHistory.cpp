#include "PriceHistory.h"
#include <iostream>
#include <iomanip>

PriceHistory::PriceHistory() : head(nullptr), tail(nullptr), size(0) {}

PriceHistory::~PriceHistory() {
    PriceNode* current = head;
    while (current) {
        PriceNode* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
    tail = nullptr;
    size = 0;
}

void PriceHistory::append(const string& date, double open, double high,
                          double low, double close, long volume) {
    PriceNode* node = new PriceNode(date, open, high, low, close, volume);
    if (!head) {
        head = node;
        tail = node;
    } else {
        tail->next = node;
        node->prev = tail;
        tail = node;
    }
    size++;
}

PriceNode* PriceHistory::findByDate(const string& date) const {
    PriceNode* current = head;
    while (current) {
        if (current->date == date) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

void PriceHistory::printRange(const string& startDate, const string& endDate) const {
    PriceNode* current = head;
    bool found = false;
    while (current) {
        if (current->date >= startDate && current->date <= endDate) {
            cout << current->date
                 << "  O:" << fixed << setprecision(2) << current->open
                 << "  H:" << current->high
                 << "  L:" << current->low
                 << "  C:" << current->close
                 << "  V:" << current->volume
                 << "\n";
            found = true;
        }
        current = current->next;
    }
    if (!found) {
        cout << "No records found in the specified date range.\n";
    }
}

PriceNode* PriceHistory::getHead() const { return head; }
PriceNode* PriceHistory::getTail() const { return tail; }
int PriceHistory::getSize() const { return size; }

PriceHistory::Iterator::Iterator(PriceNode* node) : current(node) {}

PriceHistory::Iterator& PriceHistory::Iterator::operator++() {
    if (current) {
        current = current->next;
    }
    return *this;
}

PriceNode& PriceHistory::Iterator::operator*() {
    return *current;
}

bool PriceHistory::Iterator::operator!=(const Iterator& other) const {
    return current != other.current;
}

PriceHistory::Iterator PriceHistory::begin() const { return Iterator(head); }
PriceHistory::Iterator PriceHistory::end() const { return Iterator(nullptr); }

PriceHistory::ReverseIterator::ReverseIterator(PriceNode* node) : current(node) {}

PriceHistory::ReverseIterator& PriceHistory::ReverseIterator::operator++() {
    if (current) {
        current = current->prev;
    }
    return *this;
}

PriceNode& PriceHistory::ReverseIterator::operator*() {
    return *current;
}

bool PriceHistory::ReverseIterator::operator!=(const ReverseIterator& other) const {
    return current != other.current;
}

PriceHistory::ReverseIterator PriceHistory::rbegin() const { return ReverseIterator(tail); }
PriceHistory::ReverseIterator PriceHistory::rend() const { return ReverseIterator(nullptr); }
