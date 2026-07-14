#include "test_framework.h"
#include "../include/schedule_state.h"
using namespace std;

void test_state()
{

    // ============================================================
    beginSuite("STATE -- addCourseToSlot / getCoursesInSlot");
    // ============================================================

    ASSERT_VEC_SIZE(ScheduleState().getCoursesInSlot("GHOST"), 0,
                    "empty state: slot returns empty vector");

    {
        ScheduleState s;
        s.addCourseToSlot("CS200", "S1");
        ASSERT_VEC_CONTAINS(s.getCoursesInSlot("S1"), "CS200", "addCourseToSlot: course appears in slot");
        ASSERT_VEC_SIZE(s.getCoursesInSlot("S1"), 1, "addCourseToSlot: exactly 1 course");
    }
    {
        ScheduleState s;
        s.addCourseToSlot("C1", "S1");
        s.addCourseToSlot("C2", "S1");
        s.addCourseToSlot("C3", "S1");
        ASSERT_VEC_SIZE(s.getCoursesInSlot("S1"), 3, "addCourseToSlot: 3 courses in same slot");
        ASSERT_VEC_CONTAINS(s.getCoursesInSlot("S1"), "C1", "C1 in slot");
        ASSERT_VEC_CONTAINS(s.getCoursesInSlot("S1"), "C2", "C2 in slot");
        ASSERT_VEC_CONTAINS(s.getCoursesInSlot("S1"), "C3", "C3 in slot");
    }
    {
        ScheduleState s;
        s.addCourseToSlot("C1", "S1");
        s.addCourseToSlot("C1", "S2");
        ASSERT_VEC_SIZE(s.getCoursesInSlot("S1"), 1, "same course in different slots is independent");
        ASSERT_VEC_SIZE(s.getCoursesInSlot("S2"), 1, "same course in S2 independent");
    }

    // ============================================================
    beginSuite("STATE -- removeCourseFromSlot");
    // ============================================================

    {
        ScheduleState s;
        s.addCourseToSlot("C1", "S1");
        s.addCourseToSlot("C2", "S1");
        s.removeCourseFromSlot("C1", "S1");
        ASSERT_VEC_NOT_CONTAINS(s.getCoursesInSlot("S1"), "C1", "C1 removed");
        ASSERT_VEC_CONTAINS(s.getCoursesInSlot("S1"), "C2", "C2 still there");
        ASSERT_VEC_SIZE(s.getCoursesInSlot("S1"), 1, "1 course remains");
    }
    {
        ScheduleState s;
        s.addCourseToSlot("C1", "S1");
        s.removeCourseFromSlot("GHOST", "S1");
        ASSERT_VEC_SIZE(s.getCoursesInSlot("S1"), 1, "remove non-existent is safe");
    }
    {
        ScheduleState s;
        s.removeCourseFromSlot("C1", "EMPTY"); // must not crash
        ASSERT_VEC_EMPTY(s.getCoursesInSlot("EMPTY"), "remove from empty slot is safe");
    }
    {
        ScheduleState s;
        s.addCourseToSlot("C1", "S1");
        s.removeCourseFromSlot("C1", "S1");
        ASSERT_VEC_EMPTY(s.getCoursesInSlot("S1"), "slot empty after removing last course");
    }

    // ============================================================
    beginSuite("STATE -- moveCourseBetweenSlots");
    // ============================================================

    {
        ScheduleState s;
        s.addCourseToSlot("C1", "S1");
        s.moveCourseBetweenSlots("C1", "S1", "S2");
        ASSERT_VEC_CONTAINS(s.getCoursesInSlot("S2"), "C1", "course in new slot after move");
        ASSERT_VEC_NOT_CONTAINS(s.getCoursesInSlot("S1"), "C1", "course absent from old slot");
    }
    {
        ScheduleState s;
        s.addCourseToSlot("C1", "S1");
        s.addCourseToSlot("C2", "S1");
        s.moveCourseBetweenSlots("C1", "S1", "S2");
        ASSERT_VEC_CONTAINS(s.getCoursesInSlot("S1"), "C2", "other course unaffected by move");
        ASSERT_VEC_SIZE(s.getCoursesInSlot("S1"), 1, "old slot has 1 course remaining");
    }

    // ============================================================
    beginSuite("STATE -- clearSlot");
    // ============================================================

    {
        ScheduleState s;
        s.addCourseToSlot("C1", "S1");
        s.addCourseToSlot("C2", "S1");
        s.addCourseToSlot("C3", "S1");
        s.clearSlot("S1");
        ASSERT_VEC_EMPTY(s.getCoursesInSlot("S1"), "slot empty after clearSlot");
    }
    {
        ScheduleState s;
        s.addCourseToSlot("C1", "S1");
        s.addCourseToSlot("C2", "S2");
        s.clearSlot("S1");
        ASSERT_VEC_CONTAINS(s.getCoursesInSlot("S2"), "C2", "other slot unaffected by clearSlot");
    }

    // ============================================================
    beginSuite("STATE -- Instructor Operations");
    // ============================================================

    ASSERT_VEC_EMPTY(ScheduleState().getInstructorSlots("Ghost"), "unknown instructor returns empty");
    ASSERT_EQ(ScheduleState().getInstructorLoad("Ghost"), 0, "unknown instructor load == 0");

    {
        ScheduleState s;
        s.assignInstructorToSlot("DrA", "S1");
        ASSERT_VEC_CONTAINS(s.getInstructorSlots("DrA"), "S1", "slot in instructor list");
        ASSERT_EQ(s.getInstructorLoad("DrA"), 1, "load == 1");
    }
    {
        ScheduleState s;
        s.assignInstructorToSlot("DrA", "S1");
        s.assignInstructorToSlot("DrA", "S2");
        s.assignInstructorToSlot("DrA", "S3");
        ASSERT_EQ(s.getInstructorLoad("DrA"), 3, "load == 3 after 3 assignments");
        ASSERT_VEC_CONTAINS(s.getInstructorSlots("DrA"), "S1", "S1 in list");
        ASSERT_VEC_CONTAINS(s.getInstructorSlots("DrA"), "S3", "S3 in list");
    }
    {
        ScheduleState s;
        s.assignInstructorToSlot("DrA", "S1");
        s.assignInstructorToSlot("DrA", "S2");
        s.removeInstructorFromSlot("DrA", "S1");
        ASSERT_VEC_NOT_CONTAINS(s.getInstructorSlots("DrA"), "S1", "S1 removed");
        ASSERT_VEC_CONTAINS(s.getInstructorSlots("DrA"), "S2", "S2 still present");
        ASSERT_EQ(s.getInstructorLoad("DrA"), 1, "load decremented to 1");
    }
    {
        ScheduleState s;
        s.assignInstructorToSlot("DrA", "S1");
        s.removeInstructorFromSlot("DrA", "GHOST");
        ASSERT_EQ(s.getInstructorLoad("DrA"), 1, "remove ghost slot is safe");
    }

    // ============================================================
    beginSuite("STATE -- Room Operations");
    // ============================================================

    ASSERT_TRUE(ScheduleState().isRoomFreeInSlot("R1", "S1"), "room free before any assignment");
    ASSERT_EQ(ScheduleState().getRoomLoad("GHOST"), 0, "unknown room load == 0");

    {
        ScheduleState s;
        s.assignRoomToSlot("R1", "S1");
        ASSERT_FALSE(s.isRoomFreeInSlot("R1", "S1"), "room not free after assignment");
        ASSERT_TRUE(s.isRoomFreeInSlot("R1", "S2"), "room free in different slot");
        ASSERT_EQ(s.getRoomLoad("R1"), 1, "room load == 1");
        ASSERT_VEC_CONTAINS(s.getRoomSlots("R1"), "S1", "S1 in room slots");
    }
    {
        ScheduleState s;
        s.assignRoomToSlot("R1", "S1");
        s.assignRoomToSlot("R1", "S2");
        s.assignRoomToSlot("R1", "S3");
        ASSERT_EQ(s.getRoomLoad("R1"), 3, "room load == 3");
    }
    {
        ScheduleState s;
        s.assignRoomToSlot("R1", "S1");
        s.removeRoomFromSlot("R1", "S1");
        ASSERT_TRUE(s.isRoomFreeInSlot("R1", "S1"), "room free after remove");
        ASSERT_EQ(s.getRoomLoad("R1"), 0, "room load back to 0");
    }

    // ============================================================
    beginSuite("STATE -- Course Assignment Operations");
    // ============================================================

    ASSERT_FALSE(ScheduleState().isCourseAssigned("C1"), "course not assigned in fresh state");
    {
        ScheduleState _tmp;
        auto _p = _tmp.getCourseAssignment("GHOST");
        ASSERT_EQ(_p.first, string(""), "unassigned course pair.first is empty");
        ASSERT_EQ(_p.second, string(""), "unassigned course pair.second is empty");
    }

    {
        ScheduleState s;
        s.assignCourse("C1", "R1", "S1");
        ASSERT_TRUE(s.isCourseAssigned("C1"), "course marked assigned");
        ASSERT_PAIR_EQ(s.getCourseAssignment("C1"), "R1", "S1", "assignment pair correct");
    }
    {
        ScheduleState s;
        s.assignCourse("C1", "R1", "S1");
        s.assignCourse("C1", "R2", "S2"); // overwrite
        ASSERT_PAIR_EQ(s.getCourseAssignment("C1"), "R2", "S2", "overwrite updates pair");
    }
    {
        ScheduleState s;
        s.assignCourse("C1", "R1", "S1");
        s.unassignCourse("C1");
        ASSERT_FALSE(s.isCourseAssigned("C1"), "unassigned after unassign");
        ASSERT_PAIR_EQ(s.getCourseAssignment("C1"), "", "", "empty pair after unassign");
    }
    {
        ScheduleState s;
        s.unassignCourse("GHOST"); // must not crash
        ASSERT_FALSE(s.isCourseAssigned("GHOST"), "unassign ghost is safe");
    }

    // ============================================================
    beginSuite("STATE -- assign() full-state");
    // ============================================================

    {
        ScheduleState s;
        volatile bool ok = s.assign("C1", "I1", "R1", "S1");
        ASSERT_TRUE(ok, "assign returns true on valid input");
        ASSERT_VEC_CONTAINS(s.getCoursesInSlot("S1"), "C1", "slotToCourses updated");
        ASSERT_VEC_CONTAINS(s.getInstructorSlots("I1"), "S1", "instructorToSlots updated");
        ASSERT_FALSE(s.isRoomFreeInSlot("R1", "S1"), "roomToSlots updated");
        ASSERT_TRUE(s.isCourseAssigned("C1"), "courseAssignment updated");
        ASSERT_PAIR_EQ(s.getCourseAssignment("C1"), "R1", "S1", "assignment pair correct");
    }
    {
        ScheduleState s;
        ASSERT_FALSE(s.assign("", "I1", "R1", "S1"), "empty course -> false");
        ASSERT_FALSE(s.assign("C1", "", "R1", "S1"), "empty instructor -> false");
        ASSERT_FALSE(s.assign("C1", "I1", "", "S1"), "empty room -> false");
        ASSERT_FALSE(s.assign("C1", "I1", "R1", ""), "empty slot -> false");
    }

    // ============================================================
    beginSuite("STATE -- remove() full-state");
    // ============================================================

    {
        ScheduleState s;
        s.assign("C1", "I1", "R1", "S1");
        s.assign("C2", "I2", "R2", "S1");
        s.remove("C1", "I1", "R1", "S1");
        ASSERT_VEC_NOT_CONTAINS(s.getCoursesInSlot("S1"), "C1", "C1 removed from slot");
        ASSERT_VEC_NOT_CONTAINS(s.getInstructorSlots("I1"), "S1", "I1 freed from slot");
        ASSERT_TRUE(s.isRoomFreeInSlot("R1", "S1"), "R1 freed");
        ASSERT_FALSE(s.isCourseAssigned("C1"), "C1 unassigned");
        ASSERT_VEC_CONTAINS(s.getCoursesInSlot("S1"), "C2", "C2 still in slot");
    }
    {
        ScheduleState s;
        s.assign("C1", "I1", "R1", "S1");
        s.assign("C2", "I1", "R2", "S2");
        s.remove("C1", "I1", "R1", "S1");
        ASSERT_VEC_CONTAINS(s.getInstructorSlots("I1"), "S2", "instructor other slot unaffected");
        ASSERT_EQ(s.getInstructorLoad("I1"), 1, "instructor load decremented");
    }

    // ============================================================
    beginSuite("STATE -- move() full-state");
    // ============================================================

    {
        ScheduleState s;
        s.assign("C1", "I1", "R1", "S1");
        s.move("C1", "I1", "R1", "S1", "R2", "S2");
        ASSERT_PAIR_EQ(s.getCourseAssignment("C1"), "R2", "S2", "course has new assignment");
        ASSERT_VEC_CONTAINS(s.getCoursesInSlot("S2"), "C1", "in new slot");
        ASSERT_VEC_CONTAINS(s.getInstructorSlots("I1"), "S2", "instructor in new slot");
        ASSERT_FALSE(s.isRoomFreeInSlot("R2", "S2"), "new room occupied");
        ASSERT_VEC_NOT_CONTAINS(s.getCoursesInSlot("S1"), "C1", "gone from old slot");
        ASSERT_TRUE(s.isRoomFreeInSlot("R1", "S1"), "old room freed");
        ASSERT_VEC_NOT_CONTAINS(s.getInstructorSlots("I1"), "S1", "instructor freed from old slot");
    }

    // ============================================================
    beginSuite("STATE -- isConsistent()");
    // ============================================================

    {
        ScheduleState s;
        map<string, Course> courses;
        ASSERT_TRUE(s.isConsistent(courses), "empty state is consistent");
    }
    {
        ScheduleState s;
        map<string, Course> courses = {{"C1", {"C1", "I1", 45}}};
        s.assign("C1", "I1", "R1", "S1");
        ASSERT_TRUE(s.isConsistent(courses), "consistent after clean assign");
    }
    {
        ScheduleState s;
        map<string, Course> courses = {{"C1", {"C1", "I1", 45}}};
        s.assign("C1", "I1", "R1", "S1");
        s.remove("C1", "I1", "R1", "S1");
        ASSERT_TRUE(s.isConsistent(courses), "consistent after assign+remove");
    }
    {
        ScheduleState s;
        map<string, Course> courses = {{"C1", {"C1", "I1", 45}}};
        // Partial manual update -- deliberately inconsistent
        s.assignCourse("C1", "R1", "S1"); // only courseAssignment updated
        ASSERT_FALSE(s.isConsistent(courses), "inconsistent: slotToCourses missing");
    }
    {
        ScheduleState s;
        map<string, Course> courses = {{"C1", {"C1", "I1", 45}}};
        s.assignCourse("C1", "R1", "S1");
        s.addCourseToSlot("C1", "S1");
        s.assignRoomToSlot("R1", "S1");
        // instructorToSlots NOT updated
        ASSERT_FALSE(s.isConsistent(courses), "inconsistent: instructorToSlots missing");
    }
    {
        ScheduleState s;
        map<string, Course> courses = {
            {"C1", {"C1", "I1", 45}},
            {"C2", {"C2", "I2", 80}},
            {"C3", {"C3", "I3", 25}}};
        s.assign("C1", "I1", "R1", "S1");
        s.assign("C2", "I2", "R2", "S2");
        s.assign("C3", "I3", "R3", "S3");
        ASSERT_TRUE(s.isConsistent(courses), "consistent for 3 independent assigns");
    }

    // ============================================================
    beginSuite("STATE -- getAllInstructors / getAllAssignedCourses");
    // ============================================================

    {
        ScheduleState s;
        s.assign("C1", "I1", "R1", "S1");
        s.assign("C2", "I2", "R2", "S2");
        auto instrs = s.getAllInstructors();
        ASSERT_VEC_CONTAINS(instrs, "I1", "I1 in getAllInstructors");
        ASSERT_VEC_CONTAINS(instrs, "I2", "I2 in getAllInstructors");
    }
    {
        ScheduleState s;
        s.assign("C1", "I1", "R1", "S1");
        s.assign("C2", "I2", "R2", "S2");
        auto assigned = s.getAllAssignedCourses();
        ASSERT_VEC_CONTAINS(assigned, "C1", "C1 in getAllAssignedCourses");
        ASSERT_VEC_CONTAINS(assigned, "C2", "C2 in getAllAssignedCourses");
        ASSERT_VEC_SIZE(assigned, 2, "exactly 2 assigned courses");
    }
    {
        ScheduleState s;
        s.assign("C1", "I1", "R1", "S1");
        s.assign("C2", "I2", "R2", "S2");
        s.remove("C1", "I1", "R1", "S1");
        auto assigned = s.getAllAssignedCourses();
        ASSERT_VEC_NOT_CONTAINS(assigned, "C1", "C1 gone after remove");
        ASSERT_VEC_CONTAINS(assigned, "C2", "C2 still present");
        ASSERT_VEC_SIZE(assigned, 1, "1 course remaining");
    }
}
