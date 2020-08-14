#!/usr/bin/python

import re
import subprocess
import os, sys, json



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



class PlannerCall(object):
    def build_args(self):
        return ["--build", "release64"]

    def get_path(self):
        """Assume that this script always lives in the base dir of the infrastructure."""
        return os.path.abspath(get_script_dir())

    def get_callstring(self, **kwargs):
        return []


class TopqReformulationPlannerCall(PlannerCall):
    def get_callstring(self, **kwargs):
        return [sys.executable, os.path.join(self.get_path(), 'fast-downward.py')] + self.build_args() + \
            ["{curr_task_name}".format(**kwargs), "--internal-previous-portfolio-plans", str("{num_previous_plans}".format(**kwargs))] + \
            self.planner_args(**kwargs)

    def planner_args(self, **kwargs):
        if "use_symmetries" in kwargs:
            return ["--symmetries",
              "sym=structural_symmetries(time_bound=0,search_symmetries=dks, \
               stabilize_initial_state=true, keep_operator_symmetries=true, dump=true)",
               "--search", "forbid_iterative(reformulate = FORBID_MULTIPLE_PLAN_MULTISETS, \
               extend_plans_with_symmetry=sym, dump=false, change_operator_names=false, \
               number_of_plans_to_read={num_plans_to_read}, external_plans_path={external_plans_path})".format(**kwargs)]
        else:
            return ["--search", "forbid_iterative(reformulate = FORBID_MULTIPLE_PLAN_MULTISETS, \
               dump=false, change_operator_names=false, \
               number_of_plans_to_read={num_plans_to_read}, external_plans_path={external_plans_path})".format(**kwargs)]


class DiverseReformulationPlannerCall(PlannerCall):
    def get_callstring(self, **kwargs):
        return [sys.executable, os.path.join(self.get_path(), 'fast-downward.py')] + self.build_args() + \
            ["{curr_task_name}".format(**kwargs), "--internal-previous-portfolio-plans", str("{num_previous_plans}".format(**kwargs))] + \
            self.planner_args(**kwargs)

    def planner_args(self, **kwargs):
        return ["--symmetries",
              "sym=structural_symmetries(time_bound=0,search_symmetries=dks, \
               stabilize_initial_state=true, keep_operator_symmetries=true)",
               "--search", "forbid_iterative(reformulate = FORBID_SINGLE_PLAN_MULTISET, \
               extend_plans_with_symmetry=sym, dump=false, change_operator_names=false, number_of_plans={num_plans_needed}, \
               number_of_plans_to_read={num_plans_to_read}, external_plans_path={external_plans_path})".format(**kwargs)]


class TopkReformulationPlannerCall(PlannerCall):
    def get_callstring(self, **kwargs):
        return [sys.executable, os.path.join(self.get_path(), 'fast-downward.py')] + self.build_args() + \
            ["{curr_task_name}".format(**kwargs), "--internal-previous-portfolio-plans", str("{num_previous_plans}".format(**kwargs))] + \
            self.planner_args(**kwargs)

    def planner_args(self, **kwargs):
        return ["--symmetries",
              "sym=structural_symmetries(time_bound=0,search_symmetries=dks, \
               stabilize_initial_state=true, keep_operator_symmetries=true)",
               "--search", "forbid_iterative(reformulate = FORBID_MULTIPLE_PLANS, \
               extend_plans_with_symmetry=sym,  reduce_plan_orders={reordering}, dump=false, \
               number_of_plans={num_remaining_plans},  change_operator_names=true,\
               external_plan_file={external_plan_file})".format(**kwargs)]



class BasePlannerCall(PlannerCall):
    def get_callstring(self, **kwargs):
        task_args = []
        if "curr_task_name" in kwargs:
            task_args = [ "{curr_task_name}".format(**kwargs) ]
        else:
            task_args = [os.path.abspath("{domain_file}".format(**kwargs)), os.path.abspath("{problem_file}".format(**kwargs))]

        return [sys.executable, os.path.join(self.get_path(), 'fast-downward.py')] + self.build_args() + task_args + \
            ["--internal-previous-portfolio-plans", str("{num_previous_plans}".format(**kwargs))] + \
            self.planner_args(**kwargs)



