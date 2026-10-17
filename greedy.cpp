
#include "greedy.h"
#include <cstdlib>
#include <unordered_set>
#include <vector>

using namespace std;

vector<int> allocateGreedy(
    const vector<Student>& students,
    const vector<Seat>& seats
) {
    // -1 means that a student has not been allocated a seat.
    vector<int> allocation(students.size(), -1);

    // Remember which seats are occupied or already assigned.
    vector<bool> used(seats.size(), false);

    for (size_t i = 0; i < seats.size(); ++i) {
        used[i] = seats[i].occupied;
    }

    // Track students already processed to avoid duplicate IDs.
    unordered_set<string> seenIDs;

    // Allocate students needing accessible seating first.
    for (int priority = 0; priority < 2; ++priority) {
        for (size_t i = 0; i < students.size(); ++i) {
            const Student& student = students[i];

            // First pass: accessible students.
            // Second pass: all other students.
            if ((priority == 0) != student.needsAccessible) {
                continue;
            }

            // Do not allocate the same student ID twice.
            if (seenIDs.count(student.id)) {
                continue;
            }
            seenIDs.insert(student.id);

            int bestSeat = -1;
            int fallbackSeat = -1;

            // Search for a suitable seat.
            for (size_t j = 0; j < seats.size(); ++j) {
                const Seat& seat = seats[j];

                if (used[j]) {
                    continue;
                }

                // Accessible students must use accessible seats.
                if (student.needsAccessible && !seat.accessible) {
                    continue;
                }

                bool adjacentClassmate = false;

                // Check students who have already been seated.
                for (size_t k = 0; k < students.size(); ++k) {
                    int assignedSeat = allocation[k];

                    if (assignedSeat == -1) {
                        continue;
                    }

                    const Student& otherStudent = students[k];
                    const Seat& otherSeat = seats[assignedSeat];

                    if (student.className == otherStudent.className &&
                        seat.hallId == otherSeat.hallId &&
                        seat.row == otherSeat.row &&
                        abs(seat.column - otherSeat.column) == 1) {
                        adjacentClassmate = true;
                        break;
                    }
                }

                // Prefer a seat without a same-class neighbour.
                if (!adjacentClassmate) {
                    bestSeat = static_cast<int>(j);
                    break;
                }

                // Keep a valid seat as a backup.
                if (fallbackSeat == -1) {
                    fallbackSeat = static_cast<int>(j);
                }
            }

            // Prefer the best seat; otherwise use the backup.
            int chosenSeat = (bestSeat != -1)
                             ? bestSeat
                             : fallbackSeat;

            if (chosenSeat != -1) {
                allocation[i] = chosenSeat;
                used[chosenSeat] = true;
            }
        }
    }

    return allocation;
}

