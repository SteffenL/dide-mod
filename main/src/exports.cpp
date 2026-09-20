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

HRESULT WINAPI DirectSoundCreate(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN pUnkOuter) {
    return g_wrapper->fn.DirectSoundCreate(pcGuidDevice, ppDS, pUnkOuter);
}

HRESULT WINAPI DirectSoundEnumerateA(LPDSENUMCALLBACKA pDSEnumCallback, LPVOID pContext) {
    return g_wrapper->fn.DirectSoundEnumerateA(pDSEnumCallback, pContext);
}

HRESULT WINAPI DirectSoundEnumerateW(LPDSENUMCALLBACKW pDSEnumCallback, LPVOID pContext) {
    return g_wrapper->fn.DirectSoundEnumerateW(pDSEnumCallback, pContext);
}

HRESULT WINAPI DirectSoundCaptureCreate(LPCGUID pcGuidDevice, LPDIRECTSOUNDCAPTURE* ppDSC, LPUNKNOWN pUnkOuter) {
    return g_wrapper->fn.DirectSoundCaptureCreate(pcGuidDevice, ppDSC, pUnkOuter);
}

HRESULT WINAPI DirectSoundCaptureEnumerateA(LPDSENUMCALLBACKA pDSEnumCallback, LPVOID pContext) {
    return g_wrapper->fn.DirectSoundCaptureEnumerateA(pDSEnumCallback, pContext);
}

HRESULT WINAPI DirectSoundCaptureEnumerateW(LPDSENUMCALLBACKW pDSEnumCallback, LPVOID pContext) {
    return g_wrapper->fn.DirectSoundCaptureEnumerateW(pDSEnumCallback, pContext);
}

HRESULT WINAPI DirectSoundCreate8(LPCGUID pcGuidDevice, LPDIRECTSOUND8* ppDS8, LPUNKNOWN pUnkOuter) {
    return g_wrapper->fn.DirectSoundCreate8(pcGuidDevice, ppDS8, pUnkOuter);
}

HRESULT WINAPI DirectSoundCaptureCreate8(LPCGUID pcGuidDevice, LPDIRECTSOUNDCAPTURE8* ppDSC8, LPUNKNOWN pUnkOuter) {
    return g_wrapper->fn.DirectSoundCaptureCreate8(pcGuidDevice, ppDSC8, pUnkOuter);
}

HRESULT WINAPI DirectSoundFullDuplexCreate(LPCGUID pcGuidCaptureDevice, LPCGUID pcGuidRenderDevice,
                                           LPCDSCBUFFERDESC pcDSCBufferDesc, LPCDSBUFFERDESC pcDSBufferDesc, HWND hWnd,
                                           DWORD dwLevel, LPDIRECTSOUNDFULLDUPLEX* ppDSFD,
                                           LPDIRECTSOUNDCAPTUREBUFFER8* ppDSCBuffer8, LPDIRECTSOUNDBUFFER8* ppDSBuffer8,
                                           LPUNKNOWN pUnkOuter) {
    return g_wrapper->fn.DirectSoundFullDuplexCreate(pcGuidCaptureDevice, pcGuidRenderDevice, pcDSCBufferDesc,
                                                     pcDSBufferDesc, hWnd, dwLevel, ppDSFD, ppDSCBuffer8, ppDSBuffer8,
                                                     pUnkOuter);
}

HRESULT WINAPI GetDeviceID(LPCGUID pGuidSrc, LPGUID pGuidDest) {
    return g_wrapper->fn.GetDeviceID(pGuidSrc, pGuidDest);
}

HRESULT __stdcall DllCanUnloadNow() { return g_wrapper->fn.DllCanUnloadNow(); }

HRESULT __stdcall DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) {
    return g_wrapper->fn.DllGetClassObject(rclsid, riid, ppv);
}
}
