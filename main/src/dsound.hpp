#pragma once

#include "misc.hpp"

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

struct DSoundFunctions {
    NotNull<DirectSoundCreate_t> DirectSoundCreate;
    NotNull<DirectSoundEnumerateA_t> DirectSoundEnumerateA;
    NotNull<DirectSoundEnumerateW_t> DirectSoundEnumerateW;
    NotNull<DirectSoundCaptureCreate_t> DirectSoundCaptureCreate;
    NotNull<DirectSoundCaptureEnumerateA_t> DirectSoundCaptureEnumerateA;
    NotNull<DirectSoundCaptureEnumerateW_t> DirectSoundCaptureEnumerateW;
    NotNull<DirectSoundCreate8_t> DirectSoundCreate8;
    NotNull<DirectSoundCaptureCreate8_t> DirectSoundCaptureCreate8;
    NotNull<DirectSoundFullDuplexCreate_t> DirectSoundFullDuplexCreate;
    NotNull<GetDeviceID_t> GetDeviceID;
    NotNull<DllCanUnloadNow_t> DllCanUnloadNow;
    NotNull<DllGetClassObject_t> DllGetClassObject;
};

Result<DSoundFunctions> load_dsound_functions(NotNull<HMODULE> dll);
std::filesystem::path get_system_dsound_dll_path();

} // namespace dsound
