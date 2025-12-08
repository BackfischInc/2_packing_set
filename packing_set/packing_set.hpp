#ifndef PACKING_SET_HPP
#define PACKING_SET_HPP

#include <cstdint>
#include <vector>
#include <cstring>
#include <span>

#include "../graph/csr_graph.hpp"

class packing_set {
  std::vector<uint8_t> solution_set;
  int size = 0;

public:
  explicit packing_set(const int& n) : solution_set(n, 0) {}

  packing_set(const packing_set&) = default;

  packing_set& operator=(const packing_set& other) {
    if (this != &other) {
      size = other.size;
      std::memcpy(solution_set.data(), other.solution_set.data(), solution_set.size());
    }
    return *this;
  }

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

  void add_solution_node(const int& id, const std::span<const int>& neighbors, std::vector<int>& set_neighbors);

  void remove_solution_node(const int& id, const csr_graph& graph, std::vector<int>& set_neighbors);

  void remove_solution_nodes(const std::vector<int>& ids, const csr_graph& graph, std::vector<int>& set_neighbors);
};

#endif
