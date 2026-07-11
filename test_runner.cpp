#include "models/Models.h"
#include "storage/Storage.h"
#include "utils/Utils.h"
#include "scheduler/Scheduler.h"
#include <iostream>
#include <cassert>
#include <fstream>
#include <vector>

using namespace std;
using namespace Models;

void resetFiles() {
    ofstream f1(Storage::USERS_FILE, ios::trunc);
    ofstream f2(Storage::ROOMS_FILE, ios::trunc);
    ofstream f3(Storage::BOOKINGS_FILE, ios::trunc);
    ofstream f4(Storage::WAITLIST_FILE, ios::trunc);
}

int main() {
    cout << "========================================================\n";
    cout << "         Running Automated Verification Suite           \n";
    cout << "========================================================\n";
    resetFiles();

    // 1. User Setup
    vector<User> users;
    User u1 = {1, "admin", "admin123", "admin"};
    User u2 = {2, "alice", "alice123", "student"};
    User u3 = {3, "bob", "bob123", "student"};
    users.push_back(u1);
    users.push_back(u2);
    users.push_back(u3);
    assert(Storage::saveUsers(users));
    cout << "[PASS] User Accounts Initialized Successfully\n";

    // 2. Room Setup
    Room r1 = {"R101", "Classroom 101", "Classroom", "Building A", 50};
    assert(Scheduler::addRoom(r1));
    assert(Scheduler::roomExists("R101"));
    cout << "[PASS] Room CRUD: Added Room R101 (Classroom)\n";

    // 3. First Booking
    bool joinedWait = false;
    string b1 = Scheduler::bookRoom("R101", 2, "2026-07-15", "10:00", "12:00", false, joinedWait);
    assert(!b1.empty());
    assert(!joinedWait);
    cout << "[PASS] Room Booking: Confirmed Booking ID: " << b1 << " (Alice: 10:00 - 12:00)\n";

    // 4. Booking Conflict Validation (Same Room, Overlapping Time)
    string b2 = Scheduler::bookRoom("R101", 3, "2026-07-15", "11:00", "13:00", false, joinedWait);
    assert(b2.empty());
    assert(!joinedWait);
    cout << "[PASS] Conflict Check: Overlap booking successfully blocked\n";

    // 5. Waiting List Queue Placement
    string w1 = Scheduler::bookRoom("R101", 3, "2026-07-15", "11:00", "13:00", true, joinedWait);
    assert(!w1.empty());
    assert(joinedWait);
    cout << "[PASS] Waitlist: Bob queued on waitlist (Ticket: " << w1 << ")\n";

    // Check waitlist exists in storage
    auto waitlist = Scheduler::getWaitlist();
    assert(waitlist.size() == 1);

    // 6. Cancellation & Auto-Promotion Event
    vector<string> promotions;
    bool cancelSuccess = Scheduler::cancelBooking(b1, 2, false, promotions);
    assert(cancelSuccess);
    cout << "[PASS] Cancel Booking: Booking B001 cancelled successfully\n";

    // Check Bob was promoted
    assert(promotions.size() == 1);
    cout << "[PASS] Waitlist Auto-Promotion Event Triggered:\n";
    cout << "       -> " << promotions[0] << "\n";

    // Check Bob's active booking
    auto bookings = Scheduler::getAllBookings();
    assert(bookings[0].bookingId == b1 && bookings[0].status == "Cancelled");
    assert(bookings[1].status == "Confirmed" && bookings[1].userId == 3);
    cout << "[PASS] Booking Status Verified: Bob is now Confirmed (11:00 - 13:00)\n";

    // Check waitlist is now clear
    auto waitlistAfter = Scheduler::getWaitlist();
    assert(waitlistAfter.empty());
    cout << "[PASS] Waitlist Queue Cleaned Up\n";

    // 7. Utilization Statistics Check
    auto stats = Scheduler::getUtilizationStats();
    assert(stats.size() == 1);
    assert(stats[0].totalBookings == 1);
    assert(stats[0].totalHours == 2.0);
    cout << "[PASS] Dashboard Stats Computed Correctly\n";

    cout << "\n========================================================\n";
    cout << "         ALL TESTS PASSED SUCCESSFULLY!                 \n";
    cout << "========================================================\n";
    return 0;
}
