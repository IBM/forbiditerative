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


BENCHMARKS_DIR = os.environ["DOWNWARD_BENCHMARKS"]
SUITE = ["agricola-sat18-strips", "airport", "barman-sat11-strips", "barman-sat14-strips", "blocks", "childsnack-sat14-strips", "data-network-sat18-strips", "depot", "driverlog", "elevators-sat08-strips", "elevators-sat11-strips", "floortile-sat11-strips", "floortile-sat14-strips", "freecell", "ged-sat14-strips", "grid", "gripper", "hiking-sat14-strips", "logistics00", "logistics98", "miconic", "movie", "mprime", "mystery", "nomystery-sat11-strips", "openstacks-sat08-strips", "openstacks-sat11-strips", "openstacks-sat14-strips", "openstacks-strips", "organic-synthesis-sat18-strips", "organic-synthesis-split-sat18-strips", "parcprinter-08-strips", "parcprinter-sat11-strips", "parking-sat11-strips", "parking-sat14-strips", "pathways", "pegsol-08-strips", "pegsol-sat11-strips", "pipesworld-notankage", "pipesworld-tankage", "psr-small", "rovers", "satellite", "scanalyzer-08-strips", "scanalyzer-sat11-strips", "snake-sat18-strips", "sokoban-sat08-strips", "sokoban-sat11-strips", "spider-sat18-strips", "storage", "termes-sat18-strips", "tetris-sat14-strips", "thoughtful-sat14-strips", "tidybot-sat11-strips", "tpp", "transport-sat08-strips", "transport-sat11-strips", "transport-sat14-strips", "trucks-strips", "visitall-sat11-strips", "visitall-sat14-strips", "woodworking-sat08-strips", "woodworking-sat11-strips", "zenotravel"]
        

ATTRIBUTES = ['coverage', 'num_plans', 'total_time', 'found_worse_plan', 'proved_no_more_plans']

config_name = 'diverse-agl' 
config_date = '2022-02-15'
report_name = '%s-%s' % (config_name,config_date)

   
ENV = LocalEnvironment(processes=48)
# Create a new experiment.
exp = Experiment(environment=ENV)
# Add built-in parsers.

# Add custom parser.
exp.add_parser('parser-diverse.py')

def add_exp(planner_name, alg, exp_k):
    for task in suites.build_suite(BENCHMARKS_DIR, SUITE):
        run = exp.add_run()
        # Create symbolic links and aliases. This is optional. We
        # could also use absolute paths in add_command().
        run.add_resource('domain', task.domain_file, symlink=True)
        run.add_resource('problem', task.problem_file, symlink=True)
        # We could also use exp.add_resource().
        run.add_command(
            'run-planner',
            [planner_name, '{domain}', '{problem}',  exp_k],
            time_limit=1800,
            memory_limit=4096, soft_stdout_limit=None, hard_stdout_limit=None)
        # AbsoluteReport needs the following properties:
        # 'domain', 'problem', 'algorithm', 'coverage'.
        run.set_property('domain', task.domain)
        run.set_property('problem', task.problem)
        run.set_property('algorithm', "diverse-%s-%s" % (alg, exp_k) )
        run.set_property('k', exp_k)
        run.set_property('domain_file', task.domain_file)
        run.set_property('problem_file', task.problem_file)
        # Every run has to have a unique id in the form of a list.
        # The algorithm name is only really needed when there are
        # multiple algorithms.
        run.set_property('id', [config_name, task.domain, task.problem, alg, str(exp_k)])

planners = { "da": "plan_diverse_agl.sh"}

for k in [1000]:
    for alg in planners:
        planner_name = os.path.join(get_planner_dir(), planners[alg])
        add_exp(planner_name, alg, k)


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
