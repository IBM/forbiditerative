#include "d_evaluator.h"

#include "../option_parser.h"
#include "../plugin.h"

namespace d_evaluator {

static ScalarEvaluator *_parse(OptionParser &parser) {
    parser.document_synopsis(
        "d-value evaluator",
        "Returns the d-value (path length) of the search node.");
    Heuristic::add_options_to_parser(parser);

    Options opts = parser.parse();
    if (parser.dry_run())
        return 0;
    else
        return new DEvaluator(opts);
}

static Plugin<ScalarEvaluator> _plugin("d", _parse);
}
