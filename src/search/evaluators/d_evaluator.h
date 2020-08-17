#ifndef EVALUATORS_D_EVALUATOR_H
#define EVALUATORS_D_EVALUATOR_H

#include "../global_state.h"
#include "../global_operator.h"
#include "../heuristic.h"
#include "../option_parser.h"
#include "../plugin.h"

#include "../utils/system.h"

#include <limits>

using namespace std;

namespace d_evaluator {
static const int INFTY = numeric_limits<int>::max();

/*
  NOTE: This evaluator computes g values according to the transformed
  task. There are inherent problems with this because g values are
  path-dependent. See the discussion for issue980.

  Any specific behaviour requires cooperation from the search
  algorithm to trigger reopening/reevaluation at the right time, and
  it may also require changes to the way we use the heuristic cache.

  There are at least two conceptual problems:

  1. This implementation updates g values opportunistically at the
  time that the heuristic is notified about the existence of state
  transitions. For example, assume there exist transitions A => B => C
  => D and A => C such that A => B => C and A => C have different
  costs. Let's say that the given notification order first discovers
  the more expensive path and then notifies the algorithm about the
  edge C => D. Then the g-value for D will be based on this more
  expensive path to C, even if the cheaper path is later found and the
  g-value of C is updated. That is, updates to the g-value of a state
  are not propagated to its descendants that have already been
  considered.

  This is not a problem if notification happens in the correct order,
  for example, in the order in which the Dijkstra algorithm considers
  states, because then such a scenario can never happen. But it is
  something to be aware of in the general case.

  2. Like with every evaluator where the evaluation result changes
  over time, such changes cannot automatically propagate into the open
  list. It is up to the search algorithm to implement a policy on when
  and how to reconsider the value of evaluators that can change over
  time. The current solution is likely not satisfactory in general.
*/

/*
  NOTE: We derive from Heuristic rather than from Evaluator because
  Heuristic introduces task transformations and the heuristic cache,
  both of which we need for this. The class Heuristic is misnamed, and
  we should eventually address this.
*/

class DEvaluator : public Heuristic {
protected:
    virtual int compute_heuristic(const GlobalState &/*global_state*/) override {
        cerr << "PrototypeGEvaluator::compute_heuristic should never be called." << endl;
        utils::exit_with(utils::ExitCode::CRITICAL_ERROR);
    }

public:
    explicit DEvaluator(const Options &opts)
        : Heuristic(opts) {
        cout << "Initializing prototype g evaluator..." << endl;
        if (!cache_h_values) {
            cerr << "Estimate caching must not be disabled for prototype g evaluator."
                 << endl << "Terminating." << endl;
            utils::exit_with(utils::ExitCode::UNSUPPORTED);
        }
    }

    virtual void notify_initial_state(const GlobalState &initial_state) override {
        auto &cache_entry = heuristic_cache[initial_state];
        cache_entry.h = 0;
        cache_entry.dirty = false;
    }

    virtual bool notify_state_transition(const GlobalState &parent_state,
                                         const GlobalOperator &op,
                                         const GlobalState &state) override {
        assert(cache_h_values);
        int parent_g = heuristic_cache[parent_state].h;
        int op_no = get_op_index_hacked(&op);
        int op_cost = task_proxy.get_operators()[op_no].get_cost();
        int new_g = parent_g + op_cost;
        auto &cache_entry = heuristic_cache[state];
        int old_g =  cache_entry.h != NO_VALUE ? cache_entry.h : INFTY;
        if (new_g < old_g) {            
            cache_entry.h = new_g;
            cache_entry.dirty = false;
        }
        return true;
    }
};
}

#endif
