#! /usr/bin/env python

import os, shutil, glob
import logging
import timers

import planner_call
from . import plan_manager as pm 

class Planner(object): 
    def __init__(self, args):
        self._iterationStep = 1
        self._timer = timers.Timer()
        self._args = args
        dest = os.path.join(os.getcwd(), self._get_found_plans_dir())
        if os.path.exists(dest):
            shutil.rmtree(dest)

    def _report_iteration_step(self, num_plans_found, success):
        report_msg = "done"
        if not success:
            report_msg = "terminated unexpectedly"
            
        logging.info("Iteration step %s is %s, found %s plans, time %s" % (self._iterationStep, report_msg, num_plans_found, self._timer))
        self._iterationStep += 1

    def _get_external_planner_callstring(self, func, task_manager, plan_manager, time_limit):
        logging.info("Iteration step %s, time limit %s" % (self._iterationStep , time_limit))

        logging.info("Running external planner to get a plan")
        name = task_manager.get_current_task_path()
        num_previous_plans = plan_manager.get_plan_counter()
        command = func(self._args.domain, self._args.problem, curr_task_name=name, num_previous_plans=num_previous_plans)
        logging.debug("Running " + str(command))
        return command

    def cleanup(self, plan_manager):
        if self._args.use_local_folder and self._args.clean_local_folder:
            local_folder = plan_manager.get_plans_folder()
            logging.info("Removing local folder %s" % local_folder)
            if os.path.exists(local_folder):
                shutil.rmtree(local_folder)

    def _finalize(self, folder):
        if self._args.plans_as_json:
            plans = glob.glob(os.path.join(folder, "sas_plan*"))
            logging.info("Dumping plans to json: %s plans" % len(plans))
            command = planner_call.get_plans_to_json_callstring(self._args.domain, self._args.problem, folder, len(plans), self._args.results_file)
            try:
                planner_call.make_call(command, None, os.getcwd(), enable_output=False)
            except:
                raise

    def report_done(self):
        logging.info("All iterations are done %s" % self._timer)

    def report_done_external_planner_run(self):
        logging.info("DONE Running external planner %s" % self._timer)

    def report_done_reformulation_run(self):
        logging.info("DONE Reformulating %s" % self._timer)

    def _get_found_plans_dir(self):
        FOUND_PLANS_DIR = "found_plans"
        return FOUND_PLANS_DIR

    def _get_done_plans_dir(self):
        DONE_PLANS_DIR = "done"
        return os.path.join(self._get_found_plans_dir(), DONE_PLANS_DIR)


class TopKPlanner(Planner):
    def get_plan_manager(self, local_folder):
        return pm.PlanManager("sas_plan", local_folder, compute_best_known=True)

    def report_iteration_step(self, plan_manager, success):
        self._report_iteration_step(plan_manager.get_plan_counter_upto_best_known_bound(), success)

    def get_external_planner_callstring(self, task_manager, plan_manager, time_limit):
        return self._get_external_planner_callstring(planner_call.get_external_optimal_planner_callstring, task_manager, plan_manager, time_limit)

    def get_reformulation_callstring(self, task_manager, plan_manager):
        plan_file = plan_manager.get_last_processed_plan()
        if plan_file is None:
            logging.info("Plan manages has no plans")
            exit(1) 

        if not os.path.exists(plan_file):
            logging.info("File %s is not found" % plan_file)
            exit(1) 

        plan_manager.forget_last_processed_plan()
        num_previous_plans = plan_manager.get_plan_counter() 
        num_remaining_plans = self._args.number_of_plans - plan_manager.get_plan_counter_upto_best_known_bound()
        current_task = task_manager.get_current_task_path()
        command = planner_call.get_topk_reformulation_callstring(self._args, current_task, plan_file, num_previous_plans, num_remaining_plans)
        logging.info("Reformulating the planning task, forbidding found solutions")
        logging.debug("Running " + str(command))
        return command

    def enough_plans_found(self, plan_manager):
        return plan_manager.get_plan_counter_upto_best_known_bound() >= self._args.number_of_plans

    def report_number_of_plans(self, plan_manager):
        plan_manager.report_number_of_plans(best_plans=True)

    def finalize(self, plan_manager):
        plan_manager.map_plans_back()
        dest = os.path.join(os.getcwd(), self._get_done_plans_dir())
        if self._args.use_local_folder:
            ## Copying best plans to current work dir
            logging.info("Copying back to current work directory")
            if not os.path.exists(dest):
                os.makedirs(dest)
            for plan in plan_manager.get_best_local_plans():
                logging.debug("copying %s to %s" % (plan, dest))
                shutil.copy2(plan, dest)

        self._finalize(dest)


