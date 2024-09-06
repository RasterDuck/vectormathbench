#!/bin/bash

# Check if there is a --debug argument
if [ "$1" == "--debug" ]; then
    BUILD_TYPE="Debug"
    TARGET_DIR="./build/dbg"
else
    BUILD_TYPE="Release"
    TARGET_DIR="./build/rel"
fi

# Execute the build
mkdir -p $TARGET_DIR
cd $TARGET_DIR
cmake -GNinja -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_EXPORT_COMPILE_COMMANDS=true ../.. || (
    echo "CMake failed"
    exit 1
)

cp compile_commands.json ../../compile_commands.json

rm -f ./vectormathbench_sse42
rm -f ./vectormathbench_avx
rm -f ./vectormathbench_avx2

ninja || (
    echo "Build failed"
    exit 2
)

echo "# Benchmark Results" > BENCHMARKS.md
echo "The following measurements were taken on an AMD Ryzen 9 5900HS using Pop!_OS 22.04 LTS after running \`pyperf system tune\`." >> BENCHMARKS.md
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