#include "DynamicSIPStrategy.h"
#include "CSVParser.h"
#include "CircularQueue.h" // Required for the 200-day MA
#include <iomanip> 
#include <sstream> 
#include <vector>
#include <cmath>
#include <algorithm> 

using namespace std;

DynamicSIPStrategy::DynamicSIPStrategy(double dipThreshold, double rallyThreshold, double multiplier)
    : dipThreshold(dipThreshold), rallyThreshold(rallyThreshold), multiplier(multiplier) {}

SimResult DynamicSIPStrategy::backtest(PriceHistory* history,
                                      double monthlyCapital,
                                      int startYear,
                                      int endYear) {
    SimResult result{};
    result.strategyName = getName();

    if (!history || monthlyCapital <= 0.0) {
        return result;
    }

    // Initialize the trend filter
    CircularQueue ma200(200);

    double shares = 0.0;
    double cash = 0.0;
    double totalContributed = 0.0;
    int totalTrades = 0;
    vector<double> portfolioValues;

    int currentYear = -1;
    int currentMonth = -1;
    double lastPrice = 0.0;

    for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) {
        PriceNode& node = *it;
        
        // Feed the moving average every single day
        ma200.enqueue(node.close);

        int year = CSVParser::extractYear(node.date);
        int month = CSVParser::extractMonth(node.date);
        
        if (year < startYear || year > endYear) {
            continue;
        }

        // SIP Trigger: First trading day of a new month
        if (year != currentYear || month != currentMonth) {
            cash += monthlyCapital;
            totalContributed += monthlyCapital;

            double investAmount = 0.0;

            // Wait until the MA is fully primed before making dynamic decisions
            if (!ma200.isFull()) {
                investAmount = monthlyCapital; // Default to Fixed SIP behavior while priming
            } 
            else {
                double currentMA = ma200.getAverage();

                if (node.close < currentMA) {
                    // BEAR MARKET: Turn off the tap. Hoard cash.
                    investAmount = 0.0;
                } else {
                    // BULL MARKET: Turn on the tap. 
                    // Deploy regular monthly capital PLUS a percentage of our hoarded war chest.
                    // We map the 'multiplier' parameter to this drain rate (e.g., 0.33)
                    double excessCash = cash - monthlyCapital;
                    investAmount = monthlyCapital;
                    
                    if (excessCash > 0.0) {
                        investAmount += (excessCash * multiplier); 
                    }
                }
            }

            // Force 100% deployment on the absolute last month to strictly satisfy the budget rule
            if (year == endYear && month == 12) {
                investAmount = cash;
            }

            // Safety cap to prevent over-drawing
            if (investAmount > cash) {
                investAmount = cash;
            }

            // Execute the trade
            if (investAmount > 0.0) {
                shares += investAmount / node.close;
                cash -= investAmount;
                totalTrades++;
            }

            currentYear = year;
            currentMonth = month;
        }

        lastPrice = node.close;
        portfolioValues.push_back(cash + shares * node.close);
    }

    // Final performance calculations
    double finalValue = cash + shares * lastPrice;
    result.finalValue = finalValue;
    result.totalInvested = totalContributed;
    if (totalContributed > 0.0) {
        result.totalReturn = (finalValue - totalContributed) / totalContributed * 100.0;
    }
    int years = endYear - startYear + 1;
    result.cagr = calculateCAGR(totalContributed, finalValue, years);
    result.maxDrawdown = calculateMaxDrawdown(portfolioValues);
    result.totalTrades = totalTrades;
    return result;
}

string DynamicSIPStrategy::getName() const {
    ostringstream out;
    out << fixed << setprecision(2)
        << "Dynamic SIP (drain rate=" << multiplier << ")";
    return out.str();
}

double DynamicSIPStrategy::getDipThreshold() const { return dipThreshold; }

double DynamicSIPStrategy::getRallyThreshold() const { return rallyThreshold; }

double DynamicSIPStrategy::getMultiplier() const { return multiplier; }