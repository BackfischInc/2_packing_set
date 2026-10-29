# 2-Packing-Set Runtime Optimization

This branch contains a runtime-focused optimization pass for the 2-packing-set solver. The algorithm remains an iterated local search (ILS): each outer iteration perturbs the current solution, locally maximizes it, and either keeps an improvement or rolls the changes back.

The optimization branch is named `runtime-optimization-ils`.

## Scope

This README documents the changes made during the runtime optimization pass. The worktree already contained unrelated uncommitted changes when the branch was created; those changes were preserved and were not rewritten as part of this pass.

## Changes made

### 1. Fixed duplicate entries in the iteration queue

File: `packing_set/iteration_queue.cpp`

The queue uses a generation marker for each vertex. A vertex should appear at most once in `next_indices` during one queue generation, even if many changed vertices request that vertex's neighborhood.

The old code compared the stored marker with `iteration`, but stored `iteration + 1`. Therefore repeated insertions into the current generation were not suppressed. Every repeated request appended another copy of the same vertex until the queue limit was reached. This inflated `maximize_solution`, made the queue limit trigger prematurely, and caused the same local-search operation to be evaluated repeatedly.

The implementation now computes the generation being populated and compares against that same value:

```cpp
const uint64_t next_generation = iteration + 1;
if (indicator[index] != next_generation) {
  indicator[index] = next_generation;
  next_indices.push_back(index);
}
```

This preserves the existing two-buffer queue design while restoring its intended uniqueness guarantee. It is the highest-impact fix in this pass because distance-two neighborhoods are added from many locations throughout the local search.

### 2. Reserved queue storage up front

File: `packing_set/iteration_queue.hpp`

The queue now reserves capacity for both `curr_indices` and `next_indices` in its constructor. Local search repeatedly swaps and clears these vectors. Their allocated capacity is retained between iterations, so reserving once avoids repeated growth and memory reallocations in the hot loop.

This does not change queue behavior or its maximum size; it only moves expected allocation cost out of the search loop.

### 3. Replaced ordered candidate storage for 2-to-1 swaps

Files: `local_search/local_swaps.hpp`, `local_search/local_swaps.cpp`

The 2-to-1 swap search stored potential replacement vertices in `std::set<uint64_t>`. Candidate membership is the important operation in this routine; candidates do not need sorted traversal.

The type is now `std::unordered_set<uint64_t>`. Membership and insertion change from tree-based `O(log d)` operations to expected `O(1)` operations, where `d` is the local candidate count. The candidate set is also reserved to approximately twice the degree of the current vertex, reducing hash-table rehashing while candidates are discovered.

The helper signature was updated accordingly. The behavior of the 2-to-1 swap test is unchanged: it still rejects incompatible or insufficiently weighted pairs and applies the first valid improving swap.

### 4. Reduced the size of each perturbation

File: `local_search/iterated_ls.cpp`

The random value used by perturbation is sampled from `0.005` to `0.1` and is interpreted as the fraction of the graph to perturb. The old calculation divided the number of vertices by this fraction, producing approximately `10n` to `200n` mutation attempts per perturbation.

The new calculation uses multiplication and enforces at least one mutation:

```cpp
max_iterations = max(1, graph.amount_nodes() * iter_amount);
```

This gives approximately `0.5%` to `10%` of the graph per perturbation. The loop counter was also changed to `uint64_t` to match graph and iteration sizes.

This is an algorithmic tuning change rather than merely a micro-optimization. It keeps the ILS balance between diversification and intensification while preventing excessive work in each perturbation.

### 5. Made perturbation safe for isolated vertices

File: `local_search/iterated_ls.cpp`

After every mutation, perturbation follows three random graph edges. The old code indexed into the neighbor span without checking whether it was empty. For an isolated vertex this could perform modulo-by-zero arithmetic and crash.

The code now stops the random walk when a vertex has no neighbors. This avoids invalid arithmetic and lets the outer ILS iteration continue normally.

### 6. Improved time-limit checking

File: `local_search/iterated_ls.cpp`

The search previously checked the clock only every 10,000 iterations and used a minute-duration conversion for the intermediate value. The new implementation:

- checks every 1,024 iterations using a cheap bit-mask test;
- measures elapsed time in seconds;
- converts the configured minute limit explicitly to seconds.

This reduces overshoot of the requested time limit without calling the clock on every iteration. It also removes the misleading `hours` typedef, which actually represented a seconds-based duration. The check remains outside inner mutation and neighborhood loops, so its overhead is small relative to graph traversal and swap evaluation.

### 7. Added explicit standard-library support

File: `local_search/iterated_ls.cpp`

`std::max` is now used to enforce at least one perturbation step. The file therefore explicitly includes `<algorithm>` instead of relying on an indirect include.

### 8. Added a lazy cached distance-two CSR structure

Files: `graph/csr_graph.hpp`, `graph/csr_graph.cpp`, `local_search/local_swaps.cpp`

Queue expansion previously recomputed the same nested neighbor traversal for every changed vertex. This appeared in perturbation, automatic inclusion, and 2-to-1 swaps, and dominated runtime on high-degree graphs.

The graph now lazily builds a second CSR structure containing each vertex's unique distance-one-or-two neighborhood. `add_neighborhood()` performs one linear scan over the cached span instead of repeatedly scanning every neighbor-of-neighbor. Lazy construction avoids paying the preprocessing cost when search never starts, while the cache is reused for the rest of the run.

### 9. Converted the base graph to standard CSR offsets

Files: `graph/csr_graph.hpp`, `graph/csr_graph.cpp`

