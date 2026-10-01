#include "dsound.hpp"

#include <filesystem>

#include <dsound.h>
#include <windows.h>

namespace dsound {

std::filesystem::path get_system_dsound_dll_path() {
    wchar_t system_dir[MAX_PATH]{};
    GetSystemDirectoryW(system_dir, MAX_PATH);
    return std::filesystem::path{system_dir} / L"DSOUND.DLL";
}

} // namespace dsound
