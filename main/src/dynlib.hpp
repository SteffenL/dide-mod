#pragma once

#include "misc.hpp"

#include <filesystem>
#include <format>

#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

class DynLib {
public:
    explicit DynLib(const std::filesystem::path& name);
    ~DynLib();
    DynLib& operator=(const DynLib&) = delete;
    DynLib(const DynLib&) = delete;
    DynLib& operator=(DynLib&&) noexcept;
    DynLib(DynLib&&) noexcept;
    static DynLib attach(void* existing_handle, bool unowned = false);
    static DynLib from_loaded(const std::filesystem::path& name, bool unowned = false);
    static bool is_loaded(const std::filesystem::path& name) noexcept;

    template<typename T>
    auto sym(const char* name) const {
        using fn_t = std::add_pointer_t<std::remove_pointer_t<T>>;
        if (auto fn{reinterpret_cast<fn_t>(sym_impl(name))}) {
            return fn;
        } else {
            throw Error{std::format("Function not found: {}", name)};
        }
    }

    void* sym_impl(const char* name) const;
    void* handle() const noexcept;
    uintptr_t address() const noexcept;
    const std::filesystem::path& name() const noexcept;
    void release() noexcept;

private:
    explicit DynLib(const std::filesystem::path& name, NotNull<void*> module, bool unowned = false);

    void* m_handle{};
    std::filesystem::path m_name;
    bool m_unowned{};
};

#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif
