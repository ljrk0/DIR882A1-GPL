

#include "dm_internal.h"
#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif
extern INT32 GetDeviceInfo(DM_GET_VALUE_T *pGetValue);

/*******************************************************************************
  Module   : ScmDeviceGetRegisterHandler                                                   
  Function : register get handler                                               
  Input    : void                                                               
  Ounput   : BOOL8: DM_TRUE/DM_FALSE                                          
  Note     : null                                                               
*******************************************************************************/
BOOL8 ScmDeviceGetRegisterHandler(void)
{
    DM_REG_GET_HANDLER_PARA_T       *pPara;
    BOOL8                           bRtn = DM_TRUE;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter ScmDeviceGetRegisterHandler");
    pPara = (DM_REG_GET_HANDLER_PARA_T *)malloc(sizeof(DM_REG_GET_HANDLER_PARA_T));

    if(pPara == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ ScmDeviceGetRegisterHandler malloc() failed for pPara");
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara, 0, sizeof(DM_REG_GET_HANDLER_PARA_T));

    pPara->lParaCnt = 1;
    pPara->paParaPath = (INT8 **)malloc(sizeof(INT8 *)*pPara->lParaCnt);

    if(pPara->paParaPath == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ ScmDeviceGetRegisterHandler malloc() failed for pPara->paParaPath");
        DM_FREE(pPara);
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara->paParaPath, 0, pPara->lParaCnt);

    /* Register Get Device.DeviceInfo.Manufacturer Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.Manufacturer";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)GetDeviceInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DeviceInfo.ManufacturerOUI Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.ManufacturerOUI";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)GetDeviceInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DeviceInfo.ExtensionIdentifier Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.X_BC0304_ExtensionIdentifier";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)GetDeviceInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DeviceInfo.HNBIDRealm Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.X_BC0304_HNBIDRealm";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)GetDeviceInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DeviceInfo.ProductClass Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.ProductClass";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)GetDeviceInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DeviceInfo.SerialNumber Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.SerialNumber";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)GetDeviceInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DeviceInfo.HardwareVersion Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.HardwareVersion";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)GetDeviceInfo;
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
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave ScmDeviceGetRegisterHandler");
    return bRtn;
}

