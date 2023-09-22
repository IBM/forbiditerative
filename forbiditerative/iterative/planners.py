#! /usr/bin/env python

import os, shutil
# import glob
import logging
import math

# from planner_call import BaseCostOptimalPlannerCall, ShortestOptimalPlannerCall, CerberusPlannerCall, TopqReformulationPlannerCall, TopkReformulationPlannerCall, DiverseReformulationPlannerCall, AdditionalPlansPlannerCall, make_call
from forbiditerative import planner_call, timers
from . import plan_manager as pm 

class Planner(object): 
    def __init__(self, args):
        self._iterationStep = 1
        self._elapsed_time = timers.Timer()
        self._args = args
        dest = os.path.join(os.getcwd(), self._get_found_plans_dir())
        if os.path.exists(dest):
            shutil.rmtree(dest)

    def get_remaining_time(self):
        if self._args.overall_time_limit:
            return int(self._args.overall_time_limit) - self._elapsed_time._elapsed_cpu_time()
        else:
            return None

    def _report_iteration_step(self, num_plans_found, success):
        report_msg = "done"
        if not success:
            report_msg = "terminated unexpectedly"
            
        logging.info("Iteration step %s is %s, found %s plans, time %s" % (self._iterationStep, report_msg, num_plans_found, self._elapsed_time))
        self._iterationStep += 1

    def _get_planner_callstring(self, pc, task_manager, plan_manager, **kwargs):
        logging.info("Iteration step %s, time limit %s" % (self._iterationStep , self.get_remaining_time()))
        logging.info("Running external planner to get a plan")

        pcargs = kwargs
        
        if self._args.case_sensitive:
            pcargs["case_sensitive"] = True
        if self._args.build:
            pcargs["build"] = self._args.build
        pcargs["domain_file"] = self._args.domain
        pcargs["problem_file"] = self._args.problem
        pcargs["sas_file"] = self._args.sas_file

        name = task_manager.get_current_task_path()
        if name is not None:
            pcargs["curr_task_name"] = name
        pcargs["num_previous_plans"] = plan_manager.get_plan_counter()

        # Adding a bound on the absolute cost, if available
        cost_bound = self._get_absolute_cost_bound(plan_manager)
        if cost_bound:
            pcargs["cost_bound"] = cost_bound

        command = pc.get_callstring(**pcargs)
        logging.debug("Running " + str(command))
        return command

    def _get_absolute_cost_bound(self, plan_manager):
        if not self._args.quality_bound or not plan_manager._best_known_bound:
            return None
        
        return math.floor(self._args.quality_bound * plan_manager.get_best_plan_cost()) + 1

    def process_new_plans(self, plan_manager):
        return plan_manager.process_new_plans()

    def cleanup_plans(self, plan_manager):
        logging.info("Cleaning up plans %s" % self._elapsed_time)
        plan_manager.remove_aux_actions()
        logging.info("DONE Cleaning up plans %s" % self._elapsed_time)
        
    def cleanup(self, plan_manager):
        if self._args.use_local_folder and self._args.clean_local_folder:
            local_folder = plan_manager.get_plans_folder()
            logging.info("Removing local folder %s" % local_folder)
            if os.path.exists(local_folder):
                shutil.rmtree(local_folder)

    def _finalize(self, plan_manager, folder):
        if self._args.plans_as_json:
            plan_manager.plans_to_json(self._args.results_file)
            """
            ## The following part only needed if we need to generate states along the plan.
            plans = glob.glob(os.path.join(folder, "sas_plan*"))
            logging.info("Dumping plans to json: %s plans" % len(plans))
            command = planner_call.get_plans_to_json_callstring(self._args.domain, self._args.problem, folder, len(plans), self._args.results_file)
            try:
                planner_call.make_call(command, None, os.getcwd(), enable_output=False)
            except:
                raise
            """

    def report_done(self):
        logging.info("All iterations are done %s" % self._elapsed_time)

    def report_done_external_planner_run(self):
        logging.info("DONE Running external planner %s" % self._elapsed_time)

    def report_done_reformulation_run(self):
        logging.info("DONE Reformulating %s" % self._elapsed_time)

    def _get_found_plans_dir(self):
        FOUND_PLANS_DIR = "found_plans"
        return FOUND_PLANS_DIR

    def _get_done_plans_dir(self):
        DONE_PLANS_DIR = "done"
        return os.path.join(self._get_found_plans_dir(), DONE_PLANS_DIR)


    def get_extend_plans_callstring(self,task_manager, plan_manager):
        return None

    def _enough_plans_found_number(self, plan_manager, up_to_best_known_bound):

        return plan_manager.get_number_valid_plans(up_to_best_known_bound) >= self._args.number_of_plans

    def _enough_plans_found_quality(self, plan_manager):
        max_cost = plan_manager.get_highest_plan_cost()
        min_cost = plan_manager.get_best_plan_cost()
        cost_bound = self._args.quality_bound * min_cost
        return max_cost > cost_bound

    def _enough_plans_found(self, plan_manager, up_to_best_known_bound):
        if self._args.number_of_plans:
            return self._enough_plans_found_number(plan_manager, up_to_best_known_bound)

        return self._enough_plans_found_quality(plan_manager)

    def check_unsolvable(self, planner_output):
        # print("Checking for strings in output")
        # if "NO SOLUTION: Bound was reached" not in planner_output:
        #     print(planner_output)
        # print("Done checking")
        return ("Completely explored state space -- no solution" in planner_output) or ("NO SOLUTION: Bound was reached" in planner_output)
            

    def _get_default_reformulation_callstring(self, pc, task_manager, plan_manager):
        pcargs = {}
        num_existing_plans = plan_manager.get_plan_counter()
        pcargs["curr_task_name"] = task_manager.get_original_task_path()
        pcargs["external_plans_path"] = plan_manager.get_plans_folder()
        pcargs["num_plans_to_read"] = num_existing_plans
        pcargs["num_previous_plans"] = 0


        if self._args.build:
            pcargs["build"] = self._args.build

        if self._args.number_of_plans:
            pcargs["num_total_plans"] = self._args.number_of_plans

        if self._args.symmetries:
            pcargs["use_symmetries"] = True

        command = pc.get_callstring(**pcargs)
        logging.info("Reformulating the planning task, forbidding found solutions")
        logging.debug("Running " + str(command))
        return command

