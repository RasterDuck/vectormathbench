# Benchmark Results 
 
## System Info 
CPU: AMD Ryzen 9 3900X 12-Core Processor             
OS: Microsoft Windows 10 Pro 
OS Version: 10.0.19045 N/A Build 19045 
 
## SSE 4.2 

|  ns/op |           op/s | err% | total | benchmark                                                   |
| -----: | -------------: | ---: | ----: | :---------------------------------------------------------- |
|   1.23 | 816,222,938.97 | 0.3% |  0.15 | `Store int (reference 'no-op')`                             |
|   1.22 | 819,400,169.37 | 0.4% |  0.15 | `SimpleMath::Vector2 addition`                              |
|   1.47 | 681,893,222.44 | 0.5% |  0.18 | `SimpleMath::Vector3 addition`                              |
|   1.47 | 679,374,136.82 | 0.5% |  0.18 | `SimpleMath::Vector4 addition`                              |
|   1.22 | 817,538,838.45 | 0.2% |  0.15 | `glm::vec2 addition`                                        |
|   1.48 | 676,663,478.13 | 0.4% |  0.18 | `glm::vec3 addition`                                        |
|   1.23 | 811,890,242.94 | 0.1% |  0.15 | `glm::vec4 addition`                                        |
|   1.24 | 807,089,473.94 | 0.2% |  0.15 | `DirectX::XMFLOAT2 addition`                                |
|   1.47 | 680,737,786.82 | 0.3% |  0.18 | `DirectX::XMFLOAT3 addition`                                |
|   1.24 | 809,026,902.09 | 0.2% |  0.15 | `DirectX::XMFLOAT4 addition without Loads`                  |
|   1.50 | 667,508,842.82 | 0.2% |  0.18 | `Vectormath::Vector2 addition`                              |
|   1.25 | 800,022,272.02 | 0.1% |  0.15 | `Vectormath::Vector3 addition`                              |
|   1.24 | 803,673,518.84 | 0.3% |  0.15 | `Vectormath::Vector4 addition`                              |
|   1.25 | 800,482,156.85 | 0.1% |  0.15 | `move::vec2f addition (float)`                              |
|   1.25 | 799,728,304.13 | 0.1% |  0.15 | `move::vec3f addition (float)`                              |
|   1.23 | 810,989,112.24 | 0.1% |  0.15 | `move::vec4f addition (float)`                              |
|   1.23 | 809,775,611.18 | 0.5% |  0.15 | `move::vec2d addition (double)`                             |
|   1.22 | 817,953,648.92 | 0.2% |  0.15 | `move::vec3d addition (double)`                             |
|   1.22 | 820,210,271.23 | 0.3% |  0.15 | `move::vec4d addition (double)`                             |
|   1.22 | 818,811,446.77 | 0.1% |  0.15 | `rtm::vector4f addition`                                    |
|   1.22 | 819,752,946.86 | 0.3% |  0.15 | `rtm::vector4d addition`                                    |
|   3.22 | 310,450,585.75 | 0.6% |  0.38 | `Complex operation 1 with SimpleMath::Vector*`              |
|   1.51 | 662,937,939.78 | 0.5% |  0.18 | `Complex operation 1 with glm::vec*`                        |
|   3.20 | 312,708,533.12 | 0.5% |  0.38 | `Complex operation 1 with DXM`                              |
|   4.04 | 247,332,675.29 | 1.0% |  0.48 | `Complex operation 1 with DXM w/out loads`                  |
|   1.52 | 657,086,598.00 | 0.7% |  0.18 | `Complex operation 1 with Vectormath`                       |
|   1.53 | 653,581,976.20 | 0.2% |  0.18 | `Complex operation 1 with move::vectormath::* (float)`      |
|   1.50 | 668,874,193.38 | 0.5% |  0.18 | `Complex operation 1 with move::vectormath::* (double)`     |
|   3.66 | 273,097,845.33 | 0.6% |  0.44 | `Complex operation 2 with SimpleMath::Vector3`              |
|   1.23 | 814,125,327.56 | 0.4% |  0.15 | `Complex operation 2 with glm::vec3`                        |
|   1.48 | 675,576,423.24 | 0.8% |  0.18 | `Complex operation 2 with DXM`                              |
|   1.72 | 581,122,037.79 | 0.3% |  0.21 | `Complex operation 2 with DXM w/out loads`                  |
|   1.24 | 803,890,245.15 | 0.2% |  0.15 | `Complex operation 2 with Vectormath`                       |
|   1.24 | 803,873,597.00 | 0.9% |  0.15 | `Complex operation 2 with move::vectormath (float)`         |
|   1.47 | 679,047,093.86 | 0.3% |  0.18 | `Complex operation 2 with move::vectormath (double)`        |
|   1.49 | 669,523,025.93 | 0.1% |  0.18 | `Complex operation 3 with SimpleMath::Vector4`              |
|   1.25 | 800,848,413.51 | 0.3% |  0.15 | `Complex operation 3 with glm::vec4`                        |
|   1.25 | 799,622,012.45 | 0.6% |  0.15 | `Complex operation 3 with DXM`                              |
|   1.49 | 672,620,263.30 | 0.4% |  0.18 | `Complex operation 3 with DXM w/out loads`                  |
|   1.25 | 801,789,772.73 | 0.3% |  0.15 | `Complex operation 3 with Vectormath`                       |
|   1.24 | 808,383,788.03 | 0.4% |  0.15 | `Complex operation 3 with move::vectormath (float)`         |
|   1.25 | 797,635,157.68 | 0.5% |  0.15 | `Complex operation 3 with move::vectormath (double)`        |
|   1.25 | 802,883,959.18 | 0.3% |  0.15 | `Complex operation 3 with rtm::vector4f`                    |
|   1.49 | 670,932,619.99 | 0.2% |  0.18 | `Complex operation 3 with rtm::vector4d`                    |
|  48.58 |  20,584,746.27 | 0.4% |  5.80 | `Construct model matrix SimpleMath`                         |
| 160.76 |   6,220,508.57 | 0.4% | 19.26 | `Construct model matrix glm`                                |
|  49.00 |  20,409,601.67 | 0.3% |  5.85 | `Construct model matrix DXM (manual)`                       |
|  41.89 |  23,870,370.52 | 0.3% |  5.02 | `Construct model matrix DXM (builtin)`                      |
|  20.14 |  49,657,157.22 | 0.2% |  2.41 | `Construct model matrix Vectormath`                         |
|  38.18 |  26,188,302.25 | 0.4% |  4.57 | `Construct model matrix move::vectormath (manual, float)`   |
|   7.90 | 126,605,195.62 | 0.4% |  0.95 | `Construct model matrix move::vectormath (builtin, float)`  |
|  71.00 |  14,083,781.26 | 1.1% |  8.53 | `Construct model matrix move::vectormath (manual, double)`  |
|  32.62 |  30,654,899.65 | 0.8% |  3.90 | `Construct model matrix move::vectormath (builtin, double)` |
|   1.50 | 667,535,559.50 | 1.9% |  0.18 | `Construct rtm::qvvf (~transformation matrix)`              |
|   1.98 | 505,009,552.38 | 0.2% |  0.24 | `Construct rtm::qvvd (~transformation matrix)`              |
|   7.20 | 138,909,613.25 | 0.3% |  0.86 | `Construct model matrix rtm::matrix3x4f`                    |
|  10.59 |  94,444,276.40 | 0.3% |  1.27 | `Construct model matrix rtm::matrix3x4d`                    |
|   7.84 | 127,605,220.77 | 0.2% |  0.94 | `Construct model matrix rtm::matrix4x4f`                    |
|  10.47 |  95,496,689.58 | 0.3% |  1.25 | `Construct model matrix rtm::matrix4x4d`                    |
|  25.13 |  39,790,156.25 | 0.5% |  3.00 | `Construct view matrix SimpleMath`                          |
|  21.59 |  46,322,016.58 | 0.4% |  2.58 | `Construct view matrix glm`                                 |
|  25.15 |  39,768,145.78 | 0.5% |  3.01 | `Construct view matrix DXM`                                 |
|  28.94 |  34,548,811.44 | 0.2% |  3.46 | `Construct view matrix Vectormath`                          |
|   1.46 | 683,288,853.74 | 0.2% |  0.17 | `Construct view matrix move::vectormath (float)`            |
|  17.56 |  56,958,477.67 | 0.3% |  2.10 | `Construct view matrix move::vectormath (double)`           |
|   1.46 | 683,224,866.64 | 0.2% |  0.17 | `Construct view matrix rtm::matrix4x4f`                     |
|  17.50 |  57,147,620.07 | 0.2% |  2.09 | `Construct view matrix rtm::matrix4x4d`                     |
|  14.54 |  68,768,054.82 | 0.3% |  1.74 | `Construct perspective matrix SimpleMath`                   |
|   2.20 | 454,765,836.03 | 0.3% |  0.26 | `Construct perspective matrix glm`                          |
|  14.60 |  68,481,110.12 | 0.8% |  1.74 | `Construct perspective matrix DXM`                          |
|  12.25 |  81,633,795.56 | 0.6% |  1.46 | `Construct perspective matrix Vectormath`                   |
|   6.46 | 154,868,524.37 | 0.3% |  0.77 | `Construct perspective matrix move::vectormath (float)`     |
|   2.46 | 406,173,044.34 | 0.4% |  0.29 | `Construct perspective matrix move::vectormath (double)`    |
|   6.40 | 156,189,829.09 | 0.6% |  0.77 | `Construct perspective matrix rtm::matrix4x4f`              |
|   2.44 | 410,455,676.88 | 0.2% |  0.29 | `Construct perspective matrix rtm::matrix4x4d`              |
|   1.96 | 511,500,069.71 | 0.4% |  0.24 | `Construct ortho matrix SimpleMath`                         |
|   1.47 | 680,040,802.45 | 0.4% |  0.18 | `Construct ortho matrix glm`                                |
|   1.48 | 676,143,714.47 | 0.1% |  0.18 | `Construct ortho matrix DXM`                                |
|  33.42 |  29,925,268.94 | 0.6% |  3.99 | `Construct ortho matrix Vectormath`                         |
|   1.48 | 674,170,788.52 | 0.5% |  0.18 | `Construct ortho matrix move::vectormath (float)`           |
|   2.46 | 406,234,893.14 | 0.3% |  0.29 | `Construct ortho matrix move::vectormath (double)`          |
|   1.46 | 682,984,133.15 | 0.2% |  0.17 | `Construct ortho matrix rtm::matrix4x4f`                    |
|   2.45 | 408,346,531.15 | 0.7% |  0.29 | `Construct ortho matrix rtm::matrix4x4d`                    |
|   5.55 | 180,239,401.33 | 0.8% |  0.66 | `Vector matrix multiply SimpleMath`                         |
|   5.70 | 175,295,743.69 | 0.4% |  0.68 | `Vector matrix multiply glm`                                |
|   5.56 | 179,888,038.07 | 0.3% |  0.66 | `Vector matrix multiply DXM`                                |
|   7.30 | 137,047,439.52 | 0.1% |  0.87 | `Vector matrix multiply Vectormath`                         |
|   5.54 | 180,520,627.50 | 0.7% |  0.66 | `Vector matrix multiply move::vectormath (float)`           |
|   6.09 | 164,132,792.04 | 0.3% |  0.73 | `Vector matrix multiply move::vectormath (double)`          |
|   1.48 | 676,876,032.73 | 0.3% |  0.18 | `Vector matrix multiply rtm::matrix4x4f`                    |
|   2.69 | 371,434,234.63 | 0.4% |  0.32 | `Vector matrix multiply rtm::matrix4x4d`                    |
|   3.24 | 309,062,389.06 | 0.2% |  0.39 | `Point QVV multiply rtm::qvvf`                              |
|   9.99 | 100,132,068.11 | 0.2% |  1.19 | `Point QVV multiply rtm::qvvd`                              |
|   4.63 | 215,827,027.59 | 0.3% |  0.55 | `Matrix matrix multiply SimpleMath`                         |
|  22.13 |  45,191,896.79 | 0.6% |  2.64 | `Matrix matrix multiply glm`                                |
|   4.70 | 212,838,482.69 | 0.4% |  0.56 | `Matrix matrix multiply DXM`                                |
|   4.67 | 214,348,947.23 | 0.3% |  0.56 | `Matrix matrix multiply Vectormath`                         |
|   3.70 | 270,565,698.76 | 0.4% |  0.44 | `Matrix matrix multiply move::vectormath (float)`           |
|  11.53 |  86,714,497.19 | 0.6% |  1.38 | `Matrix matrix multiply move::vectormath (double)`          |
|   7.37 | 135,651,169.33 | 0.1% |  0.88 | `QVV QVV multiply rtm::qvvf`                                |
|  18.54 |  53,940,482.78 | 0.4% |  2.22 | `QVV QVV multiply rtm::qvvd`                                |
|   3.68 | 271,822,536.25 | 0.3% |  0.44 | `Matrix matrix multiply rtm::matrix4x4f`                    |
|  11.48 |  87,090,982.77 | 0.3% |  1.37 | `Matrix matrix multiply rtm::matrix4x4d`                    |
 
