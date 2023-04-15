#!/bin/bash

mkdir build
cd build
cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=true .. || (
    echo "CMake failed"
    exit 1
)

cp compile_commands.json ../compile_commands.json

ninja || (
    echo "Build failed"
    exit 2
)

echo "# Benchmark Results" > BENCHMARKS.md
echo "" >> BENCHMARKS.md

echo "## SSE" >> BENCHMARKS.md
./vectormathbench_sse >> BENCHMARKS.md

echo "" >> BENCHMARKS.md
echo "## AVX" >> BENCHMARKS.md
./vectormathbench_avx >> BENCHMARKS.md

cp BENCHMARKS.md ../BENCHMARKS.md