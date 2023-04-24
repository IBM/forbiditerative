#include "forbid_iterative_search.h"

#include "evaluation_context.h"
#include "option_parser.h"
#include "plugin.h"
#include "plan_manager.h"
#include "../structural_symmetries/group.h"
#include "../structural_symmetries/operator_permutation.h"

#include "algorithms/ordered_set.h"
#include "../tasks/root_task.h"
#include "../tasks/graph_forbid_reformulated_task.h"
#include "../tasks/plan_forbid_reformulated_task.h"
#include "../tasks/multiset_forbid_reformulated_task.h"
#include "../tasks/multisets_forbid_reformulated_task.h"
#include "../tasks/super_multisets_forbid_reformulated_task.h"
#include "../tasks/supersets_forbid_reformulated_task.h"


#include "utils/countdown_timer.h"
#include "utils/system.h"
#include "utils/timer.h"

#include <unordered_set>
#include <cassert>
#include <iostream>
#include <fstream>
#include <limits>

using namespace std;
// using utils::ExitCode;


ForbidIterativeSearch::ForbidIterativeSearch(const Options &opts)
    : SearchEngine(opts),
      number_of_plans_to_read(opts.get<int>("number_of_plans_to_read")),
      dump_states_json(opts.get<bool>("dump_states_json")),
      dump_causal_links_json(opts.get<bool>("dump_causal_links_json")),
      reduce_plan_orders(plans::PlanOrdersReduction(opts.get<plans::PlanOrdersReduction>("reduce_plan_orders"))),
      reformulate(TaskReformulationType(opts.get<TaskReformulationType>("reformulate"))),
      change_operator_names(opts.get<bool>("change_operator_names")),
      number_of_plans(opts.get<int>("number_of_plans")),
      dump_debug_info(opts.get<bool>("dump")),
      dumping_plans_files(opts.get<bool>("dumping_plans_files")),
      read_plans_and_dump_graph(opts.get<bool>("read_plans_and_dump_graph")),
      number_of_edges_until_greedy_clean(opts.get<int>("number_of_edges_until_greedy_clean")),
      external_plan_file(""),
      external_plans_path(""),
      json_file_to_dump(""),
      is_external_plan_file(false),
      is_external_plans_path(false),
      is_json_file_to_dump(false) {

    if (opts.contains("extend_plans_with_symmetry")) {
        symmetry_group =  opts.get<shared_ptr<Group>>("extend_plans_with_symmetry");
        if (!symmetry_group->is_stabilizing_initial_state()) {
            cerr << "For extending plans with symmetries we need to stabilize the initial state" << endl;
            utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
        }

        if (symmetry_group && !symmetry_group->is_initialized()) {
            utils::g_log << "Initializing symmetries for extending plans with" << endl;
            symmetry_group->compute_symmetries(task_proxy);
        }
    } else {
        symmetry_group = nullptr;
    }
    if (opts.contains("external_plan_file")) {

        if (reformulate != TaskReformulationType::NONE &&
            reformulate != TaskReformulationType::NONE_FIND_ADDITIONAL_PLANS && 
            reformulate != TaskReformulationType::FORBID_MULTIPLE_PLANS) {
            cerr << "Only FORBID_MULTIPLE_PLANS, NONE, or NONE_FIND_ADDITIONAL_PLANS should be used with a single input plan" << endl;
            utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
        }

        external_plan_file = opts.get<string>("external_plan_file");
        is_external_plan_file = true;
    } else if (opts.contains("external_plans_path")) {
        // If the option is used, multiple plans are loaded

        if (reformulate == TaskReformulationType::FORBID_SINGLE_PLAN ||
            reformulate == TaskReformulationType::FORBID_MULTIPLE_PLANS ||
            reformulate == TaskReformulationType::NONE_FIND_ADDITIONAL_PLANS) {
            cerr << "Only FORBID_SINGLE_PLAN_MULTISET or FORBID_MULTIPLE_PLAN_MULTISETS should be used with multiple plans" << endl;
            utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
        }

        if (number_of_plans_to_read <= 0) {
            cerr << "At least one plan should be specified" << endl;
            utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
        }
        external_plans_path = opts.get<string>("external_plans_path");
        is_external_plans_path = true;
        if (opts.contains("json_file_to_dump")) {
            json_file_to_dump = opts.get<string>("json_file_to_dump");
            is_json_file_to_dump = true;
        }
    }
}

