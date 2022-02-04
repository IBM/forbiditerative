#include "red_black_operator.h"
#include "../utils/logging.h"

#include <map>

using namespace std;

namespace red_black {
RedBlackOperator::RedBlackOperator(int _op_no, TaskProxy task_proxy) : op_no(_op_no) {
    // Filling all pre and effects as red
    vector<int> pre_by_var(task_proxy.get_variables().size(), -1);
    OperatorProxy op = task_proxy.get_operators()[op_no];
    PreconditionsProxy preconditions = op.get_preconditions();
    for (FactProxy fact : preconditions) {
        red_precondition.push_back(fact);
        pre_by_var[fact.get_variable().get_id()] = fact.get_value();
    }
    EffectsProxy effects = op.get_effects();
    for (EffectProxy eff : effects) {
        red_effect.push_back(eff);
        VariableProxy var = eff.get_fact().get_variable();
        pre_value_by_effect.insert(pair<int, int>(var.get_id(), pre_by_var[var.get_id()]));

        EffectConditionsProxy conditions = eff.get_conditions();
        for (FactProxy cond : conditions) {
            red_condition[eff.get_fact().get_pair()].push_back(cond);
        }
    }
}

int RedBlackOperator::get_pre_value_by_effect(EffectProxy eff) const {
    map<int,int>::const_iterator it = pre_value_by_effect.find(eff.get_fact().get_variable().get_id());
    if (it != pre_value_by_effect.end())
        return it->second;
    return -1;
}

static bool compare_fact_proxy(const FactProxy &f1, const FactProxy &f2) {
    if (f1.get_variable() != f2.get_variable())
        return f1.get_variable().get_id() < f2.get_variable().get_id();
    return f1.get_value() < f2.get_value();
}

static bool compare_effect_proxy(const EffectProxy &f1, const EffectProxy &f2) {
    return compare_fact_proxy(f1.get_fact(), f2.get_fact());
}


void RedBlackOperator::set_black_pre_eff(const vector<bool>& black_vars) {
    // Separating black effects from red
    vector<FactProxy> temp_prec;
    map<int,FactProxy> prec_per_var;

    temp_prec.swap(red_precondition);
    for (FactProxy fact : temp_prec) {
        VariableProxy var = fact.get_variable();
        if (black_vars[var.get_id()]) {
            black_precondition.push_back(fact);
        } else {
            red_precondition.push_back(fact);
            prec_per_var.insert(pair<int, FactProxy>(var.get_id(), fact));
        }
    }
    vector<EffectProxy> temp_eff;
    temp_eff.swap(red_effect);
    for (EffectProxy eff : temp_eff) {
        VariableProxy var = eff.get_fact().get_variable();
        if (black_vars[var.get_id()]) {
            black_effect.push_back(eff);
        } else {
            red_effect.push_back(eff);
            map<int,FactProxy>::iterator it = prec_per_var.find(var.get_id());
            if (it != prec_per_var.end()) {
                red_precondition_not_prevail.push_back(it->second);
            }
        }
    }
    utils::HashMap<FactPair, vector<FactProxy>> temp_cond;
    temp_cond.swap(red_condition);
    for (auto& e : temp_cond) {
        FactPair eff = e.first;
        vector<FactProxy>& conditions = e.second;
        for (FactProxy cond : conditions) {
            VariableProxy var = cond.get_variable();
            if (black_vars[var.get_id()]) {
                black_condition[eff].push_back(cond);
            } else {
                red_condition[eff].push_back(cond);
            }
        }
    }

    // Michael (Nov 2017): Ordering all the vectors
    sort(red_precondition.begin(), red_precondition.end(), compare_fact_proxy);
    sort(black_precondition.begin(), black_precondition.end(), compare_fact_proxy);
    sort(red_precondition_not_prevail.begin(), red_precondition_not_prevail.end(), compare_fact_proxy);

    sort(red_effect.begin(), red_effect.end(), compare_effect_proxy);
    sort(black_effect.begin(), black_effect.end(), compare_effect_proxy);

    for (std::pair<FactPair, vector<FactProxy>> elem : red_condition) {
        sort(elem.second.begin(), elem.second.end(), compare_fact_proxy);
    }
    for (std::pair<FactPair, vector<FactProxy>> elem : black_condition) {
        sort(elem.second.begin(), elem.second.end(), compare_fact_proxy);
    }
}


std::vector<FactProxy> RedBlackOperator::get_red_condition(FactPair eff) const {
    utils::HashMap<FactPair, std::vector<FactProxy>>::const_iterator it = red_condition.find(eff);
    return (it == red_condition.end()) ? std::vector<FactProxy>() : it->second;
}

std::vector<FactProxy> RedBlackOperator::get_black_condition(FactPair eff) const {
    utils::HashMap<FactPair, std::vector<FactProxy>>::const_iterator it = black_condition.find(eff);
    return (it == black_condition.end()) ? std::vector<FactProxy>() : it->second;
}

int RedBlackOperator::get_num_red_effect_conditions(FactProxy eff) const {
    utils::HashMap<FactPair, std::vector<FactProxy>>::const_iterator it = red_condition.find(eff.get_pair());
    return (it == red_condition.end()) ? 0 : it->second.size();
}


int RedBlackOperator::get_black_precondition_value(VariableProxy var) const {
    // TODO: Reimplement in a way more suitable for frequent use
    // We could use pre_by_var vector

    for (FactProxy fact : black_precondition) {
        if (var == fact.get_variable())
            return fact.get_value();
    }
    return -1;
}

bool RedBlackOperator::is_red_applicable(const int *curr_state_buffer) const {
    for (FactProxy fact : red_precondition) {
        if (curr_state_buffer[fact.get_variable().get_id()] != fact.get_value())
            return false;
    }
    return true;
}

bool RedBlackOperator::is_red_applicable(const int *curr_state_buffer, const utils::HashSet<FactPair>& extra_red_facts) const {
    for (FactProxy fact : red_precondition) {
        if (extra_red_facts.find(fact.get_pair()) == extra_red_facts.end() && curr_state_buffer[fact.get_variable().get_id()] != fact.get_value())
            return false;
    }
    return true;
}


bool RedBlackOperator::is_applicable(const int *curr_state_buffer) const {
    if (!is_red_applicable(curr_state_buffer))
        return false;

    for (FactProxy fact : black_precondition) {
        if (curr_state_buffer[fact.get_variable().get_id()] != fact.get_value())
            return false;
    }
    return true;
}

bool RedBlackOperator::is_applicable(const int *curr_state_buffer, const utils::HashSet<FactPair>& extra_red_facts) const {
    if (!is_red_applicable(curr_state_buffer, extra_red_facts))
        return false;

    for (FactProxy fact : black_precondition) {
        if (curr_state_buffer[fact.get_variable().get_id()] != fact.get_value())
            return false;
    }
    return true;
}

bool RedBlackOperator::is_applicable(const State& state) const {
    for (FactProxy fact : red_precondition) {
        if (state[fact.get_variable()].get_value() != fact.get_value())
            return false;
    }
    for (FactProxy fact : black_precondition) {
        if (state[fact.get_variable()].get_value() != fact.get_value())
            return false;
    }
    return true;
}

bool RedBlackOperator::does_fire(EffectProxy eff, int *curr_state_buffer) const {
    EffectConditionsProxy conditions = eff.get_conditions();
    for (FactProxy fact : conditions) {
        if (curr_state_buffer[fact.get_variable().get_id()] != fact.get_value())
            return false;
    }
    return true;
}

void RedBlackOperator::apply(int *curr_state_buffer) const {
    vector<EffectProxy> firing_effects;
    for (EffectProxy eff : red_effect) {
        if (does_fire(eff, curr_state_buffer))
            firing_effects.push_back(eff);
    }
    for (EffectProxy eff : black_effect) {
        if (does_fire(eff, curr_state_buffer))
            firing_effects.push_back(eff);
    }
    for (EffectProxy eff : firing_effects) {
        FactProxy fact = eff.get_fact();
        curr_state_buffer[fact.get_variable().get_id()] = fact.get_value();
    }
}

void RedBlackOperator::dump() const {
    utils::g_log << "< red: ";
    for (FactProxy fact : red_precondition) {
        utils::g_log << " [" << fact.get_name() <<"]";
    }
    utils::g_log << ", black: ";
    for (FactProxy fact : black_precondition) {
        utils::g_log << " [" << fact.get_name() <<"]";
    }
    utils::g_log << " | red: ";
    for (EffectProxy eff : red_effect) {
        utils::g_log << " [";
        EffectConditionsProxy conditions = eff.get_conditions();
        for (FactProxy fact : conditions) {
            utils::g_log << " " << fact.get_name();
        }
        utils::g_log << " => " << eff.get_fact().get_name() << "]";
    }
    utils::g_log << ", black: ";
    for (EffectProxy eff : black_effect) {
        utils::g_log << " [";
        EffectConditionsProxy conditions = eff.get_conditions();
        for (FactProxy fact : conditions) {
            utils::g_log << " " << fact.get_name();
        }
        utils::g_log << " => " << eff.get_fact().get_name() << "]";
    }
    utils::g_log << " >" << endl;
}
}
