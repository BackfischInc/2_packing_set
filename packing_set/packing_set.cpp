#include "packing_set.hpp"

#include <iostream>
#include <unordered_set>

void packing_set::add_solution_node(const uint64_t& id, const csr_graph& graph) {
  insert(id, graph);
  set_neighbors[id] = id;

  for (const auto& neighbor: graph.get_neighbors(id)) {
    remove(neighbor, graph);
    set_neighbors[neighbor] = id;
  }
}

void packing_set::remove_solution_node(const uint64_t& id, const csr_graph& graph) {
  remove(id, graph);
  set_neighbors[id] = -1;
  changelog.push(id);

  for (const auto& neighbor: graph.get_neighbors(id)) {
    remove(neighbor, graph);
    set_neighbors[neighbor] = -1;
  }
}

void packing_set::remove_solution_nodes(const std::unordered_set<uint64_t>& ids, const csr_graph& graph) {
  for (const uint64_t id: ids) {
    remove_solution_node(id, graph);
  }
}

void packing_set::get_set_partners(std::unordered_set<uint64_t>& result,
                                   const uint64_t& curr, const csr_graph& graph) const {
  result.clear();

  if (get_neighbor(curr) != -1) {
    result.insert(get_neighbor(curr));
  }
  for (const uint64_t& node: graph.get_neighbors(curr)) {
    if (get_neighbor(node) != -1) {
      result.insert(get_neighbor(node));
    }
  }
}

uint64_t packing_set::get_weight(const std::unordered_set<uint64_t>& set, const csr_graph& graph) {
  uint64_t result = 0;

  for (const uint64_t& node: set) {
    result += graph.get_weight(node);
  }

  return result;
}

void packing_set::unwind(const csr_graph& graph, std::unordered_set<uint64_t>& set_nodes) {
  while (!changelog.empty()) {
    const uint64_t curr = changelog.top();
    changelog.pop();

    get_set_partners(set_nodes, curr, graph);

    for (const uint64_t& node: set_nodes) {
      remove(node, graph);
      set_neighbors[node] = -1;

      for (const uint64_t& neighbor: graph.get_neighbors(node)) {
        remove(neighbor, graph);
        set_neighbors[neighbor] = -1;
      }
    }

    insert(curr, graph);
    set_neighbors[curr] = curr;
    for (const uint64_t& neighbor: graph.get_neighbors(curr)) {
      remove(neighbor, graph);
      set_neighbors[neighbor] = curr;
    }
  }
}

void packing_set::clear_changelog() {
  while (!changelog.empty()) {
    changelog.pop();
  }
}