###################################################################################################
#####  Top-k planners

class TopKPlanner(Planner):
    def get_plan_manager(self, local_folder):
        return pm.PlanManager("sas_plan", local_folder, compute_best_known=True)

    def report_iteration_step(self, plan_manager, success):
        self._report_iteration_step(plan_manager.get_number_valid_plans(up_to_best_known_bound=True), success)

    def get_planner_callstring(self, task_manager, plan_manager):
        return self._get_planner_callstring(planner_call.BaseCostOptimalPlannerCall(), task_manager, plan_manager, shortest=False, consistent=False)

    def get_reformulation_callstring(self, task_manager, plan_manager):
        plan_file = plan_manager.get_last_processed_plan()
        if plan_file is None:
            logging.info("Plan manager has no plans")
            exit(1) 

        if not os.path.exists(plan_file):
            logging.info("File %s is not found" % plan_file)
            exit(1) 

        plan_manager.forget_last_processed_plan()
        num_previous_plans = plan_manager.get_plan_counter() 
        num_remaining_plans = self._args.number_of_plans - plan_manager.get_number_valid_plans(up_to_best_known_bound=True)

        pcargs = {}
        pcargs["curr_task_name"] = task_manager.get_current_task_path()
        pcargs["external_plan_file"] = plan_file
        pcargs["num_previous_plans"] = num_previous_plans
        pcargs["num_total_plans"] = num_remaining_plans

        if self._args.build:
            pcargs["build"] = self._args.build

        pcargs["reordering"] = self._args.reordering
        pc = planner_call.TopkReformulationPlannerCall()
        command = pc.get_callstring(**pcargs)

        logging.info("Reformulating the planning task, forbidding found solutions")
        logging.debug("Running " + str(command))
        return command

    def cleanup_plans(self, plan_manager):
        logging.info("Not cleaning up plans between iterations %s" % self._elapsed_time)

    def report_number_of_plans(self, plan_manager):
        plan_manager.report_number_of_plans(best_plans=False)

    def finalize(self, plan_manager):
        plan_manager.remove_aux_actions()
        dest = os.path.join(os.getcwd(), self._get_done_plans_dir())
        if self._args.use_local_folder:
            ## Copying best plans to current work dir
            logging.info("Copying back to current work directory")
            if not os.path.exists(dest):
                os.makedirs(dest)
            for plan in plan_manager.get_best_local_plans():
                logging.debug("copying %s to %s" % (plan, dest))
                shutil.copy2(plan, dest)

        self._finalize(plan_manager, dest)

    def enough_plans_found(self, plan_manager):
        return self._enough_plans_found(plan_manager, up_to_best_known_bound=True)


