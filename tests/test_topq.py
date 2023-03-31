import sys
import unittest
from pathlib import Path
from tests.util import execute_plan, get_planning_tasks

resources = Path(__file__).parent / 'benchmark'
logistics = resources / "logistics-unittest"
miconic = resources / "miconic-unittest"


class TestTopQ(unittest.TestCase):
    def setUp(self) -> None:
        super().setUp()
        self.miconic_planning_tasks = get_planning_tasks(miconic)
        self.logistics_planning_tasks = get_planning_tasks(logistics)

    def test_miconic_unordered_topq(self):
        domain, problem = self.miconic_planning_tasks[0]
        result = execute_plan('iterative-unordered-topq', domain, problem, max_plans=10, quality_bound=1.0)
        costs = [plan['cost'] for plan in result['plans']]
        self.assertEqual(10, len(result['plans']))
        self.assertEqual(6, min(costs))
        self.assertEqual(6, max(costs))

    def test_logistics_unordered_topq(self):
        domain, problem = self.logistics_planning_tasks[0]
        result = execute_plan('iterative-unordered-topq', domain, problem, max_plans=5, quality_bound=1.0)
        costs = [plan['cost'] for plan in result['plans']]
        self.assertEqual(1, len(result['plans']))
        self.assertEqual(20, min(costs))
        self.assertEqual(20, max(costs))
