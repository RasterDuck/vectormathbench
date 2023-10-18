# Benchmark Results 
 
## System Info 
CPU: AMD Ryzen 9 3900X 12-Core Processor
OS: Microsoft Windows 10 Pro
OS Version: 10.0.19045 N/A Build 19045
 
## SSE 4.2 

|  ns/op |           op/s | err% | total | benchmark                                                  |
| -----: | -------------: | ---: | ----: | :--------------------------------------------------------- |
|   1.23 | 812,294,189.97 | 0.2% |  0.15 | `Store int (reference 'no-op')`                            |
|   1.24 | 806,681,690.76 | 0.2% |  0.15 | `SimpleMath::Vector2 addition`                             |
|   1.49 | 670,751,175.07 | 0.1% |  0.18 | `SimpleMath::Vector3 addition`                             |
|   1.50 | 666,936,407.81 | 0.1% |  0.18 | `SimpleMath::Vector4 addition`                             |
|   1.24 | 805,544,331.03 | 0.1% |  0.15 | `glm::vec2 addition`                                       |
|   1.24 | 803,305,117.81 | 0.4% |  0.15 | `glm::vec3 addition`                                       |
|   1.24 | 804,434,317.28 | 0.2% |  0.15 | `glm::vec4 addition`                                       |
|   1.24 | 806,659,762.87 | 0.5% |  0.15 | `DirectX::XMFLOAT2 addition`                               |
|   1.24 | 805,695,250.51 | 0.2% |  0.15 | `DirectX::XMFLOAT3 addition`                               |
|   1.23 | 810,872,790.90 | 0.3% |  0.15 | `DirectX::XMFLOAT4 addition without Loads`                 |
|   1.23 | 811,308,664.39 | 0.2% |  0.15 | `Vectormath::Vector2 addition`                             |
|   1.24 | 805,375,743.16 | 0.4% |  0.15 | `Vectormath::Vector3 addition`                             |
|   1.24 | 805,813,193.40 | 0.2% |  0.15 | `Vectormath::Vector4 addition`                             |
|   1.25 | 799,948,503.48 | 0.1% |  0.15 | `move::vec2 addition`                                      |
|   1.25 | 801,449,019.83 | 0.3% |  0.15 | `move::vec3 addition`                                      |
|   1.24 | 805,372,735.07 | 0.2% |  0.15 | `move::vec4 addition`                                      |
|   1.49 | 671,892,687.33 | 0.1% |  0.18 | `move::fastvec2 addition`                                  |
|   1.49 | 671,014,842.85 | 0.1% |  0.18 | `move::fastvec3 addition`                                  |
|   1.24 | 809,660,220.17 | 0.1% |  0.15 | `move::fastvec4 addition`                                  |
|   1.24 | 806,182,945.23 | 0.1% |  0.15 | `rtm::vector4f addition`                                   |
|   1.24 | 806,700,062.76 | 0.1% |  0.15 | `rtm::vector4d addition`                                   |
|   4.53 | 220,910,477.15 | 0.2% |  0.54 | `Complex operation 1 with SimpleMath::Vector*`             |
|   1.24 | 804,733,378.75 | 0.2% |  0.15 | `Complex operation 1 with glm::vec*`                       |
|   3.00 | 333,353,021.45 | 0.1% |  0.36 | `Complex operation 1 with DXM`                             |
|   3.92 | 255,304,373.08 | 0.1% |  0.47 | `Complex operation 1 with DXM w/out loads`                 |
|   1.24 | 804,600,863.77 | 0.2% |  0.15 | `Complex operation 1 with Vectormath`                      |
|   3.25 | 307,236,162.29 | 0.2% |  0.39 | `Complex operation 1 with move::vectormath::*`             |
|   2.75 | 363,959,202.49 | 0.1% |  0.33 | `Complex operation 1 with move::vectormath::fast*`         |
|   3.69 | 270,766,450.78 | 0.1% |  0.44 | `Complex operation 2 with SimpleMath::Vector3`             |
|   1.25 | 800,722,770.39 | 0.1% |  0.15 | `Complex operation 2 with glm::vec3`                       |
|   1.50 | 666,864,119.42 | 0.1% |  0.18 | `Complex operation 2 with DXM`                             |
|   1.76 | 567,911,231.02 | 0.2% |  0.21 | `Complex operation 2 with DXM w/out loads`                 |
|   1.24 | 804,761,714.68 | 0.2% |  0.15 | `Complex operation 2 with Vectormath`                      |
|   2.75 | 363,918,673.53 | 0.3% |  0.33 | `Complex operation 2 with move::vectormath`                |
|   1.25 | 796,993,925.48 | 0.2% |  0.15 | `Complex operation 2 with move::vectormath::fast*`         |
|   5.68 | 175,955,407.16 | 0.1% |  0.68 | `Complex operation 3 with SimpleMath::Vector4`             |
|   1.48 | 676,926,832.72 | 0.2% |  0.18 | `Complex operation 3 with glm::vec4`                       |
|   1.23 | 809,891,655.92 | 0.2% |  0.15 | `Complex operation 3 with DXM`                             |
|   1.50 | 668,008,687.28 | 0.3% |  0.18 | `Complex operation 3 with DXM w/out loads`                 |
|   1.49 | 671,780,195.40 | 0.1% |  0.18 | `Complex operation 3 with Vectormath`                      |
|   1.24 | 809,533,162.13 | 0.3% |  0.15 | `Complex operation 3 with move::vectormath`                |
|   1.23 | 809,873,227.71 | 0.1% |  0.15 | `Complex operation 3 with move::vectormath::fast*`         |
|   1.23 | 810,924,555.98 | 0.1% |  0.15 | `Complex operation 3 with rtm::vector4f`                   |
|   1.24 | 805,777,636.48 | 0.1% |  0.15 | `Complex operation 3 with rtm::vector4d`                   |
|  49.23 |  20,313,182.68 | 0.1% |  5.88 | `Construct model matrix SimpleMath`                        |
| 161.74 |   6,182,756.28 | 0.1% | 19.32 | `Construct model matrix glm`                               |
|  49.32 |  20,273,862.48 | 0.2% |  5.88 | `Construct model matrix DXM (manual)`                      |
|  42.31 |  23,636,789.53 | 0.3% |  5.04 | `Construct model matrix DXM (builtin)`                     |
|  21.77 |  45,940,654.15 | 0.5% |  2.60 | `Construct model matrix Vectormath`                        |
|  52.01 |  19,225,810.32 | 0.1% |  6.22 | `Construct model matrix move::vectormath::fast* (manual)`  |
|  43.74 |  22,863,380.58 | 0.2% |  5.22 | `Construct model matrix move::vectormath::fast* (builtin)` |
|  51.84 |  19,289,029.63 | 0.1% |  6.19 | `Construct model matrix move::vectormath (manual)`         |
|  43.71 |  22,876,526.87 | 0.1% |  5.22 | `Construct model matrix move::vectormath (builtin)`        |
|   1.23 | 810,384,934.36 | 0.3% |  0.15 | `Construct rtm::qvvf (~transformation matrix)`             |
|   1.97 | 507,524,043.95 | 0.2% |  0.24 | `Construct rtm::qvvd (~transformation matrix)`             |
|   7.21 | 138,761,428.49 | 0.2% |  0.86 | `Construct model matrix rtm::matrix3x4f`                   |
|  10.99 |  91,015,892.76 | 0.2% |  1.31 | `Construct model matrix rtm::matrix3x4d`                   |
|   8.00 | 125,015,034.63 | 0.1% |  0.95 | `Construct model matrix rtm::matrix4x4f`                   |
|  10.79 |  92,647,344.85 | 0.3% |  1.29 | `Construct model matrix rtm::matrix4x4d`                   |
|  27.10 |  36,906,023.55 | 0.2% |  3.24 | `Construct view matrix SimpleMath`                         |
|  21.65 |  46,190,596.96 | 0.2% |  2.59 | `Construct view matrix glm`                                |
|  25.22 |  39,658,267.44 | 0.2% |  3.01 | `Construct view matrix DXM`                                |
|  29.17 |  34,277,285.77 | 0.1% |  3.48 | `Construct view matrix Vectormath`                         |
|  25.26 |  39,595,901.70 | 0.1% |  3.01 | `Construct view matrix move::vectormath::fast*`            |
|  25.22 |  39,652,677.58 | 0.2% |  3.01 | `Construct view matrix move::vectormath`                   |
|   1.48 | 674,837,122.00 | 0.1% |  0.18 | `Construct view matrix rtm::matrix4x4f`                    |
|  39.11 |  25,568,053.24 | 0.3% |  4.67 | `Construct view matrix rtm::matrix4x4d`                    |
|  14.62 |  68,410,695.34 | 0.2% |  1.75 | `Construct perspective matrix SimpleMath`                  |
|   2.23 | 448,402,285.90 | 0.3% |  0.27 | `Construct perspective matrix glm`                         |
|  14.92 |  67,013,246.74 | 0.2% |  1.78 | `Construct perspective matrix DXM`                         |
|  11.77 |  84,980,924.71 | 0.5% |  1.40 | `Construct perspective matrix Vectormath`                  |
|  14.91 |  67,086,427.33 | 0.2% |  1.78 | `Construct perspective matrix move::vectormath::fast*`     |
|  14.92 |  67,036,371.54 | 0.1% |  1.78 | `Construct perspective matrix move::vectormath`            |
|  13.19 |  75,796,425.04 | 0.2% |  1.58 | `Construct perspective matrix rtm::matrix4x4f`             |
|   2.48 | 403,082,580.69 | 0.1% |  0.30 | `Construct perspective matrix rtm::matrix4x4d`             |
|   1.99 | 503,465,769.91 | 0.1% |  0.24 | `Construct ortho matrix SimpleMath`                        |
|   1.49 | 670,981,465.86 | 0.2% |  0.18 | `Construct ortho matrix glm`                               |
|   1.51 | 661,551,816.29 | 1.0% |  0.18 | `Construct ortho matrix DXM`                               |
|  15.85 |  63,097,409.68 | 0.2% |  1.90 | `Construct ortho matrix Vectormath`                        |
|   1.76 | 568,455,150.90 | 0.2% |  0.21 | `Construct ortho matrix move::vectormath::fast*`           |
|   2.02 | 496,071,487.39 | 0.3% |  0.24 | `Construct ortho matrix move::vectormath`                  |
|   1.49 | 670,471,926.54 | 0.3% |  0.18 | `Construct ortho matrix rtm::matrix4x4f`                   |
|   2.50 | 400,761,516.94 | 0.2% |  0.30 | `Construct ortho matrix rtm::matrix4x4d`                   |
|   9.80 | 102,035,227.54 | 0.2% |  1.17 | `Vector matrix multiply SimpleMath`                        |
|   5.44 | 183,876,484.54 | 1.1% |  0.65 | `Vector matrix multiply glm`                               |
|   5.84 | 171,363,985.59 | 0.2% |  0.70 | `Vector matrix multiply DXM`                               |
|   5.35 | 186,774,184.17 | 0.3% |  0.64 | `Vector matrix multiply Vectormath`                        |
|   5.73 | 174,656,625.08 | 0.2% |  0.68 | `Vector matrix multiply move::vectormath::fast*`           |
|   1.50 | 668,859,357.08 | 0.3% |  0.18 | `Vector matrix multiply rtm::matrix4x4f`                   |
|   2.75 | 363,987,614.67 | 0.2% |  0.33 | `Vector matrix multiply rtm::matrix4x4d`                   |
|   3.34 | 299,552,704.03 | 0.1% |  0.40 | `Point QVV multiply rtm::qvvf`                             |
|  10.49 |  95,352,791.02 | 0.1% |  1.25 | `Point QVV multiply rtm::qvvd`                             |
|   4.75 | 210,683,990.63 | 0.1% |  0.57 | `Matrix matrix multiply SimpleMath`                        |
|  22.35 |  44,739,411.80 | 0.2% |  2.67 | `Matrix matrix multiply glm`                               |
|   4.74 | 211,077,305.78 | 0.2% |  0.57 | `Matrix matrix multiply DXM`                               |
|   4.73 | 211,583,789.30 | 0.2% |  0.56 | `Matrix matrix multiply Vectormath`                        |
|   4.74 | 210,883,117.25 | 0.1% |  0.57 | `Matrix matrix multiply move::vectormath::fast*`           |
|   4.72 | 211,799,421.55 | 0.1% |  0.56 | `Matrix matrix multiply move::vectormath`                  |
|   7.46 | 134,000,983.50 | 0.2% |  0.89 | `QVV QVV multiply rtm::qvvf`                               |
|  18.89 |  52,947,387.29 | 0.2% |  2.26 | `QVV QVV multiply rtm::qvvd`                               |
|   3.74 | 267,163,431.13 | 0.4% |  0.45 | `Matrix matrix multiply rtm::matrix4x4f`                   |
|  11.69 |  85,529,824.66 | 0.1% |  1.40 | `Matrix matrix multiply rtm::matrix4x4d`                   |
 
