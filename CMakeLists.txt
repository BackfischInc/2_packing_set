cmake_minimum_required(VERSION 3.28)
project(2_Packing_Set)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

target_compile_options(2_Packing_Set PRIVATE
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-DNDEBUG>
)

# Link-time optimization lets the compiler inline small graph and queue
# helpers across translation units when the selected toolchain supports it.
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
if (ipo_supported)
    set_property(TARGET 2_Packing_Set PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()
