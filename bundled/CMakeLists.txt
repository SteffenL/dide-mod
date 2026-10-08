configure_file(
    dide_mod_readme.txt.in
    generated/dide_mod_readme.txt
    @ONLY
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/generated/dide_mod_readme.txt"
        dide_mod.ini
    DESTINATION .
    COMPONENT dm_archive
)
