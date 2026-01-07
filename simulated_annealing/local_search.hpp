#ifndef LOCAL_SEARCH_HPP
#define LOCAL_SEARCH_HPP
#include "../packing_set/packing_set.hpp"
#include "../graph/csr_graph.hpp"
#include "temperature.hpp"

void local_search(const csr_graph& graph, packing_set& solution_set, const bool& weighted,
                  const unsigned long long& iterations, temperature& temp);

bool search_node(const uint64_t& curr, const csr_graph& graph, packing_set& solution_set, temperature& temp,
                 uint64_t& current_result, const uint64_t& iteration,
                 const double& random_value, const bool& weighted);

void update_current_result(uint64_t& current_result, const uint64_t& size, const uint64_t& set_weight,
                           const uint64_t& node_weight, const bool& weighted);

int get_diff(const size_t& size, const uint64_t& set_weight, const uint64_t& node_weight, const bool& weighted);

bool direct_swap(const size_t& size, const uint64_t& set_weight, const uint64_t& node_weight, const bool& weighted);

void print_update(const uint64_t& best_result, const uint64_t& diff, const double& current_temp,
                  const double& completion);

#endif //LOCAL_SEARCH_HPP
