#include "permutation.h"

#include "../tasks/root_task.h"
#include "../utils/logging.h"

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <fstream>
#include <vector>
#include <numeric>


using namespace std;

void Permutation::_allocate() {
    value.resize(group.get_permutation_length());
}

void Permutation::_copy_value_from_permutation(const Permutation &perm) {
    for (int i = 0; i < group.get_permutation_length(); i++) {
        value[i] = perm.get_value(i);
    }
}

void Permutation::_inverse_value_from_permutation(const Permutation &perm) {
    for (int i = 0; i < group.get_permutation_length(); i++) {
        value[perm.get_value(i)] = i;
    }
}

Permutation::Permutation(const Group &_group) : group(_group) {
    _allocate();
    iota(value.begin(), value.end(), 0);
    finalize();
}

Permutation::Permutation(const Group &_group, const unsigned int* full_permutation)
    : group(_group) {
    _allocate();
    for (int i = 0; i < group.get_permutation_length(); i++) {
        value[i] = full_permutation[i];
    }
    finalize();
}

Permutation::Permutation(const Group &_group, const vector<int> &full_permutation)
    : group(_group), value(full_permutation) {
    finalize();
}

Permutation::Permutation(const Permutation &perm, bool invert)
    : group(perm.group) {
    _allocate();
    if (invert) {
        _inverse_value_from_permutation(perm);
    } else {
        // TODO: we could use a real, default copy constructor here.
        _copy_value_from_permutation(perm);
    }
    finalize();
}

Permutation::Permutation(const Permutation &perm1, const Permutation &perm2)
    : group(perm1.group) {
    _allocate();
    for (int i = 0; i < group.get_permutation_length(); i++) {
        value[i] = perm2.get_value(perm1.get_value(i));
    }
    finalize();
}

void Permutation::finalize(){
    int num_vars = group.get_permutation_num_variables();
    // Compute and sort affected variables, set from_vars.
    from_vars.assign(num_vars, -1);
    vector<bool> affected(num_vars, false);
    for (int ind = num_vars; ind < group.get_permutation_length(); ++ind) {
        int val = value[ind];
        if (ind == val) {
            continue;
        }

        int var = group.get_var_by_index(ind);
        int to_var = group.get_var_by_index(val);

        if (!affected[var]) {
            vars_affected.push_back(var);
            affected[var] = true;
        }
        if (!affected[to_var]) {
            vars_affected.push_back(to_var);
            affected[to_var] = true;
        }
        // Keeping the orig. var for each var.
        from_vars[to_var] = var;
    }
    sort(vars_affected.begin(), vars_affected.end());

    // Going over the vector from_vars of the mappings of the variables and
    // finding cycles.
    vector<bool> marked;
    marked.assign(group.get_permutation_length(), false);
    for (int var = 0; var < num_vars; var++) {
        if (marked[var] || from_vars[var] == -1)
            continue;

        int current = var;
        marked[current] = true;
        vector<int> cycle;
        cycle.push_back(current);

        while (from_vars[current] != var){
            current = from_vars[current];
            marked[current] = true;
            cycle.insert(cycle.begin(),current);
        }
        // Get here when from_vars[current] == i.
        affected_vars_cycles.push_back(cycle);
    }
}

bool Permutation::identity() const{
    return vars_affected.size() == 0;
}

void Permutation::print_cycle_notation() const {
    vector<int> done;
    for (int i = group.get_permutation_num_variables(); i < group.get_permutation_length(); i++){
        if (find(done.begin(), done.end(), i) == done.end()){
            int current = i;
            if(get_value(i) == i) continue; //don't print cycles of size 1

            pair<int, int> varval = group.get_var_val_by_index(i);
            utils::g_log<<"("<< tasks::g_root_task->get_fact_name(FactPair(varval.first, varval.second))  <<" ";

            while(get_value(current) != i){
                done.push_back(current);
                current = get_value(current);

                pair<int, int> currvarval = group.get_var_val_by_index(current);
                utils::g_log<< tasks::g_root_task->get_fact_name(FactPair(currvarval.first, currvarval.second)) <<" ";
            }
            done.push_back(current);
            utils::g_log<<") ";
        }
    }
    utils::g_log << endl << "Variables:  ";
    for(size_t i = 0; i < vars_affected.size(); i++) utils::g_log << vars_affected[i] << "  ";
    utils::g_log << endl << "Variables permuted:  ";

    for(size_t i = 0; i < vars_affected.size(); i++) utils::g_log << from_vars[vars_affected[i]] << " -> " << vars_affected[i] << "  ";
    utils::g_log << endl;

    utils::g_log << "Affected variables by cycles: " << endl;
    print_affected_variables_by_cycles();
}

