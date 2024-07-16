#! /usr/bin/env python

import sys, json
import subprocess
import logging
from subprocess import SubprocessError
from pathlib import Path
from typing import List, Literal, Optional

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
            out = subprocess.run([sys.executable, "-B", "-m", "forbiditerative.plan"] + default_build_args + planner_args + ["--results-file", str(result_file.name)], 
                           stdout = subprocess.PIPE, stderr = subprocess.PIPE)

            data = {}
            data["planner_output"] = out.stdout.decode()
            data["planner_error"] = out.stderr.decode()
            data["plans"] = []

            plans_file = Path(str(result_file.name))
            if plans_file.is_file() and plans_file.stat().st_size > 0:
                plans = json.loads(plans_file.read_text(encoding="UTF-8"))
                data["plans"] = plans["plans"]

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

def plan_unordered_topq(domain_file : Path, problem_file : Path, quality_bound : float, number_of_plans_bound : Optional[int] = None, timeout: Optional[int] = None, suppress_planner_output : Optional[bool] = True) -> dict:

    planner_args = ["--planner", "unordered_topq", "--domain", str(domain_file.absolute()), 
                "--problem", str(problem_file.absolute()), "--quality-bound", str(quality_bound)]

    return run_planner(extend_args(planner_args, timeout, suppress_planner_output))


def plan_submultisets_topq(domain_file : Path, problem_file : Path, quality_bound : float, number_of_plans_bound : Optional[int] = None, timeout: Optional[int] = None, suppress_planner_output : Optional[bool] = True) -> dict:

    planner_args = ["--planner", "submultisets_topq", "--domain", str(domain_file.absolute()), 
                "--problem", str(problem_file.absolute()), "--quality-bound", str(quality_bound)]
    if number_of_plans_bound:
        planner_args.extend(["--number-of-plans", str(number_of_plans_bound)])
    return run_planner(extend_args(planner_args, timeout, suppress_planner_output))


def plan_subsets_topq(domain_file : Path, problem_file : Path, quality_bound : float, number_of_plans_bound : Optional[int] = None, timeout: Optional[int] = None, suppress_planner_output : Optional[bool] = True) -> dict:
    planner_args = ["--planner", "subsets_topq", "--domain", str(domain_file.absolute()), 
                "--problem", str(problem_file.absolute()), "--quality-bound", str(quality_bound)]
    if number_of_plans_bound:
        planner_args.extend(["--number-of-plans", str(number_of_plans_bound)])
    return run_planner(extend_args(planner_args, timeout, suppress_planner_output))


def plan_topk(domain_file : Path, problem_file : Path, number_of_plans_bound : int, timeout: Optional[int] = None, suppress_planner_output : Optional[bool] = True) -> dict:
    planner_args = ["--planner", "topk", "--domain", str(domain_file.absolute()), 
                "--problem", str(problem_file.absolute()), "--number-of-plans", str(number_of_plans_bound)]
    return run_planner(extend_args(planner_args, timeout, suppress_planner_output))
    

def plan_diverse_agl(domain_file : Path, problem_file : Path, number_of_plans_bound : int, timeout: Optional[int] = None, suppress_planner_output : Optional[bool] = True) -> dict:
    planner_args = ["--planner", "diverse", "--domain", str(domain_file.absolute()), "--problem", str(problem_file.absolute()), "--number-of-plans", str(number_of_plans_bound)]
    return run_planner(extend_args(planner_args, timeout, suppress_planner_output))

def extend_args(planner_args, timeout: Optional[int] = None, suppress_planner_output : Optional[bool] = True):
    planner_args.extend(["--symmetries", "--use-local-folder", "--clean-local-folder", "--plans-as-json"])
    if suppress_planner_output:
        planner_args.extend(["--suppress-planners-output"])
    if timeout:
        planner_args.extend(["--overall-time-limit", str(timeout)])
    return planner_args

