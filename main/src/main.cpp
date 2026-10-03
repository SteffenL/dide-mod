#include "log.hpp"
#include "mod.hpp"

#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID /*lpvReserved*/) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        ::DisableThreadLibraryCalls(hinstDLL);
        if (invoke_and_log_exception([hinstDLL] {
                init(hinstDLL);
                run();
            })) {
            on_init_error();
            return FALSE;
        }
        break;
    }
    return TRUE;
}
