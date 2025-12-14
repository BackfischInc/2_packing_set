#ifndef PACKING_SET_HPP
#define PACKING_SET_HPP

#include <cstdint>
#include <vector>
#include <cstring>
#include <set>
#include <span>

#include "../graph/csr_graph.hpp"

class packing_set {
  std::vector<uint8_t> solution_set;
  std::vector<int> set_neighbors;
  int size = 0;

public:
  explicit packing_set(const int& n) : solution_set(n, 0), set_neighbors(n, -1) {}

  packing_set(const packing_set&) = default;

  [[nodiscard]] int get_size() const { return size; }

  void insert(const int& index) {
    if (!solution_set[index]) {
      solution_set[index] = 1;
      ++size;
    }
  }

  void remove(const int& index) {
    if (solution_set[index]) {
      solution_set[index] = 0;
      --size;
    }
  }

  [[nodiscard]] bool get_value(const int& index) const {
    return solution_set[index];
  }

  [[nodiscard]] int get_neighbor(const int& id) const {
    return set_neighbors[id];
  }

  void add_solution_node(const int& id, const csr_graph& graph);

  void remove_solution_node(const int& id, const csr_graph& graph);

  void remove_solution_nodes(const std::set<int>& ids, const csr_graph& graph);

  [[nodiscard]] std::set<int> get_set_partners(const int& curr, const std::span<const int>& nodes) const;

  [[nodiscard]] int get_weight(const csr_graph& graph) const;

  static int get_weight(const std::set<int>& set, const csr_graph& graph);

  [[nodiscard]] bool verify_set_neighbors(const csr_graph& graph) const;
};

#endif
