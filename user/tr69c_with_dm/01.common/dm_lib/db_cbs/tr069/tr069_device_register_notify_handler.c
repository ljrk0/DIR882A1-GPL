

#include "dm_internal.h"
#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif
extern void Tr069NotifyInd(DM_NOTIFY_T *pPara);
extern void Tr069ActiveNotifyInd(DM_NOTIFY_T *pPara);

/*******************************************************************************
  Module   : Tr069DeviceNotifyRegisterHandler                                                   
  Function : register notify handler                                            
  Input    : void                                                               
  Ounput   : BOOL8: DM_TRUE/DM_FALSE                                          
  Note     : null                                                               
*******************************************************************************/
BOOL8 Tr069DeviceNotifyRegisterHandler(void)
{
    DM_NOTIFY_HANDLER_REG_PARA_T    *pPara;
    BOOL8                           bRtn = DM_TRUE;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter Tr069DeviceNotifyRegisterHandler");
    pPara = (DM_NOTIFY_HANDLER_REG_PARA_T *)malloc(sizeof(DM_NOTIFY_HANDLER_REG_PARA_T));

    if(pPara == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Tr069DeviceNotifyRegisterHandler malloc() failed for pPara");
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara, 0, sizeof(DM_NOTIFY_HANDLER_REG_PARA_T));

    pPara->lParaCnt = 1;
    pPara->paParaPath = (INT8 **)malloc(sizeof(INT8 *)*pPara->lParaCnt);

    if(pPara->paParaPath == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Tr069DeviceNotifyRegisterHandler malloc() failed for pPara->paParaPath");
        DM_FREE(pPara);
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara->paParaPath, 0, pPara->lParaCnt);

    //Register Notify Device.Services.FAPService.FAPControl.UMTS.OpState Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.FAPControl.UMTS.OpState";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.FAPControl.UMTS.RFTxStatus Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.FAPControl.UMTS.RFTxStatus";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.FAPControl.LTE.OpState Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.FAPControl.LTE.OpState";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
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
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.FAPControl.CDMA2000.OneX.OpState Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.FAPControl.CDMA2000.OneX.OpState";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.FAPControl.CDMA2000.OneX.RFTxStatus Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.FAPControl.CDMA2000.OneX.RFTxStatus";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.FAPControl.CDMA2000.HRPD.OpState Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.FAPControl.CDMA2000.HRPD.OpState";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.FAPControl.CDMA2000.HRPD.RFTxStatus Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.FAPControl.CDMA2000.HRPD.RFTxStatus";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.WCDMA.ScanStatus Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.WCDMA.ScanStatus";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.WCDMA.LastScanTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.WCDMA.LastScanTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.TDSCDMA.ScanStatus Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.X_BC0304_TDSCDMA.ScanStatus";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.TDSCDMA.LastScanTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.X_BC0304_TDSCDMA.LastScanTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.GSM.ScanStatus Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.GSM.ScanStatus";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.GSM.LastScanTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.GSM.LastScanTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.LTE.ScanStatus Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.LTE.ScanStatus";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.LTE.LastScanTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.LTE.LastScanTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DeviceInfo.SoftwareVersion Handler.
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.SoftwareVersion";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DeviceInfo.ProvisioningCode Handler.
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.ProvisioningCode";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.EnableCWMP Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.EnableCWMP";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.URL Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.URL";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.Username Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.Username";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.Password Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.Password";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.DiffServ Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.X_BC0304_DiffServ";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.PeriodicInformEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.PeriodicInformEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.PeriodicInformInterval Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.PeriodicInformInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.PeriodicInformTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.PeriodicInformTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.ConnectionRequestURL Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.ConnectionRequestURL";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.ConnectionRequestUsername Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.ConnectionRequestUsername";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.ConnectionRequestPassword Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.ConnectionRequestPassword";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.UpgradesManaged Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.UpgradesManaged";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.DefaultActiveNotificationThrottle Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.DefaultActiveNotificationThrottle";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.CWMPRetryMinimumWaitInterval Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.CWMPRetryMinimumWaitInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.CWMPRetryIntervalMultiplier Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.CWMPRetryIntervalMultiplier";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.STUNEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.STUNEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.STUNServerAddress Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.STUNServerAddress";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.STUNServerPort Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.STUNServerPort";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.STUNUsername Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.STUNUsername";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.STUNPassword Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.STUNPassword";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.STUNMaximumKeepAlivePeriod Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.STUNMaximumKeepAlivePeriod";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.STUNMinimumKeepAlivePeriod Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.STUNMinimumKeepAlivePeriod";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.InstanceMode Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.InstanceMode";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.AutoCreateInstances Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.AutoCreateInstances";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.ConnReqJabberID Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.ConnReqJabberID";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.AutonomousTransferCompletePolicy.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.AutonomousTransferCompletePolicy.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.AutonomousTransferCompletePolicy.TransferTypeFilter Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.AutonomousTransferCompletePolicy.TransferTypeFilter";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.AutonomousTransferCompletePolicy.ResultTypeFilter Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.AutonomousTransferCompletePolicy.ResultTypeFilter";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.ManagementServer.AutonomousTransferCompletePolicy.FileTypeFilter Handler.
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.AutonomousTransferCompletePolicy.FileTypeFilter";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.GatewayInfo.ManufacturerOUI Handler.
    pPara->paParaPath[0] = (INT8*)"Device.GatewayInfo.ManufacturerOUI";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.GatewayInfo.ProductClass Handler.
    pPara->paParaPath[0] = (INT8*)"Device.GatewayInfo.ProductClass";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.GatewayInfo.SerialNumber Handler.
    pPara->paParaPath[0] = (INT8*)"Device.GatewayInfo.SerialNumber";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IP.Interface. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IP.Interface.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.IKEv2SA. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FaultMgmt.ExpeditedEvent.EventTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FaultMgmt.ExpeditedEvent.EventTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FaultMgmt.ExpeditedEvent.AlarmIdentifier Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FaultMgmt.ExpeditedEvent.AlarmIdentifier";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FaultMgmt.ExpeditedEvent.NotificationType Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FaultMgmt.ExpeditedEvent.NotificationType";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FaultMgmt.ExpeditedEvent.ManagedObjectInstance Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FaultMgmt.ExpeditedEvent.ManagedObjectInstance";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FaultMgmt.ExpeditedEvent.EventType Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FaultMgmt.ExpeditedEvent.EventType";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FaultMgmt.ExpeditedEvent.ProbableCause Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FaultMgmt.ExpeditedEvent.ProbableCause";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FaultMgmt.ExpeditedEvent.SpecificProblem Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FaultMgmt.ExpeditedEvent.SpecificProblem";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FaultMgmt.ExpeditedEvent.PerceivedSeverity Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FaultMgmt.ExpeditedEvent.PerceivedSeverity";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FaultMgmt.ExpeditedEvent.AdditionalText Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FaultMgmt.ExpeditedEvent.AdditionalText";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FaultMgmt.ExpeditedEvent.AdditionalInformation Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FaultMgmt.ExpeditedEvent.AdditionalInformation";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FaultMgmt.QueuedEvent.EventTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FaultMgmt.QueuedEvent.EventTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FaultMgmt.QueuedEvent.AlarmIdentifier Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FaultMgmt.QueuedEvent.AlarmIdentifier";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FaultMgmt.QueuedEvent.NotificationType Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FaultMgmt.QueuedEvent.NotificationType";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FaultMgmt.QueuedEvent.ManagedObjectInstance Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FaultMgmt.QueuedEvent.ManagedObjectInstance";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FaultMgmt.QueuedEvent.EventType Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FaultMgmt.QueuedEvent.EventType";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FaultMgmt.QueuedEvent.ProbableCause Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FaultMgmt.QueuedEvent.ProbableCause";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FaultMgmt.QueuedEvent.SpecificProblem Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FaultMgmt.QueuedEvent.SpecificProblem";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FaultMgmt.QueuedEvent.PerceivedSeverity Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FaultMgmt.QueuedEvent.PerceivedSeverity";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FaultMgmt.QueuedEvent.AdditionalText Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FaultMgmt.QueuedEvent.AdditionalText";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FaultMgmt.QueuedEvent.AdditionalInformation Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FaultMgmt.QueuedEvent.AdditionalInformation";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069NotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FAP.GPS.LastScanTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FAP.GPS.LastScanTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)Tr069ActiveNotifyInd;
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
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave Tr069DeviceNotifyRegisterHandler");
    return bRtn;
}
