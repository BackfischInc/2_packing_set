#ifndef ITERATED_LS_HPP
#define ITERATED_LS_HPP

#include <random>

#include "../graph/csr_graph.hpp"
#include "../packing_set/packing_set.hpp"

class iteration_queue;

void iterated_local_search(packing_set& solution_set, const csr_graph& graph,
                           const bool& weighted);

void perturb_solution(const csr_graph& graph, packing_set& solution_set,
                      iteration_queue& queue, const uint64_t& max_node_amount,
                      std::unordered_set<uint64_t>& set_nodes,
                      std::uniform_int_distribution<uint64_t>& int_dist,
                      std::uniform_real_distribution<>& real_dist,
                      std::mt19937& gen);

bool is_better(const packing_set& solution_set,
               const uint64_t& prev, const bool& weighted);

#endif //ITERATED_LS_HPP
