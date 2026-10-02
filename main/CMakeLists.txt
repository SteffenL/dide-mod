generate_scm_version(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.cpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/version.cpp"
    "PROJECT_SCM_COMMIT")

add_library(dm MODULE)
target_sources(dm PRIVATE
    src/config.cpp
    src/dll_notify.cpp
    src/dynlib.cpp
    src/host.cpp
    src/log.cpp
    src/main.cpp
    src/minhook.cpp
    src/mod_impl/ce5_impl.cpp
    src/mod_impl/ce6_impl.cpp
    src/mod.cpp
    src/platform.cpp
    src/unicode.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/generated/version.cpp"
)

if(DM_USE_DSOUND_PROXY)
    target_compile_definitions(dm PRIVATE DM_USE_DSOUND_PROXY)
    target_sources(dm PRIVATE
        src/dsound/dsound.cpp
        src/dsound/exports.cpp
        src/dsound/exports.def
    )
endif()

if(DM_USE_XINPUT_PROXY)
    target_compile_definitions(dm PRIVATE DM_USE_XINPUT_PROXY)
    target_sources(dm PRIVATE
        src/xinput/exports.cpp
        src/xinput/exports.def
        src/xinput/xinput.cpp
    )
endif()

target_link_libraries(dm PRIVATE dbghelp shlwapi version minhook)
set_target_properties(dm PROPERTIES
    OUTPUT_NAME dide_mod
    PREFIX ""
    CXX_VISIBILITY_PRESET hidden
)

install(TARGETS dm LIBRARY DESTINATION . COMPONENT dm_archive)
