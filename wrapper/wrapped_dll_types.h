#pragma once

#include <stdint.h>
#include <dsound.h>


struct OrigFunctions
{
	typedef HRESULT (WINAPI *DirectSoundCreate_t)(__in_opt LPCGUID pcGuidDevice, __deref_out LPDIRECTSOUND *ppDS, __null LPUNKNOWN pUnkOuter);
	typedef HRESULT (WINAPI *DirectSoundEnumerateA_t)(__in LPDSENUMCALLBACKA pDSEnumCallback, __in_opt LPVOID pContext);
	typedef HRESULT (WINAPI *DirectSoundEnumerateW_t)(__in LPDSENUMCALLBACKW pDSEnumCallback, __in_opt LPVOID pContext);

	typedef HRESULT (WINAPI *DirectSoundCaptureCreate_t)(__in_opt LPCGUID pcGuidDevice, __deref_out LPDIRECTSOUNDCAPTURE *ppDSC, __null LPUNKNOWN pUnkOuter);
	typedef HRESULT (WINAPI *DirectSoundCaptureEnumerateA_t)(__in LPDSENUMCALLBACKA pDSEnumCallback, __in_opt LPVOID pContext);
	typedef HRESULT (WINAPI *DirectSoundCaptureEnumerateW_t)(__in LPDSENUMCALLBACKW pDSEnumCallback, __in_opt LPVOID pContext);

	typedef HRESULT (WINAPI *DirectSoundCreate8_t)(__in_opt LPCGUID pcGuidDevice, __deref_out LPDIRECTSOUND8 *ppDS8, __null LPUNKNOWN pUnkOuter);
	typedef HRESULT (WINAPI *DirectSoundCaptureCreate8_t)(__in_opt LPCGUID pcGuidDevice, __deref_out LPDIRECTSOUNDCAPTURE8 *ppDSC8, __null LPUNKNOWN pUnkOuter);
	typedef HRESULT (WINAPI *DirectSoundFullDuplexCreate_t)(
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

	typedef HRESULT (WINAPI *GetDeviceID_t)(__in_opt LPCGUID pGuidSrc, __out LPGUID pGuidDest);

	typedef HRESULT (__stdcall *DllCanUnloadNow_t)();
	typedef HRESULT (__stdcall *DllGetClassObject_t)(
		_In_  REFCLSID rclsid,
		_In_  REFIID   riid,
		_Out_ LPVOID   *ppv);


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
