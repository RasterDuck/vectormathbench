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

Intersection latency and throughput are intentionally separate capabilities.
Latency tables expose individual hit and miss paths. Throughput tables process
256 varied rays with a realistic mixture of hits and misses and report the
amortized cost per ray.

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

Focused register-resident operations report reciprocal throughput: successive
iterations are independent and an out-of-order CPU can overlap them. A result
below one nanosecond is therefore possible and should not be read as
dependency-chain latency or as the cost of updating an array of objects.
Intersection tables explicitly separate individual-call latency cases from
256-ray, mixed-input throughput cases.

Matrix throughput benchmarks use independent, non-constant input pairs instead
of feeding each result into the next iteration. This measures throughput rather
than a dependency-chain latency unless a benchmark explicitly says otherwise.
Constant-expression synthetic micro-operations are not run because they are
especially vulnerable to constant folding and do not resemble frame-loop work.
GLM is built with its normal configuration rather than being forced into its
pure scalar implementation.

Before any measurements, the executable checks semantic parity for
normalization, particle integration, camera-basis construction, quaternion
rotation, ray-box intersection, ray-triangle intersection, and point-segment
closest points. The checks cover all participating single-precision libraries
plus Move and RTM double-precision intersection adapters. `--verify-only` runs
these checks without collecting timings. Normal CI runs that mode for every ISA
build, which also catches compiler- and instruction-set-specific
miscompilations.

GCC targets disable strict-aliasing optimization because the compared
DirectXMath and Sony Vectormath versions use pointer type-punning internally.
Without that compatibility flag, optimized AVX builds can discard DirectXMath
stores and produce deceptively low timings for operations that did not occur.

Use `--json PATH` and `--csv PATH` to save every nanobench measurement in
machine-readable form:

```sh
./build/vectormathbench_sse42 \
  --json benchmark-results/sse42.json \
  --csv benchmark-results/sse42.csv
```

Push and pull-request CI builds every ISA variant on Linux and Windows and runs
the semantic checks. Push and manual runs also collect timings after the
verification job. Shared hosted runners are too noisy for performance gating,
so these are diagnostic reports rather than pass/fail regression thresholds.
Those runs upload Markdown, JSON, and CSV results.

## API-v2 Phase A representation proof

The `vectormathbench_phase_a_sse42`, `_avx`, and `_avx2` executables measure
the architectural choices that are intentionally absent from the
cross-library capability ranking:

- public native-backed versus fixed-array and raw-native `Vec3f`;
- 8-byte versus 16-byte `Vec2f`;
- sprite integration and UI affine-transform workloads comparing the two
  `Vec2f` representations;
- persistent 16-byte compute versus 12-byte packed particle storage;
- fused packed-to-16-byte-GPU transforms;
- fused transforms over interleaved strided fields.

Each storage workload runs at 256, 4,096, 65,536, and 1,048,576 elements to
show cache and bandwidth crossovers. These tables answer representation and
data-flow questions; they do not rank libraries implementing different public
APIs.

The matching Phase B executables compare semantic `Point3f` and
`Direction3f` transforms with their raw-`Vec3f` equivalents. They also compare
validating direction input once at a trust boundary with normalizing it during
every game-loop use.

The Phase C executables compare the new typed CPU geometry API with equivalent
raw-`Vec3f` kernels. They cover prepared one-ray/many-AABB traversal,
per-query ray/AABB tests, predicate versus detailed ray/triangle queries, and
point/segment closest points over cache- and working-set-sized batches. The
point/segment capability also compares GLM and DirectXMath because both expose
matching semantics. Each executable verifies semantic parity before collecting
timings, and CI runs the parity-only mode on every ISA build.

By default CMake fetches the exact Move revision recorded in `CMakeLists.txt`.
During library development, configure against a local checkout instead:

```sh
cmake -S . -B build/phase-a \
  -DMOVE_VECTORMATH_SOURCE_DIR=/path/to/move-vectormath \
  -DBUILD_TESTING=OFF
```

# Results

TL;DR: Based on the current benchmarks (reflected in intermediate-benchmarks.md), it seems that the generally highest performing configuration across both AMD and Intel is Realtime Math, though it lacks many features compared to the other libraries.  `move::vectormath` seems to have generally similar performance to RTM (which makes sense given that it's built on top of RTM), though similarly suffers from a lack of features at the moment (though it does have more features than RTM, as well as including more game-centric extensions for RTM based on DXM/GLM, depending on the operation in question).  DXM with SSE4.2 seems to be the fastest non-RTM library, though it trades blows with Vectormath for matrix operations and GLM for vector operations.

See the [BENCHMARKS](BENCHMARKS.md) document for more details.

# Building and installing

See the [BUILDING](BUILDING.md) document.

# Contributing

See the [CONTRIBUTING](CONTRIBUTING.md) document.

# Licensing

This project is licensed under GPLv3 to encourage users to contribute their additions back to the main project.
