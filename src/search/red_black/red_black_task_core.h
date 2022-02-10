#ifndef RED_BLACK_RED_BLACK_TASK_CORE_H
#define RED_BLACK_RED_BLACK_TASK_CORE_H

#include "red_black_operator.h"
#include "dtg_operators.h"
#include "../task_utils/causal_graph.h"
#include "../task_proxy.h"
#include "../option_parser.h"

#include <vector>

namespace red_black {
class RedBlackTaskCore {
    TaskProxy task_proxy;
    std::vector<std::shared_ptr<DtgOperators>> dtgs_by_transition;
    // Keeping sas operators for faster checks
    std::vector<std::shared_ptr<RedBlackOperator>> red_black_sas_operators;
    //TODO: Check if can be freed after initialization
    std::vector<ConnectivityStatus> connectivity_status;

    std::vector<bool> invertible_vars;  // Keeps invertible variables until black variables are set
    size_t num_invertible_vars;
    utils::LogProxy log;

    void create_extended_DTGs(const std::shared_ptr<AbstractTask> task);
    void prepare_DTGs_for_invertibility_check();
    void check_invertibility();
    void check_connectivity();
    void free_initial_data();

public:
    RedBlackTaskCore(const std::shared_ptr<AbstractTask> task, utils::LogProxy &log);

    void initialize();
    std::shared_ptr<DtgOperators> get_dtg(VariableProxy var) const { return dtgs_by_transition[var.get_id()]; }
    std::shared_ptr<RedBlackOperator> get_rb_sas_operator(int op_no) const { return red_black_sas_operators[op_no]; }

    // Called from RedBlackHeuristic
    void free_mem();
    ConnectivityStatus get_connectivity_status(VariableProxy var) const { return connectivity_status[var.get_id()]; }
    size_t get_num_invertible_vars() const { return num_invertible_vars; }
    bool is_invertible(VariableProxy var) const { return invertible_vars[var.get_id()]; }

    std::string get_variable_name_and_domain(VariableProxy var) const;
};
}
#endif
