#include "dx11.hpp"
#include "log.hpp"
#include "minhook.hpp"

namespace dx11 {
namespace {
std::optional<dx11::Dx11Libraries> g_libs;
std::optional<dx11::d3d::Functions<std::type_identity_t>> g_d3d_original;
} // namespace

HRESULT dx11_d3d_D3D11CreateDevice_detour(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software,
                                          UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels,
                                          UINT SDKVersion, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel,
                                          ID3D11DeviceContext** ppImmediateContext) {
    LOG("D3D11CreateDevice: SDKVersion = {}", SDKVersion);
    const auto res{g_d3d_original->D3D11CreateDevice(pAdapter, DriverType, Software, Flags, pFeatureLevels,
                                                     FeatureLevels, SDKVersion, ppDevice, pFeatureLevel,
                                                     ppImmediateContext)};
    if (FAILED(res)) {
        return res;
    }
    // ID3D11Device* device{*ppDevice};
    return res;
}

void hook() {
    minhook::create_hook("d3d11.D3D11CreateDevice", g_libs->d3d.fn.D3D11CreateDevice.get(),
                         dx11_d3d_D3D11CreateDevice_detour, g_d3d_original->D3D11CreateDevice);
    minhook::queue_enable_hook("d3d11.D3D11CreateDevice", g_libs->d3d.fn.D3D11CreateDevice.get());
    minhook::apply_queued();
}
} // namespace dx11
