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
REVISIONS = ["c292531fa6cdbeae95a0bf576fd50a65967ed5cc"]

CONFIGS = [
    # IssueConfig("blind", ["--search", "astar(blind())"]),
    IssueConfig("shortest-blind-dks", ["--symmetries", "sym=structural_symmetries(time_bound=0,search_symmetries=dks, stabilize_initial_state=false)","--search", "shortest_astar(blind(),symmetries=sym)"]),
    IssueConfig("shortest-lmcut-dks", ["--symmetries", "sym=structural_symmetries(time_bound=0,search_symmetries=dks, stabilize_initial_state=false)","--search", "shortest_astar(lmcut(),symmetries=sym)"]),
    IssueConfig("shortest-ms-dks", ["--symmetries", "sym=structural_symmetries(time_bound=0,search_symmetries=dks, stabilize_initial_state=false)","--search", "shortest_astar(merge_and_shrink(transform=no_transform(), cache_estimates=true, merge_strategy=merge_strategy=merge_sccs(order_of_sccs=topological,merge_selector=score_based_filtering(scoring_functions=[goal_relevance,dfp,total_order])),shrink_strategy=shrink_strategy=shrink_bisimulation(greedy=false), prune_unreachable_states=true, prune_irrelevant_states=true, max_states=-1, max_states_before_merge=-1, threshold_before_merge=-1, verbosity=normal, main_loop_max_time=infinity),symmetries=sym)"]),
    IssueConfig("shortest-ms-us-dks", ["--symmetries", "sym=structural_symmetries(time_bound=0,search_symmetries=dks, stabilize_initial_state=false)","--search", "shortest_astar(merge_and_shrink(transform=no_transform(), cache_estimates=true, merge_strategy=merge_strategy=merge_sccs(order_of_sccs=topological,merge_selector=score_based_filtering(scoring_functions=[goal_relevance,dfp,total_order])),shrink_strategy=shrink_strategy=shrink_bisimulation(greedy=false), prune_unreachable_states=false, prune_irrelevant_states=true, max_states=-1, max_states_before_merge=-1, threshold_before_merge=-1, verbosity=normal, main_loop_max_time=infinity),symmetries=sym)"]),
    IssueConfig("shortest-cegar-dks", ["--symmetries", "sym=structural_symmetries(time_bound=0,search_symmetries=dks, stabilize_initial_state=false)","--search", "shortest_astar(cegar(),symmetries=sym)"]),
    IssueConfig("shortest-hmax-dks", ["--symmetries", "sym=structural_symmetries(time_bound=0,search_symmetries=dks, stabilize_initial_state=false)","--search", "shortest_astar(hmax(),symmetries=sym)"]),
    IssueConfig("shortest-ipdb-dks", ["--symmetries", "sym=structural_symmetries(time_bound=0,search_symmetries=dks, stabilize_initial_state=false)","--search", "shortest_astar(ipdb(pdb_max_size=2000000, collection_max_size=20000000, num_samples=1000, min_improvement=10, max_time=infinity, random_seed=-1, max_time_dominance_pruning=infinity, transform=no_transform(), cache_estimates=true),symmetries=sym)"]),
]

SUITE = ["agricola-opt18-strips", "barman-opt11-strips", "data-network-opt18-strips", "elevators-opt08-strips", "elevators-opt11-strips", "floortile-opt11-strips", "floortile-opt14-strips", "ged-opt14-strips", "openstacks-opt08-strips", "openstacks-opt11-strips", "openstacks-opt14-strips", "organic-synthesis-split-opt18-strips", "parcprinter-08-strips", "parcprinter-opt11-strips", "pegsol-08-strips", "pegsol-opt11-strips", "petri-net-alignment-opt18-strips", "scanalyzer-08-strips", "scanalyzer-opt11-strips", "sokoban-opt08-strips", "sokoban-opt11-strips", "spider-opt18-strips", "tetris-opt14-strips", "transport-opt08-strips", "transport-opt11-strips", "transport-opt14-strips", "woodworking-opt08-strips", "woodworking-opt11-strips"]


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

attributes = (
            IssueExperiment.DEFAULT_TABLE_ATTRIBUTES + ["plan_length"])
exp.add_absolute_report_step(attributes=attributes)



exp.run_steps()
