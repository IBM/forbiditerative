#include "novelty_heuristic_simplified.h"

#include "../option_parser.h"
#include "../plugin.h"
#include "../evaluation_context.h"
#include "../utils/logging.h"

using namespace std;
namespace novelty_heuristic_simplified {

NoveltyHeuristicSimplified::NoveltyHeuristicSimplified(const Options &opts)
    : Heuristic(opts), novelty_heuristic(opts.get<shared_ptr<Evaluator>>("eval")),
        solution_found_by_heuristic(false),
	    dump_value(opts.get<bool>("dump")),
        log(utils::get_log_from_options(opts)),
        statistics(log) {
    log  << "Initializing novelty heuristic..." << endl;
    // Setting the value to DEAD_END initially.
    VariablesProxy variables = task_proxy.get_variables();
    novelty_per_variable_value.assign(variables.size(), std::vector<int>());
    for (VariableProxy var : variables) {
        novelty_per_variable_value[var.get_id()].assign(var.get_domain_size(), DEAD_END);
    }
    log  << "Done initializing novelty heuristic" << endl;
}

NoveltyHeuristicSimplified::~NoveltyHeuristicSimplified() {
}

int NoveltyHeuristicSimplified::compute_heuristic(const State &ancestor_state) {
    solution_found_by_heuristic = false;
    EvaluationContext eval_context(ancestor_state, 0, false, &statistics);
    int heuristic_value = eval_context.get_evaluator_value_or_infinity(novelty_heuristic.get());
    if (heuristic_value == EvaluationResult::INFTY)
        return DEAD_END;

    State state = convert_ancestor_state(ancestor_state);

    int strictly_better_novelty_facts_estimate = 0;
    int strictly_worse_novelty_facts_estimate = 0;
    for (FactProxy fact : state) {
        int var = fact.get_variable().get_id();
        int value = fact.get_value();

        int curr_value = novelty_per_variable_value[var][value];
        if (curr_value == DEAD_END || curr_value > heuristic_value) {
            novelty_per_variable_value[var][value] = heuristic_value;
            strictly_better_novelty_facts_estimate++;
        } else if (curr_value < heuristic_value) {
            strictly_worse_novelty_facts_estimate++;
        }
    }
    int ret = task_proxy.get_variables().size();
    if (strictly_better_novelty_facts_estimate > 0) {
        ret -= strictly_better_novelty_facts_estimate;
    } else {
        ret += strictly_worse_novelty_facts_estimate;
    }
    // Check if solution found by heuristic
    if (novelty_heuristic->found_solution()) {
        solution_found_by_heuristic = true;
    }
    if (dump_value) {
		log  << "NoveltyValue " << ret << endl;
	}
    return ret;
}

const std::vector<OperatorID>& NoveltyHeuristicSimplified::get_solution() const {
    if (solution_found_by_heuristic)
        return novelty_heuristic->get_solution();
    return Heuristic::get_solution();
}

static shared_ptr<Heuristic> _parse(OptionParser &parser) {
    parser.document_synopsis("Novelty heuristic", "");
    parser.document_language_support("action costs", "supported");
    parser.document_language_support("conditional effects", "supported");
    parser.document_language_support("axioms", "supported");
    parser.document_property("admissible", "no");
    parser.document_property("consistent", "no");
    parser.document_property("safe", "yes");
    parser.document_property("preferred operators", "no");

    Heuristic::add_options_to_parser(parser);
    parser.add_option<shared_ptr<Evaluator>>("eval", "Heuristic for novelty calculation");

    parser.add_option<bool>(
		"dump", 
		"Dump the Novelty value for each state", 
		"false");
        
    utils::add_log_options_to_parser(parser);

    Options opts = parser.parse();
    if (parser.dry_run())
        return nullptr;
    else {
        return make_shared<NoveltyHeuristicSimplified>(opts);
    }
}


static Plugin<Evaluator> _plugin("novelty_simplified", _parse);
}
