#! /usr/bin/env python

import sys, json
import subprocess
import logging
from subprocess import SubprocessError
from pathlib import Path

import forbiditerative
build_dir = Path(forbiditerative.__file__).parent / 'builds' / 'release' / 'bin'
default_build_args = ["--build", str(build_dir.absolute())]

def run_planner(planner_args) -> dict:

    try:
        import tempfile
        with tempfile.NamedTemporaryFile() as result_file:
            subprocess.run([sys.executable, "-B", "-m", "forbiditerative.plan"] + default_build_args + planner_args + ["--results-file", str(result_file.name)])

            with open(str(result_file.name)) as plans_file:
                data = json.load(plans_file)
            return data
        
    except SubprocessError as err:
        logging.error(err.output.decode())
        return None
    



"""
def plan_unordered_topq(domain_file : Path, problem_file : Path, quality_bound : float, number_of_plans_bound : int = None) -> dict:

    try:
        import tempfile
        with tempfile.NamedTemporaryFile() as result_file:
            planner_args = ["--planner", "unordered_topq", "--domain", str(domain_file.absolute()), 
                "--problem", str(problem_file.absolute()), "--quality-bound", str(quality_bound), "--symmetries", "--use-local-folder", "--clean-local-folder", "--suppress-planners-output", "--plans-as-json", "--results-file", str(result_file.name)]
            if number_of_plans_bound:
                planner_args.extend(["--number-of-plans", str(number_of_plans_bound)])

            subprocess.run([sys.executable, "-B", "-m", "forbiditerative.plan"] + build_args + planner_args)

            with open(str(result_file.name)) as plans_file:
                data = json.load(plans_file)
            return data
        
    except SubprocessError as err:
        logging.error(err.output.decode())
        return None
"""

def plan_unordered_topq(domain_file : Path, problem_file : Path, quality_bound : float, number_of_plans_bound : int = None) -> dict:

    planner_args = ["--planner", "unordered_topq", "--domain", str(domain_file.absolute()), 
                "--problem", str(problem_file.absolute()), "--quality-bound", str(quality_bound), 
                "--symmetries", "--use-local-folder", "--clean-local-folder", "--suppress-planners-output", "--plans-as-json"]
    if number_of_plans_bound:
        planner_args.extend(["--number-of-plans", str(number_of_plans_bound)])
    return run_planner(planner_args)


def plan_submultisets_topq(domain_file : Path, problem_file : Path, quality_bound : float, number_of_plans_bound : int = None) -> dict:

    planner_args = ["--planner", "submultisets_topq", "--domain", str(domain_file.absolute()), 
                "--problem", str(problem_file.absolute()), "--quality-bound", str(quality_bound), 
                "--symmetries", "--use-local-folder", "--clean-local-folder", "--suppress-planners-output", "--plans-as-json"]
    if number_of_plans_bound:
        planner_args.extend(["--number-of-plans", str(number_of_plans_bound)])
    return run_planner(planner_args)


def plan_subsets_topq(domain_file : Path, problem_file : Path, quality_bound : float, number_of_plans_bound : int = None) -> dict:

    planner_args = ["--planner", "subsets_topq", "--domain", str(domain_file.absolute()), 
                "--problem", str(problem_file.absolute()), "--quality-bound", str(quality_bound), 
                "--symmetries", "--use-local-folder", "--clean-local-folder", "--suppress-planners-output", "--plans-as-json"]
    if number_of_plans_bound:
        planner_args.extend(["--number-of-plans", str(number_of_plans_bound)])
    return run_planner(planner_args)


def plan_topk(domain_file : Path, problem_file : Path, number_of_plans_bound : int) -> dict:
    planner_args = ["--planner", "topk", "--domain", str(domain_file.absolute()), 
                "--problem", str(problem_file.absolute()), "--number-of-plans", str(number_of_plans_bound), 
                "--symmetries", "--use-local-folder", "--clean-local-folder", "--suppress-planners-output", "--plans-as-json"]
    return run_planner(planner_args)
    

def plan_diverse_agl(domain_file : Path, problem_file : Path, number_of_plans_bound : int) -> dict:
    planner_args = ["--planner", "diverse", "--domain", str(domain_file.absolute()), 
                "--problem", str(problem_file.absolute()), "--number-of-plans", str(number_of_plans_bound), 
                "--symmetries", "--use-local-folder", "--clean-local-folder", "--suppress-planners-output", "--plans-as-json"]
    return run_planner(planner_args)

