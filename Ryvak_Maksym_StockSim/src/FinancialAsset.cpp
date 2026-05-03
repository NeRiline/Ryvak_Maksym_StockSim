#include "FinancialAsset.h"

// Constructor — initializes all base class members
FinancialAsset::FinancialAsset(const string& ticker, const string& name, const string& sector)
    : ticker(ticker), name(name), sector(sector) {}

// Virtual destructor — required for correct polymorphic deletion
FinancialAsset::~FinancialAsset() {}

// --- Getters & Setters (provided) ---
string FinancialAsset::getTicker() const { return ticker; }
string FinancialAsset::getName()   const { return name; }
string FinancialAsset::getSector() const { return sector; }
void   FinancialAsset::setSector(const string& s) { sector = s; }

// NOTE: printSummary(), calculateAnnualReturn(), and getType() are pure virtual.
// They are implemented in derived classes: Stock and ETF.
