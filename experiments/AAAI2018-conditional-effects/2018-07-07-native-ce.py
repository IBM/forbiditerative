#! /usr/bin/env python
# -*- coding: utf-8 -*-

import os
import subprocess

from lab.environments import LocalEnvironment, GridEnvironment
from lab.reports import Attribute, arithmetic_mean, geometric_mean
from downward.reports.absolute import AbsoluteReport
from downward.reports.compare import ComparativeReport
from lab import tools

from common_setup import IssueConfig, IssueExperiment 

class OracleGridEngineEnvironment(GridEnvironment):
    """Abstract base class for grid environments using OGE."""
    # Must be overridden in derived classes.
    DEFAULT_QUEUE = None

    # Can be overridden in derived classes.
    JOB_HEADER_TEMPLATE_FILE = 'oge-job-header'
    RUN_JOB_BODY_TEMPLATE_FILE = 'oge-run-job-body'
    STEP_JOB_BODY_TEMPLATE_FILE = 'oge-step-job-body'
    DEFAULT_PRIORITY = 0
    HOST_RESTRICTIONS = {}
    DEFAULT_HOST_RESTRICTION = ""

    def __init__(self, queue=None, priority=None, host_restriction=None, **kwargs):
        """
        *queue* must be a valid queue name on the grid.

        *priority* must be in the range [-1023, 0] where 0 is the
        highest priority. If you're a superuser the value can be in the
        range [-1023, 1024].

        See :py:class:`~lab.environments.GridEnvironment` for inherited
        parameters.

        """
        GridEnvironment.__init__(self, **kwargs)

        if queue is None:
            queue = self.DEFAULT_QUEUE
        if priority is None:
            priority = self.DEFAULT_PRIORITY
        if host_restriction is None:
            host_restriction = self.DEFAULT_HOST_RESTRICTION

        self.queue = queue
        self.priority = priority
        assert self.priority in xrange(-1023, 1024 + 1)
        self.host_spec = self._get_host_spec(host_restriction)

    # TODO: Don't forget to remove the run-dispatcher file once we get rid
    #       of OracleGridEngineEnvironment.
    def _write_run_dispatcher(self):
        dispatcher_content = tools.fill_template(
            'run-dispatcher.py',
            task_order=self._get_task_order())
        self.exp.add_new_file(
            '', 'run-dispatcher.py', dispatcher_content, permissions=0o755)

    def write_main_script(self):
        # The main script is written by the run_steps() method.
        self._write_run_dispatcher()

    def _get_job_params(self, step, is_last):
        job_params = GridEnvironment._get_job_params(self, step, is_last)
        job_params['priority'] = self.priority
        job_params['queue'] = self.queue
        job_params['host_spec'] = self.host_spec
        job_params['notification'] = '#$ -m n'
        job_params['errfile'] = 'driver.err'

        if is_last and self.email:
            if is_run_step(step):
                logging.warning(
                    "The cluster sends mails per run, not per step."
                    " Since the last of the submitted steps would send"
                    " too many mails, we disable the notification."
                    " We recommend submitting the 'run' step together"
                    " with the 'fetch' step.")
            else:
                job_params['notification'] = '#$ -M %s\n#$ -m e' % self.email

        return job_params

    def _get_host_spec(self, host_restriction):
        if not host_restriction:
            return '## (not used)'
        else:
            hosts = self.HOST_RESTRICTIONS[host_restriction]
            return '#$ -l hostname="%s"' % '|'.join(hosts)

    def _submit_job(self, job_name, job_file, job_dir, dependency=None):
        submit = ['qsub']
        if dependency:
            submit.extend(['-hold_jid', dependency])
        submit.append(job_file)
        tools.run_command(submit, cwd=job_dir)
        return job_name


REVISION = '26939eee96a4'
REVISION = '94f9ac958edb'

try:
    from relativescatter import RelativeScatterPlotReport
    matplotlib = True
except ImportError:
    print 'matplotlib not available, scatter plots not available'
    matplotlib = False

def main(revisions=None):
    benchmarks_dir = os.environ["DOWNWARD_BENCHMARKS_IPC2018"]
    # optimal union satisficing
    suite = []
    suite.extend(['briefcaseworld', 'cavediving-14-adl', 'citycar-sat14-adl', 'fsc-blocks', 'fsc-grid-a1', 'fsc-grid-a2', 'fsc-grid-r', 'fsc-hall', 'fsc-visualmarker', 'gedp-ds2ndp', 'miconic-simpleadl', 't0-adder', 't0-coins', 't0-comm', 't0-grid-dispose', 't0-grid-push', 't0-grid-trash', 't0-sortnet', 't0-sortnet-alt', 't0-uts'])
    suite.extend(["agricola-sat18", "caldera-sat18", "caldera-split-sat18", "data-network-sat18", "flashfill-sat18", "nurikabe-sat18", "organic-synthesis-sat18", "organic-synthesis-split-sat18", "settlers-sat18", "snake-sat18", "spider-sat18", "termes-sat18"])

    environment = OracleGridEngineEnvironment(queue='all.q')

    BUILD_OPTIONS = ["release64"]
    DRIVER_OPTIONS = ["--build", "release64", "--overall-time-limit", "30m", "--overall-memory-limit", "4096M"]

    configs = {
        IssueConfig('rb-ce-cerberus', ['--heuristic', 'hrb=RB(dag=from_coloring, extract_plan=true)', '--search', 'lazy_greedy([hrb],reopen_closed=false)'],
        build_options=BUILD_OPTIONS,
        driver_options=DRIVER_OPTIONS),
    }

    exp = IssueExperiment(
        revisions=revisions,
        configs=configs,
        environment=environment,
    )
    exp.add_suite(benchmarks_dir, suite)

    exp.add_parser(exp.LAB_STATIC_PROPERTIES_PARSER)
    exp.add_parser(exp.LAB_DRIVER_PARSER)
    exp.add_parser(exp.EXITCODE_PARSER)
    exp.add_parser(exp.TRANSLATOR_PARSER)
    exp.add_parser(exp.SINGLE_SEARCH_PARSER)
    #exp.add_parser(exp.PLANNER_PARSER)

    attributes = exp.DEFAULT_TABLE_ATTRIBUTES

    exp.add_step('build', exp.build)
    exp.add_step('start', exp.start_runs)
    exp.add_fetcher(name='fetch')

    exp.add_absolute_report_step(attributes=attributes)
    report_name = os.path.basename(exp.path.rstrip('/'))
    exp.add_step('copy_report', subprocess.call,
       ['cp', os.path.join(exp.eval_dir,'%s.html' % report_name), '/storage/US1J6721/EXTERNAL'])

    exp.add_step('print_dest', subprocess.call,
       ['echo', os.path.join("https://syss063.pok.stglabs.ibm.com/users/mkatz/storage",'%s.html' % report_name)])

    algorithm_nicks = [
        'rb-multiply-out-ce'
    ]

    OTHER_REV = '94f9ac958edb'
    exp.add_fetcher('data/AAAI2018-conditional-effects-2018-07-06-baseline-multiply-out-eval',filter_algorithm=['{}-{}'.format(OTHER_REV, x) for x in algorithm_nicks])

    exp.add_report(
        ComparativeReport(
            algorithm_pairs=[('{}-{}'.format(OTHER_REV, 'rb-multiply-out-ce'), '{}-{}'.format(REVISION, 'rb-ce-cerberus'))],
            attributes=attributes,
        ),
        outfile=os.path.join(exp.eval_dir, 'a' + exp.name + '-compare.html'),
    )

    exp.run_steps()

main(revisions=[REVISION])
