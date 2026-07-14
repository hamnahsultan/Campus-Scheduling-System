#include "test_framework.h"
#include "../include/loader.h"
#include <fstream>
#include <cstdio>
using namespace std;

static void writeFile(const string& path, const string& content) {
    ofstream f(path); f << content;
}
static void rmFile(const string& path) { remove(path.c_str()); }

void test_loader() {

    beginSuite("LOADER -- split()");

    ASSERT_EQ(SAFE_IDX(split("CS200,DrAhmed,45",','),0,""), string("CS200"),   "split[0] == CS200");
    ASSERT_EQ(SAFE_IDX(split("CS200,DrAhmed,45",','),1,""), string("DrAhmed"), "split[1] == DrAhmed");
    ASSERT_EQ(SAFE_IDX(split("CS200,DrAhmed,45",','),2,""), string("45"),      "split[2] == 45");
    ASSERT_VEC_SIZE(split("CS200,DrAhmed,45",','), 3, "split 3-field line -> 3 tokens");

    ASSERT_VEC_SIZE(split("SBASSE-101",','), 1,  "no delimiter -> 1 token");
    ASSERT_EQ(SAFE_IDX(split("SBASSE-101",','),0,""), string("SBASSE-101"), "full string in token[0]");

    ASSERT_VEC_SIZE(split("",','), 1,  "empty string -> 1 token");
    ASSERT_EQ(SAFE_IDX(split("",','),0,"X"), string(""), "empty string token[0] is empty");

    ASSERT_VEC_SIZE(split("A,B,",','), 3, "trailing delimiter -> 3 tokens");
    ASSERT_EQ(SAFE_IDX(split("A,B,",','),2,"X"), string(""), "trailing empty token is empty");

    ASSERT_VEC_SIZE(split(",",','), 2, "single comma -> 2 tokens");
    ASSERT_EQ(SAFE_IDX(split(",",','),0,"X"), string(""), "first of two empty tokens");
    ASSERT_EQ(SAFE_IDX(split(",",','),1,"X"), string(""), "second of two empty tokens");

    ASSERT_EQ(SAFE_IDX(split("Dr Ahmed,CS 200",','),0,""), string("Dr Ahmed"), "spaces preserved in token");
    ASSERT_EQ(SAFE_IDX(split("A|B|C",'|'),1,""), string("B"), "pipe delimiter works");

    beginSuite("LOADER -- parseCourse()");

    ASSERT_EQ(parseCourse("CS200,DrAhmed,45").id,         string("CS200"),   "parseCourse id");
    ASSERT_EQ(parseCourse("CS200,DrAhmed,45").instructor, string("DrAhmed"), "parseCourse instructor");
    ASSERT_EQ(parseCourse("CS200,DrAhmed,45").enrollment, 45,                "parseCourse enrollment");
    ASSERT_EQ(parseCourse("MATH101,DrKhan,350").enrollment, 350, "large enrollment parsed");
    ASSERT_EQ(parseCourse("CS499,DrX,1").enrollment, 1,          "enrollment of 1 parsed");
    ASSERT_EQ(parseCourse("BIO101,Dr Ali,30").instructor, string("Dr Ali"), "instructor with space");

    beginSuite("LOADER -- parseRoom()");

    ASSERT_EQ(parseRoom("SBASSE-101,50").id,       string("SBASSE-101"), "parseRoom id");
    ASSERT_EQ(parseRoom("SBASSE-101,50").capacity, 50,                   "parseRoom capacity");
    ASSERT_EQ(parseRoom("SDSB-HALL,500").capacity, 500, "large capacity");
    ASSERT_EQ(parseRoom("TINY-001,1").capacity,    1,   "capacity of 1");

    beginSuite("LOADER -- parseSlot()");

    ASSERT_EQ(parseSlot("MWF-0930"),       string("MWF-0930"), "basic slot");
    ASSERT_EQ(parseSlot("TTh-1400\n"),     string("TTh-1400"), "strips \\n");
    ASSERT_EQ(parseSlot("MWF-0800\r\n"),   string("MWF-0800"), "strips \\r\\n");
    ASSERT_EQ(parseSlot("TTh-1100   "),    string("TTh-1100"), "strips trailing spaces");

    beginSuite("LOADER -- loadCourses()");

    writeFile("/tmp/tc.csv",
        "id,instructor,enrollment\nCS200,DrAhmed,45\nMATH101,DrKhan,80\nBIO101,DrAli,25\n");
    {
        auto courses = loadCourses("/tmp/tc.csv");
        ASSERT_VEC_SIZE(courses, 3,                    "3 courses loaded (header skipped)");
        ASSERT_EQ(SAFE_IDX(courses,0,Course{}).id,         string("CS200"),   "courses[0].id");
        ASSERT_EQ(SAFE_IDX(courses,0,Course{}).instructor, string("DrAhmed"), "courses[0].instructor");
        ASSERT_EQ(SAFE_IDX(courses,0,Course{}).enrollment, 45,                "courses[0].enrollment");
        ASSERT_EQ(SAFE_IDX(courses,1,Course{}).id,         string("MATH101"), "courses[1].id");
        ASSERT_EQ(SAFE_IDX(courses,2,Course{}).enrollment, 25,                "courses[2].enrollment");
    }
    rmFile("/tmp/tc.csv");

    writeFile("/tmp/tc2.csv", "id,instructor,enrollment\nC1,I1,10\nC2,I2,20\nC3,I3,30\nC4,I4,40\n");
    {
        auto courses = loadCourses("/tmp/tc2.csv");
        ASSERT_EQ(SAFE_IDX(courses,0,Course{}).id, string("C1"), "file order preserved: C1 first");
        ASSERT_EQ(SAFE_IDX(courses,3,Course{}).id, string("C4"), "file order preserved: C4 last");
    }
    rmFile("/tmp/tc2.csv");

    beginSuite("LOADER -- loadRooms()");

    writeFile("/tmp/tr.csv", "id,capacity\nSBASSE-101,50\nSDSB-B3,70\nSSE-201,90\n");
    {
        auto rooms = loadRooms("/tmp/tr.csv");
        ASSERT_EQ((int)rooms.size(), 3,                        "3 rooms loaded");
        ASSERT_TRUE(rooms.count("SBASSE-101") > 0,             "SBASSE-101 in map");
        ASSERT_EQ(rooms["SBASSE-101"].capacity, 50,            "SBASSE-101 capacity");
        ASSERT_TRUE(rooms.count("SDSB-B3") > 0,               "SDSB-B3 in map");
        ASSERT_EQ(rooms["SDSB-B3"].capacity,   70,             "SDSB-B3 capacity");
        ASSERT_EQ(rooms["SSE-201"].id, string("SSE-201"),      "id field preserved in map value");
    }
    rmFile("/tmp/tr.csv");

    writeFile("/tmp/tr2.csv", "id,capacity\nR1,100\nR2,200\n");
    {
        auto rooms = loadRooms("/tmp/tr2.csv");
        ASSERT_EQ(rooms.at("R2").capacity, 200, "direct .at() lookup");
    }
    rmFile("/tmp/tr2.csv");

    beginSuite("LOADER -- loadSlots()");

    writeFile("/tmp/ts.csv", "MWF-0800\nMWF-0930\nTTh-1100\n");
    {
        auto slots = loadSlots("/tmp/ts.csv");
        ASSERT_VEC_SIZE(slots, 3,                          "3 slots loaded");
        ASSERT_EQ(SAFE_IDX(slots,0,""), string("MWF-0800"), "slot[0] correct");
        ASSERT_EQ(SAFE_IDX(slots,1,""), string("MWF-0930"), "slot[1] correct");
        ASSERT_EQ(SAFE_IDX(slots,2,""), string("TTh-1100"), "slot[2] correct");
    }
    rmFile("/tmp/ts.csv");

    beginSuite("LOADER -- buildCourseMap()");

    {
        vector<Course> list = {{"CS200","DrA",40},{"MATH101","DrB",80},{"BIO101","DrC",20}};
        auto cm = buildCourseMap(list);
        ASSERT_EQ((int)cm.size(), 3,                            "map has 3 entries");
        ASSERT_TRUE(cm.count("CS200") > 0,                      "CS200 in map");
        ASSERT_TRUE(cm.count("MATH101") > 0,                    "MATH101 in map");
        ASSERT_EQ(cm["CS200"].enrollment,   40,                 "enrollment preserved");
        ASSERT_EQ(cm["MATH101"].instructor, string("DrB"),      "instructor preserved");
    }
    {
        vector<Course> list = {{"CS200","DrA",40},{"CS200","DrB",50}};
        auto cm = buildCourseMap(list);
        ASSERT_EQ((int)cm.size(), 1, "duplicate id reduces to 1 entry");
    }
}
