#pragma once

#include "wrapped_dll_types.h"
#include <stdint.h>

// Exports

extern "C" {

__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundCreate(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter);
__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundEnumerateA(LPDSENUMCALLBACKA pDSEnumCallback, LPVOID pContext);
__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundEnumerateW(LPDSENUMCALLBACKW pDSEnumCallback, LPVOID pContext);

__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundCaptureCreate(LPCGUID pcGuidDevice, LPDIRECTSOUNDCAPTURE *ppDSC, LPUNKNOWN pUnkOuter);
__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundCaptureEnumerateA(LPDSENUMCALLBACKA pDSEnumCallback, LPVOID pContext);
__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundCaptureEnumerateW(LPDSENUMCALLBACKW pDSEnumCallback, LPVOID pContext);

__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundCreate8(LPCGUID pcGuidDevice, LPDIRECTSOUND8 *ppDS8, LPUNKNOWN pUnkOuter);
__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundCaptureCreate8(LPCGUID pcGuidDevice, LPDIRECTSOUNDCAPTURE8 *ppDSC8, LPUNKNOWN pUnkOuter);
__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundFullDuplexCreate(
	LPCGUID pcGuidCaptureDevice,
	LPCGUID pcGuidRenderDevice,
	LPCDSCBUFFERDESC pcDSCBufferDesc,
	LPCDSBUFFERDESC pcDSBufferDesc,
	HWND hWnd,
	DWORD dwLevel,
	LPDIRECTSOUNDFULLDUPLEX* ppDSFD,
	LPDIRECTSOUNDCAPTUREBUFFER8 *ppDSCBuffer8,
	LPDIRECTSOUNDBUFFER8 *ppDSBuffer8,
	LPUNKNOWN pUnkOuter);

__declspec(dllexport) HRESULT WINAPI wrapper_GetDeviceID(LPCGUID pGuidSrc, LPGUID pGuidDest);

__declspec(dllexport) HRESULT __stdcall wrapper_DllCanUnloadNow();
__declspec(dllexport) HRESULT __stdcall wrapper_DllGetClassObject();

}
