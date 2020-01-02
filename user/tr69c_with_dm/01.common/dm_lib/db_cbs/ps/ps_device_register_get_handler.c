

#include "dm_internal.h"
#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif
extern INT32 PsGetUeRxPwrSnr(DM_GET_VALUE_T *pGetValue);
extern INT32 PsGetUeNum(DM_GET_VALUE_T *pGetValue);
extern INT32 PsGetUeInfo(DM_GET_VALUE_T *pGetValue);
extern INT32 PsGetTemperature(DM_GET_VALUE_T *pGetValue);

/*******************************************************************************
  Module   : PsDeviceGetRegisterHandler                                                   
  Function : register get handler                                               
  Input    : void                                                               
  Ounput   : BOOL8: DM_TRUE/DM_FALSE                                          
  Note     : null                                                               
*******************************************************************************/
BOOL8 PsDeviceGetRegisterHandler(void)
{
    DM_REG_GET_HANDLER_PARA_T       *pPara;
    BOOL8                           bRtn = DM_TRUE;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter PsDeviceGetRegisterHandler");
    pPara = (DM_REG_GET_HANDLER_PARA_T *)malloc(sizeof(DM_REG_GET_HANDLER_PARA_T));

    if(pPara == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ PsDeviceGetRegisterHandler malloc() failed for pPara");
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara, 0, sizeof(DM_REG_GET_HANDLER_PARA_T));

    pPara->lParaCnt = 1;
    pPara->paParaPath = (INT8 **)malloc(sizeof(INT8 *)*pPara->lParaCnt);

    if(pPara->paParaPath == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ PsDeviceGetRegisterHandler malloc() failed for pPara->paParaPath");
        DM_FREE(pPara);
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara->paParaPath, 0, pPara->lParaCnt);

    /* Register Get Device.Services.FAPService.SON.Debug.RSRP Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.Debug.RSRP";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)PsGetUeInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.Services.FAPService.SON.Debug.RSRQ Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.Debug.RSRQ";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)PsGetUeInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.Services.FAPService.SON.Debug.UeHistory.PLMNID Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.Debug.UeHistory.PLMNID";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)PsGetUeInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.Services.FAPService.SON.Debug.UeHistory.CellIdentity Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.Debug.UeHistory.CellIdentity";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)PsGetUeInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.Services.FAPService.Status.UeNumber Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_Status.UeNumber";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)PsGetUeNum;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.Services.FAPService.Status.VolteUeNumber Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_Status.VolteUeNumber";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)PsGetUeNum;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DeviceInfo.TemperatureStatus.TemperatureSensor.Status Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.TemperatureStatus.TemperatureSensor.Status";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)PsGetTemperature;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DeviceInfo.TemperatureStatus.TemperatureSensor.ResetTime Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.TemperatureStatus.TemperatureSensor.ResetTime";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)PsGetTemperature;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DeviceInfo.TemperatureStatus.TemperatureSensor.Value Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.TemperatureStatus.TemperatureSensor.Value";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)PsGetTemperature;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DeviceInfo.TemperatureStatus.TemperatureSensor.LastUpdate Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.TemperatureStatus.TemperatureSensor.LastUpdate";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)PsGetTemperature;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DeviceInfo.TemperatureStatus.TemperatureSensor.MinValue Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.TemperatureStatus.TemperatureSensor.MinValue";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)PsGetTemperature;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DeviceInfo.TemperatureStatus.TemperatureSensor.MinTime Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.TemperatureStatus.TemperatureSensor.MinTime";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)PsGetTemperature;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DeviceInfo.TemperatureStatus.TemperatureSensor.MaxValue Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.TemperatureStatus.TemperatureSensor.MaxValue";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)PsGetTemperature;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DeviceInfo.TemperatureStatus.TemperatureSensor.MaxTime Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.TemperatureStatus.TemperatureSensor.MaxTime";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)PsGetTemperature;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DeviceInfo.TemperatureStatus.TemperatureSensor.LowAlarmTime Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.TemperatureStatus.TemperatureSensor.LowAlarmTime";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)PsGetTemperature;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DeviceInfo.TemperatureStatus.TemperatureSensor.HighAlarmTime Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.TemperatureStatus.TemperatureSensor.HighAlarmTime";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)PsGetTemperature;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DebugMgmt.TtrTest.UeRxPwrOnPusch Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.TtrTest.UeRxPwrOnPusch";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)PsGetUeRxPwrSnr;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DebugMgmt.TtrTest.UeUlSnrPusch Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.TtrTest.UeUlSnrPusch";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)PsGetUeRxPwrSnr;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DebugMgmt.TtrTest.UeRxPwrOnPucch Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.TtrTest.UeRxPwrOnPucch";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)PsGetUeRxPwrSnr;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.DebugMgmt.TtrTest.UeUlSnrPucch Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.TtrTest.UeUlSnrPucch";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)PsGetUeRxPwrSnr;
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
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave PsDeviceGetRegisterHandler");
    return bRtn;
}

