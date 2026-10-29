#ifndef LOCAL_SWAPS_HPP
#define LOCAL_SWAPS_HPP

#include <algorithm>
#include <vector>

#include "../graph/csr_graph.hpp"
#include "../packing_set/packing_set.hpp"

class iteration_queue;

struct swap_workspace {
  std::vector<uint64_t> candidates;
  std::vector<uint64_t> marks;
  uint64_t generation = 1;

  explicit swap_workspace(const uint64_t n) : marks(n, 0) {
    candidates.reserve(64);
  }

  void begin() {
    ++generation;
    if (generation == 0) {
      std::fill(marks.begin(), marks.end(), 0);
      generation = 1;
    }
    candidates.clear();
  }

  bool contains(const uint64_t id) const {
    return marks[id] == generation;
  }

  void add(const uint64_t id) {
    if (!contains(id)) {
      marks[id] = generation;
      candidates.push_back(id);
    }
  }
};

void maximize_solution(const csr_graph& graph, packing_set& solution_set,
                       std::vector<uint64_t>& set_nodes, iteration_queue& queue,
                       const bool weighted, const bool do_2_1_swaps,
                       swap_workspace& workspace);

bool try_auto_include(const uint64_t& curr, const csr_graph& graph, packing_set& solution_set,
                      std::vector<uint64_t>& set_nodes, iteration_queue& queue,
                      const bool& weighted);

bool find_2_1_swap(const uint64_t& curr, const csr_graph& graph, packing_set& solution_set,
                   iteration_queue& queue, const bool weighted, swap_workspace& workspace);

bool try_swap(const uint64_t& node, const uint64_t& curr, std::span<const uint64_t> pot_neighbors,
              const csr_graph& graph, packing_set& solution_set,
              iteration_queue& queue, const bool& weighted);

bool has_potential(const uint64_t& node, const uint64_t& curr, const csr_graph& graph, const packing_set& set);

bool are_neighbors(const csr_graph& graph, const uint64_t& v, const uint64_t& w);

bool have_common_neighbor(const std::span<const uint64_t>& a, const std::span<const uint64_t>& b);

bool add_neighborhood(const uint64_t& node, iteration_queue& queue, const csr_graph& graph);

#endif //LOCAL_SWAPS_HPP
