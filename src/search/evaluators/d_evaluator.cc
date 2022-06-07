#include "d_evaluator.h"

#include "../evaluation_context.h"
#include "../evaluation_result.h"
#include "../option_parser.h"
#include "../plugin.h"

using namespace std;

namespace d_evaluator {
EvaluationResult DEvaluator::compute_result(EvaluationContext &eval_context) {
    EvaluationResult result;
    result.set_evaluator_value(eval_context.get_d_value());
    return result;
}

static shared_ptr<Evaluator> _parse(OptionParser &parser) {
    parser.document_synopsis(
        "d-value evaluator",
        "Returns the d-value (path length) of the search node.");
    parser.parse();
    if (parser.dry_run())
        return nullptr;
    else
        return make_shared<DEvaluator>();
}

static Plugin<Evaluator> _plugin("d", _parse, "evaluators_basic");
}
