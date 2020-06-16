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

//namespace structural_symmetries {

Group::Group(const options::Options &opts)
    : stabilize_initial_state(opts.get<bool>("stabilize_initial_state")),
      search_symmetries(SearchSymmetries(opts.get_enum("search_symmetries"))),
      symmetrical_lookups(SymmetricalLookups(opts.get_enum("symmetrical_lookups"))),
      rw_length_or_number_symmetric_states(opts.get<int>("symmetry_rw_length_or_number_states")),
      rng(utils::parse_rng_from_options(opts)),
	  dump(opts.get<bool>("dump")),
	  keep_operator_symmetries(opts.get<bool>("keep_operator_symmetries")),
      initialized(false) {
    graph_creator = new GraphCreator(opts);
    num_identity_generators = 0;
    permutation_length = 0;
    num_vars = 0;
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

const Permutation* Group::get_permutation(int index) const {
    return generators[index];
}

const OperatorPermutation* Group::get_operator_permutation(int index) const {
    return operator_generators[index];
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

    Permutation *perm = new Permutation((Group*) param, full_perm);
    if (!perm->identity()){
        ((Group*) param)->add_generator(perm);
        ((Group*) param)->add_operator_generator(full_perm);
    } else {
    	((Group*) param)->num_identity_generators++;
        delete perm;
    }
}

void Group::add_operator_generator(const unsigned int * full_perm) {
	if (!keep_operator_symmetries)
		return;

	operator_generators.push_back(new OperatorPermutation(full_perm, get_permutation_length()));
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
        get_permutation(i)->print_affected_variables_by_cycles();
    }

    for (int i = 0; i < get_num_generators(); i++) {
        cout << "Generator " << i << endl;
        get_permutation(i)->print_cycle_notation();
        //get_permutation(i)->dump_var_vals();
    }

    cout << "Extra group info:" << endl;
    cout << "Number of identity on states generators: " << num_identity_generators << endl;
    cout << "Permutation length: " << get_permutation_length() << endl;
    cout << "Permutation variables by values (" << g_variable_domain.size() << "): " << endl;
    for (int i = g_variable_domain.size(); i < get_permutation_length(); i++)
        cout << get_var_by_index(i) << "  " ;
    cout << endl;
}

void Group::dump_variables_equivalence_classes() const {
    if (get_num_generators() == 0)
        return;

    vector<int> vars_mapping;
    for (size_t i=0; i < g_variable_domain.size(); ++i)
    	vars_mapping.push_back(i);

    bool change = true;
    while (change) {
    	change = false;
    	for (int i = 0; i < get_num_generators(); i++) {
    		const std::vector<int>& affected = get_permutation(i)->get_affected_vars();
    		int min_ind = g_variable_domain.size();
    		for (int var : affected) {
    			if (min_ind > vars_mapping[var])
    				min_ind = vars_mapping[var];
    		}
    		for (int var : affected) {
    			if (vars_mapping[var] > min_ind)
    				change = true;
    			vars_mapping[var] = min_ind;
    		}
    	}
    }
    cout << "Equivalence relation:" << endl;
    int num_vars = g_variable_domain.size();
    for (int i=0; i < num_vars; ++i) {
    	vector<int> eqiv_class;
        for (size_t j=0; j < g_variable_domain.size(); ++j)
        	if (vars_mapping[j] == i)
        		eqiv_class.push_back(j);
        if (eqiv_class.size() <= 1)
        	continue;
        cout << "[";
        for (int var : eqiv_class)
        	cout << " " << g_fact_names[var][0];
        cout << " ]" << endl;
    }
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

    if (dump) {
    	dump_generators();
    	dump_variables_equivalence_classes();
    }

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
    if (symmetrical_lookups == SymmetricalLookups::ONE_STATE) {
        compute_random_symmetric_state(state,
                                       symmetric_states_registry,
                                       states);
    } else if (symmetrical_lookups == SymmetricalLookups::SUBSET_OF_STATES ||
               symmetrical_lookups == SymmetricalLookups::ALL_STATES) {
        compute_subset_all_symmetric_states(state,
                                            symmetric_states_registry,
                                            states);
    }
}

// ===============================================================================
// Methods related to OSS

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
    Permutation *new_perm = new Permutation(this);
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
    Permutation *p1 = new Permutation(*tmp, true);  //inverse
    delete tmp;
    Permutation *p2 = compose_permutation(curr_trace);
    Permutation *result = new Permutation(p2, p1);
    delete p1;
    delete p2;
    return result;
}

int Group::get_var_by_index(int ind) const {
    // In case of ind < num_vars, returns the index itself, as this is the variable part of the permutation.
    if (ind < num_vars) {
		cout << "=====> WARNING!!!! Check that this is done on purpose!" << endl;
		return ind;
	}
    return var_by_val[ind-num_vars];
}

std::pair<int, int> Group::get_var_val_by_index(const int ind) const {
    assert(ind>=num_vars);
    int var =  var_by_val[ind-num_vars];
    int val = ind - dom_sum_by_var[var];

    return make_pair(var, val);
}

int Group::get_index_by_var_val_pair(const int var, const int val) const {
	return dom_sum_by_var[var] + val;
}

Permutation*  Group::new_identity_permutation() const {
	return new Permutation(this);
}


static shared_ptr<Group> _parse(OptionParser &parser) {
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
    search_symmetries.push_back("NONE");
    search_symmetries.push_back("OSS");
    search_symmetries.push_back("DKS");
    parser.add_enum_option("search_symmetries",
                           search_symmetries,
                           "Choose the type of structural symmetries that "
                           "should be used for pruning: OSS for orbit space "
                           "search or DKS for storing the canonical "
                           "representative of every state during search",
                           "NONE");

    // Options for symmetric lookup symmetries
    vector<string> symmetrical_lookups;
    symmetrical_lookups.push_back("NONE");
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
                           "NONE");
    parser.add_option<int>("symmetry_rw_length_or_number_states",
                           "Choose the length of a random walk if sl_type="
                           "ONE or the number of symmetric states if sl_type="
                           "SUBSET",
                           "5");

    parser.add_option<bool>("dump",
                           "Dump the generators",
                           "false");

    parser.add_option<bool>("keep_operator_symmetries",
                           "Keep the operator symmetries from generators",
                           "false");

    utils::add_rng_options(parser);

    Options opts = parser.parse();

    if (parser.dry_run()) {
        return nullptr;
    } else {
        return make_shared<Group>(opts);
    }
}

static PluginTypePlugin<Group> _type_plugin(
    "Group",
    // TODO: Replace empty string by synopsis for the wiki page.
    "");

static PluginShared<Group> _plugin("structural_symmetries", _parse);
//}
