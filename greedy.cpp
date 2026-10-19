#include "greedy.h"
#include "conflicts.h"

using namespace std;

vector<int> allocateGreedy(
    const vector<Student>& students,
    const vector<Seat>& seats)
{
    vector<int> allocation(students.size(), -1);
    vector<bool> used(seats.size(), false);

    for (int i = 0; i < (int)students.size(); i++) {
        for (int j = 0; j < (int)seats.size(); j++) {
            if (used[j])
                continue;

            if (students[i].needsAccessible && !seats[j].accessible)
                continue;

            bool conflict = false;

            for (int k = 0; k < i; k++) {
                if (allocation[k] == -1)
                    continue;

                const Seat& other = seats[allocation[k]];

                if (students[i].className == students[k].className &&
                    !students[i].className.empty() &&
                    seats[j].hallId == other.hallId &&
                    seats[j].row == other.row &&
                    abs(seats[j].column - other.column) == 1) {
                    conflict = true;
                    break;
                }
            }

            if (!conflict) {
                allocation[i] = j;
                used[j] = true;
                break;
            }
        }
    }

    return allocation;
}
