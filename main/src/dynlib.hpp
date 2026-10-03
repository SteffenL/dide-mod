#pragma once

#include <filesystem>
#include <memory>
#include <optional>

#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

class DynLib {
private:
    class Impl;

public:
    explicit DynLib(std::unique_ptr<Impl> impl);
    ~DynLib();
    DynLib& operator=(const DynLib&) = delete;
    DynLib(const DynLib&) = delete;
    DynLib& operator=(DynLib&&) noexcept;
    DynLib(DynLib&&) noexcept;

    static std::optional<DynLib> try_load(const std::filesystem::path& name);
    static std::optional<DynLib> try_attach_by_handle(void* handle);
    static std::optional<DynLib> try_attach_by_name(const std::filesystem::path& name);

    static DynLib load(const std::filesystem::path& name);
    static DynLib attach_by_handle(void* handle);
    static DynLib attach_by_name(const std::filesystem::path& name);
    static void pin_by_handle(void* handle);

    template<typename T>
    auto sym(const char* name) const {
        using fn_t = std::add_pointer_t<std::remove_pointer_t<T>>;
        return reinterpret_cast<fn_t>(sym_impl(name));
    }

    void* sym_impl(const char* name) const;
    void* handle() const noexcept;
    uintptr_t address() const noexcept;
    std::filesystem::path name() const noexcept;
    void detach() noexcept;
    void release() noexcept;
    void pin();

private:
    std::unique_ptr<Impl> m_impl;
};

#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif
