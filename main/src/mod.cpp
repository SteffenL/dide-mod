#include "mod.hpp"
#include "config.hpp"
#include "dynlib.hpp"
#include "host.hpp"
#include "log.hpp"
#include "misc.hpp"
#include "mod_impl/factory.hpp"
#include "platform.hpp"
#include "startup_hook.hpp"
#include "string.hpp"
#include "version.hpp"

#include <cstdlib>
#include <format>
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
            LOG("    - Path: {}", narrow_string(pak.path.wstring()));
            if (!pak.stage.empty()) {
                LOG("      Stage: {}", pak.stage);
            }
        }
    });
}

void log_host_info(const HostAppInfo& info) {
    LOG_TX([&] {
        LOG("Host app ID: {}", info.id);
        LOG("Host app version: {}.{}.{}", info.version.major, info.version.minor, info.version.patch);
    });
}

void log_entry_point(std::string_view name) { LOG("Startup triggered by {}", name); }

void log_runtime_environment() {
    const auto wine_version{get_wine_version_str()};
    LOG("Runtime environment: {}", wine_version ? std::format("Wine {}", *wine_version) : "Windows");
}

[[noreturn]] void on_init_error() {
    // Avoid any potential loader lock trouble before calling any functions (message box) living in user32.dll
    if (!is_loader_lock_held_by_current_thread()) {
        try {
            static const auto title{std::string{msgbox_title_prefix} + "Error"};
            std::string message{"Initialization failed"};
            if (Logger::is_initialized()) {
                message += "; please see the log file for details";
            }
            message += ".";
            msgbox_error(message, title);
        } catch (...) {
            // No point in handling this
        }
    }
    kill_current_process(1);
}

void continue_mod_main(std::string_view entry_point_name, config::Config config) {
    log_entry_point(entry_point_name);
    log_runtime_environment();
    log_version();
    log_config(config);
    validate_config(config);
    auto host_info{load_host_app_info()};
    log_host_info(host_info);
    if (config.general.enable_mod) {
        run_mod_for_host(std::move(host_info), std::move(config));
    }
}

bool mod_main(void* instance) {
    try {
        auto config{load_config()};
        Logger::initialize(config.general.log_file, config.general.enable_logging);
        create_startup_hook([=, config = std::move(config)](std::string_view entry_point_name) {
            try {
                if (invoke_and_log_exception([=, config = std::move(config)]() mutable {
                        continue_mod_main(entry_point_name, std::move(config));
                    })) {
                    on_init_error();
                }
            } catch (...) {
                on_init_error();
            }
        });
        DynLib::pin_by_handle(instance);
        return true;
    } catch (...) {
        // Would be nice to report errors here but we shouldn't do it directly in DllMain
        return false;
    }
}
