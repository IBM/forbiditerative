#include "forbid_iterative_search.h"

#include "evaluation_context.h"
#include "globals.h"
#include "option_parser.h"
#include "plugin.h"

#include "algorithms/ordered_set.h"
#include "../tasks/graph_forbid_reformulated_task.h"
#include "../tasks/plan_forbid_reformulated_task.h"
#include "../tasks/multiset_forbid_reformulated_task.h"
#include "../tasks/multisets_forbid_reformulated_task.h"

#include "utils/countdown_timer.h"
#include "utils/system.h"
#include "utils/timer.h"

#include <unordered_set>
#include <cassert>
#include <iostream>
#include <limits>

using namespace std;
using utils::ExitCode;


ForbidIterativeSearch::ForbidIterativeSearch(const Options &opts)
    : SearchEngine(opts),
      number_of_plans_to_read(opts.get<int>("number_of_plans_to_read")),
      dump_states_json(opts.get<bool>("dump_states_json")),
      dump_causal_links_json(opts.get<bool>("dump_causal_links_json")),
      reduce_plan_orders(plans::PlanOrdersReduction(opts.get_enum("reduce_plan_orders"))),
      reformulate(TaskReformulationType(opts.get_enum("reformulate"))),
      change_operator_names(opts.get<bool>("change_operator_names")),
      number_of_plans(opts.get<int>("number_of_plans")),
      dump_debug_info(opts.get<bool>("dump")),
      dumping_plans_files(opts.get<bool>("dumping_plans_files")),
      read_plans_and_dump_graph(opts.get<int>("read_plans_and_dump_graph")),
      number_of_edges_until_greedy_clean(opts.get<int>("number_of_edges_until_greedy_clean")) {

    if (opts.contains("extend_plans_with_symmetry")) {
        extend_plans_with_symmetry_group =  opts.get<shared_ptr<Group>>("extend_plans_with_symmetry");
        if (!extend_plans_with_symmetry_group->is_stabilizing_initial_state()) {
            cerr << "For extending plans with symmetries we need to stabilize the initial state" << endl;
            utils::exit_with(utils::ExitCode::INPUT_ERROR);
        }

        if (extend_plans_with_symmetry_group && !extend_plans_with_symmetry_group->is_initialized()) {
            cout << "Initializing symmetries for extending plans with" << endl;
            extend_plans_with_symmetry_group->compute_symmetries();
        }
    } else {
        extend_plans_with_symmetry_group = nullptr;
    }
    if (opts.contains("external_plan_file")) {
        // If the option is used, a single plan is loaded, and a reformulation is performed
        //TODO: Check if can be unified with  external_plans_path
        SearchEngine::Plan plan = load_plan(opts.get<string>("external_plan_file"));
        vector<SearchEngine::Plan> plans;
        plans.push_back(plan);

        if (reformulate != TaskReformulationType::NONE && 
            reformulate != TaskReformulationType::NONE_FIND_ADDITIONAL_PLANS && 
            reformulate != TaskReformulationType::FORBID_MULTIPLE_PLANS) {
            cerr << "Only FORBID_MULTIPLE_PLANS, NONE, or NONE_FIND_ADDITIONAL_PLANS should be used with a single input plan" << endl;
            utils::exit_with(utils::ExitCode::INPUT_ERROR);                          
        }

        if (reformulate == TaskReformulationType::NONE_FIND_ADDITIONAL_PLANS) {
            // No reformulation, only dump plans
            create_forbid_graph_and_dump_multiple_plans(true, plan);
        } else {
            //TODO: Check why optimal is false here
            // reformulate_and_dump(false, plans);
            // Changed to true
            reformulate_and_dump(true, plans);
        }
        utils::exit_with(ExitCode::PLAN_FOUND);
    }
    if (opts.contains("external_plans_path")) {
        // If the option is used, multiple plans are loaded
        if (number_of_plans_to_read <= 0) {
            cerr << "At least one plan should be specified" << endl;
            utils::exit_with(utils::ExitCode::INPUT_ERROR);            
        }
        vector<SearchEngine::Plan> plans = load_plans(opts.get<string>("external_plans_path"), number_of_plans_to_read);
        if (opts.contains("json_file_to_dump")) {
            string filename = opts.get<string>("json_file_to_dump");

            ofstream os(filename.c_str());
            os << "{ \"plans\" : [" << endl;
            bool first_dumped = false;
            for (vector<const GlobalOperator *>& plan : plans) {
                if (first_dumped)
                    os << "," << endl;
                dump_plan_json(plan, os);
                first_dumped = true;
            }
            os << "]}" << endl;
        }
        if (reformulate == TaskReformulationType::NONE) {
            utils::exit_with(ExitCode::PLAN_FOUND);
        }
        
        if (reformulate == TaskReformulationType::FORBID_SINGLE_PLAN || 
            reformulate == TaskReformulationType::FORBID_MULTIPLE_PLANS ||
            reformulate == TaskReformulationType::NONE_FIND_ADDITIONAL_PLANS) {
            cerr << "Only FORBID_SINGLE_PLAN_MULTISET or FORBID_MULTIPLE_PLAN_MULTISETS should be used with multiple plans" << endl;
            utils::exit_with(utils::ExitCode::INPUT_ERROR);                          
        }

        unordered_set<vector<const GlobalOperator *>> unique_plans;
        vector<SearchEngine::Plan> ordered_unique_plans;
        for (vector<const GlobalOperator *>& plan : plans) {
            auto it = unique_plans.insert(plan);
            if (it.second) {
                ordered_unique_plans.push_back(plan);
            }
        }
        reformulate_and_dump(false, ordered_unique_plans);
        utils::exit_with(ExitCode::PLAN_FOUND);
    }
}

