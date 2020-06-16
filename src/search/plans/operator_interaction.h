#ifndef PLANS_OPERATOR_INTERACTION_H
#define PLANS_OPERATOR_INTERACTION_H

#include "../abstract_task.h"
#include "../pruning_method.h"

namespace plans {
class OperatorInteraction {
    std::shared_ptr<AbstractTask> task;

    void compute_sorted_operators(const TaskProxy &task_proxy);

    std::vector<std::vector<FactPair>> sorted_op_preconditions;
    std::vector<std::vector<FactPair>> sorted_op_effects;

    /* achievers[var][value] contains all operator indices of
       operators that achieve the fact (var, value). */
    std::vector<std::vector<std::vector<int>>> achievers;

    bool can_disable(int op1_no, int op2_no) const;
    bool can_conflict(int op1_no, int op2_no) const;

public:
    OperatorInteraction(const std::shared_ptr<AbstractTask> &_task);
    virtual ~OperatorInteraction() = default;

    bool interfere(int op1_no, int op2_no) const {
        return can_disable(op1_no, op2_no) ||
               can_conflict(op1_no, op2_no) ||
               can_disable(op2_no, op1_no);
    }
};

}

#endif
