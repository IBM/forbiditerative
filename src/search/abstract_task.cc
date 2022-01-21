#include "abstract_task.h"

#include "per_task_information.h"
#include "tasks/root_task.h"
#include "plugin.h"

#include <iostream>

using namespace std;

const FactPair FactPair::no_fact = FactPair(-1, -1);

ostream &operator<<(ostream &os, const FactPair &fact_pair) {
    os << fact_pair.var << "=" << fact_pair.value;
    return os;
}

// ostream &operator<<(ostream &os, const GlobalCondition &cond) {
// 	os << cond.var << " " << cond.val;
//     return os;
// }

bool AbstractTask::is_unit_cost() const {
	for (int op_no = 0 ; op_no < get_num_operators(); ++op_no) {
        if (1 != get_operator_cost(op_no, false))
            return false;
    }
    return true;
}

void AbstractTask::dump_operator_pre_post_to_SAS(std::ostream& os, int pre, FactPair eff, const vector<FactPair>& eff_cond) const {
    os << eff_cond.size() << " ";
    for (FactPair cond : eff_cond) {
    	os << cond.var << " " << cond.value << " " << std::endl;
    }
    os << eff.var << " " << pre << " " << eff.value << std::endl;
}


void AbstractTask::dump_axioms_to_SAS(std::ostream &os) const {
	os << get_num_axioms() << endl;
	for (int op_no = 0 ; op_no < get_num_axioms(); ++op_no) {
		dump_axiom_to_SAS(os, op_no);
	}
}

void AbstractTask::dump_axiom_to_SAS(std::ostream &os, int op_no) const {
    os << "begin_rule" << endl;
	vector<FactPair> cond;
	for (int cond_ind = 0; cond_ind < get_num_operator_effect_conditions(op_no, 0, true); ++cond_ind) {
		FactPair fact = get_operator_effect_condition(op_no, 0, cond_ind, true);
		cond.push_back(fact);
	}
	FactPair eff = get_operator_effect(op_no, 0, true);
	int eff_pre_val = -1;
	for (int pre_ind = 0; pre_ind < get_num_operator_preconditions(op_no, true); ++pre_ind) {
		FactPair fact = get_operator_precondition(op_no, pre_ind, true);
		if (eff.var == fact.var) {
			eff_pre_val = fact.value;
			break;
		}
	}
	dump_operator_pre_post_to_SAS(os, eff_pre_val, eff, cond);
    os << "end_rule" << endl;
}

void AbstractTask::dump_operators_to_SAS(std::ostream &os) const {
	os << get_num_operators() << endl;
	for (int op_no = 0 ; op_no < get_num_operators(); ++op_no) {
		dump_operator_to_SAS(os, op_no);
	}
}

void AbstractTask::dump_operator_to_SAS(std::ostream &os, int op_no) const {
	vector<FactPair> prevail;
	vector<int> eff_pre_val;
	vector<FactPair> all_preconditions;
	vector<FactPair> all_effects;
	vector<vector<FactPair>> all_effects_cond;
	for (int pre_ind = 0; pre_ind < get_num_operator_preconditions(op_no, false); ++pre_ind) {
		FactPair fact = get_operator_precondition(op_no, pre_ind, false);
		all_preconditions.push_back(fact);
	}
	for (int eff_ind = 0; eff_ind < get_num_operator_effects(op_no, false); ++eff_ind) {
		vector<FactPair> cond;
		for (int cond_ind = 0; cond_ind < get_num_operator_effect_conditions(op_no, eff_ind, false); ++cond_ind) {
			FactPair fact = get_operator_effect_condition(op_no, eff_ind, cond_ind, false);
			cond.push_back(fact);
		}
		FactPair eff = get_operator_effect(op_no, eff_ind, false);
		all_effects.push_back(eff);
		all_effects_cond.push_back(cond);
	}
	eff_pre_val.assign(all_effects.size(), -1);
	for (FactPair c : all_preconditions) {
		// Checking if in any effect
		bool found = false;
		for (size_t i=0; i < all_effects.size(); ++i) {
			const FactPair& eff = all_effects[i];
			if (eff.var != c.var)
				continue;
			found = true;
			eff_pre_val[i] = c.value;
		}

		if (!found)
			prevail.push_back(c);
	}

	os << "begin_operator" << endl;
    os << get_operator_name(op_no, false) << endl;
    os << prevail.size() << endl;
    for (FactPair cond : prevail){
    	os << cond.var << " " << cond.value << endl;
    }
    os << all_effects.size() << endl;
    for (size_t i = 0; i < all_effects.size(); ++i){
    	dump_operator_pre_post_to_SAS(os, eff_pre_val[i], all_effects[i], all_effects_cond[i]);
    }
    os << get_operator_cost(op_no, false) << endl;
    os << "end_operator" << endl;
}

