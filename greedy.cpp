
#include "greedy.h"
#include <algorithm>
#include <cstdlib>
#include <vector>

using namespace std;

bool hasGreedyConflict(
    int studentIndex,
    int seatIndex,
    const vector<Student>& students,
    const vector<Seat>& seats,
    const vector<int>& allocation
) {
    for (int i = 0; i < (int)students.size(); i++) {
        if (allocation[i] == -1)
            continue;

        const Seat& a = seats[seatIndex];
        const Seat& b = seats[allocation[i]];

        if (students[i].className == students[studentIndex].className &&
            a.hallId == b.hallId &&
            a.row == b.row &&
            abs(a.column - b.column) == 1) {
            return true;
        }
    }

    return false;
}

vector<int> allocateGreedy(
    const vector<Student>& students,
    const vector<Seat>& seats
) {
    int n = (int)students.size();
    int m = (int)seats.size();

    vector<int> allocation(n, -1);
    vector<bool> used(m, false);

    // Prioritize students who need accessible seating.
    vector<int> order(n);
    for (int i = 0; i < n; i++)
        order[i] = i;

    stable_sort(order.begin(), order.end(),
        [&](int a, int b) {
            return students[a].needsAccessible >
                   students[b].needsAccessible;
        });

    for (int student : order) {
        // First pass: avoid same-class adjacency.
        // Second pass: allow adjacency if necessary.
        for (int pass = 0; pass < 2; pass++) {
            for (int s = 0; s < m; s++) {
                if (used[s])
                    continue;

                if (students[student].needsAccessible &&
                    !seats[s].accessible)
                    continue;

                bool conflict = hasGreedyConflict(
                    student, s, students, seats, allocation
                );

                if (pass == 0 && conflict)
                    continue;

                if (pass == 1 && !conflict)
                    continue;

                allocation[student] = s;
                used[s] = true;
                break;
            }

            if (allocation[student] != -1)
                break;
        }
    }

    return allocation;
}

