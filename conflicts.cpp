#include "conflicts.h"
#include <algorithm>
#include <cmath>

using namespace std;

string seatLocation(int index, const vector<Seat>& seats)
{
    if (index < 0 || index >= seats.size())
        return "Invalid seat index " + to_string(index);

    const Seat& seat = seats[index];

    return "Hall " + seat.hallId +
           ", Row " + to_string(seat.row) +
           ", Column " + to_string(seat.column);
}

vector<string> detectUnallocatedStudents(
    const vector<Student>& students,
    const vector<int>& allocation)
{
    vector<string> result;

    for (int i = 0; i < students.size(); i++) {
        if (i >= allocation.size() || allocation[i] == -1) {
            result.push_back(
                "Unallocated student: " + students[i].id +
                " - " + students[i].name
            );
        }
    }

    return result;
}

vector<string> detectHardViolations(
    const vector<Student>& students,
    const vector<Seat>& seats,
    const vector<int>& allocation,
    const vector<Hall>& halls)
{
    vector<string> result;
    int limit = min(students.size(), allocation.size());

    if (allocation.size() > students.size()) {
        result.push_back("Allocation has more entries than students.");
    }

    for (int i = 0; i < limit; i++) {
        int seatIndex = allocation[i];

        if (seatIndex == -1)
            continue;

        if (seatIndex < -1 || seatIndex >= seats.size()) {
            result.push_back(
                "Invalid seat index for student " + students[i].id
            );
            continue;
        }

        const Seat& seat = seats[seatIndex];

        if (seat.hallId.empty())
            result.push_back("Empty hall ID for student " + students[i].id);

        if (seat.row <= 0)
            result.push_back("Invalid row for student " + students[i].id);

        if (seat.column <= 0)
            result.push_back("Invalid column for student " + students[i].id);

        if (students[i].needsAccessible && !seat.accessible) {
            result.push_back(
                "Accessibility conflict: Student " + students[i].id +
                " is assigned to a non-accessible seat."
            );
        }

        if (!halls.empty()) {
            bool found = false;

            for (const Hall& hall : halls) {
                if (hall.id == seat.hallId) {
                    found = true;

                    if (seat.row > hall.rows || seat.column > hall.columns) {
                        result.push_back(
                            "Seat is outside the dimensions of Hall " + hall.id
                        );
                    }

                    break;
                }
            }

            if (!found) {
                result.push_back(
                    "Unknown hall " + seat.hallId +
                    " for student " + students[i].id
                );
            }
        }

        for (int j = 0; j < i; j++) {
            int otherIndex = allocation[j];

            if (otherIndex < 0 || otherIndex >= seats.size())
                continue;

            const Seat& other = seats[otherIndex];

            if (students[i].id == students[j].id) {
                result.push_back(
                    "Duplicate student ID assigned to multiple seats: " +
                    students[i].id
                );
                break;
            }

            if (seat.hallId == other.hallId &&
                seat.row == other.row &&
                seat.column == other.column) {
                result.push_back(
                    "Duplicate seat assignment at " + seatLocation(seatIndex, seats)
                );
                break;
            }

            if (!students[i].className.empty() &&
                students[i].className == students[j].className &&
                seat.hallId == other.hallId &&
                seat.row == other.row &&
                abs(seat.column - other.column) == 1) {
                result.push_back(
                    "Adjacent students from the same class: " +
                    students[i].name + " and " + students[j].name
                );
            }
        }
    }

    return result;
}

vector<string> detectHardViolations(
    const vector<Student>& students,
    const vector<Seat>& seats,
    const vector<int>& allocation)
{
    return detectHardViolations(
        students, seats, allocation, vector<Hall>{}
    );
}


vector<string> detectConflicts(
    const vector<Student>& students,
    const vector<Seat>& seats,
    const vector<int>& allocation,
    const vector<Hall>& halls)
{
    return detectHardViolations(students, seats, allocation, halls);
}


ConflictReport generateConflictReport(
    const vector<Student>& students,
    const vector<Seat>& seats,
    const vector<int>& allocation)
{
    ConflictReport report;

    report.hardViolations =
        detectHardViolations(students, seats, allocation);

    report.unallocatedStudents =
        detectUnallocatedStudents(students, allocation);

    return report;
}

ConflictReport generateConflictReport(
    const vector<Student>& students,
    const vector<Seat>& seats,
    const vector<int>& allocation,
    const vector<Hall>& halls)
{
    ConflictReport report;

    report.hardViolations =
        detectHardViolations(students, seats, allocation, halls);

    report.unallocatedStudents =
        detectUnallocatedStudents(students, allocation);

    return report;
}
