#! /usr/bin/env python3

import argparse

import os
import sys
from pathlib import Path
# from driver import limits, arguments

from subprocess import SubprocessError
import logging

# from forbiditerative import timers
from forbiditerative import copy_plans
from forbiditerative.planner_call import make_call, get_base_dir
from forbiditerative.iterative import planners, task_manager as tm


def get_planner(args):
    ### TODO: Get the actual planner from args
    if args.planner == "topk":
        return planners.TopKPlanner(args)
    if args.planner == "unordered_topq":
        return planners.UnorderedTopQualityPlanner(args)
    if args.planner == "extended_unordered_topq":
        return planners.ExtendedUnorderedTopQualityPlanner(args)
    if args.planner == "topq_via_topk":
        return planners.TopQualityViaTopKPlanner(args)
    if args.planner == "subsets_topq":
        return planners.SubsetTopQualityPlanner(args)
    if args.planner == "submultisets_topq":
        return planners.SubMultisetTopQualityPlanner(args)                
    if args.planner == "topk_via_unordered_topq":
        return planners.TopKViaUnorderedTopQualityPlanner(args)
    if args.planner == "topq_via_unordered_topq":
        return planners.TopQualityViaUnorderedTopQualityPlanner(args)
    if args.planner == "diverse": 
        return planners.DiversePlanner(args)
    ##
    logging.error("Wrong planner definition")

def find_plans(args):
    planner = get_planner(args)
    enable_planners_output = True
    if args.suppress_planners_output:
        enable_planners_output = False

    local_folder = get_base_dir()
    if args.use_local_folder:
        local_folder = copy_plans.create_local_folder(False)

    logging.info("Running in %s" % local_folder)

    plan_manager = planner.get_plan_manager(local_folder)

    plan_manager.delete_existing_plans()
    task_manager = tm.TaskManager("reformulated_output.sas", local_folder, keep_intermediate_tasks=args.keep_intermediate_tasks)

    command = planner.get_planner_callstring(task_manager, plan_manager)
    try:
        make_call(command, planner.get_remaining_time(), local_folder, enable_output=enable_planners_output)
    except SubprocessError as err:
        logging.info("External planner did not produce a plan")
        if planner.check_unsolvable(err.output.decode()):
            logging.info("External planner proved that there are no more plans")

        if enable_planners_output:
            print(err.output)
        planner.report_iteration_step(plan_manager, success=False)
        planner.finalize(plan_manager)
        planner.cleanup(plan_manager)
        planner.report_done()
        # Planner failure might be a normal behavior, when there are no plans
        return
        # raise

    num_plans_processed = planner.process_new_plans(plan_manager)
    planner.report_done_external_planner_run()
    planner.report_number_of_plans(plan_manager)

    if num_plans_processed == 0:
        logging.info("No plans were found")
        planner.finalize(plan_manager)
        planner.cleanup(plan_manager)
        planner.report_done()
        return

    # At this point, we have exactly one plan 
    assert(num_plans_processed == 1)
    if args.number_of_plans == 1:
        planner.report_iteration_step(plan_manager, success=True)
        planner.finalize(plan_manager)
        planner.cleanup(plan_manager)
        planner.report_done()
        return

    ## Keeping the original sas
    if args.sas_file:
        task_manager.copy_task(args.sas_file)
    else:
        task_manager.add_task("output.sas")

    command = planner.get_extend_plans_callstring(task_manager, plan_manager)

    if command is not None:
        ## Adding plans from the just found plan
        try:
            make_call(command, planner.get_remaining_time(), local_folder, enable_output=enable_planners_output)
        except:
            planner.report_iteration_step(plan_manager, success=False)
            planner.finalize(plan_manager)
            planner.cleanup(plan_manager)
            return
        planner.process_new_plans(plan_manager)
        planner.report_done_plans_extension_run()
        planner.report_number_of_plans(plan_manager)
        if planner.enough_plans_found(plan_manager):
            planner.report_iteration_step(plan_manager, success=True)
            planner.finalize(plan_manager)
            planner.cleanup(plan_manager)
            planner.report_done()
            return

    # calling the reformulation
    command = planner.get_reformulation_callstring(task_manager, plan_manager)
    try:
        make_call(command, planner.get_remaining_time(), local_folder, enable_output=enable_planners_output)
    except:
        planner.report_iteration_step(plan_manager, success=False)
        planner.finalize(plan_manager)
        planner.cleanup(plan_manager)
        return

    planner.process_new_plans(plan_manager)
    planner.report_done_reformulation_run()
    planner.report_number_of_plans(plan_manager)

    if not task_manager.add_task("reformulated_output.sas"):
        ## Checking if done
        done = planner.enough_plans_found(plan_manager)
        planner.report_iteration_step(plan_manager, success=done)
        planner.finalize(plan_manager)
        planner.cleanup(plan_manager)
        if done:
            planner.report_done()
        return

    # First iteration is done
    planner.report_iteration_step(plan_manager, success=True)

    while not planner.enough_plans_found(plan_manager):
        command = planner.get_planner_callstring(task_manager, plan_manager)
        try:
            make_call(command, planner.get_remaining_time(), local_folder, enable_output=enable_planners_output)
        except SubprocessError as err:
            logging.info("External planner did not produce a plan")
            if planner.check_unsolvable(err.output.decode()):
                logging.info("External planner proved that there are no more plans")
            if enable_planners_output:
                print(err.output)
            planner.report_iteration_step(plan_manager, success=False)
            planner.finalize(plan_manager)
            planner.cleanup(plan_manager)
            planner.report_done()

            # Planner failure might be a normal behavior, when there are no plans
            return
            # raise

        num_plans_processed = planner.process_new_plans(plan_manager)
        planner.report_done_external_planner_run()
        planner.report_number_of_plans(plan_manager)

        if num_plans_processed == 0:
            logging.info("Iteration terminated, no plans were found, exiting")
            break

        assert(num_plans_processed == 1)

        if planner.enough_plans_found(plan_manager):
            logging.info("Found all needed plans")
            planner.report_iteration_step(plan_manager, success=True)
            break

        #  Cleaning up auxillary actions from plans for the reformulation
        planner.cleanup_plans(plan_manager)

        command = planner.get_extend_plans_callstring(task_manager, plan_manager)
        if command is not None:
            ## Adding plans from the just found plan
            try:
                make_call(command, planner.get_remaining_time(), local_folder, enable_output=enable_planners_output)
            except:
                planner.report_iteration_step(plan_manager, success=False)
                planner.finalize(plan_manager)
                planner.cleanup(plan_manager)
                return
            planner.process_new_plans(plan_manager)
            planner.report_done_plans_extension_run()
            planner.report_number_of_plans(plan_manager)
            if planner.enough_plans_found(plan_manager):
                planner.report_iteration_step(plan_manager, success=True)
                planner.finalize(plan_manager)
                planner.cleanup(plan_manager)
                planner.report_done()
                return

        # calling the reformulation
        command = planner.get_reformulation_callstring(task_manager, plan_manager)
        try:
            make_call(command, planner.get_remaining_time(), local_folder, enable_output=enable_planners_output)
        except:
            planner.report_iteration_step(plan_manager, success=False)
            planner.finalize(plan_manager)
            planner.cleanup(plan_manager)
            return

        planner.process_new_plans(plan_manager)
        planner.report_number_of_plans(plan_manager)
        planner.report_done_reformulation_run()

        if not task_manager.add_task("reformulated_output.sas"):
            # This can happen if all plans are found
            logging.info("Could not add the reformulated task")
            break

        ## Current iteration step is done, going to the next one
        planner.report_iteration_step(plan_manager, success=True)


    planner.finalize(plan_manager)
    planner.cleanup(plan_manager)
    planner.report_done()


