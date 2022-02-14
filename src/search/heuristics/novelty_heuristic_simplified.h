#ifndef HEURISTICS_NOVELTY_HEURISTIC_SIMPLIFIED_H
#define HEURISTICS_NOVELTY_HEURISTIC_SIMPLIFIED_H

#include "../heuristic.h"
#include "../search_statistics.h"
#include "../utils/logging.h"

#include <vector>
namespace novelty_heuristic_simplified {
class NoveltyHeuristicSimplified : public Heuristic {
	std::shared_ptr<Evaluator> novelty_heuristic;
    bool solution_found_by_heuristic;
 	bool dump_value;
    std::vector<std::vector<int> > novelty_per_variable_value;
    utils::LogProxy log;
    SearchStatistics statistics;

protected:
    virtual int compute_heuristic(const State &ancestor_state) override;
public:
    NoveltyHeuristicSimplified(const options::Options &options);
    virtual ~NoveltyHeuristicSimplified() override;

    virtual bool found_solution() const override { return novelty_heuristic->found_solution(); } 
    virtual const std::vector<OperatorID>& get_solution() const override;

};
}
#endif
