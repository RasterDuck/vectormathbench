# Vector Math Benchmark

This project benchmarks several open source vector math libraries against one another to establish a baseline for performance.  Currently, it tests [GLM](https://github.com/g-truc/glm), [DirectXMath](https://github.com/microsoft/DirectXMath), [SimpleMath from DirectXTK](https://github.com/microsoft/DirectXTK/blob/main/Inc/SimpleMath.h), [this fork of Sony's Vectormath](https://github.com/glampert/vectormath), [move::vectormath](https://github.com/move-engine/move-vectormath), and [Realtime Math](https://github.com/nfrechette/rtm).  It tests the performance of all libraries under SSE4.2, AVX, and AVX2.

Note that this repository was created with `cmake-init`, and much of what is here is boilerplate related to it.  The only really important things in the repository are `src/main.cpp`, which includes all of the benchmarking code, and [BENCHMARKS.md](BENCHMARKS.md), which contains the latest benchmarking results.

# Results

TL;DR: Based on the current benchmarks (reflected in intermediate-benchmarks.md), it seems that the generally highest performing configuration across both AMD and Intel is Realtime Math, though it lacks many features compared to the other libraries.  `move::vectormath` seems to have generally similar performance to RTM (which makes sense given that it's built on top of RTM), though similarly suffers from a lack of features at the moment (though it does have more features than RTM, as well as including more game-centric extensions for RTM based on DXM/GLM, depending on the operation in question).  DXM with SSE4.2 seems to be the fastest non-RTM library, though it trades blows with Vectormath for matrix operations and GLM for vector operations.

See the [BENCHMARKS](BENCHMARKS.md) document for more details.

# Building and installing

See the [BUILDING](BUILDING.md) document.

# Contributing

See the [CONTRIBUTING](CONTRIBUTING.md) document.

# Licensing

This project is licensed under GPLv3 to encourage users to contribute their additions back to the main project.