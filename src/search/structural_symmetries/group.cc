#include "group.h"

#include "graph_creator.h"
#include "permutation.h"
#include "operator_permutation.h"

#include "../option_parser.h"
#include "../per_state_information.h"
#include "../plugin.h"
#include "../state_registry.h"
#include "../task_proxy.h"
#include "../utils/memory.h"
#include "../tasks/root_task.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <queue>


using namespace std;
using namespace utils;

Group::Group(const options::Options &opts)
    : stabilize_initial_state(opts.get<bool>("stabilize_initial_state")),
      stabilize_goal(opts.get<bool>("stabilize_goal")),
      use_color_for_stabilizing_goal(opts.get<bool>("use_color_for_stabilizing_goal")),
      time_bound(opts.get<int>("time_bound")),
      dump_symmetry_graph(opts.get<bool>("dump_symmetry_graph")),
      search_symmetries(opts.get<SearchSymmetries>("search_symmetries")),
      dump_permutations(opts.get<bool>("dump_permutations")),
      write_search_generators(opts.get<bool>("write_search_generators")),
      write_all_generators(opts.get<bool>("write_all_generators")),
      keep_operator_symmetries(opts.get<bool>("keep_operator_symmetries")),
      num_vars(0),
      num_operators(0),
      permutation_length(0),
      graph_size(0),
      num_identity_generators(0),
      initialized(false) {
}

const Permutation &Group::get_permutation(int index) const {
    return generators[index];
}

const OperatorPermutation &Group::get_operator_permutation(int index) const { 
    return operator_generators[index]; 
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
        exit_with(ExitCode::SEARCH_CRITICAL_ERROR);
    }
    GraphCreator graph_creator;
    bool success = graph_creator.compute_symmetries(
        task_proxy,
        stabilize_initial_state,
        stabilize_goal,
        use_color_for_stabilizing_goal,
        time_bound,
        dump_symmetry_graph,
        this);
    if (!success) {
        generators.clear();
    }
    // Set initialized to true regardless of whether symmetries have been
    // found or not to avoid future attempts at computing symmetries if
    // none can be found.
    initialized = true;

    if (write_search_generators || write_all_generators) {
        write_generators();
        utils::exit_with(utils::ExitCode::SUCCESS);
    }
}

void Group::write_generators() const {
    assert(write_search_generators || write_all_generators);

    /*
      To avoid writing large generators, we first compute the set of vertices
      that is actually affected by any generator and assign them consecutive
      numbers.
    */
    unordered_map<int, int> vertex_to_id;
    int vertex_counter = 0;
    for (const auto &generator : to_be_written_generators) {
        for (const pair<const int, int> &key_val : generator) {
            if (!vertex_to_id.count(key_val.first)) {
                vertex_to_id[key_val.first] = vertex_counter++;
            }
        }
    }

    /*
      Then we go over all generators again, writing them out as permutations
      (python-style lists) using the vertex-to-id mapping.
    */
    ofstream file;
    file.open("generators.py", std::ios_base::out);
    for (const auto &generator : to_be_written_generators) {
        vector<int> permutation(vertex_counter);
        iota(permutation.begin(), permutation.end(), 0);
        for (const pair<const int, int> &key_val : generator) {
            permutation[vertex_to_id[key_val.first]] = vertex_to_id[key_val.second];
        }
        file << "[";
        for (size_t i = 0; i < permutation.size(); ++i) {
            file << permutation[i];
            if (i != permutation.size() - 1) {
                file << ", ";
            }
        }
        file << "]" << endl;
    }
    file.close();
}

void Group::add_to_be_written_generator(const unsigned int *generator) {
    assert(write_search_generators || write_all_generators);
    int length = (write_search_generators ? permutation_length : graph_size);
    unordered_map<int, int> gen_map;
    for (int from = 0; from < length; ++from) {
        int to = generator[from];
        if (from != to) {
            gen_map[from] = to;
        }
    }
    to_be_written_generators.push_back(move(gen_map));
}

