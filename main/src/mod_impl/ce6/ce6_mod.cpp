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
#include "../../unicode.hpp"
#include "ce6.hpp"

#include <cstdint>
#include <mutex>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>

namespace ce6::mod {
namespace {
HostAppInfo m_host_info;
config::Config m_config;
DllNotifyReg m_ntdll_notify;
ce6::Libraries m_libs;
ce6::engine::Functions<std::type_identity_t> m_engine_original;
ce6::fs::Functions<std::type_identity_t> m_fs_original;
bool* m_dev_menu_ptr{};
std::once_flag m_find_dev_menu_once_flag;
bool m_libs_loaded{};

void check_libs();
void on_engine_lib_loaded(DynLib lib);
void on_filesystem_lib_loaded(DynLib lib);
void on_game_lib_loaded(DynLib lib);
void on_all_libs_loaded();
void hook();
void load_paks(const config::Config& cfg);
void set_dev_menu_enabled(bool enable);
bool* find_dev_menu_enable();
void log_libs();
bool ce_fs_add_source_detour(const char* path, ce6::fs::FFSAddSourceFlags::ENUM flags);
void ce_engine_InitializeGameScript_detour(void* p1, void* p2);

void check_libs() {
    m_ntdll_notify.subscribe([](void* handle, std::filesystem::path dll_path) {
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
    lib.pin();
    m_libs.engine.lib.emplace(std::move(lib));
    m_libs.engine.fn.emplace(m_libs.engine.lib.value());
    if (m_libs.all_ok()) {
        on_all_libs_loaded();
    }
}

void on_filesystem_lib_loaded(DynLib lib) {
    lib.pin();
    m_libs.filesystem.lib.emplace(std::move(lib));
    m_libs.filesystem.fn.emplace(m_libs.filesystem.lib.value());
    if (m_libs.all_ok()) {
        on_all_libs_loaded();
    }
}

void on_game_lib_loaded(DynLib lib) {
    lib.pin();
    m_libs.game.lib.emplace(std::move(lib));
    if (m_libs.all_ok()) {
        on_all_libs_loaded();
    }
}

void on_all_libs_loaded() {
    if (m_libs_loaded) {
        return;
    }
    m_libs_loaded = true;
    log_libs();
    hook();
    if (m_config.features.developer_menu) {
        set_dev_menu_enabled(true);
    }
}

void hook() {
    minhook::initialize();
    minhook::create_hook("fs.add_source", m_libs.filesystem.fn->add_source.get(), ce_fs_add_source_detour,
                         m_fs_original.add_source);
    minhook::create_hook("engine.InitializeGameScript", m_libs.engine.fn->InitializeGameScript.get(),
                         ce_engine_InitializeGameScript_detour, m_engine_original.InitializeGameScript);
    minhook::queue_enable_hook("fs.add_source", m_libs.filesystem.fn->add_source.get());
    minhook::queue_enable_hook("engine.InitializeGameScript", m_libs.engine.fn->InitializeGameScript.get());
    minhook::apply_queued();
}

void load_paks(const config::Config& cfg) {
    using ce6::fs::FFSAddSourceFlags;
    for (const auto& pak_path : cfg.load_custom_paks) {
        const auto pak_path_utf8{pak_path.u8string()};
        const auto* pak_path_c{reinterpret_cast<const char*>(pak_path_utf8.c_str())};
        LOG_TX([&] {
            LOG_PARTIAL("Adding custom source: {}", pak_path_c);
            const auto flags{static_cast<FFSAddSourceFlags::ENUM>(FFSAddSourceFlags::SUBDIRS |
                                                                  FFSAddSourceFlags::BROWSABLE)};
            const auto loaded{m_fs_original.add_source(pak_path_c, flags)};
            LOG_PARTIAL(" ({})\n", loaded ? "OK" : "error");
        });
    }
}

void set_dev_menu_enabled(bool enable) {
    std::call_once(m_find_dev_menu_once_flag, [&] {
        if (auto found{find_dev_menu_enable()}) {
            m_dev_menu_ptr = found;
        }
    });
    if (!m_dev_menu_ptr) {
        return;
    }
    if (*m_dev_menu_ptr == enable) {
        return;
    }
    LOG("Setting dev menu enabled to {}.", enable);
    *m_dev_menu_ptr = enable;
}

bool* find_dev_menu_enable() {
    const auto code_start{get_base_of_code(m_libs.game.lib->address())};
    const auto code_end{code_start + get_size_of_code(m_libs.game.lib->address())};
    const std::span code_range{reinterpret_cast<const char*>(code_start), reinterpret_cast<const char*>(code_end)};

    LOG("Searching for developer menu offset from {:#x} to {:#x}...", code_start, code_end);

    // Code for DI - DIR and DL have minor variations
    /* 0:   74 0c                  je     0xe
       2:   c6 05 74 ea 86 00 01   movb   $0x1,0x86ea74(%rip)
       9:   e9 bc 05 00 00         jmp    0x5ca
       e:   8b de                  mov    %esi,%ebx */
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

void log_libs() {
    LOG_TX([] {
        LOG("Game DLL: {:#x}", m_libs.game.lib->address());
        LOG("Engine DLL: {:#x}", m_libs.engine.lib->address());
        LOG("Filesystem DLL: {:#x}", m_libs.filesystem.lib->address());
    });
}

bool ce_fs_add_source_detour(const char* path, ce6::fs::FFSAddSourceFlags::ENUM flags) {
    return LOG_TX([&] {
        LOG_PARTIAL("Adding source: \"{}\" {}", path, static_cast<std::underlying_type_t<decltype(flags)>>(flags));
        const auto result{m_fs_original.add_source(path, flags)};
        LOG_PARTIAL(" (returned {})\n", result);
        return result;
    });
}

void ce_engine_InitializeGameScript_detour(void* p1, void* p2) {
    m_engine_original.InitializeGameScript(p1, p2);
    load_paks(m_config);
}

} // namespace

void ce6_mod_run(HostAppInfo host_info, config::Config config) {
    m_host_info = std::move(host_info);
    m_config = std::move(config);
    check_libs();
}

} // namespace ce6::mod
