#pragma once
#include <string>
#include <vector>
#include <map>
#include <climits>
#include <utility>
#include <iostream>
#include <algorithm>
#include "types.h"
#include "schedule_state.h"

class Scheduler
{
public:
    Scheduler(ScheduleState &state,
              const map<string, Course> &courses,
              const map<string, Room> &rooms,
              const vector<string> &slots);

    // Candidate generation
    vector<string> getAvailableRooms(const Course &course) const;
    vector<string> getAvailableSlots(const Course &course) const;
    vector<pair<string, string>> getFeasibleAssignments(const Course &course, const vector<string> &availableRooms, const vector<string> &availableSlots) const;

    // Assignment strategies
    bool assignFirstAvailable(const Course &course, const vector<pair<string, string>> &candidates);

    // Scheduling flow
    bool scheduleCourse(const Course &course);
    void buildInitialSchedule();
    void unscheduleCourse(const string &courseId);

    // Advanced moves
    bool swapCourses(const string &courseIdA, const string &courseIdB);

private:
    ScheduleState &state;
    const map<string, Course> &courses;
    const map<string, Room> &rooms;
    const vector<string> &slots;
};
