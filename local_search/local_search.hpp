#ifndef LOCAL_SEARCH_HPP
#define LOCAL_SEARCH_HPP
#include "../packing_set/packing_set.hpp"
#include "../graph/csr_graph.hpp"
#include "../temperature/simulated_annealing.hpp"

void local_search(const csr_graph& graph, packing_set& solution_set, const bool& weighted,
                  const unsigned long long& iterations, simulated_annealing& temp);

bool search_node(const int& curr, const csr_graph& graph, packing_set& solution_set, simulated_annealing& temp,
                 int& current_result, const unsigned long long int& iteration,
                 const double& random_value, const bool& weighted);

void update_current_result(int& current_result, const int& size, const int& set_weight, const int& node_weight,
                           const bool& weighted);

int get_diff(const size_t& size, const int& set_weight, const int& node_weight, const bool& weighted);

bool direct_swap(const size_t& size, const int& set_weight, const int& node_weight, const bool& weighted);

void print_update(const int& best_result, const int& diff, const double& current_temp, const double& completion);

#endif //LOCAL_SEARCH_HPP
