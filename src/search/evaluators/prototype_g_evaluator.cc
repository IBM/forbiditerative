#include "prototype_g_evaluator.h"

#include "../heuristic.h"

#include "../option_parser.h"
#include "../plugin.h"


namespace prototype_g_evaluator {

void PrototypeGEvaluator::get_path_dependent_evaluators(std::set<Evaluator *> &evals) {
    evals.insert(this);
}

void PrototypeGEvaluator::notify_initial_state(const State &initial_state) {
    auto &cache_entry = heuristic_cache[initial_state];
    cache_entry.h = 0;
    cache_entry.dirty = false;
}

void PrototypeGEvaluator::notify_state_transition(const State &parent_state,
                                         OperatorID op_id,
                                         const State &state) {
    assert(is_estimate_cached(parent_state));
    int parent_g = get_cached_estimate(parent_state);
    /*
      NOTE: We have to be careful here if we apply task
      transformations that change the meaning of the operator IDs.
      But this applies to all notify_... methods.
    */
    int op_cost = task_proxy.get_operators()[op_id].get_cost();
    int new_g = parent_g + op_cost;
    bool is_previously_reached_state = is_estimate_cached(state);
    int old_g = is_previously_reached_state ? get_cached_estimate(state) : INFTY;
    if (new_g < old_g) {
        auto &cache_entry = heuristic_cache[state];
        cache_entry.h = new_g;
        cache_entry.dirty = false;
    }
}

static shared_ptr<Evaluator> _parse(OptionParser &parser) {
    parser.document_synopsis(
        "prototype g-value evaluator",
        "Prototype code -- no documentation.");
    Heuristic::add_options_to_parser(parser);
    Options opts = parser.parse();
    if (parser.dry_run())
        return nullptr;
    else
        return make_shared<PrototypeGEvaluator>(opts);
}

static Plugin<Evaluator> _plugin("prototype_g", _parse, "evaluators_basic");
}