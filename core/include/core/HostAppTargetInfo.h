#pragma once

#include <cstdint>

class HostAppTargetInfo
{
public:
	static uintptr_t MainExe;
	static uintptr_t GameDll;
	static uintptr_t EngineDll;
	static uintptr_t FilesystemDll;
};
