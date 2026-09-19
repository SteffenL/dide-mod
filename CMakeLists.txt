cmake_minimum_required(VERSION 3.22)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

project(dide_mod)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

if(NOT MSVC)
    add_compile_options(-static)
    add_link_options(-static)
endif()

include(cmake/scm.cmake)

add_subdirectory(thirdparty)
add_subdirectory(main)
