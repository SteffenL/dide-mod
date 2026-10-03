#pragma once

#include "../../dynlib.hpp"
#include "../../misc.hpp"

namespace ce6 {
namespace fs {
struct FFSAddSourceFlags {
    // Some names discovered in debug info of Dead Island, others unconfirmed
    enum ENUM {
        SUBDIRS = 1,
        APPEND = 2,
        STRIP_LAST_DIR = 4,
        BROWSABLE = 8,
        ALLOW_DUPLICATES = 16,
        PRELOAD = 32,
        CACHE = 64, // Guess based on: if fs_cache_enabled && (flags & 0x40) == 0 then init_cache()
    };
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

struct Libraries {
    bool all_ok() const { return game.lib.has_value() && engine.lib.has_value() && filesystem.lib.has_value(); }

    struct {
        std::optional<DynLib> lib;
    } game;
    struct {
        std::optional<DynLib> lib;
        std::optional<engine::Functions<NotNull>> fn;
    } engine;
    struct {
        std::optional<DynLib> lib;
        std::optional<fs::Functions<NotNull>> fn;
    } filesystem;
};
} // namespace ce6
