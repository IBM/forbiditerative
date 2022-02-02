#ifndef ALGORITHMS_TRANSITIVE_CLOSURE_H
#define ALGORITHMS_TRANSITIVE_CLOSURE_H

#include <vector>
#include <iostream>

/*
 * The algorithm implements Floyd-Warshall
*/

namespace transitive_closure {

class TransitiveClosure {
    const std::vector<std::vector<int> > &graph;
    std::vector<std::vector<int> > solution;

public:
    TransitiveClosure(const std::vector<std::vector<int> > &graph);
    bool is_connected(int from, int to) const;
};
}
#endif
