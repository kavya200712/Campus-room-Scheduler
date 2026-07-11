#include "models/Models.h"
#include "storage/Storage.h"
#include "utils/Utils.h"
#include "scheduler/Scheduler.h"
#include <iostream>

using namespace std;
using namespace Models;
#include <string>
#include <vector>
#include <iomanip>
#include <map>

// Safe input helper
string readLine() {
    string s;
    getline(cin, s);
    return Utils::trim(s);
}

// Choice helper
int readChoice() {
    string s = readLine();
    try {
        return stoi(s);
    } catch (...) {
        return -1;
    }
}

// User Context
User currentUser = {0, "", "", ""};
bool loggedIn = false;

// Forward declarations of menu handlers
void welcomeMenu();
void studentMenu();
void adminMenu();
void handleLogin();
void handleRegister();

// Student Actions
void searchRoomsWorkflow();
void bookRoomWorkflow();
void cancelBookingWorkflow(bool isAdmin);
void viewMyBookingsWorkflow();

// Admin Actions
void addRoomWorkflow();
void updateRoomWorkflow();
void deleteRoomWorkflow();
void viewAllBookingsWorkflow();
void viewWaitlistWorkflow();
void viewUtilizationWorkflow();

int main() {
    // Initialise files
    Storage::initFiles();

    // Default admin initialization if file is empty (for testing ease)
    vector<User> users;
    Storage::loadUsers(users);
    if (users.empty()) {
        // Create a default admin and student
        User defaultAdmin = {1, "admin", "admin123", "admin"};
        User defaultStudent = {2, "student", "student123", "student"};
        users.push_back(defaultAdmin);
        users.push_back(defaultStudent);
        Storage::saveUsers(users);
    }

    cout << "========================================================\n";
    cout << "   Welcome to Campus Resource Scheduler & Management   \n";
    cout << "========================================================\n";

    while (true) {
        if (!loggedIn) {
            welcomeMenu();
        } else {
            if (currentUser.role == "admin") {
                adminMenu();
            } else {
                studentMenu();
            }
        }
    }

    return 0;
}

void welcomeMenu() {
    cout << "\n--- Welcome Menu ---\n";
    cout << "1. Login\n";
    cout << "2. Register\n";
    cout << "3. Exit\n";
    cout << "Select option (1-3): ";
    
    int choice = readChoice();
    switch (choice) {
        case 1:
            handleLogin();
            break;
        case 2:
            handleRegister();
            break;
        case 3:
            cout << "Thank you for using Campus Resource Scheduler. Goodbye!\n";
            exit(0);
        default:
            cout << "Invalid choice. Please select 1, 2, or 3.\n";
            break;
    }
}

void handleLogin() {
    vector<User> users;
    Storage::loadUsers(users);

    cout << "\n=== Login ===\n";
    cout << "Username: ";
    string username = readLine();
    cout << "Password: ";
    string password = readLine();

    for (const auto& u : users) {
        if (u.username == username && u.password == password) {
            currentUser = u;
            loggedIn = true;
            cout << "Login successful! Welcome back, " << username << " (" << u.role << ").\n";
            return;
        }
    }
    cout << "Invalid username or password. Please try again.\n";
}

void handleRegister() {
    vector<User> users;
    Storage::loadUsers(users);

    cout << "\n=== Register ===\n";
    cout << "Enter username: ";
    string username = readLine();
    if (username.empty() || username.find(',') != string::npos) {
        cout << "Invalid username (cannot contain commas or be empty).\n";
        return;
    }

    for (const auto& u : users) {
        if (u.username == username) {
            cout << "Username already exists. Please choose a different name.\n";
            return;
        }
    }

    cout << "Enter password: ";
    string password = readLine();
    if (password.empty() || password.find(',') != string::npos) {
        cout << "Invalid password (cannot contain commas or be empty).\n";
        return;
    }

    cout << "Select Role:\n1. Student\n2. Admin\nChoice (1-2): ";
    int choice = readChoice();
    string role = "student";
    if (choice == 2) {
        role = "admin";
    } else if (choice != 1) {
        cout << "Invalid choice. Defaulting to Student role.\n";
    }

    int nextId = 1;
    for (const auto& u : users) {
        if (u.id >= nextId) nextId = u.id + 1;
    }

    User newUser = { nextId, username, password, role };
    users.push_back(newUser);
    if (Storage::saveUsers(users)) {
        cout << "Registration successful! You can now login.\n";
    } else {
        cout << "Database write error. Registration failed.\n";
    }
}

