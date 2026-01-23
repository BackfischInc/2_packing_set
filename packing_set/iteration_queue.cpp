#include "iteration_queue.hpp"

bool iteration_queue::add_node(const uint64_t& index) {
  if (amount_nodes >= max_size) {
    return false;
  }

  if (indicator[index] != iteration) {
    indicator[index] = iteration + 1;
    next_indices.push_back(index);

    ++amount_nodes;
  }
  return true;
}

std::span<const uint64_t> iteration_queue::curr_nodes() const {
  return curr_indices;
}

void iteration_queue::next_iter() {
  curr_indices.swap(next_indices);
  next_indices.clear();

  amount_nodes = 0;
  ++iteration;
}
