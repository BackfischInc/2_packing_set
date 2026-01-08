#ifndef PACKING_SET_HPP
#define PACKING_SET_HPP

#include <cstdint>
#include <vector>
#include <cstring>
#include <set>
#include <span>
#include <stack>

#include "../graph/csr_graph.hpp"

class packing_set {
  std::vector<uint8_t> solution_set;
  std::vector<uint64_t> set_neighbors;
  std::stack<uint64_t> changelog;

  uint64_t size = 0;

public:
  explicit packing_set(const uint64_t& n) : solution_set(n, 0), set_neighbors(n, -1) {}

  [[nodiscard]] uint64_t get_size() const { return size; }

  void insert(const uint64_t& index) {
    if (!solution_set[index]) {
      solution_set[index] = 1;
      ++size;
    }
  }

  void remove(const uint64_t& index) {
    if (solution_set[index]) {
      solution_set[index] = 0;
      --size;
    }
  }

  [[nodiscard]] bool get_value(const uint64_t& index) const {
    return solution_set[index];
  }

  [[nodiscard]] uint64_t get_neighbor(const uint64_t& id) const {
    return set_neighbors[id];
  }

  void add_solution_node(const uint64_t& id, const csr_graph& graph);

  void remove_solution_node(const uint64_t& id, const csr_graph& graph);

  void remove_solution_nodes(const std::set<uint64_t>& ids, const csr_graph& graph);

  [[nodiscard]] std::set<uint64_t> get_set_partners(const uint64_t& curr, const std::span<const uint64_t>& nodes) const;

  [[nodiscard]] uint64_t get_weight(const csr_graph& graph) const;

  static uint64_t get_weight(const std::set<uint64_t>& set, const csr_graph& graph);

  [[nodiscard]] bool verify_set_neighbors(const csr_graph& graph) const;

  void unwind(const csr_graph& graph);

  void clear_changelog();
};

#endif
