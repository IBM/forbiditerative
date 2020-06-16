#include "abstract_task.h"

#include "plugin.h"
#include "global_operator.h"
#include "globals.h"

#include <iostream>

using namespace std;

const FactPair FactPair::no_fact = FactPair(-1, -1);

ostream &operator<<(ostream &os, const FactPair &fact_pair) {
    os << fact_pair.var << "=" << fact_pair.value;
    return os;
}

ostream &operator<<(ostream &os, const GlobalCondition &cond) {
	os << cond.var << " " << cond.val;
    return os;
}

void AbstractTask::dump_operator_pre_post_to_SAS(std::ostream& os, int pre, const GlobalEffect& eff) const {
    os << eff.conditions.size() << " ";
    for (GlobalCondition cond : eff.conditions) {
    	os << cond << std::endl;
    }
    os << eff.var << " " << pre << " " << eff.val << std::endl;
}


void AbstractTask::dump_axioms_to_SAS(std::ostream &os) const {
	os << get_num_axioms() << endl;
	for (int op_no = 0 ; op_no < get_num_axioms(); ++op_no) {
		dump_axiom_to_SAS(os, op_no);
	}
}

void AbstractTask::dump_axiom_to_SAS(std::ostream &os, int op_no) const {
    os << "begin_rule" << endl;
	vector<GlobalCondition> cond;
	for (int cond_ind = 0; cond_ind < get_num_operator_effect_conditions(op_no, 0, true); ++cond_ind) {
		FactPair fact = get_operator_effect_condition(op_no, 0, cond_ind, true);
		cond.push_back(GlobalCondition(fact.var, fact.value, false));
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
	dump_operator_pre_post_to_SAS(os, eff_pre_val, GlobalEffect(eff.var, eff.value, cond, false));
    os << "end_rule" << endl;
}

void AbstractTask::dump_operators_to_SAS(std::ostream &os) const {
	os << get_num_operators() << endl;
	for (int op_no = 0 ; op_no < get_num_operators(); ++op_no) {
		dump_operator_to_SAS(os, op_no);
	}
}

void AbstractTask::dump_operator_to_SAS(std::ostream &os, int op_no) const {
	vector<GlobalCondition> prevail;
	vector<int> eff_pre_val;
	vector<GlobalCondition> all_preconditions;
	vector<GlobalEffect> all_effects;
	for (int pre_ind = 0; pre_ind < get_num_operator_preconditions(op_no, false); ++pre_ind) {
		FactPair fact = get_operator_precondition(op_no, pre_ind, false);
		all_preconditions.push_back(GlobalCondition(fact.var, fact.value, false));
	}
	for (int eff_ind = 0; eff_ind < get_num_operator_effects(op_no, false); ++eff_ind) {
		vector<GlobalCondition> cond;
		for (int cond_ind = 0; cond_ind < get_num_operator_effect_conditions(op_no, eff_ind, false); ++cond_ind) {
			FactPair fact = get_operator_effect_condition(op_no, eff_ind, cond_ind, false);
			cond.push_back(GlobalCondition(fact.var, fact.value, false));
		}
		FactPair eff = get_operator_effect(op_no, eff_ind, false);
		all_effects.push_back(GlobalEffect(eff.var, eff.value, cond, false));
	}
	eff_pre_val.assign(all_effects.size(), -1);
	for (GlobalCondition c : all_preconditions) {
		// Checking if in any effect
		bool found = false;
		for (size_t i=0; i < all_effects.size(); ++i) {
			const GlobalEffect& eff = all_effects[i];
			if (eff.var != c.var)
				continue;
			found = true;
			eff_pre_val[i] = c.val;
		}

		if (!found)
			prevail.push_back(c);
	}

	os << "begin_operator" << endl;
    os << get_operator_name(op_no, false) << endl;
    os << prevail.size() << endl;
    for (GlobalCondition cond : prevail){
    	os << cond << endl;
    }
    os << all_effects.size() << endl;
    for (size_t i = 0; i < all_effects.size(); ++i){
    	dump_operator_pre_post_to_SAS(os, eff_pre_val[i], all_effects[i]);
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
	dump_metric(os);
	dump_variables_to_SAS(os);

	dump_mutexes(os);  //TODO: this is dumped from the global variables, which is wrong.

	dump_initial_state_to_SAS(os);
	dump_goal_to_SAS(os);

	dump_operators_to_SAS(os);
	dump_axioms_to_SAS(os);
}

static PluginTypePlugin<AbstractTask> _type_plugin(
    "AbstractTask",
    // TODO: Replace empty string by synopsis for the wiki page.
    "");
