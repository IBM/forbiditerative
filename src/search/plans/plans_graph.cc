#include "plans_graph.h"
#include "../algorithms/priority_queues.h"
#include "../utils/timer.h"
#include "../utils/hash.h"
#include "../task_proxy.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <deque>
#include <vector>

namespace std {
template <>
struct hash<plans::DfsNodeNaive> {
	std::size_t operator()(const plans::DfsNodeNaive& k) const {

        size_t hash = 0;
        std::hash<StateID> state_hasher;
        std::hash<size_t> op_idx_hasher;

        utils::hash_combine(hash, state_hasher(k.get_state()));

		for (size_t i = 0; i < k.get_remaining_ops().size() ; ++i) {
			size_t op_idx = k.get_remaining_ops()[i];
	        utils::hash_combine(hash, op_idx_hasher(op_idx));
		}
		return hash;
    	}
};
}

using namespace std;
namespace plans {

inline std::string state_label(const GlobalState& s) {
	std::string state_string ;
	for (size_t i = 0; i < g_variable_domain.size(); ++i) {
		state_string += g_fact_names[i][s[i]] + "\n";
	}
	return state_string;
}

PlansGraph::PlansGraph(size_t num_plans, bool _optimal) :
        num_plans_to_dump(num_plans),
        optimal(_optimal),
        best_plan_cost(-1),
        op_interaction(0),
        goal_distance(-1),
        init_distance(-1),
        num_edges_until_greedy_clean(-1) {
    registry = new StateRegistry(*g_root_task(), *g_state_packer, *g_axiom_evaluator, g_initial_state_data);
}

PlansGraph::~PlansGraph() {
    remove_all_nodes_and_edges();
}

void PlansGraph::add_plan(SearchEngine::Plan plan, PlanOrdersReduction reduce_orders) {

	// Assumption: optimal plans are added this way
	int plan_cost = calculate_plan_cost(plan);
    if (best_plan_cost == -1 || plan_cost < best_plan_cost)
		best_plan_cost = plan_cost;

	cout << "Adding first plan" << endl;
    add_plan_no_reduce(plan);
	cout << "Done, dumping before the next step" << endl;
    dump_plans(1);
	cout << "Done dumping" << endl;

    if (reduce_orders == PlanOrdersReduction::NONE) {
		// Nothing else to do
		return;
	} else if (reduce_orders == PlanOrdersReduction::NEIGHBOURS_INTERFERE) {
		op_interaction = new OperatorInteraction(g_root_task());
		add_plan_reduce_order_neighbors(plan);
	} else if (reduce_orders == PlanOrdersReduction::NAIVE_ALL) {
		add_plan_bfs_naive(plan);
	} else if (reduce_orders == PlanOrdersReduction::NAIVE_ALL_DFS) {
		add_plan_dfs_naive(plan);
	} else if (reduce_orders == PlanOrdersReduction::NAIVE_ALL_DFS_NODUP) {
		add_plan_dfs_naive_no_duplicate_detection(plan);
	} else {
		cerr << "Plan orders reduction option not supported" << endl;
		utils::exit_with(utils::ExitCode::UNSUPPORTED);
	}
}

void PlansGraph::add_plan_no_reduce(SearchEngine::Plan plan) {
	int cost_so_far = 0;
	GlobalState current = registry->get_initial_state();
	for (size_t i=0; i < plan.size(); ++i) {
		const GlobalOperator* op = plan[i];
		cout << "Applying operator " << op->get_name() << endl;
		if (!op->is_applicable(current)) {
			cerr << "Operator not applicable!" << endl;
			op->dump();
            utils::exit_with(utils::ExitCode::CRITICAL_ERROR);
		}

		const GlobalState& next = registry->get_successor_state(current, *op);
		int op_no = get_op_index_hacked(op);
		add_edge(current, op_no, next);
		update_goal_distance(current, best_plan_cost - cost_so_far);
		current = next;
		cost_so_far += op->get_cost();
		update_goal_distance(current, best_plan_cost - cost_so_far);
	}
}

void PlansGraph::update_goal_distance(const GlobalState& state, int distance) {
	if (goal_distance[state] == -1 || goal_distance[state] > distance)
		goal_distance[state] = distance;
}

int PlansGraph::get_goal_distance(const GlobalState& state) const {
	if (goal_distance[state] == -1)
		return std::numeric_limits<int>::max();
	return goal_distance[state];
}

void PlansGraph::update_goal_distance(const StateID& stateID, int distance) {
	const GlobalState& state = registry->lookup_state(stateID);
	update_goal_distance(state, distance);
}

int PlansGraph::get_goal_distance(const StateID& stateID) const {
	const GlobalState& state = registry->lookup_state(stateID);
	return get_goal_distance(state);
}

void PlansGraph::update_init_distance(const StateID& stateID, int distance) {
	const GlobalState& state = registry->lookup_state(stateID);
	update_init_distance(state, distance);
}

int PlansGraph::get_init_distance(const StateID& stateID) const {
	const GlobalState& state = registry->lookup_state(stateID);
	return get_init_distance(state);
}

void PlansGraph::update_init_distance(const GlobalState& state, int distance) {
	if (init_distance[state] == -1 || init_distance[state] > distance)
		init_distance[state] = distance;
}

int PlansGraph::get_init_distance(const GlobalState& state) const {
	if (init_distance[state] == -1)
		return std::numeric_limits<int>::max();
	return init_distance[state];
}

void PlansGraph::add_plan_reduce_order_neighbors(SearchEngine::Plan plan) {
	// Go over the plan, adding edges.
	// At each step, look forward for interchangeable operators (all pairwise interchangeable)
	// If found, add all these in a bfs manner

	GlobalState current = registry->get_initial_state();
	update_goal_distance(current, best_plan_cost);

	size_t curr_ind = 0;
	while (curr_ind < plan.size()) {
		int num_interchangeable = check_interchangeability(current, plan, curr_ind);

		add_ops_bfs(current, plan, curr_ind, num_interchangeable);
		// Getting the state that results from applying all these ops (in any order)
		for (int i = 0; i < num_interchangeable; ++i) {
			const GlobalOperator* op = plan[curr_ind];
			current = registry->get_successor_state(current, *op);
			curr_ind++;
		}
	}
}

void PlansGraph::add_ops_bfs(const GlobalState& start, SearchEngine::Plan plan, int curr_ind, int num_interchangeable) {

	StatesSet closed;

	deque<StateID> bfs_queue;
	bfs_queue.push_back(start.get_id());
	PerStateInformation<vector<bool>> covered;
	covered[start] = vector<bool>(num_interchangeable, false);
	while (!bfs_queue.empty()) {
		StateID currID = bfs_queue.front();
		bfs_queue.pop_front();
		if (closed.find(currID) != closed.end()) {
			//cout << "State " << currID << " is already closed" << endl;
			continue;
		}
		closed.insert(currID);
		const GlobalState& current = registry->lookup_state(currID);
		int current_cost = get_goal_distance(current);

		vector<bool>& current_covered = covered[current];
		//cout << "######### Expanding node " << current.get_id() << " (" << currID <<")" << endl;
		//current.dump_pddl();
		for (int i = 0; i < num_interchangeable; ++i) {
			if (current_covered[i])
				continue;
			const GlobalOperator* op = plan[curr_ind + i];
			// Check applicability
			if (!op->is_applicable(current)) {
	            utils::exit_with(utils::ExitCode::CRITICAL_ERROR);
			}

			//cout << " ----------------" << endl;
			//current.dump_pddl();
			//op->dump();
			const GlobalState& next = registry->get_successor_state(current, *op);
			if (closed.find(next.get_id()) != closed.end())
				continue;
			//current.dump_pddl();
			int op_no = get_op_index_hacked(op);
			add_edge(current, op_no, next);
			update_goal_distance(next, current_cost - op->get_cost());
			//current.dump_pddl();
			//TODO: Check if not copied twice
			vector<bool> second_covered = current_covered;
			second_covered[i] = true;
			//cout << "Adding to the queue:" << endl;
			bfs_queue.push_back(next.get_id());
			//next.dump_pddl();
			covered[next] = second_covered;
		}
	}
}


int PlansGraph::check_interchangeability(const GlobalState& current, SearchEngine::Plan plan, int from_index) const {
	for (size_t i=from_index+1; i < plan.size(); ++i) {
		const GlobalOperator* op = plan[i];
		int op_no = get_op_index_hacked(op);
		if (!op->is_applicable(current)) {
            return (i - from_index);
		}
		// Going over all operators up until now
		for (size_t j=from_index; j < i; ++j) {
			const GlobalOperator* prev_op = plan[j];
			int prev_op_no = get_op_index_hacked(prev_op);

			if (op_interaction->interfere(op_no, prev_op_no))
				return (i - from_index);
		}
	}
	return (plan.size() - from_index);
}

void PlansGraph::add_plan_bfs_naive(SearchEngine::Plan plan) {
	// Implements an addition of a single plan - naive
	// A BFS procedure is used to construct all possible orders.
	// Nodes are kept in a closed list (set) for duplicate detection.

	StatesSet closed;

	deque<StateID> bfs_queue;
	const GlobalState& init = registry->get_initial_state();

	bfs_queue.push_back(init.get_id());
	PerStateInformation<vector<bool>> covered;
	covered[init] = vector<bool>(plan.size(), false);
	while (!bfs_queue.empty()) {
		StateID currID =  bfs_queue.front();
		bfs_queue.pop_front();
		if (closed.find(currID) != closed.end()) {
			//cout << "State " << currID << " is already closed" << endl;
			continue;
		}
		closed.insert(currID);
		const GlobalState& current = registry->lookup_state(currID);

		vector<bool>& current_covered = covered[current];
		//cout << "######### Expanding node " << current.get_id() << " (" << currID <<")" << endl;
		//current.dump_pddl();
		for (size_t i = 0; i < plan.size(); ++i) {
			if (current_covered[i])
				continue;
			const GlobalOperator* op = plan[i];
			// Check applicability
			if (!op->is_applicable(current))
				continue;

			//cout << " ----------------" << endl;
			//current.dump_pddl();
			//op->dump();
			const GlobalState& next = registry->get_successor_state(current, *op);
			if (closed.find(next.get_id()) != closed.end())
				continue;
			//current.dump_pddl();
			int op_no = get_op_index_hacked(op);
			add_edge(current, op_no, next);
			//current.dump_pddl();
			//TODO: Check if not copied twice
			vector<bool> second_covered = current_covered;
			second_covered[i] = true;
			//cout << "Adding to the queue:" << endl;
			bfs_queue.push_back(next.get_id());
			//next.dump_pddl();
			covered[next] = second_covered;
		}
	}
	// Computing the distances!!
	// This is needed for DFS guidance when finding plans
	backwards_dijkstra();
	remove_non_plans_naive();
}

void PlansGraph::add_plan_dfs_naive(SearchEngine::Plan plan) {
	// Implements an addition of a single plan - naive
	// A DFS procedure is used to construct all possible orders.
	// Nodes are kept in a closed list (set) for duplicate detection.

	cout << "Starting adding plans in DFS manner... " << endl;

	size_t count_goal_states = 0;
	vector<shared_ptr<DfsNodeNaive>> dfs_queue;
	unordered_set<shared_ptr<DfsNodeNaive>> closed_list;

	const GlobalState& init = registry->get_initial_state();
	vector<size_t> init_ops;
	for (size_t i=0; i < plan.size(); ++i) {
		init_ops.push_back(i);
	}
	shared_ptr<DfsNodeNaive> init_node = make_shared<DfsNodeNaive>(init.get_id(), init_ops, nullptr);
	dfs_queue.push_back(init_node);
	//init_node.dump();

	while (!dfs_queue.empty() && count_goal_states < num_plans_to_dump) {
		//cout << "Getting node from the queue" << endl;
		shared_ptr<DfsNodeNaive> curr = dfs_queue.back();
		//curr.dump();
		dfs_queue.pop_back();
		const StateID& currID = curr->get_state();
		if (closed_list.find(curr) != closed_list.end()) {
			//cout << "State " << currID << " is already closed" << endl;
			continue;
		}
		closed_list.insert(curr);
		const GlobalState& current = registry->lookup_state(currID);
		//cout << "######### Expanding node " << current.get_id() << " (" << currID <<")" << endl;
		//current.dump_pddl();
		const vector<size_t>& remaining = curr->get_remaining_ops();
		vector<shared_ptr<DfsNodeNaive>> valid_successors;
		unordered_set<int> covered_op_no;
		for (size_t i = 0; i < remaining.size() ; ++i) {
			//cout << "Index " << i << " out of " << remaining.size() << endl;
			size_t op_idx = remaining[i];
			//cout << "Operator index " << op_idx << endl;
			const GlobalOperator* op = plan[op_idx];
			//cout << "Operator: " << op->get_name() << endl;
			int op_no = get_op_index_hacked(op);

			if (covered_op_no.find(op_no) != covered_op_no.end())
				continue;
			covered_op_no.insert(op_no);

			// Check applicability
			if (!op->is_applicable(current))
				continue;

			//cout << "Applicable, getting successor state" << endl;
			const GlobalState& next = registry->get_successor_state(current, *op);
			vector<size_t> next_ops;
			next_ops.insert(next_ops.end(), remaining.begin(), remaining.begin() + i);
			next_ops.insert(next_ops.end(), remaining.begin() + i + 1, remaining.end());

			add_edge(current, op_no, next);
			if (test_goal(next)) {
				// New goal found, count
				count_goal_states++;
				//cout << "Found " << count_goal_states << " goal states" << endl;
			}
			//TODO: Check if not copied twice
			//cout << "Adding to the queue:" << endl;
			shared_ptr<DfsNodeNaive> next_node = make_shared<DfsNodeNaive>(next.get_id(), next_ops, curr);
			valid_successors.push_back(next_node);
		}
		// Moving the unique successors to the queue
		unordered_set<shared_ptr<DfsNodeNaive>> succ_set;
		for (size_t i = 0; i < valid_successors.size() ; ++i) {
			shared_ptr<DfsNodeNaive> succ = valid_successors[valid_successors.size() - 1 - i];
			if (succ_set.find(succ) == succ_set.end()) {
				dfs_queue.push_back(succ);
				succ_set.insert(succ);
			}
		}
	}
	cout << "Computing the distances!!" << endl;
	// This  is needed for DFS guidance when finding plans
	backwards_dijkstra();
	// For dfs, computing init distances
	remove_non_plans_naive();
	forward_dijkstra();
	remove_init_unreachable();
}

void PlansGraph::add_plan_dfs_naive_no_duplicate_detection(SearchEngine::Plan plan) {
	// Implements an addition of a single plan - naive
	// A DFS procedure is used to construct all possible orders.

	cout << "Starting adding plans in DFS manner, no duplicate detection ... " << endl;

	size_t count_goal_states = 0;
	vector<shared_ptr<DfsNodeNaive>> dfs_queue;

	const GlobalState& init = registry->get_initial_state();
	vector<size_t> init_ops;
	for (size_t i=0; i < plan.size(); ++i) {
		init_ops.push_back(i);
	}
	shared_ptr<DfsNodeNaive> init_node = make_shared<DfsNodeNaive>(init.get_id(), init_ops, nullptr);
	dfs_queue.push_back(init_node);
	//init_node.dump();
	int num_edges_since_greedy_clean = 0;
	int num_goal_states_since_dump_intermediate = 0;
	while (!dfs_queue.empty()) {
		if (count_goal_states >= num_plans_to_dump) {
			// Checking if we can break here already - update the actual number of current optimal plans
			break;
		}
		//cout << "Getting node from the queue" << endl;
		shared_ptr<DfsNodeNaive> curr = dfs_queue.back();
		//curr.dump();
		dfs_queue.pop_back();
		const StateID& currID = curr->get_state();
		const GlobalState& current = registry->lookup_state(currID);
		const vector<size_t>& remaining = curr->get_remaining_ops();
		//cout << "######### Expanding node " << current.get_id() << " (" << currID <<"), " << remaining.size() << " remaining actions" << endl;
		//current.dump_pddl();
		vector<shared_ptr<DfsNodeNaive>> valid_successors;
		unordered_set<int> covered_op_no;
		for (size_t i = 0; i < remaining.size() ; ++i) {
		    // Getting the operator and checking that it was not previously used on the same parent.
			size_t op_idx = remaining[i];
			const GlobalOperator* op = plan[op_idx];
			int op_no = get_op_index_hacked(op);

			if (covered_op_no.find(op_no) != covered_op_no.end())
				continue;
			covered_op_no.insert(op_no);

			// Check applicability
			if (!op->is_applicable(current))
				continue;

			//cout << "Applicable, getting successor state" << endl;
			const GlobalState& next = registry->get_successor_state(current, *op);
			//cout << "Operator: " << op->get_name() << " generates node " << next.get_id() << endl;
			// Skipping loops
			if (curr->is_on_the_path(next.get_id()))
				continue;

			// Creating an operator vector for the next node.
			vector<size_t> next_ops;
			next_ops.insert(next_ops.end(), remaining.begin(), remaining.begin() + i);
			next_ops.insert(next_ops.end(), remaining.begin() + i + 1, remaining.end());

			bool new_edge = add_edge(current, op_no, next);
			if (new_edge)
			    num_edges_since_greedy_clean++;
			if (test_goal(next)) {
				// New goal found, count
				count_goal_states++;
				//cout << "Found " << count_goal_states << " goal states" << endl;
				num_goal_states_since_dump_intermediate++;
				//num_edges_since_greedy_prune = 0;
				// Not adding the goal successor to the DFS queue.
				continue;
			}
			//TODO: Check if not copied twice
			//cout << "Adding to the queue:" << endl;
			shared_ptr<DfsNodeNaive> next_node = make_shared<DfsNodeNaive>(next.get_id(), next_ops, curr);
			valid_successors.push_back(next_node);
		}
		// Moving the unique successors to the queue
		int num_added_successors = 0;
		unordered_set<shared_ptr<DfsNodeNaive>> succ_set;
		for (size_t i = 0; i < valid_successors.size() ; ++i) {
			shared_ptr<DfsNodeNaive> succ = valid_successors[valid_successors.size() - 1 - i];
			if (succ_set.find(succ) == succ_set.end()) {
				dfs_queue.push_back(succ);
				num_added_successors++;
				succ_set.insert(succ);
			}
		}
		if (num_added_successors) {
		    //cout << " ==> Added " << num_added_successors << " successors to the DFS queue with init distance " << plan.size() - remaining.size() + 1
		    //     << ",  queue size is now " << dfs_queue.size() << endl;
		} else {
		    //cout << " ==> Number of edges added since the last goal state was found: " << num_edges_since_goal_state << endl;
		    if (num_edges_until_greedy_clean != -1 && num_edges_since_greedy_clean > num_edges_until_greedy_clean) {
		        // Performing greedy clean
		        //TODO: Check that it is correct here to drop these steps in the non-optimal case
		        if (optimal) {
		            backwards_dijkstra();
		            remove_non_plans_naive();
		        }
		        if (num_goal_states_since_dump_intermediate > 0) {
		            dump_plans(num_plans_to_dump);
		            num_goal_states_since_dump_intermediate = 0;
		        }
		        num_edges_since_greedy_clean = 0;
		    }
		    curr.reset();
		}
	}
	cout << "Computing the distances!!" << endl;
	// This is needed for DFS guidance when finding plans
	backwards_dijkstra();
	remove_non_plans_naive();
	forward_dijkstra();
	remove_init_unreachable();
}

void PlansGraph::remove_all_nodes_and_edges() {
    nodes.clear();
    edges_by_source.clear();
    origin_states_by_operators.clear();
}

void PlansGraph::remove_non_plans_naive() {
	// Post-processing: leave only edges on optimal paths, by the cheapest distances.
	// We need to remove edges from edges_by_source and source ids from origin_states_by_operators
	StatesSet from_to_remove;
	for (const StateID& from : get_nodes()) {
		int from_dist = get_goal_distance(from);
		ToEdges& edges = get_outgoing_edges(from);
		vector<int> to_remove;
		for (auto edge : edges) {
			StateID to_state_id = edge.second;
			int to_dist = get_goal_distance(to_state_id);

			int op_no = edge.first;
			int op_cost = g_operators[op_no].get_cost();

			if (from_dist == std::numeric_limits<int>::max() ||
					to_dist == std::numeric_limits<int>::max() ||
					from_dist != to_dist + op_cost) {
				// Removing the transition
				to_remove.push_back(op_no);
			}
		}
		for (int op_no  : to_remove) {
			edges.erase(op_no);
			remove_origin_state_by_op(op_no, from);
		}
		// Removing empty nodes that are not goal
		if (edges.empty()) {
			const GlobalState& state = registry->lookup_state(from);
			if (!test_goal(state))
				from_to_remove.insert(from);
		}
	}
	for (const StateID& from : from_to_remove)
		nodes.erase(from);
}

void PlansGraph::remove_init_unreachable() {
	// Post-processing: remove nodes not reachable from the initial state
	StatesSet from_to_remove;
	for (const StateID& from : get_nodes()) {
		if (std::numeric_limits<int>::max() > get_init_distance(from))
			continue;

		ToEdges& edges = get_outgoing_edges(from);
		for (auto edge : edges) {
			int op_no = edge.first;
			remove_origin_state_by_op(op_no, from);
		}
		edges.clear();
		from_to_remove.insert(from);
	}
	for (const StateID& from : from_to_remove)
		nodes.erase(from);
}


bool PlansGraph::remove_origin_state_by_op(int op_no, const StateID& id) {
	StatesSet& origin_states = origin_states_by_operators[op_no];
	return origin_states.erase(id);
}

void PlansGraph::forward_dijkstra() {
	cout << "Running Dijkstra for computing distances from init!" << endl;
    priority_queues::AdaptiveQueue<StateID> open_queue;

	//	cout << "--------------------------------------------------------------------" << endl;
	for (const StateID& from : get_nodes()) {
		update_init_distance(from, -1);
	}
	const GlobalState& init = registry->get_initial_state();
	update_init_distance(init, 0);
	open_queue.push(0, init.get_id());
	while (!open_queue.empty()) {
		pair<int, StateID> top_pair = open_queue.pop();
       	int g = top_pair.first;
       	StateID from = top_pair.second;

   		for (auto edge : get_outgoing_edges(from)) {
   			int op_no = edge.first;
   			StateID to_state_id = edge.second;
       		int to_g = g + g_operators[op_no].get_cost();

       		if (to_g < get_init_distance(to_state_id)) {
				//cout << "   Updating value for " << from << " to value: " << from_g << endl;
       			update_init_distance(to_state_id, to_g);
       			open_queue.push(to_g, to_state_id);
       		}
       	}
	}
}

void PlansGraph::backwards_dijkstra() {
	cout << "Running Dijkstra!" << endl;
	// Building the backward graph - edges to nodes
    priority_queues::AdaptiveQueue<StateID> open_queue;
    std::unordered_map<StateID, std::vector<std::pair<int,StateID>>> edges_by_target;

	//cout << "--------------------------------------------------------------------" << endl;
	for (const StateID& from : get_nodes()) {
		const GlobalState& current = registry->lookup_state(from);
		if (test_goal(current)) {
	        open_queue.push(0, from);
	        update_goal_distance(current, 0);
			//cout << "Adding goal node with goal distance 0:  " << from << endl;
		} else {
			// Since earlier the goal distances for the original plan were already set
	        update_goal_distance(current, -1);
		}

		for (auto edge : get_outgoing_edges(from)) {
			int op_no = edge.first;
			StateID to_state_id = edge.second;
			if (edges_by_target.find(to_state_id) == edges_by_target.end())
				edges_by_target[to_state_id] = std::vector<std::pair<int,StateID>>();
			// Adding the backward edge
			//cout << "Added backward edge: " << to_state_id << " <- op: " << op_no << ", id: " << from  << " " << endl;
			edges_by_target[to_state_id].push_back(make_pair(op_no, from));
		}
	}

	//cout << "--------------------------------------------------------------------" << endl;
	while (!open_queue.empty()) {
		pair<int, StateID> top_pair = open_queue.pop();
        int to_g = top_pair.first;
        StateID to = top_pair.second;

        for (pair<int,StateID> transition : edges_by_target[to]) {
        	int op_no = transition.first;
        	StateID from = transition.second;

        	int from_g = to_g + g_operators[op_no].get_cost();

        	if (from_g < get_goal_distance(from)) {
				//cout << "   Updating value for " << from << " to value: " << from_g << endl;
        		update_goal_distance(from, from_g);
        		open_queue.push(from_g, from);
        	}
        }
	}
}

bool PlansGraph::add_symmetry(const Permutation* fact_symmetry, const OperatorPermutation* op_symmetry) {
	// TODO: Check if going over the operators and permuting the origin states can be faster

	// Implements addition of symmetric counterparts
	// Each node is permuted into a new node, and each operator is permuted into a new operator
	// Traversing the nodes and for each node permuting the outgoing edges

	bool changed = false;
	for (const StateID& from : get_nodes()) {
		//cout << " -------  Symmetry from state" << endl;
		//registry->lookup_state(from).dump_pddl();
		const GlobalState& from_state = registry->lookup_state(from);

		const GlobalState& permuted_from = registry->permute_state(from_state, fact_symmetry);
		update_goal_distance(permuted_from, get_goal_distance(from_state));
		//cout << "  to state" << endl;
		//permuted_from.dump_pddl();
		for (auto edge : get_outgoing_edges(from)) {
			int op_no = edge.first;
			//cout << "From op: " << g_operators[op_no].get_name() << endl;
			int permuted_op_no = op_symmetry->get_permuted_operator_no(op_no);
			//cout << "To op:   " << g_operators[permuted_op_no].get_name() << endl;

			const GlobalState& permuted_next = registry->get_successor_state(permuted_from, g_operators[permuted_op_no]);

			//cout << "To node: " << permuted_next.get_id() << endl;
			bool curr_change = add_edge(permuted_from, permuted_op_no, permuted_next);
			changed |= curr_change;
			if (curr_change) {
				StateID& to = edge.second;
				const GlobalState& to_state = registry->lookup_state(to);
				update_goal_distance(permuted_next, get_goal_distance(to_state));
				//cout << "Added edge!" << endl;
			}
		}
	}
	return changed;
}


void PlansGraph::dump_graph() {
	for (const StateID& from : get_nodes()) {
		GlobalState from_state = registry->lookup_state(from);
		cout << "-------------------------------------------------------" << endl;
		cout << "  " << from << endl;
		//from_state.dump_pddl();

		for (auto edge : get_outgoing_edges(from)) {
			int op_no = edge.first;
			const StateID& to = edge.second;

			cout << "    " << g_operators[op_no].get_name() << "       " << to <<endl;
		}
	}
}

void PlansGraph::dump_graph_dot(bool verbose) {
	dump_graph_dot(g_num_previously_generated_plans, verbose);
}

void PlansGraph::dump_graph_dot(int num_plan, bool verbose) {
	ofstream os("graph" + std::to_string(num_plan) + ".dot");
	os << "digraph {\n";

	for (const StateID& from : get_nodes()) {
		GlobalState from_state = registry->lookup_state(from);
		os << from.hash() << " [ peripheries=\"1\", shape=\"rectangle\", ";

		if (test_goal(from_state)) {
			os << "style=\"rounded, filled\", fillcolor=\"red\", ";
		} else {
			os << "style=\"rounded, filled\", fillcolor=\"yellow\", ";
		}
		os << "label=\"#"<< from.hash() << "\\n";
		os << " init dist: " << get_init_distance(from_state) << "\\n";
		os << " dist: " << get_goal_distance(from_state) << "\\n";
		if (verbose)
			os << state_label(from_state) << "\\n";
		os << "\" ]\n";
	}

	for (const StateID& from : get_nodes()) {
		for (auto edge : get_outgoing_edges(from)) {
			int op_no = edge.first;
			const StateID& to = edge.second;

			os << from.hash() << "  ->  " << to.hash();
			os <<" [ label=\"#" << g_operators[op_no].get_name();
			os << "/"<< g_operators[op_no].get_cost();
			os << "\" ]\n";

		}
	}
	os << "}\n" << endl;
	os.close();
}


void PlansGraph::dump_graph_file() {
	ofstream os("graph" + std::to_string(g_num_previously_generated_plans) + ".txt");
	for (const StateID& from : get_nodes()) {
		GlobalState from_state = registry->lookup_state(from);
		os << from.hash() << ", " << test_goal(from_state) << "\n";
	}

	for (const StateID& from : get_nodes()) {
		for (auto edge : get_outgoing_edges(from)) {
			int op_no = edge.first;
			const StateID& to = edge.second;

			os << from.hash() << ", " << g_operators[op_no].get_name() << ", " << to.hash();
			os << "\n";
		}
	}
	os.close();
}


bool PlansGraph::add_edge(const GlobalState& current, int op_no, const GlobalState& next) {
	// Adding a single edge
	// Returns true if the edge did not previously exist
	nodes.insert(current.get_id());
	bool ret = false;
	ToEdges& edges = get_outgoing_edges(current);
	ToEdges::iterator it = edges.find(op_no);
	//cout << "Adding edge: <" << current.get_id() << ", " << g_operators[op_no].get_name() << ", " << next.get_id() << ">"  << endl;
	if (it == edges.end()) {
		edges.insert(std::pair<int, StateID>(op_no, next.get_id()));
		origin_states_by_operators[op_no].insert(current.get_id());
		//cout << " YES";
		ret = true;
		nodes.insert(next.get_id());
	}

	//cout << endl;
	return ret;
}

void PlansGraph::dump_plans(size_t number_of_plans) {
	find_plans_dfs(number_of_plans);
	cout << "Dumped " << optimal_plans.size() << (optimal ? " optimal ": " ") << "plans for this iteration, found additional " << non_optimal_plans.size() << " plans. [t=" << utils::g_timer << "]" << endl;
	for (SearchEngine::Plan plan : non_optimal_plans) {
		save_plan(plan, true);
	}
}

void PlansGraph::find_plans_dfs(size_t number_of_plans) {
	//cout << "Start dumping plans, optimal cost is " << best_plan_cost << endl;

	// Using DFS to find plans
	// Using a set of visited states along the path, to escape loops
	vector<shared_ptr<DFSNode>> dfs_queue;
	const GlobalState& init = registry->get_initial_state();
	shared_ptr<DFSNode> init_node = make_shared<DFSNode>(init.get_id(), nullptr, -1, 0);
	dfs_queue.push_back(init_node);

	while (!dfs_queue.empty()) {
		if (number_of_plans <= optimal_plans.size())
			return;
		shared_ptr<DFSNode> node = dfs_queue.back();
		dfs_queue.pop_back();
		//cout << "Getting node with path cost " << node->plan_cost << endl;

		const GlobalState& current = registry->lookup_state(node->state_id);

		if (test_goal(current)) {
			// Check for plan cost, if optimal then save and increase count, otherwise save for later
			// Reconstruct plan
			//cout << "Goal found, reconstructing plan" << endl;
			SearchEngine::Plan current_plan;
			shared_ptr<DFSNode> curr_node = node;
			while (true) {
				int op_no = curr_node->achieving_op;
				if (op_no == -1)
					break;
				current_plan.push_back(&g_operators[op_no]);
				curr_node = curr_node->parent;
			}
			// Revert plan
			std::reverse(current_plan.begin(),current_plan.end());
			//cout << "Reconstruction done, saving plan" << endl;
			if (!optimal || node->plan_cost == best_plan_cost) {
			    std::pair<PlansSet::iterator, bool > result = optimal_plans.insert(current_plan);
			    if (result.second) {
			        save_plan(current_plan, true);
					//cout << "Plan cost: " << best_plan_cost << endl;
			    }
			} else {
				non_optimal_plans.insert(current_plan);
				//cout << "Found non optimal plan" << endl;
			}
			//continue;
		}
	    priority_queues::AdaptiveQueue<pair<int,StateID>> priority_queue;
	    assert(priority_queue.empty());
		ToEdges& edges = get_outgoing_edges(node->state_id);
		//cout << "Adding virtual pushes: " << edges.size() << endl;
	    priority_queue.add_virtual_pushes(max((int)edges.size(),best_plan_cost));
	    //int num_pushes = 0;
		for (auto edge : edges) {
			int op_no = edge.first;
			StateID nextID = edge.second;
			// Checking if node is not already on the path
			// Need for not going into loops
			if (node->is_on_the_path(nextID))
				continue;

			int distance = get_goal_distance(nextID);
			if (distance == std::numeric_limits<int>::max()) {
				continue;
			}
			priority_queue.push(best_plan_cost - distance, make_pair(op_no, nextID));
			//num_pushes++;
		}
		//cout << "Actual number of pushes: " << num_pushes << endl;

		while (!priority_queue.empty()) {
			pair<int, pair<int,StateID>> top_pair = priority_queue.pop();

			StateID nextID = top_pair.second.second;
			int op_no = top_pair.second.first;

			int next_cost = node->plan_cost + g_operators[op_no].get_cost();
			shared_ptr<DFSNode> next_node = make_shared<DFSNode>(nextID, node, op_no, next_cost);
			dfs_queue.push_back(next_node);
		}
	}
}

void PlansGraph::dump_plan(vector<int>& ops_so_far) {
	cout << "----------------------------------" << endl;
	cout << "Plan number " << optimal_plans.size() << endl;

	for (size_t i = 0; i < ops_so_far.size(); ++i) {
		int op_no = ops_so_far[i];
		const GlobalOperator& op = g_operators[op_no];
        cout << op.get_name() << " (" << op.get_cost() << ")" << endl;
	}
}

inline vector<int> get_ops_no_from_name(string op_name) {
	vector<int> ret;
	for (size_t op_no = 0; op_no < g_operators.size(); ++op_no) {
		if (g_operators[op_no].get_name().compare(op_name) == 0)
			ret.push_back(op_no);
	}
	return ret;
}


void PlansGraph::add_non_deterministic_plan(vector<string> plan) {
	GlobalState current = registry->get_initial_state();
	for (size_t i=0; i < plan.size(); ++i) {
		string op_name = plan[i];
		for (int op_no : get_ops_no_from_name(op_name)) {
			const GlobalOperator& op = g_operators[op_no];
			if (!op.is_applicable(current)) {
	            utils::exit_with(utils::ExitCode::CRITICAL_ERROR);
			}
			const GlobalState& next = registry->get_successor_state(current, op);
			add_edge(current, op_no, next);
			current = next;
		}
	}
}

}
