#ifndef EVALUATORS_PROTOTYPE_G_EVALUATOR_H
#define EVALUATORS_PROTOTYPE_G_EVALUATOR_H

#include "../heuristic.h"

#include "../option_parser.h"

#include "../utils/logging.h"
#include "../utils/system.h"

#include <limits>

using namespace std;

namespace prototype_g_evaluator {

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

class PrototypeGEvaluator : public Heuristic {
protected:
    virtual int compute_heuristic(const State &/*gancestor_state*/) override {
        cerr << "PrototypeGEvaluator::compute_heuristic should never be called." << endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }

public:
    explicit PrototypeGEvaluator(const Options &opts)
        : Heuristic(opts) {
        utils::g_log << "Initializing prototype g evaluator..." << endl;
        if (!does_cache_estimates()) {
            cerr << "Estimate caching must not be disabled for prototype g evaluator."
                 << endl << "Terminating." << endl;
            utils::exit_with(utils::ExitCode::SEARCH_UNSUPPORTED);
        }
    }

    virtual void get_path_dependent_evaluators(std::set<Evaluator *> &evals) override;
    virtual void notify_initial_state(const State &initial_state) override;
    virtual void notify_state_transition(const State &parent_state,
                                         OperatorID op_id,
                                         const State &state) override;

};

}
#endif
