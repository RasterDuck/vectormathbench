cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED on)

include(cmake/prelude.cmake)

project(
    vectormathbench
    VERSION 0.1.0
    DESCRIPTION "A benchmarking utility for comparing various vector math libraries for games"
    HOMEPAGE_URL "https://github.com/rasterduck/vectormathbench"
    LANGUAGES CXX
)

include(CTest)

# Add fdiagnostics-color=always when the compiler supports it
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fdiagnostics-color" HAS_DIAGNOSTICS_COLOR)
if (HAS_DIAGNOSTICS_COLOR)
  add_compile_options(-fdiagnostics-color=always)
endif()

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

include(cmake/cpm.cmake)

# ---- Declare executable ----


CPMAddPackage("gh:g-truc/glm#0.9.9.8@0.9.9.8")
CPMAddPackage("gh:microsoft/DirectXMath#dec2022")
set(MOVE_VECTORMATH_SOURCE_DIR "" CACHE PATH
    "Optional local move-vectormath checkout for architecture benchmarks")

if(MOVE_VECTORMATH_SOURCE_DIR)
  add_subdirectory(
    "${MOVE_VECTORMATH_SOURCE_DIR}"
    "${CMAKE_BINARY_DIR}/move-vectormath-local"
    EXCLUDE_FROM_ALL
  )
else()
  CPMAddPackage(
    NAME move-vectormath
    GITHUB_REPOSITORY move-engine/move-vectormath
    GIT_TAG c32dbda5b34b43359662a8b00129bf0f609d91fc
  )
endif()

# CPMAddPackage(
#   NAME rtm
#   GITHUB_REPOSITORY nfrechette/rtm
#   GIT_TAG v2.1.5
#   DOWNLOAD_ONLY YES
# )

# add_library(rtm INTERFACE)
# target_include_directories(rtm INTERFACE ${rtm_SOURCE_DIR}/includes)

CPMAddPackage(
  NAME sony_vectormath
  GITHUB_REPOSITORY glampert/vectormath
  GIT_TAG ee960fad0a4bbbf0ee2e7d03fc749c49ebeefaef
  DOWNLOAD_ONLY YES
)

add_library(sony_vectormath INTERFACE)
target_include_directories(sony_vectormath INTERFACE ${sony_vectormath_SOURCE_DIR})

add_library(vmb_common INTERFACE)
target_link_libraries(vmb_common INTERFACE glm DirectXMath sony_vectormath move::vectormath rtm)
target_sources(vmb_common INTERFACE src/main.cpp)

if (UNIX)
  # Required for DXM
  target_include_directories(vmb_common INTERFACE src/shims/linux)

  # DirectXMath and Sony Vectormath contain pointer type-punning that is not
  # valid under GCC's strict-aliasing rules. Without this, optimized AVX builds
  # can discard DirectXMath stores and benchmark operations that never happen.
  target_compile_options(vmb_common INTERFACE -fno-strict-aliasing)
  
  # Ensure we're targeting x86-64
  target_compile_options(vmb_common INTERFACE -march=x86-64)
  
  # If we're not in debug config
  if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(vmb_common INTERFACE -O3)
  endif()
endif()

add_executable(vectormathbench_sse42)
target_link_libraries(vectormathbench_sse42 PRIVATE vmb_common)
set_property(TARGET vectormathbench_sse42 PROPERTY OUTPUT_NAME
  vectormathbench_capabilities_sse42)

add_executable(vectormathbench_avx)
target_link_libraries(vectormathbench_avx PRIVATE vmb_common)
set_property(TARGET vectormathbench_avx PROPERTY OUTPUT_NAME
  vectormathbench_capabilities_avx)


add_executable(vectormathbench_avx2)
target_link_libraries(vectormathbench_avx2 PRIVATE vmb_common)
set_property(TARGET vectormathbench_avx2 PROPERTY OUTPUT_NAME
  vectormathbench_capabilities_avx2)

if (WIN32)
  target_compile_options(vectormathbench_sse42 PRIVATE /arch:SSE42)
  target_compile_options(vectormathbench_avx PRIVATE /arch:AVX)
  target_compile_options(vectormathbench_avx2 PRIVATE /arch:AVX2)
