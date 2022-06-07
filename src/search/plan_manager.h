#ifndef PLAN_MANAGER_H
#define PLAN_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>

class OperatorID;
class TaskProxy;

using Plan = std::vector<OperatorID>;

class PlanManager {
    std::string plan_filename;
    int num_previously_generated_plans;
    bool is_part_of_anytime_portfolio;
public:
    PlanManager();

    void set_plan_filename(const std::string &plan_filename);
    void set_num_previously_generated_plans(int num_previously_generated_plans);
    void set_is_part_of_anytime_portfolio(bool is_part_of_anytime_portfolio);

    /*
      Set generates_multiple_plan_files to true if the planner can find more than
      one plan and should number the plans as FILENAME.1, ..., FILENAME.n.
    */
    void save_plan(
        const Plan &plan, const TaskProxy &task_proxy,
        bool generates_multiple_plan_files = false);


    void load_plan(std::string path_to_plan_file, Plan &plan, const TaskProxy &task_proxy) const;
    void load_plans(std::string path_to_plan_folder, int num_plans, std::vector<Plan> &plans, const TaskProxy &task_proxy) const;

    int get_num_previously_generated_plans() const { return num_previously_generated_plans; }
    
private:
    void compute_ops_by_names(std::unordered_map<std::string, OperatorID>& ops_by_names, const TaskProxy &task_proxy) const;
    void load_single_plan(std::string path_to_plan_file,
        const std::unordered_map<std::string, OperatorID>& ops_by_names, Plan &plan) const;

    
};

extern int calculate_plan_cost(const Plan &plan, const TaskProxy &task_proxy);

#endif