class TopKViaUnorderedTopQualityPlanner(Planner):
    def get_plan_manager(self, local_folder):
        return pm.PlanManager("sas_plan", local_folder, compute_best_known=True)

    def report_iteration_step(self, plan_manager, success):
        self._report_iteration_step(plan_manager.get_number_valid_plans(up_to_best_known_bound=True), success)

    def get_planner_callstring(self, task_manager, plan_manager):
        return self._get_planner_callstring(planner_call.ShortestOptimalPlannerCall(), task_manager, plan_manager, shortest=True, consistent=False)

    def get_reformulation_callstring(self, task_manager, plan_manager):
        pcargs = {}
        pcargs["curr_task_name"] = task_manager.get_original_task_path()
        pcargs["external_plans_path"] = plan_manager.get_plans_folder()
        pcargs["num_plans_to_read"] = plan_manager.get_plan_counter() 
        pcargs["num_previous_plans"] = 0

        if self._args.build:
            pcargs["build"] = self._args.build

        pc = planner_call.TopqReformulationPlannerCall()
        command = pc.get_callstring(**pcargs)

        logging.info("Reformulating the planning task, forbidding found solutions")

        logging.debug("Number of plans to forbid: %s, number of plans up to the best known bound: %s" % (plan_manager.get_plan_counter(), plan_manager.get_number_valid_plans(up_to_best_known_bound=True)) )


        logging.debug("Running " + str(command))
        return command

    def report_number_of_plans(self, plan_manager):
        plan_manager.report_number_of_plans(best_plans=False)

    def finalize(self, plan_manager):
        plan_manager.remove_aux_actions()
        dest = os.path.join(os.getcwd(), self._get_done_plans_dir())
        if self._args.use_local_folder:
            ## Copying best plans to current work dir
            logging.info("Copying back to current work directory")
            if not os.path.exists(dest):
                os.makedirs(dest)
            for plan in plan_manager.get_best_local_plans():
                logging.debug("copying %s to %s" % (plan, dest))
                shutil.copy2(plan, dest)

        self._finalize(plan_manager, dest)

    def get_extend_plans_callstring(self,task_manager, plan_manager):
        plan_file = plan_manager.get_last_processed_plan()
        if plan_file is None:
            logging.info("Plan manages has no plans")
            exit(1)

        if not os.path.exists(plan_file):
            logging.info("File %s is not found" % plan_file)
            exit(1)

        plan_manager.forget_last_processed_plan()
        num_previous_plans = plan_manager.get_plan_counter()
        num_remaining_plans = self._args.number_of_plans - plan_manager.get_number_valid_plans(up_to_best_known_bound=True)
        task = task_manager.get_original_task_path()

        pcargs = {}
        pcargs["curr_task_name"] = task
        pcargs["external_plan_file"] = plan_file
        pcargs["num_previous_plans"] = num_previous_plans
        pcargs["num_total_plans"] = num_remaining_plans

        if self._args.build:
            pcargs["build"] = self._args.build

        if self._args.symmetries:
            pcargs["use_symmetries"] = True

        pc = planner_call.AdditionalPlansPlannerCall()
        command = pc.get_callstring(**pcargs)

        logging.info("Extending the set of found solutions")
        logging.debug("Running " + str(command))
        return command

    def report_done_plans_extension_run(self):
        logging.info("DONE Extending the set of plans %s" % self._elapsed_time)

    def enough_plans_found(self, plan_manager):
        return self._enough_plans_found(plan_manager, up_to_best_known_bound=True)


