#pragma once

#include "../../dynlib.hpp"
#include "../../misc.hpp"

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
class IGame;

/*class IGame {
public:
    void MountDlc(const char* p1, const char* p2);
};*/

using InitializeGameScript_t = void (*)(void* p1, void* p2);

#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wattributes"
#endif

using MountDlc_t = void(__thiscall*)(IGame* self, const char* p1, const char* p2);

#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif

template<template<typename> typename Wrapper>
struct Functions {
    Functions() = default;
    Functions(const DynLib& lib)
            : InitializeGameScript{lib.sym<InitializeGameScript_t>("InitializeGameScript")},
              IGame_MountDlc{lib.sym<MountDlc_t>("?MountDlc@IGame@@QAEXPBD0@Z")} {}

    Wrapper<InitializeGameScript_t> InitializeGameScript;
    Wrapper<MountDlc_t> IGame_MountDlc;
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
} // namespace ce5
