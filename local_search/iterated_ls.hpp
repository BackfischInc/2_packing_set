#ifndef ITERATED_LS_HPP
#define ITERATED_LS_HPP

#include "../graph/csr_graph.hpp"
#include "../packing_set/packing_set.hpp"

void iterated_local_search(packing_set& solution_set, const csr_graph& graph, const bool& weighted);

void perturb_solution(const csr_graph& graph, packing_set& solution_set);

bool is_better(const csr_graph& graph, const packing_set& solution_set, const uint64_t& prev, const bool& weighted);

#endif //ITERATED_LS_HPP