###################################################################################################
#####  Top Quality planners

class UnorderedTopQualityPlanner(Planner):
    def get_plan_manager(self, local_folder):
        return pm.PlanManager("sas_plan", local_folder, compute_best_known=True)

    def report_iteration_step(self, plan_manager, success):
        assert(plan_manager.get_plan_counter() == plan_manager.get_number_valid_plans(up_to_best_known_bound=True))
        self._report_iteration_step(plan_manager.get_plan_counter(), success)

    def get_planner_callstring(self, task_manager, plan_manager):
        return self._get_planner_callstring(planner_call.ShortestOptimalPlannerCall(), task_manager, plan_manager, shortest=True, consistent=False)

    def get_reformulation_callstring(self, task_manager, plan_manager):
        pcargs = {}
        pcargs["curr_task_name"] = task_manager.get_original_task_path()
        pcargs["external_plans_path"] = plan_manager.get_plans_folder()
        pcargs["num_plans_to_read"] = plan_manager.get_plan_counter()
        pcargs["num_previous_plans"] = 0

        if self._args.build:
            pcargs["build"] = self._args.build

        if self._args.symmetries:
            pcargs["use_symmetries"] = True

        pc = planner_call.TopqReformulationPlannerCall()
        command = pc.get_callstring(**pcargs)

        logging.info("Reformulating the planning task, forbidding found solutions")
        logging.debug("Running " + str(command))
        return command

    def report_number_of_plans(self, plan_manager):
        plan_manager.report_number_of_plans(best_plans=True)

    def finalize(self, plan_manager):
        plan_manager.remove_aux_actions()
        dest = os.path.join(os.getcwd(), self._get_done_plans_dir())
        if self._args.use_local_folder:
            logging.info("Copying back to current work directory")
            ## Copying best plans to current work dir
            # If done, either last plan is above the bound or proved that there are no better plans
            last_plan = plan_manager.get_last_processed_plan()
            if last_plan is None:
                logging.info("No plans to copy")
                self._finalize(plan_manager, dest)
                return

            dest_last_plan = os.path.join(os.getcwd(), self._get_found_plans_dir())
            if not os.path.exists(dest_last_plan):
                os.makedirs(dest_last_plan)

            ## If last plan is above the bound, copying it separately
            if self._enough_plans_found_quality(plan_manager):
                logging.info("All plans up to requested quality are found")
                logging.debug("copying %s to %s" % (last_plan, dest_last_plan))
                shutil.copy2(last_plan, dest_last_plan)
                plan_manager.forget_last_processed_plan()

            if not os.path.exists(dest):
                os.makedirs(dest)
            for plan in plan_manager.get_local_plans():
                logging.debug("copying %s to %s" % (plan, dest))
                shutil.copy2(plan, dest)
        self._finalize(plan_manager, dest)

    def enough_plans_found(self, plan_manager):
        # Allowing to specify the number of plans bound as an additional constraint
        if self._args.number_of_plans and self._enough_plans_found_number(plan_manager, up_to_best_known_bound=True):
            return True
        return self._enough_plans_found_quality(plan_manager)

