#include "conflicts.h"

#include <cmath>
#include <map>
#include <sstream>
#include <algorithm>

// Helper to format seat location string
static std::string formatSeatLocation(int seatIdx, const std::vector<Seat>& seats)
{
    if (seatIdx >= 0 && seatIdx < static_cast<int>(seats.size()))
    {
        return "Seat [Hall " + seats[seatIdx].hallId +
               ", Row " + std::to_string(seats[seatIdx].row) +
               ", Col " + std::to_string(seats[seatIdx].column) + "]";
    }
    return "Seat index " + std::to_string(seatIdx);
}

// 1. Detect duplicate student assignments (same student ID allocated multiple seats)
static std::vector<std::string> checkDuplicateStudentAssignments(
    const std::vector<Student>& students,
    const std::vector<int>& allocation,
    const std::vector<Seat>& seats
)
{
    std::vector<std::string> violations;
    // Map student ID to all student roster indices that have an active seat assignment
    std::map<std::string, std::vector<int>> studentIdToAllocatedIndices;

    int totalStudents = static_cast<int>(students.size());
    int allocationSize = static_cast<int>(allocation.size());
    int limit = std::min(totalStudents, allocationSize);

    for (int i = 0; i < limit; i++)
    {
        // Only count actual seat assignments
        if (allocation[i] >= 0 && !students[i].id.empty())
        {
            studentIdToAllocatedIndices[students[i].id].push_back(i);
        }
    }

    for (const auto& entry : studentIdToAllocatedIndices)
    {
        const std::string& studentId = entry.first;
        const std::vector<int>& allocatedIndices = entry.second;

        // Report only when two or more allocated records share the same student ID
        if (allocatedIndices.size() > 1)
        {
            std::ostringstream oss;
            oss << "Duplicate student assignment: Student ID '" << studentId << "' is assigned to "
                << allocatedIndices.size() << " seats: ";

            for (size_t k = 0; k < allocatedIndices.size(); k++)
            {
                if (k > 0)
                {
                    oss << ", ";
                }
                int idx = allocatedIndices[k];
                oss << "Record " << idx << " ('" << students[idx].name
                    << "', Class: " << students[idx].className << ") at "
                    << formatSeatLocation(allocation[idx], seats);
            }

            violations.push_back(oss.str());
        }
    }

    return violations;
}


// 2. Detect duplicate seat assignments (multiple students assigned to the same seat)
static std::vector<std::string> checkDuplicateSeatAssignments(
    const std::vector<Student>& students,
    const std::vector<Seat>& seats,
    const std::vector<int>& allocation
)
{
    std::vector<std::string> violations;
    std::map<std::string, std::vector<int>> seatKeyToStudents;

    int totalStudents = static_cast<int>(students.size());
    int allocationSize = static_cast<int>(allocation.size());
    int limit = std::min(totalStudents, allocationSize);

    for (int i = 0; i < limit; i++)
    {
        int seatIdx = allocation[i];
        // Only consider valid seat indices for physical seat collisions
        if (seatIdx >= 0 && seatIdx < static_cast<int>(seats.size()))
        {
            std::string key = seats[seatIdx].hallId + "#" +
                              std::to_string(seats[seatIdx].row) + "#" +
                              std::to_string(seats[seatIdx].column);
            seatKeyToStudents[key].push_back(i);
        }
    }

    for (const auto& entry : seatKeyToStudents)
    {
        const std::vector<int>& studentIndices = entry.second;
        if (studentIndices.size() > 1)
        {
            const Seat& s = seats[allocation[studentIndices[0]]];
            std::ostringstream oss;
            oss << "Duplicate seat assignment: Seat [Hall " << s.hallId
                << ", Row " << s.row
                << ", Column " << s.column
                << "] is assigned to multiple students: ";

            for (size_t k = 0; k < studentIndices.size(); k++)
            {
                if (k > 0)
                {
                    oss << ", ";
                }
                int sIdx = studentIndices[k];
                oss << "Student " << students[sIdx].id
                    << " (" << students[sIdx].name << ")";
            }

            violations.push_back(oss.str());
        }
    }

    return violations;
}

