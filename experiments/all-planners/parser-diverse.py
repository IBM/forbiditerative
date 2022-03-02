#! /usr/bin/env python
# -*- coding: utf-8 -*-


import re
import glob, os, sys
import json
import itertools

from lab.parser import Parser

print("Parsing with custom parser")

_PLAN_INFO_REGEX = re.compile(r"; cost = (\d+) \((unit cost|general cost)\)\n")

def get_plan_cost(path):
    try:
        with open(path) as input_file:
            line = None
            for line in input_file:
                if line.strip().startswith(";"):
                    continue
            # line is now the last line
            match = _PLAN_INFO_REGEX.match(line)
            if match:
                return int(match.group(1))
            return None
    except:
        return None


def get_plan_costs(plans_folder):
    ret = []
    for counter in itertools.count(1):
        name = "sas_plan.%d" % counter
        plan_filename = os.path.join(plans_folder, name)
        if not os.path.exists(plan_filename):
            break
        cost = get_plan_cost(plan_filename)
        if cost is not None:
            ret.append(cost)
    return ret

def plans(content, props):
    costs = get_plan_costs(os.path.join('found_plans' , 'done'))
    costs.extend(get_plan_costs('found_plans'))
    props["plan_costs"] = costs
    props["num_plans"] = len(costs)


def all_found_not_enough(content, props):
    props["all_found"] = int(
        "These are all plans of quality up to the specified bound" in content
    )
    props["not_enough_plans"] = int(
        "The number of plans found in the first phase is smaller than the requested number of plans" in content
    )


def unsolvable(content):
    p = re.findall(r'Completely explored state space -- no solution', content, re.M)
    q = re.findall(r'External planner proved that there are no more plans', content, re.M)
    return len(p) > 0 or len(q) > 0 


def get_data_from_static():
    with open("static-properties", 'r') as sp:
        return json.load(sp)

def get_k(props):
    return 10000


def coverage(content, props):
    finished = 'All iterations are done' in content
    found_worse_plan = "extra_plans_cost" in props
    proved_no_more_plans = unsolvable(content)
    props["found_worse_plan"] = int(found_worse_plan)
    props["proved_no_more_plans"] = int(proved_no_more_plans)
    #k_bound_reached = props["num_plans"] >= get_k(props)
    props["coverage"] = int(finished and (found_worse_plan or proved_no_more_plans))


def total_time(content, props):
    p = re.findall(r'INFO     All iterations are done \[(.+)s CPU, (.+)s wall-clock\]', content, re.M)
    if len(p) > 0:
        props["total_time"] = float(p[0][0])   


parser = Parser()
parser.add_function(plans)
parser.add_function(all_found_not_enough)
parser.add_function(coverage)
parser.add_function(total_time)

parser.parse()
