#include "../cengine/ce5.hpp"
#include "../config.hpp"
#include "../dll_notify.hpp"
#include "../host.hpp"

#include <mutex>
#include <type_traits>

namespace ce5::mod {

constexpr std::string_view di_id{"DeadIsland"};
constexpr std::string_view dir_id{"DeadIsland Riptide"};
constexpr std::string_view engine_dll_name{"engine_x86_rwdi.dll"};
constexpr std::string_view filesystem_dll_name{"filesystem_x86_rwdi.dll"};
constexpr std::string_view di_game_dll_name{"game_x86_rwdi.dll"};
constexpr std::string_view dir_game_dll_name{"gamedll_x86_rwdi.dll"};

class Ce5ModImpl {
public:
    Ce5ModImpl(HostAppInfo host_info, config::Config config);
    ~Ce5ModImpl();
    Ce5ModImpl(const Ce5ModImpl&) noexcept = delete;
    Ce5ModImpl& operator=(const Ce5ModImpl&) noexcept = delete;
    Ce5ModImpl(Ce5ModImpl&&) noexcept = delete;
    Ce5ModImpl& operator=(Ce5ModImpl&&) noexcept = delete;

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
    static bool ce_fs_add_source_detour(const char* path, ce5::fs::FFSAddSourceFlags::ENUM flags);
    static void ce_engine_InitializeGameScript_detour(void* p1, void* p2);

#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wattributes"
#endif

    static void __fastcall ce_engine_IGame_MountDlc_detour(ce5::engine::IGame* self, void* dummy, const char* p1,
                                                           const char* p2);

#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif

    static Ce5ModImpl* sm_self;
    HostAppInfo m_host_info;
    config::Config m_config;
    DllNotifyReg m_ntdll_notify;
    ce5::Libraries m_libs;
    ce5::engine::Functions<std::type_identity_t> m_engine_original;
    ce5::fs::Functions<std::type_identity_t> m_fs_original;
    bool* m_dev_menu_ptr{};
    std::once_flag m_find_dev_menu_once_flag;
    bool m_hooked{};
    bool m_libs_loaded{};
};

} // namespace ce5::mod
