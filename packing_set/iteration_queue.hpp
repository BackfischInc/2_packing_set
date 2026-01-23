//
// Created by Marie on 23.01.2026.
//

#ifndef ITERATION_QUEUE_HPP
#define ITERATION_QUEUE_HPP
#include <cstdint>
#include <span>
#include <vector>

class iteration_queue {
  std::vector<uint64_t> indicator;

  std::vector<uint64_t> curr_indices;
  std::vector<uint64_t> next_indices;

  uint64_t amount_nodes = 0;
  uint64_t iteration = 1;
  uint64_t max_size;

public:
  explicit iteration_queue(const uint64_t& size, const uint64_t& max_size):
    indicator(size), max_size(max_size) {}

  bool add_node(const uint64_t& index);

  [[nodiscard]] std::span<const uint64_t> curr_nodes() const;

  void next_iter();

  [[nodiscard]] uint64_t size() const {
    return amount_nodes;
  }
};

#endif //ITERATION_QUEUE_HPP
