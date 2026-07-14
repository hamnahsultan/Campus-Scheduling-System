#include "../include/score_engine.h"

static string getBuildingPrefix(const string &rid)
{
  int position = -1;
  for (size_t i = 0; i < rid.length(); i++)
  {
    if (rid[i] == '-')
    {
      position = i;
      break;
    }
  }
  if (position == -1)
  {
    return rid;
  }
  string pre = "";
  for (int i = 0; i < position; i++)
  {
    pre += rid[i];
  }
  return pre;
}

static int slotindex(const vector<string> &slots, const string &slot)
{
  for (size_t i = 0; i < slots.size(); i++)
  {
    if (slots[i] == slot)
    {
      return i;
    }
  }
  return -1;
}

static bool ConsecutiveSlotCheck(const vector<string> &slots, const string &a, const string &b)
{
  int x = slotindex(slots, a);
  int y = slotindex(slots, b);

  if (x == -1 || y == -1)
  {
    return false;
  }

  if (x + 1 != y && y + 1 != x)
  {
    return false;
  }

  if (a.size() < 3 || b.size() < 3)
  {
    return false;
  }
  if (a.substr(0, 3) == b.substr(0, 3))
  {
    return true;
  }
  return false;
}

ScoreEngine::ScoreEngine(ScheduleState &state,
                         Scheduler &scheduler,
                         const map<string, Course> &courses,
                         const map<string, Room> &rooms,
                         const vector<string> &slots)
    : state(state), scheduler(scheduler),
      courses(courses), rooms(rooms), slots(slots) {}

// ---- Scoring ----

double ScoreEngine::computeUtilizationScore() const
{
  double wasted = 0.0;
  vector<string> assigned = state.getAllAssignedCourses();

  for (size_t i = 0; i < assigned.size(); i++)
  {
    string cid = assigned[i];

    auto it = courses.find(cid);
    if (it == courses.end())
    {
      continue;
    }

    pair<string, string> placement = state.getCourseAssignment(cid);
    string rid = placement.first;

    auto r = rooms.find(rid);
    if (r == rooms.end())
    {
      continue;
    }

    int difference = r->second.capacity - it->second.enrollment;
    if (difference > 0)
    {
      wasted += difference;
    }
  }
  return wasted;
}

double ScoreEngine::computeBalanceScore() const
{
  if (slots.empty())
  {
    return 0.0;
  }

  int minload = -1;
  int maxload = -1;

  for (size_t i = 0; i < slots.size(); i++)
  {
    int load = state.getCoursesInSlot(slots[i]).size();

    if (minload == -1 || load < minload)
    {
      minload = load;
    }

    if (maxload == -1 || load > maxload)
    {
      maxload = load;
    }
  }
  return (maxload - minload);
}

double ScoreEngine::computeTravelScore() const
{
  double total = 0.0;
  vector<string> instructors = state.getAllInstructors();

  for (size_t i = 0; i < instructors.size(); i++)
  {
    string instructor = instructors[i];
    vector<string> islots = state.getInstructorSlots(instructor);

    for (size_t a = 0; a < islots.size(); a++)
    {
      for (size_t b = a + 1; b < islots.size(); b++)
      {
        if (!ConsecutiveSlotCheck(slots, islots[a], islots[b]))
        {
          continue;
        }

        vector<string> first = state.getCoursesInSlot(islots[a]);
        vector<string> second = state.getCoursesInSlot(islots[b]);

        string firsttemp = "";
        string secondtemp = "";

        for (size_t x = 0; x < first.size(); x++)
        {
          auto c = courses.find(first[x]);
          if (c != courses.end() && c->second.instructor == instructor)
          {
            firsttemp = first[x];
            break;
          }
        }

        for (size_t x = 0; x < second.size(); x++)
        {
          auto c = courses.find(second[x]);
          if (c != courses.end() && c->second.instructor == instructor)
          {
            secondtemp = second[x];
            break;
          }
        }

        if (firsttemp == "" || secondtemp == "")
        {
          continue;
        }

        pair<string, string> assign1 = state.getCourseAssignment(firsttemp);
        pair<string, string> assign2 = state.getCourseAssignment(secondtemp);

        string building1 = getBuildingPrefix(assign1.first);
        string building2 = getBuildingPrefix(assign2.first);

        if (building1 != building2)
        {
          total += 1.0;
        }
      }
    }
  }
  return total;
}

