#include "misc.hpp"

#include <filesystem>
#include <string>
#include <vector>

#include <nowide/convert.hpp>

#include <windows.h>
// Must come after windows.h
#include <dbghelp.h>

std::filesystem::path exe_path() {
    std::vector<wchar_t> buffer(MAX_PATH, '\0');
    while (true) {
        const std::size_t size{::GetModuleFileNameW(NULL, buffer.data(), static_cast<DWORD>(buffer.size()))};
        if (size == 0) {
            throw Error{"Unable to get exe path"};
        }
        if (size >= buffer.size()) {
            buffer.resize(size);
            continue;
        }
        return std::wstring_view{buffer.data(), size};
    };
}

uintptr_t get_base_of_code(uintptr_t image_base) {
    const auto nt_headers{::ImageNtHeader(reinterpret_cast<void*>(image_base))};
    return image_base + nt_headers->OptionalHeader.BaseOfCode;
}

uintptr_t get_size_of_code(uintptr_t image_base) {
    const auto nt_headers{::ImageNtHeader(reinterpret_cast<void*>(image_base))};
    return nt_headers->OptionalHeader.SizeOfCode;
}

std::filesystem::path exe_dir() { return exe_path().parent_path(); }

void msgbox_error(const std::string& text, const std::string& title) {
    ::MessageBoxW(nullptr, nowide::widen(text).c_str(), nowide::widen(title).c_str(), MB_TASKMODAL | MB_ICONERROR);
}
