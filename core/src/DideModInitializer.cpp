#include <core/DideModInitializer.h>
#include "DideMod.h"
#include "Config.h"
#include "ConfigFile.h"
#include <core/StandardPaths.h>


std::unique_ptr<DideMod> DideModInitializer::m_impl;


bool DideModInitializer::Initialize()
{
	if (m_impl) {
		return false;
	}

	auto exeDir = StandardPaths::GetExecutableDir();
	auto configFilePath = exeDir + "\\dide_mod.ini";

	Config config;
	ConfigFile configFile(configFilePath);
	configFile.Load(config);

	m_impl.reset(new DideMod(config));
	if (!m_impl->Initialize()) {
		return false;
	}

	return true;
}

void DideModInitializer::Shutdown()
{
	if (!m_impl) {
		return;
	}

	m_impl->Shutdown();
	m_impl.reset();
}
