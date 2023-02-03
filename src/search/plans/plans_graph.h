#ifndef PLANS_PLANS_GRAPH_H
#define PLANS_PLANS_GRAPH_H

#include "../state_registry.h"
#include "../search_engine.h"
#include "../task_proxy.h"
#include "../state_id.h"
#include "../structural_symmetries/group.h"
#include "operator_interaction.h"
#include "../plan_manager.h"

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <vector>


namespace utils {
// inline void feed(HashState &hash_state, const OperatorID &op) {
//     feed(hash_state, op.hash());
// }
inline void feed(HashState &hash_state, const StateID &state_id) {
    feed(hash_state, state_id.hash());
}
}


namespace plans {

class DFSNode {

    void reconstruct_recursive_plan(Plan &plan) const {
		if (achieving_op == -1)
			return;
		plan.push_back(OperatorID(achieving_op));
		parent->reconstruct_recursive_plan(plan);
    }

public:
    StateID state_id;
    std::shared_ptr<DFSNode> parent;
    int achieving_op;
    int plan_cost;
    DFSNode(StateID _state_id, std::shared_ptr<DFSNode> _parent, int _achieving_op, int _plan_cost) : state_id(_state_id), parent(_parent), achieving_op(_achieving_op), plan_cost(_plan_cost) {}
    bool is_on_the_path(StateID id) const {
    	if (state_id == id) {
			return true;
		}
    	if (achieving_op == -1) {
    		return false;
		}
		return parent->is_on_the_path(id);
	}
   	const StateID& get_state() const {return state_id; }

   	void dump() const {
   		std::cout << "StateID: " << state_id << std::endl;
   		std::cout << "Achieving op: " << achieving_op << std::endl;
   		std::cout << "Sequence cost: " << plan_cost << std::endl;
   	}

	int get_plan_cost() const {
		return plan_cost;
	}
    void reconstruct_plan(Plan &plan) const {
		reconstruct_recursive_plan(plan);
		// Revert plan
		std::reverse(plan.begin(),plan.end());
    }

	bool is_already_applied(int op_no, size_t cnt) const {
		if (cnt == 0 || achieving_op == -1) 
			return false;
		
		if (achieving_op == op_no) {
			if (cnt == 1)
				return true;
			return parent->is_already_applied(op_no, cnt - 1);
		}
		return parent->is_already_applied(op_no, cnt);
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
    // typedef std::unordered_set<StateID> StatesSet;
    typedef utils::HashSet<StateID> StatesSet;
    typedef std::unordered_set<int> OpsSet;
    // typedef std::unordered_set<Plan> PlansSet;
	typedef utils::HashSet<Plan> PlansSet;
    //typedef std::unordered_map<int, size_t> MultisetPlan;
    typedef std::map<int, size_t> MultisetPlan;

private:
    const TaskProxy& task_proxy;
	std::shared_ptr<StateRegistry> registry;
	bool add_edge(const State& current, int op_no, const State& next);
    size_t num_plans_to_dump;
    bool optimal;
    int best_plan_cost;
    PlansSet optimal_plans;
    PlansSet non_optimal_plans;
    void dump_plan(std::vector<int>& ops_so_far);
    std::shared_ptr<OperatorInteraction> op_interaction;
    void add_plan_bfs_naive(const Plan& plan);
    void add_plan_dfs_naive(const Plan& plan);
    void add_plan_dfs_naive_no_duplicate_detection(PlanManager& plan_manager, const Plan& plan);

    void add_plan_reduce_order_neighbors(const Plan& plan);
    void dump_plans_bfs(int number_of_plans);
    // For DFS dump procedure we need a bound on plan cost
    void find_plans_dfs(PlanManager& plan_manager, size_t number_of_plans);
    int check_interchangeability(const State& current, const Plan& plan, int from_index) const;
    void add_ops_bfs(const State& current, const Plan& plan, int curr_ind, int num_interchangeable);
	PerStateInformation<int> goal_distance;
	void update_goal_distance(const State& state, int distance);
	void update_goal_distance(const StateID& stateID, int distance);
	int get_goal_distance(const State& state) const;
	int get_goal_distance(const StateID& stateID) const;
	// For removing unreachable from init states
	PerStateInformation<int> init_distance;
	void update_init_distance(const State& state, int distance);
	void update_init_distance(const StateID& stateID, int distance);
	int get_init_distance(const State& state) const;
	int get_init_distance(const StateID& stateID) const;

	void forward_dijkstra();
	void backwards_dijkstra();
	bool remove_origin_state_by_op(int op_no, const StateID& id);

    utils::HashMap<StateID, ToEdges> edges_by_source;
    StatesSet nodes;
    std::unordered_map<int, StatesSet> origin_states_by_operators;

    int num_edges_until_greedy_clean;
    void remove_non_plans_naive();
    void remove_init_unreachable();
    void remove_all_nodes_and_edges();
	size_t num_appearances(OperatorID op, MultisetPlan& plan_multiset) const;

public:
    PlansGraph(const TaskProxy& _task_proxy, size_t num_plans, bool _optimal);
    virtual ~PlansGraph();

    const StatesSet& get_nodes() const { return nodes; }
    ToEdges& get_outgoing_edges(const StateID& from) { return edges_by_source[from]; }
    ToEdges& get_outgoing_edges(const State& from) { return get_outgoing_edges(from.get_id()); }

    const std::unordered_map<int, StatesSet>& get_origin_states_by_operators() const { return origin_states_by_operators; }
    int get_number_involved_operators() const { return origin_states_by_operators.size(); }

    void add_plan(PlanManager& plan_manager, const Plan& plan, PlanOrdersReduction reduce_orders);
    bool add_symmetry(std::shared_ptr<Group> group, int gen_index);

    StateID get_initial_state_id() const { return registry->get_initial_state().get_id(); }
    void dump_plans(PlanManager& plan_manager, size_t number_of_plans);

    void dump_graph();
    void dump_dot_graph(int num_plan, bool verbose);
    void dump_graph_file(int num_plan);
    // void add_non_deterministic_plan(std::vector<std::string> plan);
    int get_number_of_plans_dumped() const { return optimal_plans.size(); }
    void set_num_edges_until_greedy_clean(int num) { num_edges_until_greedy_clean = num; }

    void dump_reordering_plans_dfs_naive_no_duplicate_detection(PlanManager& plan_manager, const Plan& plan, std::vector<Plan> &found_plans);
    void dump_reordering_plans_neighbours_interfere(Plan& plan);
	void plan_to_multiset(const Plan &plan, MultisetPlan& plan_multiset) const;
	void plan_to_multiset(const std::vector<int> &plan, MultisetPlan& plan_multiset) const;

	// Made public to support manual creation of plans graph for dumping to dot
    void add_plan_no_reduce(const Plan& plan);

};
}
#endif
