#include "graph_creator.h"

#include "group.h"
#include "permutation.h"

#include "../task_proxy.h"

#include "../utils/collections.h"
#include "../utils/timer.h"

#include "../bliss/graph.h"

#include <map>


using namespace std;

//TODO: Add vertex for axioms.
enum color_t {VARIABLE_VERTEX, VALUE_VERTEX, GOAL_VERTEX, INIT_VERTEX,
              CONDITIONAL_EFFECT_VERTEX, CONDITIONAL_DELETE_EFFECT_VERTEX,
              MAX_VALUE};

static map<color_t, string> dot_colors = {
    {VARIABLE_VERTEX, "green"},
    {VALUE_VERTEX, "gold"},
    {GOAL_VERTEX, "red"},
    {INIT_VERTEX, "orange"},
    {CONDITIONAL_EFFECT_VERTEX, "dodgerblue"},
    {CONDITIONAL_DELETE_EFFECT_VERTEX, "navyblue"},
    {MAX_VALUE, "lightskyblue"},
};

static void out_of_memory_handler() {
    throw bliss::BlissMemoryOut();
}

// Function that is called from the graph automorphism tool.
void add_permutation_to_group(void *group, unsigned int, const unsigned int *permutation) {
    ((Group*) group)->add_raw_generator(permutation);
}

bool GraphCreator::compute_symmetries(
    const TaskProxy &task_proxy,
    const bool stabilize_initial_state,
    const int time_bound,
    const bool dump_symmetry_graph,
    Group *group) {
    bool success = false;
    new_handler original_new_handler = set_new_handler(out_of_memory_handler);
    try {
        utils::Timer timer;
        cout << "Initializing symmetry " << endl;
        bliss::Digraph bliss_graph = bliss::Digraph();
        create_bliss_directed_graph(
            task_proxy, stabilize_initial_state, dump_symmetry_graph, group, bliss_graph);
        bliss_graph.set_splitting_heuristic(bliss::Digraph::shs_flm);
        bliss_graph.set_time_limit(time_bound);
//        bliss_graph.set_generators_bound(generators_bound);
        bliss::Stats stats1;
        cout << "Using Bliss to find group generators" << endl;
        bliss_graph.canonical_form(stats1,&(add_permutation_to_group),group);
        cout << "Done initializing symmetries: " << timer << endl;
        group->statistics();
        success = true;
    } catch (bliss::BlissException &e) {
        e.dump();
    }
    set_new_handler(original_new_handler);
    return success;
}

struct DotNode {
    int index;
    string label;
    string color;

    DotNode(int index, string label, string color)
        : index(index), label(label), color(color) {
    }

    void print() const {
        cout << "    node" << index << " [shape=circle, label="
             << label << ", style=filled, colorscheme=\"X11\", fillcolor=\""
             << color << "\"];" << endl;
    }
};

struct DotGraph {
    vector<DotNode> nodes;
    vector<vector<int>> neighbors;

    void add_node(int index, string label, string color) {
        nodes.emplace_back(index, label, color);
        neighbors.push_back(vector<int>());
    }

    void change_node_color(int index, string color) {
        assert(utils::in_bounds(index, nodes));
        DotNode &node = nodes[index];
        assert(node.index == index);
        node.color = color;
    }

    void add_edge(int from, int to) {
        assert(utils::in_bounds(from, neighbors));
        assert(utils::in_bounds(from, nodes));
        assert(utils::in_bounds(to, nodes));
        neighbors[from].push_back(to);
    }

    void print() const {
        cout << "digraph symmetry_graph {" << endl;
        for (const DotNode &node : nodes) {
            node.print();
        }
        for (size_t node_index = 0; node_index < neighbors.size(); ++node_index) {
            const vector<int> &node_neighbors = neighbors[node_index];
            for (int neighbor : node_neighbors) {
                cout << "    node" << node_index << " -> node" << neighbor << endl;
            }
        }
        cout << "}" << endl;
    }
};

static DotGraph dot_graph;

