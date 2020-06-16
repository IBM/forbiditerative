#ifndef GLOBAL_OPERATOR_H
#define GLOBAL_OPERATOR_H

#include "global_state.h"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

struct GlobalCondition {
    int var;
    int val;
    explicit GlobalCondition(std::istream &in);
    GlobalCondition(int variable, int value, bool check_facts=true);

    bool is_applicable(const GlobalState &state) const {
        return state[var] == val;
    }

    bool operator==(const GlobalCondition &other) const {
        return var == other.var && val == other.val;
    }

    bool operator!=(const GlobalCondition &other) const {
        return !(*this == other);
    }

    void dump_SAS(std::ofstream& os) const {
        os << var << " " << val << std::endl;
    }

    void dump() const;
};

struct GlobalEffect {
    int var;
    int val;
    std::vector<GlobalCondition> conditions;
    explicit GlobalEffect(std::istream &in);
    GlobalEffect(int variable, int value, const std::vector<GlobalCondition> &conds, bool check_facts=true);

    bool does_fire(const GlobalState &state) const {
        for (size_t i = 0; i < conditions.size(); ++i)
            if (!conditions[i].is_applicable(state))
                return false;
        return true;
    }

    void dump() const;
};

class GlobalOperator {
    bool is_an_axiom;
    std::vector<GlobalCondition> preconditions;
    std::vector<GlobalEffect> effects;
    std::string name;
    int cost;

    void read_pre_post(std::istream &in);
    int get_reformulated_op_type(const std::vector<GlobalCondition>& extra_pre, const std::vector<GlobalEffect>& extra_eff) const;

public:
    explicit GlobalOperator(std::istream &in, bool is_axiom);
    void dump() const;
    const std::string &get_name() const {return name; }

    bool is_axiom() const {return is_an_axiom; }

    const std::vector<GlobalCondition> &get_preconditions() const {return preconditions; }
    const std::vector<GlobalEffect> &get_effects() const {return effects; }

    bool is_applicable(const GlobalState &state) const {
        for (size_t i = 0; i < preconditions.size(); ++i)
            if (!preconditions[i].is_applicable(state))
                return false;
        return true;
    }

    void dump_SAS(std::ofstream& os, const std::vector<GlobalCondition>& extra_pre,
		    const std::vector<GlobalEffect>& extra_eff) const;
    void dump_pre_post_SAS(std::ofstream& os, int pre, GlobalEffect eff) const;

    int get_cost() const {return cost; }
};

extern int get_op_index_hacked(const GlobalOperator *op);

#endif
