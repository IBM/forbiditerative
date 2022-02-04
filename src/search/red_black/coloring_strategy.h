#ifndef RED_BLACK_COLORING_STRATEGY_H
#define RED_BLACK_COLORING_STRATEGY_H

#include <vector>
#include <cassert>
#include <time.h>
#include <cstdlib>
#include <stdio.h>
#include <iostream>

#include "red_black_task_core.h"
#include "red_black_operator.h"
#include "dtg_operators.h"
#include "../utils/rng.h"
#include "../task_utils/causal_graph.h"
#include "../task_proxy.h"

using namespace std;

namespace red_black {

enum BlackDAG {
    GREEDY_LEVEL,
    FROM_COLORING,
    FALSE
};

class ColoringStrategy {
    TaskProxy task_proxy;
    RedBlackTaskCore* red_black_task_core;
    int number_of_black_variables;
    BlackDAG black_dag;
    bool shortest_paths_calculated;
    bool set_conflicting_to_red;
    const utils::Verbosity verbosity;
    bool use_connected;

    vector<bool> black_vars;
    int get_number_of_black_variables() const { return number_of_black_variables; }
    void recompute_number_of_black_variables();


    DtgOperators* get_dtg(VariableProxy v) const { return red_black_task_core->get_dtg(v); }
    shared_ptr<RedBlackOperator> get_rb_sas_operator(int op_no) const { return red_black_task_core->get_rb_sas_operator(op_no); }
    bool is_invertible(VariableProxy var) const { return red_black_task_core->is_invertible(var); }
    size_t get_num_invertible_vars() const { return red_black_task_core->get_num_invertible_vars(); }

    void set_black_variables_vertex_cover(std::vector<int>& red_variables);

    void precalculate_shortest_paths_for_var(VariableProxy var, bool force_computation);
    void set_variables_order_by_level_heuristic(vector<int>& order);
    void set_variables_order_for_vertex_cover(vector<int>& order);
    void paint_red_by_vertex_cover(vector<int>& order, int* elements, vector<int>& red_variables);
    void set_black_variables_while_DAG();
    void set_DAG_blacks(vector<bool>& blacks, const vector<vector<int> >& bidirectional_edges);
    int get_DAG_next_node_level(const vector<bool>& curr_unassigned) const;

    int get_best_index(vector<int>& order, int* elements);
    int get_index_of_leftmost_nonzero(vector<int>& order, int* elements) const;

    bool is_already_DAG(const vector<bool>& blacks) const;
    bool is_disconnected(const vector<bool>& blacks) const;

    void get_cg_neighbours(vector<int> &neigh, VariableProxy node) const;

    bool are_black_variables_connected();
    bool are_black_variables_singly_connected();
    bool is_leaf(VariableProxy var) const { return get_cg_successors(var).size() == 0; }
    bool is_root(VariableProxy var) const { return get_cg_predecessors(var).size() == 0; }
    std::string get_variable_name_and_domain(VariableProxy var) const { return red_black_task_core->get_variable_name_and_domain(var); }
    void set_invertible_as_red(VariableProxy var);
    void set_invertible_as_black(VariableProxy var);

    void set_black_variables(RedBlackTaskCore* core);
public:
    ColoringStrategy(const options::Options &options, const AbstractTask &task);

    void initialize(RedBlackTaskCore* core);

    void precalculate_variables(bool force_computation);
    void dump_options() const {};
    void free_mem();
    bool is_black(VariableProxy var) const { return black_vars[var.get_id()]; }
    const vector<bool> get_black_variables() const { return black_vars; }

    bool is_use_connected() const { return use_connected; }
    void set_use_connected(bool use) { use_connected = use; }
    const vector<int> &get_cg_predecessors(VariableProxy var) const { return task_proxy.get_causal_graph().get_predecessors(var.get_id()); }
    const vector<int> &get_cg_successors(VariableProxy var) const {return task_proxy.get_causal_graph().get_successors(var.get_id()); }

};
}
#endif
