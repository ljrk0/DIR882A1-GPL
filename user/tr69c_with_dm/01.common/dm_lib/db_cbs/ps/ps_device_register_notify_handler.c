

#include "dm_internal.h"
#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif
extern void PsNotifyInd(DM_NOTIFY_T *pPara);

/*******************************************************************************
  Module   : PsDeviceNotifyRegisterHandler                                                   
  Function : register notify handler                                            
  Input    : void                                                               
  Ounput   : BOOL8: DM_TRUE/DM_FALSE                                          
  Note     : null                                                               
*******************************************************************************/
BOOL8 PsDeviceNotifyRegisterHandler(void)
{
    DM_NOTIFY_HANDLER_REG_PARA_T    *pPara;
    BOOL8                           bRtn = DM_TRUE;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter PsDeviceNotifyRegisterHandler");
    pPara = (DM_NOTIFY_HANDLER_REG_PARA_T *)malloc(sizeof(DM_NOTIFY_HANDLER_REG_PARA_T));

    if(pPara == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ PsDeviceNotifyRegisterHandler malloc() failed for pPara");
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara, 0, sizeof(DM_NOTIFY_HANDLER_REG_PARA_T));

    pPara->lParaCnt = 1;
    pPara->paParaPath = (INT8 **)malloc(sizeof(INT8 *)*pPara->lParaCnt);

    if(pPara->paParaPath == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ PsDeviceNotifyRegisterHandler malloc() failed for pPara->paParaPath");
        DM_FREE(pPara);
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara->paParaPath, 0, pPara->lParaCnt);

    //Register Notify Device.Services.FAPService.Alias Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.Alias";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.DNPrefix Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.DNPrefix";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.FAPControl.SelfConfigEvents Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.FAPControl.SelfConfigEvents";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.FAPControl.EnclosureTamperingDetected Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.FAPControl.EnclosureTamperingDetected";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.FAPControl.LTE.AdminState Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.FAPControl.LTE.AdminState";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.FAPControl.LTE.PMConfig Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.FAPControl.LTE.PMConfig";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.FAPControl.LTE.Gateway.SecGWServer1 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.FAPControl.LTE.Gateway.SecGWServer1";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.FAPControl.LTE.Gateway.SecGWServer2 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.FAPControl.LTE.Gateway.SecGWServer2";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.FAPControl.LTE.Gateway.SecGWServer3 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.FAPControl.LTE.Gateway.SecGWServer3";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.FAPControl.LTE.Gateway.S1SigLinkServerList Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.FAPControl.LTE.Gateway.S1SigLinkServerList";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.FAPControl.LTE.Gateway.S1ConnectionMode Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.FAPControl.LTE.Gateway.S1ConnectionMode";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.FAPControl.LTE.Gateway.S1SigLinkPort Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.FAPControl.LTE.Gateway.S1SigLinkPort";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.AccessMgmt.LTE.AccessMode Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.AccessMgmt.LTE.AccessMode";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.AccessMgmt.LTE.MaxCSGMembers Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.AccessMgmt.LTE.MaxCSGMembers";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.AccessMgmt.LTE.MaxNonCSGMembers Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.AccessMgmt.LTE.MaxNonCSGMembers";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.AccessMgmt.LTE.MaxResourceNonCSGMembers Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.AccessMgmt.LTE.MaxResourceNonCSGMembers";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.AccessMgmt.LTE.CSGID Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.AccessMgmt.LTE.CSGID";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.AccessMgmt.LTE.HNBName Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.AccessMgmt.LTE.HNBName";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
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
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
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
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.AccessMgmt.LTE.MaxGbrPrbPercent Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.AccessMgmt.LTE.X_BC0304_MaxGbrPrbPercent";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.AccessMgmt.LTE.ImsEmergencySupport Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.AccessMgmt.LTE.X_BC0304_ImsEmergencySupport";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.AccessMgmt.LTE.LocalIPAccess.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.AccessMgmt.LTE.LocalIPAccess.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.Tunnel. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.Tunnel.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.Tunnel.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.Tunnel.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.Tunnel.Alias Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.Tunnel.Alias";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.Tunnel.TunnelRef Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.Tunnel.TunnelRef";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.Tunnel.PLMNID Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.Tunnel.PLMNID";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.AllowedCipheringAlgorithmList Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.AllowedCipheringAlgorithmList";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.AllowedIntegrityProtectionAlgorithmList Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.AllowedIntegrityProtectionAlgorithmList";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.TAC Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.TAC";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.EAID Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.EAID";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.PLMNList.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.PLMNList.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.PLMNList.Alias Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.PLMNList.Alias";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.PLMNList.IsPrimary Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.PLMNList.IsPrimary";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.PLMNList.PLMNID Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.PLMNList.PLMNID";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.PLMNList.CellReservedForOperatorUse Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.PLMNList.CellReservedForOperatorUse";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.Alias Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.Alias";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.QCI Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.QCI";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.Type Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.Type";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.Priority Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.Priority";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.PacketDelayBudget Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.PacketDelayBudget";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.BlerTarget Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_BlerTarget";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.DscpInfo Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_DscpInfo";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.TrafficClass Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_TrafficClass";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.DeliverOrder Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_DeliverOrder";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.DlForwardEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_DlForwardEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.UlForwardEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_UlForwardEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.MaxNonCSGERABs Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_MaxNonCSGERABs";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.NoDataInterval Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_NoDataInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.DRX.DRXEnabled Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_DRX.DRXEnabled";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.DRX.OnDurationTimer Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_DRX.OnDurationTimer";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.DRX.DRXInactivityTimer Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_DRX.DRXInactivityTimer";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.DRX.DRXRetransmissionTimer Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_DRX.DRXRetransmissionTimer";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.DRX.LongDRXCycle Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_DRX.LongDRXCycle";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.DRX.ShortDrxSupport Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_DRX.ShortDrxSupport";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.DRX.ShortDRXCycle Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_DRX.ShortDRXCycle";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.DRX.DRXShortCycleTimer Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_DRX.DRXShortCycleTimer";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.UeDlBsd Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.UeDlBsd";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.UeUlBsd Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.UeUlBsd";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.RLC.RlcMode Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.RLC.RlcMode";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.RLC.Am.tPollRetransmit Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.RLC.Am.tPollRetransmit";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.RLC.Am.pollPdu Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.RLC.Am.pollPdu";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.RLC.Am.pollByte Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.RLC.Am.pollByte";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.RLC.Am.maxRetxThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.RLC.Am.maxRetxThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.RLC.Am.tReordering Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.RLC.Am.tReordering";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.RLC.Am.tStatusProhibit Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.RLC.Am.tStatusProhibit";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.RLC.Um.UlSnFieldLength Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.RLC.Um.UlSnFieldLength";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.RLC.Um.DlSnFieldLength Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.RLC.Um.DlSnFieldLength";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.RLC.Um.tReordering Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.RLC.Um.tReordering";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.PDCP.UeDiscardTimer Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.PDCP.UeDiscardTimer";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.PDCP.NwDiscardTimer Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.PDCP.NwDiscardTimer";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.PDCP.AmStatusRptRequired Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.PDCP.AmStatusRptRequired";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.PDCP.UmPdcpSnSize Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.PDCP.UmPdcpSnSize";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.PDCP.LossLessSrnsRelocFlag Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.PDCP.LossLessSrnsRelocFlag";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.PDCP.HeaderCompressionSupport Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.PDCP.HeaderCompressionSupport";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.PDCP.RohcInfo.MaxCid Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.PDCP.RohcInfo.MaxCid";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.PDCP.RohcInfo.ProfileList.Profile0x0001 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.PDCP.RohcInfo.ProfileList.Profile0x0001";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.PDCP.RohcInfo.ProfileList.Profile0x0002 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.PDCP.RohcInfo.ProfileList.Profile0x0002";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.PDCP.RohcInfo.ProfileList.Profile0x0003 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.PDCP.RohcInfo.ProfileList.Profile0x0003";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.PDCP.RohcInfo.ProfileList.Profile0x0004 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.PDCP.RohcInfo.ProfileList.Profile0x0004";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.PDCP.RohcInfo.ProfileList.Profile0x0006 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.PDCP.RohcInfo.ProfileList.Profile0x0006";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.PDCP.RohcInfo.ProfileList.Profile0x0101 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.PDCP.RohcInfo.ProfileList.Profile0x0101";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.PDCP.RohcInfo.ProfileList.Profile0x0102 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.PDCP.RohcInfo.ProfileList.Profile0x0102";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.PDCP.RohcInfo.ProfileList.Profile0x0103 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.PDCP.RohcInfo.ProfileList.Profile0x0103";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.PDCP.RohcInfo.ProfileList.Profile0x0104 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.PDCP.RohcInfo.ProfileList.Profile0x0104";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.LCH.LchPriority Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.LCH.LchPriority";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.LCH.BucketSizeDuration Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.LCH.BucketSizeDuration";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.LCH.LchGroupPresent Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.LCH.LchGroupPresent";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.EPC.QoS.ERAB.LCH.LogicalChannelGroup Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.EPC.QoS.X_BC0304_ERAB.LCH.LogicalChannelGroup";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Common.CellIdentity Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Common.CellIdentity";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Common.EnbIdType Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Common.EnbIdType";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Common.CellNum Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Common.X_BC0304_CellNum";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Common.Ext.CellIdentity Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Common.X_BC0304_Ext.CellIdentity";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.S1AP.TRelocPrep Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.S1AP.TRelocPrep";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.S1AP.TRelocOverall Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.S1AP.TRelocOverall";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.S1U.GTPUEchoInterval Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.S1U.GTPUEchoInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.S1U.GTPUIPType Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.S1U.GTPUIPType";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.S1U.GTPUEchoN3 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.S1U.X_BC0304_GTPUEchoN3";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.S1U.EnableFastPath Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.S1U.X_BC0304_EnableFastPath";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.S1U.LocalPortNum Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.S1U.X_BC0304_LocalPortNum";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.S1U.GtpSocketSndBufSize Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.S1U.X_BC0304_GtpSocketSndBufSize";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.S1U.GtpSocketRcvBufSize Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.S1U.X_BC0304_GtpSocketRcvBufSize";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.S1U.GtpSocketRcvLimit Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.S1U.X_BC0304_GtpSocketRcvLimit";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.S1U.GtpuHeaderLen Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.S1U.X_BC0304_GtpuHeaderLen";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.S1U.LwaEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.S1U.X_BC0304_LwaEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.S1U.LwaConfig Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.S1U.X_BC0304_LwaConfig";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RF.EARFCNDL Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RF.EARFCNDL";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RF.EARFCNUL Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RF.EARFCNUL";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RF.FreqBandIndicator Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RF.FreqBandIndicator";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RF.ReferenceSignalPower Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RF.ReferenceSignalPower";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RF.PhyCellID Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RF.PhyCellID";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RF.PSCHPowerOffset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RF.PSCHPowerOffset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RF.SSCHPowerOffset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RF.SSCHPowerOffset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RF.PBCHPowerOffset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RF.PBCHPowerOffset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RF.PCHPowerOffset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RF.X_BC0304_PCHPowerOffset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RF.Ext.EARFCNDL Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RF.X_BC0304_Ext.EARFCNDL";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RF.Ext.EARFCNUL Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RF.X_BC0304_Ext.EARFCNUL";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RF.Ext.FreqBandIndicator Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RF.X_BC0304_Ext.FreqBandIndicator";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RF.Ext.DLBandwidth Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RF.X_BC0304_Ext.DLBandwidth";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RF.Ext.ULBandwidth Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RF.X_BC0304_Ext.ULBandwidth";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RF.Ext.ReferenceSignalPower Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RF.X_BC0304_Ext.ReferenceSignalPower";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RF.Ext.PhyCellID Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RF.X_BC0304_Ext.PhyCellID";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RF.Ext.PSCHPowerOffset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RF.X_BC0304_Ext.PSCHPowerOffset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RF.Ext.SSCHPowerOffset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RF.X_BC0304_Ext.SSCHPowerOffset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RF.Ext.PBCHPowerOffset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RF.X_BC0304_Ext.PBCHPowerOffset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RF.Ext.PCHPowerOffset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RF.X_BC0304_Ext.PCHPowerOffset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.DlCpType Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_DlCpType";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.UlCpType Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_UlCpType";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.AntennaInfo.Config.TransmissionModeBitmap Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.AntennaInfo.X_BC0304_Config.TransmissionModeBitmap";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.AntennaInfo.Config.TransAntennaSelectionSupport Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.AntennaInfo.X_BC0304_Config.TransAntennaSelectionSupport";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.AntennaInfo.Config.TransSelectionSetup Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.AntennaInfo.X_BC0304_Config.TransSelectionSetup";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PDSCH.Pb Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PDSCH.Pb";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PDSCH.Pa Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PDSCH.Pa";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PCFICH.PcfichPwrOffset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_PCFICH.PcfichPwrOffset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PHICH.Resource Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_PHICH.Resource";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PHICH.Duration Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_PHICH.Duration";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PHICH.PwrOffset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_PHICH.PwrOffset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.SRS.SRSEnabled Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.SRS.SRSEnabled";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.SRS.SRSBandwidthConfig Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.SRS.SRSBandwidthConfig";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.SRS.SRSMaxUpPTS Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.SRS.SRSMaxUpPTS";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.SRS.AckNackSRSSimultaneousTransmission Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.SRS.AckNackSRSSimultaneousTransmission";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.SRS.SrsSubframeConfig Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.SRS.X_BC0304_SrsSubframeConfig";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.SRS.SrsSubframeOffset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.SRS.X_BC0304_SrsSubframeOffset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.SRS.UlConfigDed.SrsBandwidth Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.SRS.X_BC0304_UlConfigDed.SrsBandwidth";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.SRS.UlConfigDed.SrsHopBandwidth Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.SRS.X_BC0304_UlConfigDed.SrsHopBandwidth";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.SRS.UlConfigDed.Duration Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.SRS.X_BC0304_UlConfigDed.Duration";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRACH.RootSequenceIndex Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRACH.RootSequenceIndex";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRACH.ConfigurationIndex Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRACH.ConfigurationIndex";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRACH.HighSpeedFlag Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRACH.HighSpeedFlag";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRACH.ZeroCorrelationZoneConfig Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRACH.ZeroCorrelationZoneConfig";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRACH.FreqOffset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRACH.FreqOffset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRACH.Ext.RootSequenceIndex Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRACH.X_BC0304_Ext.RootSequenceIndex";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRACH.Ext.ConfigurationIndex Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRACH.X_BC0304_Ext.ConfigurationIndex";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRACH.Ext.HighSpeedFlag Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRACH.X_BC0304_Ext.HighSpeedFlag";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRACH.Ext.ZeroCorrelationZoneConfig Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRACH.X_BC0304_Ext.ZeroCorrelationZoneConfig";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRACH.Ext.FreqOffset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRACH.X_BC0304_Ext.FreqOffset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUCCH.DeltaPUCCHShift Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUCCH.DeltaPUCCHShift";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUCCH.NRBCQI Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUCCH.NRBCQI";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUCCH.N1PUCCHAN Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUCCH.N1PUCCHAN";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUCCH.CQIPUCCHResourceIndex Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUCCH.CQIPUCCHResourceIndex";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUCCH.ConfigDed.TddAckNackFeedbackMode Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUCCH.X_BC0304_ConfigDed.TddAckNackFeedbackMode";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUCCH.ConfigDed.AckNackRepetition.RepetitionFactor Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUCCH.X_BC0304_ConfigDed.AckNackRepetition.RepetitionFactor";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUCCH.ConfigDed.AckNackRepetition.N1PucchAnRep Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUCCH.X_BC0304_ConfigDed.AckNackRepetition.N1PucchAnRep";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUSCH.NSB Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUSCH.NSB";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUSCH.HoppingMode Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUSCH.HoppingMode";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUSCH.HoppingOffset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUSCH.HoppingOffset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUSCH.ULRS.GroupHoppingEnabled Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUSCH.ULRS.GroupHoppingEnabled";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUSCH.ULRS.SequenceHoppingEnabled Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUSCH.ULRS.SequenceHoppingEnabled";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUSCH.ConfigDed.BetaOffsetAckIndex Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUSCH.X_BC0304_ConfigDed.BetaOffsetAckIndex";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUSCH.ConfigDed.BetaOffsetRiIndex Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUSCH.X_BC0304_ConfigDed.BetaOffsetRiIndex";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUSCH.ConfigDed.BetaOffsetCqiIndex Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PUSCH.X_BC0304_ConfigDed.BetaOffsetCqiIndex";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.P0NominalPUSCH Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.P0NominalPUSCH";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.Alpha Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.Alpha";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.P0NominalPUCCH Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.P0NominalPUCCH";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.Ext.P0NominalPUSCH Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.X_BC0304_Ext.P0NominalPUSCH";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.Ext.P0NominalPUCCH Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.X_BC0304_Ext.P0NominalPUCCH";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.ConfigDed.UlPcCtrlSupport Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.X_BC0304_ConfigDed.UlPcCtrlSupport";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.ConfigDed.P0UePusch Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.X_BC0304_ConfigDed.P0UePusch";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.ConfigDed.DeltaMscEnabled Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.X_BC0304_ConfigDed.DeltaMscEnabled";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.ConfigDed.AccumulationEnabled Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.X_BC0304_ConfigDed.AccumulationEnabled";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.ConfigDed.P0UePucch Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.X_BC0304_ConfigDed.P0UePucch";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.ConfigDed.PSrsOffset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.X_BC0304_ConfigDed.PSrsOffset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.ConfigDed.Fc Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.ULPowerControl.X_BC0304_ConfigDed.Fc";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.TpcPdcchConfigPucch.PucchTpcSupport Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_TpcPdcchConfigPucch.PucchTpcSupport";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.TpcPdcchConfigPucch.IsFormat3 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_TpcPdcchConfigPucch.IsFormat3";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.TpcPdcchConfigPusch.PuschTpcSupport Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_TpcPdcchConfigPusch.PuschTpcSupport";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.TpcPdcchConfigPusch.IsFormat3 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_TpcPdcchConfigPusch.IsFormat3";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.CqiReportConfig.AperiodicSupport Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_CqiReportConfig.AperiodicSupport";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.CqiReportConfig.AperiodicCqiFeedbackType Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_CqiReportConfig.AperiodicCqiFeedbackType";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.CqiReportConfig.AperiodicCqiInterval Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_CqiReportConfig.AperiodicCqiInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.CqiReportConfig.NomPdschRsEpreOffset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_CqiReportConfig.NomPdschRsEpreOffset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.CqiReportConfig.CqiReportPeriodic.CqiBandType Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_CqiReportConfig.CqiReportPeriodic.CqiBandType";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.CqiReportConfig.CqiReportPeriodic.SubbandCqiKValue Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_CqiReportConfig.CqiReportPeriodic.SubbandCqiKValue";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.CqiReportConfig.CqiReportPeriodic.SimulAckNackCqiSupport Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_CqiReportConfig.CqiReportPeriodic.SimulAckNackCqiSupport";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.SchedulingRequestConfig.DsrTransMax Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_SchedulingRequestConfig.DsrTransMax";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.CsiRsConfig.CsiRsSupport Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_CsiRsConfig.CsiRsSupport";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.CsiRsConfig.AntennaPortsCount Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_CsiRsConfig.AntennaPortsCount";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.CsiRsConfig.ResourceConfig Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_CsiRsConfig.ResourceConfig";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.CsiRsConfig.SubframeConfig Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_CsiRsConfig.SubframeConfig";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.CsiRsConfig.PC Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_CsiRsConfig.PC";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.CsiRsConfig.ZeroTxPwrResourceConfigList Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_CsiRsConfig.ZeroTxPwrResourceConfigList";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.CsiRsConfig.ZeroTxPwrSubframeConfig Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.X_BC0304_CsiRsConfig.ZeroTxPwrSubframeConfig";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.MBSFN.NeighCellConfig Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.MBSFN.NeighCellConfig";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.MBSFN.SFConfigList.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.MBSFN.SFConfigList.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.MBSFN.SFConfigList.Alias Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.MBSFN.SFConfigList.Alias";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.MBSFN.SFConfigList.SyncStratumID Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.MBSFN.SFConfigList.SyncStratumID";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.MBSFN.SFConfigList.RadioFrameAllocationPeriod Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.MBSFN.SFConfigList.RadioFrameAllocationPeriod";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.MBSFN.SFConfigList.RadioframeAllocationOffset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.MBSFN.SFConfigList.RadioframeAllocationOffset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.MBSFN.SFConfigList.RadioFrameAllocationSize Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.MBSFN.SFConfigList.RadioFrameAllocationSize";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.MBSFN.SFConfigList.SubFrameAllocations Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.MBSFN.SFConfigList.SubFrameAllocations";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRS.NumPRSResourceBlocks Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRS.NumPRSResourceBlocks";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRS.PRSConfigurationIndex Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRS.PRSConfigurationIndex";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRS.NumConsecutivePRSSubfames Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.PRS.NumConsecutivePRSSubfames";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.TDDFrame.SubFrameAssignment Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.TDDFrame.SubFrameAssignment";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.TDDFrame.SpecialSubframePatterns Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.TDDFrame.SpecialSubframePatterns";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.TDDFrame.Ext.SubFrameAssignment Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.TDDFrame.X_BC0304_Ext.SubFrameAssignment";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.PHY.TDDFrame.Ext.SpecialSubframePatterns Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.PHY.TDDFrame.X_BC0304_Ext.SpecialSubframePatterns";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.TimeAlignmentTimerCommon Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_TimeAlignmentTimerCommon";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.TimeAlignmentTimerDedicated Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_TimeAlignmentTimerDedicated";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.DlMaxArq Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_DlMaxArq";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.SrProhibitTimer Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_SrProhibitTimer";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.SCellDeactivationTimer Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_SCellDeactivationTimer";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.ExtendedBsrSize Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_ExtendedBsrSize";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.ExtendedPhr Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_ExtendedPhr";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.DataReportMode Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_DataReportMode";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.MaxConsecutiveDtx Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_MaxConsecutiveDtx";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.MaxConsecutiveBadCrc Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_MaxConsecutiveBadCrc";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.TbHighWaterMark Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_TbHighWaterMark";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.TbLowWaterMark Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_TbLowWaterMark";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PrachPrbScheAlgPeriodic Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PrachPrbScheAlgPeriodic";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PreambleCntThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PreambleCntThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.NumberOfRaPreambles Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.NumberOfRaPreambles";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.SizeOfRaGroupA Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.SizeOfRaGroupA";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.MessageSizeGroupA Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.MessageSizeGroupA";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.MessagePowerOffsetGroupB Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.MessagePowerOffsetGroupB";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.PowerRampingStep Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.PowerRampingStep";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.PreambleInitialReceivedTargetPower Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.PreambleInitialReceivedTargetPower";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.PreambleTransMax Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.PreambleTransMax";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.ResponseWindowSize Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.ResponseWindowSize";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.ContentionResolutionTimer Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.ContentionResolutionTimer";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.MaxHARQMsg3Tx Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.MaxHARQMsg3Tx";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.MaxHARQMsg4Tx Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.X_BC0304_MaxHARQMsg4Tx";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.DynamicPreambleInitialPowerEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.X_BC0304_DynamicPreambleInitialPowerEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.PreambleInitialPowerSINR Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.X_BC0304_PreambleInitialPowerSINR";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.PreambleInitialPowerChangeThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.X_BC0304_PreambleInitialPowerChangeThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.PreambleInitialPowerChangeImmediateThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.X_BC0304_PreambleInitialPowerChangeImmediateThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.PreambleInitialPowerChangeProhibitTimer Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.X_BC0304_PreambleInitialPowerChangeProhibitTimer";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.MaxAllowedPreambleInitialPower Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.X_BC0304_MaxAllowedPreambleInitialPower";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.Ext.PreambleInitialReceivedTargetPower Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.RACH.X_BC0304_Ext.PreambleInitialReceivedTargetPower";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PCH.DefaultPagingCycle Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PCH.DefaultPagingCycle";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PCH.Nb Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PCH.Nb";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.DRX.DRXEnabled Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.DRX.DRXEnabled";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.DRX.OnDurationTimer Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.DRX.OnDurationTimer";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.DRX.DRXInactivityTimer Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.DRX.DRXInactivityTimer";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.DRX.DRXRetransmissionTimer Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.DRX.DRXRetransmissionTimer";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.DRX.LongDRXCycle Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.DRX.LongDRXCycle";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.DRX.ShortDrxSupport Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.DRX.X_BC0304_ShortDrxSupport";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.DRX.ShortDRXCycle Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.DRX.ShortDRXCycle";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.DRX.DRXShortCycleTimer Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.DRX.DRXShortCycleTimer";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipMeasurementPeriodic Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipMeasurementPeriodic";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.OutOfSyncMaxCnt Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.OutOfSyncMaxCnt";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PuschBlerChgUlCqiEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PuschBlerChgUlCqiEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PuschUlCqiChgSirEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PuschUlCqiChgSirEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PuschOuterLoopPcPeriod Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PuschOuterLoopPcPeriod";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PuschInnerLoopPcPeriod Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PuschInnerLoopPcPeriod";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PuschUlCqiUpdateStep Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PuschUlCqiUpdateStep";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PuschSirUpdateStep Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PuschSirUpdateStep";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PuschUlCqiTarget Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PuschUlCqiTarget";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PuschMaxUlCqiTarget Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PuschMaxUlCqiTarget";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PuschMinUlCqiTarget Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PuschMinUlCqiTarget";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PuschSirTarget Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PuschSirTarget";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PuschSirTargetSib Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PuschSirTargetSib";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PuschMaxSirTarget Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PuschMaxSirTarget";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PuschMinSirTarget Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PuschMinSirTarget";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PuschUlCqiBalance Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PuschUlCqiBalance";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PuschSirBalance Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PuschSirBalance";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PuschBlerMaxTargetCnt Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PuschBlerMaxTargetCnt";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PuschBlerMinTargetCnt Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PuschBlerMinTargetCnt";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PuschMaxRxPower Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PuschMaxRxPower";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PucchOuterLoopPcEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PucchOuterLoopPcEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PucchOuterLoopPcPeriod Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PucchOuterLoopPcPeriod";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PucchInnerLoopPcPeriod Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PucchInnerLoopPcPeriod";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PucchSirUpdateStep Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PucchSirUpdateStep";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PucchSirTarget Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PucchSirTarget";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PucchSirTargetSib Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PucchSirTargetSib";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PucchMaxSirTarget Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PucchMaxSirTarget";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PucchMinSirTarget Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PucchMinSirTarget";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PucchSirBalance Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PucchSirBalance";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PucchDtxMaxTargetCnt Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PucchDtxMaxTargetCnt";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PucchDtxMinTargetCnt Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PucchDtxMinTargetCnt";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PucchMaxRxPower Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PucchMaxRxPower";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.RipPowerCtrlConfig.PucchAbnormalThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.RipPowerCtrlConfig.PucchAbnormalThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.OuterLoopCtrlConfig.DLOuterLoopCtrlEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.OuterLoopCtrlConfig.DLOuterLoopCtrlEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.OuterLoopCtrlConfig.DlCqiDeltaMax Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.OuterLoopCtrlConfig.DlCqiDeltaMax";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.OuterLoopCtrlConfig.DlCqiAlpha20msMore Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.OuterLoopCtrlConfig.DlCqiAlpha20msMore";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.OuterLoopCtrlConfig.DlCqiAlpha20msLess Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.OuterLoopCtrlConfig.DlCqiAlpha20msLess";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.OuterLoopCtrlConfig.DlMinDeltaMcs Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.OuterLoopCtrlConfig.DlMinDeltaMcs";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.OuterLoopCtrlConfig.DlMaxDeltaMcs Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.OuterLoopCtrlConfig.DlMaxDeltaMcs";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.OuterLoopCtrlConfig.DlMcsDown Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.OuterLoopCtrlConfig.DlMcsDown";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.OuterLoopCtrlConfig.DlMcsUp Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.OuterLoopCtrlConfig.DlMcsUp";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.OuterLoopCtrlConfig.ULOuterLoopCtrlEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.OuterLoopCtrlConfig.ULOuterLoopCtrlEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.OuterLoopCtrlConfig.ucUlCqiDeltaMax Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.OuterLoopCtrlConfig.ucUlCqiDeltaMax";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.OuterLoopCtrlConfig.ucUlCqiAlpha Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.OuterLoopCtrlConfig.ucUlCqiAlpha";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.OuterLoopCtrlConfig.cUlMinDeltaMcs Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.OuterLoopCtrlConfig.cUlMinDeltaMcs";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.OuterLoopCtrlConfig.cUlMaxDeltaMcs Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.OuterLoopCtrlConfig.cUlMaxDeltaMcs";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.OuterLoopCtrlConfig.sUlMcsDown Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.OuterLoopCtrlConfig.sUlMcsDown";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.OuterLoopCtrlConfig.sUlMcsUp Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.OuterLoopCtrlConfig.sUlMcsUp";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.PhrConfig.PhrConfigEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.PhrConfig.PhrConfigEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.PhrConfig.PeriodicPhrTimer Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.PhrConfig.PeriodicPhrTimer";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.PhrConfig.ProhibitPhrTimer Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.PhrConfig.ProhibitPhrTimer";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.PwrCtrlConfig.PhrConfig.DlPathLossChange Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_PwrCtrlConfig.PhrConfig.DlPathLossChange";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.FSSEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.FSSEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.BCCH.PdcchFormatForSi Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.BCCH.PdcchFormatForSi";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.BCCH.McsIndexForSi Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.BCCH.McsIndexForSi";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.BCCH.TxTimes Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.BCCH.TxTimes";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.PCCH.PdcchFormatForPch Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.PCCH.PdcchFormatForPch";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.PCCH.McsIndexForPch Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.PCCH.McsIndexForPch";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.RAR.PdcchFormatForRar Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.RAR.PdcchFormatForRar";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.RAR.McsIndexForRar Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.RAR.McsIndexForRar";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.RAR.UlMsg3Mcs Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.RAR.UlMsg3Mcs";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.CCCH.PdcchFormatForCcch Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.CCCH.PdcchFormatForCcch";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.CCCH.McsIndexForCcch Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.CCCH.McsIndexForCcch";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.DlMcsCorrectionConfig.DlMcsCorrectionFlg Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.DlMcsCorrectionConfig.DlMcsCorrectionFlg";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.DlMcsCorrectionConfig.Period Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.DlMcsCorrectionConfig.Period";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.DlMcsCorrectionConfig.BlerTarget Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.DlMcsCorrectionConfig.BlerTarget";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.DL_DCCH_DTCH.DownlinkMaxUserPerTti Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.DL_DCCH_DTCH.DownlinkMaxUserPerTti";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.DL_DCCH_DTCH.DownlinkMaxSpsUserPerTti Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.DL_DCCH_DTCH.DownlinkMaxSpsUserPerTti";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.DL_DCCH_DTCH.DownlinkMaxCaUserPerTti Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.DL_DCCH_DTCH.DownlinkMaxCaUserPerTti";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.DL_DCCH_DTCH.DownlinkMaxCeuUserPerTti Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.DL_DCCH_DTCH.DownlinkMaxCeuUserPerTti";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.DL_DCCH_DTCH.DownlinkScheduleAlgorithm Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.DL_DCCH_DTCH.DownlinkScheduleAlgorithm";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.DL_DCCH_DTCH.DynamicPdcchFormatFlg Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.DL_DCCH_DTCH.DynamicPdcchFormatFlg";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.DL_DCCH_DTCH.PdcchFormatForDlDcch Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.DL_DCCH_DTCH.PdcchFormatForDlDcch";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.DL_DCCH_DTCH.DlMaxMcs Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.DL_DCCH_DTCH.DlMaxMcs";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.DL_DCCH_DTCH.DlMinMcs Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.DL_DCCH_DTCH.DlMinMcs";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.DL_DCCH_DTCH.CeuSubCqiThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.DL_DCCH_DTCH.CeuSubCqiThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.DL_DCCH_DTCH.CcuSubCqiThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.DL_DCCH_DTCH.CcuSubCqiThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.DL_DCCH_DTCH.ProportionalFairness.WeightForCqi Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.DL_DCCH_DTCH.ProportionalFairness.WeightForCqi";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.DL_DCCH_DTCH.ProportionalFairness.WeightForLchPrio Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.DL_DCCH_DTCH.ProportionalFairness.WeightForLchPrio";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.DL_DCCH_DTCH.ProportionalFairness.WeightForPacketDelay Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.DL_DCCH_DTCH.ProportionalFairness.WeightForPacketDelay";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.UL_DCCH_DTCH.UplinkMaxUserPerTti Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.UL_DCCH_DTCH.UplinkMaxUserPerTti";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.UL_DCCH_DTCH.UplinkMaxSpsUserPerTti Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.UL_DCCH_DTCH.UplinkMaxSpsUserPerTti";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.UL_DCCH_DTCH.UplinkScheduleAlgorithm Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.UL_DCCH_DTCH.UplinkScheduleAlgorithm";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.UL_DCCH_DTCH.UplinkZeroBoScheInterval Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.UL_DCCH_DTCH.UplinkZeroBoScheInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.UL_DCCH_DTCH.UplinkZeroBoTimerLength Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.UL_DCCH_DTCH.UplinkZeroBoTimerLength";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.UL_DCCH_DTCH.PdcchFormatForUlDcch Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.UL_DCCH_DTCH.PdcchFormatForUlDcch";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.UL_DCCH_DTCH.PrachResourceScheFlag Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.UL_DCCH_DTCH.PrachResourceScheFlag";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.UL_DCCH_DTCH.DynamicPucchResourceScheFlag Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.UL_DCCH_DTCH.DynamicPucchResourceScheFlag";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.UL_DCCH_DTCH.UplinkSpsScheEnableFlg Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.UL_DCCH_DTCH.UplinkSpsScheEnableFlg";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.UL_DCCH_DTCH.PhrLimitPrbEnableFlg Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.UL_DCCH_DTCH.PhrLimitPrbEnableFlg";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.UL_DCCH_DTCH.PhrThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.UL_DCCH_DTCH.PhrThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.UL_DCCH_DTCH.UeDefaultGbr Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.UL_DCCH_DTCH.UeDefaultGbr";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.UL_DCCH_DTCH.UlMaxMcs Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.UL_DCCH_DTCH.UlMaxMcs";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.UL_DCCH_DTCH.UlMinMcs Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.UL_DCCH_DTCH.UlMinMcs";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.UL_DCCH_DTCH.SrGrantTbSize Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.UL_DCCH_DTCH.SrGrantTbSize";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.UL_DCCH_DTCH.SrGrantUpdateTimerLength Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.UL_DCCH_DTCH.SrGrantUpdateTimerLength";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.UL_DCCH_DTCH.ProportionalFairness.WeightForUlCqi Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.UL_DCCH_DTCH.ProportionalFairness.WeightForUlCqi";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.UL_DCCH_DTCH.ProportionalFairness.WeightForUlLcgPrio Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.UL_DCCH_DTCH.ProportionalFairness.WeightForUlLcgPrio";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.UL_DCCH_DTCH.ProportionalFairness.WeightForUlBsrDelay Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.UL_DCCH_DTCH.ProportionalFairness.WeightForUlBsrDelay";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.SCellActiveConfig.SCellActivePrbThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.SCellActiveConfig.SCellActivePrbThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.SCellActiveConfig.SCellDeactivePrbThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.SCellActiveConfig.SCellDeactivePrbThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.SCellActiveConfig.SCellActiveBoThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.SCellActiveConfig.SCellActiveBoThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.MAC.Scheduler.SCellActiveConfig.SCellDeactiveBoThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.MAC.X_BC0304_Scheduler.SCellActiveConfig.SCellDeactiveBoThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RLC.FcStartThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RLC.X_BC0304_FcStartThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RLC.FcStopThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RLC.X_BC0304_FcStopThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB1.DefaultConfiguration Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB1.DefaultConfiguration";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB1.TPollRetransmit Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB1.TPollRetransmit";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB1.PollPDU Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB1.PollPDU";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB1.PollByte Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB1.PollByte";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB1.MaxRetxThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB1.MaxRetxThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB1.TReordering Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB1.TReordering";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB1.TStatusProhibit Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB1.TStatusProhibit";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB2.DefaultConfiguration Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB2.DefaultConfiguration";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB2.TPollRetransmit Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB2.TPollRetransmit";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB2.PollPDU Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB2.PollPDU";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB2.PollByte Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB2.PollByte";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB2.MaxRetxThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB2.MaxRetxThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB2.TReordering Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB2.TReordering";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB2.TStatusProhibit Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RLC.SRB2.TStatusProhibit";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SPS.DlSpsConfig.DlSpsEnabled Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SPS.DlSpsConfig.DlSpsEnabled";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SPS.DlSpsConfig.DlSchedInterval Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SPS.DlSpsConfig.DlSchedInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SPS.DlSpsConfig.ImplReleaseAfter Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SPS.DlSpsConfig.ImplReleaseAfter";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SPS.UlSpsConfig.UlSpsEnabled Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SPS.UlSpsConfig.UlSpsEnabled";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SPS.UlSpsConfig.UlSchedInterval Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SPS.UlSpsConfig.UlSchedInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SPS.UlSpsConfig.ImplReleaseAfter Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SPS.UlSpsConfig.ImplReleaseAfter";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SPS.UlSpsConfig.p0PersistentPresent Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SPS.UlSpsConfig.p0PersistentPresent";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SPS.UlSpsConfig.p0NominalPuschPersistent Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SPS.UlSpsConfig.p0NominalPuschPersistent";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SPS.UlSpsConfig.p0UEPuschPersistent Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SPS.UlSpsConfig.p0UEPuschPersistent";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SPS.UlSpsConfig.twoIntervalsConfigPresent Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SPS.UlSpsConfig.twoIntervalsConfigPresent";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SPS.UlSpsConfig.twoIntervalsConfig Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SPS.UlSpsConfig.twoIntervalsConfig";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.UEC.UeIdCumulativeAssignFlg Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_UEC.UeIdCumulativeAssignFlg";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.UEC.CsfbPsHoSupportFlg Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_UEC.CsfbPsHoSupportFlg";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.UEC.OverloadReductionTimerLength Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_UEC.OverloadReductionTimerLength";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.UEC.LteHoDelayTimerLength Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_UEC.LteHoDelayTimerLength";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.UEC.SrvccHoDelayTimerLength Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_UEC.SrvccHoDelayTimerLength";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.UEC.IdleModeMobilityCtrlInfoPresent Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_UEC.IdleModeMobilityCtrlInfoPresent";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.UEC.UeDeleteHandling Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_UEC.UeDeleteHandling";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.UEC.QCI.ERabSetupFailedHandling Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_UEC.QCI.ERabSetupFailedHandling";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRA.CASupport Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRA.CASupport";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRA.BackhaulBandwithEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRA.BackhaulBandwithEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRA.BackhaulBandwithThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRA.BackhaulBandwithThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRA.CongestionCheckEnabled Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRA.CongestionCheckEnabled";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRA.CongestionCheckTimerLength Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRA.CongestionCheckTimerLength";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRA.SrAdjustEnabled Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRA.SrAdjustEnabled";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRA.SrAdjustTimerLength Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRA.SrAdjustTimerLength";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRA.LoadBalanceEnabled Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRA.LoadBalanceEnabled";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRA.LoadBalanceTimerLength Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRA.LoadBalanceTimerLength";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRA.UeNumDiffThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRA.UeNumDiffThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRA.GbrERabNumDiffThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRA.GbrERabNumDiffThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRA.GbrPrbNumDiffThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRA.GbrPrbNumDiffThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRA.PucchPrbNum Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRA.PucchPrbNum";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRA.Pucch2Shift Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRA.Pucch2Shift";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRA.SrPeriodShort Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRA.SrPeriodShort";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRA.SrPeriodLong Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRA.SrPeriodLong";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRA.CqiPeriodShort Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRA.CqiPeriodShort";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRA.CqiPeriodLong Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRA.CqiPeriodLong";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRA.Mri Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRA.Mri";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRA.SrsPeriod Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRA.SrsPeriod";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRC.MaxConcurrentProcedure Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRC.MaxConcurrentProcedure";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRC.PagingNum Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRC.PagingNum";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRC.RrcConnRejWaitTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRC.RrcConnRejWaitTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRC.RrcCounterCheckTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRC.RrcCounterCheckTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRC.RxMaxTimesCheckFlg Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRC.RxMaxTimesCheckFlg";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRC.DiscardCheckPeriodic Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_RRC.DiscardCheckPeriodic";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.ModifiPeriodCoeff Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.ModifiPeriodCoeff";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.SiWindow Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.SiWindow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib2.SibEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib2.SibEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib2.PeriodicityLow Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib2.PeriodicityLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib2.PeriodicityHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib2.PeriodicityHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib3.SibEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib3.SibEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib3.PeriodicityLow Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib3.PeriodicityLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib3.PeriodicityHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib3.PeriodicityHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib4.SibEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib4.SibEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib4.PeriodicityLow Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib4.PeriodicityLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib4.PeriodicityHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib4.PeriodicityHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib5.SibEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib5.SibEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib5.PeriodicityLow Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib5.PeriodicityLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib5.PeriodicityHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib5.PeriodicityHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib6.SibEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib6.SibEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib6.PeriodicityLow Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib6.PeriodicityLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib6.PeriodicityHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib6.PeriodicityHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib7.SibEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib7.SibEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib7.PeriodicityLow Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib7.PeriodicityLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib7.PeriodicityHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib7.PeriodicityHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib8.SibEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib8.SibEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib8.PeriodicityLow Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib8.PeriodicityLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib8.PeriodicityHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib8.PeriodicityHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib9.SibEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib9.SibEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib9.PeriodicityLow Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib9.PeriodicityLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib9.PeriodicityHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib9.PeriodicityHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib10.SibEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib10.SibEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib10.PeriodicityLow Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib10.PeriodicityLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib10.PeriodicityHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib10.PeriodicityHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib11.SibEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib11.SibEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib11.PeriodicityLow Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib11.PeriodicityLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib11.PeriodicityHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib11.PeriodicityHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib12.SibEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib12.SibEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib12.PeriodicityLow Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib12.PeriodicityLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib12.PeriodicityHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib12.PeriodicityHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib13.SibEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib13.SibEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib13.PeriodicityLow Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib13.PeriodicityLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.SysInfoConfig.Sib13.PeriodicityHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.X_BC0304_SysInfoConfig.Sib13.PeriodicityHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRCTimers.T300 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RRCTimers.T300";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRCTimers.T301 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RRCTimers.T301";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRCTimers.T302 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RRCTimers.T302";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRCTimers.T304EUTRA Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RRCTimers.T304EUTRA";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRCTimers.T304IRAT Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RRCTimers.T304IRAT";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRCTimers.T310 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RRCTimers.T310";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRCTimers.T311 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RRCTimers.T311";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRCTimers.T320 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RRCTimers.T320";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRCTimers.N310 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RRCTimers.N310";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.RRCTimers.N311 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.RRCTimers.N311";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.CellBarred Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.CellBarred";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.IntraFreqReselection Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_IntraFreqReselection";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.BarringForEmergency Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.BarringForEmergency";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Ext.CellBarred Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Ext.CellBarred";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.BarringForMOSignalling.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_BarringForMOSignalling.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.BarringForMOSignalling.BarringFactor Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_BarringForMOSignalling.BarringFactor";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.BarringForMOSignalling.BarringTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_BarringForMOSignalling.BarringTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.BarringForMOSignalling.BarringForSpecialAC Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_BarringForMOSignalling.BarringForSpecialAC";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.BarringForMOData.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_BarringForMOData.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.BarringForMOData.BarringFactor Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_BarringForMOData.BarringFactor";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.BarringForMOData.BarringTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_BarringForMOData.BarringTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.BarringForMOData.BarringForSpecialAC Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_BarringForMOData.BarringForSpecialAC";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.BarringForMMTELVoice.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_BarringForMMTELVoice.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.BarringForMMTELVoice.BarringFactor Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_BarringForMMTELVoice.BarringFactor";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.BarringForMMTELVoice.BarringTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_BarringForMMTELVoice.BarringTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.BarringForMMTELVoice.BarringForSpecialAC Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_BarringForMMTELVoice.BarringForSpecialAC";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.BarringForMMTELVideo.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_BarringForMMTELVideo.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.BarringForMMTELVideo.BarringFactor Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_BarringForMMTELVideo.BarringFactor";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.BarringForMMTELVideo.BarringTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_BarringForMMTELVideo.BarringTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.BarringForMMTELVideo.BarringForSpecialAC Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_BarringForMMTELVideo.BarringForSpecialAC";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.BarringForCSFB.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_BarringForCSFB.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.BarringForCSFB.BarringFactor Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_BarringForCSFB.BarringFactor";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.BarringForCSFB.BarringTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_BarringForCSFB.BarringTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.BarringForCSFB.BarringForSpecialAC Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_BarringForCSFB.BarringForSpecialAC";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Congestion.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Congestion.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Congestion.CellBarred Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Congestion.CellBarred";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Congestion.BarringForEmergency Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Congestion.BarringForEmergency";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Congestion.BarringForMOSignalling.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Congestion.BarringForMOSignalling.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Congestion.BarringForMOSignalling.BarringFactor Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Congestion.BarringForMOSignalling.BarringFactor";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Congestion.BarringForMOSignalling.BarringTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Congestion.BarringForMOSignalling.BarringTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Congestion.BarringForMOSignalling.BarringForSpecialAC Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Congestion.BarringForMOSignalling.BarringForSpecialAC";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Congestion.BarringForMOData.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Congestion.BarringForMOData.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Congestion.BarringForMOData.BarringFactor Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Congestion.BarringForMOData.BarringFactor";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Congestion.BarringForMOData.BarringTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Congestion.BarringForMOData.BarringTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Congestion.BarringForMOData.BarringForSpecialAC Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Congestion.BarringForMOData.BarringForSpecialAC";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Congestion.BarringForMMTELVoice.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Congestion.BarringForMMTELVoice.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Congestion.BarringForMMTELVoice.BarringFactor Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Congestion.BarringForMMTELVoice.BarringFactor";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Congestion.BarringForMMTELVoice.BarringTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Congestion.BarringForMMTELVoice.BarringTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Congestion.BarringForMMTELVoice.BarringForSpecialAC Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Congestion.BarringForMMTELVoice.BarringForSpecialAC";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Congestion.BarringForMMTELVideo.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Congestion.BarringForMMTELVideo.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Congestion.BarringForMMTELVideo.BarringFactor Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Congestion.BarringForMMTELVideo.BarringFactor";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Congestion.BarringForMMTELVideo.BarringTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Congestion.BarringForMMTELVideo.BarringTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Congestion.BarringForMMTELVideo.BarringForSpecialAC Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Congestion.BarringForMMTELVideo.BarringForSpecialAC";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Congestion.BarringForCSFB.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Congestion.BarringForCSFB.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Congestion.BarringForCSFB.BarringFactor Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Congestion.BarringForCSFB.BarringFactor";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Congestion.BarringForCSFB.BarringTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Congestion.BarringForCSFB.BarringTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.Congestion.BarringForCSFB.BarringForSpecialAC Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.CellRestriction.X_BC0304_Congestion.BarringForCSFB.BarringForSpecialAC";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.Qhyst Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.Qhyst";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.QHystSFMedium Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.QHystSFMedium";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.QHystSFHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.QHystSFHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.TEvaluation Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.TEvaluation";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.THystNormal Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.THystNormal";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.NCellChangeMedium Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.NCellChangeMedium";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.NCellChangeHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.NCellChangeHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.QRxLevMinSIB5 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.X_BC0304_QRxLevMinSIB5";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.QOffsetFreq Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.X_BC0304_QOffsetFreq";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.TReselectionEUTRA Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.X_BC0304_TReselectionEUTRA";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.CellReselectionPriority Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.X_BC0304_CellReselectionPriority";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.ThreshXHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.X_BC0304_ThreshXHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.ThreshXLow Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.X_BC0304_ThreshXLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.PMax Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.X_BC0304_PMax";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.TReselectionEUTRASFMedium Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.X_BC0304_TReselectionEUTRASFMedium";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.TReselectionEUTRASFHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.X_BC0304_TReselectionEUTRASFHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.QQualMinEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.X_BC0304_QQualMinEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.QQualMin Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.X_BC0304_QQualMin";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.ThreshXHighQ Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.X_BC0304_ThreshXHighQ";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.ThreshXLowQ Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.X_BC0304_ThreshXLowQ";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.AllowedMeasBandwidth Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.X_BC0304_AllowedMeasBandwidth";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.PresenceAntennaPort1 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.Common.X_BC0304_PresenceAntennaPort1";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.QRxLevMinSIB1 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.QRxLevMinSIB1";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.QRxLevMinSIB3 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.QRxLevMinSIB3";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.QRxLevMinOffset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.QRxLevMinOffset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.SIntraSearch Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.SIntraSearch";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.TReselectionEUTRA Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.TReselectionEUTRA";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.SNonIntraSearch Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.SNonIntraSearch";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.CellReselectionPriority Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.CellReselectionPriority";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.PMax Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.PMax";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.PMaxOffset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.X_BC0304_PMaxOffset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.ThreshServingLow Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.ThreshServingLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.TReselectionEUTRASFMedium Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.TReselectionEUTRASFMedium";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.TReselectionEUTRASFHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.TReselectionEUTRASFHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.SIntraSearchEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.X_BC0304_SIntraSearchEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.SIntraSearchP Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.X_BC0304_SIntraSearchP";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.SIntraSearchQ Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.X_BC0304_SIntraSearchQ";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.SNonIntraSearchEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.X_BC0304_SNonIntraSearchEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.SNonIntraSearchP Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.X_BC0304_SNonIntraSearchP";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.SNonIntraSearchQ Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.X_BC0304_SNonIntraSearchQ";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.QQualMinSib3Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.X_BC0304_QQualMinSib3Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.QQualMinSib3 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.X_BC0304_QQualMinSib3";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.ThreshServingLowQEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.X_BC0304_ThreshServingLowQEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.ThreshServingLowQ Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.X_BC0304_ThreshServingLowQ";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.QQualMinSib1Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.X_BC0304_QQualMinSib1Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.QQualMinSib1 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.X_BC0304_QQualMinSib1";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.QQualMinOffset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IntraFreq.X_BC0304_QQualMinOffset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.Alias Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.Alias";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.EUTRACarrierARFCN Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.EUTRACarrierARFCN";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.QRxLevMinSIB5 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.QRxLevMinSIB5";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.QOffsetFreq Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.QOffsetFreq";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.TReselectionEUTRA Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.TReselectionEUTRA";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.CellReselectionPriority Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.CellReselectionPriority";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.ThreshXHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.ThreshXHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.ThreshXLow Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.ThreshXLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.PMax Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.PMax";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.TReselectionEUTRASFMedium Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.TReselectionEUTRASFMedium";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.TReselectionEUTRASFHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.TReselectionEUTRASFHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.QQualMinEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.X_BC0304_QQualMinEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.QQualMin Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.X_BC0304_QQualMin";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.ThreshXHighQ Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.X_BC0304_ThreshXHighQ";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.ThreshXLowQ Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.X_BC0304_ThreshXLowQ";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.AllowedMeasBandwidth Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.X_BC0304_AllowedMeasBandwidth";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.PresenceAntennaPort1 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.InterFreq.Carrier.X_BC0304_PresenceAntennaPort1";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.TReselectionUTRA Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.TReselectionUTRA";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.TReselectionUTRASFMedium Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.TReselectionUTRASFMedium";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.TReselectionUTRASFHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.TReselectionUTRASFHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDD. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANFDD.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDD.QRxLevMin Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANFDD.QRxLevMin";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDD.QQualMin Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANFDD.QQualMin";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDD.CellReselectionPriority Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANFDD.CellReselectionPriority";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDD.ThreshXHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANFDD.ThreshXHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDD.ThreshXLow Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANFDD.ThreshXLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDD.PMaxUTRA Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANFDD.PMaxUTRA";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDD.ThreshXQEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANFDD.ThreshXQEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDD.ThreshXHighQ Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANFDD.ThreshXHighQ";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDD.ThreshXLowQ Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANFDD.ThreshXLowQ";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANTDD. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANTDD.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANTDD.QRxLevMin Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANTDD.QRxLevMin";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANTDD.QQualMin Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANTDD.QQualMin";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANTDD.CellReselectionPriority Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANTDD.CellReselectionPriority";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANTDD.ThreshXHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANTDD.ThreshXHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANTDD.ThreshXLow Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANTDD.ThreshXLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANTDD.PMaxUTRA Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANTDD.PMaxUTRA";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.Alias Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.Alias";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.UTRACarrierARFCN Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.UTRACarrierARFCN";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.QRxLevMin Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.QRxLevMin";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.QQualMin Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.QQualMin";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.CellReselectionPriority Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.CellReselectionPriority";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.ThreshXHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.ThreshXHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.ThreshXLow Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.ThreshXLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.PMaxUTRA Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.PMaxUTRA";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.ThreshXQEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.X_BC0304_ThreshXQEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.ThreshXHighQ Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.X_BC0304_ThreshXHighQ";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.ThreshXLowQ Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANFDDFreq.X_BC0304_ThreshXLowQ";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANTDDFreq. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANTDDFreq.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANTDDFreq.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANTDDFreq.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANTDDFreq.Alias Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANTDDFreq.Alias";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANTDDFreq.UTRACarrierARFCN Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANTDDFreq.UTRACarrierARFCN";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANTDDFreq.QRxLevMin Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANTDDFreq.QRxLevMin";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANTDDFreq.QQualMin Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANTDDFreq.QQualMin";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANTDDFreq.CellReselectionPriority Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANTDDFreq.CellReselectionPriority";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANTDDFreq.ThreshXHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANTDDFreq.ThreshXHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANTDDFreq.ThreshXLow Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANTDDFreq.ThreshXLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.UTRANTDDFreq.PMaxUTRA Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.UTRA.X_BC0304_UTRANTDDFreq.PMaxUTRA";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.TReselectionGERAN Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.TReselectionGERAN";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.TReselectionGERANSFMedium Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.TReselectionGERANSFMedium";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.TReselectionGERANSFHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.TReselectionGERANSFHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.QRxLevMin Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.X_BC0304_QRxLevMin";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.CellReselectionPriority Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.X_BC0304_CellReselectionPriority";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.ThreshXHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.X_BC0304_ThreshXHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.ThreshXLow Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.X_BC0304_ThreshXLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.PMaxGERAN Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.X_BC0304_PMaxGERAN";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.GERANFreqGroup. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.GERANFreqGroup.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.GERANFreqGroup.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.GERANFreqGroup.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.GERANFreqGroup.Alias Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.GERANFreqGroup.Alias";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.GERANFreqGroup.BandIndicator Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.GERANFreqGroup.BandIndicator";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.GERANFreqGroup.BCCHARFCN Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.GERANFreqGroup.BCCHARFCN";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.GERANFreqGroup.QRxLevMin Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.GERANFreqGroup.QRxLevMin";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.GERANFreqGroup.CellReselectionPriority Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.GERANFreqGroup.CellReselectionPriority";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.GERANFreqGroup.ThreshXHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.GERANFreqGroup.ThreshXHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.GERANFreqGroup.ThreshXLow Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.GERANFreqGroup.ThreshXLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.GERANFreqGroup.PMaxGERAN Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.GERAN.GERANFreqGroup.X_BC0304_PMaxGERAN";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.CDMA2000.SearchWindowSize Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.CDMA2000.SearchWindowSize";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.CDMA2000.TReselectionCDMA2000 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.CDMA2000.TReselectionCDMA2000";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.CDMA2000.TReselectionCDMA2000SFMedium Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.CDMA2000.TReselectionCDMA2000SFMedium";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.CDMA2000.TReselectionCDMA2000SFHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.CDMA2000.TReselectionCDMA2000SFHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.CDMA2000.CDMA2000Band. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.CDMA2000.CDMA2000Band.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.CDMA2000.CDMA2000Band.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.CDMA2000.CDMA2000Band.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.CDMA2000.CDMA2000Band.Alias Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.CDMA2000.CDMA2000Band.Alias";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.CDMA2000.CDMA2000Band.BandClass Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.CDMA2000.CDMA2000Band.BandClass";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.CDMA2000.CDMA2000Band.CellReselectionPriority Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.CDMA2000.CDMA2000Band.CellReselectionPriority";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.CDMA2000.CDMA2000Band.ThreshXHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.CDMA2000.CDMA2000Band.ThreshXHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.CDMA2000.CDMA2000Band.ThreshXLow Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.IdleMode.IRAT.CDMA2000.CDMA2000Band.ThreshXLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.FilterCoefficientRSRP Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.FilterCoefficientRSRP";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.FilterCoefficientRSRQ Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.FilterCoefficientRSRQ";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A1ThresholdRSRP Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A1ThresholdRSRP";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A1ThresholdRSRQ Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A1ThresholdRSRQ";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A2ThresholdRSRP Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A2ThresholdRSRP";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A2ThresholdRSRQ Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A2ThresholdRSRQ";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A3Offset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A3Offset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.ReportOnLeave Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.ReportOnLeave";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A4ThresholdRSRP Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A4ThresholdRSRP";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A4ThresholdRSRQ Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A4ThresholdRSRQ";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A5Threshold1RSRP Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A5Threshold1RSRP";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A5Threshold1RSRQ Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A5Threshold1RSRQ";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A5Threshold2RSRP Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A5Threshold2RSRP";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A5Threshold2RSRQ Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A5Threshold2RSRQ";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A6Offset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_A6Offset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A6ReportOnLeave Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_A6ReportOnLeave";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.Hysteresis Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.Hysteresis";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.TimeToTrigger Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.TimeToTrigger";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A1Hysteresis Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_A1Hysteresis";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A1TimeToTrigger Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_A1TimeToTrigger";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A2Hysteresis Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_A2Hysteresis";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A2TimeToTrigger Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_A2TimeToTrigger";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A3Hysteresis Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_A3Hysteresis";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A3TimeToTrigger Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_A3TimeToTrigger";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A4Hysteresis Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_A4Hysteresis";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A4TimeToTrigger Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_A4TimeToTrigger";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A5Hysteresis Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_A5Hysteresis";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A5TimeToTrigger Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_A5TimeToTrigger";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A6Hysteresis Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_A6Hysteresis";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.A6TimeToTrigger Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_A6TimeToTrigger";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.TriggerQuantity Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.TriggerQuantity";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.ReportQuantity Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.ReportQuantity";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.ReportInterval Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.ReportInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.ReportAmount Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.ReportAmount";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.GapPattern Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_GapPattern";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.SMeasure Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_SMeasure";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.QOffsetFreq Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_QOffsetFreq";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.IntraFreqHandoverPermit Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_IntraFreqHandoverPermit";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.InterFreq. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_InterFreq.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.InterFreq.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_InterFreq.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.InterFreq.EUTRACarrierARFCN Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_InterFreq.EUTRACarrierARFCN";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.InterFreq.QOffsetFreq Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_InterFreq.QOffsetFreq";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.InterFreq.AllowedMeasBandwidth Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_InterFreq.AllowedMeasBandwidth";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.InterFreq.PresenceAntennaPort1 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.EUTRA.X_BC0304_InterFreq.PresenceAntennaPort1";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.QoffsettUTRA Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.QoffsettUTRA";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.QoffsettUTRATDD Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.X_BC0304_QoffsettUTRATDD";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.FilterCoefficientUTRA Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.FilterCoefficientUTRA";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.MeasQuantityUTRAFDD Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.MeasQuantityUTRAFDD";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.B1ThresholdUTRARSCP Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.B1ThresholdUTRARSCP";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.B1ThresholdUTRAEcN0 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.B1ThresholdUTRAEcN0";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.QoffsetGERAN Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.QoffsetGERAN";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.FilterCoefficientGERAN Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.FilterCoefficientGERAN";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.B1ThresholdGERAN Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.B1ThresholdGERAN";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.QoffsetCDMA2000 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.QoffsetCDMA2000";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.MeasQuantityCDMA2000 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.MeasQuantityCDMA2000";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.B1ThresholdCDMA2000 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.B1ThresholdCDMA2000";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.B2Threshold1EUTRARSRP Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.X_BC0304_B2Threshold1EUTRARSRP";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.B2Threshold1EUTRARSRQ Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.X_BC0304_B2Threshold1EUTRARSRQ";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.B2Threshold2UTRARSCP Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.B2Threshold2UTRARSCP";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.B2Threshold2UTRAEcN0 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.B2Threshold2UTRAEcN0";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.B2Threshold2GERAN Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.B2Threshold2GERAN";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.B2Threshold2CDMA2000 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.B2Threshold2CDMA2000";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.Hysteresis Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.Hysteresis";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.TimeToTrigger Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.TimeToTrigger";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.B1Hysteresis Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.X_BC0304_B1Hysteresis";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.B1TimeToTrigger Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.X_BC0304_B1TimeToTrigger";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.B2Hysteresis Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.X_BC0304_B2Hysteresis";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.B2TimeToTrigger Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.X_BC0304_B2TimeToTrigger";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.ReportInterval Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.ReportInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.ReportAmount Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.ConnMode.IRAT.ReportAmount";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.SPID. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.X_BC0304_SPID.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.SPID.SPID Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.X_BC0304_SPID.SPID";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.SPID.EUTRANCarriersPriority Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.X_BC0304_SPID.EUTRANCarriersPriority";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.SPID.UTRANCarriersPriority Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.X_BC0304_SPID.UTRANCarriersPriority";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.SPID.GERANCarriersPriority Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.Mobility.X_BC0304_SPID.GERANCarriersPriority";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.MaxLTECellEntries Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.MaxLTECellEntries";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.Alias Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.Alias";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.MustInclude Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.MustInclude";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.NoRemove Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.X_BC0304_NoRemove";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.NoX2 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.X_BC0304_NoX2";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.NoX2HO Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.X_BC0304_NoX2HO";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.PLMNID Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.PLMNID";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.CellType Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.X_BC0304_CellType";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.CID Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.CID";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.TAC Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.X_BC0304_TAC";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.AccessMode Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.X_BC0304_AccessMode";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.CSGID Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.X_BC0304_CSGID";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.EUTRACarrierARFCN Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.EUTRACarrierARFCN";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.PhyCellID Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.PhyCellID";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.QOffset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.QOffset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.CIO Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.CIO";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.RSTxPower Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.RSTxPower";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.Blacklisted Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.Blacklisted";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.BlacklistedSIB Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.X_BC0304_BlacklistedSIB";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.AntennaPortsCount Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.X_BC0304_AntennaPortsCount";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.DLBandwidth Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.X_BC0304_DLBandwidth";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.SubFrameAssignment Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.X_BC0304_SubFrameAssignment";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.SpecialSubframePatterns Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.X_BC0304_SpecialSubframePatterns";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.PLMNList2. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.X_BC0304_PLMNList2.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.PLMNList2.PLMNID Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.LTECell.X_BC0304_PLMNList2.PLMNID";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.MaxUMTSEntries Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.MaxUMTSEntries";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.MaxTDSCDMAEntries Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.X_BC0304_MaxTDSCDMAEntries";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.MaxGSMEntries Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.MaxGSMEntries";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.MaxCDMA2000Entries Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.MaxCDMA2000Entries";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.Alias Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.Alias";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.MustInclude Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.MustInclude";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.PLMNID Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.PLMNID";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.RNCID Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.RNCID";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.CID Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.CID";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.LAC Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.LAC";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.RAC Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.RAC";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.URA Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.URA";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.UARFCNUL Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.UARFCNUL";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.UARFCNDL Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.UARFCNDL";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.PCPICHScramblingCode Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.PCPICHScramblingCode";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.PCPICHTxPower Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.UMTS.PCPICHTxPower";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.TDSCDMA. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.X_BC0304_TDSCDMA.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.TDSCDMA.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.X_BC0304_TDSCDMA.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.TDSCDMA.Alias Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.X_BC0304_TDSCDMA.Alias";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.TDSCDMA.MustInclude Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.X_BC0304_TDSCDMA.MustInclude";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.TDSCDMA.PLMNID Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.X_BC0304_TDSCDMA.PLMNID";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.TDSCDMA.RNCID Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.X_BC0304_TDSCDMA.RNCID";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.TDSCDMA.CID Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.X_BC0304_TDSCDMA.CID";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.TDSCDMA.LAC Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.X_BC0304_TDSCDMA.LAC";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.TDSCDMA.RAC Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.X_BC0304_TDSCDMA.RAC";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.TDSCDMA.URA Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.X_BC0304_TDSCDMA.URA";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.TDSCDMA.UARFCN Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.X_BC0304_TDSCDMA.UARFCN";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.TDSCDMA.CellParametersId Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.X_BC0304_TDSCDMA.CellParametersId";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.GSM. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.GSM.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.GSM.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.GSM.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.GSM.Alias Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.GSM.Alias";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.GSM.MustInclude Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.GSM.MustInclude";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.GSM.PLMNID Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.GSM.PLMNID";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.GSM.LAC Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.GSM.LAC";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.GSM.BSIC Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.GSM.BSIC";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.GSM.CI Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.GSM.CI";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.GSM.BandIndicator Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.GSM.BandIndicator";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.GSM.BCCHARFCN Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.GSM.BCCHARFCN";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.CDMA2000. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.CDMA2000.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.CDMA2000.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.CDMA2000.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.CDMA2000.Alias Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.CDMA2000.Alias";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.CDMA2000.MustInclude Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.CDMA2000.MustInclude";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.CDMA2000.BandClass Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.CDMA2000.BandClass";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.CDMA2000.ARFCN Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.CDMA2000.ARFCN";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.CDMA2000.PNOffset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.CDMA2000.PNOffset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.CDMA2000.Type Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.CDMA2000.Type";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.CDMA2000.CID Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborList.InterRATCell.CDMA2000.CID";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborListInUse.LTECell.NoX2HO Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborListInUse.LTECell.X_BC0304_NoX2HO";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.CellConfig.LTE.RAN.NeighborListInUse.InterRATCell.TDSCDMA.CellParametersId Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.CellConfig.LTE.RAN.NeighborListInUse.InterRATCell.X_BC0304_TDSCDMA.CellParametersId";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.WCDMA.InServiceHandling Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.WCDMA.InServiceHandling";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.WCDMA.ScanOnBoot Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.WCDMA.ScanOnBoot";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.WCDMA.ScanPeriodically Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.WCDMA.ScanPeriodically";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.WCDMA.PeriodicInterval Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.WCDMA.PeriodicInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.WCDMA.PeriodicTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.WCDMA.PeriodicTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.WCDMA.REMPLMNList Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.WCDMA.REMPLMNList";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.WCDMA.REMBandList Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.WCDMA.REMBandList";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.WCDMA.UARFCNDLList Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.WCDMA.UARFCNDLList";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.WCDMA.ScanTimeout Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.WCDMA.ScanTimeout";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.TDSCDMA.InServiceHandling Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.X_BC0304_TDSCDMA.InServiceHandling";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.TDSCDMA.ScanOnBoot Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.X_BC0304_TDSCDMA.ScanOnBoot";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.TDSCDMA.ScanPeriodically Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.X_BC0304_TDSCDMA.ScanPeriodically";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.TDSCDMA.PeriodicInterval Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.X_BC0304_TDSCDMA.PeriodicInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.TDSCDMA.PeriodicTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.X_BC0304_TDSCDMA.PeriodicTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.TDSCDMA.REMPLMNList Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.X_BC0304_TDSCDMA.REMPLMNList";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.TDSCDMA.REMBandList Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.X_BC0304_TDSCDMA.REMBandList";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.TDSCDMA.UARFCNList Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.X_BC0304_TDSCDMA.UARFCNList";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.TDSCDMA.ScanTimeout Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.X_BC0304_TDSCDMA.ScanTimeout";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.GSM.InServiceHandling Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.GSM.InServiceHandling";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.GSM.ScanOnBoot Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.GSM.ScanOnBoot";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.GSM.ScanPeriodically Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.GSM.ScanPeriodically";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.GSM.PeriodicInterval Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.GSM.PeriodicInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.GSM.PeriodicTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.GSM.PeriodicTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.GSM.REMPLMNList Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.GSM.REMPLMNList";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.GSM.REMBandList Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.GSM.REMBandList";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.GSM.ARFCNList Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.GSM.ARFCNList";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.UMTS.GSM.ScanTimeout Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.UMTS.GSM.ScanTimeout";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.LTE.InServiceHandling Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.LTE.InServiceHandling";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.LTE.ScanOnBoot Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.LTE.ScanOnBoot";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.LTE.ScanPeriodically Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.LTE.ScanPeriodically";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.LTE.PeriodicInterval Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.LTE.PeriodicInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.LTE.PeriodicTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.LTE.PeriodicTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.LTE.REMPLMNList Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.LTE.REMPLMNList";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.LTE.REMBandList Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.LTE.REMBandList";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.LTE.EUTRACarrierARFCNDLList Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.LTE.EUTRACarrierARFCNDLList";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.LTE.ScanTimeout Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.LTE.ScanTimeout";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.LTE.CarrierMeas.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.LTE.CarrierMeas.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.LTE.CarrierMeas.Alias Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.LTE.CarrierMeas.Alias";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.LTE.CarrierMeas.CarrierARFCNDL Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.LTE.CarrierMeas.CarrierARFCNDL";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.REM.LTE.CarrierMeas.CarrierChWidth Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.REM.LTE.CarrierMeas.CarrierChWidth";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.Transport.SCTP.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.Transport.SCTP.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.Transport.SCTP.HBInterval Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.Transport.SCTP.HBInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.Transport.SCTP.MaxAssociationRetransmits Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.Transport.SCTP.MaxAssociationRetransmits";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.Transport.SCTP.MaxInitRetransmits Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.Transport.SCTP.MaxInitRetransmits";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.Transport.SCTP.MaxPathRetransmits Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.Transport.SCTP.MaxPathRetransmits";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.Transport.SCTP.RTOInitial Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.Transport.SCTP.RTOInitial";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.Transport.SCTP.RTOMax Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.Transport.SCTP.RTOMax";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.Transport.SCTP.RTOMin Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.Transport.SCTP.RTOMin";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.Transport.SCTP.ValCookieLife Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.Transport.SCTP.ValCookieLife";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.Transport.SCTP.LocalPortNum Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.Transport.SCTP.X_BC0304_LocalPortNum";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.Transport.SCTP.OutBandStreamCount Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.Transport.SCTP.X_BC0304_OutBandStreamCount";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.Transport.SCTP.InBandStreamCount Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.Transport.SCTP.X_BC0304_InBandStreamCount";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.Transport.SCTP.DscpInfo Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.Transport.SCTP.X_BC0304_DscpInfo";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.Transport.SCTP.MaxStreamNum Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.Transport.SCTP.X_BC0304_MaxStreamNum";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.Transport.SCTP.S1AssocMaxNum Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.Transport.SCTP.X_BC0304_S1AssocMaxNum";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.Transport.SCTP.S1AllowedConnMaxNum Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.Transport.SCTP.X_BC0304_S1AllowedConnMaxNum";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.Transport.SCTP.X2AssocMaxNum Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.Transport.SCTP.X_BC0304_X2AssocMaxNum";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.Transport.SCTP.X2AllowedConnMaxNum Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.Transport.SCTP.X_BC0304_X2AllowedConnMaxNum";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.Transport.Packet.EchoInterval Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.Transport.Packet.EchoInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.L2Para.DlSigMailboxLen Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_L2Para.DlSigMailboxLen";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.L2Para.UlSigMailboxLen Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_L2Para.UlSigMailboxLen";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.L2Para.DlDataMailboxLen Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_L2Para.DlDataMailboxLen";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.L2Para.UlDataMailboxLen Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_L2Para.UlDataMailboxLen";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.L2Para.MemCfgFilePath Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_L2Para.MemCfgFilePath";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.L2Para.InitDspByPs Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_L2Para.InitDspByPs";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.L2Para.DlRateLimitFlg Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_L2Para.DlRateLimitFlg";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.L2Para.stepSize Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_L2Para.stepSize";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.L2Para.highThr Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_L2Para.highThr";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.L2Para.highThrCnt Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_L2Para.highThrCnt";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.L2Para.lowThr Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_L2Para.lowThr";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.L2Para.lowThrCnt Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_L2Para.lowThrCnt";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.L2Para.minProcessSduCnt Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_L2Para.minProcessSduCnt";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.L2Para.maxStep Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_L2Para.maxStep";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.L2Para.maxProcessMsgCnt Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_L2Para.maxProcessMsgCnt";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.L2Para.qci Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_L2Para.qci";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.L2Para.CodeToFlgTbl Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_L2Para.CodeToFlgTbl";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.X2.X2Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_X2.X2Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.X2.X2EnbIdListPresent Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_X2.X2EnbIdListPresent";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.X2.X2EnbIdList Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_X2.X2EnbIdList";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.X2.X2SigLinkPort Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_X2.X2SigLinkPort";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.X2.EnbConfigTransTimerLength Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_X2.EnbConfigTransTimerLength";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.X2.X2SetupRetryCount Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_X2.X2SetupRetryCount";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.REM.CellSearchRetryCount Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.REM.CellSearchRetryCount";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.REM.EutraSib1RetryCount Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.REM.EutraSib1RetryCount";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.REM.EutraSibRetryCount Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.REM.EutraSibRetryCount";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.REM.SibReadProtectTimerLen Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.REM.SibReadProtectTimerLen";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.REM.RTWPThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.REM.RTWPThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.REM.NeighborRSRPThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.REM.NeighborRSRPThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.REM.CellLockNeighborRSRPHigh Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.REM.CellLockNeighborRSRPHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.REM.ContinuousREMProhibitInterval Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.REM.ContinuousREMProhibitInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.FreqSync.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.FreqSync.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.FreqSync.FirstPrioritySyncSource Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.FreqSync.FirstPrioritySyncSource";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.FreqSync.SecondPrioritySyncSource Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.FreqSync.SecondPrioritySyncSource";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.FreqSync.ThirdPrioritySyncSource Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.FreqSync.ThirdPrioritySyncSource";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.FreqSync.SyncTimerLength Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.FreqSync.SyncTimerLength";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.FreqSync.Periodically Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.FreqSync.Periodically";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.FreqSync.PeriodicInterval Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.FreqSync.PeriodicInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.FreqSync.Priority Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.FreqSync.Priority";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.FreqSync.SuccessTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.FreqSync.SuccessTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.FreqSync.AFCValue Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.FreqSync.AFCValue";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.FreqSync.AFCCorrectionTimeThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.FreqSync.AFCCorrectionTimeThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.FreqSync.MaxAllowedDriftPPM Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.FreqSync.MaxAllowedDriftPPM";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.FreqSync.TCXOAgeingCoff Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.FreqSync.TCXOAgeingCoff";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.FreqSync.DACPerPPM Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.FreqSync.DACPerPPM";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.FreqSync.DACStep Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.FreqSync.DACStep";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.FreqSync.RSRPThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.FreqSync.RSRPThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.FreqSync.ECIOThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.FreqSync.ECIOThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.FreqSync.SNRThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.FreqSync.SNRThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.FreqSync.FreqOffsetThreshold1 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.FreqSync.FreqOffsetThreshold1";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.FreqSync.FreqOffsetThreshold2 Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.FreqSync.FreqOffsetThreshold2";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.FreqSync.TimeAlignEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.FreqSync.TimeAlignEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.FreqSync.TimeOutOfSyncReportTimer Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.FreqSync.TimeOutOfSyncReportTimer";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.SelfConfig.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.SelfConfig.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.SelfConfig.Persistency Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.SelfConfig.Persistency";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.SelfConfig.TimeThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.SelfConfig.TimeThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.TPM.EARFCNDLToProtect Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.TPM.EARFCNDLToProtect";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.TPM.UARFCNDLToProtect Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.TPM.UARFCNDLToProtect";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.ANR.UEPeriodicANREnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.ANR.UEPeriodicANREnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.ANR.UEPeriodicANRUENumThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.ANR.UEPeriodicANRUENumThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.ANR.UEPeriodicANRMeasObject Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.ANR.UEPeriodicANRMeasObject";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.ANR.MeasurementTimerLength Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.ANR.MeasurementTimerLength";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.ANR.ProhibitTimerLength Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.ANR.ProhibitTimerLength";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.ANR.HomeEnbEarfcnRange Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.ANR.HomeEnbEarfcnRange";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.ANR.HomeEnbPciRange Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.ANR.HomeEnbPciRange";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.ANR.LTENrInactTimerLength Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.ANR.LTENrInactTimerLength";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.ANR.InterRATNrInactTimerLength Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.ANR.InterRATNrInactTimerLength";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.PCI.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.PCI.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.PCI.PCIConfusionHoFailureThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.PCI.PCIConfusionHoFailureThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.FHM.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.FHM.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.RIP.RIPThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.RIP.RIPThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.DB.AutoFlushEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.DB.AutoFlushEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.DB.AutoFlushPeriod Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.DB.AutoFlushPeriod";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.DB.SonDmGlobal Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.DB.SonDmGlobal";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.Debug.PeriodMeasEnable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.Debug.PeriodMeasEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.Debug.UeId Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.Debug.UeId";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.Services.FAPService.SON.Debug.UeHistory.CellIndex Handler.
    pPara->paParaPath[0] = (INT8*)"Device.Services.FAPService.X_BC0304_SON.Debug.UeHistory.CellIndex";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DeviceInfo.TemperatureStatus.TemperatureSensor.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.TemperatureStatus.TemperatureSensor.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DeviceInfo.TemperatureStatus.TemperatureSensor.Reset Handler.
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.TemperatureStatus.TemperatureSensor.Reset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DeviceInfo.TemperatureStatus.TemperatureSensor.LowAlarmValue Handler.
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.TemperatureStatus.TemperatureSensor.LowAlarmValue";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DeviceInfo.TemperatureStatus.TemperatureSensor.HighAlarmValue Handler.
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.TemperatureStatus.TemperatureSensor.HighAlarmValue";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DeviceInfo.TemperatureStatus.TemperatureSensor.PollingInterval Handler.
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.TemperatureStatus.TemperatureSensor.PollingInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DeviceInfo.TemperatureStatus.TemperatureSensor.AlarmOffMargin Handler.
    pPara->paParaPath[0] = (INT8*)"Device.DeviceInfo.TemperatureStatus.TemperatureSensor.X_BC0304_AlarmOffMargin";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
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
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.TBDump.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.TBDump.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.TBDump.HostIPAddress Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.TBDump.HostIPAddress";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.DebugMgmt.TtrTest.UeS1apId Handler.
    pPara->paParaPath[0] = (INT8*)"Device.X_BC0304_DebugMgmt.TtrTest.UeS1apId";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PsNotifyInd;
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
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave PsDeviceNotifyRegisterHandler");
    return bRtn;
}