class ExtendedUnorderedTopQualityPlanner(UnorderedTopQualityPlanner):
    """
    This extends the unordered top-quality planner to find additional solutions
    by reordering found plans, if not enough solutions are found
    """
    def finalize(self, plan_manager):
        plan_manager.remove_aux_actions()
        dest = os.path.join(os.getcwd(), self._get_done_plans_dir())
        if self._args.use_local_folder:
            logging.info("Copying back to current work directory")
            ## Copying best plans to current work dir
            # If done, either last plan is above the bound or proved that there are no better plans
            last_plan = plan_manager.get_last_processed_plan()
            if last_plan is None:
                logging.info("No plans to copy")
                self._finalize(plan_manager, dest)
                return

            dest_last_plan = os.path.join(os.getcwd(), self._get_found_plans_dir())
            if not os.path.exists(dest_last_plan):
                os.makedirs(dest_last_plan)

            ## If last plan is above the bound, copying it separately
            if self._enough_plans_found_quality(plan_manager):
                logging.info("All plans up to requested quality are found")
                logging.debug("copying %s to %s" % (last_plan, dest_last_plan))
                shutil.copy2(last_plan, dest_last_plan)
                plan_manager.forget_last_processed_plan()
                ## Removing the last processed plan from the old location
                os.remove(last_plan)

            if not os.path.exists(dest):
                os.makedirs(dest)

            # At this point, only the relevant plans are among the local plans
            # Before copying, we extend them
            logging.info("The number of plans found before extending is %s" % plan_manager.get_plan_counter())

            # Check if need to be extended at all
            if plan_manager.get_plan_counter() < self._args.number_of_plans:
                plans_to_extend = [plan for plan in plan_manager.get_local_plans()]
                logging.info("The plans that are going to be extended are " + " ".join(plans_to_extend))
                for plan in plans_to_extend:

                    self.extend_plan(plan_manager, plan)
                    plan_manager.process_new_plans()
                logging.info("DONE Extending the set of plans %s" % self._elapsed_time)

            self.report_number_of_plans(plan_manager)
            # Now we are copying the extended set of plans
            for plan in plan_manager.get_local_plans():
                logging.debug("copying %s to %s" % (plan, dest))
                shutil.copy2(plan, dest)

        self._finalize(plan_manager, dest)

    def extend_plan(self, plan_manager, plan_file):
        num_previous_plans = plan_manager.get_plan_counter()
        num_remaining_plans = self._args.number_of_plans - num_previous_plans
        if num_remaining_plans <= 0:
            return

        logging.info("Number of plans found so far is %s, number of remaining plans is %s" % (num_previous_plans, num_remaining_plans))

        pcargs = {}
        pcargs["external_plan_file"] = plan_file
        pcargs["num_previous_plans"] = num_previous_plans
        pcargs["num_total_plans"] = self._args.number_of_plans
        pcargs["domain_file"] = self._args.domain
        pcargs["problem_file"] = self._args.problem
        pcargs["sas_file"] = self._args.sas_file

        if self._args.case_sensitive:
            pcargs["case_sensitive"] = True
        if self._args.build:
            pcargs["build"] = self._args.build

        if self._args.symmetries:
            pcargs["use_symmetries"] = True

        pc = planner_call.AdditionalPlansPlannerCall()
        command = pc.get_callstring(**pcargs)

        logging.info("Extending the plan %s" % plan_file )
        logging.debug("Running " + str(command))
        enable_planners_output = True
        if self._args.suppress_planners_output:
            enable_planners_output = False
        try:
            planner_call.make_call(command, self.get_remaining_time(), plan_manager.get_plans_folder(), enable_output=enable_planners_output)
        except:
            raise

    def enough_plans_found(self, plan_manager):
        # Allowing to specify the number of plans bound as an additional constraint
        if self._args.number_of_plans and self._enough_plans_found_number(plan_manager, up_to_best_known_bound=True):
            return True
        return self._enough_plans_found_quality(plan_manager)


class TopQualityViaTopKPlanner(Planner):
    def get_plan_manager(self, local_folder):
        return pm.PlanManager("sas_plan", local_folder, compute_best_known=True)

    def report_iteration_step(self, plan_manager, success):
        self._report_iteration_step(plan_manager.get_number_valid_plans(up_to_best_known_bound=True), success)

    def get_planner_callstring(self, task_manager, plan_manager):
        return self._get_planner_callstring(planner_call.BaseCostOptimalPlannerCall(), task_manager, plan_manager, shortest=False, consistent=False)

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
        num_remaining_plans = self._args.upper_bound_on_number_of_plans - num_previous_plans

        pcargs = {}
        pcargs["curr_task_name"] = task_manager.get_current_task_path()
        pcargs["external_plan_file"] = plan_file
        pcargs["num_previous_plans"] = num_previous_plans
        # TODO: Check this one!
        pcargs["num_total_plans"] = num_remaining_plans

        if self._args.build:
            pcargs["build"] = self._args.build

        pcargs["reordering"] = self._args.reordering
        pc = planner_call.TopkReformulationPlannerCall()
        command = pc.get_callstring(**pcargs)

        logging.info("Reformulating the planning task, forbidding found solutions")
        logging.debug("Running " + str(command))
        return command

    def report_number_of_plans(self, plan_manager):
        plan_manager.report_number_of_plans(best_plans=True)

    def finalize(self, plan_manager):
        plan_manager.remove_aux_actions()
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
            for plan in plan_manager.get_local_plans_for_cost(plan_manager._best_known_bound+1):
                logging.debug("copying %s to %s" % (plan, dest_prove_plan))
                shutil.copy2(plan, dest_prove_plan)
            # Get plans below bound
            for plan in plan_manager.get_local_plans_upto_bound(plan_manager._best_known_bound):
                logging.debug("copying %s to %s" % (plan, dest))
                shutil.copy2(plan, dest)
        self._finalize(plan_manager, dest)

    def enough_plans_found(self, plan_manager):
        return self._enough_plans_found(plan_manager, up_to_best_known_bound=True)


