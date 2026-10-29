#include "csr_graph.hpp"

#include <cstdint>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>

csr_graph::csr_graph(const std::string& file_name) {
  std::ifstream file(file_name);

  if (!file.is_open()) {
    std::cerr << "Error opening file " << file_name << std::endl;
    exit(-1);
  }

  std::string line;

  while (std::getline(file, line)) {
    if (!line.empty() && line[0] != '%' && line.find_first_not_of(" \t\r\n") != std::string::npos) {
      break;
    }
  }

  uint64_t i = 0, j = 0, w = 0;
  std::istringstream n_m_dec(line);
  if (!(n_m_dec >> i >> j)) {
    throw std::runtime_error("Invalid graph header: " + file_name);
  }
  n_m_dec >> w;
  this->n = i;
  this->m = j;
  if (w == 10) {
    weighted = true;
  }

  starts = std::vector<uint64_t>(n + 1, 0);
  neighbors.reserve(2 * m);
  weights = std::vector<uint64_t>(n);

  uint64_t curr_node = 0;

  while (curr_node < n && std::getline(file, line)) {
    std::istringstream iss(line);
    uint64_t value;
    bool start = true;
    starts[curr_node] = neighbors.size();
    while (iss >> value) {
      if (start) {
        start = false;
        weights[curr_node] = value;
      } else {
        if (value == 0 || value > n) {
          throw std::runtime_error("Invalid vertex id in graph: " + file_name);
        }
        neighbors.push_back(value - 1);
      }
    }
    std::sort(neighbors.begin() + static_cast<std::ptrdiff_t>(starts[curr_node]), neighbors.end());
    neighbors.erase(std::unique(neighbors.begin() + static_cast<std::ptrdiff_t>(starts[curr_node]), neighbors.end()),
                    neighbors.end());
    ++curr_node;
  }

  if (curr_node != n) {
    throw std::runtime_error("Graph contains fewer adjacency lines than declared: " + file_name);
  }
  starts[n] = neighbors.size();
  // The header is retained for diagnostics/output, but all access uses the
  // actual parsed CSR extent. This prevents malformed headers from causing an
  // out-of-bounds span.
  m = neighbors.size() / 2;
}

std::span<const uint64_t> csr_graph::get_neighbors(const uint64_t& id) const {
  return std::span{neighbors.data() + starts[id], starts[id + 1] - starts[id]};
}

void csr_graph::build_square() const {
  if (square_ready) {
    return;
  }

  square_starts.assign(n + 1, 0);
  square_neighbors.clear();

  std::vector<uint64_t> marks(n, 0);
  uint64_t mark = 0;
  for (uint64_t u = 0; u < n; ++u) {
    ++mark;
    square_starts[u] = square_neighbors.size();
    for (const uint64_t v : get_neighbors(u)) {
      if (v == u) {
        continue;
      }
      if (marks[v] != mark) {
        marks[v] = mark;
        square_neighbors.push_back(v);
      }
      for (const uint64_t w : get_neighbors(v)) {
        if (w != u && marks[w] != mark) {
          marks[w] = mark;
          square_neighbors.push_back(w);
        }
      }
    }
  }
  square_starts[n] = square_neighbors.size();
  square_ready = true;
}

std::span<const uint64_t> csr_graph::get_square_neighbors(const uint64_t& id) const {
  build_square();
  return std::span{square_neighbors.data() + square_starts[id],
                   square_starts[id + 1] - square_starts[id]};
}

void csr_graph::output_square_graph(const std::string& name) const {
  build_square();
  uint64_t total_entries = square_neighbors.size();
  uint64_t M2 = total_entries / 2;
  std::ofstream out(name + "_squared.graph");
  out << n << " " << M2 << (weighted ? " 10" : "") << "\n";

  for (uint64_t u = 0; u < n; ++u) {
    if (weighted) {
      out << get_weight(u) << " ";
    }
    for (const uint64_t v : get_square_neighbors(u)) {
      out << v + 1 << " ";
    }
    out << "\n";
  }
}
