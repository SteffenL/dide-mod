#include "DideMod.h"
#include <core/ModMsgBox.h>
#include <core/Logging.h>
#include <core/HostAppTargetInfo.h>
#include <core/StandardPaths.h>
#include <core/CoreVersion.h>

#include <cstring>
#include <Windows.h>
#include <MinHook.h>
#include <DbgHelp.h>

#pragma comment(lib, "DbgHelp.lib")


struct FFSAddSourceFlags
{
	enum ENUM
	{
		Unknown09 = 9
	};
};

typedef bool (*fs_add_source_t)(const char* path, FFSAddSourceFlags::ENUM flags);
fs_add_source_t fs_add_source_original;

bool fs_add_source_detour(const char* path, FFSAddSourceFlags::ENUM flags)
{
	EZLOGGER << "Adding source (flags = " << flags << "): " << path;
	auto success = fs_add_source_original(path, flags);
	EZLOGGER << " (" << (success ? std::string("OK") : std::string("error")) << ")" << std::endl;
	return success;
}


typedef void (*engine_InitializeGameScript_t)(void* p1, void* p2);
engine_InitializeGameScript_t engine_InitializeGameScript_original;
DideMod* g_mod = nullptr;

void engine_InitializeGameScript_detour(void* p1, void* p2)
{
	if (g_mod) {
		g_mod->LoadPaks();
	}

	engine_InitializeGameScript_original(p1, p2);
}


DideMod::DideMod(Config config)
	: m_config(config)
{
}

DideMod::~DideMod()
{
}

bool DideMod::Initialize()
{
	initializeLogger(m_config.General.LogFile, m_config.General.EnableLogging);
	logVersion();

	if (!validateConfig()) {
		EZLOGGER << "Configuration is invalid." << std::endl;
		return false;
	}

	EZLOGGER << "Initializing..." << std::endl;

	logConfig();

	if (!m_config.General.EnableMod) {
		return true;
	}

	EZLOGGER << "Game DLL image base: " << std::hex << HostAppTargetInfo::GameDll << std::endl;
	EZLOGGER << "Engine DLL image base: " << std::hex << HostAppTargetInfo::EngineDll << std::endl;
	EZLOGGER << "Filesystem DLL image base: " << std::hex << HostAppTargetInfo::FilesystemDll << std::endl;

	g_mod = this;


	fs_add_source_t fs_add_source = (fs_add_source_t)GetProcAddress((HMODULE)HostAppTargetInfo::FilesystemDll, "?add_source@fs@@YA_NPEBDW4ENUM@FFSAddSourceFlags@@@Z");
	if (!fs_add_source) {
		EZLOGGER << "Failed to get address of filesystem.fs::add_source." << std::endl;
		return false;
	}

	EZLOGGER << "filesystem.fs::add_source: " << std::hex << fs_add_source << std::endl;
	MH_CreateHook(fs_add_source, fs_add_source_detour, (LPVOID*)&fs_add_source_original);
	MH_QueueEnableHook(fs_add_source);


	engine_InitializeGameScript_t engine_InitializeGameScript = (engine_InitializeGameScript_t)GetProcAddress((HMODULE)HostAppTargetInfo::EngineDll, "InitializeGameScript");
	if (!engine_InitializeGameScript) {
		EZLOGGER << "Failed to get address of engine.InitializeGameScript." << std::endl;
		return false;
	}

	EZLOGGER << "engine.InitializeGameScript: " << std::hex << engine_InitializeGameScript << std::endl;
	MH_CreateHook(engine_InitializeGameScript, engine_InitializeGameScript_detour, (LPVOID*)&engine_InitializeGameScript_original);
	MH_QueueEnableHook(engine_InitializeGameScript);


	MH_ApplyQueued();

	if (!enableDeveloperMenu()) {
		return false;
	}

	return true;
}

void DideMod::Shutdown()
{
	EZLOGGER << "Shutting down..." << std::endl;
}

bool DideMod::validateConfig()
{
	if (m_config.General.EnableLogging) {
		if (m_config.General.LogFile.empty()) {
			EZLOGGER << "Log file path is not configured!" << std::endl;
			return false;
		}
	}

	return true;
}

uintptr_t DideMod::getBaseOfCode(uintptr_t imageBase) const
{
	auto ntHeaders = ImageNtHeader(reinterpret_cast<void*>(imageBase));
	return imageBase + ntHeaders->OptionalHeader.BaseOfCode;
}

bool DideMod::enableDeveloperMenu()
{
	if (!m_config.Features.DeveloperMenu) {
		return true;
	}

	auto patchAddress = HostAppTargetInfo::GameDll + 0xACCFFC;
	EZLOGGER << "Trying patch address: " << std::hex << patchAddress << std::endl;
	if (memcmp((const void*)patchAddress, "\x38\x1d\xe0\xf2\x7a\x00\x0f\x84\x0b\x09\x00\x00", 12) != 0) {
		EZLOGGER << "Cannot apply patch due to mismatch." << std::endl;
		return false;
	}

	DWORD oldProtect;
	if (!VirtualProtect((void*)patchAddress, 12, PAGE_EXECUTE_READWRITE, &oldProtect)) {
		EZLOGGER << "Cannot change virtual memory protection." << std::endl;
		return false;
	}

	memcpy((void*)patchAddress, "\xc7\x05\xdc\xf2\x7a\x00\x01\x00\x00\x00\x90\x90", 12);
	FlushInstructionCache(GetCurrentProcess(), (LPCVOID)patchAddress, 12);

	VirtualProtect((void*)patchAddress, 12, oldProtect, NULL);
	return true;
}

void DideMod::logConfig()
{
	EZLOGGER << "Configuration:" << std::endl;

	EZLOGGER << "  " << "General:" << std::endl;
	EZLOGGER << "    " << "EnableMod = " << m_config.General.EnableMod << std::endl;
	EZLOGGER << "    " << "EnableLogging = " << m_config.General.EnableLogging << std::endl;
	EZLOGGER << "    " << "LogFile = " << m_config.General.LogFile << std::endl;

	EZLOGGER << "  " << "Features:" << std::endl;
	EZLOGGER << "    " << "DeveloperMenu = " << m_config.Features.DeveloperMenu << std::endl;
	EZLOGGER << "    " << "CustomPak = " << m_config.Features.CustomPak << std::endl;

	EZLOGGER << "  " << "CustomPak:" << std::endl;
	for (auto it = m_config.LoadCustomPaks.begin(); it != m_config.LoadCustomPaks.end(); ++it) {
		EZLOGGER << "    " << *it << std::endl;
	}
}

void DideMod::logVersion()
{
	EZLOGGER << "Core version: " << CoreGetVersion() << std::endl;
}

void DideMod::LoadPaks()
{
	for (auto it = m_config.LoadCustomPaks.begin(); it != m_config.LoadCustomPaks.end(); ++it) {
		auto& pakPath = *it;
		EZLOGGER << "Adding custom source: " << pakPath;
		auto success = fs_add_source_original(pakPath.c_str(), FFSAddSourceFlags::Unknown09);
		EZLOGGER << " (" << (success ? std::string("OK") : std::string("error")) << ")" << std::endl;
	}
}
