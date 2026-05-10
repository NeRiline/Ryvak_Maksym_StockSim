/*
 * ESE 224 – Final Project: StockSim
 * Historical Market Analyzer & Trading Strategy Simulator
 *
 * Student Name : Maksym Ryvak, Rubayeth Hasan
 * Student ID   : 116607096
 *
 * Instructions:
 * 1. Implement all classes listed in the header files under include/.
 * 2. Create corresponding .cpp files in src/ for each header.
 * 3. Place your Yahoo Finance CSV files in data/ (SPY.csv, AAPL.csv, TSLA.csv).
 * 4. Complete the menu handlers below — each case should call the relevant
 * class methods you implemented.
 * 5. Do NOT use std::queue, std::stack, std::list, std::map, std::unordered_map,
 * or any external library. std::vector, std::string, std::sort are allowed.
 *
 * Compile with C++11 or later:
 * g++ -std=c++11 -Iinclude src/*.cpp main.cpp -o stocksim
 */

#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

// Include all your headers here once implemented
#include "include/FinancialAsset.h"
#include "include/PriceNode.h"
#include "include/PriceHistory.h"
#include "include/CSVParser.h"
#include "include/Stock.h"
#include "include/ETF.h"
#include "include/CircularQueue.h"
#include "include/TradeStack.h"
#include "include/OrderQueue.h"
#include "include/StockBST.h"
#include "include/Portfolio.h"
#include "include/TradingStrategy.h"
#include "include/FixedSIPStrategy.h"
#include "include/DynamicSIPStrategy.h"
#include "include/GoldenCrossStrategy.h"
#include "include/MomentumStrategy.h"
#include "include/StockManager.h"

using namespace std;

// ---------------------------------------------------------------
// Forward declarations for menu handler functions
// ---------------------------------------------------------------
void menuLoadData(StockManager<ETF>& etfManager, StockManager<Stock>& stockManager);
void menuDisplayHistory(StockManager<Stock>& stockManager, StockManager<ETF>& etfManager);
void menuSearchByDate(StockManager<Stock>& stockManager, StockManager<ETF>& etfManager);
void menuBSTRangeSearch(StockBST& bst);
void menuInsertIntoBST(StockBST& bst, StockManager<Stock>& stockManager);
void menuDisplayBST(StockBST& bst);
void menuAddToPortfolio(Portfolio& portfolio);
void menuRemoveFromPortfolio(Portfolio& portfolio);
void menuQueueOrder(Portfolio& portfolio);
void menuExecuteOrder(Portfolio& portfolio);
void menuUndoTrade(Portfolio& portfolio);
void menuRunStrategy(StockManager<ETF>& etfManager, StockManager<Stock>& stockManager);
void menuCompareStrategies(StockManager<ETF>& etfManager, StockManager<Stock>& stockManager);
void menuParameterSweep(StockManager<ETF>& etfManager);
void menuPortfolioSummary(Portfolio& portfolio);
void menuTradeHistory(Portfolio& portfolio);

// Bonus
void parameterSweep(ETF* spy, double monthlyCapital, int startYear, int endYear,
                    double dipStart, double dipEnd, double dipStep,
                    double rallyStart, double rallyEnd, double rallyStep,
                    double multStart, double multEnd, double multStep,
                    int topN,
                    StockBST& bst);

// ---------------------------------------------------------------
// Utility: print the main menu
// ---------------------------------------------------------------
void printMenu(const string& studentName, const string& studentID) {
    cout << "\n===== StockSim: Historical Market Analyzer =====\n";
    cout << "Student: " << studentName << "  |  ID: " << studentID << "\n";
    cout << "-------------------------------------------------\n";
    cout << " [1]  Load stock data from CSV\n";
    cout << " [2]  Display price history (linked list traversal)\n";
    cout << " [3]  Search by date range\n";
    cout << " [4]  Find stocks by return range (BST range query)\n";
    cout << " [5]  Insert stock into performance BST\n";
    cout << " [6]  Display BST (inorder / preorder / postorder)\n";
    cout << " [7]  Add stock to portfolio\n";
    cout << " [8]  Remove stock from portfolio\n";
    cout << " [9]  Queue a pending order\n";
    cout << "[10]  Execute next pending order\n";
    cout << "[11]  Undo last trade\n";
    cout << "[12]  Run strategy simulation\n";
    cout << "[13]  Compare all strategies head-to-head\n";
    cout << "[14]  Display portfolio summary\n";
    cout << "[15]  Display full trade history\n";
    cout << "[16]  Dynamic SIP grid search (bonus)\n";
    cout << " [0]  Exit\n";
    cout << "-------------------------------------------------\n";
    cout << "Enter choice: ";
}

