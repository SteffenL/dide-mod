#pragma once

#include "libpad_types.h"
#include <stdint.h>

// Exports

__declspec(dllexport) int32_t cellPadInit(uint32_t max_connect);
__declspec(dllexport) int32_t cellPadEnd(void);
__declspec(dllexport) int32_t cellPadClearBuf(uint32_t port_no);
__declspec(dllexport) int32_t cellPadGetData(uint32_t port_no, CellPadData *data);
__declspec(dllexport) int32_t cellPadSetActDirect(uint32_t port_no, CellPadActParam* param);
__declspec(dllexport) int32_t cellPadGetInfo2(CellPadInfo2 *info);
__declspec(dllexport) int32_t cellPadSetPortSetting(uint32_t port_no, uint32_t port_setting);

__declspec(dllexport) int32_t cellPadGetCapabilityInfo(uint32_t port_no, CellPadCapabilityInfo *data);
__declspec(dllexport) int32_t cellPadGetInfo(CellPadInfo *info);
__declspec(dllexport) int32_t cellPadSetPressMode(uint32_t port_no, uint32_t mode);
__declspec(dllexport) int32_t cellPadInfoPressMode(uint32_t port_no);
__declspec(dllexport) int32_t cellPadSetSensorMode(uint32_t port_no, uint32_t mode);
__declspec(dllexport) int32_t cellPadInfoSensorMode(uint32_t port_no);
