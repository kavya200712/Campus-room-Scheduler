#ifndef STORAGE_H
#define STORAGE_H

#include "../models/Models.h"
#include <vector>
#include <string>

using namespace std;

namespace Storage {
    const string USERS_FILE = "users.txt";
    const string ROOMS_FILE = "rooms.txt";
    const string BOOKINGS_FILE = "bookings.txt";
    const string WAITLIST_FILE = "waiting_list.txt";

    // Ensures that the storage files exist, creating empty files if they are missing.
    void initFiles();

    bool loadUsers(vector<Models::User>& users);
    bool saveUsers(const vector<Models::User>& users);

    bool loadRooms(vector<Models::Room>& rooms);
    bool saveRooms(const vector<Models::Room>& rooms);

    bool loadBookings(vector<Models::Booking>& bookings);
    bool saveBookings(const vector<Models::Booking>& bookings);

    bool loadWaitlist(vector<Models::WaitlistEntry>& waitlist);
    bool saveWaitlist(const vector<Models::WaitlistEntry>& waitlist);
}

#endif // STORAGE_H
