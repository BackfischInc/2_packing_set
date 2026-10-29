#include "iteration_queue.hpp"

bool iteration_queue::add_node(const uint64_t& index) {
  if (amount_nodes >= max_size) {
    return false;
  }

  // Nodes are marked with the generation of next_indices.  The old code
  // compared against `iteration` but stored `iteration + 1`, which meant
  // that every repeated insertion in the same generation was accepted.
  const uint64_t next_generation = iteration + 1;
  if (indicator[index] != next_generation) {
    indicator[index] = next_generation;
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
