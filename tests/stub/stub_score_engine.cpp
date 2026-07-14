#include <cstdio>
void test_score_engine();
extern int TESTS_RUN, TESTS_PASSED, TESTS_FAILED, TESTS_CRASH;
void installSignalHandlers() {}
int main() {
    printf("\n=== MODULE: SCORE_ENGINE ===\n");
    test_score_engine();
    printf("\nPassed: %d / %d  |  Failed (wrong): %d  |  Crashed: %d\n",
           TESTS_PASSED, TESTS_RUN,
           TESTS_FAILED - TESTS_CRASH, TESTS_CRASH);
    return TESTS_FAILED ? 1 : 0;
}
