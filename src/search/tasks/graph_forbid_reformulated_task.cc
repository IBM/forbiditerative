#include "graph_forbid_reformulated_task.h"

#include <cassert>

#include "../utils/system.h"

using namespace std;


namespace extra_tasks {
GraphForbidReformulatedTask::GraphForbidReformulatedTask(
    const shared_ptr<AbstractTask> parent,
	plans::PlansGraph* plans_graph, bool change_operator_names)
    : DelegatingTask(parent),
	  forbidding_graph(plans_graph),
	  change_operator_names(change_operator_names),
	  num_states(0),
	  operators_on_graph(0) {

	// Numbering the nodes - these correspond to additional variables

	for (const StateID& from : forbidding_graph->get_nodes()) {
		int from_index = add_state_index(from);
		for (auto edge : forbidding_graph->get_outgoing_edges(from)) {
			const StateID& to = edge.second;
			int to_index = add_state_index(to);
			int op_no = edge.first;
			add_edge_index(op_no, from_index, to_index);
		}
	}
	// Keep for each operator the nodes in which it is applied
	from_nodes_indexes_by_parent_operator.assign(parent->get_num_operators(), vector<int>());

	// Compute the indexes for new operators
	// First go all operators once -  not on the graph with index 0 and on the graph with index 1.
	// Then go all ops that are on graph with index 2, and then all ops for edges with index 3.
	on_graph.assign(parent->get_num_operators(), false);
	for (auto it : forbidding_graph->get_origin_states_by_operators()) {
		int op_no = it.first;
		if (!on_graph[op_no]) {
			operators_on_graph++;
			on_graph[op_no] = true;
			graph_operators_indexes.push_back(op_no);
		}
		for (const StateID& from_node : it.second) {
			from_nodes_indexes_by_parent_operator[op_no].push_back(get_state_index(from_node));
		}
	}

	// Creating initial state values by copying from the parent and pushing the new variables initial values
	initial_state_values = parent->get_initial_state_values();
	initial_state_values.push_back(1);
	initial_state_values.insert(initial_state_values.end(), num_states, 0);
	// Setting initial state value to 1
	int initial_state_index = parent->get_num_variables() + 1 + get_state_index(forbidding_graph->get_initial_state_id());
	initial_state_values[initial_state_index] = 1;
	cout << "Initial state var index: " << initial_state_index << endl;
}

int GraphForbidReformulatedTask::add_state_index(const StateID& stateID) {
	auto it = state_indices.find(stateID);
	if (it == state_indices.end()) {
		state_indices.insert(std::pair<StateID, int>(stateID, num_states));
		num_states++;
		return num_states - 1;
	}
	return it->second;
}

int GraphForbidReformulatedTask::get_state_index(const StateID& stateID) const {
	auto it = state_indices.find(stateID);
	if (it == state_indices.end()) {
		return -1;
	}
	return it->second;
}

void GraphForbidReformulatedTask::add_edge_index(int op_no, int from_index, int to_index) {
	edge_ops.push_back(GraphEdge(op_no, from_index, to_index));
	//cout << "edge: (" << from_index << ", " << op_no << ", " << to_index << ")" << endl;
}

const GraphEdge& GraphForbidReformulatedTask::get_edge_index(int op_index) const {
	return edge_ops[op_index];
}

bool GraphForbidReformulatedTask::is_operator_on_graph(int op_no) const {
	return on_graph[op_no];
}

int GraphForbidReformulatedTask::get_num_operator_appearances_on_plan(int op_no) const {
	return from_nodes_indexes_by_parent_operator[op_no].size();
}

int GraphForbidReformulatedTask::get_plan_index_ordered(int op_no, int appearance_index) const {
	return from_nodes_indexes_by_parent_operator[op_no][appearance_index];
}

int GraphForbidReformulatedTask::get_parent_op_index(int index) const {
	// Getting the index of the corresponding operator in the parent task
	// First go all the operators from the parent task (type 0 for non-graph operators, type 1 for graph opreators)
	// Then all the graph operators (no repetitions), with type 2
	// Then all the graph edges (operators with repetitions), with type 3
	int num_parent_ops = parent->get_num_operators();
	if (index < num_parent_ops)
		return index;
	int relative_index = index - num_parent_ops;
	if (relative_index < operators_on_graph)
		return graph_operators_indexes[relative_index];

	relative_index -= operators_on_graph;
	assert(relative_index >= 0 && relative_index < edge_ops.size());

	return get_edge_index(relative_index).op_no;
}

int GraphForbidReformulatedTask::get_op_type(int index) const {
	// Returns type 0 for operators not on plan
	//         type 1 for operators "already discarded"
	//         type 2 for operators "discarding pi"
	//         type 3 for operators "following pi"
	int num_parent_ops = parent->get_num_operators();
	if (index < num_parent_ops) {
		return is_operator_on_graph(index);
	}

	int relative_index = index - num_parent_ops;
	if (relative_index < operators_on_graph)
		return 2;

	return 3;
}

int GraphForbidReformulatedTask::get_possible_var_index() const {
	return parent->get_num_variables();
}

int GraphForbidReformulatedTask::get_following_var_from_index(int op_index) const {
	// This is valid for type 3 only.
	assert(get_op_type(op_index) == 3);
	int relative_index = op_index - parent->get_num_operators() - operators_on_graph;
	return parent->get_num_variables() + 1 + get_edge_index(relative_index).from;
}

int GraphForbidReformulatedTask::get_following_var_to_index(int op_index) const {
	// This is valid for type 3 only.
	assert(get_op_type(op_index) == 3);
	int relative_index = op_index - parent->get_num_operators() - operators_on_graph;
	return parent->get_num_variables() + 1 + get_edge_index(relative_index).to;
}

int GraphForbidReformulatedTask::get_num_variables() const {
    return parent->get_num_variables() + num_states + 1 ;
}

string GraphForbidReformulatedTask::get_variable_name(int var) const {
	if (var < parent->get_num_variables())
		return parent->get_variable_name(var);
	if (var == parent->get_num_variables())
		return "possible";
	int ind = var - parent->get_num_variables() - 1;
	return "following" + std::to_string(ind);
}

int GraphForbidReformulatedTask::get_variable_domain_size(int var) const {
	if (var < parent->get_num_variables())
		return parent->get_variable_domain_size(var);

	return 2;
}

int GraphForbidReformulatedTask::get_variable_axiom_layer(int var) const {
	if (var < parent->get_num_variables())
		return parent->get_variable_axiom_layer(var);

	return -1;
}

int GraphForbidReformulatedTask::get_variable_default_axiom_value(int var) const {
	if (var < parent->get_num_variables())
		return parent->get_variable_default_axiom_value(var);
	if (var <= parent->get_num_variables() + 1)
		return 1;
	return 0;
}

string GraphForbidReformulatedTask::get_fact_name(const FactPair &fact) const {
	if (fact.var < parent->get_num_variables())
		return parent->get_fact_name(fact);
	assert(fact.value >= 0 && fact.value <= 1);

	string ret = "";
    if (fact.value == 0)
        ret += "NegatedAtom ";
    else
        ret += "Atom ";

    if (fact.var == parent->get_num_variables())
        return ret + "__reformulation_possible()";

    int ind = fact.var - parent->get_num_variables() - 1;
    return ret + "__reformulation_following" + std::to_string(ind) + "()";
}

bool GraphForbidReformulatedTask::are_facts_mutex(const FactPair &fact1, const FactPair &fact2) const {
	if (fact1.var < parent->get_num_variables() && fact2.var < parent->get_num_variables())
		return parent->are_facts_mutex(fact1, fact2);

	if (fact1.var >= parent->get_num_variables() &&
			fact1.var == fact2.var &&
			fact1.value != fact2.value)
		return true;

	return false;
//    ABORT("GraphForbidReformulatedTask doesn't support querying mutexes.");
}

int GraphForbidReformulatedTask::get_operator_cost(int index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_operator_cost(index, is_axiom);

	int parent_op_index = get_parent_op_index(index);
	return parent->get_operator_cost(parent_op_index, is_axiom);
}

string GraphForbidReformulatedTask::get_operator_name(int index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_operator_name(index, is_axiom);

	int parent_op_index = get_parent_op_index(index);
	string name = parent->get_operator_name(parent_op_index, is_axiom);
	if (change_operator_names) {
		int op_type = get_op_type(index);
		return name + "__###__" + std::to_string(index)+ "__###__" + std::to_string(op_type);
	}
	return name;
}

int GraphForbidReformulatedTask::get_num_operators() const {
	// The number of operators according to the fixed reformulation is
	// number of operators + number of operators on the plan + plan length (number of operators on the plan with repetitions)
	// First, we have all parent operators (non-plan operators of type 0 and plan operators of type 1),
	// then operators of type 2 (no repetition),
	// and finally operators of type 3 (with repetitions).

    return parent->get_num_operators() + operators_on_graph + edge_ops.size();
}

int GraphForbidReformulatedTask::get_num_operator_preconditions(int index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_num_operator_preconditions(index, is_axiom);

	int op_type = get_op_type(index);
	assert(op_type >= 0 && op_type <= 3);

	int parent_op_index = get_parent_op_index(index);
	if (op_type == 0)
		return parent->get_num_operator_preconditions(parent_op_index, is_axiom);

	if (op_type == 1)
		return parent->get_num_operator_preconditions(parent_op_index, is_axiom) + 1;

	if (op_type == 2)
		return parent->get_num_operator_preconditions(parent_op_index, is_axiom) + 1 + get_num_operator_appearances_on_plan(parent_op_index);

	assert(op_type == 3);
	return parent->get_num_operator_preconditions(parent_op_index, is_axiom) + 2;
}

FactPair GraphForbidReformulatedTask::get_operator_precondition(
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
		int relative_fact_index = fact_index - parent_num_pre - 1;
		int op_on_plan_index = parent->get_num_variables() + 1
				+ get_plan_index_ordered(parent_op_index, relative_fact_index);
		return FactPair(op_on_plan_index, 0);
	}
	assert(op_type == 3);
	return FactPair(get_following_var_from_index(op_index), 1);
}

