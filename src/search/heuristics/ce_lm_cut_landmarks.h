#ifndef HEURISTICS_CE_LM_CUT_LANDMARKS_H
#define HEURISTICS_CE_LM_CUT_LANDMARKS_H

#include "../task_proxy.h"

#include "../algorithms/priority_queues.h"

#include <cassert>
#include <functional>
#include <memory>
#include <vector>

namespace ce_lm_cut_heuristic {
// TODO: Fix duplication with the other relaxation heuristics.
struct RelaxedProposition;

enum PropositionStatus {
    UNREACHED = 0,
    REACHED = 1,
    GOAL_ZONE = 2,
    BEFORE_GOAL_ZONE = 3
};

struct RelaxedOperator;

struct RelaxedOperatorGroup {
    int original_op_id;
    int base_cost;
    int cost : 31;
    bool marked : 1;
    // NOTE: Mixed type bit fields are not guarantueed to be compact in memory.
    //       See the comment for SearchNodeInfo.
    std::vector<RelaxedOperator> relaxed_operators;

    size_t size() const {
        return relaxed_operators.size();
    }

    RelaxedOperator &operator[](size_t i) {
        return relaxed_operators[i];
    }

    const RelaxedOperator &operator[](size_t i) const {
        return relaxed_operators[i];
    }

    RelaxedOperatorGroup(int original_op_id, int base_cost_)
        : original_op_id(original_op_id), base_cost(base_cost_), marked(false) {
    }
};

struct RelaxedOperator {
    RelaxedOperatorGroup *group;
    std::vector<RelaxedProposition *> preconditions;
    std::vector<RelaxedProposition *> effects;

    int unsatisfied_preconditions;
    int h_max_supporter_cost; // h_max_cost of h_max_supporter
    RelaxedProposition *h_max_supporter;
    RelaxedOperator(const std::vector<RelaxedProposition *> &&pre,
                    const std::vector<RelaxedProposition *> &&eff,
                    RelaxedOperatorGroup *group_)
        : group(group_), preconditions(pre), effects(eff) {
    }

    inline void update_h_max_supporter();
};

struct RelaxedProposition {
    std::vector<RelaxedOperator *> precondition_of;
    std::vector<RelaxedOperator *> effect_of;

    PropositionStatus status;
    int h_max_cost;
};

class CELandmarkCutLandmarks {
    // RelaxedOperators are grouped by the Operator that induced them
    std::vector<RelaxedOperatorGroup> relaxed_operator_groups;
    std::vector<std::vector<RelaxedProposition>> propositions;
    RelaxedProposition artificial_precondition;
    RelaxedProposition artificial_goal;
    int num_propositions;
    priority_queues::AdaptiveQueue<RelaxedProposition *> priority_queue;

    void initialize();
    void build_relaxed_operator(const OperatorProxy &op, RelaxedOperatorGroup &group);
    void add_relaxed_operator(std::vector<RelaxedProposition *> &&precondition,
                              std::vector<RelaxedProposition *> &&effects,
                              RelaxedOperatorGroup &group);
    RelaxedProposition *get_proposition(const FactProxy &fact);
    void setup_exploration_queue();
    void setup_exploration_queue_state(const State &state);
    void first_exploration(const State &state);
    void first_exploration_incremental(std::vector<RelaxedOperator *> &cut);
    void second_exploration(const State &state,
                            std::vector<RelaxedProposition *> &queue,
                            std::vector<RelaxedOperator *> &cut);

    void enqueue_if_necessary(RelaxedProposition *prop, int cost) {
        assert(cost >= 0);
        if (prop->status == UNREACHED || prop->h_max_cost > cost) {
            prop->status = REACHED;
            prop->h_max_cost = cost;
            priority_queue.push(cost, prop);
        }
    }

    void mark_goal_plateau(RelaxedProposition *subgoal);
    void validate_h_max() const;
public:
    using Landmark = std::vector<int>;
    using CostCallback = std::function<void (int)>;
    using LandmarkCallback = std::function<void (const Landmark &, int)>;

    CELandmarkCutLandmarks(const TaskProxy &task_proxy);
    virtual ~CELandmarkCutLandmarks();

    /*
      Compute LM-cut landmarks for the given state.

      If cost_callback is not nullptr, it is called once with the cost of each
      discovered landmark.

      If landmark_callback is not nullptr, it is called with each discovered
      landmark (as a vector of operator indices) and its cost. This requires
      making a copy of the landmark, so cost_callback should be used if only the
      cost of the landmark is needed.

      Returns true iff state is detected as a dead end.
    */
    bool compute_landmarks(State state, CostCallback cost_callback,
                           LandmarkCallback landmark_callback);
};

inline void RelaxedOperator::update_h_max_supporter() {
    assert(!unsatisfied_preconditions);
    for (size_t i = 0; i < preconditions.size(); ++i)
        if (preconditions[i]->h_max_cost > h_max_supporter->h_max_cost)
            h_max_supporter = preconditions[i];
    h_max_supporter_cost = h_max_supporter->h_max_cost;
}
}

#endif