The original graph stored `n - 1` cumulative endpoints and had special cases for the first and last vertex in `get_neighbors()`. It now stores `n + 1` standard CSR offsets, making every neighbor range simply `[starts[v], starts[v + 1])`. This removes branches from every neighbor lookup and aligns the base representation with the cached square graph.

### 10. Hardened graph parsing and normalized adjacency lists

File: `graph/csr_graph.cpp`

The parser now initializes header fields before extraction, validates vertex identifiers, stores the actual parsed adjacency extent, rejects files with too few adjacency lines, and sorts/deduplicates each adjacency list.

Using the actual parsed extent prevents a malformed edge-count header from creating spans over uninitialized memory. Sorting is required by the fast binary-search adjacency test and the two-pointer common-neighbor test.

### 11. Replaced hot-path partner hash sets with reusable marked vectors

Files: `packing_set/packing_set.*`, `local_search/iterated_ls.*`, `local_search/local_swaps.*`

Conflict partners are small local collections. Constructing and clearing an `unordered_set` for every candidate caused hashing, allocation, and bucket-management overhead in the hottest routines.

`packing_set` now maintains a reusable mark array and generation counter. It collects unique partners into a reusable `std::vector<uint64_t>` in linear time, using the mark array for duplicate suppression. Vector/span overloads were added for removal and weight calculation, while the old unordered-set overloads remain as compatibility helpers.

### 12. Accelerated adjacency membership checks

File: `local_search/local_swaps.cpp`

Because adjacency lists are now normalized and sorted, `are_neighbors()` uses `std::binary_search()` instead of a linear scan. An individual membership check therefore changes from `O(deg)` to `O(log deg)`, which is useful inside the 2-to-1 candidate loop.

### 13. Replaced neighborhood-reconstruction rollback with transactional undo

Files: `packing_set/packing_set.hpp`, `packing_set/packing_set.cpp`,
`local_search/iterated_ls.cpp`

Rejected ILS iterations previously called `unwind()`, which reconstructed the
prior packing set by repeatedly finding partners and rescanning their
neighborhoods. That made rollback expensive precisely when an iteration did
not improve the incumbent solution.

The packing set now supports transactions. At the start of each ILS iteration
it saves the objective totals. The first time a vertex is modified, its old
solution bit and old partner value are recorded. Repeated modifications to the
same vertex in that transaction do not create duplicate log entries. A rejected
iteration restores the log in reverse order and restores the saved size and
weight directly. An accepted iteration simply clears the log.

Rollback is therefore proportional to the number of distinct modified
vertices, rather than requiring new graph traversals for every changed
solution vertex.

### 14. Reused 2-to-1 swap candidate storage

Files: `local_search/local_swaps.hpp`, `local_search/local_swaps.cpp`,
`local_search/iterated_ls.cpp`

The 2-to-1 search previously allocated and populated a new hash table for
every solution vertex examined. A reusable `swap_workspace` now owns the
candidate vector and a generation-mark array. Each search clears the vector by
length and advances a generation counter, so duplicate detection is expected
`O(1)` without allocating buckets or nodes in the hot loop.

The workspace is created once per ILS run and passed through local
maximization. This preserves the same candidate ordering semantics while
reducing allocation pressure and improving cache locality. Candidate discovery
also reuses the graph's cached distance-two CSR span instead of rescanning
every neighbor-of-neighbor during each swap search.

### 15. Improved Release build configuration

File: `CMakeLists.txt`

The project no longer overwrites global `CMAKE_CXX_FLAGS`. Release optimization
flags are scoped to the executable and include `-O3` and `-DNDEBUG`. CMake also
checks whether the active toolchain supports interprocedural optimization and
enables link-time optimization for Release builds when available.

This allows Debug and other configurations to retain their intended flags,
while Release builds can inline and optimize across translation units.

## Behavior intentionally retained

- The solver still uses iterated local search.
- Perturbation still removes or inserts vertices using the existing packing-set bookkeeping.
- Local maximization still supports automatic inclusion and optional 2-to-1 swaps.
- Weighted and unweighted objectives remain available.
- The existing changelog/unwind rollback mechanism remains in place.
- The command-line interface remains unchanged. The parser is stricter about malformed input and requires the declared number of adjacency lines.

## Validation

The project was configured and built successfully with a fresh Linux CMake build directory:

```bash
cmake -S . -B /tmp/packing-set-build3 -DCMAKE_BUILD_TYPE=Release
cmake --build /tmp/packing-set-build3 -j2
```

The resulting executable compiled all solver components successfully. A small
structurally valid graph was also used to verify startup, parsing, cached
distance-two construction, square-graph output, and the zero-minute
early-termination path. The same valid input completed successfully under an
AddressSanitizer/UndefinedBehaviorSanitizer build.

## Known fixture issue

The bundled `example.graph` is inconsistent with the parser's expected METIS-style format: its header declares 5 edges, implying 10 adjacency entries for an undirected graph, but the file contains only 8. The new parser rejects or avoids using the missing entries, but the fixture still does not represent a valid benchmark graph and should not be used for performance comparisons.

That input file was not modified in this pass. For reliable benchmarking, provide graphs whose header edge count matches their adjacency lists.

## Building and running

From the project root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/2_Packing_Set -i path/to/input.graph -time_limit 1
```

Useful options include:

- `-weighted` — optimize total vertex weight rather than cardinality;
- `-disable_2_1` — disable the more expensive 2-to-1 swap phase;
- `-max_size N` — limit the perturbation queue size;
- `-time_limit N` — run for approximately `N` minutes;
- `-verbose` — print improvements as they are found.

For large or high-degree graphs, `-disable_2_1` remains useful when measuring the queue and perturbation changes separately from the 2-to-1 neighborhood search.
