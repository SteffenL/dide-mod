#include "startup_hook.hpp"
#include "dynlib.hpp"
#include "log.hpp"
#include "minhook.hpp"
#include "platform.hpp"

#include <atomic>
#include <functional>
#include <string_view>

#include <windows.h>

namespace {

using GetStartupInfoA_t = VOID(WINAPI*)(LPSTARTUPINFOA lpStartupInfo);
using GetStartupInfoW_t = VOID(WINAPI*)(LPSTARTUPINFOW lpStartupInfo);
using RtlGetVersion_t = NTSTATUS(NTAPI*)(PRTL_OSVERSIONINFOW lpVersionInformation);

GetStartupInfoA_t g_original_GetStartupInfoA{};
GetStartupInfoW_t g_original_GetStartupInfoW{};
std::function<void(std::string_view)> g_callback;
std::atomic_bool g_startup_hit;

template<typename Name, typename Callable>
void wrap_call(const Name& name, Callable fn) {
    // Assuming that we reached here during the Windows loader process initialization, we're almost guaranteed to have
    // been called early by the CRT startup routine of either the main executable or a DLL. A DLL's CRT startup routine
    // will be called while the calling thread is owning the loader lock, but the lock will not be owned by the calling
    // thread during the main executable's own CRT startup routine. The first call during this initialization stage,
    // where the calling thread doesn't own the lock, should therefore be in the main executable's CRT startup routine.
    // While all statically imported DLLs should be initialized by then, neither the executable's static initializers
    // nor CRT state is guaranteed to be complete. We should be able to avoid deadlocks concerning the loader lock, and
    // work that doesn't depend on partially initialized state should be safe as well. If we reached here after the
    // Windows loader process initialization, it may also be too late to do whatever the callback function needs to do.
    // Having multiple different hooks that end up here is OK as we just process the first plausibly safe call.
    try {
        if (g_startup_hit) {
            return;
        }
        // We may reach this code multiple times while the loader lock is owned the current thread, so try to avoid it
        if (is_loader_lock_held_by_current_thread()) {
            if (::IsDebuggerPresent()) {
                LOG("Deferring startup in {} due to owned loader lock.", name);
            }
            return;
        }
        if (::IsDebuggerPresent()) {
            LOG("Clear to start up in {}.", name);
            // To check if the loader lock is owned by the current thread, run the executable under WinDbg, then run the
            // command "!critsec ntdll!LdrpLoaderLock" in WinDbg.
            __debugbreak();
        }
        if (!g_startup_hit.exchange(true)) {
            fn(name);
        }
    } catch (...) {
        // Can't really do anything about this, but don't let the exception escape
    }
}

VOID WINAPI kernel32_GetStartupInfoA_detour(LPSTARTUPINFOA lpStartupInfo) {
    wrap_call("kernel32.GetStartupInfoA", g_callback);
    g_original_GetStartupInfoA(lpStartupInfo);
}

VOID WINAPI kernel32_GetStartupInfoW_detour(LPSTARTUPINFOW lpStartupInfo) {
    wrap_call("kernel32.GetStartupInfoW", g_callback);
    g_original_GetStartupInfoW(lpStartupInfo);
}

} // namespace

void create_startup_hook(std::function<void(std::string_view)> callback) {
    g_callback = std::move(callback);

    // This is an early entry point outside of the loader lock, assuming that it still works, which it does on
    // Windows 11 10.0.26200.
    // This will not work with Wine until implemented there: https://bugs.winehq.org/show_bug.cgi?id=55949
    // As of Wine 11.0, it is not implemented.
    set_post_process_init_routine([] { wrap_call("PEB.PostProcessInitRoutine", g_callback); });

    // In case the above does not work, we hook some functions that are likely to be called early, and hopefully not
    // always called while the loader lock is owned by the current thread.

    minhook::initialize();

    minhook::create_hook("kernel32.GetStartupInfoA", ::GetStartupInfoA, kernel32_GetStartupInfoA_detour,
                         g_original_GetStartupInfoA);
    minhook::queue_enable_hook("kernel32.GetStartupInfoA", ::GetStartupInfoA);

    minhook::create_hook("kernel32.GetStartupInfoW", ::GetStartupInfoW, kernel32_GetStartupInfoW_detour,
                         g_original_GetStartupInfoW);
    minhook::queue_enable_hook("kernel32.GetStartupInfoW", ::GetStartupInfoW);

    minhook::apply_queued();
}
