#include "graph_creator.h"

#include "group.h"
#include "permutation.h"

#include "../global_operator.h"
#include "../global_state.h"
#include "../globals.h"
#include "../option_parser.h"

#include "../utils/timer.h"

#include "../bliss/graph.h"


using namespace std;

//namespace structural_symmetries {

//TODO: Add vertex for axioms.
enum color_t {PREDICATE_VERTEX, VALUE_VERTEX, PRECOND_VERTEX, EFFECT_VERTEX,
              GOAL_VERTEX, INIT_VERTEX, CONDITIONAL_EFFECT_VERTEX,
              CONDITIONAL_DELETE_EFFECT_VERTEX, MAX_VALUE};

static void out_of_memory_handler() {
    throw bliss::BlissMemoryOut();
}

GraphCreator::GraphCreator(const Options &opts)
    : time_bound(opts.get<int>("time_bound")),
//      generators_bound(opts.get<int>("generators_bound")),
      stabilize_initial_state(opts.get<bool>("stabilize_initial_state")) {
}

GraphCreator::~GraphCreator() {
}

bool GraphCreator::compute_symmetries(Group *group) {
    bool success = false;
    new_handler original_new_handler = set_new_handler(out_of_memory_handler);
    try {
        utils::Timer timer;
        cout << "Initializing symmetry " << endl;
        bliss::Digraph bliss_graph = bliss::Digraph();
        create_bliss_directed_graph(group, bliss_graph);
        bliss_graph.set_splitting_heuristic(bliss::Digraph::shs_flm);
        bliss_graph.set_time_limit(time_bound);
//        bliss_graph.set_generators_bound(generators_bound);
        bliss::Stats stats1;
        cout << "Using Bliss to find group generators" << endl;
        bliss_graph.canonical_form(stats1,&(Group::add_permutation),group);
        cout << "Got " << group->get_num_generators()
             << " group generators" << endl;
//        group->dump_generators();
        cout << "Got " << group->num_identity_generators
             << " additional group generators that are identity on states (but not on operators)" << endl;
        cout << "Done initializing symmetries: " << timer << endl;
        group->statistics();
        success = true;
    } catch (bliss::BlissException &e) {
        e.dump();
    }
    set_new_handler(original_new_handler);
    return success;
}

void GraphCreator::create_bliss_directed_graph(Group *group, bliss::Digraph &bliss_graph) const {
    // Differ from create_bliss_graph() in (a) having one node per action (incoming arcs from pre, outgoing to eff),
    //                                 and (b) not having a node for goal, recoloring the respective values.
   // initialization

    int num_vars = g_variable_domain.size();
    int num_of_vertex = num_vars;
    for (int num_of_variable = 0; num_of_variable < num_vars; num_of_variable++){
        group->dom_sum_by_var.push_back(num_of_vertex);
        num_of_vertex+=g_variable_domain[num_of_variable];
        for(int num_of_value = 0; num_of_value < g_variable_domain[num_of_variable]; num_of_value++){
        	group->var_by_val.push_back(num_of_variable);
        }
    }

    group->set_permutation_num_variables(num_vars);
    group->set_permutation_length(num_of_vertex);

    int idx = 0;
    // add vertex for each varaible
    for (int i = 0; i < num_vars; i++){
       idx = bliss_graph.add_vertex(PREDICATE_VERTEX);
    }
    // now add values vertices for each predicate
    for (int i = 0; i < num_vars; i++){
       for (int j = 0; j < g_variable_domain[i]; j++){
          idx = bliss_graph.add_vertex(VALUE_VERTEX);
          bliss_graph.add_edge(idx,i);
       }
    }

    // now add vertices for operators
    for (size_t i = 0; i < g_operators.size(); i++){
        const GlobalOperator& op = g_operators[i];
        bliss_graph.add_vertex(MAX_VALUE + op.get_cost());
    }

    // now add vertices for axioms
    for (size_t i = 0; i < g_axioms.size(); i++){
        const GlobalOperator& op = g_axioms[i];
        bliss_graph.add_vertex(MAX_VALUE + op.get_cost());
    }

    for (size_t i = 0; i < g_operators.size(); i++){
        const GlobalOperator& op = g_operators[i];
        int op_idx = group->get_permutation_length() + i;
        add_operator_directed_graph(group, bliss_graph, op, op_idx);
    }

    for (size_t i = 0; i < g_axioms.size(); i++){
        const GlobalOperator& op = g_axioms[i];
        int op_idx = group->get_permutation_length() + g_operators.size() + i;
        add_operator_directed_graph(group, bliss_graph, op, op_idx);
    }

    if (stabilize_initial_state) {
        /*
          Note: We cannot color both abstract initial and goal states, because
          there exist problme instances where a goal fact is already true in
          the initial state (e.g. psr-small:p02-s5...), hence we would recolor
          the vertex and either not stabilizing the initial state or the goal
          state, depending on the order of coloring.
        */
        idx = bliss_graph.add_vertex(INIT_VERTEX);
        for (int var = 0; var < group->get_permutation_num_variables(); ++var) {
            int val = g_initial_state_data[var];
            int init_idx = group->get_index_by_var_val_pair(var, val);
            bliss_graph.add_edge(idx, init_idx);
        }
    }

    // Recoloring the goal values
    for (size_t i = 0; i < g_goal.size(); i++){
        int var = g_goal[i].first;
        int val = g_goal[i].second;
        int goal_idx = group->get_index_by_var_val_pair(var, val);
        bliss_graph.change_color(goal_idx, GOAL_VERTEX);
    }


}

