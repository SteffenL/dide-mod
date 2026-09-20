#include "mod.hpp"
#include "config.hpp"
#include "host.hpp"
#include "log.hpp"
#include "minhook.hpp"
#include "misc.hpp"
#include "pattern.hpp"
#include "platform.hpp"
#include "unicode.hpp"
#include "version.hpp"

#include <cstdlib>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <windows.h>

namespace cengine {
struct FFSAddSourceFlags {
    enum ENUM { Unknown09 = 9 };
};

using fs_add_source_t = bool (*)(const char* path, FFSAddSourceFlags::ENUM flags);
using engine_InitializeGameScript_t = void (*)(void* p1, void* p2);

template<template<typename> typename Wrapper>
struct FsFunctions {
    Wrapper<fs_add_source_t> add_source;
};

template<template<typename> typename Wrapper>
struct EngineFunctions {
    Wrapper<engine_InitializeGameScript_t> InitializeGameScript;
};

template<template<typename> typename Wrapper>
struct Functions {
    FsFunctions<Wrapper> fs;
    EngineFunctions<Wrapper> engine;
};

FsFunctions<NotNull> load_fs_functions(NotNull<HMODULE> dll) {
    return {
        .add_source = gpa<fs_add_source_t>(dll.get(), "?add_source@fs@@YA_NPEBDW4ENUM@FFSAddSourceFlags@@@Z"),
    };
}

EngineFunctions<NotNull> load_engine_functions(NotNull<HMODULE> dll) {
    return {
        .InitializeGameScript = gpa<engine_InitializeGameScript_t>(dll.get(), "InitializeGameScript"),
    };
}

} // namespace cengine

void log_version() { LOG("Core version: {}", project_get_version()); }

void validate_config(const config::Config& cfg) {
    if (cfg.general.enable_logging) {
        if (cfg.general.log_file.empty()) {
            throw Error{"Log file path is not configured!"};
        }
    }
}

namespace {
constexpr std::string_view msgbox_title_prefix{"[DIDE mod] "};
config::Config g_config;
HostAppInfo g_host_info;
std::optional<cengine::Functions<NotNull>> g_cengine;
std::optional<cengine::Functions<std::type_identity_t>> g_cengine_original;
bool* g_dev_menu_enabled{};
} // namespace

config::Config load_config() {
    const auto config_file_path{exe_dir() / "dide_mod.ini"};
    try {
        return config::load_file(config_file_path);
    } catch (const std::exception& ex) {
        throw Error::format("Unable to load config file at {}: {}", narrow_string(config_file_path.wstring()),
                            ex.what());
    }
}

void log_config(const config::Config& cfg) {
    LOG_TX([&] {
        LOG("Configuration:");
        LOG("  General:");
        LOG("    EnableMod = {}", cfg.general.enable_mod);
        LOG("    EnableLogging = {}", cfg.general.enable_logging);
        LOG("    LogFile = {}", narrow_string(cfg.general.log_file.wstring()));
        LOG("  Features:");
        LOG("    DeveloperMenu = {}", cfg.features.developer_menu);
        LOG("    CustomPak = {}", cfg.features.custom_pak);
        LOG("  CustomPak:");
        for (const auto& pak : cfg.load_custom_paks) {
            LOG("    {}", narrow_string(pak.wstring()));
        }
    });
}

void log_host_info(const HostAppInfo& info) {
    LOG_TX([&] {
        LOG("Game DLL image base: {:#x}", info.game_dll);
        LOG("Engine DLL image base: {:#x}", info.engine_dll);
        LOG("Filesystem DLL image base: {:#x}", info.filesystem_dll);
    });
}

void load_paks(const config::Config& cfg) {
    for (const auto& pak_path : cfg.load_custom_paks) {
        const auto pak_path_utf8{pak_path.u8string()};
        const auto* pak_path_c{reinterpret_cast<const char*>(pak_path_utf8.c_str())};
        LOG_TX([&] {
            LOG_PARTIAL("Adding custom source: {}", pak_path_c);
            const auto loaded{g_cengine_original->fs.add_source(pak_path_c, cengine::FFSAddSourceFlags::Unknown09)};
            LOG_PARTIAL(" ({})\n", loaded ? "OK" : "error");
        });
    }
}

bool ce_fs_add_source_detour(const char* path, cengine::FFSAddSourceFlags::ENUM flags) {
    return LOG_TX([&] {
        LOG_PARTIAL("Adding source: \"{}\" {}", path, static_cast<std::underlying_type_t<decltype(flags)>>(flags));
        const auto result{g_cengine_original->fs.add_source(path, flags)};
        LOG_PARTIAL(" (returned {})\n", result);
        return result;
    });
}

