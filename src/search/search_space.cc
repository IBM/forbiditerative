#include "search_space.h"

#include "global_state.h"
#include "globals.h"
#include "task_proxy.h"

#include "structural_symmetries/group.h"
#include "structural_symmetries/permutation.h"
#include "task_utils/successor_generator.h"

#include <cassert>
#include "search_node_info.h"

using namespace std;

SearchNode::SearchNode(const StateRegistry &state_registry,
                       StateID state_id,
                       SearchNodeInfo &info,
                       OperatorCost cost_type)
    : state_registry(state_registry),
      state_id(state_id),
      info(info),
      cost_type(cost_type) {
    assert(state_id != StateID::no_state);
}

GlobalState SearchNode::get_state() const {
    return state_registry.lookup_state(state_id);
}

bool SearchNode::is_open() const {
    return info.status == SearchNodeInfo::OPEN;
}

bool SearchNode::is_closed() const {
    return info.status == SearchNodeInfo::CLOSED;
}

bool SearchNode::is_dead_end() const {
    return info.status == SearchNodeInfo::DEAD_END;
}

bool SearchNode::is_new() const {
    return info.status == SearchNodeInfo::NEW;
}

int SearchNode::get_g() const {
    assert(info.g >= 0);
    return info.g;
}

int SearchNode::get_real_g() const {
    return info.real_g;
}

void SearchNode::open_initial() {
    assert(info.status == SearchNodeInfo::NEW);
    info.status = SearchNodeInfo::OPEN;
    info.g = 0;
    info.real_g = 0;
    info.parent_state_id = StateID::no_state;
    info.creating_operator = OperatorID::no_operator;
}

void SearchNode::open(const SearchNode &parent_node,
                      const OperatorProxy &parent_op) {
    assert(info.status == SearchNodeInfo::NEW);
    info.status = SearchNodeInfo::OPEN;
    info.g = parent_node.info.g + get_adjusted_action_cost(parent_op, cost_type);
    info.real_g = parent_node.info.real_g + parent_op.get_cost();
    info.parent_state_id = parent_node.get_state_id();
    info.creating_operator = OperatorID(parent_op.get_id());
}

void SearchNode::reopen(const SearchNode &parent_node,
                        const OperatorProxy &parent_op) {
    assert(info.status == SearchNodeInfo::OPEN ||
           info.status == SearchNodeInfo::CLOSED);

    // The latter possibility is for inconsistent heuristics, which
    // may require reopening closed nodes.
    info.status = SearchNodeInfo::OPEN;
    info.g = parent_node.info.g + get_adjusted_action_cost(parent_op, cost_type);
    info.real_g = parent_node.info.real_g + parent_op.get_cost();
    info.parent_state_id = parent_node.get_state_id();
    info.creating_operator = OperatorID(parent_op.get_id());
}

// like reopen, except doesn't change status
void SearchNode::update_parent(const SearchNode &parent_node,
                               const OperatorProxy &parent_op) {
    assert(info.status == SearchNodeInfo::OPEN ||
           info.status == SearchNodeInfo::CLOSED);
    // The latter possibility is for inconsistent heuristics, which
    // may require reopening closed nodes.
    info.g = parent_node.info.g + get_adjusted_action_cost(parent_op, cost_type);
    info.real_g = parent_node.info.real_g + parent_op.get_cost();
    info.parent_state_id = parent_node.get_state_id();
    info.creating_operator = OperatorID(parent_op.get_id());
}

void SearchNode::close() {
    assert(info.status == SearchNodeInfo::OPEN);
    info.status = SearchNodeInfo::CLOSED;
}

void SearchNode::mark_as_dead_end() {
    info.status = SearchNodeInfo::DEAD_END;
}

void SearchNode::dump(const TaskProxy &task_proxy) const {
    cout << state_id << ": ";
    get_state().dump_fdr();
    if (info.creating_operator != OperatorID::no_operator) {
        OperatorsProxy operators = task_proxy.get_operators();
        OperatorProxy op = operators[info.creating_operator.get_index()];
        cout << " created by " << op.get_name()
             << " from " << info.parent_state_id << endl;
    } else {
        cout << " no parent" << endl;
    }
}

SearchSpace::SearchSpace(StateRegistry &state_registry, OperatorCost cost_type)
    : state_registry(state_registry),
      cost_type(cost_type) {
}

SearchNode SearchSpace::get_node(const GlobalState &state) {
    return SearchNode(
        state_registry, state.get_id(), search_node_infos[state], cost_type);
}

void SearchSpace::trace_path(const GlobalState &goal_state,
                             vector<OperatorID> &path,
                             const TaskProxy &task_proxy,
                             const shared_ptr<Group> &group) const {
    if (group && group->has_symmetries()) {
        trace_path_with_symmetries(goal_state, path, task_proxy, group);
        return;
    }
    GlobalState current_state = goal_state;
    assert(path.empty());
    for (;;) {
        const SearchNodeInfo &info = search_node_infos[current_state];
        if (info.creating_operator == OperatorID::no_operator) {
            assert(info.parent_state_id == StateID::no_state);
            break;
        }
        path.push_back(info.creating_operator);
        current_state = state_registry.lookup_state(info.parent_state_id);
    }
    reverse(path.begin(), path.end());
}

