#include <iostream>

#include "packing_set/packing_set.hpp"
#include "graph/csr_graph.hpp"
#include "local_search/iterated_ls.hpp"
#include "verifier/verifier.hpp"
#include "vector"
#include "local_search/local_search.hpp"
#include "local_search/local_swaps.hpp"

int main() {
  std::cout << "Hello, World! Du Hurensohn" << std::endl;

  const std::string path = "../../../../w2pack_all/";

  const std::vector<std::string> file_names = {
    "mesh_buddha-uniform.graph",
    /*"road_central-geometric.graph",
      "road_central-hybrid.graph",
      "road_central-unit.graph",
      "road_central-uniform.graph",
      "road_usa-degree.graph",
      "road_usa-geometric.graph",
      "road_usa-hybrid.graph",
      "road_usa-unit.graph",
    "road_usa-uniform.graph" */
  };

  std::cout << "===========" << std::endl;
  for (const std::string& file_name: file_names) {
    std::cout << file_name << std::endl;
    std::cout << "\tparsing the graph...";
    const csr_graph graph(path + file_name);
    std::cout << "[DONE]" << std::endl;

    packing_set solution_set(graph.amount_nodes());
    constexpr bool weighted = true;

    std::cout << "\texecuting local search..." << std::endl;
    iterated_local_search(solution_set, graph, weighted);

    std::cout << "\tfound solution of size: " <<
      (weighted ? solution_set.get_weight(graph) : solution_set.get_size()) << std::endl;
    std::cout << "\t" << (is_valid(graph, solution_set) ? "Valid" : "Invalid") << " Solution" << std::endl << std::endl;

    std::cout << "\texecuting 2-1 swaps...";
    execute_2_1_swap(graph, solution_set, weighted);

    std::cout << "\tFinal Solution Size: " <<
        (weighted
           ? solution_set.get_weight(graph)
           : solution_set.get_size())
        << std::endl;
    std::cout << "\t" << (is_valid(graph, solution_set) ? "Valid" : "Invalid") << " Solution" << std::endl;
  }

  return 0;
}
