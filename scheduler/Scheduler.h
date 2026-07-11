#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../models/Models.h"
#include <vector>
#include <string>

using namespace std;

namespace Scheduler {
    // Room management
    bool addRoom(const Models::Room& room);
    bool updateRoom(const string& roomId, const Models::Room& updatedRoom);
    bool deleteRoom(const string& roomId);

    // Checks if a room exists
    bool roomExists(const string& roomId);

    // Checks availability
    bool isRoomAvailable(const string& roomId, const string& date, int startMin, int endMin, const string& skipBookingId = "");

    // Search rooms with optional filters
    // Empty strings or -1 for minCapacity means no filter for that field
    vector<Models::Room> searchRooms(const string& type, int minCapacity, const string& building, 
                                  const string& date, const string& startTime, const string& endTime);

    // Book room
    // Returns booking ID if successful, empty string if failed.
    // If forceWaitlist is true, will append to waitlist when overlapping occurs.
    // If joinedWaitlist becomes true, the user is added to waitlist.
    string bookRoom(const string& roomId, int userId, const string& date, 
                         const string& startTime, const string& endTime, bool forceWaitlist, bool& joinedWaitlist);

    // Cancel booking
    // Returns true if successfully cancelled (and handles waitlist promotion).
    // promotedUsers will contain messages about who was promoted from the waitlist.
    bool cancelBooking(const string& bookingId, int userId, bool isAdmin, vector<string>& promotedUsers);

    // Bookings and waitlist queries
    vector<Models::Booking> getUserBookings(int userId);
    vector<Models::Booking> getAllBookings();
    vector<Models::WaitlistEntry> getWaitlist();

    // Stats
    struct UtilizationStats {
        string roomId;
        string roomName;
        int totalBookings;
        double totalHours;
    };
    vector<UtilizationStats> getUtilizationStats();
}

#endif // SCHEDULER_H
