#include <core/Logging.h>

std::string g_ezlogger_logOutputFilePath;

void initializeLogger(const std::string& filePath, bool enableLogging)
{
	axter::ezlogger<>::set_verbosity_level_tolerance(enableLogging ? axter::log_default_verbosity_level : axter::log_verbosity_not_set);
	g_ezlogger_logOutputFilePath = filePath;
}
