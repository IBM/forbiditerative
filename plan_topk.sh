#!/bin/bash

# $1 domain
# $2 problem
# $3 number of plans (k)
# $4 (optional, default NONE) reordering (NONE, NEIGHBOURS_INTERFERE, NAIVE_ALL_DFS_NODUP)

if [ "$#" -lt 3 ] || [ "$#" -gt 4 ]; then
    echo "Illegal number of parameters"
    exit 1
fi

REORDERING=""
if [ "$#" -eq 4 ]; then
    REORDERING="--reordering $4"
fi

SOURCE="$( dirname "${BASH_SOURCE[0]}" )"
$SOURCE/plan.py --planner topk --domain $1 --problem $2 --number-of-plans $3 $REORDERING --symmetries --use-local-folder --clean-local-folder #--keep-intermediate-tasks #  #--plans-as-json 