// 3. Hall, row, and column validation
static std::vector<std::string> checkHallRowColumn(
    const std::vector<Student>& students,
    const std::vector<Seat>& seats,
    const std::vector<int>& allocation,
    const std::vector<Hall>& halls
)
{
    std::vector<std::string> violations;

    int totalStudents = static_cast<int>(students.size());
    int allocationSize = static_cast<int>(allocation.size());

    if (allocationSize > totalStudents)
    {
        violations.push_back(
            "Allocation size mismatch: Allocation vector contains " +
            std::to_string(allocationSize) + " entries, exceeding student count (" +
            std::to_string(totalStudents) + ")"
        );
    }

    int limit = std::min(totalStudents, allocationSize);

    for (int i = 0; i < limit; i++)
    {
        int seatIdx = allocation[i];
        if (seatIdx == -1)
        {
            continue; // Valid unallocated student
        }

        if (seatIdx < -1)
        {
            violations.push_back(
                "Invalid seat index: Student " + students[i].id + " (" +
                students[i].name + ") has invalid negative seat index " +
                std::to_string(seatIdx) + " (must be -1 for unallocated, or >= 0 for a valid seat)"
            );
            continue;
        }

        if (seatIdx >= static_cast<int>(seats.size()))
        {
            violations.push_back(
                "Invalid seat index: Student " + students[i].id + " (" +
                students[i].name + ") is assigned to out-of-bounds seat index " +
                std::to_string(seatIdx) + " (valid range: 0 to " +
                std::to_string(static_cast<int>(seats.size()) - 1) + ")"
            );
            continue;
        }

        const Seat& seat = seats[seatIdx];

        if (seat.hallId.empty())
        {
            violations.push_back(
                "Invalid hall: Seat index " + std::to_string(seatIdx) +
                " assigned to Student " + students[i].id + " has an empty hall ID"
            );
        }

        if (seat.row <= 0)
        {
            violations.push_back(
                "Invalid row: Student " + students[i].id +
                " is assigned to non-positive row " + std::to_string(seat.row) +
                " in Hall " + seat.hallId
            );
        }

        if (seat.column <= 0)
        {
            violations.push_back(
                "Invalid column: Student " + students[i].id +
                " is assigned to non-positive column " + std::to_string(seat.column) +
                " in Hall " + seat.hallId
            );
        }

        if (!halls.empty() && !seat.hallId.empty())
        {
            bool hallFound = false;
            for (const auto& hall : halls)
            {
                if (hall.id == seat.hallId)
                {
                    hallFound = true;

                    if (seat.row > hall.rows)
                    {
                        violations.push_back(
                            "Invalid row: Student " + students[i].id +
                            " is assigned to Row " + std::to_string(seat.row) +
                            " in Hall " + seat.hallId +
                            ", which exceeds maximum rows (" +
                            std::to_string(hall.rows) + ")"
                        );
                    }

                    if (seat.column > hall.columns)
                    {
                        violations.push_back(
                            "Invalid column: Student " + students[i].id +
                            " is assigned to Column " + std::to_string(seat.column) +
                            " in Hall " + seat.hallId +
                            ", which exceeds maximum columns (" +
                            std::to_string(hall.columns) + ")"
                        );
                    }
                    break;
                }
            }

            if (!hallFound)
            {
                violations.push_back(
                    "Invalid hall: Student " + students[i].id +
                    " is assigned to unknown Hall ID '" + seat.hallId + "'"
                );
            }
        }
    }

    return violations;
}

// 4. Accessibility constraint validation
static std::vector<std::string> checkAccessibility(
    const std::vector<Student>& students,
    const std::vector<Seat>& seats,
    const std::vector<int>& allocation
)
{
    std::vector<std::string> violations;

    int totalStudents = static_cast<int>(students.size());
    int allocationSize = static_cast<int>(allocation.size());
    int limit = std::min(totalStudents, allocationSize);

    for (int i = 0; i < limit; i++)
    {
        int seatIdx = allocation[i];
        if (seatIdx >= 0 && seatIdx < static_cast<int>(seats.size()))
        {
            if (students[i].needsAccessible && !seats[seatIdx].accessible)
            {
                std::ostringstream oss;
                oss << "Accessibility constraint violation: Student "
                    << students[i].id << " (" << students[i].name
                    << ") requires accessible seating, but is assigned to non-accessible Seat [Hall "
                    << seats[seatIdx].hallId << ", Row " << seats[seatIdx].row
                    << ", Column " << seats[seatIdx].column << "]";
                violations.push_back(oss.str());
            }
        }
    }

    return violations;
}

