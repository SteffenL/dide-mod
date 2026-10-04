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
#include "../../string.hpp"
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
constexpr std::string_view stage_after_main = "after_main";
constexpr std::string_view stage_after_dlc = "after_dlc";
constexpr std::string_view stage_default = stage_after_main;

HostAppInfo g_host_info;
config::Config g_config;
DllNotifyReg g_ntdll_notify;
ce5::Libraries g_libs;
ce5::engine::Functions<std::type_identity_t> g_engine_original;
ce5::fs::Functions<std::type_identity_t> g_fs_original;
bool* g_dev_menu_ptr{};
std::once_flag g_find_dev_menu_once_flag;
bool g_libs_loaded{};

std::string_view get_game_dll_name(std::string_view game_id);
void check_libs();
void on_engine_lib_loaded(DynLib lib);
void on_filesystem_lib_loaded(DynLib lib);
void on_game_lib_loaded(DynLib lib);
void on_all_libs_loaded();
void load_paks(const config::Config& cfg, std::string_view stage);
void set_dev_menu_enabled(bool enable);
bool* find_dev_menu_enable();
bool ce_fs_add_source_detour(const char* path, ce5::fs::FFSAddSourceFlags::ENUM flags);
void ce_engine_InitializeGameScript_detour(void* p1, void* p2);

static void __fastcall ce_engine_IGame_MountDlc_detour(ce5::engine::IGame* self, void* dummy, const char* p1,
                                                       const char* p2);

std::string_view get_game_dll_name(std::string_view game_id) {
    return game_id == di_id ? di_game_dll_name : dir_game_dll_name;
}

void check_libs() {
    g_ntdll_notify.subscribe([](void* handle, std::filesystem::path dll_path) {
        invoke_and_log_exception([&] {
            const std::filesystem::path name{dll_path.filename()};
            const auto name_str{narrow_string(name.wstring())};
            if (name_str == engine_dll_name) {
                on_engine_lib_loaded(DynLib::attach_by_handle(handle));
            } else if (name_str == filesystem_dll_name) {
                on_filesystem_lib_loaded(DynLib::attach_by_handle(handle));
            } else if (name_str == get_game_dll_name(g_host_info.id)) {
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
    if (auto lib{DynLib::try_attach_by_name(get_game_dll_name(g_host_info.id))}) {
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
    minhook::create_hook("engine.IGame::MountDlc", g_libs.engine.fn->IGame_MountDlc.get(),
                         ce_engine_IGame_MountDlc_detour, g_engine_original.IGame_MountDlc);
    minhook::queue_enable_hook("engine.InitializeGameScript", g_libs.engine.fn->InitializeGameScript.get());
    minhook::queue_enable_hook("engine.IGame::MountDlc", g_libs.engine.fn->IGame_MountDlc.get());
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
    using ce5::fs::FFSAddSourceFlags;
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
    const auto& search_info{search_infos.at(g_host_info.id)};

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

bool ce_fs_add_source_detour(const char* path, ce5::fs::FFSAddSourceFlags::ENUM flags) {
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

void __fastcall ce_engine_IGame_MountDlc_detour(ce5::engine::IGame* self, void* /*dummy*/, const char* p1,
                                                const char* p2) {
    g_engine_original.IGame_MountDlc(self, p1, p2);
    load_paks(g_config, stage_after_dlc);
}

} // namespace

void ce5_mod_run(HostAppInfo host_info, config::Config config) {
    minhook::initialize();
    g_host_info = std::move(host_info);
    g_config = std::move(config);
    check_libs();
}

} // namespace ce5::mod
