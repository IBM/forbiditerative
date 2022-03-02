#include "novelty_heuristic.h"

#include "../option_parser.h"
#include "../plugin.h"
#include "../evaluation_context.h"
#include "../utils/logging.h"
#include "../utils/system.h"
#include "../plan_manager.h"

using namespace std;
 
namespace novelty_heuristic {
NoveltyHeuristic::NoveltyHeuristic(const Options &opts)
    : Heuristic(opts), novelty_heuristic(opts.get<shared_ptr<Evaluator>>("eval")),
	  solution_found_by_heuristic(false),
	  novelty_heuristics_largest_value(DEAD_END),
	  type(opts.get<NoveltyType>("type")),
	  multiplier(opts.get<int>("multiplier")),
	  dump_value(opts.get<bool>("dump")),

	  use_preferred_operators(opts.get<bool>("pref")),
	  cutoff_type(opts.get<CutoffType>("cutoff_type")),
	  cutoff_bound(opts.get<int>("cutoff_bound", std::numeric_limits<int>::min())),
	  num_ops_bound(opts.get<int>("num_ops_bound")),
	  preferred_operators_from_evals(cutoff_type == NO_CUTOFF),  // In this case, we don't need to store novelty values per operator
	  reached_by_op_id(-1),
      log(utils::get_log_from_options(opts)),
      statistics(log) {
    log  << "Initializing novelty heuristic..." << endl;
	// Setting the value to DEAD_END initially.
    VariablesProxy variables = task_proxy.get_variables();
    novelty_per_variable_value.assign(variables.size(), std::vector<int>());
	for (VariableProxy var : variables) {
        novelty_per_variable_value[var.get_id()].assign(var.get_domain_size(), DEAD_END);
	}
	if (store_values_for_operators()) {
		if (log.is_at_least_normal()) {
	    	log  << "Allocating memory for storing heuristic values per operator" << endl;
		}
		OperatorsProxy operators = task_proxy.get_operators();
		novelty_per_operator.assign(operators.size(), DEAD_END);
    }
	log  << "Done initializing novelty heuristic" << endl;
}

NoveltyHeuristic::~NoveltyHeuristic() {
}

int NoveltyHeuristic::compute_heuristic(const State &ancestor_state) {
	solution_found_by_heuristic = false;
	EvaluationContext eval_context(ancestor_state, 0, true, &statistics);
	int heuristic_value = eval_context.get_evaluator_value_or_infinity(novelty_heuristic.get());
	if (heuristic_value == EvaluationResult::INFTY)
    	return DEAD_END;
	update_maximal_value(heuristic_value);

	compute_preferred_operators(heuristic_value, eval_context);
    State state = convert_ancestor_state(ancestor_state);

	int strictly_better_novelty_facts_estimate = 0;
	int strictly_worse_novelty_facts_estimate = 0;
	for (FactProxy fact : state) {	  
		int curr_value = get_value_for_fact(fact);
		if (curr_value == DEAD_END || curr_value > heuristic_value) {
			update_value_for_fact(fact, heuristic_value);
			strictly_better_novelty_facts_estimate += get_estimate_novel(curr_value, heuristic_value, log);
		} else if (curr_value < heuristic_value) {
			strictly_worse_novelty_facts_estimate += get_estimate_non_novel(curr_value, heuristic_value, log);
		}
	}
	int ret = multiplier * task_proxy.get_variables().size();
	if (type == BASIC) {
		ret = (strictly_better_novelty_facts_estimate > 0 ? 0 : 1);
	} else if (type == SEPARATE_NOVEL) {
		ret -= strictly_better_novelty_facts_estimate;
	} else if (type == SEPARATE_BOTH || type == SEPARATE_BOTH_AGGREGATE) {
		if (strictly_better_novelty_facts_estimate > 0)
			ret -= strictly_better_novelty_facts_estimate;
	    else
	    	ret += strictly_worse_novelty_facts_estimate;
	} else {
		utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
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

int NoveltyHeuristic::get_value_for_fact(const FactProxy& fact) const {
	int var = fact.get_variable().get_id();
	int val = fact.get_value();

	return novelty_per_variable_value[var][val];
}

void NoveltyHeuristic::update_value_for_fact(const FactProxy& fact, int value) {
	int var = fact.get_variable().get_id();
	int val = fact.get_value();

	novelty_per_variable_value[var][val] = value;
}

int NoveltyHeuristic::get_value_for_operator(OperatorID op_id) const {
	return novelty_per_operator[op_id.get_index()];
}

void NoveltyHeuristic::update_value_for_operator(OperatorID op_id, int value) {
	novelty_per_operator[op_id.get_index()] = value;
}

bool NoveltyHeuristic::store_values_for_operators() const {
	return (use_preferred_operators && !preferred_operators_from_evals);
}

void NoveltyHeuristic::notify_state_transition(
    const State &, OperatorID op_id,
    const State &) {

    if (store_values_for_operators()) {
		reached_by_op_id = op_id;
    }
}

const std::vector<OperatorID>& NoveltyHeuristic::get_solution() const {
    if (solution_found_by_heuristic) {
		return novelty_heuristic->get_solution();
	}
    return Heuristic::get_solution();
}

void NoveltyHeuristic::compute_candidate_operators_argmax(std::vector<OperatorID>& candidates, const std::vector<OperatorID> &pref_ops) const {
	// max value
	int max_value = std::numeric_limits<int>::min();
	for (OperatorID op_id : pref_ops) {
		int curr_value = get_value_for_operator(op_id);
        if (curr_value == DEAD_END) {
			max_value = curr_value;
        	break;
		} else if (curr_value > max_value) {
			max_value = curr_value;
        }
	}
	for (OperatorID op_id : pref_ops) {
		int curr_value = get_value_for_operator(op_id);
        if (curr_value == max_value) {
            candidates.push_back(op_id);
        }
	}
    // rng->shuffle(candidates);
}

void NoveltyHeuristic::compute_candidate_operators_random(int heuristic_value, std::vector<OperatorID>& candidates, const std::vector<OperatorID> &pref_ops) const {
	//All operators novel above cutoff_bound, randmoly shuffle and take the first elements
	for (OperatorID op_id : pref_ops) {
		int curr_value = get_value_for_operator(op_id);
		if (curr_value == DEAD_END || curr_value - heuristic_value > cutoff_bound) {
            candidates.push_back(op_id);
		}
	}
    // rng->shuffle(candidates);
}

void NoveltyHeuristic::compute_candidate_operators_ordered(int heuristic_value, std::vector<OperatorID>& candidates, const std::vector<OperatorID> &pref_ops) const {
	//All operators novel above cutoff_bound, sorting by value, descending
    std::vector<pair<int,OperatorID>> candidates_values;
	for (OperatorID op_id : pref_ops) {
		int curr_value = get_value_for_operator(op_id);
		if (curr_value == DEAD_END || curr_value - heuristic_value > cutoff_bound) {
            candidates_values.push_back(make_pair(curr_value, op_id));
		}
	}
    // Sorting, descending order
    std::sort(candidates_values.begin(), candidates_values.end(), [](const pair<int,OperatorID> &l, const pair<int,OperatorID> &r) {return l.first > r.first;});
    for (auto p : candidates_values) {
        candidates.push_back(p.second);
    }
}

void NoveltyHeuristic::compute_preferred_operators(int heuristic_value, EvaluationContext& eval_context) {
	if (!use_preferred_operators) {
		return;
	}
    // Updating for the operator that reaches the current state
	if (reached_by_op_id.get_index() >= 0 && store_values_for_operators()) {
		int curr_value = get_value_for_operator(reached_by_op_id);
		if (curr_value == DEAD_END || curr_value > heuristic_value) {
			if (log.is_at_least_debug()) {
				log << "Updating value for operator " << reached_by_op_id << " from " << curr_value << " to " << heuristic_value << endl;
			}
			update_value_for_operator(reached_by_op_id, heuristic_value);
		}
	}

    // Selecting the set of preferred operators to choose from
    const std::vector<OperatorID> &pref_ops_heuristic = eval_context.get_preferred_operators(novelty_heuristic.get());
    std::vector<OperatorID> candidates;
	// for cutoff_bound-novelty pruning
	if (cutoff_type == ARGMAX) {
		compute_candidate_operators_argmax(candidates, pref_ops_heuristic);
	} else if (cutoff_type == ALL_RANDOM) {
		compute_candidate_operators_random(heuristic_value, candidates, pref_ops_heuristic);
	} else if (cutoff_type == ALL_ORDERED) {
		compute_candidate_operators_ordered(heuristic_value, candidates, pref_ops_heuristic);
	} else if (cutoff_type == NO_CUTOFF) {
        // Adding all pref ops
        candidates.insert(candidates.begin(), pref_ops_heuristic.begin(), pref_ops_heuristic.end());
        // rng->shuffle(candidates);
	} else {
        ABORT("unknown cutoff type");
	}

    // Select a subset of selected pref ops, according to the bounds
    // Get the absolute number of ops, choose the top/random elements
    int num_ops_to_select = (int) candidates.size();
    num_ops_to_select = min( num_ops_to_select, num_ops_bound);

    // If the number of operators to select is smaller than the set size, choosing elements randomly
    if (cutoff_type != ALL_ORDERED && num_ops_to_select < (int) candidates.size()) {
        cerr <<  "This configuration is not supported " << endl;
    } else {
        for (int i = 0; i < num_ops_to_select; ++i) {
            set_preferred(task_proxy.get_operators()[candidates[i]]);
        }
    }
}

void NoveltyHeuristic::update_maximal_value(int value) {
	if (novelty_heuristics_largest_value == DEAD_END) {
		novelty_heuristics_largest_value = value;
		return;
	}

	if (value > novelty_heuristics_largest_value) {
		novelty_heuristics_largest_value = value;
	}
}

int NoveltyHeuristic::compute_aggregated_score(std::vector<int>& values) const {
	int max = 0;
	for (int val : values) {
		if (val > max)
			max = val;
	}
	return max;
}

int NoveltyHeuristic::get_estimate_novel(int curr_value, int heur_value, utils::LogProxy &clog) const {
	if (curr_value == DEAD_END) {
		return multiplier;
	}
	//Here, curr_value > heur_value
	if (type == BASIC || type == SEPARATE_NOVEL || type == SEPARATE_BOTH) {
		return multiplier;
	}

	if (type == SEPARATE_BOTH_AGGREGATE) {
		// Here, we return a value between 0 and multiplier
		double diff =  ((double) (curr_value - heur_value)) / novelty_heuristics_largest_value;
		if (clog.is_at_least_debug()) {
			clog << "Diff: " << diff << endl;
		}
		return (int) (multiplier * diff);
	}
	utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
}

int NoveltyHeuristic::get_estimate_non_novel(int curr_value, int heur_value, utils::LogProxy &clog) const {
	if (type == BASIC ||
		type == SEPARATE_NOVEL ||
		curr_value == heur_value) {
		return 0;
	}

	//Here, curr_value < heur_value
	if (type == SEPARATE_BOTH) {
		return multiplier;
	}

	if (type == SEPARATE_BOTH_AGGREGATE) {
		// Here, we return a value between 1 and multiplier
	    double diff =  ((double) (heur_value - curr_value)) / novelty_heuristics_largest_value;
		if (clog.is_at_least_debug()) {
			clog << "Diff: " << diff << endl;
			clog << "h: " << heur_value << " - c: " << curr_value << " =  " << diff << " * multiplier: " << multiplier <<  " / "<< novelty_heuristics_largest_value << " = " << (int) (multiplier * diff)  << endl;
		}
		return (int) (multiplier * diff);
	}
	utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
}

static shared_ptr<Heuristic> _parse(OptionParser &parser) {
    parser.document_synopsis("Novelty heuristic", "");
	parser.document_language_support("action costs", "supported");
    parser.document_language_support("conditional effects", "supported");
    parser.document_language_support("axioms", "supported");
    parser.document_property("admissible", "no");
    parser.document_property("consistent", "no");
    parser.document_property("safe", "yes");
    parser.document_property("preferred operators", "yes");

    Heuristic::add_options_to_parser(parser);
    parser.add_option<shared_ptr<Evaluator>>("eval", "Heuristic for novelty calculation");

    vector<string> heur_opt;
    heur_opt.push_back("basic");
    heur_opt.push_back("separate_novel");
    heur_opt.push_back("separate_both");
    heur_opt.push_back("separate_both_aggregate");
    parser.add_enum_option<NoveltyType>("type",
    				   	   heur_opt,
						   "Novelty definition type",
						   "separate_both");

    parser.add_option<int>(
		"multiplier", 
		"Integer multiplier for the value of each fact", 
		"1", 
		Bounds("1", "infinity"));

    parser.add_option<bool>(
		"dump", 
		"Dump the Novelty value for each state", 
		"false");


    parser.add_option<bool>(
		"pref", 
		"Compute preferred operators", 
		"false");

    // Disabled, now set by  cutoff_type=no_cutoff
    // parser.add_option<bool>(
	// 	"eval_prefs", 
	// 	"Set preferred operators from evals", 
	// 	"false");

	parser.add_option<int>(
		"cutoff_bound", 
		"Cutoff bound for novelty pruning of preferred operators", 
		std::to_string(std::numeric_limits<int>::min()));

	parser.add_option<int>(
		"num_ops_bound", 
		"Bound on the amount of preferred operators",
		"infinity",
		Bounds("1", "infinity"));

    parser.add_option<double>(
        "num_ops_relative_bound",
        "Relative bound on the amount of preferred operators",
        "1.0");

    vector<string> cuttof_opt;
    cuttof_opt.push_back("argmax");
    cuttof_opt.push_back("all_ordered");
    cuttof_opt.push_back("all_random");
    cuttof_opt.push_back("no_cutoff");
	parser.add_enum_option<CutoffType>("cutoff_type",
						cuttof_opt,
						"Novelty cutoff type (none) for basic options from heuristic",
						"no_cutoff");

    utils::add_log_options_to_parser(parser);

    Options opts = parser.parse();
    if (parser.dry_run())
        return nullptr;
    else {
        return make_shared<NoveltyHeuristic>(opts);
	}
}


static Plugin<Evaluator> _plugin("novelty", _parse);
}
