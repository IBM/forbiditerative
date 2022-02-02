#! /usr/bin/env python
# -*- coding: utf-8 -*-

import os
import subprocess

from lab.environments import LocalEnvironment
from lab.reports import Attribute, arithmetic_mean, geometric_mean
from downward.reports.absolute import AbsoluteReport
from downward.reports.compare import ComparativeReport
# from downward.reports.scatter import ScatterPlotReport
from lab import tools

from common_setup import IssueConfig, IssueExperiment 



REVISION = 'c2d153b863bd8256f5a4289c98f4a5f14689c2ab'


def main(revisions=None):
    benchmarks_dir = '/data/software/ISR-benchmarks'
    suite = ["grid-lifted", "queen-lifted", "grid-split", "queen-split"]
    environment = LocalEnvironment(processes=48)

    BUILD_OPTIONS = []
    DRIVER_OPTIONS = ["--overall-time-limit", "60m", "--overall-memory-limit", "4096M"]

    configs = set()

    configs.add(IssueConfig('cerberus-nffpo-co1', ["--evaluator",
        "hlm=lmcount(lm_rhw(reasonable_orders=true),transform=adapt_costs(one),pref=false)",
        "--evaluator", 
        "hrb=RB(dag=from_coloring, extract_plan=true, transform=adapt_costs(one))",
        "--evaluator", 
        "hn=novelty_test(evals=[hrb], type=separate_both, pref=true, cutoff_type=all_ordered,cutoff_bound=1)",
        "--search", 'lazy(open=alt([tiebreaking([hn, hrb]), single(hn,pref_only=true), single(hlm)]), preferred=[hn], cost_type=one,reopen_closed=false)'],
                build_options=BUILD_OPTIONS, driver_options=DRIVER_OPTIONS))


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

   
    exp.add_fetcher('data/IJCAI2021-noveltyops-2021-01-17-red-black-base-eval')
    # exp.add_comparison_table_step(attributes=attributes)

    def rename_algorithms(run):
        name = run["algorithm"]
        paper_names = {
            '{}-{}'.format(REVISION, 'cerberus'): "cerberus",
            '{}-{}'.format(REVISION, 'cerberus-nffpo'): "cerberus-nffpo"}
        
        run["algorithm"] = paper_names.get(name, name)
        return run
    algs = ["cerberus", "cerberus-nffpo"]

    exp.add_absolute_report_step(attributes=attributes, filter=rename_algorithms, filter_algorithm=algs)

    # pairs = [ ('{}-{}'.format(REVISION, 'sbff-ffnpo-all-ordered'), '{}-{}'.format(REVISION, 'sbff-ffnpo-all-ordered-cutoff0') ), 
    # ('{}-{}'.format(REVISION, 'sbff-ffpo'), '{}-{}'.format(REVISION, 'sbff-ffnpo-argmax') )]
    pairs = [ ("cerberus", "cerberus-nffpo")]
    exp.add_report(
        ComparativeReport(filter=rename_algorithms, 
            algorithm_pairs=pairs,
            attributes=attributes,
        ),
        outfile=os.path.join(exp.eval_dir, 'a' + exp.name + '-compare.html'),
    )

    exp.run_steps()

main(revisions=[REVISION])
