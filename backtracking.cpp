#include "backtracking.h"

#include <functional>
#include <vector>

using namespace std;

namespace {

bool createsSameClassAdjacency(
    const vector<Student>& students,
    const vector<Seat>& seats,
    const vector<int>& allocation,
    int studentIndex,
    int seatIndex
)
{
    const Seat& candidateSeat = seats[seatIndex];

    for (int i = 0; i < (int)students.size(); i++)
    {
        if (allocation[i] == -1)
            continue;

        if (students[i].className != students[studentIndex].className)
            continue;

        const Seat& assignedSeat = seats[allocation[i]];

        if (assignedSeat.hallId != candidateSeat.hallId)
            continue;

        if (assignedSeat.row != candidateSeat.row)
            continue;

        if (abs(assignedSeat.column - candidateSeat.column) != 1)
            continue;

        return true;
    }

    return false;
}

bool isSeatStillAvailable(
    const vector<bool>& used,
    const vector<Seat>& seats,
    int seatIndex
)
{
    return !used[seatIndex] && !seats[seatIndex].occupied;
}

int countFreeSeats(const vector<bool>& used, const vector<Seat>& seats)
{
    int freeSeats = 0;

    for (int i = 0; i < (int)seats.size(); i++)
    {
        if (!used[i] && !seats[i].occupied)
            freeSeats++;
    }

    return freeSeats;
}

}

vector<int> allocateBacktracking(
    const vector<Student>& students,
    const vector<Seat>& seats
)
{
    vector<int> allocation(students.size(), -1);
    vector<bool> used(seats.size(), false);

    for (int i = 0; i < (int)seats.size(); i++)
    {
        if (seats[i].occupied)
            used[i] = true;
    }

    vector<int> order;
    order.reserve(students.size());

    for (int i = 0; i < (int)students.size(); i++)
    {
        if (students[i].needsAccessible)
            order.push_back(i);
    }

    for (int i = 0; i < (int)students.size(); i++)
    {
        if (!students[i].needsAccessible)
            order.push_back(i);
    }

    function<bool(int)> solve = [&](int index) -> bool
    {
        if (index == (int)order.size())
            return true;

        int studentIndex = order[index];
        vector<int> preferredSeats;
        vector<int> fallbackSeats;

        int remainingStudents = (int)order.size() - index;
        int freeSeats = countFreeSeats(used, seats);

        if (freeSeats < remainingStudents)
            return false;

        for (int seatIndex = 0; seatIndex < (int)seats.size(); seatIndex++)
        {
            if (!isSeatStillAvailable(used, seats, seatIndex))
                continue;

            if (students[studentIndex].needsAccessible && !seats[seatIndex].accessible)
                continue;

            bool conflictByAdjacency = createsSameClassAdjacency(
                students,
                seats,
                allocation,
                studentIndex,
                seatIndex
            );

            if (!conflictByAdjacency)
                preferredSeats.push_back(seatIndex);
            else
                fallbackSeats.push_back(seatIndex);
        }

        vector<int> candidates = preferredSeats;
        candidates.insert(candidates.end(), fallbackSeats.begin(), fallbackSeats.end());

        for (int seatIndex : candidates)
        {
            used[seatIndex] = true;
            allocation[studentIndex] = seatIndex;

            if (solve(index + 1))
                return true;

            allocation[studentIndex] = -1;
            used[seatIndex] = false;
        }

        return false;
    };

    solve(0);
    return allocation;
}