void studentMenu() {
    cout << "\n=== Student Portal (" << currentUser.username << ") ===\n";
    cout << "1. Search Rooms\n";
    cout << "2. Book a Room\n";
    cout << "3. Cancel a Booking\n";
    cout << "4. View My Bookings / History\n";
    cout << "5. Logout\n";
    cout << "Select option (1-5): ";

    int choice = readChoice();
    switch (choice) {
        case 1:
            searchRoomsWorkflow();
            break;
        case 2:
            bookRoomWorkflow();
            break;
        case 3:
            cancelBookingWorkflow(false);
            break;
        case 4:
            viewMyBookingsWorkflow();
            break;
        case 5:
            cout << "Logging out...\n";
            loggedIn = false;
            currentUser = {0, "", "", ""};
            break;
        default:
            cout << "Invalid selection.\n";
            break;
    }
}

void adminMenu() {
    cout << "\n=== Admin Dashboard (" << currentUser.username << ") ===\n";
    cout << "1. Add a Room\n";
    cout << "2. Update Room Details\n";
    cout << "3. Delete a Room\n";
    cout << "4. Search Rooms\n";
    cout << "5. View All Bookings & History\n";
    cout << "6. View Waiting List\n";
    cout << "7. View Room Utilization Stats\n";
    cout << "8. Cancel a Booking (Admin override)\n";
    cout << "9. Logout\n";
    cout << "Select option (1-9): ";

    int choice = readChoice();
    switch (choice) {
        case 1:
            addRoomWorkflow();
            break;
        case 2:
            updateRoomWorkflow();
            break;
        case 3:
            deleteRoomWorkflow();
            break;
        case 4:
            searchRoomsWorkflow();
            break;
        case 5:
            viewAllBookingsWorkflow();
            break;
        case 6:
            viewWaitlistWorkflow();
            break;
        case 7:
            viewUtilizationWorkflow();
            break;
        case 8:
            cancelBookingWorkflow(true);
            break;
        case 9:
            cout << "Logging out...\n";
            loggedIn = false;
            currentUser = {0, "", "", ""};
            break;
        default:
            cout << "Invalid selection.\n";
            break;
    }
}

// Output Formatting Helpers
void printRoomsTable(const vector<Room>& rooms) {
    if (rooms.empty()) {
        cout << "\nNo rooms found matching the criteria.\n";
        return;
    }
    cout << "\n" << left << setw(10) << "Room ID"
              << setw(20) << "Name"
              << setw(15) << "Type"
              << setw(15) << "Building"
              << right << setw(10) << "Capacity" << "\n";
    cout << string(70, '-') << "\n";
    for (const auto& r : rooms) {
        cout << left << setw(10) << r.id
                  << setw(20) << r.name
                  << setw(15) << r.type
                  << setw(15) << r.building
                  << right << setw(10) << r.capacity << "\n";
    }
    cout << string(70, '-') << "\n";
}

// Workflows
void searchRoomsWorkflow() {
    cout << "\n--- Search Rooms (Leave blank to bypass any filter) ---\n";
    
    cout << "Room Type (Classroom, Seminar Hall, Lab, Meeting Room, Study Room): ";
    string type = readLine();

    cout << "Minimum Capacity (Enter number, or blank for all): ";
    string capStr = readLine();
    int minCap = -1;
    if (!capStr.empty()) {
        try {
            minCap = stoi(capStr);
        } catch (...) {
            cout << "Invalid capacity input. Ignoring filter.\n";
        }
    }

    cout << "Building Name: ";
    string building = readLine();

    cout << "Search availability on a specific date? (y/n): ";
    string checkTime = readLine();
    string date = "";
    string startTime = "";
    string endTime = "";

    if (checkTime == "y" || checkTime == "Y") {
        cout << "Enter Date (YYYY-MM-DD): ";
        date = readLine();
        if (!Utils::isValidDate(date)) {
            cout << "Invalid date format. Aborting search parameter.\n";
            return;
        }
        cout << "Enter Start Time (HH:MM): ";
        startTime = readLine();
        cout << "Enter End Time (HH:MM): ";
        endTime = readLine();

        int startMin = Utils::timeToMinutes(startTime);
        int endMin = Utils::timeToMinutes(endTime);
        if (startMin == -1 || endMin == -1 || startMin >= endMin) {
            cout << "Invalid time interval. Aborting search parameter.\n";
            return;
        }
    }

    vector<Room> results = Scheduler::searchRooms(type, minCap, building, date, startTime, endTime);
    printRoomsTable(results);
}

