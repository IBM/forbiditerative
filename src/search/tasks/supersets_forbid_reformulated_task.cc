#include "supersets_forbid_reformulated_task.h"

#include <cassert>
#include <algorithm>

#include "../utils/system.h"

using namespace std;


namespace extra_tasks {
SupersetsForbidReformulatedTask::SupersetsForbidReformulatedTask(
    const shared_ptr<AbstractTask> parent,
    std::vector<std::unordered_set<int>>& sets)
        : DelegatingTask(parent),
	    forbidding_sets(sets) {
    // Creating the set union of all forbidding sets
    // Computing the number of appearances of all operators in all forbidding sets
	// This is done to compute the indexes for new operators
	// First come all operators from the original task: o^u for all original operators o (exactly )
    // We don't store these values, as these are identity
    // Then come operators that allow for reaching the goal: o_i for o in forbidding_sets[i]
    int set_id = 0;
    int var_id = 0;
    unordered_map<int,unordered_set<int>> sets_for_op;
	for (auto fs : forbidding_sets) {
        for (int op_no : fs) {
            extra_op_index_to_parent_op_set_indices.push_back(OperatorIndices(op_no, set_id));
            if (forbidding_ops_to_var.find(op_no) == forbidding_ops_to_var.end()) {
                forbidding_ops_to_var[op_no] = var_id++;
                var_no_to_op_no.push_back(op_no);
            }
            sets_for_op[op_no].insert(set_id);
        }
        set_id++;
    }
    // Inverting the forbidding_ops_to_sets
    for (auto fs : sets_for_op) {
        vector<int> sets_per_op;
        for (int i=0; i < (int) forbidding_sets.size(); ++i) {
            if (fs.second.find(i) == fs.second.end()) {
                // op is not in the set i
                sets_per_op.push_back(i);
            }
        }
        forbidding_ops_to_sets[fs.first]= std::move(sets_per_op);
    }


	// The variables include 
    //     the original ones 
    //     one variable for tracking the sets 
    //     one variable per action in the set union, counting their applications
	// Creating initial state values by copying from the parent and pushing the new variables initial values
	initial_state_values = parent->get_initial_state_values();
    initial_state_values.push_back(0);  // Extra var for tracking sets (domain size forbidding_sets.size() + 1) 
	initial_state_values.insert(initial_state_values.end(), forbidding_ops_to_var.size(), 0); // Extra var per op, tracking its application
	initial_state_values.insert(initial_state_values.end(), forbidding_sets.size(), 0); // Extra var per set, tracking application of action outside the set
}

bool SupersetsForbidReformulatedTask::is_operator_on_plans(int op_no) const {
    return forbidding_ops_to_var.find(op_no) != forbidding_ops_to_var.end();
}

int SupersetsForbidReformulatedTask::get_num_sets_for_op(int op_no) const {
    auto it = forbidding_ops_to_sets.find(op_no);
    if (it == forbidding_ops_to_sets.end())
        return 0;

    return (int)it->second.size();
}

const SupersetsForbidReformulatedTask::OperatorIndices& SupersetsForbidReformulatedTask::get_parent_op_index(int op_index) const {
	// Getting the index of the corresponding operator in the parent task, and of a set from which it came
    assert (op_index >= parent->get_num_operators()); 

    int relative_index = op_index - parent->get_num_operators();
	return extra_op_index_to_parent_op_set_indices[relative_index];
}

int SupersetsForbidReformulatedTask::get_op_for_var_index(int var_index) const { 
    // Getting the op_no for the relative index of the extra var that tracks the op_no
	return var_no_to_op_no[var_index]; 
}

int SupersetsForbidReformulatedTask::get_set_tracking_var_index() const { 
	return parent->get_num_variables(); 
}

int SupersetsForbidReformulatedTask::get_op_tracking_var_index(int op_no) const {
    auto it = forbidding_ops_to_var.find(op_no);
    assert(it != forbidding_ops_to_var.end());
    return parent->get_num_variables() + 1 + it->second;
}

int SupersetsForbidReformulatedTask::get_sets_tracking_var_index(int set_index) const { 
	return parent->get_num_variables() + 1 + (int)forbidding_ops_to_var.size() + set_index; 
}

// //////////////////
int SupersetsForbidReformulatedTask::get_num_variables() const {
    return (int) initial_state_values.size();
}

string SupersetsForbidReformulatedTask::get_variable_name(int var) const {
	if (var < parent->get_num_variables())
		return parent->get_variable_name(var);

	int relative_index = var - parent->get_num_variables();

	if (relative_index == 0)
		return "tracking_sets";

	relative_index--;
    if (relative_index < (int) forbidding_ops_to_var.size()) {
        int op_no = get_op_for_var_index(relative_index);
        string op_name = parent->get_operator_name(op_no, false);
        std::replace(op_name.begin(), op_name.end(), ' ', '_');
        return "tracking_op_" + op_name + "_" + std::to_string(relative_index);
    }
    relative_index -= (int) forbidding_ops_to_var.size();
    assert(relative_index < (int) forbidding_sets.size());
    return "tracking_set_" + std::to_string(relative_index);
}

int SupersetsForbidReformulatedTask::get_variable_domain_size(int var) const {
	if (var < parent->get_num_variables())
		return parent->get_variable_domain_size(var);

	int relative_index = var - parent->get_num_variables();
	if (relative_index == 0)
        return (int) forbidding_sets.size() + 1;

	return 2;
}

int SupersetsForbidReformulatedTask::get_variable_axiom_layer(int var) const {
	if (var < parent->get_num_variables())
		return parent->get_variable_axiom_layer(var);

	return -1;
}

int SupersetsForbidReformulatedTask::get_variable_default_axiom_value(int var) const {
	if (var < parent->get_num_variables())
		return parent->get_variable_default_axiom_value(var);
	return 0;
}

string SupersetsForbidReformulatedTask::get_fact_name(const FactPair &fact) const {
	if (fact.var < parent->get_num_variables())
		return parent->get_fact_name(fact);
	int relative_index = fact.var - parent->get_num_variables();
	if (relative_index == 0) {
        // The fact value corresponds to the number of sets checked so far
        return "Atom __tested_sets_" + std::to_string(fact.value) + "()";
    }

	relative_index--;
    if (relative_index < (int) forbidding_ops_to_var.size()) {
        string atom_name = "__operator_applied()";
        return (fact.value == 0 ? "NegatedAtom " + atom_name : "Atom " + atom_name);
    } 
    relative_index -= (int) forbidding_ops_to_var.size();
    assert(relative_index < (int) forbidding_sets.size());
    string atom_name = "__operator_outside_set_applied()";
    return (fact.value == 0 ? "NegatedAtom " + atom_name : "Atom " + atom_name);
}

bool SupersetsForbidReformulatedTask::are_facts_mutex(const FactPair &fact1, const FactPair &fact2) const {
	if (fact1.var < parent->get_num_variables() && fact2.var < parent->get_num_variables())
		return parent->are_facts_mutex(fact1, fact2);

	if (fact1.var >= parent->get_num_variables() &&
			fact1.var == fact2.var &&
			fact1.value != fact2.value)
		return true;

	return false;
}

int SupersetsForbidReformulatedTask::get_operator_cost(int index, bool is_axiom) const {
	if (is_axiom || index < parent->get_num_operators())
		return parent->get_operator_cost(index, is_axiom);

    return 0;
}

string SupersetsForbidReformulatedTask::get_operator_name(int index, bool is_axiom) const {
	if (is_axiom || index < parent->get_num_operators())
		return parent->get_operator_name(index, is_axiom);

    // Getting the parent op_id and the set id 
    auto ids = get_parent_op_index(index);
	return "__###__goal_achieving__set_" + std::to_string(ids.set_id) + "_op_" + parent->get_operator_name(ids.parent_op_no, is_axiom);
}

int SupersetsForbidReformulatedTask::get_num_operators() const {
	// The number of operators in the reformulation is number of original operators + extra goal achieving operators
    return parent->get_num_operators() + (int) extra_op_index_to_parent_op_set_indices.size();
}

int SupersetsForbidReformulatedTask::get_num_operator_preconditions(int index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_num_operator_preconditions(index, is_axiom);

	if (index < parent->get_num_operators()) {
		return parent->get_num_operator_preconditions(index, is_axiom) + 1;
    }
    return parent->get_num_goals() + 3;
}

FactPair SupersetsForbidReformulatedTask::get_operator_precondition(
    int op_index, int fact_index, bool is_axiom) const {

	if (is_axiom)
		return parent->get_operator_precondition(op_index, fact_index, is_axiom);

	if (op_index < parent->get_num_operators()) {
        if (fact_index < parent->get_num_operator_preconditions(op_index, is_axiom)) {
		    return parent->get_operator_precondition(op_index, fact_index, is_axiom);
        }
        // Extra pre
        return FactPair(get_set_tracking_var_index(),0);
    }

    // Extra op
    // The first preconditions is the original goal
    if (fact_index < parent->get_num_goals()) {
        return parent->get_goal_fact(fact_index);
    } 

    // Getting the parent op_id and the set id 
    auto ids = get_parent_op_index(op_index);
    assert(ids.set_id >= 0);
    // Then comes the op tracking var \overline{v}_o
    if (fact_index == parent->get_num_goals()) {
        return FactPair(get_op_tracking_var_index(ids.parent_op_no),0);
    }
    if (fact_index == parent->get_num_goals() + 1) {
        // Then comes the set tracking var \overline{v}
        return FactPair(get_set_tracking_var_index(),ids.set_id);
    }
    // Last comes the outside set tracking var \overline{v}_i
    return FactPair(get_sets_tracking_var_index(ids.set_id),1);

}

int SupersetsForbidReformulatedTask::get_num_operator_effects(int op_index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_num_operator_effects(op_index, is_axiom);

	if (op_index < parent->get_num_operators()) {
        int num_sets_op_not_in = (int)forbidding_sets.size();
        int is_in_set = 0;
        if (is_operator_on_plans(op_index)) {
            is_in_set = 1;
            // getting the number of sets op is in
            num_sets_op_not_in = get_num_sets_for_op(op_index);
        }  
		return parent->get_num_operator_effects(op_index, is_axiom) + is_in_set + num_sets_op_not_in;
    }
    return 1;
}

int SupersetsForbidReformulatedTask::get_num_operator_effect_conditions(
    int op_index, int eff_index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_num_operator_effect_conditions(op_index, eff_index, is_axiom);


	if (op_index < parent->get_num_operators() && eff_index < parent->get_num_operator_effects(op_index, is_axiom)) 
		return parent->get_num_operator_effect_conditions(op_index, eff_index, is_axiom);        

	// The additional effects are unconditional
	return 0;
}

FactPair SupersetsForbidReformulatedTask::get_operator_effect_condition(
    int op_index, int eff_index, int cond_index, bool is_axiom) const {
    return parent->get_operator_effect_condition(op_index, eff_index, cond_index, is_axiom);
}

FactPair SupersetsForbidReformulatedTask::get_operator_effect(
    int op_index, int eff_index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_operator_effect(op_index, eff_index, is_axiom);

	if (op_index < parent->get_num_operators()) {
        int num_parent_effects = parent->get_num_operator_effects(op_index, is_axiom);
        if (eff_index < num_parent_effects) {
		    return parent->get_operator_effect(op_index, eff_index, is_axiom);
        }
        bool is_op_on_plans = is_operator_on_plans(op_index);
        if (is_op_on_plans && eff_index == num_parent_effects) {
            // Possible extra effect
            return FactPair(get_op_tracking_var_index(op_index),1);
        }
        // The next effects are on set variables
        if (is_op_on_plans) {
            int relative_eff_index = eff_index - num_parent_effects - 1;
            // taking the sets from forbidding_ops_to_sets
            auto it = forbidding_ops_to_sets.find(op_index);
            assert(it != forbidding_ops_to_sets.end());

            int set_index = it->second[relative_eff_index];
            return FactPair(get_sets_tracking_var_index(set_index),1);
        } 
        int relative_eff_index = eff_index - num_parent_effects;
        return FactPair(get_sets_tracking_var_index(relative_eff_index),1);
    }
    // Extra op
    // Getting the parent op_id and the set id 
    auto ids = get_parent_op_index(op_index);
    assert(ids.set_id >= 0);
    return FactPair(get_set_tracking_var_index(),ids.set_id+1);
}

int SupersetsForbidReformulatedTask::get_num_goals() const {
    return parent->get_num_goals() + 1;
}

FactPair SupersetsForbidReformulatedTask::get_goal_fact(int index) const {
	if (index < parent->get_num_goals())
		return parent->get_goal_fact(index);

    return FactPair(get_set_tracking_var_index(), (int) forbidding_sets.size());
}

vector<int> SupersetsForbidReformulatedTask::get_initial_state_values() const {
    return initial_state_values;
}

void SupersetsForbidReformulatedTask::convert_state_values_from_parent(
    vector<int> &) const {
	ABORT("SupersetsForbidReformulatedTask doesn't support getting a state from the parent state.");
}

}
