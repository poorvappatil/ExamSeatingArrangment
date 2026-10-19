
#ifndef CONFLICTS_H
#define CONFLICTS_H

#include "models.h"
#include <string>
#include <vector>

using namespace std;

struct ConflictReport {
    vector<string> hardViolations;
    vector<string> unallocatedStudents;

    bool hasHardViolations() const {
        return !hardViolations.empty();
    }

    bool hasUnallocated() const {
        return !unallocatedStudents.empty();
    }

    bool hasAnyIssues() const {
        return hasHardViolations() || hasUnallocated();
    }

    int totalIssues() const {
        return hardViolations.size() + unallocatedStudents.size();
    }
};

vector<string> detectConflicts(const vector<Student>& students,
                               const vector<Seat>& seats,
                               const vector<int>& allocation);

vector<string> detectConflicts(const vector<Student>& students,
                               const vector<Seat>& seats,
                               const vector<int>& allocation,
                               const vector<Hall>& halls);

vector<string> detectHardViolations(const vector<Student>& students,
                                    const vector<Seat>& seats,
                                    const vector<int>& allocation);

vector<string> detectHardViolations(const vector<Student>& students,
                                    const vector<Seat>& seats,
                                    const vector<int>& allocation,
                                    const vector<Hall>& halls);

vector<string> detectUnallocatedStudents(const vector<Student>& students,
                                         const vector<int>& allocation);

ConflictReport generateConflictReport(const vector<Student>& students,
                                      const vector<Seat>& seats,
                                      const vector<int>& allocation);

ConflictReport generateConflictReport(const vector<Student>& students,
                                      const vector<Seat>& seats,
                                      const vector<int>& allocation,
                                      const vector<Hall>& halls);

#endif
