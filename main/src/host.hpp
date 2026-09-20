#pragma once

#include <cstdint>

class HostAppInfo {
public:
    uintptr_t main_exe;
    uintptr_t game_dll;
    uintptr_t engine_dll;
    uintptr_t filesystem_dll;
};

HostAppInfo load_host_app_info();
