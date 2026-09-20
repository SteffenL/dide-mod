#pragma once

#include "misc.hpp"

#include <filesystem>
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

    std::vector<std::filesystem::path> load_custom_paks;
};

Result<Config> load_file(const std::filesystem::path& file_path);

} // namespace config
