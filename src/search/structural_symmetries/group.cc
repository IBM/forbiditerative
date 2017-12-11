#include "group.h"

#include "graph_creator.h"
#include "permutation.h"

#include "../global_state.h"
#include "../option_parser.h"
#include "../per_state_information.h"
#include "../plugin.h"
#include "../state_registry.h"

#include <algorithm>
#include <iostream>
#include <queue>


using namespace std;
using namespace utils;

Group::Group(const options::Options &opts)
    : stabilize_initial_state(opts.get<bool>("stabilize_initial_state")),
      search_symmetries(SearchSymmetries(opts.get_enum("search_symmetries"))),
      dump_permutations(opts.get<bool>("dump_permutations")),
      num_vars(0),
      permutation_length(0),
      num_identity_generators(0),
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

const Permutation &Group::get_permutation(int index) const {
    return *generators[index];
}

void Group::add_to_dom_sum_by_var(int summed_dom) {
    dom_sum_by_var.push_back(summed_dom);
}

void Group::add_to_var_by_val(int var) {
    var_by_val.push_back(var);
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

void Group::add_generator(const Permutation *gen) {
    generators.push_back(gen);
}

void Group::increase_identity_generator_count() {
    ++num_identity_generators;
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
        Permutation *tmp = new Permutation(*new_perm, get_permutation(perm_index[i]));
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
    Permutation *result = new Permutation(*p2, *p1);
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
