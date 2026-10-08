#!/usr/bin/env bash
set -euo pipefail

is_nightly=0

if [[ "${OSTYPE}" =~ cygwin|msys ]]; then
    toolchain=msvc
    generator=""
else
    toolchain=mingw
    generator="Ninja Multi-Config"
fi

while (( $# > 0 )); do
    arg_name=$1
    shift
    case "${arg_name}" in
        -G)
            generator=$1
            shift
            ;;
        --nightly)
            is_nightly=1
            ;;
        *)
            echo "Invalid argument: ${arg_name}"
            exit 1
            ;;
    esac
done

if [[ ! -z "${generator}" ]]; then
    echo "Generator: ${generator}"
else
    echo "Generator: (not set)"
fi

echo "Nightly: ${is_nightly}"
echo "Toolchain: ${toolchain}"

cmake_args=()
cpack_args=()

if [[ ! -z "${generator}" ]]; then
    cmake_args+=(-G "${generator}")
fi

if [[ "${is_nightly}" == "1" ]]; then
    cpack_args+=(-D DM_NIGHTLY=ON)
fi

if [[ "${toolchain}" == "mingw" ]]; then
    cmake -B build-x86 -D CMAKE_TOOLCHAIN_FILE=cmake/toolchains/i686-windows-mingw.cmake "${cmake_args[@]}"
    cmake --build build-x86 --config Release

    cmake -B build-x64 -D CMAKE_TOOLCHAIN_FILE=cmake/toolchains/x86_64-windows-mingw.cmake "${cmake_args[@]}"
    cmake --build build-x64 --config Release
elif [[ "${toolchain}" == "msvc" ]]; then
    cmake -A Win32 -B build-x86
    cmake --build build-x86 --config Release "${cmake_args[@]}"

    cmake -A x64 -B build-x64
    cmake --build build-x64 --config Release "${cmake_args[@]}"
fi

cpack --config build-x64/CPackConfig.cmake -B dist -C Release -D "CPACK_INSTALL_CMAKE_PROJECTS=build-x86;dide_mod;dm_archive;/;build-x64;dide_mod;dm_archive;/" "${cpack_args[@]}"
