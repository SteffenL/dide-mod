#include "libpad.h"
#include "hooks.h"
#include <core/DideModInitializer.h>

int32_t cellPadInit(uint32_t max_connect)
{
    return g_origDllFunctions.cellPadInit(max_connect);
}

int32_t cellPadEnd(void)
{
    return g_origDllFunctions.cellPadEnd();
}

int32_t cellPadClearBuf(uint32_t port_no)
{
    return g_origDllFunctions.cellPadClearBuf(port_no);
}

int32_t cellPadGetData(uint32_t port_no, CellPadData *data)
{
    return g_origDllFunctions.cellPadGetData(port_no, data);
}

int32_t cellPadSetActDirect(uint32_t port_no, CellPadActParam* param)
{
    return g_origDllFunctions.cellPadSetActDirect(port_no, param);
}

int32_t cellPadGetInfo2(CellPadInfo2 *info)
{
    return g_origDllFunctions.cellPadGetInfo2(info);
}

int32_t cellPadSetPortSetting(uint32_t port_no, uint32_t port_setting)
{
    return g_origDllFunctions.cellPadSetPortSetting(port_no, port_setting);
}

int32_t cellPadGetCapabilityInfo(uint32_t port_no, CellPadCapabilityInfo *data)
{
    return g_origDllFunctions.cellPadGetCapabilityInfo(port_no, data);
}

int32_t cellPadGetInfo(CellPadInfo *info)
{
    return g_origDllFunctions.cellPadGetInfo(info);
}

int32_t cellPadSetPressMode(uint32_t port_no, uint32_t mode)
{
    return g_origDllFunctions.cellPadSetPressMode(port_no, mode);
}

int32_t cellPadInfoPressMode(uint32_t port_no)
{
    return g_origDllFunctions.cellPadInfoPressMode(port_no);
}

int32_t cellPadSetSensorMode(uint32_t port_no, uint32_t mode)
{
    return g_origDllFunctions.cellPadSetSensorMode(port_no, mode);
}

int32_t cellPadInfoSensorMode(uint32_t port_no)
{
    return g_origDllFunctions.cellPadInfoSensorMode(port_no);
}
