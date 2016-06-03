#include "hooks.h"
#include <core/DideModInitializer.h>
#include <core/ModMsgBox.h>
#include <core/Logging.h>
#include <core/HostAppTargetInfo.h>

#include <MinHook.h>

#include <string>
#include <stdio.h>
#include <strsafe.h>


#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)

OrigFunctions g_origDllFunctions;
HMODULE g_origDll = NULL;


bool loadLibpad()
{
	std::string originalDllName = "libpad.backup.dll";
    auto origDll = LoadLibraryA(originalDllName.c_str());
	if (!origDll) {
		EZLOGGER << "Failed to load libpad; make sure the original file is named \"" << originalDllName << "\"." << std::endl;
        return false;
    }

    g_origDll = origDll;

    g_origDllFunctions.cellPadInit = reinterpret_cast<OrigFunctions::cellPadInit_t>(GetProcAddress(origDll, "cellPadInit"));
    g_origDllFunctions.cellPadEnd = reinterpret_cast<OrigFunctions::cellPadEnd_t>(GetProcAddress(origDll, "cellPadEnd"));
    g_origDllFunctions.cellPadClearBuf = reinterpret_cast<OrigFunctions::cellPadClearBuf_t>(GetProcAddress(origDll, "cellPadClearBuf"));
    g_origDllFunctions.cellPadGetData = reinterpret_cast<OrigFunctions::cellPadGetData_t>(GetProcAddress(origDll, "cellPadGetData"));
    g_origDllFunctions.cellPadSetActDirect = reinterpret_cast<OrigFunctions::cellPadSetActDirect_t>(GetProcAddress(origDll, "cellPadSetActDirect"));
    g_origDllFunctions.cellPadGetInfo2 = reinterpret_cast<OrigFunctions::cellPadGetInfo2_t>(GetProcAddress(origDll, "cellPadGetInfo2"));
    g_origDllFunctions.cellPadSetPortSetting = reinterpret_cast<OrigFunctions::cellPadSetPortSetting_t>(GetProcAddress(origDll, "cellPadSetPortSetting"));

    g_origDllFunctions.cellPadGetCapabilityInfo = reinterpret_cast<OrigFunctions::cellPadGetCapabilityInfo_t>(GetProcAddress(origDll, "cellPadGetCapabilityInfo"));
    g_origDllFunctions.cellPadGetInfo = reinterpret_cast<OrigFunctions::cellPadGetInfo_t>(GetProcAddress(origDll, "cellPadGetInfo"));
    g_origDllFunctions.cellPadSetPressMode = reinterpret_cast<OrigFunctions::cellPadSetPressMode_t>(GetProcAddress(origDll, "cellPadSetPressMode"));
    g_origDllFunctions.cellPadInfoPressMode = reinterpret_cast<OrigFunctions::cellPadInfoPressMode_t>(GetProcAddress(origDll, "cellPadInfoPressMode"));
    g_origDllFunctions.cellPadSetSensorMode = reinterpret_cast<OrigFunctions::cellPadSetSensorMode_t>(GetProcAddress(origDll, "cellPadSetSensorMode"));
    g_origDllFunctions.cellPadInfoSensorMode = reinterpret_cast<OrigFunctions::cellPadInfoSensorMode_t>(GetProcAddress(origDll, "cellPadInfoSensorMode"));

    if (!g_origDllFunctions.cellPadInit ||
        !g_origDllFunctions.cellPadEnd ||
        !g_origDllFunctions.cellPadClearBuf ||
        !g_origDllFunctions.cellPadGetData ||
        !g_origDllFunctions.cellPadSetActDirect ||
        !g_origDllFunctions.cellPadGetInfo2 ||
        !g_origDllFunctions.cellPadSetPortSetting ||

        !g_origDllFunctions.cellPadGetCapabilityInfo ||
        !g_origDllFunctions.cellPadGetInfo ||
        !g_origDllFunctions.cellPadSetPressMode ||
        !g_origDllFunctions.cellPadInfoPressMode ||
        !g_origDllFunctions.cellPadSetSensorMode ||
        !g_origDllFunctions.cellPadInfoSensorMode)
	{
		EZLOGGER << "Found \"" << originalDllName << "\" but failed to load it properly." << std::endl;
        return false;
    }

    return !!origDll;
}

bool initializeGameBaseAddresses()
{
	auto mainExe = GetModuleHandleA(NULL);
	if (!mainExe) {
		EZLOGGER << "Failed to get handle of main EXE." << std::endl;
		return false;
	}

	auto gameDll = GetModuleHandleA("gamedll_x64_rwdi.dll");
	if (!gameDll) {
		EZLOGGER << "Game DLL is not loaded." << std::endl;
		return false;
	}

	auto engineDll = GetModuleHandleA("engine_x64_rwdi.dll");
	if (!engineDll) {
		EZLOGGER << "Engine DLL is not loaded." << std::endl;
		return false;
	}

	auto filesystemDll = GetModuleHandleA("filesystem_x64_rwdi.dll");
	if (!filesystemDll) {
		EZLOGGER << "File system DLL is not loaded." << std::endl;
		return false;
	}

	HostAppTargetInfo::MainExe = reinterpret_cast<uintptr_t>(mainExe);
	HostAppTargetInfo::GameDll = reinterpret_cast<uintptr_t>(gameDll);
	HostAppTargetInfo::EngineDll = reinterpret_cast<uintptr_t>(engineDll);
	HostAppTargetInfo::FilesystemDll = reinterpret_cast<uintptr_t>(filesystemDll);

	return true;
}

BOOL initializeHooks()
{
	bool success = false;

	do 
	{
		if (!loadLibpad()) {
			break;
		}

		if (!initializeGameBaseAddresses()) {
			break;
		}

		if (MH_Initialize() != MH_OK) {
			EZLOGGER << "Failed to initialize MinHook." << std::endl;
			break;
		}

		if (!DideModInitializer::Initialize()) {
			break;
		}

		success = true;
	} while (false);

	if (!success) {
		ModMsgBox::Show(NULL, "Initialization failed; please see the log file for details.", MB_ICONERROR);
		ExitProcess(1);
		return FALSE;
	}

    return TRUE;
}

BOOL shutdownHooks()
{
	DideModInitializer::Shutdown();
    MH_Uninitialize();

    if (g_origDll && FreeLibrary(g_origDll)) {
        g_origDll = NULL;
    }

    return TRUE;
}
