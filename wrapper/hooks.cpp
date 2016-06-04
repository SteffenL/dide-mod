#include "hooks.h"
#include <core/DideModInitializer.h>
#include <core/ModMsgBox.h>
#include <core/Logging.h>
#include <core/HostAppTargetInfo.h>

#include <nowide/convert.hpp>
#include <MinHook.h>

#include <string>
#include <stdio.h>
#include <strsafe.h>


#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)

OrigFunctions g_origDllFunctions;
HMODULE g_origDll = NULL;


bool loadWrappedDll()
{
	wchar_t systemDir[MAX_PATH] = {0};
	GetSystemDirectoryW(systemDir, MAX_PATH);
	wchar_t originalDllName[] = L"DSOUND.DLL";
	wchar_t originalDllPath[MAX_PATH] = {0};
	lstrcatW(originalDllPath, systemDir);
	lstrcatW(originalDllPath, L"\\");
	lstrcatW(originalDllPath, originalDllName);

    auto origDll = LoadLibraryW(originalDllPath);
	if (!origDll) {
		EZLOGGER << "Failed to load wrapped DLL: " << nowide::narrow(originalDllPath) << std::endl;
        return false;
    }

    g_origDll = origDll;

    g_origDllFunctions.DirectSoundCreate = reinterpret_cast<OrigFunctions::DirectSoundCreate_t>(GetProcAddress(origDll, "DirectSoundCreate"));
    g_origDllFunctions.DirectSoundEnumerateA = reinterpret_cast<OrigFunctions::DirectSoundEnumerateA_t>(GetProcAddress(origDll, "DirectSoundEnumerateA"));
    g_origDllFunctions.DirectSoundEnumerateW = reinterpret_cast<OrigFunctions::DirectSoundEnumerateW_t>(GetProcAddress(origDll, "DirectSoundEnumerateW"));

    g_origDllFunctions.DirectSoundCaptureCreate = reinterpret_cast<OrigFunctions::DirectSoundCaptureCreate_t>(GetProcAddress(origDll, "DirectSoundCaptureCreate"));
    g_origDllFunctions.DirectSoundCaptureEnumerateA = reinterpret_cast<OrigFunctions::DirectSoundCaptureEnumerateA_t>(GetProcAddress(origDll, "DirectSoundCaptureEnumerateA"));
    g_origDllFunctions.DirectSoundCaptureEnumerateW = reinterpret_cast<OrigFunctions::DirectSoundCaptureEnumerateW_t>(GetProcAddress(origDll, "DirectSoundCaptureEnumerateW"));

	g_origDllFunctions.DirectSoundCreate8 = reinterpret_cast<OrigFunctions::DirectSoundCreate8_t>(GetProcAddress(origDll, "DirectSoundCreate8"));
    g_origDllFunctions.DirectSoundCaptureCreate8 = reinterpret_cast<OrigFunctions::DirectSoundCaptureCreate8_t>(GetProcAddress(origDll, "DirectSoundCaptureCreate8"));
    g_origDllFunctions.DirectSoundFullDuplexCreate = reinterpret_cast<OrigFunctions::DirectSoundFullDuplexCreate_t>(GetProcAddress(origDll, "DirectSoundFullDuplexCreate"));

    g_origDllFunctions.GetDeviceID = reinterpret_cast<OrigFunctions::GetDeviceID_t>(GetProcAddress(origDll, "GetDeviceID"));

    g_origDllFunctions.DllCanUnloadNow = reinterpret_cast<OrigFunctions::DllCanUnloadNow_t>(GetProcAddress(origDll, "DllCanUnloadNow"));
    g_origDllFunctions.DllGetClassObject = reinterpret_cast<OrigFunctions::DllGetClassObject_t>(GetProcAddress(origDll, "DllGetClassObject"));

    if (!g_origDllFunctions.DirectSoundCreate ||
        !g_origDllFunctions.DirectSoundEnumerateA ||
        !g_origDllFunctions.DirectSoundEnumerateW ||

        !g_origDllFunctions.DirectSoundCaptureCreate ||
        !g_origDllFunctions.DirectSoundCaptureEnumerateA ||
        !g_origDllFunctions.DirectSoundCaptureEnumerateW ||

        !g_origDllFunctions.DirectSoundCreate8 ||
        !g_origDllFunctions.DirectSoundCaptureCreate8 ||
        !g_origDllFunctions.DirectSoundFullDuplexCreate ||

        !g_origDllFunctions.GetDeviceID ||

        !g_origDllFunctions.DllCanUnloadNow ||
        !g_origDllFunctions.DllGetClassObject)
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
		if (!loadWrappedDll()) {
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
