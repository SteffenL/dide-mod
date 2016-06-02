#pragma once

#include "Config.h"
#include <string>


class ConfigFile
{
public:
	ConfigFile(const std::string& filePath);
	virtual ~ConfigFile();
	void Load(Config& config);
	void Save(const Config& config) const;

private:
	std::string m_filePath;
	void loadGeneral(Config& config);
	void loadFeatures(Config& config);
	void loadCustomPak(Config& config);
};
