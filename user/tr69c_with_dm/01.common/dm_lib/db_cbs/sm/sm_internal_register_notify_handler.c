

#include "dm_internal.h"
#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif
extern void SmNotifyInd(DM_NOTIFY_T *pPara);

/*******************************************************************************
  Module   : SmInternalNotifyRegisterHandler                                                   
  Function : register notify handler                                            
  Input    : void                                                               
  Ounput   : BOOL8: DM_TRUE/DM_FALSE                                          
  Note     : null                                                               
*******************************************************************************/
BOOL8 SmInternalNotifyRegisterHandler(void)
{
    DM_NOTIFY_HANDLER_REG_PARA_T    *pPara;
    BOOL8                           bRtn = DM_TRUE;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter SmInternalNotifyRegisterHandler");
    pPara = (DM_NOTIFY_HANDLER_REG_PARA_T *)malloc(sizeof(DM_NOTIFY_HANDLER_REG_PARA_T));

    if(pPara == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ SmInternalNotifyRegisterHandler malloc() failed for pPara");
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara, 0, sizeof(DM_NOTIFY_HANDLER_REG_PARA_T));

    pPara->lParaCnt = 1;
    pPara->paParaPath = (INT8 **)malloc(sizeof(INT8 *)*pPara->lParaCnt);

    if(pPara->paParaPath == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ SmInternalNotifyRegisterHandler malloc() failed for pPara->paParaPath");
        DM_FREE(pPara);
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara->paParaPath, 0, pPara->lParaCnt);

    //Register Notify Internal.Ema.SessionStatus.EntityId Handler.
    pPara->paParaPath[0] = (INT8*)"Internal.Ema.SessionStatus.EntityId";
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
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave SmInternalNotifyRegisterHandler");
    return bRtn;
}
