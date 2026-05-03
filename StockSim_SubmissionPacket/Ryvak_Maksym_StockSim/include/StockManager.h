#ifndef STOCK_MANAGER_H
#define STOCK_MANAGER_H

#include "FinancialAsset.h"
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
using namespace std;

// Generic manager for any collection of FinancialAsset-derived objects.
// Must be fully defined in this header (template classes cannot be split across .cpp).
//
// Usage:
//   StockManager<Stock> stockManager;   // manages Stock objects
//   StockManager<ETF>   etfManager;     // manages ETF objects
//
// T must be a type that:
//   - Inherits from FinancialAsset
//   - Has getTicker(), calculateAnnualReturn(int), printSummary()
//
// Course module: Templates (generic programming)
template <typename T>
class StockManager {
private:
    vector<T*> assets;  // owns all T* pointers — must delete in destructor

public:
    StockManager() {}

    ~StockManager() {
        // TODO: Delete every T* in assets to free memory.
        //       Then call assets.clear().
    }

    // Add a heap-allocated asset. StockManager takes ownership.
    void addAsset(T* asset) {
        // TODO: push_back asset into assets vector.
    }

    // Remove and delete the asset with the given ticker.
    // Does nothing if ticker is not found.
    void removeAsset(const string& ticker) {
        // TODO:
        //  Iterate over assets. When found (asset->getTicker() == ticker):
        //    delete the pointer, erase from vector, return.
    }

    // Linear search by ticker. Returns nullptr if not found.
    T* findByTicker(const string& ticker) const {
        // TODO: Loop through assets, return the one matching getTicker().
        return nullptr; // placeholder
    }

    // Sort assets in-place by calculateAnnualReturn(year), descending.
    // Uses std::sort with a lambda comparator.
    void sortByAnnualReturn(int year) {
        // TODO: std::sort(assets.begin(), assets.end(), comparator)
        //  Lambda: [year](T* a, T* b) { return a->calculateAnnualReturn(year) > b->calculateAnnualReturn(year); }
    }

    // Sort assets alphabetically by ticker, ascending.
    void sortByTicker() {
        // TODO: std::sort with lambda comparing getTicker().
    }

    // Call printSummary() on each asset in the collection.
    void printAll() const {
        // TODO: Loop and call asset->printSummary() for each.
    }

    int getCount() const {
        return (int)assets.size();
    }
};

#endif // STOCK_MANAGER_H