class DiversePlanner(Planner):
    def get_plan_manager(self, local_folder):
        return pm.PlanManager("sas_plan", local_folder, compute_best_known=False)

    def report_iteration_step(self, plan_manager, success):
        self._report_iteration_step(plan_manager.get_plan_counter(), success)

    def get_external_planner_callstring(self, task_manager, plan_manager, time_limit):
        return self._get_external_planner_callstring(planner_call.get_external_satisficing_planner_callstring, task_manager, plan_manager, time_limit)

    def get_reformulation_callstring(self, task_manager, plan_manager):
        orig_task_name = task_manager.get_original_task_path()
        input_plans_folder_name = plan_manager.get_plans_folder()
        num_plans_found = plan_manager.get_plan_counter() 

        command = planner_call.get_diverse_reformulation_callstring(curr_task_name=orig_task_name, num_plans_needed=self._args.number_of_plans, external_plans_path=input_plans_folder_name, num_plans_to_read=num_plans_found, num_previous_plans=0)
        logging.info("Reformulating the planning task, forbidding found solutions")
        logging.debug("Running " + str(command))
        return command

    def enough_plans_found(self, plan_manager):
        return plan_manager.get_plan_counter() >= self._args.number_of_plans

    def report_number_of_plans(self, plan_manager):
        plan_manager.report_number_of_plans(best_plans=False)

    def finalize(self, plan_manager):
        dest = os.path.join(os.getcwd(), self._get_found_plans_dir())
        if self._args.use_local_folder:
            ## Copying best plans to current work dir
            logging.info("Copying back to current work directory")
            if not os.path.exists(dest):
                os.makedirs(dest)
            for plan in plan_manager.get_local_plans():
                logging.debug("copying %s to %s" % (plan, dest))
                shutil.copy2(plan, dest)
        self._finalize(dest)


class TopQPlanner(Planner):
    def get_plan_manager(self, local_folder):
        return pm.PlanManager("sas_plan", local_folder, compute_best_known=True)

    def report_iteration_step(self, plan_manager, success):
        assert(plan_manager.get_plan_counter() == plan_manager.get_plan_counter_upto_best_known_bound())
        self._report_iteration_step(plan_manager.get_plan_counter(), success)

    def get_external_planner_callstring(self, task_manager, plan_manager, time_limit):
        return self._get_external_planner_callstring(planner_call.get_external_optimal_planner_callstring, task_manager, plan_manager, time_limit)

    def get_reformulation_callstring(self, task_manager, plan_manager):
        orig_task_name = task_manager.get_original_task_path()
        input_plans_folder_name = plan_manager.get_plans_folder()
        num_plans_found = plan_manager.get_plan_counter() 
 
        command = planner_call.get_topq_reformulation_callstring(curr_task_name=orig_task_name, external_plans_path=input_plans_folder_name, num_plans_to_read=num_plans_found, num_previous_plans=0)
        logging.info("Reformulating the planning task, forbidding found solutions")
        logging.debug("Running " + str(command))
        return command

    def enough_plans_found(self, plan_manager):
        max_cost = plan_manager.get_highest_plan_cost()
        min_cost = plan_manager.get_best_plan_cost()
        cost_bound = self._args.quality_bound * min_cost
        return max_cost > cost_bound

    def report_number_of_plans(self, plan_manager):
        plan_manager.report_number_of_plans(best_plans=False)

    def finalize(self, plan_manager):
        dest = os.path.join(os.getcwd(), self._get_done_plans_dir())
        if self._args.use_local_folder:
            logging.info("Copying back to current work directory")
            ## Copying best plans to current work dir
            # Last plan is above the bound
            last_plan = plan_manager.get_last_processed_plan()
            dest_last_plan = os.path.join(os.getcwd(), self._get_found_plans_dir())
            if not os.path.exists(dest_last_plan):
                os.makedirs(dest_last_plan)
            logging.debug("copying %s to %s" % (last_plan, dest_last_plan))
            shutil.copy2(last_plan, dest_last_plan)
            plan_manager.forget_last_processed_plan()

            if not os.path.exists(dest):
                os.makedirs(dest)
            for plan in plan_manager.get_local_plans():
                logging.debug("copying %s to %s" % (plan, dest))
                shutil.copy2(plan, dest)
        self._finalize(dest)



