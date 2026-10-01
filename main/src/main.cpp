#include "log.hpp"
#include "mod.hpp"

#ifdef DM_USE_DSOUND_PROXY
    #include "dsound/exports.hpp"
#endif
#ifdef DM_USE_XINPUT_PROXY
    #include "xinput/exports.hpp"
#endif

#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID /*lpvReserved*/) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        ::DisableThreadLibraryCalls(hinstDLL);
        if (invoke_and_log_exception([] {
                init();
#ifdef DM_USE_DSOUND_PROXY
                create_dsound_wrapper();
#endif
#ifdef DM_USE_XINPUT_PROXY
                create_xinput_wrapper();
#endif
                create_mod();
            })) {
            on_init_error();
            return FALSE;
        }
        break;
    case DLL_PROCESS_DETACH:
        invoke_and_log_exception([] {
            destroy_mod();
#ifdef DM_USE_XINPUT_PROXY
            destroy_xinput_wrapper();
#endif
#ifdef DM_USE_DSOUND_PROXY
            destroy_dsound_wrapper();
#endif
        });
        break;
    }
    return TRUE;
}
