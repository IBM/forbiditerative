#ifndef RED_BLACK_DTG_OPERATORS_H
#define RED_BLACK_DTG_OPERATORS_H

#include "../algorithms/priority_queues.h"
#include "red_black_operator.h"
#include "../abstract_task.h"

#include <vector>
#include <list>
#include <string>


namespace red_black {
typedef std::pair<sas_operator, EffectProxy> op_eff_pair;

/*
enum EdgeStatus {
    ALWAYS_ENABLED,
    ENABLED,
    DISABLED
};
*/

struct GraphEdge {
    int to;
    int op_no;
    int cost;
    bool initially_enabled;

    GraphEdge(int _to, int _op_no, int _cost, bool init_enabled)
        : to(_to), op_no(_op_no), cost(_cost),
          initially_enabled(init_enabled) {
    }

};

class RedBlackHeuristic;

enum ConnectivityStatus {
    ALL_PAIRS_CONNECTED,
    ALL_CONNECTED_TO_GOAL,
    NEITHER
};


enum TransitionEnablementStatus {
    ONLY_CURRENT_TRANSITIONS,
    ENABLED_BEFORE_RUN,
    ENABLED_DURING_RUN
};

class DtgOperators {

    TaskProxy task_proxy;
    utils::LogProxy log;

    int var;
    bool is_root;
    int range;
    int goal_val;
    std::vector<std::vector<std::vector<op_eff_pair>>> ops_by_from_to; // Deleted/cleared for all variables after initialization

//    // For directed required part invertible
//    bool directed_required_part_invertible;
//    std::vector<bool> required_part; // Deleted for all variables after initialization
//    bool required_part_found;
//    set<int> requested; // Deleted for all variables after initialization
//    int entry_value;

    // For calculating the shortest paths for black and storing the achieved values for red
    std::vector<bool> achieved_vals;

    std::list<int> red_sufficient_unachieved, default_list;
    std::vector<bool> red_sufficient_achieved;
    std::vector<std::list<int>::iterator> red_sufficient_unachieved_iterators;

    RedBlackHeuristic* base_pointer;

    int number_achieved_vals;
    int number_sufficient_unachieved_vals;
    bool use_sufficient_unachieved;
    bool use_black_reachable;

    // For marking reachable black vals
    std::vector<int> reachable_black_vals;
    int number_reachable_black_vals;

    int current_value;
    int missing_value;
    int* dijkstra_distance; // Deleted for red variables after initialization
    int* dijkstra_ops; // Deleted for red variables after initialization
    int* dijkstra_prev; // Deleted for red variables after initialization

    std::vector<int>** sol_edges; // Deleted for red variables after initialization
    int** solution; // Deleted for all variables after initialization

    std::vector<std::vector<GraphEdge> > complete_forward_graph;  // Deleted for red variables after initialization

    TransitionEnablementStatus transitions_status;

    bool black_initialized;
    bool shortest_paths_calculated;

    std::vector<int> plan;
    std::vector<bool> ops_sufficient;
    bool is_red_connected;
    void restore_path_from_dijkstra_ops(int to_state, std::vector<int>& path) const;

    // Used for checking invertibility, once, in the initialization. Not used during the search for heuristic computation.
    bool is_transition_invertible(int from_value, int to_value, utils::LogProxy &clog) const;
    const std::vector<op_eff_pair>& get_ops_from_to(int from, int to) const;

    void dijkstra_search(priority_queues::AdaptiveQueue<int> &queue);  // This one works on the complete forward graph
    void astar_search(priority_queues::AdaptiveQueue<int> &queue, int goal);  // This one works on the complete forward graph

    const std::vector<int>& get_shortest_path_for_root();
    const std::vector<int>& get_shortest_path_for_root_from_to(int from, int to);

    void set_root() { is_root = true; }
    void dump_shortest_paths_for_root(utils::LogProxy &clog) const;
    void dump_shortest_paths_for_root_from_to(int i, int j, utils::LogProxy &clog) const;

    void dump_complete_forward_graph(utils::LogProxy &clog) const;
    std::string get_value_name(int value) const { return task_proxy.get_variables()[var].get_fact(value).get_name(); }

