#include "globals.h"

#include "axioms.h"
#include "causal_graph.h"
#include "global_operator.h"
#include "global_state.h"
#include "heuristic.h"
#include "successor_generator.h"

#include "algorithms/int_packer.h"
#include "tasks/root_task.h"
#include "tasks/plan_forbid_reformulated_task.h"
#include "utils/logging.h"
#include "utils/rng.h"
#include "utils/system.h"
#include "utils/timer.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <exception>
#include <system_error>


using namespace std;
using utils::ExitCode;

const int PRE_FILE_VERSION = 3;


// TODO: This needs a proper type and should be moved to a separate
//       mutexes.cc file or similar, accessed via something called
//       g_mutexes. (Right now, the interface is via global function
//       are_mutex, which is at least better than exposing the data
//       structure globally.)

static vector<vector<set<FactPair>>> g_inconsistent_facts;

bool test_goal(const GlobalState &state) {
    for (size_t i = 0; i < g_goal.size(); ++i) {
        if (state[g_goal[i].first] != g_goal[i].second) {
            return false;
        }
    }
    return true;
}

int calculate_plan_cost(const vector<const GlobalOperator *> &plan) {
    // TODO: Refactor: this is only used by save_plan (see below)
    //       and the SearchEngine classes and hence should maybe
    //       be moved into the SearchEngine (along with save_plan).
    int plan_cost = 0;
    for (size_t i = 0; i < plan.size(); ++i) {
        plan_cost += plan[i]->get_cost();
    }
    return plan_cost;
}

void save_plan(const vector<const GlobalOperator *> &plan,
               bool generates_multiple_plan_files) {
    // TODO: Refactor: this is only used by the SearchEngine classes
    //       and hence should maybe be moved into the SearchEngine.
    ostringstream filename;
    filename << g_plan_filename;
    int plan_number = g_num_previously_generated_plans + 1;
    if (generates_multiple_plan_files || g_is_part_of_anytime_portfolio) {
        filename << "." << plan_number;
    } else {
        assert(plan_number == 1);
    }
    ofstream outfile(filename.str());
    for (size_t i = 0; i < plan.size(); ++i) {
//        cout << plan[i]->get_name() << " (" << plan[i]->get_cost() << ")" << endl;
        outfile << "(" << plan[i]->get_name() << ")" << endl;
    }
    int plan_cost = calculate_plan_cost(plan);
    outfile << "; cost = " << plan_cost << " ("
            << (is_unit_cost() ? "unit cost" : "general cost") << ")" << endl;
    outfile.close();
//    cout << "Plan length: " << plan.size() << " step(s)." << endl;
//    cout << "Plan cost: " << plan_cost << endl;
    ++g_num_previously_generated_plans;
}

vector<const GlobalOperator *> load_plan(string path_to_plan_file) {
    // TODO: Refactor: this is only used by the SearchEngine classes
    //       and hence should maybe be moved into the SearchEngine.

    // Creating a hashmap from operator names to pointers
    std::unordered_map<string, const GlobalOperator *> ops_by_names;
    for (const GlobalOperator &op : g_operators) {
        const GlobalOperator* op_p = &op;
        //cout << "Reading op " << op_p->get_name() << endl;
        ops_by_names[op.get_name()] = op_p;
        auto it = ops_by_names.find(op.get_name());
        if (it == ops_by_names.end()) {
            cout << "Problem adding operator!!!" << endl;
            utils::exit_with(ExitCode::INPUT_ERROR);
        }
        //cout << "Added operator " << it->second->get_name() << endl;
    }
    return load_single_plan(path_to_plan_file, ops_by_names);
}

vector<vector<const GlobalOperator *>> load_plans(string path_to_plan_folder, int num_plans) {
    // TODO: Refactor: this is only used by the SearchEngine classes
    //       and hence should maybe be moved into the SearchEngine.
    // Assumption: the files are named sas_plan.1 .. sas_plan.num_plans
    // TODO: make it work with other OS
    // Creating a hashmap from operator names to pointers
    std::unordered_map<string, const GlobalOperator *> ops_by_names;
    for (const GlobalOperator &op : g_operators) {
        const GlobalOperator* op_p = &op;
        //cout << "Reading op " << op_p->get_name() << endl;
        ops_by_names[op.get_name()] = op_p;
        auto it = ops_by_names.find(op.get_name());
        if (it == ops_by_names.end()) {
            cout << "Problem adding operator!!!" << endl;
            utils::exit_with(ExitCode::INPUT_ERROR);
        }
        //cout << "Added operator " << it->second->get_name() << endl;
    }
    vector<vector<const GlobalOperator *>> plans;
    for (int plan_no=1; plan_no <= num_plans; ++plan_no) {
        string fname = path_to_plan_folder + "/" + "sas_plan." + std::to_string(plan_no);
        //cout << "FNAME: " << fname << endl;
        plans.push_back(load_single_plan(fname, ops_by_names));
    }
    return plans;
}


