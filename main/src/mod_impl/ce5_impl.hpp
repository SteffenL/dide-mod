#include "../cengine/ce5.hpp"
#include "../config.hpp"

#include <type_traits>

class Ce5ModImpl {
public:
    Ce5ModImpl(config::Config config);
    ~Ce5ModImpl();
    Ce5ModImpl(const Ce5ModImpl&) noexcept = delete;
    Ce5ModImpl& operator=(const Ce5ModImpl&) noexcept = delete;
    Ce5ModImpl(Ce5ModImpl&&) noexcept = default;
    Ce5ModImpl& operator=(Ce5ModImpl&&) noexcept = default;

private:
    void hook();
    void unhook();
    void load_paks(const config::Config& cfg);
    void set_dev_menu_enabled(bool enable);
    bool* find_dev_menu_enable();
    //static bool ce_fs_add_source_detour(const char* path, ce5::fs::FFSAddSourceFlags::ENUM flags);
    static void ce_engine_InitializeGameScriptDLL_detour();

    static Ce5ModImpl* sm_self;
    config::Config m_config;
    ce5::Libraries m_libs;
    ce5::engine::Functions<std::type_identity_t> m_engine_original;
    //ce5::fs::Functions<std::type_identity_t> m_fs_original;
    bool* m_dev_menu_enabled{};
};
