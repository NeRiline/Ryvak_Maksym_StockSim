# AI Prompts Log — ESE 224 Final Project

**Student Name:** Maksym Ryvak and Rubayeth Hasan
**Student ID:**       116607096 116179991

---

## Instructions

For every AI prompt you use during this project, add an entry below in the format shown.
This file gets submitted with your project and is the source of truth for your AI Documentation grade (10 pts).

You must also fill out the AI Usage Feedback Form after completing the project:
https://docs.google.com/forms/d/1NpkLKFrnvGgSsycC4Ba0IhCPSL_MIHtGaU-X84D5CnY

---

## Format

```
### Section: [e.g., PriceHistory / CSVParser / GoldenCrossStrategy / Report Section 3]
**Prompt:**
[paste your exact prompt here]

**What you changed / why (if any):**
[what was wrong or insufficient, and what you modified]
```

---

## Prompts


---

## Prompts

### Section: CSVParser
**Prompt:**
How would we implement a C++ CSVParser class that loads Yahoo Finance CSV files into a custom PriceHistory linked list, skips malformed rows, filters dates from 2000-01-01 to 2020-01-01, and handles both standard Yahoo column order and the different NVDA column order by reading the header names?

**What you changed / why (if any):**
We modified the parser to match our exact PriceHistory::append parameters and added checks for empty fields so malformed CSV rows would not crash the program.

---

### Section: PriceHistory / Linked List Iterators
**Prompt:**
Explain how to implement a doubly linked list called PriceHistory in C++ using PriceNode objects. It should Include append, findByDate, printRange, a forward iterator using next pointers, a reverse iterator using prev pointers, and a destructor that correctly deletes every node.

**What you changed / why (if any):**
We rewrote the iterator code so it did not use STL iterator inheritance. We also adjusted the destructor to match the raw pointer structure required by the project.

---

### Section: Stock / ETF / FinancialAsset
**Prompt:**
Help me design the FinancialAsset abstract base class and the Stock and ETF derived classes for a C++ stock simulator. FinancialAsset should have virtual methods like printSummary, calculateAnnualReturn, and getType. Stock should own a PriceHistory pointer, load CSV data, get prices by date, and calculate annual return. ETF should extend Stock behavior and include an expense ratio.

**What you changed / why (if any):**
We changed the constructors and include files to match our project structure. We also made sure destructors were virtual so derived objects could be deleted safely through base class pointers.

---

### Section: CircularQueue
**Prompt:**
Write a fixed-size CircularQueue class in C++ using a dynamic array, not std::queue. It should support enqueue with overwrite behavior when full, dequeue, peek, getAverage, isFull, isEmpty, getCount, and a destructor. This will be used for 50-day and 200-day moving averages.

**What you changed / why (if any):**
We adjusted the enqueue logic to overwrite the oldest value when the queue was full, because the moving average only needs the most recent N closing prices.

---

### Section: TradeStack
**Prompt:**
Help me implement a pointer-based TradeStack class in C++ for our portfolio trade history. It should use a StackNode struct, support push, pop, peek, isEmpty, getSize, printAll, and properly delete all nodes in the destructor. Do not use std::stack.

**What you changed / why (if any):**
We added empty-stack checks before pop and peek so the program would not access invalid memory if the user tried to undo a trade before any trades existed.

---

### Section: Report Data Structure Design / TradeStack
**Prompt:**
Help me explain the TradeStack section of our StockSim report. Explain how the stack stores completed trades in LIFO order, why this supports undo operations, and the time complexity of push, pop, peek, and printAll.

**What you changed / why (if any):**
We revised the explanation so it focused on why the most recent trade should be undone first and how that matches stack behavior.

---

### Section: OrderQueue
**Prompt:**
Help me implement a FIFO OrderQueue class in C++ using linked nodes and raw pointers. Each Order should store ticker, type, side, targetPrice, shares, and submittedDate. The queue should support enqueue, dequeue, peek, isEmpty, getSize, printAll, and a destructor. Do not use std::queue.

**What you changed / why (if any):**
We modified the queue so both front and rear pointers update correctly when the last item is removed. This fixed errors when dequeuing the final pending order.

---


### Section: StockBST
**Prompt:**
Help me implement a binary search tree in C++ for storing stock performance by a double key, such as return percentage or final portfolio value. The BST should support insert, exact search, rangeSearch, inorder, preorder, postorder, getHeight, clear, and a destructor.

**What you changed / why (if any):**
We changed the print formatting to show ticker, key, and year clearly. We also adjusted rangeSearch so it only searches branches that could contain values inside the requested range.

