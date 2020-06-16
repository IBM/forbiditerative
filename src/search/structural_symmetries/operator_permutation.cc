#include "operator_permutation.h"

#include "../global_operator.h"
#include "../globals.h"
#include "../utils/system.h"

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>


using namespace std;

//namespace structural_symmetries {
/*
OperatorPermutation::OperatorPermutation(const Permutation& permutation){
	// Constructing the vector of permuted ops from the permutation
	_allocate();
	for (size_t i = 0; i < g_operators.size(); ++i) {
		int op_no = Permutation::length + i;
		int to_op_no = find_symmetric_op_no(permutation, op_no);
    	set_value(op_no, to_op_no);
    }
	finalize();
}
*/
OperatorPermutation::OperatorPermutation(const unsigned int* full_permutation, int length) {
	_allocate();
	for (size_t op_no = 0; op_no < g_operators.size(); op_no++) {
		int op_idx = length + op_no;
		int to_op_no = full_permutation[op_idx] - length;
    	set_value(op_no, to_op_no);
	}
	finalize();
}

OperatorPermutation::~OperatorPermutation(){
	_deallocate();
}

bool OperatorPermutation::identity() const{
	return ops_affected.size() == 0;
}

void OperatorPermutation::set_value(int op_no, int to_op_no) {
	assert(0 <= op_no && op_no < g_operators.size());
	assert(0 <= to_op_no && to_op_no < g_operators.size());
	assert(!is_op_affected[op_no]);

	to_ops_no[op_no] = to_op_no;

    if (op_no == to_op_no)
		return;

    // No need for the check here - should be run only once for each op_no
	if (!is_op_affected[op_no]) {
		ops_affected.push_back(op_no);
		is_op_affected[op_no] = true;
	}
}


void OperatorPermutation::finalize(){
	// Sorting the vector of affected variables
	::sort(ops_affected.begin(), ops_affected.end());
}

void OperatorPermutation::_allocate() {
	to_ops_no.assign(g_operators.size(), -1);
	is_op_affected.assign(g_operators.size(), false);
	ops_affected.clear();
}

void OperatorPermutation::_deallocate() {
}

//int OperatorPermutation::find_symmetric_op_no(const Permutation& /*permutation*/, int op_no) const {
//	//TODO: Implement if needed
//	utils::exit_with(utils::ExitCode::UNSUPPORTED);
//	return op_no;
//}
//}
