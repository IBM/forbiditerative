#!/bin/bash

# $1 domain
# $2 problem
# $3 number of plans (k)
# $4 OPTIONAL overall time limit (seconds)
TIMELIMIT=""
if [ "$#" -eq 4 ]; then
    TIMELIMIT="--overall-time-limit $4"
elif [ "$#" -ne 3 ]; then
    echo "Illegal number of parameters"
fi

SOURCE="$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)"
export PYTHONPATH=${SOURCE} && $SOURCE/forbiditerative/plan.py --planner diverse --domain $1 --problem $2 --number-of-plans $3 --symmetries --use-local-folder --clean-local-folder ${TIMELIMIT} # --suppress-planners-output
# export PYTHONPATH=$PWD && $SOURCE/forbiditerative/plan.py --planner diverse --domain $1 --problem $2 --number-of-plans $3 --symmetries --use-local-folder --keep-intermediate-tasks

