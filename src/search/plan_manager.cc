#include "plan_manager.h"

#include "task_proxy.h"

#include "task_utils/task_properties.h"
#include "utils/logging.h"

#include <fstream>
#include <iostream>
#include <sstream>


using namespace std;

int calculate_plan_cost(const Plan &plan, const TaskProxy &task_proxy) {
    OperatorsProxy operators = task_proxy.get_operators();
    int plan_cost = 0;
    for (OperatorID op_id : plan) {
        plan_cost += operators[op_id].get_cost();
    }
    return plan_cost;
}

PlanManager::PlanManager()
    : plan_filename("sas_plan"),
      num_previously_generated_plans(0),
      is_part_of_anytime_portfolio(false) {
}

void PlanManager::set_plan_filename(const string &plan_filename_) {
    plan_filename = plan_filename_;
}

void PlanManager::set_num_previously_generated_plans(int num_previously_generated_plans_) {
    num_previously_generated_plans = num_previously_generated_plans_;
}

void PlanManager::set_is_part_of_anytime_portfolio(bool is_part_of_anytime_portfolio_) {
    is_part_of_anytime_portfolio = is_part_of_anytime_portfolio_;
}

void PlanManager::save_plan(
    const Plan &plan, const TaskProxy &task_proxy,
    bool generates_multiple_plan_files) {
    ostringstream filename;
    filename << plan_filename;
    int plan_number = num_previously_generated_plans + 1;
    if (generates_multiple_plan_files || is_part_of_anytime_portfolio) {
        filename << "." << plan_number;
    } else {
        assert(plan_number == 1);
    }
    ofstream outfile(filename.str());
    if (outfile.rdstate() & ofstream::failbit) {
        cerr << "Failed to open plan file: " << filename.str() << endl;
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }
    OperatorsProxy operators = task_proxy.get_operators();
    for (OperatorID op_id : plan) {
        cout << operators[op_id].get_name() << " (" << operators[op_id].get_cost() << ")" << endl;
        outfile << "(" << operators[op_id].get_name() << ")" << endl;
    }
    int plan_cost = calculate_plan_cost(plan, task_proxy);
    bool is_unit_cost = task_properties::is_unit_cost(task_proxy);
    outfile << "; cost = " << plan_cost << " ("
            << (is_unit_cost ? "unit cost" : "general cost") << ")" << endl;
    outfile.close();
    utils::g_log << "Plan length: " << plan.size() << " step(s)." << endl;
    utils::g_log << "Plan cost: " << plan_cost << endl;
    ++num_previously_generated_plans;
}

void PlanManager::load_plan(string path_to_plan_file, Plan &plan, const TaskProxy &task_proxy) const {
    std::unordered_map<string, OperatorID> ops_by_names;
    compute_ops_by_names(ops_by_names, task_proxy);
    load_single_plan(path_to_plan_file, ops_by_names, plan);
}

void PlanManager::load_plans(string path_to_plan_folder, int num_plans, vector<Plan> &plans, const TaskProxy &task_proxy) const {
    // Assumption: the files are named sas_plan.1 .. sas_plan.num_plans
    // TODO: make it work with other OS
    std::unordered_map<string, OperatorID> ops_by_names;
    compute_ops_by_names(ops_by_names, task_proxy);
    for (int plan_no=1; plan_no <= num_plans; ++plan_no) {
        string fname = path_to_plan_folder + "/" + "sas_plan." + std::to_string(plan_no);
        //cout << "FNAME: " << fname << endl;
        Plan plan;
        load_single_plan(fname, ops_by_names, plan);
        plans.push_back(plan);
    }
}

void PlanManager::load_single_plan(std::string path_to_plan_file,
        const std::unordered_map<std::string, OperatorID>& ops_by_names, Plan &plan) const {

    utils::g_log << "Reading plan from file " << path_to_plan_file << endl;
    ifstream planfile;
    planfile.open(path_to_plan_file);

    if (!planfile.is_open()) {
        throw std::system_error(errno, std::system_category(), "failed to open file");

        cerr << "File is not open!" << endl;
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }
    string line;
    //cout << "-----------------------------------------------------------------------" << endl;
    while(std::getline(planfile, line)) {
        if (line.size() == 0 || line[0] == ';')
            continue;
        string op_name = line.substr(1, line.size()-2);
        //op_name = op_name.substr(0, op_name.find("__###__"));
        //cout << op_name << endl;
        auto it = ops_by_names.find(op_name);
        if (it == ops_by_names.end()) {
            // Trying adding a trailing space
            string op_name_trailing_space = op_name + " ";
            it = ops_by_names.find(op_name_trailing_space);
            if (it == ops_by_names.end()) {
                cerr << "#" << op_name << "#   Operator not found!!!" << endl;
                cerr << "Operator names:" << endl;
                for (auto name : ops_by_names) {
                    cerr << "#" << name.first << "#" << endl;
                }
                utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
            }
        }
        //cout << "Name " << it->first << endl;
        //cout << "Found operator " << it->second->get_name() << endl;
        plan.push_back(it->second);
    }
}

void PlanManager::compute_ops_by_names(unordered_map<string, OperatorID>& ops_by_names, const TaskProxy &task_proxy) const {
    // Creating a hashmap from operator names to ids
    OperatorsProxy operators = task_proxy.get_operators();
    for (OperatorProxy op : operators) {
        ops_by_names.insert({op.get_name(), OperatorID(op.get_id())});
        // ops_by_names[op.get_name()] = OperatorID(op.get_id());
    }
}