void Group::add_raw_generator(const unsigned int *generator) {
    Permutation permutation(*this, generator);
    if (permutation.identity()) {
        ++num_identity_generators;
        if (write_all_generators) {
            add_to_be_written_generator(generator);
        }
    } else {
        if (write_search_generators || write_all_generators) {
            add_to_be_written_generator(generator);
        }
        generators.push_back(move(permutation));
        if (keep_operator_symmetries) {
            OperatorPermutation op_permutation(*this, generator);
            operator_generators.push_back(move(op_permutation));
        }
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
        utils::g_log << "Generator " << i << endl;
        get_permutation(i).print_cycle_notation();
        get_permutation(i).dump_var_vals();
    }

    int num_vars = tasks::g_root_task->get_num_variables();
    utils::g_log << "Extra group info:" << endl;
    utils::g_log << "Number of identity on states generators: " << num_identity_generators << endl;
    utils::g_log << "Permutation length: " << get_permutation_length() << endl;
    utils::g_log << "Permutation variables by values (" << num_vars << "): " << endl;
    for (int i = num_vars; i < get_permutation_length(); i++)
        utils::g_log << get_var_by_index(i) << "  " ;
    utils::g_log << endl;
}

void Group::dump_variables_equivalence_classes() const {
    if (get_num_generators() == 0)
        return;

    int num_vars = tasks::g_root_task->get_num_variables();

    vector<int> vars_mapping;
    for (int i=0; i < num_vars; ++i)
        vars_mapping.push_back(i);

    bool change = true;
    while (change) {
        change = false;
        for (int i = 0; i < get_num_generators(); i++) {
            const std::vector<int>& affected = get_permutation(i).get_affected_vars();
            int min_ind = num_vars;
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
    utils::g_log << "Equivalence relation:" << endl;
    for (int i=0; i < num_vars; ++i) {
        vector<int> eqiv_class;
        for (int j=0; j < num_vars; ++j)
            if (vars_mapping[j] == i)
                eqiv_class.push_back(j);
        if (eqiv_class.size() <= 1)
            continue;
        utils::g_log << "[";
        for (int var : eqiv_class)
            utils::g_log << " " << tasks::g_root_task->get_fact_name(FactPair(var, 0));
        utils::g_log << " ]" << endl;
    }
}

void Group::statistics() const {
    utils::g_log << "Size of the grounded symmetry graph: "
         << graph_size << endl;
    utils::g_log << "Number of search generators (affecting facts): "
         << get_num_generators() << endl;
    utils::g_log << "Number of identity generators (on facts, not on operators): "
         << get_num_identity_generators() << endl;
    utils::g_log << "Total number of generators: "
         << get_num_generators() + get_num_identity_generators() << endl;

    if (dump_permutations) {
        dump_generators();
        dump_variables_equivalence_classes();
    }
}

vector<int> Group::get_canonical_representative(const State &state) const {
    assert(has_symmetries());
    state.unpack();
    vector<int> canonical_state = state.get_unpacked_values();

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

vector<int> Group::compute_permutation_trace_to_canonical_representative(const State &state) const {
    // TODO: duplicate code with get_canonical_representative
    assert(has_symmetries());
    state.unpack();
    vector<int> canonical_state = state.get_unpacked_values();

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
        const State& from_state, const State& to_state) const {
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
        utils::g_log << "=====> WARNING!!!! Check that this is done on purpose!" << endl;
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
    parser.add_option<bool>("stabilize_goal",
                            "Compute symmetries stabilizing the goal",
                            "true");
    parser.add_option<bool>("use_color_for_stabilizing_goal",
                            "Use a color to stabilize the goal instead of "
                            "using an additional node linked to goal values.",
                            "true");
    parser.add_option<bool>("dump_symmetry_graph",
                           "Dump symmetry graph in dot format",
                           "false");
    parser.add_option<bool>("keep_operator_symmetries",
                            "Compute and keep operator symmetries",
                            "false");

    // Type of search symmetries to be used
    vector<string> search_symmetries;
    search_symmetries.push_back("NONE");
    search_symmetries.push_back("OSS");
    search_symmetries.push_back("DKS");
    parser.add_enum_option<SearchSymmetries>("search_symmetries",
                           search_symmetries,
                           "Choose the type of structural symmetries that "
                           "should be used for pruning: OSS for orbit space "
                           "search or DKS for storing the canonical "
                           "representative of every state during search",
                           "NONE");

    parser.add_option<bool>("dump_permutations",
                           "Dump the generators",
                           "false");
    parser.add_option<bool>(
        "write_search_generators",
        "Write symmetry group generators that affect variables to a file and "
        "stop afterwards.",
        "false");
    parser.add_option<bool>(
        "write_all_generators",
        "Write all symmetry group generators to a file, including those that "
        "do not affect variables, and stop afterwards.",
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
    "",
    "symmetries");

static Plugin<Group> _plugin("structural_symmetries", _parse);
