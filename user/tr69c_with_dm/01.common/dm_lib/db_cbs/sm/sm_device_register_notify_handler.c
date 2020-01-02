

#include "dm_internal.h"
#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif
extern void SmNotifyInd(DM_NOTIFY_T *pPara);

/*******************************************************************************
  Module   : SmDeviceNotifyRegisterHandler                                                   
  Function : register notify handler                                            
  Input    : void                                                               
  Ounput   : BOOL8: DM_TRUE/DM_FALSE                                          
  Note     : null                                                               
*******************************************************************************/
BOOL8 SmDeviceNotifyRegisterHandler(void)
{
    DM_NOTIFY_HANDLER_REG_PARA_T    *pPara;
    BOOL8                           bRtn = DM_TRUE;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter SmDeviceNotifyRegisterHandler");
    pPara = (DM_NOTIFY_HANDLER_REG_PARA_T *)malloc(sizeof(DM_NOTIFY_HANDLER_REG_PARA_T));

    if(pPara == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ SmDeviceNotifyRegisterHandler malloc() failed for pPara");
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara, 0, sizeof(DM_NOTIFY_HANDLER_REG_PARA_T));

    pPara->lParaCnt = 1;
    pPara->paParaPath = (INT8 **)malloc(sizeof(INT8 *)*pPara->lParaCnt);

    if(pPara->paParaPath == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ SmDeviceNotifyRegisterHandler malloc() failed for pPara->paParaPath");
        DM_FREE(pPara);
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara->paParaPath, 0, pPara->lParaCnt);

    //Register Notify Device.IPsec.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.PassThrough. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.X_BC0304_PassThrough.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.PassThrough.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.X_BC0304_PassThrough.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.PassThrough.Destination Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.X_BC0304_PassThrough.Destination";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Filter. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Filter.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Filter.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Filter.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Filter.Order Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Filter.Order";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Filter.Alias Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Filter.Alias";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Filter.Interface Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Filter.Interface";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Filter.AllInterfaces Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Filter.AllInterfaces";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Filter.DestIP Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Filter.DestIP";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Filter.DestMask Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Filter.DestMask";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Filter.DestIPExclude Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Filter.DestIPExclude";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Filter.SourceIP Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Filter.SourceIP";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Filter.SourceMask Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Filter.SourceMask";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Filter.SourceIPExclude Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Filter.SourceIPExclude";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Filter.Protocol Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Filter.Protocol";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Filter.ProtocolExclude Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Filter.ProtocolExclude";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Filter.DestPort Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Filter.DestPort";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Filter.DestPortRangeMax Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Filter.DestPortRangeMax";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Filter.DestPortExclude Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Filter.DestPortExclude";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Filter.SourcePort Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Filter.SourcePort";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Filter.SourcePortRangeMax Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Filter.SourcePortRangeMax";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Filter.SourcePortExclude Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Filter.SourcePortExclude";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Filter.ProcessingChoice Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Filter.ProcessingChoice";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Filter.Profile Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Filter.Profile";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.Alias Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.Alias";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.MaxChildSAs Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.MaxChildSAs";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.RemoteEndpoints Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.RemoteEndpoints";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.RemoteId Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.X_BC0304_RemoteId";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.SubnetIPType Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.X_BC0304_SubnetIPType";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.ForwardingPolicy Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.ForwardingPolicy";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.Protocol Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.Protocol";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.IKEv2AuthenticationMethod Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.IKEv2AuthenticationMethod";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.IKEv2PeerAuthenticationMethod Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.X_BC0304_IKEv2PeerAuthenticationMethod";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.CMPv2Profile Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.X_BC0304_CMPv2Profile";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.IKEv2AllowedEncryptionAlgorithms Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.IKEv2AllowedEncryptionAlgorithms";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.ESPAllowedEncryptionAlgorithms Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.ESPAllowedEncryptionAlgorithms";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.IKEv2AllowedPseudoRandomFunctions Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.IKEv2AllowedPseudoRandomFunctions";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.IKEv2AllowedIntegrityAlgorithms Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.IKEv2AllowedIntegrityAlgorithms";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.AHAllowedIntegrityAlgorithms Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.AHAllowedIntegrityAlgorithms";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.ESPAllowedIntegrityAlgorithms Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.ESPAllowedIntegrityAlgorithms";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.IKEv2AllowedDiffieHellmanGroupTransforms Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.IKEv2AllowedDiffieHellmanGroupTransforms";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.ESPAllowedDiffieHellmanGroupTransforms Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.X_BC0304_ESPAllowedDiffieHellmanGroupTransforms";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.IKEv2DeadPeerDetectionTimeout Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.IKEv2DeadPeerDetectionTimeout";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.IKEv2NATTKeepaliveTimeout Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.IKEv2NATTKeepaliveTimeout";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.AntiReplayWindowSize Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.AntiReplayWindowSize";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.DoNotFragment Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.DoNotFragment";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.DSCPMarkPolicy Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.DSCPMarkPolicy";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.StrictCRLPolicy Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.X_BC0304_StrictCRLPolicy";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.Reauth Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.X_BC0304_Reauth";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.IKEv2SATrafficLimit Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.IKEv2SATrafficLimit";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.IKEv2SATimeLimit Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.IKEv2SATimeLimit";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.IKEv2SAExpiryAction Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.IKEv2SAExpiryAction";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.ChildSATrafficLimit Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.ChildSATrafficLimit";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.ChildSATimeLimit Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.ChildSATimeLimit";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.ChildSAExpiryAction Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.ChildSAExpiryAction";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.SentCPAttrNumberOfEntries Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.SentCPAttrNumberOfEntries";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.SentCPAttr. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.SentCPAttr.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.SentCPAttr.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.SentCPAttr.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.SentCPAttr.Alias Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.SentCPAttr.Alias";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.SentCPAttr.Type Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.SentCPAttr.Type";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.IPsec.Profile.SentCPAttr.Value Handler.
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Profile.SentCPAttr.Value";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SmNotifyInd;
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
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave SmDeviceNotifyRegisterHandler");
    return bRtn;
}
