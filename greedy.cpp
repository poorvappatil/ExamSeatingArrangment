#include "greedy.h"
#include <cstdlib>
#include <unordered_set>
using namespace std;

// These helper functions are only used in this file.
namespace {

// Sort students so accessibility requests come first.
void sortStudents(
    const vector<Student>& students,
    vector<int>& order
) {
    // Simple bubble sort.
    for (int i = 0; i < (int)order.size() - 1; i++) {
        for (int j = 0; j < (int)order.size() - i - 1; j++) {

            int first = order[j];
            int second = order[j + 1];

            if (!students[first].needsAccessible &&
                 students[second].needsAccessible) {
                int temp = order[j];
                order[j] = order[j + 1];
                order[j + 1] = temp;
            }
        }
    }
}

// Check if two seats are next to each other in the same row.
bool isAdjacent(const Seat& a, const Seat& b) {
    return a.hallId == b.hallId &&
           a.row == b.row &&
           abs(a.column - b.column) == 1;
}

// Try to find a suitable seat for one student.
bool assignStudent(
    const vector<Student>& students,
    const vector<Seat>& seats,
    vector<bool>& used,
    vector<int>& allocation,
    int studentIndex
) {
    int backupSeat = -1;

    for (int j = 0; j < (int)seats.size(); j++) {

        // Skip seats that are already occupied.
        if (used[j])
            continue;

        // Students who need accessible seats must get one.
        if (students[studentIndex].needsAccessible &&
            !seats[j].accessible)
            continue;

        bool sameClassBeside = false;

        // Check students who have already been seated.
        for (int k = 0; k < (int)students.size(); k++) {
            if (allocation[k] == -1)
                continue;

            if (students[studentIndex].className !=
                students[k].className)
                continue;

            int otherSeat = allocation[k];

            if (isAdjacent(seats[j], seats[otherSeat])) {
                sameClassBeside = true;
                break;
            }
        }

        // Prefer a seat without a same-class neighbour.
        if (!sameClassBeside) {
            allocation[studentIndex] = j;
            used[j] = true;
            return true;
        }

        // Keep a valid backup if every seat causes adjacency.
        if (backupSeat == -1)
            backupSeat = j;
    }

    // Use the backup only if no preferred seat was found.
    if (backupSeat != -1) {
        allocation[studentIndex] = backupSeat;
        used[backupSeat] = true;
        return true;
    }

    // No suitable seat was available.
    return false;
}

} // End of helper functions

vector<int> allocateGreedy(
    const vector<Student>& students,
    const vector<Seat>& seats
) {
    // -1 means that the student has not been seated.
    vector<int> allocation(students.size(), -1);
    vector<bool> used(seats.size(), false);

    // Mark seats that were already occupied.
    for (int j = 0; j < (int)seats.size(); j++)
        used[j] = seats[j].occupied;

    // Keep only the first occurrence of each student ID.
    unordered_set<string> seenIDs;
    vector<int> order;

    for (int i = 0; i < (int)students.size(); i++) {
        if (seenIDs.count(students[i].id) == 0) {
            seenIDs.insert(students[i].id);
            order.push_back(i);
        }
    }

    // Give students needing accessible seats priority.
    sortStudents(students, order);

    // Assign seats in the sorted order.
    for (int p = 0; p < (int)order.size(); p++) {
        int studentIndex = order[p];

        assignStudent(
            students, seats, used, allocation, studentIndex
        );
    }

    return allocation;
}