// 5. Horizontal adjacency conflict detection for students from the same class
static std::vector<std::string> checkHorizontalAdjacency(
    const std::vector<Student>& students,
    const std::vector<Seat>& seats,
    const std::vector<int>& allocation
)
{
    std::vector<std::string> violations;

    int totalStudents = static_cast<int>(students.size());
    int allocationSize = static_cast<int>(allocation.size());
    int limit = std::min(totalStudents, allocationSize);

    for (int i = 0; i < limit; i++)
    {
        int seatA = allocation[i];
        if (seatA < 0 || seatA >= static_cast<int>(seats.size()))
        {
            continue;
        }

        for (int j = i + 1; j < limit; j++)
        {
            int seatB = allocation[j];
            if (seatB < 0 || seatB >= static_cast<int>(seats.size()))
            {
                continue;
            }

            const Seat& s1 = seats[seatA];
            const Seat& s2 = seats[seatB];

            // Same hall, same row, and adjacent column
            if (s1.hallId == s2.hallId &&
                s1.row == s2.row &&
                std::abs(s1.column - s2.column) == 1)
            {
                if (!students[i].className.empty() &&
                    students[i].className == students[j].className)
                {
                    std::ostringstream oss;
                    oss << "Horizontal adjacency conflict: Student "
                        << students[i].id << " (" << students[i].name
                        << ") and Student " << students[j].id << " (" << students[j].name
                        << ") from class '" << students[i].className
                        << "' are seated adjacent in Hall " << s1.hallId
                        << ", Row " << s1.row
                        << " (Columns " << s1.column << " and " << s2.column << ")";
                    violations.push_back(oss.str());
                }
            }
        }
    }

    return violations;
}

// 6. Separate reporting for unallocated students and hard seating violations
std::vector<std::string> detectUnallocatedStudents(
    const std::vector<Student>& students,
    const std::vector<int>& allocation
)
{
    std::vector<std::string> unallocated;

    int totalStudents = static_cast<int>(students.size());
    int allocationSize = static_cast<int>(allocation.size());

    for (int i = 0; i < totalStudents; i++)
    {
        if (i >= allocationSize || allocation[i] == -1)
        {
            std::ostringstream oss;
            oss << "Unallocated student: " << students[i].id
                << " - " << students[i].name
                << " (Class: " << students[i].className
                << (students[i].needsAccessible ? ", Accessible: Required" : ", Accessible: Not required")
                << ")";
            unallocated.push_back(oss.str());
        }
    }

    return unallocated;
}

std::vector<std::string> detectHardViolations(
    const std::vector<Student>& students,
    const std::vector<Seat>& seats,
    const std::vector<int>& allocation,
    const std::vector<Hall>& halls
)
{
    std::vector<std::string> violations;

    // 1. Duplicate student assignments
    auto dupStudents = checkDuplicateStudentAssignments(students, allocation, seats);
    violations.insert(violations.end(), dupStudents.begin(), dupStudents.end());

    // 2. Duplicate seat assignments
    auto dupSeats = checkDuplicateSeatAssignments(students, seats, allocation);
    violations.insert(violations.end(), dupSeats.begin(), dupSeats.end());

    // 3. Hall, row, and column validation
    auto dimErrors = checkHallRowColumn(students, seats, allocation, halls);
    violations.insert(violations.end(), dimErrors.begin(), dimErrors.end());

    // 4. Accessibility constraint validation
    auto accessErrors = checkAccessibility(students, seats, allocation);
    violations.insert(violations.end(), accessErrors.begin(), accessErrors.end());

    // 5. Horizontal adjacency conflict detection
    auto adjErrors = checkHorizontalAdjacency(students, seats, allocation);
    violations.insert(violations.end(), adjErrors.begin(), adjErrors.end());

    return violations;
}

ConflictReport generateConflictReport(
    const std::vector<Student>& students,
    const std::vector<Seat>& seats,
    const std::vector<int>& allocation,
    const std::vector<Hall>& halls
)
{
    ConflictReport report;
    report.hardViolations = detectHardViolations(students, seats, allocation, halls);
    report.unallocatedStudents = detectUnallocatedStudents(students, allocation);
    return report;
}

// Primary interface called by main.cpp
std::vector<std::string> detectConflicts(
    const std::vector<Student>& students,
    const std::vector<Seat>& seats,
    const std::vector<int>& allocation
)
{
    return detectHardViolations(students, seats, allocation);
}

// Overload allowing hall dimension validation when halls are provided
std::vector<std::string> detectConflicts(
    const std::vector<Student>& students,
    const std::vector<Seat>& seats,
    const std::vector<int>& allocation,
    const std::vector<Hall>& halls
)
{
    return detectHardViolations(students, seats, allocation, halls);
}