class TopQualityViaUnorderedTopQualityPlanner(Planner):
    def get_plan_manager(self, local_folder):
        return pm.PlanManager("sas_plan", local_folder, compute_best_known=True)

    def report_iteration_step(self, plan_manager, success):
        self._report_iteration_step(plan_manager.get_number_valid_plans(up_to_best_known_bound=True), success)

    def get_planner_callstring(self, task_manager, plan_manager):
        return self._get_planner_callstring(planner_call.ShortestOptimalPlannerCall(), task_manager, plan_manager, shortest=True, consistent=False)

    def get_reformulation_callstring(self, task_manager, plan_manager):
        pcargs = {}
        pcargs["curr_task_name"] = task_manager.get_original_task_path()
        pcargs["external_plans_path"] = plan_manager.get_plans_folder()
        pcargs["num_plans_to_read"] = plan_manager.get_plan_counter()
        pcargs["num_previous_plans"] = 0

        if self._args.build:
            pcargs["build"] = self._args.build

        pc = planner_call.TopqReformulationPlannerCall()
        command = pc.get_callstring(**pcargs)

        logging.info("Reformulating the planning task, forbidding found solutions")

        logging.debug("Number of plans to forbid: %s, number of plans up to the best known bound: %s" % (plan_manager.get_plan_counter(), plan_manager.get_number_valid_plans(up_to_best_known_bound=True)) )
        logging.debug("Running " + str(command))
        return command

    def report_number_of_plans(self, plan_manager):
        plan_manager.report_number_of_plans(best_plans=True)

    def finalize(self, plan_manager):
        plan_manager.remove_aux_actions()
        dest = os.path.join(os.getcwd(), self._get_done_plans_dir())
        if self._args.use_local_folder:
            logging.info("Copying back to current work directory")
            ## Copying best plans to current work dir
            # If done, either last plan is above the bound or proved that there are no better plans
            last_plan = plan_manager.get_last_processed_plan()
            if last_plan is None:
                logging.info("No plans to copy")
                self._finalize(plan_manager, dest)
                return

            dest_last_plan = os.path.join(os.getcwd(), self._get_found_plans_dir())
            if not os.path.exists(dest_last_plan):
                os.makedirs(dest_last_plan)

            ## If last plan is above the bound, copying it separately
            if self._enough_plans_found_quality(plan_manager):
                logging.info("All plans up to requested quality are found")
                logging.debug("copying %s to %s" % (last_plan, dest_last_plan))
                shutil.copy2(last_plan, dest_last_plan)
                plan_manager.forget_last_processed_plan()

            if not os.path.exists(dest):
                os.makedirs(dest)
            for plan in plan_manager.get_local_plans():
                logging.debug("copying %s to %s" % (plan, dest))
                shutil.copy2(plan, dest)
        self._finalize(plan_manager, dest)

    def get_extend_plans_callstring(self,task_manager, plan_manager):
        plan_file = plan_manager.get_last_processed_plan()
        if plan_file is None:
            logging.info("Plan manages has no plans")
            exit(1)

        if not os.path.exists(plan_file):
            logging.info("File %s is not found" % plan_file)
            exit(1)

        plan_manager.forget_last_processed_plan()
        num_previous_plans = plan_manager.get_plan_counter()
        task = task_manager.get_original_task_path()

        pcargs = {}
        pcargs["curr_task_name"] = task
        pcargs["external_plan_file"] = plan_file
        pcargs["num_previous_plans"] = num_previous_plans
        pcargs["num_total_plans"] = self._args.upper_bound_on_number_of_plans

        if self._args.build:
            pcargs["build"] = self._args.build

        if self._args.symmetries:
            pcargs["use_symmetries"] = True

        pc = planner_call.AdditionalPlansPlannerCall()
        command = pc.get_callstring(**pcargs)

        logging.info("Extending the set of found solutions")
        logging.debug("Running " + str(command))
        return command

    def report_done_plans_extension_run(self):
        logging.info("DONE Extending the set of plans %s" % self._elapsed_time)

    def enough_plans_found(self, plan_manager):
        # Allowing to specify the number of plans bound as an additional constraint
        if self._args.number_of_plans and self._enough_plans_found_number(plan_manager, up_to_best_known_bound=True):
            return True
        return self._enough_plans_found_quality(plan_manager)


