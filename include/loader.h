#pragma once
#include <string>
#include <vector>
#include <map>
#include <climits>
#include <utility>
#include <iostream>

#include <fstream>
#include "types.h"

vector<string> split(string line, char delimiter);
Course parseCourse(string line);
Room parseRoom(string line);
string parseSlot(string line);
vector<Course> loadCourses(string filename);
map<string, Room> loadRooms(string filename);
vector<string> loadSlots(string filename);
map<string, Course> buildCourseMap(vector<Course> &courses);
