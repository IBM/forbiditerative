#include "group.h"

#include "graph_creator.h"
#include "permutation.h"

#include "../global_state.h"
#include "../option_parser.h"
#include "../per_state_information.h"
#include "../plugin.h"
#include "../state_registry.h"

#include "../utils/rng.h"
#include "../utils/rng_options.h"

#include <algorithm>
#include <iostream>
#include <queue>


using namespace std;
using namespace utils;

Group::Group(const options::Options &opts)
    : stabilize_initial_state(opts.get<bool>("stabilize_initial_state")),
      search_symmetries(SearchSymmetries(opts.get_enum("search_symmetries"))),
      symmetrical_lookups(SymmetricalLookups(opts.get_enum("symmetrical_lookups"))),
      rw_length_or_number_symmetric_states(opts.get<int>("symmetry_rw_length_or_number_states")),
      rng(utils::parse_rng_from_options(opts)),
      initialized(false) {
    graph_creator = new GraphCreator(opts);
}

Group::~Group() {
    delete_generators();
    delete graph_creator;
}

void Group::delete_generators() {
    for (size_t i = 0; i < generators.size(); ++i) {
        delete generators[i];
    }
    generators.clear();
}

const Permutation *Group::get_permutation(int index) const {
    return generators[index];
}

void Group::compute_symmetries() {
    assert(!initialized);
    initialized = true;
    if (!generators.empty() || !graph_creator) {
        cerr << "Already computed symmetries" << endl;
        exit_with(ExitCode::CRITICAL_ERROR);
    }
    if (!graph_creator->compute_symmetries(this)) {
        // Computing symmetries ran out of memory
        delete_generators();
    }
    delete graph_creator;
    graph_creator = 0;
}

/**
 * Add new permutation to the list of permutations
 * The function will be called from bliss
 */
void Group::add_permutation(void* param, unsigned int, const unsigned int * full_perm){
    Permutation *perm = new Permutation(full_perm);
    if (!perm->identity()){
        ((Group*) param)->add_generator(perm);
    } else {
        delete perm;
    }
}

void Group::add_generator(const Permutation *gen) {
    generators.push_back(gen);
}

int Group::get_num_generators() const {
    return generators.size();
}

void Group::dump_generators() const {
    if (get_num_generators() == 0)
        return;
    for (int i = 0; i < get_num_generators(); i++) {
        cout << "Generator " << i << endl;
        get_permutation(i)->print_cycle_notation();
        get_permutation(i)->dump_var_vals();
    }

    cout << "Extra group info:" << endl;
    cout << "Permutation length: " << Permutation::length << endl;
    cout << "Permutation variables by values (" << g_variable_domain.size() << "): " << endl;
    for (int i = g_variable_domain.size(); i < Permutation::length; i++)
        cout << Permutation::get_var_by_index(i) << "  " ;
    cout << endl;
}

void Group::statistics() const {
    int num_gen = get_num_generators();
    cout << "Number of generators: " << num_gen << endl;
    cout << "Order of generators: [";
    for (int gen_no = 0; gen_no < num_gen; ++gen_no) {
        cout << get_permutation(gen_no)->get_order();
        if (gen_no != num_gen - 1)
            cout << ", ";
    }
    cout << "]" << endl;
}


// ===============================================================================
// Methods related to symmetric lookups

void Group::compute_random_symmetric_state(const GlobalState &state,
                                           StateRegistry &symmetric_states_registry,
                                           std::vector<GlobalState> &states) const {
    GlobalState state_copy = symmetric_states_registry.copy_and_register_state(state);
    // Perform random walk of length random_walk_length to compute a single random symmetric state.
    GlobalState current_state = state_copy;
    for (int i = 0; i < rw_length_or_number_symmetric_states; ++i) {
        int gen_no = (*rng)(get_num_generators());
        current_state = symmetric_states_registry.permute_state(current_state, get_permutation(gen_no));
    }
    // Only take the resulting state if it differs from the given one
    if (current_state.get_id() != state_copy.get_id()) {
        states.push_back(current_state);
    }
}

