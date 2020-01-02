

#include "dm_internal.h"
#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif
extern INT32 Tr69cGetConnReqUrlReq(DM_GET_VALUE_T *pGetValue);

/*******************************************************************************
  Module   : Tr069DeviceGetRegisterHandler                                                   
  Function : register get handler                                               
  Input    : void                                                               
  Ounput   : BOOL8: DM_TRUE/DM_FALSE                                          
  Note     : null                                                               
*******************************************************************************/
BOOL8 Tr069DeviceGetRegisterHandler(void)
{
    DM_REG_GET_HANDLER_PARA_T       *pPara;
    BOOL8                           bRtn = DM_TRUE;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter Tr069DeviceGetRegisterHandler");
    pPara = (DM_REG_GET_HANDLER_PARA_T *)malloc(sizeof(DM_REG_GET_HANDLER_PARA_T));

    if(pPara == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Tr069DeviceGetRegisterHandler malloc() failed for pPara");
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara, 0, sizeof(DM_REG_GET_HANDLER_PARA_T));

    pPara->lParaCnt = 1;
    pPara->paParaPath = (INT8 **)malloc(sizeof(INT8 *)*pPara->lParaCnt);

    if(pPara->paParaPath == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Tr069DeviceGetRegisterHandler malloc() failed for pPara->paParaPath");
        DM_FREE(pPara);
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara->paParaPath, 0, pPara->lParaCnt);

    /* Register Get Device.ManagementServer.ConnectionRequestURL Handler. */
    pPara->paParaPath[0] = (INT8*)"Device.ManagementServer.ConnectionRequestURL";
    pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)Tr69cGetConnReqUrlReq;
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
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave Tr069DeviceGetRegisterHandler");
    return bRtn;
}

