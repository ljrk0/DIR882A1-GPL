

#include "dm_internal.h"
#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif
extern INT32 DmmGetDeviceInfo(DM_GET_VALUE_T *pGetValue);

/*******************************************************************************
  Module   : DmmDeviceGetRegisterHandler                                                   
  Function : register get handler                                               
  Input    : void                                                               
  Ounput   : BOOL8: DM_TRUE/DM_FALSE                                          
  Note     : null                                                               
*******************************************************************************/
BOOL8 DmmDeviceGetRegisterHandler(void)
{
    DM_REG_GET_HANDLER_PARA_T       *pPara;
    BOOL8                           bRtn = DM_TRUE;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmmDeviceGetRegisterHandler");
    pPara = (DM_REG_GET_HANDLER_PARA_T *)malloc(sizeof(DM_REG_GET_HANDLER_PARA_T));

    if(pPara == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmmDeviceGetRegisterHandler malloc() failed for pPara");
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara, 0, sizeof(DM_REG_GET_HANDLER_PARA_T));

    pPara->lParaCnt = 1;
    pPara->paParaPath = (INT8 **)malloc(sizeof(INT8 *)*pPara->lParaCnt);

    if(pPara->paParaPath == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmmDeviceGetRegisterHandler malloc() failed for pPara->paParaPath");
        DM_FREE(pPara);
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara->paParaPath, 0, pPara->lParaCnt);

    /* Register Get Device.DeviceInfo.ProcessStatus.CPUUsage Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.ProcessStatus.CPUUsage";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)DmmGetDeviceInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DeviceInfo.ProcessStatus.CPUUsageForPM Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.ProcessStatus.X_BC0304_CPUUsageForPM";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)DmmGetDeviceInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DeviceInfo.ProcessStatus.CPUPeekUsageForPM Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.ProcessStatus.X_BC0304_CPUPeekUsageForPM";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)DmmGetDeviceInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DeviceInfo.ProcessStatus.CPUUsageOfCPUL.CPUID Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.ProcessStatus.X_BC0304_CPUUsageOfCPUL.CPUID";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)DmmGetDeviceInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DeviceInfo.ProcessStatus.CPUUsageOfCPUL.CPUAverageUsage Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.ProcessStatus.X_BC0304_CPUUsageOfCPUL.CPUAverageUsage";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)DmmGetDeviceInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DeviceInfo.ProcessStatus.CPUUsageOfCPUL.CPUAverageUsageForPM Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.ProcessStatus.X_BC0304_CPUUsageOfCPUL.CPUAverageUsageForPM";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)DmmGetDeviceInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DeviceInfo.ProcessStatus.CPUUsageOfCPUL.CPUPeekUsageForPM Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.ProcessStatus.X_BC0304_CPUUsageOfCPUL.CPUPeekUsageForPM";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)DmmGetDeviceInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    DM_FREE(pPara->paParaPath);
    DM_FREE(pPara);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmmDeviceGetRegisterHandler");
    return bRtn;
}

