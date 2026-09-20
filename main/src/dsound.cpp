#include "dsound.hpp"
#include "misc.hpp"
#include "platform.hpp"

#include <filesystem>

#include <dsound.h>
#include <windows.h>

namespace dsound {

DSoundFunctions load_dsound_functions(NotNull<HMODULE> dll) {
    try {
        return DSoundFunctions{
            .DirectSoundCreate = gpa<DirectSoundCreate_t>(dll, "DirectSoundCreate"),
            .DirectSoundEnumerateA = gpa<DirectSoundEnumerateA_t>(dll, "DirectSoundEnumerateA"),
            .DirectSoundEnumerateW = gpa<DirectSoundEnumerateW_t>(dll, "DirectSoundEnumerateW"),
            .DirectSoundCaptureCreate = gpa<DirectSoundCaptureCreate_t>(dll, "DirectSoundCaptureCreate"),
            .DirectSoundCaptureEnumerateA = gpa<DirectSoundCaptureEnumerateA_t>(dll, "DirectSoundCaptureEnumerateA"),
            .DirectSoundCaptureEnumerateW = gpa<DirectSoundCaptureEnumerateW_t>(dll, "DirectSoundCaptureEnumerateW"),
            .DirectSoundCreate8 = gpa<DirectSoundCreate8_t>(dll, "DirectSoundCreate8"),
            .DirectSoundCaptureCreate8 = gpa<DirectSoundCaptureCreate8_t>(dll, "DirectSoundCaptureCreate8"),
            .DirectSoundFullDuplexCreate = gpa<DirectSoundFullDuplexCreate_t>(dll, "DirectSoundFullDuplexCreate"),
            .GetDeviceID = gpa<GetDeviceID_t>(dll, "GetDeviceID"),
            .DllCanUnloadNow = gpa<DllCanUnloadNow_t>(dll, "DllCanUnloadNow"),
            .DllGetClassObject = gpa<DllGetClassObject_t>(dll, "DllGetClassObject"),
        };
    } catch (const std::exception& ex) {
        throw Error::format("Failed to load dsound function(s): {}", ex.what());
    }
}

std::filesystem::path get_system_dsound_dll_path() {
    wchar_t system_dir[MAX_PATH]{};
    GetSystemDirectoryW(system_dir, MAX_PATH);
    return std::filesystem::path{system_dir} / L"DSOUND.DLL";
}

} // namespace dsound
