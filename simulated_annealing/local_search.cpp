#include "local_search.hpp"

#include <iostream>
#include <random>
#include <chrono>

void local_search(const csr_graph& graph, packing_set& solution_set, const bool& weighted,
                  const unsigned long long& iterations, temperature& temp) {
  uint64_t current_result = weighted ? solution_set.get_weight(graph) : solution_set.get_size();
  uint64_t best_result = current_result;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint64_t> uint64_t_dist(0, graph.amount_nodes() - 1);
  std::uniform_real_distribution double_dist(0.0, 1.0);

  constexpr double output_freq = 0.1;
  const auto output_interval = static_cast<unsigned long long>(static_cast<double>(iterations) * output_freq);

  for (uint64_t i = 0; i <= iterations; i++) {
    const uint64_t curr = uint64_t_dist(gen);

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
}

bool search_node(const uint64_t& curr, const csr_graph& graph, packing_set& solution_set, temperature& temp,
                 uint64_t& current_result, const uint64_t& iteration,
                 const double& random_value, const bool& weighted) {
  const std::set<uint64_t> set_nodes = solution_set.get_set_partners(curr, graph);
  const uint64_t& weight = packing_set::get_weight(set_nodes, graph);

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

void update_current_result(uint64_t& current_result, const uint64_t& size, const uint64_t& set_weight, const uint64_t& node_weight,
                           const bool& weighted) {
  if (weighted) {
    current_result = current_result - set_weight + node_weight;
  } else {
    current_result = size;
  }
}

int get_diff(const size_t& size, const uint64_t& set_weight, const uint64_t& node_weight, const bool& weighted) {
  if (!weighted) {
    return static_cast<int>(size) - 1;
  }

  return set_weight - node_weight;
}

bool direct_swap(const size_t& size, const uint64_t& set_weight, const uint64_t& node_weight, const bool& weighted) {
  if (!weighted) {
    return size <= 1;
  }

  return node_weight >= set_weight;
}

void print_update(const uint64_t& best_result, const uint64_t& diff, const double& current_temp, const double& completion) {
 // printf("\tbest: %8d, diff: %8d, temp: %.4f, done: %.1f\n",
 //        best_result, diff, current_temp, completion);
}
