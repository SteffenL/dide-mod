#pragma once

#include "misc.hpp"

#include <filesystem>
#include <format>
#include <iosfwd>
#include <memory>
#include <mutex>

class Logger final {
    struct private_t {};

public:
    using manip_fn_t = std::ostream& (*)(std::ostream&);

    Logger(private_t, const std::filesystem::path& file_path, bool enable)
            : m_enabled{enable}, m_os{make_fstream(file_path)} {}

    template<typename T>
    Logger& operator<<(T&& value) {
        if (m_enabled && m_os) {
            std::scoped_lock lock{m_mutex};
            *m_os << std::forward<T>(value);
        }
        return *this;
    }

    Logger& operator<<(manip_fn_t manip) {
        if (m_enabled && m_os) {
            std::scoped_lock lock{m_mutex};
            *m_os << manip;
        }
        return *this;
    }

    template<typename... Args>
    void print(std::format_string<Args...> format_str, Args&&... args) {
        if (m_enabled && m_os) {
            std::scoped_lock lock{m_mutex};
            *m_os << std::format(std::move(format_str), std::forward<Args>(args)...);
        };
    }

    template<typename... Args>
    void println(std::format_string<Args...> format_str, Args&&... args) {
        if (m_enabled && m_os) {
            std::scoped_lock lock{m_mutex};
            *m_os << std::format(std::move(format_str), std::forward<Args>(args)...) << '\n';
            if (!m_in_tx) {
                m_os->flush();
            }
        };
    }

    template<typename Callable>
    auto tx(Callable callable) {
        struct Guard final {
            Logger* self{};
            explicit Guard(Logger* self) noexcept : self{self} { self->m_in_tx = true; }
            ~Guard() {
                self->m_in_tx = false;
                self->m_os->flush();
            }
        };
        std::scoped_lock lock{m_mutex};
        Guard guard{this};
        return callable();
    }

    static void init(const std::filesystem::path& file_path, bool enable) { set(create(file_path, enable)); }

    static std::unique_ptr<Logger> create(const std::filesystem::path& file_path, bool enable) {
        return std::make_unique<Logger>(private_t{}, file_path, enable);
    }

    static Logger& get() { return *sm_instance; }
    static void set(std::unique_ptr<Logger> instance) { sm_instance = std::move(instance); }

private:
    static std::unique_ptr<std::ostream> make_fstream(const std::filesystem::path& file_path);

    bool m_enabled{};
    std::unique_ptr<std::ostream> m_os;
    mutable std::recursive_mutex m_mutex;
    static std::unique_ptr<Logger> sm_instance;
    bool m_in_tx{};
};

#define LOG_TX(...) Logger::get().tx(__VA_ARGS__)
#define LOG_PARTIAL(...) Logger::get().print(__VA_ARGS__)
#define LOG(...) Logger::get().println(__VA_ARGS__)

template<typename Callable>
bool invoke_and_log_exception(Callable callable) noexcept {
    if (auto eptr{catch_exception(std::forward<Callable>(callable))}) {
        try {
            std::rethrow_exception(eptr);
        } catch (const Error& ex) {
            const auto& loc{ex.where()};
            LOG("Error: {}\n  in: {}\n  at: {}:{}:{}\n", ex.what(), loc.function_name(), loc.file_name(), loc.line(),
                loc.column());
            return true;
        } catch (const std::exception& ex) {
            LOG("Error: {}", ex.what());
            return true;
        } catch (...) {
            LOG("An unknown error occurred.");
            return true;
        }
    }
    return false;
}
