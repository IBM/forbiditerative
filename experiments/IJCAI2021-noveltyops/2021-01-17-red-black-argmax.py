#! /usr/bin/env python
# -*- coding: utf-8 -*-

import os
import subprocess

from lab.environments import LocalEnvironment
from lab.reports import Attribute, arithmetic_mean, geometric_mean
from downward.reports.absolute import AbsoluteReport
# from downward.reports.compare import ComparativeReport
# from downward.reports.scatter import ScatterPlotReport
from lab import tools

from common_setup import IssueConfig, IssueExperiment 



REVISION = 'c2d153b863bd8256f5a4289c98f4a5f14689c2ab'


def main(revisions=None):
    benchmarks_dir = os.environ["DOWNWARD_BENCHMARKS"]
    suite = ["agricola-sat18-strips", "airport", "barman-sat11-strips", "barman-sat14-strips", "blocks", "childsnack-sat14-strips", "data-network-sat18-strips", "depot", "driverlog", "elevators-sat08-strips", "elevators-sat11-strips", "floortile-sat11-strips", "floortile-sat14-strips", "freecell", "ged-sat14-strips", "grid", "gripper", "hiking-sat14-strips", "logistics00", "logistics98", "miconic", "movie", "mprime", "mystery", "nomystery-sat11-strips", "openstacks-sat08-strips", "openstacks-sat11-strips", "openstacks-sat14-strips", "openstacks-strips", "organic-synthesis-sat18-strips", "organic-synthesis-split-sat18-strips", "parcprinter-08-strips", "parcprinter-sat11-strips", "parking-sat11-strips", "parking-sat14-strips", "pathways", "pegsol-08-strips", "pegsol-sat11-strips", "pipesworld-notankage", "pipesworld-tankage", "psr-small", "rovers", "satellite", "scanalyzer-08-strips", "scanalyzer-sat11-strips", "snake-sat18-strips", "sokoban-sat08-strips", "sokoban-sat11-strips", "spider-sat18-strips", "storage", "termes-sat18-strips", "tetris-sat14-strips", "thoughtful-sat14-strips", "tidybot-sat11-strips", "tpp", "transport-sat08-strips", "transport-sat11-strips", "transport-sat14-strips", "trucks-strips", "visitall-sat11-strips", "visitall-sat14-strips", "woodworking-sat08-strips", "woodworking-sat11-strips", "zenotravel"]
    # suite = ["elevators-sat08-strips:p01.pddl"]
    environment = LocalEnvironment(processes=48)

    BUILD_OPTIONS = []
    DRIVER_OPTIONS = ['--alias', 'seq-agl-cerberus-novelops-argmax', "--overall-time-limit", "30m", "--overall-memory-limit", "4096M"]

    configs = set()

    configs.add(IssueConfig('cerberus', [],
                build_options=BUILD_OPTIONS, driver_options=DRIVER_OPTIONS))


    # for seed in seeds:
        # configs.add(IssueConfig('lamaf-lm%s' % i, ['--evaluator', "hlm=lmcount(lm_factory=lm_rhw(reasonable_orders=true),transform=adapt_costs(one),pref=true,preferred_selection_probability=0.25,random_seed=%s)" % seed, "--evaluator", "hff=ff(transform=adapt_costs(one))", '--search', 'lazy_greedy([hff,hlm],preferred=[hff,hlm], cost_type=one,reopen_closed=false)'],
        #         build_options=BUILD_OPTIONS, driver_options=DRIVER_OPTIONS))

    #     configs.add(IssueConfig('ff-po-0.25-%s' % seed, ['--evaluator', 'hff=ff(preferred_selection_probability=0.25,random_seed=%s)' % seed, '--search', 'lazy(alt([single(hff), single(hff, pref_only=true)]), preferred=[hff])'],
    #             build_options=BUILD_OPTIONS, driver_options=DRIVER_OPTIONS))

    exp = IssueExperiment(
        revisions=revisions,
        configs=configs,
        environment=environment,
    )
    exp.add_suite(benchmarks_dir, suite)

    #exp.add_parser(exp.LAB_STATIC_PROPERTIES_PARSER)
    #exp.add_parser(exp.LAB_DRIVER_PARSER)
    #exp.add_parser(exp.EXITCODE_PARSER)
    #exp.add_parser(exp.TRANSLATOR_PARSER)
    exp.add_parser(exp.SINGLE_SEARCH_PARSER)
    #exp.add_parser(exp.PLANNER_PARSER)

    attributes = exp.DEFAULT_TABLE_ATTRIBUTES

    exp.add_step('build', exp.build)
    exp.add_step('start', exp.start_runs)
    exp.add_fetcher(name='fetch')

   
 
    # exp.add_comparison_table_step(attributes=attributes)

    exp.add_absolute_report_step(attributes=attributes)

    exp.run_steps()

main(revisions=[REVISION])
