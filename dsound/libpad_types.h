#pragma once

#include <stdint.h>

typedef void CellPadData;
typedef void CellPadActParam;
typedef void CellPadInfo2;
typedef void CellPadCapabilityInfo;
typedef void CellPadInfo;

struct OrigFunctions
{
    typedef int32_t (*cellPadInit_t)(uint32_t max_connect);
    typedef int32_t (*cellPadEnd_t)(void);
    typedef int32_t (*cellPadClearBuf_t)(uint32_t port_no);
    typedef int32_t (*cellPadGetData_t)(uint32_t port_no, CellPadData *data);
    typedef int32_t (*cellPadSetActDirect_t)(uint32_t port_no, CellPadActParam* param);
    typedef int32_t (*cellPadGetInfo2_t)(CellPadInfo2 *info);
    typedef int32_t (*cellPadSetPortSetting_t)(uint32_t port_no, uint32_t port_setting);

    typedef int32_t (*cellPadGetCapabilityInfo_t)(uint32_t port_no, CellPadCapabilityInfo *data);
    typedef int32_t (*cellPadGetInfo_t)(CellPadInfo *info);
    typedef int32_t (*cellPadSetPressMode_t)(uint32_t port_no, uint32_t mode);
    typedef int32_t (*cellPadInfoPressMode_t)(uint32_t port_no);
    typedef int32_t (*cellPadSetSensorMode_t)(uint32_t port_no, uint32_t mode);
    typedef int32_t (*cellPadInfoSensorMode_t)(uint32_t port_no);


    cellPadInit_t cellPadInit;
    cellPadEnd_t cellPadEnd;
    cellPadClearBuf_t cellPadClearBuf;
    cellPadGetData_t cellPadGetData;
    cellPadSetActDirect_t cellPadSetActDirect;
    cellPadGetInfo2_t cellPadGetInfo2;
    cellPadSetPortSetting_t cellPadSetPortSetting;

    cellPadGetCapabilityInfo_t cellPadGetCapabilityInfo;
    cellPadGetInfo_t cellPadGetInfo;
    cellPadSetPressMode_t cellPadSetPressMode;
    cellPadInfoPressMode_t cellPadInfoPressMode;
    cellPadSetSensorMode_t cellPadSetSensorMode;
    cellPadInfoSensorMode_t cellPadInfoSensorMode;
};
