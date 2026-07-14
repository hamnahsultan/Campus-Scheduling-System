#include "../include/scheduler.h"

Scheduler::Scheduler(ScheduleState &state,
                     const map<string, Course> &courses,
                     const map<string, Room> &rooms,
                     const vector<string> &slots)
    : state(state), courses(courses), rooms(rooms), slots(slots) {}

// ---- Candidate Generation ----

vector<string> Scheduler::getAvailableRooms(const Course &course) const
{
    vector<string> availableRooms;

    for (auto it = rooms.begin(); it != rooms.end(); it++)
    {
        if (it->second.capacity >= course.enrollment)
        {
            availableRooms.push_back(it->first);
        }
    }
    return availableRooms;
}

vector<string> Scheduler::getAvailableSlots(const Course &course) const
{
    vector<string> availableSlots;
    vector<string> busySlots = state.getInstructorSlots(course.instructor);

    for (size_t i = 0; i < slots.size(); i++)
    {
        bool busy = false;

        for (size_t j = 0; j < busySlots.size(); j++)
        {
            if (slots[i] == busySlots[j])
            {
                busy = true;
                break;
            }
        }
        if (busy == false)
        {
            availableSlots.push_back(slots[i]);
        }
    }
    return availableSlots;
}

vector<pair<string, string>> Scheduler::getFeasibleAssignments(
    const Course &course,
    const vector<string> &availableRooms,
    const vector<string> &availableSlots) const
{
    vector<pair<string, string>> feasible;

    for (size_t i = 0; i < availableRooms.size(); i++)
    {
        for (size_t j = 0; j < availableSlots.size(); j++)
        {
            if (state.isRoomFreeInSlot(availableRooms[i], availableSlots[j]))
            {
                feasible.push_back(make_pair(availableRooms[i], availableSlots[j]));
            }
        }
    }
    return feasible;
}

// ---- Assignment Strategies ----

bool Scheduler::assignFirstAvailable(const Course &course,
                                     const vector<pair<string, string>> &candidates)
{
    if (candidates.empty())
    {
        return false;
    }

    string room = candidates[0].first;
    string slot = candidates[0].second;

    if (state.assign(course.id, course.instructor, room, slot) == 0)
    {
        return false;
    }
    return true;
}

// ---- Scheduling Flow ----

bool Scheduler::scheduleCourse(const Course &course)
{
    vector<string> availableRooms = getAvailableRooms(course);
    vector<string> availableSlots = getAvailableSlots(course);

    vector<pair<string, string>> candidates = getFeasibleAssignments(course, availableRooms, availableSlots);

    if (assignFirstAvailable(course, candidates) == 0)
    {
        return false;
    }
    return true;
}

void Scheduler::buildInitialSchedule()
{
    for (auto it = courses.begin(); it != courses.end(); it++)
    {
        if (state.isCourseAssigned(it->first) == false)
        {
            scheduleCourse(it->second);
        }
    }
}

void Scheduler::unscheduleCourse(const string &courseId)
{
    if (state.isCourseAssigned(courseId) == false)
    {
        return;
    }

    auto i = courses.find(courseId);
    if (i == courses.end())
    {
        return;
    }

    pair<string, string> placement = state.getCourseAssignment(courseId);
    string room = placement.first;
    string slot = placement.second;
    string instructor = i->second.instructor;
    state.remove(courseId, instructor, room, slot);
}

// ---- Advanced Moves ----

bool Scheduler::swapCourses(const string &idA, const string &idB)
{
    if (state.isCourseAssigned(idA) == false || state.isCourseAssigned(idB) == false)
    {
        return false;
    }

    auto x = courses.find(idA);
    auto y = courses.find(idB);

    if (x == courses.end() || y == courses.end())
    {
        return false;
    }

    Course courseA = x->second;
    Course courseB = y->second;

    pair<string, string> a = state.getCourseAssignment(idA);
    pair<string, string> b = state.getCourseAssignment(idB);

    string roomA = a.first;
    string slotA = a.second;
    string roomB = b.first;
    string slotB = b.second;

    state.remove(idA, courseA.instructor, roomA, slotA);
    state.remove(idB, courseB.instructor, roomB, slotB);

    bool checkA = false;
    bool checkB = false;

    if (rooms.find(roomB) != rooms.end())
    {
        if (rooms.at(roomB).capacity >= courseA.enrollment &&
            state.isRoomFreeInSlot(roomB, slotB))
        {
            vector<string> iforA = state.getInstructorSlots(courseA.instructor);
            bool Abusy = false;

            for (size_t i = 0; i < iforA.size(); i++)
            {
                if (iforA[i] == slotB)
                {
                    Abusy = true;
                    break;
                }
            }

            if (Abusy == false)
            {
                checkA = true;
            }
        }
    }

    if (rooms.find(roomA) != rooms.end())
    {
        if (rooms.at(roomA).capacity >= courseB.enrollment && state.isRoomFreeInSlot(roomA, slotA))
        {
            vector<string> iforB = state.getInstructorSlots(courseB.instructor);
            bool BusyBee = false;

            for (size_t i = 0; i < iforB.size(); i++)
            {
                if (iforB[i] == slotA)
                {
                    BusyBee = true;
                    break;
                }
            }

            if (BusyBee == false)
            {
                checkB = true;
            }
        }
    }

    if (checkA && checkB)
    {
        state.assign(idA, courseA.instructor, roomB, slotB);
        state.assign(idB, courseB.instructor, roomA, slotA);
        return true;
    }

    state.assign(idA, courseA.instructor, roomA, slotA);
    state.assign(idB, courseB.instructor, roomB, slotB);
    return false;
}