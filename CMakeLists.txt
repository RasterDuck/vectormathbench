cmake_minimum_required(VERSION 3.21 FATAL_ERROR)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED on)

include(cmake/prelude.cmake)

project(
    vectormathbench
    VERSION 0.1.0
    DESCRIPTION "A benchmarking utility for comparing various vector math libraries for games"
    HOMEPAGE_URL "https://github.com/rasterduck/vectormathbench"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

include(cmake/cpm.cmake)

# ---- Declare executable ----


CPMAddPackage("gh:g-truc/glm#0.9.9.8@0.9.9.8")
CPMAddPackage("gh:g-truc/glm#0.9.9.8@0.9.9.8")
CPMAddPackage("gh:microsoft/DirectXMath#dec2022")

CPMAddPackage(
  NAME sony_vectormath
  GITHUB_REPOSITORY glampert/vectormath
  GIT_TAG ee960fad0a4bbbf0ee2e7d03fc749c49ebeefaef
  DOWNLOAD_ONLY YES
)



add_library(sony_vectormath INTERFACE)
target_include_directories(sony_vectormath INTERFACE ${sony_vectormath_SOURCE_DIR})

add_library(vmb_common INTERFACE)
target_link_libraries(vmb_common INTERFACE glm DirectXMath sony_vectormath)
target_compile_definitions(vmb_common INTERFACE _XM_SSE_INTRINSICS_)
target_sources(vmb_common INTERFACE src/main.cpp)

if (UNIX)
  # Ensure we're targeting x86-64
  target_compile_options(vmb_common INTERFACE -march=x86-64)
  target_include_directories(vmb_common INTERFACE src/shims/linux)
endif()

add_executable(vectormathbench_avx)
target_link_libraries(vectormathbench_avx PRIVATE vmb_common)
set_property(TARGET vectormathbench_avx PROPERTY OUTPUT_NAME vectormathbench_avx)
target_compile_definitions(vectormathbench_avx PRIVATE __AVX__)
target_compile_definitions(vectormathbench_avx PRIVATE _XM_AVX_INTRINSICS_)

if (UNIX)
  target_compile_options(vectormathbench_avx PRIVATE -mavx)
endif()

add_executable(vectormathbench_sse)
target_link_libraries(vectormathbench_sse PRIVATE vmb_common)
set_property(TARGET vectormathbench_sse PROPERTY OUTPUT_NAME vectormathbench_sse)
target_compile_definitions(vectormathbench_sse PRIVATE __SSE__)

if (UNIX)
  target_compile_options(vectormathbench_sse PRIVATE -msse)
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
