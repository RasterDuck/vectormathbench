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
echo "The following measurements were taken on an AMD Ryzen 9 5900HS using Pop!_OS 22.04 LTS after running `pyperf system tune`." >> BENCHMARKS.md
echo "" >> BENCHMARKS.md

echo "## SSE 4.2" >> BENCHMARKS.md
./vectormathbench_sse42 >> BENCHMARKS.md

echo "" >> BENCHMARKS.md
echo "## AVX" >> BENCHMARKS.md
./vectormathbench_avx >> BENCHMARKS.md

echo "" >> BENCHMARKS.md
echo "## AVX2" >> BENCHMARKS.md
./vectormathbench_avx2 >> BENCHMARKS.md

# cp BENCHMARKS.md ../BENCHMARKS.md