def get_landmarks(domain_file : Path, problem_file : Path, method: LandmarkMethods = default_landmark_method) -> dict:
    """Execute the planner on the task, no search."""
    try:
        import tempfile
        with tempfile.TemporaryDirectory() as run_dir:

            landmarks_file = Path(str(run_dir)) / "landmarks.json"
            command =  ["--alias", f'get_landmarks_{method}', str(domain_file.absolute()), str(problem_file.absolute())]
            out = subprocess.run([sys.executable, "-B", "-m", "driver.main"] + default_build_args + command, cwd=run_dir, stdout = subprocess.PIPE, stderr = subprocess.PIPE)

            data = {}
            data["planner_output"] = out.stdout.decode()
            data["planner_error"] = out.stderr.decode()
                
            data["landmarks"] = []

            if landmarks_file.is_file() and landmarks_file.stat().st_size > 0:
                lms = json.loads(landmarks_file.read_text(encoding="UTF-8"))
                data["landmarks"] = lms["landmarks"]

            return data
    
    except SubprocessError as err:
        logging.error(err.output.decode())
        return None
    

def get_dot(domain_file : Path, problem_file : Path, plans: List[List[str]]) -> str:
    """Execute the planner on the task, no search."""
    try:
        import tempfile
        with tempfile.TemporaryDirectory() as run_dir:

            # run_dir = Path(tempfile.gettempdir())
            graph_file = Path(str(run_dir)) / "graph0.dot"
            plans_path = Path(str(run_dir)) / "plans"
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
            subprocess.run([sys.executable, "-B", "-m", "driver.main"] + default_build_args + command, cwd=run_dir, stdout = subprocess.DEVNULL, stderr = subprocess.DEVNULL)

            res = ""
            if graph_file.is_file() and graph_file.stat().st_size > 0:
                res = graph_file.read_text(encoding="UTF-8")

            return res
    
    except SubprocessError as err:
        logging.error(err.output.decode())
        return None
    
def get_PDG(domain_file : Path, problem_file : Path) -> str:
    """Execute the planner on the task, no search."""
    try:
        import tempfile
        with tempfile.TemporaryDirectory() as run_dir:

            # run_dir = Path(tempfile.gettempdir())
            graph_file = Path(str(run_dir)) / "symmetry-graph.dot"

            command = [str(domain_file.absolute()), str(problem_file.absolute())] + ["--symmetries", 
                    'sym=structural_symmetries(time_bound=0,search_symmetries=oss,dump_symmetry_graph=true)', 
                    "--search", 'astar(const(infinity()),symmetries=sym)']
            subprocess.run([sys.executable, "-B", "-m", "driver.main"] + default_build_args + command, cwd=run_dir, stdout = subprocess.DEVNULL, stderr = subprocess.DEVNULL)

            res = ""
            if graph_file.is_file() and graph_file.stat().st_size > 0:
                res = graph_file.read_text(encoding="UTF-8")

            return res
    
    except SubprocessError as err:
        logging.error(err.output.decode())
        return None
    

def get_ASG(domain_file : Path, problem_file : Path) -> str:
    """Execute the planner on the task, no search."""
    try:
        import tempfile
        with tempfile.TemporaryDirectory() as run_dir:

            # run_dir = Path(tempfile.gettempdir())
            graph_file = Path(str(run_dir)) / "abstract-structure-graph.dot"

            command = [str(domain_file.absolute()), str(problem_file.absolute())] + ["--translate-options", "--compute-abstract-structure-graph", "--only-functions-from-initial-state", "--dump-dot-graph"]
            subprocess.run([sys.executable, "-B", "-m", "driver.main"] + default_build_args + command, cwd=run_dir, stdout = subprocess.DEVNULL, stderr = subprocess.DEVNULL)

            res = ""
            if graph_file.is_file() and graph_file.stat().st_size > 0:
                res = graph_file.read_text(encoding="UTF-8")

            return res
    
    except SubprocessError as err:
        logging.error(err.output.decode())
        return None
