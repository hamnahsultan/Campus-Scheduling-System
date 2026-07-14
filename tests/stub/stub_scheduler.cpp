#include <cstdio>
void test_scheduler();
extern int TESTS_RUN, TESTS_PASSED, TESTS_FAILED, TESTS_CRASH;
void installSignalHandlers() {}
int main() {
    printf("\n=== MODULE: SCHEDULER ===\n");
    test_scheduler();
    printf("\nPassed: %d / %d  |  Failed (wrong): %d  |  Crashed: %d\n",
           TESTS_PASSED, TESTS_RUN,
           TESTS_FAILED - TESTS_CRASH, TESTS_CRASH);
    return TESTS_FAILED ? 1 : 0;
}
