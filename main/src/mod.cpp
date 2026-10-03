#include "mod.hpp"
#include "config.hpp"
#include "host.hpp"
#include "log.hpp"
#include "misc.hpp"
#include "mod_impl/ce5_impl.hpp"
#include "mod_impl/ce6_impl.hpp"
#include "platform.hpp"
#include "unicode.hpp"
#include "version.hpp"

#include <cstdlib>
#include <format>
#include <memory>
#include <string>
#include <string_view>

void log_version() { LOG("Core version: {}", project_get_version()); }

void validate_config(const config::Config& cfg) {
    if (cfg.general.enable_logging) {
        if (cfg.general.log_file.empty()) {
            throw Error{"Log file path is not configured!"};
        }
    }
}

namespace {
constexpr std::string_view msgbox_title_prefix{"[DIDE mod] "};
config::Config g_config;
HostAppInfo g_host_info;
std::unique_ptr<ModBase> g_mod;
} // namespace

config::Config load_config() {
    const auto config_file_path{exe_dir() / "dide_mod.ini"};
    try {
        return config::load_file(config_file_path);
    } catch (const std::exception& ex) {
        throw Error{std::format("Unable to load config file at {}: {}", narrow_string(config_file_path.wstring()),
                                ex.what())};
    }
}

void log_config(const config::Config& cfg) {
    LOG_TX([&] {
        LOG("Configuration:");
        LOG("  General:");
        LOG("    EnableMod = {}", cfg.general.enable_mod);
        LOG("    EnableLogging = {}", cfg.general.enable_logging);
        LOG("    LogFile = {}", narrow_string(cfg.general.log_file.wstring()));
        LOG("  Features:");
        LOG("    DeveloperMenu = {}", cfg.features.developer_menu);
        LOG("    CustomPak = {}", cfg.features.custom_pak);
        LOG("  CustomPak:");
        for (const auto& pak : cfg.load_custom_paks) {
            LOG("    {}", narrow_string(pak.wstring()));
        }
    });
}

void log_host_info(const HostAppInfo& info) {
    LOG_TX([&] {
        LOG("Host app ID: {}", info.id);
        LOG("Host app version: {}.{}.{}", info.version.major, info.version.minor, info.version.patch);
    });
}

void init(void* instance) {
    g_config = load_config();
    Logger::init(g_config.general.log_file, g_config.general.enable_logging);
    log_version();
    validate_config(g_config);
    log_config(g_config);
    DynLib::pin_by_handle(instance);
}

void create_mod() {
    if (!g_config.general.enable_mod) {
        return;
    }

    g_host_info = load_host_app_info();
    log_host_info(g_host_info);

    if (g_host_info.id == ce6::mod::dide_id || g_host_info.id == ce6::mod::dirde_id ||
        g_host_info.id == ce6::mod::dl_id) {
        g_mod = std::make_unique<ce6::mod::Ce6ModImpl>(g_host_info, g_config);
    } else if (g_host_info.id == ce5::mod::di_id || g_host_info.id == ce5::mod::dir_id) {
        g_mod = std::make_unique<ce5::mod::Ce5ModImpl>(g_host_info, g_config);
    } else {
        throw Error{std::format("Unknown host ID: {}", g_host_info.id)};
    }

    g_mod->run();
}

void on_init_error() noexcept {
    static const auto title{std::string{msgbox_title_prefix} + "Error"};
    msgbox_error("Initialization failed; please see the log file for details.", title);
    std::exit(1);
}
