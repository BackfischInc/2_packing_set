#include "csr_graph.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

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

  int i, j, w;
  std::istringstream n_m_dec(line);
  n_m_dec >> i >> j >> w;
  this->n = i;
  this->m = j;
  if (w == 10) {
    weighted = true;
  }

  starts = std::vector<int>(n - 1);
  neighbors = std::vector<int>(2 * m);
  weights = std::vector<int>(n);

  int curr_node = 0;

  int max_degree = 0;

  while (std::getline(file, line)) {
    std::vector<int> numbers;

    std::istringstream iss(line);
    int value;
    bool start = true;
    while (iss >> value) {
      if (start) {
        start = false;
        weights[curr_node]= value;
      } else {
        numbers.push_back(value - 1);
      }
    }

    int deg = static_cast<int>(numbers.size());
    if (deg > max_degree) {
      max_degree = deg;
    }

    if (curr_node != n - 1) {
      starts[curr_node] = curr_node == 0 ? deg : deg + starts[curr_node - 1];
    }

    for (int it1 = 0; it1 < deg; it1++) {
      const int start_idx = curr_node == 0 ? 0 : starts[curr_node - 1];
      neighbors[start_idx + it1] = numbers[it1];
    }

    ++curr_node;
  }

  max_deg = max_degree;
}

std::span<const int> csr_graph::get_neighbors(const int& id) const {
  const size_t deg =
      id == 0
        ? starts[id]
        : id == n - 1
            ? 2 * m - starts[id - 1]
            : starts[id] - starts[id - 1];

  const int start_idx = id == 0 ? 0 : starts[id - 1];

  return std::span{neighbors.begin() + start_idx, deg};
}
