#! /usr/bin/env python

import re

from lab.parser import Parser


"""
[t=0.0125196s, 4292632 KB] Total number of invertible variables is 7
[t=0.0125584s, 4292632 KB] Invertible variables connection status: yes
[t=0.012565s, 4292632 KB] Invertible variables without leafs connection status: no
[t=0.0125697s, 4292632 KB] Invertible variables with one directional connection status: no
[t=0.0127497s, 4292632 KB] Total number of black variables is 3
[t=0.0127543s, 4292632 KB] Total number of black root variables is 3
[t=0.0127583s, 4292632 KB] Total number of variables is 7
[t=0.0127623s, 4292632 KB] Total number of variables with all pairs of values connected is 3
[t=0.0127662s, 4292632 KB] Total number of variables with all values connected to goal is 0
[t=0.0127702s, 4292632 KB] Total number of black variables with strongly connected parents only is 0
[t=0.0127744s, 4292632 KB] Maximal number of side effects for black variable is 0
"""
parser = Parser()
parser.add_pattern("rb_invertible_variables", r"Total number of invertible variables is (\d+)", type=int)
parser.add_pattern("rb_black_variables", r"Total number of black variables is (\d+)", type=int)
parser.add_pattern("rb_black_root_variables", r"Total number of black root variables is (\d+)", type=int)
parser.add_pattern("rb_all_variables", r"Total number of variables is (\d+)", type=int)
parser.add_pattern("rb_variables_dtg_complete", r"Total number of variables with all pairs of values connected is (\d+)", type=int)
parser.add_pattern("rb_variables_all_values_connected_to_goal", r"Total number of variables with all values connected to goal is (\d+)", type=int)
parser.add_pattern("rb_black_variables_only_strongly_connected_parents", r"Total number of black variables with strongly connected parents only is (\d+)", type=int)
parser.add_pattern("rb_max_side_effects_of_black_variables", r"Maximal number of side effects for black variable is (\d+)", type=int)

parser.parse()
