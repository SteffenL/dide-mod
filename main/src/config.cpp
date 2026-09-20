#include "config.hpp"
#include "misc.hpp"

#include <array>
#include <cwchar>
#include <filesystem>
#include <functional>
#include <string>

#include <windows.h>

namespace config {
namespace {

unsigned int ini_read_uint(const std::filesystem::path& file_path, const wchar_t* section_name, const wchar_t* key_name,
                           unsigned int default_value) {
    std::array<wchar_t, 11> value_str{};
    ::GetPrivateProfileStringW(section_name, key_name, std::to_wstring(default_value).c_str(), value_str.data(),
                               static_cast<DWORD>(value_str.size()), file_path.c_str());
    return static_cast<unsigned int>(std::wcstoul(value_str.data(), nullptr, 10));
}

std::wstring ini_read_string(const std::filesystem::path& file_path, const wchar_t* section_name,
                             const wchar_t* key_name, const wchar_t* default_value) {
    // The allocated size is 1024 including an implicit null, and since GetPrivateProfileStringW also writes a null,
    // it leaves 1022 usable characters.
    std::wstring value_str(1023, '\0');
    const auto value_length{::GetPrivateProfileStringW(section_name, key_name, default_value ? default_value : NULL,
                                                       value_str.data(), static_cast<DWORD>(value_str.size()),
                                                       file_path.c_str())};
    value_str.resize(value_length);
    return value_str;
}

void ini_enum_keys(const std::filesystem::path& file_path, const wchar_t* section_name,
                   std::function<void(std::wstring)> cb) {
    std::array<wchar_t, 1024> key_names_buffer{};
    ::GetPrivateProfileStringW(section_name, NULL, NULL, key_names_buffer.data(),
                               static_cast<DWORD>(key_names_buffer.size()), file_path.c_str());
    for (const wchar_t* key_name{key_names_buffer.data()}; *key_name != L'\0'; key_name += std::wcslen(key_name) + 1) {
        // Line comments (;) are automatically ignored by the Windows API function
        cb(key_name);
    }
}

} // namespace

void load_general(Config& config, const std::filesystem::path& file_path) {
    static constexpr auto* section_name{L"General"};
    config.general.enable_mod = static_cast<bool>(ini_read_uint(file_path, section_name, L"EnableMod", 0));
    config.general.enable_logging = static_cast<bool>(ini_read_uint(file_path, section_name, L"EnableLogging", 0));
    config.general.log_file = ini_read_string(file_path, section_name, L"LogFile", L"dide_mod.log");
}

void load_features(Config& config, const std::filesystem::path& file_path) {
    static constexpr auto* section_name{L"Features"};
    config.features.developer_menu = static_cast<bool>(ini_read_uint(file_path, section_name, L"DeveloperMenu", 0));
    config.features.custom_pak = static_cast<bool>(ini_read_uint(file_path, section_name, L"CustomPak", 0));
}

void load_custom_pak(Config& config, const std::filesystem::path& file_path) {
    if (!config.features.custom_pak) {
        return;
    }

    static constexpr auto* section_name{L"CustomPak"};

    ini_enum_keys(file_path, section_name, [&](auto pak_path) {
        if (pak_path.empty()) {
            // Ignore
            return;
        }

        const auto pak_enabled{static_cast<bool>(ini_read_uint(file_path, section_name, pak_path.c_str(), 0))};
        if (!pak_enabled) {
            // Ignore
            return;
        }

        config.load_custom_paks.emplace_back(pak_path);
    });
}

Config load_file(const std::filesystem::path& file_path) {
    Config config;
    load_general(config, file_path);
    load_features(config, file_path);
    load_custom_pak(config, file_path);
    return config;
}

} // namespace config
