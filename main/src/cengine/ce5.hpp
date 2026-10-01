#pragma once

#include "../dynlib.hpp"
#include "../misc.hpp"
#include "../platform.hpp"

namespace ce5 {
namespace fs {
struct FFSAddSourceFlags {
    enum ENUM {
        SUBDIRS = 1,
        APPEND = 2,
        STRIP_LAST_DIR = 4,
        BROWSABLE = 8,
        ALLOW_DUPLICATES = 16,
        PRELOAD = 32,
    };
};

using add_source_t = bool (*)(const char* path, FFSAddSourceFlags::ENUM flags);

template<template<typename> typename Wrapper>
struct Functions {
    Functions() = default;
    Functions(const DynLib& lib) : add_source{lib.sym<add_source_t>("?add_source@fs@@YA_NPBDH@Z")} {}

    Wrapper<add_source_t> add_source;
};

} // namespace fs

namespace engine {
using InitializeGameScriptDLL_t = void (*)();

template<template<typename> typename Wrapper>
struct Functions {
    Functions() = default;
    Functions(const DynLib& lib)
            : InitializeGameScriptDLL{lib.sym<InitializeGameScriptDLL_t>("InitializeGameScriptDLL")} {}

    Wrapper<InitializeGameScriptDLL_t> InitializeGameScriptDLL;
};
} // namespace engine

struct Libraries {
    struct {
        DynLib lib{DynLib::from_loaded(exe_dir() / "game_x86_rwdi.dll")};
    } game;
    struct {
        DynLib lib{DynLib::from_loaded(exe_dir() / "engine_x86_rwdi.dll")};
        engine::Functions<NotNull> fn{lib};
    } engine;
    struct {
        DynLib lib{DynLib::from_loaded(exe_dir() / "filesystem_x87_rwdi.dll")};
        fs::Functions<NotNull> fn{lib};
    } filesystem;
};
} // namespace ce5
