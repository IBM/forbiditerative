#! /usr/bin/env python3

import itertools
import os

from lab.environments import LocalEnvironment, BaselSlurmEnvironment
from lab.reports import Attribute, arithmetic_mean, geometric_mean

from downward.reports.compare import ComparativeReport

import common_setup
from common_setup import IssueConfig, IssueExperiment

DIR = os.path.dirname(os.path.abspath(__file__))
SCRIPT_NAME = os.path.splitext(os.path.basename(__file__))[0]
BENCHMARKS_DIR = os.environ['DOWNWARD_BENCHMARKS']
REVISIONS = ['forbid-iterative']
CONFIGS = [
    IssueConfig('lmcut-oss-por', ['--symmetries', 'sym=structural_symmetries(time_bound=0,search_symmetries=oss)', '--search', 'astar(lmcut(),symmetries=sym,verbosity=silent,pruning=stubborn_sets_simple(min_required_pruning_ratio=0.01,expansions_before_checking_pruning_ratio=1000))']),
]

SUITE = common_setup.DEFAULT_OPTIMAL_SUITE
ENVIRONMENT = LocalEnvironment(processes=48)


exp = IssueExperiment(
    revisions=REVISIONS,
    configs=CONFIGS,
    environment=ENVIRONMENT,
)
exp.add_suite(BENCHMARKS_DIR, SUITE)

exp.add_parser(exp.EXITCODE_PARSER)
exp.add_parser(exp.TRANSLATOR_PARSER)
exp.add_parser(exp.SINGLE_SEARCH_PARSER)
exp.add_parser(exp.PLANNER_PARSER)

exp.add_parser('symmetries-parser.py')

exp.add_step('build', exp.build)
exp.add_step('start', exp.start_runs)
exp.add_fetcher(name='fetch')

extra_attributes=[
    Attribute('num_search_generators', absolute=True, min_wins=False),
    Attribute('num_operator_generators', absolute=True, min_wins=False),
    Attribute('num_total_generators', absolute=True, min_wins=False),
    Attribute('symmetry_graph_size', absolute=True, min_wins=True),
    Attribute('time_symmetries', absolute=False, min_wins=True, function=geometric_mean),
    Attribute('symmetry_group_order', absolute=True, min_wins=False),
]
attributes = list(exp.DEFAULT_TABLE_ATTRIBUTES)
attributes.extend(extra_attributes)

exp.add_absolute_report_step(attributes=attributes)


def make_comparison_tables():
    compared_configs = [("forbid-iterative-lmcut-oss-por", "no-d-eval-lmcut-oss-por", "Diff (lmcut-oss-por)")]

    report = ComparativeReport(compared_configs, attributes=attributes)
    outfile = os.path.join(
                    exp.eval_dir,
                    "%s-compare.%s" % (
                        exp.name, report.output_format))
    report(exp.eval_dir, outfile)

    sreport = RelativeScatterPlotReport(
            attributes=["memory"],
            filter_algorithm=["forbid-iterative-lmcut-oss-por", "no-d-eval-lmcut-oss-por"],
            get_category=lambda run1, run2: run1["domain"])
    
    outfile = os.path.join(
                    exp.eval_dir,
                    "%s-memory.%s" % (
                        exp.name, sreport.output_format))
    sreport(exp.eval_dir, outfile)

exp.add_step("make-comparison-tables", make_comparison_tables)


exp.run_steps()
