#pragma once

#include <cstdint>
#include <string>

class HostAppInfo {
public:
    std::string id;
    struct Version {
        uint32_t major{};
        uint32_t minor{};
        uint32_t patch{};
    } version;
    uintptr_t main_exe;
};

HostAppInfo load_host_app_info();
