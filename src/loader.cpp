#include "../include/loader.h"

vector<string> split(string line, char delimiter)
{
    vector<string> result;
    string temp = "";

    for (size_t i = 0; i < line.length(); i++)
    {
        if (line[i] == delimiter)
        {
            result.push_back(temp);
            temp = "";
        }
        else
        {
            temp += line[i];
        }
    }
    result.push_back(temp);
    return result;
}

Course parseCourse(string line)
{
    vector<string> sep = split(line, ',');
    Course c;
    c.id = sep[0];
    c.instructor = sep[1];
    c.enrollment = stoi(sep[2]);
    return c;
}

Room parseRoom(string line)
{
    vector<string> sep = split(line, ',');
    Room r;
    r.id = sep[0];
    r.capacity = stoi(sep[1]);
    return r;
}

string parseSlot(string line)
{
    string result;
    for (size_t i = 0; i < line.length(); i++)
    {
        if (line[i] != ' ' && line[i] != '\n' && line[i] != '\r')
        {
            result += line[i];
        }
    }
    return result;
}

vector<Course> loadCourses(string filename)
{
    ifstream file(filename);
    vector<Course> courses;
    string line;

    getline(file, line);
    while (true)
    {
        if (!getline(file, line))
        {
            break;
        }

        if (line == "")
        {
            continue;
        }
        Course c = parseCourse(line);
        courses.push_back(c);
    }
    file.close();
    return courses;
}

map<string, Room> loadRooms(string filename)
{
    ifstream file(filename);
    map<string, Room> rooms;
    string line;

    getline(file, line);
    while (true)
    {
        if (!getline(file, line))
        {
            break;
        }

        if (line == "")
        {
            continue;
        }
        Room r = parseRoom(line);
        rooms[r.id] = r;
    }
    file.close();
    return rooms;
}

vector<string> loadSlots(string filename)
{
    ifstream file(filename);
    vector<string> slots;
    string line;

    while (true)
    {
        if (!getline(file, line))
        {
            break;
        }

        if (line == "")
        {
            continue;
        }
        string s = parseSlot(line);
        slots.push_back(s);
    }
    file.close();
    return slots;
}

map<string, Course> buildCourseMap(vector<Course> &courses)
{
    map<string, Course> cmap;
    for (size_t i = 0; i < courses.size(); i++)
    {
        string id = courses[i].id;
        if (cmap.find(id) == cmap.end())
        {
            cmap[id] = courses[i];
        }
    }
    return cmap;
}