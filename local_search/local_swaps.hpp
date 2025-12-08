#ifndef LOCAL_SWAPS_HPP
#define LOCAL_SWAPS_HPP
#include <map>

#include "../graph/csr_graph.hpp"
#include "../packing_set/packing_set.hpp"

void execute_2_1_swap(const csr_graph& graph, packing_set& solution_set, const bool& weighted);

void fill_buckets(std::map<int, std::vector<int>>& buckets, const csr_graph& graph, const packing_set& solution_set);

bool execute_swap(std::map<int, std::vector<int>>& buckets, const csr_graph& graph,
                  packing_set& solution_set, const bool& weighted);

bool are_neighbors(const csr_graph& graph, const int& v, const int& w);

bool have_common_neighbor(const std::span<const int>& a, const std::span<const int>& b);

#endif //LOCAL_SWAPS_HPP
