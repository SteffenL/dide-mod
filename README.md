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

- Bash shell for the build script (optional)
- C++20 compiler
- CMake >= 3.22
- Ninja

## Building

With this command the project is built for x86 and x64 separately, then combined into a distributable archive in `dist/`.

```
scripts/build.sh
```
