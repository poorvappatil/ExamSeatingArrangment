#include "greedy.h"

using namespace std;

// These helpers are only needed inside this file.
namespace
{
// Treat seats that are already occupied as unavailable.
void markOccupiedSeats(const vector<Seat>& seats, vector<bool>& used)
{
    for (size_t j = 0; j < seats.size(); ++j)
        used[j] = seats[j].occupied;
}

// Give one student the first seat that fits their needs.
bool assignStudent(
    const Student& student,
    const vector<Seat>& seats,
    vector<bool>& used,
    vector<int>& allocation,
    size_t studentIndex
)
{
    for (size_t seatIndex = 0; seatIndex < seats.size(); ++seatIndex)
    {
        // Skip seats that have already been taken.
        if (used[seatIndex])
            continue;

        // Students who need accessible seating can only use accessible seats.
        if (student.needsAccessible && !seats[seatIndex].accessible)
            continue;

        // Save the seat number and mark it taken for the next student.
        allocation[studentIndex] = static_cast<int>(seatIndex);
        used[seatIndex] = true;
        return true;
    }

    // No suitable seat was available, so the student stays unassigned.
    return false;
}
}

// Return the seat index for each student, or -1 if they could not be seated.
vector<int> allocateGreedy(
    const vector<Student>& students,
    const vector<Seat>& seats
)
{
    // Every student starts unassigned; no seats are used yet.
    vector<int> allocation(students.size(), -1);
    vector<bool> used(seats.size(), false);

    // Keep seats that were already occupied out of the allocation.
    markOccupiedSeats(seats, used);

    // Give accessible-seat requests first choice.
    for (size_t i = 0; i < students.size(); ++i)
        if (students[i].needsAccessible)
            assignStudent(students[i], seats, used, allocation, i);

    // Then place everyone else in the first open seat.
    for (size_t i = 0; i < students.size(); ++i)
        if (!students[i].needsAccessible)
            assignStudent(students[i], seats, used, allocation, i);

    return allocation;
}
