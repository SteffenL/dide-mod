#include "dsound/exports.hpp"
#include "log.hpp"
#include "mod.hpp"
#include "xinput/exports.hpp"

#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID /*lpvReserved*/) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        ::DisableThreadLibraryCalls(hinstDLL);
        if (invoke_and_log_exception([] {
                init();
                create_dsound_wrapper();
                //create_xinput_wrapper();
                create_mod();
            })) {
            on_init_error();
            return FALSE;
        }
        break;
    case DLL_PROCESS_DETACH:
        invoke_and_log_exception([] {
            destroy_mod();
            //destroy_xinput_wrapper();
            destroy_dsound_wrapper();
        });
        break;
    }
    return TRUE;
}
