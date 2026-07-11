#include "Storage.h"
#include "../utils/Utils.h"
#include <fstream>
#include <iostream>

namespace Storage {
    using namespace std;
    using namespace Models;

    void initFiles() {
        ofstream f1(USERS_FILE, ios::app);
        ofstream f2(ROOMS_FILE, ios::app);
        ofstream f3(BOOKINGS_FILE, ios::app);
        ofstream f4(WAITLIST_FILE, ios::app);
    }

    bool loadUsers(vector<User>& users) {
        users.clear();
        ifstream file(USERS_FILE);
        if (!file.is_open()) return false;

        string line;
        while (getline(file, line)) {
            line = Utils::trim(line);
            if (line.empty()) continue;
            vector<string> tokens = Utils::split(line, ',');
            if (tokens.size() == 4) {
                User u;
                try {
                    u.id = stoi(Utils::trim(tokens[0]));
                    u.username = Utils::trim(tokens[1]);
                    u.password = Utils::trim(tokens[2]);
                    u.role = Utils::trim(tokens[3]);
                    users.push_back(u);
                } catch (...) {
                    // Ignore malformed lines
                }
            }
        }
        return true;
    }

    bool saveUsers(const vector<User>& users) {
        ofstream file(USERS_FILE, ios::trunc);
        if (!file.is_open()) return false;

        for (const auto& u : users) {
            file << u.id << ","
                 << u.username << ","
                 << u.password << ","
                 << u.role << "\n";
        }
        return true;
    }

    bool loadRooms(vector<Room>& rooms) {
        rooms.clear();
        ifstream file(ROOMS_FILE);
        if (!file.is_open()) return false;

        string line;
        while (getline(file, line)) {
            line = Utils::trim(line);
            if (line.empty()) continue;
            vector<string> tokens = Utils::split(line, ',');
            if (tokens.size() == 5) {
                Room r;
                try {
                    r.id = Utils::trim(tokens[0]);
                    r.name = Utils::trim(tokens[1]);
                    r.type = Utils::trim(tokens[2]);
                    r.building = Utils::trim(tokens[3]);
                    r.capacity = stoi(Utils::trim(tokens[4]));
                    rooms.push_back(r);
                } catch (...) {
                    // Ignore malformed lines
                }
            }
        }
        return true;
    }

    bool saveRooms(const vector<Room>& rooms) {
        ofstream file(ROOMS_FILE, ios::trunc);
        if (!file.is_open()) return false;

        for (const auto& r : rooms) {
            file << r.id << ","
                 << r.name << ","
                 << r.type << ","
                 << r.building << ","
                 << r.capacity << "\n";
        }
        return true;
    }

    bool loadBookings(vector<Booking>& bookings) {
        bookings.clear();
        ifstream file(BOOKINGS_FILE);
        if (!file.is_open()) return false;

        string line;
        while (getline(file, line)) {
            line = Utils::trim(line);
            if (line.empty()) continue;
            vector<string> tokens = Utils::split(line, ',');
            if (tokens.size() == 7) {
                Booking b;
                try {
                    b.bookingId = Utils::trim(tokens[0]);
                    b.roomId = Utils::trim(tokens[1]);
                    b.userId = stoi(Utils::trim(tokens[2]));
                    b.date = Utils::trim(tokens[3]);
                    b.startTime = Utils::trim(tokens[4]);
                    b.endTime = Utils::trim(tokens[5]);
                    b.status = Utils::trim(tokens[6]);
                    bookings.push_back(b);
                } catch (...) {
                    // Ignore malformed lines
                }
            }
        }
        return true;
    }

    bool saveBookings(const vector<Booking>& bookings) {
        ofstream file(BOOKINGS_FILE, ios::trunc);
        if (!file.is_open()) return false;

        for (const auto& b : bookings) {
            file << b.bookingId << ","
                 << b.roomId << ","
                 << b.userId << ","
                 << b.date << ","
                 << b.startTime << ","
                 << b.endTime << ","
                 << b.status << "\n";
        }
        return true;
    }

    bool loadWaitlist(vector<WaitlistEntry>& waitlist) {
        waitlist.clear();
        ifstream file(WAITLIST_FILE);
        if (!file.is_open()) return false;

        string line;
        while (getline(file, line)) {
            line = Utils::trim(line);
            if (line.empty()) continue;
            vector<string> tokens = Utils::split(line, ',');
            if (tokens.size() == 7) {
                WaitlistEntry w;
                try {
                    w.waitlistId = Utils::trim(tokens[0]);
                    w.roomId = Utils::trim(tokens[1]);
                    w.userId = stoi(Utils::trim(tokens[2]));
                    w.date = Utils::trim(tokens[3]);
                    w.startTime = Utils::trim(tokens[4]);
                    w.endTime = Utils::trim(tokens[5]);
                    w.timestamp = stoll(Utils::trim(tokens[6]));
                    waitlist.push_back(w);
                } catch (...) {
                    // Ignore malformed lines
                }
            }
        }
        return true;
    }

    bool saveWaitlist(const vector<WaitlistEntry>& waitlist) {
        ofstream file(WAITLIST_FILE, ios::trunc);
        if (!file.is_open()) return false;

        for (const auto& w : waitlist) {
            file << w.waitlistId << ","
                 << w.roomId << ","
                 << w.userId << ","
                 << w.date << ","
                 << w.startTime << ","
                 << w.endTime << ","
                 << w.timestamp << "\n";
        }
        return true;
    }
}