void GraphCreator::create_bliss_directed_graph(
    const TaskProxy &task_proxy,
    const bool stabilize_initial_state,
    const bool dump_symmetry_graph,
    Group *group,
    bliss::Digraph &bliss_graph) const {
    // Differ from create_bliss_graph() in (a) having one node per action (incoming arcs from pre, outgoing to eff),
    //                                 and (b) not having a node for goal, recoloring the respective values.

    // initialization
    VariablesProxy vars = task_proxy.get_variables();
    int num_vars = vars.size();
    int num_of_vertex = num_vars;
    for (VariableProxy var : vars) {
        int var_id = var.get_id();
        group->add_to_dom_sum_by_var(num_of_vertex);
        num_of_vertex += var.get_domain_size();
        for(int num_of_value = 0; num_of_value < var.get_domain_size(); num_of_value++){
            group->add_to_var_by_val(var_id);
        }
    }

    group->set_permutation_num_variables(num_vars);
    group->set_permutation_length(num_of_vertex);

    int idx = 0;
    // add vertex for each variable
    for (int i = 0; i < num_vars; i++) {
       idx = bliss_graph.add_vertex(VARIABLE_VERTEX);

       if (dump_symmetry_graph) {
           dot_graph.add_node(idx, "var" + to_string(i), dot_colors[VARIABLE_VERTEX]);
       }
    }
    // now add values vertices for each predicate
    for (VariableProxy var : vars) {
        int var_id = var.get_id();
        for (int i = 0; i < var.get_domain_size(); i++){
            idx = bliss_graph.add_vertex(VALUE_VERTEX);
            bliss_graph.add_edge(idx, var_id);

            if (dump_symmetry_graph) {
                dot_graph.add_node(idx, "val" + to_string(i), dot_colors[VALUE_VERTEX]);
                dot_graph.add_edge(idx, var_id);
            }
        }
    }

    // now add vertices for operators
    for (OperatorProxy op : task_proxy.get_operators()) {
        int color = MAX_VALUE + op.get_cost();
        idx = bliss_graph.add_vertex(color);

        if (dump_symmetry_graph) {
            dot_graph.add_node(
                idx,
                // TODO: see below: also change to get_id()?
                "op" + to_string(op.get_global_operator_id().get_index()),
                dot_colors[MAX_VALUE]);
        }
    }

    // now add vertices for axioms
    for (OperatorProxy ax : task_proxy.get_axioms()) {
        int color = MAX_VALUE + ax.get_cost();
        bliss_graph.add_vertex(color);

        if (dump_symmetry_graph) {
            dot_graph.add_node(
                idx,
                "ax" + to_string(ax.get_id()),
                dot_colors[MAX_VALUE]);
        }
    }

    for (OperatorProxy op : task_proxy.get_operators()) {
        // TODO: can we use op.get_id() instead? What does the index stand for?
        int op_idx = group->get_permutation_length() + op.get_global_operator_id().get_index();
        add_operator_directed_graph(dump_symmetry_graph, group, bliss_graph, op, op_idx);
    }

    for (size_t i = 0; i < task_proxy.get_axioms().size(); i++) {
        OperatorProxy ax = task_proxy.get_axioms()[i];
        int op_idx = group->get_permutation_length() + task_proxy.get_operators().size() + i;
        add_operator_directed_graph(dump_symmetry_graph, group, bliss_graph, ax, op_idx);
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

        if (dump_symmetry_graph) {
            dot_graph.add_node(idx, "init", dot_colors[INIT_VERTEX]);
        }

        for (FactProxy init_fact : task_proxy.get_initial_state()) {
            int init_idx = group->get_index_by_var_val_pair(
                init_fact.get_variable().get_id(), init_fact.get_value());
            bliss_graph.add_edge(idx, init_idx);

            if (dump_symmetry_graph) {
                dot_graph.add_edge(idx, init_idx);
            }
        }
    }

    // Recoloring the goal values
    for (FactProxy goal_fact : task_proxy.get_goals()) {
        int goal_idx = group->get_index_by_var_val_pair(goal_fact.get_variable().get_id(), goal_fact.get_value());
        bliss_graph.change_color(goal_idx, GOAL_VERTEX);

        if (dump_symmetry_graph) {
            dot_graph.change_node_color(goal_idx, dot_colors[GOAL_VERTEX]);
        }
    }

    if (dump_symmetry_graph) {
        dot_graph.print();
    }
}

