#ifndef __DM_SRV_COMMON_H_
#define __DM_SRV_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "common_type_def.h"
#include "osa.h"
#include "dmsrv_log.h"
#include "dm_srv.h"

#define DM_SRV_SUCCESS  OSA_SUCCESS
#define DM_SRV_FAILURE  OSA_FAILURE
#define DM_SRV_TRUE     OSA_TRUE
#define DM_SRV_FALSE    OSA_FALSE

#define DM_SRV_MAX_LOG_SIZE     1024

/** DM SRV trace macro */
#define DM_SRV_ERROR(pString,   ...)   DmSrv_Log(DM_SRV_LOG_PS_TRACE_ERROR,   __FILE__, __LINE__, pString, ##__VA_ARGS__)
#define DM_SRV_WARNING(pString, ...)   DmSrv_Log(DM_SRV_LOG_PS_TRACE_WARNING, __FILE__, __LINE__, pString, ##__VA_ARGS__)
#define DM_SRV_INFO(pString, ...)      DmSrv_Log(DM_SRV_LOG_PS_TRACE_INFO,   __FILE__, __LINE__, pString, ##__VA_ARGS__)
#define DM_SRV_TRACE_H(pString, ...)   DmSrv_Log(DM_SRV_LOG_PS_TRACE_HIGH,    __FILE__, __LINE__, pString, ##__VA_ARGS__)
#define DM_SRV_TRACE_M(pString, ...)   DmSrv_Log(DM_SRV_LOG_PS_TRACE_MED,     __FILE__, __LINE__, pString, ##__VA_ARGS__)
#define DM_SRV_TRACE_L(pString, ...)   DmSrv_Log(DM_SRV_LOG_PS_TRACE_LOW,     __FILE__, __LINE__, pString, ##__VA_ARGS__)

/** DM SRV Alaram ID <Same with dm library besides DM_SRV_LOG_PS_TRACE_XXXX > */
#define DM_SRV_LOG_PS_TRACE_LOW              0
#define DM_SRV_LOG_PS_TRACE_MED              1
#define DM_SRV_LOG_PS_TRACE_HIGH             2
#define DM_SRV_LOG_PS_TRACE_INFO             3
#define DM_SRV_LOG_PS_TRACE_WARNING          4
#define DM_SRV_LOG_PS_TRACE_ERROR            5

/** type define */
typedef enum {
	DM_SRV_PROCESS_MONITOR,
	DM_SRV_PROCESS_LISTENER,
} DM_SRV_PROCESS;

typedef struct {
	DM_SRV_PROCESS process;
	BOOL8          isTerminated;
	
	pid_t          servicePid;       /* service pid */
} DMSRV_CTRL_T;

/** data declare */
extern DMSRV_CTRL_T g_DmSrvCtrl;

/** Function declare */
extern INT32 DmListener_Main();

extern void  DmSrv_RegSignalHandler();
extern void  DmSrv_HandleHangup();

extern void  DmSrv_Initialize();
extern void  DmSrv_Cleanup(INT32 code);

extern void  DmSrv_Log(INT32 level, char *file, UINT32 lineNum, const char *pFmt, ...);

#ifdef __cplusplus
}
#endif    /* __cpluscplus */

#endif    /* __DM_SRV_COMMON_H_ */
