

#include "dm_internal.h"
#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif
extern void PmMeasConfigChange(DM_NOTIFY_T *pPara);

/*******************************************************************************
  Module   : PmDeviceNotifyRegisterHandler                                                   
  Function : register notify handler                                            
  Input    : void                                                               
  Ounput   : BOOL8: DM_TRUE/DM_FALSE                                          
  Note     : null                                                               
*******************************************************************************/
BOOL8 PmDeviceNotifyRegisterHandler(void)
{
    DM_NOTIFY_HANDLER_REG_PARA_T    *pPara;
    BOOL8                           bRtn = DM_TRUE;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter PmDeviceNotifyRegisterHandler");
    pPara = (DM_NOTIFY_HANDLER_REG_PARA_T *)malloc(sizeof(DM_NOTIFY_HANDLER_REG_PARA_T));

    if(pPara == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ PmDeviceNotifyRegisterHandler malloc() failed for pPara");
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara, 0, sizeof(DM_NOTIFY_HANDLER_REG_PARA_T));

    pPara->lParaCnt = 1;
    pPara->paParaPath = (INT8 **)malloc(sizeof(INT8 *)*pPara->lParaCnt);

    if(pPara->paParaPath == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ PmDeviceNotifyRegisterHandler malloc() failed for pPara->paParaPath");
        DM_FREE(pPara);
        bRtn = DM_FALSE;
        return bRtn;
    }

    memset(pPara->paParaPath, 0, pPara->lParaCnt);

    //Register Notify Device.PeriodicStatistics.SampleSet. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.PeriodicStatistics.SampleSet.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PmMeasConfigChange;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.PeriodicStatistics.SampleSet.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.PeriodicStatistics.SampleSet.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PmMeasConfigChange;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.PeriodicStatistics.SampleSet.Name Handler.
    pPara->paParaPath[0] = (INT8*)"Device.PeriodicStatistics.SampleSet.Name";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PmMeasConfigChange;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.PeriodicStatistics.SampleSet.SampleInterval Handler.
    pPara->paParaPath[0] = (INT8*)"Device.PeriodicStatistics.SampleSet.SampleInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PmMeasConfigChange;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.PeriodicStatistics.SampleSet.ReportSamples Handler.
    pPara->paParaPath[0] = (INT8*)"Device.PeriodicStatistics.SampleSet.ReportSamples";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PmMeasConfigChange;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.PeriodicStatistics.SampleSet.TimeReference Handler.
    pPara->paParaPath[0] = (INT8*)"Device.PeriodicStatistics.SampleSet.TimeReference";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PmMeasConfigChange;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.PeriodicStatistics.SampleSet.Parameter. Handler.
    pPara->paParaPath[0] = (INT8*)"Device.PeriodicStatistics.SampleSet.Parameter.";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PmMeasConfigChange;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.PeriodicStatistics.SampleSet.Parameter.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.PeriodicStatistics.SampleSet.Parameter.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PmMeasConfigChange;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.PeriodicStatistics.SampleSet.Parameter.Reference Handler.
    pPara->paParaPath[0] = (INT8*)"Device.PeriodicStatistics.SampleSet.Parameter.Reference";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PmMeasConfigChange;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.PeriodicStatistics.SampleSet.Parameter.LowThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.PeriodicStatistics.SampleSet.Parameter.LowThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PmMeasConfigChange;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.PeriodicStatistics.SampleSet.Parameter.HighThreshold Handler.
    pPara->paParaPath[0] = (INT8*)"Device.PeriodicStatistics.SampleSet.Parameter.HighThreshold";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PmMeasConfigChange;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FAP.PerfMgmt.Config.Enable Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FAP.PerfMgmt.Config.Enable";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PmMeasConfigChange;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FAP.PerfMgmt.Config.URL Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FAP.PerfMgmt.Config.URL";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PmMeasConfigChange;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FAP.PerfMgmt.Config.Username Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FAP.PerfMgmt.Config.Username";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PmMeasConfigChange;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FAP.PerfMgmt.Config.Password Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FAP.PerfMgmt.Config.Password";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PmMeasConfigChange;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FAP.PerfMgmt.Config.PeriodicUploadInterval Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FAP.PerfMgmt.Config.PeriodicUploadInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PmMeasConfigChange;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FAP.PerfMgmt.Config.PeriodicUploadTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FAP.PerfMgmt.Config.PeriodicUploadTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PmMeasConfigChange;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FAP.PerfMgmt.Config.MaxWaitTime Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FAP.PerfMgmt.Config.X_BC0304_MaxWaitTime";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PmMeasConfigChange;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FAP.PerfMgmt.Config.connTimeout Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FAP.PerfMgmt.Config.X_BC0304_connTimeout";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PmMeasConfigChange;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FAP.PerfMgmt.Config.limitRate Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FAP.PerfMgmt.Config.X_BC0304_limitRate";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PmMeasConfigChange;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FAP.PerfMgmt.Config.retryMax Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FAP.PerfMgmt.Config.X_BC0304_retryMax";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PmMeasConfigChange;
    bRtn = DmRegisterNotifyHandlerReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s notify register handler failed!\n", pPara->paParaPath[0]);
        DM_FREE(pPara->paParaPath);
        DM_FREE(pPara);
        return DM_FALSE;
    }

    //Register Notify Device.FAP.PerfMgmt.Config.retryInterval Handler.
    pPara->paParaPath[0] = (INT8*)"Device.FAP.PerfMgmt.Config.X_BC0304_retryInterval";
    pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)PmMeasConfigChange;
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
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave PmDeviceNotifyRegisterHandler");
    return bRtn;
}
