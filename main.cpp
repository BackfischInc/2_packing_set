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
    "cactus50-degree.graph",    // 17
    "cactus100-degree.graph",   // 31
    "cactus150-degree.graph",   // 49
    "cactus200-degree.graph",   // 65
    "cactus250-degree.graph",   // 82
    "cactus300-degree.graph",   // 100
    "cactus350-degree.graph",   // 116
    "cactus400-degree.graph",   // 133
    "cactus450-degree.graph",   // 148
    "cactus500-degree.graph",   // 166
    "cactus550-degree.graph",   // 179
    "cactus600-degree.graph",   // 199
    "cactus650-degree.graph",   // 214
    "cactus700-degree.graph",   // 232
    "cactus750-degree.graph",   // 250
    "cactus800-degree.graph",   // 264
    "cactus850-degree.graph",   // 282
    "cactus900-degree.graph",   // 300
    "cactus950-degree.graph",   // 315
    "cactus1000-degree.graph",  // 332
  };

  std::cout << "===========" << std::endl;
  for (const std::string& file_name: file_names) {
    std::cout << file_name << std::endl;
    // std::cout << "parsing the graph...";
    const csr_graph graph(path + file_name);
    // std::cout << "[DONE]" << std::endl;

    packing_set solution_set(graph.amount_nodes());
    constexpr bool weighted = false;

    iterated_local_search(solution_set, graph, weighted);
    // std::cout << "====" << std::endl;

    std::cout << "final solution size: " <<
        (weighted
           ? solution_set.get_weight(graph)
           : solution_set.get_size())
        << std::endl;
    std::cout << (is_valid(graph, solution_set) ? "valid" : "invalid") << " solution" << std::endl;
    std::cout << "===========" << std::endl;
  }

  return 0;
}
