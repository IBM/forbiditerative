#ifndef SEARCH_ENGINES_SHORTEST_EAGER_SEARCH_H
#define SEARCH_ENGINES_SHORTEST_EAGER_SEARCH_H

#include "../open_list.h"
#include "../search_engine.h"

#include <memory>
#include <vector>

class Evaluator;
class Group;
class PruningMethod;

namespace options {
class OptionParser;
class Options;
}

/*
The class is currently copied from EagerSearch, but should be made to derive from EagerSearch.
*/
namespace eager_search {
class ShortestEagerSearch : public SearchEngine {
    const bool reopen_closed_nodes;
    std::shared_ptr<Group> group;

    std::unique_ptr<StateOpenList> open_list;
    std::shared_ptr<Evaluator> f_evaluator;
    /*
      Note: orbit space search and duplicate pruning with dks does not work
      with preferred operators and multi path search.
    */
    bool use_oss() const;
    bool use_dks() const;

    std::vector<Evaluator *> path_dependent_evaluators;
    std::vector<std::shared_ptr<Evaluator>> preferred_operator_evaluators;
    std::shared_ptr<Evaluator> lazy_evaluator;

    std::shared_ptr<PruningMethod> pruning_method;

    void start_f_value_statistics(EvaluationContext &eval_context);
    void update_f_value_statistics(EvaluationContext &eval_context);
    void reward_progress();

protected:
    virtual void initialize() override;
    virtual SearchStatus step() override;

public:
    explicit ShortestEagerSearch(const options::Options &opts);
    virtual ~ShortestEagerSearch() = default;

    virtual void print_statistics() const override;

    void dump_search_space() const;
};
}

#endif
