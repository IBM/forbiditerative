import sys
import unittest
from pathlib import Path
from tests.util import execute_driver, get_planning_tasks

resources = Path(__file__).parent / 'benchmark'
logistics = resources / "logistics-unittest"
miconic = resources / "miconic-unittest"


class TestOptimal(unittest.TestCase):
    def setUp(self) -> None:
        super().setUp()
        self.miconic_planning_tasks = get_planning_tasks(miconic)
        self.logistics_planning_tasks = get_planning_tasks(logistics)

    def test_miconic_shortest_optimal(self):
        results = [execute_driver('shortest-optimal', domain, problem) for domain, problem in self.miconic_planning_tasks]
        self.assertEqual(1, len(results[0]['plans']))
        self.assertEqual(6, results[0]['plans'][0]['cost'])
        self.assertEqual(1, len(results[1]['plans']))
        self.assertEqual(26, results[1]['plans'][0]['cost'])

    def test_logistics_shortest_optimal(self):
        results = [execute_driver('shortest-optimal', domain, problem) for domain, problem in self.logistics_planning_tasks]
        self.assertEqual(1, len(results[0]['plans']))
        self.assertEqual(20, results[0]['plans'][0]['cost'])
        self.assertEqual(1, len(results[1]['plans']))
        self.assertEqual(40, results[1]['plans'][0]['cost'])

    def test_miconic_optimal(self):
        results = [execute_driver('optimal', domain, problem) for domain, problem in self.miconic_planning_tasks]
        self.assertEqual(1, len(results[0]['plans']))
        self.assertEqual(6, results[0]['plans'][0]['cost'])
        self.assertEqual(1, len(results[1]['plans']))
        self.assertEqual(26, results[1]['plans'][0]['cost'])

    def test_logistics_optimal(self):
        results = [execute_driver('optimal', domain, problem) for domain, problem in self.logistics_planning_tasks]
        self.assertEqual(1, len(results[0]['plans']))
        self.assertEqual(20, results[0]['plans'][0]['cost'])
        self.assertEqual(1, len(results[1]['plans']))
        self.assertEqual(40, results[1]['plans'][0]['cost'])
