#include "search_statistics.h"

#include "globals.h"

#include "utils/timer.h"
#include "utils/system.h"

#include <iostream>

using namespace std;


SearchStatistics::SearchStatistics() {
    expanded_states = 0;
    reopened_states = 0;
    evaluated_states = 0;
    evaluations = 0;
    generated_states = 0;
    dead_end_states = 0;
    generated_ops = 0;

    lastjump_expanded_states = 0;
    lastjump_reopened_states = 0;
    lastjump_evaluated_states = 0;
    lastjump_generated_states = 0;
    last_jump_symmetrical_states_generated = 0;
    last_jump_symmetry_improved_evaluations = 0;
    last_jump_improving_symmetrical_states = 0;

    lastjump_f_value = -1;
}

void SearchStatistics::report_f_value_progress(int f) {
    if (f > lastjump_f_value) {
        lastjump_f_value = f;
        print_f_line();
        lastjump_expanded_states = expanded_states;
        lastjump_reopened_states = reopened_states;
        lastjump_evaluated_states = evaluated_states;
        lastjump_generated_states = generated_states;
        last_jump_symmetrical_states_generated = g_symmetrical_states_generated;
        last_jump_symmetry_improved_evaluations = g_symmetry_improved_evaluations;
        last_jump_improving_symmetrical_states = g_improving_symmetrical_states;
    }
}

void SearchStatistics::print_f_line() const {
    cout << "f = " << lastjump_f_value
         << " [";
    print_basic_statistics();
    cout << "]" << endl;
}

void SearchStatistics::print_basic_statistics() const {
    cout << evaluated_states << " evaluated, "
         << expanded_states << " expanded, ";
    if (reopened_states > 0) {
        cout << reopened_states << " reopened, ";
    }
    cout << "t=" << utils::g_timer;
    cout << ", " << utils::get_peak_memory_in_kb() << " KB";
}

void SearchStatistics::print_detailed_statistics() const {
    cout << "Expanded " << expanded_states << " state(s)." << endl;
    cout << "Reopened " << reopened_states << " state(s)." << endl;
    cout << "Evaluated " << evaluated_states << " state(s)." << endl;
    cout << "Evaluations: " << evaluations << endl;
    cout << "Generated " << generated_states << " state(s)." << endl;
    cout << "Dead ends: " << dead_end_states << " state(s)." << endl;
    cout << "Symmetrical states generated: " << g_symmetrical_states_generated << endl;
    cout << "Symmetry-improved evaluations: " << g_symmetry_improved_evaluations << endl;
    cout << "Improving symmetrical states: " << g_improving_symmetrical_states << endl;

    if (lastjump_f_value >= 0) {
        cout << "Expanded until last jump: "
             << lastjump_expanded_states << " state(s)." << endl;
        cout << "Reopened until last jump: "
             << lastjump_reopened_states << " state(s)." << endl;
        cout << "Evaluated until last jump: "
             << lastjump_evaluated_states << " state(s)." << endl;
        cout << "Generated until last jump: "
             << lastjump_generated_states << " state(s)." << endl;
        cout << "Symmetrical states generated until last jump: "
             << last_jump_symmetrical_states_generated << " state(s)." << endl;
        cout << "Symmetry-improved evaluations until last jump: "
             << last_jump_symmetry_improved_evaluations << endl;
        cout << "Improving symmetrical states until last jump: "
             << last_jump_improving_symmetrical_states << endl;
    }
}
