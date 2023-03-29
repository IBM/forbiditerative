import sys
import subprocess
import tempfile
import unittest
import re
from pathlib import Path
from typing import Iterator, Tuple, Literal

resources = Path(__file__).parent / 'benchmark'
logistics = resources / "logistics-unittest"
miconic = resources / "miconic-unittest"


def get_planning_tasks(path: Path) -> Iterator[Tuple[Path, Path]]:
    domain = path / "domain.pddl"
    yield from ((domain, problem) for problem in path.glob("p*.pddl"))


def execute_planner(category: Literal['optimal', 'shortest-optimal'], domain: Path, problem: Path):
    categories = {
        'optimal': 'seq-opt-oss-por-lmcut',
        'shortest-optimal': "seq-sopt-oss-por-lmcut"
    }
    with tempfile.NamedTemporaryFile() as temp_file:
        plan_file = Path(tempfile.gettempdir()) / temp_file.name
        subprocess.run([sys.executable, "-m", "driver.main", "--alias", categories[category], "--plan-file", plan_file.absolute(), domain.absolute(), problem.absolute()])

        content = plan_file.read_text().splitlines()
        content = [x.strip() for x in content]
        actions = [x[1:-1] for x in content if x.startswith("(")]
        result = {'actions': actions}
        costs = [x for x in content if not x.startswith("(") and "cost" in x]
        ## Assuming for now only one such entry
        # mkatz: Assumption does not hold for SymbA, no cost reported
        # assert(len(cost) == 1)
        if len(costs) >= 1:
            q = re.findall(r'; cost = (\d+)', costs[0], re.M)
            result['cost'] = int(q[0])

        return result


class TestOptimal(unittest.TestCase):
    def setUp(self) -> None:
        super().setUp()
        self.miconic_planning_tasks = list(get_planning_tasks(miconic))
        self.logistics_planning_tasks = list(get_planning_tasks(logistics))
        sys.dont_write_bytecode = True  # abseil's driver attempts to dynamically load .py files and fails if __pycache__ is generated

    def test_miconic_shortest_optimal(self):
        results = [execute_planner('shortest-optimal', domain, problem) for domain, problem in self.miconic_planning_tasks]
        self.assertEqual(results[0]['cost'], 6)
        self.assertEqual(results[1]['cost'], 26)

    def test_logistics_shortest_optimal(self):
        results = [execute_planner('shortest-optimal', domain, problem) for domain, problem in self.logistics_planning_tasks]
        self.assertEqual(results[0]['cost'], 20)
        self.assertEqual(results[1]['cost'], 40)

    def test_miconic_optimal(self):
        results = [execute_planner('optimal', domain, problem) for domain, problem in self.miconic_planning_tasks]
        self.assertEqual(results[0]['cost'], 6)
        self.assertEqual(results[1]['cost'], 26)

    def test_logistics_optimal(self):
        results = [execute_planner('optimal', domain, problem) for domain, problem in self.logistics_planning_tasks]
        self.assertEqual(results[0]['cost'], 20)
        self.assertEqual(results[1]['cost'], 40)