void Group::compute_subset_all_symmetric_states(const GlobalState &state,
                                                StateRegistry &symmetric_states_registry,
                                                vector<GlobalState> &states) const {
    // TODO: improve efficiency by disallowing a permutation to be applied
    // sequently more than its order times?
//    cout << "original  state: ";
//    for (size_t i = 0; i < g_variable_domain.size(); ++i) {
//        cout << i << "=" << state[i] << ",";
//    }
//    cout << endl;

    /*
      Systematically generate symmetric states until the whished number is
      reached (rw_length_or_number_symmetric_states) or until all states
      have been generated.
    */
    GlobalState state_copy = symmetric_states_registry.copy_and_register_state(state);
    queue<StateID> open_list;
    open_list.push(state_copy.get_id());
    int num_gen = get_num_generators();
    PerStateInformation<bool> reached(false);
    reached[state_copy] = true;
    while (!open_list.empty()) {
        GlobalState current_state = symmetric_states_registry.lookup_state(open_list.front());
        open_list.pop();
        for (size_t i = 0; i < g_variable_domain.size(); ++i) {
            assert(current_state[i] >= 0 && current_state[i] < g_variable_domain[i]);
        }
        for (int gen_no = 0; gen_no < num_gen; ++gen_no) {
            GlobalState permuted_state =
                    symmetric_states_registry.permute_state(current_state, get_permutation(gen_no));
            for (size_t i = 0; i < g_variable_domain.size(); ++i) {
                assert(permuted_state[i] >= 0 && permuted_state[i] < g_variable_domain[i]);
            }
//            cout << "applying generator " << gen_no << " to state " << current_state.get_id() << endl;
//            cout << "symmetric state: ";
//            for (size_t i = 0; i < g_variable_domain.size(); ++i) {
//                cout << i << "=" << permuted_state[i] << ",";
//            }
//            cout << endl;
            if (!reached[permuted_state]) {
//                cout << "its a new state, with id " << permuted_state.get_id() << endl;
                states.push_back(permuted_state);
                if (static_cast<int>(states.size()) == rw_length_or_number_symmetric_states) {
                    // If number_of_states == -1, this test can never trigger and hence
                    // we compute the set of all symmetric states as desired.
                    return;
                }
                reached[permuted_state] = true;
                open_list.push(permuted_state.get_id());
            }
        }
    }
}

void Group::compute_symmetric_states(const GlobalState &state,
                                     StateRegistry &symmetric_states_registry,
                                     vector<GlobalState> &states) const {
    if (symmetrical_lookups == ONE_STATE) {
        compute_random_symmetric_state(state,
                                       symmetric_states_registry,
                                       states);
    } else if (symmetrical_lookups == SUBSET_OF_STATES || symmetrical_lookups == ALL_STATES) {
        compute_subset_all_symmetric_states(state,
                                            symmetric_states_registry,
                                            states);
    }
}


// ===============================================================================
// Methods related to orbit search symmetries

int *Group::get_canonical_representative(const GlobalState &state) const {
    int *canonical_state = new int[g_variable_domain.size()];
    for (size_t i = 0; i < g_variable_domain.size(); ++i) {
        canonical_state[i] = state[i];
    }

    int size = get_num_generators();
    if (size == 0)
        return canonical_state;

    bool changed = true;
    while (changed) {
        changed = false;
        for (int i=0; i < size; i++) {
            if (generators[i]->replace_if_less(canonical_state)) {
                changed =  true;
            }
        }
    }
    return canonical_state;
}

Permutation *Group::compose_permutation(const Trace& perm_index) const {
    Permutation *new_perm = new Permutation();
    for (size_t i = 0; i < perm_index.size(); ++i) {
        Permutation *tmp = new Permutation(new_perm, get_permutation(perm_index[i]));
        delete new_perm;
        new_perm = tmp;
    }
    return new_perm;
}

