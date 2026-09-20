#include "exports.hpp"
#include "log.hpp"
#include "mod.hpp"

#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE /*hinstDLL*/, DWORD fdwReason, LPVOID /*lpvReserved*/) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        if (invoke_and_log_exception([] {
                create_dsound_wrapper();
                create_mod();
            })) {
            on_init_error();
            return FALSE;
        }
        break;
    case DLL_PROCESS_DETACH:
        destroy_mod();
        destroy_dsound_wrapper();
        break;
    }
    return TRUE;
}
