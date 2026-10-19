#ifndef GREEDY_H
#define GREEDY_H

#include "models.h"
#include <vector>

using namespace std;

vector<int> allocateGreedy(
    const vector<Student>& students,
    const vector<Seat>& seats
);

#endif
