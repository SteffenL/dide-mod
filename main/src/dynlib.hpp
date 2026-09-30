#pragma once

#include "misc.hpp"

#include <filesystem>

#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

class DynLib {
public:
    explicit DynLib(std::filesystem::path name, NotNull<void*> module);
    explicit DynLib(std::filesystem::path name);
    ~DynLib();
    DynLib& operator=(const DynLib&) = delete;
    DynLib(const DynLib&) = delete;
    DynLib& operator=(DynLib&&) noexcept;
    DynLib(DynLib&&) noexcept;
    static DynLib from_loaded(std::filesystem::path name);

    template<typename T>
    auto sym(const char* name) const {
        using fn_t = std::add_pointer_t<std::remove_pointer_t<T>>;
        if (auto fn{reinterpret_cast<fn_t>(sym_impl(name))}) {
            return fn;
        } else {
            throw Error::format("Function not found: {}", name);
        }
    }

    void* sym_impl(const char* name) const;
    void* handle() const noexcept;
    uintptr_t address() const noexcept;
    const std::filesystem::path& name() const noexcept { return m_name; }

private:
    void* m_handle{};
    std::filesystem::path m_name;
};

#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif
