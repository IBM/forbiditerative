#include "coloring_strategy.h"

#include "../option_parser.h"
#include "../plugin.h"
#include "../algorithms/topological_sort.h"

#include <cassert>

using namespace std;

namespace red_black {
bool pair_compare ( const pair<int,int>& l, const pair<int,int>& r) { return l.first < r.first; }
bool pair_compare_double ( const pair<double,int>& l, const pair<double,int>& r) { return l.first < r.first; }

ColoringStrategy::ColoringStrategy(const Options &opts, const std::shared_ptr<AbstractTask> task) :
        task_proxy(*task),
        red_black_task_core(0),
        number_of_black_variables(0),
        black_dag(opts.get<BlackDAG>("dag")),
        shortest_paths_calculated(false),
        set_conflicting_to_red(opts.get<bool>("set_conflicting_to_red")),
        log(utils::get_log_from_options(opts)),
        use_connected(true) {
}

void ColoringStrategy::free_mem() {
    black_vars.clear();
}


void ColoringStrategy::initialize(RedBlackTaskCore* core) {
    set_black_variables(core);
}

void ColoringStrategy::set_black_variables(RedBlackTaskCore* core) {
    red_black_task_core = core;
	if (log.is_at_least_normal()) {
        utils::g_log  << "Total number of invertible variables is " << get_num_invertible_vars() << endl;
    }
    if (get_num_invertible_vars() == 0) {
        // Releasing the allocated memory, nothing more to do...
        free_mem();
        return;
    }
    VariablesProxy variables = task_proxy.get_variables();
    vector<bool> conflicting_variables(variables.size(), false);
    if (set_conflicting_to_red) {
        OperatorsProxy operators = task_proxy.get_operators();
        for (OperatorProxy op : operators) {
            EffectsProxy eff = op.get_effects();
            for (EffectProxy e1 : eff) {
                VariableProxy var1 = e1.get_fact().get_variable();
                if (!is_invertible(var1))
                    continue;
                int val1 = e1.get_fact().get_value();
                for (EffectProxy e2 : eff) {
                    VariableProxy var2 = e2.get_fact().get_variable();
                    if (var1 != var2)
                        continue;

                    int val2 = e2.get_fact().get_value();
                    if (val1 == val2)
                        continue;
                    conflicting_variables[var1.get_id()] = true;
                }
            }
        }
    }
    black_vars.clear();
    // We start by setting all invertible to be black. Then, we disconnect them by marking some of them as red.
    for (VariableProxy var : variables) {
        black_vars.push_back(is_invertible(var));
    }

    // Printing whether there are causal links between invertible variables
	if (log.is_at_least_normal()) {
        bool black_connected = are_black_variables_connected();
        utils::g_log  << "Invertible variables connection status: " << (black_connected ? "yes" : "no") << endl;
    }
    number_of_black_variables = get_num_invertible_vars();
    // Painting sink variables red.
    for (VariableProxy var : variables) {
        // Calculate the degrees
        if (!is_black(var))
            continue;

        // Skipping the sink variables - these don't support anyone and thus can be marked red.
        if (is_leaf(var)) {
            set_invertible_as_red(var);
        } else if (set_conflicting_to_red && conflicting_variables[var.get_id()]) {
            set_invertible_as_red(var);
        }
    }

	if (log.is_at_least_normal()) {
        // Printing whether there are causal links between invertible variables after painting leafs red
        utils::g_log  << "Invertible variables without leafs connection status: " << (are_black_variables_connected() ? "yes" : "no") << endl;
        utils::g_log  << "Invertible variables with one directional connection status: " << (are_black_variables_singly_connected() ? "yes" : "no") << endl;

        utils::g_log  << "Setting black variables..." << endl;
    }
    if (get_number_of_black_variables() == 1) {
        return;
    }
    if (black_dag == FALSE || black_dag == FROM_COLORING) {
        std::vector<int> red_variables;
        set_black_variables_vertex_cover(red_variables);
    } else if (black_dag == GREEDY_LEVEL) {
        set_black_variables_while_DAG();
    } else {
        cerr << "Something is wrong - no black DAG creation method." << endl;
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }
}

void ColoringStrategy::set_invertible_as_red(VariableProxy var) {
    if (!is_black(var))
        return;
    number_of_black_variables--;
    black_vars[var.get_id()] = false;
}

void ColoringStrategy::set_invertible_as_black(VariableProxy var) {
    if (is_black(var))
        return;
    number_of_black_variables++;
    black_vars[var.get_id()] = true;
}

void ColoringStrategy::set_black_variables_vertex_cover(std::vector<int>& red_variables) {
   // Selecting the subset of variables to be marked as black.
    // For that, greedy Vertex Cover is found, the rest of the vertices are disconnected.
    bool disconnected = true;
    // Going over the current black variables (initialized by marking the invertible variables black), create a subgraph from the causal graph.
    VariablesProxy variables = task_proxy.get_variables();
    int* degrees = new int[variables.size()];
    for (VariableProxy var : variables) {
        degrees[var.get_id()] = 0;
        // Calculate the degrees
        if (!is_black(var))
            continue;

        vector<int> neigh;
        get_cg_neighbours(neigh, var);

        for (int neighbour : neigh) {
            VariableProxy neigh_var = variables[neighbour];
            if (!is_black(neigh_var))
                continue;

            // Increasing the degree
            degrees[var.get_id()]++;
            disconnected = false;
        }
    }

    if (disconnected) {
        delete [] degrees;
        return;
    }

    vector<int> order;
    set_variables_order_for_vertex_cover(order);
    // For painting red until tractable
    paint_red_by_vertex_cover(order, degrees, red_variables);

    delete [] degrees;
}

void ColoringStrategy::precalculate_variables(bool force_computation) {
    // In some cases, here the black indices are not set yet
    if (shortest_paths_calculated) {
        return;
    }
	if (log.is_at_least_normal()) {
        utils::g_log  << "Adding edges to forward graph, for the later calculation of missing values" << endl;
    }
    // Setting the affecting actions
    for (size_t op_no = 0; op_no < task_proxy.get_operators().size(); ++op_no) {
        // Adding the operator to the complete_forward_graph
        OperatorProxy op = task_proxy.get_operators()[op_no];
		if (log.is_at_least_debug()) {
            utils::g_log  << "For operator with index " << op_no << "    " << op.get_name() << endl;
        }
        int op_cost = op.get_cost();
        EffectsProxy effects = op.get_effects();
        for (EffectProxy eff : effects) {
            VariableProxy var = eff.get_fact().get_variable();
    		if (log.is_at_least_debug()) {

                utils::g_log  << "Effect variable " << var.get_name() << " is " << (is_black(var) ? "black" : "red") << endl;
            }
            if (!is_black(var) && !is_use_connected()) { // Only for black vars
                continue;
            }
    		if (log.is_at_least_debug()) {
                utils::g_log  << "Use connected: " << (is_use_connected() ? "yes" : "no") << endl;
            }

            int pre_value = get_rb_sas_operator(op_no)->get_pre_value_by_effect(eff);
		    if (log.is_at_least_debug()) {
                utils::g_log  << "From value: " << pre_value << endl;
            }
            int post_value = eff.get_fact().get_value();

    		if (log.is_at_least_debug()) {
                utils::g_log  << "To value: " << post_value << endl;
            }

            bool is_root = (get_cg_predecessors(var).size() == 0);

            int pre_value_min, pre_value_max;
            if (pre_value == -1) {
                pre_value_min = 0;
                pre_value_max = var.get_domain_size();
            } else {
                pre_value_min = pre_value;
                pre_value_max = pre_value + 1;
            }
            for (int value = pre_value_min; value < pre_value_max; ++value) {
                if (value == post_value) {
                    continue;
                }
        		if (log.is_at_least_debug()) {
                    utils::g_log  << "Adding edge to complete forward graph, value: " << value << ", to value: " << post_value << ", for operator: " << op_no << " with cost " << op_cost << " with " << (is_root ? "no " : "") << "red preconditions" << endl;
                }
                get_dtg(var)->add_edge_to_complete_forward_graph(value, post_value, op_no, op_cost, is_root );
        		if (log.is_at_least_debug()) {
                    utils::g_log  << "Added edge from " << value << " to " << post_value << " for operator " << op_no << " with cost " << op_cost << " with " << (is_root ? "no " : "") << "red preconditions" << endl;
                }
            }
        }
		if (log.is_at_least_debug()) {
            utils::g_log  << "Operator " << op_no << " with " << op.get_effects().size() << " black effects" << endl;
        }
    }

	if (log.is_at_least_normal()) {
        utils::g_log  << "Precalculating all pair shortest paths" << endl;
    }
    VariablesProxy variables = task_proxy.get_variables();
    for (VariableProxy var : variables) {
        precalculate_shortest_paths_for_var(var, force_computation || DtgOperators::use_astar);
    }
    shortest_paths_calculated = true;
	if (log.is_at_least_debug()) {
        utils::g_log  << "DONE Precalculating all pair shortest paths" << endl;
    }
}

void ColoringStrategy::precalculate_shortest_paths_for_var(VariableProxy var, bool force_computation) {
    if (shortest_paths_calculated) {
    	if (log.is_at_least_normal()) {
            utils::g_log  << "All shortest paths already calculated " << endl;
        }
        return;
    }
	if (log.is_at_least_debug()) {
        utils::g_log  << "----------> Variable: " << var.get_name() << endl;
        utils::g_log  << "---------->               is " << (is_black(var) ? "black" : "red") << endl;
    }

    if (!is_black(var) && is_use_connected()) {
	    if (log.is_at_least_debug()) {
            utils::g_log  << "Storing shortest paths and costs for connected red variable " << var.get_name() << endl;
        }
        if (get_cg_predecessors(var).size() == 0) {
            get_dtg(var)->calculate_shortest_paths_for_root();
        } else {
            // TODO: Think of some smarter way of implementing that
            get_dtg(var)->calculate_shortest_paths_ignore_prevail_conditions();
        }
    }

    if (!is_black(var)) {
        return;
    }

    if (get_cg_predecessors(var).size() == 0) {
    	if (log.is_at_least_debug()) {
            utils::g_log  << "Storing shortest paths and costs for root variable " << var.get_name() << endl;
        }
        get_dtg(var)->calculate_shortest_paths_for_root();
        return;
    }

    if (force_computation) {
        // Storing shortest paths ignoring external preconditions for all black variables
        // Since it is already done for the root variables, skipping them here
    	if (log.is_at_least_debug()) {
            utils::g_log  << "Storing shortest paths costs ignoring external preconditions for variable " << var.get_name() << endl;
        }
        get_dtg(var)->calculate_shortest_paths_ignore_prevail_conditions();
    }
}

void ColoringStrategy::set_variables_order_by_level_heuristic(vector<int>& order) {
    int num_variables = task_proxy.get_variables().size();
    for (int var = num_variables - 1; var >=0; --var) {
        order.push_back(var);
    }
}

void ColoringStrategy::set_variables_order_for_vertex_cover(vector<int>& order) {
    order.clear();
    set_variables_order_by_level_heuristic(order);
	if (log.is_at_least_normal()) {
        utils::g_log  << "Variables order is set" << endl;
    }
}

void ColoringStrategy::paint_red_by_vertex_cover(vector<int>& order, int* elements, vector<int>& red_variables) {
    // Greedy algorithm goes over nodes, taking one into the cover.
    // Iteratively remove edges until there are no nodes of degree > 0 (that means, no edges left)
    // Each iteration a node is greedily selected to be considered next, it is removed from the graph and degrees are updated
    // This node is going to be in the vertex cover, so not going to be black.
    while (true) {
    	if (log.is_at_least_debug()) {
            utils::g_log  << "Number of edges left:" << endl;
            for (size_t i=0; i < task_proxy.get_variables().size(); ++i) {
                // Printing the indexes of all variables (number of edges)
                utils::g_log  << "[" << i << "] :  " << elements[i] << endl;
            }
        }
        // Getting the next node according to the selected method to be marked red
        int best_node = get_best_index(order, elements);
        if (-1 == best_node) {
            break;
        }

    	if (log.is_at_least_debug()) {
            utils::g_log  << "Best node found: " << best_node << endl;
        }
        VariableProxy best_var = task_proxy.get_variables()[best_node];
        red_variables.push_back(best_node);

        // Going over all currently black neighbors, reducing their degree
        vector<int> neigh;
        get_cg_neighbours(neigh, best_var);
        for (int neighbour : neigh) {
            VariableProxy neigh_var = task_proxy.get_variables()[neighbour];
            if (!is_black(neigh_var)) {
                continue;
            }
            // Decreasing the degree
            elements[neighbour]--;
        }
        elements[best_node] = 0;
        set_invertible_as_red(best_var);
    }
}


void ColoringStrategy::set_black_variables_while_DAG() {
    // Implement the following algorithm:
    // Start with an empty set BV and a set of invertible variables IV
    // While there are variables to consider:
    // 1. Pick a variable v from IV, remove it from IV.
    // 2. If BV\cup {v} induces DAG, add it to BV.

    // Computing the number of variable connected by bidirectional edge
    // Construct an adjacency matrix

    VariablesProxy variables = task_proxy.get_variables();
    vector<vector<int> > adj_matrix(variables.size(), vector<int>());
    vector<vector<int> > bidirectional_edges(variables.size(), vector<int>());
    for (VariableProxy var : variables) {
        if (!is_black(var)) {
            continue;
        }
        adj_matrix[var.get_id()].assign(variables.size(), 0);
        for(int succ : get_cg_successors(var)) {
            VariableProxy to_var = variables[succ];
            if (is_black(to_var)) {
                adj_matrix[var.get_id()][to_var.get_id()] = 1;
            }
        }
    }
    // Checking whether there is a pair of vars with single edge between them
    for (VariableProxy var1 : variables) {
        if (!is_black(var1)) {
            continue;
        }
        for (VariableProxy var2 : variables) {
            if (!is_black(var2) || var1 == var2) {
                continue;
            }
            if (adj_matrix[var1.get_id()][var2.get_id()] + adj_matrix[var2.get_id()][var1.get_id()] == 2) {
                bidirectional_edges[var1.get_id()].push_back(var2.get_id());
            }
        }
    }

    vector<bool> curr_blacks;
    set_DAG_blacks(curr_blacks, bidirectional_edges);

    // Setting the black variables
    black_vars.swap(curr_blacks);
    recompute_number_of_black_variables();
}

void ColoringStrategy::recompute_number_of_black_variables() {
    number_of_black_variables=0;
    VariablesProxy variables = task_proxy.get_variables();

    for (VariableProxy var : variables) {
        if (is_black(var)) {
            number_of_black_variables++;
        }
    }
}


void ColoringStrategy::set_DAG_blacks(vector<bool>& blacks, const vector<vector<int> >& bidirectional_edges) {
    assert(blacks.size() == 0);
    blacks.assign(task_proxy.get_variables().size(), false);
    vector<bool> curr_unassigned = black_vars;

    while (true) {
        // Getting the next vertex for painting black
        int vert = get_DAG_next_node_level(curr_unassigned);
        if (vert == -1) {
            break;
        }
        assert (!blacks[vert]);
        curr_unassigned[vert] = false;

        blacks[vert] = true;
        if (is_already_DAG(blacks)) {
            // For faster computation, paint all bidirectionally connected red
            const vector<int>& bd = bidirectional_edges[vert];
            for (size_t p=0; p < bd.size(); ++p) {
                int to_vert = bd[p];
                curr_unassigned[to_vert] = false;
            }
        } else {
            // Painting it red
            blacks[vert] = false;
        }
    }
}


int ColoringStrategy::get_DAG_next_node_level(const vector<bool>& curr_unassigned) const {
    // By level heuristic
    for (size_t i = 0; i < curr_unassigned.size(); ++i) {
        if (curr_unassigned[i]) {
            return i;
        }
    }
    return -1;
}

// Used for getting the next node to paint red, for all black selection options
int ColoringStrategy::get_best_index(vector<int>& order, int* elements) {

    // If stopping when DAG is obtained, here we need to check whether the remaining part is already a DAG
    if ((black_dag == FROM_COLORING) && is_already_DAG(black_vars)) {
        return -1;
    }
    return get_index_of_leftmost_nonzero(order, elements);
}

int ColoringStrategy::get_index_of_leftmost_nonzero(vector<int>& order, int* elements) const {
    for (int var : order) {
        if (elements[var] <= 0) {
            continue;
        }
        return var;
    }
    return -1;
}

bool ColoringStrategy::is_already_DAG(const vector<bool>& blacks) const {
    // Creating a graph out of the black vars
    // Sorting topologically. If succeeded, then DAG, otherwise not

    vector<int> black_ids;
    vector<int> ids_for_vars(task_proxy.get_variables().size(),-1);

    for (size_t i = 0; i < blacks.size(); ++i) {
        if (!blacks[i]) {
            continue;
        }
        ids_for_vars[i] = black_ids.size();
        black_ids.push_back(i);
    }
    // Creating the graph
    vector<vector<int> > graph(black_ids.size(), vector<int>());

    for (size_t i=0; i < black_ids.size(); ++i) {
        VariableProxy var = task_proxy.get_variables()[black_ids[i]];
        for(int succ : get_cg_successors(var)) {
            if (!blacks[succ]) {
                continue;
            }
            int to_id = ids_for_vars[succ];

            graph[i].push_back(to_id);
        }
    }

    topological_sort::TopologicalSort ts(graph);
    vector<int> res;
    return ts.get_result(res);
}

bool ColoringStrategy::is_disconnected(const vector<bool>& blacks) const {
    for (size_t i = 0; i < blacks.size(); ++i) {
        if (!blacks[i]) {
            continue;
        }
        VariableProxy var = task_proxy.get_variables()[i];
        for(int succ : get_cg_successors(var)) {
            if (blacks[succ]) {
                return false;
            }
        }
    }
    return true;
}

void ColoringStrategy::get_cg_neighbours(vector<int> &neigh, VariableProxy node) const {
    vector<int> pred =  get_cg_predecessors(node);
    vector<int> succ =  get_cg_successors(node);

    sort(pred.begin(), pred.end());
    sort(succ.begin(), succ.end());

    set_union(pred.begin(), pred.end(), succ.begin(), succ.end(), back_inserter(neigh));
}

bool ColoringStrategy::are_black_variables_connected() {
    VariablesProxy variables = task_proxy.get_variables();

    for (VariableProxy var : variables) {
        if (!is_black(var)) {
            continue;
        }
        for(int succ : get_cg_successors(var)) {
            VariableProxy to_var = variables[succ];
            if (is_black(to_var)) {
                return true;
            }
        }
    }
    return false;
}

bool ColoringStrategy::are_black_variables_singly_connected() {
    // Construct an adjacency matrix
    VariablesProxy variables = task_proxy.get_variables();

    vector<vector<int> > adj_matrix(variables.size(), vector<int>());
    for (VariableProxy var : variables) {
        if (!is_black(var)) {
            continue;
        }
        adj_matrix[var.get_id()].assign(variables.size(), 0);
        for(int succ : get_cg_successors(var)) {
            VariableProxy to_var = variables[succ];
            if (is_black(to_var)) {
                adj_matrix[var.get_id()][succ] = 1;
            }
        }
    }
    // Checking whether there is a pair of vars with single edge between them
    for (VariableProxy var : variables) {
        if (!is_black(var)) {
            continue;
        }
        for (size_t j = var.get_id()+1; j <  variables.size(); ++j) {
            VariableProxy to_var = variables[j];
            if (!is_black(to_var)) {
                continue;
            }
            if (adj_matrix[var.get_id()][j] + adj_matrix[j][var.get_id()] == 1) {
                return true;
            }
        }
    }
    return false;
}

}
