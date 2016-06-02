#pragma once

#include <string>

extern std::string g_ezlogger_logOutputFilePath;
#define EZLOGGER_OUTPUT_FILENAME g_ezlogger_logOutputFilePath
#include <ezlogger/ezlogger_headers.hpp>

extern void initializeLogger(const std::string& filePath, bool enableLogging);
