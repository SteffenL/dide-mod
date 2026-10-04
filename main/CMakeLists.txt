generate_scm_version(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.cpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/version.cpp"
    "PROJECT_SCM_COMMIT")

add_library(dm MODULE)
target_sources(dm PRIVATE
    src/config.cpp
    src/dll_notify.cpp
    src/dllmain.cpp
    src/dsound/dsound.cpp
    src/dsound/exports.cpp
    src/dsound/exports.def
    src/dx11.cpp
    src/dynlib.cpp
    src/host.cpp
    src/log.cpp
    src/minhook.cpp
    src/mod_impl/factory.cpp
    src/mod.cpp
    src/platform.cpp
    src/string.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/generated/version.cpp"
)

try_compile(CE5_SUPPORTED SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/mod_impl/ce5/compiler_check.cpp")
if (CE5_SUPPORTED)
    target_sources(dm PRIVATE src/mod_impl/ce5/ce5_mod.cpp)
endif()

try_compile(CE6_SUPPORTED SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/mod_impl/ce6/compiler_check.cpp")
if (CE6_SUPPORTED)
    target_sources(dm PRIVATE src/mod_impl/ce6/ce6_mod.cpp)
endif()

target_link_libraries(dm PRIVATE dbghelp shlwapi version minhook)
set_target_properties(dm PROPERTIES
    OUTPUT_NAME dsound
    PREFIX ""
    CXX_VISIBILITY_PRESET hidden
)

install(TARGETS dm LIBRARY DESTINATION . COMPONENT dm_archive)
