#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

#include "dmsrv_common.h"
#include "device_external_type.h"

/**
 *  static area
 */
static void  DmSrv_SignalHandler(INT32 dwSignal);

DMSRV_CTRL_T g_DmSrvCtrl;

/**
 *  register signal handler
 */
void DmSrv_RegSignalHandler()
{
	struct sigaction act,oldact;
	
	memset(&act, 0, sizeof(struct sigaction));
	memset(&oldact, 0, sizeof(struct sigaction));
	
	act.sa_handler = DmSrv_SignalHandler;
	act.sa_flags = 0;
    sigaction(SIGINT, &act,&oldact);
    sigaction(SIGTERM, &act,&oldact);
    sigaction(SIGQUIT, &act,&oldact);
    sigaction(SIGHUP, &act,&oldact);

    act.sa_handler = SIG_IGN;
    act.sa_flags = 0;
    sigaction(SIGPIPE,&act,&oldact);
    sigaction(SIGTRAP,&act,&oldact);

	pthread_sigmask(SIG_SETMASK, &act.sa_mask, NULL);
	
	return;
}

/**
 *  signal handler
 */
void DmSrv_SignalHandler(INT32 dwSignal)
{
	INT8 *pProcessName = "Monitor";
	if (g_DmSrvCtrl.process == DM_SRV_PROCESS_LISTENER)
	{
		pProcessName = "Listener";
	}
	
	DM_SRV_TRACE_H("DM Service - DM %s receive signal %d, SIGTERM(%d) SIGINT(%d) SIGQUIT(%d)", 
					pProcessName, dwSignal, SIGTERM, SIGINT, SIGQUIT);

    switch (dwSignal)
    {
        case SIGINT:    /* 2 */
			DM_SRV_INFO("DM Service - Notify DM %s to stop...", pProcessName);
            g_DmSrvCtrl.isTerminated = DM_SRV_TRUE;
            break;
        case SIGTERM:   /* 15 */
        case SIGQUIT:   /* 3 */
			DM_SRV_TRACE_L("DM Service - %s ignore this signal ...", pProcessName);
            break;
        case SIGHUP:
			if(g_DmSrvCtrl.servicePid != 0)
			{
				if (g_DmSrvCtrl.servicePid != getpid()) 
				{
					kill(g_DmSrvCtrl.servicePid, SIGHUP);
				}
				else
				{
					DmSrv_HandleHangup();
				}
			}
			
            break;
        default:
            break;
    }

    return;
}

/**
 *  OSA trace hooker
 */
void  DmSrv_HandleHangup()
{
	DmFlushDBfiles();	
}

/**
 *  OSA trace hooker
 */
static VOID DmSrv_OsaTraceHooker(INT32 traceLevel, const INT8 *file, INT32 line, INT8 *pTraceContext)
{
//	UINT16 usLevel  = TRACE_LEVEL_LOW;
//    UINT8 ucEventId = 0;
	INT8 traces[256] = {0};

	if (pTraceContext != NULL)
	{
#if 0
		switch(traceLevel&OSA_DEBUG_LEVEL_MASK)
		{
			case OSA_DEBUG_LEVEL_DEBUG:
			{
				usLevel  = TRACE_LEVEL_LOW;
			}
			break;
			case OSA_DEBUG_LEVEL_TRACE:
			{
				usLevel  = TRACE_LEVEL_HIGH;
			}
			break;
			case OSA_DEBUG_LEVEL_WARNING:
			{
				usLevel  = TRACE_LEVEL_WARNING;
			}
			break;
			case OSA_DEBUG_LEVEL_ERROR:
			{
				usLevel  = TRACE_LEVEL_ERROR;/* here should raise an alarm instead later. */
			}
			break;
            case OSA_DEBUG_LEVEL_INFO:
            {
                ucEventId = traceLevel>>24;
                ps_board_log(GROUP_OSA_CPUH,ucEventId,pTraceContext,line,0xffff);
            }
            break;
			default:
				usLevel  = TRACE_LEVEL_LOW;
			break;
		}
#endif
        if ((traceLevel&OSA_DEBUG_LEVEL_MASK)<=OSA_DEBUG_LEVEL_ERROR)
        {
    		snprintf(traces, sizeof(traces), "<DmSrv> OSA DBG --- f: %s l: %d %s", file, line, pTraceContext);
    		//DmSrv_PrcLogTrace(usLevel, traces);
    		//if (usLevel < TRACE_LEVEL_WARNING)
    		{
    			printf("%s \n", traces);
    		}
        }
	}
	
	return;
}

