#ifndef STRUCTURAL_SYMMETRIES_GRAPH_CREATOR_H
#define STRUCTURAL_SYMMETRIES_GRAPH_CREATOR_H

#include <vector>

namespace bliss {
    class Digraph;
}
class GlobalEffect;
class GlobalOperator;
class Group;

/**
 * This class will create a bliss graph which will be used to find the
 * automorphism groups
 */

class GraphCreator  {
    void create_bliss_directed_graph(
        bool stabilize_initial_state, Group *group, bliss::Digraph &bliss_graph) const;
    void add_operator_directed_graph(
        Group *group,
        bliss::Digraph &bliss_graph,
        const GlobalOperator& op,
        int op_idx) const;
    bool effect_can_be_overwritten(int ind, const std::vector<GlobalEffect>& effects) const;
public:
    GraphCreator() = default;
    virtual ~GraphCreator() = default;
    bool compute_symmetries(bool stabilize_initial_state, int time_bound, Group *group);
};

#endif
