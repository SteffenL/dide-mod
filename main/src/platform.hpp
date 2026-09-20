#pragma once

#include "misc.hpp"

#include <filesystem>
#include <string>

#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

void* gpa_impl(void* module, const char* name);

template<typename T>
auto gpa(NotNull<void*> dll, const char* name) {
    using fn_t = std::add_pointer_t<std::remove_pointer_t<T>>;
    if (auto fn{reinterpret_cast<fn_t>(gpa_impl(dll.get(), name))}) {
        return fn;
    } else {
        throw Error::format("Function not found: {}", name);
    }
}

#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif

uintptr_t get_base_of_code(uintptr_t image_base);
uintptr_t get_size_of_code(uintptr_t image_base);
std::filesystem::path exe_path();
std::filesystem::path exe_dir();
void msgbox_error(const std::string& text, const std::string& title);
