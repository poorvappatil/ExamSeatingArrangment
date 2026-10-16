
#ifndef MODELS_H
#define MODELS_H

#include <string>
using namespace std;

struct Student {
    string id;
    string name;
    string className;
    bool needsAccessible;
};

struct Hall {
    string id;
    string name;
    int rows;
    int columns;
    int accessibleColumn;
};

struct Seat {
    string hallId;
    int row;
    int column;
    bool accessible;
    bool occupied;
};

#endif

