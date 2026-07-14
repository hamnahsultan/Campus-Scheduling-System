#include "test_framework.h"
#include <iostream>
using namespace std;

void test_loader();
void test_state();
void test_scheduler();
void test_score_engine();
void test_integration();

struct SR
{
    string name;
    int weight;
    int ran;
    int passed;
};
static int snapR, snapP;
static void snap()
{
    snapR = TESTS_RUN;
    snapP = TESTS_PASSED;
}
static SR diff(const string &n, int w)
{
    return {n, w, TESTS_RUN - snapR, TESTS_PASSED - snapP};
}

int main()
{
    installSignalHandlers();

    cout << "\n";
    cout << "############################################################\n";
    cout << "  CS200 PA3 -- Automated Test Suite\n";
    cout << "  The LUMS Campus Scheduler\n";
    cout << "############################################################\n";

    snap();
    test_loader();
    auto rL = diff("Module 1: Loader", 15);
    snap();
    test_state();
    auto rS = diff("Module 2: ScheduleState", 20);
    snap();
    test_scheduler();
    auto rSc = diff("Module 3: Scheduler", 30);
    snap();
    test_score_engine();
    auto rSE = diff("Module 4: ScoreEngine", 30);
    snap();
    test_integration();
    auto rI = diff("Integration / End-to-End", 5);

    printSummary();

    cout << "\n";
    cout << "============================================================\n";
    cout << "  WEIGHTED GRADING BREAKDOWN\n";
    cout << "============================================================\n";
    cout << "  Module                        Wt  Pass/Total  Score\n";
    cout << "  ----------------------------------------------------------\n";

    vector<SR> res = {rL, rS, rSc, rSE, rI};
    double earned = 0, total = 0;
    for (auto &r : res)
    {
        double pct = (r.ran > 0) ? (double)r.passed / r.ran : 0.0;
        double sc = pct * r.weight;
        earned += sc;
        total += r.weight;
        cout << "  " << r.name;
        int pad = 30 - (int)r.name.size();
        for (int i = 0; i < pad; i++)
            cout << " ";
        cout << r.weight << "   "
             << r.passed << "/" << r.ran
             << "    " << (int)(sc + 0.5) << "/" << r.weight << "\n";
    }
    cout << "  ----------------------------------------------------------\n";
    cout << "  TOTAL                         " << (int)total
         << "            " << (int)(earned + 0.5) << "/" << (int)total << "\n";
    cout << "============================================================\n\n";

    return (TESTS_FAILED == 0) ? 0 : 1;
}
