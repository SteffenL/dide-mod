#include "ce6_impl.hpp"
#include "../cengine/ce6.hpp"
#include "../config.hpp"
#include "../dynlib.hpp"
#include "../log.hpp"
#include "../minhook.hpp"
#include "../misc.hpp"
#include "../pattern.hpp"
#include "../platform.hpp"
#include "../unicode.hpp"

#include <cstdint>
#include <span>
#include <type_traits>
#include <utility>

namespace ce6::mod {

Ce6ModImpl::Ce6ModImpl(HostAppInfo host_info, config::Config config)
        : m_host_info{std::move(host_info)}, m_config{std::move(config)} {
    sm_self = this;
    check_libs();
}

Ce6ModImpl::~Ce6ModImpl() {
    set_dev_menu_enabled(false);
    unhook();
}

void Ce6ModImpl::check_libs() {
    m_ntdll_notify.subscribe([this](void* handle, std::filesystem::path dll_path) {
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

void Ce6ModImpl::on_engine_lib_loaded(DynLib lib) {
    m_libs.engine.lib.emplace(std::move(lib));
    m_libs.engine.fn.emplace(m_libs.engine.lib.value());
    if (m_libs.all_ok()) {
        on_all_libs_loaded();
    }
}

void Ce6ModImpl::on_filesystem_lib_loaded(DynLib lib) {
    m_libs.filesystem.lib.emplace(std::move(lib));
    m_libs.filesystem.fn.emplace(m_libs.filesystem.lib.value());
    if (m_libs.all_ok()) {
        on_all_libs_loaded();
    }
}

void Ce6ModImpl::on_game_lib_loaded(DynLib lib) {
    m_libs.game.lib.emplace(std::move(lib));
    if (m_libs.all_ok()) {
        on_all_libs_loaded();
    }
}

void Ce6ModImpl::on_all_libs_loaded() {
    if (m_libs_loaded) {
        return;
    }
    m_libs_loaded = true;
    log_libs();
    hook();
    if (m_config.features.developer_menu) {
        sm_self->set_dev_menu_enabled(true);
    }
}

void Ce6ModImpl::hook() {
    minhook::initialize();
    minhook::create_hook("fs.add_source", m_libs.filesystem.fn->add_source.get(), ce_fs_add_source_detour,
                         m_fs_original.add_source);
    minhook::create_hook("engine.InitializeGameScript", m_libs.engine.fn->InitializeGameScript.get(),
                         ce_engine_InitializeGameScript_detour, m_engine_original.InitializeGameScript);
    minhook::queue_enable_hook("fs.add_source", m_libs.filesystem.fn->add_source.get());
    minhook::queue_enable_hook("engine.InitializeGameScript", m_libs.engine.fn->InitializeGameScript.get());
    minhook::apply_queued();
    m_hooked = true;
}

void Ce6ModImpl::unhook() {
    minhook::queue_disable_hook("engine.InitializeGameScript", m_libs.engine.fn->InitializeGameScript.get());
    minhook::queue_disable_hook("fs.add_source", m_libs.filesystem.fn->add_source.get());
    minhook::apply_queued();
    minhook::remove_hook("engine.InitializeGameScript", m_libs.engine.fn->InitializeGameScript.get());
    minhook::remove_hook("fs.add_source", m_libs.filesystem.fn->add_source.get());
    minhook::uninitialize();
}

void Ce6ModImpl::load_paks(const config::Config& cfg) {
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

void Ce6ModImpl::set_dev_menu_enabled(bool enable) {
    std::call_once(m_find_dev_menu_once_flag, [&] {
        if (auto found{sm_self->find_dev_menu_enable()}) {
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

bool* Ce6ModImpl::find_dev_menu_enable() {
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

void Ce6ModImpl::log_libs() const {
    LOG_TX([this] {
        LOG("Game DLL: {:#x}", m_libs.game.lib->address());
        LOG("Engine DLL: {:#x}", m_libs.engine.lib->address());
        LOG("Filesystem DLL: {:#x}", m_libs.filesystem.lib->address());
    });
}

bool Ce6ModImpl::ce_fs_add_source_detour(const char* path, ce6::fs::FFSAddSourceFlags::ENUM flags) {
    return LOG_TX([&] {
        LOG_PARTIAL("Adding source: \"{}\" {}", path, static_cast<std::underlying_type_t<decltype(flags)>>(flags));
        const auto result{sm_self->m_fs_original.add_source(path, flags)};
        LOG_PARTIAL(" (returned {})\n", result);
        return result;
    });
}

void Ce6ModImpl::ce_engine_InitializeGameScript_detour(void* p1, void* p2) {
    sm_self->m_engine_original.InitializeGameScript(p1, p2);
    sm_self->load_paks(sm_self->m_config);
}

Ce6ModImpl* Ce6ModImpl::sm_self{};

} // namespace ce6::mod