void bookRoomWorkflow() {
    cout << "\n--- Book a Room ---\n";
    cout << "Enter Room ID: ";
    string roomId = readLine();
    if (!Scheduler::roomExists(roomId)) {
        cout << "Room ID does not exist.\n";
        return;
    }

    cout << "Enter Date (YYYY-MM-DD, e.g. " << Utils::getCurrentDate() << "): ";
    string date = readLine();
    if (!Utils::isValidDate(date)) {
        cout << "Invalid date format.\n";
        return;
    }

    if (Utils::isDateBefore(date, Utils::getCurrentDate())) {
        cout << "Warning: You are booking a date in the past (" << date << "). Current system date is " << Utils::getCurrentDate() << ".\n";
        cout << "Do you want to proceed anyway? (y/n): ";
        string proceed = readLine();
        if (proceed != "y" && proceed != "Y") {
            cout << "Booking cancelled.\n";
            return;
        }
    }

    cout << "Enter Start Time (HH:MM, 24-hr format): ";
    string startTime = readLine();
    cout << "Enter End Time (HH:MM, 24-hr format): ";
    string endTime = readLine();

    int startMin = Utils::timeToMinutes(startTime);
    int endMin = Utils::timeToMinutes(endTime);
    if (startMin == -1 || endMin == -1 || startMin >= endMin) {
        cout << "Error: Invalid start or end time.\n";
        return;
    }

    bool joinedWaitlist = false;
    // Attempt normal booking
    string bookingId = Scheduler::bookRoom(roomId, currentUser.id, date, startTime, endTime, false, joinedWaitlist);
    
    if (!bookingId.empty()) {
        cout << "\nSuccess! Room " << roomId << " has been booked.\n";
        cout << "Your Booking ID is: " << bookingId << "\n";
    } else {
        cout << "\nThis slot conflicts with an existing booking.\n";
        cout << "Would you like to join the Waiting List for this slot? (y/n): ";
        string choice = readLine();
        if (choice == "y" || choice == "Y") {
            string waitId = Scheduler::bookRoom(roomId, currentUser.id, date, startTime, endTime, true, joinedWaitlist);
            if (joinedWaitlist && !waitId.empty()) {
                cout << "Success! You have been added to the waitlist.\n";
                cout << "Your Waitlist Ticket ID is: " << waitId << "\n";
            } else {
                cout << "Error occurred while adding to waitlist.\n";
            }
        } else {
            cout << "Booking request discarded.\n";
        }
    }
}

void cancelBookingWorkflow(bool isAdmin) {
    cout << "\n--- Cancel Booking ---\n";
    cout << "Enter Booking ID to cancel: ";
    string bookingId = readLine();

    vector<string> promotions;
    bool success = Scheduler::cancelBooking(bookingId, currentUser.id, isAdmin, promotions);

    if (success) {
        cout << "Booking " << bookingId << " has been successfully cancelled.\n";
        if (!promotions.empty()) {
            cout << "\n[Waitlist Auto-Promotion Event]\n";
            for (const auto& msg : promotions) {
                cout << " -> " << msg << "\n";
            }
        }
    } else {
        cout << "Error: Booking ID not found, already cancelled, or you do not have permission to cancel it.\n";
    }
}

void viewMyBookingsWorkflow() {
    cout << "\n--- My Bookings & History ---\n";
    vector<Booking> myBookings = Scheduler::getUserBookings(currentUser.id);

    if (myBookings.empty()) {
        cout << "You have no bookings on record.\n";
        return;
    }

    cout << left << setw(12) << "Booking ID"
              << setw(10) << "Room ID"
              << setw(12) << "Date"
              << setw(10) << "Start"
              << setw(10) << "End"
              << setw(12) << "Status" << "\n";
    cout << string(66, '-') << "\n";

    for (const auto& b : myBookings) {
        cout << left << setw(12) << b.bookingId
                  << setw(10) << b.roomId
                  << setw(12) << b.date
                  << setw(10) << b.startTime
                  << setw(10) << b.endTime
                  << setw(12) << b.status << "\n";
    }
    cout << string(66, '-') << "\n";
}

void addRoomWorkflow() {
    cout << "\n--- Add a New Room ---\n";
    cout << "Enter Room ID (unique code, e.g. R104): ";
    string id = readLine();
    if (id.empty() || id.find(',') != string::npos) {
        cout << "Invalid ID. No commas allowed.\n";
        return;
    }
    if (Scheduler::roomExists(id)) {
        cout << "Error: Room ID already exists.\n";
        return;
    }

    cout << "Enter Room Name (e.g. CS Seminar Room): ";
    string name = readLine();
    if (name.empty() || name.find(',') != string::npos) {
        cout << "Invalid name. No commas allowed.\n";
        return;
    }

    cout << "Enter Room Type (Classroom, Seminar Hall, Lab, Meeting Room, Study Room): ";
    string type = readLine();
    if (type.empty() || type.find(',') != string::npos) {
        cout << "Invalid type. No commas allowed.\n";
        return;
    }

    cout << "Enter Building: ";
    string building = readLine();
    if (building.empty() || building.find(',') != string::npos) {
        cout << "Invalid building. No commas allowed.\n";
        return;
    }

    cout << "Enter Capacity: ";
    int capacity = readChoice();
    if (capacity <= 0) {
        cout << "Capacity must be a positive integer.\n";
        return;
    }

    Room newRoom = { id, name, type, building, capacity };
    if (Scheduler::addRoom(newRoom)) {
        cout << "Room " << id << " added successfully!\n";
    } else {
        cout << "Database write error. Room could not be saved.\n";
    }
}

void updateRoomWorkflow() {
    cout << "\n--- Update Room Details ---\n";
    cout << "Enter Room ID to update: ";
    string id = readLine();
    if (!Scheduler::roomExists(id)) {
        cout << "Room ID does not exist.\n";
        return;
    }

    // Load rooms to show current values
    vector<Room> rooms;
    Storage::loadRooms(rooms);
    Room current;
    for (const auto& r : rooms) {
        if (r.id == id) {
            current = r;
            break;
        }
    }

    cout << "Updating Room " << id << " (Leave blank to keep current value):\n";
    
    cout << "Name [" << current.name << "]: ";
    string name = readLine();
    if (name.empty()) name = current.name;
    if (name.find(',') != string::npos) {
        cout << "Invalid characters. Update aborted.\n";
        return;
    }

    cout << "Type [" << current.type << "]: ";
    string type = readLine();
    if (type.empty()) type = current.type;
    if (type.find(',') != string::npos) {
        cout << "Invalid characters. Update aborted.\n";
        return;
    }

    cout << "Building [" << current.building << "]: ";
    string building = readLine();
    if (building.empty()) building = current.building;
    if (building.find(',') != string::npos) {
        cout << "Invalid characters. Update aborted.\n";
        return;
    }

    cout << "Capacity [" << current.capacity << "]: ";
    string capStr = readLine();
    int capacity = current.capacity;
    if (!capStr.empty()) {
        try {
            capacity = stoi(capStr);
            if (capacity <= 0) {
                cout << "Capacity must be positive. Update aborted.\n";
                return;
            }
        } catch (...) {
            cout << "Invalid capacity input. Update aborted.\n";
            return;
        }
    }

    Room updated = { id, name, type, building, capacity };
    if (Scheduler::updateRoom(id, updated)) {
        cout << "Room " << id << " updated successfully!\n";
    } else {
        cout << "Database write error. Room could not be updated.\n";
    }
}