/**
 *  DM lib trace hooker
 */
static void DmSrv_DmTraceHooker(const INT8 *file, INT32 line, UINT16 traceLevel, UINT32 errType, INT8 *pTraceContext)
{
//    UINT16 usLevel  = TRACE_LEVEL_LOW;

    if (pTraceContext != NULL)
    {
#if 0
        switch(traceLevel)
        {
            case DM_TRACE_LEVEL_LOW:
                usLevel  = TRACE_LEVEL_LOW;
                break;
            case DM_TRACE_LEVEL_MIDDLE:
                usLevel  = TRACE_LEVEL_MED;
                break;
            case DM_TRACE_LEVEL_HIGH:
                usLevel  = TRACE_LEVEL_HIGH;
                break;
            case DM_TRACE_LEVEL_WARNING:
                usLevel  = TRACE_LEVEL_WARNING;
                break;
            case DM_TRACE_LEVEL_ERROR:
                usLevel  = TRACE_LEVEL_ERROR;
                break;
            case DM_TRACE_LEVEL_NONE:
            default:
                usLevel  = TRACE_LEVEL_LOW;
                break;
        }

        DmSrv_PrcLogTrace(usLevel, "<DmSrv> %s", pTraceContext);
		if (usLevel < TRACE_LEVEL_WARNING)
#endif
		{
			printf("%s \n", pTraceContext);
		}
    }

    return;
}

/**
 *  Initialize enviroment
 */
void DmSrv_Initialize()
{
    setsid();
    chdir("/");
    umask(0);

	pid_t pid = fork();
	if( pid < 0)
	{
		DM_SRV_ERROR("DM Service - Failed to create entry process. %d %s", errno, strerror(errno));
		return;
	}
	else if(pid > 0)
	{
		exit(0);  // exit parent process to make self to become a deamon
	}

	DmSrv_StartPrcLog();
    osa_debug_callback(DmSrv_OsaTraceHooker);
    DmTraceHookerRegister(DmSrv_DmTraceHooker);
	
	if(DmSrvCheckPidFile(DM_TRUE)) /* Only create pid file here */
	{
	    DM_SRV_INFO("DM Service - DM service only be started once.");
		DmSrv_StopPrcLog();	
		exit(0);
	}

    return;
}

/**
 *  Initialize enviroment
 */
void DmSrv_Cleanup(INT32 code)
{
    DmSrv_StopPrcLog();
	
	DmSrvUnlinkPidFile();  /* Delete PID file */

    return;
}

/**
 *  Trace output log
 */
void  DmSrv_Log(INT32 level, char *file, UINT32 lineNum, const char *pFmt, ...)
{
	va_list ap;
	INT8 buf[DM_SRV_MAX_LOG_SIZE] = {0};

	va_start(ap, pFmt);
	vsnprintf(buf, sizeof(buf), pFmt, ap);
	va_end(ap);

	switch(level)
	{
	case DM_SRV_LOG_PS_TRACE_LOW :
		//DmSrv_PrcLogTrace(TRACE_LEVEL_LOW, buf);
		break;
	case DM_SRV_LOG_PS_TRACE_MED :
		//DmSrv_PrcLogTrace(TRACE_LEVEL_MED, buf);
		break;
	case DM_SRV_LOG_PS_TRACE_HIGH :
		//DmSrv_PrcLogTrace(TRACE_LEVEL_HIGH, buf);
		printf("%s \n", buf);
		break;
	case DM_SRV_LOG_PS_TRACE_INFO:
		//DmSrv_PrcLogTrace(TRACE_LEVEL_HIGH, buf);
		printf("%s \n", buf);
		break;	
	case DM_SRV_LOG_PS_TRACE_WARNING :
		//DmSrv_PrcLogTrace(TRACE_LEVEL_WARNING, buf);
		printf("%s \n", buf);
		break;
	case DM_SRV_LOG_PS_TRACE_ERROR :	
		//DmSrv_PrcLogTrace(TRACE_LEVEL_ERROR, buf);
		printf("%s \n", buf);
		break;
	default :
		//DmSrv_PrcLogTrace(TRACE_LEVEL_ERROR, buf);
		printf("%s \n", buf);
		break;
	}

	return;
}

/**
 * DON't delete this function
 * unused function, but it can load libDbConv in compile
 */
void DmSrv_LoadDbConvLib() 
{
	INT8* pData = DmConvertIntToStr(2);
	printf("%s \n", pData);
	free(pData);
}
