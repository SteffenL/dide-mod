#include "ce5_mod.hpp"
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
#include "ce5.hpp"

#include <cstdint>
#include <mutex>
#include <span>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace ce5::mod {
namespace {
HostAppInfo m_host_info;
config::Config m_config;
DllNotifyReg m_ntdll_notify;
ce5::Libraries m_libs;
ce5::engine::Functions<std::type_identity_t> m_engine_original;
ce5::fs::Functions<std::type_identity_t> m_fs_original;
bool* m_dev_menu_ptr{};
std::once_flag m_find_dev_menu_once_flag;
bool m_libs_loaded{};

std::string_view get_game_dll_name(std::string_view game_id);
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
bool ce_fs_add_source_detour(const char* path, ce5::fs::FFSAddSourceFlags::ENUM flags);
void ce_engine_InitializeGameScript_detour(void* p1, void* p2);

#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wattributes"
#endif

static void __fastcall ce_engine_IGame_MountDlc_detour(ce5::engine::IGame* self, void* dummy, const char* p1,
                                                       const char* p2);

#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif

std::string_view get_game_dll_name(std::string_view game_id) {
    return game_id == di_id ? di_game_dll_name : dir_game_dll_name;
}

void check_libs() {
    m_ntdll_notify.subscribe([](void* handle, std::filesystem::path dll_path) {
        invoke_and_log_exception([&] {
            const std::filesystem::path name{dll_path.filename()};
            const auto name_str{narrow_string(name.wstring())};
            if (name_str == engine_dll_name) {
                on_engine_lib_loaded(DynLib::attach_by_handle(handle));
            } else if (name_str == filesystem_dll_name) {
                on_filesystem_lib_loaded(DynLib::attach_by_handle(handle));
            } else if (name_str == get_game_dll_name(m_host_info.id)) {
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
    if (auto lib{DynLib::try_attach_by_name(get_game_dll_name(m_host_info.id))}) {
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
    minhook::create_hook("engine.IGame::MountDlc", m_libs.engine.fn->IGame_MountDlc.get(),
                         ce_engine_IGame_MountDlc_detour, m_engine_original.IGame_MountDlc);
    minhook::queue_enable_hook("fs.add_source", m_libs.filesystem.fn->add_source.get());
    minhook::queue_enable_hook("engine.InitializeGameScript", m_libs.engine.fn->InitializeGameScript.get());
    minhook::queue_enable_hook("engine.IGame::MountDlc", m_libs.engine.fn->IGame_MountDlc.get());
    minhook::apply_queued();
}

void load_paks(const config::Config& cfg) {
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
        /*
        0:   74 12                  je     0x14
        2:   5f                     pop    %edi
        3:   5e                     pop    %esi
        4:   c6 05 47 21 c7 10 01   movb   $0x1,0x10c72147
        b:   b0 01                  mov    $0x1,%al
        */
        {"DeadIsland Riptide", Pattern{"74125F5EC60547", 0x4 + 2}},
    };
    const auto& search_info{search_infos.at(m_host_info.id)};

    const auto match_offset{find_pattern(search_info.pattern, code_range)};
    if (!match_offset) {
        LOG("Could not find developer menu offset");
        return nullptr;
    }

    const uint32_t enable_menu_address{*reinterpret_cast<const uint32_t*>(code_start + *match_offset +
                                                                          search_info.offset)};
    auto* enable_menu{reinterpret_cast<bool*>(enable_menu_address)};

    LOG("Developer menu pattern: {:#x}", match_offset.value());
    LOG("Developer menu variable: {:#x}", enable_menu_address);

    return enable_menu;
}

void log_libs() {
    LOG_TX([] {
        LOG("Game DLL: {:#x}", m_libs.game.lib->address());
        LOG("Engine DLL: {:#x}", m_libs.engine.lib->address());
        LOG("Filesystem DLL: {:#x}", m_libs.filesystem.lib->address());
    });
}

bool ce_fs_add_source_detour(const char* path, ce5::fs::FFSAddSourceFlags::ENUM flags) {
    return LOG_TX([&] {
        LOG_PARTIAL("Adding source: \"{}\" {}", path, static_cast<std::underlying_type_t<decltype(flags)>>(flags));
        const auto result{m_fs_original.add_source(path, flags)};
        LOG_PARTIAL(" (returned {})\n", result);
        return result;
    });
}

void ce_engine_InitializeGameScript_detour(void* p1, void* p2) { m_engine_original.InitializeGameScript(p1, p2); }

#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wattributes"
#endif

void __fastcall ce_engine_IGame_MountDlc_detour(ce5::engine::IGame* self, void* /*dummy*/, const char* p1,
                                                const char* p2) {
    m_engine_original.IGame_MountDlc(self, p1, p2);
    load_paks(m_config);
}

#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif
} // namespace

void ce5_mod_run(HostAppInfo host_info, config::Config config) {
    m_host_info = std::move(host_info);
    m_config = std::move(config);
    check_libs();
}

} // namespace ce5::mod
