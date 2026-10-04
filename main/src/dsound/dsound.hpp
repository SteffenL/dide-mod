#pragma once

#include "../dynlib.hpp"

#include <filesystem>

#include <dsound.h>

namespace dsound {

using DirectSoundCreate_t = HRESULT WINAPI(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN pUnkOuter);
using DirectSoundEnumerateA_t = HRESULT WINAPI(LPDSENUMCALLBACKA pDSEnumCallback, LPVOID pContext);
using DirectSoundEnumerateW_t = HRESULT WINAPI(LPDSENUMCALLBACKW pDSEnumCallback, LPVOID pContext);
using DirectSoundCaptureCreate_t = HRESULT WINAPI(LPCGUID pcGuidDevice, LPDIRECTSOUNDCAPTURE* ppDSC,
                                                  LPUNKNOWN pUnkOuter);
using DirectSoundCaptureEnumerateA_t = HRESULT WINAPI(LPDSENUMCALLBACKA pDSEnumCallback, LPVOID pContext);
using DirectSoundCaptureEnumerateW_t = HRESULT WINAPI(LPDSENUMCALLBACKW pDSEnumCallback, LPVOID pContext);
using DirectSoundCreate8_t = HRESULT WINAPI(LPCGUID pcGuidDevice, LPDIRECTSOUND8* ppDS8, LPUNKNOWN pUnkOuter);
using DirectSoundCaptureCreate8_t = HRESULT WINAPI(LPCGUID pcGuidDevice, LPDIRECTSOUNDCAPTURE8* ppDSC8,
                                                   LPUNKNOWN pUnkOuter);
using DirectSoundFullDuplexCreate_t = HRESULT WINAPI(LPCGUID pcGuidCaptureDevice, LPCGUID pcGuidRenderDevice,
                                                     LPCDSCBUFFERDESC pcDSCBufferDesc, LPCDSBUFFERDESC pcDSBufferDesc,
                                                     HWND hWnd, DWORD dwLevel, LPDIRECTSOUNDFULLDUPLEX* ppDSFD,
                                                     LPDIRECTSOUNDCAPTUREBUFFER8* ppDSCBuffer8,
                                                     LPDIRECTSOUNDBUFFER8* ppDSBuffer8, LPUNKNOWN pUnkOuter);
using GetDeviceID_t = HRESULT WINAPI(LPCGUID pGuidSrc, LPGUID pGuidDest);
using DllCanUnloadNow_t = HRESULT();
using DllGetClassObject_t = HRESULT(REFCLSID rclsid, REFIID riid, LPVOID* ppv);

template<template<typename> typename Wrapper>
struct Functions {
    Functions() = default;
    Functions(const DynLib& lib)
            : DirectSoundCreate{lib.sym<DirectSoundCreate_t>("DirectSoundCreate")},
              DirectSoundEnumerateA{lib.sym<DirectSoundEnumerateA_t>("DirectSoundEnumerateA")},
              DirectSoundEnumerateW{lib.sym<DirectSoundEnumerateW_t>("DirectSoundEnumerateW")},
              DirectSoundCaptureCreate{lib.sym<DirectSoundCaptureCreate_t>("DirectSoundCaptureCreate")},
              DirectSoundCaptureEnumerateA{lib.sym<DirectSoundCaptureEnumerateA_t>("DirectSoundCaptureEnumerateA")},
              DirectSoundCaptureEnumerateW{lib.sym<DirectSoundCaptureEnumerateW_t>("DirectSoundCaptureEnumerateW")},
              DirectSoundCreate8{lib.sym<DirectSoundCreate8_t>("DirectSoundCreate8")},
              DirectSoundCaptureCreate8{lib.sym<DirectSoundCaptureCreate8_t>("DirectSoundCaptureCreate8")},
              DirectSoundFullDuplexCreate{lib.sym<DirectSoundFullDuplexCreate_t>("DirectSoundFullDuplexCreate")},
              GetDeviceID{lib.sym<GetDeviceID_t>("GetDeviceID")},
              DllCanUnloadNow{lib.sym<DllCanUnloadNow_t>("DllCanUnloadNow")},
              DllGetClassObject{lib.sym<DllGetClassObject_t>("DllGetClassObject")} {}

    Wrapper<DirectSoundCreate_t> DirectSoundCreate;
    Wrapper<DirectSoundEnumerateA_t> DirectSoundEnumerateA;
    Wrapper<DirectSoundEnumerateW_t> DirectSoundEnumerateW;
    Wrapper<DirectSoundCaptureCreate_t> DirectSoundCaptureCreate;
    Wrapper<DirectSoundCaptureEnumerateA_t> DirectSoundCaptureEnumerateA;
    Wrapper<DirectSoundCaptureEnumerateW_t> DirectSoundCaptureEnumerateW;
    Wrapper<DirectSoundCreate8_t> DirectSoundCreate8;
    Wrapper<DirectSoundCaptureCreate8_t> DirectSoundCaptureCreate8;
    Wrapper<DirectSoundFullDuplexCreate_t> DirectSoundFullDuplexCreate;
    Wrapper<GetDeviceID_t> GetDeviceID;
    Wrapper<DllCanUnloadNow_t> DllCanUnloadNow;
    Wrapper<DllGetClassObject_t> DllGetClassObject;
};

std::filesystem::path get_system_dsound_dll_path();

} // namespace dsound
