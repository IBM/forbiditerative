#ifndef RED_BLACK_RED_BLACK_OPERATOR_H
#define RED_BLACK_RED_BLACK_OPERATOR_H

#include <vector>
#include <map>

#include "../task_proxy.h"
#include "../abstract_task.h"

namespace red_black {
class RedBlackOperator {
    std::vector<FactProxy> red_precondition, black_precondition;
    std::vector<EffectProxy> red_effect, black_effect;
    std::vector<FactProxy> red_precondition_not_prevail;

    std::map<int,int> pre_value_by_effect;
    utils::HashMap<FactPair, std::vector<FactProxy>> red_condition, black_condition;

    int op_no;
    bool does_fire(EffectProxy eff, int *curr_state_buffer) const;
public:
    RedBlackOperator(int _op_no, TaskProxy task_proxy);

    void set_black_pre_eff(const std::vector<bool>& black_vars);
    const std::vector<FactProxy>& get_red_precondition() const { return red_precondition;}
    const std::vector<FactProxy>& get_black_precondition() const { return black_precondition;}
    const std::vector<EffectProxy>& get_red_effect() const { return red_effect;}
    const std::vector<EffectProxy>& get_black_effect() const { return black_effect;}

    std::vector<FactProxy> get_red_condition(FactPair eff) const;
    std::vector<FactProxy> get_black_condition(FactPair eff) const;

    const std::vector<FactProxy>& get_red_precondition_not_prevail() const { return red_precondition_not_prevail;}
    int get_black_precondition_value(VariableProxy var) const;

    int get_pre_value_by_effect(EffectProxy eff) const;

    bool is_red_applicable(const int *curr_state_buffer) const;
    bool is_red_applicable(const int *curr_state_buffer, const utils::HashSet<FactPair>& extra_red_facts) const;
    bool is_applicable(const int *curr_state_buffer) const;
    bool is_applicable(const int *curr_state_buffer, const utils::HashSet<FactPair>& extra_red_facts) const;
    bool is_applicable(const State& state) const;
    void apply(int *curr_state_buffer) const;
    void dump() const;
    int get_op_no() const { return op_no; }

    int get_num_red_effect_conditions(FactProxy eff) const;
    bool has_red_conditional_effects() const { return !red_condition.empty(); }
    bool has_black_conditional_effects() const { return !black_condition.empty(); }

};

typedef std::shared_ptr<const RedBlackOperator> sas_operator;

}
#endif