elseif (UNIX)
  target_compile_options(vectormathbench_sse42 PRIVATE -msse4.2 -mno-avx)
  target_compile_options(vectormathbench_avx PRIVATE -mavx)
  target_compile_options(vectormathbench_avx2 PRIVATE -mfma -mavx2)
endif()

function(add_representation_benchmark target)
  add_executable(${target} src/representation.cpp)
  target_link_libraries(${target} PRIVATE move::vectormath)
  target_include_directories(${target} PRIVATE src)

  if (UNIX)
    target_compile_options(${target} PRIVATE
      -O3
      -march=x86-64
      ${ARGN}
    )
  elseif (MSVC)
    target_compile_options(${target} PRIVATE /O2 ${ARGN})
  endif()
endfunction()

function(add_semantic_transform_benchmark target)
  add_executable(${target} src/semantic_transforms.cpp)
  target_link_libraries(${target} PRIVATE move::vectormath)
  target_include_directories(${target} PRIVATE src)

  if (UNIX)
    target_compile_options(${target} PRIVATE
      -O3
      -march=x86-64
      ${ARGN}
    )
  elseif (MSVC)
    target_compile_options(${target} PRIVATE /O2 ${ARGN})
  endif()
endfunction()

function(add_geometry_query_benchmark target)
  add_executable(${target} src/geometry_queries.cpp)
  target_link_libraries(${target} PRIVATE move::vectormath glm DirectXMath)
  target_include_directories(${target} PRIVATE src)

  if (UNIX)
    target_include_directories(${target} PRIVATE src/shims/linux)
    target_compile_options(${target} PRIVATE
      -O3
      -march=x86-64
      -fno-strict-aliasing
      ${ARGN}
    )
  elseif (MSVC)
    target_compile_options(${target} PRIVATE /O2 ${ARGN})
  endif()
endfunction()

function(add_core_operations_benchmark target)
  add_executable(${target} src/core_operations.cpp)
  target_link_libraries(${target} PRIVATE move::vectormath)
  target_include_directories(${target} PRIVATE src)

  if (UNIX)
    target_compile_options(${target} PRIVATE
      -O3
      -march=x86-64
      ${ARGN}
    )
  elseif (MSVC)
    target_compile_options(${target} PRIVATE /O2 ${ARGN})
  endif()
endfunction()

function(add_matrix_operations_benchmark target)
  add_executable(${target} src/matrix_operations.cpp)
  target_link_libraries(${target} PRIVATE move::vectormath)
  target_include_directories(${target} PRIVATE src)

  if (UNIX)
    target_compile_options(${target} PRIVATE
      -O3
      -march=x86-64
      ${ARGN}
    )
  elseif (MSVC)
    target_compile_options(${target} PRIVATE /O2 ${ARGN})
  endif()
endfunction()

if (WIN32)
  add_representation_benchmark(vectormathbench_representation_sse42 /arch:SSE42)
  add_representation_benchmark(vectormathbench_representation_avx /arch:AVX)
  add_representation_benchmark(vectormathbench_representation_avx2 /arch:AVX2)
  add_semantic_transform_benchmark(vectormathbench_semantic_transforms_sse42 /arch:SSE42)
  add_semantic_transform_benchmark(vectormathbench_semantic_transforms_avx /arch:AVX)
  add_semantic_transform_benchmark(vectormathbench_semantic_transforms_avx2 /arch:AVX2)
  add_geometry_query_benchmark(vectormathbench_geometry_queries_sse42 /arch:SSE42)
  add_geometry_query_benchmark(vectormathbench_geometry_queries_avx /arch:AVX)
  add_geometry_query_benchmark(vectormathbench_geometry_queries_avx2 /arch:AVX2)
  add_core_operations_benchmark(vectormathbench_core_operations_sse42 /arch:SSE42)
  add_core_operations_benchmark(vectormathbench_core_operations_avx /arch:AVX)
  add_core_operations_benchmark(vectormathbench_core_operations_avx2 /arch:AVX2)
  add_matrix_operations_benchmark(vectormathbench_matrix_operations_sse42 /arch:SSE42)
  add_matrix_operations_benchmark(vectormathbench_matrix_operations_avx /arch:AVX)
  add_matrix_operations_benchmark(vectormathbench_matrix_operations_avx2 /arch:AVX2)