// ---------------------------------------------------------------
// main
// ---------------------------------------------------------------
int main() {
    // --- Student login ---
    string studentName, studentID;
    cout << "========================================\n";
    cout << "  ESE 224 StockSim — Student Login\n";
    cout << "========================================\n";
    cout << "Enter your full name: ";
    getline(cin, studentName);
    cout << "Enter your student ID: ";
    getline(cin, studentID);
    cout << "\nWelcome, " << studentName << "!\n";

    // --- Initialize shared objects ---
    StockManager<ETF>   etfManager;
    StockManager<Stock> stockManager;
    StockBST            performanceBST;
    Portfolio           portfolio(studentName, 10000.0);  // start with $10,000 cash

    // --- Main menu loop ---
    int choice = -1;
    while (choice != 0) {
        printMenu(studentName, studentID);
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case  1: menuLoadData(etfManager, stockManager);                        break;
            case  2: menuDisplayHistory(stockManager, etfManager);                  break;
            case  3: menuSearchByDate(stockManager, etfManager);                    break;
            case  4: menuBSTRangeSearch(performanceBST);                            break;
            case  5: menuInsertIntoBST(performanceBST, stockManager);               break;
            case  6: menuDisplayBST(performanceBST);                                break;
            case  7: menuAddToPortfolio(portfolio);                                 break;
            case  8: menuRemoveFromPortfolio(portfolio);                            break;
            case  9: menuQueueOrder(portfolio);                                     break;
            case 10: menuExecuteOrder(portfolio);                                   break;
            case 11: menuUndoTrade(portfolio);                                      break;
            case 12: menuRunStrategy(etfManager, stockManager);                     break;
            case 13: menuCompareStrategies(etfManager, stockManager);               break;
            case 14: menuPortfolioSummary(portfolio);                               break;
            case 15: menuTradeHistory(portfolio);                                   break;
            case 16: menuParameterSweep(etfManager);                                break;
            case  0: cout << "Goodbye, " << studentName << "!\n";                  break;
            default: cout << "Invalid choice. Please enter 0–16.\n";               break;
        }
    }

    return 0;
}

// ---------------------------------------------------------------
// Menu handler stubs — implement each one below
// ---------------------------------------------------------------

void menuLoadData(StockManager<ETF>& etfManager, StockManager<Stock>& stockManager) {
    cout << "Load which dataset?\n";
    cout << " [1] SPX (S&P 500 Index)\n";
    cout << " [2] AMZN (Amazon)\n";
    cout << " [3] NVDA (NVIDIA)\n";
    cout << " [4] All\n";
    cout << "Enter choice: ";

    int choice = 0;
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    auto loadSPX = [&]() {
        if (etfManager.findByTicker("SPX")) {
            cout << "SPX already loaded.\n";
            return;
        }
        ETF* spx = new ETF("SPX", "S&P 500 Index", "Index", 0.0003);
        if (!spx->loadFromCSV("data/SPX.csv")) {
            delete spx;
            return;
        }
        etfManager.addAsset(spx);
    };

    auto loadAMZN = [&]() {
        if (stockManager.findByTicker("AMZN")) {
            cout << "AMZN already loaded.\n";
            return;
        }
        Stock* amzn = new Stock("AMZN", "Amazon.com Inc.", "Consumer Discretionary");
        if (!amzn->loadFromCSV("data/AMZN.csv")) {
            delete amzn;
            return;
        }
        stockManager.addAsset(amzn);
    };

    auto loadNVDA = [&]() {
        if (stockManager.findByTicker("NVDA")) {
            cout << "NVDA already loaded.\n";
            return;
        }
        Stock* nvda = new Stock("NVDA", "NVIDIA Corporation", "Technology");
        if (!nvda->loadFromCSV("data/NVidia_stock_history.csv")) {
            delete nvda;
            return;
        }
        stockManager.addAsset(nvda);
    };

    switch (choice) {
        case 1: loadSPX(); break;
        case 2: loadAMZN(); break;
        case 3: loadNVDA(); break;
        case 4: loadSPX(); loadAMZN(); loadNVDA(); break;
        default: cout << "Invalid choice.\n"; break;
    }
}

