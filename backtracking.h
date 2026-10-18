#ifndef BACKTRACKING_H
#define BACKTRACKING_H

#include "models.h"
#include <vector> 

using namespace std;

vector<int> allocateBacktracking(
    const vector<Student>& students,
    const vector<Seat>& seats
);

#endif
