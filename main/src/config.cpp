#include "config.hpp"
#include "misc.hpp"

#include <filesystem>

#include <windows.h>

namespace config {

void load_general(Config& config, const wchar_t* file_path) {
    static constexpr auto* section_name{L"General"};

    {
        std::vector<wchar_t> value_str(1024, 0);
        ::GetPrivateProfileStringW(section_name, L"EnableMod", NULL, value_str.data(),
                                   static_cast<DWORD>(value_str.size() / sizeof(value_str[0])), file_path);
        unsigned int value = 0;
        swscanf_s(value_str.data(), L"%u", &value);
        config.general.enable_mod = value ? true : false;
    }

    {
        std::vector<wchar_t> value_str(1024, 0);
        ::GetPrivateProfileStringW(section_name, L"EnableLogging", NULL, value_str.data(),
                                   static_cast<DWORD>(value_str.size() / sizeof(value_str[0])), file_path);
        unsigned int value = 0;
        swscanf_s(value_str.data(), L"%u", &value);
        config.general.enable_logging = value ? true : false;
    }

    {
        std::vector<wchar_t> value_str(1024, 0);
        ::GetPrivateProfileStringW(section_name, L"LogFile", L"dide_mod.log", value_str.data(),
                                   static_cast<DWORD>(value_str.size() / sizeof(value_str[0])), file_path);
        config.general.log_file = value_str.data();
    }
}

void load_features(Config& config, const wchar_t* file_path) {
    static constexpr auto* section_name{L"Features"};

    {
        std::vector<wchar_t> value_str(1024, 0);
        ::GetPrivateProfileStringW(section_name, L"DeveloperMenu", NULL, value_str.data(),
                                   static_cast<DWORD>(value_str.size() / sizeof(value_str[0])), file_path);
        unsigned int value = 0;
        swscanf_s(value_str.data(), L"%u", &value);
        config.features.developer_menu = value ? true : false;
    }

    {
        std::vector<wchar_t> value_str(1024, 0);
        ::GetPrivateProfileStringW(section_name, L"CustomPak", NULL, value_str.data(),
                                   static_cast<DWORD>(value_str.size() / sizeof(value_str[0])), file_path);
        unsigned int value = 0;
        swscanf_s(value_str.data(), L"%u", &value);
        config.features.custom_pak = value ? true : false;
    }
}

void load_custom_pak(Config& config, const wchar_t* file_path) {
    if (!config.features.custom_pak) {
        return;
    }

    static constexpr auto* section_name{L"CustomPak"};

    {
        std::vector<wchar_t> key_names_buffer(1024, 0);
        ::GetPrivateProfileStringW(section_name, NULL, NULL, key_names_buffer.data(),
                                   static_cast<DWORD>(key_names_buffer.size() / sizeof(key_names_buffer[0])),
                                   file_path);

        for (const wchar_t* key_name = key_names_buffer.data(); *key_name != L'\0'; key_name += wcslen(key_name) + 1) {
            // Line comments (;) are automatically ignored by the Windows API function
            std::wstring_view pak_path{key_name};
            if (pak_path.empty()) {
                // Ignore
                continue;
            }

            std::vector<wchar_t> pak_enabled_str(1024, 0);
            ::GetPrivateProfileStringW(section_name, key_name, NULL, pak_enabled_str.data(),
                                       static_cast<DWORD>(pak_enabled_str.size() / sizeof(pak_enabled_str[0])),
                                       file_path);
            unsigned int pak_enabled = 0;
            swscanf_s(pak_enabled_str.data(), L"%u", &pak_enabled);
            if (!pak_enabled) {
                // Ignore
                continue;
            }

            config.load_custom_paks.emplace_back(pak_path);
        }
    }
}

Result<Config> load_file(const std::filesystem::path& file_path) {
    Config config;
    const auto* file_path_c{file_path.c_str()};
    load_general(config, file_path_c);
    load_features(config, file_path_c);
    load_custom_pak(config, file_path_c);
    return config;
}

} // namespace config
