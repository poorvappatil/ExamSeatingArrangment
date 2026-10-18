#include "backtracking.h"
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <vector>

using namespace std;

bool hasConflict(
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

        if (students[i].className ==
                students[studentIndex].className &&
            a.hallId == b.hallId &&
            a.row == b.row &&
            abs(a.column - b.column) == 1) {
            return true;
        }
    }

    return false;
}

vector<int> allocateBacktracking(
    const vector<Student>& students,
    const vector<Seat>& seats
) {
    int n = (int)students.size();
    int m = (int)seats.size();

    vector<int> order(n);
    for (int i = 0; i < n; i++)
        order[i] = i;

    // Prioritize students needing accessible seating.
    stable_sort(order.begin(), order.end(),
        [&](int a, int b) {
            return students[a].needsAccessible >
                   students[b].needsAccessible;
        });

    vector<int> current(n, -1);
    vector<int> best(n, -1);
    vector<bool> used(m, false);

    int bestCount = 0;
    int bestConflicts = 1000000000;

    function<void(int, int)> solve =
        [&](int position, int count) {

        // Count same-class horizontal adjacency conflicts.
        int conflicts = 0;

        for (int i = 0; i < n; i++) {
            if (current[i] == -1)
                continue;

            for (int j = i + 1; j < n; j++) {
                if (current[j] == -1)
                    continue;

                const Seat& a = seats[current[i]];
                const Seat& b = seats[current[j]];

                if (students[i].className ==
                        students[j].className &&
                    a.hallId == b.hallId &&
                    a.row == b.row &&
                    abs(a.column - b.column) == 1) {
                    conflicts++;
                }
            }
        }

        // Keep the best arrangement found.
        if (count > bestCount ||
            (count == bestCount &&
             conflicts < bestConflicts)) {
            bestCount = count;
            bestConflicts = conflicts;
            best = current;
        }

        if (position == n)
            return;

        int student = order[position];

        // Pass 0: Try seats without adjacency conflicts.
        // Pass 1: Try seats with adjacency conflicts.
        for (int pass = 0; pass < 2; pass++) {
            for (int s = 0; s < m; s++) {

                if (used[s])
                    continue;

                if (students[student].needsAccessible &&
                    !seats[s].accessible)
                    continue;

                bool conflict = hasConflict(
                    student, s, students, seats, current
                );

                if (pass == 0 && conflict)
                    continue;

                if (pass == 1 && !conflict)
                    continue;

                // CHOOSE
                current[student] = s;
                used[s] = true;

                // EXPLORE
                solve(position + 1, count + 1);

                // UNDO
                used[s] = false;
                current[student] = -1;
            }
        }

        solve(position + 1, count);
    };

    solve(0, 0);

    return best;
}

