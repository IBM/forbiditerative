#ifndef STRUCTURAL_SYMMETRIES_OPERATOR_PERMUTATION_H
#define STRUCTURAL_SYMMETRIES_OPERATOR_PERMUTATION_H

//class Permutation;

//#include "permutation.h"
//#include "group.h"

#include <vector>
#include <memory>

//class Group;

//namespace structural_symmetries {

class OperatorPermutation {
public:
//    explicit OperatorPermutation(const Permutation& permutation);
    explicit OperatorPermutation(const unsigned int *full_perm, int length);
    ~OperatorPermutation();

    bool identity() const;

    int get_permuted_operator_no(int op_no) const {
        return to_ops_no[op_no];
    }

    const std::vector<int>& get_affected_operators_no() const { return ops_affected; }
private:
//    Group* group;
    std::vector<int> to_ops_no;
    std::vector<int> ops_affected;
    std::vector<bool> is_op_affected;

    void set_value(int ind, int val);
    void finalize();
    void _allocate();
    void _deallocate();

//    int find_symmetric_op_no(const Permutation& permutation, int op_no) const;
};
//}
#endif
