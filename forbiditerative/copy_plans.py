#! /usr/bin/env python

import json
import os, glob, shutil
import logging

# import planner_call
from forbiditerative.planner_call import PlansToJsonPlannerCall, make_call

def get_plan_files(folder=None):
    plans = "sas_plan*"
    if folder:
        plans = os.path.join(folder, plans)
    return glob.glob(plans)


def get_path_to_file_folder(dir, local_folder=None):
    if local_folder is None:
        return dir
    return os.path.join(local_folder, dir)

def get_found_plans_dir(local_folder=None):
    FOUND_PLANS_DIR = "found_plans"
    return get_path_to_file_folder(FOUND_PLANS_DIR, local_folder)

def get_found_optimal_plans_dir(local_folder=None):
    return os.path.join(get_found_plans_dir(local_folder), "done")

def get_found_non_optimal_plans_dir(local_folder=None):
    return os.path.join(get_found_plans_dir(local_folder), "tmp")

def os_clean_all(local_folder=None):
    reformulated_output = get_path_to_file_folder('reformulated_output.*', local_folder)
    sas_plan = get_path_to_file_folder('sas_plan*', local_folder)
    input_plans_folder_name_sas_plan = os.path.join(get_found_plans_dir(local_folder), sas_plan)
    input_plans_folder_name_optimal_sas_plan = os.path.join(get_found_optimal_plans_dir(local_folder), sas_plan)
    input_plans_folder_name_nonoptimal_sas_plan = os.path.join(get_found_non_optimal_plans_dir(local_folder), sas_plan)

    for name in glob.glob(reformulated_output) + \
                glob.glob(sas_plan) + \
                glob.glob(input_plans_folder_name_sas_plan) + \
                glob.glob(input_plans_folder_name_optimal_sas_plan) + \
                glob.glob(input_plans_folder_name_nonoptimal_sas_plan):
        if os.path.exists(name):
            os.remove(name)


def os_mv(from_file, to_file, local_folder):
    if not os.path.exists(os.path.join(local_folder, from_file)):
        return False
    os.rename(os.path.join(local_folder, from_file), os.path.join(local_folder, to_file))
    return True


def keep_found_plans(optimal_plans, non_optimal_plans, intermediate_plans, local_folder):
    if not os.path.exists(get_found_plans_dir(local_folder)):
        os.makedirs(get_found_plans_dir(local_folder))
    if not os.path.exists(get_found_optimal_plans_dir(local_folder)):
        os.makedirs(get_found_optimal_plans_dir(local_folder))
    if not os.path.exists(get_found_non_optimal_plans_dir(local_folder)):
        os.makedirs(get_found_non_optimal_plans_dir(local_folder))

    if optimal_plans:
        for name in optimal_plans:
            os_mv(name, os.path.join(get_found_optimal_plans_dir(),os.path.basename(name)), local_folder)
    if non_optimal_plans:
        for name in non_optimal_plans:
            os_mv(name, os.path.join(get_found_non_optimal_plans_dir(),os.path.basename(name)), local_folder)
    if intermediate_plans:
        for name in intermediate_plans:
            os_mv(name, os.path.join(get_found_plans_dir(),os.path.basename(name)), local_folder)


def get_number_of_plans(local_folder):
    return len(glob.glob(os.path.join(get_found_optimal_plans_dir(local_folder), 'sas_plan*'))), \
            len(glob.glob(os.path.join(get_found_non_optimal_plans_dir(local_folder), 'sas_plan*')))


def create_local_folder(create_found_plans_dir=True):
    import uuid
    local_path = os.path.join("/", "tmp" , "planner_runs")
    if not os.path.exists(local_path):
        os.makedirs(local_path)
    local_folder_name = os.path.join(local_path , str(uuid.uuid4()).lower())
    if not os.path.exists(local_folder_name):
        os.makedirs(local_folder_name)

    if create_found_plans_dir:
        input_plans_folder_name = os.path.join(local_folder_name, get_found_plans_dir())
        if not os.path.exists(input_plans_folder_name):
            os.makedirs(input_plans_folder_name)
        return input_plans_folder_name
    return local_folder_name

def get_dest_plans():
    return glob.glob(os.path.join(get_dest_plans_folder(), 'sas_plan*'))

def get_dest_optimal_plans():
    return glob.glob(os.path.join(get_dest_optimal_plans_folder(), 'sas_plan*'))

def get_dest_non_optimal_plans():
    return glob.glob(os.path.join(get_dest_non_optimal_plans_folder(), 'sas_plan*'))

def get_dest_plans_folder():
    return os.path.join(os.getcwd(), get_found_plans_dir())

