#ifndef LOCAL_SWAPS_HPP
#define LOCAL_SWAPS_HPP

#include "../graph/csr_graph.hpp"
#include "../packing_set/packing_set.hpp"

void maximize_solution(const csr_graph& graph, packing_set& solution_set,
                       std::set<uint64_t>& curr_nodes, std::set<uint64_t>& next_nodes,
                        const uint64_t& max_node_amount, const bool& weighted);

bool try_auto_include(const uint64_t& curr, const csr_graph& graph, packing_set& solution_set,
                      std::set<uint64_t>& next_nodes, const bool& weighted);

bool find_2_1_swap(const uint64_t& curr, const csr_graph& graph, packing_set& solution_set,
                   std::set<uint64_t>& next_nodes, const bool& weighted);

bool try_swap(const uint64_t& node, const uint64_t& curr, const std::set<uint64_t>& pot_neighbors, const csr_graph& graph,
  packing_set& solution_set, std::set<uint64_t>& next_nodes, const bool& weighted);

bool has_potential(const uint64_t& node, const uint64_t& curr, const csr_graph& graph, const packing_set& set);

bool are_neighbors(const csr_graph& graph, const uint64_t& v, const uint64_t& w);

bool have_common_neighbor(const std::span<const uint64_t>& a, const std::span<const uint64_t>& b);

#endif //LOCAL_SWAPS_HPP
