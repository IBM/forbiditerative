#include "ce_lm_cut_landmarks.h"

#include "../task_utils/task_properties.h"

#include <algorithm>
#include <limits>
#include <utility>

using namespace std;

namespace ce_lm_cut_heuristic {
// construction and destruction
CELandmarkCutLandmarks::CELandmarkCutLandmarks(const TaskProxy &task_proxy) {
    task_properties::verify_no_axioms(task_proxy);

    // Build propositions.
    num_propositions = 2; // artificial goal and artificial precondition
    VariablesProxy variables = task_proxy.get_variables();
    propositions.resize(variables.size());
    for (FactProxy fact : variables.get_facts()) {
        int var_id = fact.get_variable().get_id();
        propositions[var_id].push_back(RelaxedProposition());
        ++num_propositions;
    }

    // Build relaxed operators for operators and axioms.
    relaxed_operator_groups.reserve(task_proxy.get_operators().size() + 1);
    for (OperatorProxy op : task_proxy.get_operators()) {
        relaxed_operator_groups.push_back(RelaxedOperatorGroup(op.get_id(), op.get_cost()));
    }
    // Add group for artificial goal operator.
    // This must be added before groups are used to keep pointers stable.
    relaxed_operator_groups.push_back(RelaxedOperatorGroup(0, 0));
    for (size_t i = 0; i < task_proxy.get_operators().size(); ++i) {
        OperatorProxy op = task_proxy.get_operators()[i];
        build_relaxed_operator(op, relaxed_operator_groups[i]);
    }

    // Simplify relaxed operators.
    // simplify();
    /* TODO: Put this back in and test if it makes sense,
       but only after trying out whether and how much the change to
       unary operators hurts. */

    // Build artificial goal proposition and operator.
    vector<RelaxedProposition *> goal_op_pre, goal_op_eff;
    for (FactProxy goal : task_proxy.get_goals()) {
        goal_op_pre.push_back(get_proposition(goal));
    }
    goal_op_eff.push_back(&artificial_goal);
    /* Use the invalid operator ID -1 so accessing
       the artificial operator will generate an error. */
//    add_relaxed_operator(move(goal_op_pre), move(goal_op_eff), -1, 0);
    add_relaxed_operator(move(goal_op_pre), move(goal_op_eff),
                         relaxed_operator_groups[task_proxy.get_operators().size()]);

    // Cross-reference relaxed operators.
    for (RelaxedOperatorGroup &group : relaxed_operator_groups) {
        for (RelaxedOperator &op : group.relaxed_operators) {
            for (RelaxedProposition *pre : op.preconditions)
                pre->precondition_of.push_back(&op);
            for (RelaxedProposition *eff : op.effects)
                eff->effect_of.push_back(&op);
        }
    }
}

CELandmarkCutLandmarks::~CELandmarkCutLandmarks() {
}

void CELandmarkCutLandmarks::build_relaxed_operator(
    const OperatorProxy &op, RelaxedOperatorGroup &group) {
    vector<RelaxedProposition *> precondition;
    vector<RelaxedProposition *> effects;
    for (FactProxy pre : op.get_preconditions()) {
        precondition.push_back(get_proposition(pre));
    }
    for (EffectProxy eff : op.get_effects()) {
        if (eff.get_conditions().empty()) {
            effects.push_back(get_proposition(eff.get_fact()));
        }
    }
    if (!effects.empty()) {
        vector<RelaxedProposition *> precondition_copy(precondition);
        add_relaxed_operator(
            move(precondition_copy), move(effects), group);
    }
    for (EffectProxy eff : op.get_effects()) {
        if (!eff.get_conditions().empty()) {
            vector<RelaxedProposition *> cond_precondition(precondition);
            vector<RelaxedProposition *> cond_effects;
            for (FactProxy effect_cond : eff.get_conditions()) {
                cond_precondition.push_back(get_proposition(effect_cond));
            }
            cond_effects.push_back(get_proposition(eff.get_fact()));
            // TODO: If it's worth grouping together effects that have no effect
            // condition, then it's probably also worth otherwise grouping together
            // effects that have the same effect condition. It would require copying
            // the operator representation in some form and then sorting based on the
            // preconditions.
            add_relaxed_operator(move(cond_precondition), move(cond_effects), group);
        }
    }
}

void CELandmarkCutLandmarks::add_relaxed_operator(
    vector<RelaxedProposition *> &&precondition,
    vector<RelaxedProposition *> &&effects,
    RelaxedOperatorGroup &group) {
    RelaxedOperator relaxed_op(
        move(precondition), move(effects), &group);
    if (relaxed_op.preconditions.empty())
        relaxed_op.preconditions.push_back(&artificial_precondition);
    group.relaxed_operators.push_back(relaxed_op);
}

RelaxedProposition *CELandmarkCutLandmarks::get_proposition(
    const FactProxy &fact) {
    int var_id = fact.get_variable().get_id();
    int val = fact.get_value();
    return &propositions[var_id][val];
}

// heuristic computation
void CELandmarkCutLandmarks::setup_exploration_queue() {
    priority_queue.clear();

    for (auto &var_props : propositions) {
        for (RelaxedProposition &prop : var_props) {
            prop.status = UNREACHED;
        }
    }

    artificial_goal.status = UNREACHED;
    artificial_precondition.status = UNREACHED;

    for (RelaxedOperatorGroup &group : relaxed_operator_groups) {
        for (RelaxedOperator &op : group.relaxed_operators) {
            op.unsatisfied_preconditions = op.preconditions.size();
            op.h_max_supporter = 0;
            op.h_max_supporter_cost = numeric_limits<int>::max();
        }
    }
}

void CELandmarkCutLandmarks::setup_exploration_queue_state(const State &state) {
    for (FactProxy init_fact : state) {
        enqueue_if_necessary(get_proposition(init_fact), 0);
    }
    enqueue_if_necessary(&artificial_precondition, 0);
}

void CELandmarkCutLandmarks::first_exploration(const State &state) {
    assert(priority_queue.empty());
    setup_exploration_queue();
    setup_exploration_queue_state(state);
    while (!priority_queue.empty()) {
        pair<int, RelaxedProposition *> top_pair = priority_queue.pop();
        int popped_cost = top_pair.first;
        RelaxedProposition *prop = top_pair.second;
        int prop_cost = prop->h_max_cost;
        assert(prop_cost <= popped_cost);
        if (prop_cost < popped_cost)
            continue;
        const vector<RelaxedOperator *> &triggered_operators =
            prop->precondition_of;
        for (RelaxedOperator *relaxed_op : triggered_operators) {
            --relaxed_op->unsatisfied_preconditions;
            assert(relaxed_op->unsatisfied_preconditions >= 0);
            if (relaxed_op->unsatisfied_preconditions == 0) {
                relaxed_op->h_max_supporter = prop;
                relaxed_op->h_max_supporter_cost = prop_cost;
                int target_cost = prop_cost + relaxed_op->group->cost;
                for (RelaxedProposition *effect : relaxed_op->effects) {
                    enqueue_if_necessary(effect, target_cost);
                }
            }
        }
    }
}

void CELandmarkCutLandmarks::first_exploration_incremental(
    vector<RelaxedOperator *> &cut) {
    assert(priority_queue.empty());
    /* We pretend that this queue has had as many pushes already as we
       have propositions to avoid switching from bucket-based to
       heap-based too aggressively. This should prevent ever switching
       to heap-based in problems where action costs are at most 1.
    */
    priority_queue.add_virtual_pushes(num_propositions);
    for (RelaxedOperator *cut_op : cut) {
        RelaxedOperatorGroup *group = cut_op->group;
        assert(group);
        for (RelaxedOperator &op : group->relaxed_operators) {
            if (op.h_max_supporter) {
                int cost = op.h_max_supporter_cost + group->cost;
                for (RelaxedProposition *effect : op.effects)
                    enqueue_if_necessary(effect, cost);
            }
        }
    }
    while (!priority_queue.empty()) {
        pair<int, RelaxedProposition *> top_pair = priority_queue.pop();
        int popped_cost = top_pair.first;
        RelaxedProposition *prop = top_pair.second;
        int prop_cost = prop->h_max_cost;
        assert(prop_cost <= popped_cost);
        if (prop_cost < popped_cost)
            continue;
        const vector<RelaxedOperator *> &triggered_operators =
            prop->precondition_of;
        for (RelaxedOperator *relaxed_op : triggered_operators) {
            if (relaxed_op->h_max_supporter == prop) {
                int old_supp_cost = relaxed_op->h_max_supporter_cost;
                if (old_supp_cost > prop_cost) {
                    relaxed_op->update_h_max_supporter();
                    int new_supp_cost = relaxed_op->h_max_supporter_cost;
                    if (new_supp_cost != old_supp_cost) {
                        // This operator has become cheaper.
                        assert(new_supp_cost < old_supp_cost);
                        int target_cost = new_supp_cost + relaxed_op->group->cost;
                        for (RelaxedProposition *effect : relaxed_op->effects)
                            enqueue_if_necessary(effect, target_cost);
                    }
                }
            }
        }
    }
}

void CELandmarkCutLandmarks::second_exploration(
    const State &state, vector<RelaxedProposition *> &second_exploration_queue,
    vector<RelaxedOperator *> &cut) {
    assert(second_exploration_queue.empty());
    assert(cut.empty());

    artificial_precondition.status = BEFORE_GOAL_ZONE;
    second_exploration_queue.push_back(&artificial_precondition);

    for (FactProxy init_fact : state) {
        RelaxedProposition *init_prop = get_proposition(init_fact);
        init_prop->status = BEFORE_GOAL_ZONE;
        second_exploration_queue.push_back(init_prop);
    }

    while (!second_exploration_queue.empty()) {
        RelaxedProposition *prop = second_exploration_queue.back();
        second_exploration_queue.pop_back();
        const vector<RelaxedOperator *> &triggered_operators =
            prop->precondition_of;
        for (RelaxedOperator *relaxed_op : triggered_operators) {
            if (relaxed_op->h_max_supporter == prop) {
                bool reached_goal_zone = false;
                for (RelaxedProposition *effect : relaxed_op->effects) {
                    if (effect->status == GOAL_ZONE) {
                        assert(relaxed_op->group->cost > 0);
                        reached_goal_zone = true;
                        cut.push_back(relaxed_op);
                        break;
                    }
                }
                if (!reached_goal_zone) {
                    for (RelaxedProposition *effect : relaxed_op->effects) {
                        if (effect->status != BEFORE_GOAL_ZONE) {
                            assert(effect->status == REACHED);
                            effect->status = BEFORE_GOAL_ZONE;
                            second_exploration_queue.push_back(effect);
                        }
                    }
                }
            }
        }
    }
}

void CELandmarkCutLandmarks::mark_goal_plateau(RelaxedProposition *subgoal) {
    // NOTE: subgoal can be null if we got here via recursion through
    // a zero-cost action that is relaxed unreachable. (This can only
    // happen in domains which have zero-cost actions to start with.)
    // For example, this happens in pegsol-strips #01.
    if (subgoal && subgoal->status != GOAL_ZONE) {
        subgoal->status = GOAL_ZONE;
        for (RelaxedOperator *achiever : subgoal->effect_of)
            if (achiever->group->cost == 0)
                mark_goal_plateau(achiever->h_max_supporter);
    }
}

void CELandmarkCutLandmarks::validate_h_max() const {
#ifndef NDEBUG
    // Using conditional compilation to avoid complaints about unused
    // variables when using NDEBUG. This whole code does nothing useful
    // when assertions are switched off anyway.
    for (const RelaxedOperatorGroup &group : relaxed_operator_groups) {
        for (const RelaxedOperator &op : group.relaxed_operators) {
            if (op.unsatisfied_preconditions) {
                bool reachable = true;
                for (RelaxedProposition *pre : op.preconditions) {
                    if (pre->status == UNREACHED) {
                        reachable = false;
                        break;
                    }
                }
                assert(!reachable);
                assert(!op.h_max_supporter);
            } else {
                assert(op.h_max_supporter);
                int h_max_cost = op.h_max_supporter_cost;
                assert(h_max_cost == op.h_max_supporter->h_max_cost);
                for (RelaxedProposition *pre : op.preconditions) {
                    assert(pre->status != UNREACHED);
                    assert(pre->h_max_cost <= h_max_cost);
                }
            }
        }
    }
#endif
}

bool CELandmarkCutLandmarks::compute_landmarks(
    State state, CostCallback cost_callback,
    LandmarkCallback landmark_callback) {
    for (RelaxedOperatorGroup &group : relaxed_operator_groups) {
        group.cost = group.base_cost;
    }
    // The following three variables could be declared inside the loop
    // ("second_exploration_queue" even inside second_exploration),
    // but having them here saves reallocations and hence provides a
    // measurable speed boost.
    vector<RelaxedOperator *> cut;
    Landmark landmark;
    vector<RelaxedProposition *> second_exploration_queue;
    first_exploration(state);
    // validate_h_max();  // too expensive to use even in regular debug mode
    if (artificial_goal.status == UNREACHED)
        return true;

    int num_iterations = 0;
    while (artificial_goal.h_max_cost != 0) {
        ++num_iterations;
        mark_goal_plateau(&artificial_goal);
        assert(cut.empty());
        second_exploration(state, second_exploration_queue, cut);
        assert(!cut.empty());
        int cut_cost = numeric_limits<int>::max();
        for (RelaxedOperator *op : cut)
            cut_cost = min(cut_cost, op->group->cost);

        for (RelaxedOperator *cut_op : cut) {
            RelaxedOperatorGroup *group = cut_op->group;
            assert(group);
            if (!group->marked) {
                group->cost -= cut_cost;
                group->marked = true;
            }
        }
        for (RelaxedOperator *cut_op : cut) {
            cut_op->group->marked = false;
        }

        if (cost_callback) {
            cost_callback(cut_cost);
        }
        if (landmark_callback) {
            landmark.clear();
            for (RelaxedOperator *cut_op : cut) {
                RelaxedOperatorGroup *group = cut_op->group;
                landmark.push_back(group->original_op_id);
            }
            landmark_callback(landmark, cut_cost);
        }

        first_exploration_incremental(cut);
        // validate_h_max();  // too expensive to use even in regular debug mode
        cut.clear();

        /*
          Note: This could perhaps be made more efficient, for example by
          using a round-dependent counter for GOAL_ZONE and BEFORE_GOAL_ZONE,
          or something based on total_cost, so that we don't need a per-round
          reinitialization.
        */
        for (auto &var_props : propositions) {
            for (RelaxedProposition &prop : var_props) {
                if (prop.status == GOAL_ZONE || prop.status == BEFORE_GOAL_ZONE)
                    prop.status = REACHED;
            }
        }
        artificial_goal.status = REACHED;
        artificial_precondition.status = REACHED;
    }
    return false;
}
}
