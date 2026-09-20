#include "exports.hpp"
#include "dsound.hpp"
#include "misc.hpp"
#include "unicode.hpp"

#include <optional>

using namespace dsound;

struct DSoundWrapper {
    DSoundFunctions fn;

    static Result<DSoundWrapper> create() {
        const auto dll_path{get_system_dsound_dll_path()};
        try {
            const NotNull<HMODULE> dll_handle{::LoadLibraryW(dll_path.c_str())};
            return load_dsound_functions(dll_handle).transform([](auto fn) { return DSoundWrapper{fn}; });
        } catch (const std::exception& ex) {
            return std::unexpected{Error::format("Failed to load wrapped DLL at {}: {}",
                                                 narrow_string(dll_path.wstring()), ex.what())};
        }
    }
};

namespace {
std::optional<DSoundWrapper> g_wrapper;
}

void create_dsound_wrapper() {
    if (auto res{DSoundWrapper::create()}) {
        g_wrapper = res.value();
    } else {
        throw Error::format("Failed to create dsound wrapper: {}", res.error().what());
    }
}

void destroy_dsound_wrapper() { g_wrapper.reset(); }

extern "C" {

__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundCreate(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS,
                                                               LPUNKNOWN pUnkOuter) {
    return g_wrapper->fn.DirectSoundCreate(pcGuidDevice, ppDS, pUnkOuter);
}

__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundEnumerateA(LPDSENUMCALLBACKA pDSEnumCallback, LPVOID pContext) {
    return g_wrapper->fn.DirectSoundEnumerateA(pDSEnumCallback, pContext);
}

__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundEnumerateW(LPDSENUMCALLBACKW pDSEnumCallback, LPVOID pContext) {
    return g_wrapper->fn.DirectSoundEnumerateW(pDSEnumCallback, pContext);
}

__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundCaptureCreate(LPCGUID pcGuidDevice, LPDIRECTSOUNDCAPTURE* ppDSC,
                                                                      LPUNKNOWN pUnkOuter) {
    return g_wrapper->fn.DirectSoundCaptureCreate(pcGuidDevice, ppDSC, pUnkOuter);
}

__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundCaptureEnumerateA(LPDSENUMCALLBACKA pDSEnumCallback,
                                                                          LPVOID pContext) {
    return g_wrapper->fn.DirectSoundCaptureEnumerateA(pDSEnumCallback, pContext);
}

__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundCaptureEnumerateW(LPDSENUMCALLBACKW pDSEnumCallback,
                                                                          LPVOID pContext) {
    return g_wrapper->fn.DirectSoundCaptureEnumerateW(pDSEnumCallback, pContext);
}

__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundCreate8(LPCGUID pcGuidDevice, LPDIRECTSOUND8* ppDS8,
                                                                LPUNKNOWN pUnkOuter) {
    return g_wrapper->fn.DirectSoundCreate8(pcGuidDevice, ppDS8, pUnkOuter);
}

__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundCaptureCreate8(LPCGUID pcGuidDevice,
                                                                       LPDIRECTSOUNDCAPTURE8* ppDSC8,
                                                                       LPUNKNOWN pUnkOuter) {
    return g_wrapper->fn.DirectSoundCaptureCreate8(pcGuidDevice, ppDSC8, pUnkOuter);
}

__declspec(dllexport) HRESULT WINAPI wrapper_DirectSoundFullDuplexCreate(
    LPCGUID pcGuidCaptureDevice, LPCGUID pcGuidRenderDevice, LPCDSCBUFFERDESC pcDSCBufferDesc,
    LPCDSBUFFERDESC pcDSBufferDesc, HWND hWnd, DWORD dwLevel, LPDIRECTSOUNDFULLDUPLEX* ppDSFD,
    LPDIRECTSOUNDCAPTUREBUFFER8* ppDSCBuffer8, LPDIRECTSOUNDBUFFER8* ppDSBuffer8, LPUNKNOWN pUnkOuter) {
    return g_wrapper->fn.DirectSoundFullDuplexCreate(pcGuidCaptureDevice, pcGuidRenderDevice, pcDSCBufferDesc,
                                                     pcDSBufferDesc, hWnd, dwLevel, ppDSFD, ppDSCBuffer8, ppDSBuffer8,
                                                     pUnkOuter);
}

__declspec(dllexport) HRESULT WINAPI wrapper_GetDeviceID(LPCGUID pGuidSrc, LPGUID pGuidDest) {
    return g_wrapper->fn.GetDeviceID(pGuidSrc, pGuidDest);
}

__declspec(dllexport) HRESULT __stdcall wrapper_DllCanUnloadNow() { return g_wrapper->fn.DllCanUnloadNow(); }

__declspec(dllexport) HRESULT __stdcall wrapper_DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) {
    return g_wrapper->fn.DllGetClassObject(rclsid, riid, ppv);
}
}
