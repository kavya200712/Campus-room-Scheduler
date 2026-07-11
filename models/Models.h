#ifndef MODELS_H
#define MODELS_H

#include <string>

using namespace std;

namespace Models {
    struct User {
        int id;
        string username;
        string password;
        string role; // "admin" or "student"
    };

    struct Room {
        string id;
        string name;
        string type; // "Classroom", "Seminar Hall", "Lab", "Meeting Room", "Study Room"
        string building;
        int capacity;
    };

    struct Booking {
        string bookingId;
        string roomId;
        int userId;
        string date; // YYYY-MM-DD
        string startTime; // HH:MM
        string endTime; // HH:MM
        string status; // "Confirmed" or "Cancelled"
    };

    struct WaitlistEntry {
        string waitlistId;
        string roomId;
        int userId;
        string date; // YYYY-MM-DD
        string startTime; // HH:MM
        string endTime; // HH:MM
        long long timestamp; // raw epoch/order value
    };
}

#endif // MODELS_H
