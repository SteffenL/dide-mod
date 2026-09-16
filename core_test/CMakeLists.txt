add_executable(dm_core_test)
target_sources(dm_core_test PRIVATE main.cpp)
target_link_libraries(dm_core_test PRIVATE dm_core)
add_test(NAME dm_core_test COMMAND dm_core_test)
