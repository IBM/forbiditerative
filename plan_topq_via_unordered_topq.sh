#!/bin/bash

# $1 domain
# $2 problem
# $3 quality bound (>= 1.0)

if [ "$#" -lt 3 ] || [ "$#" -gt 4 ]; then
    echo "Illegal number of parameters"
    exit 1
fi

SOURCE="$( dirname "${BASH_SOURCE[0]}" )"
$SOURCE/plan.py --planner topq_via_unordered_topq --domain $1 --problem $2 --quality-bound $3 --symmetries --use-local-folder --clean-local-folder # --keep-intermediate-tasks #--plans-as-json 

