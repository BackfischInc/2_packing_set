#ifndef ITERATED_LS_HPP
#define ITERATED_LS_HPP

#include "../graph/csr_graph.hpp"
#include "../packing_set/packing_set.hpp"

void execute_iteration(packing_set& solution_set, const csr_graph& graph, bool weighted, int iteration_amount);

void iterated_local_search(packing_set& solution_set, const csr_graph& graph, const bool& weighted);

void perturb_solution(const csr_graph& graph, packing_set& solution_set);

void update_if_better(const csr_graph& graph, packing_set& best_solution,
                      const packing_set& working_set, const bool& weighted);

#endif //ITERATED_LS_HPP
