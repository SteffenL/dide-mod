#pragma once

#include "dynlib.hpp"

#include <filesystem>
#include <functional>
#include <vector>

struct DllNotifyReg {
    DllNotifyReg();
    ~DllNotifyReg();
    DllNotifyReg(const DllNotifyReg&) = delete;
    DllNotifyReg& operator=(const DllNotifyReg&) = delete;
    DllNotifyReg(DllNotifyReg&& other) noexcept;
    DllNotifyReg& operator=(DllNotifyReg&& other) noexcept;
    void notify(std::filesystem::path full_dll_name);
    void subscribe(std::function<void(std::filesystem::path)> callback);

private:
    DynLib m_ntdll{"ntdll.dll"};
    std::vector<std::function<void(std::filesystem::path)>> m_callbacks;
    void* m_cookie{};
    bool m_moved{};
};
