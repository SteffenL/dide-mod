include(FetchContent)
find_package(Git REQUIRED)

FetchContent_Declare(minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_TAG c3fcafdc10146beb5919319d0683e44e3c30d537 # v1.3.4
    PATCH_COMMAND "${GIT_EXECUTABLE}" apply "${CMAKE_CURRENT_SOURCE_DIR}/patch/minhook_1.3.4.patch"
)

FetchContent_Declare(nowide
    URL https://github.com/boostorg/nowide/releases/download/v11.3.1/nowide_standalone_v11.3.1.tar.gz
    URL_HASH SHA256=eaec4d331e3961f5eeb10c46a11691d62047900a7a40765b0f23cdd3181e6ca6
)

FetchContent_MakeAvailable(minhook nowide)
