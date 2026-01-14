#include "iterated_ls.hpp"
#include "local_swaps.hpp"

#include <iostream>

void iterated_local_search(packing_set& solution_set, const csr_graph& graph, const bool& weighted) {
  constexpr uint64_t iter_cutoff = 100000;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint64_t> int_dist(0, graph.amount_nodes() - 1);
  std::uniform_real_distribution real_dist(0.005, 0.1);

  uint64_t best_result = weighted ? solution_set.get_weight(graph) : solution_set.get_size();

  std::set<uint64_t> curr_nodes;
  std::set<uint64_t> next_nodes;

  uint64_t last_improvement = 0;

  for (int i = 1; ; ++i) {
    const uint64_t max_node_amount = int_dist(gen) % 60 + 15;

    /*
    if (i % iter_cutoff == 0 && i - last_improvement > iter_cutoff) {
      break;
    }
    */

    perturb_solution(graph, solution_set, curr_nodes, max_node_amount, int_dist, real_dist, gen);
    maximize_solution(graph, solution_set, curr_nodes, next_nodes, max_node_amount, weighted);

    if (is_better(graph, solution_set, best_result, weighted)) {
      best_result = weighted ? solution_set.get_weight(graph) : solution_set.get_size();
      solution_set.clear_changelog();
      last_improvement = i;
      printf("[%6d] %7llu\n", i, best_result);
    } else {
      solution_set.unwind(graph);
    }
  }
}

void perturb_solution(const csr_graph& graph, packing_set& solution_set,
                      std::set<uint64_t>& next_nodes, const uint64_t& max_node_amount,
                      std::uniform_int_distribution<uint64_t>& int_dist,
                      std::uniform_real_distribution<>& real_dist,
                      std::mt19937& gen) {

  uint64_t curr = int_dist(gen);
  const double iter_amount = real_dist(gen);
  const double max_iterations = static_cast<double>(graph.amount_nodes()) / iter_amount;

  for (int i = 0; i < max_iterations && next_nodes.size() < max_node_amount; ++i) {

    if (solution_set.get_value(curr)) {
      solution_set.remove_solution_node(curr, graph);

      std::span<const uint64_t> neighbors = graph.get_neighbors(curr);
      next_nodes.insert(neighbors.begin(), neighbors.end());

      for (const uint64_t& node : next_nodes) {
        neighbors = graph.get_neighbors(node);
        next_nodes.insert(neighbors.begin(), neighbors.end());
      }
    } else {
      std::set<uint64_t> set_nodes = solution_set.get_set_partners(curr, graph.get_neighbors(curr));
      solution_set.remove_solution_nodes(set_nodes, graph);
      solution_set.add_solution_node(curr, graph);

      for (const uint64_t& node : set_nodes) {
        std::span<const uint64_t> neighbors = graph.get_neighbors(node);
        next_nodes.insert(neighbors.begin(), neighbors.end());
      }

      for (const uint64_t& node : next_nodes) {
        std::span<const uint64_t> neighbors = graph.get_neighbors(node);
        next_nodes.insert(neighbors.begin(), neighbors.end());
      }

      std::span<const uint64_t> neighbors = graph.get_neighbors(curr);
      next_nodes.insert(neighbors.begin(), neighbors.end());
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
