#!/bin/bash
# $1 domain
# $2 problem
# $3 number of plans (k)
# $4 quality bound (b)
# $5 metric (stability, state, uniqueness, stability-state, stability-uniqueness, state-uniqueness, stability-state-uniqueness, top-cost)

if [ "$#" -ne 5 ]; then
    echo "Illegal number of parameters"
    exit 1
fi

domain="$(cd "$(dirname "$1")"; pwd)/$(basename "$1")"
problem="$(cd "$(dirname "$2")"; pwd)/$(basename "$2")"

SOURCE="$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)"
$SOURCE/plan.py --planner extended_unordered_topq --domain $domain --problem $problem --number-of-plans $3 --quality-bound $4 --symmetries --use-local-folder --clean-local-folder #--suppress-planners-output #--keep-intermediate-tasks 

FINALPLANSDIR=$PWD/found_plans/done
PLANSDIR=$PWD/found_plans/almost_done

## Renaming the folder
mv $FINALPLANSDIR $PLANSDIR

source $SOURCE/helper_functions.sh

num_found_plans=`ls -1q $PLANSDIR/sas_plan.* | wc -l | tr -d '[:space:]'`
echo "The number of plans found after extending is "$num_found_plans
num_plans_to_find=$3
if [ "$num_found_plans" -lt $num_plans_to_find ]; then
    echo "The number of plans found in the first phase is smaller than the requested number of plans"
    echo "These are all plans of quality up to the specified bound"
    echo "Computing the metric of the set of all found plans"
    num_plans_to_find=$num_found_plans
fi

SCORE="subset_optimal("$(get_metric_param $5)"aggregator_metric=min,plans_as_multisets=true,plans_subset_size=$num_plans_to_find,metric_computation_method=mip,dump_plans=true)"

echo $DIVERSE_SCORE_COMPUTATION_PATH/fast-downward.py $domain $problem --diversity-score \"$SCORE\" --internal-plan-files-path $PLANSDIR --internal-num-plans-to-read $num_found_plans

(mkdir -p $FINALPLANSDIR && cd $FINALPLANSDIR &&
$DIVERSE_SCORE_COMPUTATION_PATH/fast-downward.py $domain $problem --diversity-score $SCORE --internal-plan-files-path $PLANSDIR --internal-num-plans-to-read $num_found_plans)
