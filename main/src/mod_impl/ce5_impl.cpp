#include "ce5_impl.hpp"
#include "../cengine/ce5.hpp"
#include "../config.hpp"
#include "../log.hpp"
#include "../minhook.hpp"
#include "../misc.hpp"
#include "../pattern.hpp"
#include "../platform.hpp"
#include "../unicode.hpp"

#include <cstdint>
#include <span>
#include <unordered_map>
#include <utility>

Ce5ModImpl::Ce5ModImpl(HostAppInfo host_info, config::Config config)
        : m_host_info{std::move(host_info)}, m_config{std::move(config)} {
    sm_self = this;
    check_libs();
}

Ce5ModImpl::~Ce5ModImpl() {
    set_dev_menu_enabled(false);
    unhook();
}

void Ce5ModImpl::check_libs() {
    m_ntdll_notify.subscribe([this](std::filesystem::path name) {
        const auto name_str{narrow_string(name.wstring())};
        LOG("DLL: {}", name_str);
        if (name_str.ends_with("\\engine_x86_rwdi.dll")) {
            on_engine_lib_loaded();
        } else if (name_str.ends_with("\\filesystem_x86_rwdi.dll")) {
            on_filesystem_lib_loaded();
        } else if (name_str.ends_with("\\game_x86_rwdi.dll")) {
            on_game_lib_loaded();
        }
    });
    if (DynLib::is_loaded("engine_x86_rwdi.dll")) {
        on_engine_lib_loaded();
    }
    if (DynLib::is_loaded("filesystem_x86_rwdi.dll")) {
        on_filesystem_lib_loaded();
    }
    if (DynLib::is_loaded("game_x86_rwdi.dll")) {
        on_game_lib_loaded();
    }
}

void Ce5ModImpl::on_engine_lib_loaded() {
    m_libs.engine.lib.emplace("engine_x86_rwdi.dll");
    m_libs.engine.fn.emplace(m_libs.engine.lib.value());
    if (m_libs.all_ok()) {
        on_all_libs_loaded();
    }
}

void Ce5ModImpl::on_filesystem_lib_loaded() {
    m_libs.filesystem.lib.emplace("filesystem_x86_rwdi.dll");
    m_libs.filesystem.fn.emplace(m_libs.filesystem.lib.value());
    if (m_libs.all_ok()) {
        on_all_libs_loaded();
    }
}

void Ce5ModImpl::on_game_lib_loaded() {
    m_libs.game.lib.emplace("game_x86_rwdi.dll");
    if (m_libs.all_ok()) {
        on_all_libs_loaded();
    }
}

void Ce5ModImpl::on_all_libs_loaded() {
    if (m_libs_loaded) {
        return;
    }
    m_libs_loaded = true;
    log_libs();
    hook();
    if (auto found{sm_self->find_dev_menu_enable()}) {
        sm_self->m_dev_menu_enabled = found;
        sm_self->set_dev_menu_enabled(sm_self->m_config.features.developer_menu);
    }
}

void Ce5ModImpl::hook() {
    minhook::initialize();
    minhook::create_hook("fs.add_source", m_libs.filesystem.fn->add_source.get(), ce_fs_add_source_detour,
                         m_fs_original.add_source);
    minhook::create_hook("engine.InitializeGameScript", m_libs.engine.fn->InitializeGameScript.get(),
                         ce_engine_InitializeGameScript_detour, m_engine_original.InitializeGameScript);
    minhook::create_hook("engine.IGame::MountDlc", m_libs.engine.fn->IGame_MountDlc.get(),
                         ce_engine_IGame_MountDlc_detour, m_engine_original.IGame_MountDlc);
    minhook::queue_enable_hook("fs.add_source", m_libs.filesystem.fn->add_source.get());
    minhook::queue_enable_hook("engine.InitializeGameScript", m_libs.engine.fn->InitializeGameScript.get());
    minhook::queue_enable_hook("engine.IGame::MountDlc", m_libs.engine.fn->IGame_MountDlc.get());
    minhook::apply_queued();
    m_hooked = true;
}

void Ce5ModImpl::unhook() {
    if (!m_hooked) {
        return;
    }
    minhook::queue_disable_hook("engine.IGame::MountDlc", m_libs.engine.fn->IGame_MountDlc.get());
    minhook::queue_disable_hook("engine.InitializeGameScript", m_libs.engine.fn->InitializeGameScript.get());
    minhook::queue_disable_hook("fs.add_source", m_libs.filesystem.fn->add_source.get());
    minhook::apply_queued();
    minhook::remove_hook("engine.IGame::MountDlc", m_libs.engine.fn->IGame_MountDlc.get());
    minhook::remove_hook("engine.InitializeGameScript", m_libs.engine.fn->InitializeGameScript.get());
    minhook::remove_hook("fs.add_source", m_libs.filesystem.fn->add_source.get());
    minhook::uninitialize();
}