    bool is_transition_enabled(const GraphEdge& trans, int from, utils::LogProxy &clog) const;

    // For delaying the goal achievement
    bool check_connected_from_to(int from, int to);

//    bool is_condition_included(FactProxy cond, const std::vector<FactProxy> &pre) const;
//    bool is_condition_included(FactProxy cond, const std::vector<EffectProxy> &effs) const;
    bool is_condition_included(FactProxy cond, op_eff_pair op_eff) const;

    bool is_op_transition_invertible(op_eff_pair op_eff, int from_value, int to_value, utils::LogProxy &clog) const;
//    bool is_transition_invertible_by_op(op_eff_pair op_eff, op_eff_pair by_op_eff) const;
    bool is_transition_invertible_by_op_conditional(op_eff_pair op_eff, op_eff_pair by_op_eff) const;

public:
    DtgOperators(int v, const std::shared_ptr<AbstractTask> task, utils::LogProxy &log);
    virtual ~DtgOperators();

    static bool use_astar;

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // Used once, in the initialization. Not used during the search for heuristic computation.
    // For all variables
    void add_operator_from_to(int from, int to, sas_operator sas_op, EffectProxy eff);
    bool check_invertibility(utils::LogProxy &clog) const;
    void set_follow_red_facts() { use_sufficient_unachieved = true; }
    void set_use_black_reachable();

    // For delaying the goal achievement
    ConnectivityStatus check_connectivity();

    // For black variables only
    void initialize_black(RedBlackHeuristic* base);
    void calculate_shortest_paths_for_root();

    void calculate_shortest_paths_ignore_prevail_conditions();
    void add_edge_to_complete_forward_graph(int from, int to, int op_no, int op_cost, bool no_red_prec);

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // Used during the search for heuristic computation.
    // For all variables
    bool mark_achieved_val(int val, bool is_black = false);
    bool is_achieved(int val) const;
    void clear_all_marks();
    void clear_sufficient();
    void mark_as_sufficient(int val);
    int num_sufficient_unachieved() const;
    const std::list<int>& get_sufficient_unachieved() const;
    // Heuristic moving the sufficient goal values to the end
    void postpone_sufficient_goal();
    bool is_sufficient_unachieved(int val) const;

    void clear_reachable();
    bool mark_as_reachable(int val);
    void update_reachable();
    bool is_reachable(int val) const;

    void clear_black_data_for_red_var();
    void clear_initial_data();
    int num_achieved_values() const { return number_achieved_vals; }

    // For black variables only
    void mark_missing_val(int val);
    void clear_missing_mark();
    bool is_change_needed() const;

    const std::vector<int>& calculate_shortest_path();
    const std::vector<int>& calculate_shortest_path(const std::vector<int>& values);
    const std::vector<int>& calculate_shortest_path_from_to(int from, int to);
    const std::vector<int>& calculate_shortest_path_to(int to);

    const std::vector<int>& get_current_shortest_path() const { return plan; }
    void clear_calculated_path() { plan.clear(); }

    int get_current_shortest_path_cost() const;
    int get_current_shortest_path_cost_to(int to) const;
    bool is_root_var() const {return is_root; }

    int get_current_value() const { return current_value; }
    int get_missing_value() const { return missing_value; }
    int get_shortest_distance_ignore_prevail_conditions(int from, int to) const;
    void set_red_connected() { is_red_connected = true; }

    int get_cost_of_resolving_conflict(int to) const;
    void free_solution();
    void free_solution_edges_for_root();
    void set_goal_val(int val) { goal_val = val; }

    void set_transitions_enablement_status(TransitionEnablementStatus curr) { transitions_status = curr; }
    TransitionEnablementStatus get_transitions_enablement_status() { return transitions_status; }

    const std::vector<bool>& get_sufficient_achieved() const { return red_sufficient_achieved; }
    bool is_sufficient_achieved(int val) const { return red_sufficient_achieved[val]; }

/* For later implementation of other reversibility approximations
 *
private:
    bool is_entry_value(int val) const;
public:
    bool is_directed_required_part_invertible() const;
    void find_required_part();
    void add_requested_value(int val);
*/
};
}
#endif
