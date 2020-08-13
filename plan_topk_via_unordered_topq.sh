#!/bin/bash

# $1 domain
# $2 problem
# $3 number of plans (k)

if [ "$#" -lt 3 ] || [ "$#" -gt 4 ]; then
    echo "Illegal number of parameters"
    exit 1
fi

SOURCE="$( dirname "${BASH_SOURCE[0]}" )"
$SOURCE/plan.py --planner topk_via_unordered_topq --domain $1 --problem $2 --number-of-plans $3 --symmetries --use-local-folder --clean-local-folder # --keep-intermediate-tasks # --plans-as-json 