---

### Section: Portfolio
**Prompt:**
Help me implement a Portfolio class in C++ that stores holdings in a std::vector, supports buying shares, selling shares, undoing the last trade using TradeStack, queueing pending orders using OrderQueue, executing the next order, calculating total value and return, printing holdings, and sorting holdings by ticker or unrealized return.

**What you changed / why (if any):**
We adjusted the buy and sell logic to correctly update average cost basis and share counts. We also added checks so the user cannot sell more shares than they own.

---

### Section: TradingStrategy Base Class
**Prompt:**
Help me implement a TradingStrategy abstract base class for our StockSim project. It should define a SimResult struct, a pure virtual backtest method, a pure virtual getName method, and helper methods to calculate CAGR and max drawdown from portfolio values.

**What you changed / why (if any):**
We changed the CAGR formula to use the correct number of years from the backtest range. We also made max drawdown track the running peak before calculating each decline.

---

### Section: FixedSIPStrategy
**Prompt:**
Help me write a FixedSIPStrategy backtest in C++. The strategy should iterate through PriceHistory, invest a fixed monthly amount on the first available trading day of each month, allow fractional shares, track total invested, final value, total return, CAGR, max drawdown, and total trades.

**What you changed / why (if any):**
We modified the month-detection logic so it invests only once per month, even though the dataset skips weekends and holidays.

---

### Section: DynamicSIPStrategy
**Prompt:**
Help me design a DynamicSIPStrategy for our StockSim project. It should remain budget-neutral compared with Fixed SIP, only use price data available up to the current day, invest more during market dips, carry unused monthly cash forward, and return the same SimResult metrics as the other strategies.

**Prompt:**
How can the DynamicSIPStrategy be improved? How much better than Fixed SIP it can be in theory?

**What you changed / why (if any):**
We changed the algorithm to make sure total deployed capital did not exceed the Fixed SIP budget. We also removed any logic that would accidentally use future price information.

---

### Section: GoldenCrossStrategy
**Prompt:**
Help me implement a GoldenCrossStrategy in C++ using two CircularQueue objects for 50-day and 200-day moving averages. The strategy should buy when the 50-day moving average crosses above the 200-day moving average and sell when it crosses below.

**What you changed / why (if any):**
We added variables to store the previous day’s moving average relationship so the program detects an actual crossover instead of repeatedly buying or selling every day.

---

### Section: MomentumStrategy
**Prompt:**
Help me implement a 6-month MomentumStrategy in C++. It should use PriceHistory data to calculate trailing return, buy or stay invested when momentum is above a threshold, sell or stay in cash when momentum is negative, and rebalance monthly.

**What you changed / why (if any):**
We adjusted the lookback logic because the stock data skips weekends and holidays. We used available trading days instead of assuming exactly one node per calendar day.

---

### Section: StockManager Template
**Prompt:**
Help me write a C++ template class called StockManager that stores pointers to FinancialAsset-derived objects in a vector. It should support addAsset, removeAsset, findByTicker, sortByAnnualReturn, sortByTicker, printAll, getCount, and a destructor that deletes stored pointers.

**What you changed / why (if any):**
We kept the template implementation fully in the header file because C++ templates need their definitions available at compile time.

---

### Section: Main Menu System
**Prompt:**
Help me complete the main.cpp menu handlers for our StockSim project. The menu should load stock data, display price history, search by date range, insert and search BST results, manage portfolio trades, queue and execute orders, undo trades, run one strategy, compare all strategies, and display trade history.

**What you changed / why (if any):**
We adjusted the prompts and function calls to match our exact class names and method signatures. We also added input validation in some menu options to prevent crashes from invalid user choices.

---

### Section: Debugging / Compilation
**Prompt:**
Our C++ StockSim project is giving compilation errors related to class headers, constructors, and method signatures. Help me identify why the files are not linking correctly and explain how to fix include guards, missing methods, mismatched function declarations, and constructor calls.


**What you changed / why (if any):**
We applied only the fixes that matched our own header files. Some suggested names did not match our project, so We renamed them to match our existing classes.

—

### Section: Trading Strategy Writeup
**Prompt:**
Given the trading strategies that were implemented earlier, please give me a write up on how these trading strategies function, what parameters they used, and what the expected outcome was.

**What you changed / why (if any):**
—

### Section: Improvements
**Prompt:**
Given our project and integrations, what improvements could our strategies/implementations have that would make a more effective simulation?