std::vector<const GlobalOperator *> load_single_plan(std::string path_to_plan_file,
        const std::unordered_map<std::string, const GlobalOperator *>& ops_by_names) {
    // TODO: Refactor: this is only used by the SearchEngine classes
    //       and hence should maybe be moved into the SearchEngine.
    //cout << "Reading plan from file " << path_to_plan_file << endl;
    ifstream planfile;
    planfile.open(path_to_plan_file);

    if (!planfile.is_open()) {
        throw std::system_error(errno, std::system_category(), "failed to open file");

        cerr << "File is not open!" << endl;
        utils::exit_with(ExitCode::INPUT_ERROR);
    }
    string line;
    //cout << "-----------------------------------------------------------------------" << endl;
    vector<const GlobalOperator *> plan;
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
                cout << "#" << op_name << "#   Operator not found!!!" << endl;
                cout << "Operator names:" << endl;
                for (auto name : ops_by_names) {
                    cout << "#" << name.first << "#" << endl;
                }
                utils::exit_with(ExitCode::INPUT_ERROR);
            }
        }
        //cout << "Name " << it->first << endl;
        //cout << "Found operator " << it->second->get_name() << endl;
        plan.push_back(it->second);
    }
    return plan;
}


void check_magic(istream &in, string magic) {
    string word;
    in >> word;
    if (word != magic) {
        cout << "Failed to match magic word '" << magic << "'." << endl;
        cout << "Got '" << word << "'." << endl;
        if (magic == "begin_version") {
            cerr << "Possible cause: you are running the planner "
                 << "on a preprocessor file from " << endl
                 << "an older version." << endl;
        }
        utils::exit_with(ExitCode::INPUT_ERROR);
    }
}

void read_and_verify_version(istream &in) {
    int version;
    check_magic(in, "begin_version");
    in >> version;
    check_magic(in, "end_version");
    if (version != PRE_FILE_VERSION) {
        cerr << "Expected preprocessor file version " << PRE_FILE_VERSION
             << ", got " << version << "." << endl;
        cerr << "Exiting." << endl;
        utils::exit_with(ExitCode::INPUT_ERROR);
    }
}

void read_metric(istream &in) {
    check_magic(in, "begin_metric");
    in >> g_use_metric;
    check_magic(in, "end_metric");
}

void read_variables(istream &in) {
    int count;
    in >> count;
    for (int i = 0; i < count; ++i) {
        check_magic(in, "begin_variable");
        string name;
        in >> name;
        g_variable_name.push_back(name);
        int layer;
        in >> layer;
        g_axiom_layers.push_back(layer);
        int range;
        in >> range;
        g_variable_domain.push_back(range);
        in >> ws;
        vector<string> fact_names(range);
        for (size_t j = 0; j < fact_names.size(); ++j)
            getline(in, fact_names[j]);
        g_fact_names.push_back(fact_names);
        check_magic(in, "end_variable");
    }
}

void read_mutexes(istream &in) {
    g_inconsistent_facts.resize(g_variable_domain.size());
    for (size_t i = 0; i < g_variable_domain.size(); ++i)
        g_inconsistent_facts[i].resize(g_variable_domain[i]);

    int num_mutex_groups;
    in >> num_mutex_groups;

    /* NOTE: Mutex groups can overlap, in which case the same mutex
       should not be represented multiple times. The current
       representation takes care of that automatically by using sets.
       If we ever change this representation, this is something to be
       aware of. */
    vector<vector<FactPair>> g_invariant_groups;

    for (int i = 0; i < num_mutex_groups; ++i) {
        check_magic(in, "begin_mutex_group");
        int num_facts;
        in >> num_facts;
        vector<FactPair> invariant_group;
        invariant_group.reserve(num_facts);
        for (int j = 0; j < num_facts; ++j) {
            int var;
            int value;
            in >> var >> value;
            invariant_group.emplace_back(var, value);
        }
        g_invariant_groups.push_back(invariant_group);
        check_magic(in, "end_mutex_group");
        for (const FactPair &fact1 : invariant_group) {
            for (const FactPair &fact2 : invariant_group) {
                if (fact1.var != fact2.var) {
                    /* The "different variable" test makes sure we
                       don't mark a fact as mutex with itself
                       (important for correctness) and don't include
                       redundant mutexes (important to conserve
                       memory). Note that the translator (at least
                       with default settings) removes mutex groups
                       that contain *only* redundant mutexes, but it
                       can of course generate mutex groups which lead
                       to *some* redundant mutexes, where some but not
                       all facts talk about the same variable. */
                    g_inconsistent_facts[fact1.var][fact1.value].insert(fact2);
                }
            }
        }
    }
}

