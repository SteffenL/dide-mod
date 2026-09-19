generate_scm_version(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.cpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/version.cpp"
    "PROJECT_SCM_COMMIT")

add_library(dm MODULE)
target_sources(dm PRIVATE
    src/config.cpp
    src/dsound.cpp
    src/exports.cpp
    src/exports.def
    src/host.cpp
    src/log.cpp
    src/main.cpp
    src/minhook.cpp
    src/mod.cpp
    src/platform.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/generated/version.cpp"
)
target_link_libraries(dm PRIVATE dbghelp shlwapi minhook nowide)
set_target_properties(dm PROPERTIES
    OUTPUT_NAME dsound
    PREFIX ""
    CXX_VISIBILITY_PRESET hidden
)