void menuDisplayHistory(StockManager<Stock>& stockManager, StockManager<ETF>& etfManager) {
    cout << "Enter ticker: ";
    string ticker;
    getline(cin, ticker);

    PriceHistory* history = nullptr;
    Stock* stock = stockManager.findByTicker(ticker);
    if (stock) {
        history = stock->getHistory();
    } else {
        ETF* etf = etfManager.findByTicker(ticker);
        if (etf) {
            history = etf->getHistory();
        }
    }

    if (!history) {
        cout << "Ticker not found or history not loaded.\n";
        return;
    }

    cout << "How many records to display? (enter 0 for all): ";
    int limit = 0;
    cin >> limit;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    int printed = 0;
    for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) {
        PriceNode& node = *it;
        cout << node.date
             << "  O:" << fixed << setprecision(2) << node.open
             << "  H:" << node.high
             << "  L:" << node.low
             << "  C:" << node.close
             << "  V:" << node.volume
             << "\n";

        printed++;
        if (limit > 0 && printed >= limit) {
            break;
        }
    }
}

// Prompts user for ticker and date range, then prints all price nodes in that range
void menuSearchByDate(StockManager<Stock>& stockManager, StockManager<ETF>& etfManager) {
    cout << "Enter ticker: ";
    string ticker;
    getline(cin, ticker);

    PriceHistory* history = nullptr;
    Stock* stock = stockManager.findByTicker(ticker);
    if (stock) {
        history = stock->getHistory();
    } else {
        ETF* etf = etfManager.findByTicker(ticker);
        if (etf) {
            history = etf->getHistory();
        }
    }

    if (!history) {
        cout << "Ticker not found or history not loaded.\n";
        return;
    }

    string startDate, endDate;
    cout << "Start date (YYYY-MM-DD): ";
    getline(cin, startDate);
    cout << "End date (YYYY-MM-DD): ";
    getline(cin, endDate);

    history->printRange(startDate, endDate);
}

// Prompts user for return % range, then prints all BST nodes in that range
void menuBSTRangeSearch(StockBST& bst) {
    double low = 0.0;
    double high = 0.0;
    cout << "Enter low return %: ";
    cin >> low;
    cout << "Enter high return %: ";
    cin >> high;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    vector<StockBST::BSTNode*> results;
    bst.rangeSearch(low, high, results);
    if (results.empty()) {
        cout << "No results in range.\n";
        return;
    }

    for (size_t i = 0; i < results.size(); ++i) {
        cout << results[i]->ticker << " (" << fixed << setprecision(2) << results[i]->key << ")  " << results[i]->year << "\n";
    }
}

void menuInsertIntoBST(StockBST& bst, StockManager<Stock>& stockManager) {
    cout << "Enter stock ticker: ";
    string ticker;
    getline(cin, ticker);

    int year = 0;
    cout << "Enter year: ";
    cin >> year;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    Stock* stock = stockManager.findByTicker(ticker);
    if (!stock) {
        cout << "Stock not found.\n";
        return;
    }

    double annualReturn = stock->calculateAnnualReturn(year);
    bst.insert(ticker, annualReturn, year);
    cout << "Inserted " << ticker << " with return " << fixed << setprecision(2) << annualReturn << "% for " << year << ".\n";
}

// Prompts user for traversal type, then displays BST nodes in that order
void menuDisplayBST(StockBST& bst) {
    cout << "Choose traversal: [1] Inorder  [2] Preorder  [3] Postorder: ";
    int choice = 0;
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (choice == 1) {
        bst.inorder();
    } else if (choice == 2) {
        bst.preorder();
    } else if (choice == 3) {
        bst.postorder();
    } else {
        cout << "Invalid traversal choice.\n";
    }
}

// Prompts user for trade details, then executes a buy transaction on the portfolio
void menuAddToPortfolio(Portfolio& portfolio) {
    string ticker;
    int shares = 0;
    double price = 0.0;
    string date;

    cout << "Ticker: ";
    getline(cin, ticker);
    cout << "Shares: ";
    cin >> shares;
    cout << "Price: ";
    cin >> price;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Date (YYYY-MM-DD): ";
    getline(cin, date);

    portfolio.buyShares(ticker, shares, price, date);
}

