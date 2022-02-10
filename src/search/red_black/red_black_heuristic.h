#ifndef RED_BLACK_RED_BLACK_HEURISTIC_H
#define RED_BLACK_RED_BLACK_HEURISTIC_H

#include "../heuristics/ff_heuristic.h"
#include "dtg_operators.h"
#include "red_black_operator.h"
#include "../task_utils/causal_graph.h"
#include "red_black_task.h"
#include "../utils/logging.h"

#include <iostream>
#include <sstream>
#include <vector>

namespace red_black {
enum ActionApplicationResult {
    ACTION_APPLICABLE,
    ACTION_NOT_APPLICABLE,
    ACTION_SELF_LOOP
};

class RedBlackHeuristic: public ff_heuristic::FFHeuristic {
    std::vector<int> current_applicable_sequence;

    int* connected_state_buffer;
    int* black_state_buffer;

    int ff_cost;

    RedBlackTask red_black_task;
    const bool conditional_effects_task;

    typedef std::vector<std::vector<int> > ParallelRelaxedPlan;
    ParallelRelaxedPlan parallel_relaxed_plan;
    std::vector<std::vector<bool> > propositions_per_operator;
    bool applicability_status;
    bool solution_found_by_heuristic;
    const bool extract_plan;
    bool initialized;
    utils::LogProxy log;

    int *curr_state_buffer;
    void initialize();
    int get_red_black_plan_cost(const State &state);
    bool currently_op_prec_unchanged(int op_no) const;
    bool is_semi_relaxed_achieved(VariableProxy var, int val) const;
    int add_red_black_plan_suffix(int h_val);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int resolve_conflicts();
    int resolve_conflicts_disconnected();
    const std::vector<int>& get_path_for_var(VariableProxy var);
    int resolve_conflicts_DAG();
    void add_path_for_var_from_to(VariableProxy var, int from, int to, std::vector<int>& curr_sequence);
    const std::vector<int>& get_path_for_var_from_to(VariableProxy var, int from, int to);
    int get_black_prv(int op_no, VariableProxy var) const { return get_rb_sas_operator(op_no)->get_black_precondition_value(var); }

    bool is_path_achieving_action_precondition_by_step(const std::vector<int>& ops, int op_no, size_t index) const;
    bool is_path_achieving_var_val_by_step(const std::vector<int>& ops, FactProxy varval, size_t index) const;

    int get_next_action_reg(bool skip_black_pre_may_delete_red_sufficient_achieved = false);
    int get_operator_estimated_conflict_cost_black_reachability(int op_no, FactProxy eff) const;
    int get_black_fact_estimated_conflict_cost_black_reachability(FactProxy fact) const;

    bool is_red_effects_only_action(int op_no) const { return get_num_black_preconditions(op_no) == 0; }
    bool black_precondition_is_enabled(FactProxy black_pre) const;

    void update_marks() { red_black_task.update_marks_fact_following(); }
    void update_marks(int op_no) { red_black_task.update_marks_fact_following(op_no); }
    void reset_all_marks();
    void set_new_marks_for_state(const State &state);
    int get_next_action();

    std::shared_ptr<DtgOperators> get_dtg(VariableProxy v) const { return red_black_task.get_dtg(v); }
    std::shared_ptr<RedBlackOperator> get_rb_sas_operator(int op_no) const { return red_black_task.get_rb_sas_operator(op_no); }

    bool is_black(VariableProxy var) const { return red_black_task.is_black(var); }

    // Getting the number of red and black preconditions from the sas operators, no need to store them
    int get_num_black_preconditions(int op_no) const { return get_rb_sas_operator(op_no)->get_black_precondition().size(); }
    int get_num_red_preconditions(int op_no) const { return get_rb_sas_operator(op_no)->get_red_precondition().size(); }
    int get_num_red_effect_conditions(int op_no, FactProxy eff) const { return get_rb_sas_operator(op_no)->get_num_red_effect_conditions(eff); }

