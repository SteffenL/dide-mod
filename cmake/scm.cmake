include_guard(GLOBAL)

find_package(Git REQUIRED)

if(DEFINED PROJECT_SCM_VERSION)
    set(CHECK_COMMAND_ARGS "${CMAKE_COMMAND}" -E echo "${PROJECT_SCM_VERSION}")
else()
    set(CHECK_COMMAND_ARGS "${GIT_EXECUTABLE}" rev-parse HEAD)
endif()

set(PROJECT_SCM_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/scm_version.txt")

add_custom_command(
    OUTPUT "${PROJECT_SCM_VERSION_FILE}"
    COMMAND ${CHECK_COMMAND_ARGS} > "${PROJECT_SCM_VERSION_FILE}.tmp"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${PROJECT_SCM_VERSION_FILE}.tmp" "${PROJECT_SCM_VERSION_FILE}"
    COMMAND "${CMAKE_COMMAND}" -E rm "${PROJECT_SCM_VERSION_FILE}.tmp"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMENT "Checking SCM version"
    VERBATIM
)
add_custom_target(generate_project_scm_version_file DEPENDS "${PROJECT_SCM_VERSION_FILE}")

function(generate_scm_version INPUT_FILE OUTPUT_FILE VARIABLE_NAME)
    add_custom_command(
        OUTPUT "${OUTPUT_FILE}"
        COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/scm_gen.cmake" "${INPUT_FILE}" "${OUTPUT_FILE}" "${PROJECT_SCM_VERSION_FILE}" "${VARIABLE_NAME}"
        DEPENDS "${INPUT_FILE}" "${PROJECT_SCM_VERSION_FILE}" generate_project_scm_version_file
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        COMMENT "Generating SCM version source file"
        VERBATIM
    )
endfunction()
