#!/usr/bin/python

import re
import subprocess
import os, sys, json


FAST_DOWNWARD_ARGS = ["--build", "release64"]


def make_call(command, time_limit, local_folder, enable_output=False):
    if (sys.version_info > (3, 0)):
        import subprocess
    else:
        import subprocess32 as subprocess

    if enable_output:
        subprocess.check_call(command, timeout=time_limit, cwd=local_folder)
    else:
        FNULL = open(os.devnull, 'w')
        subprocess.check_call(command, timeout=time_limit, cwd=local_folder,stdout=FNULL, stderr=subprocess.STDOUT)


def get_external_optimal_planner_path():
    return get_base_dir()

def get_external_satisficing_planner_path():
    path = os.getenv('DIVERSE_FAST_DOWNWARD_PLANNER_PATH')
    if not path:
        print("Environment variable DIVERSE_FAST_DOWNWARD_PLANNER_PATH specifying a path to a planner to be used for getting a single plan should be declared")
        exit(1)
    return path

def get_script():
    """Get file name of main script."""
    return os.path.abspath(sys.argv[0])

def get_script_dir():
    """Get directory of main script.

    Usually a relative directory (depends on how it was called by the user.)"""
    return os.path.dirname(get_script())

def get_base_dir():
    """Assume that this script always lives in the base dir of the infrastructure."""
    return os.path.abspath(get_script_dir())

def get_topq_reformulation_callstring(curr_task_name, external_plans_path, num_plans_to_read, num_previous_plans):
    base_dir = get_base_dir()
    return [sys.executable, os.path.join(base_dir, 'fast-downward.py')] + \
        FAST_DOWNWARD_ARGS + [curr_task_name, "--internal-previous-portfolio-plans", str(num_previous_plans)] + \
        get_topq_reformulation_planner_args(num_plans_to_read, external_plans_path)

def get_topq_reformulation_planner_args(num_plans_to_read, external_plans_path):
    ## Assuming the file is in the work folder
    ## No search is performed!
    return ["--symmetries",
              "sym=structural_symmetries(time_bound=0,search_symmetries=dks, \
               stabilize_initial_state=true, keep_operator_symmetries=true)",
               "--search", "forbid_iterative(reformulate = FORBID_MULTIPLE_PLAN_MULTISETS, \
               extend_plans_with_symmetry=sym, dump=false, change_operator_names=false, \
               number_of_plans_to_read=%s, external_plans_path=%s)" % (num_plans_to_read, external_plans_path)]


def get_diverse_reformulation_callstring(curr_task_name, num_plans_needed, external_plans_path, num_plans_to_read, num_previous_plans):
    base_dir = get_base_dir()
    return [sys.executable, os.path.join(base_dir, 'fast-downward.py')] + \
        FAST_DOWNWARD_ARGS + [curr_task_name, "--internal-previous-portfolio-plans", str(num_previous_plans)] + \
        get_diverse_reformulation_planner_args(num_plans_to_read, num_plans_needed, external_plans_path)

def get_diverse_reformulation_planner_args(num_plans_to_read, num_plans_needed, external_plans_path):
    ## Assuming the file is in the work folder
    ## No search is performed!
    return ["--symmetries",
              "sym=structural_symmetries(time_bound=0,search_symmetries=dks, \
               stabilize_initial_state=true, keep_operator_symmetries=true)",
               "--search", "forbid_iterative(reformulate = FORBID_SINGLE_PLAN_MULTISET, \
               extend_plans_with_symmetry=sym, dump=false, change_operator_names=false, number_of_plans=%s, \
               number_of_plans_to_read=%s, external_plans_path=%s)" % (num_plans_needed, num_plans_to_read, external_plans_path)]


def get_external_optimal_planner_callstring(domain_file, problem_file, curr_task_name=None, num_previous_plans=0):
    ## For the first iteration we use domain and problem, and for the next iterations we use the sas file in input_file
    base_dir = get_external_optimal_planner_path()
    INPUT_ARGS = ["--internal-previous-portfolio-plans", str(num_previous_plans)]
    if curr_task_name is None:       
        # First iteration
        INPUT_ARGS = [os.path.abspath(domain_file), os.path.abspath(problem_file)] + INPUT_ARGS
    else:
        # Next iterations
        INPUT_ARGS = [curr_task_name] + INPUT_ARGS
        
    #return [sys.executable, os.path.join(base_dir, 'fast-downward.py'), "--keep-sas-file"] + FAST_DOWNWARD_ARGS + INPUT_ARGS + get_external_optimal_planner_args()
    return [sys.executable, os.path.join(base_dir, 'fast-downward.py')] + FAST_DOWNWARD_ARGS + INPUT_ARGS + get_external_optimal_planner_args()


def get_external_optimal_planner_args():
    # OSS with celmcut
    #return ["--search", "astar(blind())"]
    return ["--symmetries",
              "sym=structural_symmetries(time_bound=0,search_symmetries=oss, \
               stabilize_initial_state=false, keep_operator_symmetries=false)",
            "--search",
            "astar(celmcut(), symmetries=sym)"]



def get_external_satisficing_planner_callstring(domain_file, problem_file, curr_task_name=None, num_previous_plans=0):
    ## For the first iteration we use domain and problem, and for the next iterations we use the sas file in input_file
    base_dir = get_external_satisficing_planner_path()
    INPUT_ARGS = ["--internal-previous-portfolio-plans", str(num_previous_plans)]
    if curr_task_name is None:       
        # First iteration
        INPUT_ARGS = [os.path.abspath(domain_file), os.path.abspath(problem_file)] + INPUT_ARGS
    else:
        # Next iterations
        INPUT_ARGS = [curr_task_name] + INPUT_ARGS
        
    return [sys.executable, os.path.join(base_dir, 'fast-downward.py'), "--keep-sas-file"] + INPUT_ARGS + get_external_satisficing_planner_args()

