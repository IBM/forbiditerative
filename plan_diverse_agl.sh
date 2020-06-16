#!/bin/bash

# $1 domain
# $2 problem
# $3 number of plans (k)

if [ "$#" -ne 3 ]; then
    echo "Illegal number of parameters"
fi

SOURCE="$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)"
$SOURCE/plan.py --planner diverse --domain $1 --problem $2 --number-of-plans $3 --use-local-folder --clean-local-folder #--plans-as-json

