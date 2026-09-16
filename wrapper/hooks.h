#pragma once

#include "wrapped_dll_types.h"
#include <windows.h>

extern OrigFunctions g_origDllFunctions;
extern HMODULE g_origDll;

BOOL initializeHooks();
BOOL shutdownHooks();
