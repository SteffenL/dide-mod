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
    void notify(void* handle, std::filesystem::path dll_path);
    void subscribe(std::function<void(void*, std::filesystem::path)> callback);

private:
    DynLib m_ntdll{DynLib::attach_by_name("ntdll.dll")};
    std::vector<std::function<void(void*, std::filesystem::path)>> m_callbacks;
    void* m_cookie{};
    bool m_moved{};
};