## AVX 

|  ns/op |           op/s | err% | total | benchmark                                                  |
| -----: | -------------: | ---: | ----: | :--------------------------------------------------------- |
|   1.24 | 807,037,890.31 | 0.7% |  0.15 | `Store int (reference 'no-op')`                            |
|   1.23 | 814,115,173.34 | 2.3% |  0.15 | `SimpleMath::Vector2 addition`                             |
|   1.23 | 812,009,268.50 | 1.1% |  0.15 | `SimpleMath::Vector3 addition`                             |
|   1.49 | 670,137,981.41 | 0.6% |  0.18 | `SimpleMath::Vector4 addition`                             |
|   1.24 | 809,106,633.45 | 0.5% |  0.15 | `glm::vec2 addition`                                       |
|   1.50 | 668,667,653.89 | 1.1% |  0.18 | `glm::vec3 addition`                                       |
|   1.24 | 806,176,981.86 | 0.3% |  0.15 | `glm::vec4 addition`                                       |
|   1.23 | 809,887,101.74 | 0.3% |  0.15 | `DirectX::XMFLOAT2 addition`                               |
|   1.24 | 805,945,096.56 | 0.1% |  0.15 | `DirectX::XMFLOAT3 addition`                               |
|   1.22 | 817,770,845.31 | 1.1% |  0.15 | `DirectX::XMFLOAT4 addition without Loads`                 |
|   1.22 | 822,190,319.56 | 0.3% |  0.15 | `Vectormath::Vector2 addition`                             |
|   1.21 | 825,157,398.77 | 0.4% |  0.14 | `Vectormath::Vector3 addition`                             |
|   1.22 | 819,862,714.89 | 0.1% |  0.15 | `Vectormath::Vector4 addition`                             |
|   1.46 | 683,588,671.34 | 0.1% |  0.17 | `move::vec2 addition`                                      |
|   1.23 | 816,315,773.12 | 0.1% |  0.15 | `move::vec3 addition`                                      |
|   1.23 | 815,368,720.31 | 0.2% |  0.15 | `move::vec4 addition`                                      |
|   1.22 | 816,828,630.27 | 0.1% |  0.15 | `move::fastvec2 addition`                                  |
|   1.22 | 816,529,709.18 | 0.0% |  0.15 | `move::fastvec3 addition`                                  |
|   1.22 | 816,506,118.36 | 0.1% |  0.15 | `move::fastvec4 addition`                                  |
|   1.49 | 672,613,633.71 | 0.9% |  0.18 | `rtm::vector4f addition`                                   |
|   6.61 | 151,258,282.62 | 0.2% |  0.79 | `rtm::vector4d addition`                                   |
|  12.41 |  80,581,560.96 | 0.5% |  1.50 | `Complex operation 1 with SimpleMath::Vector*`             |
|   1.21 | 828,175,164.49 | 0.5% |  0.14 | `Complex operation 1 with glm::vec*`                       |
|   4.59 | 217,720,978.78 | 0.5% |  0.55 | `Complex operation 1 with DXM`                             |
|   5.46 | 183,117,663.62 | 0.3% |  0.65 | `Complex operation 1 with DXM w/out loads`                 |
|   1.24 | 808,781,257.02 | 0.1% |  0.15 | `Complex operation 1 with Vectormath`                      |
|  11.46 |  87,244,583.67 | 0.6% |  1.36 | `Complex operation 1 with move::vectormath::*`             |
|   1.93 | 517,535,829.80 | 0.5% |  0.23 | `Complex operation 1 with move::vectormath::fast*`         |
|  30.93 |  32,331,374.73 | 0.2% |  3.70 | `Complex operation 2 with SimpleMath::Vector3`             |
|   1.22 | 818,289,968.48 | 0.2% |  0.15 | `Complex operation 2 with glm::vec3`                       |
|   1.47 | 679,876,535.92 | 0.1% |  0.18 | `Complex operation 2 with DXM`                             |
|   1.97 | 507,422,259.00 | 0.0% |  0.24 | `Complex operation 2 with DXM w/out loads`                 |
|   1.22 | 816,487,512.91 | 0.0% |  0.15 | `Complex operation 2 with Vectormath`                      |
|  23.65 |  42,279,933.56 | 0.4% |  2.83 | `Complex operation 2 with move::vectormath`                |
|   1.47 | 681,027,856.79 | 0.2% |  0.18 | `Complex operation 2 with move::vectormath::fast*`         |
|   2.79 | 358,769,430.31 | 0.2% |  0.33 | `Complex operation 3 with SimpleMath::Vector4`             |
|   1.46 | 685,988,668.59 | 0.2% |  0.17 | `Complex operation 3 with glm::vec4`                       |
|   1.22 | 820,921,193.52 | 0.6% |  0.15 | `Complex operation 3 with DXM`                             |
|   1.49 | 671,536,373.85 | 0.5% |  0.18 | `Complex operation 3 with DXM w/out loads`                 |
|   1.51 | 663,814,330.76 | 0.7% |  0.18 | `Complex operation 3 with Vectormath`                      |
|   1.25 | 799,164,164.72 | 0.2% |  0.15 | `Complex operation 3 with move::vectormath`                |
|   1.25 | 797,343,658.44 | 0.5% |  0.15 | `Complex operation 3 with move::vectormath::fast*`         |
|   1.25 | 797,538,408.99 | 0.2% |  0.15 | `Complex operation 3 with rtm::vector4f`                   |
|   6.55 | 152,590,746.38 | 1.1% |  0.78 | `Complex operation 3 with rtm::vector4d`                   |
|  54.38 |  18,390,251.35 | 0.2% |  6.49 | `Construct model matrix SimpleMath`                        |
| 141.22 |   7,081,369.74 | 0.5% | 16.86 | `Construct model matrix glm`                               |
|  63.20 |  15,823,941.24 | 0.5% |  7.57 | `Construct model matrix DXM (manual)`                      |
|  81.74 |  12,233,183.70 | 0.2% |  9.80 | `Construct model matrix DXM (builtin)`                     |
|  21.83 |  45,815,067.27 | 1.1% |  2.61 | `Construct model matrix Vectormath`                        |
|  82.28 |  12,154,195.48 | 0.6% |  9.85 | `Construct model matrix move::vectormath::fast* (manual)`  |
|  63.92 |  15,643,730.99 | 0.5% |  7.64 | `Construct model matrix move::vectormath::fast* (builtin)` |
|  57.66 |  17,343,333.89 | 1.3% |  6.87 | `Construct model matrix move::vectormath (manual)`         |
|  63.62 |  15,717,639.09 | 0.6% |  7.60 | `Construct model matrix move::vectormath (builtin)`        |
|   1.20 | 833,833,633.51 | 0.5% |  0.14 | `Construct rtm::qvvf (~transformation matrix)`             |
|   7.94 | 126,010,813.39 | 0.9% |  0.95 | `Construct rtm::qvvd (~transformation matrix)`             |
|  10.78 |  92,793,448.84 | 0.9% |  1.29 | `Construct model matrix rtm::matrix3x4f`                   |
|  18.56 |  53,885,586.82 | 0.4% |  2.22 | `Construct model matrix rtm::matrix3x4d`                   |
|   6.34 | 157,631,041.90 | 0.3% |  0.76 | `Construct model matrix rtm::matrix4x4f`                   |
|  47.77 |  20,934,696.45 | 0.5% |  5.72 | `Construct model matrix rtm::matrix4x4d`                   |
|  47.36 |  21,113,371.41 | 0.3% |  5.67 | `Construct view matrix SimpleMath`                         |
|  21.12 |  47,339,156.19 | 0.9% |  2.53 | `Construct view matrix glm`                                |
|  17.99 |  55,591,393.57 | 0.5% |  2.15 | `Construct view matrix DXM`                                |
|  23.81 |  42,006,748.72 | 0.8% |  2.85 | `Construct view matrix Vectormath`                         |
|  20.64 |  48,440,915.26 | 0.3% |  2.47 | `Construct view matrix move::vectormath::fast*`            |
|  54.36 |  18,397,166.04 | 1.2% |  6.47 | `Construct view matrix move::vectormath`                   |
|   7.05 | 141,899,813.53 | 0.3% |  0.84 | `Construct view matrix rtm::matrix4x4f`                    |
|  96.52 |  10,360,891.07 | 0.3% | 11.52 | `Construct view matrix rtm::matrix4x4d`                    |
|  11.68 |  85,652,267.38 | 0.8% |  1.39 | `Construct perspective matrix SimpleMath`                  |
|   1.93 | 517,731,134.53 | 0.7% |  0.23 | `Construct perspective matrix glm`                         |
|  11.24 |  88,987,040.43 | 0.1% |  1.34 | `Construct perspective matrix DXM`                         |
|   1.47 | 680,993,131.16 | 0.2% |  0.18 | `Construct perspective matrix Vectormath`                  |
|  13.91 |  71,916,369.89 | 0.3% |  1.66 | `Construct perspective matrix move::vectormath::fast*`     |
|  13.81 |  72,413,982.77 | 1.3% |  1.66 | `Construct perspective matrix move::vectormath`            |
|  10.19 |  98,093,461.05 | 0.2% |  1.22 | `Construct perspective matrix rtm::matrix4x4f`             |
|  22.03 |  45,400,121.30 | 0.6% |  2.62 | `Construct perspective matrix rtm::matrix4x4d`             |
|   7.97 | 125,425,793.56 | 0.3% |  0.95 | `Construct ortho matrix SimpleMath`                        |
|   1.47 | 680,233,068.93 | 0.5% |  0.18 | `Construct ortho matrix glm`                               |
|   1.48 | 676,918,830.58 | 0.3% |  0.18 | `Construct ortho matrix DXM`                               |
|   8.90 | 112,412,306.60 | 0.4% |  1.06 | `Construct ortho matrix Vectormath`                        |
|   7.35 | 136,121,898.36 | 0.4% |  0.88 | `Construct ortho matrix move::vectormath::fast*`           |
|   7.97 | 125,415,430.57 | 2.1% |  0.96 | `Construct ortho matrix move::vectormath`                  |
|   1.47 | 680,239,366.69 | 1.0% |  0.18 | `Construct ortho matrix rtm::matrix4x4f`                   |
|  22.18 |  45,086,994.77 | 1.3% |  2.64 | `Construct ortho matrix rtm::matrix4x4d`                   |
|   7.04 | 142,087,165.07 | 0.3% |  0.84 | `Vector matrix multiply SimpleMath`                        |
|   5.27 | 189,878,737.46 | 0.5% |  0.63 | `Vector matrix multiply glm`                               |
|   5.72 | 174,741,668.85 | 0.9% |  0.68 | `Vector matrix multiply DXM`                               |
|   5.17 | 193,334,222.67 | 0.7% |  0.62 | `Vector matrix multiply Vectormath`                        |
|   5.71 | 175,131,346.46 | 0.3% |  0.68 | `Vector matrix multiply move::vectormath::fast*`           |
|   1.47 | 678,916,610.68 | 0.1% |  0.18 | `Vector matrix multiply rtm::matrix4x4f`                   |
|   6.56 | 152,353,969.88 | 0.2% |  0.78 | `Vector matrix multiply rtm::matrix4x4d`                   |
|   3.25 | 307,516,895.59 | 0.1% |  0.39 | `Point QVV multiply rtm::qvvf`                             |
|  12.25 |  81,661,251.88 | 0.1% |  1.46 | `Point QVV multiply rtm::qvvd`                             |
|  12.22 |  81,808,508.29 | 0.3% |  1.46 | `Matrix matrix multiply SimpleMath`                        |
|  19.11 |  52,322,791.64 | 1.0% |  2.27 | `Matrix matrix multiply glm`                               |
|  10.98 |  91,105,353.58 | 0.2% |  1.31 | `Matrix matrix multiply DXM`                               |
|   4.30 | 232,488,877.02 | 0.1% |  0.51 | `Matrix matrix multiply Vectormath`                        |
|  10.86 |  92,091,418.28 | 0.3% |  1.30 | `Matrix matrix multiply move::vectormath::fast*`           |
|  11.42 |  87,561,126.00 | 0.4% |  1.36 | `Matrix matrix multiply move::vectormath`                  |
|  10.96 |  91,216,333.17 | 0.3% |  1.31 | `QVV QVV multiply rtm::qvvf`                               |
|  23.27 |  42,971,739.01 | 0.4% |  2.80 | `QVV QVV multiply rtm::qvvd`                               |
|   3.41 | 293,673,006.63 | 0.3% |  0.41 | `Matrix matrix multiply rtm::matrix4x4f`                   |
|  15.31 |  65,310,424.24 | 0.9% |  1.83 | `Matrix matrix multiply rtm::matrix4x4d`                   |
 
