#! /usr/bin/env python

import os, sys
import subprocess

from lab.experiment import Experiment

# In the future, these modules should live in a separate
# "planning" or "solver" package.
from downward import suites
from lab.environments import LocalEnvironment
from lab.reports import Attribute, arithmetic_mean, geometric_mean
from downward.reports.absolute import AbsoluteReport
from downward.reports.compare import ComparativeReport
from lab import tools

import common_setup

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

def get_path_level_up(path):
    return os.path.dirname(path)

def get_planner_dir():
    return get_path_level_up(get_path_level_up(get_base_dir()))

## Suite for optimal


BENCHMARKS_DIR = os.environ['DOWNWARD_BENCHMARKS']

SUITE = common_setup.DEFAULT_OPTIMAL_SUITE

ATTRIBUTES = ['coverage', 'num_plans', 'total_time', 'found_worse_plan', 'proved_no_more_plans']

config_name = 'all-topq' 
config_date = '2022-02-01'

report_name = '%s-%s' % (config_name,config_date)

   
ENV = LocalEnvironment(processes=48)
# Create a new experiment.
exp = Experiment(environment=ENV)
# Add built-in parsers.

# Add custom parser.
exp.add_parser('parser.py')

def add_exp(planner_name, alg, exp_q):
    for task in suites.build_suite(BENCHMARKS_DIR, SUITE):
        run = exp.add_run()
        # Create symbolic links and aliases. This is optional. We
        # could also use absolute paths in add_command().
        run.add_resource('domain', task.domain_file, symlink=True)
        run.add_resource('problem', task.problem_file, symlink=True)
        # We could also use exp.add_resource().
        run.add_command(
            'run-planner',
            [planner_name, '{domain}', '{problem}',  exp_q],
            time_limit=1800,
            memory_limit=4096, soft_stdout_limit=None, hard_stdout_limit=None)
        # AbsoluteReport needs the following properties:
        # 'domain', 'problem', 'algorithm', 'coverage'.
        run.set_property('domain', task.domain)
        run.set_property('problem', task.problem)
        run.set_property('algorithm', "FI-%s-%s" % (alg, exp_q) )
        run.set_property('q', exp_q)
        run.set_property('domain_file', task.domain_file)
        run.set_property('problem_file', task.problem_file)
        # Every run has to have a unique id in the form of a list.
        # The algorithm name is only really needed when there are
        # multiple algorithms.
        run.set_property('id', [config_name, task.domain, task.problem, alg, str(exp_q)])

planners = { 
"topuq": "plan_unordered_topq.sh",
"topq-k" : "plan_topq_via_topk.sh",
"topq-uq" : "plan_topq_via_unordered_topq.sh"
}

for q in [1.0, 1.1]:
    for alg in planners:
        planner_name = os.path.join(get_planner_dir(), planners[alg])
        add_exp(planner_name, alg, q)


# Add step that writes experiment files to disk.
exp.add_step('build', exp.build)

# Add step that executes all runs.
exp.add_step('start', exp.start_runs)

# Add step that collects properties from run directories and
# writes them to *-eval/properties.
exp.add_fetcher(name='fetch')
# exp.add_fetcher('data/')

# Make a report.
exp.add_report(AbsoluteReport(attributes=ATTRIBUTES), outfile='%s.html' % report_name)

# Parse the commandline and run the specified steps.
exp.run_steps()