def get_dest_optimal_plans_folder():
    return os.path.join(os.getcwd(), get_found_optimal_plans_dir())

def get_dest_non_optimal_plans_folder():
    return os.path.join(os.getcwd(), get_found_non_optimal_plans_dir())


def copy_found_plans_back(local_folder):
    # Assuming current work directory
    logging.info("copying back to current work directory") 
    dest_plans_folder = get_dest_plans_folder()
    if not os.path.exists(dest_plans_folder):
        os.makedirs(dest_plans_folder)
    dest_opt = get_dest_optimal_plans_folder()
    dest_non_opt = get_dest_non_optimal_plans_folder()
    for f in [dest_opt, dest_non_opt]:
        if not os.path.exists(f):
            os.makedirs(f)

    for plan_file_path in glob.glob(os.path.join(local_folder, get_found_plans_dir(), 'sas_plan*')):
        #logging.info("copying %s to %s" % (plan_file_path, dest_plans_folder))
        shutil.copy2(plan_file_path, dest_plans_folder)

    for plan_file_path in glob.glob(os.path.join(get_found_optimal_plans_dir(local_folder), 'sas_plan*')):
        #logging.info("copying %s to %s" % (plan_file_path, dest_opt))
        shutil.copy2(plan_file_path, dest_opt)
    for plan_file_path in glob.glob(os.path.join(get_found_non_optimal_plans_dir(local_folder), 'sas_plan*')):
        #logging.info("copying %s to %s" % (plan_file_path, dest_non_opt))
        shutil.copy2(plan_file_path, dest_non_opt)

def delete_found_plans(local_folder):
    logging.info("deleting from local folder")
    all_plans = glob.glob(os.path.join(get_found_plans_dir(local_folder), 'sas_plan*')) + \
                glob.glob(os.path.join(get_found_optimal_plans_dir(local_folder), 'sas_plan*')) + \
                glob.glob(os.path.join(get_found_non_optimal_plans_dir(local_folder), 'sas_plan*'))
    for plan_file_path in all_plans:
        #logging.info("deleting %s" % plan_file_path)
        os.remove(plan_file_path)

def move_found_plans_back(local_folder):
    # Assuming current work directory

    dest_plans_folder = os.path.join(os.getcwd(), get_found_plans_dir())
    if not os.path.exists(dest_plans_folder):
        os.makedirs(dest_plans_folder)
    dest_opt = os.path.join(os.getcwd(), get_found_optimal_plans_dir())
    dest_non_opt = os.path.join(os.getcwd(), get_found_non_optimal_plans_dir())
    for f in [dest_opt, dest_non_opt]:
        if not os.path.exists(f):
            os.makedirs(f)

    for plan_file_path in glob.glob(os.path.join(local_folder, get_found_plans_dir(), 'sas_plan*')):
        file_name = os.path.basename(plan_file_path)
        logging.info("moving %s to %s" % (plan_file_path, dest_plans_folder))
        os.rename(plan_file_path, os.path.join(dest_plans_folder, file_name))

    for plan_file_path in glob.glob(os.path.join(get_found_optimal_plans_dir(local_folder), 'sas_plan*')):
        file_name = os.path.basename(plan_file_path)
        logging.info("moving %s to %s" % (plan_file_path, dest_opt))
        os.rename(plan_file_path, os.path.join(dest_opt, file_name))
    for plan_file_path in glob.glob(os.path.join(get_found_non_optimal_plans_dir(local_folder), 'sas_plan*')):
        file_name = os.path.basename(plan_file_path)
        logging.info("moving %s to %s" % (plan_file_path, dest_non_opt))
        os.rename(plan_file_path, os.path.join(dest_non_opt, file_name))

def check_if_sas_format(plan_file):
    ## For now, parsing file name and checking whether it is called sas_plan.*
    fname = os.path.basename(plan_file)
    fname_no_ext = os.path.splitext(fname)[0]
    #logging.info("Checking if sas format %s" % fname_no_ext)
    return fname_no_ext == 'sas_plan'

def copy_plans_to_folder_no_rename(plans, input_plans_folder_name):
    for plan_file in plans:
        #logging.info("Copying file %s to folder %s" % (plan_file, input_plans_folder_name))
        shutil.copy2(plan_file, input_plans_folder_name)
    return input_plans_folder_name, len(plans)

def copy_plans_to_folder_rename(plans, input_plans_folder_name):
    file_id = 0
    for plan_file in plans:
        file_id += 1
        new_plan_file_name = os.path.join(input_plans_folder_name, "sas_plan.%s" % file_id)
        shutil.copy2(plan_file, new_plan_file_name)
        
    return input_plans_folder_name, file_id

