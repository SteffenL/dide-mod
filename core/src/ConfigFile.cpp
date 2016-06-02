#include "ConfigFile.h"
#include <vector>
#include <nowide/convert.hpp>
#include <Windows.h>
#include <core/ModMsgBox.h>
#include <core/Logging.h>


ConfigFile::ConfigFile(const std::string& filePath)
	: m_filePath(filePath)
{
}

ConfigFile::~ConfigFile()
{
}

void ConfigFile::Load(Config& config)
{
	loadGeneral(config);
	loadFeatures(config);
	loadCustomPak(config);
}

void ConfigFile::Save(const Config& config) const
{
	throw std::logic_error("Not implemented");
}

void ConfigFile::loadGeneral(Config& config)
{
	auto filePathW = nowide::widen(m_filePath);
	auto filePath = filePathW.c_str();

	const wchar_t* sectionName = L"General";

	{
		std::vector<wchar_t> valueStr(1024, 0);
		GetPrivateProfileStringW(sectionName, L"EnableMod", NULL, valueStr.data(), static_cast<DWORD>(valueStr.size() / sizeof(valueStr[0])), filePath);
		unsigned int value = 0;
		swscanf_s(valueStr.data(), L"%u", &value);
		config.General.EnableMod = value ? true : false;
	}

	{
		std::vector<wchar_t> valueStr(1024, 0);
		GetPrivateProfileStringW(sectionName, L"EnableLogging", NULL, valueStr.data(), static_cast<DWORD>(valueStr.size() / sizeof(valueStr[0])), filePath);
		unsigned int value = 0;
		swscanf_s(valueStr.data(), L"%u", &value);
		config.General.EnableLogging = value ? true : false;
	}

	{
		std::vector<wchar_t> valueStr(1024, 0);
		GetPrivateProfileStringW(sectionName, L"LogFile", L"dide_mod.log", valueStr.data(), static_cast<DWORD>(valueStr.size() / sizeof(valueStr[0])), filePath);
		config.General.LogFile = nowide::narrow(valueStr.data());
	}
}

void ConfigFile::loadFeatures(Config& config)
{
	auto filePathW = nowide::widen(m_filePath);
	auto filePath = filePathW.c_str();

	const wchar_t* sectionName = L"Features";

	{
		std::vector<wchar_t> valueStr(1024, 0);
		GetPrivateProfileStringW(sectionName, L"DeveloperMenu", NULL, valueStr.data(), static_cast<DWORD>(valueStr.size() / sizeof(valueStr[0])), filePath);
		unsigned int value = 0;
		swscanf_s(valueStr.data(), L"%u", &value);
		config.Features.DeveloperMenu = value ? true : false;
	}

	{
		std::vector<wchar_t> valueStr(1024, 0);
		GetPrivateProfileStringW(sectionName, L"CustomPak", NULL, valueStr.data(), static_cast<DWORD>(valueStr.size() / sizeof(valueStr[0])), filePath);
		unsigned int value = 0;
		swscanf_s(valueStr.data(), L"%u", &value);
		config.Features.CustomPak = value ? true : false;
	}
}

void ConfigFile::loadCustomPak(Config& config)
{
	if (!config.Features.CustomPak) {
		return;
	}

	auto filePathW = nowide::widen(m_filePath);
	auto filePath = filePathW.c_str();

	const wchar_t* sectionName = L"CustomPak";

	{
		std::vector<wchar_t> keyNamesBuffer(1024, 0);
		GetPrivateProfileStringW(sectionName, NULL, NULL, keyNamesBuffer.data(), static_cast<DWORD>(keyNamesBuffer.size() / sizeof(keyNamesBuffer[0])), filePath);

		for (wchar_t* keyName = keyNamesBuffer.data(); *keyName != L'\0'; keyName += wcslen(keyName) + 1) {
			// Line comments (;) are automatically ignored by the Windows API function
			std::string pakPath = nowide::narrow(keyName);
			if (pakPath.empty()) {
				// Ignore
				continue;
			}

			std::vector<wchar_t> pakEnabledStr(1024, 0);
			GetPrivateProfileStringW(sectionName, keyName, NULL, pakEnabledStr.data(), static_cast<DWORD>(pakEnabledStr.size() / sizeof(pakEnabledStr[0])), filePath);
			unsigned int pakEnabled = 0;
			swscanf_s(pakEnabledStr.data(), L"%u", &pakEnabled);
			if (!pakEnabled) {
				// Ignore
				continue;
			}

			config.LoadCustomPaks.emplace_back(pakPath);
		}
	}
}