double ScoreEngine::computeFinalScore() const
{
  double util = computeUtilizationScore();
  double balance = computeBalanceScore();
  double travel = computeTravelScore();

  return util + balance + travel;
}

// ---- Problem Identification ----

string ScoreEngine::identifyWorstSlot() const
{
  if (slots.empty())
  {
    return "";
  }

  string worst = slots[0];
  int maxload = state.getCoursesInSlot(slots[0]).size();

  for (size_t i = 1; i < slots.size(); i++)
  {
    int load = state.getCoursesInSlot(slots[i]).size();
    if (load > maxload)
    {
      maxload = load;
      worst = slots[i];
    }
  }
  return worst;
}

string ScoreEngine::identifyUnderutilizedRoom() const
{
  string worstroom = "";
  int maxwaste = -1;

  for (auto it = rooms.begin(); it != rooms.end(); it++)
  {
    string rid = it->first;
    int waste = 0;

    vector<string> rslots = state.getRoomSlots(rid);
    for (size_t i = 0; i < rslots.size(); i++)
    {
      vector<string> courseshere = state.getCoursesInSlot(rslots[i]);

      for (size_t j = 0; j < courseshere.size(); j++)
      {
        pair<string, string> placement = state.getCourseAssignment(courseshere[j]);
        if (placement.first == rid)
        {
          auto c = courses.find(courseshere[j]);
          if (c != courses.end())
          {
            int difference = it->second.capacity - c->second.enrollment;
            if (difference > 0)
            {
              waste += difference;
            }
          }
        }
      }
    }

    if (waste > maxwaste)
    {
      maxwaste = waste;
      worstroom = rid;
    }
  }
  return worstroom;
}

// ---- One-Pass Fixes ----

int ScoreEngine::fixBalance()
{
  if (slots.empty())
  {
    return 0;
  }

  int totalcourses = 0;
  for (size_t i = 0; i < slots.size(); i++)
  {
    totalcourses += state.getCoursesInSlot(slots[i]).size();
  }

  double average = (double)totalcourses / (double)slots.size();
  int count = 0;

  for (size_t i = 0; i < slots.size(); i++)
  {
    string current = slots[i];
    vector<string> coursehere = state.getCoursesInSlot(current);

    while ((double)coursehere.size() > average)
    {
      bool moved = false;

      for (size_t c = 0; c < coursehere.size(); c++)
      {
        string cid = coursehere[c];
        auto co = courses.find(cid);

        if (co == courses.end())
        {
          continue;
        }

        Course course = co->second;
        pair<string, string> old = state.getCourseAssignment(cid);
        string oldr = old.first;
        string olds = old.second;

        for (size_t s = 0; s < slots.size(); s++)
        {
          string target = slots[s];

          if (target == olds)
          {
            continue;
          }

          if ((double)state.getCoursesInSlot(target).size() >= average)
          {
            continue;
          }

          vector<string> ibusy = state.getInstructorSlots(course.instructor);
          bool busy = false;

          for (size_t b = 0; b < ibusy.size(); b++)
          {
            if (ibusy[b] == target)
            {
              busy = true;
              break;
            }
          }

          if (busy != 0)
          {
            continue;
          }

          if (state.isRoomFreeInSlot(oldr, target))
          {
            state.move(cid, course.instructor, oldr, olds, oldr, target);
            count++;
            moved = true;
            break;
          }

          vector<string> possible = scheduler.getAvailableRooms(course);
          for (size_t r = 0; r < possible.size(); r++)
          {
            if (state.isRoomFreeInSlot(possible[r], target))
            {
              state.move(cid, course.instructor, oldr, olds, possible[r], target);
              count++;
              moved = true;
              break;
            }
          }

          if (moved != 0)
          {
            break;
          }
        }

        if (moved != 0)
        {
          break;
        }
      }

      if (moved == 0)
      {
        break;
      }

      coursehere = state.getCoursesInSlot(current);
    }
  }
  return count;
}

