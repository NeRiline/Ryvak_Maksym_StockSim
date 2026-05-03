#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include "PriceHistory.h"
#include <string>
using namespace std;

// Static utility class for loading Yahoo Finance historical CSV files.
// Yahoo Finance CSV format (header row):
//   Date, Open, High, Low, Close, Adj Close, Volume
// This parser uses the "Close" column as the closing price.
// Dates are formatted "YYYY-MM-DD".
class CSVParser {
public:
    // Opens the CSV file at 'filename', skips the header row, and builds
    // a PriceHistory linked list from each valid data row.
    // Returns a heap-allocated PriceHistory* (caller owns it).
    // Returns nullptr if the file cannot be opened.
    static PriceHistory* loadHistory(const string& filename);

    // Returns true if 'date' falls within [startDate, endDate] (inclusive).
    // Works on "YYYY-MM-DD" strings: lexicographic == chronological.
    static bool dateInRange(const string& date, const string& start, const string& end);

    // Extracts the 4-digit year from "YYYY-MM-DD" and returns it as an int.
    static int extractYear(const string& date);

    // Extracts the 2-digit month from "YYYY-MM-DD" and returns it as an int.
    static int extractMonth(const string& date);

private:
    CSVParser() = delete; // all-static class — no instantiation
};

#endif // CSV_PARSER_H
