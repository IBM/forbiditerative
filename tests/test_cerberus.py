import sys
import unittest
from pathlib import Path
from tests.util import execute_driver, get_planning_tasks

resources = Path(__file__).parent / 'benchmark'
logistics = resources / "logistics-unittest"


class TestCerberus(unittest.TestCase):
    def setUp(self) -> None:
        super().setUp()
        self.logistics_planning_tasks = get_planning_tasks(logistics)

    @unittest.skip("Skipped because it takes an extremely long time.")
    def test_logistics_satisficing_cerberus(self):
        domain, problem = self.logistics_planning_tasks[0]
        result = execute_driver('satisficing', domain, problem)
        self.assertEqual(20, result['plans'][0]['cost'])

    def test_logistics_agile_cerberus(self):
        domain, problem = self.logistics_planning_tasks[0]
        result = execute_driver('agile', domain, problem)
        self.assertEqual(20, result['plans'][0]['cost'])