def validate_input_planning_task(args):
    if not args.sas_file and (not args.domain or not args.problem):
        logging.error("Input planning task must be specified either via --domain DOMAIN --problem PROBLEM or via --sas-file SASFILE")
        exit(1)

    if args.sas_file and (args.domain or args.problem):
        logging.error("Only one of [--domain DOMAIN --problem PROBLEM] or [--sas-file SASFILE] can be used")
        exit(1)

def validate_input(args):
    if args.conditional_effects and (not args.reordering or args.reordering =="NEIGHBOURS_INTERFERE"):
        logging.error("Cannot use NEIGHBOURS_INTERFERE reordering with conditional effects")
        exit(1)

    if not args.planner:
        logging.error("Required parameters --planner PLANNER (topk, unordered_topq, extended_unordered_topq, topq_via_topk, topk_via_unordered_topq, topq_via_unordered_topq, or diverse)")
        exit(1)

    if args.planner in ["topk", "topk_via_unordered_topq", "diverse", "extended_unordered_topq"] and not args.number_of_plans:
        logging.error("Required parameter --number-of-plans NUMBER_OF_PLANS")
        exit(1)
    if args.planner in ["unordered_topq", "subsets_topq", "submultisets_topq",  "extended_unordered_topq", "topq_via_topk", "topq_via_unordered_topq"] and not args.quality_bound:
        logging.error("Required parameter --quality-bound QUALITY_BOUND")
        exit(1)

    validate_input_planning_task(args)

    ## TODO: Check if needed
    valid_reduce_order_methods = ["NONE", "NEIGHBOURS_INTERFERE", "NAIVE_ALL_DFS_NODUP"]
    ## Check that the parameter obtains one of the valid values (NONE for default)
    if args.reordering not in valid_reduce_order_methods:
        logging.error("Valid plan reordering methods are " + " ".join(valid_reduce_order_methods))
        exit(1)

    if args.clean_local_folder and args.keep_intermediate_tasks:  
        logging.error("Cannot use --clean-local-folder with --keep-intermediate-tasks")
        exit(1)

