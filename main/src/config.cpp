#include "config.hpp"
#include "string.hpp"

#include <array>
#include <cwchar>
#include <filesystem>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

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

bool parse_bool(const std::string& s, bool default_value) noexcept {
    try {
        size_t pos{};
        const auto value{std::stoll(s, &pos)};
        if (pos == s.size()) {
            return value != 0;
        }
        return default_value;
    } catch (const std::exception&) {
        return default_value;
    }
}

Config::CustomPakEntry parse_custom_pak_entry(const std::filesystem::path& path, const std::string& input) {
    enum class State { prop_name, prop_value, ignore_prop_value };

    std::vector<std::string> positionals;
    std::unordered_map<std::string, std::string> properties;
    std::string prop_name;
    std::string prop_value;
    State state{State::prop_name};

    for (std::string::size_type i{}; i < input.size(); ++i) {
        const auto c{input[i]};
        switch (state) {
        case State::prop_name:
            if (c == ',') {
                if (!prop_name.empty()) {
                    positionals.emplace_back(trim(prop_name));
                    prop_name.clear();
                }
                continue;
            }
            if (c == '=') {
                if (prop_name.empty()) {
                    state = State::ignore_prop_value;
                } else {
                    state = State::prop_value;
                }
                continue;
            }
            prop_name += c;
            break;
        case State::prop_value:
            if (c == ',') {
                properties.insert_or_assign(std::string{trim(prop_name)}, std::string{trim(prop_value)});
                prop_name.clear();
                prop_value.clear();
                state = State::prop_name;
                continue;
            }
            prop_value += c;
            break;
        case State::ignore_prop_value:
            if (c == ',') {
                state = State::prop_name;
                continue;
            }
            break;
        }
    }

    if (state == State::prop_value) {
        properties.insert_or_assign(std::string{trim(prop_name)}, std::string{trim(prop_value)});
    } else if (state == State::prop_name && !prop_name.empty()) {
        positionals.emplace_back(trim(prop_name));
    }

    Config::CustomPakEntry entry;
    entry.path = path;

    if (positionals.size() > 0) {
        entry.enabled = parse_bool(positionals[0], entry.enabled);
    }

    for (auto& [pn, pv] : properties) {
        if (pn == "stage") {
            entry.stage = std::move(pv);
        } else if (pn == "enabled") {
            entry.enabled = parse_bool(pv, entry.enabled);
        }
    }

    return entry;
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

        const auto entry_str{ini_read_string(file_path, section_name, pak_path.c_str(), L"0")};
        const auto entry{parse_custom_pak_entry(pak_path, narrow_string(entry_str))};
        if (!entry.enabled) {
            // Ignore
            return;
        }

        config.load_custom_paks.push_back(std::move(entry));
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
