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

add_executable(vectormathbench_exe src/main.cpp)
add_executable(vectormathbench::exe ALIAS vectormathbench_exe)

set_property(TARGET vectormathbench_exe PROPERTY OUTPUT_NAME vectormathbench)

target_compile_features(vectormathbench_exe PRIVATE cxx_std_17)

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

target_link_libraries(vectormathbench_exe PRIVATE glm DirectXMath sony_vectormath)
target_compile_definitions(vectormathbench_exe PRIVATE __AVX__)
target_compile_definitions(vectormathbench_exe PRIVATE _XM_AVX_INTRINSICS_)
target_compile_definitions(vectormathbench_exe PRIVATE _XM_SSE_INTRINSICS_)

if (UNIX)
  target_include_directories(vectormathbench_exe PRIVATE src/shims/linux)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx")
endif()

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

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