// Prompts user for trade details, then executes a sell transaction on the portfolio
void menuRemoveFromPortfolio(Portfolio& portfolio) {
    string ticker;
    int shares = 0;
    double price = 0.0;
    string date;

    cout << "Ticker: ";
    getline(cin, ticker);
    cout << "Shares: ";
    cin >> shares;
    cout << "Price: ";
    cin >> price;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Date (YYYY-MM-DD): ";
    getline(cin, date);

    portfolio.sellShares(ticker, shares, price, date);
}

// Prompts user for order details, then adds a pending order to the portfolio's OrderQueue
void menuQueueOrder(Portfolio& portfolio) {
    Order order;
    cout << "Ticker: ";
    getline(cin, order.ticker);
    cout << "Type (MARKET/LIMIT): ";
    getline(cin, order.type);
    cout << "Side (BUY/SELL): ";
    getline(cin, order.side);
    cout << "Shares: ";
    cin >> order.shares;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Date (YYYY-MM-DD): ";
    getline(cin, order.submittedDate);

    transform(order.type.begin(), order.type.end(), order.type.begin(), ::toupper);
    transform(order.side.begin(), order.side.end(), order.side.begin(), ::toupper);

    if (order.type == "LIMIT") {
        cout << "Target Price: ";
        cin >> order.targetPrice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    } else {
        order.targetPrice = 0.0;
    }

    portfolio.queueOrder(order);
    cout << "Order queued.\n";
}

// Prompts user for current price and date, then attempts
void menuExecuteOrder(Portfolio& portfolio) {
    double currentPrice = 0.0;
    string date;
    cout << "Current market price: ";
    cin >> currentPrice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Date (YYYY-MM-DD): ";
    getline(cin, date);

    portfolio.executeNextOrder(currentPrice, date);
}

// Undoes the most recent trade by popping the TradeStack and performing the opposite transaction
void menuUndoTrade(Portfolio& portfolio) {
    portfolio.undoLastTrade();
}

// Prompts user for strategy parameters, then runs a backtest on the selected stock/ETF
void menuRunStrategy(StockManager<ETF>& etfManager, StockManager<Stock>& stockManager) {
    cout << "Select strategy:\n";
    cout << " [1] Fixed SIP\n";
    cout << " [2] Dynamic SIP\n";
    cout << " [3] Golden Cross (50/200 MA)\n";
    cout << " [4] 6-Month Momentum\n";
    cout << "Enter choice: ";

    int choice = 0;
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Ticker: ";
    string ticker;
    getline(cin, ticker);

    PriceHistory* history = nullptr;
    Stock* stock = stockManager.findByTicker(ticker);
    if (stock) {
        history = stock->getHistory();
    } else {
        ETF* etf = etfManager.findByTicker(ticker);
        if (etf) {
            history = etf->getHistory();
        }
    }

    if (!history) {
        cout << "Ticker not found or history not loaded.\n";
        return;
    }

    double monthlyCapital = 0.0;
    int startYear = 0;
    int endYear = 0;
    cout << "Monthly capital: ";
    cin >> monthlyCapital;
    cout << "Start year: ";
    cin >> startYear;
    cout << "End year: ";
    cin >> endYear;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (choice == 1) {
        FixedSIPStrategy strategy;
        // Run the backtest and print results
        SimResult result = strategy.backtest(history, monthlyCapital, startYear, endYear);
        strategy.printResult(result);
        return;
    }

    if (choice == 2) {
        double dipThreshold = 5.0;
        double rallyThreshold = 10.0;
        double multiplier = 2.0;
        cout << "Dip threshold (%): ";
        cin >> dipThreshold;
        cout << "Rally threshold (%): ";
        cin >> rallyThreshold;
        cout << "Dip multiplier: ";
        cin >> multiplier;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        DynamicSIPStrategy strategy(dipThreshold, rallyThreshold, multiplier);
        SimResult result = strategy.backtest(history, monthlyCapital, startYear, endYear);
        strategy.printResult(result);
        return;
    }

    if (choice == 3) {
        int shortWindow = 50;
        int longWindow = 200;
        cout << "Short MA window (default 50): ";
        cin >> shortWindow;
        cout << "Long MA window (default 200): ";
        cin >> longWindow;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        GoldenCrossStrategy strategy(shortWindow, longWindow);
        SimResult result = strategy.backtest(history, monthlyCapital, startYear, endYear);
        strategy.printResult(result);
        return;
    }

    if (choice == 4) {
        double momentumThreshold = 5.0;
        int lookbackDays = 126;
        cout << "Momentum threshold (%): ";
        cin >> momentumThreshold;
        cout << "Lookback days (default 126): ";
        cin >> lookbackDays;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        MomentumStrategy strategy(momentumThreshold, lookbackDays);
        SimResult result = strategy.backtest(history, monthlyCapital, startYear, endYear);
        strategy.printResult(result);
        return;
    }

    cout << "Invalid strategy choice.\n";
}

