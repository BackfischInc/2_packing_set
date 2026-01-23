cmake_minimum_required(VERSION 3.28)
project(2_Packing_Set)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O3")

add_executable(2_Packing_Set
        main.cpp
        graph/csr_graph.hpp
        graph/csr_graph.cpp
        packing_set/packing_set.hpp
        packing_set/packing_set.cpp
        packing_set/iteration_queue.cpp
        packing_set/iteration_queue.hpp
        local_search/iterated_ls.cpp
        local_search/iterated_ls.hpp
        local_search/local_swaps.cpp
        local_search/local_swaps.hpp
        verifier/verifier.hpp
        verifier/verifier.cpp
)
