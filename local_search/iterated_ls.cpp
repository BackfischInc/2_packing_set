#include "iterated_ls.hpp"
#include "local_swaps.hpp"
#include "../packing_set/iteration_queue.hpp"

#include <chrono>
#include <format>
#include <iostream>
#include <algorithm>

typedef std::chrono::steady_clock::time_point p_time;
typedef std::chrono::duration<long long, std::ratio<1, 1000>> milliseconds;

void iterated_local_search(packing_set& solution_set, const csr_graph& graph, const parameters& args) {
  const bool weighted = args.weighted;
  const bool do_2_1_swaps = !args.disable_2_1;
  const uint64_t max_node_amount = args.queue_size;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint64_t> int_dist(0, graph.amount_nodes() - 1);
  std::uniform_real_distribution real_dist(0.005, 0.1);

  iteration_queue queue(graph.amount_nodes(), args.queue_size * 100);
  std::vector<uint64_t> set_nodes;
  set_nodes.reserve(16);
  uint64_t best_result = weighted ? solution_set.get_weight() : solution_set.get_size();

  bool print_update = false;

  const p_time begin = std::chrono::steady_clock::now();
  for (uint64_t i = 0; ; ++i) {
    if ((i & 1023) == 0) {
      const p_time curr = std::chrono::steady_clock::now();
      const auto diff = std::chrono::duration_cast<std::chrono::seconds>(curr - begin);

      if (diff >= std::chrono::seconds(args.time_limit * 60LL)) {
        return;
      }
    }

    solution_set.begin_transaction();
    perturb_solution(graph, solution_set, queue, max_node_amount, set_nodes, int_dist, real_dist, gen);
    maximize_solution(graph, solution_set, set_nodes, queue, weighted, do_2_1_swaps);

    if (weighted ? solution_set.get_weight() > best_result : solution_set.get_size() > best_result) {     // O(1)
      best_result = weighted ? solution_set.get_weight() : solution_set.get_size();                       // O(1)
      solution_set.clear_changelog();                                                                     // O(n)
      print_update = true;                                                                                // O(1)
    } else {
      solution_set.unwind(graph, set_nodes);                                                           // O(n * deg²)
    }

    if (args.verbose) {
      if (print_update) {
        const p_time curr = std::chrono::steady_clock::now();
        const milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds>(curr - begin);

        std::cout
            << std::format("Solution Size: {:10}, ", best_result)
            << std::format("Found in {:9} seconds", static_cast<double>(diff.count()) / 1000.)
            << std::endl;

        print_update = false;
      }
    }
  }
}

// O(n * deg³)
void perturb_solution(const csr_graph& graph, packing_set& solution_set,
                      iteration_queue& queue, const uint64_t& max_node_amount,
                      std::vector<uint64_t>& set_nodes,
                      std::uniform_int_distribution<uint64_t>& int_dist,
                      std::uniform_real_distribution<>& real_dist,
                      std::mt19937& gen) {
  uint64_t curr = int_dist(gen);                                                        // O(1)
  const double iter_amount = real_dist(gen);                                            // O(1)
  // iter_amount is the fraction of the graph used for the perturbation.
  // Dividing by it performed 10n--200n mutations per perturbation.
  const uint64_t max_iterations = std::max<uint64_t>(
      1, static_cast<uint64_t>(static_cast<double>(graph.amount_nodes()) * iter_amount));

  for (uint64_t i = 0; i < max_iterations && queue.size() < max_node_amount; ++i) {
    if (solution_set.get_value(curr)) {                                                    // O(1)
      solution_set.remove_solution_node(curr, graph);                                      // O(deg)
      if (!add_neighborhood(curr, queue, graph)) {
        return;
      }
    } else {
      solution_set.get_set_partners(set_nodes, curr, graph);                            // O(deg)
      solution_set.remove_solution_nodes(set_nodes, graph);                                // O(deg³)
      solution_set.add_solution_node(curr, graph);                                         // O(deg²)

      for (const uint64_t& node: set_nodes) {                                              // O(deg²)
        if (!add_neighborhood(node, queue, graph)) {
          return;
        }
      }

      if (!add_neighborhood(curr, queue, graph)) {
        return;
      }
    }

    for (int j = 0; j < 3; ++j) {                                                          // O(1)
      const auto& neighbors = graph.get_neighbors(curr);     // O(1)
      if (neighbors.empty()) {
        break;
      }
      curr = neighbors[int_dist(gen) % neighbors.size()];                               // O(1)
    }
  }
}
