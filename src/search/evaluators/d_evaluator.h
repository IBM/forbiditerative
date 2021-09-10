#ifndef EVALUATORS_D_EVALUATOR_H
#define EVALUATORS_D_EVALUATOR_H

#include "../evaluator.h"

namespace d_evaluator {
class DEvaluator : public Evaluator {
public:
    DEvaluator() = default;
    virtual ~DEvaluator() override = default;

    virtual EvaluationResult compute_result(
        EvaluationContext &eval_context) override;

    virtual void get_path_dependent_evaluators(std::set<Evaluator *> &) override {}
};
}

#endif
