#pragma once

#include <memory>

class DideMod;

class DideModInitializer
{
public:
	static bool Initialize();
	static void Shutdown();

private:
	static std::unique_ptr<DideMod> m_impl;
};
