#include "mod.hpp"

#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID /*lpvReserved*/) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        ::DisableThreadLibraryCalls(hinstDLL);
        return mod_main(hinstDLL) ? TRUE : FALSE;
    }
    return TRUE;
}
