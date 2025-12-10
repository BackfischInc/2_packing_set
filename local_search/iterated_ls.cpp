#include "iterated_ls.hpp"
#include "local_search.hpp"

#include <iostream>
#include <random>

void iterated_local_search(packing_set& solution_set, const csr_graph& graph, const bool& weighted) {
  packing_set best_solution(graph.amount_nodes());

  constexpr unsigned long long first_iter_mult = 0xFF;
  constexpr unsigned long long other_iter_mult = 0x8F;

  constexpr double first_iter_start = 1.;
  constexpr double other_iter_start = 0.2;

  constexpr double first_iter_cutoff = 0.9;
  constexpr double other_iter_cutoff = 0.5;

  constexpr double perturb_amount = 0.005;

  std::cout << "\t=====" << std::endl;

  for (int i = 0; i < 5; ++i) {
    printf("\titeration: %2d\n", i + 1);
    packing_set working_set(best_solution);

    const unsigned long long iterations = graph.amount_nodes() * (i == 0 ? first_iter_mult : other_iter_mult);

    if (i != 0) {
      printf("\t=====\n\tperturbing solution of size: %8d...", weighted ? working_set.get_weight(graph) : working_set.get_size());
      perturb_solution(graph, working_set, perturb_amount);
      printf("[DONE]\n\t=====\n");
      update_if_better(graph, best_solution, working_set, weighted);
    }

    simulated_annealing temperature(iterations,
                                    i == 0 ? first_iter_start : other_iter_start,
                                    i == 0 ? first_iter_cutoff : other_iter_cutoff);

    local_search(graph, working_set, weighted, iterations, temperature);
    update_if_better(graph, best_solution, working_set, weighted);
    std::cout << "\t=====" << std::endl;
  }

  solution_set = best_solution;
}

void perturb_solution(const csr_graph& graph, packing_set& solution_set, const double& amount) {
  const int amount_iterations = static_cast<int>(static_cast<double>(graph.amount_nodes()) * amount);

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

void update_if_better(const csr_graph& graph, packing_set& best_solution,
                      const packing_set& working_set, const bool& weighted) {
  if (!weighted) {
    if (working_set.get_size() > best_solution.get_size()) {
      std::cout << "\t===== found better solution c: =====\n" << std::endl;
      best_solution.~packing_set();
      new(&best_solution) packing_set(working_set);
    }
  } else {
    if (working_set.get_weight(graph) > best_solution.get_weight(graph)) {
      std::cout << "\t===== found better solution c: =====\n" << std::endl;
      best_solution.~packing_set();
      new(&best_solution) packing_set(working_set);
    }
  }
}
