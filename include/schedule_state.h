#pragma once
#include <string>
#include <vector>
#include <map>
#include <climits>
#include <utility>
#include <iostream>
#include <algorithm>
#include "types.h"

class ScheduleState
{
public:
    ScheduleState();

    // Slot
    void addCourseToSlot(string course, string slot);
    void removeCourseFromSlot(string course, string slot);
    void moveCourseBetweenSlots(string course, string fromSlot, string toSlot);
    void clearSlot(string slot);
    vector<string> getCoursesInSlot(string slot) const;

    // Instructor
    void assignInstructorToSlot(string instructor, string slot);
    void removeInstructorFromSlot(string instructor, string slot);
    vector<string> getInstructorSlots(string instructor) const;
    int getInstructorLoad(string instructor) const;

    // Room
    void assignRoomToSlot(string room, string slot);
    void removeRoomFromSlot(string room, string slot);
    bool isRoomFreeInSlot(string room, string slot) const;
    vector<string> getRoomSlots(string room) const;
    int getRoomLoad(string room) const;

    // Course assignment
    void assignCourse(string course, string room, string slot);
    void unassignCourse(string course);
    pair<string, string> getCourseAssignment(string course) const;
    bool isCourseAssigned(string course) const;

    // Full-state (always use these)
    bool assign(string course, string instructor, string room, string slot);
    void remove(string course, string instructor, string room, string slot);
    void move(string course, string instructor, string oldRoom, string oldSlot, string newRoom, string newSlot);
    bool isConsistent(const map<string, Course> &courses) const;

    // Expose keys for iteration (needed by Analyzer / ScoreEngine)
    vector<string> getAllInstructors() const;
    vector<string> getAllAssignedCourses() const;

private:
    map<string, vector<string>> slotToCourses;
    map<string, vector<string>> instructorToSlots;
    map<string, vector<string>> roomToSlots;
    map<string, pair<string, string>> courseAssignment;
};