## AVX 

|  ns/op |           op/s | err% | total | benchmark                                                   |
| -----: | -------------: | ---: | ----: | :---------------------------------------------------------- |
|   1.28 | 780,161,778.71 | 1.5% |  0.15 | `Store int (reference 'no-op')`                             |
|   1.29 | 777,781,904.55 | 0.5% |  0.15 | `SimpleMath::Vector2 addition`                              |
|   1.24 | 806,389,853.95 | 1.4% |  0.15 | `SimpleMath::Vector3 addition`                              |
|   1.24 | 805,319,175.66 | 0.2% |  0.15 | `SimpleMath::Vector4 addition`                              |
|   1.23 | 810,794,522.71 | 0.1% |  0.15 | `glm::vec2 addition`                                        |
|   1.24 | 807,937,542.76 | 0.2% |  0.15 | `glm::vec3 addition`                                        |
|   1.49 | 672,441,396.69 | 0.2% |  0.18 | `glm::vec4 addition`                                        |
|   1.23 | 811,430,115.93 | 0.2% |  0.15 | `DirectX::XMFLOAT2 addition`                                |
|   1.49 | 670,050,177.96 | 0.5% |  0.18 | `DirectX::XMFLOAT3 addition`                                |
|   1.24 | 804,640,882.25 | 0.6% |  0.15 | `DirectX::XMFLOAT4 addition without Loads`                  |
|   1.24 | 808,923,982.64 | 0.4% |  0.15 | `Vectormath::Vector2 addition`                              |
|   1.27 | 787,677,864.18 | 0.9% |  0.15 | `Vectormath::Vector3 addition`                              |
|   1.27 | 784,713,775.65 | 0.7% |  0.15 | `Vectormath::Vector4 addition`                              |
|   1.49 | 671,486,328.29 | 0.5% |  0.18 | `move::vec2f addition (float)`                              |
|   1.23 | 811,451,040.36 | 0.3% |  0.15 | `move::vec3f addition (float)`                              |
|   1.22 | 817,454,171.79 | 0.6% |  0.15 | `move::vec4f addition (float)`                              |
|   1.48 | 675,416,231.48 | 0.9% |  0.18 | `move::vec2d addition (double)`                             |
|   1.23 | 810,986,540.36 | 0.1% |  0.15 | `move::vec3d addition (double)`                             |
|   1.24 | 805,928,173.47 | 0.1% |  0.15 | `move::vec4d addition (double)`                             |
|   1.49 | 669,943,737.20 | 0.2% |  0.18 | `rtm::vector4f addition`                                    |
|   1.26 | 790,603,898.35 | 1.5% |  0.15 | `rtm::vector4d addition`                                    |
|  11.94 |  83,743,531.15 | 0.9% |  1.44 | `Complex operation 1 with SimpleMath::Vector*`              |
|   1.49 | 672,848,536.20 | 0.6% |  0.18 | `Complex operation 1 with glm::vec*`                        |
|   4.47 | 223,513,836.95 | 1.1% |  0.54 | `Complex operation 1 with DXM`                              |
|   5.46 | 183,145,121.20 | 0.3% |  0.65 | `Complex operation 1 with DXM w/out loads`                  |
|   1.22 | 816,745,126.65 | 0.5% |  0.15 | `Complex operation 1 with Vectormath`                       |
|   1.46 | 685,007,421.15 | 0.2% |  0.17 | `Complex operation 1 with move::vectormath::* (float)`      |
|   1.22 | 818,109,675.78 | 0.7% |  0.15 | `Complex operation 1 with move::vectormath::* (double)`     |
|  31.51 |  31,737,710.76 | 0.5% |  3.77 | `Complex operation 2 with SimpleMath::Vector3`              |
|   1.23 | 811,071,872.65 | 0.5% |  0.15 | `Complex operation 2 with glm::vec3`                        |
|   1.47 | 679,997,683.00 | 0.4% |  0.18 | `Complex operation 2 with DXM`                              |
|   1.80 | 556,197,801.35 | 0.8% |  0.21 | `Complex operation 2 with DXM w/out loads`                  |
|   1.26 | 793,468,260.66 | 0.5% |  0.15 | `Complex operation 2 with Vectormath`                       |
|   1.26 | 793,491,894.50 | 0.7% |  0.15 | `Complex operation 2 with move::vectormath (float)`         |
|   1.47 | 680,955,582.35 | 0.6% |  0.18 | `Complex operation 2 with move::vectormath (double)`        |
|   1.25 | 802,290,703.49 | 1.0% |  0.15 | `Complex operation 3 with SimpleMath::Vector4`              |
|   1.22 | 817,797,670.86 | 0.5% |  0.15 | `Complex operation 3 with glm::vec4`                        |
|   1.46 | 683,389,411.77 | 0.2% |  0.17 | `Complex operation 3 with DXM`                              |
|   1.31 | 765,389,589.55 | 0.7% |  0.16 | `Complex operation 3 with DXM w/out loads`                  |
|   1.25 | 801,758,660.31 | 0.2% |  0.15 | `Complex operation 3 with Vectormath`                       |
|   1.50 | 668,836,022.28 | 0.3% |  0.18 | `Complex operation 3 with move::vectormath (float)`         |
|   1.25 | 800,482,258.56 | 0.1% |  0.15 | `Complex operation 3 with move::vectormath (double)`        |
|   1.50 | 667,400,807.55 | 0.3% |  0.18 | `Complex operation 3 with rtm::vector4f`                    |
|   1.24 | 807,829,834.08 | 0.2% |  0.15 | `Complex operation 3 with rtm::vector4d`                    |
|  55.04 |  18,169,732.39 | 0.3% |  6.57 | `Construct model matrix SimpleMath`                         |
| 141.88 |   7,048,195.62 | 0.2% | 16.93 | `Construct model matrix glm`                                |
|  75.62 |  13,223,979.44 | 0.6% |  9.03 | `Construct model matrix DXM (manual)`                       |
|  87.38 |  11,444,150.04 | 0.7% | 10.44 | `Construct model matrix DXM (builtin)`                      |
|  17.76 |  56,307,318.22 | 0.9% |  2.12 | `Construct model matrix Vectormath`                         |
|  35.30 |  28,330,851.82 | 0.5% |  4.21 | `Construct model matrix move::vectormath (manual, float)`   |
|   6.30 | 158,611,829.27 | 0.5% |  0.76 | `Construct model matrix move::vectormath (builtin, float)`  |
| 145.48 |   6,873,696.65 | 0.2% | 17.34 | `Construct model matrix move::vectormath (manual, double)`  |
|  46.37 |  21,567,523.37 | 0.6% |  5.54 | `Construct model matrix move::vectormath (builtin, double)` |
|   1.52 | 659,809,939.41 | 0.7% |  0.18 | `Construct rtm::qvvf (~transformation matrix)`              |
|   7.37 | 135,692,231.64 | 1.4% |  0.88 | `Construct rtm::qvvd (~transformation matrix)`              |
|  14.01 |  71,368,979.22 | 1.1% |  1.67 | `Construct model matrix rtm::matrix3x4f`                    |
|  35.86 |  27,886,679.36 | 0.7% |  4.28 | `Construct model matrix rtm::matrix3x4d`                    |
|   6.35 | 157,417,933.75 | 0.2% |  0.76 | `Construct model matrix rtm::matrix4x4f`                    |
|  25.69 |  38,926,712.03 | 0.7% |  3.07 | `Construct model matrix rtm::matrix4x4d`                    |
|  48.51 |  20,613,734.91 | 0.4% |  5.82 | `Construct view matrix SimpleMath`                          |
|  21.33 |  46,876,690.31 | 0.6% |  2.56 | `Construct view matrix glm`                                 |
|  18.72 |  53,432,300.80 | 0.6% |  2.24 | `Construct view matrix DXM`                                 |
|  24.13 |  41,437,400.35 | 0.8% |  2.87 | `Construct view matrix Vectormath`                          |
|  11.89 |  84,099,332.27 | 0.6% |  1.42 | `Construct view matrix move::vectormath (float)`            |
|  18.70 |  53,485,109.90 | 0.6% |  2.23 | `Construct view matrix move::vectormath (double)`           |
|   7.24 | 138,177,723.99 | 0.6% |  0.86 | `Construct view matrix rtm::matrix4x4f`                     |
|  18.74 |  53,364,034.29 | 1.4% |  2.24 | `Construct view matrix rtm::matrix4x4d`                     |
|  12.28 |  81,421,382.88 | 0.7% |  1.47 | `Construct perspective matrix SimpleMath`                   |
|   1.96 | 509,703,302.37 | 0.1% |  0.23 | `Construct perspective matrix glm`                          |
|  11.42 |  87,598,545.65 | 0.7% |  1.37 | `Construct perspective matrix DXM`                          |
|   1.49 | 669,799,674.74 | 0.4% |  0.18 | `Construct perspective matrix Vectormath`                   |
|   5.63 | 177,655,313.45 | 0.5% |  0.67 | `Construct perspective matrix move::vectormath (float)`     |
|   2.50 | 399,561,422.67 | 0.7% |  0.30 | `Construct perspective matrix move::vectormath (double)`    |
|   5.53 | 180,979,096.91 | 0.7% |  0.66 | `Construct perspective matrix rtm::matrix4x4f`              |
|   2.48 | 403,181,160.11 | 0.6% |  0.30 | `Construct perspective matrix rtm::matrix4x4d`              |
|  11.01 |  90,803,344.32 | 0.6% |  1.32 | `Construct ortho matrix SimpleMath`                         |
|   1.49 | 669,428,717.04 | 0.6% |  0.18 | `Construct ortho matrix glm`                                |
|   1.49 | 670,206,252.77 | 0.9% |  0.18 | `Construct ortho matrix DXM`                                |
|   3.59 | 278,348,646.45 | 0.3% |  0.43 | `Construct ortho matrix Vectormath`                         |
|   1.46 | 685,788,470.59 | 0.2% |  0.17 | `Construct ortho matrix move::vectormath (float)`           |
|   2.46 | 406,355,551.43 | 0.9% |  0.30 | `Construct ortho matrix move::vectormath (double)`          |
|   1.52 | 657,010,766.69 | 0.5% |  0.18 | `Construct ortho matrix rtm::matrix4x4f`                    |
|   2.53 | 395,172,055.84 | 0.3% |  0.30 | `Construct ortho matrix rtm::matrix4x4d`                    |
|   5.64 | 177,162,753.16 | 0.1% |  0.67 | `Vector matrix multiply SimpleMath`                         |
|   5.45 | 183,418,877.52 | 0.5% |  0.65 | `Vector matrix multiply glm`                                |
|   5.63 | 177,718,878.73 | 0.4% |  0.67 | `Vector matrix multiply DXM`                                |
|   5.32 | 187,927,201.15 | 1.0% |  0.63 | `Vector matrix multiply Vectormath`                         |
|   5.52 | 181,297,534.14 | 0.8% |  0.66 | `Vector matrix multiply move::vectormath (float)`           |
|   6.22 | 160,789,712.62 | 1.4% |  0.74 | `Vector matrix multiply move::vectormath (double)`          |
|   1.73 | 576,692,497.22 | 0.4% |  0.21 | `Vector matrix multiply rtm::matrix4x4f`                    |
|   2.23 | 447,574,158.54 | 0.1% |  0.27 | `Vector matrix multiply rtm::matrix4x4d`                    |
|   3.37 | 296,355,330.21 | 0.4% |  0.40 | `Point QVV multiply rtm::qvvf`                              |
|  10.77 |  92,810,085.95 | 0.5% |  1.29 | `Point QVV multiply rtm::qvvd`                              |
|  16.42 |  60,909,797.46 | 0.5% |  1.95 | `Matrix matrix multiply SimpleMath`                         |
|  18.91 |  52,884,948.86 | 0.8% |  2.25 | `Matrix matrix multiply glm`                                |
|  10.95 |  91,311,224.96 | 1.0% |  1.30 | `Matrix matrix multiply DXM`                                |
|   4.39 | 227,869,885.88 | 0.1% |  0.52 | `Matrix matrix multiply Vectormath`                         |
|   3.43 | 291,621,106.66 | 0.1% |  0.41 | `Matrix matrix multiply move::vectormath (float)`           |
|  17.56 |  56,954,474.57 | 0.5% |  2.09 | `Matrix matrix multiply move::vectormath (double)`          |
|  10.12 |  98,854,616.52 | 0.5% |  1.21 | `QVV QVV multiply rtm::qvvf`                                |
|  25.03 |  39,955,371.75 | 0.7% |  2.99 | `QVV QVV multiply rtm::qvvd`                                |
|   3.49 | 286,147,179.00 | 2.4% |  0.42 | `Matrix matrix multiply rtm::matrix4x4f`                    |
|  15.55 |  64,291,348.85 | 1.5% |  1.86 | `Matrix matrix multiply rtm::matrix4x4d`                    |
 
