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

void packing_set::copy(const packing_set& other) {
  for (int i = 0; i < this->set_neighbors.size(); ++i) {
    this->set_neighbors[i] = other.set_neighbors[i];
    this->solution_set[i] = other.solution_set[i];
  }
}
