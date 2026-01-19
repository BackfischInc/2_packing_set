#include "local_swaps.hpp"

#include <ranges>
#include <random>

void maximize_solution(const csr_graph& graph, packing_set& solution_set,
                       std::set<uint64_t>& curr_nodes, std::set<uint64_t>& next_nodes,
                       const uint64_t& max_node_amount, const bool& weighted) {
  bool swap_occurred = true;
  int iter = 0;

  while (swap_occurred) {
    iter++;
    swap_occurred = false;

    for (const uint64_t& curr: curr_nodes) {
      if (solution_set.get_value(curr)) {
        /*if (find_2_1_swap(curr, graph, solution_set, next_nodes, weighted)) {
          swap_occurred = true;
        }*/
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
  std::set<uint64_t> pot_nodes;

  for (const uint64_t& neighbor: graph.get_neighbors(curr)) {
    if (has_potential(neighbor, curr, graph, solution_set)) {
      pot_nodes.insert(neighbor);
    }
  }

  for (const uint64_t& neighbor: graph.get_neighbors(curr)) {
    for (const uint64_t& pot_node: graph.get_neighbors(neighbor)) {
      if (!pot_nodes.contains(pot_node)) {
        if (has_potential(pot_node, curr, graph, solution_set)) {
          if (try_swap(pot_node, curr, pot_nodes, graph, solution_set, next_nodes, weighted)) {
            return true;
          }

          pot_nodes.insert(pot_node);
        }
      }
    }
  }

  return false;
}

bool try_swap(const uint64_t& node, const uint64_t& curr, const std::set<uint64_t>& pot_neighbors,
              const csr_graph& graph, packing_set& solution_set, std::set<uint64_t>& next_nodes, const bool& weighted) {
  for (const uint64_t& v: pot_neighbors) {
    if (weighted && graph.get_weight(v) + graph.get_weight(node) <= graph.get_weight(curr)) {
      continue;
    }

    if (!are_neighbors(graph, v, node)
        && !have_common_neighbor(graph.get_neighbors(v), graph.get_neighbors(node))) {

      solution_set.remove_solution_node(curr, graph);
      solution_set.add_solution_node(v, graph);
      solution_set.add_solution_node(node, graph);

      std::span<const uint64_t> neighbors = graph.get_neighbors(v);
      next_nodes.insert(neighbors.begin(), neighbors.end());

      neighbors = graph.get_neighbors(node);
      next_nodes.insert(neighbors.begin(), neighbors.end());

      neighbors = graph.get_neighbors(curr);
      next_nodes.insert(neighbors.begin(), neighbors.end());

      for (const uint64_t& next: next_nodes) {
        neighbors = graph.get_neighbors(next);
        next_nodes.insert(neighbors.begin(), neighbors.end());
      }

      return true;
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

  for (const uint64_t& neighbor: graph.get_neighbors(node)) {
    const uint64_t& n_set_neighbor = set.get_neighbor(neighbor);
    if (n_set_neighbor != curr && n_set_neighbor != -1) {
      return false;
    }
  }

  return true;
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
