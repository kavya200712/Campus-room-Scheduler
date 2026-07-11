#include "Scheduler.h"
#include "../storage/Storage.h"
#include "../utils/Utils.h"
#include <ctime>
#include <iostream>

namespace Scheduler {
    using namespace std;
    using namespace Models;

    // Helper to generate the next sequential ID with a prefix
    string getNextId(const string& prefix, const vector<string>& existingIds) {
        int maxNum = 0;
        for (int i = 0; i < existingIds.size(); i++) {
            const string& id = existingIds[i];
            if (id.length() > prefix.length() && id.substr(0, prefix.length()) == prefix) {
                try {
                    int num = stoi(id.substr(prefix.length()));
                    if (num > maxNum) maxNum = num;
                } catch (...) {}
            }
        }
        int nextNum = maxNum + 1;
        string numStr = to_string(nextNum);
        while (numStr.length() < 3) {
            numStr = "0" + numStr;
        }
        return prefix + numStr;
    }

    bool roomExists(const string& roomId) {
        vector<Room> rooms;
        if (!Storage::loadRooms(rooms)) return false;
        for (const auto& r : rooms) {
            if (r.id == roomId) return true;
        }
        return false;
    }

    bool addRoom(const Room& room) {
        vector<Room> rooms;
        Storage::loadRooms(rooms);
        for (const auto& r : rooms) {
            if (r.id == room.id) {
                return false; // Room ID must be unique
            }
        }
        rooms.push_back(room);
        return Storage::saveRooms(rooms);
    }

    bool updateRoom(const string& roomId, const Room& updatedRoom) {
        vector<Room> rooms;
        if (!Storage::loadRooms(rooms)) return false;
        bool found = false;
        for (auto& r : rooms) {
            if (r.id == roomId) {
                r.name = updatedRoom.name;
                r.type = updatedRoom.type;
                r.building = updatedRoom.building;
                r.capacity = updatedRoom.capacity;
                found = true;
                break;
            }
        }
        if (!found) return false;
        return Storage::saveRooms(rooms);
    }

    bool deleteRoom(const string& roomId) {
        vector<Room> rooms;
        if (!Storage::loadRooms(rooms)) return false;
        
        bool found = false;
        for (int i = 0; i < rooms.size(); i++) {
            if (rooms[i].id == roomId) {
                rooms.erase(rooms.begin() + i);
                found = true;
                break;
            }
        }
        
        if (!found) return false; // Room not found
        Storage::saveRooms(rooms);

        // Cancel all active bookings for this room
        vector<Booking> bookings;
        if (Storage::loadBookings(bookings)) {
            bool modified = false;
            for (auto& b : bookings) {
                if (b.roomId == roomId && b.status == "Confirmed") {
                    b.status = "Cancelled";
                    modified = true;
                }
            }
            if (modified) {
                Storage::saveBookings(bookings);
            }
        }

        // Clear waitlist for this room
        vector<WaitlistEntry> waitlist;
        if (Storage::loadWaitlist(waitlist)) {
            vector<WaitlistEntry> remainingWaitlist;
            for (int i = 0; i < waitlist.size(); i++) {
                if (waitlist[i].roomId != roomId) {
                    remainingWaitlist.push_back(waitlist[i]);
                }
            }
            waitlist = remainingWaitlist;
            Storage::saveWaitlist(waitlist);
        }

        return true;
    }

    bool isRoomAvailable(const string& roomId, const string& date, int startMin, int endMin, const string& skipBookingId) {
        vector<Booking> bookings;
        if (!Storage::loadBookings(bookings)) return true;

        for (int i = 0; i < bookings.size(); i++) {
            const auto& b = bookings[i];
            if (b.status == "Confirmed" && b.roomId == roomId && b.date == date) {
                if (!skipBookingId.empty() && b.bookingId == skipBookingId) {
                    continue;
                }
                int bStart = Utils::timeToMinutes(b.startTime);
                int bEnd = Utils::timeToMinutes(b.endTime);
                if (bStart != -1 && bEnd != -1) {
                    // Check overlap
                    if (startMin < bEnd && bStart < endMin) {
                        return false; // Conflicting slot
                    }
                }
            }
        }
        return true;
    }

    // Custom helper for case-insensitive substring search
    bool containsIgnoreCase(string str, string sub) {
        for (int i = 0; i < str.length(); i++) {
            str[i] = tolower(str[i]);
        }
        for (int i = 0; i < sub.length(); i++) {
            sub[i] = tolower(sub[i]);
        }
        return str.find(sub) != string::npos;
    }

