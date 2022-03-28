#!/bin/bash

# $1 domain
# $2 problem
# $3 plan
# $4 number of plans (upper bound)

if [ "$#" -ne 4 ]; then
    echo "Illegal number of parameters"
    exit 1
fi

#./fast-downward.py examples/gripper/domain.pddl examples/gripper/prob01.pddl --symmetries "sym=structural_symmetries(time_bound=0,search_symmetries=dks, stabilize_initial_state=true, keep_operator_symmetries=true)" --search "forbid_iterative(reformulate = NONE_FIND_ADDITIONAL_PLANS, extend_plans_with_symmetry=sym,  reduce_plan_orders=NAIVE_ALL_DFS_NODUP, dump=false, number_of_plans=1000,  external_plan_file=found_plans/done/sas_plan.1)"

SYM_PARAM='sym=structural_symmetries(time_bound=0,search_symmetries=dks,stabilize_initial_state=true,keep_operator_symmetries=true)'
SEARCH_PARAM="forbid_iterative(reformulate=NONE_FIND_ADDITIONAL_PLANS,extend_plans_with_symmetry=sym,reduce_plan_orders=NAIVE_ALL_DFS_NODUP,dump=false,number_of_plans=$4,external_plan_file=$3)"

SOURCE="$( dirname "${BASH_SOURCE[0]}" )"

$SOURCE/fast-downward.py $1 $2 --symmetries $SYM_PARAM --search $SEARCH_PARAM

#echo $SOURCE/fast-downward.py $1 $2 --symmetries \"$SYM_PARAM\" --search \"$SEARCH_PARAM\"
