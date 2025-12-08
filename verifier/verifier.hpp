#ifndef VERIFIER_HPP
#define VERIFIER_HPP

#include "../graph/csr_graph.hpp"
#include "../packing_set/packing_set.hpp"

bool is_valid(const csr_graph& graph, const packing_set& solution);

#endif