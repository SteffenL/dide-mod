#pragma once

#include "dynlib.hpp"
#include "misc.hpp"

namespace cengine {
namespace fs {
struct FFSAddSourceFlags {
    enum ENUM { Unknown09 = 9 };
};

using add_source_t = bool (*)(const char* path, FFSAddSourceFlags::ENUM flags);

template<template<typename> typename Wrapper>
struct Functions {
    Functions() = default;
    Functions(const DynLib& lib)
            : add_source{lib.sym<add_source_t>("?add_source@fs@@YA_NPEBDW4ENUM@FFSAddSourceFlags@@@Z")} {}

    Wrapper<add_source_t> add_source;
};

} // namespace fs

namespace engine {
using InitializeGameScript_t = void (*)(void* p1, void* p2);

template<template<typename> typename Wrapper>
struct Functions {
    Functions() = default;
    Functions(const DynLib& lib) : InitializeGameScript{lib.sym<InitializeGameScript_t>("InitializeGameScript")} {}

    Wrapper<InitializeGameScript_t> InitializeGameScript;
};
} // namespace engine

struct CEngineLibraries {
    struct {
        DynLib lib{DynLib::from_loaded("gamedll_x64_rwdi.dll")};
    } game;
    struct {
        DynLib lib{DynLib::from_loaded("engine_x64_rwdi.dll")};
        engine::Functions<NotNull> fn{lib};
    } engine;
    struct {
        DynLib lib{DynLib::from_loaded("filesystem_x64_rwdi.dll")};
        fs::Functions<NotNull> fn{lib};
    } filesystem;
};
} // namespace cengine
