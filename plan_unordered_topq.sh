#!/bin/bash

# $1 domain
# $2 problem
# $3 quality bound (>= 1.0)
# $4 k bound (optional)

if [ "$#" -lt 3 ] || [ "$#" -gt 4 ]; then
    echo "Illegal number of parameters"
    exit 1
fi

KBOUND=""
if [ "$#" -eq 4 ]; then
    KBOUND="--number-of-plans $4"
fi

SOURCE="$( dirname "${BASH_SOURCE[0]}" )"
$SOURCE/plan.py --planner unordered_topq --domain $1 --problem $2 --quality-bound $3 $KBOUND --symmetries --use-local-folder --clean-local-folder #--keep-intermediate-tasks 

