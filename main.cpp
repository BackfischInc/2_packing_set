#include <iostream>

#include "packing_set/packing_set.hpp"
#include "graph/csr_graph.hpp"
#include "local_search/iterated_ls.hpp"
#include "verifier/verifier.hpp"
#include "vector"

int main() {
  std::cout << "Hello, World! Du Hurensohn" << std::endl;

  const std::string path = "../../../../w2pack_all/";

  const std::vector<std::string> file_names = {
    "snap_com-amazon.graph"
    /*"mesh_buddha-uniform.graph",
    "road_central-geometric.graph",
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
    std::cout << "parsing the graph...";
    const csr_graph graph(path + file_name);
    std::cout << "[DONE]\n" << std::endl;

    packing_set solution_set(graph.amount_nodes());
    constexpr bool weighted = true;

    iterated_local_search(solution_set, graph, weighted);
    std::cout << "====" << std::endl;

    std::cout << "final Solution Size: " <<
        (weighted
           ? solution_set.get_weight(graph)
           : solution_set.get_size())
        << std::endl;
    std::cout << (is_valid(graph, solution_set) ? "valid" : "invalid") << " solution" << std::endl;
  }

  return 0;
}
