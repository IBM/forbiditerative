#!/usr/bin/python

import os, sys

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
    sys.stdout.flush()
    po = subprocess.check_output(command, timeout=time_limit, cwd=local_folder)

    if enable_output:
        print(po.decode())


class PlannerCall(object):
    def get_path(self):
        """Assume that this script always lives in the base dir of the infrastructure."""
        return os.path.abspath(get_script_dir())

    def get_callstring(self, **kwargs):
        return []

    def get_task_args(self, **kwargs):
        if "curr_task_name" in kwargs:
            return [ "{curr_task_name}".format(**kwargs) ]
        elif "sas_file" in kwargs and kwargs["sas_file"] is not None:
            return [ os.path.abspath("{sas_file}".format(**kwargs)) ]
        else:
            translator_options = ["--translate-options", "--case-sensitive", "--search-options"] if 'case_sensitive' in kwargs else []
            return ["--transform-task", "preprocess", os.path.abspath("{domain_file}".format(**kwargs)), os.path.abspath("{problem_file}".format(**kwargs))] + translator_options


class ReformulationPlannerCall(PlannerCall):
    def get_callstring(self, **kwargs):
        return [sys.executable, os.path.join(self.get_path(), 'fast-downward.py'), "--keep-sas-file"] + \
               (["--build", kwargs['build']] if 'build' in kwargs else []) + \
               ["{curr_task_name}".format(**kwargs), "--internal-previous-portfolio-plans", str("{num_previous_plans}".format(**kwargs))] + \
               self.planner_args(**kwargs)


class TopqReformulationPlannerCall(ReformulationPlannerCall):
    def planner_args(self, **kwargs):
        if "use_symmetries" in kwargs:
            return ["--symmetries",
              "sym=structural_symmetries(time_bound=0,search_symmetries=dks, \
               stabilize_initial_state=true, keep_operator_symmetries=true)",
               "--search", "forbid_iterative(reformulate = FORBID_MULTIPLE_PLAN_MULTISETS, \
               extend_plans_with_symmetry=sym, dump=false, change_operator_names=false, \
               number_of_plans_to_read={num_plans_to_read}, external_plans_path={external_plans_path})".format(**kwargs)]
        else:
            return ["--search", "forbid_iterative(reformulate = FORBID_MULTIPLE_PLAN_MULTISETS, \
               dump=false, change_operator_names=false, \
               number_of_plans_to_read={num_plans_to_read}, external_plans_path={external_plans_path})".format(**kwargs)]


class TopqSupersetReformulationPlannerCall(ReformulationPlannerCall):
    def planner_args(self, **kwargs):

        numplans = ""
        if "num_total_plans" in kwargs:
            numplans = "number_of_plans={num_total_plans}, ".format(**kwargs)

        if "use_symmetries" in kwargs:
            return ["--symmetries",
              "sym=structural_symmetries(time_bound=0,search_symmetries=dks, \
               stabilize_initial_state=true, keep_operator_symmetries=true)",
               "--search", "forbid_iterative(reformulate = FORBID_MULTIPLE_PLAN_SUPERSETS, \
               extend_plans_with_symmetry=sym, dump=false, change_operator_names=true, %s\
               number_of_plans_to_read={num_plans_to_read}, external_plans_path={external_plans_path})".format(**kwargs) % numplans]
        else:
            return ["--search", "forbid_iterative(reformulate = FORBID_MULTIPLE_PLAN_SUPERSETS, \
               dump=false, change_operator_names=true, %s \
               number_of_plans_to_read={num_plans_to_read}, external_plans_path={external_plans_path})".format(**kwargs) % numplans]



class TopqSuperMultisetReformulationPlannerCall(ReformulationPlannerCall):
    def planner_args(self, **kwargs):

        numplans = ""
        if "num_total_plans" in kwargs:
            numplans = "number_of_plans={num_total_plans}, ".format(**kwargs)

        if "use_symmetries" in kwargs:
            return ["--symmetries",
              "sym=structural_symmetries(time_bound=0,search_symmetries=dks, \
               stabilize_initial_state=true, keep_operator_symmetries=true)",
               "--search", "forbid_iterative(reformulate = FORBID_MULTIPLE_PLAN_SUPERMULTISETS, \
               extend_plans_with_symmetry=sym, dump=false, change_operator_names=true,  %s\
               number_of_plans_to_read={num_plans_to_read}, external_plans_path={external_plans_path})".format(**kwargs) % numplans]
        else:
            return ["--search", "forbid_iterative(reformulate = FORBID_MULTIPLE_PLAN_SUPERMULTISETS, \
               dump=false, change_operator_names=true,  %s \
               number_of_plans_to_read={num_plans_to_read}, external_plans_path={external_plans_path})".format(**kwargs) % numplans]


