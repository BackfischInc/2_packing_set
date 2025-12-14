#include "local_swaps.hpp"

#include <iostream>
#include <ranges>
#include <algorithm>

void maximize_solution(const csr_graph& graph, packing_set& solution_set, const bool weighted) {
  bool swap_occurred = true;
  int iter = 0;
  std::cout << "\t[" << iter << "] " << (weighted ? solution_set.get_weight(graph) : solution_set.get_size()) <<
      std::endl;
  while (swap_occurred) {
    iter++;
    swap_occurred = false;
    for (int i = 0; i < graph.amount_nodes(); ++i) {
      if (solution_set.get_value(i)) {
        swap_occurred = swap_occurred || find_2_1_swap(i, graph, solution_set, weighted);
      } else {
        swap_occurred = swap_occurred || try_auto_include(i, graph, solution_set, weighted);
      }
    }
    std::cout << "\t[" << iter << "] " << (weighted ? solution_set.get_weight(graph) : solution_set.get_size()) <<
        std::endl;
  }
}

bool try_auto_include(const int& curr, const csr_graph& graph, packing_set& solution_set, const bool& weighted) {
  const std::set<int>& set_neighbors = solution_set.get_set_partners(curr, graph.get_neighbors(curr));

  if (set_neighbors.empty()) {
    solution_set.add_solution_node(curr, graph);
    return true;
  }

  if (weighted) {
    const int set_weight = packing_set::get_weight(set_neighbors, graph);
    if (graph.get_weight(curr) > set_weight) {
      solution_set.remove_solution_nodes(set_neighbors, graph);
      solution_set.add_solution_node(curr, graph);
      return true;
    }
  }

  return false;
}

bool find_2_1_swap(const int& curr, const csr_graph& graph, packing_set& solution_set, const bool& weighted) {
  std::set<int> neighborhood_2;
  for (const int& neighbor_1: graph.get_neighbors(curr)) {
    neighborhood_2.insert(neighbor_1);
    for (const int& neighbor_2: graph.get_neighbors(neighbor_1)) {
      neighborhood_2.insert(neighbor_2);
    }
  }

  for (auto iter = neighborhood_2.begin(); iter != neighborhood_2.end(); ++iter) {
    const int& v = *iter;
    if (v != curr && has_potential(v, curr, graph, solution_set)) {
      for (auto iter_1 = iter; iter_1 != neighborhood_2.end(); ++iter_1) {
        const int& w = *iter_1;
        if (w != v && w != curr && has_potential(w, curr, graph, solution_set)) {
          if (weighted && graph.get_weight(v) + graph.get_weight(w) <= graph.get_weight(curr)) {
            continue;
          }

          if (!are_neighbors(graph, v, w) &&
              !have_common_neighbor(graph.get_neighbors(v), graph.get_neighbors(w))) {
            solution_set.remove_solution_node(curr, graph);
            solution_set.add_solution_node(v, graph);
            solution_set.add_solution_node(w, graph);
            return true;
          }
        }
      }
    }
  }

  return false;
}

bool has_potential(const int& node, const int& curr, const csr_graph& graph, const packing_set& set) {
  const int& set_neighbor = set.get_neighbor(node);
  if (set_neighbor != curr && set_neighbor != -1) {
    return false;
  }

  return std::ranges::all_of(
    graph.get_neighbors(node),
    [&](const int& neighbor) {
      const int n_set_neighbor = set.get_neighbor(neighbor);
      return n_set_neighbor == curr || n_set_neighbor == -1;
    });
}

bool are_neighbors(const csr_graph& graph, const int& v, const int& w) {
  for (const int& neighbor: graph.get_neighbors(w)) {
    if (neighbor == v) {
      return true;
    }
    if (neighbor > v) {
      return false;
    }
  }

  return false;
}

bool have_common_neighbor(const std::span<const int>& a, const std::span<const int>& b) {
  int i = 0, j = 0;

  while (i < a.size() && j < b.size()) {
    if (a[i] == b[j]) {
      return true;
    }

    if (a[i] < b[j]) {
      i++;
    } else {
      j++;
    }
  }
  return false;
}
