#ifndef HEURISTICS_CE_LM_CUT_HEURISTIC_H
#define HEURISTICS_CE_LM_CUT_HEURISTIC_H

#include "../heuristic.h"

#include <memory>

class GlobalState;

namespace options {
class Options;
}

namespace ce_lm_cut_heuristic {
class CELandmarkCutLandmarks;

class CELandmarkCutHeuristic : public Heuristic {
    std::unique_ptr<CELandmarkCutLandmarks> landmark_generator;

    virtual int compute_heuristic(const GlobalState &global_state) override;
    int compute_heuristic(const State &state);
public:
    explicit CELandmarkCutHeuristic(const options::Options &opts);
    virtual ~CELandmarkCutHeuristic() override;
};
}

#endif
