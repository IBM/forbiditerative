#include "operator_interaction.h"

#include "../task_tools.h"

#include "../utils/collections.h"

#include <algorithm>
#include <cassert>

using namespace std;

namespace plans {
// Relies on both fact sets being sorted by variable.
bool contain_conflicting_fact(const vector<FactPair> &facts1,
                              const vector<FactPair> &facts2) {
    auto facts1_it = facts1.begin();
    auto facts2_it = facts2.begin();
    while (facts1_it != facts1.end() && facts2_it != facts2.end()) {
        if (facts1_it->var < facts2_it->var) {
            ++facts1_it;
        } else if (facts1_it->var > facts2_it->var) {
            ++facts2_it;
        } else {
            if (facts1_it->value != facts2_it->value)
                return true;
            ++facts1_it;
            ++facts2_it;
        }
    }
    return false;
}

OperatorInteraction::OperatorInteraction(const shared_ptr<AbstractTask> &_task) : task(_task) {
    TaskProxy task_proxy(*task);
    verify_no_axioms(task_proxy);
    verify_no_conditional_effects(task_proxy);

    compute_sorted_operators(task_proxy);
}

// Relies on op_preconds and op_effects being sorted by variable.
bool OperatorInteraction::can_disable(int op1_no, int op2_no) const {
    return contain_conflicting_fact(sorted_op_effects[op1_no],
                                    sorted_op_preconditions[op2_no]);
}

// Relies on op_effect being sorted by variable.
bool OperatorInteraction::can_conflict(int op1_no, int op2_no) const {
    return contain_conflicting_fact(sorted_op_effects[op1_no],
                                    sorted_op_effects[op2_no]);
}

void OperatorInteraction::compute_sorted_operators(const TaskProxy &task_proxy) {
    OperatorsProxy operators = task_proxy.get_operators();

    sorted_op_preconditions = utils::map_vector<vector<FactPair>>(
        operators, [](const OperatorProxy &op) {
            return utils::sorted<FactPair>(get_fact_pairs(op.get_preconditions()));
        });

    sorted_op_effects = utils::map_vector<vector<FactPair>>(
        operators, [](const OperatorProxy &op) {
            return utils::sorted<FactPair>(
                utils::map_vector<FactPair>(
                    op.get_effects(),
                    [](const EffectProxy &eff) {return eff.get_fact().get_pair(); }));
        });
}
}
