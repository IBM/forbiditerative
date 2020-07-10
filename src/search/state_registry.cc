#include "state_registry.h"

#include "per_state_information.h"
#include "task_proxy.h"

#include "structural_symmetries/group.h"
#include "structural_symmetries/permutation.h"

#include "task_utils/task_properties.h"
#include "utils/logging.h"

using namespace std;

StateRegistry::StateRegistry(const TaskProxy &task_proxy)
    : task_proxy(task_proxy),
      state_packer(task_properties::g_state_packers[task_proxy]),
      axiom_evaluator(g_axiom_evaluators[task_proxy]),
      num_variables(task_proxy.get_variables().size()),
      state_data_pool(get_bins_per_state()),
      canonical_state_data_pool(get_bins_per_state()),
      registered_states(
          StateIDSemanticHash(state_data_pool, get_bins_per_state()),
          StateIDSemanticEqual(state_data_pool, get_bins_per_state())),
      canonical_registered_states(
          StateIDSemanticHash(canonical_state_data_pool, get_bins_per_state()),
          StateIDSemanticEqual(canonical_state_data_pool, get_bins_per_state())),
      group(0),
      has_symmetries_and_uses_dks(false),
      cached_initial_state(0) {
}


StateRegistry::~StateRegistry() {
    delete cached_initial_state;
}

void StateRegistry::set_group(const shared_ptr<Group> &group_) {
    // Group is only set from eager_search if it has symmetries and uses DKS.
    group = group_;
    has_symmetries_and_uses_dks = true;
}

StateID StateRegistry::insert_id_or_pop_state() {
    if (has_symmetries_and_uses_dks) {
        return insert_id_or_pop_state_dks();
    }
    /*
      Attempt to insert a StateID for the last state of state_data_pool
      if none is present yet. If this fails (another entry for this state
      is present), we have to remove the duplicate entry from the
      state data pool.
    */
    StateID id(state_data_pool.size() - 1);
    pair<int, bool> result = registered_states.insert(id.value);
    bool is_new_entry = result.second;
    if (!is_new_entry) {
        state_data_pool.pop_back();
    }
    assert(registered_states.size() == static_cast<int>(state_data_pool.size()));
    return StateID(result.first);
}

StateID StateRegistry::insert_id_or_pop_state_dks() {
    /*
      Attempt to insert a StateID for the last state of state_data_pool
      if none is present yet. If this fails (another entry for this state
      is present), we have to remove the duplicate entry from the
      state data pool.
    */
    StateID id(state_data_pool.size() - 1);
    // Adding an entry for the canonical state to the canonical_state_data_pool
    vector<int> canonical_state =
        group->get_canonical_representative(
            GlobalState(state_data_pool[state_data_pool.size() - 1], *this, id));
    PackedStateBin *canonical_buffer = new PackedStateBin[state_packer.get_num_bins()];
    fill_n(canonical_buffer, state_packer.get_num_bins(), 0);
    for (int i = 0; i < num_variables; ++i) {
        state_packer.set(canonical_buffer, i, canonical_state[i]);
    }
    canonical_state_data_pool.push_back(canonical_buffer);
    delete[] canonical_buffer;

    pair<int, bool> result = canonical_registered_states.insert(id.value);
    bool is_new_entry = result.second;
    if (!is_new_entry) {
        state_data_pool.pop_back();
        canonical_state_data_pool.pop_back();
    }
    assert(canonical_registered_states.size() == static_cast<int>(state_data_pool.size()));
    return StateID(result.first);
}

GlobalState StateRegistry::lookup_state(StateID id) const {
    return GlobalState(state_data_pool[id.value], *this, id);
}

const GlobalState &StateRegistry::get_initial_state() {
    if (cached_initial_state == 0) {
        PackedStateBin *buffer = new PackedStateBin[get_bins_per_state()];
        // Avoid garbage values in half-full bins.
        fill_n(buffer, get_bins_per_state(), 0);

        State initial_state = task_proxy.get_initial_state();
        for (size_t i = 0; i < initial_state.size(); ++i) {
            state_packer.set(buffer, i, initial_state[i].get_value());
        }
        state_data_pool.push_back(buffer);
        // buffer is copied by push_back
        delete[] buffer;
        StateID id = insert_id_or_pop_state();
        cached_initial_state = new GlobalState(lookup_state(id));
    }
    return *cached_initial_state;
}

//TODO it would be nice to move the actual state creation (and operator application)
//     out of the StateRegistry. This could for example be done by global functions
//     operating on state buffers (PackedStateBin *).
GlobalState StateRegistry::get_successor_state(const GlobalState &predecessor, const OperatorProxy &op) {
    assert(!op.is_axiom());
    state_data_pool.push_back(predecessor.get_packed_buffer());
    PackedStateBin *buffer = state_data_pool[state_data_pool.size() - 1];
    for (EffectProxy effect : op.get_effects()) {
        if (does_fire(effect, predecessor)) {
            FactPair effect_pair = effect.get_fact().get_pair();
            state_packer.set(buffer, effect_pair.var, effect_pair.value);
        }
    }
    axiom_evaluator.evaluate(buffer, state_packer);
    StateID id = insert_id_or_pop_state();
    return lookup_state(id);
}

GlobalState StateRegistry::register_state_buffer(const vector<int> &state) {
    PackedStateBin *buffer = new PackedStateBin[state_packer.get_num_bins()];
    fill_n(buffer, state_packer.get_num_bins(), 0);
    for (int i = 0; i < num_variables; ++i) {
        state_packer.set(buffer, i, state[i]);
    }
    state_data_pool.push_back(buffer);
    delete[] buffer;
    StateID id = insert_id_or_pop_state();
    return lookup_state(id);
}

GlobalState StateRegistry::permute_state(const GlobalState &state, const Permutation &permutation) {
    PackedStateBin *buffer = new PackedStateBin[state_packer.get_num_bins()];
    fill_n(buffer, state_packer.get_num_bins(), 0);
    for (int i = 0; i < num_variables; ++i) {
        pair<int, int> var_val = permutation.get_new_var_val_by_old_var_val(i, state[i]);
        assert(var_val.second < task_proxy.get_variables()[var_val.first].get_domain_size());
        state_packer.set(buffer, var_val.first, var_val.second);
    }
    state_data_pool.push_back(buffer);
    // buffer is copied by push_back
    delete[] buffer;
    StateID id = insert_id_or_pop_state();
    return lookup_state(id);
}

int StateRegistry::get_bins_per_state() const {
    return state_packer.get_num_bins();
}

int StateRegistry::get_state_size_in_bytes() const {
    return get_bins_per_state() * sizeof(PackedStateBin);
}

void StateRegistry::print_statistics() const {
    utils::g_log << "Number of registered states: " << size() << endl;
    registered_states.print_statistics();
}
