#include "backtracking.h"
#include <cmath>

using namespace std;

void solve(
    int index,
    const vector<Student>& students,
    const vector<Seat>& seats,
    vector<int>& allocation,
    vector<int>& best,
    vector<bool>& used,
    int count,
    int& bestCount)
{
    if (count > bestCount) {
        bestCount = count;
        best = allocation;
    }

    if (index == (int)students.size() || bestCount == (int)students.size())
        return;

    for (int i = 0; i < (int)seats.size(); i++) {
        if (used[i])
            continue;

        if (students[index].needsAccessible && !seats[i].accessible)
            continue;

        bool conflict = false;

        for (int j = 0; j < index; j++) {
            if (allocation[j] == -1)
                continue;

            const Seat& other = seats[allocation[j]];

            if (!students[index].className.empty() &&
                students[index].className == students[j].className &&
                seats[i].hallId == other.hallId &&
                seats[i].row == other.row &&
                abs(seats[i].column - other.column) == 1) {
                conflict = true;
                break;
            }
        }

        if (conflict)
            continue;

        allocation[index] = i;
        used[i] = true;

        solve(index + 1, students, seats, allocation,
              best, used, count + 1, bestCount);

        used[i] = false;
        allocation[index] = -1;

        if (bestCount == (int)students.size())
            return;
    }

    // Try leaving this student unallocated.
    allocation[index] = -1;

    solve(index + 1, students, seats, allocation,
          best, used, count, bestCount);
}

vector<int> allocateBacktracking(
    const vector<Student>& students,
    const vector<Seat>& seats)
{
    vector<int> allocation(students.size(), -1);
    vector<int> best(students.size(), -1);
    vector<bool> used(seats.size(), false);
    int bestCount = 0;

    solve(0, students, seats, allocation,
          best, used, 0, bestCount);

    return best;
}
