#include "dll_notify.hpp"
#include "misc.hpp"

#include <filesystem>
#include <functional>
#include <string>
#include <string_view>
#include <utility>

#include <windows.h>

struct UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWCH Buffer;
};

using PCUNICODE_STRING = const UNICODE_STRING*;
using PUNICODE_STRING = UNICODE_STRING*;

#define LDR_DLL_NOTIFICATION_REASON_LOADED 1
#define LDR_DLL_NOTIFICATION_REASON_UNLOADED 2

struct LDR_DLL_LOADED_NOTIFICATION_DATA {
    ULONG Flags;
    PUNICODE_STRING FullDllName;
    PUNICODE_STRING BaseDllName;
    PVOID DllBase;
    ULONG SizeOfImage;
};

struct LDR_DLL_UNLOADED_NOTIFICATION_DATA {
    ULONG Flags;
    PCUNICODE_STRING FullDllName;
    PCUNICODE_STRING BaseDllName;
    PVOID DllBase;
    ULONG SizeOfImage;
};

union LDR_DLL_NOTIFICATION_DATA {
    LDR_DLL_LOADED_NOTIFICATION_DATA Loaded;
    LDR_DLL_UNLOADED_NOTIFICATION_DATA Unloaded;
};

using PCLDR_DLL_NOTIFICATION_DATA = const LDR_DLL_NOTIFICATION_DATA*;

using LDR_DLL_NOTIFICATION_FUNCTION = VOID NTAPI(ULONG NotificationReason, PCLDR_DLL_NOTIFICATION_DATA NotificationData,
                                                 PVOID Context);

using PLDR_DLL_NOTIFICATION_FUNCTION = LDR_DLL_NOTIFICATION_FUNCTION*;

using LdrRegisterDllNotification_t = NTSTATUS NTAPI(ULONG Flags, PLDR_DLL_NOTIFICATION_FUNCTION NotificationFunction,
                                                    PVOID Context, PVOID* Cookie);

using LdrUnregisterDllNotification_t = NTSTATUS NTAPI(PVOID Cookie);

namespace {
VOID NTAPI notify_cb(ULONG NotificationReason, PCLDR_DLL_NOTIFICATION_DATA NotificationData, PVOID Context) {
    if (auto* self{reinterpret_cast<DllNotifyReg*>(Context)}) {
        if (NotificationReason == LDR_DLL_NOTIFICATION_REASON_LOADED) {
            const auto* name{NotificationData->Loaded.FullDllName};
            self->notify(NotificationData->Loaded.DllBase,
                         std::wstring_view{name->Buffer, name->Length / sizeof(name->Buffer[0])});
        }
    }
}
} // namespace

DllNotifyReg::DllNotifyReg() {
    const auto LdrRegisterDllNotification{m_ntdll.sym<LdrRegisterDllNotification_t>("LdrRegisterDllNotification")};
    if (LdrRegisterDllNotification(0, notify_cb, this, &m_cookie) != 0) {
        throw Error{"Failed to register DLL notify"};
    }
}

DllNotifyReg::~DllNotifyReg() {
    if (m_moved) {
        return;
    }
    if (m_cookie) {
        const auto LdrUnregisterDllNotification{m_ntdll.sym<LdrUnregisterDllNotification_t>(
            "LdrUnregisterDllNotification")};
        LdrUnregisterDllNotification(m_cookie);
    }
}

DllNotifyReg::DllNotifyReg(DllNotifyReg&& other) noexcept
        : m_ntdll{std::move(other.m_ntdll)}, m_callbacks{std::move(other.m_callbacks)},
          m_cookie{std::exchange(other.m_cookie, nullptr)}, m_moved{std::exchange(other.m_moved, true)} {}

DllNotifyReg& DllNotifyReg::operator=(DllNotifyReg&& other) noexcept {
    if (this != &other) {
        m_ntdll = std::move(other.m_ntdll);
        m_callbacks = std::move(other.m_callbacks);
        m_cookie = std::exchange(other.m_cookie, nullptr);
        m_moved = std::exchange(other.m_moved, true);
    }
    return *this;
}

void DllNotifyReg::notify(void* handle, std::filesystem::path dll_path) {
    for (auto& cb : m_callbacks) {
        cb(handle, std::move(dll_path));
    }
}

void DllNotifyReg::subscribe(std::function<void(void*, std::filesystem::path)> callback) {
    m_callbacks.push_back(std::move(callback));
}
