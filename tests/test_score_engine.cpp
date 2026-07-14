#include "test_framework.h"
#include "../include/score_engine.h"
using namespace std;

static map<string, Course> g_se_courses = {
    {"CS200", {"CS200", "DrA", 40}}, {"MATH101", {"MATH101", "DrB", 80}}, {"BIO101", {"BIO101", "DrC", 25}}, {"CS300", {"CS300", "DrA", 30}}, {"EE201", {"EE201", "DrD", 35}}};
static map<string, Room> g_se_rooms = {
    {"SBASSE-101", {"SBASSE-101", 50}}, {"SBASSE-102", {"SBASSE-102", 35}}, {"SDSB-B3", {"SDSB-B3", 90}}, {"SSE-201", {"SSE-201", 200}}};
static vector<string> g_se_slots = {
    "MWF-0800", "MWF-0930", "MWF-1100", "TTh-0800", "TTh-0930"};
static map<string, Course> mkC() { return g_se_courses; }
static map<string, Room> mkR() { return g_se_rooms; }
static vector<string> mkS() { return g_se_slots; }

void test_score_engine()
{

    beginSuite("SCORE_ENGINE -- computeUtilizationScore");

    {
        ScheduleState st;
        auto c = mkC();
        auto r = mkR();
        auto s = mkS();
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);
        st.assign("CS200", "DrA", "SBASSE-101", "MWF-0800"); // waste=50-40=10
        ASSERT_DOUBLE_EQ(se.computeUtilizationScore(), 10.0, "waste == 10");
    }
    {
        ScheduleState st;
        auto c = mkC();
        auto r = mkR();
        auto s = mkS();
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);
        st.assign("CS200", "DrA", "SSE-201", "MWF-0800"); // waste=200-40=160
        ASSERT_DOUBLE_EQ(se.computeUtilizationScore(), 160.0, "waste == 160");
    }
    {
        ScheduleState st;
        auto c = mkC();
        auto r = mkR();
        auto s = mkS();
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);
        ASSERT_DOUBLE_EQ(se.computeUtilizationScore(), 0.0, "empty schedule waste == 0");
    }

    beginSuite("SCORE_ENGINE -- computeBalanceScore");

    {
        ScheduleState st;
        auto c = mkC();
        auto r = mkR();
        auto s = mkS();
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);
        // 1 course in each of 5 slots
        for (int i = 0; i < 5; i++)
            st.addCourseToSlot("C" + to_string(i), s[i]);
        ASSERT_DOUBLE_EQ(se.computeBalanceScore(), 0.0, "perfectly balanced = 0.0");
    }
    {
        ScheduleState st;
        auto c = mkC();
        auto r = mkR();
        auto s = mkS();
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);
        for (int i = 0; i < 5; i++)
            st.addCourseToSlot("C" + to_string(i), "MWF-0800");
        // max=5 min=0 -> balance=5
        ASSERT_DOUBLE_EQ(se.computeBalanceScore(), 5.0, "balance = max(5)-min(0) = 5");
    }
    {
        ScheduleState st;
        auto c = mkC();
        auto r = mkR();
        auto s = mkS();
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);
        st.addCourseToSlot("A1", "MWF-0800");
        st.addCourseToSlot("A2", "MWF-0800");
        st.addCourseToSlot("A3", "MWF-0800");
        st.addCourseToSlot("B1", "MWF-0930");
        // max=3 min=0 -> 3
        ASSERT_DOUBLE_EQ(se.computeBalanceScore(), 3.0, "balance = max(3)-min(0) = 3");
    }

    beginSuite("SCORE_ENGINE -- computeTravelScore");

    {
        ScheduleState st;
        auto c = mkC();
        auto r = mkR();
        auto s = mkS();
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);
        // DrA: MWF-0800 SBASSE-101, MWF-0930 SBASSE-102 -- same building
        st.assign("CS200", "DrA", "SBASSE-101", "MWF-0800");
        st.assign("CS300", "DrA", "SBASSE-102", "MWF-0930");
        ASSERT_DOUBLE_EQ(se.computeTravelScore(), 0.0, "same building = 0 penalty");
    }
    {
        ScheduleState st;
        auto c = mkC();
        auto r = mkR();
        auto s = mkS();
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);
        // DrA: MWF-0800 SBASSE-101, MWF-0930 SDSB-B3 -- different building
        st.assign("CS200", "DrA", "SBASSE-101", "MWF-0800");
        st.assign("CS300", "DrA", "SDSB-B3", "MWF-0930");
        ASSERT_DOUBLE_GT(se.computeTravelScore(), 0.0, "different buildings = penalty > 0");
    }
    {
        ScheduleState st;
        auto c = mkC();
        auto r = mkR();
        auto s = mkS();
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);
        ASSERT_DOUBLE_EQ(se.computeTravelScore(), 0.0, "empty schedule travel score == 0");
    }

    beginSuite("SCORE_ENGINE -- computeFinalScore");

    {
        ScheduleState st;
        auto c = mkC();
        auto r = mkR();
        auto s = mkS();
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);
        ASSERT_DOUBLE_EQ(se.computeFinalScore(), 0.0, "empty schedule final score == 0");
    }

    beginSuite("SCORE_ENGINE -- identifyWorstSlot / identifyUnderutilizedRoom");

    {
        ScheduleState st;
        auto c = mkC();
        auto r = mkR();
        auto s = mkS();
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);
        st.addCourseToSlot("C1", "MWF-0800");
        st.addCourseToSlot("C2", "MWF-0930");
        st.addCourseToSlot("C3", "MWF-0930");
        st.addCourseToSlot("C4", "MWF-1100");
        st.addCourseToSlot("C5", "MWF-1100");
        st.addCourseToSlot("C6", "MWF-1100");
        ASSERT_EQ(se.identifyWorstSlot(), string("MWF-1100"), "MWF-1100 worst slot (3 courses)");
    }

    // -------------------------------------------------
    {
        ScheduleState st;
        auto c = mkC();
        auto r = mkR();
        auto s = mkS();
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);
        st.addCourseToSlot("C1", "TTh-0800");
        st.addCourseToSlot("C2", "TTh-0800");
        st.addCourseToSlot("C3", "TTh-0800");
        st.addCourseToSlot("C4", "TTh-0800");
        st.addCourseToSlot("C5", "MWF-0930");
        st.addCourseToSlot("C6", "MWF-0930");
        st.addCourseToSlot("C7", "MWF-1100");
        ASSERT_EQ(se.identifyWorstSlot(), string("TTh-0800"), "slot with highest load is returned");
    }
    {
        ScheduleState st;
        auto c = mkC();
        auto r = mkR();
        auto s = mkS();
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);
        st.assign("CS200", "DrA", "SBASSE-101", "MWF-0800");  // waste = 50-40 = 10
        st.assign("BIO101", "DrC", "SBASSE-102", "MWF-0930"); // waste = 35-25 = 10
        st.assign("MATH101", "DrB", "SDSB-B3", "MWF-1100");   // waste = 90-80 = 10
        st.assign("EE201", "DrD", "SSE-201", "TTh-0800");     // waste = 200-35 = 165
        ASSERT_EQ(se.identifyUnderutilizedRoom(), string("SSE-201"), "room with most wasted seats is returned");
    }
    {
        ScheduleState st;
        auto c = mkC();
        auto r = mkR();
        auto s = mkS();
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);
        st.assign("CS200", "DrA", "SBASSE-101", "MWF-0800");  // waste = 10
        st.assign("CS300", "DrA", "SDSB-B3", "MWF-0930");     // waste = 90-30 = 60
        st.assign("BIO101", "DrC", "SBASSE-102", "MWF-1100"); // waste = 35-25 = 10
        ASSERT_EQ(se.identifyUnderutilizedRoom(), string("SDSB-B3"), "largest-waste room identified correctly");
    }
    {
        ScheduleState st;
        auto c = mkC();
        auto r = mkR();
        auto s = mkS();
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);
        st.assign("MATH101", "DrB", "SSE-201", "MWF-0800");  // waste = 200-80 = 120
        st.assign("CS200", "DrA", "SBASSE-101", "MWF-0930"); // waste = 10
        st.assign("EE201", "DrD", "SBASSE-102", "TTh-0800"); // waste = 0
        ASSERT_EQ(se.identifyUnderutilizedRoom(), string("SSE-201"), "underutilized room remains the one with maximum waste");
    }

    beginSuite("SCORE_ENGINE -- Fix Functions");

    {
        ScheduleState st;
        auto c = mkC();
        auto r = mkR();
        auto s = mkS();
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);

        sch.buildInitialSchedule();
        double before = se.computeBalanceScore();

        // Artificially overload one slot using real, catalog-backed courses.
        // First remove these courses from wherever the initial builder placed them.
        const vector<string> overloaded = {"CS200", "MATH101", "BIO101"};

        for (const string &courseId : overloaded)
        {
            if (st.isCourseAssigned(courseId))
            {
                auto [room, slot] = st.getCourseAssignment(courseId);
                st.remove(courseId, c.at(courseId).instructor, room, slot);
            }
        }

        // Reassign them into the same slot in different rooms.
        st.assign("CS200", c.at("CS200").instructor, "SBASSE-101", "MWF-0800"), "reassign CS200 into overloaded slot";
        st.assign("MATH101", c.at("MATH101").instructor, "SSE-201", "MWF-0800"), "reassign MATH101 into overloaded slot";
        st.assign("BIO101", c.at("BIO101").instructor, "SBASSE-102", "MWF-0800"), "reassign BIO101 into overloaded slot";

        double overloadedScore = se.computeBalanceScore();
        ASSERT_DOUBLE_GT(overloadedScore, before, "balance score should worsen after overloading one slot");

        se.fixBalance();
        double after = se.computeBalanceScore();

        ASSERT_DOUBLE_GTE(overloadedScore, after, "balance score should decrease after fixBalance");
    }
    {
        ScheduleState st;
        auto c = mkC();
        auto r = mkR();
        auto s = mkS();
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);
        st.assign("CS200", "DrA", "SSE-201", "MWF-0800"); // waste=160
        ASSERT_DOUBLE_EQ(se.computeUtilizationScore(), 160.0, "utilization 160 before fixRoomUsage");
        se.fixRoomUsage();
        double after = se.computeUtilizationScore();
        ASSERT_DOUBLE_GTE(160.0, after, "utilization should decrease after fixRoomUsage");
    }
    {
        ScheduleState st;
        auto c = mkC();
        auto r = mkR();
        auto s = mkS();
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);
        // DrA: MWF-0800 SBASSE-101, MWF-0930 SDSB-B3 -- travel penalty
        st.assign("CS200", "DrA", "SBASSE-101", "MWF-0800");
        st.assign("CS300", "DrA", "SDSB-B3", "MWF-0930");
        double before = se.computeTravelScore();
        se.fixTravel();
        double after = se.computeTravelScore();
        ASSERT_DOUBLE_GTE(before, after, "travel score should decrease after fixTravel");
    }
    //------------------------------------------------

    {
        ScheduleState st;
        auto c = mkC();
        auto r = mkR();
        auto s = mkS();
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);

        sch.buildInitialSchedule();
        se.fixBalance();

        auto assigned = st.getAllAssignedCourses();
        volatile bool ok = true;
        volatile bool cap_ok = true;

        for (int i = 0; i < (int)assigned.size(); i++)
        {
            auto [r1, s1] = st.getCourseAssignment(assigned[i]);
            string inst1 = c.at(assigned[i]).instructor;

            if (!(r.at(r1).capacity >= c.at(assigned[i]).enrollment))
            {
                cap_ok = false;
            }

            for (int j = i + 1; j < (int)assigned.size(); j++)
            {
                auto [r2, s2] = st.getCourseAssignment(assigned[j]);
                string inst2 = c.at(assigned[j]).instructor;

                if (r1 == r2 && s1 == s2)
                    ok = false;
                if (inst1 == inst2 && s1 == s2)
                    ok = false;
            }
        }

        ASSERT_TRUE(cap_ok, "assigned room has sufficient capacity after fixBalance");
        ASSERT_TRUE(ok, "fixBalance leaves no room/instructor clashes");
        ASSERT_TRUE(st.isConsistent(c), "state consistent after fixBalance");
    }
    {
        ScheduleState st;
        auto c = mkC();
        auto r = mkR();
        auto s = mkS();
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);

        sch.buildInitialSchedule();
        se.fixRoomUsage();

        auto assigned = st.getAllAssignedCourses();
        volatile bool ok = true;
        volatile bool cap_ok = true;

        for (int i = 0; i < (int)assigned.size(); i++)
        {
            auto [r1, s1] = st.getCourseAssignment(assigned[i]);
            string inst1 = c.at(assigned[i]).instructor;

            if (!(r.at(r1).capacity >= c.at(assigned[i]).enrollment))
            {
                cap_ok = false;
            }

            for (int j = i + 1; j < (int)assigned.size(); j++)
            {
                auto [r2, s2] = st.getCourseAssignment(assigned[j]);
                string inst2 = c.at(assigned[j]).instructor;

                if (r1 == r2 && s1 == s2)
                    ok = false;
                if (inst1 == inst2 && s1 == s2)
                    ok = false;
            }
        }

        ASSERT_TRUE(cap_ok, "assigned room has sufficient capacity after fixRoomUsage");
        ASSERT_TRUE(ok, "fixRoomUsage leaves no room/instructor clashes");
        ASSERT_TRUE(st.isConsistent(c), "state consistent after fixRoomUsage");
    }
    {
        ScheduleState st;
        auto c = mkC();
        auto r = mkR();
        auto s = mkS();
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);

        sch.buildInitialSchedule();
        se.fixTravel();

        auto assigned = st.getAllAssignedCourses();
        volatile bool ok = true;
        volatile bool cap_ok = true;

        for (int i = 0; i < (int)assigned.size(); i++)
        {
            auto [r1, s1] = st.getCourseAssignment(assigned[i]);
            string inst1 = c.at(assigned[i]).instructor;

            if (!(r.at(r1).capacity >= c.at(assigned[i]).enrollment))
            {
                cap_ok = false;
            }

            for (int j = i + 1; j < (int)assigned.size(); j++)
            {
                auto [r2, s2] = st.getCourseAssignment(assigned[j]);
                string inst2 = c.at(assigned[j]).instructor;

                if (r1 == r2 && s1 == s2)
                    ok = false;
                if (inst1 == inst2 && s1 == s2)
                    ok = false;
            }
        }

        ASSERT_TRUE(cap_ok, "assigned room has sufficient capacity after fixTravel");
        ASSERT_TRUE(ok, "fixTravel leaves no room/instructor clashes");
        ASSERT_TRUE(st.isConsistent(c), "state consistent after fixTravel");
    }

    beginSuite("SCORE_ENGINE -- improveSchedule integration");

    {
        ScheduleState st;
        auto c = mkC();
        auto r = mkR();
        auto s = mkS();
        Scheduler sch(st, c, r, s);
        ScoreEngine se(st, sch, c, r, s);
        sch.buildInitialSchedule();
        se.improveSchedule();
        ASSERT_TRUE(st.isConsistent(c), "state consistent after improveSchedule");
    }
}
