#include <Windows.h>
#include "hooks.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        return initializeHooks();
        break;
    case DLL_PROCESS_DETACH:
        return shutdownHooks();
        break;
    }

    return TRUE;
}
