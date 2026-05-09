#include "CSVParser.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <cctype>

namespace {
    string trim(const string& s) {
        size_t start = 0;
        size_t end = s.size();
        while (start < end && isspace(static_cast<unsigned char>(s[start]))) {
            start++;
        }
        while (end > start && isspace(static_cast<unsigned char>(s[end - 1]))) {
            end--;
        }
        string out = s.substr(start, end - start);
        if (!out.empty() && out.front() == '"' && out.back() == '"') {
            out = out.substr(1, out.size() - 2);
        }
        return out;
    }

    vector<string> splitCSV(const string& line) {
        vector<string> fields;
        string field;
        stringstream ss(line);
        while (getline(ss, field, ',')) {
            fields.push_back(trim(field));
        }
        return fields;
    }

    int findColumnIndex(const vector<string>& columns, const string& name) {
        for (size_t i = 0; i < columns.size(); ++i) {
            if (columns[i] == name) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    bool parseDouble(const string& s, double& out) {
        if (s.empty() || s == "null") {
            return false;
        }
        try {
            out = stod(s);
            return true;
        } catch (...) {
            return false;
        }
    }

    bool parseLong(const string& s, long& out) {
        if (s.empty() || s == "null") {
            return false;
        }
        try {
            out = stol(s);
            return true;
        } catch (...) {
            return false;
        }
    }
}

PriceHistory* CSVParser::loadHistory(const string& filename) {
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cout << "Failed to open CSV file: " << filename << "\n";
        return nullptr;
    }

    string headerLine;
    if (!getline(file, headerLine)) {
        return nullptr;
    }

    vector<string> columns = splitCSV(headerLine);
    int dateIdx = findColumnIndex(columns, "Date");
    int openIdx = findColumnIndex(columns, "Open");
    int highIdx = findColumnIndex(columns, "High");
    int lowIdx = findColumnIndex(columns, "Low");
    int closeIdx = findColumnIndex(columns, "Close");
    int volumeIdx = findColumnIndex(columns, "Volume");

    if (dateIdx < 0 || openIdx < 0 || highIdx < 0 || lowIdx < 0 || closeIdx < 0 || volumeIdx < 0) {
        cout << "CSV header missing required columns in: " << filename << "\n";
        return nullptr;
    }

    const string startDate = "2000-01-01";
    const string endDate = "2020-01-01";

    PriceHistory* history = new PriceHistory();
    string line;
    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        vector<string> fields = splitCSV(line);
        if (static_cast<int>(fields.size()) <= volumeIdx) {
            continue;
        }

        string date = fields[dateIdx];
        if (!dateInRange(date, startDate, endDate)) {
            continue;
        }

        double open = 0.0;
        double high = 0.0;
        double low = 0.0;
        double close = 0.0;
        long volume = 0;

        if (!parseDouble(fields[openIdx], open) ||
            !parseDouble(fields[highIdx], high) ||
            !parseDouble(fields[lowIdx], low) ||
            !parseDouble(fields[closeIdx], close) ||
            !parseLong(fields[volumeIdx], volume)) {
            continue;
        }

        history->append(date, open, high, low, close, volume);
    }

    return history;
}

bool CSVParser::dateInRange(const string& date, const string& start, const string& end) {
    return (date >= start && date <= end);
}

int CSVParser::extractYear(const string& date) {
    if (date.size() < 4) {
        return -1;
    }
    return stoi(date.substr(0, 4));
}

int CSVParser::extractMonth(const string& date) {
    if (date.size() < 7) {
        return -1;
    }
    return stoi(date.substr(5, 2));
}
