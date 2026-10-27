#ifndef CONFLICTS_H
#define CONFLICTS_H
#include "models.h"
#include <string>
#include <vector>
using namespace std;

struct ConflictReport {
    vector<string> hardViolations;
    vector<string> unallocatedStudents;

    bool hasHardViolations() { return !hardViolations.empty(); }

    bool hasUnallocated() { return !unallocatedStudents.empty(); }

    bool hasAnyIssues()  { return hasHardViolations() || hasUnallocated(); }
    
    int totalIssues()  { return static_cast<int>(hardViolations.size() + unallocatedStudents.size()); }
};

vector<string> detectConflicts(vector<Student>& students, vector<Seat>& seats, vector<int>& allocation, vector<Hall>& halls);

vector<string> detectHardViolations(vector<Student>& students, vector<Seat>& seats, vector<int>& allocation);

vector<string> detectHardViolations(vector<Student>& students,vector<Seat>& seats, vector<int>& allocation,vector<Hall>& halls);

vector<string> detectUnallocatedStudents(vector<Student>& students, vector<int>& allocation);

ConflictReport generateConflictReport( vector<Student>& students,  vector<Seat>& seats,vector<int>& allocation);

ConflictReport generateConflictReport(vector<Student>& students, vector<Seat>& seats, vector<int>& allocation, vector<Hall>& halls);
#endif
