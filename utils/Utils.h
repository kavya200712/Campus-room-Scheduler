#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

using namespace std;

namespace Utils {
    // Helper to trim leading/trailing whitespace from a string
    string trim(const string& str);

    // Splits a string by a delimiter
    vector<string> split(const string& str, char delimiter);

    // Converts HH:MM format string to minutes since midnight
    // Returns -1 if the format is invalid or values are out of bounds
    int timeToMinutes(const string& timeStr);

    // Converts minutes since midnight back to HH:MM format
    string minutesToTime(int minutes);

    // Validates if the string is a valid date in YYYY-MM-DD format
    bool isValidDate(const string& dateStr);

    // Helper to check if date1 is before date2 (both in YYYY-MM-DD format)
    bool isDateBefore(const string& date1, const string& date2);

    // Get current system date in YYYY-MM-DD format
    string getCurrentDate();
}

#endif // UTILS_H
