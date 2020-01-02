

#include "dm_internal.h"
#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif
extern INT32 SmGetIPsecInfo(DM_GET_VALUE_T *pGetValue);

/*******************************************************************************
  Module   : SmDeviceGetRegisterHandler                                                   
  Function : register get handler                                               
  Input    : void                                                               
  Ounput   : BOOL8: DM_TRUE/DM_FALSE                                          
  Note     : null                                                               
*******************************************************************************/
BOOL8 SmDeviceGetRegisterHandler(void)
{
    DM_REG_GET_HANDLER_PARA_T       *pPara;
    BOOL8                           bRtn = DM_TRUE;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter SmDeviceGetRegisterHandler");
    pPara = (DM_REG_GET_HANDLER_PARA_T *)malloc(sizeof(DM_REG_GET_HANDLER_PARA_T));

    if(pPara == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ SmDeviceGetRegisterHandler malloc() failed for pPara");
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara, 0, sizeof(DM_REG_GET_HANDLER_PARA_T));

    pPara->lParaCnt = 1;
    pPara->paParaPath = (INT8 **)malloc(sizeof(INT8 *)*pPara->lParaCnt);

    if(pPara->paParaPath == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ SmDeviceGetRegisterHandler malloc() failed for pPara->paParaPath");
        DM_FREE(pPara);
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara->paParaPath, 0, pPara->lParaCnt);

    /* Register Get Device.IPsec.Status Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Status";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.Filter.Status Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Filter.Status";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.Tunnel. Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Tunnel.";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.Tunnel.Alias Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Tunnel.Alias";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.Tunnel.TunnelInterface Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Tunnel.TunnelInterface";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.Tunnel.TunneledInterface Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Tunnel.TunneledInterface";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.Tunnel.Filters Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Tunnel.Filters";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.Tunnel.Stats. Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Tunnel.Stats.";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.Tunnel.Stats.DecryptionErrors Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Tunnel.Stats.DecryptionErrors";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.Tunnel.Stats.IntegrityErrors Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Tunnel.Stats.IntegrityErrors";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.Tunnel.Stats.ReplayErrors Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Tunnel.Stats.ReplayErrors";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.Tunnel.Stats.PolicyErrors Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Tunnel.Stats.PolicyErrors";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.Tunnel.Stats.OtherReceiveErrors Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.Tunnel.Stats.OtherReceiveErrors";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA. Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.Status Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.Status";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.Alias Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.Alias";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.Tunnel Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.Tunnel";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.LocalAddress Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.LocalAddress";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.RemoteAddress Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.RemoteAddress";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.EncryptionAlgorithm Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.EncryptionAlgorithm";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.EncryptionKeyLength Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.EncryptionKeyLength";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.PseudoRandomFunction Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.PseudoRandomFunction";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.IntegrityAlgorithm Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.IntegrityAlgorithm";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.DiffieHellmanGroupTransform Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.DiffieHellmanGroupTransform";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.CreationTime Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.CreationTime";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.NATDetected Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.NATDetected";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ReceivedCPAttrNumberOfEntries Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ReceivedCPAttrNumberOfEntries";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ChildSANumberOfEntries Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ChildSANumberOfEntries";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.Stats. Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.Stats.";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.Stats.BytesSent Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.Stats.BytesSent";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.Stats.BytesReceived Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.Stats.BytesReceived";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.Stats.PacketsSent Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.Stats.PacketsSent";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.Stats.PacketsReceived Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.Stats.PacketsReceived";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.Stats.ErrorsSent Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.Stats.ErrorsSent";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.Stats.DecryptionErrors Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.Stats.DecryptionErrors";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.Stats.IntegrityErrors Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.Stats.IntegrityErrors";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.Stats.OtherReceiveErrors Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.Stats.OtherReceiveErrors";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ReceivedCPAttr. Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ReceivedCPAttr.";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ReceivedCPAttr.Type Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ReceivedCPAttr.Type";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ReceivedCPAttr.Value Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ReceivedCPAttr.Value";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ChildSA. Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ChildSA.";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ChildSA.Status Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ChildSA.Status";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ChildSA.Alias Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ChildSA.Alias";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ChildSA.InboundSPI Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ChildSA.InboundSPI";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ChildSA.OutboundSPI Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ChildSA.OutboundSPI";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ChildSA.LocalTrafficSelector Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ChildSA.X_BC0304_LocalTrafficSelector";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ChildSA.RemoteTrafficSelector Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ChildSA.X_BC0304_RemoteTrafficSelector";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ChildSA.CreationTime Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ChildSA.CreationTime";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ChildSA.Stats. Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ChildSA.Stats.";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ChildSA.Stats.BytesSent Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ChildSA.Stats.BytesSent";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ChildSA.Stats.BytesReceived Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ChildSA.Stats.BytesReceived";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ChildSA.Stats.PacketsSent Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ChildSA.Stats.PacketsSent";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ChildSA.Stats.PacketsReceived Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ChildSA.Stats.PacketsReceived";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ChildSA.Stats.ErrorsSent Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ChildSA.Stats.ErrorsSent";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ChildSA.Stats.DecryptionErrors Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ChildSA.Stats.DecryptionErrors";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ChildSA.Stats.IntegrityErrors Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ChildSA.Stats.IntegrityErrors";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ChildSA.Stats.ReplayErrors Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ChildSA.Stats.ReplayErrors";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ChildSA.Stats.PolicyErrors Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ChildSA.Stats.PolicyErrors";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
    bRtn = DmRegisterGetHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s get register handler failed!", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    /* Register Get Device.IPsec.IKEv2SA.ChildSA.Stats.OtherReceiveErrors Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.IPsec.IKEv2SA.ChildSA.Stats.OtherReceiveErrors";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)SmGetIPsecInfo;
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
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave SmDeviceGetRegisterHandler");
    return bRtn;
}

