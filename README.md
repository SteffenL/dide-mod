# dide_mod

The original simple pak loader for Dead Island Definitive Edition game series and accidentally also Dying Light.

## Requirements

* C++20 compiler
* CMake
* Ninja

## Building

### Linux/MinGW

```
cmake -G "Ninja Multi-Config" -B build -D CMAKE_TOOLCHAIN_FILE=cmake/toolchains/x86_64-windows-mingw.cmake
cmake --build build --config Release --target package
```

### Windows/MSVC

```
cmake -G "Visual Studio 17 2022" -B build
cmake --build build --config Release --target package
```
