#include "dynlib.hpp"
#include "misc.hpp"

#include <filesystem>
#include <format>
#include <memory>
#include <optional>

#include <windows.h>

class DynLib::Impl {
public:
    explicit Impl(NotNull<HMODULE> handle) : m_handle{handle.get()} {}
    ~Impl() { release(); }
    Impl& operator=(const Impl&) = delete;
    Impl(const Impl&) = delete;
    Impl& operator=(Impl&& other) = delete;
    Impl(Impl&& other) = delete;

    static std::unique_ptr<Impl> try_load(const std::filesystem::path& name) {
        if (auto handle{::LoadLibraryW(name.c_str())}) {
            return std::make_unique<Impl>(handle);
        }
        return nullptr;
    }

    static std::unique_ptr<Impl> try_attach_by_handle(NotNull<HMODULE> handle) {
        HMODULE handle_{};
        if (::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCWSTR>(handle.get()),
                                 &handle_)) {
            return std::make_unique<Impl>(handle_);
        }
        return nullptr;
    }

    static std::unique_ptr<Impl> try_attach_by_name(const std::filesystem::path& name) {
        HMODULE handle{};
        if (::GetModuleHandleExW(0, name.c_str(), &handle)) {
            return std::make_unique<Impl>(handle);
        }
        return nullptr;
    }

    static std::unique_ptr<Impl> load(const std::filesystem::path& name) {
        if (auto impl{try_load(name)}) {
            return impl;
        }
        throw Error{std::format("Unable to load library ({}): {}", ::GetLastError(),
                                reinterpret_cast<const char*>(name.u8string().c_str()))};
    }

    static std::unique_ptr<Impl> attach_by_handle(NotNull<HMODULE> handle) {
        if (auto impl{try_attach_by_handle(handle)}) {
            return impl;
        }
        throw Error{std::format("Unable to attach to library by handle ({})", ::GetLastError())};
    }

    static std::unique_ptr<Impl> attach_by_name(const std::filesystem::path& name) {
        if (auto impl{try_attach_by_name(name)}) {
            return impl;
        }
        throw Error{std::format("Unable to attach to library ({}): {}", ::GetLastError(),
                                reinterpret_cast<const char*>(name.u8string().c_str()))};
    }

    void* sym_impl(const char* name) const {
        if (auto fn{reinterpret_cast<void*>(::GetProcAddress(reinterpret_cast<HMODULE>(m_handle), name))}) {
            return fn;
        }
        throw Error{std::format("Function not found: {}", name)};
    }

    HMODULE handle() const noexcept { return m_handle; }
    uintptr_t address() const noexcept { return reinterpret_cast<uintptr_t>(m_handle); }

    std::filesystem::path name() const noexcept {
        std::array<wchar_t, MAX_PATH> name{};
        ::GetModuleFileNameW(m_handle, name.data(), static_cast<DWORD>(name.size()));
        return name.data();
    }

    void detach() noexcept { m_handle = nullptr; }

    void release() noexcept {
        if (m_handle) {
            ::FreeLibrary(m_handle);
            m_handle = nullptr;
        }
    }

private:
    HMODULE m_handle{};
};

DynLib::DynLib(std::unique_ptr<Impl> impl) : m_impl{std::move(impl)} {}

std::optional<DynLib> DynLib::try_load(const std::filesystem::path& name) {
    if (auto impl{Impl::try_load(name)}) {
        return std::make_optional<DynLib>(std::move(impl));
    }
    return std::nullopt;
}

std::optional<DynLib> DynLib::try_attach_by_handle(void* handle) {
    if (auto impl{Impl::try_attach_by_handle(static_cast<HMODULE>(handle))}) {
        return std::make_optional<DynLib>(std::move(impl));
    }
    return std::nullopt;
}

std::optional<DynLib> DynLib::try_attach_by_name(const std::filesystem::path& name) {
    if (auto impl{Impl::try_attach_by_name(name)}) {
        return std::make_optional<DynLib>(std::move(impl));
    }
    return std::nullopt;
}

DynLib::~DynLib() = default;
DynLib& DynLib::operator=(DynLib&&) noexcept = default;
DynLib::DynLib(DynLib&&) noexcept = default;
DynLib DynLib::load(const std::filesystem::path& name) { return DynLib{Impl::load(name)}; }
DynLib DynLib::attach_by_handle(void* handle) { return DynLib{Impl::attach_by_handle(static_cast<HMODULE>(handle))}; }
DynLib DynLib::attach_by_name(const std::filesystem::path& name) { return DynLib{Impl::attach_by_name(name)}; }
void* DynLib::sym_impl(const char* name) const { return m_impl->sym_impl(name); }
void* DynLib::handle() const noexcept { return m_impl->handle(); }
uintptr_t DynLib::address() const noexcept { return m_impl->address(); }
std::filesystem::path DynLib::name() const noexcept { return m_impl->name(); }
void DynLib::detach() noexcept { m_impl->detach(); }
void DynLib::release() noexcept { m_impl->release(); }
