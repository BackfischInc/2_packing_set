cmake_minimum_required(VERSION 3.30)
project(2_Packing_Set)

set(CMAKE_CXX_STANDARD 20)

add_executable(2_Packing_Set
        main.cpp
        graph/csr_graph.hpp
        graph/csr_graph.cpp
        verifier/verifier.hpp
        verifier/verifier.cpp
        packing_set/packing_set.hpp
        packing_set/packing_set.cpp
        local_search/local_search.cpp
        local_search/local_search.hpp
        local_search/iterated_ls.cpp
        local_search/iterated_ls.hpp
        local_search/local_swaps.cpp
        local_search/local_swaps.hpp
        temperature/simulated_annealing.hpp
        temperature/simulated_annealing.cpp
)
