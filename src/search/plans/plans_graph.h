#ifndef PLANS_PLANS_GRAPH_H
#define PLANS_PLANS_GRAPH_H

#include "../state_registry.h"
#include "../search_engine.h"
#include "../task_proxy.h"
#include "../state_id.h"
#include "../structural_symmetries/group.h"
#include "operator_interaction.h"

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <vector>

namespace plans {

class DFSNode {
public:
    StateID state_id;
    std::shared_ptr<DFSNode> parent;
    int achieving_op;
    int plan_cost;
    DFSNode(StateID _state_id, std::shared_ptr<DFSNode> _parent, int _achieving_op, int _plan_cost) : state_id(_state_id), parent(_parent), achieving_op(_achieving_op), plan_cost(_plan_cost) {}
    	bool is_on_the_path(StateID id) const {
    		if (state_id == id)
    			return true;
    		if (achieving_op == -1)
    			return false;
    		return parent->is_on_the_path(id);
    	}
};

enum class PlanOrdersReduction {
	NONE,
	NEIGHBOURS_INTERFERE,
	NAIVE_ALL,
	NAIVE_ALL_DFS,
    NAIVE_ALL_DFS_NODUP
};

class DfsNodeNaive {
	const StateID state;
	const std::vector<size_t> remaining_ops;
    std::shared_ptr<DfsNodeNaive> parent;

public:
	DfsNodeNaive(StateID s, const std::vector<size_t>& ops, std::shared_ptr<DfsNodeNaive> _parent) : state(s), remaining_ops(std::move(ops)), parent(_parent) {}

    bool operator==(const DfsNodeNaive &other) const {
    	if (state != other.state || remaining_ops.size() != other.remaining_ops.size())
    		return false;

    	for (size_t i = 0; i < remaining_ops.size() ; ++i) {
       		if (remaining_ops[i] != other.remaining_ops[i])
       			return false;
    	}
    	return true;
   	}
   	const StateID& get_state() const {return state; }
   	const std::vector<size_t>& get_remaining_ops() const { return remaining_ops;}

   	void dump() const {
   		std::cout << "StateID: " << state << std::endl;
   		for (size_t op_idx : remaining_ops) {
   			std::cout << " " << op_idx ;
   		}
   		std::cout << std::endl;
   	}

   	bool is_on_the_path(StateID id) const {
   		if (state == id)
   			return true;
   		if (parent == nullptr)
   			return false;
   		return parent->is_on_the_path(id);
   	}
};

class PlansGraph {
public:
    typedef std::unordered_map<int, StateID> ToEdges;
    typedef std::unordered_set<StateID> StatesSet;
    typedef std::unordered_set<int> OpsSet;
    typedef std::unordered_set<SearchEngine::Plan> PlansSet;

private:
    bool add_edge(const GlobalState& current, int op_no, const GlobalState& next);
    size_t num_plans_to_dump;
    bool optimal;
    int best_plan_cost;
    StateRegistry* registry;
    PlansSet optimal_plans;
    PlansSet non_optimal_plans;
    void dump_plan(std::vector<int>& ops_so_far);
    OperatorInteraction* op_interaction;
    void add_plan_bfs_naive(SearchEngine::Plan plan);
    void add_plan_dfs_naive(SearchEngine::Plan plan);
    void add_plan_dfs_naive_no_duplicate_detection(SearchEngine::Plan plan);

    void add_plan_reduce_order_neighbors(SearchEngine::Plan plan);
    void add_plan_no_reduce(SearchEngine::Plan plan);
    void dump_plans_bfs(int number_of_plans);
    // For DFS dump procedure we need a bound on plan cost
    void find_plans_dfs(size_t number_of_plans);
    int check_interchangeability(const GlobalState& current, SearchEngine::Plan plan, int from_index) const;
    void add_ops_bfs(const GlobalState& current, SearchEngine::Plan plan, int curr_ind, int num_interchangeable);
	PerStateInformation<int> goal_distance;
	void update_goal_distance(const GlobalState& state, int distance);
	void update_goal_distance(const StateID& stateID, int distance);
	int get_goal_distance(const GlobalState& state) const;
	int get_goal_distance(const StateID& stateID) const;
	// For removing unreachable from init states
	PerStateInformation<int> init_distance;
	void update_init_distance(const GlobalState& state, int distance);
	void update_init_distance(const StateID& stateID, int distance);
	int get_init_distance(const GlobalState& state) const;
	int get_init_distance(const StateID& stateID) const;

	void forward_dijkstra();
	void backwards_dijkstra();
	bool remove_origin_state_by_op(int op_no, const StateID& id);
    void dump_graph_dot(int num_plan, bool verbose);

    std::unordered_map<StateID, ToEdges> edges_by_source;
    StatesSet nodes;
    std::unordered_map<int, StatesSet> origin_states_by_operators;

    int num_edges_until_greedy_clean;
    void remove_non_plans_naive();
    void remove_init_unreachable();
    void remove_all_nodes_and_edges();

public:
    PlansGraph(size_t num_plans, bool _optimal);
    virtual ~PlansGraph();

    const StatesSet& get_nodes() const { return nodes; }
    ToEdges& get_outgoing_edges(const StateID& from) { return edges_by_source[from]; }
    ToEdges& get_outgoing_edges(const GlobalState& from) { return get_outgoing_edges(from.get_id()); }

    const std::unordered_map<int, StatesSet>& get_origin_states_by_operators() const { return origin_states_by_operators; }
    int get_number_involved_operators() const { return origin_states_by_operators.size(); }

    void add_plan(SearchEngine::Plan plan, PlanOrdersReduction reduce_orders);
    bool add_symmetry(const Permutation* fact_symmetry, const OperatorPermutation* op_symmetry);

    StateID get_initial_state_id() const { return registry->get_initial_state().get_id(); }
    void dump_plans(size_t number_of_plans);

    void dump_graph();
    void dump_graph_dot(bool verbose=true);
    void dump_graph_file();
    void add_non_deterministic_plan(std::vector<std::string> plan);
    int get_number_of_plans_dumped() const { return optimal_plans.size(); }
    void set_num_edges_until_greedy_clean(int num) { num_edges_until_greedy_clean = num; }
};
}
#endif
