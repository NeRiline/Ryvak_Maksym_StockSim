#ifndef FINANCIAL_ASSET_H
#define FINANCIAL_ASSET_H

#include <string>
using namespace std;

// Abstract base class for all financial instruments (stocks, ETFs, etc.)
// Demonstrates OOP: abstract class, pure virtual methods, virtual destructor
class FinancialAsset {
protected:
    string ticker;
    string name;
    string sector;

public:
    FinancialAsset(const string& ticker, const string& name, const string& sector);
    virtual ~FinancialAsset();

    // Getters
    string getTicker() const;
    string getName()   const;
    string getSector() const;
    void   setSector(const string& sector);

    // Pure virtual — every derived class MUST implement these
    virtual void   printSummary()                        const = 0;
    virtual double calculateAnnualReturn(int year)       const = 0;
    virtual string getType()                             const = 0;
};

#endif // FINANCIAL_ASSET_H
