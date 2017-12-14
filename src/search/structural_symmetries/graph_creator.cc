#include "graph_creator.h"

#include "group.h"
#include "permutation.h"

#include "../task_proxy.h"

#include "../utils/collections.h"
#include "../utils/timer.h"

#include "../bliss/graph.h"

#include <fstream>
#include <map>


using namespace std;

enum color_t {VARIABLE_VERTEX, VALUE_VERTEX, GOAL_VERTEX, INIT_VERTEX,
              CONDITIONAL_EFFECT_VERTEX, CONDITIONAL_DELETE_EFFECT_VERTEX, AXIOM_VERTEX, OPERATOR_VERTEX};

static map<color_t, string> dot_colors = {
    {VARIABLE_VERTEX, "green"},
    {VALUE_VERTEX, "gold"},
    {GOAL_VERTEX, "red"},
    {INIT_VERTEX, "orange"},
    {CONDITIONAL_EFFECT_VERTEX, "dodgerblue"},
    {CONDITIONAL_DELETE_EFFECT_VERTEX, "navyblue"},
    {AXIOM_VERTEX, "lightskyblue"},
    {OPERATOR_VERTEX, "lightskyblue"},
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
        cout << "Initializing symmetries" << endl;
        bliss::Digraph bliss_graph = bliss::Digraph();
        create_bliss_directed_graph(
            task_proxy, stabilize_initial_state, dump_symmetry_graph, group, bliss_graph);
        bliss_graph.set_splitting_heuristic(bliss::Digraph::shs_flm);
        bliss_graph.set_time_limit(time_bound);
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

    void write(ofstream &file) const {
        file << "    node" << index << " [shape=circle, label="
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

    void write() const {
        ofstream file;
        file.open ("symmetry-graph.dot");
        file << "digraph symmetry_graph {" << endl;
        for (const DotNode &node : nodes) {
            node.write(file);
        }
        for (size_t node_index = 0; node_index < neighbors.size(); ++node_index) {
            const vector<int> &node_neighbors = neighbors[node_index];
            for (int neighbor : node_neighbors) {
                file << "    node" << node_index << " -> node" << neighbor << endl;
            }
        }
        file << "}" << endl;
        file.close();
    }
};

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
    int num_vertices_so_far = vars.size();
    for (VariableProxy var : vars) {
        int var_id = var.get_id();
        group->add_to_dom_sum_by_var(num_vertices_so_far);
        num_vertices_so_far += var.get_domain_size();
        for(int num_of_value = 0; num_of_value < var.get_domain_size(); num_of_value++){
            group->add_to_var_by_val(var_id);
        }
    }

    group->set_permutation_num_variables(vars.size());
    group->set_permutation_length(num_vertices_so_far);

    DotGraph dot_graph;
    int vertex = 0;
    // add vertex for each variable
    for (size_t i = 0; i < vars.size(); ++i) {
       vertex = bliss_graph.add_vertex(VARIABLE_VERTEX);

       if (dump_symmetry_graph) {
           dot_graph.add_node(vertex, "var" + to_string(i), dot_colors[VARIABLE_VERTEX]);
       }
    }

    // now add values vertices for each predicate
    for (VariableProxy var : vars) {
        int var_id = var.get_id();
        for (int value = 0; value < var.get_domain_size(); value++){
            vertex = bliss_graph.add_vertex(VALUE_VERTEX);
            bliss_graph.add_edge(vertex, var_id);

            if (dump_symmetry_graph) {
                dot_graph.add_node(vertex, "val" + to_string(value), dot_colors[VALUE_VERTEX]);
                dot_graph.add_edge(vertex, var_id);
            }
        }
    }

    // now add vertices for operators
    for (OperatorProxy op : task_proxy.get_operators()) {
        int color = OPERATOR_VERTEX + op.get_cost();
        vertex = bliss_graph.add_vertex(color);

        if (dump_symmetry_graph) {
            dot_graph.add_node(
                vertex,
                "op" + to_string(op.get_id()),
                dot_colors[OPERATOR_VERTEX]);
        }

        add_operator_directed_graph(dump_symmetry_graph, group, bliss_graph, dot_graph, op, vertex);
    }

    // now add vertices for axioms
    for (OperatorProxy ax : task_proxy.get_axioms()) {
        int color = AXIOM_VERTEX;  //Assuming 0 cost for axioms
        vertex = bliss_graph.add_vertex(color);

        if (dump_symmetry_graph) {
            dot_graph.add_node(
                vertex,
                "ax" + to_string(ax.get_id()),
                dot_colors[AXIOM_VERTEX]);
        }

        add_operator_directed_graph(dump_symmetry_graph, group, bliss_graph, dot_graph, ax, vertex);
    }

    if (stabilize_initial_state) {
        /*
          Note: We cannot color both abstract initial and goal states, because
          there exist problme instances where a goal fact is already true in
          the initial state (e.g. psr-small:p02-s5...), hence we would recolor
          the vertex and either not stabilizing the initial state or the goal
          state, depending on the order of coloring.
        */
        vertex = bliss_graph.add_vertex(INIT_VERTEX);

        if (dump_symmetry_graph) {
            dot_graph.add_node(vertex, "init", dot_colors[INIT_VERTEX]);
        }

        for (FactProxy init_fact : task_proxy.get_initial_state()) {
            int init_vertex = group->get_index_by_var_val_pair(
                init_fact.get_variable().get_id(), init_fact.get_value());
            bliss_graph.add_edge(vertex, init_vertex);

            if (dump_symmetry_graph) {
                dot_graph.add_edge(vertex, init_vertex);
            }
        }
    }

    // Recoloring the goal values
    for (FactProxy goal_fact : task_proxy.get_goals()) {
        int goal_vertex = group->get_index_by_var_val_pair(
            goal_fact.get_variable().get_id(), goal_fact.get_value());
        bliss_graph.change_color(goal_vertex, GOAL_VERTEX);

        if (dump_symmetry_graph) {
            dot_graph.change_node_color(goal_vertex, dot_colors[GOAL_VERTEX]);
        }
    }

    if (dump_symmetry_graph) {
        dot_graph.write();
    }
}

