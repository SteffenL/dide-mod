#include <core/Logging.h>
#include <nowide/fstream.hpp>

void Logger::init(const std::string& filePath, bool enableLogging) { set(create(filePath, enableLogging)); }

std::unique_ptr<Logger> Logger::create(const std::string& filePath, bool enableLogging) {
    return std::unique_ptr<Logger>{new Logger{filePath, enableLogging}};
}

Logger& Logger::get() { return *instance(); }
void Logger::set(std::unique_ptr<Logger> instance) { Logger::instance() = std::move(instance); }

Logger::Logger(const std::string& filePath, bool enableLogging)
        : m_enabled{enableLogging}, m_os{new nowide::ofstream{filePath}} {}

std::unique_ptr<Logger>& Logger::instance() {
    static std::unique_ptr<Logger> instance;
    return instance;
}
