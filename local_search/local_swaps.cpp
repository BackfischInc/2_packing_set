#include "local_swaps.hpp"

#include <iostream>
#include <map>

void execute_2_1_swap(const csr_graph& graph, packing_set& solution_set,
                      std::vector<int>& set_neighbors, const bool& weighted) {
  bool found_swap = true;
  int i = 0;

  while (found_swap && i < 10) {
    ++i;

    std::map<int, std::vector<int>> buckets;
    fill_buckets(buckets, graph, set_neighbors);
    found_swap = execute_swap(buckets, graph, solution_set, set_neighbors, weighted);
  }
}

void fill_buckets(std::map<int, std::vector<int>>& buckets, const csr_graph& graph,
                  const std::vector<int>& set_neighbors) {
  for (int i = 0; i < graph.amount_nodes(); ++i) {
    if (set_neighbors[i] == i) {
      continue;
    }

    int curr_neigh = set_neighbors[i];
    bool put_in_bucket = true;

    for (const int& neighbor: graph.get_neighbors(i)) {
      const int& neighbor_neigh = set_neighbors[neighbor];

      if (neighbor_neigh != -1) {
        if (curr_neigh == -1) {
          curr_neigh = neighbor_neigh;
        } else {
          if (curr_neigh != neighbor_neigh) {
            put_in_bucket = false;
            break;
          }
        }
      }
    }

    if (curr_neigh != -1 && put_in_bucket) {
      buckets[curr_neigh].push_back(i);
    }
  }
}

bool execute_swap(std::map<int, std::vector<int>>& buckets, const csr_graph& graph, packing_set& solution_set,
                  std::vector<int>& set_neighbors, const bool& weighted) {
  for (const auto& [idx , bucket]: buckets) {

    for (int i = 0; i < bucket.size(); ++i) {
      for (int j = i + 1; j < bucket.size(); ++j) {
        const int v = bucket[i];
        const int w = bucket[j];

        if (weighted) {
          const bool do_swap = graph.get_weight(v) + graph.get_weight(w) > graph.get_weight(idx);
          if (!do_swap) {
            continue;
          }
        }

        const auto& neighbors_v = graph.get_neighbors(v);
        const auto& neighbors_w = graph.get_neighbors(w);

        if (!are_neighbors(graph, v, w) && !have_common_neighbor(neighbors_v, neighbors_w)) {
          solution_set.remove_solution_node(idx, graph, set_neighbors);
          solution_set.add_solution_node(v, neighbors_v, set_neighbors);
          solution_set.add_solution_node(w, neighbors_w, set_neighbors);

          return true;
        }
      }
    }
  }

  return false;
}

bool are_neighbors(const csr_graph& graph, const int& v, const  int& w) {
  for (const int& neighbor: graph.get_neighbors(w)) {
    if (neighbor == v) {
      return true;
    }
    if (neighbor > v) {
      return false;
    }
  }

  return false;
}

bool have_common_neighbor(const std::span<const int>& a, const std::span<const int>& b) {
  int i = 0, j = 0;

  while (i < a.size() && j < b.size()) {
    if (a[i] == b[j]) {
      return true;
    }

    if (a[i] < b[j]) {
      i++;
    } else {
      j++;
    }
  }
  return false;
}
