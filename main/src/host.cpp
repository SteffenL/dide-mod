#include "host.hpp"
#include "misc.hpp"

#include <windows.h>

HostAppInfo load_host_app_info() {
    const auto main_exe{reinterpret_cast<uintptr_t>(::GetModuleHandleA(NULL))};
    if (!main_exe) {
        throw Error{"Failed to get handle of main EXE"};
    }

    const auto game_dll{reinterpret_cast<uintptr_t>(::GetModuleHandleA("gamedll_x64_rwdi.dll"))};
    if (!game_dll) {
        throw Error{"Game DLL is not loaded"};
    }

    const auto engine_dll{reinterpret_cast<uintptr_t>(::GetModuleHandleA("engine_x64_rwdi.dll"))};
    if (!engine_dll) {
        throw Error{"Engine DLL is not loaded"};
    }

    const auto filesystem_dll{reinterpret_cast<uintptr_t>(::GetModuleHandleA("filesystem_x64_rwdi.dll"))};
    if (!filesystem_dll) {
        throw Error{"File system DLL is not loaded"};
    }

    return HostAppInfo{
        .main_exe = main_exe,
        .game_dll = game_dll,
        .engine_dll = engine_dll,
        .filesystem_dll = filesystem_dll,
    };
}