def set_default_build_path():
    """ 
    Operates directly on sys.argv and sets the default value of --build. Prioritizes builds in this order:
    1) Any provided --build path
    2) <repo>/builds/BUILD/bin if it exists
    3) <site-packages>/forbiditeritive/builds/BUILD/bin if it exists (the package has been installed as a library)

    NOTE: Operating directly on sys.argv is generally considered bad practice but we need to modify the build arg
    before it reaches the upstream driver code, and are unable to pass in a copy to driver. 
    """

    logging.info("Setting default build path if one does not exist")

    if "--build" in sys.argv:
        #A build path has been explicitly provided by the end user, don't modify anything
        logging.info("A build path has been explicitly provided")
        return 
    
    forbiditerative_path = Path(__file__).parent 
    regular_build_path = forbiditerative_path.parent / 'builds' / 'release' / 'bin'    
    if os.path.exists(regular_build_path):
        logging.info(f"Local build found under {str(regular_build_path)}")
        sys.argv.append("--build")
        sys.argv.append(str(regular_build_path))
        return
    
    package_build_path = forbiditerative_path / 'builds' / 'release' / 'bin'    
    if os.path.exists(package_build_path):
        logging.info(f"Package build found under {str(package_build_path)}")
        sys.argv.append("--build")
        sys.argv.append(str(package_build_path))
        return
    
    logging.error(f"""
    No binary directory was found at the default search locations: {str(package_build_path)},
    {str(regular_build_path)}, or provided via the --build flag.
    If you are building locally from source, run build.py in the main folder.
    If you are installing as a python package from local source, run "pip install ." in the main folder
    or if installing from the remote source "pip install git+https://github.com/IBM/forbiditerative.git" 
    If you have manually installed the binaries in a separate location please specify a path to them with the --build argument.
    Note: when installing locally with "pip install -e .", a --build flag must be provided to the binaries.  
    """)
    exit(1)

if __name__ == "__main__":
    set_default_build_path()
    parser = argparse.ArgumentParser(
        add_help=False)
    lim = parser.add_argument_group(
        title="time and memory limits")
    lim.add_argument("--overall-time-limit")
    lim.add_argument("--overall-memory-limit")

    parser.add_argument("--build", help="BUILD can be a predefined build name like release "
                                        "(default) and debug, a custom build name, or the path to "
                                        "a directory holding the planner binaries. The driver "
                                        "first looks for the planner binaries under 'BUILD'. If "
                                        "this path does not exist, it tries the directory "
                                        "'<repo>/builds/BUILD/bin', where the build script creates "
                                        "them by default.")
    parser.add_argument("--planner", help="The type of planner", choices=["topk", "topk_via_unordered_topq", "unordered_topq",  "extended_unordered_topq", "topq_via_topk", "topq_via_unordered_topq", "subsets_topq", "submultisets_topq", "diverse"])
    parser.add_argument("--domain", help="PDDL domain file")
    parser.add_argument("--problem", help="PDDL problem file")
    parser.add_argument("--sas-file", help="SAS+ problem file")

    parser.add_argument("--number-of-plans", help="The overall number of plans", type=int)
    parser.add_argument("--quality-bound", help="A relative (to an optimal plan cost) bound on the plans quality (>= 1.0)", type=float)

    parser.add_argument("--symmetries", help="Extend plans with symmetries", action="store_true")
    parser.add_argument("--reordering", help="Extend plans by reordering", choices=["NONE", "NEIGHBOURS_INTERFERE", "NAIVE_ALL_DFS_NODUP"], default="NAIVE_ALL_DFS_NODUP")
    parser.add_argument("--keep-intermediate-tasks", help="Keeps the files for reformulated tasks", action="store_true")
    parser.add_argument("--conditional-effects", help="Configuration that supports conditional effects", action="store_true")

    parser.add_argument("--use-local-folder", help="Use local folder for cwd", action="store_true")
    parser.add_argument("--clean-local-folder", help="Clean the plan files in the local folder after copying", action="store_true")
    parser.add_argument("--plans-as-json", help="Dump plans as a json file", action="store_true")
    parser.add_argument("--results-file", help="File name for dumping plans", default="results.json")

    parser.add_argument("--upper-bound-on-number-of-plans", help="The overall bound on the number of plans", type=int, default=1000000)

    parser.add_argument("--suppress-planners-output", help="Suppress the output of the individual planners", action="store_true")
    parser.add_argument("--case-sensitive", action="store_true", help="treat the input PDDL as case sensitive")
     
    args = parser.parse_args()

    logging.basicConfig(level=logging.INFO, #level=logging.DEBUG,
                        format="%(levelname)-8s %(message)s",
                        stream=sys.stdout)

    validate_input(args)

    # limits.set_time_limit_in_seconds(parser, args, "overall")
    #limits.set_memory_limit_in_bytes(parser, args, "overall")
    
    find_plans(args)