//TODO: Use separate color for axioms
//TODO: Change the order of vertices creation to support keeping actions in the permutation (no need for keeping conditional effect vertices).
void GraphCreator::add_operator_directed_graph(Group *group, bliss::Digraph &bliss_graph,
                                               const GlobalOperator& op, int op_idx) const {
    const vector<GlobalCondition> &conditions = op.get_preconditions();
    const vector<GlobalEffect> &effects = op.get_effects();
    vector<pair<int, int> > prevails;
    for (size_t i = 0; i < conditions.size(); ++i) {
        int cond_var = conditions[i].var;
        bool is_prevail = true;
        for (size_t j = 0; j < effects.size(); ++j) {
            if (effects[j].var == cond_var) {
                is_prevail = false;
                break;
            }
        }
        if (is_prevail) {
            prevails.push_back(make_pair(cond_var, conditions[i].val));
        }
    }
    // for every effect variable, collect the value of the possible
    // precondition on that variabale (or -1)
    vector<int> effects_pre_vals(effects.size(), -1);
    for (size_t i = 0; i < effects.size(); ++i) {
        int eff_var = effects[i].var;
        for (size_t j = 0; j < conditions.size(); ++j) {
            if (conditions[j].var == eff_var) {
                assert(conditions[j].val != -1);
                effects_pre_vals[i] = conditions[j].val;
                break;
            }
        }
    }

    for (size_t idx1 = 0; idx1 < prevails.size(); idx1++){
        int var = prevails[idx1].first;
        int val = prevails[idx1].second;
        //int prv_idx = Permutation::dom_sum_by_var[var] + val;
        int prv_idx = group->get_index_by_var_val_pair(var, val);
        bliss_graph.add_edge(prv_idx, op_idx);
    }
    for (size_t idx1 = 0; idx1 < effects.size(); idx1++){
        int var = effects[idx1].var;
        int pre_val = effects_pre_vals[idx1];

        if (pre_val!= -1){
            //int pre_idx = Permutation::dom_sum_by_var[var] + pre_val;
            int pre_idx = group->get_index_by_var_val_pair(var, pre_val);

            bliss_graph.add_edge(pre_idx, op_idx);
        }

        int eff_val = effects[idx1].val;
        //int eff_idx = Permutation::dom_sum_by_var[var] + eff_val;
        int eff_idx = group->get_index_by_var_val_pair(var, eff_val);

        if (effects[idx1].conditions.size() == 0) {
            bliss_graph.add_edge(op_idx, eff_idx);
        } else {
//            	cout << "Adding a node for conditional effect" << endl;
            // Adding a node for each condition. An edge from op to node, an edge from node to eff,
            // for each cond, an edge from cond to node.
            color_t effect_color = CONDITIONAL_EFFECT_VERTEX;
            if (effect_can_be_overwritten(idx1, effects)) {
                effect_color = CONDITIONAL_DELETE_EFFECT_VERTEX;
            }
            int cond_op_idx = bliss_graph.add_vertex(effect_color);
            bliss_graph.add_edge(op_idx, cond_op_idx); // Edge from operator to conditional effect
            bliss_graph.add_edge(cond_op_idx, eff_idx); // Edge from conditional effect to effect
            // Adding edges for conds
            for (size_t c = 0; c < effects[idx1].conditions.size(); c++){
                int c_var = effects[idx1].conditions[c].var;
                int c_val = effects[idx1].conditions[c].val;
                //int c_idx = Permutation::dom_sum_by_var[c_var] + c_val;
                int c_idx = group->get_index_by_var_val_pair(c_var, c_val);

                bliss_graph.add_edge(c_idx, cond_op_idx); // Edge from condition to conditional effect
            }
        }
    }

}

bool GraphCreator::effect_can_be_overwritten(int ind, const std::vector<GlobalEffect> &effects) const {
    // Checking whether the effect is a delete effect that can be overwritten by an add effect
    int num_effects = effects.size();
    assert(ind < num_effects);
    int var = effects[ind].var;
    int eff_val = effects[ind].val;
    if (eff_val != g_variable_domain[var] - 1) // the value is not none_of_those
        return false;

    // Go over the next effects of the same variable, skipping the none_of_those
    for (int i=ind+1; i < num_effects; i++) {
        if (var != effects[i].var) // Next variable
            return false;
        if (effects[i].val == g_variable_domain[var] - 1)
            continue;
        // Found effect on the same variable which is not none_of_those
        return true;
    }
    return false;
}
//}
