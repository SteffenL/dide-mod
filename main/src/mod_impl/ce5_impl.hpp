#include "../cengine/ce5.hpp"
#include "../config.hpp"
#include "../dll_notify.hpp"

#include <type_traits>

class Ce5ModImpl {
public:
    Ce5ModImpl(config::Config config);
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
    void finish_setup();
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

    static __fastcall void ce_engine_IGame_MountDlc_detour(ce5::engine::IGame* self, void* dummy, const char* p1,
                                                           const char* p2);

#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif

    static Ce5ModImpl* sm_self;
    bool m_moved{};
    config::Config m_config;
    DllNotifyReg m_ntdll_notify;
    ce5::Libraries m_libs;
    ce5::engine::Functions<std::type_identity_t> m_engine_original;
    ce5::fs::Functions<std::type_identity_t> m_fs_original;
    bool* m_dev_menu_enabled{};
    bool m_hooked{};
    bool m_libs_loaded{};
    bool m_setup_done{};
};
