#include "dsound.h"
#include "hooks.h"
#include <core/DideModInitializer.h>


HRESULT WINAPI wrapper_DirectSoundCreate(__in_opt LPCGUID pcGuidDevice, __deref_out LPDIRECTSOUND *ppDS, __null LPUNKNOWN pUnkOuter)
{
	return g_origDllFunctions.DirectSoundCreate(pcGuidDevice, ppDS, pUnkOuter);
}

HRESULT WINAPI wrapper_DirectSoundEnumerateA(__in LPDSENUMCALLBACKA pDSEnumCallback, __in_opt LPVOID pContext)
{
	return g_origDllFunctions.DirectSoundEnumerateA(pDSEnumCallback, pContext);
}

HRESULT WINAPI wrapper_DirectSoundEnumerateW(__in LPDSENUMCALLBACKW pDSEnumCallback, __in_opt LPVOID pContext)
{
	return g_origDllFunctions.DirectSoundEnumerateW(pDSEnumCallback, pContext);
}


HRESULT WINAPI wrapper_DirectSoundCaptureCreate(__in_opt LPCGUID pcGuidDevice, __deref_out LPDIRECTSOUNDCAPTURE *ppDSC, __null LPUNKNOWN pUnkOuter)
{
	return g_origDllFunctions.DirectSoundCaptureCreate(pcGuidDevice, ppDSC, pUnkOuter);
}

HRESULT WINAPI wrapper_DirectSoundCaptureEnumerateA(__in LPDSENUMCALLBACKA pDSEnumCallback, __in_opt LPVOID pContext)
{
	return g_origDllFunctions.DirectSoundCaptureEnumerateA(pDSEnumCallback, pContext);
}

HRESULT WINAPI wrapper_DirectSoundCaptureEnumerateW(__in LPDSENUMCALLBACKW pDSEnumCallback, __in_opt LPVOID pContext)
{
	return g_origDllFunctions.DirectSoundCaptureEnumerateW(pDSEnumCallback, pContext);
}


HRESULT WINAPI wrapper_DirectSoundCreate8(__in_opt LPCGUID pcGuidDevice, __deref_out LPDIRECTSOUND8 *ppDS8, __null LPUNKNOWN pUnkOuter)
{
	return g_origDllFunctions.DirectSoundCreate8(pcGuidDevice, ppDS8, pUnkOuter);
}

HRESULT WINAPI wrapper_DirectSoundCaptureCreate8(__in_opt LPCGUID pcGuidDevice, __deref_out LPDIRECTSOUNDCAPTURE8 *ppDSC8, __null LPUNKNOWN pUnkOuter)
{
	return g_origDllFunctions.DirectSoundCaptureCreate8(pcGuidDevice, ppDSC8, pUnkOuter);
}

HRESULT WINAPI wrapper_DirectSoundFullDuplexCreate(
	__in_opt LPCGUID pcGuidCaptureDevice,
	__in_opt LPCGUID pcGuidRenderDevice,
	__in LPCDSCBUFFERDESC pcDSCBufferDesc,
	__in LPCDSBUFFERDESC pcDSBufferDesc,
	HWND hWnd,
	DWORD dwLevel,
	__deref_out LPDIRECTSOUNDFULLDUPLEX* ppDSFD,
	__deref_out LPDIRECTSOUNDCAPTUREBUFFER8 *ppDSCBuffer8,
	__deref_out LPDIRECTSOUNDBUFFER8 *ppDSBuffer8,
	__null LPUNKNOWN pUnkOuter)
{
	return g_origDllFunctions.DirectSoundFullDuplexCreate(
		pcGuidCaptureDevice,
		pcGuidRenderDevice,
		pcDSCBufferDesc,
		pcDSBufferDesc,
		hWnd,
		dwLevel,
		ppDSFD,
		ppDSCBuffer8,
		ppDSBuffer8,
		pUnkOuter);
}

HRESULT WINAPI wrapper_GetDeviceID(__in_opt LPCGUID pGuidSrc, __out LPGUID pGuidDest)
{
	return g_origDllFunctions.GetDeviceID(pGuidSrc, pGuidDest);
}


HRESULT __stdcall wrapper_DllCanUnloadNow()
{
	return g_origDllFunctions.DllCanUnloadNow();
}

HRESULT __stdcall wrapper_DllGetClassObject(
	_In_  REFCLSID rclsid,
	_In_  REFIID   riid,
	_Out_ LPVOID   *ppv)
{
	return g_origDllFunctions.DllGetClassObject(rclsid, riid, ppv);
}
