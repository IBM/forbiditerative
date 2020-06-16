#ifndef TASKS_GRAPH_FORBID_REFORMULATED_TASK_H
#define TASKS_GRAPH_FORBID_REFORMULATED_TASK_H

#include "delegating_task.h"
#include "../plans/plans_graph.h"
#include "../per_state_information.h"

#include <vector>
#include <unordered_map>

namespace extra_tasks {
struct GraphEdge {
	int op_no;
	int from;
	int to;

	GraphEdge(int _op_no, int _from, int _to) : op_no(_op_no), from(_from), to(_to) {}
};

class GraphForbidReformulatedTask : public tasks::DelegatingTask {
	plans::PlansGraph* forbidding_graph;
	bool change_operator_names;
	int num_states;
	std::unordered_map<StateID, int> state_indices;
	int add_state_index(const StateID& stateID);
	int get_state_index(const StateID& stateID) const;

	std::vector<GraphEdge> edge_ops;
	void add_edge_index(int op_no, int from_index, int to_index);
	const GraphEdge& get_edge_index(int op_index) const;

	int operators_on_graph;
	std::vector<int> initial_state_values;
	std::vector<bool> on_graph;
	std::vector<int> graph_operators_indexes;

	std::vector<std::vector<int>> from_nodes_indexes_by_parent_operator;

	//std::vector<std::vector<int>> plan_operators_indexes_by_parent_operator;

	bool is_operator_on_graph(int op_no) const;
	int get_num_operator_appearances_on_plan(int op_no) const;
	int get_plan_index_ordered(int op_no, int appearance_index) const;

//	int get_plan_op_index(int index) const;
	int get_parent_op_index(int index) const;
	int get_op_type(int index) const;

	int get_possible_var_index() const;
	int get_following_var_from_index(int op_index) const;
	int get_following_var_to_index(int op_index) const;

//	int get_num_non_plan_operators() const;
public:
	GraphForbidReformulatedTask(
        const std::shared_ptr<AbstractTask> parent,
		plans::PlansGraph* plans_graph, bool change_operator_names=false);
    virtual ~GraphForbidReformulatedTask() override = default;

    virtual int get_num_variables() const override;
    virtual std::string get_variable_name(int var) const override;
    virtual int get_variable_domain_size(int var) const override;
    virtual int get_variable_axiom_layer(int var) const override;
    virtual int get_variable_default_axiom_value(int var) const override;
    virtual std::string get_fact_name(const FactPair &fact) const override;
    virtual bool are_facts_mutex(
        const FactPair &fact1, const FactPair &fact2) const override;

    virtual int get_operator_cost(int index, bool is_axiom) const override;
    virtual std::string get_operator_name(int index, bool is_axiom) const override;
    virtual int get_num_operators() const override;
    virtual int get_num_operator_preconditions(int index, bool is_axiom) const override;
    virtual FactPair get_operator_precondition(
        int op_index, int fact_index, bool is_axiom) const override;
    virtual int get_num_operator_effects(int op_index, bool is_axiom) const override;
    virtual int get_num_operator_effect_conditions(
        int op_index, int eff_index, bool is_axiom) const override;
    virtual FactPair get_operator_effect_condition(
        int op_index, int eff_index, int cond_index, bool is_axiom) const override;
    virtual FactPair get_operator_effect(
        int op_index, int eff_index, bool is_axiom) const override;
    virtual const GlobalOperator *get_global_operator(int index, bool is_axiom) const override;

    virtual int get_num_goals() const override;
    virtual FactPair get_goal_fact(int index) const override;

    virtual std::vector<int> get_initial_state_values() const override;
    virtual void convert_state_values_from_parent(
        std::vector<int> &values) const override;

};
}

#endif
