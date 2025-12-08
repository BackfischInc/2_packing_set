#ifndef CSR_GRAPH_HPP
#define CSR_GRAPH_HPP

#include <span>
#include <string>
#include <vector>

class csr_graph {
  std::vector<int> starts;
  std::vector<int> neighbors;
  std::vector<int> weights;
  int n, m;
  bool weighted = false;

public:
  int max_deg;

  explicit csr_graph(const std::string& file_name);

  [[nodiscard]] std::span<const int> get_neighbors(const int& id) const;

  [[nodiscard]] const int& amount_nodes() const { return n; }

  [[nodiscard]] const bool& is_weighted() const { return weighted; }

  [[nodiscard]] const int& get_weight(const int& id) const { return weights[id]; }
};

#endif
