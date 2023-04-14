cmake_minimum_required(VERSION 3.14)

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
CPMAddPackage("gh:microsoft/DirectXMath#dec2022")

target_link_libraries(vectormathbench_exe PRIVATE glm DirectXMath)

if (UNIX)
  target_include_directories(vectormathbench_exe PRIVATE src/shims/linux)
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
