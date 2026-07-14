#include "test_framework.h"
#include "../include/scheduler.h"
using namespace std;

// ALL catalog data is STATIC -- never destroyed, safe across all test blocks
static map<string, Course> g_c = {
    {"CS200", {"CS200", "DrA", 40}}, {"MATH101", {"MATH101", "DrB", 80}}, {"BIO101", {"BIO101", "DrC", 25}}, {"CS300", {"CS300", "DrA", 30}}, {"TINY", {"TINY", "DrX", 1}}, {"HUGE", {"HUGE", "DrY", 200}}};
static map<string, Room> g_r = {
    {"SBASSE-101", {"SBASSE-101", 50}}, {"SBASSE-102", {"SBASSE-102", 35}}, {"SDSB-B3", {"SDSB-B3", 70}}, {"SSE-201", {"SSE-201", 90}}, {"TINY-R", {"TINY-R", 5}}, {"HUGE-R", {"HUGE-R", 250}}};
static vector<string> g_s = {
    "MWF-0800", "MWF-0930", "MWF-1100", "TTh-0800", "TTh-0930", "TTh-1100", "TTh-1400"};

// Small local-only catalogs for isolated tests -- also static
static map<string, Course> g_swap_c = {{"C1", {"C1", "I1", 30}}, {"C2", {"C2", "I2", 30}}};
static map<string, Room> g_swap_r = {{"R1", {"R1", 50}}, {"R2", {"R2", 50}}};
static vector<string> g_swap_s = {"S1", "S2"};
static map<string, Course> g_cap_c = {{"BIG", {"BIG", "I1", 100}}, {"SMALL", {"SMALL", "I2", 10}}};
static map<string, Room> g_cap_r = {{"R-BIG", {"R-BIG", 120}}, {"R-SMALL", {"R-SMALL", 15}}};
static map<string, Course> g_tiny_c = {{"BIG", {"BIG", "I1", 1000}}};
static map<string, Room> g_tiny_r = {{"TINY", {"TINY", 5}}};
static vector<string> g_tiny_s = {"S1"};
static map<string, Course> g_exact_c = {{"C1", {"C1", "I1", 50}}};
static map<string, Room> g_exact_r = {{"EXACT", {"EXACT", 50}}, {"SMALL", {"SMALL", 49}}};
static map<string, Room> g_nolarge_r = {{"SBASSE-101", {"SBASSE-101", 50}}, {"TINY-R", {"TINY-R", 5}}};
static map<string, Course> g_best_c = {{"C1", {"C1", "I1", 40}}};
static map<string, Room> g_best_r = {{"BIG", {"BIG", 200}}, {"MED", {"MED", 45}}, {"EXACT", {"EXACT", 40}}};
static vector<string> g_one_s = {"S1"};
static map<string, Room> g_one_r = {{"R1", {"R1", 50}}};
static map<string, Course> g_busy_c = {{"C1", {"C1", "BUSY", 10}}, {"C2", {"C2", "BUSY", 10}}, {"C3", {"C3", "BUSY", 10}}};
static map<string, Room> g_busy_r = {{"R1", {"R1", 20}}, {"R2", {"R2", 20}}};
static vector<string> g_two_s = {"S1", "S2"};

void test_scheduler()
{

    beginSuite("SCHEDULER -- getAvailableRooms");

    { // CS200(40): rooms >=40 qualify
        ScheduleState st;
        Scheduler sch(st, g_c, g_r, g_s);
        auto av = sch.getAvailableRooms(g_c.at("CS200"));
        ASSERT_VEC_CONTAINS(av, "SBASSE-101", "50>=40 qualifies");
        ASSERT_VEC_CONTAINS(av, "SDSB-B3", "70>=40 qualifies");
        ASSERT_VEC_CONTAINS(av, "SSE-201", "90>=40 qualifies");
        ASSERT_VEC_CONTAINS(av, "HUGE-R", "250>=40 qualifies");
        ASSERT_VEC_NOT_CONTAINS(av, "SBASSE-102", "35<40 does not qualify");
        ASSERT_VEC_NOT_CONTAINS(av, "TINY-R", "5<40 does not qualify");
    }
    { // BIO101(25)
        ScheduleState st;
        Scheduler sch(st, g_c, g_r, g_s);
        auto av = sch.getAvailableRooms(g_c.at("BIO101"));
        ASSERT_VEC_CONTAINS(av, "SBASSE-102", "35>=25 qualifies");
        ASSERT_VEC_NOT_CONTAINS(av, "TINY-R", "5<25 does not qualify");
    }
    { // Exact capacity match
        ScheduleState st;
        Scheduler sch(st, g_exact_c, g_exact_r, g_one_s);
        auto av = sch.getAvailableRooms(g_exact_c.at("C1"));
        ASSERT_VEC_CONTAINS(av, "EXACT", "exact capacity qualifies");
        ASSERT_VEC_NOT_CONTAINS(av, "SMALL", "one seat short does not qualify");
    }
    { // No room fits
        ScheduleState st;
        Scheduler sch(st, g_c, g_nolarge_r, g_one_s);
        auto av = sch.getAvailableRooms(g_c.at("HUGE"));
        ASSERT_VEC_EMPTY(av, "empty when no room fits");
    }

    beginSuite("SCHEDULER -- getAvailableSlots");

    { // All slots free
        ScheduleState st;
        Scheduler sch(st, g_c, g_r, g_s);
        auto av = sch.getAvailableSlots(g_c.at("CS200"));
        ASSERT_VEC_SIZE(av, (int)g_s.size(), "all slots free when instructor unbooked");
    }
    { // DrA booked in MWF-0800
        ScheduleState st;
        st.assignInstructorToSlot("DrA", "MWF-0800");
        Scheduler sch(st, g_c, g_r, g_s);
        auto av = sch.getAvailableSlots(g_c.at("CS200"));
        ASSERT_VEC_NOT_CONTAINS(av, "MWF-0800", "booked slot excluded");
        ASSERT_VEC_CONTAINS(av, "MWF-0930", "free slot included");
        ASSERT_EQ((int)av.size(), (int)g_s.size() - 1, "one fewer slot");
    }
    { // DrA fully booked
        ScheduleState st;
        for (auto &sl : g_s)
            st.assignInstructorToSlot("DrA", sl);
        Scheduler sch(st, g_c, g_r, g_s);
        auto av = sch.getAvailableSlots(g_c.at("CS200"));
        ASSERT_VEC_EMPTY(av, "empty when instructor fully booked");
    }
    { // DrB unaffected by DrA's booking
        ScheduleState st;
        st.assignInstructorToSlot("DrA", "MWF-0800");
        Scheduler sch(st, g_c, g_r, g_s);
        auto av = sch.getAvailableSlots(g_c.at("MATH101")); // DrB
        ASSERT_VEC_CONTAINS(av, "MWF-0800", "MWF-0800 free for DrB");
        ASSERT_VEC_SIZE(av, (int)g_s.size(), "DrB has all slots");
    }
    // ------------------

    { // Different instructor unaffected by someone else's booking
        ScheduleState st;
        st.assignInstructorToSlot("DrA", "MWF-0800");
        Scheduler sch(st, g_c, g_r, g_s);
        auto av = sch.getAvailableSlots(g_c.at("MATH101")); // DrB
        ASSERT_VEC_CONTAINS(av, "MWF-0800", "slot remains available for different instructor");
        ASSERT_VEC_SIZE(av, (int)g_s.size(), "different instructor keeps all slots");
    }

    beginSuite("SCHEDULER -- getFeasibleAssignments");

    { // 2 rooms x 3 slots = 6
        ScheduleState st;
        vector<string> vec = {"S1", "S2", "S3"};
        Scheduler sch(st, g_c, g_one_r, vec);
        vector<string> ar = {"R1"};
        map<string, Room> r2 = {{"R1", {"R1", 50}}, {"R2", {"R2", 50}}};
        ScheduleState st2;
        Scheduler sch2(st2, g_c, r2, {"S1", "S2", "S3"});
        vector<string> ar2 = {"R1", "R2"}, as = {"S1", "S2", "S3"};
        auto cands = sch2.getFeasibleAssignments(g_c.at("CS200"), ar2, as);
        ASSERT_EQ((int)cands.size(), 6, "2 rooms x 3 slots = 6 candidates");
    }
    { // Occupied room filtered
        ScheduleState st;
        st.assignRoomToSlot("SBASSE-101", "MWF-0800");
        Scheduler sch(st, g_c, g_r, g_s);
        vector<string> ar = {"SBASSE-101"}, as = {"MWF-0800", "MWF-0930"};
        auto cands = sch.getFeasibleAssignments(g_c.at("CS200"), ar, as);
        ASSERT_EQ((int)cands.size(), 1, "only MWF-0930 valid after MWF-0800 occupied");
    }
    { // All occupied = empty
        ScheduleState st;
        st.assignRoomToSlot("SBASSE-101", "MWF-0800");
        st.assignRoomToSlot("SBASSE-101", "MWF-0930");
        Scheduler sch(st, g_c, g_r, g_s);
        vector<string> ar = {"SBASSE-101"}, as = {"MWF-0800", "MWF-0930"};
        auto cands = sch.getFeasibleAssignments(g_c.at("CS200"), ar, as);
        ASSERT_VEC_EMPTY(cands, "empty when all combinations occupied");
    }
    //----------------------------------------------------------------------------

    // ------------------------------

    { // Room already booked in slot is filtered from feasible assignments
        ScheduleState st;
        st.assignRoomToSlot("SBASSE-101", "MWF-0800");
        Scheduler sch(st, g_c, g_r, g_s);
        vector<string> ar = {"SBASSE-101", "SDSB-B3"};
        vector<string> as = {"MWF-0800"};
        auto cands = sch.getFeasibleAssignments(g_c.at("CS200"), ar, as);

        ASSERT_EQ((int)cands.size(), 1, "busy room/slot pair excluded from candidates");
        ASSERT_PAIR_EQ(cands[0], "SDSB-B3", "MWF-0800", "only free room remains feasible");
    }
    { // All candidate rooms booked in the slot -> no feasible assignment
        ScheduleState st;
        st.assignRoomToSlot("SBASSE-101", "MWF-0800");
        st.assignRoomToSlot("SDSB-B3", "MWF-0800");
        Scheduler sch(st, g_c, g_r, g_s);
        vector<string> ar = {"SBASSE-101", "SDSB-B3"};
        vector<string> as = {"MWF-0800"};
        auto cands = sch.getFeasibleAssignments(g_c.at("CS200"), ar, as);

        ASSERT_VEC_EMPTY(cands, "empty when every candidate room is already booked in slot");
    }

    beginSuite("SCHEDULER -- assignFirstAvailable");

    { // Picks first available
        ScheduleState st;
        Scheduler sch(st, g_best_c, g_best_r, g_one_s);
        vector<pair<string, string>> cands = {{"BIG", "S1"}, {"MED", "S1"}, {"EXACT", "S1"}};
        volatile bool ok = sch.assignFirstAvailable(g_best_c.at("C1"), cands);
        ASSERT_TRUE(ok, "assignFirstAvailable returns true");
        ASSERT_PAIR_EQ(st.getCourseAssignment("C1"), "BIG", "S1", "First available picks BIG");
    }
    { // State fully updated
        ScheduleState st;
        Scheduler sch(st, g_best_c, g_one_r, g_one_s);
        sch.assignFirstAvailable(g_best_c.at("C1"), {{"R1", "S1"}});
        ASSERT_TRUE(st.isCourseAssigned("C1"), "course assigned");
        ASSERT_FALSE(st.isRoomFreeInSlot("R1", "S1"), "room occupied");
        ASSERT_VEC_CONTAINS(st.getCoursesInSlot("S1"), "C1", "course in slot");
        ASSERT_VEC_CONTAINS(st.getInstructorSlots("I1"), "S1", "instructor booked");
    }
    { // Empty candidates
        ScheduleState st;
        Scheduler sch(st, g_best_c, g_one_r, g_one_s);
        volatile bool ok = sch.assignFirstAvailable(g_best_c.at("C1"), {});
        ASSERT_FALSE(ok, "returns false on empty candidates");
        ASSERT_FALSE(st.isCourseAssigned("C1"), "state unchanged");
    }

    beginSuite("SCHEDULER -- scheduleCourse");

    { // Returns true and assigns
        ScheduleState st;
        Scheduler sch(st, g_c, g_r, g_s);
        volatile bool ok = sch.scheduleCourse(g_c.at("CS200"));
        ASSERT_TRUE(ok, "scheduleCourse returns true");
        ASSERT_TRUE(st.isCourseAssigned("CS200"), "CS200 is assigned");
    }
    { // Room has sufficient capacity
        ScheduleState st;
        Scheduler sch(st, g_c, g_r, g_s);
        sch.scheduleCourse(g_c.at("CS200"));
        string room = st.getCourseAssignment("CS200").first;
        ASSERT_TRUE(g_r.count(room) > 0, "assigned room in catalog");
        ASSERT_TRUE(g_r.at(room).capacity >= g_c.at("CS200").enrollment,
                    "room has sufficient capacity");
    }
    {
        ScheduleState st;
        st.assignRoomToSlot("MED", "S1");
        st.assignRoomToSlot("MED", "S2");
        st.assignRoomToSlot("BIG", "S1");

        Scheduler sch(st, g_c, g_best_r, g_two_s);

        volatile bool ok = sch.scheduleCourse(g_c.at("CS200"));
        auto [room, slot] = st.getCourseAssignment("CS200");
        ASSERT_FALSE(room == "MED" && slot == "S1", "avoids busy MED/S1");
        ASSERT_FALSE(room == "MED" && slot == "S2", "avoids busy MED/S2");
        ASSERT_FALSE(room == "BIG" && slot == "S1", "avoids busy BIG/S1");
    }
    { // DrA teaches CS200 and CS300 -- must be different slots
        ScheduleState st;
        Scheduler sch(st, g_c, g_r, g_s);
        sch.scheduleCourse(g_c.at("CS200"));
        sch.scheduleCourse(g_c.at("CS300"));
        string s1 = st.getCourseAssignment("CS200").second;
        string s2 = st.getCourseAssignment("CS300").second;
        ASSERT_NEQ(s1, s2, "same-instructor courses in different slots");
    }
    { // No room fits -- returns false, state clean
        ScheduleState st;
        Scheduler sch(st, g_tiny_c, g_tiny_r, g_tiny_s);
        volatile bool ok = sch.scheduleCourse(g_tiny_c.at("BIG"));
        ASSERT_FALSE(ok, "returns false when no room fits");
        ASSERT_FALSE(st.isCourseAssigned("BIG"), "state unchanged on failure");
    }

    // --------------------------------------

    { // Room clash: already-booked room/slot is avoided
        ScheduleState st;
        st.assignRoomToSlot("SBASSE-101", "MWF-0800");
        Scheduler sch(st, g_c, g_r, g_s);

        volatile bool ok = sch.scheduleCourse(g_c.at("CS200"));
        ASSERT_TRUE(ok, "scheduleCourse succeeds despite one busy room/slot");
        ASSERT_TRUE(st.isCourseAssigned("CS200"), "course assigned");

        auto [room, slot] = st.getCourseAssignment("CS200");
        ASSERT_FALSE(room == "SBASSE-101" && slot == "MWF-0800",
                     "does not assign course into already-booked room/slot");
    }
    { // Room clash: if only candidate room in the slot is busy, that slot is not used
        ScheduleState st;
        st.assignRoomToSlot("SBASSE-101", "MWF-0800");
        vector<string> vec = {"MWF-0800", "MWF-0930"};
        Scheduler sch(st, g_c, g_r, vec);

        volatile bool ok = sch.scheduleCourse(g_c.at("CS200"));
        ASSERT_TRUE(ok, "scheduleCourse finds another valid assignment");
        ASSERT_TRUE(st.isCourseAssigned("CS200"), "course assigned");

        auto [room, slot] = st.getCourseAssignment("CS200");
        ASSERT_FALSE(room == "SBASSE-101" && slot == "MWF-0800",
                     "busy room/slot combination is avoided");
    }
    { // Room clash: fails when the only feasible room/slot is already occupied
        static map<string, Course> clash_c = {{"C1", {"C1", "I1", 40}}};
        static map<string, Room> clash_r = {{"R1", {"R1", 40}}, {"R2", {"R2", 39}}};
        static vector<string> clash_s = {"S1"};

        ScheduleState st;
        st.assignRoomToSlot("R1", "S1");
        Scheduler sch(st, clash_c, clash_r, clash_s);

        volatile bool ok = sch.scheduleCourse(clash_c.at("C1"));
        ASSERT_FALSE(ok, "returns false when only feasible room/slot is already occupied");
        ASSERT_FALSE(st.isCourseAssigned("C1"), "state unchanged on room-clash failure");
    }
    { // Instructor clash: busy instructor's slot is avoided
        ScheduleState st;
        st.assignInstructorToSlot("DrA", "MWF-0800");
        Scheduler sch(st, g_c, g_r, g_s);

        volatile bool ok = sch.scheduleCourse(g_c.at("CS200")); // DrA
        ASSERT_TRUE(ok, "scheduleCourse succeeds despite one busy instructor slot");
        ASSERT_TRUE(st.isCourseAssigned("CS200"), "course assigned");

        auto [room, slot] = st.getCourseAssignment("CS200");
        (void)room;
        ASSERT_NEQ(slot, string("MWF-0800"),
                   "does not assign instructor into an already-busy slot");
    }
    { // Instructor clash: same instructor's two courses must land in different slots
        ScheduleState st;
        Scheduler sch(st, g_c, g_r, g_s);

        volatile bool ok1 = sch.scheduleCourse(g_c.at("CS200")); // DrA
        volatile bool ok2 = sch.scheduleCourse(g_c.at("CS300")); // DrA

        ASSERT_TRUE(ok1, "first course scheduled");
        ASSERT_TRUE(ok2, "second course scheduled");

        string s1 = st.getCourseAssignment("CS200").second;
        string s2 = st.getCourseAssignment("CS300").second;
        ASSERT_NEQ(s1, s2, "same instructor not double-booked in same slot");
    }
    { // Instructor clash: fails when instructor's only slot is already occupied
        static map<string, Course> inst_c = {
            {"C1", {"C1", "I1", 10}}};
        static map<string, Room> inst_r = {
            {"R1", {"R1", 50}}};
        static vector<string> inst_s = {"S1"};

        ScheduleState st;
        st.assignInstructorToSlot("I1", "S1");
        Scheduler sch(st, inst_c, inst_r, inst_s);

        volatile bool ok = sch.scheduleCourse(inst_c.at("C1"));
        ASSERT_FALSE(ok, "returns false when instructor is busy in the only slot");
        ASSERT_FALSE(st.isCourseAssigned("C1"), "state unchanged on instructor-clash failure");
    }
    { // Instructor clash takes precedence even when room is free
        static map<string, Course> inst2_c = {
            {"C1", {"C1", "I1", 10}}};
        static map<string, Room> inst2_r = {
            {"R1", {"R1", 50}}};
        static vector<string> inst2_s = {"S1"};

        ScheduleState st;
        st.assignInstructorToSlot("I1", "S1");
        Scheduler sch(st, inst2_c, inst2_r, inst2_s);

        ASSERT_TRUE(st.isRoomFreeInSlot("R1", "S1"), "room starts free");
        volatile bool ok = sch.scheduleCourse(inst2_c.at("C1"));
        ASSERT_FALSE(ok, "still fails because instructor conflict blocks assignment");
        ASSERT_FALSE(st.isCourseAssigned("C1"), "course remains unassigned");
    }

    beginSuite("SCHEDULER -- buildInitialSchedule");

    { // All courses scheduled
        ScheduleState st;
        Scheduler sch(st, g_c, g_r, g_s);
        sch.buildInitialSchedule();
        int scheduled = 0;
        for (auto &[id, co] : g_c)
            if (st.isCourseAssigned(id))
                scheduled++;
        ASSERT_EQ(scheduled, (int)g_c.size(), "all courses scheduled");
    }
    { // State consistent
        ScheduleState st;
        Scheduler sch(st, g_c, g_r, g_s);
        sch.buildInitialSchedule();
        ASSERT_TRUE(st.isConsistent(g_c), "state consistent after buildInitialSchedule");
    }
    { // No room double-booking
        ScheduleState st;
        Scheduler sch(st, g_c, g_r, g_s);
        sch.buildInitialSchedule();
        volatile bool ok = true;
        auto assigned = st.getAllAssignedCourses();
        for (int i = 0; i < (int)assigned.size(); i++)
            for (int j = i + 1; j < (int)assigned.size(); j++)
            {
                auto [r1, s1] = st.getCourseAssignment(assigned[i]);
                auto [r2, s2] = st.getCourseAssignment(assigned[j]);
                if (r1 == r2 && s1 == s2)
                    ok = false;
            }
        ASSERT_TRUE(ok, "no room double-booking");
    }
    { // No instructor double-booking
        ScheduleState st;
        Scheduler sch(st, g_c, g_r, g_s);
        sch.buildInitialSchedule();
        // DrA teaches CS200 and CS300 -- different slots
        string s1 = st.getCourseAssignment("CS200").second;
        string s2 = st.getCourseAssignment("CS300").second;
        ASSERT_NEQ(s1, s2, "DrA's courses in different slots");
    }
    { // Only 2/3 scheduled when instructor has 2 slots for 3 courses
        ScheduleState st;
        Scheduler sch(st, g_busy_c, g_busy_r, g_two_s);
        sch.buildInitialSchedule();
        int n = 0;
        for (auto &[id, co] : g_busy_c)
            if (st.isCourseAssigned(id))
                n++;
        ASSERT_EQ(n, 2, "only 2/3 with same instructor and 2 slots");
    }

    beginSuite("SCHEDULER -- unscheduleCourse");

    { // All four maps cleared
        ScheduleState st;
        Scheduler sch(st, g_c, g_r, g_s);
        sch.scheduleCourse(g_c.at("CS200"));
        string room = st.getCourseAssignment("CS200").first;
        string slot = st.getCourseAssignment("CS200").second;
        sch.unscheduleCourse("CS200");
        ASSERT_FALSE(st.isCourseAssigned("CS200"), "unassigned");
        ASSERT_VEC_NOT_CONTAINS(st.getCoursesInSlot(slot), "CS200", "removed from slot");
        ASSERT_TRUE(st.isRoomFreeInSlot(room, slot), "room freed");
        ASSERT_VEC_NOT_CONTAINS(st.getInstructorSlots("DrA"), slot, "instructor freed");
    }
    { // Safe no-op
        ScheduleState st;
        Scheduler sch(st, g_c, g_r, g_s);
        sch.unscheduleCourse("CS200");
        ASSERT_FALSE(st.isCourseAssigned("CS200"), "safe no-op on unscheduled course");
    }

    beginSuite("SCHEDULER -- swapCourses");

    { // Successful swap
        ScheduleState st;
        Scheduler sch(st, g_swap_c, g_swap_r, g_swap_s);
        st.assign("C1", "I1", "R1", "S1");
        st.assign("C2", "I2", "R2", "S2");
        volatile bool ok = sch.swapCourses("C1", "C2");
        volatile bool c1r = (st.getCourseAssignment("C1").first == "R2");
        volatile bool c1s = (st.getCourseAssignment("C1").second == "S2");
        volatile bool c2r = (st.getCourseAssignment("C2").first == "R1");
        volatile bool c2s = (st.getCourseAssignment("C2").second == "S1");
        volatile bool cons = st.isConsistent(g_swap_c);
        ASSERT_TRUE(ok, "swap returns true");
        ASSERT_TRUE(c1r, "C1 room after swap == R2");
        ASSERT_TRUE(c1s, "C1 slot after swap == S2");
        ASSERT_TRUE(c2r, "C2 room after swap == R1");
        ASSERT_TRUE(c2s, "C2 slot after swap == S1");
        ASSERT_TRUE(cons, "state consistent after swap");
    }
    { // Capacity violation
        ScheduleState st;
        Scheduler sch(st, g_cap_c, g_cap_r, g_swap_s);
        st.assign("BIG", "I1", "R-BIG", "S1");
        st.assign("SMALL", "I2", "R-SMALL", "S2");
        volatile bool ok = sch.swapCourses("BIG", "SMALL");
        volatile bool bR = (st.getCourseAssignment("BIG").first == "R-BIG");
        volatile bool bS = (st.getCourseAssignment("BIG").second == "S1");
        volatile bool sR = (st.getCourseAssignment("SMALL").first == "R-SMALL");
        volatile bool sS = (st.getCourseAssignment("SMALL").second == "S2");
        ASSERT_FALSE(ok, "swap rejected on capacity violation");
        ASSERT_TRUE(bR, "BIG still in R-BIG");
        ASSERT_TRUE(bS, "BIG still in S1");
        ASSERT_TRUE(sR, "SMALL still in R-SMALL");
        ASSERT_TRUE(sS, "SMALL still in S2");
    }

    // Wrong Test Case, Removed:
    // { // Room clash at destination slot
    //     static map<string, Course> swap_room_c = {
    //         {"C1", {"C1", "I1", 30}},
    //         {"C2", {"C2", "I2", 30}},
    //         {"C3", {"C3", "I3", 30}}};
    //     static map<string, Room> swap_room_r = {
    //         {"R1", {"R1", 50}},
    //         {"R2", {"R2", 50}},
    //         {"R3", {"R3", 50}}};
    //     static vector<string> swap_room_s = {"S1", "S2"};

    //     ScheduleState st;
    //     Scheduler sch(st, swap_room_c, swap_room_r, swap_room_s);
    //     st.assign("C1", "I1", "R1", "S1");
    //     st.assign("C2", "I2", "R2", "S2");
    //     st.assign("C3", "I3", "R1", "S2"); // blocks C2 from moving into R1,S2

    //     volatile bool ok = sch.swapCourses("C1", "C2");

    //     volatile bool c1r = (st.getCourseAssignment("C1").first == "R1");
    //     volatile bool c1s = (st.getCourseAssignment("C1").second == "S1");
    //     volatile bool c2r = (st.getCourseAssignment("C2").first == "R2");
    //     volatile bool c2s = (st.getCourseAssignment("C2").second == "S2");

    //     ASSERT_FALSE(ok, "swap rejected when destination room/slot is already occupied");
    //     ASSERT_TRUE(c1r, "C1 still in R1");
    //     ASSERT_TRUE(c1s, "C1 still in S1");
    //     ASSERT_TRUE(c2r, "C2 still in R2");
    //     ASSERT_TRUE(c2s, "C2 still in S2");
    // }

    //----------------------------------------------------------------------------------

    { // Instructor clash at destination slot
        static map<string, Course> swap_inst_c = {
            {"C1", {"C1", "I1", 30}},
            {"C2", {"C2", "I2", 30}},
            {"C3", {"C3", "I1", 30}}};
        static map<string, Room> swap_inst_r = {
            {"R1", {"R1", 50}},
            {"R2", {"R2", 50}},
            {"R3", {"R3", 50}}};
        static vector<string> swap_inst_s = {"S1", "S2"};

        ScheduleState st;
        Scheduler sch(st, swap_inst_c, swap_inst_r, swap_inst_s);
        st.assign("C1", "I1", "R1", "S1");
        st.assign("C2", "I2", "R2", "S2");
        st.assign("C3", "I1", "R3", "S2"); // I1 already busy in S2, blocks C1 from moving to S2

        volatile bool ok = sch.swapCourses("C1", "C2");

        volatile bool c1r = (st.getCourseAssignment("C1").first == "R1");
        volatile bool c1s = (st.getCourseAssignment("C1").second == "S1");
        volatile bool c2r = (st.getCourseAssignment("C2").first == "R2");
        volatile bool c2s = (st.getCourseAssignment("C2").second == "S2");

        ASSERT_FALSE(ok, "swap rejected when instructor would be double-booked");
        ASSERT_TRUE(c1r, "C1 still in R1");
        ASSERT_TRUE(c1s, "C1 still in S1");
        ASSERT_TRUE(c2r, "C2 still in R2");
        ASSERT_TRUE(c2s, "C2 still in S2");
    }
}
