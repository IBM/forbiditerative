#ifndef HEURISTICS_NOVELTY_HEURISTIC_H
#define HEURISTICS_NOVELTY_HEURISTIC_H

#include "../heuristic.h"
#include "../search_statistics.h"
#include "../utils/logging.h"

#include <vector>
namespace novelty_heuristic {

enum NoveltyType {
    BASIC,
    SEPARATE_NOVEL,
    SEPARATE_BOTH,
    SEPARATE_BOTH_AGGREGATE
};

enum CutoffType {
    ARGMAX,
    ALL_ORDERED,
    ALL_RANDOM,
    NO_CUTOFF
};

class NoveltyHeuristic : public Heuristic {
	std::shared_ptr<Evaluator> novelty_heuristic;
    bool solution_found_by_heuristic;
	int novelty_heuristics_largest_value;
	NoveltyType type;
	int multiplier;

	bool dump_value;
    std::vector<std::vector<int>> novelty_per_variable_value;
    
    bool use_preferred_operators;
    CutoffType cutoff_type;
    int cutoff_bound;
    int num_ops_bound;
    bool preferred_operators_from_evals;
    OperatorID reached_by_op_id;
    std::vector<int> novelty_per_operator;

    utils::LogProxy log;
    SearchStatistics statistics;

    int get_value_for_fact(const FactProxy& fact) const;
    void update_value_for_fact(const FactProxy& fact, int value);
    int get_estimate_novel(int curr_value, int heur_value, utils::LogProxy &log) const ;
    int get_estimate_non_novel(int curr_value, int heur_value, utils::LogProxy &log) const ;
    int compute_aggregated_score(std::vector<int>& values) const;
    void update_maximal_value(int value);

    // For operators
    int get_value_for_operator(OperatorID op_id) const;
    void update_value_for_operator(OperatorID op_id, int value);
    bool store_values_for_operators() const;

    void compute_preferred_operators(int heuristic_value, EvaluationContext& eval_context);
    void compute_candidate_operators_argmax(std::vector<OperatorID>& candidates, const std::vector<OperatorID> &pref_ops) const;
    void compute_candidate_operators_random(int heuristic_value, std::vector<OperatorID>& candidates, const std::vector<OperatorID> &pref_ops) const;
    void compute_candidate_operators_ordered(int heuristic_value, std::vector<OperatorID>& candidates, const std::vector<OperatorID> &pref_ops) const;

protected:
    virtual int compute_heuristic(const State &ancestor_state) override;
public:
    NoveltyHeuristic(const options::Options &options);
    virtual ~NoveltyHeuristic() override;

    virtual void get_path_dependent_evaluators(
        std::set<Evaluator *> &evals) override {
        evals.insert(this);
    }

    virtual void notify_state_transition(const State &parent_state,
                                         OperatorID op_id,
                                         const State &state) override;

    virtual bool found_solution() const override { return novelty_heuristic->found_solution(); } 
    virtual const std::vector<OperatorID>& get_solution() const override;
};
}
#endif
