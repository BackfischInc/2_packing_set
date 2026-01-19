#include "packing_set.hpp"

#include <iostream>

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

void packing_set::remove_solution_nodes(const std::set<uint64_t>& ids, const csr_graph& graph) {
  for (const uint64_t id: ids) {
    remove_solution_node(id, graph);
  }
}

std::set<uint64_t> packing_set::get_set_partners(const uint64_t& curr, const std::span<const uint64_t>& nodes) const {
  std::set<uint64_t> result;

  result.insert(get_neighbor(curr));
  for (const uint64_t& node: nodes) {
    result.insert(get_neighbor(node));
  }

  result.extract(-1);

  return result;
}

uint64_t packing_set::get_weight(const std::set<uint64_t>& set, const csr_graph& graph) {
  uint64_t result = 0;

  for (const uint64_t& node: set) {
    result += graph.get_weight(node);
  }

  return result;
}

bool packing_set::verify_set_neighbors(const csr_graph& graph) const {
  for (int i = 0; i < graph.amount_nodes(); ++i) {
    if (get_value(i)) {
      for (const uint64_t& neighbor: graph.get_neighbors(i)) {
        if (get_neighbor(neighbor) != i) {
          return false;
        }
      }
    }
  }

  return true;
}

void packing_set::unwind(const csr_graph& graph) {
  while (!changelog.empty()) {
    const uint64_t curr = changelog.top();
    changelog.pop();

    std::set<uint64_t> set_partners = get_set_partners(curr, graph.get_neighbors(curr));

    for (const uint64_t& node: set_partners) {
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
