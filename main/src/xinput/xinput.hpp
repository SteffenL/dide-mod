#pragma once

#include "../dynlib.hpp"

#include <filesystem>

#include <windows.h>

namespace xinput {

using XINPUT_STATE = void;
using XINPUT_VIBRATION = void;
using XINPUT_CAPABILITIES = void;
using XINPUT_CAPABILITIES = void;
using XINPUT_BATTERY_INFORMATION = void;
using XINPUT_KEYSTROKE = void;
using XINPUT_BUSINFO = void;
using XINPUT_CAPABILITIESEX = void;

// XInput 1.3
using XInputGetState_t = DWORD WINAPI(DWORD dwUserIndex, XINPUT_STATE* pState);
using XInputSetState_t = DWORD WINAPI(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);
using XInputGetCapabilities_t = DWORD WINAPI(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities);
using XInputEnable_t = DWORD WINAPI(BOOL enable);
using XInputGetDSoundAudioDeviceGuids_t = DWORD WINAPI(DWORD dwUserIndex, GUID* pDSoundRenderGuid,
                                                       GUID* pDSoundCaptureGuid);
using XInputGetBatteryInformation_t = DWORD WINAPI(DWORD dwUserIndex, BYTE devType,
                                                   XINPUT_BATTERY_INFORMATION* pBatteryInformation);
using XInputGetKeystroke_t = DWORD WINAPI(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE* pKeystroke);

// XInput 1.3 undocumented
using XInputGetStateEx_t = DWORD WINAPI(DWORD dwUserIndex, XINPUT_STATE* pState);
using XInputWaitForGuideButton_t = DWORD WINAPI(DWORD dwUserIndex, DWORD dwFlag, LPVOID pVoid);
using XInputCancelGuideButtonWait_t = DWORD WINAPI(DWORD dwUserIndex);
using XInputPowerOffController_t = DWORD WINAPI(DWORD dwUserIndex);

// XInput 1.4
using XInputGetAudioDeviceIds_t = DWORD WINAPI(DWORD dwUserIndex, LPWSTR pRenderDeviceId, UINT* pRenderCount,
                                               LPWSTR pCaptureDeviceId, UINT* pCaptureCount);

// XInput 1.4 undocumented
using XInputGetBaseBusInformation_t = DWORD WINAPI(DWORD dwUserIndex, XINPUT_BUSINFO* pBusinfo);
using XInputGetCapabilitiesEx_t = DWORD WINAPI(DWORD dwUnk, DWORD dwUserIndex, DWORD dwFlags,
                                               XINPUT_CAPABILITIESEX* pCapabilitiesEx);

template<template<typename> typename Wrapper>
struct Functions {
    Functions() = default;
    Functions(const DynLib& lib)
            : // XInput 1.3
              XInputGetState{lib.sym<XInputGetState_t>("XInputGetState")},
              XInputSetState{lib.sym<XInputSetState_t>("XInputSetState")},
              XInputGetCapabilities{lib.sym<XInputGetCapabilities_t>("XInputGetCapabilities")},
              XInputEnable{lib.sym<XInputEnable_t>("XInputEnable")},
              XInputGetDSoundAudioDeviceGuids{lib.sym<XInputGetDSoundAudioDeviceGuids_t>(
                  "XInputGetDSoundAudioDeviceGuids")},
              XInputGetBatteryInformation{lib.sym<XInputGetBatteryInformation_t>("XInputGetBatteryInformation")},
              XInputGetKeystroke{lib.sym<XInputGetKeystroke_t>("XInputGetKeystroke")},

              // XInput 1.3 undocumented
              XInputGetStateEx{lib.sym<XInputGetStateEx_t>("XInputGetStateEx")},
              XInputWaitForGuideButton{lib.sym<XInputWaitForGuideButton_t>("XInputWaitForGuideButton")},
              XInputCancelGuideButtonWait{lib.sym<XInputCancelGuideButtonWait_t>("XInputCancelGuideButtonWait")},
              XInputPowerOffController{lib.sym<XInputPowerOffController_t>("XInputPowerOffController")},

              // XInput 1.4
              XInputGetAudioDeviceIds{lib.sym<XInputGetAudioDeviceIds_t>("XInputGetAudioDeviceIds")},

              // XInput 1.4 undocumented
              XInputGetBaseBusInformation{lib.sym<XInputGetBaseBusInformation_t>("XInputGetBaseBusInformation}")},
              XInputGetCapabilitiesEx{lib.sym<XInputGetCapabilitiesEx_t>("XInputGetCapabilitiesEx")} {}

    // XInput 1.3
    Wrapper<XInputGetState_t> XInputGetState;
    Wrapper<XInputSetState_t> XInputSetState;
    Wrapper<XInputGetCapabilities_t> XInputGetCapabilities;
    Wrapper<XInputEnable_t> XInputEnable;
    Wrapper<XInputGetDSoundAudioDeviceGuids_t> XInputGetDSoundAudioDeviceGuids;
    Wrapper<XInputGetBatteryInformation_t> XInputGetBatteryInformation;
    Wrapper<XInputGetKeystroke_t> XInputGetKeystroke;

    // XInput 1.3 undocumented
    Wrapper<XInputGetStateEx_t> XInputGetStateEx;
    Wrapper<XInputWaitForGuideButton_t> XInputWaitForGuideButton;
    Wrapper<XInputCancelGuideButtonWait_t> XInputCancelGuideButtonWait;
    Wrapper<XInputPowerOffController_t> XInputPowerOffController;

    // XInput 1.4
    Wrapper<XInputGetAudioDeviceIds_t> XInputGetAudioDeviceIds;

    // XInput 1.4 undocumented
    Wrapper<XInputGetBaseBusInformation_t> XInputGetBaseBusInformation;
    Wrapper<XInputGetCapabilitiesEx_t> XInputGetCapabilitiesEx;
};

std::filesystem::path get_system_xinput_dll_path();

} // namespace xinput
