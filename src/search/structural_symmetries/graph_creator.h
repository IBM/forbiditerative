#ifndef SYMMETRIES_GRAPH_CREATOR_H
#define SYMMETRIES_GRAPH_CREATOR_H

#include <vector>

namespace bliss {
    class Digraph;
}
class GlobalEffect;
class GlobalOperator;
class Group;

namespace options {
class Options;
}

/**
 * This class will create a bliss graph which will be used to find the
 * automorphism groups
 */

class GraphCreator  {
    int time_bound;
    //int generators_bound;
    bool stabilize_initial_state;

    void create_bliss_directed_graph(bliss::Digraph &bliss_graph) const;
    void add_operator_directed_graph(bliss::Digraph &bliss_graph,
                                     const GlobalOperator& op, int op_idx) const;
    bool effect_can_be_overwritten(int ind, const std::vector<GlobalEffect>& effects) const;
public:
    explicit GraphCreator(const options::Options &opts);
    ~GraphCreator();
    bool compute_symmetries(Group *group);
};

#endif
