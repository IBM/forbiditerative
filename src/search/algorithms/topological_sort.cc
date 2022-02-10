#include "topological_sort.h"
#include <algorithm>
#include <vector>
#include <set>
using namespace std;

namespace topological_sort {

bool TopologicalSort::get_result(vector<int>& res) {
	// returns true iff the graph is a DAG
	// Algorithm by  Kahn (1962)

	set<int> queue;

	size_t node_count = graph.size();
	// Total number of incoming edges
	vector<int> incoming(node_count, 0);
	// Current number of removed incoming edges
	vector<int> curr_incoming(node_count, 0);

    for (size_t i = 0; i < node_count; ++i) {
    	const vector<int>& succ = graph[i];
        for (size_t j = 0; j < succ.size(); ++j){
        	incoming[succ[j]]++;
        }
    }
    for (size_t i = 0; i < node_count; ++i) {
    	if (incoming[i] == 0)
    		queue.insert(i);
    }
    while (!queue.empty()) {
    	set<int>::iterator it = queue.begin();
    	int node = *it;
    	queue.erase(it);
    	res.push_back(node);
    	const vector<int>& succ = graph[node];
        for (size_t j = 0; j < succ.size(); ++j){
        	int to_node = succ[j];
        	curr_incoming[to_node]++;
        	if (incoming[to_node] == curr_incoming[to_node])
        		queue.insert(to_node);
        }
    }

    for (size_t i = 0; i < node_count; ++i) {
    	if (incoming[i] != curr_incoming[i])
    		return false;
    }

    return true;
}

}