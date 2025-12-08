#include "local_search.hpp"

#include <iostream>
#include <random>
#include <chrono>

void local_search(const csr_graph& graph, packing_set& solution_set, const bool& weighted,
                  const unsigned long long& iterations, simulated_annealing& temp) {
  const unsigned long long int& n = graph.amount_nodes();

  int current_weight = weighted ? get_weight(solution_set, graph) : -1;
  int best_result = weighted ? current_weight : solution_set.get_size();

  std::vector<int> set_nodes;
  set_nodes.reserve(graph.max_deg);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution double_dist(0.0, 1.0);
  std::uniform_int_distribution int_dist(0, static_cast<int>(n - 1));

  for (unsigned long long int i = 0; i < iterations; i++) {
    const int curr = int_dist(gen);

    if (solution_set.get_value(curr)) {
      continue;
    }

    const auto& nodes_to_check = graph.get_neighbors(curr);

    if (!weighted) {
      set_nodes.clear();
      get_set_partners(set_nodes, solution_set, curr, nodes_to_check, graph, weighted);

      if (const double randomValue = double_dist(gen);
        set_nodes.size() <= 1 || randomValue <= temp.get_probability(
          solution_set.get_size(),
          solution_set.get_size() - static_cast<int>(set_nodes.size()) + 1,
          i)) {
        solution_set.remove_solution_nodes(set_nodes, graph);
        solution_set.add_solution_node(curr, graph.get_neighbors(curr));
      }

      if (solution_set.get_size() > best_result) {
        best_result = solution_set.get_size();
      }
    } else {
      set_nodes.clear();
      const int weight = get_set_partners(set_nodes, solution_set, curr, nodes_to_check, graph, weighted);

      if (const double randomValue = double_dist(gen);
        graph.get_weight(curr) >= weight ||
        randomValue <= temp.get_probability(
          current_weight,
          current_weight - weight + graph.get_weight(curr),
          i)) {
        solution_set.remove_solution_nodes(set_nodes, graph);
        solution_set.add_solution_node(curr, graph.get_neighbors(curr));

        current_weight = current_weight - weight + graph.get_weight(curr);
      }

      if (current_weight > best_result) {
        best_result = current_weight;
      }
    }

    if (i % n == 0) {
      const double completion = static_cast<double>(i) / static_cast<double>(iterations);
      printf("\t\tbest found size: %8d, current diff: %6d, temp: %.3f, completion: %.3f\n",
             best_result,
             weighted
               ? best_result - get_weight(
                   solution_set, graph)
               : best_result - solution_set.get_size(),
             temp.get_temp(i),
             completion);
    }
  }

  printf("\t\tbest found size: %8d, current diff: %6d, temp: %.3f, completion: %.3f\n\t[DONE]\n",
         best_result, weighted ? best_result - get_weight(solution_set, graph) : best_result - solution_set.get_size(),
         0., 1.
  );
}

// find all distinct nodes in the neighborhood that are already in the solution set and would have to be removed
// to put the current node in
int get_set_partners(std::vector<int>& result, const packing_set& solution_set, const int& curr,
                     const std::span<const int>& nodes, const csr_graph& graph, const bool& weighted) {
  int weight = 0;

  if (solution_set.get_neighbor(curr) != -1) {
    result.push_back(solution_set.get_neighbor(curr));
    if (weighted) { weight += graph.get_weight(solution_set.get_neighbor(curr)); }
  }

  for (const int& node: nodes) {
    if (const int partner = solution_set.get_neighbor(node);
      partner != -1 && std::ranges::find(result, partner) == result.end()) {
      result.push_back(solution_set.get_neighbor(node));
      if (weighted) { weight += graph.get_weight(solution_set.get_neighbor(node)); }
    }
  }

  return weight;
}

int get_weight(const packing_set& set, const csr_graph& graph) {
  const int n = graph.amount_nodes();
  int result = 0;

  for (int i = 0; i < n; ++i) {
    if (set.get_value(i)) {
      result += graph.get_weight(i);
    }
  }

  return result;
}

int get_weight(const std::vector<int>& set, const csr_graph& graph) {
  int result = 0;

  for (const int& node: set) {
    result += graph.get_weight(node);
  }

  return result;
}
