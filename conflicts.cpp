
#include "conflicts.h"
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <cstdlib>

using namespace std;

vector<string> detectConflicts(
    const vector<Student>& students,
    const vector<Seat>& seats,
    const vector<int>& allocation
) {
    vector<string> messages;
    set<int> usedSeats;

    int n = (int)students.size();

    if ((int)allocation.size() != n) {
        messages.push_back(
            "Hard-rule violation: allocation size does not match student count."
        );
        return messages;
    }

    for (int i = 0; i < n; i++) {
        int s = allocation[i];

        // Unallocated students are reported separately.
        if (s == -1) {
            messages.push_back(
                "Unallocated: " + students[i].id +
                " - " + students[i].name
            );
            continue;
        }

        // Check whether the seat index is valid.
        if (s < 0 || s >= (int)seats.size()) {
            messages.push_back(
                "Hard-rule violation: invalid seat index for " +
                students[i].id
            );
            continue;
        }

        // Check whether two students share the same seat.
        if (!usedSeats.insert(s).second) {
            messages.push_back(
                "Hard-rule violation: duplicate seat assignment for " +
                students[i].id
            );
        }

        // Check accessibility requirements.
        if (students[i].needsAccessible &&
            !seats[s].accessible) {
            messages.push_back(
                "Hard-rule violation: accessibility requirement not met for " +
                students[i].id
            );
        }

        // Check whether another student has the same ID.
        for (int j = 0; j < i; j++) {
            if (students[i].id == students[j].id) {
                messages.push_back(
                    "Hard-rule violation: duplicate student ID " +
                    students[i].id
                );
                break;
            }
        }
    }

    // Check same-class horizontal adjacency.
    for (int i = 0; i < n; i++) {
        if (allocation[i] < 0 ||
            allocation[i] >= (int)seats.size())
            continue;

        for (int j = i + 1; j < n; j++) {
            if (allocation[j] < 0 ||
                allocation[j] >= (int)seats.size())
                continue;

            const Seat& a = seats[allocation[i]];
            const Seat& b = seats[allocation[j]];

            if (students[i].className == students[j].className &&
                a.hallId == b.hallId &&
                a.row == b.row &&
                abs(a.column - b.column) == 1) {

                messages.push_back(
                    "Soft conflict: same-class students " +
                    students[i].id + " and " +
                    students[j].id + " are adjacent."
                );
            }
        }
    }

    if (messages.empty()) {
        messages.push_back(
            "No conflicts detected."
        );
    }

    return messages;
}