void deleteRoomWorkflow() {
    cout << "\n--- Delete a Room ---\n";
    cout << "Enter Room ID to delete: ";
    string id = readLine();
    if (!Scheduler::roomExists(id)) {
        cout << "Room ID does not exist.\n";
        return;
    }

    cout << "Are you absolutely sure you want to delete room " << id << "?\n";
    cout << "This will cancel all confirmed bookings and clear the waitlist for this room. (y/n): ";
    string choice = readLine();

    if (choice == "y" || choice == "Y") {
        if (Scheduler::deleteRoom(id)) {
            cout << "Room " << id << " and its associated reservations have been deleted.\n";
        } else {
            cout << "Database error occurred. Deletion failed.\n";
        }
    } else {
        cout << "Deletion aborted.\n";
    }
}

void viewAllBookingsWorkflow() {
    cout << "\n--- All Bookings & History ---\n";
    vector<Booking> bookings = Scheduler::getAllBookings();

    if (bookings.empty()) {
        cout << "No bookings have been made yet.\n";
        return;
    }

    // Load users for ID to Username lookup
    vector<User> users;
    Storage::loadUsers(users);
    auto getUsername = [&](int uid) {
        for (const auto& u : users) {
            if (u.id == uid) return u.username;
        }
        return string("User ID: ") + to_string(uid);
    };

    cout << left << setw(12) << "Booking ID"
              << setw(10) << "Room ID"
              << setw(15) << "User"
              << setw(12) << "Date"
              << setw(10) << "Start"
              << setw(10) << "End"
              << setw(12) << "Status" << "\n";
    cout << string(81, '-') << "\n";

    for (const auto& b : bookings) {
        cout << left << setw(12) << b.bookingId
                  << setw(10) << b.roomId
                  << setw(15) << getUsername(b.userId)
                  << setw(12) << b.date
                  << setw(10) << b.startTime
                  << setw(10) << b.endTime
                  << setw(12) << b.status << "\n";
    }
    cout << string(81, '-') << "\n";
}

void viewWaitlistWorkflow() {
    cout << "\n--- Current Waiting List Tickets ---\n";
    vector<WaitlistEntry> waitlist = Scheduler::getWaitlist();

    if (waitlist.empty()) {
        cout << "The waiting list is currently empty.\n";
        return;
    }

    vector<User> users;
    Storage::loadUsers(users);
    auto getUsername = [&](int uid) {
        for (const auto& u : users) {
            if (u.id == uid) return u.username;
        }
        return string("User ID: ") + to_string(uid);
    };

    cout << left << setw(12) << "Ticket ID"
              << setw(10) << "Room ID"
              << setw(15) << "User"
              << setw(12) << "Date"
              << setw(10) << "Start"
              << setw(10) << "End"
              << setw(15) << "Queue Time" << "\n";
    cout << string(84, '-') << "\n";

    for (const auto& w : waitlist) {
        // Convert timestamp to printable local date/time
        time_t rawtime = static_cast<time_t>(w.timestamp);
        tm* timeinfo = localtime(&rawtime);
        char buffer[20];
        if (timeinfo) {
            strftime(buffer, sizeof(buffer), "%m-%d %H:%M:%S", timeinfo);
        } else {
            sprintf(buffer, "%lld", w.timestamp);
        }

        cout << left << setw(12) << w.waitlistId
                  << setw(10) << w.roomId
                  << setw(15) << getUsername(w.userId)
                  << setw(12) << w.date
                  << setw(10) << w.startTime
                  << setw(10) << w.endTime
                  << setw(15) << buffer << "\n";
    }
    cout << string(84, '-') << "\n";
}

void viewUtilizationWorkflow() {
    cout << "\n--- Room Utilization Dashboard ---\n";
    vector<Scheduler::UtilizationStats> stats = Scheduler::getUtilizationStats();

    if (stats.empty()) {
        cout << "No rooms on record to calculate statistics.\n";
        return;
    }

    cout << left << setw(10) << "Room ID"
              << setw(20) << "Room Name"
              << right << setw(18) << "Confirmed Bookings"
              << setw(16) << "Booked Hours" << "\n";
    cout << string(64, '-') << "\n";

    for (const auto& s : stats) {
        cout << left << setw(10) << s.roomId
                  << setw(20) << s.roomName
                  << right << setw(18) << s.totalBookings
                  << fixed << setprecision(1) << setw(16) << s.totalHours << "\n";
    }
    cout << string(64, '-') << "\n";
}
