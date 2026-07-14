#include "test_framework.h"
#include "../include/loader.h"
#include "../include/schedule_state.h"
#include "../include/scheduler.h"
#include "../include/score_engine.h"
#include <fstream>
#include <cstdio>
using namespace std;

// ── Tiny inline CSV helpers for edge-case tests (write to /tmp) ────────────
static void writeTmp(const string &path, const string &content)
{
    ofstream f(path);
    f << content;
}
static void rmTmp(const string &path) { remove(path.c_str()); }

// ── Real LUMS data paths (relative to project root) ───────────────────────
static const string COURSES_CSV = "data/courses.csv";
static const string ROOMS_CSV = "data/rooms.csv";
static const string SLOTS_CSV = "data/slots.csv";

void test_integration()
{

    // ============================================================
    beginSuite("INTEGRATION -- Loading Real LUMS Data");
    // ============================================================

    {
        auto cl = loadCourses(COURSES_CSV);
        ASSERT_EQ((int)cl.size(), 34, "34 SSE/CS/EE/MATH/PHY courses loaded");
        ASSERT_EQ(SAFE_IDX(cl, 0, Course{}).id, string("CS200"), "first course is CS200");
    }
    {
        auto r = loadRooms(ROOMS_CSV);
        ASSERT_EQ((int)r.size(), 20, "20 LUMS rooms loaded");
        ASSERT_TRUE(r.count("SBASSE-9B2") > 0, "SBASSE-9B2 in map");
        ASSERT_TRUE(r.count("SDSB-B3") > 0, "SDSB-B3 in map");
        ASSERT_TRUE(r.count("AcadBlock-A11") > 0, "AcadBlock-A11 in map");
        ASSERT_EQ(r.at("SBASSE-9B2").capacity, 250, "lecture theatre capacity 250");
        ASSERT_EQ(r.at("SDSB-B3").capacity, 150, "SDSB-B3 capacity 150");
        ASSERT_EQ(r.at("SBASSE-10-201").capacity, 60, "lab room capacity 60");
    }
    {
        auto s = loadSlots(SLOTS_CSV);
        ASSERT_EQ((int)s.size(), 14, "14 time slots loaded (MWF + TTh)");
        ASSERT_EQ(SAFE_IDX(s, 0, ""), string("MWF-0800"), "first slot is MWF-0800");
        ASSERT_EQ(SAFE_IDX(s, 7, ""), string("TTh-0800"), "8th slot is TTh-0800");
    }
    {
        auto cl = loadCourses(COURSES_CSV);
        auto cm = buildCourseMap(cl);
        ASSERT_EQ((int)cm.size(), 34, "course map has 34 entries");
        ASSERT_TRUE(cm.count("CS200") > 0, "CS200 in map");
        ASSERT_TRUE(cm.count("PHY332") > 0, "PHY332 in map");
        ASSERT_EQ(cm.at("CS200").instructor, string("Mian Muhammad Awais"),
                  "CS200 instructor correct");
        ASSERT_EQ(cm.at("CS200").enrollment, 120, "CS200 enrollment 120");
        ASSERT_EQ(cm.at("MATH101").enrollment, 90, "MATH101 enrollment 90");
    }

    // ============================================================
    beginSuite("INTEGRATION -- Capacity Constraints (Real LUMS Rooms)");
    // ============================================================

    {
        // CS200 (enrollment=120) should be placed in SBASSE-9B2(250), SDSB-B3(150) or SDSB-B1(150)
        // NOT in SBASSE-10-201(60) or SDSB-203(60)
        auto cl = loadCourses(COURSES_CSV);
        auto r = loadRooms(ROOMS_CSV);
        auto s = loadSlots(SLOTS_CSV);
        auto c = buildCourseMap(cl);
        ScheduleState st;
        Scheduler sch(st, c, r, s);
        sch.buildInitialSchedule();
        ASSERT_TRUE(st.isCourseAssigned("CS200"), "CS200 scheduled");
        string room = st.getCourseAssignment("CS200").first;
        ASSERT_TRUE(r.at(room).capacity >= 120,
                    "CS200 placed in room with capacity >= 120");
    }
    {
        // PHY332 (enrollment=45) fits in SBASSE-10-201(60) -- smallest valid room
        auto cl = loadCourses(COURSES_CSV);
        auto r = loadRooms(ROOMS_CSV);
        auto s = loadSlots(SLOTS_CSV);
        auto c = buildCourseMap(cl);
        ScheduleState st;
        Scheduler sch(st, c, r, s);
        sch.buildInitialSchedule();
        ASSERT_TRUE(st.isCourseAssigned("PHY332"), "PHY332 scheduled");
        string room = st.getCourseAssignment("PHY332").first;
        ASSERT_TRUE(r.at(room).capacity >= 45,
                    "PHY332 placed in room with capacity >= 45");
    }
    {
        // PHY104
        auto cl = loadCourses(COURSES_CSV);
        auto r = loadRooms(ROOMS_CSV);
        auto s = loadSlots(SLOTS_CSV);
        auto c = buildCourseMap(cl);
        ScheduleState st;
        Scheduler sch(st, c, r, s);
        sch.buildInitialSchedule();
        ASSERT_TRUE(st.isCourseAssigned("PHY104"), "PHY104 scheduled");
        string room = st.getCourseAssignment("PHY104").first;
        ASSERT_TRUE(r.at(room).capacity >= 95,
                    "PHY104 (95 students) placed in adequate room");
    }

    // ============================================================
    beginSuite("INTEGRATION -- Instructor Conflicts (Real LUMS Faculty)");
    // ============================================================

    {
        // Maryam Abdul Ghafoor teaches CS202 and CS464 -- must be different slots
        auto cl = loadCourses(COURSES_CSV);
        auto r = loadRooms(ROOMS_CSV);
        auto s = loadSlots(SLOTS_CSV);
        auto c = buildCourseMap(cl);
        ScheduleState st;
        Scheduler sch(st, c, r, s);
        sch.buildInitialSchedule();
        ASSERT_TRUE(st.isCourseAssigned("CS202"), "CS202 scheduled");
        ASSERT_TRUE(st.isCourseAssigned("CS464"), "CS464 scheduled");
        string s1 = st.getCourseAssignment("CS202").second;
        string s2 = st.getCourseAssignment("CS464").second;
        ASSERT_NEQ(s1, s2, "CS202 and CS464 (Maryam Abdul Ghafoor) in different slots");
    }
    {
        // Aamina Jamal Khan teaches CS310 and CS466 -- must be different slots
        auto cl = loadCourses(COURSES_CSV);
        auto r = loadRooms(ROOMS_CSV);
        auto s = loadSlots(SLOTS_CSV);
        auto c = buildCourseMap(cl);
        ScheduleState st;
        Scheduler sch(st, c, r, s);
        sch.buildInitialSchedule();
        ASSERT_TRUE(st.isCourseAssigned("CS310"), "CS310 scheduled");
        ASSERT_TRUE(st.isCourseAssigned("CS466"), "CS466 scheduled");
        string s1 = st.getCourseAssignment("CS310").second;
        string s2 = st.getCourseAssignment("CS466").second;
        ASSERT_NEQ(s1, s2,
                   "CS310 and CS466 (Aamina Jamal Khan) in different slots");
    }
    {
        // Omar Khawar Malik teaches MATH230, MATH231, PHY305 -- all different slots
        auto cl = loadCourses(COURSES_CSV);
        auto r = loadRooms(ROOMS_CSV);
        auto s = loadSlots(SLOTS_CSV);
        auto c = buildCourseMap(cl);
        ScheduleState st;
        Scheduler sch(st, c, r, s);
        sch.buildInitialSchedule();
        string s1 = st.getCourseAssignment("MATH230").second;
        string s2 = st.getCourseAssignment("MATH231").second;
        string s3 = st.getCourseAssignment("PHY305").second;
        ASSERT_NEQ(s1, s2, "MATH230 and MATH231 (Omar Khawar Malik) in different slots");
        ASSERT_NEQ(s1, s3, "MATH230 and PHY305 (Omar Khawar Malik) in different slots");
        ASSERT_NEQ(s2, s3, "MATH231 and PHY305 (Omar Khawar Malik) in different slots");
    }

    // ============================================================
    beginSuite("INTEGRATION -- Room Double-Booking (Real Rooms)");
    // ============================================================

    {
        auto cl = loadCourses(COURSES_CSV);
        auto r = loadRooms(ROOMS_CSV);
        auto s = loadSlots(SLOTS_CSV);
        auto c = buildCourseMap(cl);
        ScheduleState st;
        Scheduler sch(st, c, r, s);
        sch.buildInitialSchedule();
        // Check no two courses share the same room AND slot
        volatile bool ok = true;
        auto assigned = st.getAllAssignedCourses();
        for (int i = 0; i < (int)assigned.size(); i++)
            for (int j = i + 1; j < (int)assigned.size(); j++)
            {
                auto [r1, sl1] = st.getCourseAssignment(assigned[i]);
                auto [r2, sl2] = st.getCourseAssignment(assigned[j]);
                if (r1 == r2 && sl1 == sl2)
                    ok = false;
            }
        ASSERT_TRUE(ok, "no room double-booking across all 34 LUMS courses");
    }

    // ============================================================
    beginSuite("INTEGRATION -- Full Schedule Consistency");
    // ============================================================

    {
        auto cl = loadCourses(COURSES_CSV);
        auto r = loadRooms(ROOMS_CSV);
        auto s = loadSlots(SLOTS_CSV);
        auto c = buildCourseMap(cl);
        ScheduleState st;
        Scheduler sch(st, c, r, s);
        sch.buildInitialSchedule();
        ASSERT_TRUE(st.isConsistent(c),
                    "state fully consistent after scheduling 34 LUMS courses");
    }
    {
        // All assigned rooms must be real LUMS rooms
        auto cl = loadCourses(COURSES_CSV);
        auto r = loadRooms(ROOMS_CSV);
        auto s = loadSlots(SLOTS_CSV);
        auto c = buildCourseMap(cl);
        ScheduleState st;
        Scheduler sch(st, c, r, s);
        sch.buildInitialSchedule();
        volatile bool ok = true;
        for (auto &[id, co] : c)
        {
            if (!st.isCourseAssigned(id))
                continue;
            string room = st.getCourseAssignment(id).first;
            if (r.count(room) == 0)
                ok = false;
        }
        ASSERT_TRUE(ok, "all assigned rooms are valid LUMS room IDs");
    }
    {
        // All assigned slots must be real LUMS slots
        auto cl = loadCourses(COURSES_CSV);
        auto r = loadRooms(ROOMS_CSV);
        auto s = loadSlots(SLOTS_CSV);
        auto c = buildCourseMap(cl);
        ScheduleState st;
        Scheduler sch(st, c, r, s);
        sch.buildInitialSchedule();
        volatile bool ok = true;
        for (auto &[id, co] : c)
        {
            if (!st.isCourseAssigned(id))
                continue;
            string slot = st.getCourseAssignment(id).second;
            bool found = false;
            for (auto &sv : s)
                if (sv == slot)
                {
                    found = true;
                    break;
                }
            if (!found)
                ok = false;
        }
        ASSERT_TRUE(ok, "all assigned slots are valid LUMS time slots");
    }

    // ============================================================
    beginSuite("INTEGRATION -- Score Engine on Real Schedule");
    // ============================================================

    {
        // Balance score should be reasonable (not extreme) with 34 courses / 14 slots
        auto cl = loadCourses(COURSES_CSV);
        auto r = loadRooms(ROOMS_CSV);
        auto s = loadSlots(SLOTS_CSV);
        auto c = buildCourseMap(cl);
        ScheduleState st;
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);
        sch.buildInitialSchedule();
        se.fixBalance();
        double bal = se.computeBalanceScore();
        ASSERT_DOUBLE_GTE(bal, 0.0, "balance score >= 0");
        ASSERT_DOUBLE_GTE(20.0, bal, "balance score <= 20 (not wildly unbalanced)");
    }
    {
        // State stays consistent after improvement
        auto cl = loadCourses(COURSES_CSV);
        auto r = loadRooms(ROOMS_CSV);
        auto s = loadSlots(SLOTS_CSV);
        auto c = buildCourseMap(cl);
        ScheduleState st;
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);
        sch.buildInitialSchedule();
        se.improveSchedule();
        ASSERT_TRUE(st.isConsistent(c), "state consistent after improveSchedule on real LUMS data");
    }

    // ============================================================
    beginSuite("INTEGRATION -- Edge Cases");
    // ============================================================

    {
        // Empty course list
        map<string, Course> c;
        map<string, Room> r = {{"R1", {"R1", 50}}};
        vector<string> s = {"S1"};
        ScheduleState st;
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);
        sch.buildInitialSchedule();
        ASSERT_DOUBLE_EQ(se.computeFinalScore(), 0.0, "empty course list scores 0");
    }
    {
        // Single course, single room, single slot
        map<string, Course> c = {{"CS200", {"CS200", "Mian Muhammad Awais", 120}}};
        map<string, Room> r = {{"SBASSE-9B2", {"SBASSE-9B2", 250}}};
        vector<string> s = {"TTh-0800"};
        ScheduleState st;
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);
        sch.buildInitialSchedule();
        ASSERT_TRUE(st.isCourseAssigned("CS200"), "single course scheduled");
        ASSERT_PAIR_EQ(st.getCourseAssignment("CS200"), "SBASSE-9B2", "TTh-0800", "assigned to the only valid room/slot");
        ASSERT_TRUE(st.isConsistent(c), "consistent");
    }
    {
        // Course too large for any room
        map<string, Course> c = {{"HUGE", {"HUGE", "Dr X", 9999}}};
        map<string, Room> r = {{"SBASSE-9B2", {"SBASSE-9B2", 250}}};
        vector<string> s = {"MWF-0800"};
        ScheduleState st;
        Scheduler sch(st, c, r, s);
        sch.buildInitialSchedule();
        ASSERT_FALSE(st.isCourseAssigned("HUGE"), "course with 9999 enrollment not scheduled (no room fits)");
    }
    {
        // Instructor teaches more courses than slots available
        map<string, Course> c = {
            {"C1", {"C1", "Mian Muhammad Awais", 30}},
            {"C2", {"C2", "Mian Muhammad Awais", 30}},
            {"C3", {"C3", "Mian Muhammad Awais", 30}}};
        map<string, Room> r = {{"SDSB-101", {"SDSB-101", 80}}};
        vector<string> s = {"MWF-0800", "TTh-0800"}; // only 2 slots, 3 courses
        ScheduleState st;
        Scheduler sch(st, c, r, s);
        sch.buildInitialSchedule();
        int n = 0;
        for (auto &[id, co] : c)
            if (st.isCourseAssigned(id))
                n++;
        ASSERT_EQ(n, 2, "only 2/3 courses scheduled when instructor has 2 slots");
    }

    // ============================================================
    beginSuite("INTEGRATION -- Tiny Inline CSV Round-Trip");
    // ============================================================

    {
        // Write, load, and use a tiny hand-crafted LUMS-style schedule
        writeTmp("/tmp/pa3_c.csv",
                 "id,instructor,enrollment\n"
                 "CS200,Mian Muhammad Awais,120\n"
                 "EE220,Nauman Zafar Butt,80\n"
                 "MATH101,Haniya Azam,90\n");
        writeTmp("/tmp/pa3_r.csv",
                 "id,capacity\n"
                 "SBASSE-9B2,250\nSBASSE-10-301,80\nSBASSE-10-201,60\n");
        writeTmp("/tmp/pa3_s.csv",
                 "MWF-0800\nMWF-0930\nTTh-0800\nTTh-0930\n");
        auto cl = loadCourses("/tmp/pa3_c.csv");
        auto r = loadRooms("/tmp/pa3_r.csv");
        auto s = loadSlots("/tmp/pa3_s.csv");
        auto c = buildCourseMap(cl);
        ASSERT_EQ((int)cl.size(), 3, "3 courses loaded from tmp");
        ASSERT_EQ((int)r.size(), 3, "3 rooms loaded from tmp");
        ASSERT_EQ((int)s.size(), 4, "4 slots loaded from tmp");
        ScheduleState st;
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);
        sch.buildInitialSchedule();
        ASSERT_EQ((int)st.getAllAssignedCourses().size(), 3, "all 3 courses scheduled");
        ASSERT_TRUE(st.isConsistent(c), "state consistent after round-trip");
        rmTmp("/tmp/pa3_c.csv");
        rmTmp("/tmp/pa3_r.csv");
        rmTmp("/tmp/pa3_s.csv");
    }
}
