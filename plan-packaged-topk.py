#! /usr/bin/env python

import sys
import subprocess
import logging
from subprocess import SubprocessError

if __name__ == "__main__":
    domain = sys.argv[1]
    problem = sys.argv[2]
    k = sys.argv[3]
    command = [sys.executable, "-m" "forbiditerative.plan", "--planner", "topk", "--domain", domain, "--problem", problem, "--number-of-plans", k, "--reordering", "NEIGHBOURS_INTERFERE", "--symmetries", "--use-local-folder", "--clean-local-folder", "--suppress-planners-output"]
    try:
        subprocess.check_call(command, shell=False)
    except subprocess.CalledProcessError as e:
        logging.error(e.output.decode())

