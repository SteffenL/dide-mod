#include "mod.hpp"

#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
#ifndef DM_STATIC_RUNTIME
        // Do not call this function from a DLL that is linked to the static C run-time library (CRT). The static CRT
        // requires DLL_THREAD_ATTACH and DLL_THREAD_DETACH notifications to function properly.
        // Source:
        // https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-disablethreadlibrarycalls
        ::DisableThreadLibraryCalls(hinstDLL);
#endif
        return mod_main(hinstDLL) ? TRUE : FALSE;
    case DLL_PROCESS_DETACH:
        if (lpvReserved != nullptr) {
            // Process is terminating
            break;
        }
        // If we ever need to clean up, do it here
        break;
    }
    return TRUE;
}