def get_external_satisficing_planner_args():
    return _get_cerberus_first(pref="true", dag="from_coloring")
    """
    return ["--heuristic", "hrb=RB(dag=from_coloring, extract_plan=true)", 
            "--heuristic", "hn=novelty(eval=hrb)",  
            "--heuristic", "hlm=lmcount(lm_rhw(reasonable_orders=true,lm_cost_type=ONE))", 
            "--search", "lazy(open=alt([tiebreaking([hn, hrb]), single(hrb,pref_only=true), single(hlm), single(hlm,pref_only=true)], boost=1000), preferred=[hrb,hlm])"]
    """

def _get_cerberus_first(**kwargs):
    return [
        "--if-unit-cost",
        "--evaluator",
        "hlm=lmcount(lm_rhw(reasonable_orders=true),pref={pref})".format(**kwargs),
        "--evaluator", 
        "hrb=RB(dag={dag}, extract_plan=true)".format(**kwargs),
        "--evaluator", 
        "hn=novelty(eval=hrb)",        
        "--search", """lazy(open=alt([tiebreaking([hn, hrb]), single(hrb,pref_only=true), single(hlm), single(hlm,pref_only=true)], boost=1000),preferred=[hrb,hlm])""",
        "--if-non-unit-cost",
        "--evaluator",
        "hlm=lmcount(lm_rhw(reasonable_orders=true),transform=adapt_costs(one),pref={pref})".format(**kwargs),
        "--evaluator", 
        "hrb=RB(dag={dag}, extract_plan=true, transform=adapt_costs(one))".format(**kwargs),
        "--evaluator", 
        "hn=novelty(eval=hrb)",        
        "--search", """lazy(open=alt([tiebreaking([hn, hrb]), single(hrb,pref_only=true), single(hlm), single(hlm,pref_only=true)], boost=1000), preferred=[hrb,hlm],
                                     cost_type=one,reopen_closed=false)""",
        "--always"]
        # Append --always to be on the safe side if we want to append
        # additional options later.


def get_topk_reformulation_callstring(args, curr_task_name, external_plan_file, num_previous_plans, num_remaining_plans):
    base_dir = get_base_dir()
    return [sys.executable, os.path.join(base_dir, 'fast-downward.py')] + \
        FAST_DOWNWARD_ARGS + [curr_task_name, "--internal-previous-portfolio-plans", str(num_previous_plans)] + \
        get_topk_reformulation_planner_args(args.reordering, num_remaining_plans, external_plan_file)

def get_topk_reformulation_planner_args(reordering, num_remaining_plans, external_plan_file):
    ## Assuming the file is in the work folder
    ## No search is performed!
    
    return ["--symmetries",
              "sym=structural_symmetries(time_bound=0,search_symmetries=dks, \
               stabilize_initial_state=true, keep_operator_symmetries=true)",
               "--search", "forbid_iterative(reformulate = FORBID_MULTIPLE_PLANS, \
               extend_plans_with_symmetry=sym,  reduce_plan_orders=%s, dump=false, \
               number_of_plans=%s,  change_operator_names=true,\
               external_plan_file=%s)" % (reordering, num_remaining_plans, external_plan_file)]


def get_additional_plans_callstring(args, curr_task_name, external_plan_file, num_previous_plans, num_remaining_plans, use_symmetries):
    base_dir = get_base_dir()
    return [sys.executable, os.path.join(base_dir, 'fast-downward.py')] + \
        FAST_DOWNWARD_ARGS + [curr_task_name, "--internal-previous-portfolio-plans", str(num_previous_plans)] + \
        get_additional_plans_planner_args(args.reordering, num_remaining_plans, external_plan_file, use_symmetries)

def get_additional_plans_planner_args(reordering, num_remaining_plans, external_plan_file, use_symmetries):
    ## Assuming the file is in the work folder
    ## No search is performed!
    if use_symmetries:
        return ["--symmetries",
              "sym=structural_symmetries(time_bound=0,search_symmetries=dks, \
               stabilize_initial_state=true, keep_operator_symmetries=true)",
               "--search", "forbid_iterative(reformulate = NONE_FIND_ADDITIONAL_PLANS, \
               extend_plans_with_symmetry=sym,  reduce_plan_orders=%s, dump=false, \
               number_of_plans=%s,  change_operator_names=true,\
               external_plan_file=%s)" % (reordering, num_remaining_plans, external_plan_file)]
    else:
        return ["--search", "forbid_iterative(reformulate = NONE_FIND_ADDITIONAL_PLANS, \
               reduce_plan_orders=%s, dump=false, number_of_plans=%s,  change_operator_names=true,\
               external_plan_file=%s)" % (reordering, num_remaining_plans, external_plan_file)]


def get_plans_to_json_callstring(domain_file, problem_file, plans_path, num_plans, results_file):
    base_dir = get_base_dir()
    return [sys.executable, os.path.join(base_dir, 'fast-downward.py')] \
            + FAST_DOWNWARD_ARGS + [os.path.abspath(domain_file), \
            os.path.abspath(problem_file), "--search", \
            "forbid_iterative(number_of_plans_to_read=%s, external_plans_path=%s, \
            dump_causal_links_json=false, dump_states_json=true, \
            json_file_to_dump=%s)" % (num_plans, plans_path, results_file)] 