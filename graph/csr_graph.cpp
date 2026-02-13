#include "csr_graph.hpp"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
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
        weights[curr_node] = value;
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
  const uint32_t N = n;

  // -------- PASS 1: compute degree of each vertex in square graph --------
  std::vector<uint32_t> degree(N, 0);

  std::unique_ptr<uint32_t[]> visited(new uint32_t[N]);
  std::fill(visited.get(), visited.get() + N, 0);

  uint32_t current_mark = 1;

  for (uint32_t u = 0; u < N; ++u) {
      std::cout << "\r" << u << " / " << n << std::flush;
    current_mark++;
    uint32_t local_degree = 0;

    uint64_t u_begin = (u == 0 ? 0 : starts[u - 1]);
    uint64_t u_end = (u == N - 1 ? 2 * m : starts[u]);

    for (uint64_t i = u_begin; i < u_end; ++i) {

      uint32_t v = neighbors[i];
      if (v == u) continue;

      if (visited[v] != current_mark) {
        visited[v] = current_mark;
        if (v > u) local_degree++;
      }

      // 2-hop neighbors
      uint64_t v_begin = (v == 0 ? 0 : starts[v - 1]);
      uint64_t v_end = (v == N - 1 ? 2 * m : starts[v]);

      for (uint64_t j = v_begin; j < v_end; ++j) {
        uint32_t w = neighbors[j];
        if (w == u) continue;

        if (visited[w] != current_mark) {
          visited[w] = current_mark;
          if (w > u) local_degree++;
        }
      }
    }

    degree[u] = local_degree;
  }

  std::cout << "\n";

  // -------- Build CSR prefix (correct forward prefix sum) --------
  std::vector<uint64_t> sq_starts(N);
  uint64_t total_forward_edges = 0;

  for (uint32_t u = 0; u < N; ++u) {
      std::cout << "\r" << u << " / " << n << std::flush;
    sq_starts[u] = total_forward_edges;
    total_forward_edges += degree[u];
  }

  std::cout << "\n";

  // Each forward edge represents one undirected edge
  uint64_t M2 = total_forward_edges;

  // Allocate symmetric adjacency (2 * M2 entries)
  std::vector<uint32_t> sq_neighbors(2 * M2);

  // Reset insertion counters
  std::fill(degree.begin(), degree.end(), 0);
  std::fill(visited.get(), visited.get() + N, 0);
  current_mark = 1;

  // -------- PASS 2: fill symmetric adjacency --------
  for (uint32_t u = 0; u < N; ++u) {
      std::cout << "\r" << u << " / " << n << std::flush;
    current_mark++;

    uint64_t u_begin = (u == 0 ? 0 : starts[u - 1]);
    uint64_t u_end = (u == N - 1 ? 2 * m : starts[u]);

    for (uint64_t i = u_begin; i < u_end; ++i) {
      uint32_t v = neighbors[i];
      if (v == u) continue;

      if (visited[v] != current_mark) {
        visited[v] = current_mark;

        if (v > u) {
          uint64_t pos_u = sq_starts[u] + degree[u]++;
          uint64_t pos_v = sq_starts[v] + degree[v]++;

          sq_neighbors[pos_u] = v;
          sq_neighbors[pos_v] = u;
        }
      }

      uint64_t v_begin = (v == 0 ? 0 : starts[v - 1]);
      uint64_t v_end = (v == N - 1 ? 2 * m : starts[v]);

      for (uint64_t j = v_begin; j < v_end; ++j) {
        uint32_t w = neighbors[j];
        if (w == u) continue;

        if (visited[w] != current_mark) {
          visited[w] = current_mark;

          if (w > u) {
            uint64_t pos_u = sq_starts[u] + degree[u]++;
            uint64_t pos_w = sq_starts[w] + degree[w]++;

            sq_neighbors[pos_u] = w;
            sq_neighbors[pos_w] = u;
          }
        }
      }
    }
  }

  std::cout << "\n";

  // -------- Output in METIS format --------
  std::ofstream out(name + "_squared.graph");

  out << N << " " << M2 << "\n";

  for (uint32_t u = 0; u < N; ++u) {
      std::cout << "\r" << u << " / " << n << std::flush;
    uint64_t begin = sq_starts[u];
    uint64_t end = begin + degree[u];

    for (uint64_t i = begin; i < end; ++i) {
      out << (sq_neighbors[i] + 1) << " "; // 1-based indexing
    }

    out << "\n";
  }
  std::cout << "\n";

  out.close();
}
