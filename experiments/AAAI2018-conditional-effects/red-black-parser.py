#! /usr/bin/env python

"""
Plan length: 11 step(s).
Plan cost: 11
Initial state h value: 15/9.
Expanded 15 state(s).
Reopened 0 state(s).
Evaluated 16 state(s).
Evaluations: 32
Generated 63 state(s).
Dead ends: 0 state(s).
Best solution cost so far: 11
Solution found - keep searching
Starting search: lazy_wastar(list(hrb, hlm), preferred = list(hrb, hlm), w = 2)
Conducting lazy best first search, (real) bound = 11

[...]

Completely explored state space -- no solution!


Number of invertible variables is 0

---------------------------------------------------------------------------------------
Total number of black variables is 35
Total number of black root variables is 0
Total number of variables is 63
Total number of variables with all pairs of values connected is 0
Total number of variables with all values connected to goal is 0
Total number of black variables with strongly connected parents only is 0
Maximal number of side effects for black variable is 4
---------------------------------------------------------------------------------------
"""

import re
import glob

from lab.parser import Parser


def get_plans(content, props):
    p = re.compile(r'Plan cost: (\d+)')
    res = p.findall(content)
    if len(res) > 0:
        props['plans'] = res

def coverage(content, props):
    if props.get('plans') is not None and len(props.get('plans')) > 0 or props.get('unsolvable'):
        props['coverage'] = 1  

def get_best_plan(content, props):
    if props.get('plans') is not None and len(props.get('plans')) > 0:
	props['plan_cost'] = min(props.get('plans'))


def unsolvable(content, props):
    res = re.findall(r'Completely explored state space', content, re.M)
    #print res
    if len(res) > 0 and props.get('plans') is None:
        props['unsolvable'] = 1

def total_time(content, props):
    p = re.findall(r'Total time: (.+)s$', content, re.M)
    props.pop('total_time', None)

    if len(p) > 0:
        props['total_time'] = max([float(i) for i in p])


parser = Parser()
parser.add_pattern('evaluations', r'Evaluated (\d+) state\(s\).', required=False)
parser.add_pattern('invertible', r'Number of invertible variables is (\d+)', required=False)
parser.add_pattern('black', r'Total number of black variables is (\d+)', required=False)

parser.add_function(get_plans)
parser.add_function(get_best_plan)
parser.add_function(unsolvable)
parser.add_function(coverage)
parser.add_function(total_time)
parser.parse()
