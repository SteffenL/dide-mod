#include "dynlib.hpp"
#include "misc.hpp"

#include <filesystem>
#include <format>
#include <utility>

#include <windows.h>

namespace {
void* load_library(const std::filesystem::path& name) {
    auto handle{::LoadLibraryW(name.c_str())};
    if (!handle) {
        throw Error{std::format("Unable to load library ({}): {}", ::GetLastError(),
                                reinterpret_cast<const char*>(name.u8string().c_str()))};
    }
    return reinterpret_cast<void*>(handle);
}

void* find_loaded_library_unchecked(const std::filesystem::path& name, bool unowned = false) noexcept {
    HMODULE handle{};
    if (!::GetModuleHandleExW(unowned ? GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT : 0, name.c_str(), &handle)) {
        return nullptr;
    }
    return reinterpret_cast<void*>(handle);
}

void* find_loaded_library(const std::filesystem::path& name, bool unowned = false) {
    if (auto* handle{find_loaded_library_unchecked(name, unowned)}) {
        return handle;
    }
    throw Error{std::format("Unable to find loaded library ({}): {}", ::GetLastError(),
                            reinterpret_cast<const char*>(name.u8string().c_str()))};
}
} // namespace

DynLib::DynLib(const std::filesystem::path& name, NotNull<void*> handle, bool unowned)
        : m_handle{handle.get()}, m_name{name}, m_unowned{unowned} {}

DynLib::DynLib(const std::filesystem::path& name) : m_handle{load_library(name)}, m_name{name} {}
DynLib::~DynLib() { release(); }

DynLib DynLib::from_loaded(const std::filesystem::path& name, bool unowned) {
    return DynLib{name, find_loaded_library(name, unowned), unowned};
}

bool DynLib::is_loaded(const std::filesystem::path& name) noexcept {
    return !!find_loaded_library_unchecked(name, true);
}

DynLib& DynLib::operator=(DynLib&& other) noexcept {
    if (this != &other) {
        release();
        m_handle = std::exchange(other.m_handle, nullptr);
        m_name = std::move(other.m_name);
        m_unowned = std::exchange(other.m_unowned, true);
    }
    return *this;
}

DynLib::DynLib(DynLib&& other) noexcept
        : m_handle{std::exchange(other.m_handle, nullptr)}, m_name{std::move(other.m_name)},
          m_unowned{std::exchange(other.m_unowned, true)} {}

void* DynLib::sym_impl(const char* name) const {
    return reinterpret_cast<void*>(::GetProcAddress(reinterpret_cast<HMODULE>(m_handle), name));
}

void* DynLib::handle() const noexcept { return m_handle; }
uintptr_t DynLib::address() const noexcept { return reinterpret_cast<uintptr_t>(m_handle); }
const std::filesystem::path& DynLib::name() const noexcept { return m_name; };

void DynLib::release() noexcept {
    if (m_handle && !m_unowned) {
        ::FreeLibrary(static_cast<HMODULE>(m_handle));
    }
}
