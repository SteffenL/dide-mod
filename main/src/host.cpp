#include "host.hpp"
#include "misc.hpp"
#include "unicode.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <format>
#include <optional>
#include <string_view>
#include <vector>

#include <windows.h>

HostAppInfo load_host_app_info() {
    const auto main_exe{::GetModuleHandleA(NULL)};
    if (!main_exe) {
        throw Error{"Failed to get handle of main EXE"};
    }

    std::array<wchar_t, MAX_PATH> main_exe_file_name{};
    ::GetModuleFileNameW(main_exe, main_exe_file_name.data(), static_cast<DWORD>(main_exe_file_name.size()));

    std::string host_id;
    std::optional<HostAppInfo::Version> host_version;

    const auto version_size{::GetFileVersionInfoSizeW(main_exe_file_name.data(), NULL)};
    if (version_size > 0) {
        std::vector<uint8_t> version_block(version_size, 0);
        ::GetFileVersionInfoW(main_exe_file_name.data(), NULL, version_size, version_block.data());

        VS_FIXEDFILEINFO* file_info{};
        UINT file_info_size{};
        ::VerQueryValueW(version_block.data(), L"\\", reinterpret_cast<LPVOID*>(&file_info), &file_info_size);
        if (file_info && file_info_size > 0) {
            const auto sig{file_info->dwSignature};
            assert(sig == 0xfeef04bd);
            host_version = HostAppInfo::Version{
                .major = HIWORD(file_info->dwFileVersionMS),
                .minor = LOWORD(file_info->dwFileVersionMS),
                .patch = HIWORD(file_info->dwFileVersionLS),
            };
        }

        struct LANGANDCODEPAGE {
            WORD wLanguage;
            WORD wCodePage;
        }* translation_array{};
        UINT translation_size{};
        ::VerQueryValueW(version_block.data(), L"\\VarFileInfo\\Translation",
                         reinterpret_cast<LPVOID*>(&translation_array), &translation_size);
        if (translation_array && translation_size > 0) {
            for (UINT i{}; i < translation_size / sizeof(LANGANDCODEPAGE); ++i) {
                const auto& translation{translation_array[i]};
                if (host_id.empty()) {
                    wchar_t* product_name{};
                    UINT product_name_size{};
                    ::VerQueryValueW(version_block.data(),
                                     std::format(L"\\StringFileInfo\\{:04x}{:04x}\\ProductName", translation.wLanguage,
                                                 translation.wCodePage)
                                         .c_str(),
                                     reinterpret_cast<LPVOID*>(&product_name), &product_name_size);
                    if (product_name && product_name_size > 0) {
                        host_id = narrow_string(trim_right(std::wstring_view{product_name, product_name_size}));
                    }
                }
            }
        }
    }

    if (host_id.empty()) {
        throw Error{"Unable to identify host app"};
    }

    if (!host_version.has_value()) {
        throw Error{"Unable to determine host app version"};
    }

    const auto game_dll{::GetModuleHandleA("gamedll_x64_rwdi.dll")};
    if (!game_dll) {
        throw Error{"Game DLL is not loaded"};
    }

    const auto engine_dll{::GetModuleHandleA("engine_x64_rwdi.dll")};
    if (!engine_dll) {
        throw Error{"Engine DLL is not loaded"};
    }

    const auto filesystem_dll{::GetModuleHandleA("filesystem_x64_rwdi.dll")};
    if (!filesystem_dll) {
        throw Error{"File system DLL is not loaded"};
    }

    return HostAppInfo{
        .id = std::move(host_id),
        .version = host_version.value(),
        .main_exe = reinterpret_cast<uintptr_t>(main_exe),
        .game_dll = reinterpret_cast<uintptr_t>(game_dll),
        .engine_dll = reinterpret_cast<uintptr_t>(engine_dll),
        .filesystem_dll = reinterpret_cast<uintptr_t>(filesystem_dll),
    };
}