// Prompts user for strategy parameters, then runs a backtest on the selected stock/ETF and compares all strategies head-to-head
void menuCompareStrategies(StockManager<ETF>& etfManager, StockManager<Stock>& stockManager) {
    cout << "Enter ticker to compare strategies (e.g., SPX, NVDA, AMZN): ";
    string ticker;
    getline(cin, ticker);

    PriceHistory* history = nullptr;
    
    // Check Stocks first
    Stock* stock = stockManager.findByTicker(ticker);
    if (stock) {
        history = stock->getHistory();
    } else {
        // If not a stock, check ETFs
        ETF* etf = etfManager.findByTicker(ticker);
        if (etf) {
            history = etf->getHistory();
        }
    }

    if (!history) {
        cout << ticker << " data not loaded. Please load it first via Menu Option 1.\n";
        return;
    }

    double monthlyCapital = 0.0;
    int startYear = 0;
    int endYear = 0;
    cout << "Monthly capital: ";
    cin >> monthlyCapital;
    cout << "Start year: ";
    cin >> startYear;
    cout << "End year: ";
    cin >> endYear;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    FixedSIPStrategy fixedStrategy;
    // Using your newly optimized 2D sweep parameters!
    DynamicSIPStrategy dynamic(32.5, 0, 0.70); 
    GoldenCrossStrategy golden;
    MomentumStrategy momentum;

    SimResult rFixed = fixedStrategy.backtest(history, monthlyCapital, startYear, endYear);
    SimResult rDynamic = dynamic.backtest(history, monthlyCapital, startYear, endYear);
    SimResult rGolden = golden.backtest(history, monthlyCapital, startYear, endYear);
    SimResult rMomentum = momentum.backtest(history, monthlyCapital, startYear, endYear);

    // Print a formatted comparison table
    cout << "\nStrategy Comparison (" << ticker << ")\n";
    cout << left << setw(35) << "Strategy"
         << right << setw(14) << "Final"
         << setw(10) << "CAGR%"
         << setw(12) << "MaxDD%"
         << setw(10) << "Trades" << "\n";

    auto printRow = [](const SimResult& r) {
        cout << left << setw(35) << r.strategyName
             << right << setw(14) << fixed << setprecision(2) << r.finalValue
             << setw(10) << fixed << setprecision(2) << r.cagr
             << setw(12) << fixed << setprecision(2) << r.maxDrawdown
             << setw(10) << r.totalTrades << "\n";
    };

    printRow(rFixed);
    printRow(rDynamic);
    printRow(rGolden);
    printRow(rMomentum);
}

