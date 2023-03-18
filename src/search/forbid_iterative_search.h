#ifndef FORBID_ITERATIVE_SEARCH_H
#define FORBID_ITERATIVE_SEARCH_H

#include "search_engine.h"
#include "../plans/plans_graph.h"
#include "utils/hash.h"

#include <vector>

class Group;
class Heuristic;

namespace options {
class OptionParser;
class Options;
}


namespace plans {
enum class PlanOrdersReduction;
}

enum class TaskReformulationType {
	NONE,
	NONE_FIND_ADDITIONAL_PLANS,
	FORBID_SINGLE_PLAN, 
	FORBID_MULTIPLE_PLANS,
	FORBID_SINGLE_PLAN_MULTISET,
    FORBID_MULTIPLE_PLAN_MULTISETS,
    FORBID_MULTIPLE_PLAN_SUPERSETS,
    FORBID_MULTIPLE_PLAN_SUPERMULTISETS
};


class ForbidIterativeSearch: public SearchEngine {
private:
    // bool dump_forbid_plans_reformulation;
    // bool dump_forbid_multiset_reformulation;
    // bool dump_forbid_multisets_reformulation;
    // bool dump_forbid_plan_reformulation;
    std::shared_ptr<Group> symmetry_group;
    int number_of_plans_to_read;
    bool dump_states_json;
    bool dump_causal_links_json;

    plans::PlanOrdersReduction reduce_plan_orders;

    TaskReformulationType reformulate;
    bool change_operator_names;
    int number_of_plans;
    bool dump_debug_info;
    bool dumping_plans_files;
    int read_plans_and_dump_graph;
    int number_of_edges_until_greedy_clean;
    std::string external_plan_file;
    std::string external_plans_path;
    std::string json_file_to_dump;
    bool is_external_plan_file;
    bool is_external_plans_path;
    bool is_json_file_to_dump;

    // Components associated with deduplication
    // 1. A flag to indicate whether deduplication is enabled
    // 2. A predefined string of action name prefix (pr)
    //    such that duplicate will be name original_action_name + pr + #no
    bool is_deduplicating;
    std::string duplicate_prefix;
    // 2. A function to extend multisets to consider potential duplicates
    void extend_for_duplicates(std::unordered_map<int, int>& multiset) const;
    // 3. A function to identify the original action name
    std::string get_original_name(std::string act_name) const;

    void get_plan_for_op_ids(const std::vector<int>& plan_ids, Plan& plan) const;

    plans::PlansGraph* create_forbid_graph_and_dump_multiple_plans(bool optimal, const Plan &current_plan);

    void reformulate_and_dump_single_plan(const char* filename, const Plan &current_plan) const;
    void reformulate_and_dump_multiple_plans_graph(const char* filename, bool optimal, const Plan &current_plan);
    void reformulate_and_dump_multiset(const char* filename, std::vector<Plan> &current_plans);
    void reformulate_and_dump_superset(const char* filename, std::vector<Plan> &current_plans);
    void reformulate_and_dump_read_plans_and_dump_graph(const char* filename, std::vector<Plan> &current_plans) const;
    // void compute_metrics(const options::Options &opts, const std::vector<std::vector<const GlobalOperator *>>& plans);

    std::shared_ptr<AbstractTask> create_reformulated_task(std::vector<std::vector<int>>& plans) const;
    std::shared_ptr<AbstractTask> create_reformulated_task_multiset(std::vector<std::vector<int>>& plans) const;
    std::shared_ptr<AbstractTask> create_reformulated_task_multisets(std::vector<std::vector<int>>& plans) const;
    std::shared_ptr<AbstractTask> create_reformulated_task_super_multisets(std::vector<std::vector<int>>& plans) const;
    std::shared_ptr<AbstractTask> create_reformulated_task_supersets(std::vector<std::vector<int>>& plans) const;

    void plan_to_multiset(const std::vector<int>& plan, std::unordered_map<int, int>& plan_multiset) const;
    bool multiset_union(std::unordered_map<int, int>& multiset, const std::unordered_map<int, int>& from_multiset) const;
    bool enough_plans_found(int num_found_plans) const;
    void dump_plan_json(const Plan& plan, std::ostream& os) const;

protected:

    virtual void initialize() {}
    virtual SearchStatus step();

    void reformulate_and_dump(bool optimal, std::vector<Plan> &plans);
public:
    ForbidIterativeSearch(const options::Options &opts);
    virtual ~ForbidIterativeSearch();
    virtual void print_statistics() const;
//    virtual void save_plan_if_necessary() const;
    static void add_forbid_plan_reformulation_option(options::OptionParser &parser);

};


#endif
