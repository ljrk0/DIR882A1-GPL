#ifndef __DM_SRV_LOG_H_
#define __DM_SRV_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif 

//#include "prc_ps_log.h"
//#include "prc_perview.h"
#include "dm_interface.h"

//#define DmSrv_StartPrcLog()         prc_trace_service_start(PRC_TRACE_SERVICE_AS_CLIENT,0)
//#define DmSrv_StopPrcLog()          prc_trace_service_stop()
//#define DmSrv_PrcLogTrace(x, ...)   ps_board_trace(GROUP_OAM_DM, x, __VA_ARGS__)
#define DmSrv_StartPrcLog()
#define DmSrv_StopPrcLog()
#define DmSrv_PrcLogTrace(x, ...)

#define DmSrv_NotifyDmSrvExit(SvcState, code)  \
do {   \
   SvcState.event = DM_SRV_EVT_PROC_EXIT; \
   SvcState.exitCode = code;  \
   DmSrvServiceStateNotify(&SvcState);  \
} while(0)

#define DmSrv_NotifyDmSrvFatalError(SvcState, code)  \
do { \
   SvcState.event = DM_SRV_EVT_FATAL_ERR; \
   SvcState.error = code;	\
   DmSrvServiceStateNotify(&SvcState);  \
} while(0)

#define DmSrv_NotifyDmSrvSignal(SvcState, code)  \
do {  \
   SvcState.event = DM_SRV_EVT_SIGNAL_ERR; \
   SvcState.signalNo = code;    \
   DmSrvServiceStateNotify(&SvcState);  \
} while(0)

#define DmSrv_NotifyDmSrvStopError(SvcState, code)  \
do { \
   SvcState.event = DM_SRV_EVT_STOPED_ERR; \
   SvcState.signalNo = code;    \
   DmSrvServiceStateNotify(&SvcState); \
} while(0)

#ifdef __cplusplus
}
#endif    /* __cpluscplus */

#endif    /* __DM_SRV_LOG_H_ */
