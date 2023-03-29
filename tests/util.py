import json
import re
import subprocess
import sys
import tempfile
from pathlib import Path
from typing import Dict, List, Literal, Optional, Tuple

DriverCategories = Literal['optimal', 'shortest-optimal', 'satisficing', 'agile']
PlanCategories = Literal['iterative-topk', 'iterative-unordered-topq', 'iterative-subset-topq',
                         'iterative-submultiset-topq']


def get_planning_tasks(path: Path) -> List[Tuple[Path, Path]]:
    domain = path / "domain.pddl"
    return [(domain, problem) for problem in path.glob("p*.pddl")]


def read_plan(plan_file: Path):
    content = plan_file.read_text().splitlines()
    content = [x.strip() for x in content]
    actions = [x[1:-1] for x in content if x.startswith("(")]
    result = {'actions': actions}
    costs = [x for x in content if not x.startswith("(") and "cost" in x]

    # Assuming for now only one such entry
    # mkatz: Assumption does not hold for SymbA, no cost reported
    # assert(len(cost) == 1)
    if len(costs) >= 1:
        q = re.findall(r'; cost = (\d+)', costs[0], re.M)
        result['cost'] = int(q[0])
    return result


def execute_driver(category: DriverCategories, domain: Path, problem: Path):
    categories: Dict[DriverCategories, str] = {
        'optimal': 'seq-opt-oss-por-lmcut',
        'shortest-optimal': 'seq-sopt-oss-por-lmcut',
        'satisficing': 'seq-sat-cerberus2018',
        'agile': 'seq-agl-cerberus2018'
    }
    extra_args: Dict[DriverCategories, List] = {
        'optimal': [],
        'shortest-optimal': [],
        'satisficing': ["--transform-task", "preprocess"],
        'agile': ["--transform-task", "preprocess"],
    }
    with tempfile.NamedTemporaryFile() as temp_file:
        plan_file = Path(tempfile.gettempdir()) / temp_file.name
        subprocess.run(
            [sys.executable, "-m", "driver.main"] +
            ["--plan-file", plan_file.absolute()] +
            extra_args.get(category, []) +
            ["--alias", categories[category]] +
            [str(domain.absolute()), str(problem.absolute())]
        )
        return {'plans': [read_plan(plan_file)]}


def execute_plan(category: PlanCategories, domain: Path, problem: Path, max_plans: int,
                 quality_bound: Optional[float] = None):
    planners: Dict[PlanCategories, str] = {
        'iterative-topk': 'topk',
        'iterative-unordered-topq': 'unordered_topq',
        'iterative-subset-topq': 'subsets_topq',
        'iterative-submultiset-topq': 'submultisets_topq'
    }
    if 'topq' in category and quality_bound is None:
        raise ValueError("`quality-bound` is required if using a topq planner.")

    with tempfile.NamedTemporaryFile() as temp_file:
        plan_file = Path(tempfile.gettempdir()) / temp_file.name
        subprocess.run(
            [sys.executable, "-m", "forbiditerative.plan"] +
            ["--planner", planners[category]] +
            ["--domain", str(domain.absolute()), "--problem", str(problem.absolute())] +
            ["--number-of-plans", str(max_plans)] +
            (["--quality-bound", str(quality_bound)] if quality_bound is not None else []) +
            ["--symmetries", "--use-local-folder", "--clean-local-folder", "--plans-as-json"] +
            ["--results-file", str(plan_file.absolute())]
        )
        result = json.loads(plan_file.read_text())
        for plan in result['plans']:
            plan['cost'] = int(plan['cost'])
        return result
