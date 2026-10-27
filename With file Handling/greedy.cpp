#include "greedy.h"
#include <cstdlib>
#include <unordered_set>
using namespace std;

namespace {

// put students who need accessible seats first
void sortStudents(vector<Student>& students, vector<int>& order) {
    for (int i = 0; i < order.size() - 1; i++) {
        for (int j = 0; j < order.size() - i - 1; j++) {
            int first = order[j];
            int second = order[j + 1];
            if (!students[first].needsAccessible && students[second].needsAccessible) {
                int temp = order[j];
                order[j] = order[j + 1];
                order[j + 1] = temp;
            }
        }
    }
}

// check if two seats are next to each other in the same row
bool isAdjacent(Seat& a, Seat& b) {
    return a.hallId == b.hallId && a.row == b.row && abs(a.column - b.column) == 1;
}

// find a seat for one student
bool assignStudent(vector<Student>& students, vector<Seat>& seats, vector<bool>& used, vector<int>& allocation, int studentIndex) {
    int backupSeat = -1;

    for (int j = 0; j < seats.size(); j++) {
        if (used[j]) continue;

        // students who need accessible seating must get an accessible seat
        if (students[studentIndex].needsAccessible && !seats[j].accessible) continue;

        bool sameClassBeside = false;

        // check students who already have seats
        for (int k = 0; k < students.size(); k++) {
            if (allocation[k] == -1) continue;

            // skip empty classes and different classes
            if (students[studentIndex].className.empty() || students[studentIndex].className != students[k].className) continue;

            int otherSeat = allocation[k];
            if (otherSeat < 0 || otherSeat >= seats.size()) continue;

            if (isAdjacent(seats[j], seats[otherSeat])) {
                sameClassBeside = true;
                break;
            }
        }

        // choose a seat without a same-class neighbour if possible
        if (!sameClassBeside) {
            allocation[studentIndex] = j;
            used[j] = true;
            return true;
        }

        // save a backup if every available seat has a neighbour
        if (backupSeat == -1) backupSeat = j;
    }

    // use the backup if no preferred seat was found
    if (backupSeat != -1) {
        allocation[studentIndex] = backupSeat;
        used[backupSeat] = true;
        return true;
    }
    return false;
}
} 

vector<int> allocateGreedy(vector<Student>& students, vector<Seat>& seats) {
    vector<int> allocation(students.size(), -1);
    vector<bool> used(seats.size(), false);

    // mark seats that are already occupied
    for (int j = 0; j < seats.size(); j++) used[j] = seats[j].occupied;

    // keep only the first occurrence of each student id
    unordered_set<string> seenIDs;
    vector<int> order;
    for (int i = 0; i < students.size(); i++) {
        if (seenIDs.count(students[i].id) == 0) {
            seenIDs.insert(students[i].id);
            order.push_back(i);
        }
    }

    // give accessible-seat requests priority
    sortStudents(students, order);

    // assign seats in the chosen order
    for (int p = 0; p < order.size(); p++) {
        assignStudent(students, seats, used, allocation, order[p]);
    }
    return allocation;
}