    vector<Room> searchRooms(const string& type, int minCapacity, const string& building, 
                                  const string& date, const string& startTime, const string& endTime) {
        vector<Room> rooms;
        vector<Room> results;
        if (!Storage::loadRooms(rooms)) return results;

        int searchStartMin = -1;
        int searchEndMin = -1;
        if (!date.empty() && !startTime.empty() && !endTime.empty()) {
            searchStartMin = Utils::timeToMinutes(startTime);
            searchEndMin = Utils::timeToMinutes(endTime);
        }

        for (int i = 0; i < rooms.size(); i++) {
            const auto& r = rooms[i];
            // Filter by room type
            if (!type.empty() && !containsIgnoreCase(r.type, type)) {
                continue;
            }
            // Filter by minimum capacity
            if (minCapacity != -1 && r.capacity < minCapacity) {
                continue;
            }
            // Filter by building
            if (!building.empty() && !containsIgnoreCase(r.building, building)) {
                continue;
            }
            // Filter by date & time availability
            if (searchStartMin != -1 && searchEndMin != -1) {
                if (!isRoomAvailable(r.id, date, searchStartMin, searchEndMin)) {
                    continue;
                }
            }
            results.push_back(r);
        }
        return results;
    }

    string bookRoom(const string& roomId, int userId, const string& date, 
                         const string& startTime, const string& endTime, bool forceWaitlist, bool& joinedWaitlist) {
        joinedWaitlist = false;

        if (!roomExists(roomId)) return "";
        if (!Utils::isValidDate(date)) return "";
        
        int startMin = Utils::timeToMinutes(startTime);
        int endMin = Utils::timeToMinutes(endTime);
        if (startMin == -1 || endMin == -1 || startMin >= endMin) return "";

        vector<Booking> bookings;
        Storage::loadBookings(bookings);

        bool available = isRoomAvailable(roomId, date, startMin, endMin);
        if (available) {
            vector<string> existingIds;
            for (int i = 0; i < bookings.size(); i++) {
                existingIds.push_back(bookings[i].bookingId);
            }
            string nextId = getNextId("B", existingIds);

            Booking newBooking;
            newBooking.bookingId = nextId;
            newBooking.roomId = roomId;
            newBooking.userId = userId;
            newBooking.date = date;
            newBooking.startTime = startTime;
            newBooking.endTime = endTime;
            newBooking.status = "Confirmed";

            bookings.push_back(newBooking);
            Storage::saveBookings(bookings);
            return nextId;
        } else {
            if (forceWaitlist) {
                vector<WaitlistEntry> waitlist;
                Storage::loadWaitlist(waitlist);

                vector<string> existingIds;
                for (int i = 0; i < waitlist.size(); i++) {
                    existingIds.push_back(waitlist[i].waitlistId);
                }
                string nextId = getNextId("W", existingIds);

                WaitlistEntry newEntry;
                newEntry.waitlistId = nextId;
                newEntry.roomId = roomId;
                newEntry.userId = userId;
                newEntry.date = date;
                newEntry.startTime = startTime;
                newEntry.endTime = endTime;
                newEntry.timestamp = static_cast<long long>(time(nullptr));

                waitlist.push_back(newEntry);
                Storage::saveWaitlist(waitlist);
                joinedWaitlist = true;
                return nextId;
            }
            return "";
        }
    }

