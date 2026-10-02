#include "../cengine/ce6.hpp"
#include "../config.hpp"
#include "../dll_notify.hpp"
#include "../host.hpp"

#include <mutex>
#include <type_traits>

namespace ce6::mod {

constexpr std::string_view dide_id{"DeadIslandDE"};
constexpr std::string_view dirde_id{"DeadIslandRiptideDE"};
constexpr std::string_view dl_id{"DyingLight"};
constexpr std::string_view engine_dll_name{"engine_x64_rwdi.dll"};
constexpr std::string_view filesystem_dll_name{"filesystem_x64_rwdi.dll"};
constexpr std::string_view game_dll_name{"gamedll_x64_rwdi.dll"};

class Ce6ModImpl {
public:
    Ce6ModImpl(HostAppInfo host_info, config::Config config);
    ~Ce6ModImpl();
    Ce6ModImpl(const Ce6ModImpl&) noexcept = delete;
    Ce6ModImpl& operator=(const Ce6ModImpl&) noexcept = delete;
    Ce6ModImpl(Ce6ModImpl&&) noexcept = delete;
    Ce6ModImpl& operator=(Ce6ModImpl&&) noexcept = delete;

private:
    void check_libs();
    void on_engine_lib_loaded();
    void on_filesystem_lib_loaded();
    void on_game_lib_loaded();
    void on_all_libs_loaded();
    void hook();
    void unhook();
    void load_paks(const config::Config& cfg);
    void set_dev_menu_enabled(bool enable);
    bool* find_dev_menu_enable();
    void log_libs() const;
    static bool ce_fs_add_source_detour(const char* path, ce6::fs::FFSAddSourceFlags::ENUM flags);
    static void ce_engine_InitializeGameScript_detour(void* p1, void* p2);

    static Ce6ModImpl* sm_self;
    bool m_moved{};
    HostAppInfo m_host_info;
    config::Config m_config;
    DllNotifyReg m_ntdll_notify;
    ce6::Libraries m_libs;
    ce6::engine::Functions<std::type_identity_t> m_engine_original;
    ce6::fs::Functions<std::type_identity_t> m_fs_original;
    bool* m_dev_menu_ptr{};
    std::once_flag m_find_dev_menu_once_flag;
    bool m_hooked{};
    bool m_libs_loaded{};
};

} // namespace ce6::mod
