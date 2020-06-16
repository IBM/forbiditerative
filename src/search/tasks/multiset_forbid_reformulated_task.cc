#include "multiset_forbid_reformulated_task.h"

#include <cassert>

#include "../utils/system.h"

using namespace std;


namespace extra_tasks {
MultisetForbidReformulatedTask::MultisetForbidReformulatedTask(
    const shared_ptr<AbstractTask> parent,
    std::unordered_map<int, int>& multiset, bool change_operator_names)
    : DelegatingTask(parent),
	  forbidding_multiset(multiset),
	    change_operator_names(change_operator_names),
	  operators_on_plan(0) {
    operators_on_plan = forbidding_multiset.size();
	// Compute the indexes for new operators
	// First go all operators from the original task: types 0 and 1 (not on plan and discarded)
    // We don't store these values, as these are identity
	// Then go all operators from the multiset with type 2 (discarding)
    int count_ops = 0;
    for (std::pair<int, int> e : forbidding_multiset) {
        int op_no = e.first;
        plan_operators_indexes.push_back(op_no);
        op_no_to_var_no[op_no] = count_ops++;
        var_no_to_op_no.push_back(op_no);
    }
    // Then go all operators from the multiset with type 3 (following, promoting the values).
	for (std::pair<int, int> e : forbidding_multiset) {
	    int op_no = e.first;
	    int count = e.second;
	    plan_operators_indexes.insert(plan_operators_indexes.end(), count, op_no);
	    for (int i=0; i < count; ++i)
	        plan_operators_following_effs.push_back(i+1);
	}
	cout << "Number of operators in the multiset is: " << operators_on_plan << endl;
	// The variables include the original ones, plus one variable for track plan deviation and one variable per action on a plan, counting their applications
	// Creating initial state values by copying from the parent and pushing the new variables initial values
	initial_state_values = parent->get_initial_state_values();
	initial_state_values.push_back(1); // Tracking plan following
	initial_state_values.insert(initial_state_values.end(), operators_on_plan, 0);
}

bool MultisetForbidReformulatedTask::is_operator_on_plan(int op_no) const {
    return forbidding_multiset.find(op_no) != forbidding_multiset.end();
}

int MultisetForbidReformulatedTask::get_parent_op_index(int index) const {
	// Getting the index of the corresponding operator in the parent task
	// First go all the operators from the parent task (type 0 for non-plan operators, type 1 for plan operators)
	// Then all the plan operators (no repetitions), with type 2
	// Then all the plan operators with repetitions, with type 3
	int num_parent_ops = parent->get_num_operators();
	if (index < num_parent_ops) // All type 0 + 1
		return index;
	int relative_index = index - num_parent_ops;
	return plan_operators_indexes[relative_index];
}

int MultisetForbidReformulatedTask::get_op_type(int index) const {
	// Returns type 0 for operators not on plan
	//         type 1 for operators "already discarded"
	//         type 2 for operators "discarding pi"
	//         type 3 for operators "following pi"
	int num_parent_ops = parent->get_num_operators();
	if (index < num_parent_ops) {
		return is_operator_on_plan(index);
	}

	int relative_index = index - num_parent_ops;
	if (relative_index < operators_on_plan)
		return 2;

	return 3;
}

int MultisetForbidReformulatedTask::get_possible_var_index() const {
	return parent->get_num_variables();
}

int MultisetForbidReformulatedTask::get_following_var_index(int op_index) const {
	// This is valid for type 2 and 3 only.
	assert(get_op_type(op_index) == 2 || get_op_type(op_index) == 3);
    int parent_op_index = get_parent_op_index(op_index);
    auto it = op_no_to_var_no.find(parent_op_index);
    assert(it != op_no_to_var_no.end());
    return parent->get_num_variables() + 1 + it->second;
}

int MultisetForbidReformulatedTask::get_following_operator_pre(int op_index) const {
    // This is valid for type 3 only.
    return get_following_operator_eff(op_index) - 1;
}

int MultisetForbidReformulatedTask::get_following_operator_eff(int op_index) const {
    // This is valid for type 3 only.
    assert(get_op_type(op_index) == 3);
    int relative_index = op_index - parent->get_num_operators() - operators_on_plan;
    return plan_operators_following_effs[relative_index];
}

int MultisetForbidReformulatedTask::get_number_appearances(int op_index) const {
    int parent_op_index = get_parent_op_index(op_index);
    auto it = forbidding_multiset.find(parent_op_index);
    assert(it != forbidding_multiset.end());
    return it->second;
}

int MultisetForbidReformulatedTask::get_num_variables() const {
    return parent->get_num_variables() + operators_on_plan + 1;
}

string MultisetForbidReformulatedTask::get_variable_name(int var) const {
	if (var < parent->get_num_variables())
		return parent->get_variable_name(var);
	if (var == parent->get_num_variables())
		return "possible";
	int ind = var - parent->get_num_variables() - 1;
	return "following" + std::to_string(ind);
}

int MultisetForbidReformulatedTask::get_variable_domain_size(int var) const {
	if (var < parent->get_num_variables())
		return parent->get_variable_domain_size(var);
    if (var == parent->get_num_variables())
        return 2;
    int ind = var - parent->get_num_variables() - 1;
	int op_no = var_no_to_op_no[ind];

    auto it = forbidding_multiset.find(op_no);
    assert(it != forbidding_multiset.end());
    return it->second + 1;
}

int MultisetForbidReformulatedTask::get_variable_axiom_layer(int var) const {
	if (var < parent->get_num_variables())
		return parent->get_variable_axiom_layer(var);

	return -1;
}

int MultisetForbidReformulatedTask::get_variable_default_axiom_value(int var) const {
	if (var < parent->get_num_variables())
		return parent->get_variable_default_axiom_value(var);
	if (var <= parent->get_num_variables() + 1)
		return 1;
	return 0;
}

string MultisetForbidReformulatedTask::get_fact_name(const FactPair &fact) const {
	if (fact.var < parent->get_num_variables())
		return parent->get_fact_name(fact);
	assert(fact.value >= 0 && fact.value <= 1);

    if (fact.var == parent->get_num_variables())
        return (fact.value == 0 ? "NegatedAtom __reformulation_possible()" : "Atom __reformulation_possible()");

    int ind = fact.var - parent->get_num_variables() - 1;
    int op_no = var_no_to_op_no[ind];

    return "Atom __reformulation_applied_" + std::to_string(op_no) + "_" + std::to_string(fact.value) + "_times()";
}

bool MultisetForbidReformulatedTask::are_facts_mutex(const FactPair &fact1, const FactPair &fact2) const {
	if (fact1.var < parent->get_num_variables() && fact2.var < parent->get_num_variables())
		return parent->are_facts_mutex(fact1, fact2);

	if (fact1.var >= parent->get_num_variables() &&
			fact1.var == fact2.var &&
			fact1.value != fact2.value)
		return true;

	return false;
//    ABORT("PlanForbidReformulatedTask doesn't support querying mutexes.");
}

int MultisetForbidReformulatedTask::get_operator_cost(int index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_operator_cost(index, is_axiom);

	int parent_op_index = get_parent_op_index(index);
	return parent->get_operator_cost(parent_op_index, is_axiom);
}

string MultisetForbidReformulatedTask::get_operator_name(int index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_operator_name(index, is_axiom);

	int parent_op_index = get_parent_op_index(index);
	string name = parent->get_operator_name(parent_op_index, is_axiom);
    if (change_operator_names) {
        int op_type = get_op_type(index);
        return name + "__###__" + std::to_string(op_type);
    }
	return name;
}

int MultisetForbidReformulatedTask::get_num_operators() const {
	// The number of operators according to the fixed reformulation is
	// number of operators + number of operators on the plan + plan length (number of operators on the plan with repetitions)
	// First, we have all parent operators (non-plan operators of type 0 and plan operators of type 1),
	// then operators of type 2 (no repetition),
	// and finally operators of type 3 (with repetitions).

    return parent->get_num_operators() + plan_operators_indexes.size();
}

int MultisetForbidReformulatedTask::get_num_operator_preconditions(int index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_num_operator_preconditions(index, is_axiom);

	int op_type = get_op_type(index);
	assert(op_type >= 0 && op_type <= 3);

	int parent_op_index = get_parent_op_index(index);
	if (op_type == 0)
		return parent->get_num_operator_preconditions(parent_op_index, is_axiom);

	if (op_type == 1)
		return parent->get_num_operator_preconditions(parent_op_index, is_axiom) + 1;

	assert(op_type == 2 || op_type == 3);
	return parent->get_num_operator_preconditions(parent_op_index, is_axiom) + 2;
}

FactPair MultisetForbidReformulatedTask::get_operator_precondition(
    int op_index, int fact_index, bool is_axiom) const {

	if (is_axiom)
		return parent->get_operator_precondition(op_index, fact_index, is_axiom);

	int parent_op_index = get_parent_op_index(op_index);
	int parent_num_pre = parent->get_num_operator_preconditions(parent_op_index, is_axiom);
	if (fact_index < parent_num_pre)
		return parent->get_operator_precondition(parent_op_index, fact_index, is_axiom);

	int op_type = get_op_type(op_index);
	assert(op_type >= 1 && op_type <= 3);

	if (op_type == 1)
		return FactPair(get_possible_var_index(), 0);

	// op_type == 2 || op_type == 3
	// first additional precondition is the same for both cases
	if (fact_index == parent_num_pre)
		return FactPair(get_possible_var_index(), 1);

	// next additional preconditions
	if (op_type == 2) {
		return FactPair(get_following_var_index(op_index), get_number_appearances(op_index));
	}
	assert(op_type == 3);
	return FactPair(get_following_var_index(op_index), get_following_operator_pre(op_index));
}

int MultisetForbidReformulatedTask::get_num_operator_effects(int op_index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_num_operator_effects(op_index, is_axiom);

	int op_type = get_op_type(op_index);
	assert(op_type >= 0 && op_type <= 3);
	int parent_op_index = get_parent_op_index(op_index);
	if (op_type == 0 || op_type == 2 || op_type == 3)
		return parent->get_num_operator_effects(parent_op_index, is_axiom) + 1;

	assert(op_type == 1);
	return parent->get_num_operator_effects(parent_op_index, is_axiom);
}

int MultisetForbidReformulatedTask::get_num_operator_effect_conditions(
    int op_index, int eff_index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_num_operator_effect_conditions(op_index, eff_index, is_axiom);

	int parent_op_index = get_parent_op_index(op_index);
	int parent_num_effs = parent->get_num_operator_effects(parent_op_index, is_axiom);
	if (eff_index < parent_num_effs)
		return parent->get_num_operator_effect_conditions(parent_op_index, eff_index, is_axiom);

	// The additional effects are unconditional
	return 0;
}

FactPair MultisetForbidReformulatedTask::get_operator_effect_condition(
    int op_index, int eff_index, int cond_index, bool is_axiom) const {
	int parent_op_index = op_index;
	if (!is_axiom)
		parent_op_index = get_parent_op_index(op_index);
	return parent->get_operator_effect_condition(parent_op_index, eff_index, cond_index, is_axiom);
}

FactPair MultisetForbidReformulatedTask::get_operator_effect(
    int op_index, int eff_index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_operator_effect(op_index, eff_index, is_axiom);

	int parent_op_index = get_parent_op_index(op_index);
	int parent_num_effs = parent->get_num_operator_effects(parent_op_index, is_axiom);
	if (eff_index < parent_num_effs)
		return parent->get_operator_effect(parent_op_index, eff_index, is_axiom);

	int op_type = get_op_type(op_index);
	assert(op_type >= 0 && op_type <= 3);

	if (op_type == 0 || op_type == 2)
		return FactPair(get_possible_var_index(), 0);

	assert(op_type == 3);
	return FactPair(get_following_var_index(op_index), get_following_operator_eff(op_index));
}

const GlobalOperator *MultisetForbidReformulatedTask::get_global_operator(int , bool ) const {
	ABORT("PlanForbidReformulatedTask doesn't support getting the operator explicitly.");
}

int MultisetForbidReformulatedTask::get_num_goals() const {
    return parent->get_num_goals() + 1;
}

FactPair MultisetForbidReformulatedTask::get_goal_fact(int index) const {
	if (index < parent->get_num_goals())
		return parent->get_goal_fact(index);

    return FactPair(get_possible_var_index(), 0);
}

vector<int> MultisetForbidReformulatedTask::get_initial_state_values() const {
    return initial_state_values;
}

void MultisetForbidReformulatedTask::convert_state_values_from_parent(
    vector<int> &) const {
	ABORT("MultisetForbidReformulatedTask doesn't support getting a state from the parent state.");
}

}
