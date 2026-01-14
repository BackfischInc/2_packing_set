#include "local_swaps.hpp"

#include <ranges>
#include <algorithm>
#include <random>

void maximize_solution(const csr_graph& graph, packing_set& solution_set,
                       std::set<uint64_t>& curr_nodes, std::set<uint64_t>& next_nodes,
                       const uint64_t max_node_amount, const bool& weighted) {
  bool swap_occurred = true;
  int iter = 0;

  while (swap_occurred) {
    iter++;
    swap_occurred = false;

    for (const uint64_t& curr: curr_nodes) {
      if (solution_set.get_value(curr)) {
        /*
        if (find_2_1_swap(curr, graph, solution_set, next_nodes, weighted)) {
          swap_occurred = true;
        }
        */
      } else {
        if (try_auto_include(curr, graph, solution_set, next_nodes, weighted)) {
          swap_occurred = true;
        }
      }

      if (next_nodes.size() >= max_node_amount) {
        break;
      }
    }

    curr_nodes = next_nodes;
    next_nodes.clear();
  }
}

bool try_auto_include(const uint64_t& curr, const csr_graph& graph, packing_set& solution_set,
                      std::set<uint64_t>& next_nodes, const bool& weighted) {
  const std::set<uint64_t>& set_neighbors = solution_set.get_set_partners(curr, graph.get_neighbors(curr));

  if (set_neighbors.empty()) {
    solution_set.add_solution_node(curr, graph);

    std::span<const uint64_t> neighbors = graph.get_neighbors(curr);
    next_nodes.insert(neighbors.begin(), neighbors.end());
    return true;
  }

  if (!weighted) {
    if (set_neighbors.size() == 1) {
      solution_set.remove_solution_nodes(set_neighbors, graph);
      solution_set.add_solution_node(curr, graph);

      for (const uint64_t& node: set_neighbors) {
        std::span<const uint64_t> neighbors = graph.get_neighbors(node);
        next_nodes.insert(neighbors.begin(), neighbors.end());
      }

      const std::span<const uint64_t> neighbors = graph.get_neighbors(curr);
      next_nodes.insert(neighbors.begin(), neighbors.end());

      return false;
    }
  } else {
    if (graph.get_weight(curr) > packing_set::get_weight(set_neighbors, graph)) {
      solution_set.remove_solution_nodes(set_neighbors, graph);
      solution_set.add_solution_node(curr, graph);

      for (const uint64_t& node: set_neighbors) {
        std::span<const uint64_t> neighbors = graph.get_neighbors(node);
        next_nodes.insert(neighbors.begin(), neighbors.end());
      }

      std::span<const uint64_t> neighbors = graph.get_neighbors(curr);
      next_nodes.insert(neighbors.begin(), neighbors.end());

      for (const uint64_t& node: next_nodes) {
        neighbors = graph.get_neighbors(node);
        next_nodes.insert(neighbors.begin(), neighbors.end());
      }

      return true;
    }
  }

  return false;
}

bool find_2_1_swap(const uint64_t& curr, const csr_graph& graph, packing_set& solution_set,
                   std::set<uint64_t>& next_nodes, const bool& weighted) {
  std::set<uint64_t> pot_neighbors;

  for (const uint64_t& neighbor_1: graph.get_neighbors(curr)) {
    if (has_potential(neighbor_1, curr, graph, solution_set)) {
      pot_neighbors.insert(neighbor_1);
    }
    for (const uint64_t& neighbor_2: graph.get_neighbors(neighbor_1)) {
      if (has_potential(neighbor_2, curr, graph, solution_set)) {
        pot_neighbors.insert(neighbor_2);
      }
    }
  }

  for (auto iter = pot_neighbors.begin(); iter != pot_neighbors.end(); ++iter) {
    const uint64_t& v = *iter;
    for (auto iter_1 = iter; iter_1 != pot_neighbors.end(); ++iter_1) {
      const uint64_t& w = *iter_1;

      if (v == w) {
        continue;
      }

      if (weighted && graph.get_weight(v) + graph.get_weight(w) <= graph.get_weight(curr)) {
        continue;
      }

      if (!are_neighbors(graph, v, w) &&
          !have_common_neighbor(graph.get_neighbors(v), graph.get_neighbors(w))) {
        solution_set.remove_solution_node(curr, graph);
        solution_set.add_solution_node(v, graph);
        solution_set.add_solution_node(w, graph);

        std::span<const uint64_t> neighbors = graph.get_neighbors(v);
        next_nodes.insert(neighbors.begin(), neighbors.end());

        neighbors = graph.get_neighbors(w);
        next_nodes.insert(neighbors.begin(), neighbors.end());

        neighbors = graph.get_neighbors(curr);
        next_nodes.insert(neighbors.begin(), neighbors.end());

        for (const uint64_t& node: next_nodes) {
          neighbors = graph.get_neighbors(node);
          next_nodes.insert(neighbors.begin(), neighbors.end());
        }

        return true;
      }
    }
  }

  return false;
}

bool has_potential(const uint64_t& node, const uint64_t& curr, const csr_graph& graph, const packing_set& set) {
  if (node == curr) {
    return false;
  }

  const uint64_t& set_neighbor = set.get_neighbor(node);
  if (set_neighbor != curr && set_neighbor != -1) {
    return false;
  }

  return std::ranges::all_of(
    graph.get_neighbors(node),
    [&](const uint64_t& neighbor) {
      const uint64_t n_set_neighbor = set.get_neighbor(neighbor);
      return n_set_neighbor == curr || n_set_neighbor == -1;
    });
}

bool are_neighbors(const csr_graph& graph, const uint64_t& v, const uint64_t& w) {
  for (const uint64_t& neighbor: graph.get_neighbors(w)) {
    if (neighbor == v) {
      return true;
    }
    if (neighbor > v) {
      return false;
    }
  }

  return false;
}

bool have_common_neighbor(const std::span<const uint64_t>& a, const std::span<const uint64_t>& b) {
  uint64_t i = 0, j = 0;

  while (i < a.size() && j < b.size()) {
    if (a[i] == b[j]) {
      return true;
    }

    if (a[i] < b[j]) { i++; } else { j++; }
  }
  return false;
}
