import sys
import unittest
from pathlib import Path
from tests.util import execute_plan, get_planning_tasks

resources = Path(__file__).parent / 'benchmark'
logistics = resources / "logistics-unittest"
miconic = resources / "miconic-unittest"


class TestTopK(unittest.TestCase):
    def setUp(self) -> None:
        super().setUp()
        self.miconic_planning_tasks = get_planning_tasks(miconic)
        self.logistics_planning_tasks = get_planning_tasks(logistics)

    def test_miconic_iterative_topk(self):
        domain, problem = self.miconic_planning_tasks[0]
        result = execute_plan('iterative-topk', domain, problem, max_plans=5, quality_bound=None)
        costs = [plan['cost'] for plan in result['plans']]
        self.assertEqual(5, len(result['plans']))
        self.assertEqual(6, costs[0])

    def test_logistics_iterative_topk(self):
        domain, problem = self.logistics_planning_tasks[0]
        result = execute_plan('iterative-topk', domain, problem, max_plans=5, quality_bound=None)
        costs = [plan['cost'] for plan in result['plans']]
        self.assertEqual(5, len(result['plans']))
        self.assertEqual(20, costs[0])