int ScoreEngine::fixRoomUsage()
{
  int changes = 0;
  vector<string> assigned = state.getAllAssignedCourses();

  for (size_t i = 0; i < assigned.size(); i++)
  {
    string cid = assigned[i];

    auto c = courses.find(cid);
    if (c == courses.end())
    {
      continue;
    }

    Course course = c->second;
    pair<string, string> placement = state.getCourseAssignment(cid);
    string currentRoom = placement.first;
    string current = placement.second;

    auto r = rooms.find(currentRoom);
    if (r == rooms.end())
    {
      continue;
    }

    int waste = r->second.capacity - course.enrollment;
    if (waste <= 50)
    {
      continue;
    }

    string bestr = currentRoom;
    int bestw = waste;

    for (auto it = rooms.begin(); it != rooms.end(); it++)
    {
      string TOPROOM = it->first;
      int capacity = it->second.capacity;

      if (capacity < course.enrollment)
      {
        continue;
      }

      if (TOPROOM != currentRoom && !state.isRoomFreeInSlot(TOPROOM, current))
      {
        continue;
      }

      int ww = capacity - course.enrollment;
      if (ww >= 0 && ww < bestw)
      {
        bestw = ww;
        bestr = TOPROOM;
      }
    }

    if (bestr != currentRoom)
    {
      state.move(cid, course.instructor, currentRoom, current, bestr, current);
      changes++;
    }
  }
  return changes;
}

int ScoreEngine::fixTravel()
{
  int fixed = 0;
  vector<string> instructors = state.getAllInstructors();

  for (size_t i = 0; i < instructors.size(); i++)
  {
    string instructor = instructors[i];
    vector<string> islots = state.getInstructorSlots(instructor);

    for (size_t a = 0; a < islots.size(); a++)
    {
      for (size_t b = 0; b < islots.size(); b++)
      {
        if (a == b)
        {
          continue;
        }

        string slot1 = islots[a];
        string slot2 = islots[b];

        int i1 = slotindex(slots, slot1);
        int i2 = slotindex(slots, slot2);

        if (i1 == -1 || i2 == -1 || i2 != i1 + 1)
        {
          continue;
        }

        if (slot1.substr(0, 3) != slot2.substr(0, 3))
        {
          continue;
        }

        vector<string> first = state.getCoursesInSlot(slot1);
        vector<string> second = state.getCoursesInSlot(slot2);

        string firsttemp = "";
        string secondtemp = "";

        for (size_t x = 0; x < first.size(); x++)
        {
          auto c = courses.find(first[x]);
          if (c != courses.end() && c->second.instructor == instructor)
          {
            firsttemp = first[x];
            break;
          }
        }

        for (size_t x = 0; x < second.size(); x++)
        {
          auto c = courses.find(second[x]);
          if (c != courses.end() && c->second.instructor == instructor)
          {
            secondtemp = second[x];
            break;
          }
        }

        if (firsttemp == "" || secondtemp == "")
        {
          continue;
        }

        pair<string, string> assign1 = state.getCourseAssignment(firsttemp);
        pair<string, string> assign2 = state.getCourseAssignment(secondtemp);

        string room1 = assign1.first;
        string room2 = assign2.first;

        if (getBuildingPrefix(room1) == getBuildingPrefix(room2))
        {
          continue;
        }

        auto it = courses.find(secondtemp);
        if (it == courses.end())
        {
          continue;
        }

        Course secondc = it->second;
        string target = getBuildingPrefix(room1);
        string bestr = room2;

        for (auto r = rooms.begin(); r != rooms.end(); r++)
        {
          if (getBuildingPrefix(r->first) != target)
          {
            continue;
          }

          if (r->second.capacity < secondc.enrollment)
          {
            continue;
          }

          if (r->first != room2 && !state.isRoomFreeInSlot(r->first, slot2))
          {
            continue;
          }

          bestr = r->first;
          break;
        }

        if (bestr != room2)
        {
          state.move(secondtemp, secondc.instructor, room2, slot2, bestr, slot2);
          fixed++;
        }
      }
    }
  }
  return fixed;
}

void ScoreEngine::improveSchedule()
{
  fixBalance();
  fixRoomUsage();
  fixTravel();
}