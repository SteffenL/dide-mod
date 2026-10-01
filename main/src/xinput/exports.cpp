#include "exports.hpp"
#include "../dynlib.hpp"
#include "../misc.hpp"
#include "xinput.hpp"

#include <optional>

using namespace xinput;

struct XInputWrapper {
    DynLib lib{get_system_xinput_dll_path()};
    Functions<NotNull> fn{lib};
};

namespace {
std::optional<XInputWrapper> g_wrapper;
}

void create_xinput_wrapper() { g_wrapper = XInputWrapper{}; }
void destroy_xinput_wrapper() { g_wrapper.reset(); }

extern "C" {

//
// XInput 1.3
//

DWORD WINAPI XInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState) {
    return g_wrapper->fn.XInputGetState(dwUserIndex, pState);
}

DWORD WINAPI XInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration) {
    return g_wrapper->fn.XInputSetState(dwUserIndex, pVibration);
}

DWORD WINAPI XInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities) {
    return g_wrapper->fn.XInputGetCapabilities(dwUserIndex, dwFlags, pCapabilities);
}

DWORD WINAPI XInputEnable(BOOL enable) { return g_wrapper->fn.XInputEnable(enable); }

DWORD WINAPI XInputGetDSoundAudioDeviceGuids(DWORD dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid) {
    return g_wrapper->fn.XInputGetDSoundAudioDeviceGuids(dwUserIndex, pDSoundRenderGuid, pDSoundCaptureGuid);
}

DWORD WINAPI XInputGetBatteryInformation(DWORD dwUserIndex, BYTE devType,
                                         XINPUT_BATTERY_INFORMATION* pBatteryInformation) {
    return g_wrapper->fn.XInputGetBatteryInformation(dwUserIndex, devType, pBatteryInformation);
}

DWORD WINAPI XInputGetKeystroke(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE* pKeystroke) {
    return g_wrapper->fn.XInputGetKeystroke(dwUserIndex, dwReserved, pKeystroke);
}

//
// XInput 1.3 undocumented
//

DWORD WINAPI XInputGetStateEx(DWORD dwUserIndex, XINPUT_STATE* pState) {
    return g_wrapper->fn.XInputGetStateEx(dwUserIndex, pState);
}

DWORD WINAPI XInputWaitForGuideButton(DWORD dwUserIndex, DWORD dwFlag, LPVOID pVoid) {
    return g_wrapper->fn.XInputWaitForGuideButton(dwUserIndex, dwFlag, pVoid);
}

DWORD WINAPI XInputCancelGuideButtonWait(DWORD dwUserIndex) {
    return g_wrapper->fn.XInputCancelGuideButtonWait(dwUserIndex);
}

DWORD WINAPI XInputPowerOffController(DWORD dwUserIndex) { return g_wrapper->fn.XInputPowerOffController(dwUserIndex); }

//
// XInput 1.4
//

DWORD WINAPI XInputGetAudioDeviceIds(DWORD dwUserIndex, LPWSTR pRenderDeviceId, UINT* pRenderCount,
                                     LPWSTR pCaptureDeviceId, UINT* pCaptureCount) {
    return g_wrapper->fn.XInputGetAudioDeviceIds(dwUserIndex, pRenderDeviceId, pRenderCount, pCaptureDeviceId,
                                                 pCaptureCount);
}

//
// XInput 1.4 undocumented
//

DWORD WINAPI XInputGetBaseBusInformation(DWORD dwUserIndex, XINPUT_BUSINFO* pBusinfo) {
    return g_wrapper->fn.XInputGetBaseBusInformation(dwUserIndex, pBusinfo);
}

DWORD WINAPI XInputGetCapabilitiesEx(DWORD dwUnk, DWORD dwUserIndex, DWORD dwFlags,
                                     XINPUT_CAPABILITIESEX* pCapabilitiesEx) {
    return g_wrapper->fn.XInputGetCapabilitiesEx(dwUnk, dwUserIndex, dwFlags, pCapabilitiesEx);
}
}