# Planner for forbidding supersets of previously found plans
class SubsetTopQualityPlanner(UnorderedTopQualityPlanner):
    def get_reformulation_callstring(self, task_manager, plan_manager):
        return self._get_default_reformulation_callstring(planner_call.TopqSupersetReformulationPlannerCall(), task_manager, plan_manager)

class SubMultisetTopQualityPlanner(UnorderedTopQualityPlanner):
    def get_reformulation_callstring(self, task_manager, plan_manager):
        return self._get_default_reformulation_callstring(planner_call.TopqSuperMultisetReformulationPlannerCall(), task_manager, plan_manager)


###################################################################################################
#####  Diverse planners

class DiversePlanner(Planner):
    def get_planner_call(self):
        return planner_call.CerberusPlannerCall()

    def get_plan_manager(self, local_folder):
        return pm.PlanManager("sas_plan", local_folder, compute_best_known=False)

    def report_iteration_step(self, plan_manager, success):
        self._report_iteration_step(plan_manager.get_plan_counter(), success)

    def get_planner_callstring(self, task_manager, plan_manager):
        return self._get_planner_callstring(self.get_planner_call(), task_manager, plan_manager)

    def get_reformulation_callstring(self, task_manager, plan_manager):
        return self._get_default_reformulation_callstring(planner_call.DiverseReformulationPlannerCall(), task_manager, plan_manager)
        # pcargs = {}
        # num_existing_plans = plan_manager.get_plan_counter()
        # pcargs["curr_task_name"] = task_manager.get_original_task_path()
        # pcargs["external_plans_path"] = plan_manager.get_plans_folder()
        # pcargs["num_plans_to_read"] = num_existing_plans
        # pcargs["num_previous_plans"] = 0

        # if self._args.build:
        #     pcargs["build"] = self._args.build

        # if self._args.number_of_plans:
        #     pcargs["num_total_plans"] = self._args.number_of_plans - num_existing_plans

        # if self._args.symmetries:
        #     pcargs["use_symmetries"] = True

        # pc = planner_call.DiverseReformulationPlannerCall()
        # command = pc.get_callstring(**pcargs)
        # logging.info("Reformulating the planning task, forbidding found solutions")
        # logging.debug("Running " + str(command))
        # return command

    def report_number_of_plans(self, plan_manager):
        plan_manager.report_number_of_plans(best_plans=False)

    def finalize(self, plan_manager):
        plan_manager.remove_aux_actions()
        dest = os.path.join(os.getcwd(), self._get_found_plans_dir())
        if self._args.use_local_folder:
            ## Copying best plans to current work dir
            logging.info("Copying back to current work directory")
            if not os.path.exists(dest):
                os.makedirs(dest)
            for plan in plan_manager.get_local_plans():
                logging.debug("copying %s to %s" % (plan, dest))
                shutil.copy2(plan, dest)
        self._finalize(plan_manager, dest)

    def enough_plans_found(self, plan_manager):
        return self._enough_plans_found(plan_manager, up_to_best_known_bound=False)
