#ifndef SEARCH_SPACE_H
#define SEARCH_SPACE_H

#include "operator_cost.h"
#include "per_state_information.h"
#include "search_node_info.h"

#include <vector>

class Group;
class OperatorProxy;
class State;
class TaskProxy;

namespace utils {
class LogProxy;
}

class SearchNode {
    State state;
    SearchNodeInfo &info;
public:
    SearchNode(const State &state, SearchNodeInfo &info);

    const State &get_state() const;

    bool is_new() const;
    bool is_open() const;
    bool is_closed() const;
    bool is_dead_end() const;

    int get_g() const;
    int get_d() const;
    int get_real_g() const;

    void open_initial();
    void open(const SearchNode &parent_node,
              const OperatorProxy &parent_op,
              int adjusted_cost);
    void reopen(const SearchNode &parent_node,
                const OperatorProxy &parent_op,
                int adjusted_cost);
    void update_parent(const SearchNode &parent_node,
                       const OperatorProxy &parent_op,
                       int adjusted_cost);
    void close();
    void mark_as_dead_end();

    void dump(const TaskProxy &task_proxy, utils::LogProxy &log) const;
};


class SearchSpace {
    PerStateInformation<SearchNodeInfo> search_node_infos;

    StateRegistry &state_registry;

    void trace_path_with_symmetries(const State &goal_state,
                                    std::vector<OperatorID> &path,
                                    const std::shared_ptr<AbstractTask> &task,
                                    const std::shared_ptr<Group> &group) const;
    void dump_state(std::ostream& os, const State& state) const;
    bool dump_causal_link(const std::vector<OperatorID> &plan, 
                          const std::vector<std::vector<EffectProxy>>& plan_firing_effects,
                          size_t to_index, FactProxy cond, std::ostream& os, bool coma, const TaskProxy &task_proxy) const;

    utils::LogProxy &log;
public:
    SearchSpace(StateRegistry &state_registry, utils::LogProxy &log);

    SearchNode get_node(const State &state);
    void trace_path(const State &goal_state,
                    std::vector<OperatorID> &path,
                    const std::shared_ptr<AbstractTask> &task,
                    const std::shared_ptr<Group> &group = nullptr) const;

    void dump(const TaskProxy &task_proxy) const;
    void print_statistics() const;
    void dump_trace(const std::vector<StateID> &plan_trace, std::ostream& os) const;
    void trace_from_plan(const std::vector<OperatorID> &plan, std::vector<StateID> &plan_trace, const TaskProxy &task_proxy) const;
    void dump_partial_order_from_plan(const std::vector<OperatorID> &plan, std::ostream& os, const TaskProxy &task_proxy) const;

};

#endif
