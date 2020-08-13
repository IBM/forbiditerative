#!/bin/bash
# $1 domain
# $2 problem
# $3 number of plans (k)
# $4 metric (stability, state, uniqueness, stability-state, stability-uniqueness, state-uniqueness, stability-state-uniqueness, top-cost)
# $5 total number of plans (optional, used k as default)

if [ "$#" -lt 4 ] || [ "$#" -gt 5 ]; then
    echo "Illegal number of parameters"
    exit 1
fi

num_plans=$3

if [ "$#" -eq 5 ]; then
    num_plans=$5
fi

if [ "$num_plans" -lt $3 ]; then
    echo "Total number of plans, if specified, should be at least as large as k"
    exit 1
fi


SOURCE="$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)"
$SOURCE/plan.py --planner diverse --domain $1 --problem $2 --number-of-plans $num_plans --use-local-folder --clean-local-folder

PLANSDIR=$SOURCE/found_plans

## replace the value in num_found_plans with the actual number of plans in PLANSDIR
num_plans=`ls -1q $PLANSDIR/sas_plan.* | wc -l`

function get_metric_param() {
    ret=""
    if [[ $1 == *"stability"* ]]; then
        ret=$ret"compute_stability_metric=true,"
    fi    
    if [[ $1 == *"state"* ]]; then
        ret=$ret"compute_states_metric=true,"
    fi    
    if [[ $1 == *"uniqueness"* ]]; then
        ret=$ret"compute_uniqueness_metric=true,"
    fi    
    echo $ret 
}

SCORE="subset("$(get_metric_param $4)"aggregator_metric=avg,plans_as_multisets=false,plans_subset_size=$3,exact_method=false,dump_plans=true)"

domain="$(cd "$(dirname "$1")"; pwd)/$(basename "$1")"
problem="$(cd "$(dirname "$2")"; pwd)/$(basename "$2")"
echo $DIVERSE_SCORE_COMPUTATION_PATH/fast-downward.py $domain $problem --diversity-score \"$SCORE\" --internal-plan-files-path $PLANSDIR --internal-num-plans-to-read $num_plans

(mkdir -p $PLANSDIR/done && cd $PLANSDIR/done &&
$DIVERSE_SCORE_COMPUTATION_PATH/fast-downward.py $domain $problem --diversity-score $SCORE --internal-plan-files-path $PLANSDIR --internal-num-plans-to-read $num_plans)
