#include "../dynlib.hpp"
#include "../misc.hpp"
#include "xinput.hpp"

using namespace xinput;

struct XInputWrapper {
    DynLib lib{DynLib::load(get_system_xinput_dll_path())};
    Functions<NotNull> fn{lib};
};

namespace {
XInputWrapper& wrapper() {
    static XInputWrapper instance;
    return instance;
}
} // namespace

extern "C" {

//
// XInput 1.3
//

DWORD WINAPI XInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState) {
    return wrapper().fn.XInputGetState(dwUserIndex, pState);
}

DWORD WINAPI XInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration) {
    return wrapper().fn.XInputSetState(dwUserIndex, pVibration);
}

DWORD WINAPI XInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities) {
    return wrapper().fn.XInputGetCapabilities(dwUserIndex, dwFlags, pCapabilities);
}

DWORD WINAPI XInputEnable(BOOL enable) { return wrapper().fn.XInputEnable(enable); }

DWORD WINAPI XInputGetDSoundAudioDeviceGuids(DWORD dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid) {
    return wrapper().fn.XInputGetDSoundAudioDeviceGuids(dwUserIndex, pDSoundRenderGuid, pDSoundCaptureGuid);
}

DWORD WINAPI XInputGetBatteryInformation(DWORD dwUserIndex, BYTE devType,
                                         XINPUT_BATTERY_INFORMATION* pBatteryInformation) {
    return wrapper().fn.XInputGetBatteryInformation(dwUserIndex, devType, pBatteryInformation);
}

DWORD WINAPI XInputGetKeystroke(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE* pKeystroke) {
    return wrapper().fn.XInputGetKeystroke(dwUserIndex, dwReserved, pKeystroke);
}

//
// XInput 1.3 undocumented
//

DWORD WINAPI XInputGetStateEx(DWORD dwUserIndex, XINPUT_STATE* pState) {
    return wrapper().fn.XInputGetStateEx(dwUserIndex, pState);
}

DWORD WINAPI XInputWaitForGuideButton(DWORD dwUserIndex, DWORD dwFlag, LPVOID pVoid) {
    return wrapper().fn.XInputWaitForGuideButton(dwUserIndex, dwFlag, pVoid);
}

DWORD WINAPI XInputCancelGuideButtonWait(DWORD dwUserIndex) {
    return wrapper().fn.XInputCancelGuideButtonWait(dwUserIndex);
}

DWORD WINAPI XInputPowerOffController(DWORD dwUserIndex) { return wrapper().fn.XInputPowerOffController(dwUserIndex); }

//
// XInput 1.4
//

DWORD WINAPI XInputGetAudioDeviceIds(DWORD dwUserIndex, LPWSTR pRenderDeviceId, UINT* pRenderCount,
                                     LPWSTR pCaptureDeviceId, UINT* pCaptureCount) {
    return wrapper().fn.XInputGetAudioDeviceIds(dwUserIndex, pRenderDeviceId, pRenderCount, pCaptureDeviceId,
                                                pCaptureCount);
}

//
// XInput 1.4 undocumented
//

DWORD WINAPI XInputGetBaseBusInformation(DWORD dwUserIndex, XINPUT_BUSINFO* pBusinfo) {
    return wrapper().fn.XInputGetBaseBusInformation(dwUserIndex, pBusinfo);
}

DWORD WINAPI XInputGetCapabilitiesEx(DWORD dwUnk, DWORD dwUserIndex, DWORD dwFlags,
                                     XINPUT_CAPABILITIESEX* pCapabilitiesEx) {
    return wrapper().fn.XInputGetCapabilitiesEx(dwUnk, dwUserIndex, dwFlags, pCapabilitiesEx);
}
}
