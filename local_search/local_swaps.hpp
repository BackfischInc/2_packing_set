#ifndef LOCAL_SWAPS_HPP
#define LOCAL_SWAPS_HPP

#include "../graph/csr_graph.hpp"
#include "../packing_set/packing_set.hpp"

void maximize_solution(const csr_graph& graph, packing_set& solution_set, const bool weighted);

bool try_auto_include(const int& curr, const csr_graph& graph, packing_set& solution_set, const bool& weighted);

bool find_2_1_swap(const int& curr, const csr_graph& graph, packing_set& solution_set, const bool& weighted);

bool has_potential(const int& node, const int& curr, const csr_graph& graph, const packing_set& set);

bool are_neighbors(const csr_graph& graph, const int& v, const int& w);

bool have_common_neighbor(const std::span<const int>& a, const std::span<const int>& b);

#endif //LOCAL_SWAPS_HPP