class DiverseReformulationPlannerCall(ReformulationPlannerCall):
    def planner_args(self, **kwargs):
        if "use_symmetries" in kwargs:
            return ["--symmetries",
                "sym=structural_symmetries(time_bound=0,search_symmetries=dks, \
                stabilize_initial_state=true, keep_operator_symmetries=true)",
                "--search", "forbid_iterative(reformulate = FORBID_SINGLE_PLAN_MULTISET, \
                extend_plans_with_symmetry=sym, dump=false, change_operator_names=false, number_of_plans={num_total_plans}, \
                number_of_plans_to_read={num_plans_to_read}, external_plans_path={external_plans_path})".format(**kwargs)]
        else:
            return ["--search", "forbid_iterative(reformulate = FORBID_SINGLE_PLAN_MULTISET, \
                dump=false, change_operator_names=false, number_of_plans={num_total_plans}, \
                number_of_plans_to_read={num_plans_to_read}, external_plans_path={external_plans_path})".format(**kwargs)]


class TopkReformulationPlannerCall(ReformulationPlannerCall):
    def planner_args(self, **kwargs):
        return ["--symmetries",
              "sym=structural_symmetries(time_bound=0,search_symmetries=dks, \
               stabilize_initial_state=true, keep_operator_symmetries=true)",
               "--search", "forbid_iterative(reformulate = FORBID_MULTIPLE_PLANS, \
               extend_plans_with_symmetry=sym,  reduce_plan_orders={reordering}, dump=false, \
               number_of_plans={num_total_plans},  change_operator_names=true,\
               external_plan_file={external_plan_file})".format(**kwargs)]


class BasePlannerCall(PlannerCall):
    def get_callstring(self, **kwargs):
        return [sys.executable, os.path.join(self.get_path(), 'fast-downward.py'), "--keep-sas-file"] + \
               (["--build", kwargs['build']] if 'build' in kwargs else []) + \
               self.get_task_args(**kwargs) + \
               ["--internal-previous-portfolio-plans", str("{num_previous_plans}".format(**kwargs))] + \
               self.planner_args(**kwargs)



class BaseCostOptimalPlannerCall(BasePlannerCall):
    def planner_args(self, **kwargs):
        bound_opt = "bound={cost_bound},".format(**kwargs) if "cost_bound" in kwargs else ""
        if "consistent" in kwargs and kwargs["consistent"]:
            return ["--symmetries",
                    "sym=structural_symmetries(time_bound=0,search_symmetries=oss, \
                    stabilize_initial_state=false, keep_operator_symmetries=false)",
                    "--search",
                    "astar(blind(),%s symmetries=sym,pruning=stubborn_sets_simple(min_required_pruning_ratio=0.01,expansions_before_checking_pruning_ratio=1000))" % bound_opt ]

        return ["--if-conditional-effects", "--evaluator", "h=celmcut()",
                "--if-no-conditional-effects", "--evaluator", "h=lmcut()",
                "--always", "--symmetries", "sym=structural_symmetries(time_bound=0,search_symmetries=oss, \
                    stabilize_initial_state=false, keep_operator_symmetries=false)",
                "--search",
                "astar(h,%s symmetries=sym,pruning=stubborn_sets_simple(min_required_pruning_ratio=0.01,expansions_before_checking_pruning_ratio=1000))" % bound_opt ]


class BaseSatisficingPlannerCall(BasePlannerCall):
    def planner_args(self, **kwargs):
        # LAMA first iteration
        return ["--evaluator",
                "hlm=lmcount(lm_factory=lm_reasonable_orders_hps(lm_rhw()),transform=adapt_costs(one),pref=false)",
                "--evaluator", "hff=ff(transform=adapt_costs(one))",
                "--search", """lazy_greedy([hff,hlm],preferred=[hff,hlm],
                               cost_type=one,reopen_closed=false)"""]
        
class ShortestOptimalPlannerCall(PlannerCall):
    # def get_path(self):
    #     path = os.getenv('SHORTEST_OPTIMAL_FAST_DOWNWARD_PLANNER_PATH')
    #     if not path:
    #         print("Environment variable SHORTEST_OPTIMAL_FAST_DOWNWARD_PLANNER_PATH has to be specified.")
    #         exit(1)
    #     return path

    def get_callstring(self, **kwargs):
        return [sys.executable, os.path.join(self.get_path(), 'fast-downward.py'), "--keep-sas-file"] + \
               (["--build", kwargs['build']] if 'build' in kwargs else []) + \
               self.get_task_args(**kwargs) + \
               ["--internal-previous-portfolio-plans", str("{num_previous_plans}".format(**kwargs))] + \
               self.planner_args(**kwargs)

    def planner_args(self, **kwargs):
        shortest = "shortest" in kwargs and kwargs["shortest"]
        bound_opt = "bound={cost_bound},".format(**kwargs) if "cost_bound" in kwargs else ""
        search_opt = "shortest_astar" if shortest else "astar"
        return [
        "--if-conditional-effects", "--evaluator", "h=celmcut()",
        "--if-no-conditional-effects", "--evaluator", "h=lmcut()",
        "--always", "--symmetries", "sym=structural_symmetries(time_bound=0,search_symmetries=oss, stabilize_initial_state=false)",
        "--if-unit-cost",
        "--search", "astar(h,%s symmetries=sym,pruning=stubborn_sets_simple(min_required_pruning_ratio=0.01,expansions_before_checking_pruning_ratio=1000))" % (bound_opt),
        "--if-non-unit-cost",
        "--search", "%s(h,%s symmetries=sym,pruning=stubborn_sets_simple(min_required_pruning_ratio=0.01,expansions_before_checking_pruning_ratio=1000))" % (search_opt, bound_opt),
        "--always"]


class CerberusPlannerCall(PlannerCall):
    # def get_path(self):
    #     path = os.getenv('DIVERSE_FAST_DOWNWARD_PLANNER_PATH')
    #     if not path:
    #         print("Environment variable DIVERSE_FAST_DOWNWARD_PLANNER_PATH has to be specified.")
    #         exit(1)
    #     return path

    def get_callstring(self, **kwargs):
        return [sys.executable, os.path.join(self.get_path(), 'fast-downward.py'), "--keep-sas-file"] + \
               (["--build", kwargs['build']] if 'build' in kwargs else []) + \
               self.get_task_args(**kwargs) + \
               ["--internal-previous-portfolio-plans", str("{num_previous_plans}".format(**kwargs))] + \
               self.planner_args(**kwargs)

    def planner_args(self, **kwargs):
        return self._get_cerberus_first(pref="true", dag="from_coloring")

    def _get_cerberus_first(self, **kwargs):
        return [
        "--if-unit-cost",
        "--evaluator",
        "hlm=lmcount(lm_reasonable_orders_hps(lm_rhw()),pref={pref})".format(**kwargs),
        "--evaluator", 
        "hrb=RB(dag={dag}, extract_plan=true)".format(**kwargs),
        "--evaluator", 
        "hn=novelty(eval=hrb, type=separate_both)",
        "--search", """lazy(open=alt([tiebreaking([hn, hrb]), single(hrb,pref_only=true), single(hlm), single(hlm,pref_only=true)], boost=1000),preferred=[hrb,hlm])""",
        "--if-non-unit-cost",
        "--evaluator",
        "hlm=lmcount(lm_reasonable_orders_hps(lm_rhw()),transform=adapt_costs(one),pref={pref})".format(**kwargs),
        "--evaluator", 
        "hrb=RB(dag={dag}, extract_plan=true, transform=adapt_costs(one))".format(**kwargs),
        "--evaluator", 
        "hn=novelty(eval=hrb, type=separate_both)",
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
                number_of_plans={num_total_plans}, external_plan_file={external_plan_file})".format(**kwargs)]
        else:
            return ["--search", "forbid_iterative(reformulate = NONE_FIND_ADDITIONAL_PLANS, \
                dump=false, number_of_plans={num_total_plans},\
                external_plan_file={external_plan_file})".format(**kwargs)]



class PlansToJsonPlannerCall(PlannerCall):
    def get_callstring(self, **kwargs):
        translator_options = ["--translate-options", "--case-sensitive", "--search-options"] if 'case_sensitive' in kwargs else []
        return [sys.executable, os.path.join(self.get_path(), 'fast-downward.py'), "--keep-sas-file"] + \
               (["--build", kwargs['build']] if 'build' in kwargs else []) + \
               [os.path.abspath("{domain_file}".format(**kwargs)), os.path.abspath("{problem_file}".format(**kwargs))] + \
               translator_options + self.planner_args(**kwargs)

    def planner_args(self, **kwargs):
        return ["--search", \
            "forbid_iterative(number_of_plans_to_read={num_plans}, external_plans_path={plans_path}, \
            dump_causal_links_json=false, dump_states_json=true, \
            json_file_to_dump={results_file})".format(**kwargs)]
