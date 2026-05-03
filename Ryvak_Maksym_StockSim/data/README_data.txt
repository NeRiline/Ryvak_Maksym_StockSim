How to download the required CSV data files
===========================================

This project uses historical stock data from Yahoo Finance (free, no login required).

For each ticker below, follow these steps:

1. Go to: https://finance.yahoo.com
2. Search for the ticker symbol in the search bar
3. Click on the result, then select the "Historical Data" tab
4. Set Time Period to "Max" (this gives the full available history)
5. Set Frequency to "Daily"
6. Click "Apply", then "Download"
7. Rename the downloaded file as shown below and place it in this data/ folder

Required files:
---------------
  SPY.csv    (S&P 500 ETF Trust — primary dataset, ~25 years)
  AAPL.csv   (Apple Inc.)
  TSLA.csv   (Tesla Inc. — note: only available from mid-2010)

Expected CSV format (Yahoo Finance default):
--------------------------------------------
  Date,Open,High,Low,Close,Adj Close,Volume
  2000-01-03,148.25,148.25,143.88,145.44,111.55,8164300
  ...

Your CSVParser::loadHistory() must:
  - Skip the first (header) row
  - Use the "Close" column (index 4) as the closing price
  - Handle missing/null values gracefully (skip the row)
  - Work correctly even if date ranges differ across files
    (TSLA starts in 2010; SPY starts in 1993)
