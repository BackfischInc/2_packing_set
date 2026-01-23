#include "iterated_ls.hpp"
#include "local_swaps.hpp"
#include "../packing_set/iteration_queue.hpp"

#include <chrono>
#include <format>
#include <iostream>

typedef std::chrono::steady_clock::time_point p_time;
typedef std::chrono::duration<long long> seconds;
typedef std::chrono::duration<long long, std::ratio<1, 1000>> milliseconds;

void iterated_local_search(packing_set& solution_set, const csr_graph& graph, const bool& weighted) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint64_t> int_dist(0, graph.amount_nodes() - 1);
  std::uniform_real_distribution real_dist(0.005, 0.1);

  iteration_queue queue(graph.amount_nodes(), 1000);
  uint64_t best_result = weighted ? solution_set.get_weight() : solution_set.get_size();
  std::unordered_set<uint64_t> set_nodes;
  const p_time begin = std::chrono::steady_clock::now();
  bool print_update = false;

  for (int i = 0; ; ++i) {
    if (i % 10000 == 0) {
      const p_time curr = std::chrono::steady_clock::now();
      const seconds diff = std::chrono::duration_cast<std::chrono::seconds>(curr - begin);
      if (diff >= std::chrono::hours(3)) {
        break;
      }
    }

    constexpr uint64_t max_node_amount = 30;

    perturb_solution(graph, solution_set, queue, max_node_amount, set_nodes, int_dist, real_dist, gen);
    maximize_solution(graph, solution_set, set_nodes, queue, weighted);

    if (is_better(solution_set, best_result, weighted)) {
      best_result = weighted ? solution_set.get_weight() : solution_set.get_size();
      solution_set.clear_changelog();
      print_update = true;
    } else {
      solution_set.unwind(graph, set_nodes);
    }

    if (print_update && i % 1000 == 0) {
      const p_time curr = std::chrono::steady_clock::now();
      const milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds>(curr - begin);

      std::cout
          << std::format("\rIteration: {:6},  Solution Size: {:8}, ", i, best_result)
          << std::format("Found in {:9} seconds", static_cast<double>(diff.count()) / 1000.)
          << std::flush;

      print_update = false;
    }
  }
}

void perturb_solution(const csr_graph& graph, packing_set& solution_set,
                      iteration_queue& queue, const uint64_t& max_node_amount,
                      std::unordered_set<uint64_t>& set_nodes,
                      std::uniform_int_distribution<uint64_t>& int_dist,
                      std::uniform_real_distribution<>& real_dist,
                      std::mt19937& gen) {
  uint64_t curr = int_dist(gen);
  const double iter_amount = real_dist(gen);
  const double max_iterations = static_cast<double>(graph.amount_nodes()) / iter_amount;

  for (int i = 0; i < max_iterations && queue.size() < max_node_amount; ++i) {
    if (solution_set.get_value(curr)) {
      solution_set.remove_solution_node(curr, graph);

      std::span<const uint64_t> neighbors = graph.get_neighbors(curr);
      for (const uint64_t& neighbor: neighbors) {
        queue.add_node(neighbor);
      }

      for (const uint64_t& node: neighbors) {
        for (const uint64_t& neighbor: graph.get_neighbors(node)) {
          queue.add_node(neighbor);
        }
      }

    } else {
      solution_set.get_set_partners(set_nodes, curr, graph);
      solution_set.remove_solution_nodes(set_nodes, graph);
      solution_set.add_solution_node(curr, graph);

      for (const uint64_t& node: set_nodes) {
        std::span<const uint64_t> neighbors = graph.get_neighbors(node);
        for (const uint64_t& neighbor : neighbors) {
          queue.add_node(neighbor);

          for (const uint64_t& neighbor_2 : graph.get_neighbors(neighbor)) {
            queue.add_node(neighbor_2);
          }
        }
      }

      for (const uint64_t& neighbor: graph.get_neighbors(curr)) {
        queue.add_node(neighbor);
      }
    }

    for (int j = 0; j < 3; ++j) {
      const auto& neighbors = graph.get_neighbors(curr);
      curr = neighbors[int_dist(gen) % neighbors.size()];
    }
  }
}

bool is_better(const packing_set& solution_set, const uint64_t& prev, const bool& weighted) {
  if (weighted) {
    return solution_set.get_weight() > prev;
  }
  return solution_set.get_size() > prev;
}
