

#include "dm_internal.h"
#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif
extern void SonNotifyInd(DM_NOTIFY_T *pPara);

/*******************************************************************************
  Module   : PsSon_dmNotifyRegisterHandler                                                   
  Function : register notify handler                                            
  Input    : void                                                               
  Ounput   : BOOL8: DM_TRUE/DM_FALSE                                          
  Note     : null                                                               
*******************************************************************************/
BOOL8 PsSon_dmNotifyRegisterHandler(void)
{
    DM_NOTIFY_HANDLER_REG_PARA_T    *pPara;
    BOOL8                           bRtn = DM_TRUE;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter PsSon_dmNotifyRegisterHandler");
    pPara = (DM_NOTIFY_HANDLER_REG_PARA_T *)malloc(sizeof(DM_NOTIFY_HANDLER_REG_PARA_T));

    if(pPara == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ PsSon_dmNotifyRegisterHandler malloc() failed for pPara");
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara, 0, sizeof(DM_NOTIFY_HANDLER_REG_PARA_T));

    pPara->lParaCnt = 1;
    pPara->paParaPath = (INT8 **)malloc(sizeof(INT8 *)*pPara->lParaCnt);

    if(pPara->paParaPath == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ PsSon_dmNotifyRegisterHandler malloc() failed for pPara->paParaPath");
        DM_FREE(pPara);
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara->paParaPath, 0, pPara->lParaCnt);

    //Register Notify SON_DM.REM.Enabled Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.REM.Enabled";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.REM.Interval Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.REM.Interval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.REM.Count Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.REM.Count";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.TPM.Enabled Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.TPM.Enabled";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.TPM.NonCsgCheck Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.TPM.NonCsgCheck";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.TPM.PowerSettingOffset Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.TPM.PowerSettingOffset";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.TPM.PollutionMitigationLevel Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.TPM.PollutionMitigationLevel";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.TPM.Pini Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.TPM.Pini";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.TPM.Pmin Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.TPM.Pmin";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.TPM.Pmax Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.TPM.Pmax";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.TPM.NCellRscpThresholdTable. Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.TPM.NCellRscpThresholdTable.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.TPM.NCellRscpThresholdTable.RSRP Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.TPM.NCellRscpThresholdTable.RSRP";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.TPM.NCellRscpThresholdTable.MaxTxPower Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.TPM.NCellRscpThresholdTable.MaxTxPower";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.TPM.LutPpmCorrection.PPMLow Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.TPM.LutPpmCorrection.PPMLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.TPM.LutPpmCorrection.PPMMedium Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.TPM.LutPpmCorrection.PPMMedium";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.TPM.LutPpmCorrection.PPMHigh Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.TPM.LutPpmCorrection.PPMHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.TPM.LutAdjMacroCorrection. Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.TPM.LutAdjMacroCorrection.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.TPM.LutAdjMacroCorrection.RSRP Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.TPM.LutAdjMacroCorrection.RSRP";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.TPM.LutAdjMacroCorrection.Correction Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.TPM.LutAdjMacroCorrection.Correction";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.TPM.LutCcMacroCorrection. Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.TPM.LutCcMacroCorrection.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.TPM.LutCcMacroCorrection.RSRP Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.TPM.LutCcMacroCorrection.RSRP";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.TPM.LutCcMacroCorrection.Correction Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.TPM.LutCcMacroCorrection.Correction";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.IM.Enabled Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.IM.Enabled";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.IM.UeClassificationEnabled Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.IM.UeClassificationEnabled";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.IM.ImFairLevel Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.IM.ImFairLevel";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.IM.ImFairFactor Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.IM.ImFairFactor";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.IM.ImHardFFR Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.IM.ImHardFFR";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.IM.ImDeltaRsrpHigh Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.IM.ImDeltaRsrpHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.IM.ImDeltaRsrpLow Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.IM.ImDeltaRsrpLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.IM.ImUeMeasInterval Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.IM.ImUeMeasInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.IM.ImIirCoeff Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.IM.ImIirCoeff";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.IM.ImCqiAverageType Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.IM.ImCqiAverageType";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.IM.ImPaLow Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.IM.ImPaLow";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.IM.ImPaHigh Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.IM.ImPaHigh";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.IM.ImBaseOnDynamicUEReport Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.IM.ImBaseOnDynamicUEReport";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.IM.ImForceICIC Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.IM.ImForceICIC";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.IM.BW.ImDlProtectedPrbList. Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.IM.BW.ImDlProtectedPrbList.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.IM.BW.ImDlProtectedPrbList.Bitmap Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.IM.BW.ImDlProtectedPrbList.Bitmap";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.CCO.Enabled Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.CCO.Enabled";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.CCO.UeMeasReportPeriod Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.CCO.UeMeasReportPeriod";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.CCO.RsrqThreshold1 Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.CCO.RsrqThreshold1";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.CCO.RsrqThreshold2 Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.CCO.RsrqThreshold2";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.CCO.RsrpThresholdServingNoise Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.CCO.RsrpThresholdServingNoise";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.CCO.RsrpThresholdServingInf Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.CCO.RsrpThresholdServingInf";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.CCO.RsrpThresholdNeighbor2Cell Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.CCO.RsrpThresholdNeighbor2Cell";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.CCO.RsrpThresholdNeighbor3Cell Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.CCO.RsrpThresholdNeighbor3Cell";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.CCO.RsrqHysteresis Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.CCO.RsrqHysteresis";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.CCO.RsrpHysteresis Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.CCO.RsrpHysteresis";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.CCO.SampleCountThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.CCO.SampleCountThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.CCO.Step Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.CCO.Step";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.CCO.CellEdgeRatio Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.CCO.CellEdgeRatio";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.CCO.Weights.Good Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.CCO.Weights.Good";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.CCO.Weights.NoiseDominated Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.CCO.Weights.NoiseDominated";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.CCO.Weights.InterferenceDominated2Cell Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.CCO.Weights.InterferenceDominated2Cell";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.CCO.Weights.InterferenceDominated3Cell Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.CCO.Weights.InterferenceDominated3Cell";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MRO.Enabled Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MRO.Enabled";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MRO.MroInterFreqEnabled Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MRO.MroInterFreqEnabled";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MRO.MroPeriodicity Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MRO.MroPeriodicity";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MRO.MroAperiodicEnable Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MRO.MroAperiodicEnable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MRO.PowerChangeThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MRO.PowerChangeThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MRO.PingPongHoTimeThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MRO.PingPongHoTimeThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MRO.X2StoreUEContextTime Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MRO.X2StoreUEContextTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MRO.S1StoreUEContextTime Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MRO.S1StoreUEContextTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MRO.TargetHOFailureRate Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MRO.TargetHOFailureRate";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MRO.TargetPingpongRate Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MRO.TargetPingpongRate";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MRO.MinRlfEvent Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MRO.MinRlfEvent";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MRO.MroWeights.TL Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MRO.MroWeights.TL";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MRO.MroWeights.TE Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MRO.MroWeights.TE";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MRO.MroWeights.WCA Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MRO.MroWeights.WCA";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MRO.MroWeights.WCB Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MRO.MroWeights.WCB";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MRO.MroWeights.PP Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MRO.MroWeights.PP";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MLB.Enabled Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MLB.Enabled";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MLB.MlbPeriodicity Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MLB.MlbPeriodicity";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MLB.ReportingPeriodicity Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MLB.ReportingPeriodicity";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MLB.GtpEchoDelayCoff Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MLB.GtpEchoDelayCoff";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MLB.S1TnlLowLoadThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MLB.S1TnlLowLoadThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MLB.S1TnlMedLoadThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MLB.S1TnlMedLoadThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MLB.S1TnlHighLoadThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MLB.S1TnlHighLoadThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MLB.HwLowLoadThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MLB.HwLowLoadThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MLB.HwMedLoadThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MLB.HwMedLoadThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MLB.HwHighLoadThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MLB.HwHighLoadThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MLB.PrbLowLoadThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MLB.PrbLowLoadThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MLB.PrbMedLoadThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MLB.PrbMedLoadThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MLB.PrbHighLoadThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MLB.PrbHighLoadThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MLB.CapacityLowLoadThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MLB.CapacityLowLoadThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MLB.CapacityMedLoadThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MLB.CapacityMedLoadThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify SON_DM.MLB.CapacityHighLoadThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"SON_DM.MLB.CapacityHighLoadThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)SonNotifyInd;
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
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave PsSon_dmNotifyRegisterHandler");
    return bRtn;
}
