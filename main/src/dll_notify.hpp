#pragma once

#include "dynlib.hpp"

#include <filesystem>
#include <functional>
#include <vector>

struct DllNotifyReg {
    using NotifyCallbackFn = std::function<void(void* handle, std::filesystem::path dll_path)>;

    DllNotifyReg();
    ~DllNotifyReg();
    DllNotifyReg(const DllNotifyReg&) = delete;
    DllNotifyReg& operator=(const DllNotifyReg&) = delete;
    DllNotifyReg(DllNotifyReg&& other) noexcept;
    DllNotifyReg& operator=(DllNotifyReg&& other) noexcept;
    void notify(void* handle, std::filesystem::path dll_path);
    void subscribe(NotifyCallbackFn callback);

private:
    DynLib m_ntdll{DynLib::attach_by_name("ntdll.dll")};
    std::vector<NotifyCallbackFn> m_callbacks;
    void* m_cookie{};
    bool m_moved{};
};