ForbidIterativeSearch::~ForbidIterativeSearch() {
}

void ForbidIterativeSearch::print_statistics() const {

}

SearchStatus ForbidIterativeSearch::step() {
    if (is_external_plan_file) {
        // If the option is used, a single plan is loaded, and a reformulation is performed
        //TODO: Check if can be unified with  external_plans_path
        Plan plan;
        plan_manager.load_plan(external_plan_file, plan, task_proxy);
        vector<Plan> plans = { plan };

        if (reformulate == TaskReformulationType::NONE_FIND_ADDITIONAL_PLANS) {
            // No reformulation, only dump plans
            // Adding the first plan to the set of optimal plans already found without dumping
            // Since the number of required plans does not include the loaded plan, we increase it
            number_of_plans++;
            shared_ptr<plans::PlansGraph> forbid_graph = make_shared<plans::PlansGraph>(task_proxy, number_of_plans, true);
            forbid_graph->set_num_edges_until_greedy_clean(number_of_edges_until_greedy_clean);
            forbid_graph->dump_reordering_plans_neighbours_interfere(plans[0]);
                        
            if (symmetry_group) {
                utils::g_log << "Extending partial order plans with symmetries... " << endl;
                // Going over the symmetries until fixed point is reached
                int num_symmetries = symmetry_group->get_num_generators();
                while (!enough_plans_found(forbid_graph->get_number_of_plans_dumped())) {
                    bool change = false;
                    for (int i=0; i < num_symmetries; ++i) {
                        bool curr_change = forbid_graph->add_symmetry(symmetry_group, i);
                        if (enough_plans_found(forbid_graph->get_number_of_plans_dumped()))
                            break;
                        change |= curr_change;
                        if (curr_change)
                            forbid_graph->dump_plans(plan_manager, number_of_plans);
                    }
                    if (!change)
                        break;
                }
                utils::g_log << "Done extending partial order plans with symmetries" << endl;
            }
            forbid_graph->dump_plans(plan_manager, number_of_plans);
            utils::g_log << "Done dumping plans" << endl;
        } else {
            //TODO: Check why optimal is false here
            // reformulate_and_dump(false, plans);
            // Changed to true
            reformulate_and_dump(true, plans);
        }
        utils::exit_with(utils::ExitCode::SUCCESS);
    } else if (is_external_plans_path) {
        // If the option is used, multiple plans are loaded
        vector<Plan> plans;
        plan_manager.load_plans(external_plans_path, number_of_plans_to_read, plans, task_proxy);
        if (is_json_file_to_dump) {
            ofstream os(json_file_to_dump.c_str());
            os << "{ \"plans\" : [" << endl;
            bool first_dumped = false;
            for (const Plan& plan : plans) {
                if (first_dumped)
                    os << "," << endl;
                dump_plan_json(plan, os);
                first_dumped = true;
            }
            os << "]}" << endl;
        }
        // if (reformulate == TaskReformulationType::NONE) {
        //     utils::exit_with(utils::ExitCode::SUCCESS);
        // }
        
        utils::HashSet<Plan> unique_plans;
        vector<Plan> ordered_unique_plans;
        for (const Plan& plan : plans) {
            auto it = unique_plans.insert(plan);
            if (it.second) {
                ordered_unique_plans.push_back(plan);
            }
        }
        reformulate_and_dump(false, ordered_unique_plans);
        utils::exit_with(utils::ExitCode::SUCCESS);
    }
    return SOLVED; 
}

