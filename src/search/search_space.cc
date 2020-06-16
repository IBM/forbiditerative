#include "search_space.h"

#include "global_operator.h"
#include "global_state.h"
#include "globals.h"
#include "successor_generator.h"

#include "structural_symmetries/group.h"
#include "structural_symmetries/permutation.h"

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
    info.creating_operator = -1;
}

void SearchNode::open(const SearchNode &parent_node,
                      const GlobalOperator *parent_op) {
    assert(info.status == SearchNodeInfo::NEW);
    info.status = SearchNodeInfo::OPEN;
    info.g = parent_node.info.g + get_adjusted_action_cost(*parent_op, cost_type);
    info.real_g = parent_node.info.real_g + parent_op->get_cost();
    info.parent_state_id = parent_node.get_state_id();
    info.creating_operator = get_op_index_hacked(parent_op);
}

void SearchNode::reopen(const SearchNode &parent_node,
                        const GlobalOperator *parent_op) {
    assert(info.status == SearchNodeInfo::OPEN ||
           info.status == SearchNodeInfo::CLOSED);

    // The latter possibility is for inconsistent heuristics, which
    // may require reopening closed nodes.
    info.status = SearchNodeInfo::OPEN;
    info.g = parent_node.info.g + get_adjusted_action_cost(*parent_op, cost_type);
    info.real_g = parent_node.info.real_g + parent_op->get_cost();
    info.parent_state_id = parent_node.get_state_id();
    info.creating_operator = get_op_index_hacked(parent_op);
}

// like reopen, except doesn't change status
void SearchNode::update_parent(const SearchNode &parent_node,
                               const GlobalOperator *parent_op) {
    assert(info.status == SearchNodeInfo::OPEN ||
           info.status == SearchNodeInfo::CLOSED);
    // The latter possibility is for inconsistent heuristics, which
    // may require reopening closed nodes.
    info.g = parent_node.info.g + get_adjusted_action_cost(*parent_op, cost_type);
    info.real_g = parent_node.info.real_g + parent_op->get_cost();
    info.parent_state_id = parent_node.get_state_id();
    info.creating_operator = get_op_index_hacked(parent_op);
}

void SearchNode::close() {
    assert(info.status == SearchNodeInfo::OPEN);
    info.status = SearchNodeInfo::CLOSED;
}

void SearchNode::mark_as_dead_end() {
    info.status = SearchNodeInfo::DEAD_END;
}

