#ifndef SYMMETRIES_PERMUTATION_H
#define SYMMETRIES_PERMUTATION_H

#include <vector>

class GlobalState;

class Permutation{
public:
    Permutation();
    explicit Permutation(const unsigned int *full_perm);
    Permutation(const Permutation &perm, bool invert=false);
    Permutation(const Permutation &perm1, const Permutation &perm2);
    ~Permutation();

    Permutation& operator =(const Permutation&);
    //bool operator ==(const Permutation&) const;

    bool identity() const;
    // Michael: version for the trace_back
//    void permute_state(const int* from_state, int* to_state) const;
//    void permute_state(const GlobalState &from_state,
//                       std::vector<int> &to_state) const;
    void print_cycle_notation() const;
    int get_value(int ind) const {
        return value[ind];
    }
//    int get_inverse_value(int ind) const { return inverse_value[ind]; }
    void dump_var_vals() const;
    void dump() const;
    void dump_fdr() const;

    int get_order() const {
        return order;
    }

    static int length;
    static std::vector<int> var_by_val;
    static std::vector<int> dom_sum_by_var;
    static int num_vars;

    static int get_var_by_index(int val);
    static std::pair<int, int> get_var_val_by_index(const int ind);
    static int get_index_by_var_val_pair(const int var, const int val);

    std::pair<int, int> get_new_var_val_by_old_var_val(const int var, const int val) const;
//    std::pair<int, int> get_old_var_val_by_new_var_val(const int var, const int val) const;

    bool replace_if_less(int*) const;
//    bool is_stabilizing_state(const GlobalState& state) const;

private:
    int* value;
//    int* inverse_value;
    std::vector<int> vars_affected;
    std::vector<bool> affected;
    bool borrowed_buffer;
    // Need to keep the connection between affected vars, ie which var goes into which.
    std::vector<int> from_vars;
//    int* buff_for_state_copy;
    // Affected vars by cycles
    std::vector<std::vector<int> > affected_vars_cycles;
    int order;

    void set_value(int ind, int val);
    void set_affected(int ind, int val);

    void finalize();
    void _allocate();
    void _deallocate();
    void _copy_value_from_permutation(const Permutation &perm);
    void _inverse_value_from_permutation(const Permutation &perm);
};

#endif
