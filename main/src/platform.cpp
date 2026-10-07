#include "platform.hpp"
#include "dynlib.hpp"
#include "misc.hpp"
#include "string.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <windows.h>
// Must come after windows.h
#include <dbghelp.h>
#include <intrin.h>
#include <winternl.h>

std::filesystem::path exe_path() {
    std::vector<wchar_t> buffer(MAX_PATH, '\0');
    while (true) {
        const std::size_t size{::GetModuleFileNameW(NULL, buffer.data(), static_cast<DWORD>(buffer.size()))};
        if (size == 0) {
            throw Error{"Unable to get exe path"};
        }
        if (size >= buffer.size()) {
            buffer.resize(size);
            continue;
        }
        return std::wstring_view{buffer.data(), size};
    };
}

uintptr_t get_base_of_code(uintptr_t image_base) {
    const auto nt_headers{::ImageNtHeader(reinterpret_cast<void*>(image_base))};
    return image_base + nt_headers->OptionalHeader.BaseOfCode;
}

uintptr_t get_size_of_code(uintptr_t image_base) {
    const auto nt_headers{::ImageNtHeader(reinterpret_cast<void*>(image_base))};
    return nt_headers->OptionalHeader.SizeOfCode;
}

std::filesystem::path exe_dir() { return exe_path().parent_path(); }

void msgbox_error(const std::string& text, const std::string& title) {
    ::MessageBoxW(nullptr, widen_string(text).c_str(), widen_string(title).c_str(), MB_TASKMODAL | MB_ICONERROR);
}

[[noreturn]] void kill_current_process(int exit_code) {
    std::ignore = ::TerminateProcess(::GetCurrentProcess(), static_cast<UINT>(exit_code));
    // Alternative to C++23 std::unreachable()
#if defined(_MSC_VER) && !defined(__clang__)
    __assume(false);
#else
    __builtin_unreachable();
#endif
}

#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Warray-bounds"
#endif

namespace {
std::function<void()> g_ppi_callback;
PPS_POST_PROCESS_INIT_ROUTINE g_ppi_old_fn{};
} // namespace

VOID __stdcall PostProcessInitRoutine_detour() {
    auto* peb{NtCurrentTeb()->ProcessEnvironmentBlock};
    peb->PostProcessInitRoutine = g_ppi_old_fn;
    if (g_ppi_old_fn) {
        g_ppi_old_fn();
    }
    if (g_ppi_callback) {
        g_ppi_callback();
    }
}

void set_post_process_init_routine(std::function<void()> callback) {
    g_ppi_callback = std::move(callback);
    auto* peb{NtCurrentTeb()->ProcessEnvironmentBlock};
    g_ppi_old_fn = peb->PostProcessInitRoutine;
    peb->PostProcessInitRoutine = PostProcessInitRoutine_detour;
}

bool is_loader_lock_held_by_current_thread() {
    using RtlIsCriticalSectionLockedByThread_t = BOOL(NTAPI*)(PRTL_CRITICAL_SECTION CriticalSection);
    auto ntdll{DynLib::attach_by_name("ntdll.dll")};
    const auto RtlIsCriticalSectionLockedByThread{ntdll.sym<RtlIsCriticalSectionLockedByThread_t>(
        "RtlIsCriticalSectionLockedByThread")};

    // Get the loader lock critical section from the PEB structure
    auto* peb{reinterpret_cast<PBYTE>(NtCurrentTeb()->ProcessEnvironmentBlock)};
#if defined(_M_AMD64)
    auto cs{*reinterpret_cast<PRTL_CRITICAL_SECTION*>(peb + 0x110)};
#elif defined(_M_IX86)
    auto cs{*reinterpret_cast<PRTL_CRITICAL_SECTION*>(peb + 0xa0)};
#else
    #error Unsupported architecture
#endif

    return !!RtlIsCriticalSectionLockedByThread(cs);
}

#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif

std::optional<std::string> get_wine_version_str() {
    using wine_get_version_t = const char* (*)();
    auto ntdll{DynLib::attach_by_name("ntdll.dll")};
    const auto wine_get_version{ntdll.try_sym<wine_get_version_t>("wine_get_version")};
    if (wine_get_version) {
        return std::make_optional(wine_get_version());
    }
    return std::nullopt;
}