void GraphCreator::add_operator_directed_graph(
    const bool dump_symmetry_graph,
    Group *group,
    bliss::Digraph &bliss_graph,
    DotGraph &dot_graph,
    const OperatorProxy& op,
    int op_vertex) const {
    PreconditionsProxy preconditions = op.get_preconditions();
    for (FactProxy prec_fact : preconditions) {
        int var_id = prec_fact.get_pair().var;
        int value = prec_fact.get_pair().value;
        if (value != -1) {
            int fact_vertex = group->get_index_by_var_val_pair(var_id, value);
            bliss_graph.add_edge(fact_vertex, op_vertex);

            if (dump_symmetry_graph) {
                dot_graph.add_edge(fact_vertex, op_vertex);
            }
        }
    }

    EffectsProxy effects = op.get_effects();
    for (size_t effect_id = 0; effect_id < effects.size(); ++effect_id) {
        EffectProxy effect = effects[effect_id];
        EffectConditionsProxy effect_condition = effect.get_conditions();

        int effect_var_id = effect.get_fact().get_pair().var;
        int effect_value = effect.get_fact().get_pair().value;
        int effect_vertex = group->get_index_by_var_val_pair(effect_var_id, effect_value);
        if (effect_condition.empty()) {
            bliss_graph.add_edge(op_vertex, effect_vertex);

            if (dump_symmetry_graph) {
                dot_graph.add_edge(op_vertex, effect_vertex);
            }
        } else {
            // Adding a node for each condition. An edge from op to node, an edge from node to eff,
            // for each cond, an edge from cond to node.
            color_t effect_color = CONDITIONAL_EFFECT_VERTEX;
            if (effect_can_be_overwritten(effect_id, effects)) {
                effect_color = CONDITIONAL_DELETE_EFFECT_VERTEX;
            }
            int cond_op_vertex = bliss_graph.add_vertex(effect_color);
            bliss_graph.add_edge(op_vertex, cond_op_vertex); // Edge from operator to conditional effect
            bliss_graph.add_edge(cond_op_vertex, effect_vertex); // Edge from conditional effect to effect

            if (dump_symmetry_graph) {
                dot_graph.add_node(
                    cond_op_vertex, "effect" + to_string(effect_id), dot_colors[effect_color]);
                dot_graph.add_edge(op_vertex, cond_op_vertex);
                dot_graph.add_edge(cond_op_vertex, effect_vertex);
            }

            // Adding edges for conds
            for (FactProxy prec_fact : effects[effect_id].get_conditions()) {
                int c_vertex = group->get_index_by_var_val_pair(
                    prec_fact.get_variable().get_id(), prec_fact.get_value());

                // Edge from condition to conditional effect
                bliss_graph.add_edge(c_vertex, cond_op_vertex);

                if (dump_symmetry_graph) {
                    dot_graph.add_edge(c_vertex, cond_op_vertex);
                }
            }
        }
    }
}

bool GraphCreator::effect_can_be_overwritten(int effect_id, const EffectsProxy &effects) const {
    // Checking whether the effect is a delete effect that can be overwritten by an add effect
    // Assumptions:
    //  (1) This can happen only to the none_of_those values, and these can be
    //      overwritten only by a non none_of_those value that comes after it.
    //  (2) none_of_those is the last value of a variable
    //  (3) The variables in the effects are ordered by effect variables
    //TODO: verify that the assumptions above hold
    int num_effects = effects.size();

    assert(effect_id < num_effects);
    FactProxy effect_fact = effects[effect_id].get_fact();
    VariableProxy effect_var = effect_fact.get_variable();
    int eff_val = effect_fact.get_value();
    if (eff_val != effect_fact.get_variable().get_domain_size() - 1) // the value is not none_of_those
        return false;

    // Go over the next effects of the same variable, skipping the none_of_those
    for (int i=effect_id+1; i < num_effects; i++) {
        if (effect_var != effects[i].get_fact().get_variable()) // Next variable
            return false;
        if (effects[i].get_fact().get_value() == effect_fact.get_variable().get_domain_size() - 1)
            continue;
        // Found effect on the same variable which is not none_of_those
        return true;
    }
    return false;
}
