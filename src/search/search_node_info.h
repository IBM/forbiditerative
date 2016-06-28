#ifndef SEARCH_NODE_INFO_H
#define SEARCH_NODE_INFO_H

#include "global_operator.h"
#include "state_id.h"

// For documentation on classes relevant to storing and working with registered
// states see the file state_registry.h.

struct SearchNodeInfo {
    enum NodeStatus {NEW = 0, OPEN = 1, CLOSED = 2, DEAD_END = 3};

    unsigned int status : 2;
    int g : 30;
    StateID parent_state_id;
    const GlobalOperator *creating_operator;
    int real_g;

    SearchNodeInfo()
        : status(NEW), g(-1), parent_state_id(StateID::no_state),
          creating_operator(0), real_g(-1) {
    }
};

static_assert(sizeof(SearchNodeInfo) == 
    sizeof(int) + sizeof(StateID) + sizeof(GlobalOperator *) + sizeof(int),
    "The size of SearchNodeInfo is larger than expected. This probably means "
    "that packing two field into one integer using bitfields is not supported.");

#endif