ForbidIterativeSearch::~ForbidIterativeSearch() {
}

void ForbidIterativeSearch::dump_plan_json(std::vector<const GlobalOperator *>& plan, std::ostream& os) const {
    int plan_cost = calculate_plan_cost(plan);
    os << "{ ";
    os << "\"cost\" : " << plan_cost << "," << endl; 
    os << "\"actions\" : [" << endl;
    if (plan.size() > 0) {
        os << "\""  << plan[0]->get_name() << "\"";
        for (size_t i = 1; i < plan.size(); ++i) {
            os << ", \"" << plan[i]->get_name() << "\"";
        }
    }
    os << "]";

    if (dump_causal_links_json) {
        os << "," << endl;
        os << "\"causal_links\" : [" << endl;
        search_space.dump_partial_order_from_plan(plan, os);
        os << "]";
    }
    if (dump_states_json) {
        os << "," << endl;
        os << "\"states\" : [" << endl;

        vector<StateID> trace;
        search_space.trace_from_plan(plan, trace);
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

void ForbidIterativeSearch::reformulate_and_dump(bool optimal, std::vector<SearchEngine::Plan>& plans) {
    const char* filename = "reformulated_output.sas";
    if (reformulate == TaskReformulationType::FORBID_SINGLE_PLAN) {
        reformulate_and_dump_single_plan(filename, plans[0]);
    } else if (reformulate == TaskReformulationType::FORBID_MULTIPLE_PLANS) {
        reformulate_and_dump_multiple_plans_graph(filename, optimal, plans[0]);
    } else if (reformulate == TaskReformulationType::FORBID_SINGLE_PLAN_MULTISET || 
               reformulate == TaskReformulationType::FORBID_MULTIPLE_PLAN_MULTISETS) {
        reformulate_and_dump_multiset(filename, plans);
    } else if (read_plans_and_dump_graph > 0) {
        reformulate_and_dump_read_plans_and_dump_graph(filename, optimal);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ForbidIterativeSearch::reformulate_and_dump_single_plan(const char* filename, const SearchEngine::Plan &current_plan) const {
    ofstream os(filename);
    vector<int> forbid_plan;
    for(size_t i = 0; i < current_plan.size(); ++i) {
        forbid_plan.push_back(get_op_index_hacked(current_plan[i]));
    }
    shared_ptr<AbstractTask> reformulated_task = make_shared<extra_tasks::PlanForbidReformulatedTask>(g_root_task(), move(forbid_plan));
    reformulated_task->dump_to_SAS(os);
}

plans::PlansGraph* ForbidIterativeSearch::create_forbid_graph_and_dump_multiple_plans(bool optimal, const SearchEngine::Plan &current_plan) const {
    assert(number_of_plans >= 0);
    plans::PlansGraph* forbid_graph = new plans::PlansGraph(number_of_plans, optimal);
    forbid_graph->set_num_edges_until_greedy_clean(number_of_edges_until_greedy_clean);
    cout << "Adding the found plan:  " << flush;
    if (reduce_plan_orders == plans::PlanOrdersReduction::NONE) {
        cout << "No additional plans for reorderings are created. " << flush;
    } else if (reduce_plan_orders == plans::PlanOrdersReduction::NEIGHBOURS_INTERFERE) {
        cout << "Creating partial order plans by reducing orders between non-interfering neigbours... " << flush;
    } else if (reduce_plan_orders == plans::PlanOrdersReduction::NAIVE_ALL ||
            reduce_plan_orders == plans::PlanOrdersReduction::NAIVE_ALL_DFS ||
            reduce_plan_orders == plans::PlanOrdersReduction::NAIVE_ALL_DFS_NODUP) {
        cout << "Creating partial order plans by naively reducing all orders... " << flush;
    }
    forbid_graph->add_plan(current_plan, reduce_plan_orders);
    cout << "done! [t=" << utils::g_timer << "]" << endl;
    forbid_graph->dump_plans(number_of_plans);

    //forbid_graph->dump_graph();
    if (extend_plans_with_symmetry_group) {
        cout << "Extending partial order plans with symmetries... " << flush;
        // Going over the symmetries until fixed point is reached
        int num_symmetries = extend_plans_with_symmetry_group->get_num_generators();
        while (!enough_plans_found(forbid_graph->get_number_of_plans_dumped())) {
            bool change = false;
            for (int i=0; i < num_symmetries; ++i) {
                const Permutation* sym = extend_plans_with_symmetry_group->get_permutation(i);
                const OperatorPermutation* op_sym = extend_plans_with_symmetry_group->get_operator_permutation(i);
                bool curr_change = forbid_graph->add_symmetry(sym, op_sym);
                change |= curr_change;
                if (curr_change)
                    forbid_graph->dump_plans(number_of_plans);
                if (enough_plans_found(forbid_graph->get_number_of_plans_dumped()))
                    break;
            }
            if (!change)
                break;
        }
        cout << "done! [t=" << utils::g_timer << "]" << endl;
    }

    //cout << "=========================================================================" << endl;
    if (dump_debug_info)
        forbid_graph->dump_graph_dot(false);

    cout << "Dumping plans before reformulation " << endl;
    forbid_graph->dump_plans(number_of_plans);
    cout << "done! [t=" << utils::g_timer << "]" << endl;
    return forbid_graph;
}

void ForbidIterativeSearch::reformulate_and_dump_multiple_plans_graph(const char* filename, bool optimal, const SearchEngine::Plan &current_plan) const {
    plans::PlansGraph* forbid_graph = create_forbid_graph_and_dump_multiple_plans(optimal, current_plan);
    if (!enough_plans_found(forbid_graph->get_number_of_plans_dumped())) {
        // Not all plans are found yet
        shared_ptr<AbstractTask> reformulated_task = make_shared<extra_tasks::GraphForbidReformulatedTask>(g_root_task(), forbid_graph, change_operator_names);
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

void ForbidIterativeSearch::reformulate_and_dump_multiset(const char* filename, vector<SearchEngine::Plan> &current_plans) const {
    // Finding unique plans (as multisets)
    // Adding new, if found, to the end of the vector of current plans
    int num_operators = g_root_task()->get_num_operators();
    cout << "Adding plans... " << endl;
    std::unordered_set<vector<int>> plans_sets;
    vector<vector<int>> frontier;
    for (const SearchEngine::Plan& current_plan : current_plans) {
        vector<int> found_plan;
        vector<int> set_a(num_operators, 0);
        for (const GlobalOperator *op : current_plan) {
            //cout << op->get_name() << endl;
            int op_no = get_op_index_hacked(op);
            found_plan.push_back(op_no);
            set_a[op_no]++;
        }
        auto ret = plans_sets.insert(set_a);
        if (ret.second) {
            // New plan (as a multiset)
//            plans.insert(found_plan);
            frontier.push_back(found_plan);
        }
    }

    // Extending unique plans with symmetries
    if (extend_plans_with_symmetry_group) {
        cout << "Finding symmetric plans... " << flush;
            // Going over the symmetries until fixed point is reached
        int num_symmetries = extend_plans_with_symmetry_group->get_num_generators();

        while (!enough_plans_found((int)current_plans.size())) {
            // Applying all symmetries to all plans in a frontier. The new ones become the new frontier.
            vector<vector<int>> new_frontier;
            bool change = false;
            for (int i=0; i < num_symmetries; ++i) {
                const OperatorPermutation* op_sym = extend_plans_with_symmetry_group->get_operator_permutation(i);

                for (vector<int> current_plan : frontier) {
                    // Permuting the plan, adding to found plans, and if new, to new frontier
                    vector<int> permuted_plan;
                    vector<int> permuted_set(num_operators, 0);
                    for (int op_no : current_plan) {
                        int permuted_op_no = op_sym->get_permuted_operator_no(op_no);
                        permuted_plan.push_back(permuted_op_no);
                        permuted_set[permuted_op_no]++;
                    }
                    auto ret = plans_sets.insert(permuted_set);
                    if (ret.second) {
                        // Element inserted, adding to the new frontier
                        //plans.insert(permuted_plan);
                        current_plans.push_back(get_plan_for_op_ids(permuted_plan));
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
        for (const SearchEngine::Plan& plan : current_plans) {
            save_plan(plan, true);
        }
    }
    cout << "done! [t=" << utils::g_timer << "]" << endl;
    if (!enough_plans_found((int)current_plans.size())) {
        // Not all plans are found yet
        std::unordered_set<vector<int>> plans;
        for (const SearchEngine::Plan& current_plan : current_plans) {
            vector<int> found_plan;
            for (const GlobalOperator *op : current_plan) {
                int op_no = get_op_index_hacked(op);
                found_plan.push_back(op_no);
            }
            plans.insert(found_plan);
        }
        shared_ptr<AbstractTask> reformulated_task = create_reformulated_task(plans);

        ofstream os(filename);
        reformulated_task->dump_to_SAS(os);
        cout << "done! [t=" << utils::g_timer << "]" << endl;
    }
}

SearchEngine::Plan ForbidIterativeSearch::get_plan_for_op_ids(const vector<int>& plan_ids) const {
    SearchEngine::Plan plan;
    for (int op_no : plan_ids) {
        plan.push_back(&g_operators[op_no]);
    }
    return plan;
}

shared_ptr<AbstractTask> ForbidIterativeSearch::create_reformulated_task(std::unordered_set<vector<int>>& plans) const {
    assert(reformulate == TaskReformulationType::FORBID_SINGLE_PLAN_MULTISET || reformulate == TaskReformulationType::FORBID_MULTIPLE_PLAN_MULTISETS);

    if (reformulate == TaskReformulationType::FORBID_SINGLE_PLAN_MULTISET) {
        return create_reformulated_task_multiset(plans);
    } 
    
    return create_reformulated_task_multisets(plans);
}

shared_ptr<AbstractTask> ForbidIterativeSearch::create_reformulated_task_multiset(std::unordered_set<vector<int>>& plans) const {
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
            SearchEngine::Plan global_plan;
            for (int op_no : plan) {
                global_plan.push_back(&g_operators[op_no]);
            }
            save_plan(global_plan, true);
        }
*/        
    }
    return make_shared<extra_tasks::MultisetForbidReformulatedTask>(g_root_task(), multiset, change_operator_names);
}

shared_ptr<AbstractTask> ForbidIterativeSearch::create_reformulated_task_multisets(std::unordered_set<vector<int>>& plans) const {
    // Creating a multiset from each plan
    std::vector<std::unordered_map<int, int>> multisets;
    cout << "Forbidding " << plans.size() << " plans" << endl;
    for (vector<int> plan : plans) {
        // Building multiset for that plan
        std::unordered_map<int, int> plan_multiset;
        plan_to_multiset(plan, plan_multiset);
        multisets.push_back(plan_multiset);
    }
    return make_shared<extra_tasks::MultisetsForbidReformulatedTask>(g_root_task(), multisets, change_operator_names);
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

void ForbidIterativeSearch::reformulate_and_dump_read_plans_and_dump_graph(const char* filename, bool optimal) const {
    ofstream os(filename);
    assert(number_of_plans >= 0);
    plans::PlansGraph* forbid_graph = new plans::PlansGraph(number_of_plans, optimal);
    for (int plan_no=1; plan_no <= read_plans_and_dump_graph; ++plan_no) {
        ifstream planfile;
        string fname = "found_plans/done/sas_plan." + std::to_string(plan_no);
        planfile.open(fname);

        if (!planfile.is_open()) {
            cout << "File is not open!" << endl;
        }
        string line;
        //cout << "-----------------------------------------------------------------------" << endl;
        //cout << "Plan " << plan_no << endl;
        vector<string> plan;
        while(std::getline(planfile, line)) {
            if (line.size() == 0 || line[0] == ';')
                continue;
            string op_name = line.substr(1, line.size()-2);
            //  cout << op_name << endl;
            plan.push_back(op_name);
        }
        forbid_graph->add_non_deterministic_plan(plan);
    }
    forbid_graph->dump_graph_file();
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
    parser.add_enum_option(
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
    parser.add_enum_option(
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
    parser.add_option<int>("read_plans_and_dump_graph", "reading the plans and creating a graph, and then dumping it", "0");
    parser.add_option<bool>("dumping_plans_files", "Dumping the plans to files", "true");

}

static SearchEngine *_parse(OptionParser &parser) {
    parser.document_synopsis("Forbid iterative search", "");

    SearchEngine::add_options_to_parser(parser);
    ForbidIterativeSearch::add_forbid_plan_reformulation_option(parser);
    Options opts = parser.parse();

    ForbidIterativeSearch *engine = nullptr;
    if (!parser.dry_run()) {
        engine = new ForbidIterativeSearch(opts);
    }

    return engine;
}

static Plugin<SearchEngine> _plugin("forbid_iterative", _parse);


