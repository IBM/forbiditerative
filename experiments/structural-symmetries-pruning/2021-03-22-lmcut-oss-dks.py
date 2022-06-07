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
REVISIONS = ['536ecfbc9b14df7777ecc85bd1dd142bfc475f99']
CONFIGS = [
    IssueConfig('lmcut-dks', ['--symmetries', 'sym=structural_symmetries(time_bound=0,search_symmetries=dks)', '--search', 'astar(lmcut(),symmetries=sym,verbosity=silent)']),
    IssueConfig('lmcut-oss', ['--symmetries', 'sym=structural_symmetries(time_bound=0,search_symmetries=oss)', '--search', 'astar(lmcut(),symmetries=sym,verbosity=silent)']),
    IssueConfig('lmcut-dks-stabinit', ['--symmetries', 'sym=structural_symmetries(time_bound=0,search_symmetries=dks,stabilize_initial_state=true)', '--search', 'astar(lmcut(),symmetries=sym,verbosity=silent)']),
    IssueConfig('lmcut-oss-stabinit', ['--symmetries', 'sym=structural_symmetries(time_bound=0,search_symmetries=oss,stabilize_initial_state=true)', '--search', 'astar(lmcut(),symmetries=sym,verbosity=silent)']),
]

SUITE = common_setup.DEFAULT_OPTIMAL_SUITE
ENVIRONMENT = BaselSlurmEnvironment(
    email="silvan.sievers@unibas.ch",
    partition="infai_2",
    export=[],
    # paths obtained via:
    # module purge
    # module -q load CMake/3.15.3-GCCcore-8.3.0
    # module -q load GCC/8.3.0
    # echo $PATH
    # echo $LD_LIBRARY_PATH
    setup='export PATH=/scicore/soft/apps/binutils/2.32-GCCcore-8.3.0/bin:/scicore/soft/apps/CMake/3.15.3-GCCcore-8.3.0/bin:/scicore/soft/apps/cURL/7.66.0-GCCcore-8.3.0/bin:/scicore/soft/apps/bzip2/1.0.8-GCCcore-8.3.0/bin:/scicore/soft/apps/ncurses/6.1-GCCcore-8.3.0/bin:/scicore/soft/apps/GCCcore/8.3.0/bin:/infai/sieverss/repos/bin:/infai/sieverss/local:/export/soft/lua_lmod/centos7/lmod/lmod/libexec:/usr/local/bin:/usr/bin:/usr/local/sbin:/usr/sbin:$PATH\nexport LD_LIBRARY_PATH=/scicore/soft/apps/binutils/2.32-GCCcore-8.3.0/lib:/scicore/soft/apps/cURL/7.66.0-GCCcore-8.3.0/lib:/scicore/soft/apps/bzip2/1.0.8-GCCcore-8.3.0/lib:/scicore/soft/apps/zlib/1.2.11-GCCcore-8.3.0/lib:/scicore/soft/apps/ncurses/6.1-GCCcore-8.3.0/lib:/scicore/soft/apps/GCCcore/8.3.0/lib64:/scicore/soft/apps/GCCcore/8.3.0/lib')

if common_setup.is_test_run():
    SUITE = IssueExperiment.DEFAULT_TEST_SUITE
    ENVIRONMENT = LocalEnvironment(processes=4)

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

REVISION=REVISIONS[0]
exp.add_absolute_report_step(
    filter_algorithm=[
        '{}-lmcut-dks'.format(REVISION),
        '{}-lmcut-oss'.format(REVISION),
        '{}-lmcut-dks-stabinit'.format(REVISION),
        '{}-lmcut-oss-stabinit'.format(REVISION),
    ],
    attributes=attributes,
)

OLD_REVISION='a87197080a97'
# fetch last data from experiments run in scripts repo
exp.add_fetcher(
    'data/2020-07-10-lmcut-oss-dks-eval',
    filter_algorithm=[
        '{}-lmcut-dks'.format(OLD_REVISION),
        '{}-lmcut-oss'.format(OLD_REVISION),
        '{}-lmcut-dks-stabinit'.format(OLD_REVISION),
        '{}-lmcut-oss-stabinit'.format(OLD_REVISION),
    ],
    merge=True
)

exp.add_comparison_table_step(revisions=[OLD_REVISION, REVISION], attributes=attributes)

exp.run_steps()
