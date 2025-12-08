#include "iterated_ls.hpp"
#include "local_search.hpp"
#include "../verifier/verifier.hpp"

#include <iostream>
#include <random>

void iterated_local_search(packing_set& solution_set, std::vector<int>& set_neighbors,
  const csr_graph& graph, const bool& weighted) {
  packing_set best_solution(graph.amount_nodes());
  std::vector best_set_neighbors = set_neighbors;

  for (int i = 0; i < 10; ++i) {
    printf("\titeration: %2d\n", i+1);
    packing_set working_set = best_solution;
    std::vector<int> working_set_neighbors = best_set_neighbors;
    unsigned long long iterations = graph.amount_nodes() * static_cast<unsigned long long>(0xFF);

    if (i != 0) {
      iterations = graph.amount_nodes() * static_cast<unsigned long long>(0xF);
      perturb_solution(graph, working_set, working_set_neighbors);

      if (working_set.get_size() > best_solution.get_size()) {
        std::cout << "\t===== found better solution while perturbing lol :D =====" << std::endl;
        best_solution = working_set;
        best_set_neighbors = working_set_neighbors;
      }
    }

    simulated_annealing temperature(iterations,
                                    i == 0 ? 1. : 0.2,
                                    i == 0 ? 0.9 : 0.5);
    local_search(graph, working_set, working_set_neighbors, weighted, iterations, temperature);

    if (working_set.get_size() > best_solution.get_size()) {
      std::cout << "\t===== found better solution c: =====\n" << std::endl;
      best_solution = working_set;
      best_set_neighbors = working_set_neighbors;
    } else {
      std::cout << "\t===== didn't find better solution :c =====\n" << std::endl;
    }

    if (!is_valid(graph, solution_set)) {
      std::cout << "FUCK" << std::endl;
    }
  }

  solution_set = best_solution;
  set_neighbors = best_set_neighbors;
}

void perturb_solution(const csr_graph& graph, packing_set& solution_set, std::vector<int>& set_neighbors) {
  const int amount_iterations = static_cast<int>(graph.amount_nodes() * 0.05);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution int_dist(0, graph.amount_nodes() - 1);

  int curr = int_dist(gen);

  std::vector<int> set_nodes;
  set_nodes.reserve(graph.max_deg);

  for (int i = 0; i < amount_iterations; ++i) {
    if (solution_set.get_value(curr)) {
      solution_set.remove_solution_node(curr, graph, set_neighbors);
    } else {
      get_set_partners(set_nodes, curr, graph.get_neighbors(curr),
                       set_neighbors, graph, false);

      solution_set.remove_solution_nodes(set_nodes, graph, set_neighbors);
      solution_set.add_solution_node(curr, graph.get_neighbors(curr), set_neighbors);
    }
    set_nodes.clear();

    for (int j = 0; j < 3; ++j) {
      const auto& neighbors = graph.get_neighbors(curr);
      curr = neighbors[int_dist(gen) % neighbors.size()];
    }
  }
}
