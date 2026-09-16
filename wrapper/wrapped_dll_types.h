#pragma once

#include <dsound.h>
#include <stdint.h>

struct OrigFunctions {
    typedef HRESULT(WINAPI* DirectSoundCreate_t)(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN pUnkOuter);
    typedef HRESULT(WINAPI* DirectSoundEnumerateA_t)(LPDSENUMCALLBACKA pDSEnumCallback, LPVOID pContext);
    typedef HRESULT(WINAPI* DirectSoundEnumerateW_t)(LPDSENUMCALLBACKW pDSEnumCallback, LPVOID pContext);

    typedef HRESULT(WINAPI* DirectSoundCaptureCreate_t)(LPCGUID pcGuidDevice, LPDIRECTSOUNDCAPTURE* ppDSC,
                                                        LPUNKNOWN pUnkOuter);
    typedef HRESULT(WINAPI* DirectSoundCaptureEnumerateA_t)(LPDSENUMCALLBACKA pDSEnumCallback, LPVOID pContext);
    typedef HRESULT(WINAPI* DirectSoundCaptureEnumerateW_t)(LPDSENUMCALLBACKW pDSEnumCallback, LPVOID pContext);

    typedef HRESULT(WINAPI* DirectSoundCreate8_t)(LPCGUID pcGuidDevice, LPDIRECTSOUND8* ppDS8, LPUNKNOWN pUnkOuter);
    typedef HRESULT(WINAPI* DirectSoundCaptureCreate8_t)(LPCGUID pcGuidDevice, LPDIRECTSOUNDCAPTURE8* ppDSC8,
                                                         LPUNKNOWN pUnkOuter);
    typedef HRESULT(WINAPI* DirectSoundFullDuplexCreate_t)(LPCGUID pcGuidCaptureDevice, LPCGUID pcGuidRenderDevice,
                                                           LPCDSCBUFFERDESC pcDSCBufferDesc,
                                                           LPCDSBUFFERDESC pcDSBufferDesc, HWND hWnd, DWORD dwLevel,
                                                           LPDIRECTSOUNDFULLDUPLEX* ppDSFD,
                                                           LPDIRECTSOUNDCAPTUREBUFFER8* ppDSCBuffer8,
                                                           LPDIRECTSOUNDBUFFER8* ppDSBuffer8, LPUNKNOWN pUnkOuter);

    typedef HRESULT(WINAPI* GetDeviceID_t)(LPCGUID pGuidSrc, LPGUID pGuidDest);

    typedef HRESULT (*DllCanUnloadNow_t)();
    typedef HRESULT (*DllGetClassObject_t)(REFCLSID rclsid, REFIID riid, LPVOID* ppv);

    DirectSoundCreate_t DirectSoundCreate;
    DirectSoundEnumerateA_t DirectSoundEnumerateA;
    DirectSoundEnumerateW_t DirectSoundEnumerateW;

    DirectSoundCaptureCreate_t DirectSoundCaptureCreate;
    DirectSoundCaptureEnumerateA_t DirectSoundCaptureEnumerateA;
    DirectSoundCaptureEnumerateW_t DirectSoundCaptureEnumerateW;

    DirectSoundCreate8_t DirectSoundCreate8;
    DirectSoundCaptureCreate8_t DirectSoundCaptureCreate8;
    DirectSoundFullDuplexCreate_t DirectSoundFullDuplexCreate;

    GetDeviceID_t GetDeviceID;

    DllCanUnloadNow_t DllCanUnloadNow;
    DllGetClassObject_t DllGetClassObject;
};
