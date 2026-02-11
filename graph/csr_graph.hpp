#ifndef CSR_GRAPH_HPP
#define CSR_GRAPH_HPP

#include <cstdint>
#include <span>
#include <string>
#include <vector>

class csr_graph {
  std::vector<uint64_t> starts;
  std::vector<uint64_t> neighbors;
  std::vector<uint64_t> weights;
  uint64_t n, m;
  bool weighted = false;

public:
  explicit csr_graph(const std::string& file_name);

  [[nodiscard]] std::span<const uint64_t> get_neighbors(const uint64_t& id) const;

  [[nodiscard]] const uint64_t& amount_nodes() const { return n; }

  [[nodiscard]] const bool& is_weighted() const { return weighted; }

  [[nodiscard]] const uint64_t& get_weight(const uint64_t& id) const { return weights[id]; }

  void output_square_graph(const std::string& name) const;
};

#endif
