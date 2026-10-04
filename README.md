# dide_mod

The original simple pak loader for Dead Island game series and accidentally also Dying Light.

# Supported Games

## Chrome Engine 5 Games

- Dead Island
- Dead Island Riptide

## Chrome Engine 6 Games

- Dead Island Definitive Edition
- Dead Island Riptide Definitive Edition
- Dying Light (partial)

# Features

* Load custom pak files.
* Enable the in-game developer menu (not supported for Dying Light).

## Requirements

* C++20 compiler
* CMake
* Ninja

## Building

### Linux/MinGW

For CE5 games, specify the `i686` toolchain file; for CE6, specify the `x86_64` toolchain file.

```
cmake -G "Ninja Multi-Config" -B build -D CMAKE_TOOLCHAIN_FILE=cmake/toolchains/x86_64-windows-mingw.cmake
cmake --build build --config Release --target package
```

### Windows/MSVC

For CE5 games, specify `Win32` as the architecture; for CE6, specify `x64`.

```
cmake -G "Visual Studio 17 2022" -A x64 -B build
cmake --build build --config Release --target package
```
