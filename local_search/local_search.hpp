#ifndef LOCAL_SEARCH_HPP
#define LOCAL_SEARCH_HPP
#include "../packing_set/packing_set.hpp"
#include "../graph/csr_graph.hpp"
#include "../temperature/simulated_annealing.hpp"

void local_search(const csr_graph& graph, packing_set& solution_set, const bool& weighted,
                  const unsigned long long& iterations, simulated_annealing& temp);

void print_update(const int& best_result, const int& diff, const double& current_temp, const double& completion);

#endif //LOCAL_SEARCH_HPP
