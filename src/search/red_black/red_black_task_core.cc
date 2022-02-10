#include "red_black_task_core.h"
#include "../option_parser.h"

using namespace std;

namespace red_black {
RedBlackTaskCore::RedBlackTaskCore(const std::shared_ptr<AbstractTask> task, utils::LogProxy &log) :
                task_proxy(*task),
                num_invertible_vars(0),
                log(log) {
    // Creating the dtgs for all variables
    create_extended_DTGs(task);
}

// initialization
void RedBlackTaskCore::initialize() {
    log << "Initializing Red-Black task core..." << endl;

    // Preparing DTGs and operators for further evaluation of invertibility and other criteria
    prepare_DTGs_for_invertibility_check();
    // Checking invertibility here, move to the appropriate place later (probably to the parser)
    check_invertibility();

    log << "Number of invertible variables is " << get_num_invertible_vars() << endl;
    if (get_num_invertible_vars() > 0) {
        if (log.is_at_least_debug()) {
            // Dumping the black causal graph
            log << "Invertible causal graph:" << endl;
            VariablesProxy variables = task_proxy.get_variables();
            for (VariableProxy var : variables) {
                log << "Variable: " << var.get_name()  << (is_invertible(var) ? "" : " not") <<
                        " invertible, successors: " << endl;

                for (int succ_id : task_proxy.get_causal_graph().get_successors(var.get_id())) {
                    VariableProxy succ_var = variables[succ_id];
                    log << succ_var.get_name() << (is_invertible(succ_var) ? "" : " not") <<
                            " invertible" << endl;
                }
            }
        }

        // Setting goal values for all vars
        GoalsProxy goals = task_proxy.get_goals();
        for (FactProxy fact : goals) {
            get_dtg(fact.get_variable())->set_goal_val(fact.get_value());
        }

        check_connectivity();

        free_initial_data();
    }
    log << "Finished initializing Red-Black task core" << endl;
}

void RedBlackTaskCore::free_mem() {
    // for (std::shared_ptr<DtgOperators> dtg : dtgs_by_transition) {
    //     if (dtg)
    //         delete dtg;
    // }
    dtgs_by_transition.clear();
    invertible_vars.clear();
    connectivity_status.clear();
}

void RedBlackTaskCore::create_extended_DTGs(const std::shared_ptr<AbstractTask> task) {
    log << "Initializing extended DTGs" << endl;
    // Creating the extended DTGs from actions

    VariablesProxy variables = task_proxy.get_variables();
    for (VariableProxy var : variables) {
        dtgs_by_transition.push_back(make_shared<DtgOperators>(var.get_id(), task, log));
    }
}

void RedBlackTaskCore::prepare_DTGs_for_invertibility_check() {
    // For now, setting the operators by preconditions for all variables
    log << "Adding " << task_proxy.get_operators().size() << " operators to extended DTGs" << endl;
    // Step 2: Add ops by transition.
    red_black_sas_operators.resize(task_proxy.get_operators().size());
    for (size_t op_no = 0; op_no < task_proxy.get_operators().size(); ++op_no) {
        shared_ptr<RedBlackOperator> rb_sas_op = make_shared<RedBlackOperator>(op_no, task_proxy);
        red_black_sas_operators[op_no] = rb_sas_op;
        // Adding operator index to each proposition of the precondition

//        log << op_no << ": Created SAS operator" << endl;
        // Here, all effects are still red
        for (EffectProxy eff : rb_sas_op->get_red_effect()) {
            VariableProxy var = eff.get_fact().get_variable();
            int pre_value = rb_sas_op->get_pre_value_by_effect(eff);
            int post_value = eff.get_fact().get_value();

            int pre_value_min, pre_value_max;
            if (pre_value == -1) {
                pre_value_min = 0;
                pre_value_max = var.get_domain_size();
            } else {
                pre_value_min = pre_value;
                pre_value_max = pre_value + 1;
            }
            for (int value = pre_value_min; value < pre_value_max; ++value) {
                get_dtg(var)->add_operator_from_to(value, post_value, rb_sas_op, eff);
            }
        }
    }
}

void RedBlackTaskCore::check_invertibility() {
    log << "Checking invertibility..." << endl;
    invertible_vars.assign(task_proxy.get_variables().size(), false);
    VariablesProxy variables = task_proxy.get_variables();
    for (VariableProxy var : variables) {
        if (get_dtg(var)->check_invertibility(log)) {
            invertible_vars[var.get_id()] = true;
            num_invertible_vars++;
        }
    }
    log << "Done checking invertibility" << endl;
}

void RedBlackTaskCore::check_connectivity() {
    log << "Checking connectivity..." << endl;
    VariablesProxy variables = task_proxy.get_variables();
    for (VariableProxy var : variables) {
        /*
        if (is_leaf(var_no)) { // Skipping the leafs
            connectivity_status.push_back(NEITHER);
            continue;
        }
        */
        connectivity_status.push_back(get_dtg(var)->check_connectivity());
    }
    log << "Done checking connectivity" << endl;
}

void RedBlackTaskCore::free_initial_data() {
    VariablesProxy variables = task_proxy.get_variables();
    for (VariableProxy var : variables) {
        get_dtg(var)->clear_initial_data();
    }
}


//////////////////////////////////////////////////////////////////////////////

std::string RedBlackTaskCore::get_variable_name_and_domain(VariableProxy var) const {
        std::stringstream ss;

//        ss << "[" <<var.get_id() << "] " << var.get_name() << ": " ;
        ss << "{ " ;
        ss << var.get_fact(0).get_name();

        for (int f = 1; f < var.get_domain_size(); ++f)
                ss << ", "<< var.get_fact(f).get_name();

        ss << " }";
        return ss.str();
}
}