void ForbidIterativeSearch::dump_plan_json(const Plan& plan, std::ostream& os) const {
    int plan_cost = calculate_plan_cost(plan, task_proxy);
    os << "{ ";
    os << "\"cost\" : " << plan_cost << "," << endl; 
    os << "\"actions\" : [" << endl;
    if (plan.size() > 0) {
        OperatorsProxy operators = task_proxy.get_operators();

        os << "\""  << operators[plan[0].get_index()].get_name() << "\"";
        for (size_t i = 1; i < plan.size(); ++i) {
            os << ", \"" << operators[plan[i].get_index()].get_name() << "\"";
        }
    }
    os << "]";

    if (dump_causal_links_json) {
        os << "," << endl;
        os << "\"causal_links\" : [" << endl;
        search_space.dump_partial_order_from_plan(plan, os, task_proxy);
        os << "]";
    }
    if (dump_states_json) {
        os << "," << endl;
        os << "\"states\" : [" << endl;

        vector<StateID> trace;
        search_space.trace_from_plan(plan, trace, task_proxy);
        search_space.dump_trace(trace, os);
        os << "]";
    }
    os << "}" << endl;
}

// void ForbidIterativeSearch::save_plan_if_necessary() const {

//     // In case we forbid multiple plans, we already do that
//     if (reformulate == TaskReformulationType::FORBID_MULTIPLE_PLANS || 
//         reformulate == TaskReformulationType::FORBID_SINGLE_PLAN_MULTISET || 
//         reformulate == TaskReformulationType::FORBID_MULTIPLE_PLAN_MULTISETS)
//         return;
//     SearchEngine::save_plan_if_necessary();
// }

