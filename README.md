# dide_mod

The original simple pak loader for Dead Island game series and accidentally also Dying Light.

## Features

- Load custom pak files.
- Enable the in-game developer menu (not supported for Dying Light).

## Supported Games

### Chrome Engine 5 Games

- Dead Island
- Dead Island Riptide

### Chrome Engine 6 Games

- Dead Island Definitive Edition
- Dead Island Riptide Definitive Edition
- Dying Light (partial)

## Runtime Requirements

- OS: Windows

## Build Requirements

- C++20 compiler
- CMake >= 3.22
- Ninja

## Building

With this commands the project is built for x86 and x64 separately, then combined into a distributable archive in `dist/`.

### Compile using Linux/MinGW

```
cmake -G 'Ninja Multi-Config' -B build-x86 -D CMAKE_TOOLCHAIN_FILE=cmake/toolchains/i686-windows-mingw.cmake
cmake --build build-x86 --config Release

cmake -G 'Ninja Multi-Config' -B build-x64 -D CMAKE_TOOLCHAIN_FILE=cmake/toolchains/x86_64-windows-mingw.cmake
cmake --build build-x64 --config Release
```

### Compile using Windows/MSVC

```
cmake -G "Visual Studio 17 2022" -A Win32 -B build-x86
cmake --build build-x86 --config Release

cmake -G "Visual Studio 17 2022" -A x64 -B build-x64
cmake --build build-x64 --config Release
```

### Package

```
cpack --config build-x64/CPackConfig.cmake -B dist -C Release -D "CPACK_INSTALL_CMAKE_PROJECTS=build-x86;dide_mod;dm_archive;/;build-x64;dide_mod;dm_archive;/"
```