void AbstractTask::dump_variables_to_SAS(std::ostream &os) const {
	os << get_num_variables() << endl;
	for(int var = 0; var < get_num_variables(); ++var) {
		vector<string> vals;
		for(int val = 0; val < get_variable_domain_size(var); ++val) {
			vals.push_back(get_fact_name(FactPair(var,val)));
		}
		dump_variable(os, get_variable_name(var), get_variable_axiom_layer(var), get_variable_domain_size(var), vals);
	}
}

void AbstractTask::dump_initial_state_to_SAS(std::ostream &os) const {
	os << "begin_state" << endl;
	for (int val : get_initial_state_values())
		os << val << endl;
	os << "end_state" << endl;
}

void AbstractTask::dump_goal_to_SAS(std::ostream &os) const {
	os << "begin_goal" << endl;
	os << get_num_goals() << endl;
	for(int i = 0; i < get_num_goals(); ++i) {
		FactPair fact = get_goal_fact(i);
		os << fact.var << " " << fact.value << endl;
	}
	os << "end_goal" << endl;
}

void AbstractTask::dump_to_SAS(ostream &os) const {
	dump_version(os);
	os << "begin_metric" << endl;
	os << !is_unit_cost() << endl;
	os << "end_metric" << endl;	
    dump_variables_to_SAS(os);

	// os << "0" << endl;
	dump_mutexes(os);  //TODO: rethink

	dump_initial_state_to_SAS(os);
	dump_goal_to_SAS(os);

	dump_operators_to_SAS(os);
	dump_axioms_to_SAS(os);
}


void AbstractTask::dump_variable(std::ostream& os, std::string name, int axiom_layer, int domain, const std::vector<std::string>& values) const {
    os << "begin_variable" << endl;
    os << name << endl;
    os << axiom_layer << endl;
    os << domain << endl;
    for (size_t j=0; j < values.size(); ++j)
        os << values[j] << endl;
    os << "end_variable" << endl;
}

void AbstractTask::dump_version(std::ostream& os) const {
    os << "begin_version" << endl;
    os << tasks::PRE_FILE_VERSION << endl;
    os << "end_version" << endl;
}

void AbstractTask::dump_mutexes(std::ostream& os) const {
	// Check if exists non-trivial mutex per variable 
	// Terribly inefficient implementation...
    vector<vector<FactPair>> invariant_groups;

	for(int var = 0; var < get_num_variables(); ++var) {
    	vector<FactPair> invariant_group;

		for(int val = 0; val < get_variable_domain_size(var); ++val) {
			FactPair f(var, val); 
			invariant_group.push_back(f);

			for(int var2 = 0; var2 < get_num_variables(); ++var2) {
				if (var == var2)
					continue;
				for(int val2 = 0; val2 < get_variable_domain_size(var2); ++val2) {
					FactPair f2(var2, val2); 

					if (are_facts_mutex(f, f2)) {
						invariant_group.push_back(f2);
					}
				}

			}

		}
		if ((int)invariant_group.size() > get_variable_domain_size(var)) {
			// Keeping non-trivial groups
			invariant_groups.push_back(invariant_group);
		}
	}
    os << invariant_groups.size() << endl;
    for (vector<FactPair> invariant_group : invariant_groups) {
        os << "begin_mutex_group" << endl;
        os << invariant_group.size() << endl;
        for (FactPair fact : invariant_group) {
            os << fact.var << endl;
            os << fact.value << endl;
        }
        os << "end_mutex_group" << endl;
    }
}


static PluginTypePlugin<AbstractTask> _type_plugin(
    "AbstractTask",
    // TODO: Replace empty string by synopsis for the wiki page.
    "");
