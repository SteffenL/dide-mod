#include "log.hpp"

#include <filesystem>
#include <fstream>
#include <memory>

std::unique_ptr<Logger> Logger::sm_instance;

std::unique_ptr<std::ostream> Logger::make_fstream(const std::filesystem::path& file_path) {
    return std::make_unique<std::ofstream>(file_path);
}
