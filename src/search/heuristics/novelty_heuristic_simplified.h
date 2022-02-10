#ifndef HEURISTICS_NOVELTY_HEURISTIC_SIMPLIFIED_H
#define HEURISTICS_NOVELTY_HEURISTIC_SIMPLIFIED_H

#include "../heuristic.h"
#include "../search_statistics.h"
#include "../utils/logging.h"

namespace novelty_heuristic {
class NoveltyHeuristic : public Heuristic {
	std::shared_ptr<Evaluator> novelty_heuristic;
    bool solution_found_by_heuristic;
    std::vector<std::vector<int> > novelty_per_variable_value;
    utils::LogProxy log;
    SearchStatistics statistics;

protected:
    virtual int compute_heuristic(const State &ancestor_state) override;
public:
    NoveltyHeuristic(const options::Options &options);

    virtual bool found_solution() const override { return novelty_heuristic->found_solution(); } 
    virtual const std::vector<OperatorID>& get_solution() const override;

};
}
#endif
