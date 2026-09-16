#include "dsound.h"
#include "hooks.h"
#include <core/DideModInitializer.h>


extern "C" {

HRESULT WINAPI wrapper_DirectSoundCreate(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
{
	return g_origDllFunctions.DirectSoundCreate(pcGuidDevice, ppDS, pUnkOuter);
}

HRESULT WINAPI wrapper_DirectSoundEnumerateA(LPDSENUMCALLBACKA pDSEnumCallback, LPVOID pContext)
{
	return g_origDllFunctions.DirectSoundEnumerateA(pDSEnumCallback, pContext);
}

HRESULT WINAPI wrapper_DirectSoundEnumerateW(LPDSENUMCALLBACKW pDSEnumCallback, LPVOID pContext)
{
	return g_origDllFunctions.DirectSoundEnumerateW(pDSEnumCallback, pContext);
}


HRESULT WINAPI wrapper_DirectSoundCaptureCreate(LPCGUID pcGuidDevice, LPDIRECTSOUNDCAPTURE *ppDSC, LPUNKNOWN pUnkOuter)
{
	return g_origDllFunctions.DirectSoundCaptureCreate(pcGuidDevice, ppDSC, pUnkOuter);
}

HRESULT WINAPI wrapper_DirectSoundCaptureEnumerateA(LPDSENUMCALLBACKA pDSEnumCallback, LPVOID pContext)
{
	return g_origDllFunctions.DirectSoundCaptureEnumerateA(pDSEnumCallback, pContext);
}

HRESULT WINAPI wrapper_DirectSoundCaptureEnumerateW(LPDSENUMCALLBACKW pDSEnumCallback, LPVOID pContext)
{
	return g_origDllFunctions.DirectSoundCaptureEnumerateW(pDSEnumCallback, pContext);
}


HRESULT WINAPI wrapper_DirectSoundCreate8(LPCGUID pcGuidDevice, LPDIRECTSOUND8 *ppDS8, LPUNKNOWN pUnkOuter)
{
	return g_origDllFunctions.DirectSoundCreate8(pcGuidDevice, ppDS8, pUnkOuter);
}

HRESULT WINAPI wrapper_DirectSoundCaptureCreate8(LPCGUID pcGuidDevice, LPDIRECTSOUNDCAPTURE8 *ppDSC8, LPUNKNOWN pUnkOuter)
{
	return g_origDllFunctions.DirectSoundCaptureCreate8(pcGuidDevice, ppDSC8, pUnkOuter);
}

HRESULT WINAPI wrapper_DirectSoundFullDuplexCreate(
	LPCGUID pcGuidCaptureDevice,
	LPCGUID pcGuidRenderDevice,
	LPCDSCBUFFERDESC pcDSCBufferDesc,
	LPCDSBUFFERDESC pcDSBufferDesc,
	HWND hWnd,
	DWORD dwLevel,
	LPDIRECTSOUNDFULLDUPLEX* ppDSFD,
	LPDIRECTSOUNDCAPTUREBUFFER8 *ppDSCBuffer8,
	LPDIRECTSOUNDBUFFER8 *ppDSBuffer8,
	LPUNKNOWN pUnkOuter)
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

HRESULT WINAPI wrapper_GetDeviceID(LPCGUID pGuidSrc, LPGUID pGuidDest)
{
	return g_origDllFunctions.GetDeviceID(pGuidSrc, pGuidDest);
}


HRESULT __stdcall wrapper_DllCanUnloadNow()
{
	return g_origDllFunctions.DllCanUnloadNow();
}

HRESULT __stdcall wrapper_DllGetClassObject(
	REFCLSID rclsid,
	REFIID   riid,
	LPVOID   *ppv)
{
	return g_origDllFunctions.DllGetClassObject(rclsid, riid, ppv);
}

}
