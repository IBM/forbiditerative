#! /usr/bin/env python

import sys
import subprocess
import logging
from subprocess import SubprocessError
from driver.plan_manager import PlanManager

if __name__ == "__main__":
    domain = sys.argv[1]
    problem = sys.argv[2]
    plan_file = sys.argv[3]
    # planner_def_opts = [planner, "--plan-file", plan_file]

    planner_def_opts = [sys.executable, "-m" "forbiditerative.fast-downward", "--plan-file", plan_file]
    planner_curr_opts = ["--transform-task", "preprocess", "--alias", "seq-agl-cerberus2018", domain, problem]
    print(planner_def_opts + planner_curr_opts)

    plan_manager = PlanManager(plan_file)
    plan_manager.delete_existing_plans()
    sys.stdout.flush()

    try:
        subprocess.check_call(planner_def_opts + planner_curr_opts, shell=False)
    except subprocess.CalledProcessError as e:
        logging.error(e.output.decode())