## AVX2 

|  ns/op |           op/s | err% | total | benchmark                                                  |
| -----: | -------------: | ---: | ----: | :--------------------------------------------------------- |
|   1.23 | 812,244,174.11 | 0.1% |  0.15 | `Store int (reference 'no-op')`                            |
|   1.22 | 816,670,196.56 | 0.0% |  0.15 | `SimpleMath::Vector2 addition`                             |
|   1.22 | 816,413,170.38 | 0.1% |  0.15 | `SimpleMath::Vector3 addition`                             |
|   1.48 | 673,699,635.35 | 0.4% |  0.18 | `SimpleMath::Vector4 addition`                             |
|   1.26 | 796,666,356.47 | 0.2% |  0.15 | `glm::vec2 addition`                                       |
|   1.25 | 796,870,282.89 | 0.3% |  0.15 | `glm::vec3 addition`                                       |
|   1.24 | 804,457,255.05 | 0.1% |  0.15 | `glm::vec4 addition`                                       |
|   1.25 | 800,633,682.28 | 0.2% |  0.15 | `DirectX::XMFLOAT2 addition`                               |
|   1.24 | 803,473,748.60 | 0.3% |  0.15 | `DirectX::XMFLOAT3 addition`                               |
|   1.49 | 672,657,805.52 | 0.3% |  0.18 | `DirectX::XMFLOAT4 addition without Loads`                 |
|   1.24 | 807,121,442.59 | 0.3% |  0.15 | `Vectormath::Vector2 addition`                             |
|   1.23 | 815,335,738.64 | 0.9% |  0.15 | `Vectormath::Vector3 addition`                             |
|   1.23 | 814,762,070.93 | 0.6% |  0.15 | `Vectormath::Vector4 addition`                             |
|   1.25 | 798,226,262.63 | 1.5% |  0.15 | `move::vec2 addition`                                      |
|   1.25 | 800,877,563.94 | 0.2% |  0.15 | `move::vec3 addition`                                      |
|   1.24 | 805,312,137.13 | 0.8% |  0.15 | `move::vec4 addition`                                      |
|   1.21 | 824,812,812.35 | 0.5% |  0.14 | `move::fastvec2 addition`                                  |
|   1.24 | 805,420,540.51 | 0.7% |  0.15 | `move::fastvec3 addition`                                  |
|   1.25 | 801,706,234.17 | 0.2% |  0.15 | `move::fastvec4 addition`                                  |
|   1.23 | 810,009,202.67 | 0.1% |  0.15 | `rtm::vector4f addition`                                   |
|   6.68 | 149,728,740.87 | 0.5% |  0.80 | `rtm::vector4d addition`                                   |
|  12.59 |  79,440,296.53 | 0.6% |  1.50 | `Complex operation 1 with SimpleMath::Vector*`             |
|   1.47 | 679,836,125.94 | 0.2% |  0.18 | `Complex operation 1 with glm::vec*`                       |
|   4.68 | 213,535,205.89 | 0.1% |  0.56 | `Complex operation 1 with DXM`                             |
|   5.40 | 185,243,717.72 | 0.2% |  0.64 | `Complex operation 1 with DXM w/out loads`                 |
|   1.23 | 815,244,556.51 | 0.2% |  0.15 | `Complex operation 1 with Vectormath`                      |
|  11.53 |  86,756,445.85 | 0.1% |  1.38 | `Complex operation 1 with move::vectormath::*`             |
|   1.97 | 507,050,522.64 | 0.1% |  0.24 | `Complex operation 1 with move::vectormath::fast*`         |
|  31.37 |  31,879,401.05 | 0.3% |  3.74 | `Complex operation 2 with SimpleMath::Vector3`             |
|   1.47 | 679,062,595.83 | 0.4% |  0.18 | `Complex operation 2 with glm::vec3`                       |
|   1.50 | 664,645,227.60 | 0.5% |  0.19 | `Complex operation 2 with DXM`                             |
|   1.75 | 572,304,630.48 | 0.1% |  0.21 | `Complex operation 2 with DXM w/out loads`                 |
|   1.49 | 672,090,897.88 | 0.2% |  0.18 | `Complex operation 2 with Vectormath`                      |
|  23.91 |  41,827,661.99 | 0.3% |  2.86 | `Complex operation 2 with move::vectormath`                |
|   1.48 | 676,033,492.01 | 0.2% |  0.18 | `Complex operation 2 with move::vectormath::fast*`         |
|   2.81 | 356,433,847.64 | 0.1% |  0.34 | `Complex operation 3 with SimpleMath::Vector4`             |
|   1.47 | 680,726,215.43 | 0.0% |  0.18 | `Complex operation 3 with glm::vec4`                       |
|   1.22 | 816,533,163.49 | 0.5% |  0.15 | `Complex operation 3 with DXM`                             |
|   1.48 | 677,092,616.30 | 0.4% |  0.18 | `Complex operation 3 with DXM w/out loads`                 |
|   1.22 | 816,425,500.89 | 0.1% |  0.15 | `Complex operation 3 with Vectormath`                      |
|   1.23 | 812,860,273.93 | 0.2% |  0.15 | `Complex operation 3 with move::vectormath`                |
|   1.48 | 676,107,896.54 | 0.0% |  0.18 | `Complex operation 3 with move::vectormath::fast*`         |
|   1.26 | 794,928,304.65 | 1.3% |  0.15 | `Complex operation 3 with rtm::vector4f`                   |
|   6.58 | 152,019,430.08 | 0.1% |  0.79 | `Complex operation 3 with rtm::vector4d`                   |
|  52.82 |  18,932,151.47 | 0.1% |  6.31 | `Construct model matrix SimpleMath`                        |
| 141.21 |   7,081,423.51 | 0.0% | 16.87 | `Construct model matrix glm`                               |
|  64.42 |  15,522,011.05 | 0.9% |  7.71 | `Construct model matrix DXM (manual)`                      |
|  75.40 |  13,263,086.67 | 0.1% |  9.01 | `Construct model matrix DXM (builtin)`                     |
|  70.90 |  14,103,608.35 | 0.3% |  8.47 | `Construct model matrix Vectormath`                        |
|  66.57 |  15,021,798.86 | 0.3% |  7.95 | `Construct model matrix move::vectormath::fast* (manual)`  |
|  77.16 |  12,960,250.38 | 0.1% |  9.22 | `Construct model matrix move::vectormath::fast* (builtin)` |
|  54.69 |  18,285,809.16 | 0.1% |  6.54 | `Construct model matrix move::vectormath (manual)`         |
|  78.07 |  12,808,540.07 | 0.3% |  9.33 | `Construct model matrix move::vectormath (builtin)`        |
|   1.23 | 811,486,449.79 | 0.1% |  0.15 | `Construct rtm::qvvf (~transformation matrix)`             |
|   6.62 | 150,955,016.91 | 0.4% |  0.79 | `Construct rtm::qvvd (~transformation matrix)`             |
|   9.47 | 105,627,445.51 | 1.1% |  1.14 | `Construct model matrix rtm::matrix3x4f`                   |
|  28.53 |  35,050,688.40 | 0.3% |  3.42 | `Construct model matrix rtm::matrix3x4d`                   |
|   6.67 | 149,848,085.60 | 0.2% |  0.80 | `Construct model matrix rtm::matrix4x4f`                   |
|  30.00 |  33,331,425.35 | 0.2% |  3.58 | `Construct model matrix rtm::matrix4x4d`                   |
|  27.57 |  36,273,149.99 | 0.2% |  3.29 | `Construct view matrix SimpleMath`                         |
|  21.34 |  46,862,740.34 | 0.1% |  2.55 | `Construct view matrix glm`                                |
|  25.84 |  38,695,713.68 | 0.2% |  3.09 | `Construct view matrix DXM`                                |
|  22.33 |  44,777,048.36 | 0.2% |  2.67 | `Construct view matrix Vectormath`                         |
|  25.86 |  38,674,542.95 | 0.2% |  3.09 | `Construct view matrix move::vectormath::fast*`            |
|  25.85 |  38,686,365.76 | 0.2% |  3.09 | `Construct view matrix move::vectormath`                   |
|  31.76 |  31,484,822.21 | 0.5% |  3.80 | `Construct view matrix rtm::matrix4x4f`                    |
|  89.72 |  11,145,944.15 | 0.4% | 10.72 | `Construct view matrix rtm::matrix4x4d`                    |
|  11.75 |  85,093,814.50 | 0.4% |  1.41 | `Construct perspective matrix SimpleMath`                  |
|   1.97 | 507,794,473.87 | 0.2% |  0.23 | `Construct perspective matrix glm`                         |
|  11.29 |  88,570,428.99 | 0.2% |  1.35 | `Construct perspective matrix DXM`                         |
|   1.47 | 679,963,548.48 | 0.4% |  0.18 | `Construct perspective matrix Vectormath`                  |
|  14.01 |  71,377,442.75 | 0.1% |  1.67 | `Construct perspective matrix move::vectormath::fast*`     |
|  14.24 |  70,248,431.57 | 0.1% |  1.70 | `Construct perspective matrix move::vectormath`            |
|  10.35 |  96,586,449.58 | 0.2% |  1.24 | `Construct perspective matrix rtm::matrix4x4f`             |
|  21.84 |  45,783,475.52 | 0.1% |  2.61 | `Construct perspective matrix rtm::matrix4x4d`             |
|   8.13 | 123,065,559.67 | 0.1% |  0.97 | `Construct ortho matrix SimpleMath`                        |
|   1.48 | 676,195,333.65 | 0.1% |  0.18 | `Construct ortho matrix glm`                               |
|   1.48 | 675,112,607.24 | 0.2% |  0.18 | `Construct ortho matrix DXM`                               |
|   9.19 | 108,845,671.02 | 0.5% |  1.10 | `Construct ortho matrix Vectormath`                        |
|   7.28 | 137,316,846.58 | 0.4% |  0.87 | `Construct ortho matrix move::vectormath::fast*`           |
|   8.24 | 121,404,616.85 | 0.2% |  0.98 | `Construct ortho matrix move::vectormath`                  |
|   1.48 | 676,473,067.06 | 0.0% |  0.18 | `Construct ortho matrix rtm::matrix4x4f`                   |
|  22.04 |  45,371,680.27 | 0.3% |  2.64 | `Construct ortho matrix rtm::matrix4x4d`                   |
|   8.14 | 122,807,131.32 | 0.3% |  0.97 | `Vector matrix multiply SimpleMath`                        |
|   5.43 | 184,136,510.85 | 0.2% |  0.65 | `Vector matrix multiply glm`                               |
|   6.83 | 146,412,184.80 | 0.4% |  0.82 | `Vector matrix multiply DXM`                               |
|   5.36 | 186,460,297.03 | 0.3% |  0.64 | `Vector matrix multiply Vectormath`                        |
|   6.80 | 147,067,665.81 | 0.1% |  0.81 | `Vector matrix multiply move::vectormath::fast*`           |
|   1.47 | 678,421,748.30 | 0.3% |  0.18 | `Vector matrix multiply rtm::matrix4x4f`                   |
|   6.57 | 152,241,828.23 | 0.2% |  0.78 | `Vector matrix multiply rtm::matrix4x4d`                   |
|   3.04 | 328,634,873.98 | 0.1% |  0.36 | `Point QVV multiply rtm::qvvf`                             |
|  16.40 |  60,977,715.25 | 0.4% |  1.96 | `Point QVV multiply rtm::qvvd`                             |
|   9.81 | 101,976,589.49 | 0.1% |  1.17 | `Matrix matrix multiply SimpleMath`                        |
|  18.73 |  53,386,272.41 | 0.2% |  2.24 | `Matrix matrix multiply glm`                               |
|   9.43 | 106,057,485.72 | 0.2% |  1.12 | `Matrix matrix multiply DXM`                               |
|   3.82 | 262,037,990.09 | 0.1% |  0.46 | `Matrix matrix multiply Vectormath`                        |
|   9.07 | 110,251,030.57 | 1.0% |  1.08 | `Matrix matrix multiply move::vectormath::fast*`           |
|  10.35 |  96,622,651.83 | 0.3% |  1.24 | `Matrix matrix multiply move::vectormath`                  |
|  10.91 |  91,624,784.57 | 0.2% |  1.30 | `QVV QVV multiply rtm::qvvf`                               |
|  37.04 |  27,001,339.27 | 0.2% |  4.43 | `QVV QVV multiply rtm::qvvd`                               |
|   2.84 | 352,460,836.54 | 0.0% |  0.34 | `Matrix matrix multiply rtm::matrix4x4f`                   |
|  13.13 |  76,155,724.10 | 0.1% |  1.57 | `Matrix matrix multiply rtm::matrix4x4d`                   |
