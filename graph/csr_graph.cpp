#include "csr_graph.hpp"

#include <cstdint>
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
  std::ofstream out(name + "_squared.graph");

  // We'll count edges while generating
  uint64_t total_edges = 0;

  // Temporary visitation array
  std::vector<uint32_t> visited(n, 0);
  uint32_t current_mark = 1;

  // First pass: count edges
  for (uint64_t u = 0; u < n; ++u) {
    std::cout << u << std::endl;
    current_mark++;

    // 1-hop
    for (uint64_t i = u == 0 ? 0 : starts[u - 1]; i < (u == n - 1 ? 2 * m : starts[u]); ++i) {
      uint32_t v = neighbors[i];
      if (v == u) continue;
      if (visited[v] != current_mark) {
        visited[v] = current_mark;
        total_edges++;
      }

      // 2-hop
      for (uint64_t j = v == 0 ? 0 : starts[v - 1]; j < (v == n - 1 ? 2 * m : starts[v]); ++j) {
        uint32_t w = neighbors[j];
        if (w == u) continue;
        if (visited[w] != current_mark) {
          visited[w] = current_mark;
          total_edges++;
        }
      }
    }
  }

  total_edges /= 2; // undirected graph

  // METIS header
  out << n << " " << total_edges << "\n";

  // Second pass: write adjacency
  std::fill(visited.begin(), visited.end(), 0);
  current_mark = 1;

  for (uint32_t u = 0; u < n; ++u) {
    current_mark++;

    for (uint32_t i = u == 0 ? 0 : starts[u - 1]; i < u == n - 1 ? 2 * m : starts[u]; ++i) {
      uint32_t v = neighbors[i];
      if (v == u) continue;

      if (visited[v] != current_mark) {
        visited[v] = current_mark;
        out << (v + 1) << " ";
      }

      for (uint32_t j = v == 0 ? 0 : starts[v - 1]; j < v == n - 1 ? 2 * m : starts[v]; ++j) {
        uint32_t w = neighbors[j];
        if (w == u) continue;

        if (visited[w] != current_mark) {
          visited[w] = current_mark;
          out << w + 1 << " ";
        }
      }
    }

    out << "\n";
  }
}
