
#ifndef TASKS_SUPERSETS_FORBID_REFORMULATED_TASK_H
#define TASKS_SUPERSETS_FORBID_REFORMULATED_TASK_H

#include "delegating_task.h"

#include <vector>
#include <unordered_map>
#include <unordered_set>

/*
  The class reformulates a planning task to forbid any plan that, treated as a
  set, is a superset of one of the given sets. 

  This is done by tracking the applications for each action in each of the given
  sets, allowing to reach a modified goal only if each of these sets has at
  least one action that was not applied.
 */

namespace extra_tasks {
class SupersetsForbidReformulatedTask : public tasks::DelegatingTask {
    struct OperatorIndices {
        int parent_op_no;
        int set_id;

        OperatorIndices(int _parent_op_no, int _set_id) 
                : parent_op_no(_parent_op_no), set_id(_set_id) {}
    };
        
    const std::vector<std::unordered_set<int>> forbidding_sets;

	std::unordered_map<int,int> forbidding_ops_to_var; // keep relative var id for each op in the forbidding sets
    std::vector<OperatorIndices> extra_op_index_to_parent_op_set_indices;
    std::vector<int> var_no_to_op_no;
	std::unordered_map<int,std::vector<int>> forbidding_ops_to_sets; // keep set ids for each op in the forbidding sets

	std::vector<int> initial_state_values;

    bool is_operator_on_plans(int op_no) const;
    int get_num_sets_for_op(int op_no) const;

	const OperatorIndices& get_parent_op_index(int index) const;
    int get_op_for_var_index(int var_index) const;

	int get_set_tracking_var_index() const;
	int get_op_tracking_var_index(int op_no) const;
    int get_sets_tracking_var_index(int set_index) const;

public:
	SupersetsForbidReformulatedTask(
        const std::shared_ptr<AbstractTask> parent,
        std::vector<std::unordered_set<int>>& sets);
    virtual ~SupersetsForbidReformulatedTask() override = default;

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

    virtual int get_num_goals() const override;
    virtual FactPair get_goal_fact(int index) const override;

    virtual std::vector<int> get_initial_state_values() const override;
    virtual void convert_state_values_from_parent(
        std::vector<int> &values) const override;

};
}

#endif
