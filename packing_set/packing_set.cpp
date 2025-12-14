#include "packing_set.hpp"

#include <iostream>

void packing_set::add_solution_node(const int& id, const csr_graph& graph) {
  insert(id);
  set_neighbors[id] = id;
  for (const auto& neighbor: graph.get_neighbors(id)) {
    remove(neighbor);
    set_neighbors[neighbor] = id;
  }
}

void packing_set::remove_solution_node(const int& id, const csr_graph& graph) {
  remove(id);
  set_neighbors[id] = -1;
  for (const auto& neighbor: graph.get_neighbors(id)) {
    remove(neighbor);
    set_neighbors[neighbor] = -1;
  }
}

void packing_set::remove_solution_nodes(const std::set<int>& ids, const csr_graph& graph) {
  for (const int id: ids) {
    remove_solution_node(id, graph);
  }
}

std::set<int> packing_set::get_set_partners(const int& curr, const std::span<const int>& nodes) const {
  std::set<int> result;

  result.insert(get_neighbor(curr));
  for (const int& node: nodes) {
    result.insert(get_neighbor(node));
  }

  result.extract(-1);

  return result;
}


int packing_set::get_weight(const csr_graph& graph) const {
  const int n = graph.amount_nodes();
  int result = 0;

  for (int i = 0; i < n; ++i) {
    if (get_value(i)) {
      result += graph.get_weight(i);
    }
  }

  return result;
}

int packing_set::get_weight(const std::set<int>& set, const csr_graph& graph) {
  int result = 0;

  for (const int& node: set) {
    result += graph.get_weight(node);
  }

  return result;
}

bool packing_set::verify_set_neighbors(const csr_graph& graph) const {
  for (int i = 0; i < graph.amount_nodes(); ++i) {
    if (get_value(i)) {
      for (const int& neighbor: graph.get_neighbors(i)) {
        if (get_neighbor(neighbor) != i) {
          return false;
        }
      }
    }
  }

  return true;
}