void SearchNode::dump() const {
    cout << state_id << ": ";
    get_state().dump_fdr();
    if (info.creating_operator != -1) {
        cout << " created by " << g_operators[info.creating_operator].get_name()
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
                             vector<const GlobalOperator *> &path,
                             const shared_ptr<Group> &group) const {
    if (group && group->has_symmetries()) {
        trace_path_with_symmetries(goal_state, path, group);
    } else {
        GlobalState current_state = goal_state;
        assert(path.empty());
        for (;;) {
            const SearchNodeInfo &info = search_node_infos[current_state];
            if (info.creating_operator == -1) {
                assert(info.parent_state_id == StateID::no_state);
                break;
            }
            assert(utils::in_bounds(info.creating_operator, g_operators));
            const GlobalOperator *op = &g_operators[info.creating_operator];
            path.push_back(op);
            current_state = state_registry.lookup_state(info.parent_state_id);
        }
        reverse(path.begin(), path.end());
    }
}

void SearchSpace::trace_path_with_symmetries(const GlobalState &goal_state,
                                             vector<const GlobalOperator *> &path,
                                             const shared_ptr<Group> &group) const {
    vector<Permutation *> permutations;
    vector<GlobalState> state_trace;
    GlobalState current_state = goal_state;
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

    assert(path.empty());
    for (;;) {
        const SearchNodeInfo &info = search_node_infos[current_state];
        assert(info.status != SearchNodeInfo::NEW);
        int op_no = info.creating_operator;
        state_trace.push_back(current_state);
        // Important: new_state needs to be the initial state!
        GlobalState parent_state = state_registry.get_initial_state();
        GlobalState new_state = state_registry.get_initial_state();
        if (op_no != -1) {
            parent_state = state_registry.lookup_state(info.parent_state_id);
            const GlobalOperator *op = &g_operators[op_no];
            new_state = successor_registry->get_successor_state(parent_state, *op);
        }
        Permutation *p;
        if (new_state.get_id() != current_state.get_id()){
            p = group->create_permutation_from_state_to_state(current_state, new_state);
        } else {
            //p = new Permutation();
            p = group->new_identity_permutation();
        }
        permutations.push_back(p);
        if (op_no == -1)
            break;
        current_state = parent_state;
    }
    assert(state_trace.size() == permutations.size());
    vector<Permutation *> reverse_permutations;
//    Permutation *temp_p = new Permutation();
    Permutation *temp_p = group->new_identity_permutation();
    // Store another pointer to the id permutation and delete it in the first
    // iteration below. All other temp_p permutations cannot be deleted
    // because they are kept in reverse_permutations.
    Permutation *to_delete = temp_p;
    while (permutations.begin() != permutations.end()) {
        Permutation *p = permutations.back();
        temp_p = new Permutation(p, temp_p);
        if (to_delete) {
            delete to_delete;
            to_delete = 0;
        }
        reverse_permutations.push_back(temp_p);
        permutations.pop_back();
        delete p;
    }
    for (size_t i = 0; i < state_trace.size(); ++i){
        Permutation *permutation = reverse_permutations[state_trace.size() - i-1];
        state_trace[i] = successor_registry->permute_state(state_trace[i],
                                                           permutation);
        delete permutation;
        permutation = 0;
    }
    for (int i = state_trace.size() - 1; i > 0; i--) {
        vector<const GlobalOperator *> applicable_ops;
        g_successor_generator->generate_applicable_ops(state_trace[i], applicable_ops);
        bool found = false;
        int min_cost_op=0;
        int min_cost=numeric_limits<int>::max();

        for (size_t o = 0; o < applicable_ops.size(); o++) {
            const GlobalOperator *op = applicable_ops[o];
            GlobalState succ_state = successor_registry->get_successor_state(state_trace[i], *op);
            if (succ_state.get_id() == state_trace[i-1].get_id()) {
                found = true;
                if (op->get_cost() < min_cost) {
                    min_cost = op->get_cost();
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

void SearchSpace::trace_from_plan(const std::vector<const GlobalOperator *> &plan, std::vector<StateID> &plan_trace) const {
    assert(plan_trace.size() == 0);
    GlobalState current_state = state_registry.get_initial_state();
    for (size_t i=0; i < plan.size(); ++i) {
        const GlobalOperator *op = plan[i];
        current_state = state_registry.get_successor_state(current_state, *op);
        plan_trace.push_back(current_state.get_id());
    }
}

void SearchSpace::dump_trace(const std::vector<StateID> &plan_trace, std::ostream& os) const {

    GlobalState current_state = state_registry.get_initial_state();
    dump_state(os, current_state);
    for (size_t i=0; i < plan_trace.size(); ++i) {
        os << "," << endl;
        dump_state(os, state_registry.lookup_state(plan_trace[i]));
    }
}

void SearchSpace::dump_state(std::ostream& os, const GlobalState& state) const {
    int num_vars = state_registry.get_num_variables();
    vector<string> names;
    for (int var=0; var < num_vars; ++var) {
        string fact_name = state_registry.get_task().get_fact_name(FactPair(var, state[var]));        
        if (fact_name == "<none of those>")
            continue;
        if (fact_name.compare(0, 11, "NegatedAtom") == 0)
            continue;
        names.push_back(fact_name);
    }

    os << "[" << endl;
    size_t i = 0;
    for (; i < names.size() - 1; ++i) {
        os << "\"" << names[i] << "\"," << endl;
    }
    os << "\"" << names[i] << "\"" << endl;
    os << "]" << endl;
}

void SearchSpace::dump_partial_order_from_plan(const std::vector<const GlobalOperator *> &plan, std::ostream& os) const {
    if (plan.size() == 0) 
        return;

    // Checking which effects fire
    GlobalState current_state = state_registry.get_initial_state();
    vector<vector<GlobalEffect>> plan_firing_effects;

    for (size_t i=0; i < plan.size(); ++i) {
        const GlobalOperator *op = plan[i];
        vector<GlobalEffect> firing_effects;
        for (GlobalEffect eff : op->get_effects()) {
            if (eff.does_fire(current_state))
                firing_effects.push_back(eff);
        }
        plan_firing_effects.push_back(firing_effects);
        current_state = state_registry.get_successor_state(current_state, *op);
    }
    bool dumped_first = false;
    for (size_t i = plan.size() - 1; i > 0; --i ) { 
        for ( GlobalCondition cond : plan[i]->get_preconditions()) {
            bool dumped = dump_causal_link(plan, plan_firing_effects, i, cond, os, dumped_first);
            if (dumped) {
                dumped_first = true;
            }
        }
    }
}

bool SearchSpace::dump_causal_link(const std::vector<const GlobalOperator *> &plan, 
                                    const std::vector<std::vector<GlobalEffect>>& plan_firing_effects,
                                    size_t to_index, GlobalCondition cond, std::ostream& os, bool coma) const {
    for (size_t i = to_index; i > 0; ) { 
        --i;
        // Check if cond is in action effects

        for (GlobalEffect eff : plan_firing_effects[i]) {
            if (eff.var != cond.var || eff.val != cond.val) {
                continue;
            }
            if (coma) {
                os << ",";
            }
            os << "{ \"from\": \"" << plan[i]->get_name() << "\", "
               << "\"fact\": \"" << state_registry.get_task().get_fact_name(FactPair(eff.var, eff.val)) << "\", "
               << "\"to\": \"" << plan[to_index]->get_name() << "\"}" << endl;
            return true;
        }
    }
    return false;
}

void SearchSpace::dump() const {
    for (PerStateInformation<SearchNodeInfo>::const_iterator it =
             search_node_infos.begin(&state_registry);
         it != search_node_infos.end(&state_registry); ++it) {
        StateID id = *it;
        GlobalState state = state_registry.lookup_state(id);
        const SearchNodeInfo &node_info = search_node_infos[state];
        cout << id << ": ";
        state.dump_fdr();
        if (node_info.creating_operator != -1 &&
            node_info.parent_state_id != StateID::no_state) {
            cout << " created by " << g_operators[node_info.creating_operator].get_name()
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
