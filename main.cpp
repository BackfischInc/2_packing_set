#include <iostream>

#include "packing_set/packing_set.hpp"
#include "graph/csr_graph.hpp"
#include "local_search/iterated_ls.hpp"
#include "verifier/verifier.hpp"

bool cmdOptionExists(const int& argc, char** argv, const std::string& option) {
  for (int i = 1; i < argc; ++i) {
    if (argv[i] == option) {
      return true;
    }
  }
  return false;
}

std::string getCmdOption(const int& argc, char** argv, const std::string& option) {
  if (cmdOptionExists(argc, argv, option)) {
    for (int i = 1; i < argc; ++i) {
      if (argv[i] == option) {
        return argv[i + 1];
      }
    }
  }

  return "";
}

std::vector<std::string> get_input_files(const int& argc, char** argv) {
  std::vector<std::string> names;

  for (int i = 1; i < argc; ++i) {
    if (std::string(argv[i]) == "-i") {
      for (int j = i + 1; j < argc; ++j) {
        std::string arg = argv[j];
        if (!arg.empty() && arg[0] == '-') {
          break;
        }
        names.push_back(arg);
      }
      break;
    }
  }

  return names;
}

void print_help_options() {
  std::cout << "-i [file name 1] [file name 2] [...]" << std::endl;
  std::cout << "Input the names of all files to be worked on" << std::endl;
  std::cout << "===========" << std::endl;
  std::cout << "-weighted" << std::endl;
  std::cout << "executes a weighted search for all given graphs" << std::endl;
  std::cout << "===========" << std::endl;
  std::cout << "-verbose" << std::endl;
  std::cout << "prints out updates to the solution in every iteration with an improvement" << std::endl;
  std::cout << "===========" << std::endl;
  std::cout << "-disable_2_1" << std::endl;
  std::cout << "if given will not look for and execute 2-1 swaps" << std::endl;
  std::cout << "===========" << std::endl;
  std::cout << "-max_size [size]" << std::endl;
  std::cout << "maximum amount of nodes at the start of each iteration" << std::endl;
  std::cout << "===========" << std::endl;
  std::cout << "-time_limit [size]" << std::endl;
  std::cout << "time limit for the execution time in minutes" << std::endl;
  std::cout << "===========" << std::endl;
  std::cout << "-output_square" << std::endl;
  std::cout << "create and output a file containing the square graph" << std::endl;
  std::cout << "===========" << std::endl;
}

int main(const int argc, char* argv[]) {
  if (cmdOptionExists(argc, argv, "-h")) {
    print_help_options();
  }

  const std::vector<std::string> file_names = get_input_files(argc, argv);
  if (file_names.empty()) {
    std::cout << "No input files found" << std::endl;
    return 1;
  }

  const bool weighted = cmdOptionExists(argc, argv, "-weighted");
  const bool verbose = cmdOptionExists(argc, argv, "-verbose");
  const bool disable_2_1 = cmdOptionExists(argc, argv, "-disable_2_1");

  int queue_size = 100;
  const std::string max_size = getCmdOption(argc, argv, "-max_size");
  if (!max_size.empty()) {
    queue_size = std::stoi(max_size);
  }

  int time_limit = 60;
  const std::string limit = getCmdOption(argc, argv, "-time_limit");
  if (!limit.empty()) {
    time_limit = std::stoi(limit);
  }

  const parameters args(weighted, verbose, disable_2_1, queue_size, time_limit);

  for (const auto& file_name: file_names) {
    std::cout << "===========" << std::endl;

    std::cout << file_name << std::endl;
    std::cout << "parsing the graph...";
    const csr_graph graph(file_name);
    std::cout << "[DONE]" << std::endl;

    if (cmdOptionExists(argc, argv, "-output_square")) {
      std::cout << "===========" << std::endl;
      std::cout << "creating the square graph..." << std::endl;
      graph.output_square_graph(file_name);
      std::cout << "[DONE]" << std::endl;
    }

    std::cout << "===========" << std::endl;

    packing_set solution_set(graph.amount_nodes());
    iterated_local_search(solution_set, graph, args);

    std::cout << "===========" << std::endl;

    const uint64_t solution_size = weighted ? solution_set.get_weight() : solution_set.get_size();
    std::cout << "final solution size: " << solution_size << std::endl;
    if (is_valid(graph, solution_set)) {
      std::cout << "valid solution" << std::endl;
    } else {
      std::cout << "invalid solution" << std::endl;
    }
  }

  return 0;
}
