#include "packing_set.hpp"

void packing_set::add_solution_node(const int& id, const std::span<const int>& neighbors) {
  insert(id);
  set_neighbors[id] = id;
  for (const auto& neighbor: neighbors) {
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

void packing_set::remove_solution_nodes(const std::vector<int>& ids, const csr_graph& graph) {
  for (const int id: ids) {
    remove_solution_node(id, graph);
  }
}

std::vector<int> packing_set::get_set_partners(const int& curr, const std::span<const int>& nodes) const {
  std::vector<int> result;

  if (get_neighbor(curr) != -1) {
    result.push_back(get_neighbor(curr));
  }

  for (const int& node: nodes) {
    if (const int partner = get_neighbor(node);
      partner != -1 && std::ranges::find(result, partner) == result.end()) {
      result.push_back(get_neighbor(node));
    }
  }

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

int packing_set::get_weight(const std::vector<int>& set, const csr_graph& graph) {
  int result = 0;

  for (const int& node: set) {
    result += graph.get_weight(node);
  }

  return result;
}
