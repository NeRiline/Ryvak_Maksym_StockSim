#include "TradingStrategy.h"
#include <iostream>
#include <iomanip>
#include <cmath>

void TradingStrategy::printResult(const SimResult& result) const {
    cout << "\nStrategy: " << result.strategyName << "\n";
    cout << "Final Value   : $" << fixed << setprecision(2) << result.finalValue << "\n";
    cout << "Total Invested: $" << fixed << setprecision(2) << result.totalInvested << "\n";
    cout << "Total Return  : " << fixed << setprecision(2) << result.totalReturn << "%\n";
    cout << "CAGR          : " << fixed << setprecision(2) << result.cagr << "%\n";
    cout << "Max Drawdown  : " << fixed << setprecision(2) << result.maxDrawdown << "%\n";
    cout << "Total Trades  : " << result.totalTrades << "\n";
}

double TradingStrategy::calculateCAGR(double startVal, double endVal, int years) const {
    if (years <= 0 || startVal <= 0.0 || endVal <= 0.0) {
        return 0.0;
    }
    double cagr = pow(endVal / startVal, 1.0 / years) - 1.0;
    return cagr * 100.0;
}

double TradingStrategy::calculateMaxDrawdown(const vector<double>& portfolioValues) const {
    if (portfolioValues.empty()) {
        return 0.0;
    }
    double peak = portfolioValues[0];
    double maxDrawdown = 0.0;
    for (size_t i = 0; i < portfolioValues.size(); ++i) {
        double value = portfolioValues[i];
        if (value > peak) {
            peak = value;
        }
        if (peak > 0.0) {
            double drawdown = (peak - value) / peak * 100.0;
            if (drawdown > maxDrawdown) {
                maxDrawdown = drawdown;
            }
        }
    }
    return maxDrawdown;
}
