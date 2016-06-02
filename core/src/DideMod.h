#pragma once

#include "Config.h"

class DideMod
{
public:
	DideMod(Config config);
	virtual ~DideMod();
	bool Initialize();
	void Shutdown();
	void LoadPaks();

private:
	bool validateConfig();
	uintptr_t getBaseOfCode(uintptr_t imageBase) const;
	bool enableDeveloperMenu();

	Config m_config;
	void logConfig();
	void logVersion();
};
