# Vector Math Benchmark

This project benchmarks several open source vector math libraries against one another to establish a baseline for performance.  Currently, it tests [GLM](https://github.com/g-truc/glm), [DirectXMath](https://github.com/microsoft/DirectXMath), [SimpleMath from DirectXTK](https://github.com/microsoft/DirectXTK/blob/main/Inc/SimpleMath.h), [this fork of Sony's Vectormath](https://github.com/glampert/vectormath), [move::vectormath](https://github.com/move-engine/move-vectormath), and [Realtime Math](https://github.com/nfrechette/rtm).  It tests the performance of all libraries under SSE4.2, AVX, and AVX2.

Note that this repository was created with `cmake-init`, and much of what is here is boilerplate related to it.  The only really important things in the repository are `src/main.cpp`, which includes all of the benchmarking code, and [BENCHMARKS.md](BENCHMARKS.md), which contains the latest benchmarking results.

# Benchmark coverage

In addition to focused vector and matrix operations, the suite contains
composite workloads based on common frame-update and rendering tasks:

- Normalizing movement, aim, and lighting directions.
- Integrating particle position and velocity for one simulation step.
- Building an orthonormal camera or aim basis from eye, target, and up vectors.
- Rotating a direction by an object's quaternion orientation.
- Intersecting hit and miss rays with axis-aligned bounding boxes using the
  vectorized slab method.
- Intersecting hit and miss rays with triangles using the two-sided
  Möller–Trumbore algorithm.

The workload inputs pass through optimization barriers so the compiler cannot
replace the measured math with precomputed constants. Float implementations are
compared across all supported libraries, with double-precision variants for
`move::math` and Realtime Math. The intersection kernels share one algorithm
through library-specific adapters, keeping branches and acceptance rules
equivalent while exercising each library's native vector operations.

# Output organization

Each benchmark table represents one capability, such as direction
normalization, ray/triangle intersection, or matrix multiplication. Variants
such as hit and miss paths remain visible as separate rows. This makes the
nanobench output useful as a detailed view without mixing unrelated operations
into one large table.

At the end of a run, the executable prints a compact single-precision ranking
summary. For every directly comparable capability it reports the fastest
library, the `move::math` rank, and its gap from the winner. Double-precision
rows remain in the detailed tables but are not combined with single-precision
rankings. QVV operations also have separate tables from conventional matrix
operations because they represent a different transform representation.

Construction helpers are shown in capability tables but are not included in
the ranking summary where the libraries use materially different conventions,
such as handedness or projection variants.

# Methodology

The suite lets nanobench determine the iteration count adaptively. Each
capability uses 15 epochs, a warm-up phase, and a minimum epoch duration of
5 ms. Inputs are made opaque immediately before measured work, and outputs use
read-only optimization barriers so aggregate values are not copied or written
back as an artifact of the harness.

Matrix throughput benchmarks use independent, non-constant input pairs instead
of feeding each result into the next iteration. This measures throughput rather
than a dependency-chain latency unless a benchmark explicitly says otherwise.
Constant-expression synthetic micro-operations are not run because they are
especially vulnerable to constant folding and do not resemble frame-loop work.
GLM is built with its normal configuration rather than being forced into its
pure scalar implementation.

# Results

TL;DR: Based on the current benchmarks (reflected in intermediate-benchmarks.md), it seems that the generally highest performing configuration across both AMD and Intel is Realtime Math, though it lacks many features compared to the other libraries.  `move::vectormath` seems to have generally similar performance to RTM (which makes sense given that it's built on top of RTM), though similarly suffers from a lack of features at the moment (though it does have more features than RTM, as well as including more game-centric extensions for RTM based on DXM/GLM, depending on the operation in question).  DXM with SSE4.2 seems to be the fastest non-RTM library, though it trades blows with Vectormath for matrix operations and GLM for vector operations.

See the [BENCHMARKS](BENCHMARKS.md) document for more details.

# Building and installing

See the [BUILDING](BUILDING.md) document.

# Contributing

See the [CONTRIBUTING](CONTRIBUTING.md) document.

# Licensing

This project is licensed under GPLv3 to encourage users to contribute their additions back to the main project.
