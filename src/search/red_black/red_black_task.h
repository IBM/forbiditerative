#ifndef RED_BLACK_RED_BLACK_TASK_H
#define RED_BLACK_RED_BLACK_TASK_H

#include <vector>
#include <set>
#include <cassert>
#include <time.h>
#include <cstdlib>
#include <stdio.h>
#include <iostream>

#include "red_black_operator.h"
#include "red_black_task_core.h"
#include "coloring_strategy.h"
#include "dtg_operators.h"

#include "../task_utils/causal_graph.h"

using namespace std;

namespace red_black {
class ColoringStrategy;

class RedBlackTask {
    TaskProxy task_proxy;
    ColoringStrategy coloring;
    bool dump_conflicting_conditional_effects;
    RedBlackTaskCore core;
    bool use_black_dag;

    vector<bool> almost_roots;
    vector<vector<bool> > black_dag_edges;
    // Keeping operators by pre for red variables only.
    vector<vector<vector<int> > > ops_by_pre;
    typedef pair<int, FactProxy> OperatorEffectPair;
    vector<vector<vector<OperatorEffectPair>>> ops_eff_by_pre;

    vector<VariableProxy> black_variables;
    vector<VariableProxy> red_variables;
    // For calculation of the number of reached red preconditions;
    vector<int> ops_num_reached_red_preconditions;
    typedef utils::HashMap<FactPair, int> CountByEffect;
    vector<CountByEffect> ops_num_reached_red_effect_conditions;

    vector<vector<FactProxy> > black_var_deletes;
    // Keeping operators by effect for red variables only (used for following the relaxed facts).
    vector<vector<vector<int> > > ops_by_eff;
    // For fast update of the black vars in the red fact following option
    vector<vector<VariableProxy> > blacks_by_ops;

    // Used in get_next_action_reg, set_new_marks_for_state_fact_following
    list<int> red_sufficient_unachieved;
    vector<list<int>::iterator> red_sufficient_unachieved_iterators;

    bool conditional_effects_task;

    void print_statistics() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void initialize_connected();
    void set_red_black_indices();

    const vector<int> &get_cg_predecessors(VariableProxy node) const {return coloring.get_cg_predecessors(node); }
    const vector<int> &get_cg_successors(VariableProxy node) const {return coloring.get_cg_successors(node); }
    void increment_number_reached_red_preconditions(int op_no) { ops_num_reached_red_preconditions[op_no]++; }
    void increment_number_reached_red_effect_conditions(int op_no, FactProxy eff) { ops_num_reached_red_effect_conditions[op_no][eff.get_pair()]++; }
    void keep_operators_by_effects();
    void set_black_successors_by_ops();
    std::string get_variable_name_and_domain(VariableProxy var) const { return core.get_variable_name_and_domain(var); }
    bool is_invertible(VariableProxy var) const { return core.is_invertible(var); }
    bool check_facts_disconnected(FactProxy fact1, FactProxy fact2, const vector<int>& equivalence_class_id_for_var) const;
    bool check_pre_disconnected(int op_no, const vector<int>& equivalence_class_id_for_var) const;
    bool check_goal_disconnected(const vector<int>& equivalence_class_id_for_var) const;
    bool is_leaf(VariableProxy var) const { return get_cg_successors(var).size() == 0; }
    bool is_root(VariableProxy var) const { return get_cg_predecessors(var).size() == 0; }

    void add_red_eff_condition_op_pair(int op_no, EffectProxy eff, FactProxy fact);

public:
    RedBlackTask(const options::Options &options, const AbstractTask &task);

    void initialize();
    size_t number_of_black_variables() const { return black_variables.size(); }
    VariableProxy get_black_variable(size_t index) const { return black_variables[index]; }
    bool is_black(VariableProxy var) const { return coloring.is_black(var); }
    DtgOperators* get_dtg(VariableProxy v) const { return core.get_dtg(v); }
    RedBlackOperator* get_rb_sas_operator(int op_no) const { return core.get_rb_sas_operator(op_no); }
    ConnectivityStatus get_connectivity_status(VariableProxy var) const { return core.get_connectivity_status(var); }
    const vector<int>& get_ops_by_pre(VariableProxy var, int val) const { return ops_by_pre[var.get_id()][val]; }
    const vector<OperatorEffectPair>& get_ops_eff_by_pre(VariableProxy var, int val) const { return ops_eff_by_pre[var.get_id()][val]; }
    bool is_use_connected() const { return coloring.is_use_connected(); }
    void set_use_connected(bool use) { coloring.set_use_connected(use); }
    bool is_use_black_dag() const { return use_black_dag; }
    bool is_almost_root(VariableProxy var) const { return almost_roots[var.get_id()]; }
    int get_num_reached_red_preconditions(int op_no) const { return ops_num_reached_red_preconditions[op_no]; }
    int get_num_reached_red_effect_conditions(int op_no, FactProxy eff) const;

    void mark_red_sufficient(int op_no);
    void mark_red_sufficient(int op_no, FactPair eff);

    bool operator_has_red_conditional_effects(int op_no) const { return get_rb_sas_operator(op_no)->has_red_conditional_effects(); }

    void mark_red_precondition(VariableProxy var, int val);
    void clear_red_precondition_marks();
    void clear_black_marks();

    void reset_all_marks();
    void reset_all_marks_fact_following();
    void postpone_sufficient_goals();

    void set_new_marks_for_state(const State &state);
    void set_new_marks_for_state_fact_following(const State &state);

    void update_marks_fact_following();
    void update_marks_fact_following(int );
    void precalculate_variables(bool force_computation) { coloring.precalculate_variables(force_computation); }

    void prepare_for_red_fact_following();
    void prepare_for_red_fact_following_next_red_action_test();
    bool achieving_black_pre_may_delete_achieved_red_sufficient(int op_no) const;
    const vector<int> &get_operators_by_effect(VariableProxy var, int val) const { return ops_by_eff[var.get_id()][val]; }

    const list<int>& get_red_sufficient_unachieved_variables_list_reg() const { return red_sufficient_unachieved; }

    void free_mem();
    void free_red_data();
    void prepare_operators_for_counting_achieved_preconditions();
    void dump_options() const;
    size_t get_num_invertible_vars() const { return core.get_num_invertible_vars(); }

    bool has_conditional_effects() const { return conditional_effects_task; }
};
}
#endif