void Permutation::print_affected_variables_by_cycles() const {
    for (size_t i=0; i < affected_vars_cycles.size(); i++) {
        utils::g_log << "( " ;
        for (size_t j=0; j < affected_vars_cycles[i].size(); j++) {
            utils::g_log << affected_vars_cycles[i][j] << " ";
        }
        utils::g_log << ")  ";
    }
    utils::g_log << endl;
}

void Permutation::dump_var_vals() const {
    for (int i = 0; i < group.get_permutation_num_variables(); ++i) {
        for (int j = 0; j < tasks::g_root_task->get_variable_domain_size(i); ++j) {
            pair<int, int> var_val = get_new_var_val_by_old_var_val(i, j);
            utils::g_log << i << "=" << j << "->"
                 << var_val.first << "=" << var_val.second << ",";
        }
        utils::g_log << endl;
    }
//    utils::g_log << endl;
}

void Permutation::dump() const {
    for(int i = 0; i < group.get_permutation_length(); i++){
        if (get_value(i) != i)
            utils::g_log << setw(4) << i;
    }
    utils::g_log << endl;
    for(int i = 0; i < group.get_permutation_length(); i++){
        if (get_value(i) != i)
            utils::g_log << setw(4) << get_value(i);
    }
    utils::g_log << endl;
}

void Permutation::dump_fdr() const {
    for(int i = group.get_permutation_num_variables(); i < group.get_permutation_length(); i++){
        if (get_value(i) != i) {
            pair<int, int> varval = group.get_var_val_by_index(i);
            utils::g_log << setw(10) <<  "[" << tasks::g_root_task->get_variable_name(varval.first) << "] -> "
                     << static_cast<int>(varval.second);
        }
    }
    utils::g_log << endl;
    for(int i = group.get_permutation_num_variables(); i < group.get_permutation_length(); i++){
        if (get_value(i) != i) {
            pair<int, int> varval = group.get_var_val_by_index(get_value(i));
            utils::g_log << setw(10) <<  "[" << tasks::g_root_task->get_variable_name(varval.first) << "] -> "
                     << static_cast<int>(varval.second);
        }
    }
    utils::g_log << endl;
}

std::pair<int, int> Permutation::get_new_var_val_by_old_var_val(const int var, const int val) const {
    int old_ind = group.get_index_by_var_val_pair(var, val);
    int new_ind = get_value(old_ind);
    return group.get_var_val_by_index(new_ind);
}

//////////////////////////////////////////////////////////////////////////////////////////
// This method compares the state to the state resulting from permuting it.
// If the original state is bigger than the resulted one, it is rewritten with the latter and true is returned.
////////////////////  New version - no extra buffer is needed, faster copy ///////////////
bool Permutation::replace_if_less(vector<int> &state) const {
    if (identity())
        return false;

    int from_here = vars_affected.size(); // Will be set to value below vars_affected.size() if there is a need to overwrite the state,
    // starting from that index in the vars_affected vector.

    // Going over the affected variables, comparing the resulted values with the state values.
    for(int i = static_cast<int>(vars_affected.size())-1; i>=0; i--) {
        int to_var =  vars_affected[i];
        int from_var = from_vars[to_var];

        pair<int, int> to_pair = get_new_var_val_by_old_var_val(from_var, state[from_var]);
        assert( to_pair.first == to_var);

        // Check if the values are the same, then continue to the next aff. var.
        if (to_pair.second == state[to_var])
            continue;

        if (to_pair.second < state[to_var])
            from_here = i;

        break;
    }
    if (from_here == static_cast<int>(vars_affected.size()))
        return false;

    for(size_t i = 0; i < affected_vars_cycles.size(); i++) {
        if (affected_vars_cycles[i].size() == 1) {
            int var = affected_vars_cycles[i][0];
            pair<int, int> to_pair = get_new_var_val_by_old_var_val(var, state[var]);
            state[var] = to_pair.second;
            continue;
        }
        // Remembering one value to be rewritten last
        int last_var = affected_vars_cycles[i][affected_vars_cycles[i].size()-1];
        int last_val = state[last_var];

        for (int j=affected_vars_cycles[i].size()-1; j>0; j--) {
            // writing into variable affected_vars_cycles[i][j]
            int to_var = affected_vars_cycles[i][j];
            int from_var = affected_vars_cycles[i][j-1];
            int from_val = state[from_var];
            pair<int, int> to_pair = get_new_var_val_by_old_var_val(from_var, from_val);
            state[to_var] = to_pair.second;
        }
        // writing the last one
        pair<int, int> to_pair = get_new_var_val_by_old_var_val(last_var, last_val);
        state[affected_vars_cycles[i][0]] = to_pair.second;
    }

    return true;
}
