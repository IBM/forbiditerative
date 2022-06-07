#! /usr/bin/env python

from lab.parser import Parser

parser = Parser()
parser.add_pattern('num_search_generators', 'Number of search generators \(affecting facts\): (\d+)', required=False, type=int)
parser.add_pattern('num_operator_generators', 'Number of identity generators \(on facts, not on operators\): (\d+)', required=False, type=int)
parser.add_pattern('num_total_generators', 'Total number of generators: (\d+)', required=False, type=int)
parser.add_pattern('symmetry_graph_size', 'Size of the grounded symmetry graph: (\d+)', required=False, type=int)
parser.add_pattern('time_symmetries', 'Done initializing symmetries: (.+)s', required=False, type=float)
parser.add_pattern('symmetry_group_order', 'Symmetry group order: (\d+)', required=False, type=int)

parser.parse()
