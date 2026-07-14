#pragma once
#include <string>
#include <vector>
#include <map>
#include <climits>
#include <utility>
#include <iostream>
#include <algorithm>
#include "types.h"
#include "schedule_state.h"
#include "scheduler.h"

class ScoreEngine
{
public:
    ScoreEngine(ScheduleState &state, Scheduler &scheduler, const map<string, Course> &courses, const map<string, Room> &rooms, const vector<string> &slots);

    // --- Scoring (all return double, higher = worse) ---

    double computeUtilizationScore() const;
    double computeBalanceScore() const; // max-min slot load

    double computeTravelScore() const; // back-to-back different building
    double computeFinalScore() const;  // weighted sum

    // --- Problem identification ---
    string identifyWorstSlot() const;         // need
    string identifyUnderutilizedRoom() const; // need

    // --- One-pass targeted fixes ---
    // Each scans for ALL problems of that type and fixes them in one sweep.
    // Returns number of fixes applied.

    int fixBalance();   // moves courses out of overloaded slots
    int fixRoomUsage(); // reassigns oversized room placements
    int fixTravel();    // moves courses causing building-hop penalties

    // --- Master one-pass improvement ---
    // Calls all three fix functions once, in order.
    // May print a before/after score summary.

    void improveSchedule();

private:
    ScheduleState &state;
    Scheduler &scheduler;
    const map<string, Course> &courses;
    const map<string, Room> &rooms;
    const vector<string> &slots;
};