    bool is_black_effects_only_action(int op_no) const { return get_num_red_preconditions(op_no) == 0; }
    bool is_mixed_effects_action(int op_no) const { return (!is_red_effects_only_action(op_no) && !is_black_effects_only_action(op_no)); }
    bool is_currently_mixed_effects(int op_no) const;

    bool op_all_red_preconditions_reached(int op_no) const;
    bool op_all_red_conditions_reached(int op_no, FactProxy eff, utils::LogProxy &clog) const;
    int get_num_reached_red_preconditions(int op_no) const { return red_black_task.get_num_reached_red_preconditions(op_no); }
    int get_num_reached_red_effect_conditions(int op_no, FactProxy eff) const { return red_black_task.get_num_reached_red_effect_conditions(op_no, eff); }
    bool op_all_black_preconditions_reachable(int op_no) const;

    void mark_red_sufficient(int op_no) { red_black_task.mark_red_sufficient(op_no); }
    void mark_red_sufficient(int op_no, FactPair eff) { red_black_task.mark_red_sufficient(op_no, eff); }
    void mark_red_precondition(VariableProxy var, int val) { red_black_task.mark_red_precondition(var,val); }
    void clear_red_precondition_marks() { red_black_task.clear_red_precondition_marks(); }
    void clear_black_marks() { red_black_task.clear_black_marks(); }

    void dump_current_semi_relaxed_state(utils::LogProxy &clog, bool dump_fact = false) const;
    void dump_current_relaxed_state(utils::LogProxy &clog) const;
    void apply_action_to_current_state(int op_no);
    ActionApplicationResult apply_action_to_semi_relaxed_state(int op_no, bool check_applicability = true);
    bool effect_fires_in_semi_relaxed_state(EffectProxy eff) const;

    bool check_semi_relaxed_goal_reached_and_set_missing_black();
    bool is_semi_relaxed_goal_reached() const;

    // From SequentialRelaxedPlan
    void apply_while_possible();
    bool is_fact_true_in_current_state(FactProxy fact) const;
    bool effect_does_fire(EffectProxy eff) const;

    void get_relaxed_plan(const State &state, relaxation_heuristic::PropID goal_id);

    void remove_all_operators_from_parallel_relaxed_plan();

    void check_goal_via_state();

    bool is_op_applicable_in_current_state(OperatorProxy op) const;
    void apply_operator(OperatorProxy op);
    int get_ff_value() const { return ff_cost; }

    int compute_sequential_relaxed_plan(const State &state);
    void clear_sequential_relaxed_plan();

    void set_current_buffer_to_state(const State &state);
    utils::HashSet<FactPair> currently_not_applied_reached_red_facts;
    void add_operator_red_facts_to_currently_not_applied_reached_red_facts(int op_no);
    void clear_currently_not_applied_reached_red_facts();
    bool is_red_fact_currently_not_applied_reached(FactPair fact) const;
    bool op_is_currently_red_RB_applicable_under_currently_not_applied_reached_red_facts(int op_no) const;

protected:
    virtual int compute_heuristic(const State &ancestor_state) override;
    virtual void free_mem();
    virtual void dump_options() const;

public:
    RedBlackHeuristic(const options::Options &options);
    virtual ~RedBlackHeuristic();

    static void add_options_to_parser(OptionParser &parser);

    bool op_is_enabled(int op_no) const;
    bool op_is_currently_red_applicable(int op_no) const;
    bool op_is_currently_applicable_ignore_var(int op_no, VariableProxy var) const;
    bool is_currently_applicable(const std::vector<int>& ops, bool skip_black=false);
    bool is_currently_RB_applicable(const std::vector<int>& ops) const;

    // Michael: For checking overall applicability
    virtual bool found_solution() const override {
        return extract_plan && solution_found_by_heuristic;
    }
};
}
#endif
