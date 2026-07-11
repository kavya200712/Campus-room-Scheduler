#include "Utils.h"
#include <ctime>
#include <cctype>

namespace Utils {
    using namespace std;
    string trim(const string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }

    vector<string> split(const string& str, char delimiter) {
        vector<string> tokens;
        string token = "";
        for (int i = 0; i < str.length(); i++) {
            if (str[i] == delimiter) {
                tokens.push_back(token);
                token = "";
            } else {
                token += str[i];
            }
        }
        tokens.push_back(token);
        return tokens;
    }

    int timeToMinutes(const string& timeStr) {
        string trimmed = trim(timeStr);
        if (trimmed.length() != 5 || trimmed[2] != ':') return -1;
        for (int i = 0; i < 5; ++i) {
            if (i == 2) continue;
            if (!isdigit(trimmed[i])) return -1;
        }
        int hours = stoi(trimmed.substr(0, 2));
        int minutes = stoi(trimmed.substr(3, 2));
        if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59) return -1;
        return hours * 60 + minutes;
    }

    string minutesToTime(int minutes) {
        if (minutes < 0 || minutes >= 1440) return "00:00";
        int hours = minutes / 60;
        int mins = minutes % 60;
        
        string hStr = to_string(hours);
        if (hStr.length() < 2) hStr = "0" + hStr;
        
        string mStr = to_string(mins);
        if (mStr.length() < 2) mStr = "0" + mStr;
        
        return hStr + ":" + mStr;
    }

    bool isValidDate(const string& dateStr) {
        string trimmed = trim(dateStr);
        if (trimmed.length() != 10 || trimmed[4] != '-' || trimmed[7] != '-') return false;
        for (int i = 0; i < 10; ++i) {
            if (i == 4 || i == 7) continue;
            if (!isdigit(trimmed[i])) return false;
        }
        int year = stoi(trimmed.substr(0, 4));
        int month = stoi(trimmed.substr(5, 2));
        int day = stoi(trimmed.substr(8, 2));

        if (year < 1900 || year > 2100) return false;
        if (month < 1 || month > 12) return false;

        int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        if (month == 2) {
            bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
            if (isLeap) daysInMonth[1] = 29;
        }

        if (day < 1 || day > daysInMonth[month - 1]) return false;

        return true;
    }

    bool isDateBefore(const string& date1, const string& date2) {
        return date1 < date2;
    }

    string getCurrentDate() {
        time_t t = time(nullptr);
        tm* now = localtime(&t);
        if (!now) return "2026-07-09";
        
        string yStr = to_string(now->tm_year + 1900);
        while (yStr.length() < 4) yStr = "0" + yStr;
        
        string mStr = to_string(now->tm_mon + 1);
        if (mStr.length() < 2) mStr = "0" + mStr;
        
        string dStr = to_string(now->tm_mday);
        if (dStr.length() < 2) dStr = "0" + dStr;
        
        return yStr + "-" + mStr + "-" + dStr;
    }
}
