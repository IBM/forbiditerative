#include "global_operator.h"

#include "globals.h"

#include "utils/collections.h"
#include "utils/system.h"

#include <algorithm>
#include <cassert>
#include <iostream>

using namespace std;
using utils::ExitCode;


static void check_fact(int var, int val) {
    if (!utils::in_bounds(var, g_variable_domain)) {
        cerr << "Invalid variable id: " << var << endl;
        utils::exit_with(ExitCode::INPUT_ERROR);
    }
    if (val < 0 || val >= g_variable_domain[var]) {
        cerr << "Invalid value for variable " << var << ": " << val << endl;
        utils::exit_with(ExitCode::INPUT_ERROR);
    }
}

GlobalCondition::GlobalCondition(istream &in) {
    in >> var >> val;
    check_fact(var, val);
}

GlobalCondition::GlobalCondition(int variable, int value, bool check_facts)
    : var(variable),
      val(value) {
	if (check_facts)
		check_fact(var, val);
}

// TODO if the input file format has been changed, we would need something like this
// Effect::Effect(istream &in) {
//    int cond_count;
//    in >> cond_count;
//    for (int i = 0; i < cond_count; ++i)
//        cond.push_back(Condition(in));
//    in >> var >> post;
//}

GlobalEffect::GlobalEffect(int variable, int value, const vector<GlobalCondition> &conds, bool check_facts)
    : var(variable),
      val(value),
      conditions(conds) {
	if (check_facts)
		check_fact(var, val);
}

void GlobalOperator::read_pre_post(istream &in) {
    int cond_count, var, pre, post;
    in >> cond_count;
    vector<GlobalCondition> conditions;
    conditions.reserve(cond_count);
    for (int i = 0; i < cond_count; ++i)
        conditions.push_back(GlobalCondition(in));
    in >> var >> pre >> post;
    if (pre != -1)
        check_fact(var, pre);
    check_fact(var, post);
    if (pre != -1)
        preconditions.push_back(GlobalCondition(var, pre));
    effects.push_back(GlobalEffect(var, post, conditions));
}

GlobalOperator::GlobalOperator(istream &in, bool axiom) {
    is_an_axiom = axiom;
    if (!is_an_axiom) {
        check_magic(in, "begin_operator");
        in >> ws;
        getline(in, name);
        int count;
        in >> count;
        for (int i = 0; i < count; ++i)
            preconditions.push_back(GlobalCondition(in));
        in >> count;
        for (int i = 0; i < count; ++i)
            read_pre_post(in);

        int op_cost;
        in >> op_cost;
        cost = g_use_metric ? op_cost : 1;

        g_min_action_cost = min(g_min_action_cost, cost);
        g_max_action_cost = max(g_max_action_cost, cost);

        check_magic(in, "end_operator");
    } else {
        name = "<axiom>";
        cost = 0;
        check_magic(in, "begin_rule");
        read_pre_post(in);
        check_magic(in, "end_rule");
    }
}

void GlobalCondition::dump() const {
    cout << g_variable_name[var] << ": " << val;
}

void GlobalEffect::dump() const {
    cout << g_variable_name[var] << ":= " << val;
    if (!conditions.empty()) {
        cout << " if";
        for (size_t i = 0; i < conditions.size(); ++i) {
            cout << " ";
            conditions[i].dump();
        }
    }
}

void GlobalOperator::dump() const {
    cout << name << ":";
    for (size_t i = 0; i < preconditions.size(); ++i) {
        cout << " [";
        preconditions[i].dump();
        cout << "]";
    }
    for (size_t i = 0; i < effects.size(); ++i) {
        cout << " [";
        effects[i].dump();
        cout << "]";
    }
    cout << endl;
}

int get_op_index_hacked(const GlobalOperator *op) {
    int op_index = op - &*g_operators.begin();
    assert(op_index >= 0 && op_index < static_cast<int>(g_operators.size()));
    return op_index;
}

void GlobalOperator::dump_SAS(ofstream& os, const vector<GlobalCondition>& extra_pre,
										    const vector<GlobalEffect>& extra_eff) const {
	// extra_pre and extra_eff are meant to add to the regular pre and eff
	vector<GlobalCondition> prevail;
	vector<int> eff_pre_val;
	vector<GlobalCondition> all_preconditions(preconditions);
	vector<GlobalEffect> all_effects(effects);
	all_preconditions.insert(all_preconditions.end(),extra_pre.begin(), extra_pre.end());
	all_effects.insert(all_effects.end(), extra_eff.begin(), extra_eff.end());

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

    if (is_an_axiom) {
        os << "begin_rule" << endl;
        dump_pre_post_SAS(os, eff_pre_val[0], all_effects[0]);
        os << "end_rule" << endl;
    } else {
    	int type = get_reformulated_op_type(extra_pre, extra_eff);
        os << "begin_operator" << endl;
        os << name << " " << type << endl;
        os << prevail.size() << endl;
        for(size_t i = 0; i < prevail.size(); ++i){
            prevail[i].dump_SAS(os);
        }
        os << all_effects.size() << endl;
        for(size_t i = 0; i < all_effects.size(); ++i){
        	dump_pre_post_SAS(os, eff_pre_val[i], all_effects[i]);
        }

        if (g_use_metric)
            os << cost << endl;
        else
            os << "1" << endl;

        os << "end_operator" << endl;
    }
}

int GlobalOperator::get_reformulated_op_type(const vector<GlobalCondition>& extra_pre, const vector<GlobalEffect>& extra_eff) const {
	if (extra_pre.empty())
		return 0;
	if (extra_eff.empty())
		return 1;
	if (extra_eff.size() == 2)
		return 3;
	return 2;
}

void GlobalOperator::dump_pre_post_SAS(std::ofstream& os, int pre, GlobalEffect eff) const {
    os << eff.conditions.size() << " ";
    for(size_t i = 0; i < eff.conditions.size(); ++i){
    	eff.conditions[i].dump_SAS(os);
    }
    os << eff.var << " " << pre << " " << eff.val << std::endl;
}