void Group::get_trace(const GlobalState &state, Trace& full_trace) const {
    int size = get_num_generators();
    if (size == 0)
        return;

    int *temp_state = new int[g_variable_domain.size()];
    for(size_t i = 0; i < g_variable_domain.size(); ++i)
        temp_state[i] = state[i];
    bool changed = true;
    while (changed) {
        changed = false;
        for (int i=0; i < size; i++) {
            if (generators[i]->replace_if_less(temp_state)) {
                full_trace.push_back(i);
                changed = true;
            }
        }
    }
}

Permutation *Group::create_permutation_from_state_to_state(
        const GlobalState& from_state, const GlobalState& to_state) const {
    Trace new_trace;
    Trace curr_trace;
    get_trace(from_state, curr_trace);
    get_trace(to_state, new_trace);

    Permutation *tmp = compose_permutation(new_trace);
    Permutation *p1 = new Permutation(tmp, true);  //inverse
    delete tmp;
    Permutation *p2 = compose_permutation(curr_trace);
    Permutation *result = new Permutation(p2, p1);
    delete p1;
    delete p2;
    return result;
}

static Group *_parse(OptionParser &parser) {
    // General Bliss options
    parser.add_option<int>("time_bound",
                           "Stopping after the Bliss software reached the time bound",
                           "0");
//    parser.add_option<int>("generators_bound",
//                           "Number of found generators after which Bliss is stopped",
//                           "0");
    parser.add_option<bool>("stabilize_initial_state",
                            "Compute symmetries stabilizing the initial state",
                            "false");

    // Type of search symmetries to be used
    vector<string> search_symmetries;
    search_symmetries.push_back("NOSEARCHSYMMETRIES");
    search_symmetries.push_back("OSS");
    search_symmetries.push_back("DKS");
    parser.add_enum_option("search_symmetries",
                           search_symmetries,
                           "Choose the type of orbit symmetries that should "
                           "be used: OSS for orbit space search or DKS for "
                           "storing the canonical representative of every "
                           "state during search",
                           "NOSEARCHSYMMETRIES");

    // Options for symmetric lookup symmetries
    vector<string> symmetrical_lookups;
    symmetrical_lookups.push_back("NOSYMMETRICLOOKUPS");
    symmetrical_lookups.push_back("ONE_STATE");
    symmetrical_lookups.push_back("SUBSET_OF_STATES");
    symmetrical_lookups.push_back("ALL_STATES");
    parser.add_enum_option("symmetrical_lookups",
                           symmetrical_lookups,
                           "Choose the options for using symmetric lookups, "
                           "i.e. what symmetric states should be computed "
                           "for every heuristic evaluation:\n"
                           "- ONE_STATE: one random state, generated through a random "
                           "walk of length specified via length_or_number option\n"
                           "- SUBSET_OF_STATES: a subset of all symmetric states, "
                           "generated in the same systematic way as generating "
                           "all symmetric states, of size specified via "
                           "length_or_Number option\n"
                           "- ALL_STATES: all symmetric states, generated via BFS in "
                           "the orbit.",
                           "NOSYMMETRICLOOKUPS");
    parser.add_option<int>("symmetry_rw_length_or_number_states",
                           "Choose the length of a random walk if sl_type="
                           "ONE or the number of symmetric states if sl_type="
                           "SUBSET",
                           "5");

    utils::add_rng_options(parser);

    Options opts = parser.parse();

    if (!parser.dry_run()) {
        bool use_search_symmetries = opts.get_enum("search_symmetries");
        bool use_symmetric_lookups = opts.get_enum("symmetrical_lookups");
        if (!use_search_symmetries && !use_symmetric_lookups) {
            cerr << "You have specified a symmetries option which does use "
                    "neither search symmetries nor symmetric lookups!" << endl;
            exit_with(ExitCode::INPUT_ERROR);
        }
//        if (opts.get_enum("search_symmetries") == 2) {
//            cerr << "Not implemented" << endl;
//            exit_with(ExitCode::INPUT_ERROR);
//        }
        return new Group(opts);
    } else {
        return 0;
    }
}

static Plugin<Group> _plugin("structural_symmetries", _parse);
