#ifndef LOCAL_SEARCH_HPP
#define LOCAL_SEARCH_HPP
#include "../packing_set/packing_set.hpp"
#include "../graph/csr_graph.hpp"
#include "../temperature/simulated_annealing.hpp"

void local_search(const csr_graph& graph, packing_set& solution_set, const bool& weighted,
                  const unsigned long long& iterations, simulated_annealing& temp);

int get_set_partners(std::vector<int>& result, const packing_set& solution_set, const int& curr,
                     const std::span<const int>& nodes, const csr_graph& graph, const bool& weighted);

int get_weight(const packing_set& set, const csr_graph& graph);

int get_weight(const std::vector<int>& set, const csr_graph& graph);

#endif //LOCAL_SEARCH_HPP