void SearchSpace::trace_path_with_symmetries(const GlobalState &goal_state,
                                             vector<OperatorID> &path,
                                             const TaskProxy &task_proxy,
                                             const shared_ptr<Group> &group) const {
    assert(path.empty());
    OperatorsProxy operators = task_proxy.get_operators();

    /*
      For DKS, we need to use a separate registry to generate successor states
      to avoid generating the symmetrical successor state, which could equal
      the current_state of the state trace.

      For OSS; we can use the regular registry, as it works directly on
      canonical representatives.
    */
    StateRegistry dks_successor_state_registry(
        *g_root_task(), *g_state_packer, *g_axiom_evaluator, g_initial_state_data);

    StateRegistry *successor_registry =
        group->get_search_symmetries() == SearchSymmetries::DKS ?
                &dks_successor_state_registry : &state_registry;

    vector<RawPermutation> permutations;
    vector<GlobalState> state_trace;
    GlobalState current_state = goal_state;
    while (true) {
        const SearchNodeInfo &info = search_node_infos[current_state];
        assert(info.status != SearchNodeInfo::NEW);
        OperatorID op_id = info.creating_operator;
        state_trace.push_back(current_state);
        // Important: new_state needs to be the initial state!
        GlobalState parent_state = state_registry.get_initial_state();
        GlobalState new_state = state_registry.get_initial_state();
        if (op_id != OperatorID::no_operator) {
            parent_state = state_registry.lookup_state(info.parent_state_id);
            new_state = successor_registry->get_successor_state(parent_state, operators[op_id]);
        }
        RawPermutation p;
        if (new_state.get_id() != current_state.get_id()){
            p = group->create_permutation_from_state_to_state(current_state, new_state);
        } else {
            p = group->new_identity_raw_permutation();
        }
        permutations.push_back(move(p));
        if (op_id == OperatorID::no_operator)
            break;
        current_state = parent_state;
    }
    assert(state_trace.size() == permutations.size());
    vector<RawPermutation> reverse_permutations;
    RawPermutation temp_p = group->new_identity_raw_permutation();
    while (permutations.begin() != permutations.end()) {
        const RawPermutation &p = permutations.back();
        temp_p = group->compose_permutations(p, temp_p);
        reverse_permutations.push_back(temp_p);
        permutations.pop_back();
    }
    for (size_t i = 0; i < state_trace.size(); ++i){
        const RawPermutation &permutation = reverse_permutations[state_trace.size() - i-1];
        state_trace[i] = successor_registry->permute_state(state_trace[i],
                                                           Permutation(*group, permutation));
    }
    reverse_permutations.clear();
    for (int i = state_trace.size() - 1; i > 0; i--) {
        vector<OperatorID> applicable_ops;
        g_successor_generator->generate_applicable_ops(state_trace[i], applicable_ops);
        bool found = false;
        int min_cost_op=0;
        int min_cost=numeric_limits<int>::max();

        for (size_t o = 0; o < applicable_ops.size(); o++) {
            OperatorProxy op = operators[applicable_ops[o]];
            GlobalState succ_state = successor_registry->get_successor_state(state_trace[i], op);
            if (succ_state.get_id() == state_trace[i-1].get_id()) {
                found = true;
                if (op.get_cost() < min_cost) {
                    min_cost = op.get_cost();
                    min_cost_op = o;
                }
            }
        }
        if (!found) {
            cout << "No operator is found!!!" << endl
                 << "Cannot reach the state " << endl;
            state_trace[i-1].dump_pddl();
            cout << endl << "From the state" << endl;
            state_trace[i].dump_pddl();
            utils::exit_with(utils::ExitCode::CRITICAL_ERROR);
        }
        path.push_back(applicable_ops[min_cost_op]);
    }
}

void SearchSpace::dump(const TaskProxy &task_proxy) const {
    OperatorsProxy operators = task_proxy.get_operators();
    for (StateID id : state_registry) {
        /* The body duplicates SearchNode::dump() but we cannot create
           a search node without discarding the const qualifier. */
        GlobalState state = state_registry.lookup_state(id);
        const SearchNodeInfo &node_info = search_node_infos[state];
        cout << id << ": ";
        state.dump_fdr();
        if (node_info.creating_operator != OperatorID::no_operator &&
            node_info.parent_state_id != StateID::no_state) {
            OperatorProxy op = operators[node_info.creating_operator.get_index()];
            cout << " created by " << op.get_name()
                 << " from " << node_info.parent_state_id << endl;
        } else {
            cout << "has no parent" << endl;
        }
    }
}

void SearchSpace::print_statistics() const {
    cout << "Number of registered states: "
         << state_registry.size() << endl;
}
