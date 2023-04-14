# Vector Math Benchmark

This project benchmarks several open source vector math libraries against one another to establish a baseline for performance.  Currently, it only tests GLM, DirectXMath, and SimpleMath from DirectXTK.

Note that this repository was created with `cmake-init`, and much of what is here is boilerplate related to it.  The only really important thing in the repository is `src/main.cpp`, which includes all of the benchmarking code.

# Results
The following measurements were taken on an AMD Ryzen 9 5900HS using Pop!_OS 22.04 LTS after running `pyperf system tune`.

|               ns/op |                op/s |    err% |          ins/op |          cyc/op |    IPC |         bra/op |   miss% |     total | benchmark
|--------------------:|--------------------:|--------:|----------------:|----------------:|-------:|---------------:|--------:|----------:|:----------
|                0.22 |    4,447,213,146.26 |    0.4% |            3.00 |            1.01 |  2.981 |           0.00 |   88.8% |      0.46 | `Store int (reference 'no-op')`
|                0.45 |    2,230,738,992.82 |    0.1% |            4.00 |            2.00 |  2.000 |           0.00 |   94.5% |      0.92 | `DirectX::SimpleMath::Vector2 addition`
|                1.34 |      744,131,562.82 |    0.0% |           10.00 |            6.00 |  1.667 |           0.00 |   98.3% |      2.74 | `DirectX::SimpleMath::Vector3 addition`
|                0.91 |    1,099,550,380.07 |    0.4% |            6.00 |            4.06 |  1.477 |           0.00 |   97.3% |      1.86 | `DirectX::SimpleMath::Vector4 addition`
|                0.22 |    4,464,846,468.49 |    0.1% |            3.00 |            1.00 |  2.995 |           0.00 |   87.6% |      0.46 | `glm::vec2 addition`
|                0.89 |    1,127,031,930.34 |    0.4% |            9.00 |            3.95 |  2.277 |           0.00 |   97.6% |      1.83 | `glm::vec3 addition`
|                0.45 |    2,215,906,353.14 |    0.2% |            6.00 |            2.01 |  2.990 |           0.00 |   94.4% |      0.92 | `glm::vec4 addition`
|                0.45 |    2,226,160,856.65 |    0.1% |            4.00 |            2.00 |  1.998 |           0.00 |   94.7% |      0.92 | `DirectX::XMFLOAT2 addition`
|                1.34 |      745,218,745.26 |    0.2% |           10.00 |            5.99 |  1.669 |           0.00 |   98.4% |      2.74 | `DirectX::XMFLOAT3 addition`
|                0.45 |    2,201,120,163.19 |    0.4% |            6.00 |            2.02 |  2.966 |           0.00 |   95.2% |      0.93 | `DirectX::XMFLOAT4 addition`
|                0.45 |    2,238,263,230.38 |    0.2% |            4.00 |            2.00 |  1.997 |           0.00 |   94.5% |      0.91 | `DirectX::XMFLOAT2 addition without Loads`
|                0.45 |    2,237,217,992.94 |    0.1% |            4.00 |            2.00 |  1.996 |           0.00 |   94.4% |      0.92 | `DirectX::XMFLOAT3 addition without Loads`
|                0.45 |    2,240,910,129.19 |    0.1% |            4.00 |            2.00 |  2.000 |           0.00 |   94.7% |      0.91 | `DirectX::XMFLOAT4 addition without Loads`
|                1.13 |      881,655,445.64 |    0.7% |            6.00 |            5.08 |  1.181 |           0.00 |   98.0% |      2.32 | `Complex operation 1 with DirectX::SimpleMath::Vector*`
|                0.45 |    2,207,258,583.66 |    0.9% |            6.00 |            2.02 |  2.976 |           0.00 |   94.7% |      0.93 | `Complex operation 1 with glm::vec*`
|                0.90 |    1,108,314,883.13 |    0.2% |            6.00 |            4.01 |  1.496 |           0.00 |   99.0% |      1.84 | `Complex operation 1 with DXM`
|                3.52 |      283,878,460.14 |    0.2% |           63.00 |           15.69 |  4.016 |           0.00 |   99.8% |      7.20 | `Complex operation 1 with DXM w/out loads`
|                1.35 |      740,197,556.01 |    0.2% |           10.00 |            6.01 |  1.665 |           0.00 |   99.3% |      2.76 | `Complex operation 2 with DirectX::SimpleMath::Vector3`
|                0.90 |    1,111,837,479.91 |    0.3% |            9.00 |            4.01 |  2.245 |           0.00 |   97.5% |      1.86 | `Complex operation 2 with glm::vec3`
|                1.35 |      742,418,148.81 |    0.0% |           10.00 |            6.00 |  1.666 |           0.00 |   98.4% |      2.75 | `Complex operation 2 with DXM`
|                1.84 |      542,056,187.87 |    0.4% |           30.00 |            8.18 |  3.666 |           0.00 |   98.8% |      3.78 | `Complex operation 2 with DXM w/out loads`
|                0.91 |    1,093,704,629.16 |    0.6% |            6.00 |            4.08 |  1.470 |           0.00 |   97.7% |      1.87 | `Complex operation 3 with DirectX::SimpleMath::Vector4`
|                0.45 |    2,207,068,878.08 |    0.3% |            6.00 |            2.01 |  2.988 |           0.00 |   95.2% |      0.93 | `Complex operation 3 with glm::vec4`
|                0.45 |    2,204,556,997.62 |    0.2% |            6.00 |            2.01 |  2.987 |           0.00 |   94.7% |      0.93 | `Complex operation 3 with DXM`
|                1.49 |      672,763,018.54 |    1.0% |           14.00 |            6.60 |  2.121 |           0.00 |   98.6% |      3.03 | `Complex operation 3 with DXM w/out loads`

# Building and installing

See the [BUILDING](BUILDING.md) document.

# Contributing

See the [CONTRIBUTING](CONTRIBUTING.md) document.

# Licensing

This project is licensed under GPLv3 to encourage users to contribute their additionsback to the main project.