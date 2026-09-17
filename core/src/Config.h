#pragma once

#include <string>
#include <vector>

struct Config {
    struct {
        bool EnableMod;
        bool EnableLogging;
        std::string LogFile;
    } General;

    struct {
        bool DeveloperMenu;
        bool CustomPak;
    } Features;

    std::vector<std::string> LoadCustomPaks;
};
