#include "xinput.hpp"

#include <filesystem>

#include <dsound.h>
#include <windows.h>

namespace xinput {

std::filesystem::path get_system_xinput_dll_path() {
    wchar_t system_dir[MAX_PATH]{};
    GetSystemDirectoryW(system_dir, MAX_PATH);
    return std::filesystem::path{system_dir} / L"xinput1_3.dll";
}

} // namespace dsound