## AVX2 

|  ns/op |           op/s | err% | total | benchmark                                                   |
| -----: | -------------: | ---: | ----: | :---------------------------------------------------------- |
|   1.24 | 806,296,995.53 | 0.4% |  0.15 | `Store int (reference 'no-op')`                             |
|   1.25 | 800,257,078.01 | 1.2% |  0.15 | `SimpleMath::Vector2 addition`                              |
|   1.24 | 805,257,463.76 | 0.2% |  0.15 | `SimpleMath::Vector3 addition`                              |
|   1.24 | 804,239,680.98 | 0.3% |  0.15 | `SimpleMath::Vector4 addition`                              |
|   1.50 | 665,557,476.61 | 0.3% |  0.18 | `glm::vec2 addition`                                        |
|   1.25 | 801,254,889.99 | 0.6% |  0.15 | `glm::vec3 addition`                                        |
|   1.24 | 809,064,075.10 | 0.4% |  0.15 | `glm::vec4 addition`                                        |
|   1.24 | 808,217,514.10 | 0.2% |  0.15 | `DirectX::XMFLOAT2 addition`                                |
|   1.26 | 795,403,065.59 | 1.3% |  0.15 | `DirectX::XMFLOAT3 addition`                                |
|   1.50 | 664,887,681.30 | 0.1% |  0.18 | `DirectX::XMFLOAT4 addition without Loads`                  |
|   1.50 | 668,792,925.13 | 0.2% |  0.18 | `Vectormath::Vector2 addition`                              |
|   1.23 | 810,764,893.17 | 0.3% |  0.15 | `Vectormath::Vector3 addition`                              |
|   1.48 | 674,905,338.06 | 0.1% |  0.18 | `Vectormath::Vector4 addition`                              |
|   1.23 | 811,117,519.73 | 0.7% |  0.15 | `move::vec2f addition (float)`                              |
|   1.24 | 808,183,783.19 | 0.6% |  0.15 | `move::vec3f addition (float)`                              |
|   1.25 | 800,349,904.21 | 0.5% |  0.15 | `move::vec4f addition (float)`                              |
|   1.29 | 772,448,513.24 | 0.8% |  0.15 | `move::vec2d addition (double)`                             |
|   1.55 | 643,758,151.32 | 1.0% |  0.18 | `move::vec3d addition (double)`                             |
|   1.52 | 656,879,404.08 | 1.6% |  0.18 | `move::vec4d addition (double)`                             |
|   1.22 | 817,340,345.77 | 0.5% |  0.15 | `rtm::vector4f addition`                                    |
|   1.22 | 817,167,671.84 | 0.3% |  0.15 | `rtm::vector4d addition`                                    |
|  12.52 |  79,844,054.98 | 0.4% |  1.48 | `Complex operation 1 with SimpleMath::Vector*`              |
|   1.28 | 781,768,989.86 | 1.8% |  0.15 | `Complex operation 1 with glm::vec*`                        |
|   4.50 | 222,392,345.47 | 0.3% |  0.54 | `Complex operation 1 with DXM`                              |
|   5.41 | 184,720,132.94 | 0.9% |  0.65 | `Complex operation 1 with DXM w/out loads`                  |
|   1.22 | 820,989,297.76 | 0.2% |  0.15 | `Complex operation 1 with Vectormath`                       |
|   1.23 | 813,110,444.18 | 0.3% |  0.15 | `Complex operation 1 with move::vectormath::* (float)`      |
|   1.26 | 792,452,591.13 | 0.2% |  0.15 | `Complex operation 1 with move::vectormath::* (double)`     |
|  28.25 |  35,394,632.17 | 0.6% |  3.38 | `Complex operation 2 with SimpleMath::Vector3`              |
|   1.50 | 665,811,851.25 | 0.7% |  0.18 | `Complex operation 2 with glm::vec3`                        |
|   1.23 | 813,816,158.90 | 0.5% |  0.15 | `Complex operation 2 with DXM`                              |
|   1.82 | 549,056,139.46 | 1.5% |  0.22 | `Complex operation 2 with DXM w/out loads`                  |
|   1.29 | 776,746,535.21 | 0.5% |  0.15 | `Complex operation 2 with Vectormath`                       |
|   1.29 | 776,221,368.26 | 0.8% |  0.15 | `Complex operation 2 with move::vectormath (float)`         |
|   1.30 | 768,781,151.91 | 0.7% |  0.16 | `Complex operation 2 with move::vectormath (double)`        |
|   1.54 | 651,130,622.56 | 1.0% |  0.18 | `Complex operation 3 with SimpleMath::Vector4`              |
|   1.47 | 681,230,702.93 | 0.3% |  0.18 | `Complex operation 3 with glm::vec4`                        |
|   1.23 | 812,961,182.19 | 0.4% |  0.15 | `Complex operation 3 with DXM`                              |
|   1.52 | 656,409,734.24 | 0.8% |  0.18 | `Complex operation 3 with DXM w/out loads`                  |
|   1.50 | 664,736,613.93 | 0.6% |  0.18 | `Complex operation 3 with Vectormath`                       |
|   1.27 | 788,514,597.42 | 0.4% |  0.15 | `Complex operation 3 with move::vectormath (float)`         |
|   1.26 | 792,140,170.50 | 0.2% |  0.15 | `Complex operation 3 with move::vectormath (double)`        |
|   1.25 | 799,037,426.72 | 1.8% |  0.15 | `Complex operation 3 with rtm::vector4f`                    |
|   1.49 | 673,015,120.15 | 1.0% |  0.18 | `Complex operation 3 with rtm::vector4d`                    |
|  54.25 |  18,434,472.41 | 0.9% |  6.50 | `Construct model matrix SimpleMath`                         |
| 142.96 |   6,994,748.46 | 0.2% | 17.07 | `Construct model matrix glm`                                |
|  81.34 |  12,294,671.03 | 0.9% |  9.74 | `Construct model matrix DXM (manual)`                       |
|  64.98 |  15,389,904.25 | 0.5% |  7.76 | `Construct model matrix DXM (builtin)`                      |
|  72.27 |  13,837,556.48 | 0.4% |  8.64 | `Construct model matrix Vectormath`                         |
|  33.67 |  29,695,692.91 | 0.4% |  4.01 | `Construct model matrix move::vectormath (manual, float)`   |
|   6.65 | 150,424,326.39 | 0.3% |  0.80 | `Construct model matrix move::vectormath (builtin, float)`  |
| 174.16 |   5,741,751.57 | 0.6% | 20.80 | `Construct model matrix move::vectormath (manual, double)`  |
|  46.55 |  21,483,052.45 | 0.7% |  5.56 | `Construct model matrix move::vectormath (builtin, double)` |
|   1.25 | 799,420,765.79 | 0.2% |  0.15 | `Construct rtm::qvvf (~transformation matrix)`              |
|   7.44 | 134,363,453.14 | 1.2% |  0.89 | `Construct rtm::qvvd (~transformation matrix)`              |
|  13.89 |  72,007,035.47 | 0.5% |  1.65 | `Construct model matrix rtm::matrix3x4f`                    |
|  35.99 |  27,784,175.86 | 0.9% |  4.30 | `Construct model matrix rtm::matrix3x4d`                    |
|   6.84 | 146,253,456.60 | 0.6% |  0.82 | `Construct model matrix rtm::matrix4x4f`                    |
|  25.68 |  38,946,395.77 | 0.2% |  3.06 | `Construct model matrix rtm::matrix4x4d`                    |
|  26.21 |  38,156,981.51 | 0.3% |  3.13 | `Construct view matrix SimpleMath`                          |
|  21.32 |  46,912,278.24 | 0.4% |  2.55 | `Construct view matrix glm`                                 |
|  26.08 |  38,340,562.83 | 0.9% |  3.12 | `Construct view matrix DXM`                                 |
|  22.64 |  44,167,100.82 | 1.1% |  2.70 | `Construct view matrix Vectormath`                          |
|  11.76 |  85,063,156.57 | 1.0% |  1.41 | `Construct view matrix move::vectormath (float)`            |
|  21.01 |  47,594,329.66 | 0.8% |  2.51 | `Construct view matrix move::vectormath (double)`           |
|   7.14 | 140,011,619.30 | 0.8% |  0.86 | `Construct view matrix rtm::matrix4x4f`                     |
|  20.77 |  48,134,945.30 | 0.5% |  2.48 | `Construct view matrix rtm::matrix4x4d`                     |
|  12.26 |  81,567,322.85 | 0.6% |  1.47 | `Construct perspective matrix SimpleMath`                   |
|   1.98 | 504,736,368.74 | 0.2% |  0.24 | `Construct perspective matrix glm`                          |
|  11.41 |  87,624,218.28 | 0.6% |  1.36 | `Construct perspective matrix DXM`                          |
|   1.46 | 684,897,725.32 | 0.3% |  0.17 | `Construct perspective matrix Vectormath`                   |
|   5.52 | 181,261,145.42 | 0.8% |  0.66 | `Construct perspective matrix move::vectormath (float)`     |
|   2.45 | 408,135,727.48 | 0.6% |  0.29 | `Construct perspective matrix move::vectormath (double)`    |
|   5.57 | 179,515,792.87 | 0.5% |  0.67 | `Construct perspective matrix rtm::matrix4x4f`              |
|   2.43 | 411,330,993.36 | 0.3% |  0.29 | `Construct perspective matrix rtm::matrix4x4d`              |
|  11.03 |  90,649,272.01 | 0.6% |  1.32 | `Construct ortho matrix SimpleMath`                         |
|   1.23 | 816,291,105.93 | 0.1% |  0.15 | `Construct ortho matrix glm`                                |
|   1.49 | 673,085,087.81 | 1.2% |  0.18 | `Construct ortho matrix DXM`                                |
|   3.61 | 277,112,342.20 | 0.7% |  0.43 | `Construct ortho matrix Vectormath`                         |
|   1.47 | 680,945,409.39 | 0.3% |  0.18 | `Construct ortho matrix move::vectormath (float)`           |
|   2.48 | 403,098,927.03 | 0.8% |  0.30 | `Construct ortho matrix move::vectormath (double)`          |
|   1.49 | 673,128,651.72 | 0.4% |  0.18 | `Construct ortho matrix rtm::matrix4x4f`                    |
|   2.46 | 407,310,836.21 | 0.5% |  0.29 | `Construct ortho matrix rtm::matrix4x4d`                    |
|   6.94 | 144,099,738.29 | 0.3% |  0.83 | `Vector matrix multiply SimpleMath`                         |
|   5.47 | 182,671,213.45 | 0.7% |  0.65 | `Vector matrix multiply glm`                                |
|   6.84 | 146,258,184.37 | 0.5% |  0.82 | `Vector matrix multiply DXM`                                |
|   7.67 | 130,400,028.17 | 1.5% |  0.92 | `Vector matrix multiply Vectormath`                         |
|   6.90 | 144,844,007.13 | 0.6% |  0.82 | `Vector matrix multiply move::vectormath (float)`           |
|   6.97 | 143,479,969.35 | 0.2% |  0.83 | `Vector matrix multiply move::vectormath (double)`          |
|   1.46 | 685,306,840.30 | 0.2% |  0.17 | `Vector matrix multiply rtm::matrix4x4f`                    |
|   1.96 | 511,100,659.32 | 0.2% |  0.23 | `Vector matrix multiply rtm::matrix4x4d`                    |
|   3.08 | 324,637,161.19 | 0.2% |  0.37 | `Point QVV multiply rtm::qvvf`                              |
|  10.98 |  91,102,162.66 | 0.6% |  1.31 | `Point QVV multiply rtm::qvvd`                              |
|  14.63 |  68,364,945.09 | 1.0% |  1.74 | `Matrix matrix multiply SimpleMath`                         |
|  19.17 |  52,171,542.64 | 0.6% |  2.27 | `Matrix matrix multiply glm`                                |
|   9.51 | 105,174,147.35 | 1.1% |  1.14 | `Matrix matrix multiply DXM`                                |
|   3.95 | 252,885,479.36 | 0.6% |  0.47 | `Matrix matrix multiply Vectormath`                         |
|   2.84 | 352,562,305.67 | 0.3% |  0.34 | `Matrix matrix multiply move::vectormath (float)`           |
|  15.61 |  64,064,669.84 | 0.8% |  1.86 | `Matrix matrix multiply move::vectormath (double)`          |
|  11.04 |  90,591,757.69 | 1.1% |  1.32 | `QVV QVV multiply rtm::qvvf`                                |
|  25.29 |  39,545,413.46 | 0.8% |  3.02 | `QVV QVV multiply rtm::qvvd`                                |
|   2.88 | 347,527,563.57 | 0.4% |  0.34 | `Matrix matrix multiply rtm::matrix4x4f`                    |
|  15.47 |  64,626,592.89 | 1.4% |  1.86 | `Matrix matrix multiply rtm::matrix4x4d`                    |
