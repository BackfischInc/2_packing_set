#include "packing_set.hpp"

#include <algorithm>
#include <iostream>
#include <unordered_set>

void packing_set::begin_transaction() {
  ++transaction_generation;
  if (transaction_generation == 0) {
    std::fill(logged_generation.begin(), logged_generation.end(), 0);
    transaction_generation = 1;
  }
  saved_size = size;
  saved_weight = weight;
  changelog.clear();
}

void packing_set::record(const uint64_t index) {
  if (logged_generation[index] != transaction_generation) {
    logged_generation[index] = transaction_generation;
    changelog.push_back({index, solution_set[index], set_neighbors[index]});
  }
}

void packing_set::add_solution_node(const uint64_t& id, const csr_graph& graph) {
  insert(id, graph);
  record(id);
  set_neighbors[id] = id;

  for (const auto& neighbor: graph.get_neighbors(id)) {
    record(neighbor);
    remove(neighbor, graph);
    set_neighbors[neighbor] = id;
  }
}

void packing_set::remove_solution_node(const uint64_t& id, const csr_graph& graph) {
  record(id);
  remove(id, graph);
  set_neighbors[id] = -1;

  for (const auto& neighbor: graph.get_neighbors(id)) {
    record(neighbor);
    remove(neighbor, graph);
    set_neighbors[neighbor] = -1;
  }
}

void packing_set::remove_solution_nodes(std::span<const uint64_t> ids, const csr_graph& graph) {
  for (const uint64_t id : ids) {
    remove_solution_node(id, graph);
  }
}

void packing_set::remove_solution_nodes(const std::unordered_set<uint64_t>& ids, const csr_graph& graph) {
  for (const uint64_t id : ids) {
    remove_solution_node(id, graph);
  }
}

void packing_set::get_set_partners(std::vector<uint64_t>& result,
                                   const uint64_t& curr, const csr_graph& graph) const {
  result.clear();
  ++partner_generation;
  if (partner_generation == 0) {
    std::fill(partner_marks.begin(), partner_marks.end(), 0);
    partner_generation = 1;
  }

  const auto add_partner = [&](const uint64_t partner) {
    if (partner_marks[partner] != partner_generation) {
      partner_marks[partner] = partner_generation;
      result.push_back(partner);
    }
  };

  const uint64_t curr_partner = get_neighbor(curr);
  if (curr_partner != static_cast<uint64_t>(-1)) {
    add_partner(curr_partner);
  }
  for (const uint64_t& node: graph.get_neighbors(curr)) {
    const uint64_t partner = get_neighbor(node);
    if (partner != static_cast<uint64_t>(-1)) {
      add_partner(partner);
    }
  }
}

uint64_t packing_set::get_weight(std::span<const uint64_t> set, const csr_graph& graph) {
  uint64_t result = 0;

  for (const uint64_t& node: set) {
    result += graph.get_weight(node);
  }

  return result;
}

uint64_t packing_set::get_weight(const std::unordered_set<uint64_t>& set, const csr_graph& graph) {
  uint64_t result = 0;
  for (const uint64_t node : set) {
    result += graph.get_weight(node);
  }
  return result;
}

// O(n * deg²)
void packing_set::unwind(const csr_graph& graph, std::vector<uint64_t>& set_nodes) {
  (void)graph;
  (void)set_nodes;
  for (auto it = changelog.rbegin(); it != changelog.rend(); ++it) {
    solution_set[it->id] = it->solution;
    set_neighbors[it->id] = it->partner;
  }
  size = saved_size;
  weight = saved_weight;
  changelog.clear();
}

void packing_set::clear_changelog() {
  changelog.clear();
}
