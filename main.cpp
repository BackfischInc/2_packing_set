#include <iostream>

#include "packing_set/packing_set.hpp"
#include "graph/csr_graph.hpp"
#include "local_search/iterated_ls.hpp"

int main(const int argc, char* argv[]) {

  for (int i = 1; i < argc; ++i) {
    const std::string file_name = argv[i];

    std::cout << "===========" << std::endl;

    std::cout << file_name << std::endl;
    std::cout << "parsing the graph...";
    const csr_graph graph(file_name);
    std::cout << "[DONE]" << std::endl;

    std::cout << "===========" << std::endl;

    packing_set solution_set(graph.amount_nodes());
    constexpr bool weighted = true;

    iterated_local_search(solution_set, graph, weighted);
  }

  return 0;
}
