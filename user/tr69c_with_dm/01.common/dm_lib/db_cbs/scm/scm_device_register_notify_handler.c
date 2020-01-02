

#include "dm_internal.h"
#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif
extern void ScmNotifyInd(DM_NOTIFY_T *pPara);

/*******************************************************************************
  Module   : ScmDeviceNotifyRegisterHandler                                                   
  Function : register notify handler                                            
  Input    : void                                                               
  Ounput   : BOOL8: DM_TRUE/DM_FALSE                                          
  Note     : null                                                               
*******************************************************************************/
BOOL8 ScmDeviceNotifyRegisterHandler(void)
{
    DM_NOTIFY_HANDLER_REG_PARA_T    *pPara;
    BOOL8                           bRtn = DM_TRUE;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter ScmDeviceNotifyRegisterHandler");
    pPara = (DM_NOTIFY_HANDLER_REG_PARA_T *)malloc(sizeof(DM_NOTIFY_HANDLER_REG_PARA_T));

    if(pPara == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ ScmDeviceNotifyRegisterHandler malloc() failed for pPara");
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara, 0, sizeof(DM_NOTIFY_HANDLER_REG_PARA_T));

    pPara->lParaCnt = 1;
    pPara->paParaPath = (INT8 **)malloc(sizeof(INT8 *)*pPara->lParaCnt);

    if(pPara->paParaPath == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ ScmDeviceNotifyRegisterHandler malloc() failed for pPara->paParaPath");
        DM_FREE(pPara);
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara->paParaPath, 0, pPara->lParaCnt);

    //Register Notify Device.Services.FAPService.FAPControl.LTE.OpState Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.FAPControl.LTE.OpState";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)ScmNotifyInd;
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
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)ScmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.AccessMgmt.LTE.MaxUEsServed Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.AccessMgmt.LTE.MaxUEsServed";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)ScmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.AccessMgmt.LTE.MaxAmDrbCnt Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.AccessMgmt.LTE.X_BC0304_MaxAmDrbCnt";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)ScmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.AccessMgmt.LTE.MaxUmDrbCnt Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.AccessMgmt.LTE.X_BC0304_MaxUmDrbCnt";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)ScmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RF.DLBandwidth Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RF.DLBandwidth";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)ScmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RF.ULBandwidth Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RF.ULBandwidth";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)ScmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IP.IPStatus Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IP.X_BC0304_IPStatus";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)ScmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.AutoUpgrade.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.AutoUpgrade.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)ScmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.AutoUpgrade.FtpIp Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.AutoUpgrade.FtpIp";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)ScmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.AutoUpgrade.FtpUser Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.AutoUpgrade.FtpUser";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)ScmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.AutoUpgrade.FtpPassword Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.AutoUpgrade.FtpPassword";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)ScmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.AutoUpgrade.FtpPath Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.AutoUpgrade.FtpPath";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)ScmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Security.FileTransmission.TLSVersion Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Security.X_BC0304_FileTransmission.TLSVersion";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)ScmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Security.FileTransmission.SecurityMethod Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Security.X_BC0304_FileTransmission.SecurityMethod";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)ScmNotifyInd;
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
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave ScmDeviceNotifyRegisterHandler");
    return bRtn;
}
