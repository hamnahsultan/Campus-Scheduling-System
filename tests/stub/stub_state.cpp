#include <cstdio>
void test_state();
extern int TESTS_RUN, TESTS_PASSED, TESTS_FAILED, TESTS_CRASH;
void installSignalHandlers() {}
int main() {
    printf("\n=== MODULE: STATE ===\n");
    test_state();
    printf("\nPassed: %d / %d  |  Failed (wrong): %d  |  Crashed: %d\n",
           TESTS_PASSED, TESTS_RUN,
           TESTS_FAILED - TESTS_CRASH, TESTS_CRASH);
    return TESTS_FAILED ? 1 : 0;
}
