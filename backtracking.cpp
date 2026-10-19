#include "backtracking.h"
#include <cmath>

using namespace std;

bool canUseSeat(
    int studentIndex,
    int seatIndex,
    const vector<Student>& students,
    const vector<Seat>& seats,
    const vector<int>& allocation)
{
    const Student& student = students[studentIndex];
    const Seat& seat = seats[seatIndex];

    if (student.needsAccessible && !seat.accessible)
        return false;

    for (int i = 0; i < studentIndex; i++) {
        if (allocation[i] == -1)
            continue;

        const Seat& other = seats[allocation[i]];

        if (seat.hallId == other.hallId &&
            seat.row == other.row &&
            abs(seat.column - other.column) == 1 &&
            !student.className.empty() &&
            student.className == students[i].className) {
            return false;
        }
    }

    return true;
}

bool solve(
    int studentIndex,
    const vector<Student>& students,
    const vector<Seat>& seats,
    vector<int>& allocation,
    vector<bool>& used)
{
    if (studentIndex == (int)students.size())
        return true;

    for (int i = 0; i < (int)seats.size(); i++) {
        if (used[i])
            continue;

        if (!canUseSeat(studentIndex, i, students, seats, allocation))
            continue;

        allocation[studentIndex] = i;
        used[i] = true;

        if (solve(studentIndex + 1, students, seats, allocation, used))
            return true;

        allocation[studentIndex] = -1;
        used[i] = false;
    }

    allocation[studentIndex] = -1;

    return solve(studentIndex + 1, students, seats, allocation, used);
}

vector<int> allocateBacktracking(
    const vector<Student>& students,
    const vector<Seat>& seats)
{
    vector<int> allocation(students.size(), -1);
    vector<bool> used(seats.size(), false);

    solve(0, students, seats, allocation, used);

    return allocation;
}