// ---------------------------------------------------------------
// Prompts user for parameter ranges, then runs a grid search over the DynamicSIPStrategy parameters and stores results in a BST
void menuParameterSweep(StockManager<ETF>& etfManager) {
    ETF* spx = etfManager.findByTicker("SPX");
    if (!spx || !spx->getHistory()) {
        cout << "SPX data not loaded. Please load it first.\n";
        return;
    }

    double monthlyCapital = 0.0;
    int startYear = 0;
    int endYear = 0;
    cout << "Monthly capital: ";
    cin >> monthlyCapital;
    cout << "Start year: ";
    cin >> startYear;
    cout << "End year: ";
    cin >> endYear;

    double dipStart = 3.0;
    double dipEnd = 20.0;
    double dipStep = 1.0;
    cout << "Dip threshold start (%): ";
    cin >> dipStart;
    cout << "Dip threshold end (%): ";
    cin >> dipEnd;
    cout << "Dip threshold step (%): ";
    cin >> dipStep;

    double rallyStart = 6.0;
    double rallyEnd = 18.0;
    double rallyStep = 2.0;
    cout << "Rally threshold start (%): ";
    cin >> rallyStart;
    cout << "Rally threshold end (%): ";
    cin >> rallyEnd;
    cout << "Rally threshold step (%): ";
    cin >> rallyStep;

    double multStart = 1.5;
    double multEnd = 3.0;
    double multStep = 0.5;
    cout << "Multiplier start: ";
    cin >> multStart;
    cout << "Multiplier end: ";
    cin >> multEnd;
    cout << "Multiplier step: ";
    cin >> multStep;

    int topN = 20;
    cout << "Top N results to display (0 = all): ";
    cin >> topN;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (dipStep <= 0.0 || rallyStep <= 0.0 || multStep <= 0.0) {
        cout << "Step values must be greater than 0.\n";
        return;
    }
    if (dipStart > dipEnd || rallyStart > rallyEnd || multStart > multEnd) {
        cout << "Start values must be <= end values.\n";
        return;
    }
    if (topN < 0) {
        cout << "Top N must be 0 or greater.\n";
        return;
    }

    StockBST sweepBST;
    parameterSweep(spx, monthlyCapital, startYear, endYear,
                   dipStart, dipEnd, dipStep,
                   rallyStart, rallyEnd, rallyStep,
                   multStart, multEnd, multStep,
                   topN,
                   sweepBST);
}

void menuPortfolioSummary(Portfolio& portfolio) {
    portfolio.printHoldings();
}

void menuTradeHistory(Portfolio& portfolio) {
    portfolio.printTradeHistory();
}

// ---------------------------------------------------------------
// BONUS: Parameter Sweep for DynamicSIPStrategy
// ---------------------------------------------------------------
void parameterSweep(ETF* spy, double monthlyCapital, int startYear, int endYear,
                    double dipStart, double dipEnd, double dipStep,
                    double rallyStart, double rallyEnd, double rallyStep,
                    double multStart, double multEnd, double multStep,
                    int topN,
                    StockBST& bst) {
    if (!spy || !spy->getHistory()) {
        cout << "SPX data not loaded.\n";
        return;
    }

    int totalRuns = 0;
    struct SweepResult {
        string label;
        double finalValue;
    };
    vector<SweepResult> results;
    for (int dipIndex = 0; ; ++dipIndex) {
        double dip = dipStart + (dipIndex * dipStep);
        if (dip > dipEnd + 1e-9) {
            break;
        }
        for (int rallyIndex = 0; ; ++rallyIndex) {
            double rally = rallyStart + (rallyIndex * rallyStep);
            if (rally > rallyEnd + 1e-9) {
                break;
            }
            for (int multIndex = 0; ; ++multIndex) {
                double mult = multStart + (multIndex * multStep);
                if (mult > multEnd + 1e-9) {
                    break;
                }
                DynamicSIPStrategy strategy(dip, rally, mult);
                SimResult result = strategy.backtest(spy->getHistory(), monthlyCapital, startYear, endYear);

                ostringstream label;
                label << fixed << setprecision(1)
                      << "dip=" << dip << "% "
                      << "rally=" << rally << "% "
                      << setprecision(2) << "mult=" << mult;

                string labelText = label.str();
                bst.insert(labelText, result.finalValue, 0);
                results.push_back(SweepResult{labelText, result.finalValue});
                totalRuns++;
            }
        }
    }

    cout << "\nGrid search runs: " << totalRuns << "\n";

    if (topN == 0) {
        cout << "\nParameter sweep results (worst -> best):\n";
        bst.inorder();
    } else {
        sort(results.begin(), results.end(), [](const SweepResult& a, const SweepResult& b) {
            return a.finalValue > b.finalValue;
        });

        int count = topN;
        if (count > static_cast<int>(results.size())) {
            count = static_cast<int>(results.size());
        }

        cout << "\nTop " << count << " results:\n";
        for (int i = 0; i < count; ++i) {
            cout << results[i].label << "  $" << fixed << setprecision(2)
                 << results[i].finalValue << "\n";
        }
    }

    StockBST::BSTNode* best = bst.findMax();
    if (best) {
        cout << "\nBest result: " << best->ticker << "  $" << fixed << setprecision(2)
             << best->key << "\n";
    }
}