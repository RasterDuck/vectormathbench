# Benchmark Results

> NOTE: These benchmarks are out of date and do not include [RTM](https://github.com/nfrechette/rtm), which seems to be the fastest of the available libraries for most of the tested functionality (though it has the fewest features by a wide margin).  See [here](./intermediate-benchmarks.md) for current benchmarks from a single machine.  Full benchmarks will be rerun at some point, at which this document will be updated.

## Author's analysis

TL;DR: Based on these benchmarks, it seems that the generally highest performing configuration across both AMD and Intel is DXM with SSE4.2, though it trades blows with Vectormath for matrix operations and GLM for vector operations.  However based on initial benchmarks of RTM on a single machine, it seems like it may be generally faster than DXM under the same configurations.  More investigation is required.  The rest of this section was written before RTM was added.

* DXM seems to generally be the fastest for matrix ops (especially forming a model matrix) and trades blows with the other libraries for vector ops.
* SimpleMath introduces non-negligible overhead over DXM for most operations, but for some it is essentially identical.  I also think it could probably be improved to lower the performance gap for the slower operations, but I haven't experimented with this.  I see very little reason that SimpleMath couldn't be roughly as fast as Vectormath's C++ interface.
* GLM seems to generally be the fastest for vector2 ops and trades blows with DXM for Vector3 and 4, but it is sometimes _substantially_ slower for matrix ops.
* Vectormath generally trades blows with DXM, but interestingly has a much faster Vector3 implementation (I'm guessing it is essentially a Vector4 that just ignores the w component).  I am not surprised that Vectormath's vector3 is faster than GLM and SimpleMath, but am more surprised at the difference compared to DXM.

The clearest/most distinct "advantage" any of the benchmarks shows seems to be DXM's built-in function for building a model matrix - under SSE4.2 and AVX2 (but not AVX?), it is _much_ faster than any of the other libraries at that particular operation (assuming the other libraries don't have a built-in function for them that I was unable to find).  Assuming that isn't just reflective of an unknown bug in the benchmarks/a compiler bug/etc, I could see that potentially making a real difference for a game's performance, especially if you have many objects in your scene.

I added the Intel benchmarks because I was surprised at the apparent performance degradation between SSE4.2 and AVX/AVX2 and assumed that AMD's implementation of AVX/AVX2 may be slower than Intel's, but that doesn't seem to be the case.  I am pretty surprised at the relatively poor matrix/matrix multiplication speed on Intel, though it is possible that particular test was somehow disproportionately affected by running on a shared CPU.  Unfortunately, I do not personally own any modern Intel machines, so I can't test the performance on a non-shared processor.

## AMD-EPYC

The following measurements were taken on a Vultr CPU optimized instance (dedicated vCPU) running an AMD EPYC-Rome Processor using Debian 11.

### SSE 4.2

| ns/op |             op/s | err% | ins/op | cyc/op |   IPC | bra/op | miss% | total | benchmark                                      |
| ----: | ---------------: | ---: | -----: | -----: | ----: | -----: | ----: | ----: | :--------------------------------------------- |
|  0.26 | 3,866,930,651.19 | 0.1% |   3.00 |   1.00 | 2.995 |   0.00 | 97.5% |  0.35 | `Store int (reference 'no-op')`                |
|  0.52 | 1,935,503,133.02 | 0.1% |   4.00 |   2.00 | 1.999 |   0.00 | 98.8% |  0.71 | `SimpleMath::Vector2 addition`                 |
|  1.55 |   644,237,912.30 | 0.0% |  10.00 |   6.00 | 1.666 |   0.00 | 99.6% |  2.12 | `SimpleMath::Vector3 addition`                 |
|  1.03 |   966,204,350.66 | 0.1% |   6.00 |   4.01 | 1.497 |   0.00 | 99.4% |  1.41 | `SimpleMath::Vector4 addition`                 |
|  0.26 | 3,869,783,833.61 | 0.2% |   3.00 |   1.00 | 2.999 |   0.00 | 97.4% |  0.35 | `glm::vec2 addition`                           |
|  5.17 |   193,364,304.67 | 0.1% |  10.00 |  20.01 | 0.500 |   0.00 | 99.9% |  7.06 | `glm::vec3 addition`                           |
|  0.52 | 1,932,774,634.80 | 0.1% |   6.00 |   2.00 | 2.995 |   0.00 | 98.7% |  0.71 | `glm::vec4 addition`                           |
|  0.52 | 1,935,222,984.03 | 0.2% |   4.00 |   2.00 | 1.999 |   0.00 | 98.7% |  0.71 | `DirectX::XMFLOAT2 addition`                   |
|  1.55 |   645,390,074.32 | 0.1% |  10.00 |   6.00 | 1.668 |   0.00 | 99.6% |  2.11 | `DirectX::XMFLOAT3 addition`                   |
|  0.52 | 1,931,843,833.79 | 0.2% |   4.00 |   2.00 | 1.997 |   0.00 | 98.7% |  0.71 | `DirectX::XMFLOAT4 addition without Loads`     |
|  0.26 | 3,867,735,312.92 | 0.1% |   3.00 |   1.00 | 2.999 |   0.00 | 97.6% |  0.35 | `Vectormath::Vector2 addition`                 |
|  0.52 | 1,929,241,491.44 | 0.2% |   4.00 |   2.00 | 1.997 |   0.00 | 98.7% |  0.71 | `Vectormath::Vector3 addition`                 |
|  0.52 | 1,931,959,277.88 | 0.1% |   4.00 |   2.00 | 1.997 |   0.00 | 98.7% |  0.71 | `Vectormath::Vector4 addition`                 |
|  1.04 |   964,858,220.32 | 0.1% |   5.00 |   4.01 | 1.248 |   0.00 | 99.4% |  1.41 | `Complex operation 1 with SimpleMath::Vector*` |
|  0.52 | 1,931,000,087.72 | 0.1% |   6.00 |   2.00 | 2.995 |   0.00 | 98.9% |  0.71 | `Complex operation 1 with glm::vec*`           |
|  0.78 | 1,288,537,458.49 | 0.1% |   5.00 |   3.00 | 1.664 |   0.00 | 99.1% |  1.06 | `Complex operation 1 with DXM`                 |
|  3.94 |   253,508,519.26 | 0.1% |  61.00 |  15.26 | 3.997 |   0.00 | 99.9% |  5.39 | `Complex operation 1 with DXM w/out loads`     |
|  0.78 | 1,285,898,475.51 | 0.1% |   4.00 |   3.01 | 1.329 |   0.00 | 99.2% |  1.06 | `Complex operation 1 with Vectormath`          |
|  1.56 |   641,713,623.79 | 0.4% |  10.00 |   6.00 | 1.665 |   0.00 | 99.8% |  2.13 | `Complex operation 2 with SimpleMath::Vector3` |
|  5.20 |   192,200,259.11 | 0.1% |  10.00 |  20.10 | 0.498 |   0.00 | 99.9% |  7.10 | `Complex operation 2 with glm::vec3`           |
|  1.55 |   645,499,028.51 | 0.1% |  10.00 |   6.00 | 1.668 |   0.00 | 99.6% |  2.11 | `Complex operation 2 with DXM`                 |
|  2.17 |   460,905,867.30 | 0.7% |  29.00 |   8.40 | 3.454 |   0.00 | 99.7% |  2.96 | `Complex operation 2 with DXM w/out loads`     |
|  0.78 | 1,285,594,662.47 | 0.1% |   4.00 |   3.01 | 1.329 |   0.00 | 99.3% |  1.06 | `Complex operation 2 with Vectormath`          |
|  1.04 |   965,442,760.95 | 0.1% |   6.00 |   4.01 | 1.497 |   0.00 | 99.4% |  1.41 | `Complex operation 3 with SimpleMath::Vector4` |
|  0.52 | 1,929,151,121.00 | 0.2% |   6.00 |   2.00 | 2.993 |   0.00 | 98.8% |  0.72 | `Complex operation 3 with glm::vec4`           |
|  0.52 | 1,933,843,371.61 | 0.1% |   6.00 |   2.00 | 2.995 |   0.00 | 98.9% |  0.71 | `Complex operation 3 with DXM`                 |
|  1.01 |   993,821,806.05 | 0.7% |  13.00 |   3.89 | 3.343 |   0.00 | 99.5% |  1.38 | `Complex operation 3 with DXM w/out loads`     |
|  0.78 | 1,288,069,004.23 | 0.1% |   5.00 |   3.00 | 1.664 |   0.00 | 99.2% |  1.06 | `Complex operation 3 with Vectormath`          |
| 32.83 |    30,460,448.66 | 0.1% | 408.00 | 127.06 | 3.211 |  10.00 |  0.0% |  5.24 | `Construct model matrix SimpleMath`            |
| 64.58 |    15,484,886.17 | 0.2% | 787.00 | 248.55 | 3.166 |  34.00 |  0.0% | 10.29 | `Construct model matrix glm`                   |
| 27.78 |    36,002,989.72 | 0.2% | 356.00 | 107.46 | 3.313 |   8.00 |  0.0% |  4.44 | `Construct model matrix DXM (manual)`          |
|  3.11 |   321,927,543.18 | 0.0% |  16.00 |  12.02 | 1.331 |   0.00 | 98.1% |  0.50 | `Construct model matrix DXM (builtin)`         |
| 34.64 |    28,867,273.67 | 0.1% | 472.00 | 133.83 | 3.527 |  10.00 |  0.0% |  5.53 | `Construct model matrix Vectormath`            |
|  3.10 |   322,155,984.73 | 0.1% |  25.00 |  12.02 | 2.080 |   0.00 | 98.2% |  0.50 | `Construct view matrix SimpleMath`             |
|  3.11 |   321,994,485.61 | 0.1% |  24.00 |  12.02 | 1.997 |   0.00 | 98.5% |  0.50 | `Construct view matrix glm`                    |
|  3.11 |   321,550,857.21 | 0.3% |  25.00 |  12.02 | 2.079 |   0.00 | 98.3% |  0.50 | `Construct view matrix DXM`                    |
| 16.45 |    60,805,016.46 | 0.0% |  53.00 |  63.60 | 0.833 |   0.00 | 99.7% |  2.62 | `Construct view matrix Vectormath`             |
|  3.11 |   321,932,095.54 | 0.1% |  18.00 |  12.02 | 1.498 |   0.00 | 98.3% |  0.50 | `Construct perspective matrix SimpleMath`      |
| 22.77 |    43,924,792.04 | 0.1% |  29.00 |  88.04 | 0.329 |   0.00 | 99.8% |  3.64 | `Construct perspective matrix glm`             |
|  3.11 |   321,846,781.10 | 0.2% |  18.00 |  12.02 | 1.498 |   0.00 | 98.4% |  0.50 | `Construct perspective matrix DXM`             |
|  1.04 |   964,878,573.73 | 0.2% |   8.00 |   4.01 | 1.997 |   0.00 | 94.7% |  0.17 | `Construct perspective matrix Vectormath`      |
|  3.13 |   319,118,716.36 | 0.3% |  18.00 |  12.09 | 1.489 |   0.00 | 98.5% |  0.50 | `Construct ortho matrix SimpleMath`            |
|  3.11 |   321,058,251.87 | 0.1% |  24.00 |  12.05 | 1.991 |   0.00 | 98.4% |  0.50 | `Construct ortho matrix glm`                   |
|  3.11 |   321,993,522.57 | 0.2% |  18.00 |  12.02 | 1.498 |   0.00 | 98.0% |  0.50 | `Construct ortho matrix DXM`                   |
| 19.76 |    50,607,375.83 | 0.1% |  37.00 |  76.40 | 0.484 |   0.00 | 99.8% |  3.15 | `Construct ortho matrix Vectormath`            |
| 12.82 |    77,980,794.70 | 0.1% |  24.00 |  49.61 | 0.484 |   0.00 | 99.6% |  2.05 | `Vector matrix multiply SimpleMath`            |
| 11.91 |    83,982,777.79 | 0.1% |  20.00 |  46.05 | 0.434 |   0.00 | 99.5% |  1.90 | `Vector matrix multiply glm`                   |
|  8.85 |   112,961,166.28 | 0.0% |  21.00 |  34.25 | 0.613 |   0.00 | 99.4% |  1.41 | `Vector matrix multiply DXM`                   |
|  8.50 |   117,665,861.78 | 0.0% |  20.00 |  32.86 | 0.609 |   0.00 | 99.4% |  1.35 | `Vector matrix multiply Vectormath`            |
|  8.01 |   124,799,954.15 | 0.7% |  88.00 |  30.86 | 2.852 |   0.00 | 99.4% |  1.30 | `Matrix matrix multiply SimpleMath`            |
|  5.35 |   186,870,046.18 | 0.4% |  94.00 |  20.69 | 4.542 |   0.00 | 99.0% |  0.85 | `Matrix matrix multiply glm`                   |
|  5.52 |   181,110,083.78 | 0.2% |  88.00 |  21.38 | 4.117 |   0.00 | 99.1% |  0.88 | `Matrix matrix multiply DXM`                   |
|  8.08 |   123,774,297.07 | 0.1% |  76.00 |  31.28 | 2.430 |   0.00 | 99.3% |  1.29 | `Matrix matrix multiply Vectormath`            |

### AVX

| ns/op |             op/s | err% | ins/op | cyc/op |   IPC | bra/op | miss% | total | benchmark                                      |
| ----: | ---------------: | ---: | -----: | -----: | ----: | -----: | ----: | ----: | :--------------------------------------------- |
|  0.26 | 3,868,982,073.64 | 0.1% |   3.00 |   1.00 | 2.997 |   0.00 | 97.4% |  0.35 | `Store int (reference 'no-op')`                |
|  0.52 | 1,932,541,659.03 | 0.1% |   4.00 |   2.00 | 1.998 |   0.00 | 98.7% |  0.71 | `SimpleMath::Vector2 addition`                 |
|  5.43 |   184,190,196.28 | 0.0% |  10.00 |  21.01 | 0.476 |   0.00 | 99.9% |  7.41 | `SimpleMath::Vector3 addition`                 |
|  1.04 |   966,052,928.89 | 0.1% |   6.00 |   4.01 | 1.498 |   0.00 | 99.4% |  1.42 | `SimpleMath::Vector4 addition`                 |
|  0.26 | 3,872,304,295.64 | 0.1% |   3.00 |   1.00 | 2.999 |   0.00 | 97.4% |  0.35 | `glm::vec2 addition`                           |
|  5.23 |   191,299,245.36 | 0.4% |  10.00 |  20.23 | 0.494 |   0.00 | 99.9% |  7.13 | `glm::vec3 addition`                           |
|  0.52 | 1,934,013,854.33 | 0.1% |   6.00 |   2.00 | 2.995 |   0.00 | 98.8% |  0.71 | `glm::vec4 addition`                           |
|  0.52 | 1,935,172,838.35 | 0.1% |   4.00 |   2.00 | 1.999 |   0.00 | 98.7% |  0.71 | `DirectX::XMFLOAT2 addition`                   |
|  5.43 |   184,212,391.26 | 0.0% |  10.00 |  21.01 | 0.476 |   0.00 | 99.9% |  7.41 | `DirectX::XMFLOAT3 addition`                   |
|  0.52 | 1,931,793,165.53 | 0.1% |   4.00 |   2.00 | 1.997 |   0.00 | 98.7% |  0.71 | `DirectX::XMFLOAT4 addition without Loads`     |
|  0.26 | 3,864,558,428.76 | 0.1% |   3.00 |   1.00 | 2.998 |   0.00 | 97.4% |  0.35 | `Vectormath::Vector2 addition`                 |
|  0.52 | 1,932,457,604.11 | 0.1% |   4.00 |   2.00 | 1.997 |   0.00 | 98.9% |  0.71 | `Vectormath::Vector3 addition`                 |
|  0.52 | 1,932,365,508.21 | 0.2% |   4.00 |   2.00 | 1.997 |   0.00 | 98.8% |  0.71 | `Vectormath::Vector4 addition`                 |
|  1.03 |   966,701,105.17 | 0.1% |   5.00 |   4.01 | 1.248 |   0.00 | 99.4% |  1.41 | `Complex operation 1 with SimpleMath::Vector*` |
|  0.52 | 1,931,985,418.81 | 0.2% |   6.00 |   2.00 | 2.995 |   0.00 | 98.7% |  0.71 | `Complex operation 1 with glm::vec*`           |
|  0.78 | 1,288,115,784.22 | 0.1% |   5.00 |   3.00 | 1.664 |   0.00 | 99.1% |  1.06 | `Complex operation 1 with DXM`                 |
|  5.44 |   183,871,102.74 | 0.0% |  37.00 |  21.04 | 1.759 |   0.00 | 99.9% |  7.43 | `Complex operation 1 with DXM w/out loads`     |
|  0.78 | 1,287,753,754.86 | 0.1% |   4.00 |   3.01 | 1.331 |   0.00 | 99.2% |  1.06 | `Complex operation 1 with Vectormath`          |
|  5.17 |   193,261,482.26 | 0.0% |  12.00 |  20.01 | 0.600 |   0.00 | 99.9% |  7.06 | `Complex operation 2 with SimpleMath::Vector3` |
|  5.17 |   193,302,190.54 | 0.1% |  10.00 |  20.01 | 0.500 |   0.00 | 99.9% |  7.06 | `Complex operation 2 with glm::vec3`           |
|  5.43 |   184,121,857.91 | 0.0% |  10.00 |  21.01 | 0.476 |   0.00 | 99.9% |  7.42 | `Complex operation 2 with DXM`                 |
|  5.48 |   182,535,700.98 | 0.1% |  29.00 |  21.19 | 1.368 |   0.00 | 99.9% |  7.48 | `Complex operation 2 with DXM w/out loads`     |
|  0.78 | 1,286,691,662.73 | 0.1% |   4.00 |   3.01 | 1.330 |   0.00 | 99.2% |  1.06 | `Complex operation 2 with Vectormath`          |
|  1.04 |   965,440,213.01 | 0.1% |   6.00 |   4.01 | 1.497 |   0.00 | 99.4% |  1.41 | `Complex operation 3 with SimpleMath::Vector4` |
|  0.52 | 1,931,846,020.87 | 0.1% |   6.00 |   2.01 | 2.991 |   0.00 | 98.7% |  0.71 | `Complex operation 3 with glm::vec4`           |
|  0.52 | 1,931,199,290.69 | 0.1% |   6.00 |   2.00 | 2.993 |   0.00 | 98.7% |  0.71 | `Complex operation 3 with DXM`                 |
|  1.02 |   981,585,679.50 | 1.2% |  13.00 |   3.95 | 3.294 |   0.00 | 99.4% |  1.39 | `Complex operation 3 with DXM w/out loads`     |
|  0.78 | 1,281,198,996.91 | 0.4% |   5.00 |   3.02 | 1.658 |   0.00 | 99.2% |  1.08 | `Complex operation 3 with Vectormath`          |
| 37.06 |    26,984,774.60 | 0.1% | 327.00 | 143.23 | 2.283 |  10.00 |  0.0% |  5.93 | `Construct model matrix SimpleMath`            |
| 68.64 |    14,568,839.03 | 0.1% | 624.00 | 265.41 | 2.351 |  34.00 |  0.0% | 10.98 | `Construct model matrix glm`                   |
| 33.56 |    29,798,275.70 | 0.1% | 290.00 | 129.87 | 2.233 |  10.00 |  0.0% |  5.36 | `Construct model matrix DXM (manual)`          |
| 28.20 |    35,465,273.14 | 0.1% | 300.00 | 109.02 | 2.752 |   0.00 | 99.8% |  4.50 | `Construct model matrix DXM (builtin)`         |
| 32.61 |    30,662,495.50 | 0.1% | 385.00 | 125.26 | 3.074 |  10.00 |  0.0% |  5.20 | `Construct model matrix Vectormath`            |
|  3.12 |   320,797,407.06 | 0.4% |  21.00 |  12.02 | 1.747 |   0.00 | 99.3% |  0.50 | `Construct view matrix SimpleMath`             |
|  3.12 |   320,483,148.85 | 0.4% |  18.00 |  12.02 | 1.498 |   0.00 | 99.4% |  0.50 | `Construct view matrix glm`                    |
|  3.11 |   321,037,762.96 | 0.2% |  21.00 |  12.05 | 1.743 |   0.00 | 98.3% |  0.50 | `Construct view matrix DXM`                    |
| 12.28 |    81,464,778.37 | 0.1% |  45.00 |  47.45 | 0.948 |   0.00 | 99.6% |  1.96 | `Construct view matrix Vectormath`             |
|  3.14 |   318,659,716.22 | 0.3% |  18.00 |  12.13 | 1.484 |   0.00 | 98.2% |  0.51 | `Construct perspective matrix SimpleMath`      |
| 21.10 |    47,392,771.90 | 0.1% |  29.00 |  81.51 | 0.356 |   0.00 | 99.8% |  3.37 | `Construct perspective matrix glm`             |
|  3.12 |   320,897,591.97 | 0.1% |  18.00 |  12.05 | 1.493 |   0.00 | 98.1% |  0.50 | `Construct perspective matrix DXM`             |
|  1.04 |   962,905,624.29 | 0.2% |   8.00 |   4.02 | 1.990 |   0.00 | 94.1% |  0.17 | `Construct perspective matrix Vectormath`      |
|  3.13 |   319,387,523.65 | 0.3% |  18.00 |  12.10 | 1.487 |   0.00 | 98.1% |  0.50 | `Construct ortho matrix SimpleMath`            |
|  3.12 |   320,819,880.96 | 0.2% |  18.00 |  12.05 | 1.494 |   0.00 | 98.1% |  0.50 | `Construct ortho matrix glm`                   |
|  3.12 |   320,328,367.46 | 0.2% |  18.00 |  12.07 | 1.492 |   0.00 | 98.5% |  0.50 | `Construct ortho matrix DXM`                   |
| 39.72 |    25,173,457.96 | 0.0% |  32.00 | 153.52 | 0.208 |   0.00 | 99.9% |  6.34 | `Construct ortho matrix Vectormath`            |
| 12.06 |    82,944,827.84 | 0.2% |  17.00 |  46.58 | 0.365 |   0.00 | 99.6% |  1.92 | `Vector matrix multiply SimpleMath`            |
|  7.80 |   128,209,661.66 | 0.1% |  16.00 |  30.17 | 0.530 |   0.00 | 99.4% |  1.24 | `Vector matrix multiply glm`                   |
|  8.01 |   124,778,775.18 | 0.2% |  18.00 |  30.89 | 0.583 |   0.00 | 99.7% |  1.28 | `Vector matrix multiply DXM`                   |
|  8.52 |   117,363,880.53 | 0.2% |  16.00 |  32.87 | 0.487 |   0.00 | 99.7% |  1.36 | `Vector matrix multiply Vectormath`            |
| 15.26 |    65,532,384.55 | 0.7% |  76.00 |  58.84 | 1.292 |   0.00 | 99.7% |  2.41 | `Matrix matrix multiply SimpleMath`            |
|  5.01 |   199,427,026.80 | 0.9% |  72.00 |  19.35 | 3.722 |   0.00 | 99.0% |  0.81 | `Matrix matrix multiply glm`                   |
|  8.97 |   111,531,750.07 | 2.3% |  80.00 |  34.59 | 2.313 |   0.00 | 99.8% |  1.44 | `Matrix matrix multiply DXM`                   |
|  8.10 |   123,507,244.67 | 0.1% |  56.00 |  31.26 | 1.791 |   0.00 | 99.6% |  1.30 | `Matrix matrix multiply Vectormath`            |

### AVX2

| ns/op |             op/s | err% | ins/op | cyc/op |   IPC | bra/op | miss% | total | benchmark                                      |
| ----: | ---------------: | ---: | -----: | -----: | ----: | -----: | ----: | ----: | :--------------------------------------------- |
|  0.26 | 3,837,033,848.23 | 0.5% |   3.00 |   1.01 | 2.972 |   0.00 | 97.7% |  0.36 | `Store int (reference 'no-op')`                |
|  0.52 | 1,910,248,995.16 | 0.3% |   4.00 |   2.02 | 1.976 |   0.00 | 98.8% |  0.72 | `SimpleMath::Vector2 addition`                 |
|  5.46 |   183,219,963.79 | 0.1% |  10.00 |  21.09 | 0.474 |   0.00 | 99.9% |  7.45 | `SimpleMath::Vector3 addition`                 |
|  1.05 |   956,036,940.60 | 0.3% |   6.00 |   4.05 | 1.483 |   0.00 | 99.4% |  1.43 | `SimpleMath::Vector4 addition`                 |
|  0.26 | 3,830,205,434.14 | 0.7% |   3.00 |   1.01 | 2.980 |   0.00 | 97.4% |  0.36 | `glm::vec2 addition`                           |
|  5.28 |   189,516,240.85 | 0.2% |  10.00 |  20.40 | 0.490 |   0.00 | 99.9% |  7.20 | `glm::vec3 addition`                           |
|  0.52 | 1,905,922,822.83 | 0.2% |   6.00 |   2.03 | 2.953 |   0.00 | 98.8% |  0.72 | `glm::vec4 addition`                           |
|  0.52 | 1,920,975,421.24 | 0.3% |   4.00 |   2.01 | 1.988 |   0.00 | 98.8% |  0.71 | `DirectX::XMFLOAT2 addition`                   |
|  5.46 |   183,197,753.46 | 0.0% |  10.00 |  21.10 | 0.474 |   0.00 | 99.9% |  7.45 | `DirectX::XMFLOAT3 addition`                   |
|  0.52 | 1,926,072,516.09 | 0.2% |   4.00 |   2.01 | 1.992 |   0.00 | 98.9% |  0.71 | `DirectX::XMFLOAT4 addition without Loads`     |
|  0.26 | 3,801,010,330.90 | 0.5% |   3.00 |   1.02 | 2.954 |   0.00 | 97.7% |  0.36 | `Vectormath::Vector2 addition`                 |
|  0.52 | 1,909,833,699.01 | 0.4% |   4.00 |   2.02 | 1.976 |   0.00 | 98.8% |  0.72 | `Vectormath::Vector3 addition`                 |
|  0.52 | 1,905,783,696.78 | 0.4% |   4.00 |   2.03 | 1.969 |   0.00 | 98.7% |  0.72 | `Vectormath::Vector4 addition`                 |
|  1.05 |   955,898,478.37 | 0.3% |   5.00 |   4.04 | 1.237 |   0.00 | 99.4% |  1.44 | `Complex operation 1 with SimpleMath::Vector*` |
|  0.52 | 1,915,138,572.55 | 0.2% |   6.00 |   2.02 | 2.973 |   0.00 | 98.7% |  0.71 | `Complex operation 1 with glm::vec*`           |
|  0.78 | 1,281,991,960.44 | 0.3% |   5.00 |   3.01 | 1.663 |   0.00 | 99.6% |  1.07 | `Complex operation 1 with DXM`                 |
|  5.22 |   191,567,756.60 | 0.0% |  36.00 |  20.18 | 1.784 |   0.00 | 99.9% |  7.13 | `Complex operation 1 with DXM w/out loads`     |
|  0.78 | 1,280,782,233.51 | 0.2% |   4.00 |   3.01 | 1.330 |   0.00 | 99.7% |  1.07 | `Complex operation 1 with Vectormath`          |
|  5.21 |   192,067,633.81 | 0.1% |  12.00 |  20.02 | 0.599 |   0.00 | 99.9% |  7.09 | `Complex operation 2 with SimpleMath::Vector3` |
|  5.20 |   192,447,887.78 | 0.0% |  10.00 |  20.08 | 0.498 |   0.00 | 99.9% |  7.10 | `Complex operation 2 with glm::vec3`           |
|  5.43 |   184,069,714.17 | 0.1% |  10.00 |  21.01 | 0.476 |   0.00 | 99.9% |  7.43 | `Complex operation 2 with DXM`                 |
|  5.49 |   182,060,627.00 | 0.1% |  28.00 |  21.22 | 1.319 |   0.00 | 99.9% |  7.51 | `Complex operation 2 with DXM w/out loads`     |
|  0.78 | 1,283,981,423.09 | 0.1% |   4.00 |   3.01 | 1.328 |   0.00 | 99.3% |  1.06 | `Complex operation 2 with Vectormath`          |
|  1.04 |   964,329,235.20 | 0.2% |   6.00 |   4.01 | 1.496 |   0.00 | 99.4% |  1.42 | `Complex operation 3 with SimpleMath::Vector4` |
|  0.52 | 1,923,968,811.94 | 0.1% |   6.00 |   2.01 | 2.988 |   0.00 | 98.8% |  0.71 | `Complex operation 3 with glm::vec4`           |
|  0.52 | 1,924,421,309.11 | 0.2% |   6.00 |   2.01 | 2.986 |   0.00 | 98.7% |  0.71 | `Complex operation 3 with DXM`                 |
|  1.05 |   950,898,473.95 | 0.5% |  13.00 |   4.07 | 3.198 |   0.00 | 99.5% |  1.45 | `Complex operation 3 with DXM w/out loads`     |
|  0.78 | 1,284,413,748.66 | 0.1% |   5.00 |   3.01 | 1.662 |   0.00 | 99.2% |  1.06 | `Complex operation 3 with Vectormath`          |
| 37.03 |    27,007,466.92 | 0.1% | 275.00 | 143.15 | 1.921 |  10.00 |  0.0% |  5.91 | `Construct model matrix SimpleMath`            |
| 70.22 |    14,240,212.55 | 0.5% | 561.00 | 270.80 | 2.072 |  34.00 |  0.0% | 11.19 | `Construct model matrix glm`                   |
| 35.88 |    27,870,751.39 | 0.0% | 224.00 | 138.82 | 1.614 |   8.00 |  0.0% |  5.72 | `Construct model matrix DXM (manual)`          |
|  3.11 |   322,033,189.96 | 0.2% |  17.00 |  12.02 | 1.415 |   0.00 | 98.3% |  0.50 | `Construct model matrix DXM (builtin)`         |
| 29.37 |    34,050,547.48 | 0.0% | 331.00 | 113.63 | 2.913 |  10.00 |  0.0% |  4.68 | `Construct model matrix Vectormath`            |
|  3.10 |   322,248,151.24 | 0.1% |  27.00 |  12.02 | 2.247 |   0.00 | 98.3% |  0.50 | `Construct view matrix SimpleMath`             |
|  3.11 |   322,021,023.23 | 0.2% |  18.00 |  12.02 | 1.498 |   0.00 | 98.2% |  0.50 | `Construct view matrix glm`                    |
|  3.10 |   322,170,054.20 | 0.1% |  27.00 |  12.02 | 2.247 |   0.00 | 98.4% |  0.50 | `Construct view matrix DXM`                    |
| 17.46 |    57,278,921.43 | 0.0% |  41.00 |  67.56 | 0.607 |   0.00 | 99.7% |  2.79 | `Construct view matrix Vectormath`             |
|  3.10 |   322,613,504.87 | 0.1% |  18.00 |  12.02 | 1.498 |   0.00 | 98.3% |  0.49 | `Construct perspective matrix SimpleMath`      |
| 20.93 |    47,772,440.85 | 0.0% |  29.00 |  81.03 | 0.358 |   0.00 | 99.7% |  3.34 | `Construct perspective matrix glm`             |
|  3.11 |   321,728,689.88 | 0.2% |  18.00 |  12.02 | 1.498 |   0.00 | 98.3% |  0.50 | `Construct perspective matrix DXM`             |
|  1.04 |   964,284,563.79 | 0.1% |   8.00 |   4.01 | 1.997 |   0.00 | 94.4% |  0.17 | `Construct perspective matrix Vectormath`      |
|  3.11 |   321,552,311.31 | 0.2% |  18.00 |  12.02 | 1.498 |   0.00 | 98.1% |  0.50 | `Construct ortho matrix SimpleMath`            |
|  3.10 |   322,315,184.20 | 0.0% |  18.00 |  12.02 | 1.498 |   0.00 | 98.2% |  0.50 | `Construct ortho matrix glm`                   |
|  3.11 |   321,642,728.96 | 0.1% |  18.00 |  12.02 | 1.498 |   0.00 | 98.1% |  0.50 | `Construct ortho matrix DXM`                   |
| 39.58 |    25,265,700.97 | 0.0% |  32.00 | 153.08 | 0.209 |   0.00 | 99.9% |  6.31 | `Construct ortho matrix Vectormath`            |
| 13.23 |    75,578,273.54 | 0.1% |  14.00 |  51.20 | 0.273 |   0.00 | 99.6% |  2.11 | `Vector matrix multiply SimpleMath`            |
|  7.88 |   126,852,260.21 | 0.3% |  14.00 |  30.53 | 0.459 |   0.00 | 99.3% |  1.26 | `Vector matrix multiply glm`                   |
|  9.68 |   103,309,538.91 | 0.1% |  15.00 |  37.46 | 0.400 |   0.00 | 99.4% |  1.54 | `Vector matrix multiply DXM`                   |
|  8.32 |   120,163,006.09 | 0.0% |  14.00 |  32.22 | 0.434 |   0.00 | 99.4% |  1.33 | `Vector matrix multiply Vectormath`            |
| 10.21 |    97,949,640.06 | 0.3% |  46.00 |  39.48 | 1.165 |   0.00 | 99.5% |  1.63 | `Matrix matrix multiply SimpleMath`            |
| 15.11 |    66,200,169.16 | 0.1% |  55.00 |  58.49 | 0.940 |   0.00 | 99.6% |  2.41 | `Matrix matrix multiply glm`                   |
| 10.14 |    98,625,068.93 | 0.1% |  46.00 |  39.24 | 1.172 |   0.00 | 99.5% |  1.62 | `Matrix matrix multiply DXM`                   |
|  6.97 |   143,408,878.70 | 0.2% |  44.00 |  26.95 | 1.632 |   0.00 | 99.2% |  1.11 | `Matrix matrix multiply Vectormath`            |

## Intel XEON

The following measurements were taken on a Vultr High Frequency instance (shared vCPU - lscpu shows `Intel Core Processor (Skylake, IBRS)`) using Debian 11.

> Note: I'm not entirely sure why only a subset of the metrics from the AMD processor are available.

### SSE 4.2

|  ns/op |             op/s | err% | total | benchmark                                      |
| -----: | ---------------: | ---: | ----: | :--------------------------------------------- |
|   0.56 | 1,795,629,673.71 | 0.9% |  0.80 | `Store int (reference 'no-op')`                |
|   0.59 | 1,704,744,271.86 | 4.6% |  0.84 | `SimpleMath::Vector2 addition`                 |
|   1.65 |   605,502,584.19 | 0.3% |  2.28 | `SimpleMath::Vector3 addition`                 |
|   0.90 | 1,108,981,061.70 | 2.2% |  1.21 | `SimpleMath::Vector4 addition`                 |
|   0.55 | 1,816,220,574.31 | 0.1% |  0.75 | `glm::vec2 addition`                           |
|   5.57 |   179,423,503.88 | 1.4% |  7.64 | `glm::vec3 addition`                           |
|   0.86 | 1,159,888,489.34 | 2.8% |  1.20 | `glm::vec4 addition`                           |
|   0.56 | 1,775,924,343.73 | 0.6% |  0.77 | `DirectX::XMFLOAT2 addition`                   |
|   1.67 |   597,316,272.16 | 0.9% |  2.36 | `DirectX::XMFLOAT3 addition`                   |
|   0.55 | 1,816,202,060.83 | 0.3% |  0.75 | `DirectX::XMFLOAT4 addition without Loads`     |
|   0.55 | 1,801,826,387.42 | 0.6% |  0.77 | `Vectormath::Vector2 addition`                 |
|   0.57 | 1,741,153,870.39 | 1.0% |  0.79 | `Vectormath::Vector3 addition`                 |
|   0.58 | 1,718,224,513.58 | 1.6% |  0.79 | `Vectormath::Vector4 addition`                 |
|   0.83 | 1,200,876,926.61 | 0.7% |  1.14 | `Complex operation 1 with SimpleMath::Vector*` |
|   0.83 | 1,205,578,371.56 | 0.2% |  1.13 | `Complex operation 1 with glm::vec*`           |
|   0.82 | 1,213,214,701.77 | 0.2% |  1.13 | `Complex operation 1 with DXM`                 |
|   5.52 |   181,028,247.15 | 1.1% |  7.56 | `Complex operation 1 with DXM w/out loads`     |
|   0.84 | 1,185,920,642.66 | 0.7% |  1.15 | `Complex operation 1 with Vectormath`          |
|   1.68 |   595,275,558.58 | 0.7% |  2.29 | `Complex operation 2 with SimpleMath::Vector3` |
|   5.50 |   181,729,512.95 | 0.3% |  7.52 | `Complex operation 2 with glm::vec3`           |
|   1.65 |   605,186,044.17 | 0.5% |  2.26 | `Complex operation 2 with DXM`                 |
|   2.80 |   357,326,045.93 | 0.5% |  3.83 | `Complex operation 2 with DXM w/out loads`     |
|   0.84 | 1,192,362,920.02 | 0.8% |  1.16 | `Complex operation 2 with Vectormath`          |
|   0.84 | 1,192,060,905.52 | 1.1% |  1.16 | `Complex operation 3 with SimpleMath::Vector4` |
|   0.89 | 1,128,972,561.68 | 1.3% |  1.21 | `Complex operation 3 with glm::vec4`           |
|   0.91 | 1,103,515,718.83 | 1.3% |  1.24 | `Complex operation 3 with DXM`                 |
|   1.41 |   710,820,214.73 | 0.4% |  1.93 | `Complex operation 3 with DXM w/out loads`     |
|   0.84 | 1,188,643,775.45 | 1.3% |  1.15 | `Complex operation 3 with Vectormath`          |
|  34.31 |    29,147,951.11 | 0.2% |  0.01 | `Construct model matrix SimpleMath`            |
|  87.43 |    11,437,904.76 | 0.8% |  0.01 | `Construct model matrix glm`                   |
|  32.78 |    30,504,218.30 | 0.4% |  0.01 | `Construct model matrix DXM (manual)`          |
|   3.30 |   303,404,002.68 | 0.8% |  0.01 | `Construct model matrix DXM (builtin)`         |
|  42.58 |    23,484,500.23 | 0.3% |  0.01 | `Construct model matrix Vectormath`            |
|   3.88 |   257,580,230.17 | 1.1% |  0.01 | `Construct view matrix SimpleMath`             |
|   3.28 |   304,633,299.50 | 0.3% |  0.01 | `Construct view matrix glm`                    |
|   3.28 |   304,744,546.92 | 0.3% |  0.01 | `Construct view matrix DXM`                    |
|  14.09 |    70,991,448.68 | 0.3% |  0.01 | `Construct view matrix Vectormath`             |
|   3.88 |   257,670,622.12 | 1.2% |  0.01 | `Construct perspective matrix SimpleMath`      |
|  18.63 |    53,673,170.37 | 0.5% |  0.01 | `Construct perspective matrix glm`             |
|   3.29 |   303,780,396.08 | 0.3% |  0.01 | `Construct perspective matrix DXM`             |
|   1.09 |   916,012,073.49 | 0.1% |  0.01 | `Construct perspective matrix Vectormath`      |
|   3.84 |   260,363,107.04 | 0.3% |  0.01 | `Construct ortho matrix SimpleMath`            |
|   3.28 |   305,058,274.98 | 0.4% |  0.01 | `Construct ortho matrix glm`                   |
|   3.30 |   302,710,441.87 | 0.8% |  0.01 | `Construct ortho matrix DXM`                   |
|  31.62 |    31,626,159.30 | 0.5% |  0.01 | `Construct ortho matrix Vectormath`            |
|  10.19 |    98,182,287.50 | 0.3% |  0.01 | `Vector matrix multiply SimpleMath`            |
|   9.60 |   104,207,096.88 | 0.6% |  0.01 | `Vector matrix multiply glm`                   |
|   6.36 |   157,167,563.36 | 0.5% |  0.01 | `Vector matrix multiply DXM`                   |
|   7.35 |   136,013,836.43 | 0.5% |  0.01 | `Vector matrix multiply Vectormath`            |
| 274.57 |     3,642,024.61 | 0.4% |  0.01 | `Matrix matrix multiply SimpleMath`            |
| 238.01 |     4,201,502.73 | 0.4% |  0.01 | `Matrix matrix multiply glm`                   |
| 273.32 |     3,658,673.09 | 0.6% |  0.01 | `Matrix matrix multiply DXM`                   |
|   8.65 |   115,637,830.64 | 0.9% |  0.01 | `Matrix matrix multiply Vectormath`            |

### AVX

|  ns/op |             op/s | err% | total | benchmark                                      |
| -----: | ---------------: | ---: | ----: | :--------------------------------------------- |
|   0.56 | 1,792,211,884.46 | 0.3% |  0.76 | `Store int (reference 'no-op')`                |
|   0.56 | 1,780,009,289.26 | 0.3% |  0.77 | `SimpleMath::Vector2 addition`                 |
|   5.52 |   181,085,309.13 | 0.5% |  7.57 | `SimpleMath::Vector3 addition`                 |
|   0.83 | 1,197,663,014.62 | 0.9% |  1.14 | `SimpleMath::Vector4 addition`                 |
|   0.57 | 1,760,299,083.89 | 2.6% |  0.78 | `glm::vec2 addition`                           |
|   5.53 |   180,980,387.37 | 0.4% |  7.57 | `glm::vec3 addition`                           |
|   0.92 | 1,081,559,150.06 | 1.1% |  1.24 | `glm::vec4 addition`                           |
|   0.85 | 1,175,942,608.37 | 1.0% |  1.16 | `DirectX::XMFLOAT2 addition`                   |
|   5.51 |   181,363,129.41 | 0.3% |  7.54 | `DirectX::XMFLOAT3 addition`                   |
|   0.56 | 1,789,275,716.58 | 0.5% |  0.77 | `DirectX::XMFLOAT4 addition without Loads`     |
|   0.55 | 1,808,984,007.95 | 0.1% |  0.76 | `Vectormath::Vector2 addition`                 |
|   0.55 | 1,810,698,759.94 | 0.2% |  0.75 | `Vectormath::Vector3 addition`                 |
|   0.55 | 1,815,452,991.95 | 0.3% |  0.77 | `Vectormath::Vector4 addition`                 |
|   0.83 | 1,211,591,792.24 | 0.3% |  1.13 | `Complex operation 1 with SimpleMath::Vector*` |
|   0.83 | 1,207,781,768.33 | 0.5% |  1.14 | `Complex operation 1 with glm::vec*`           |
|   0.83 | 1,209,218,062.04 | 0.1% |  1.13 | `Complex operation 1 with DXM`                 |
|   4.54 |   220,178,648.25 | 0.1% |  6.25 | `Complex operation 1 with DXM w/out loads`     |
|   0.85 | 1,171,132,369.19 | 3.3% |  1.17 | `Complex operation 1 with Vectormath`          |
|   5.24 |   191,005,767.70 | 0.3% |  7.16 | `Complex operation 2 with SimpleMath::Vector3` |
|   5.52 |   181,156,737.16 | 0.2% |  7.54 | `Complex operation 2 with glm::vec3`           |
|   5.50 |   181,927,014.25 | 0.3% |  7.54 | `Complex operation 2 with DXM`                 |
|   6.06 |   165,031,909.29 | 0.1% |  8.28 | `Complex operation 2 with DXM w/out loads`     |
|   0.83 | 1,206,201,224.00 | 0.3% |  1.14 | `Complex operation 2 with Vectormath`          |
|   0.82 | 1,212,146,917.83 | 0.2% |  1.13 | `Complex operation 3 with SimpleMath::Vector4` |
|   0.82 | 1,215,607,516.12 | 0.3% |  1.13 | `Complex operation 3 with glm::vec4`           |
|   0.83 | 1,209,651,861.58 | 0.2% |  1.13 | `Complex operation 3 with DXM`                 |
|   1.21 |   825,837,366.46 | 0.3% |  1.65 | `Complex operation 3 with DXM w/out loads`     |
|   0.85 | 1,180,622,845.97 | 1.4% |  1.16 | `Complex operation 3 with Vectormath`          |
|  38.32 |    26,099,220.23 | 3.4% |  0.01 | `Construct model matrix SimpleMath`            |
|  85.20 |    11,736,471.43 | 1.4% |  0.01 | `Construct model matrix glm`                   |
|  35.00 |    28,570,084.19 | 1.3% |  0.01 | `Construct model matrix DXM (manual)`          |
|  29.10 |    34,364,731.20 | 1.4% |  0.01 | `Construct model matrix DXM (builtin)`         |
|  43.32 |    23,083,522.33 | 1.2% |  0.01 | `Construct model matrix Vectormath`            |
|   3.81 |   262,140,753.79 | 0.1% |  0.01 | `Construct view matrix SimpleMath`             |
|   3.28 |   304,823,557.32 | 0.4% |  0.01 | `Construct view matrix glm`                    |
|   3.30 |   302,755,819.50 | 1.0% |  0.01 | `Construct view matrix DXM`                    |
|  11.30 |    88,464,483.71 | 0.2% |  0.01 | `Construct view matrix Vectormath`             |
|   3.82 |   261,803,703.24 | 0.2% |  0.01 | `Construct perspective matrix SimpleMath`      |
|  18.61 |    53,729,285.74 | 0.7% |  0.01 | `Construct perspective matrix glm`             |
|   3.30 |   302,989,574.42 | 0.8% |  0.01 | `Construct perspective matrix DXM`             |
|   1.10 |   909,039,475.53 | 1.0% |  0.01 | `Construct perspective matrix Vectormath`      |
|   3.83 |   261,255,979.94 | 0.4% |  0.01 | `Construct ortho matrix SimpleMath`            |
|   3.30 |   303,154,822.97 | 0.9% |  0.01 | `Construct ortho matrix glm`                   |
|   3.28 |   304,849,445.26 | 0.4% |  0.01 | `Construct ortho matrix DXM`                   |
|  30.97 |    32,284,829.74 | 0.5% |  0.01 | `Construct ortho matrix Vectormath`            |
|  10.14 |    98,572,860.23 | 0.2% |  0.01 | `Vector matrix multiply SimpleMath`            |
|   6.18 |   161,780,272.33 | 1.2% |  0.01 | `Vector matrix multiply glm`                   |
|   6.41 |   156,016,226.02 | 0.2% |  0.01 | `Vector matrix multiply DXM`                   |
|   7.35 |   136,005,400.65 | 0.4% |  0.01 | `Vector matrix multiply Vectormath`            |
| 272.52 |     3,669,411.01 | 0.4% |  0.01 | `Matrix matrix multiply SimpleMath`            |
| 163.47 |     6,117,194.06 | 0.6% |  0.01 | `Matrix matrix multiply glm`                   |
| 347.10 |     2,881,028.28 | 0.7% |  0.01 | `Matrix matrix multiply DXM`                   |
|   7.71 |   129,629,707.51 | 0.1% |  0.01 | `Matrix matrix multiply Vectormath`            |

### AVX2

|  ns/op |             op/s | err% | total | benchmark                                      |
| -----: | ---------------: | ---: | ----: | :--------------------------------------------- |
|   0.58 | 1,731,576,047.78 | 1.0% |  0.80 | `Store int (reference 'no-op')`                |
|   0.56 | 1,798,789,530.99 | 1.0% |  0.78 | `SimpleMath::Vector2 addition`                 |
|   5.54 |   180,567,694.84 | 0.6% |  7.59 | `SimpleMath::Vector3 addition`                 |
|   0.84 | 1,186,748,171.70 | 0.5% |  1.15 | `SimpleMath::Vector4 addition`                 |
|   0.56 | 1,781,843,998.37 | 0.1% |  0.77 | `glm::vec2 addition`                           |
|   5.51 |   181,620,873.18 | 0.3% |  7.52 | `glm::vec3 addition`                           |
|   0.82 | 1,216,205,171.89 | 0.3% |  1.13 | `glm::vec4 addition`                           |
|   0.56 | 1,797,635,102.19 | 0.2% |  0.76 | `DirectX::XMFLOAT2 addition`                   |
|   5.51 |   181,630,237.65 | 0.3% |  7.52 | `DirectX::XMFLOAT3 addition`                   |
|   0.56 | 1,801,434,210.81 | 0.4% |  0.76 | `DirectX::XMFLOAT4 addition without Loads`     |
|   0.55 | 1,803,608,316.29 | 0.4% |  0.77 | `Vectormath::Vector2 addition`                 |
|   0.88 | 1,133,905,162.51 | 2.3% |  1.21 | `Vectormath::Vector3 addition`                 |
|   0.88 | 1,140,535,971.06 | 3.1% |  1.20 | `Vectormath::Vector4 addition`                 |
|   0.85 | 1,171,650,913.16 | 1.4% |  1.17 | `Complex operation 1 with SimpleMath::Vector*` |
|   0.85 | 1,181,514,809.63 | 0.7% |  1.16 | `Complex operation 1 with glm::vec*`           |
|   0.84 | 1,189,028,043.74 | 0.7% |  1.16 | `Complex operation 1 with DXM`                 |
|   4.46 |   224,142,068.11 | 0.4% |  6.08 | `Complex operation 1 with DXM w/out loads`     |
|   0.85 | 1,179,694,359.64 | 1.3% |  1.16 | `Complex operation 1 with Vectormath`          |
|   5.28 |   189,364,203.64 | 0.6% |  7.22 | `Complex operation 2 with SimpleMath::Vector3` |
|   5.56 |   179,740,371.22 | 0.6% |  7.60 | `Complex operation 2 with glm::vec3`           |
|   5.51 |   181,458,996.55 | 0.3% |  7.53 | `Complex operation 2 with DXM`                 |
|   5.79 |   172,784,756.72 | 0.1% |  7.90 | `Complex operation 2 with DXM w/out loads`     |
|   0.84 | 1,191,310,035.61 | 0.3% |  1.15 | `Complex operation 2 with Vectormath`          |
|   0.86 | 1,162,707,789.28 | 2.1% |  1.20 | `Complex operation 3 with SimpleMath::Vector4` |
|   0.89 | 1,124,126,024.28 | 2.9% |  1.22 | `Complex operation 3 with glm::vec4`           |
|   0.89 | 1,123,190,657.87 | 2.1% |  1.21 | `Complex operation 3 with DXM`                 |
|   1.24 |   806,186,337.11 | 3.6% |  1.74 | `Complex operation 3 with DXM w/out loads`     |
|   0.86 | 1,166,014,689.77 | 2.6% |  1.19 | `Complex operation 3 with Vectormath`          |
|  37.30 |    26,810,487.84 | 0.9% |  0.01 | `Construct model matrix SimpleMath`            |
|  82.11 |    12,178,477.11 | 0.4% |  0.01 | `Construct model matrix glm`                   |
|  33.18 |    30,135,069.19 | 0.7% |  0.01 | `Construct model matrix DXM (manual)`          |
|   3.57 |   280,301,065.10 | 0.6% |  0.01 | `Construct model matrix DXM (builtin)`         |
|  38.09 |    26,250,796.89 | 0.7% |  0.01 | `Construct model matrix Vectormath`            |
|   4.13 |   241,897,834.69 | 0.2% |  0.01 | `Construct view matrix SimpleMath`             |
|   3.59 |   278,347,336.37 | 1.2% |  0.01 | `Construct view matrix glm`                    |
|   3.62 |   276,546,659.52 | 1.1% |  0.01 | `Construct view matrix DXM`                    |
|  15.36 |    65,124,393.92 | 0.5% |  0.01 | `Construct view matrix Vectormath`             |
|   4.17 |   240,094,662.01 | 0.8% |  0.01 | `Construct perspective matrix SimpleMath`      |
|  19.41 |    51,526,435.57 | 0.7% |  0.01 | `Construct perspective matrix glm`             |
|   3.58 |   279,426,185.65 | 0.8% |  0.01 | `Construct perspective matrix DXM`             |
|   1.19 |   837,243,482.30 | 0.8% |  0.01 | `Construct perspective matrix Vectormath`      |
|   4.18 |   238,984,105.77 | 0.8% |  0.01 | `Construct ortho matrix SimpleMath`            |
|   3.55 |   281,880,296.04 | 1.0% |  0.01 | `Construct ortho matrix glm`                   |
|   3.55 |   281,746,038.75 | 0.7% |  0.01 | `Construct ortho matrix DXM`                   |
|  31.72 |    31,530,458.57 | 0.6% |  0.01 | `Construct ortho matrix Vectormath`            |
|  10.00 |    99,959,797.31 | 0.3% |  0.01 | `Vector matrix multiply SimpleMath`            |
|   5.85 |   171,037,925.34 | 0.5% |  0.01 | `Vector matrix multiply glm`                   |
|   6.16 |   162,279,953.71 | 0.5% |  0.01 | `Vector matrix multiply DXM`                   |
|   7.18 |   139,203,511.02 | 0.3% |  0.01 | `Vector matrix multiply Vectormath`            |
| 312.03 |     3,204,845.31 | 0.2% |  0.01 | `Matrix matrix multiply SimpleMath`            |
| 315.86 |     3,165,992.44 | 0.5% |  0.01 | `Matrix matrix multiply glm`                   |
| 313.55 |     3,189,316.71 | 0.4% |  0.01 | `Matrix matrix multiply DXM`                   |
|   7.12 |   140,397,245.89 | 0.9% |  0.01 | `Matrix matrix multiply Vectormath`            |