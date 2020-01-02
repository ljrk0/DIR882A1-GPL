

#include "dm_internal.h"
#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif
extern void FmNotifyInd(DM_NOTIFY_T *pPara);

/*******************************************************************************
  Module   : FmDeviceNotifyRegisterHandler                                                   
  Function : register notify handler                                            
  Input    : void                                                               
  Ounput   : BOOL8: DM_TRUE/DM_FALSE                                          
  Note     : null                                                               
*******************************************************************************/
BOOL8 FmDeviceNotifyRegisterHandler(void)
{
    DM_NOTIFY_HANDLER_REG_PARA_T    *pPara;
    BOOL8                           bRtn = DM_TRUE;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter FmDeviceNotifyRegisterHandler");
    pPara = (DM_NOTIFY_HANDLER_REG_PARA_T *)malloc(sizeof(DM_NOTIFY_HANDLER_REG_PARA_T));

    if(pPara == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ FmDeviceNotifyRegisterHandler malloc() failed for pPara");
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara, 0, sizeof(DM_NOTIFY_HANDLER_REG_PARA_T));

    pPara->lParaCnt = 1;
    pPara->paParaPath = (INT8 **)malloc(sizeof(INT8 *)*pPara->lParaCnt);

    if(pPara->paParaPath == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ FmDeviceNotifyRegisterHandler malloc() failed for pPara->paParaPath");
        DM_FREE(pPara);
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara->paParaPath, 0, pPara->lParaCnt);

    //Register Notify Device.Services.FAPService.FAPControl.LTE.OpState Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.FAPControl.LTE.OpState";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.FAPControl.LTE.RFTxStatus Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.FAPControl.LTE.RFTxStatus";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FaultMgmt.SupportedAlarm.ReportingMechanism Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FaultMgmt.SupportedAlarm.ReportingMechanism";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.TraceFilter.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.TraceFilter.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.TraceFilter.ErrLevel Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.TraceFilter.ErrLevel";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.TraceFilter.WarnLevel Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.TraceFilter.WarnLevel";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.TraceFilter.HighLevel Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.TraceFilter.HighLevel";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.TraceFilter.MidLevel Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.TraceFilter.MidLevel";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.TraceFilter.LowLevel Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.TraceFilter.LowLevel";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.TraceFilter.InternalLevel Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.TraceFilter.InternalLevel";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.TraceFilter.PsFilter Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.TraceFilter.PsFilter";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Upload.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Upload.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Upload.URL Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Upload.URL";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Upload.Username Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Upload.Username";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Upload.Password Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Upload.Password";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Upload.UploadPolicyWhenMaxExceeded Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Upload.UploadPolicyWhenMaxExceeded";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Upload.UploadPolicyWhenAlarmRaised Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Upload.UploadPolicyWhenAlarmRaised";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Upload.UploadPolicyWhenPowerOn Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Upload.UploadPolicyWhenPowerOn";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Upload.AutonomousTransferCompleteEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Upload.AutonomousTransferCompleteEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Upload.connTimeout Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Upload.connTimeout";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Upload.limitRate Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Upload.limitRate";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Upload.retryMax Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Upload.retryMax";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Upload.retryInterval Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Upload.retryInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Tcpdump.Interface Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Tcpdump.Interface";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Tcpdump.FilterType Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Tcpdump.FilterType";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Tcpdump.FilterExpression Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Tcpdump.FilterExpression";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Traces.DumpTraces.CpuhDumpSize Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Traces.DumpTraces.CpuhDumpSize";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Traces.DumpTraces.CpulDumpSize Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Traces.DumpTraces.CpulDumpSize";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Traces.ePerview.CpuhPort Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Traces.ePerview.CpuhPort";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Traces.ePerview.CpulPort Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Traces.ePerview.CpulPort";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Traces.LogServer.ConnectingPolicy Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Traces.LogServer.ConnectingPolicy";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Traces.LogServer.IPAddress Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Traces.LogServer.IPAddress";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Traces.LogServer.CpuhPort Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Traces.LogServer.CpuhPort";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Traces.LogServer.CpulPort Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Traces.LogServer.CpulPort";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Traces.LogServer.CpuhDestPath Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Traces.LogServer.CpuhDestPath";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Traces.LogServer.CpulDestPath Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Traces.LogServer.CpulDestPath";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Traces.LogServer.CpuhFileSize Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Traces.LogServer.CpuhFileSize";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.Traces.LogServer.CpulFileSize Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.Traces.LogServer.CpulFileSize";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)FmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    DM_FREE(pPara->paParaPath);
    DM_FREE(pPara);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave FmDeviceNotifyRegisterHandler");
    return bRtn;
}
