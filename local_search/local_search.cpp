#include "local_search.hpp"

#include <iostream>
#include <random>
#include <chrono>

void local_search(const csr_graph& graph, packing_set& solution_set, const bool& weighted,
                  const unsigned long long& iterations, simulated_annealing& temp) {
  const unsigned long long int& n = graph.amount_nodes();

  int current_result = weighted ? get_weight(solution_set, graph) : solution_set.get_size();
  int best_result = current_result;

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
      get_set_partners(set_nodes, solution_set, curr, nodes_to_check, graph);

      if (const double randomValue = double_dist(gen);
        set_nodes.size() <= 1 || randomValue <= temp.get_probability(static_cast<int>(set_nodes.size()) - 1, i)) {
        solution_set.remove_solution_nodes(set_nodes, graph);
        solution_set.add_solution_node(curr, graph.get_neighbors(curr));

        current_result = solution_set.get_size();
      }

      if (current_result > best_result) {
        best_result = current_result;
      }
    } else {
      set_nodes.clear();
      get_set_partners(set_nodes, solution_set, curr, nodes_to_check, graph);
      const int weight = get_weight(set_nodes, graph);

      if (const double randomValue = double_dist(gen);
        graph.get_weight(curr) >= weight ||
        randomValue <= temp.get_probability(weight - graph.get_weight(curr),
          i)) {
        solution_set.remove_solution_nodes(set_nodes, graph);
        solution_set.add_solution_node(curr, graph.get_neighbors(curr));

        current_result = current_result - weight + graph.get_weight(curr);
      }

      if (current_result > best_result) {
        best_result = current_result;
      }
    }

    if (i % n == 0 || i == iterations - 1) {
      const double completion = static_cast<double>(i + 1) / static_cast<double>(iterations);
      print_update(best_result, best_result - current_result, temp.get_temp(i), completion);
    }
  }
}

void print_update(const int& best_result, const int& diff, const double& current_temp, const double& completion) {
  printf("\t\tbest found size: %8d, current diff: %8d, temp: %.4f, completion: %.4f\n",
         best_result, diff, current_temp, completion);
}

// find all distinct nodes in the neighborhood that are already in the solution set and would have to be removed
// to put the current node in
void get_set_partners(std::vector<int>& result, const packing_set& solution_set, const int& curr,
                     const std::span<const int>& nodes, const csr_graph& graph) {
  if (solution_set.get_neighbor(curr) != -1) {
    result.push_back(solution_set.get_neighbor(curr));
  }

  for (const int& node: nodes) {
    if (const int partner = solution_set.get_neighbor(node);
      partner != -1 && std::ranges::find(result, partner) == result.end()) {
      result.push_back(solution_set.get_neighbor(node));
    }
  }
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
