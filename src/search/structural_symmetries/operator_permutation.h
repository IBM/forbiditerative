#ifndef STRUCTURAL_SYMMETRIES_OPERATOR_PERMUTATION_H
#define STRUCTURAL_SYMMETRIES_OPERATOR_PERMUTATION_H

#include <vector>

#include "group.h"

/*
  This class represents operator symmetries, i.e., it only stores a mapping of
  operators of the planning task.
*/
class OperatorPermutation{
public:
    OperatorPermutation(const Group &group, const unsigned int *full_perm);
    ~OperatorPermutation() = default;

    bool identity() const;
    int get_permuted_operator_no(int op_no) const {
        return to_ops_no[op_no];
    }

private:
    const Group &group;
    std::vector<int> to_ops_no;
    std::vector<int> ops_affected;
    std::vector<bool> is_op_affected;

    void set_value(int ind, int val);

    void finalize();
    void _allocate();
};

#endif
