#! /usr/bin/env python

import sys, json
import subprocess
import logging
from subprocess import SubprocessError
from pathlib import Path
from typing import List, Literal

import forbiditerative
build_dir = Path(forbiditerative.__file__).parent / 'builds' / 'release' / 'bin'
default_build_args = ["--build", str(build_dir.absolute())]


# From here https://www.fast-downward.org/Doc/LandmarkFactory
LandmarkMethods = Literal['exhaust', 'h1', 'h2', 'rhw', 'zg']
default_landmark_method = 'rhw'

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


def get_landmarks(domain_file : Path, problem_file : Path, method: LandmarkMethods = default_landmark_method) -> dict:
    """Execute the planner on the task, no search."""
    try:
        import tempfile

        run_dir = Path(tempfile.gettempdir())
        landmarks_file = run_dir / "landmarks.json"
        command =  ["--alias", f'get_landmarks_{method}', str(domain_file.absolute()), str(problem_file.absolute())]
        subprocess.run([sys.executable, "-B", "-m", "driver.main"] + default_build_args + command, cwd=run_dir)
        return json.loads(landmarks_file.read_text())
    
    except SubprocessError as err:
        logging.error(err.output.decode())
        return None
    

def get_dot(domain_file : Path, problem_file : Path, plans: List[List[str]]) -> str:
    """Execute the planner on the task, no search."""
    try:
        import tempfile

        run_dir = Path(tempfile.gettempdir())
        graph_file = run_dir / "graph0.dot"
        plans_path = run_dir / "plans"
        if not (plans_path.is_dir()):
            plans_path.mkdir()
        counter = 1
        for plan in plans:
            plan_file = Path(plans_path / f"sas_plan.{counter}")
            actions = ["(" + a + ")" for a in plan]
            plan_file.write_text("\n".join(actions) + "\n")
            counter += 1

        counter -= 1
        command = [str(domain_file.absolute()), str(problem_file.absolute())] + ["--search",
                f"forbid_iterative(reformulate=NONE,read_plans_and_dump_graph=true,external_plans_path={plans_path},number_of_plans_to_read={counter})"]
        subprocess.run([sys.executable, "-B", "-m", "driver.main"] + default_build_args + command, cwd=run_dir)
        return graph_file.read_text()
    
    except SubprocessError as err:
        logging.error(err.output.decode())
        return None
    