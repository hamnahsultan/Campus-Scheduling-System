CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

# ── Solution source files ────────────────────────────────────────────────────
SRC = src/loader.cpp \
      src/schedule_state.cpp \
      src/scheduler.cpp \
      src/score_engine.cpp

# ── Shared test counter definitions (compiled once per target) ────────────────
GLOBALS = tests/test_globals.cpp

# ── All test files (for full suite) ─────────────────────────────────────────
TEST_ALL = tests/test_runner.cpp \
           tests/test_loader.cpp \
           tests/test_state.cpp \
           tests/test_scheduler.cpp \
           tests/test_score_engine.cpp \
           tests/test_integration.cpp

# ════════════════════════════════════════════════════════════════════════════
#  TARGETS
#
#  make test             -- run ALL tests + weighted grading table
#  make test-loader      -- Module 1 only
#  make test-state       -- Module 2 only
#  make test-scheduler   -- Module 3 only
#  make test-score       -- Module 4 only
#  make test-integration -- Integration only
#  make help             -- print this list
#  make clean            -- remove all binaries
# ════════════════════════════════════════════════════════════════════════════


test:
	$(CXX) $(CXXFLAGS) $(SRC) $(TEST_ALL) $(GLOBALS) -o run_tests
	./run_tests

test-loader:
	$(CXX) $(CXXFLAGS) $(SRC) $(GLOBALS) \
	    tests/test_loader.cpp tests/stub/stub_loader.cpp \
	    -o run_test_loader
	./run_test_loader

test-state:
	$(CXX) $(CXXFLAGS) $(SRC) $(GLOBALS) \
	    tests/test_state.cpp tests/stub/stub_state.cpp \
	    -o run_test_state
	./run_test_state

test-scheduler:
	$(CXX) $(CXXFLAGS) $(SRC) $(GLOBALS) \
	    tests/test_scheduler.cpp tests/stub/stub_scheduler.cpp \
	    -o run_test_scheduler
	./run_test_scheduler

test-score:
	$(CXX) $(CXXFLAGS) $(SRC) $(GLOBALS) \
	    tests/test_score_engine.cpp tests/stub/stub_score_engine.cpp \
	    -o run_test_score
	./run_test_score

test-integration:
	$(CXX) $(CXXFLAGS) $(SRC) $(GLOBALS) \
	    tests/test_integration.cpp tests/stub/stub_integration.cpp \
	    -o run_test_integration
	./run_test_integration

clean:
	rm -f scheduler run_tests \
	      run_test_loader run_test_state run_test_scheduler \
	      run_test_score run_test_integration

help:
	@echo ""
	@echo "  make test             Run ALL tests + weighted grading"
	@echo "  make test-loader      Module 1: Loader only"
	@echo "  make test-state       Module 2: ScheduleState only"
	@echo "  make test-scheduler   Module 3: Scheduler only"
	@echo "  make test-score       Module 4: ScoreEngine only"
	@echo "  make test-integration Integration / end-to-end only"
	@echo "  make clean            Remove all binaries"
	@echo ""
