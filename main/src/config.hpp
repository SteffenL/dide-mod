#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace config {

struct Config {
    struct {
        bool enable_mod{};
        bool enable_logging{};
        std::filesystem::path log_file;
    } general;

    struct {
        bool developer_menu{};
        bool custom_pak{};
    } features;

    struct CustomPakEntry {
        std::filesystem::path path;
        bool enabled{};
        std::string stage;
    };

    std::vector<CustomPakEntry> load_custom_paks;
};

Config load_file(const std::filesystem::path& file_path);

} // namespace config
