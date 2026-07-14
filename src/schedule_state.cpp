#include "../include/schedule_state.h"

ScheduleState::ScheduleState() {}

// ---- helpers ----
static void vecRemove(vector<string> &v, const string &val)
{
    for (size_t i = 0; i < v.size();)
    {
        if (v[i] == val)
        {
            v.erase(v.begin() + i);
        }
        else
        {
            i++;
        }
    }
}

// ---- Slot ----
void ScheduleState::addCourseToSlot(string course, string slot)
{
    slotToCourses[slot].push_back(course);
}

void ScheduleState::removeCourseFromSlot(string course, string slot)
{
    if (slotToCourses.find(slot) == slotToCourses.end())
    {
        return;
    }
    vecRemove(slotToCourses[slot], course);
}

void ScheduleState::moveCourseBetweenSlots(string course, string fromSlot, string toSlot)
{
    removeCourseFromSlot(course, fromSlot);
    addCourseToSlot(course, toSlot);
}

void ScheduleState::clearSlot(string slot)
{
    slotToCourses[slot].clear();
}

vector<string> ScheduleState::getCoursesInSlot(string slot) const
{
    auto it = slotToCourses.find(slot);
    if (it == slotToCourses.end())
    {
        return {};
    }
    return it->second;
}
// ---- Instructor ----
void ScheduleState::assignInstructorToSlot(string instructor, string slot)
{
    instructorToSlots[instructor].push_back(slot);
}

void ScheduleState::removeInstructorFromSlot(string instructor, string slot)
{
    if (instructorToSlots.find(instructor) == instructorToSlots.end())
    {
        return;
    }
    vecRemove(instructorToSlots[instructor], slot);
}

vector<string> ScheduleState::getInstructorSlots(string instructor) const
{
    auto it = instructorToSlots.find(instructor);
    if (it == instructorToSlots.end())
    {
        return {};
    }
    return it->second;
}

int ScheduleState::getInstructorLoad(string instructor) const
{
    auto it = instructorToSlots.find(instructor);
    if (it == instructorToSlots.end())
    {
        return 0;
    }
    return it->second.size();
}

// ---- Room ----
void ScheduleState::assignRoomToSlot(string room, string slot)
{
    roomToSlots[room].push_back(slot);
}

void ScheduleState::removeRoomFromSlot(string room, string slot)
{
    if (roomToSlots.find(room) == roomToSlots.end())
    {
        return;
    }
    vecRemove(roomToSlots[room], slot);
}

bool ScheduleState::isRoomFreeInSlot(string room, string slot) const
{
    auto it = roomToSlots.find(room);
    if (it == roomToSlots.end())
    {
        return true;
    }

    for (size_t i = 0; i < it->second.size(); i++)
    {
        if (it->second[i] == slot)
        {
            return false;
        }
    }

    return true;
}

vector<string> ScheduleState::getRoomSlots(string room) const
{
    auto it = roomToSlots.find(room);
    if (it == roomToSlots.end())
    {
        return {};
    }
    return it->second;
}

int ScheduleState::getRoomLoad(string room) const
{
    auto it = roomToSlots.find(room);
    if (it == roomToSlots.end())
    {
        return 0;
    }
    return it->second.size();
}

// ---- Course assignment ----
void ScheduleState::assignCourse(string course, string room, string slot)
{
    courseAssignment[course] = pair<string, string>(room, slot);
}

void ScheduleState::unassignCourse(string course)
{
    auto it = courseAssignment.find(course);

    if (it == courseAssignment.end())
    {
        return;
    }
    courseAssignment.erase(it);
}

pair<string, string> ScheduleState::getCourseAssignment(string course) const
{
    auto it = courseAssignment.find(course);

    if (it == courseAssignment.end())
    {
        return pair<string, string>(string(""), string(""));
    }

    return it->second;
}

bool ScheduleState::isCourseAssigned(string course) const
{
    if (courseAssignment.find(course) != courseAssignment.end())
    {
        return true;
    }
    return false;
}

// ---- Full-state ----
bool ScheduleState::assign(string course, string instructor, string room, string slot)
{
    if (course == "" || instructor == "" || room == "" || slot == "")
    {
        return false;
    }

    addCourseToSlot(course, slot);
    assignInstructorToSlot(instructor, slot);
    assignRoomToSlot(room, slot);
    assignCourse(course, room, slot);

    return true;
}

void ScheduleState::remove(string course, string instructor, string room, string slot)
{
    removeCourseFromSlot(course, slot);
    removeInstructorFromSlot(instructor, slot);
    removeRoomFromSlot(room, slot);
    unassignCourse(course);
}

void ScheduleState::move(string course, string instructor,
                         string oldRoom, string oldSlot,
                         string newRoom, string newSlot)
{
    remove(course, instructor, oldRoom, oldSlot);
    assign(course, instructor, newRoom, newSlot);
}

bool ScheduleState::isConsistent(const map<string, Course> &courses) const
{
    for (auto it = courseAssignment.begin(); it != courseAssignment.end(); it++)
    {
        string course = it->first;
        string room = it->second.first;
        string slot = it->second.second;

        auto c = courses.find(course);
        if (c == courses.end())
        {
            return false;
        }

        string instructor = c->second.instructor;

        auto s = slotToCourses.find(slot);
        if (s == slotToCourses.end())
        {
            return false;
        }

        bool found = false;
        for (size_t i = 0; i < s->second.size(); i++)
        {
            if (s->second[i] == course)
            {
                found = true;
                break;
            }
        }
        if (found == 0)
        {
            return false;
        }

        auto r = roomToSlots.find(room);
        if (r == roomToSlots.end())
        {
            return false;
        }

        bool check = false;
        for (size_t i = 0; i < r->second.size(); i++)
        {
            if (r->second[i] == slot)
            {
                check = true;
                break;
            }
        }
        if (check == 0)
        {
            return false;
        }

        auto i = instructorToSlots.find(instructor);
        if (i == instructorToSlots.end())
        {
            return false;
        }

        bool yesno = false;
        for (size_t j = 0; j < i->second.size(); j++)
        {
            if (i->second[j] == slot)
            {
                yesno = true;
                break;
            }
        }
        if (yesno == 0)
        {
            return false;
        }
    }

    return true;
}

vector<string> ScheduleState::getAllInstructors() const
{
    vector<string> result;

    for (auto it = instructorToSlots.begin(); it != instructorToSlots.end(); it++)
    {
        result.push_back(it->first);
    }

    return result;
}

vector<string> ScheduleState::getAllAssignedCourses() const
{
    vector<string> result;

    for (auto it = courseAssignment.begin(); it != courseAssignment.end(); it++)
    {
        result.push_back(it->first);
    }

    return result;
}