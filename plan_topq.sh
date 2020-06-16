#!/bin/bash

# $1 domain
# $2 problem
# $3 quality bound (>= 1.0)

if [ "$#" -ne 3 ]; then
    echo "Illegal number of parameters"
    exit 1
fi

SOURCE="$( dirname "${BASH_SOURCE[0]}" )"
$SOURCE/plan.py --planner topq --domain $1 --problem $2 --quality-bound $3 --use-local-folder --clean-local-folder #--keep-intermediate-tasks 

