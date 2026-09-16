add_library(dm_wrapper MODULE)
target_sources(dm_wrapper PRIVATE
    exports.def
    hooks.cpp
    main.cpp
    wrapper.cpp
)
target_link_libraries(dm_wrapper PRIVATE dm_core minhook nowide)
set_target_properties(dm_wrapper PROPERTIES
    OUTPUT_NAME dsound
    PREFIX ""
    CXX_VISIBILITY_PRESET hidden
)

if(MINGW)
    target_compile_options(dm_wrapper PRIVATE -fpermissive)
endif()
