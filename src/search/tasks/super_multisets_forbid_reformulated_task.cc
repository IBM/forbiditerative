#include "super_multisets_forbid_reformulated_task.h"

#include <cassert>
#include <algorithm>

#include "../utils/system.h"

using namespace std;


namespace extra_tasks {
SuperMultisetsForbidReformulatedTask::SuperMultisetsForbidReformulatedTask(
    const shared_ptr<AbstractTask> parent,
    std::vector<std::unordered_map<int, int>>& multisets, bool change_operator_names)
    : DelegatingTask(parent),
	  forbidding_multisets(multisets), 
	    change_operator_names(change_operator_names) {

    // Creating the multiset union of all forbidding multisets (m_o)
    // cout << "Multisets:" << endl;
	for (auto fms : forbidding_multisets) {
	    for (auto e : fms) {
    	    int op_no = e.first;
		    int count = e.second;
            // cout << op_no << ": " << count << endl;
			if (max_count.find(op_no) != max_count.end()) {
				max_count[op_no] = max(max_count[op_no], count);
			} else {
				max_count[op_no] = count;
			}
		}
    }
    // Storing indices for new operators
    // First come o^u_i for 0 <= i <= m_o for original op o, we store a mapping for these operators to their original operator.
    int var_id = 0;
    // for (auto e : max_count) {
    for (int op_no = 0; op_no < parent->get_num_operators(); ++op_no) {       
	    int count = get_number_appearances(op_no);
        for (int i = 0 ; i <= count ; ++i) {
            // cout << "Action for operator " << op_no << ", index " << i << endl; 
            extra_op_ou_index_to_parent_op_index.push_back(OperatorIndices(op_no,i,-1));
        }
        if (count == 0) 
            continue;
        if (forbidding_ops_to_var.find(op_no) == forbidding_ops_to_var.end()) {
            forbidding_ops_to_var[op_no] = var_id++;
            var_no_to_op_no.push_back(op_no);
        }
    }

    // Then come operators that allow for reaching the goal: o_i,j for o in forbidding_multisets[j], 0<=i< m^j_o
    int multiset_id = 0;
	for (auto fms : forbidding_multisets) {
	    for (auto e : fms) {
    	    int op_no = e.first;
		    int count = e.second;   // m^j_o
            for (int i = 0 ; i < count ; ++i) {
                // cout << "Aux action for operator " << op_no << ", index " << i << ", multiset id " << multiset_id << endl; 

                extra_op_index_to_parent_op_set_indices.push_back(OperatorIndices(op_no,i,multiset_id));
            }
        }
        multiset_id++;
    }

	// The variables include 
    //     the original ones 
    //     one variable for tracking the sets 
    //     one variable per action in the set union, counting their applications
	// Creating initial state values by copying from the parent and pushing the new variables initial values
	initial_state_values = parent->get_initial_state_values();
    initial_state_values.push_back(0);  // Extra var for tracking sets (domain size forbidding_sets.size() + 1) 
	initial_state_values.insert(initial_state_values.end(), forbidding_ops_to_var.size(), 0); // Extra var per op, tracking its application
}



int SuperMultisetsForbidReformulatedTask::get_number_appearances(int op_no) const {
    auto id = max_count.find(op_no);
    if (id == max_count.end())
        return 0;
    return id->second;
}


bool SuperMultisetsForbidReformulatedTask::is_operator_on_plans(int op_no) const {
    return forbidding_ops_to_var.find(op_no) != forbidding_ops_to_var.end();
}

const SuperMultisetsForbidReformulatedTask::OperatorIndices& SuperMultisetsForbidReformulatedTask::get_parent_op_index(int op_index) const {
	// Getting the index of the corresponding operator in the parent task, and of a set from which it came
    if (op_index < (int)extra_op_ou_index_to_parent_op_index.size()) {
        return extra_op_ou_index_to_parent_op_index[op_index];
    }
    int relative_index = op_index - (int) extra_op_ou_index_to_parent_op_index.size();
	return extra_op_index_to_parent_op_set_indices[relative_index];
}





int SuperMultisetsForbidReformulatedTask::get_op_for_var_index(int var_index) const { 
    // Getting the op_no for the relative index of the extra var that tracks the op_no
	return var_no_to_op_no[var_index]; 
}


int SuperMultisetsForbidReformulatedTask::get_set_tracking_var_index() const { 
	return parent->get_num_variables(); 
}

int SuperMultisetsForbidReformulatedTask::get_op_tracking_var_index(int op_no) const {
    auto it = forbidding_ops_to_var.find(op_no);
    assert(it != forbidding_ops_to_var.end());
    return parent->get_num_variables() + 1 + it->second;
}


// //////////////////

int SuperMultisetsForbidReformulatedTask::get_num_variables() const {
    return (int) initial_state_values.size();
}

string SuperMultisetsForbidReformulatedTask::get_variable_name(int var) const {
	if (var < parent->get_num_variables())
		return parent->get_variable_name(var);

	int relative_index = var - parent->get_num_variables();

	if (relative_index == 0)
		return "tracking_multisets";

	relative_index--;
	assert(relative_index < (int) forbidding_ops_to_var.size());
	int op_no = get_op_for_var_index(relative_index);
	string op_name = parent->get_operator_name(op_no, false);
	std::replace(op_name.begin(), op_name.end(), ' ', '_');
	return "tracking_op_" + op_name + "_" + std::to_string(relative_index);
}

int SuperMultisetsForbidReformulatedTask::get_variable_domain_size(int var) const {
	if (var < parent->get_num_variables())
		return parent->get_variable_domain_size(var);

	int relative_index = var - parent->get_num_variables();
	if (relative_index == 0)
        return (int) forbidding_multisets.size() + 1;

	relative_index--;
	assert(relative_index < max_count.size());
	int op_no = get_op_for_var_index(relative_index);

    auto it = max_count.find(op_no);
    assert(it != max_count.end());
    return it->second + 1;
}

int SuperMultisetsForbidReformulatedTask::get_variable_axiom_layer(int var) const {
	if (var < parent->get_num_variables())
		return parent->get_variable_axiom_layer(var);

	return -1;
}

int SuperMultisetsForbidReformulatedTask::get_variable_default_axiom_value(int var) const {
	if (var < parent->get_num_variables())
		return parent->get_variable_default_axiom_value(var);
	return 0;
}

string SuperMultisetsForbidReformulatedTask::get_fact_name(const FactPair &fact) const {
	if (fact.var < parent->get_num_variables())
		return parent->get_fact_name(fact);
	int rel_var = fact.var - parent->get_num_variables();
	if (rel_var == 0) {
        // The fact value corresponds to the number of sets checked so far
        return "Atom __tested_multisets_" + std::to_string(fact.value)+ "()";
    }
	return "Atom __operator_applied_" + std::to_string(fact.value) + "_times()";
}

bool SuperMultisetsForbidReformulatedTask::are_facts_mutex(const FactPair &fact1, const FactPair &fact2) const {
	if (fact1.var < parent->get_num_variables() && fact2.var < parent->get_num_variables())
		return parent->are_facts_mutex(fact1, fact2);

	if (fact1.var >= parent->get_num_variables() &&
			fact1.var == fact2.var &&
			fact1.value != fact2.value)
		return true;

	return false;
}

int SuperMultisetsForbidReformulatedTask::get_operator_cost(int index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_operator_cost(index, is_axiom);

    if (index < (int)extra_op_ou_index_to_parent_op_index.size()) {
        const OperatorIndices& ids = get_parent_op_index(index);
        return parent->get_operator_cost(ids.parent_op_no, is_axiom);
    }
    return 0;
}

string SuperMultisetsForbidReformulatedTask::get_operator_name(int index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_operator_name(index, is_axiom);

    // Getting the parent op_id and the set id 
    const OperatorIndices& ids = get_parent_op_index(index);

    if (index < (int)extra_op_ou_index_to_parent_op_index.size()) {
	    string name = parent->get_operator_name(ids.parent_op_no, is_axiom);
        if (change_operator_names) {
            return name + "__###__" + std::to_string(ids.running_id);
        }
        return name;
    }
	return "__###__goal_achieving__multiset_" + std::to_string(ids.multiset_id) + "_op_" + parent->get_operator_name(ids.parent_op_no, is_axiom) + "__index__" + std::to_string(ids.running_id);
}

int SuperMultisetsForbidReformulatedTask::get_num_operators() const {
	// The number of operators in the reformulation is number of original operators + extra goal achieving operators
    return (int) extra_op_ou_index_to_parent_op_index.size() + (int) extra_op_index_to_parent_op_set_indices.size();
}

int SuperMultisetsForbidReformulatedTask::get_num_operator_preconditions(int index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_num_operator_preconditions(index, is_axiom);

	if (index < (int)extra_op_ou_index_to_parent_op_index.size()) {
        const OperatorIndices& ids = get_parent_op_index(index);

        int is_in_multiset = is_operator_on_plans(ids.parent_op_no)  ? 1 : 0;
		return parent->get_num_operator_preconditions(ids.parent_op_no, is_axiom) + 1 + is_in_multiset;
    }
    return parent->get_num_goals() + 2;
}


FactPair SuperMultisetsForbidReformulatedTask::get_operator_precondition(
    int op_index, int fact_index, bool is_axiom) const {

	if (is_axiom)
		return parent->get_operator_precondition(op_index, fact_index, is_axiom);

	if (op_index < (int)extra_op_ou_index_to_parent_op_index.size()) {
        const OperatorIndices& ids = get_parent_op_index(op_index);

        if (fact_index < parent->get_num_operator_preconditions(ids.parent_op_no, is_axiom)) {
		    return parent->get_operator_precondition(ids.parent_op_no, fact_index, is_axiom);
        }
        // Extra pre
        if (fact_index == parent->get_num_operator_preconditions(ids.parent_op_no, is_axiom)) {
            return FactPair(get_set_tracking_var_index(),0);
        }
        // op tracking pre
        return FactPair(get_op_tracking_var_index(ids.parent_op_no),ids.running_id);
    }

    // Extra op
    // The first preconditions is the original goal
    if (fact_index < parent->get_num_goals()) {
        return parent->get_goal_fact(fact_index);
    } 

    // Getting the parent op_id and the set id 
    const OperatorIndices& ids = get_parent_op_index(op_index);
    assert(ids.multiset_id >= 0);
    // Then comes the op tracking var
    if (fact_index == parent->get_num_goals()) {
        return FactPair(get_op_tracking_var_index(ids.parent_op_no),ids.running_id);
    }

    // Then comes the set tracking var
    return FactPair(get_set_tracking_var_index(),ids.multiset_id);
}

int SuperMultisetsForbidReformulatedTask::get_num_operator_effects(int op_index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_num_operator_effects(op_index, is_axiom);

	if (op_index < (int)extra_op_ou_index_to_parent_op_index.size()) {
        const OperatorIndices& ids = get_parent_op_index(op_index);

        int is_in_set = ids.running_id < get_number_appearances(ids.parent_op_no)  ? 1 : 0;
		return parent->get_num_operator_effects(ids.parent_op_no, is_axiom) + is_in_set;
    }
    // Extra op effect
    return 1;
}

int SuperMultisetsForbidReformulatedTask::get_num_operator_effect_conditions(
    int op_index, int eff_index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_num_operator_effect_conditions(op_index, eff_index, is_axiom);
    const OperatorIndices& ids = get_parent_op_index(op_index);

	if (op_index < (int)extra_op_ou_index_to_parent_op_index.size() && 
        eff_index < parent->get_num_operator_effects(ids.parent_op_no, is_axiom)) {
        return parent->get_num_operator_effect_conditions(ids.parent_op_no, eff_index, is_axiom);
    }
	// The additional effects are unconditional
	return 0;
}

FactPair SuperMultisetsForbidReformulatedTask::get_operator_effect_condition(
    int op_index, int eff_index, int cond_index, bool is_axiom) const {
    const OperatorIndices& ids = get_parent_op_index(op_index);
    return parent->get_operator_effect_condition(ids.parent_op_no, eff_index, cond_index, is_axiom);
}

FactPair SuperMultisetsForbidReformulatedTask::get_operator_effect(
    int op_index, int eff_index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_operator_effect(op_index, eff_index, is_axiom);

	if (op_index < (int)extra_op_ou_index_to_parent_op_index.size()) {
        const OperatorIndices& ids = get_parent_op_index(op_index);
        if (eff_index < parent->get_num_operator_effects(ids.parent_op_no, is_axiom)) {        
		    return parent->get_operator_effect(ids.parent_op_no, eff_index, is_axiom);
        }
        // Possible extra effect
        return FactPair(get_op_tracking_var_index(ids.parent_op_no),ids.running_id+1);
    }
    // Extra op
    // Getting the parent op_id and the set id 
    const OperatorIndices& ids = get_parent_op_index(op_index);
    assert(ids.multiset_id >= 0);
    return FactPair(get_set_tracking_var_index(),ids.multiset_id+1);
}

int SuperMultisetsForbidReformulatedTask::get_num_goals() const {
    return parent->get_num_goals() + 1;
}

FactPair SuperMultisetsForbidReformulatedTask::get_goal_fact(int index) const {
	if (index < parent->get_num_goals())
		return parent->get_goal_fact(index);

    return FactPair(get_set_tracking_var_index(), (int) forbidding_multisets.size());
}

vector<int> SuperMultisetsForbidReformulatedTask::get_initial_state_values() const {
    return initial_state_values;
}

void SuperMultisetsForbidReformulatedTask::convert_state_values_from_parent(
    vector<int> &) const {
	ABORT("SuperMultisetsForbidReformulatedTask doesn't support getting a state from the parent state.");
}

}