void read_goal(istream &in) {
    check_magic(in, "begin_goal");
    int count;
    in >> count;
    if (count < 1) {
        cerr << "Task has no goal condition!" << endl;
        utils::exit_with(ExitCode::INPUT_ERROR);
    }
    for (int i = 0; i < count; ++i) {
        int var, val;
        in >> var >> val;
        g_goal.push_back(make_pair(var, val));
    }
    check_magic(in, "end_goal");
}

void dump_goal() {
    cout << "Goal Conditions:" << endl;
    for (size_t i = 0; i < g_goal.size(); ++i)
        cout << "  " << g_variable_name[g_goal[i].first] << ": "
             << g_goal[i].second << endl;
}

void read_operators(istream &in) {
    int count;
    in >> count;
    for (int i = 0; i < count; ++i)
        g_operators.push_back(GlobalOperator(in, false));
}

void read_axioms(istream &in) {
    int count;
    in >> count;
    for (int i = 0; i < count; ++i)
        g_axioms.push_back(GlobalOperator(in, true));

    g_axiom_evaluator = new AxiomEvaluator(TaskProxy(*g_root_task()));
}

void read_everything(istream &in) {
    cout << "reading input... [t=" << utils::g_timer << "]" << endl;
    read_and_verify_version(in);
    read_metric(in);
    read_variables(in);
    read_mutexes(in);
    g_initial_state_data.resize(g_variable_domain.size());
    check_magic(in, "begin_state");
    for (size_t i = 0; i < g_variable_domain.size(); ++i) {
        in >> g_initial_state_data[i];
    }
    check_magic(in, "end_state");
    g_default_axiom_values = g_initial_state_data;

    read_goal(in);
    read_operators(in);
    read_axioms(in);

    /* TODO: We should be stricter here and verify that we
       have reached the end of "in". */

    cout << "done reading input! [t=" << utils::g_timer << "]" << endl;

    cout << "packing state variables..." << flush;
    assert(!g_variable_domain.empty());
    g_state_packer = new int_packer::IntPacker(g_variable_domain);
    cout << "done! [t=" << utils::g_timer << "]" << endl;

    int num_vars = g_variable_domain.size();
    int num_facts = 0;
    for (int var = 0; var < num_vars; ++var)
        num_facts += g_variable_domain[var];

    cout << "Variables: " << num_vars << endl;
    cout << "FactPairs: " << num_facts << endl;
    cout << "Bytes per state: "
         << g_state_packer->get_num_bins() * sizeof(int_packer::IntPacker::Bin)
         << endl;

    cout << "Building successor generator..." << flush;
    TaskProxy task_proxy(*g_root_task());
    g_successor_generator = new SuccessorGenerator(task_proxy);
    cout << "done! [t=" << utils::g_timer << "]" << endl;

    cout << "done initalizing global data [t=" << utils::g_timer << "]" << endl;
}

void dump_everything() {
    cout << "Use metric? " << g_use_metric << endl;
    cout << "Min Action Cost: " << g_min_action_cost << endl;
    cout << "Max Action Cost: " << g_max_action_cost << endl;
    // TODO: Dump the actual fact names.
    cout << "Variables (" << g_variable_name.size() << "):" << endl;
    for (size_t i = 0; i < g_variable_name.size(); ++i)
        cout << "  " << g_variable_name[i]
             << " (range " << g_variable_domain[i] << ")" << endl;
    State initial_state = TaskProxy(*g_root_task()).get_initial_state();
    cout << "Initial State (PDDL):" << endl;
    initial_state.dump_pddl();
    cout << "Initial State (FDR):" << endl;
    initial_state.dump_fdr();
    dump_goal();
    /*
    for(int i = 0; i < g_variable_domain.size(); ++i)
      g_transition_graphs[i]->dump();
    */
}

