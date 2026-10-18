#ifndef CONFLICTS_H
#define CONFLICTS_H

#include "models.h"
#include <vector>
#include <string>

// Structured report separating hard seating violations from unallocated students.
struct ConflictReport {
    std::vector<std::string> hardViolations;
    std::vector<std::string> unallocatedStudents;

    bool hasHardViolations() const {
        return !hardViolations.empty();
    }

    bool hasUnallocated() const {
        return !unallocatedStudents.empty();
    }

    bool hasAnyIssues() const {
        return hasHardViolations() || hasUnallocated();
    }
};

// Generates a structured report separating hard seating violations and unallocated students.
ConflictReport generateConflictReport(
    const std::vector<Student>& students,
    const std::vector<Seat>& seats,
    const std::vector<int>& allocation,
    const std::vector<Hall>& halls = {}
);

// Reports only unallocated students (allocation[i] == -1 or missing).
std::vector<std::string> detectUnallocatedStudents(
    const std::vector<Student>& students,
    const std::vector<int>& allocation
);

// Reports only hard seating constraint violations.
std::vector<std::string> detectHardViolations(
    const std::vector<Student>& students,
    const std::vector<Seat>& seats,
    const std::vector<int>& allocation,
    const std::vector<Hall>& halls = {}
);

// Primary interface called by main.cpp (returns hard seating violations).
// NOTE: Since main.cpp passes (students, seats, allocation), this 3-argument version
// validates seat index bounds, non-empty hall IDs, and positive row/column numbers.
// However, full boundary validation against configured Hall dimensions (rows, cols)
// is only possible when Hall definitions are provided; use the 4-argument overload below.
std::vector<std::string> detectConflicts(
    const std::vector<Student>& students,
    const std::vector<Seat>& seats,
    const std::vector<int>& allocation
);

// Overload providing full hall boundary validation when Hall definitions are supplied.
std::vector<std::string> detectConflicts(
    const std::vector<Student>& students,
    const std::vector<Seat>& seats,
    const std::vector<int>& allocation,
    const std::vector<Hall>& halls
);

#endif
