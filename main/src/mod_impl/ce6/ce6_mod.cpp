#include "ce6_mod.hpp"
#include "../../config.hpp"
#include "../../dll_notify.hpp"
#include "../../dynlib.hpp"
#include "../../host.hpp"
#include "../../log.hpp"
#include "../../minhook.hpp"
#include "../../misc.hpp"
#include "../../pattern.hpp"
#include "../../platform.hpp"
#include "../../string.hpp"
#include "ce6.hpp"

#include <cstdint>
#include <mutex>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>

namespace ce6::mod {
namespace {
constexpr std::string_view stage_after_main = "after_main";
constexpr std::string_view stage_default = stage_after_main;

HostAppInfo g_host_info;
config::Config g_config;
DllNotifyReg g_ntdll_notify;
ce6::Libraries g_libs;
ce6::engine::Functions<std::type_identity_t> g_engine_original;
ce6::fs::Functions<std::type_identity_t> g_fs_original;
bool* g_dev_menu_ptr{};
std::once_flag g_find_dev_menu_once_flag;
bool g_libs_loaded{};

void check_libs();
void on_engine_lib_loaded(DynLib lib);
void on_filesystem_lib_loaded(DynLib lib);
void on_game_lib_loaded(DynLib lib);
void on_all_libs_loaded();
void load_paks(const config::Config& cfg, std::string_view stage);
void set_dev_menu_enabled(bool enable);
bool* find_dev_menu_enable();
bool ce_fs_add_source_detour(const char* path, ce6::fs::FFSAddSourceFlags::ENUM flags);
void ce_engine_InitializeGameScript_detour(void* p1, void* p2);

void check_libs() {
    g_ntdll_notify.subscribe([](void* handle, std::filesystem::path dll_path) {
        invoke_and_log_exception([&] {
            const std::filesystem::path name{dll_path.filename()};
            const auto name_str{narrow_string(name.wstring())};
            if (name_str == engine_dll_name) {
                on_engine_lib_loaded(DynLib::attach_by_handle(handle));
            } else if (name_str == filesystem_dll_name) {
                on_filesystem_lib_loaded(DynLib::attach_by_handle(handle));
            } else if (name_str == game_dll_name) {
                on_game_lib_loaded(DynLib::attach_by_handle(handle));
            }
        });
    });
    if (auto lib{DynLib::try_attach_by_name(engine_dll_name)}) {
        on_engine_lib_loaded(std::move(lib).value());
    }
    if (auto lib{DynLib::try_attach_by_name(filesystem_dll_name)}) {
        on_filesystem_lib_loaded(std::move(lib).value());
    }
    if (auto lib{DynLib::try_attach_by_name(game_dll_name)}) {
        on_game_lib_loaded(std::move(lib).value());
    }
}

void on_engine_lib_loaded(DynLib lib) {
    if (g_libs.engine.lib.has_value()) {
        return;
    }

    lib.pin();
    g_libs.engine.lib.emplace(std::move(lib));
    g_libs.engine.fn.emplace(g_libs.engine.lib.value());
    LOG("Engine DLL: {:#x}", g_libs.engine.lib->address());

    minhook::create_hook("engine.InitializeGameScript", g_libs.engine.fn->InitializeGameScript.get(),
                         ce_engine_InitializeGameScript_detour, g_engine_original.InitializeGameScript);
    minhook::queue_enable_hook("engine.InitializeGameScript", g_libs.engine.fn->InitializeGameScript.get());
    minhook::apply_queued();

    if (g_libs.all_ok()) {
        on_all_libs_loaded();
    }
}

void on_filesystem_lib_loaded(DynLib lib) {
    if (g_libs.filesystem.lib.has_value()) {
        return;
    }

    lib.pin();
    g_libs.filesystem.lib.emplace(std::move(lib));
    g_libs.filesystem.fn.emplace(g_libs.filesystem.lib.value());
    LOG("Filesystem DLL: {:#x}", g_libs.filesystem.lib->address());

    minhook::create_hook("fs.add_source", g_libs.filesystem.fn->add_source.get(), ce_fs_add_source_detour,
                         g_fs_original.add_source);
    minhook::queue_enable_hook("fs.add_source", g_libs.filesystem.fn->add_source.get());
    minhook::apply_queued();

    if (g_libs.all_ok()) {
        on_all_libs_loaded();
    }
}

void on_game_lib_loaded(DynLib lib) {
    if (g_libs.game.lib.has_value()) {
        return;
    }

    lib.pin();
    g_libs.game.lib.emplace(std::move(lib));
    LOG("Game DLL: {:#x}", g_libs.game.lib->address());

    if (g_libs.all_ok()) {
        on_all_libs_loaded();
    }
}

void on_all_libs_loaded() {
    if (g_libs_loaded) {
        return;
    }
    g_libs_loaded = true;
    if (g_config.features.developer_menu) {
        set_dev_menu_enabled(true);
    }
}

void load_paks(const config::Config& cfg, std::string_view stage) {
    using ce6::fs::FFSAddSourceFlags;
    for (const auto& entry : cfg.load_custom_paks) {
        std::string_view entry_stage{entry.stage.empty() ? stage_default : entry.stage};
        if (entry_stage != stage) {
            continue;
        }
        const auto pak_path_utf8{entry.path.u8string()};
        const auto* pak_path_c{reinterpret_cast<const char*>(pak_path_utf8.c_str())};
        LOG_TX([&] {
            LOG_PARTIAL("Adding custom source: \"{}\"", pak_path_c);
            const auto flags{static_cast<FFSAddSourceFlags::ENUM>(FFSAddSourceFlags::SUBDIRS |
                                                                  FFSAddSourceFlags::BROWSABLE)};
            const auto loaded{g_fs_original.add_source(pak_path_c, flags)};
            LOG_PARTIAL(" ({})\n", loaded ? "OK" : "error");
        });
    }
}

void set_dev_menu_enabled(bool enable) {
    std::call_once(g_find_dev_menu_once_flag, [&] {
        if (auto found{find_dev_menu_enable()}) {
            g_dev_menu_ptr = found;
        }
    });
    if (!g_dev_menu_ptr) {
        return;
    }
    if (*g_dev_menu_ptr == enable) {
        return;
    }
    LOG("Setting dev menu enabled to {}.", enable);
    *g_dev_menu_ptr = enable;
}

bool* find_dev_menu_enable() {
    const auto code_start{get_base_of_code(g_libs.game.lib->address())};
    const auto code_end{code_start + get_size_of_code(g_libs.game.lib->address())};
    const std::span code_range{reinterpret_cast<const char*>(code_start), reinterpret_cast<const char*>(code_end)};

    LOG("Searching for developer menu offset from {:#x} to {:#x}...", code_start, code_end);

    // Code for DIDE - DIRDE and DL have minor variations
    /*
    0:   74 0c                  je     0xe
    2:   c6 05 74 ea 86 00 01   movb   $0x1,0x86ea74(%rip)
    9:   e9 bc 05 00 00         jmp    0x5ca
    e:   8b de                  mov    %esi,%ebx
    */
    const auto match_offset{find_pattern("740CC605??????0001E9BC0500008BDE", code_range)};
    if (!match_offset) {
        LOG("Could not find developer menu offset");
        return nullptr;
    }

    const uintptr_t rip{code_start + *match_offset + 0x9};
    const uint32_t enable_menu_rel_address{*reinterpret_cast<uint32_t*>(code_start + *match_offset + 0x2 + 2)};
    const uintptr_t enable_menu_abs_address{rip + enable_menu_rel_address};
    auto* enable_menu{reinterpret_cast<bool*>(enable_menu_abs_address)};

    LOG("Developer menu pattern: {:#x}", match_offset.value());
    LOG("Developer menu variable: {:#x}", enable_menu_abs_address);

    return enable_menu;
}

bool ce_fs_add_source_detour(const char* path, ce6::fs::FFSAddSourceFlags::ENUM flags) {
    return LOG_TX([&] {
        LOG_PARTIAL("Adding source: \"{}\" {}", path, static_cast<std::underlying_type_t<decltype(flags)>>(flags));
        const auto result{g_fs_original.add_source(path, flags)};
        LOG_PARTIAL(" (returned {})\n", result);
        return result;
    });
}

void ce_engine_InitializeGameScript_detour(void* p1, void* p2) {
    g_engine_original.InitializeGameScript(p1, p2);
    load_paks(g_config, stage_after_main);
}

} // namespace

void ce6_mod_run(HostAppInfo host_info, config::Config config) {
    minhook::initialize();
    g_host_info = std::move(host_info);
    g_config = std::move(config);
    check_libs();
}

} // namespace ce6::mod