bool is_unit_cost() {
    return g_min_action_cost == 1 && g_max_action_cost == 1;
}

bool has_axioms() {
    return !g_axioms.empty();
}

void verify_no_axioms() {
    if (has_axioms()) {
        cerr << "Heuristic does not support axioms!" << endl << "Terminating."
             << endl;
        utils::exit_with(ExitCode::UNSUPPORTED);
    }
}

static int get_first_conditional_effects_op_id() {
    for (size_t i = 0; i < g_operators.size(); ++i) {
        const vector<GlobalEffect> &effects = g_operators[i].get_effects();
        for (size_t j = 0; j < effects.size(); ++j) {
            const vector<GlobalCondition> &cond = effects[j].conditions;
            if (!cond.empty())
                return i;
        }
    }
    return -1;
}

bool has_conditional_effects() {
    return get_first_conditional_effects_op_id() != -1;
}

void verify_no_conditional_effects() {
    int op_id = get_first_conditional_effects_op_id();
    if (op_id != -1) {
        cerr << "Heuristic does not support conditional effects "
             << "(operator " << g_operators[op_id].get_name() << ")" << endl
             << "Terminating." << endl;
        utils::exit_with(ExitCode::UNSUPPORTED);
    }
}

void verify_no_axioms_no_conditional_effects() {
    verify_no_axioms();
    verify_no_conditional_effects();
}

bool are_mutex(const FactPair &a, const FactPair &b) {
    if (a.var == b.var) {
        // Same variable: mutex iff different value.
        return a.value != b.value;
    }
    return bool(g_inconsistent_facts[a.var][a.value].count(b));
}

const shared_ptr<AbstractTask> g_root_task() {
    static shared_ptr<AbstractTask> root_task = make_shared<tasks::RootTask>();
    return root_task;
}

bool g_use_metric;
int g_min_action_cost = numeric_limits<int>::max();
int g_max_action_cost = 0;
vector<string> g_variable_name;
vector<int> g_variable_domain;
vector<vector<string>> g_fact_names;
vector<int> g_axiom_layers;
vector<int> g_default_axiom_values;
int_packer::IntPacker *g_state_packer;
vector<int> g_initial_state_data;
vector<pair<int, int>> g_goal;
vector<GlobalOperator> g_operators;
vector<GlobalOperator> g_axioms;
AxiomEvaluator *g_axiom_evaluator;
SuccessorGenerator *g_successor_generator;

string g_plan_filename = "sas_plan";
int g_num_previously_generated_plans = 0;
bool g_is_part_of_anytime_portfolio = false;

utils::Log g_log;

int g_symmetrical_states_generated;
int g_symmetry_improved_evaluations;
int g_improving_symmetrical_states;

// Parts for dumping SAS+ task, used in forbidding plan reformulation
void dump_version(std::ostream& os) {
	os << "begin_version" << endl;
	os << PRE_FILE_VERSION << endl;
	os << "end_version" << endl;
}

void dump_metric(std::ostream& os) {
	os << "begin_metric" << endl;
	os << g_use_metric << endl;
	os << "end_metric" << endl;
}

void dump_variable(std::ostream& os, std::string name, int axiom_layer, int domain, const std::vector<std::string>& values) {
	os << "begin_variable" << endl;
	os << name << endl;
	os << axiom_layer << endl;
	os << domain << endl;
	for (size_t j=0; j < values.size(); ++j)
		os << values[j] << endl;
	os << "end_variable" << endl;
}

void dump_mutexes(std::ostream& os) {
	os << g_invariant_groups.size() << endl;
	for (vector<FactPair> invariant_group : g_invariant_groups) {
		os << "begin_mutex_group" << endl;
		os << invariant_group.size() << endl;
		for (FactPair fact : invariant_group) {
			os << fact.var << endl;
			os << fact.value << endl;
		}
		os << "end_mutex_group" << endl;
	}
}

void dump_condition_SAS(std::ostream& os, GlobalCondition cond) {
	os << cond.var << " " << cond.val << std::endl;
}

void dump_pre_post_SAS(std::ostream& os, int pre, GlobalEffect eff) {
    os << eff.conditions.size() << std::endl;
    for (GlobalCondition cond : eff.conditions) {
    	dump_condition_SAS(os, cond);
    }
    os << eff.var << " " << pre << " " << eff.val << std::endl;
}

vector<vector<FactPair>> g_invariant_groups;

