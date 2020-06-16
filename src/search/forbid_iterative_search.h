#ifndef FORBID_ITERATIVE_SEARCH_H
#define FORBID_ITERATIVE_SEARCH_H

#include "search_engine.h"
#include "../plans/plans_graph.h"

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
    FORBID_MULTIPLE_PLAN_MULTISETS
};

class ForbidIterativeSearch: public SearchEngine {
private:
    bool dump_forbid_plans_reformulation;
    bool dump_forbid_multiset_reformulation;
    bool dump_forbid_multisets_reformulation;
    bool dump_forbid_plan_reformulation;
    std::shared_ptr<Group> extend_plans_with_symmetry_group;
    int number_of_plans_to_read;
    bool dump_states_json;
    bool dump_causal_links_json;

    plans::PlanOrdersReduction reduce_plan_orders;

    TaskReformulationType reformulate;
    bool change_operator_names;
    const int number_of_plans;
    bool dump_debug_info;
    bool dumping_plans_files;
    int read_plans_and_dump_graph;
    int number_of_edges_until_greedy_clean;

    SearchEngine::Plan get_plan_for_op_ids(const std::vector<int>& plan_ids) const;

    plans::PlansGraph* create_forbid_graph_and_dump_multiple_plans(bool optimal, const SearchEngine::Plan &current_plan) const;

    void reformulate_and_dump_single_plan(const char* filename, const SearchEngine::Plan &current_plan) const;
    void reformulate_and_dump_multiple_plans_graph(const char* filename, bool optimal, const SearchEngine::Plan &current_plan) const;
    void reformulate_and_dump_multiset(const char* filename, std::vector<SearchEngine::Plan> &current_plans) const;
    void reformulate_and_dump_read_plans_and_dump_graph(const char* filename, bool optimal) const;
    void compute_metrics(const options::Options &opts, const std::vector<std::vector<const GlobalOperator *>>& plans);

    std::shared_ptr<AbstractTask> create_reformulated_task(std::unordered_set<std::vector<int>>& plans) const;
    std::shared_ptr<AbstractTask> create_reformulated_task_multiset(std::unordered_set<std::vector<int>>& plans) const;
    std::shared_ptr<AbstractTask> create_reformulated_task_multisets(std::unordered_set<std::vector<int>>& plans) const;
    void plan_to_multiset(const std::vector<int>& plan, std::unordered_map<int, int>& plan_multiset) const;
    bool multiset_union(std::unordered_map<int, int>& multiset, const std::unordered_map<int, int>& from_multiset) const;
    bool enough_plans_found(int num_found_plans) const;
    void dump_plan_json(std::vector<const GlobalOperator *>& plan, std::ostream& os) const;

protected:

    virtual void initialize() {}
    virtual SearchStatus step() { return SOLVED; }

    void reformulate_and_dump(bool optimal, std::vector<SearchEngine::Plan>& plans);
public:
    ForbidIterativeSearch(const options::Options &opts);
    virtual ~ForbidIterativeSearch();
//    virtual void print_statistics() const;
//    virtual void save_plan_if_necessary() const;
    static void add_forbid_plan_reformulation_option(options::OptionParser &parser);

};


#endif
