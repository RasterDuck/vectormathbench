install(
    TARGETS vectormathbench_exe
    RUNTIME COMPONENT vectormathbench_Runtime
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
