#include "local_search.hpp"

#include <iostream>
#include <random>
#include <chrono>

void local_search(const csr_graph& graph, packing_set& solution_set, const bool& weighted,
                  const unsigned long long& iterations, simulated_annealing& temp) {
  int current_result = weighted ? solution_set.get_weight(graph) : solution_set.get_size();
  int best_result = current_result;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution int_dist(0, graph.amount_nodes() - 1);
  std::uniform_real_distribution double_dist(0.0, 1.0);

  constexpr double output_freq = 0.1;
  const auto output_interval = static_cast<unsigned long long>(static_cast<double>(iterations) * output_freq);

  for (unsigned long long int i = 0; i <= iterations; i++) {
    const int curr = int_dist(gen);

    if (solution_set.get_value(curr)) {
      continue;
    }

    const double random_value = double_dist(gen);
    const bool did_swap = search_node(curr, graph, solution_set, temp,
                                      current_result, i, random_value, weighted);

    if (did_swap && current_result > best_result) {
      best_result = current_result;
    }

    if (i % output_interval == 0) {
      const double completion = static_cast<double>(i + 1) / static_cast<double>(iterations);
      print_update(best_result, best_result - current_result, temp.get_temp(i), completion);
    }
  }
  std::cout << std::endl;
}

bool search_node(const int& curr, const csr_graph& graph, packing_set& solution_set, simulated_annealing& temp,
                 int& current_result, const unsigned long long int& iteration,
                 const double& random_value, const bool& weighted) {
  const auto& nodes_to_check = graph.get_neighbors(curr);

  const std::set<int> set_nodes = solution_set.get_set_partners(curr, nodes_to_check);
  const int& weight = packing_set::get_weight(set_nodes, graph);

  if (direct_swap(set_nodes.size(), weight, graph.get_weight(curr), weighted) ||
      random_value <= temp.get_probability(
        get_diff(set_nodes.size(), weight, graph.get_weight(curr), weighted),
        iteration)) {
    solution_set.remove_solution_nodes(set_nodes, graph);
    solution_set.add_solution_node(curr, graph);

    update_current_result(current_result, solution_set.get_size(), weight,
                          graph.get_weight(curr), weighted);

    return true;
  }

  return false;
}

void update_current_result(int& current_result, const int& size, const int& set_weight, const int& node_weight,
                           const bool& weighted) {
  if (weighted) {
    current_result = current_result - set_weight + node_weight;
  } else {
    current_result = size;
  }
}

int get_diff(const size_t& size, const int& set_weight, const int& node_weight, const bool& weighted) {
  if (!weighted) {
    return static_cast<int>(size) - 1;
  }

  return set_weight - node_weight;
}

bool direct_swap(const size_t& size, const int& set_weight, const int& node_weight, const bool& weighted) {
  if (!weighted) {
    return size <= 1;
  }

  return node_weight >= set_weight;
}

void print_update(const int& best_result, const int& diff, const double& current_temp, const double& completion) {
  printf("\tbest: %8d, diff: %8d, temp: %.4f, done: %.1f\n",
         best_result, diff, current_temp, completion);
}
