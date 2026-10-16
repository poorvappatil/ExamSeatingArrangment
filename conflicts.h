
#ifndef CONFLICTS_H
#define CONFLICTS_H

#include "models.h"
#include <vector>
#include <string>

vector<string> detectConflicts(
    const vector<Student>& students,
    const vector<Seat>& seats,
    const vector<int>& allocation
);

#endif

