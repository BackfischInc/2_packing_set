#ifndef PACKING_SET_HPP
#define PACKING_SET_HPP

#include <cstdint>
#include <vector>
#include <cstring>
#include <set>
#include <stack>
#include <unordered_set>

#include "../graph/csr_graph.hpp"

class packing_set {
  std::vector<uint8_t> solution_set;
  std::vector<uint64_t> set_neighbors;
  std::stack<uint64_t> changelog;
  std::set<uint64_t> dist_set_neighbors;

  uint64_t size = 0;
  uint64_t weight = 0;

public:
  explicit packing_set(const uint64_t& n) :
    solution_set(n, 0), set_neighbors(n, -1) {}

  [[nodiscard]] uint64_t get_size() const { return size; }

  [[nodiscard]] uint64_t get_weight() const { return weight; }

  void insert(const uint64_t& index, const csr_graph& graph) {
    if (!solution_set[index]) {
      solution_set[index] = 1;
      ++size;
      weight += graph.get_weight(index);
    }
  }

  void remove(const uint64_t& index, const csr_graph& graph) {
    if (solution_set[index]) {
      solution_set[index] = 0;
      --size;
      weight -= graph.get_weight(index);
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

  void remove_solution_nodes(const std::unordered_set<uint64_t>& ids, const csr_graph& graph);

  void get_set_partners(std::unordered_set<uint64_t>& result,
    const uint64_t& curr, const csr_graph& graph) const;

  static uint64_t get_weight(const std::unordered_set<uint64_t>& set_nodes, const csr_graph& graph);

  void unwind(const csr_graph& graph, std::unordered_set<uint64_t>& set_nodes);

  void clear_changelog();
};

#endif