def extract_action_name_and_cost(action):
    #  0.0000: (FLY PLANE2 CITY3 CITY5 FL4 FL3) [1.0000]
    #logging.info(action) 
    act_name = action.split('(', 1)[1].split(')')[0]
    cost = int(float(action.split('[', 1)[1].split(']')[0]))
    return "(%s)" % act_name , cost

def check_if_lpg(plan_file):
    with open(plan_file, "r") as pf:
        if "LPG" in pf.read():
            return True
    return False

def copy_plans_to_folder_lpg(plans, input_plans_folder_name):
    file_id = 0
    for plan_file in plans:
        file_id += 1
        new_plan_file_name = os.path.join(input_plans_folder_name, "sas_plan.%s" % file_id)
        ## Reading the file
        with open(plan_file, "r") as pf:
            lines = pf.readlines()
            valid_actions_raw1 = [action.rstrip('\n') for action in lines if not action.startswith(";")]
            valid_actions_raw2 = [action.lower() for action in valid_actions_raw1 if action.strip()]
            #logging.info(valid_actions_raw1)
            #logging.info(valid_actions_raw2)
            actions = [extract_action_name_and_cost(action) for action in valid_actions_raw2]
            
            with open(new_plan_file_name, "w") as wpf:                        
                total_cost = 0
                unit_cost = True
                for action, cost in actions:
                    wpf.write(action)
                    wpf.write("\n")
                    total_cost += cost
                    if cost != 1:
                        unit_cost = False

                cost_type = "unit"
                if not unit_cost:
                    cost_type = "general"
                wpf.write("; cost = %s (%s cost)\n" % (total_cost, cost_type))
        
    return input_plans_folder_name, file_id

        
def copy_plans_to_folder(args):
    # Preparing the plan files for input
    if args.from_folder:
        # Assuming the input in local folder is in Fast Downward format
        num_plans = len([name for name in os.listdir(get_found_plans_dir()) if os.path.isfile(os.path.join(get_found_plans_dir(), name))])
        return get_found_plans_dir(), num_plans

    # If not from local folder, then from properties file
    input_plans_folder_name = create_local_folder()
    # Copying the plans 
    with open(args.properties_file) as pfile:
        data = json.load(pfile)
        props = data['algorithms'][args.algorithm]
        if len(props['plan_files']) == 0:
            return None, 0
        if check_if_lpg(props['plan_files'][0]):
            return copy_plans_to_folder_lpg(props['plan_files'], input_plans_folder_name)
        if check_if_sas_format(props['plan_files'][0]):
            return copy_plans_to_folder_no_rename(props['plan_files'], input_plans_folder_name)

        # Turning into sas format using val (validating, parsing the output, writing to file)
        # creating files with new names, sas_plan.id, running ids
        # Disabled
        #return copy_plans_to_folder_validate(props, input_plans_folder_name)
        # Instead, just renaming the plans
        return copy_plans_to_folder_rename(props['plan_files'], input_plans_folder_name)



def map_back_fast_downward_plan_file(name):
    lines = []
    with open(name, 'r') as f:
        content = f.readlines()
        for line in content:            
            stripped_line = line.strip()
            if not stripped_line:
                continue
            if stripped_line.startswith("(") and stripped_line.endswith(")"):
                ## Removing the added part
                a = stripped_line[:-1].split("__###__")[0]
                lines.append(a + ")\n")
                continue
            lines.append(line)

    with open(name, 'w') as f:
        f.writelines(lines)


def read_plan_and_get_cost(name):
    ## Reading from the last line in the file:
    ## ; cost = 11 (unit cost)
    import re

    f = open(name, 'r')
    text = f.read()
    f.close()
    p = re.compile(r'cost = (\d+)')
    res = p.findall(text)
#    res = re.search(r'cost = (\d+)', text, re.M)
#    logging.info(text, res)
    assert (len(res) == 1)
    cost = int(res[0])
    return cost


def get_json_from_plans(args, input_plans_folder_name, destination=os.getcwd()):
    ## Running a planner with the folder name and the number of plans to extract
    plans = get_plan_files(input_plans_folder_name)
    pc = PlansToJsonPlannerCall()
    command = pc.get_callstring(domain_file=args.domain, problem_file=args.problem, plans_path=input_plans_folder_name, num_plans=len(plans), results_file=args.results_file)
    # command = planner_call.get_plans_to_json_callstring(args.domain, args.problem, input_plans_folder_name, len(plans), args.results_file)
    try:
        make_call(command=command, time_limit=None, local_folder=destination, enable_output=False)
    except:
        raise