#ifndef ITERATED_LS_HPP
#define ITERATED_LS_HPP

#include <vector>
#include "../graph/csr_graph.hpp"
#include "../packing_set/packing_set.hpp"

void iterated_local_search(packing_set& solution_set, std::vector<int>& set_neighbors,
                           const csr_graph& graph, const bool& weighted);

void perturb_solution(const csr_graph& graph, packing_set& solution_set, std::vector<int>& set_neighbors);

#endif //ITERATED_LS_HPP