//TODO: Use separate color for axioms
void GraphCreator::add_operator_directed_graph(
    const bool dump_symmetry_graph,
    Group *group, bliss::Digraph &bliss_graph,
    const OperatorProxy& op,
    int op_idx) const {
    PreconditionsProxy preconditions = op.get_preconditions();
    EffectsProxy effects = op.get_effects();
    vector<pair<int, int> > prevails;
    for (FactProxy prec_fact : preconditions) {
        VariableProxy cond_var = prec_fact.get_variable();
        bool is_prevail = true;
        for (EffectProxy effect : effects) {
            if (effect.get_fact().get_variable() == cond_var) {
                is_prevail = false;
                break;
            }
        }
        if (is_prevail) {
            prevails.push_back(make_pair(cond_var.get_id(), prec_fact.get_value()));
        }
    }

    // for every effect variable, collect the value of the possible
    // precondition on that variabale (or -1)
    vector<int> effects_pre_vals(effects.size(), -1);
    for (size_t i = 0; i < effects.size(); ++i) {
        EffectProxy effect = effects[i];
        VariableProxy eff_var = effect.get_fact().get_variable();
        for (FactProxy pre_fact : preconditions) {
            if (pre_fact.get_variable() == eff_var) {
                assert(pre_fact.get_value() != -1);
                effects_pre_vals[i] = pre_fact.get_value();
            }
        }
    }

    for (size_t idx1 = 0; idx1 < prevails.size(); idx1++){
        int var = prevails[idx1].first;
        int val = prevails[idx1].second;
        int prv_idx = group->get_index_by_var_val_pair(var, val);
        bliss_graph.add_edge(prv_idx, op_idx);

        if (dump_symmetry_graph) {
            dot_graph.add_edge(prv_idx, op_idx);
        }
    }

    for (size_t idx1 = 0; idx1 < effects.size(); idx1++){
        int var_id = effects[idx1].get_fact().get_variable().get_id();
        int pre_val = effects_pre_vals[idx1];

        if (pre_val!= -1){
            int pre_idx = group->get_index_by_var_val_pair(var_id, pre_val);
            bliss_graph.add_edge(pre_idx, op_idx);

            if (dump_symmetry_graph) {
                dot_graph.add_edge(pre_idx, op_idx);
            }
        }

        int eff_val = effects[idx1].get_fact().get_value();
        int eff_idx = group->get_index_by_var_val_pair(var_id, eff_val);

        if (effects[idx1].get_conditions().empty()) {
            bliss_graph.add_edge(op_idx, eff_idx);

            if (dump_symmetry_graph) {
                dot_graph.add_edge(op_idx, eff_idx);
            }
        } else {
//                cout << "Adding a node for conditional effect" << endl;
            // Adding a node for each condition. An edge from op to node, an edge from node to eff,
            // for each cond, an edge from cond to node.
            color_t effect_color = CONDITIONAL_EFFECT_VERTEX;
            if (effect_can_be_overwritten(idx1, effects)) {
                effect_color = CONDITIONAL_DELETE_EFFECT_VERTEX;
            }
            int cond_op_idx = bliss_graph.add_vertex(effect_color);
            bliss_graph.add_edge(op_idx, cond_op_idx); // Edge from operator to conditional effect
            bliss_graph.add_edge(cond_op_idx, eff_idx); // Edge from conditional effect to effect

            if (dump_symmetry_graph) {
                dot_graph.add_node(
                    cond_op_idx, "effect" + to_string(idx1), dot_colors[effect_color]);
                dot_graph.add_edge(op_idx, cond_op_idx);
                dot_graph.add_edge(cond_op_idx, eff_idx);
            }

            // Adding edges for conds
            for (FactProxy prec_fact : effects[idx1].get_conditions()) {
                int c_idx = group->get_index_by_var_val_pair(
                    prec_fact.get_variable().get_id(), prec_fact.get_value());

                // Edge from condition to conditional effect
                bliss_graph.add_edge(c_idx, cond_op_idx);

                if (dump_symmetry_graph) {
                    dot_graph.add_edge(c_idx, cond_op_idx);
                }
            }
        }
    }

}

bool GraphCreator::effect_can_be_overwritten(int ind, const EffectsProxy &effects) const {
    // Checking whether the effect is a delete effect that can be overwritten by an add effect
    int num_effects = effects.size();

    assert(ind < num_effects);
    FactProxy effect_fact = effects[ind].get_fact();
    VariableProxy effect_var = effect_fact.get_variable();
    int eff_val = effect_fact.get_value();
    if (eff_val != effect_fact.get_variable().get_domain_size() - 1) // the value is not none_of_those
        return false;

    // Go over the next effects of the same variable, skipping the none_of_those
    // Warning! It seems that we assume here that the variables in the effects are ordered by effect variables.
    //          Should be changed!
    for (int i=ind+1; i < num_effects; i++) {
        if (effect_var != effects[i].get_fact().get_variable()) // Next variable
            return false;
        if (effects[i].get_fact().get_value() == effect_fact.get_variable().get_domain_size() - 1)
            continue;
        // Found effect on the same variable which is not none_of_those
        return true;
    }
    return false;
}
