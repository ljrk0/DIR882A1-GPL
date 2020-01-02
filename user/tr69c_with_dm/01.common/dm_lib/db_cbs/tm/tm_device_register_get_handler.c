

#include "dm_internal.h"
#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif
extern INT32 TmGpsGetGPSStatus(DM_GET_VALUE_T *pGetValue);
extern INT32 TmGetCurTimeHandler(DM_GET_VALUE_T *pGetValue);

/*******************************************************************************
  Module   : TmDeviceGetRegisterHandler                                                   
  Function : register get handler                                               
  Input    : void                                                               
  Ounput   : BOOL8: DM_TRUE/DM_FALSE                                          
  Note     : null                                                               
*******************************************************************************/
BOOL8 TmDeviceGetRegisterHandler(void)
{
    DM_REG_GET_HANDLER_PARA_T       *pPara;
    BOOL8                           bRtn = DM_TRUE;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter TmDeviceGetRegisterHandler");
    pPara = (DM_REG_GET_HANDLER_PARA_T *)malloc(sizeof(DM_REG_GET_HANDLER_PARA_T));

    if(pPara == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ TmDeviceGetRegisterHandler malloc() failed for pPara");
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara, 0, sizeof(DM_REG_GET_HANDLER_PARA_T));

    pPara->lParaCnt = 1;
    pPara->paParaPath = (INT8 **)malloc(sizeof(INT8 *)*pPara->lParaCnt);

    if(pPara->paParaPath == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ TmDeviceGetRegisterHandler malloc() failed for pPara->paParaPath");
        DM_FREE(pPara);
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara->paParaPath, 0, pPara->lParaCnt);

    /* Register Get Device.Time.CurrentLocalTime Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.Time.CurrentLocalTime";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)TmGetCurTimeHandler;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.FAP.GPS.ScanStatus Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.FAP.GPS.ScanStatus";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)TmGpsGetGPSStatus;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.FAP.GPS.ErrorDetails Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.FAP.GPS.ErrorDetails";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)TmGpsGetGPSStatus;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.FAP.GPS.LastScanTime Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.FAP.GPS.LastScanTime";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)TmGpsGetGPSStatus;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.FAP.GPS.LastSuccessfulScanTime Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.FAP.GPS.LastSuccessfulScanTime";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)TmGpsGetGPSStatus;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.FAP.GPS.NumberOfSatellites Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.FAP.GPS.NumberOfSatellites";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)TmGpsGetGPSStatus;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.FAP.GPS.ContinuousGPSStatus.CurrentFix Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.FAP.GPS.ContinuousGPSStatus.CurrentFix";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)TmGpsGetGPSStatus;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.FAP.GPS.ContinuousGPSStatus.GotFix Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.FAP.GPS.ContinuousGPSStatus.GotFix";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)TmGpsGetGPSStatus;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.FAP.GPS.ContinuousGPSStatus.TimingGood Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.FAP.GPS.ContinuousGPSStatus.TimingGood";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)TmGpsGetGPSStatus;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.FAP.GPS.ContinuousGPSStatus.Latitude Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.FAP.GPS.ContinuousGPSStatus.Latitude";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)TmGpsGetGPSStatus;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.FAP.GPS.ContinuousGPSStatus.Longitude Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.FAP.GPS.ContinuousGPSStatus.Longitude";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)TmGpsGetGPSStatus;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.FAP.GPS.ContinuousGPSStatus.Elevation Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.FAP.GPS.ContinuousGPSStatus.Elevation";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)TmGpsGetGPSStatus;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.FAP.GPS.ContinuousGPSStatus.LastFixTime Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.FAP.GPS.ContinuousGPSStatus.LastFixTime";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)TmGpsGetGPSStatus;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.FAP.GPS.ContinuousGPSStatus.LastFixDuration Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.FAP.GPS.ContinuousGPSStatus.LastFixDuration";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)TmGpsGetGPSStatus;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.FAP.GPS.ContinuousGPSStatus.SatellitesTracked Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.FAP.GPS.ContinuousGPSStatus.SatellitesTracked";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)TmGpsGetGPSStatus;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.FAP.GPS.ContinuousGPSStatus.ReceiverStatus Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.FAP.GPS.ContinuousGPSStatus.ReceiverStatus";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)TmGpsGetGPSStatus;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.FAP.GPS.ContinuousGPSStatus.LocationType Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.FAP.GPS.ContinuousGPSStatus.LocationType";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)TmGpsGetGPSStatus;
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
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave TmDeviceGetRegisterHandler");
    return bRtn;
}

