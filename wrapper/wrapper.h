#pragma once

#include "wrapped_dll_types.h"
#include <stdint.h>

// Exports

__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundCreate(__in_opt LPCGUID pcGuidDevice, __deref_out LPDIRECTSOUND *ppDS, __null LPUNKNOWN pUnkOuter);
__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundEnumerateA(__in LPDSENUMCALLBACKA pDSEnumCallback, __in_opt LPVOID pContext);
__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundEnumerateW(__in LPDSENUMCALLBACKW pDSEnumCallback, __in_opt LPVOID pContext);

__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundCaptureCreate(__in_opt LPCGUID pcGuidDevice, __deref_out LPDIRECTSOUNDCAPTURE *ppDSC, __null LPUNKNOWN pUnkOuter);
__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundCaptureEnumerateA(__in LPDSENUMCALLBACKA pDSEnumCallback, __in_opt LPVOID pContext);
__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundCaptureEnumerateW(__in LPDSENUMCALLBACKW pDSEnumCallback, __in_opt LPVOID pContext);

__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundCreate8(__in_opt LPCGUID pcGuidDevice, __deref_out LPDIRECTSOUND8 *ppDS8, __null LPUNKNOWN pUnkOuter);
__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundCaptureCreate8(__in_opt LPCGUID pcGuidDevice, __deref_out LPDIRECTSOUNDCAPTURE8 *ppDSC8, __null LPUNKNOWN pUnkOuter);
__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundFullDuplexCreate(
	__in_opt LPCGUID pcGuidCaptureDevice,
	__in_opt LPCGUID pcGuidRenderDevice,
	__in LPCDSCBUFFERDESC pcDSCBufferDesc,
	__in LPCDSBUFFERDESC pcDSBufferDesc,
	HWND hWnd,
	DWORD dwLevel,
	__deref_out LPDIRECTSOUNDFULLDUPLEX* ppDSFD,
	__deref_out LPDIRECTSOUNDCAPTUREBUFFER8 *ppDSCBuffer8,
	__deref_out LPDIRECTSOUNDBUFFER8 *ppDSBuffer8,
	__null LPUNKNOWN pUnkOuter);

__declspec(dllexport) HRESULT WINAPI wrapper_GetDeviceID(__in_opt LPCGUID pGuidSrc, __out LPGUID pGuidDest);

__declspec(dllexport) HRESULT __stdcall wrapper_DllCanUnloadNow();
__declspec(dllexport) HRESULT __stdcall wrapper_DllGetClassObject();
