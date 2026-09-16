#pragma once

#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <type_traits>

class Logger final {
public:
    using manip_fn_t = std::ostream& (*)(std::ostream&);

    template<typename T>
    Logger& operator<<(T&& value) {
        if (m_enabled && m_os) {
            std::lock_guard<std::mutex> lock{m_mutex};
            *m_os << std::forward<T>(value);
        }
        return *this;
    }

    Logger& operator<<(manip_fn_t manip) {
        if (m_enabled && m_os) {
            std::lock_guard<std::mutex> lock{m_mutex};
            *m_os << manip;
        }
        return *this;
    }

    static void init(const std::string& filePath, bool enableLogging);
    static std::unique_ptr<Logger> create(const std::string& filePath, bool enableLogging);
    static Logger& get();
    static void set(std::unique_ptr<Logger> instance);

private:
    Logger(const std::string& filePath, bool enableLogging);
    static std::unique_ptr<Logger>& instance();

    bool m_enabled{};
    std::unique_ptr<std::ostream> m_os;
    mutable std::mutex m_mutex;
};

#define EZLOGGER Logger::get()
