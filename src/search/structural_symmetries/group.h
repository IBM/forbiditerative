#ifndef STRUCTURAL_SYMMETRIES_GROUP_H
#define STRUCTURAL_SYMMETRIES_GROUP_H

#include <memory>
#include <vector>

class GlobalState;
class Permutation;
class StateRegistry;
class TaskProxy;

namespace options {
class Options;
}

enum class SearchSymmetries {
    NONE,
    OSS,
    DKS
};

// Permutation of bliss graph vertices.
using RawPermutation = std::vector<int>;

class Group {
    // Options for Bliss and the type of symmetries used
    const bool stabilize_initial_state;
    const int time_bound;
    const bool dump_symmetry_graph;
    const SearchSymmetries search_symmetries;
    const bool dump_permutations;

    // Group properties
    int num_vars;
    int permutation_length;
    std::vector<int> dom_sum_by_var;
    std::vector<int> var_by_val;
    int num_identity_generators;

    // Group creation
    bool initialized;
    std::vector<Permutation> generators;
    const Permutation &get_permutation(int index) const;

    // Path tracing
    std::vector<int> compute_permutation_trace_to_canonical_representative(const GlobalState& state) const;
    RawPermutation compute_permutation_from_trace(const std::vector<int> &permutation_trace) const;
    RawPermutation compute_inverse_permutation(const RawPermutation &permutation) const;
public:
    explicit Group(const options::Options &opts);
    ~Group() = default;

    // Graph creator
    void add_to_dom_sum_by_var(int summed_dom);
    void add_to_var_by_val(int var);

    // Methods for creating the group
    void compute_symmetries(const TaskProxy &task_proxy);
    void add_raw_generator(const unsigned int *generator);
    void set_permutation_num_variables(int nvars) {
        num_vars = nvars;
    }
    int get_permutation_num_variables() const {
        return num_vars;
    }
    void set_permutation_length(int length) {
        permutation_length = length;
    }
    int get_permutation_length() const {
        return permutation_length;
    }
    int get_var_by_index(int val) const;
    std::pair<int, int> get_var_val_by_index(const int ind) const;
    int get_index_by_var_val_pair(const int var, const int val) const;

    // Using the group
    int get_num_generators() const;
    int get_num_identity_generators() const {
        return num_identity_generators;
    }
    void dump_generators() const;
    void dump_variables_equivalence_classes() const;
    void statistics() const;
    bool is_stabilizing_initial_state() const {
        return stabilize_initial_state;
    }
    bool is_initialized() const {
        return initialized;
    }
    bool has_symmetries() const {
        return !generators.empty();
    }
    SearchSymmetries get_search_symmetries() const {
        return search_symmetries;
    }

    // Used for OSS
    std::vector<int> get_canonical_representative(const GlobalState &state) const;
    // Following methods: used for path tracing (OSS and DKS)
    RawPermutation new_identity_raw_permutation() const;
    RawPermutation compose_permutations(
        const RawPermutation &permutation1, const RawPermutation &permutation2) const;
    RawPermutation create_permutation_from_state_to_state(
        const GlobalState &from_state, const GlobalState &to_state) const;
};

#endif
