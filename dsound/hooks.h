#pragma once

#include "libpad_types.h"
#include <Windows.h>

extern OrigFunctions g_origDllFunctions;
extern HMODULE g_origDll;

BOOL initializeHooks();
BOOL shutdownHooks();