elseif (UNIX)
  add_representation_benchmark(
    vectormathbench_representation_sse42
    -msse4.2
    -mno-avx
  )
  add_representation_benchmark(vectormathbench_representation_avx -mavx)
  add_representation_benchmark(vectormathbench_representation_avx2 -mfma -mavx2)
  add_semantic_transform_benchmark(
    vectormathbench_semantic_transforms_sse42
    -msse4.2
    -mno-avx
  )
  add_semantic_transform_benchmark(vectormathbench_semantic_transforms_avx -mavx)
  add_semantic_transform_benchmark(vectormathbench_semantic_transforms_avx2 -mfma -mavx2)
  add_geometry_query_benchmark(
    vectormathbench_geometry_queries_sse42
    -msse4.2
    -mno-avx
  )
  add_geometry_query_benchmark(vectormathbench_geometry_queries_avx -mavx)
  add_geometry_query_benchmark(vectormathbench_geometry_queries_avx2 -mfma -mavx2)
  add_core_operations_benchmark(
    vectormathbench_core_operations_sse42
    -msse4.2
    -mno-avx
  )
  add_core_operations_benchmark(vectormathbench_core_operations_avx -mavx)
  add_core_operations_benchmark(vectormathbench_core_operations_avx2 -mfma -mavx2)
  add_matrix_operations_benchmark(
    vectormathbench_matrix_operations_sse42
    -msse4.2
    -mno-avx
  )
  add_matrix_operations_benchmark(vectormathbench_matrix_operations_avx -mavx)
  add_matrix_operations_benchmark(vectormathbench_matrix_operations_avx2 -mfma -mavx2)
else()
  add_representation_benchmark(vectormathbench_representation_sse42)
  add_semantic_transform_benchmark(vectormathbench_semantic_transforms_sse42)
  add_geometry_query_benchmark(vectormathbench_geometry_queries_sse42)
  add_core_operations_benchmark(vectormathbench_core_operations_sse42)
  add_matrix_operations_benchmark(vectormathbench_matrix_operations_sse42)
endif()

if(BUILD_TESTING)
  add_test(
    NAME vectormathbench_semantic_parity_sse42
    COMMAND vectormathbench_sse42 --verify-only
  )
  add_test(
    NAME vectormathbench_semantic_parity_avx
    COMMAND vectormathbench_avx --verify-only
  )
  add_test(
    NAME vectormathbench_semantic_parity_avx2
    COMMAND vectormathbench_avx2 --verify-only
  )
  add_test(
    NAME vectormathbench_geometry_query_parity_sse42
    COMMAND vectormathbench_geometry_queries_sse42 --verify-only
  )
  add_test(
    NAME vectormathbench_geometry_query_parity_avx
    COMMAND vectormathbench_geometry_queries_avx --verify-only
  )
  add_test(
    NAME vectormathbench_geometry_query_parity_avx2
    COMMAND vectormathbench_geometry_queries_avx2 --verify-only
  )
  add_test(
    NAME vectormathbench_core_operations_parity_sse42
    COMMAND vectormathbench_core_operations_sse42 --verify-only
  )
  add_test(
    NAME vectormathbench_core_operations_parity_avx
    COMMAND vectormathbench_core_operations_avx --verify-only
  )
  add_test(
    NAME vectormathbench_core_operations_parity_avx2
    COMMAND vectormathbench_core_operations_avx2 --verify-only
  )
  add_test(
    NAME vectormathbench_matrix_operations_parity_sse42
    COMMAND vectormathbench_matrix_operations_sse42 --verify-only
  )
  add_test(
    NAME vectormathbench_matrix_operations_parity_avx
    COMMAND vectormathbench_matrix_operations_avx --verify-only
  )
  add_test(
    NAME vectormathbench_matrix_operations_parity_avx2
    COMMAND vectormathbench_matrix_operations_avx2 --verify-only
  )
endif()

# # ---- Install rules ----

# if(NOT CMAKE_SKIP_INSTALL_RULES)
#   include(cmake/install-rules.cmake)
# endif()

# ---- Developer mode ----

if(NOT vectormathbench_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of vectormathbench"
  )
endif()

include(cmake/dev-mode.cmake)
