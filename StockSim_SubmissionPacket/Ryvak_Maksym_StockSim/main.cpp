/*
 * ESE 224 – Final Project: StockSim
 * Historical Market Analyzer & Trading Strategy Simulator
 *
 * Student Name : ___________________________
 * Student ID   : ___________________________
 *
 * Instructions:
 *   1. Implement all classes listed in the header files under include/.
 *   2. Create corresponding .cpp files in src/ for each header.
 *   3. Place your Yahoo Finance CSV files in data/ (SPY.csv, AAPL.csv, TSLA.csv).
 *   4. Complete the menu handlers below — each case should call the relevant
 *      class methods you implemented.
 *   5. Do NOT use std::queue, std::stack, std::list, std::map, std::unordered_map,
 *      or any external library. std::vector, std::string, std::sort are allowed.
 *
 * Compile with C++11 or later:
 *   g++ -std=c++11 -Iinclude src/*.cpp main.cpp -o stocksim
 */

#include <iostream>
#include <string>
#include <limits>

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
void menuCompareStrategies(StockManager<ETF>& etfManager);
void menuPortfolioSummary(Portfolio& portfolio);
void menuTradeHistory(Portfolio& portfolio);

// Bonus
void parameterSweep(ETF* spy, double monthlyCapital, int startYear, int endYear, StockBST& bst);

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
            case 13: menuCompareStrategies(etfManager);                             break;
            case 14: menuPortfolioSummary(portfolio);                               break;
            case 15: menuTradeHistory(portfolio);                                   break;
            case  0: cout << "Goodbye, " << studentName << "!\n";                  break;
            default: cout << "Invalid choice. Please enter 0–15.\n";               break;
        }
    }

    return 0;
}

// ---------------------------------------------------------------
// Menu handler stubs — implement each one below
// ---------------------------------------------------------------

void menuLoadData(StockManager<ETF>& etfManager, StockManager<Stock>& stockManager) {
    // TODO:
    //  Prompt the user to choose which ticker to load (SPY, AAPL, TSLA, or all).
    //  For SPY: create a new ETF("SPY", "SPDR S&P 500 ETF", "Index", 0.0003),
    //           call loadFromCSV("data/SPY.csv"), add to etfManager.
    //  For AAPL/TSLA: create Stock objects, load CSV, add to stockManager.
    cout << "(TODO: implement menuLoadData)" << endl;
}

void menuDisplayHistory(StockManager<Stock>& stockManager, StockManager<ETF>& etfManager) {
    // TODO:
    //  Ask user for a ticker. Find it in stockManager or etfManager.
    //  Ask how many records to display (or "all").
    //  Use the PriceHistory forward Iterator to print records.
    cout << "(TODO: implement menuDisplayHistory)" << endl;
}

void menuSearchByDate(StockManager<Stock>& stockManager, StockManager<ETF>& etfManager) {
    // TODO:
    //  Ask user for ticker, start date, and end date.
    //  Call history->printRange(startDate, endDate).
    cout << "(TODO: implement menuSearchByDate)" << endl;
}

void menuBSTRangeSearch(StockBST& bst) {
    // TODO:
    //  Ask for a low and high return % value.
    //  Call bst.rangeSearch(low, high, results) and print each result.
    cout << "(TODO: implement menuBSTRangeSearch)" << endl;
}

void menuInsertIntoBST(StockBST& bst, StockManager<Stock>& stockManager) {
    // TODO:
    //  Ask for a ticker and a year.
    //  Find the stock, compute calculateAnnualReturn(year), insert into BST.
    cout << "(TODO: implement menuInsertIntoBST)" << endl;
}

void menuDisplayBST(StockBST& bst) {
    // TODO:
    //  Ask user which traversal: inorder / preorder / postorder.
    //  Call the appropriate BST method.
    cout << "(TODO: implement menuDisplayBST)" << endl;
}

void menuAddToPortfolio(Portfolio& portfolio) {
    // TODO:
    //  Prompt for ticker, shares, price, date. Call portfolio.buyShares(...).
    cout << "(TODO: implement menuAddToPortfolio)" << endl;
}

void menuRemoveFromPortfolio(Portfolio& portfolio) {
    // TODO:
    //  Prompt for ticker, shares, current price, date. Call portfolio.sellShares(...).
    cout << "(TODO: implement menuRemoveFromPortfolio)" << endl;
}

void menuQueueOrder(Portfolio& portfolio) {
    // TODO:
    //  Prompt for order details (ticker, type, side, target price, shares, date).
    //  Build an Order struct and call portfolio.queueOrder(order).
    cout << "(TODO: implement menuQueueOrder)" << endl;
}

void menuExecuteOrder(Portfolio& portfolio) {
    // TODO:
    //  Ask for the current market price and today's date.
    //  Call portfolio.executeNextOrder(currentPrice, date).
    cout << "(TODO: implement menuExecuteOrder)" << endl;
}

void menuUndoTrade(Portfolio& portfolio) {
    // TODO: Call portfolio.undoLastTrade() and confirm to the user.
    cout << "(TODO: implement menuUndoTrade)" << endl;
}

void menuRunStrategy(StockManager<ETF>& etfManager, StockManager<Stock>& stockManager) {
    // TODO:
    //  Sub-menu: choose strategy (1=FixedSIP, 2=DynamicSIP, 3=GoldenCross, 4=Momentum).
    //  Ask for: ticker, monthlyCapital, startYear, endYear, strategy parameters.
    //  Find the asset, get its PriceHistory, create the strategy object,
    //  call strategy.backtest(...), then strategy.printResult(result).
    cout << "(TODO: implement menuRunStrategy)" << endl;
}

void menuCompareStrategies(StockManager<ETF>& etfManager) {
    // TODO:
    //  Use SPY (find in etfManager). Ask for monthlyCapital, startYear, endYear.
    //  Run all 4 strategies with default parameters.
    //  Print a comparison table showing all SimResults side by side.
    cout << "(TODO: implement menuCompareStrategies)" << endl;
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
void parameterSweep(ETF* spy, double monthlyCapital, int startYear, int endYear, StockBST& bst) {
    // TODO:
    //  For dipThreshold from 3.0 to 20.0 (step 1.0):
    //    Create DynamicSIPStrategy(dipThreshold, 10.0, 2.0)
    //    Run backtest on spy->getHistory()
    //    Insert into bst keyed by result.finalValue, with year=0
    //      and ticker = "dip=" + to_string((int)dipThreshold) + "%"
    //  After all insertions:
    //    Call bst.inorder() to print results ranked worst to best
    //    Call bst.findMax() to identify the optimal threshold
    cout << "(TODO: implement parameterSweep)" << endl;
}
