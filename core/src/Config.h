#pragma once

#include <string>
#include <vector>


class Config
{
public:
	Config();
	virtual ~Config();

public:
	struct 
	{
		bool EnableMod;
		bool EnableLogging;
		std::string LogFile;
	} General;

	struct 
	{
		bool DeveloperMenu;
		bool CustomPak;
	} Features;

	std::vector<std::string> LoadCustomPaks;
};