class TopKQPlanner(Planner):
    def get_plan_manager(self, local_folder):
        return pm.PlanManager("sas_plan", local_folder, compute_best_known=True)

    def report_iteration_step(self, plan_manager, success):
        self._report_iteration_step(plan_manager.get_plan_counter_upto_best_known_bound(), success)

    def get_external_planner_callstring(self, task_manager, plan_manager, time_limit):
        return self._get_external_planner_callstring(planner_call.get_external_optimal_planner_callstring, task_manager, plan_manager, time_limit)

    def get_reformulation_callstring(self, task_manager, plan_manager):
        plan_file = plan_manager.get_last_processed_plan()
        if plan_file is None:
            logging.info("Plan manages has no plans")
            exit(1) 

        if not os.path.exists(plan_file):
            logging.info("File %s is not found" % plan_file)
            exit(1) 

        plan_manager.forget_last_processed_plan()
        num_previous_plans = plan_manager.get_plan_counter() 
        num_remaining_plans = 1000000
        current_task = task_manager.get_current_task_path()
        command = planner_call.get_topk_reformulation_callstring(self._args, current_task, plan_file, num_previous_plans, num_remaining_plans)
        logging.info("Reformulating the planning task, forbidding found solutions")
        logging.debug("Running " + str(command))
        return command


    def enough_plans_found(self, plan_manager):
        max_cost = plan_manager._best_known_bound
        min_cost = plan_manager.get_best_plan_cost()
        cost_bound = self._args.quality_bound * min_cost
        return max_cost > cost_bound

    def report_number_of_plans(self, plan_manager):
        plan_manager.report_number_of_plans(best_plans=True)

    def finalize(self, plan_manager):
        plan_manager.map_plans_back()
        dest = os.path.join(os.getcwd(), self._get_done_plans_dir())
        if not os.path.exists(dest):
            os.makedirs(dest)
        if self._args.use_local_folder:
            logging.info("Copying back to current work directory")
            ## Copying best plans to current work dir
            # Get plan(s) above the bound
            dest_prove_plan = os.path.join(os.getcwd(), self._get_found_plans_dir())
            if not os.path.exists(dest_prove_plan):
                os.makedirs(dest_prove_plan)
            for plan in plan_manager.get_local_plans_for_cost(plan_manager._best_known_bound):
                logging.debug("copying %s to %s" % (plan, dest_prove_plan))
                shutil.copy2(plan, dest_prove_plan)
            # Get plans below bound
            for plan in plan_manager.get_local_plans_upto_bound(plan_manager._best_known_bound - 1):
                logging.debug("copying %s to %s" % (plan, dest))
                shutil.copy2(plan, dest)
        self._finalize(dest)
