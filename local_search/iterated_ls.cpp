#include "iterated_ls.hpp"
#include "local_search.hpp"

#include <iostream>
#include <random>

void iterated_local_search(packing_set& solution_set, const csr_graph& graph, const bool& weighted) {
  packing_set best_solution(graph.amount_nodes());

  for (int i = 0; i < 5; ++i) {
    printf("\titeration: %2d\n", i + 1);
    packing_set working_set(best_solution);

    unsigned long long iterations = graph.amount_nodes() * static_cast<unsigned long long>(0x5F);

    if (i != 0) {
      iterations = graph.amount_nodes() * static_cast<unsigned long long>(0xF);
      perturb_solution(graph, working_set);

      if (!weighted) {
        if (working_set.get_size() > best_solution.get_size()) {
          std::cout << "\t===== found better solution while perturbing lol :D =====" << std::endl;
          best_solution.~packing_set();
          new(&best_solution) packing_set(working_set);
        }
      } else {
        if (working_set.get_weight(graph) > best_solution.get_weight(graph)) {
          std::cout << "\t===== found better solution while perturbing lol :D =====" << std::endl;
          best_solution.~packing_set();
          new(&best_solution) packing_set(working_set);
        }
      }
    }

    simulated_annealing temperature(iterations,
                                    i == 0 ? 1. : 0.2,
                                    i == 0 ? 0.9 : 0.5);
    local_search(graph, working_set, weighted, iterations, temperature);

    if (!weighted) {
      if (working_set.get_size() > best_solution.get_size()) {
        std::cout << "\t===== found better solution c: =====\n" << std::endl;
        best_solution.~packing_set();
        new(&best_solution) packing_set(working_set);
      } else {
        std::cout << "\t===== didn't find better solution :c =====\n" << std::endl;
      }
    } else {
      if (working_set.get_weight(graph) > best_solution.get_weight(graph)) {
        std::cout << "\t===== found better solution c: =====\n" << std::endl;
        best_solution.~packing_set();
        new(&best_solution) packing_set(working_set);
      } else {
        std::cout << "\t===== didn't find better solution :c =====\n" << std::endl;
      }
    }
  }

  solution_set = best_solution;
}

void perturb_solution(const csr_graph& graph, packing_set& solution_set) {
  const int amount_iterations = static_cast<int>(graph.amount_nodes() * 0.05);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution int_dist(0, graph.amount_nodes() - 1);

  int curr = int_dist(gen);

  for (int i = 0; i < amount_iterations; ++i) {
    if (solution_set.get_value(curr)) {
      solution_set.remove_solution_node(curr, graph);
    } else {
      std::vector<int> set_nodes = solution_set.get_set_partners(curr, graph.get_neighbors(curr));

      solution_set.remove_solution_nodes(set_nodes, graph);
      solution_set.add_solution_node(curr, graph.get_neighbors(curr));
    }

    for (int j = 0; j < 3; ++j) {
      const auto& neighbors = graph.get_neighbors(curr);
      curr = neighbors[int_dist(gen) % neighbors.size()];
    }
  }
}
