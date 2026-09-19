#pragma once

#include "misc.hpp"

#include <filesystem>
#include <string>

#include <windows.h>

template<typename T>
auto gpa(NotNull<HMODULE> dll, const char* name) {
    using fn_t = std::add_pointer_t<std::remove_pointer_t<T>>;
    if (auto fn{reinterpret_cast<fn_t>(::GetProcAddress(dll.get(), name))}) {
        return fn;
    } else {
        throw Error::format("Function not found: {}", name);
    }
}

uintptr_t get_base_of_code(uintptr_t image_base);
uintptr_t get_size_of_code(uintptr_t image_base);
std::filesystem::path exe_path();
std::filesystem::path exe_dir();
void msgbox_error(const std::string& text, const std::string& title);
