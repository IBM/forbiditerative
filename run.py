#! /usr/bin/env python

import argparse
import sys, os

def make_call(command):
    if (sys.version_info > (3, 0)):
        import subprocess
    else:
        import subprocess32 as subprocess

    subprocess.check_call(command)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument("--alias", help="Alias for running one of the planners", choices=["topk", "topq", "diverse-agl", "diverse-sat"])

    args = parser.parse_args(sys.argv[1:3])
    script_name = os.path.dirname(os.path.abspath(__file__))
    if args.alias == 'topk':
        script_name = os.path.join(script_name,'plan_topk.sh')
    elif args.alias == 'topq':
        script_name = os.path.join(script_name,'plan_topq.sh')
    elif args.alias == 'diverse-agl':
        script_name = os.path.join(script_name,'plan_diverse_agl.sh')
    elif args.alias == 'diverse-sat':
        script_name = os.path.join(script_name,'plan_diverse_sat.sh')
    else:
        exit(1)

    command = [script_name] 
    command.extend(sys.argv[3:])
    make_call(command)
