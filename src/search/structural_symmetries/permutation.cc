#include "permutation.h"

#include "../global_state.h"
#include "../globals.h"

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>


using namespace std;

//namespace structural_symmetries {

void Permutation::_allocate() {
    value = new int[get_length()];
    affected.assign(get_num_vars(), false);
	vars_affected.clear();
    from_vars.assign(get_num_vars(), -1);
	affected_vars_cycles.clear();
}

void Permutation::_deallocate() {
    delete[] value;
}

void Permutation::_copy_value_from_permutation(const Permutation &perm) {
    for (int i = 0; i < get_length(); i++)
        set_value(i, perm.get_value(i));
}

void Permutation::_inverse_value_from_permutation(const Permutation &perm) {
    for (int i = 0; i < get_length(); i++)
        set_value(perm.get_value(i), i);
}

Permutation::Permutation(const Group* _group) : group(_group) {
    _allocate();
    for (int i = 0; i < get_length(); i++)
        set_value(i,i);
    finalize();
}

Permutation::Permutation(const Group* _group, const unsigned int* full_permutation) : group(_group) {
	_allocate();
	for (int i = 0; i < get_length(); i++){
    	set_value(i,full_permutation[i]);
	}
	finalize();
}

Permutation::Permutation(const Permutation &perm, bool invert) : group(perm.group) {
    _allocate();
    if (invert) {
        _inverse_value_from_permutation(perm);
    } else {
        _copy_value_from_permutation(perm);
    }
    finalize();
}

//// New constructor to use instead of * operator
Permutation::Permutation(const Permutation *perm1, const Permutation *perm2) : group(perm1->group) {
    _allocate();

    for (int i = 0; i < get_length(); i++) {
        set_value(i, perm2->get_value(perm1->get_value(i)));
    }
    finalize();
}



Permutation::~Permutation(){
	_deallocate();
}

int gcd(int m, int n) {
    assert(m > 0 && n > 0);
    int r;

    while (n != 0) {
        r = m % n;
        m = n;
        n = r;
    }
    return m;
}

int lcm(int a, int b) {
    int temp = gcd(a, b);

    return temp ? (a / temp * b) : 0;
}

void Permutation::finalize(){
	// Sorting the vector of affected variables
	::sort(vars_affected.begin(), vars_affected.end());

    // Going over the vector from_vars of the mappings of the variables and
    // finding cycles, computing the least common multiple of the cycles'
    // sizes to determine the permutation's order
	vector<bool> marked;
    marked.assign(get_length(), false);
    order = 1;
    for (int var = 0; var < static_cast<int>(from_vars.size()); var++) {
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
        order = lcm(order, cycle.size());
        // Get here when from_vars[current] == i.
        affected_vars_cycles.push_back(cycle);
	}

    // Go over all variables that are not part of a cycle and see if the
    // mapping of variable values increases the permutation's order
    for (int i = get_num_vars(); i < get_length(); ++i) {
        int to_i = get_value(i);
        int var = get_var_by_index(i);
        int to_var = get_var_by_index(to_i);
        if (!marked[i] && var == to_var && to_i != i) {
            int start = i;
            marked[start] = true;
            int current = i;
            int cycle_size = 1;
            while (get_value(current) != start) {
                current = get_value(current);
                marked[current] = true;
                ++cycle_size;
            }
            order = lcm(order, cycle_size);
        }
    }
}

bool Permutation::identity() const{
	return vars_affected.size() == 0;
}

void Permutation::print_cycle_notation() const {
	vector<int> done;
    for (int i = get_num_vars(); i < get_length(); i++){
		if (find(done.begin(), done.end(), i) == done.end()){
	        int current = i;
	        if(get_value(i) == i) continue; //don't print cycles of size 1

            pair<int, int> varval = get_var_val_by_index(i);
	        cout<<"("<< g_fact_names[varval.first][(int) varval.second]  <<" ";

	        while(get_value(current) != i){
	            done.push_back(current);
	            current = get_value(current);

                pair<int, int> currvarval = get_var_val_by_index(current);
	            cout<< g_fact_names[currvarval.first][(int) currvarval.second] <<" ";
	        }
	        done.push_back(current);
	        cout<<") ";
		}
	}
	cout << endl;
	cout << "Variables:  ";
    for(size_t i = 0; i < vars_affected.size(); i++) cout << vars_affected[i] << "  ";
	cout << endl << "Variables permuted:  ";

    for(size_t i = 0; i < vars_affected.size(); i++) cout << from_vars[vars_affected[i]] << " -> " << vars_affected[i] << "  ";
	cout << endl;

	cout << "Affected variables by cycles: " << endl;
	print_affected_variables_by_cycles();
}

void Permutation::print_affected_variables_by_cycles() const {
    for (size_t i=0; i < affected_vars_cycles.size(); i++) {
		cout << "( " ;
        for (size_t j=0; j < affected_vars_cycles[i].size(); j++) {
			cout << affected_vars_cycles[i][j] << " ";
		}
		cout << ")  ";
	}
	cout << endl;
}


void Permutation::dump_var_vals() const {
    for (int i = 0; i < get_num_vars(); ++i) {
        for (int j = 0; j < g_variable_domain[i]; ++j) {
            pair<int, int> var_val = get_new_var_val_by_old_var_val(i, j);
            cout << i << "=" << j << "->"
                 << var_val.first << "=" << var_val.second << ",";
        }
        cout << endl;
    }
//    cout << endl;
}

void Permutation::dump() const {
	for(int i = 0; i < get_length(); i++){
		if (get_value(i) != i)
			cout << setw(4) << i;
	}
	cout << endl;
	for(int i = 0; i < get_length(); i++){
		if (get_value(i) != i)
			cout << setw(4) << get_value(i);
	}
	cout << endl;
}

void Permutation::dump_fdr() const {
    for(int i = get_num_vars(); i < get_length(); i++){
		if (get_value(i) != i) {
            pair<int, int> varval = get_var_val_by_index(i);
			cout << setw(10) <<  "[" << g_variable_name[varval.first] << "] -> "
		             << static_cast<int>(varval.second);
		}
	}
	cout << endl;
    for(int i = get_num_vars(); i < get_length(); i++){
		if (get_value(i) != i) {
            pair<int, int> varval = get_var_val_by_index(get_value(i));
			cout << setw(10) <<  "[" << g_variable_name[varval.first] << "] -> "
		             << static_cast<int>(varval.second);
		}
	}
	cout << endl;
}

///////////////////////////////////////////////////////////////////////////////
void Permutation::set_value(int ind, int val) {
	value[ind] = val;
	set_affected(ind, val);
}

void Permutation::set_affected(int ind, int val) {

    if (ind < get_num_vars() || ind == val)
		return;


	int var = get_var_by_index(ind);
	int to_var = get_var_by_index(val);

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


std::pair<int, int> Permutation::get_new_var_val_by_old_var_val(const int var, const int val) const {
	int old_ind = group->get_index_by_var_val_pair(var, val);
	int new_ind = get_value(old_ind);
	return get_var_val_by_index(new_ind);
}

//////////////////////////////////////////////////////////////////////////////////////////
// This method compares the state to the state resulting from permuting it.
// If the original state is bigger than the resulted one, it is rewritten with the latter and true is returned.
////////////////////  New version - no extra buffer is needed, faster copy ///////////////
bool Permutation::replace_if_less(int* state) const {
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
//}
