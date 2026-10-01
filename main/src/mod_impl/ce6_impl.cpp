#include "ce6_impl.hpp"
#include "../cengine/ce6.hpp"
#include "../config.hpp"
#include "../log.hpp"
#include "../minhook.hpp"
#include "../misc.hpp"
#include "../pattern.hpp"
#include "../platform.hpp"

#include <cstdint>
#include <span>
#include <type_traits>
#include <utility>

Ce6ModImpl::Ce6ModImpl(config::Config config) : m_config{std::move(config)} {
    sm_self = this;
    hook();
    if (auto found{find_dev_menu_enable()}) {
        m_dev_menu_enabled = found;
        set_dev_menu_enabled(m_config.features.developer_menu);
    }
}

Ce6ModImpl::~Ce6ModImpl() {
    set_dev_menu_enabled(false);
    unhook();
}

void Ce6ModImpl::hook() {
    minhook::initialize();
    minhook::create_hook("fs.add_source", m_libs.filesystem.fn.add_source.get(), ce_fs_add_source_detour,
                         m_fs_original.add_source);
    minhook::create_hook("engine.InitializeGameScript", m_libs.engine.fn.InitializeGameScript.get(),
                         ce_engine_InitializeGameScript_detour, m_engine_original.InitializeGameScript);
    minhook::queue_enable_hook("fs.add_source", m_libs.filesystem.fn.add_source.get());
    minhook::queue_enable_hook("engine.InitializeGameScript", m_libs.engine.fn.InitializeGameScript.get());
    minhook::apply_queued();
}

void Ce6ModImpl::unhook() {
    minhook::queue_disable_hook("engine.InitializeGameScript", m_libs.engine.fn.InitializeGameScript.get());
    minhook::queue_disable_hook("fs.add_source", m_libs.filesystem.fn.add_source.get());
    minhook::apply_queued();
    minhook::remove_hook("engine.InitializeGameScript", m_libs.engine.fn.InitializeGameScript.get());
    minhook::remove_hook("fs.add_source", m_libs.filesystem.fn.add_source.get());
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
    if (!m_dev_menu_enabled) {
        return;
    }
    if (*m_dev_menu_enabled == enable) {
        return;
    }
    LOG("Setting dev menu enabled to {}.", enable);
    *m_dev_menu_enabled = enable;
}

bool* Ce6ModImpl::find_dev_menu_enable() {
    const auto code_start{get_base_of_code(m_libs.game.lib.address())};
    const auto code_end{code_start + get_size_of_code(m_libs.game.lib.address())};
    const std::span code_range{reinterpret_cast<const char*>(code_start), reinterpret_cast<const char*>(code_end)};

    LOG("Searching for developer menu offset from {:#x} to {:#x}...", code_start, code_end);

    const auto match_offset{find_pattern("740CC605??????0001E9BC0500008BDE", code_range)};
    if (!match_offset) {
        LOG("Could not find developer menu offset");
        return nullptr;
    }

    const uintptr_t rip{code_start + *match_offset + 2 + 7};
    const uint32_t enable_menu_rel_address{*reinterpret_cast<uint32_t*>(code_start + *match_offset + 4)};
    const uintptr_t enable_menu_abs_address{rip + enable_menu_rel_address};
    auto* enable_menu{reinterpret_cast<bool*>(enable_menu_abs_address)};

    return enable_menu;
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
    sm_self->load_paks(sm_self->m_config);
    sm_self->m_engine_original.InitializeGameScript(p1, p2);
}

Ce6ModImpl* Ce6ModImpl::sm_self{};
