#pragma once

#include "dynlib.hpp"
#include "misc.hpp"

#include <windows.h>
// Keep this below windows.h
#include <d3d11.h>

namespace dx11 {
namespace d3d {
using D3D11CreateDevice_t = HRESULT (*)(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software,
                                        UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels,
                                        UINT SDKVersion, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel,
                                        ID3D11DeviceContext** ppImmediateContext);

template<template<typename> typename Wrapper>
struct Functions {
    Functions(const DynLib& lib) : D3D11CreateDevice{lib.sym<D3D11CreateDevice_t>("D3D11CreateDevice")} {}

    Wrapper<D3D11CreateDevice_t> D3D11CreateDevice;
};
} // namespace d3d

struct Dx11Libraries {
    struct {
        DynLib lib{DynLib::load("d3d11.dll")};
        d3d::Functions<NotNull> fn{lib};
    } d3d;
};
} // namespace dx11
