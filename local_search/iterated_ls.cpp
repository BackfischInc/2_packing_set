#include "iterated_ls.hpp"
#include "local_swaps.hpp"

#include <iostream>
#include <random>
#include <thread>
#include <mutex>

std::mutex mtx;

void execute_iteration(packing_set& solution_set, const csr_graph& graph, const bool weighted,
                       const int iteration_amount) {
  for (int i = 0; i < iteration_amount; ++i) {
    packing_set working_set(solution_set);

    perturb_solution(graph, working_set);
    update_if_better(graph, solution_set, working_set, weighted);

    maximize_solution(graph, working_set, weighted);
    update_if_better(graph, solution_set, working_set, weighted);
  }
}

void iterated_local_search(packing_set& solution_set, const csr_graph& graph, const bool& weighted) {
  constexpr int iteration_amount = 200;

  maximize_solution(graph, solution_set, weighted);

  std::thread t1(execute_iteration, std::ref(solution_set), std::cref(graph), weighted, iteration_amount);
  std::thread t2(execute_iteration, std::ref(solution_set), std::cref(graph), weighted, iteration_amount);
  std::thread t3(execute_iteration, std::ref(solution_set), std::cref(graph), weighted, iteration_amount);
  std::thread t4(execute_iteration, std::ref(solution_set), std::cref(graph), weighted, iteration_amount);

  t1.join();
  t2.join();
  t3.join();
  t4.join();
}

void perturb_solution(const csr_graph& graph, packing_set& solution_set) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint64_t> int_dist(0, graph.amount_nodes() - 1);
  std::uniform_real_distribution real_dist(0.01, 0.2);

  uint64_t curr = int_dist(gen);

  const int amount_iterations = static_cast<int>(static_cast<double>(graph.amount_nodes()) * real_dist(gen));
  for (int i = 0; i < amount_iterations; ++i) {
    if (solution_set.get_value(curr)) {
      solution_set.remove_solution_node(curr, graph);
    } else {
      std::set<uint64_t> set_nodes = solution_set.get_set_partners(curr, graph.get_neighbors(curr));

      solution_set.remove_solution_nodes(set_nodes, graph);
      solution_set.add_solution_node(curr, graph);
    }

    for (int j = 0; j < 3; ++j) {
      const auto& neighbors = graph.get_neighbors(curr);
      curr = neighbors[int_dist(gen) % neighbors.size()];
    }
  }
}

void update_if_better(const csr_graph& graph, packing_set& best_solution,
                      const packing_set& working_set, const bool& weighted) {
  mtx.lock();
  if (!weighted) {
    if (working_set.get_size() > best_solution.get_size()) {
      best_solution.~packing_set();
      new(&best_solution) packing_set(working_set);
     //  std::cout << "[" << std::this_thread::get_id() << "] new best: " << working_set.get_size() << std::endl;
    }
  } else {
    if (working_set.get_weight(graph) > best_solution.get_weight(graph)) {
      best_solution.~packing_set();
      new(&best_solution) packing_set(working_set);
     // std::cout << "[" << std::this_thread::get_id() << "] new best: " << working_set.get_weight(graph) << std::endl;
    }
  }
  mtx.unlock();
}
