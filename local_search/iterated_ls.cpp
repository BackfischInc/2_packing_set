#include "iterated_ls.hpp"
#include "local_swaps.hpp"

#include <iostream>
#include <random>

void iterated_local_search(packing_set& solution_set, const csr_graph& graph, const bool& weighted) {
  constexpr int iteration_amount = 200;

  uint64_t best_result = weighted ? solution_set.get_weight(graph) : solution_set.get_size();

  for (int i = 1; i <= iteration_amount; ++i) {
    if (i % 10 == 0) { std::cout << i << std::endl; }

    perturb_solution(graph, solution_set);
    maximize_solution(graph, solution_set, weighted);

    if (is_better(graph, solution_set, best_result, weighted)) {
      best_result = weighted ? solution_set.get_weight(graph) : solution_set.get_size();
      solution_set.clear_changelog();

      std::cout << "[" << i << "] found new best: " << best_result << std::endl;
    } else {
      solution_set.unwind(graph);
    }
  }
}

void perturb_solution(const csr_graph& graph, packing_set& solution_set) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint64_t> int_dist(0, graph.amount_nodes() - 1);
  std::uniform_real_distribution real_dist(0.005, 0.1);

  uint64_t curr = int_dist(gen);

  const int amount_iterations = static_cast<int>(static_cast<double>(graph.amount_nodes()) * real_dist(gen));
  for (int i = 0; i < amount_iterations; ++i) {
    if (solution_set.get_value(curr)) {
      solution_set.remove_solution_node(curr, graph);
    } else {
      std::set<uint64_t> set_nodes = solution_set.get_set_partners(curr, graph.get_neighbors(curr));

      solution_set.remove_solution_nodes(set_nodes, graph);
      solution_set.add_solution_node(curr, graph);
    }

    for (int j = 0; j < 3; ++j) {
      const auto& neighbors = graph.get_neighbors(curr);
      curr = neighbors[int_dist(gen) % neighbors.size()];
    }
  }
}

bool is_better(const csr_graph& graph, const packing_set& solution_set, const uint64_t& prev, const bool& weighted) {
  if (weighted) {
    return solution_set.get_weight(graph) > prev;
  }
  return solution_set.get_size() > prev;
}
