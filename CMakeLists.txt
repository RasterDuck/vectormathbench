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
CPMAddPackage("gh:move-engine/move-vectormath#33673bec3f7910cd6d23fb20f516f9cd6cad8421")

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
target_link_libraries(vmb_common INTERFACE glm DirectXMath sony_vectormath move::vectormath rtm rtm)
target_sources(vmb_common INTERFACE src/main.cpp)

if (UNIX)
  # Required for DXM
  target_include_directories(vmb_common INTERFACE src/shims/linux)
  
  # Ensure we're targeting x86-64
  target_compile_options(vmb_common INTERFACE -march=x86-64)
  
  target_compile_definitions(vmb_common INTERFACE -DGLM_FORCE_PURE=1)

  # If we're not in debug config
  if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(vmb_common INTERFACE -O3)
  endif()
endif()

add_executable(vectormathbench_sse42)
target_link_libraries(vectormathbench_sse42 PRIVATE vmb_common)
set_property(TARGET vectormathbench_sse42 PROPERTY OUTPUT_NAME vectormathbench_sse42)

add_executable(vectormathbench_avx)
target_link_libraries(vectormathbench_avx PRIVATE vmb_common)
set_property(TARGET vectormathbench_avx PROPERTY OUTPUT_NAME vectormathbench_avx)


add_executable(vectormathbench_avx2)
target_link_libraries(vectormathbench_avx2 PRIVATE vmb_common)
set_property(TARGET vectormathbench_avx2 PROPERTY OUTPUT_NAME vectormathbench_avx2)

if (WIN32)
  target_compile_options(vectormathbench_sse42 PRIVATE /arch:SSE42)
  target_compile_options(vectormathbench_avx PRIVATE /arch:AVX)
  target_compile_options(vectormathbench_avx2 PRIVATE /arch:AVX2)
elseif (UNIX)
  target_compile_options(vectormathbench_sse42 PRIVATE -msse4.2 -mno-avx)
  target_compile_options(vectormathbench_avx PRIVATE -mavx)
  target_compile_options(vectormathbench_avx2 PRIVATE -mfma -mavx2)
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
