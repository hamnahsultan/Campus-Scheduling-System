#pragma once
#include <string>
using namespace std;

struct Course {
    string id;
    string instructor;
    int    enrollment;
};

struct Room {
    string id;
    int    capacity;
};
