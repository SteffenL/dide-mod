generate_scm_version(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/CoreVersion.cpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/CoreVersion.cpp"
    "CORE_VERSION_SCM_COMMIT")

add_library(dm_core STATIC)
target_sources(dm_core PRIVATE
    src/Config.cpp
    src/ConfigFile.cpp
    src/DideMod.cpp
    src/DideModInitializer.cpp
    src/HostAppTargetInfo.cpp
    src/Logging.cpp
    src/ModMsgBox.cpp
    src/MsgBox.cpp
    src/SequenceSearcher.cpp
    src/StandardPaths.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/generated/CoreVersion.cpp"
)
target_include_directories(dm_core PUBLIC include)
target_link_libraries(dm_core PRIVATE dbghelp shlwapi minhook nowide)

if(MINGW)
    target_compile_options(dm_core PRIVATE -fpermissive)
endif()
