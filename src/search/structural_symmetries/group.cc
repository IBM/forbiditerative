#include "group.h"

#include "graph_creator.h"
#include "permutation.h"

#include "../global_state.h"
#include "../option_parser.h"
#include "../per_state_information.h"
#include "../plugin.h"
#include "../state_registry.h"
#include "../task_proxy.h"
#include "../utils/memory.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <queue>


using namespace std;
using namespace utils;

Group::Group(const options::Options &opts)
    : stabilize_initial_state(opts.get<bool>("stabilize_initial_state")),
      time_bound(opts.get<int>("time_bound")),
      dump_symmetry_graph(opts.get<bool>("dump_symmetry_graph")),
      search_symmetries(SearchSymmetries(opts.get_enum("search_symmetries"))),
      dump_permutations(opts.get<bool>("dump_permutations")),
      num_vars(0),
      permutation_length(0),
      num_identity_generators(0),
      initialized(false) {
}

const Permutation &Group::get_permutation(int index) const {
    return generators[index];
}

void Group::add_to_dom_sum_by_var(int summed_dom) {
    dom_sum_by_var.push_back(summed_dom);
}

void Group::add_to_var_by_val(int var) {
    var_by_val.push_back(var);
}

void Group::compute_symmetries(const TaskProxy &task_proxy) {
    if (initialized || !generators.empty()) {
        cerr << "Already computed symmetries" << endl;
        exit_with(ExitCode::CRITICAL_ERROR);
    }
    GraphCreator graph_creator;
    bool success = graph_creator.compute_symmetries(
        task_proxy, stabilize_initial_state, time_bound, dump_symmetry_graph, this);
    if (!success) {
        generators.clear();
    }
    // Set initialized to true regardless of whether symmetries have been
    // found or not to avoid future attempts at computing symmetries if
    // none can be found.
    initialized = true;
}

void Group::add_raw_generator(const unsigned int *generator) {
    Permutation permutation(*this, generator);
    if (permutation.identity()) {
        ++num_identity_generators;
    } else {
        generators.push_back(move(permutation));
    }
}

int Group::get_num_generators() const {
    return generators.size();
}

void Group::dump_generators() const {
    if (get_num_generators() == 0)
        return;

    for (int i = 0; i < get_num_generators(); i++) {
        get_permutation(i).print_affected_variables_by_cycles();
    }

    for (int i = 0; i < get_num_generators(); i++) {
        cout << "Generator " << i << endl;
        get_permutation(i).print_cycle_notation();
        get_permutation(i).dump_var_vals();
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
            const std::vector<int>& affected = get_permutation(i).get_affected_vars();
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
    cout << "Number of identity generators (on states, not on operators): "
         << get_num_identity_generators() << endl;
    cout << "Order of generators: [";
    for (int gen_no = 0; gen_no < num_gen; ++gen_no) {
        cout << get_permutation(gen_no).get_order();
        if (gen_no != num_gen - 1)
            cout << ", ";
    }
    cout << "]" << endl;

    if (dump_permutations) {
        dump_generators();
        dump_variables_equivalence_classes();
    }

}

vector<int> Group::get_canonical_representative(const GlobalState &state) const {
    assert(has_symmetries());
    vector<int> canonical_state(g_variable_domain.size());
    for (size_t i = 0; i < g_variable_domain.size(); ++i) {
        canonical_state[i] = state[i];
    }

    bool changed = true;
    while (changed) {
        changed = false;
        for (int i=0; i < get_num_generators(); i++) {
            if (generators[i].replace_if_less(canonical_state)) {
                changed =  true;
            }
        }
    }
    return canonical_state;
}

vector<int> Group::compute_permutation_trace_to_canonical_representative(const GlobalState &state) const {
    // TODO: duplicate code with get_canonical_representative
    assert(has_symmetries());
    vector<int> canonical_state(g_variable_domain.size());
    for(size_t i = 0; i < g_variable_domain.size(); ++i) {
        canonical_state[i] = state[i];
    }

    vector<int> permutation_trace;
    bool changed = true;
    while (changed) {
        changed = false;
        for (int i=0; i < get_num_generators(); i++) {
            if (generators[i].replace_if_less(canonical_state)) {
                permutation_trace.push_back(i);
                changed = true;
            }
        }
    }
    return permutation_trace;
}

RawPermutation Group::compute_permutation_from_trace(const vector<int> &permutation_trace) const {
    assert(has_symmetries());
    RawPermutation new_perm = new_identity_raw_permutation();
    for (int permutation_index : permutation_trace) {
        const Permutation &permutation = generators[permutation_index];
        RawPermutation temp_perm(permutation_length);
        for (int i = 0; i < permutation_length; i++) {
           temp_perm[i] = permutation.get_value(new_perm[i]);
        }
        new_perm.swap(temp_perm);
    }
    return new_perm;
}

RawPermutation Group::compute_inverse_permutation(const RawPermutation &permutation) const {
    RawPermutation result(permutation_length);
    for (int i = 0; i < permutation_length; ++i) {
        result[permutation[i]] = i;
    }
    return result;
}

RawPermutation Group::new_identity_raw_permutation() const {
    RawPermutation result(permutation_length);
    iota(result.begin(), result.end(), 0);
    return result;
}

RawPermutation Group::compose_permutations(
    const RawPermutation &permutation1, const RawPermutation & permutation2) const {
    RawPermutation result(permutation_length);
    for (int i = 0; i < permutation_length; i++) {
       result[i] = permutation2[permutation1[i]];
    }
    return result;
}

RawPermutation Group::create_permutation_from_state_to_state(
        const GlobalState& from_state, const GlobalState& to_state) const {
    assert(has_symmetries());
    vector<int> from_state_permutation_trace = compute_permutation_trace_to_canonical_representative(from_state);
    vector<int> to_state_permutation_trace = compute_permutation_trace_to_canonical_representative(to_state);

    RawPermutation canonical_to_to_state_permutation = compute_inverse_permutation(compute_permutation_from_trace(to_state_permutation_trace));
    RawPermutation from_state_to_canonical_permutation = compute_permutation_from_trace(from_state_permutation_trace);
    return compose_permutations(from_state_to_canonical_permutation, canonical_to_to_state_permutation);
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


static shared_ptr<Group> _parse(OptionParser &parser) {
    // General Bliss options and options for GraphCreator
    parser.add_option<int>("time_bound",
                           "Stopping after the Bliss software reached the time bound",
                           "0");
    parser.add_option<bool>("stabilize_initial_state",
                            "Compute symmetries stabilizing the initial state",
                            "false");
    parser.add_option<bool>("dump_symmetry_graph",
                           "Dump symmetry graph in dot format",
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

    parser.add_option<bool>("dump_permutations",
                           "Dump the generators",
                           "false");

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
