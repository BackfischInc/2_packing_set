#include "csr_graph.hpp"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>

csr_graph::csr_graph(const std::string& file_name) {
  std::ifstream file(file_name);

  if (!file.is_open()) {
    std::cerr << "Error opening file " << file_name << std::endl;
    exit(-1);
  }

  std::string line;

  while (std::getline(file, line)) {
    if (line[0] != '%') {
      break;
    }
  }

  uint64_t i, j, w;
  std::istringstream n_m_dec(line);
  n_m_dec >> i >> j >> w;
  this->n = i;
  this->m = j;
  if (w == 10) {
    weighted = true;
  }

  starts = std::vector<uint64_t>(n - 1);
  neighbors = std::vector<uint64_t>(2 * m);
  weights = std::vector<uint64_t>(n);

  uint64_t curr_node = 0;

  while (std::getline(file, line)) {
    std::vector<uint64_t> numbers;

    std::istringstream iss(line);
    uint64_t value;
    bool start = true;
    while (iss >> value) {
      if (start) {
        start = false;
        weights[curr_node]= value;
      } else {
        numbers.push_back(value - 1);
      }
    }

    uint64_t deg = numbers.size();

    if (curr_node != n - 1) {
      starts[curr_node] = curr_node == 0 ? deg : deg + starts[curr_node - 1];
    }

    for (uint64_t it1 = 0; it1 < deg; it1++) {
      const uint64_t start_idx = curr_node == 0 ? 0 : starts[curr_node - 1];
      neighbors[start_idx + it1] = numbers[it1];
    }

    ++curr_node;
  }
}

std::span<const uint64_t> csr_graph::get_neighbors(const uint64_t& id) const {
  const size_t deg =
      id == 0
        ? starts[id]
        : id == n - 1
            ? 2 * m - starts[id - 1]
            : starts[id] - starts[id - 1];

  const uint64_t start_idx = id == 0 ? 0 : starts[id - 1];

  return std::span{neighbors.begin() + static_cast<long long>(start_idx), deg};
}

void csr_graph::output_square_graph(const std::string& name) const {
  std::vector<std::unordered_set<uint64_t>> square_graph(n);
  uint64_t edge_count = 0;

  for (int i = 0; i < n; ++i) {
    std::span<const uint64_t> one_neighbors = get_neighbors(i);

    for (const uint64_t& neighbor: one_neighbors) {
      square_graph[i].insert(neighbor + 1);

      for (const uint64_t& two_neighbor: get_neighbors(neighbor)) {
        square_graph[i].insert(two_neighbor + 1);
      }
    }

    edge_count += square_graph[i].size();
  }

  std::ofstream outfile;
  outfile.open(name + "_squared.graph");

  outfile << n << " " << edge_count << (weighted ? " 10" : " 0") << std::endl;

  for (int i = 0; i < n; ++i) {
    if (weighted) {
      outfile << weights[i] << " ";
    }
    for (const uint64_t& neighbor: square_graph[i]) {
      outfile << neighbor << " ";
    }
  }

  outfile.close();
}