void Ce5ModImpl::load_paks(const config::Config& cfg) {
    using ce5::fs::FFSAddSourceFlags;
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

void Ce5ModImpl::set_dev_menu_enabled(bool enable) {
    if (!m_dev_menu_enabled) {
        return;
    }
    if (*m_dev_menu_enabled == enable) {
        return;
    }
    LOG("Setting dev menu enabled to {}.", enable);
    *m_dev_menu_enabled = enable;
}

template<typename T>
    requires requires(T t) {
        typename T::element_type;
        typename T::size_type;
        { *t.begin() };
        { t.size() };
        requires std::integral<typename T::element_type>;
        sizeof(typename T::element_type) == 1;
        requires !std::same_as<typename T::element_type, bool>;
    }
std::string to_hex(const T& data) {
    static constexpr std::string_view alphabet{"0123456789abcdef"};
    std::string result;
    result.reserve(data.size() * 2);
    for (auto b : data) {
        result += alphabet[static_cast<uint8_t>(b) >> 4];
        result += alphabet[static_cast<uint8_t>(b) & 15];
    }
    return result;
}

bool* Ce5ModImpl::find_dev_menu_enable() {
    const auto code_start{get_base_of_code(m_libs.game.lib->address())};
    const auto code_end{code_start + get_size_of_code(m_libs.game.lib->address())};
    const std::span code_range{
        reinterpret_cast<const uint8_t*>(code_start), reinterpret_cast<const uint8_t*>(code_end)
    };

    LOG("Searching for developer menu offset from {:#x} to {:#x}...", code_start, code_end);

    struct Pattern {
        std::string_view pattern;
        size_t offset;
    };

    static const std::unordered_map<std::string_view, Pattern> search_infos = {
        /*
        0:   74 13                  je     0x15
        2:   5f                     pop    %edi
        3:   5e                     pop    %esi
        4:   5d                     pop    %ebp
        5:   c6 05 d7 09 b4 10 01   movb   $0x1,0x10b409d7
        c:   b0 01                  mov    $0x1,%al
        */
        {"DeadIsland", Pattern{"74135F5E5DC605D7", 0x5 + 2}},
        /* 0:   74 12                  je     0x14
           2:   5f                     pop    %edi
           3:   5e                     pop    %esi
           4:   c6 05 47 21 c7 10 01   movb   $0x1,0x10c72147
           b:   b0 01                  mov    $0x1,%al */
        {"DeadIsland Riptide", Pattern{"74125F5EC60547", 0x4 + 2}},
    };
    const auto& search_info{search_infos.at(m_host_info.id)};

    const auto match_offset{find_pattern(search_info.pattern, code_range)};
    if (!match_offset) {
        LOG("Could not find developer menu offset");
        return nullptr;
    }

    const uint32_t enable_menu_address{*reinterpret_cast<const uint32_t*>(code_start + *match_offset + search_info.offset)};
    auto* enable_menu{reinterpret_cast<bool*>(enable_menu_address)};

    LOG("Developer menu pattern: {:#x}", match_offset.value());
    LOG("Developer menu variable: {:#x}", enable_menu_address);

    return enable_menu;
}

void Ce5ModImpl::log_libs() const {
    LOG_TX([this] {
        LOG("Game DLL: {:#x}", m_libs.game.lib->address());
        LOG("Engine DLL: {:#x}", m_libs.engine.lib->address());
        LOG("Filesystem DLL: {:#x}", m_libs.filesystem.lib->address());
    });
}

bool Ce5ModImpl::ce_fs_add_source_detour(const char* path, ce5::fs::FFSAddSourceFlags::ENUM flags) {
    return LOG_TX([&] {
        LOG_PARTIAL("Adding source: \"{}\" {}", path, static_cast<std::underlying_type_t<decltype(flags)>>(flags));
        const auto result{sm_self->m_fs_original.add_source(path, flags)};
        LOG_PARTIAL(" (returned {})\n", result);
        return result;
    });
}

void Ce5ModImpl::ce_engine_InitializeGameScript_detour(void* p1, void* p2) {
    sm_self->m_engine_original.InitializeGameScript(p1, p2);
}

#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wattributes"
#endif

void __fastcall Ce5ModImpl::ce_engine_IGame_MountDlc_detour(ce5::engine::IGame* self, void* /*dummy*/, const char* p1,
                                                            const char* p2) {
    sm_self->m_engine_original.IGame_MountDlc(self, p1, p2);
    sm_self->load_paks(sm_self->m_config);
}

#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif

Ce5ModImpl* Ce5ModImpl::sm_self{};
