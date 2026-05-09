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
        for (size_t i = 0; i < assets.size(); ++i) {
            delete assets[i];
        }
        assets.clear();
    }

    // Add a heap-allocated asset. StockManager takes ownership.
    void addAsset(T* asset) {
        assets.push_back(asset);
    }

    // Remove and delete the asset with the given ticker.
    // Does nothing if ticker is not found.
    void removeAsset(const string& ticker) {
        for (size_t i = 0; i < assets.size(); ++i) {
            if (assets[i]->getTicker() == ticker) {
                delete assets[i];
                assets.erase(assets.begin() + static_cast<long>(i));
                return;
            }
        }
    }

    // Linear search by ticker. Returns nullptr if not found.
    T* findByTicker(const string& ticker) const {
        for (size_t i = 0; i < assets.size(); ++i) {
            if (assets[i]->getTicker() == ticker) {
                return assets[i];
            }
        }
        return nullptr; // placeholder
    }

    // Sort assets in-place by calculateAnnualReturn(year), descending.
    // Uses std::sort with a lambda comparator.
    void sortByAnnualReturn(int year) {
        sort(assets.begin(), assets.end(), [year](T* a, T* b) {
            return a->calculateAnnualReturn(year) > b->calculateAnnualReturn(year);
        });
    }

    // Sort assets alphabetically by ticker, ascending.
    void sortByTicker() {
        sort(assets.begin(), assets.end(), [](T* a, T* b) {
            return a->getTicker() < b->getTicker();
        });
    }

    // Call printSummary() on each asset in the collection.
    void printAll() const {
        for (size_t i = 0; i < assets.size(); ++i) {
            assets[i]->printSummary();
            cout << "\n";
        }
    }

    int getCount() const {
        return (int)assets.size();
    }
};

#endif // STOCK_MANAGER_H
