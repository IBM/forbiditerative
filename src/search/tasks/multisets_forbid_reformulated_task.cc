#include "multisets_forbid_reformulated_task.h"

#include <cassert>
#include <algorithm>

#include "../utils/system.h"

using namespace std;


namespace extra_tasks {
MultisetsForbidReformulatedTask::MultisetsForbidReformulatedTask(
    const shared_ptr<AbstractTask> parent,
    std::vector<std::unordered_map<int, int>>& multisets, bool change_operator_names)
    : DelegatingTask(parent),
	  forbidding_multisets(multisets), 
	    change_operator_names(change_operator_names),
	  operators_on_plans(0), number_of_plans(0) {

	for (std::unordered_map<int, int> forbidding_multiset : forbidding_multisets) {
	    for (std::pair<int, int> e : forbidding_multiset) {
    	    int op_no = e.first;
		    int count = e.second;
			if (max_count.find(op_no) != max_count.end()) {
				max_count[op_no] = max(max_count[op_no], count);
			} else {
				max_count[op_no] = count;
			}
		}
    }
	operators_on_plans = max_count.size();
	number_of_plans = forbidding_multisets.size();
	// Compute the indexes for new operators
	// First go all operators from the original task: types 0 and 1 (not on plan and o^f_{m_o})
    // We don't store these values, as these are identity
	// Then go all operators from the multiset with type 2 (o^f_i, i < m_o)
	int ind=0;
	for (std::pair<int, int> e : max_count) {
    	int op_no = e.first;
	    int count = e.second;
		first_index_per_operator[op_no] = plan_operators_indexes.size();
	    plan_operators_indexes.insert(plan_operators_indexes.end(), count, op_no);
    	op_no_to_var_no[op_no] = ind++;
       	var_no_to_op_no.push_back(op_no);
	}
	cout << "Number of operators in the multisets is: " << operators_on_plans << endl;
	// The variables include the original ones, plus one variable for track plan deviation and one variable per action on a plan, counting their applications
	// Creating initial state values by copying from the parent and pushing the new variables initial values
	initial_state_values = parent->get_initial_state_values();
	initial_state_values.insert(initial_state_values.end(), number_of_plans, 1); // Tracking plans following
	initial_state_values.insert(initial_state_values.end(), operators_on_plans, 0);
}

bool MultisetsForbidReformulatedTask::is_operator_on_plan(int op_no, size_t plan_index) const {
    return forbidding_multisets[plan_index].find(op_no) != forbidding_multisets[plan_index].end();
}
bool MultisetsForbidReformulatedTask::is_operator_on_plans(int op_no) const {
    return max_count.find(op_no) != max_count.end();
}

int MultisetsForbidReformulatedTask::get_parent_op_index(int index) const {
	// Getting the index of the corresponding operator in the parent task
	// First go all the operators from the parent task (type 0 for non-plan operators, type 1 for plan operators)
	// Then all the plan operators with type 2
	int num_parent_ops = parent->get_num_operators();
	if (index < num_parent_ops) // All type 0 + 1
		return index;
	int relative_index = index - num_parent_ops;
	assert(relative_index < plan_operators_indexes.size());
	return plan_operators_indexes[relative_index];
}

int MultisetsForbidReformulatedTask::get_op_type(int index) const {
	// Returns type 0 for operators not on plan
	//         type 1 for operators "already discarded"
	//         type 2 for operators "counting applications, discarding when necessary"
	int num_parent_ops = parent->get_num_operators();
	if (index < num_parent_ops) {
		return is_operator_on_plans(index);
	}
	return 2;
}

int MultisetsForbidReformulatedTask::get_discarded_plan_var_index(size_t plan_index) const { 
	return parent->get_num_variables() + plan_index; 
}

int MultisetsForbidReformulatedTask::get_counting_var_index(int op_index) const {
	// This is valid for type 2 only.
	assert(get_op_type(op_index) == 2);
    int parent_op_index = get_parent_op_index(op_index);
    auto it = op_no_to_var_no.find(parent_op_index);
    assert(it != op_no_to_var_no.end());
    return parent->get_num_variables() + number_of_plans + it->second;
}

int MultisetsForbidReformulatedTask::get_counting_operator_pre(int op_index) const {
    return get_counting_operator_index(op_index);
}

int MultisetsForbidReformulatedTask::get_counting_operator_eff(int op_index) const {
    return get_counting_operator_index(op_index) + 1;
}

int MultisetsForbidReformulatedTask::get_counting_operator_index(int op_index) const {
	int parent_op_index = get_parent_op_index(op_index);
    auto it = first_index_per_operator.find(parent_op_index);
    assert(it != first_index_per_operator.end());
 
	int first_index = it->second; 
	int relative_index = op_index - parent->get_num_operators();
	assert(relative_index >= first_index);
    return relative_index - first_index;
}

int MultisetsForbidReformulatedTask::get_number_appearances(int op_index) const {
    int parent_op_index = get_parent_op_index(op_index);
    auto it = max_count.find(parent_op_index);
	if (it == max_count.end())
		return 0;	
    return it->second;
}

int MultisetsForbidReformulatedTask::get_number_appearances_plan(int op_index, size_t plan_index) const {
    int parent_op_index = get_parent_op_index(op_index);
    auto it = forbidding_multisets[plan_index].find(parent_op_index);
    if (it == forbidding_multisets[plan_index].end())
		return 0;
    return it->second;
}

int MultisetsForbidReformulatedTask::get_num_variables() const {
    return parent->get_num_variables() + operators_on_plans + number_of_plans;
}

string MultisetsForbidReformulatedTask::get_variable_name(int var) const {
	if (var < parent->get_num_variables())
		return parent->get_variable_name(var);

	int relative_index = var - parent->get_num_variables();

	if (relative_index < number_of_plans)
		return "plan_id_still_possible" + std::to_string(relative_index+1);

	relative_index -= number_of_plans;
	assert(relative_index < operators_on_plans);
	int op_no = var_no_to_op_no[relative_index];
	string op_name = parent->get_operator_name(op_no, false);
	std::replace(op_name.begin(), op_name.end(), ' ', '_');
	return "following_" + op_name + "_" + std::to_string(relative_index);
}

int MultisetsForbidReformulatedTask::get_variable_domain_size(int var) const {
	if (var < parent->get_num_variables())
		return parent->get_variable_domain_size(var);

	int relative_index = var - parent->get_num_variables();
	if (relative_index < number_of_plans)
		return 2;

	relative_index -= number_of_plans;
	assert(relative_index < operators_on_plans);

	int op_no = var_no_to_op_no[relative_index];

    auto it = max_count.find(op_no);
    assert(it != max_count.end());
    return it->second + 1;
}

int MultisetsForbidReformulatedTask::get_variable_axiom_layer(int var) const {
	if (var < parent->get_num_variables())
		return parent->get_variable_axiom_layer(var);

	return -1;
}

int MultisetsForbidReformulatedTask::get_variable_default_axiom_value(int var) const {
	if (var < parent->get_num_variables())
		return parent->get_variable_default_axiom_value(var);
	if (var <= parent->get_num_variables() + 1)
		return 1;
	return 0;
}

string MultisetsForbidReformulatedTask::get_fact_name(const FactPair &fact) const {
	if (fact.var < parent->get_num_variables())
		return parent->get_fact_name(fact);
	int rel_var = fact.var - parent->get_num_variables();
	if (rel_var < number_of_plans) {
		string atom_name = "__reformulation_discard_plan_status" + std::to_string(rel_var) + "()";
        return (fact.value == 0 ? "NegatedAtom " + atom_name : "Atom " + atom_name);
	}
	rel_var -= number_of_plans;
    int op_no = var_no_to_op_no[rel_var];

    return "Atom __reformulation_applied_" + std::to_string(op_no) + "_" + std::to_string(fact.value) + "_times()";
}

bool MultisetsForbidReformulatedTask::are_facts_mutex(const FactPair &fact1, const FactPair &fact2) const {
	if (fact1.var < parent->get_num_variables() && fact2.var < parent->get_num_variables())
		return parent->are_facts_mutex(fact1, fact2);

	if (fact1.var >= parent->get_num_variables() &&
			fact1.var == fact2.var &&
			fact1.value != fact2.value)
		return true;

	return false;
}

int MultisetsForbidReformulatedTask::get_operator_cost(int index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_operator_cost(index, is_axiom);

	int parent_op_index = get_parent_op_index(index);
	return parent->get_operator_cost(parent_op_index, is_axiom);
}

string MultisetsForbidReformulatedTask::get_operator_name(int index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_operator_name(index, is_axiom);

	int parent_op_index = get_parent_op_index(index);
	string name = parent->get_operator_name(parent_op_index, is_axiom);
    if (change_operator_names) {
        int op_type = get_op_type(index);
		if (op_type == 2)
	        return name + "__###__" + std::to_string(op_type) + " " + std::to_string(index);
	    return name + "__###__" + std::to_string(op_type);
    }
	return name;
}

int MultisetsForbidReformulatedTask::get_num_operators() const {
	// The number of operators in the reformulation is number of original operators + sum of all counts.
	// First, we have all parent operators (non-plan operators of type 0 and plan operators of type 1),
	// then operators of type 2.

    return parent->get_num_operators() + plan_operators_indexes.size();
}

int MultisetsForbidReformulatedTask::get_num_operator_preconditions(int index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_num_operator_preconditions(index, is_axiom);

	int op_type = get_op_type(index);
	assert(op_type >= 0 && op_type <= 2);

	int parent_op_index = get_parent_op_index(index);
	if (op_type == 0)
		return parent->get_num_operator_preconditions(parent_op_index, is_axiom);

	return parent->get_num_operator_preconditions(parent_op_index, is_axiom) + 1;
}

FactPair MultisetsForbidReformulatedTask::get_operator_precondition(
    int op_index, int fact_index, bool is_axiom) const {

	if (is_axiom)
		return parent->get_operator_precondition(op_index, fact_index, is_axiom);

	int parent_op_index = get_parent_op_index(op_index);
	int parent_num_pre = parent->get_num_operator_preconditions(parent_op_index, is_axiom);
	if (fact_index < parent_num_pre)
		return parent->get_operator_precondition(parent_op_index, fact_index, is_axiom);

	int op_type = get_op_type(op_index);
	assert(op_type == 1 || op_type == 2);

	if (op_type == 1)
		return FactPair(get_counting_var_index(op_index), get_number_appearances(op_index));

	return FactPair(get_counting_var_index(op_index), get_counting_operator_pre(op_index));
}

int MultisetsForbidReformulatedTask::get_num_operator_effects(int op_index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_num_operator_effects(op_index, is_axiom);

	int op_type = get_op_type(op_index);
	assert(op_type >= 0 && op_type <= 2);
	int parent_op_index = get_parent_op_index(op_index);
	if (op_type == 0 || op_type == 1)
		return parent->get_num_operator_effects(parent_op_index, is_axiom) + number_of_plans;

	assert(op_type == 2);
	// Counting the number of plans we need to deviate from
	int plans_to_deviate_from = 0;
	for (size_t i=0; i < forbidding_multisets.size(); ++i) {
		int count_op = get_number_appearances_plan(op_index, i);
		int ind = get_counting_operator_index(op_index);
		if (count_op == ind) {
			plans_to_deviate_from++;
		}
	}
	return parent->get_num_operator_effects(parent_op_index, is_axiom) + plans_to_deviate_from + 1;
}

int MultisetsForbidReformulatedTask::get_num_operator_effect_conditions(
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

FactPair MultisetsForbidReformulatedTask::get_operator_effect_condition(
    int op_index, int eff_index, int cond_index, bool is_axiom) const {
	int parent_op_index = op_index;
	if (!is_axiom)
		parent_op_index = get_parent_op_index(op_index);
	return parent->get_operator_effect_condition(parent_op_index, eff_index, cond_index, is_axiom);
}

FactPair MultisetsForbidReformulatedTask::get_operator_effect(
    int op_index, int eff_index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_operator_effect(op_index, eff_index, is_axiom);

	int parent_op_index = get_parent_op_index(op_index);
	int parent_num_effs = parent->get_num_operator_effects(parent_op_index, is_axiom);
	if (eff_index < parent_num_effs)
		return parent->get_operator_effect(parent_op_index, eff_index, is_axiom);

	int op_type = get_op_type(op_index);
	assert(op_type >= 0 && op_type <= 2);
	if (op_type == 0 || op_type == 1) {
		int plan_index = eff_index - parent_num_effs;
		assert(plan_index < number_of_plans);
		return FactPair(get_discarded_plan_var_index(plan_index), 0);
	}

	assert(op_type == 2);
	// First, counting appearances, then deviating from plans 
	if (eff_index == parent_num_effs) {
		return FactPair(get_counting_var_index(op_index), get_counting_operator_eff(op_index));
	}
	int plan_index = eff_index - parent_num_effs - 1;
	int deviated_so_far=0;
	size_t i=0;
	for (; i < forbidding_multisets.size(); ++i) {
		int count_op = get_number_appearances_plan(op_index, i);
		int ind = get_counting_operator_index(op_index);
		if (count_op == ind) {
			if (plan_index == deviated_so_far) {
				// found the right plan
				break;
			}
			deviated_so_far++;
		}
	}
	return FactPair(get_discarded_plan_var_index(i), 0);
}

const GlobalOperator *MultisetsForbidReformulatedTask::get_global_operator(int , bool ) const {
	ABORT("MultisetsForbidReformulatedTask doesn't support getting the operator explicitly.");
}

int MultisetsForbidReformulatedTask::get_num_goals() const {
    return parent->get_num_goals() + number_of_plans;
}

FactPair MultisetsForbidReformulatedTask::get_goal_fact(int index) const {
	if (index < parent->get_num_goals())
		return parent->get_goal_fact(index);

	int plan_index = index - parent->get_num_goals();
	assert(plan_index < number_of_plans);
    return FactPair(get_discarded_plan_var_index(plan_index), 0);
}

vector<int> MultisetsForbidReformulatedTask::get_initial_state_values() const {
    return initial_state_values;
}

void MultisetsForbidReformulatedTask::convert_state_values_from_parent(
    vector<int> &) const {
	ABORT("MultisetsForbidReformulatedTask doesn't support getting a state from the parent state.");
}

}
