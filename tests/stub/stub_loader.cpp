#include <cstdio>
void test_loader();
extern int TESTS_RUN, TESTS_PASSED, TESTS_FAILED, TESTS_CRASH;
void installSignalHandlers() {}
int main() {
    printf("\n=== MODULE: LOADER ===\n");
    test_loader();
    printf("\nPassed: %d / %d  |  Failed (wrong): %d  |  Crashed: %d\n",
           TESTS_PASSED, TESTS_RUN,
           TESTS_FAILED - TESTS_CRASH, TESTS_CRASH);
    return TESTS_FAILED ? 1 : 0;
}