int GraphForbidReformulatedTask::get_num_operator_effects(int op_index, bool is_axiom) const {
	if (is_axiom)
		return parent->get_num_operator_effects(op_index, is_axiom);

	int op_type = get_op_type(op_index);
	assert(op_type >= 0 && op_type <= 3);
	int parent_op_index = get_parent_op_index(op_index);
	if (op_type == 0 || op_type == 2)
		return parent->get_num_operator_effects(parent_op_index, is_axiom) + 1;

	if (op_type == 1)
		return parent->get_num_operator_effects(parent_op_index, is_axiom);

	assert(op_type == 3);
	return parent->get_num_operator_effects(parent_op_index, is_axiom) + 2;
}

int GraphForbidReformulatedTask::get_num_operator_effect_conditions(
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

FactPair GraphForbidReformulatedTask::get_operator_effect_condition(
    int op_index, int eff_index, int cond_index, bool is_axiom) const {
	int parent_op_index = op_index;
	if (!is_axiom)
		parent_op_index = get_parent_op_index(op_index);
	return parent->get_operator_effect_condition(parent_op_index, eff_index, cond_index, is_axiom);
}

FactPair GraphForbidReformulatedTask::get_operator_effect(
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

	if (eff_index == parent_num_effs)
		return FactPair(get_following_var_from_index(op_index), 0);
	return FactPair(get_following_var_to_index(op_index), 1);
}

const GlobalOperator *GraphForbidReformulatedTask::get_global_operator(int , bool ) const {
	ABORT("GraphForbidReformulatedTask doesn't support getting the operator explicitly.");
	return nullptr;
}

int GraphForbidReformulatedTask::get_num_goals() const {
    return parent->get_num_goals() + 1;
}

FactPair GraphForbidReformulatedTask::get_goal_fact(int index) const {
	if (index < parent->get_num_goals())
		return parent->get_goal_fact(index);

    return FactPair(get_possible_var_index(), 0);
}

vector<int> GraphForbidReformulatedTask::get_initial_state_values() const {
    return initial_state_values;
}

void GraphForbidReformulatedTask::convert_state_values_from_parent(
    vector<int> &) const {
	ABORT("GraphForbidReformulatedTask doesn't support getting a state from the parent state.");
}

}
