#!/bin/bash
# $1 domain
# $2 problem
# $3 number of plans (k)
# $4 quality bound (b)

if [ "$#" -ne 4 ]; then
    echo "Illegal number of parameters"
    exit 1
fi

domain="$(cd "$(dirname "$1")"; pwd)/$(basename "$1")"
problem="$(cd "$(dirname "$2")"; pwd)/$(basename "$2")"

SOURCE="$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)"
export PYTHONPATH=$PWD && $SOURCE/forbiditerative/plan.py --planner extended_unordered_topq --domain $domain --problem $problem --number-of-plans $3 --quality-bound $4 --symmetries --use-local-folder --clean-local-folder #--suppress-planners-output #--keep-intermediate-tasks

FINALPLANSDIR=$PWD/found_plans/done

num_found_plans=`ls -1q $FINALPLANSDIR/sas_plan.* | wc -l | tr -d '[:space:]'`
echo "The number of plans found after extending is "$num_found_plans
if [ "$num_found_plans" -lt $3 ]; then
    echo "The number of plans found in the first phase is smaller than the requested number of plans"
    echo "These are all plans of quality up to the specified bound"
    echo "Computing the metric of the set of all found plans"
fi