    bool cancelBooking(const string& bookingId, int userId, bool isAdmin, vector<string>& promotedUsers) {
        promotedUsers.clear();
        vector<Booking> bookings;
        if (!Storage::loadBookings(bookings)) return false;

        int targetIdx = -1;
        for (int i = 0; i < bookings.size(); i++) {
            if (bookings[i].bookingId == bookingId) {
                targetIdx = i;
                break;
            }
        }

        if (targetIdx == -1) return false;
        if (!isAdmin && bookings[targetIdx].userId != userId) return false;
        if (bookings[targetIdx].status == "Cancelled") return false;

        bookings[targetIdx].status = "Cancelled";
        string roomCancelled = bookings[targetIdx].roomId;
        string dateCancelled = bookings[targetIdx].date;

        Storage::saveBookings(bookings);

        // Process waitlist for this room and date to auto-promote eligible slots
        vector<WaitlistEntry> waitlist;
        Storage::loadWaitlist(waitlist);

        // Filter waitlist entries for this room and date
        vector<WaitlistEntry> filteredWaitlist;
        for (int i = 0; i < waitlist.size(); i++) {
            if (waitlist[i].roomId == roomCancelled && waitlist[i].date == dateCancelled) {
                filteredWaitlist.push_back(waitlist[i]);
            }
        }

        // Bubble sort by timestamp (FIFO)
        for (int i = 0; i < filteredWaitlist.size(); i++) {
            for (int j = i + 1; j < filteredWaitlist.size(); j++) {
                if (filteredWaitlist[i].timestamp > filteredWaitlist[j].timestamp) {
                    WaitlistEntry temp = filteredWaitlist[i];
                    filteredWaitlist[i] = filteredWaitlist[j];
                    filteredWaitlist[j] = temp;
                }
            }
        }

        // Load users to lookup names for notification output
        vector<User> users;
        Storage::loadUsers(users);

        // Reload bookings vector to check availability after modifications
        Storage::loadBookings(bookings);

        vector<string> promotedIds; // IDs to remove from waitlist
        for (int i = 0; i < filteredWaitlist.size(); i++) {
            const auto& w = filteredWaitlist[i];
            int wStart = Utils::timeToMinutes(w.startTime);
            int wEnd = Utils::timeToMinutes(w.endTime);

            // Check if room is available for this waitlisted slot
            bool available = true;
            for (int j = 0; j < bookings.size(); j++) {
                const auto& b = bookings[j];
                if (b.status == "Confirmed" && b.roomId == w.roomId && b.date == w.date) {
                    int bStart = Utils::timeToMinutes(b.startTime);
                    int bEnd = Utils::timeToMinutes(b.endTime);
                    if (bStart != -1 && bEnd != -1) {
                        if (wStart < bEnd && bStart < wEnd) {
                            available = false;
                            break;
                        }
                    }
                }
            }

            if (available) {
                // Promote!
                vector<string> bookingIds;
                for (int j = 0; j < bookings.size(); j++) {
                    bookingIds.push_back(bookings[j].bookingId);
                }
                string newBookingId = getNextId("B", bookingIds);

                Booking newBooking;
                newBooking.bookingId = newBookingId;
                newBooking.roomId = w.roomId;
                newBooking.userId = w.userId;
                newBooking.date = w.date;
                newBooking.startTime = w.startTime;
                newBooking.endTime = w.endTime;
                newBooking.status = "Confirmed";

                bookings.push_back(newBooking);
                Storage::saveBookings(bookings);

                promotedIds.push_back(w.waitlistId);
                
                string username = "Unknown User";
                for (int j = 0; j < users.size(); j++) {
                    if (users[j].id == w.userId) {
                        username = users[j].username;
                        break;
                    }
                }
                
                string msg = "Waitlist entry " + w.waitlistId + " for student '" + username +
                                  "' promoted to active booking " + newBookingId + " (" + w.startTime + " - " + w.endTime + ").";
                promotedUsers.push_back(msg);
            }
        }

        // Remove promoted entries from waitlist and save
        if (!promotedIds.empty()) {
            vector<WaitlistEntry> remainingWaitlist;
            for (int i = 0; i < waitlist.size(); i++) {
                bool isPromoted = false;
                for (int j = 0; j < promotedIds.size(); j++) {
                    if (waitlist[i].waitlistId == promotedIds[j]) {
                        isPromoted = true;
                        break;
                    }
                }
                if (!isPromoted) {
                    remainingWaitlist.push_back(waitlist[i]);
                }
            }
            waitlist = remainingWaitlist;
            Storage::saveWaitlist(waitlist);
        }

        return true;
    }

    vector<Booking> getUserBookings(int userId) {
        vector<Booking> bookings;
        vector<Booking> results;
        if (!Storage::loadBookings(bookings)) return results;

        for (int i = 0; i < bookings.size(); i++) {
            if (bookings[i].userId == userId) {
                results.push_back(bookings[i]);
            }
        }
        return results;
    }

    vector<Booking> getAllBookings() {
        vector<Booking> bookings;
        Storage::loadBookings(bookings);
        return bookings;
    }

    vector<WaitlistEntry> getWaitlist() {
        vector<WaitlistEntry> waitlist;
        Storage::loadWaitlist(waitlist);
        return waitlist;
    }

    vector<UtilizationStats> getUtilizationStats() {
        vector<Room> rooms;
        vector<Booking> bookings;
        vector<UtilizationStats> statsList;

        if (!Storage::loadRooms(rooms)) return statsList;
        Storage::loadBookings(bookings);

        for (int i = 0; i < rooms.size(); i++) {
            const auto& r = rooms[i];
            UtilizationStats us;
            us.roomId = r.id;
            us.roomName = r.name;
            us.totalBookings = 0;
            us.totalHours = 0.0;

            for (int j = 0; j < bookings.size(); j++) {
                const auto& b = bookings[j];
                if (b.roomId == r.id && b.status == "Confirmed") {
                    us.totalBookings++;
                    int start = Utils::timeToMinutes(b.startTime);
                    int end = Utils::timeToMinutes(b.endTime);
                    if (start != -1 && end != -1 && end > start) {
                        us.totalHours += (end - start) / 60.0;
                    }
                }
            }
            statsList.push_back(us);
        }
        return statsList;
    }
}
