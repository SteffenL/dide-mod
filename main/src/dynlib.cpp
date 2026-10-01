#include "dynlib.hpp"
#include "misc.hpp"

#include <filesystem>
#include <format>
#include <utility>

#include <windows.h>

namespace {
void* load_library(const std::filesystem::path& name) {
    const auto handle{::LoadLibraryW(name.c_str())};
    if (!handle) {
        throw Error{std::format("Unable to load library: {}", reinterpret_cast<const char*>(name.u8string().c_str()))};
    }
    return reinterpret_cast<void*>(handle);
}

void* find_loaded_library(const std::filesystem::path& name) {
    HMODULE handle{};
    if (!::GetModuleHandleExW(0, name.c_str(), &handle)) {
        throw Error{std::format("Unable to find loaded library: {}",
                                reinterpret_cast<const char*>(name.u8string().c_str()))};
    }
    return reinterpret_cast<void*>(handle);
}
} // namespace

DynLib::DynLib(std::filesystem::path name, NotNull<void*> handle) : m_handle{handle.get()}, m_name{std::move(name)} {}
DynLib::DynLib(std::filesystem::path name) : m_handle{load_library(name)}, m_name{std::move(name)} {}

DynLib::~DynLib() {
    if (m_handle) {
        ::FreeLibrary(reinterpret_cast<HMODULE>(m_handle));
    }
}

DynLib DynLib::from_loaded(std::filesystem::path name) {
    auto lib{find_loaded_library(name)};
    return DynLib{std::move(name), std::move(lib)};
}

DynLib& DynLib::operator=(DynLib&& other) noexcept {
    if (this != &other) {
        m_handle = std::exchange(other.m_handle, nullptr);
    }
    return *this;
}

DynLib::DynLib(DynLib&& other) noexcept : m_handle{std::exchange(other.m_handle, nullptr)} {}

void* DynLib::sym_impl(const char* name) const {
    return reinterpret_cast<void*>(::GetProcAddress(reinterpret_cast<HMODULE>(m_handle), name));
}

void* DynLib::handle() const noexcept { return m_handle; }
uintptr_t DynLib::address() const noexcept { return reinterpret_cast<uintptr_t>(m_handle); }
