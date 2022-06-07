#! /usr/bin/env python
# -*- coding: utf-8 -*-

import os

from lab.environments import LocalEnvironment, BaselSlurmEnvironment

import common_setup
from common_setup import IssueConfig, IssueExperiment
#from relativescatter import RelativeScatterPlotReport
from itertools import combinations

DIR = os.path.dirname(os.path.abspath(__file__))
BENCHMARKS_DIR = os.environ["DOWNWARD_BENCHMARKS"]
# These revisions are all tag experimental branches off the same revision.
# we only need different tags so lab creates separate build directories in the build cache.
# We then manually recompile the code in the build cache with the correct settings.
REVISIONS = ["a0eb9a43ba45f8f9817d99ee7b8ea9676937a0a9"]
CONFIGS = [
    IssueConfig("blind", ["--search", "astar(blind())"]),
    IssueConfig("shortest-blind", ["--search", "shortest_astar(blind())"]),
    IssueConfig("lmcut", ["--search", "astar(lmcut())"]),
    IssueConfig("shortest-lmcut", ["--search", "shortest_astar(lmcut())"]),
]
SUITE = ["agricola-opt18-strips", "barman-opt11-strips", "caldera-split-opt18-adl", "cavediving-14-adl", "citycar-opt14-adl", "data-network-opt18-strips", "elevators-opt08-strips", "elevators-opt11-strips", "floortile-opt11-strips", "floortile-opt14-strips", "ged-opt14-strips", "openstacks-opt08-adl", "openstacks-opt08-strips", "openstacks-opt11-strips", "openstacks-opt14-strips", "organic-synthesis-split-opt18-strips", "parcprinter-08-strips", "parcprinter-opt11-strips", "pegsol-08-strips", "pegsol-opt11-strips", "petri-net-alignment-opt18-strips", "scanalyzer-08-strips", "scanalyzer-opt11-strips", "settlers-opt18-adl", "sokoban-opt08-strips", "sokoban-opt11-strips", "spider-opt18-strips", "tetris-opt14-strips", "transport-opt08-strips", "transport-opt11-strips", "transport-opt14-strips", "woodworking-opt08-strips", "woodworking-opt11-strips"]


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

exp.add_step('build', exp.build)
exp.add_step('start', exp.start_runs)
exp.add_fetcher(name='fetch')
exp.add_fetcher('data/issue980-v3-eval')
exp.add_fetcher('data/issue980-por-eval')

algs = ["shortest-lmcut", "shortest-lmcut-oss", "shortest-lmcut-por", "shortest-lmcut-oss-por"]
full_alg_name = ["%s-%s" % (r, a) for r in REVISIONS for a in algs]
print(full_alg_name)
attributes = (
            IssueExperiment.DEFAULT_TABLE_ATTRIBUTES + ["plan_length"])
exp.add_absolute_report_step(attributes=attributes,filter_algorithm=full_alg_name)
#exp.add_comparison_table_step(attributes=attributes)


#exp.add_comparison_table_step()
"""
for r1, r2 in combinations(REVISIONS, 2):
    for nick in ["opcount-seq-lmcut", "diverse-potentials", "optimal-lmcount"]:
        exp.add_report(RelativeScatterPlotReport(
            attributes=["total_time"],
            filter_algorithm=["%s-%s" % (r, nick) for r in [r1, r2]],
            get_category=lambda run1, run2: run1["domain"]),
            outfile="issue925-v1-total-time-%s-%s-%s.png" % (r1, r2, nick))
"""
exp.run_steps()