class BaseCostOptimalPlannerCall(BasePlannerCall):
    def planner_args(self, **kwargs):
        search_heur = "blind()" if "consistent" in kwargs and kwargs["consistent"] else "celmcut()"
        shortest_opt = "shortest=true" if "shortest" in kwargs and kwargs["shortest"] else "shortest=false"
        return ["--symmetries",
                "sym=structural_symmetries(time_bound=0,search_symmetries=oss, \
                stabilize_initial_state=false, keep_operator_symmetries=false)",
                "--search",
                "astar(%s, %s, symmetries=sym)" % (search_heur, shortest_opt) ]


class BaseSatisficingPlannerCall(BasePlannerCall):
    def planner_args(self, **kwargs):
        # LAMA first iteration
        return ["--heuristic",
    "hlm,hff=lm_ff_syn(lm_rhw(reasonable_orders=true,lm_cost_type=one),"
    "                  transform=adapt_costs(one))",
    "--search", """lazy_greedy([hff,hlm],preferred=[hff,hlm],
                               cost_type=one,reopen_closed=false)"""]


class CerberusPlannerCall(PlannerCall):
    def get_path(self):
        path = os.getenv('DIVERSE_FAST_DOWNWARD_PLANNER_PATH')
        if not path:
            print("Environment variable DIVERSE_FAST_DOWNWARD_PLANNER_PATH has to be specified.")
            exit(1)
        return path

    def get_callstring(self, **kwargs):
        task_args = []
        if "curr_task_name" in kwargs:
            task_args = [ "{curr_task_name}".format(**kwargs) ]
        else:
            task_args = [os.path.abspath("{domain_file}".format(**kwargs)), os.path.abspath("{problem_file}".format(**kwargs))]

        return [sys.executable, os.path.join(self.get_path(), 'fast-downward.py'), "--keep-sas-file"] + task_args + \
            ["--internal-previous-portfolio-plans", str("{num_previous_plans}".format(**kwargs))] + \
            self.planner_args(**kwargs)

    def planner_args(self, **kwargs):
        return self._get_cerberus_first(pref="true", dag="from_coloring")

    def _get_cerberus_first(self, **kwargs):
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

class AdditionalPlansPlannerCall(BasePlannerCall):
    def planner_args(self, **kwargs):
        if "use_symmetries" in kwargs:
            return ["--symmetries",
                "sym=structural_symmetries(time_bound=0,search_symmetries=dks, \
                stabilize_initial_state=true, keep_operator_symmetries=true)",
                "--search", "forbid_iterative(reformulate = NONE_FIND_ADDITIONAL_PLANS, \
                extend_plans_with_symmetry=sym, dump=false, \
                number_of_plans={num_remaining_plans}, external_plan_file={external_plan_file})".format(**kwargs)]
        else:
            return ["--search", "forbid_iterative(reformulate = NONE_FIND_ADDITIONAL_PLANS, \
                dump=false, number_of_plans={num_remaining_plans},\
                external_plan_file={external_plan_file})".format(**kwargs)]



class PlansToJsonPlannerCall(PlannerCall):
    def get_callstring(self, **kwargs):
        return [sys.executable, os.path.join(self.get_path(), 'fast-downward.py')] + self.build_args() + \
                [os.path.abspath("{domain_file}".format(**kwargs)), os.path.abspath("{problem_file}".format(**kwargs))] + \
            self.planner_args(**kwargs)

    def planner_args(self, **kwargs):
        return ["--search", \
            "forbid_iterative(number_of_plans_to_read={num_plans}, external_plans_path={plans_path}, \
            dump_causal_links_json=false, dump_states_json=true, \
            json_file_to_dump={results_file})".format(**kwargs)]