void ForbidIterativeSearch::reformulate_and_dump(bool optimal, std::vector<Plan>& plans) {
    const char* filename = "reformulated_output.sas";
    if (reformulate == TaskReformulationType::FORBID_SINGLE_PLAN) {
        reformulate_and_dump_single_plan(filename, plans[0]);
    } else if (reformulate == TaskReformulationType::FORBID_MULTIPLE_PLANS) {
        reformulate_and_dump_multiple_plans_graph(filename, optimal, plans[0]);
    } else if (reformulate == TaskReformulationType::FORBID_SINGLE_PLAN_MULTISET || 
               reformulate == TaskReformulationType::FORBID_MULTIPLE_PLAN_MULTISETS ||
               reformulate == TaskReformulationType::FORBID_MULTIPLE_PLAN_SUPERMULTISETS) {
        reformulate_and_dump_multiset(filename, plans);
    } else if (reformulate == TaskReformulationType::FORBID_MULTIPLE_PLAN_SUPERSETS) {
        reformulate_and_dump_superset(filename, plans);
    } else if (read_plans_and_dump_graph) {
        reformulate_and_dump_read_plans_and_dump_graph(filename, plans);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ForbidIterativeSearch::reformulate_and_dump_single_plan(const char* filename, const Plan &current_plan) const {
    ofstream os(filename);
    vector<int> forbid_plan;
    for(size_t i = 0; i < current_plan.size(); ++i) {
        forbid_plan.push_back(current_plan[i].get_index());
    }
    shared_ptr<AbstractTask> reformulated_task = make_shared<extra_tasks::PlanForbidReformulatedTask>(tasks::g_root_task, move(forbid_plan));
    reformulated_task->dump_to_SAS(os);
}

plans::PlansGraph* ForbidIterativeSearch::create_forbid_graph_and_dump_multiple_plans(bool optimal, const Plan &current_plan) {
    assert(number_of_plans >= 0);
    plans::PlansGraph* forbid_graph = new plans::PlansGraph(task_proxy, number_of_plans, optimal);
    forbid_graph->set_num_edges_until_greedy_clean(number_of_edges_until_greedy_clean);
    utils::g_log << "Adding the found plan:  ";
    if (reduce_plan_orders == plans::PlanOrdersReduction::NONE) {
        utils::g_log << "No additional plans for reorderings are created. " << endl;
    } else if (reduce_plan_orders == plans::PlanOrdersReduction::NEIGHBOURS_INTERFERE) {
        utils::g_log << "Creating partial order plans by reducing orders between non-interfering neigbours... " << endl;
    } else if (reduce_plan_orders == plans::PlanOrdersReduction::NAIVE_ALL ||
            reduce_plan_orders == plans::PlanOrdersReduction::NAIVE_ALL_DFS ||
            reduce_plan_orders == plans::PlanOrdersReduction::NAIVE_ALL_DFS_NODUP) {
        utils::g_log << "Creating partial order plans by naively reducing all orders... " << endl;
    }
    forbid_graph->add_plan(plan_manager, current_plan, reduce_plan_orders);
    utils::g_log << "Done adding the found plan, dumping the plans so far" << endl;
    forbid_graph->dump_plans(plan_manager, number_of_plans);
    utils::g_log << "Done dumping the plans" << endl;

    //forbid_graph->dump_graph();
    if (symmetry_group) {
        utils::g_log << "Extending partial order plans with symmetries... " << flush;
        // Going over the symmetries until fixed point is reached
        int num_symmetries = symmetry_group->get_num_generators();
        while (!enough_plans_found(forbid_graph->get_number_of_plans_dumped())) {
            bool change = false;
            for (int i=0; i < num_symmetries; ++i) {
                bool curr_change = forbid_graph->add_symmetry(symmetry_group, i);;
                change |= curr_change;
                if (curr_change)
                    forbid_graph->dump_plans(plan_manager, number_of_plans);
                if (enough_plans_found(forbid_graph->get_number_of_plans_dumped()))
                    break;
            }
            if (!change)
                break;
        }
        utils::g_log << "Done extending partial order plans with symmetries" << endl;
    }

    //utils::g_log << "=========================================================================" << endl;
    if (dump_debug_info)
        forbid_graph->dump_dot_graph(plan_manager.get_num_previously_generated_plans(), false);

    utils::g_log << "Dumping plans before reformulation " << endl;
    forbid_graph->dump_plans(plan_manager, number_of_plans);
    utils::g_log << "Done dumping plans before reformulation" << endl;
    return forbid_graph;
}

void ForbidIterativeSearch::reformulate_and_dump_multiple_plans_graph(const char* filename, bool optimal, const Plan &current_plan) {
    plans::PlansGraph* forbid_graph = create_forbid_graph_and_dump_multiple_plans(optimal, current_plan);
    if (!enough_plans_found(forbid_graph->get_number_of_plans_dumped())) {
        // Not all plans are found yet
        shared_ptr<AbstractTask> reformulated_task = make_shared<extra_tasks::GraphForbidReformulatedTask>(tasks::g_root_task, forbid_graph, change_operator_names);
        ofstream os(filename);
        reformulated_task->dump_to_SAS(os);
    }
}

bool ForbidIterativeSearch::enough_plans_found(int num_found_plans) const {
    if (number_of_plans == -1) {
        return false;
    }
    return num_found_plans >= number_of_plans;
}

void ForbidIterativeSearch::reformulate_and_dump_multiset(const char* filename, vector<Plan> &current_plans) {
    // Finding unique plans (as multisets)
    // Adding new, if found, to the end of the vector of current plans
    int num_operators = tasks::g_root_task->get_num_operators();
    utils::g_log << "Adding plans... " << endl;
    utils::HashSet<vector<int>> plans_sets;
    
    vector<vector<int>> frontier;
    for (const Plan& current_plan : current_plans) {
        vector<int> found_plan;
        vector<int> set_a(num_operators, 0);
        for (OperatorID op : current_plan) {
            int op_no = op.get_index();
            found_plan.push_back(op_no);
            set_a[op_no]++;
        }
        auto ret = plans_sets.insert(set_a);
        if (ret.second) {
            // New plan (as a multiset)
            frontier.push_back(found_plan);
        }
    }

    // Extending unique plans with symmetries
    if (symmetry_group) {
        utils::g_log << "Finding symmetric plans... " << endl;
        // Going over the symmetries until fixed point is reached
        // Keeping plans as action multisets for duplicate detection
        //TODO: Think of a more memory efficient way of doing that. 
        //TODO: Dump plans as we go 
        int num_symmetries = symmetry_group->get_num_generators();

        while (!enough_plans_found((int)current_plans.size())) {
            // Applying all symmetries to all plans in a frontier. The new ones become the new frontier.
            vector<vector<int>> new_frontier;
            bool change = false;
            for (int i=0; i < num_symmetries; ++i) {
                const OperatorPermutation& op_sym = symmetry_group->get_operator_permutation(i);

                for (vector<int> current_plan : frontier) {
                    // Permuting the plan, adding to found plans, and if new, to new frontier
                    vector<int> permuted_plan;
                    vector<int> permuted_set(num_operators, 0);
                    for (int op_no : current_plan) {
                        int permuted_op_no = op_sym.get_permuted_operator_no(op_no);
                        permuted_plan.push_back(permuted_op_no);
                        permuted_set[permuted_op_no]++;
                    }
                    auto ret = plans_sets.insert(permuted_set);
                    if (ret.second) {
                        // Element inserted, adding to the new frontier
                        //plans.insert(permuted_plan);
                        Plan curr;
                        get_plan_for_op_ids(permuted_plan, curr);
                        current_plans.push_back(curr);                        
                        new_frontier.push_back(permuted_plan);
                        change = true;
                    } else {
                    }
                }
                if (enough_plans_found((int)current_plans.size()))
                    break;
            }
            if (!change)
                break;
            frontier.swap(new_frontier);
        }
        utils::g_log << "done! [t=" << utils::g_timer << "]" << endl;
    }

    //utils::g_log << "=========================================================================" << endl;

    if (dumping_plans_files) {
        utils::g_log << "Dumping " << current_plans.size() << " plans before reformulation " << endl;    
        for (const Plan& plan : current_plans) {
            plan_manager.save_plan(plan, task_proxy, true);
        }
        utils::g_log << "Done dumping plans before reformulation" << endl;
    }
    if (!enough_plans_found((int)current_plans.size())) {
        // Not all plans are found yet
        utils::HashSet<vector<int>> plans;
        vector<vector<int>> ordered_plans;
        for (const Plan& current_plan : current_plans) {
            vector<int> found_plan;
            for (OperatorID op : current_plan) {
                int op_no = op.get_index();
                found_plan.push_back(op_no);
            }
            auto ret = plans.insert(found_plan);
            if (ret.second) {
                // New plan
                ordered_plans.push_back(found_plan);
            }
        }
        shared_ptr<AbstractTask> reformulated_task = create_reformulated_task(ordered_plans);
        utils::g_log << "Reformulated task is created, dumping" << endl;

        ofstream os(filename);
        reformulated_task->dump_to_SAS(os);
        utils::g_log << "Done dumping the reformulated task" << endl;
    }
}

static bool is_subset_eq(const std::unordered_set<int> &p1, const std::unordered_set<int> &p2) {
    if (p1.size() > p2.size())
        return false;
    if (p1.size() == p2.size())
        return p1 == p2;

    for (auto a : p1) {
        if (p2.find(a) == p2.end())
            return false;
    }
    return true;
}

static bool superset_duplicate(const std::vector<std::unordered_set<int>>& plans_sets, const std::unordered_set<int> &plan) {
    for (auto p : plans_sets) {
        if (is_subset_eq(p,plan) || is_subset_eq(plan, p))
            return true;
    }
    return false;
}

void ForbidIterativeSearch::reformulate_and_dump_superset(const char* filename, vector<Plan> &current_plans) {
    // Assuming unique plans (as super-sets)
    // Adding new, if found, to the end of the vector of current plans
    cout << "Adding plans... " << endl;
    std::vector<std::unordered_set<int>> plans_sets;
    vector<vector<int>> frontier;
    for (const Plan& current_plan : current_plans) {
        vector<int> found_plan;
        std::unordered_set<int> set_a;
        for (OperatorID op : current_plan) {
            //cout << op->get_name() << endl;
            int op_no = op.get_index();
            found_plan.push_back(op_no);
            set_a.insert(op_no);
        }
        if (!superset_duplicate(plans_sets, set_a)) {
            plans_sets.push_back(set_a);
            frontier.push_back(found_plan);
        }
    }

    // Extending unique plans with symmetries
    if (symmetry_group) {
        cout << "Finding symmetric plans... " << flush;
        // Going over the symmetries until fixed point is reached
        // Keeping plans as action sets, perform duplicate detection comparing subsets
        //TODO: Think of a more memory efficient way of doing that.
        //TODO: Dump plans as we go
        int num_symmetries = symmetry_group->get_num_generators();

        while (!enough_plans_found((int)current_plans.size())) {
            // Applying all symmetries to all plans in a frontier. The new ones become the new frontier.
            vector<vector<int>> new_frontier;
            bool change = false;
            for (int i=0; i < num_symmetries; ++i) {
                const OperatorPermutation& op_sym = symmetry_group->get_operator_permutation(i);

                for (vector<int> current_plan : frontier) {
                    // Permuting the plan, adding to found plans, and if new, to new frontier
                    vector<int> permuted_plan;
                    std::unordered_set<int> permuted_set;
                    for (int op_no : current_plan) {
                        int permuted_op_no = op_sym.get_permuted_operator_no(op_no);
                        permuted_plan.push_back(permuted_op_no);
                        permuted_set.insert(permuted_op_no);
                    }
                    if (!superset_duplicate(plans_sets, permuted_set)) {
                        plans_sets.push_back(permuted_set);

                        // Element inserted, adding to the new frontier
                        //plans.insert(permuted_plan);
                        Plan p;
                        get_plan_for_op_ids(permuted_plan, p);
                        current_plans.push_back(p);
                        new_frontier.push_back(permuted_plan);
                        change = true;
                    } else {
                    }
                }
                if (enough_plans_found((int)current_plans.size()))
                    break;
            }
            if (!change)
                break;
            frontier.swap(new_frontier);
        }
        cout << "done! [t=" << utils::g_timer << "]" << endl;
    }

    //cout << "=========================================================================" << endl;

    cout << "Dumping " << current_plans.size() << " plans before reformulation " << endl;
    if (dumping_plans_files) {
        for (const Plan& plan : current_plans) {
            plan_manager.save_plan(plan, task_proxy, true);
        }
    }
    cout << "done! [t=" << utils::g_timer << "]" << endl;
    if (!enough_plans_found((int)current_plans.size())) {
        // Not all plans are found yet
        utils::HashSet<vector<int>> plans_set;
        vector<vector<int>> ordered_plans;
        for (const Plan& current_plan : current_plans) {
            vector<int> found_plan;
            for (OperatorID op : current_plan) {
                int op_no = op.get_index();
                found_plan.push_back(op_no);
            }
            auto ret = plans_set.insert(found_plan);
            if (ret.second) {
                // New plan
                ordered_plans.push_back(found_plan);
            }
        }
        shared_ptr<AbstractTask> reformulated_task = create_reformulated_task(ordered_plans);
        cout << "Reformulated task is created, dumping [t=" << utils::g_timer << "]" << endl;

        ofstream os(filename);
        reformulated_task->dump_to_SAS(os);
        cout << "done! [t=" << utils::g_timer << "]" << endl;
    }
}


void ForbidIterativeSearch::get_plan_for_op_ids(const vector<int>& plan_ids, Plan& plan) const {
    for (int op_no : plan_ids) {
        plan.push_back(OperatorID(op_no));
    }
}

shared_ptr<AbstractTask> ForbidIterativeSearch::create_reformulated_task(std::vector<vector<int>>& plans) const {
    assert(reformulate == TaskReformulationType::FORBID_SINGLE_PLAN_MULTISET
            || reformulate == TaskReformulationType::FORBID_MULTIPLE_PLAN_MULTISETS
            || reformulate == TaskReformulationType::FORBID_MULTIPLE_PLAN_SUPERSETS
            || reformulate == TaskReformulationType::FORBID_MULTIPLE_PLAN_SUPERMULTISETS);

    if (reformulate == TaskReformulationType::FORBID_SINGLE_PLAN_MULTISET) {
        return create_reformulated_task_multiset(plans);
    } else if (reformulate == TaskReformulationType::FORBID_MULTIPLE_PLAN_SUPERSETS) {
        return create_reformulated_task_supersets(plans);
    } else if (reformulate == TaskReformulationType::FORBID_MULTIPLE_PLAN_SUPERMULTISETS) {
        return create_reformulated_task_super_multisets(plans);
    }
    
    return create_reformulated_task_multisets(plans);
}

shared_ptr<AbstractTask> ForbidIterativeSearch::create_reformulated_task_multiset(std::vector<vector<int>>& plans) const {
    // Creating a single multiset that is a union of all multisets
    std::unordered_map<int, int> multiset;
    for (vector<int> plan : plans) {
        // Building multiset for that plan
        std::unordered_map<int, int> plan_multiset;
        plan_to_multiset(plan, plan_multiset);
//        bool is_not_subset = multiset_union(multiset, plan_multiset);
        multiset_union(multiset, plan_multiset);
        
        //TODO: Check what's going on here - why are we dumping plans again
/*
        if (is_not_subset && dumping_plans_files) {
            // Dumping the plan
            Plan global_plan;
            for (int op_no : plan) {
                global_plan.push_back(&g_operators[op_no]);
            }
            save_plan(global_plan, true);
        }
*/        
    }
    return make_shared<extra_tasks::MultisetForbidReformulatedTask>(tasks::g_root_task, multiset, change_operator_names);
}

shared_ptr<AbstractTask> ForbidIterativeSearch::create_reformulated_task_multisets(std::vector<vector<int>>& plans) const {
    // Creating a multiset from each plan
    std::vector<std::unordered_map<int, int>> multisets;
    utils::g_log << "Forbidding " << plans.size() << " plans" << endl;
    for (vector<int> plan : plans) {
        // Building multiset for that plan
        std::unordered_map<int, int> plan_multiset;
        plan_to_multiset(plan, plan_multiset);
        multisets.push_back(plan_multiset);
    }
    return make_shared<extra_tasks::MultisetsForbidReformulatedTask>(tasks::g_root_task, multisets, change_operator_names);
}


shared_ptr<AbstractTask> ForbidIterativeSearch::create_reformulated_task_super_multisets(std::vector<vector<int>>& plans) const {
    // Creating a multiset from each plan
    std::vector<std::unordered_map<int, int>> multisets;
    cout << "Forbidding " << plans.size() << " plans" << endl;
    for (vector<int> plan : plans) {
        // Building multiset for that plan
        std::unordered_map<int, int> plan_multiset;
        plan_to_multiset(plan, plan_multiset);
        multisets.push_back(plan_multiset);
    }
    return make_shared<extra_tasks::SuperMultisetsForbidReformulatedTask>(tasks::g_root_task, multisets, change_operator_names);
}

shared_ptr<AbstractTask> ForbidIterativeSearch::create_reformulated_task_supersets(std::vector<vector<int>>& plans) const {
    // Creating a multiset from each plan
    std::vector<std::unordered_set<int>> sets;
    cout << "Forbidding " << plans.size() << " plans" << endl;
    for (auto plan : plans) {
        // Building multiset for that plan
        std::unordered_set<int> plan_set;
        plan_set.insert(plan.begin(), plan.end());
        sets.push_back(plan_set);
    }
    return make_shared<extra_tasks::SupersetsForbidReformulatedTask>(tasks::g_root_task, sets);
}


void ForbidIterativeSearch::plan_to_multiset(const std::vector<int>& plan, std::unordered_map<int, int>& plan_multiset) const {
    for (int op_no : plan) {
        auto it = plan_multiset.find(op_no);
        if (it == plan_multiset.end()) {
            plan_multiset[op_no] = 1;
        } else {
            plan_multiset[op_no]++;
        }
    }
}

bool ForbidIterativeSearch::multiset_union(std::unordered_map<int, int>& multiset, const std::unordered_map<int, int>& from_multiset) const {
    // Modifies the multiset by making a union with the second multiset, returns true iff not proper subset 
    bool is_proper_subset = true;
    // Making a union with the main multiset
    for (std::pair<int, int> e : from_multiset) {
        int op_no = e.first;
        int count = e.second;
        auto it = multiset.find(op_no);
        if (it == multiset.end()) {
            multiset[op_no] = count;
            is_proper_subset = false;
        } else {
            if (count > multiset[op_no]) {
                is_proper_subset = false;
                multiset[op_no] = count;
            }
        }
    }
    return !is_proper_subset;
}

void ForbidIterativeSearch::reformulate_and_dump_read_plans_and_dump_graph(const char* filename, std::vector<Plan> &current_plans) const {
    ofstream os(filename);
    assert(number_of_plans >= 0);

    shared_ptr<plans::PlansGraph> forbid_graph = make_shared<plans::PlansGraph>(task_proxy, number_of_plans, true);
    // OperatorsProxy operators = task_proxy.get_operators();

    for (auto p : current_plans) {
        // vector<string> plan;
        // for (auto a : p) {
        //     plan.push_back(operators[a].get_name());
        // }
        forbid_graph->add_plan_no_reduce(p);
        // forbid_graph->add_non_deterministic_plan(plan);
    }
    forbid_graph->dump_dot_graph(plan_manager.get_num_previously_generated_plans(), true);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void ForbidIterativeSearch::add_forbid_plan_reformulation_option(OptionParser &parser) {

    parser.add_option<shared_ptr<Group>>(
        "extend_plans_with_symmetry",
        "symmetries object to extend plans with",
        OptionParser::NONE);

    parser.add_option<int>("number_of_plans_to_read",
        "Getting multiple plans as an input, reformulate",
        "0");

    parser.add_option<bool>("dump_states_json",
        "Getting multiple plans as an input, dumping into files sequences of states instead",
        "false");

    parser.add_option<bool>("dump_causal_links_json",
        "Dump the partial order between actions in a plan, defined by causal links to preconditions from the latest achiever",
        "false");


    parser.add_option<string>("json_file_to_dump",
        "A path to the json file to use for dumping",
        OptionParser::NONE);

    parser.add_option<string>("external_plan_file",
        "Getting a single plan file path",
        OptionParser::NONE);

    parser.add_option<string>("external_plans_path",
        "Getting a path to the folder with plans",
        OptionParser::NONE);

    vector<string> reduce;
    reduce.push_back("NONE");
    reduce.push_back("NEIGHBOURS_INTERFERE");
    reduce.push_back("NAIVE_ALL");
    reduce.push_back("NAIVE_ALL_DFS");
    reduce.push_back("NAIVE_ALL_DFS_NODUP");
    parser.add_enum_option<plans::PlanOrdersReduction>(
        "reduce_plan_orders",
        reduce,
        "Reducing orders in a found plan",
        "NONE");

    vector<string> reformulate;
    reformulate.push_back("NONE");
    reformulate.push_back("NONE_FIND_ADDITIONAL_PLANS");
    reformulate.push_back("FORBID_SINGLE_PLAN");
    reformulate.push_back("FORBID_MULTIPLE_PLANS");
    reformulate.push_back("FORBID_SINGLE_PLAN_MULTISET");
    reformulate.push_back("FORBID_MULTIPLE_PLAN_MULTISETS");
    reformulate.push_back("FORBID_MULTIPLE_PLAN_SUPERSETS");
    reformulate.push_back("FORBID_MULTIPLE_PLAN_SUPERMULTISETS");
    parser.add_enum_option<TaskReformulationType>(
        "reformulate",
        reformulate,
        "Reformulate the input planning task to forbid plans",
        "NONE");

    parser.add_option<bool>("change_operator_names",
        "changing the names to include operator types",
        "false");
    parser.add_option<int>("number_of_plans", "Number of plans", "-1");
    parser.add_option<int>("number_of_edges_until_greedy_clean", "Number of edges added by the naive DFS algorithm until a greedy cleaning is performed", "-1");
    parser.add_option<bool>("dump", "Dumping debug info", "false");
    parser.add_option<bool>("read_plans_and_dump_graph", "reading the plans and creating a graph, and then dumping it", "false");
    parser.add_option<bool>("dumping_plans_files", "Dumping the plans to files", "true");

}



static shared_ptr<SearchEngine> _parse(OptionParser &parser) {
    parser.document_synopsis("Forbid iterative search", "");

    SearchEngine::add_options_to_parser(parser);
    ForbidIterativeSearch::add_forbid_plan_reformulation_option(parser);
    Options opts = parser.parse();


    shared_ptr<ForbidIterativeSearch> engine;

    if (!parser.dry_run()) {
        engine = make_shared<ForbidIterativeSearch>(opts);
    }

    return engine;
}

static Plugin<SearchEngine> _plugin("forbid_iterative", _parse);


