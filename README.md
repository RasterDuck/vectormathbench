# Vector Math Benchmark

This project benchmarks several open source vector math libraries against one another to establish a baseline for performance.  Currently, it tests [GLM](https://github.com/g-truc/glm), [DirectXMath](https://github.com/microsoft/DirectXMath), [SimpleMath from DirectXTK](https://github.com/microsoft/DirectXTK/blob/main/Inc/SimpleMath.h), [this fork of Sony's Vectormath](https://github.com/glampert/vectormath), and [Realtime Math](https://github.com/nfrechette/rtm).  It tests the performance of all libraries under SSE4.2, AVX, and AVX2.

Note that this repository was created with `cmake-init`, and much of what is here is boilerplate related to it.  The only really important things in the repository are `src/main.cpp`, which includes all of the benchmarking code, and [BENCHMARKS.md](BENCHMARKS.md), which contains the latest benchmarking results.

# Results

TL;DR: Based on these benchmarks, it seems that the generally highest performing configuration across both AMD and Intel is DXM with SSE4.2, though it trades blows with Vectormath for matrix operations and GLM for vector operations.  However based on initial benchmarks of RTM on a single machine, it seems like it may be generally faster than DXM under the same configurations.  More investigation is required.

See the [BENCHMARKS](BENCHMARKS.md) document for more details.

# Building and installing

See the [BUILDING](BUILDING.md) document.

# Contributing

See the [CONTRIBUTING](CONTRIBUTING.md) document.

# Licensing

This project is licensed under GPLv3 to encourage users to contribute their additions back to the main project.