void ce_engine_InitializeGameScript_detour(void* p1, void* p2) {
    load_paks(g_config);
    g_cengine_original->engine.InitializeGameScript(p1, p2);
}

cengine::Functions<NotNull> load_cengine_functions(const HostAppInfo& info) {
    using namespace cengine;
    return {
        .fs = load_fs_functions(reinterpret_cast<HMODULE>(info.filesystem_dll)),
        .engine = load_engine_functions(reinterpret_cast<HMODULE>(info.engine_dll)),
    };
}

template<typename T>
void log_cengine_functions(const T& funcs) {
    LOG_TX([&] {
        LOG("CEngine functions:");
        LOG("  fs:");
        LOG("    add_source: {:#x}", reinterpret_cast<uintptr_t>(funcs.fs.add_source.get()));
        LOG("  engine:");
        LOG("    InitializeGameScript: {:#x}", reinterpret_cast<uintptr_t>(funcs.engine.InitializeGameScript.get()));
    });
}

void add_hooks() {
    minhook::initialize();
    minhook::create_hook("fs.add_source", g_cengine->fs.add_source.get(), ce_fs_add_source_detour,
                         g_cengine_original->fs.add_source);
    minhook::create_hook("engine.InitializeGameScript", g_cengine->engine.InitializeGameScript.get(),
                         ce_engine_InitializeGameScript_detour, g_cengine_original->engine.InitializeGameScript);
    minhook::queue_enable_hook("fs.add_source", g_cengine->fs.add_source.get());
    minhook::queue_enable_hook("engine.InitializeGameScript", g_cengine->engine.InitializeGameScript.get());
    minhook::apply_queued();
}

void remove_hooks() {
    minhook::queue_disable_hook("engine.InitializeGameScript", g_cengine->engine.InitializeGameScript.get());
    minhook::queue_disable_hook("fs.add_source", g_cengine->fs.add_source.get());
    minhook::apply_queued();
    minhook::remove_hook("engine.InitializeGameScript", g_cengine->engine.InitializeGameScript.get());
    minhook::remove_hook("fs.add_source", g_cengine->fs.add_source.get());
    minhook::uninitialize();
}

bool* find_dev_menu_enable() {
    const auto code_start{get_base_of_code(g_host_info.game_dll)};
    const auto code_end{code_start + get_size_of_code(g_host_info.game_dll)};
    const std::span code_range{reinterpret_cast<const char*>(code_start), reinterpret_cast<const char*>(code_end)};

    LOG("Searching for developer menu offset from {:#x} to {:#x}...", code_start, code_end);

    const auto match_offset{find_pattern("740CC605??????0001E9BC0500008BDE", code_range)};
    if (!match_offset) {
        throw Error{"Could not find developer menu offset"};
    }

    const uintptr_t rip{code_start + *match_offset + 2 + 7};
    const uint32_t enable_menu_rel_address{*reinterpret_cast<std::uint32_t*>(code_start + *match_offset + 4)};
    const uintptr_t enable_menu_abs_address{rip + enable_menu_rel_address};
    auto* enable_menu{reinterpret_cast<bool*>(enable_menu_abs_address)};

    return enable_menu;
}

void set_dev_menu_enabled(bool enable) {
    LOG("Setting dev menu enabled to {}.", enable);
    *g_dev_menu_enabled = enable;
}

void create_mod() {
    g_config = load_config();
    Logger::init(g_config.general.log_file, g_config.general.enable_logging);
    log_version();
    validate_config(g_config);
    log_config(g_config);

    if (!g_config.general.enable_mod) {
        return;
    }

    g_host_info = load_host_app_info();
    log_host_info(g_host_info);
    g_cengine = load_cengine_functions(g_host_info);
    log_cengine_functions(*g_cengine);
    add_hooks();

    g_dev_menu_enabled = find_dev_menu_enable();
    if (g_config.features.developer_menu) {
        set_dev_menu_enabled(true);
    }
}

void destroy_mod() {
    if (g_config.features.developer_menu) {
        set_dev_menu_enabled(false);
    }

    remove_hooks();
}

void on_init_error() noexcept {
    static const auto title{std::string{msgbox_title_prefix} + "Error"};
    msgbox_error("Initialization failed; please see the log file for details.", title);
    std::exit(1);
}
