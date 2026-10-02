#include "../cengine/ce6.hpp"
#include "../config.hpp"

#include <type_traits>

class Ce6ModImpl {
public:
    Ce6ModImpl(config::Config config);
    ~Ce6ModImpl();
    Ce6ModImpl(const Ce6ModImpl&) noexcept = delete;
    Ce6ModImpl& operator=(const Ce6ModImpl&) noexcept = delete;
    Ce6ModImpl(Ce6ModImpl&&) noexcept = delete;
    Ce6ModImpl& operator=(Ce6ModImpl&&) noexcept = delete;

private:
    void finish_entry();
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
    config::Config m_config;
    ce6::Libraries m_libs;
    ce6::engine::Functions<std::type_identity_t> m_engine_original;
    ce6::fs::Functions<std::type_identity_t> m_fs_original;
    bool* m_dev_menu_enabled{};
};
