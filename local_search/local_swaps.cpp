#include "local_swaps.hpp"

#include "../packing_set/iteration_queue.hpp"

#include <algorithm>

// each iteration: O(n * deg^5)
void maximize_solution(const csr_graph& graph, packing_set& solution_set,
                       std::vector<uint64_t>& set_nodes,
                       iteration_queue& queue, const bool& weighted, const bool& do_2_1_swaps) {
  bool swap_occurred = true;

  while (swap_occurred) {                                                                                    // O(?)
    queue.next_iter();                                                                                       // O(1)
    swap_occurred = false;                                                                                   // O(1)

    for (const uint64_t& curr: queue.curr_nodes()) {                                                         // O(n)
      if (do_2_1_swaps && solution_set.get_value(curr)) {                                                    // O(1)
        swap_occurred |= find_2_1_swap(curr, graph, solution_set, queue, weighted);                    // O(deg^5)
      } else {
        swap_occurred |= try_auto_include(curr, graph, solution_set, set_nodes, queue, weighted);   // O(deg²)
      }
    }
  }
}

// O(deg²)
bool try_auto_include(const uint64_t& curr, const csr_graph& graph, packing_set& solution_set,
                      std::vector<uint64_t>& set_nodes, iteration_queue& queue,
                      const bool& weighted) {
  solution_set.get_set_partners(set_nodes, curr, graph);                                                // O(deg)

  if (set_nodes.empty()) {                                                                                 // O(1)
    solution_set.add_solution_node(curr, graph);                                                           // O(deg)
    add_neighborhood(curr, queue, graph);                                                               // O(deg²)
    return true;
  }

  if (!weighted) {                                                                                         // O(1)
    if (set_nodes.size() == 1) {                                                                           // O(1)
      solution_set.remove_solution_nodes(set_nodes, graph);                                                // O(deg)
      solution_set.add_solution_node(curr, graph);                                                         // O(deg)
      add_neighborhood(curr, queue, graph);                                                             // O(deg²)
      return false;
    }
  } else {
    if (graph.get_weight(curr) > packing_set::get_weight(set_nodes, graph)) {                              // O(deg)
      solution_set.remove_solution_nodes(set_nodes, graph);                                                // O(deg)
      solution_set.add_solution_node(curr, graph);                                                         // O(deg)
      add_neighborhood(curr, queue, graph);                                                             // O(deg²)
      return true;
    }
  }

  return false;
}

// O(deg^5)
bool find_2_1_swap(const uint64_t& curr, const csr_graph& graph, packing_set& solution_set,
                   iteration_queue& queue, const bool& weighted) {
  std::unordered_set<uint64_t> pot_nodes;
  pot_nodes.reserve(graph.get_neighbors(curr).size() * 2 + 1);

  for (const uint64_t& neighbor: graph.get_neighbors(curr)) {                                       // O(deg)
    if (has_potential(neighbor, curr, graph, solution_set)) {                                  // O(deg)
      pot_nodes.insert(neighbor);                                                                   // O(1)
    }
  }

  for (const uint64_t& neighbor: graph.get_neighbors(curr)) {                                       // O(deg)
    for (const uint64_t& pot_node: graph.get_neighbors(neighbor)) {                                 // O(deg)
      if (!pot_nodes.contains(pot_node)) {                                                          // O(deg)
        if (has_potential(pot_node, curr, graph, solution_set)) {                                   // O(deg)
          if (try_swap(pot_node, curr, pot_nodes, graph, solution_set, queue, weighted)) {    // O(deg³)
            return true;
          }

          pot_nodes.insert(pot_node);                                                               // O(1)
        }
      }
    }
  }

  return false;
}

// O(deg³)
bool try_swap(const uint64_t& node, const uint64_t& curr, const std::unordered_set<uint64_t>& pot_neighbors,
              const csr_graph& graph, packing_set& solution_set,
              iteration_queue& queue, const bool& weighted) {
  for (const uint64_t& v: pot_neighbors) {                                                           // O(deg)
    if (weighted && graph.get_weight(v) + graph.get_weight(node) <= graph.get_weight(curr)) {        // O(1)
      continue;
    }

    if (!are_neighbors(graph, v, node)                                                            // O(deg)
        && !have_common_neighbor(graph.get_neighbors(v), graph.get_neighbors(node))) {           // O(deg)
      solution_set.remove_solution_node(curr, graph);                                                // O(deg)
      solution_set.add_solution_node(v, graph);                                                      // O(deg)
      solution_set.add_solution_node(node, graph);                                                   // O(deg)
      add_neighborhood(v, queue, graph);                                                          // O(deg²)
      add_neighborhood(node, queue, graph);                                                       // O(deg²)
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
  const auto neighbors = graph.get_neighbors(w);
  return std::binary_search(neighbors.begin(), neighbors.end(), v);
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

bool add_neighborhood(const uint64_t& node, iteration_queue& queue, const csr_graph& graph) {
  // The graph caches this distance-two neighborhood. Recomputing the nested
  // adjacency traversal here was one of the dominant costs of the ILS loop.
  for (const uint64_t neighbor : graph.get_square_neighbors(node)) {
    if (!queue.add_node(neighbor)) {
      return false;
    }
  }

  return true;
}
