#! /usr/bin/env python

import sys, json
import subprocess
import logging
from subprocess import SubprocessError
from pathlib import Path

import forbiditerative

def plan_unordered_topq(domain_file : Path, problem_file : Path, quality_bound : float, number_of_plans_bound : int = None) -> dict:
    build_dir = Path(forbiditerative.__file__).parent / 'builds' / 'release' / 'bin'
    build_args = ["--build", str(build_dir.absolute())]

    try:
        import tempfile
        with tempfile.NamedTemporaryFile() as result_file:
            planner_args = ["--planner", "unordered_topq", "--domain", str(domain_file.absolute()), 
                "--problem", str(problem_file.absolute()), "--quality-bound", str(quality_bound), "--symmetries", "--use-local-folder", "--clean-local-folder", "--suppress-planners-output", "--plans-as-json", "--results-file", str(result_file.name)]
            if number_of_plans_bound:
                planner_args.extend(["--number-of-plans", str(number_of_plans_bound)])

            print(build_args + planner_args)
            subprocess.run([sys.executable, "-B", "-m", "forbiditerative.plan"] + build_args + planner_args)

            with open(str(result_file.name)) as plans_file:
                data = json.load(plans_file)
            return data
        
    except SubprocessError as err:
        logging.error(err.output.decode())