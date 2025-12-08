#include "verifier.hpp"

#include <iostream>
#include <ostream>

#include "../graph/csr_graph.hpp"
#include "../packing_set/packing_set.hpp"

bool is_valid(const csr_graph& graph, const packing_set& solution) {
  for (int i = 0; i < graph.amount_nodes(); ++i) {
    const auto neighbors = graph.get_neighbors(i);

    // nodes in the set may not also have neighbors in the set
    if (solution.get_value(i)) {
      for (const auto& neighbor: neighbors) {
        if (solution.get_value(neighbor)) {
          std::cout << i << " " << neighbor << std::endl;
          return false;
        }
      }
    }

    // nodes not in the set may only have at most one neighbor in the set
    else {
      bool found_set_neighbor = false;
      for (const auto& neighbor: neighbors) {
        if (solution.get_value(neighbor)) {
          if (found_set_neighbor) {
            std::cout << i << std::endl;
            return false;
          }
          found_set_neighbor = true;
        }
      }
    }
  }

  return true;
}
