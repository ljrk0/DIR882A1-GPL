#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/wait.h>

#include "dm_internal.h"
#include "dm_srv.h"

#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif

#define DM_SRV_PID_FILE         "/tmp/dmsrv.pid"
#define DM_SRV_MSGQ_FTOK_PATH   "/tmp/dmsrv.mon"
#define DM_SRV_MSGQ_LENGTH      5

#define DM_SRV_MON_THREAD_EXIT_EVENT  1

static pid_t  g_dmmonitor_pid = 0;
static DM_SRV_STATE_MONITOR_CALLBACK g_DmSrvStateMonitorCB = NULL;
static DM_SRV_MONITOR_THREAD_T       g_DmSrvMonThread;

/**
 * Check/create PID file, return TRUE if already running
 */
BOOL8 DmSrvCheckPidFile(BOOL8 isServer)
{
	struct stat st;
	FILE*  fpPid = NULL;

	if (stat(DM_SRV_PID_FILE, &st) == 0)
	{
		fpPid = fopen(DM_SRV_PID_FILE, "r");
		if (fpPid)
		{
			char buf[64];
			pid_t pid = 0;

			memset(buf, 0, sizeof(buf));
			if (fread(buf, 1, sizeof(buf), fpPid))
			{
				buf[sizeof(buf) - 1] = '\0';
				pid = atoi(buf);
			}
			fclose(fpPid);
			if (pid && kill(pid, 0) == 0)
			{
				return DM_TRUE;    /* DM service process is running */
			}
		}
		unlink(DM_SRV_PID_FILE);
	}

	/* for server, create new pidfile */
	if (isServer)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ create dmsrv pid file");

		fpPid = fopen(DM_SRV_PID_FILE, "w");
		if (fpPid)
		{
			int fd = fileno(fpPid);
			if (fd == -1 || fcntl(fd, F_SETFD, FD_CLOEXEC) == -1)
			{
				DM_ERROR(DM_ERR_FILE_OPERATION_ERROR, "DM Trace ------ setting FD_CLOEXEC for '"DM_SRV_PID_FILE"' failed: %s",
						 strerror(errno));
			}
			fprintf(fpPid, "%d\n", getpid());
			fflush(fpPid);
			fclose(fpPid);
		}
	}

	return DM_FALSE;
}

/**
 * Delete/truncate the PID file
 */
VOID DmSrvUnlinkPidFile()
{
	FILE*  fpPid = fopen(DM_SRV_PID_FILE, "w");
	if (fpPid)
	{
		ftruncate(fileno(fpPid), 0);
		fclose(fpPid);
	}

	unlink(DM_SRV_PID_FILE);
	unlink("/tmp/sock_dm");
}

/**
 * Clear all FD inherited from parent process
 */
static INT32 DmSrvClearParentFDs(void)
{
	INT8 fd_dir[128];
	INT32 lowfd = 3;/* only keep stdin, stdout, stderr */
	INT32 fd = 0;
	DIR * dir = NULL;
	struct dirent * fileptr = NULL;

	/* try to close only open file descriptors on Linux... */
	snprintf(fd_dir, sizeof(fd_dir), "/proc/%u/fd", getpid());
	if (access(fd_dir, F_OK) == 0)
	{
		dir = opendir(fd_dir);

		while((fileptr = readdir(dir))!=NULL)
		{
			fd = atoi(fileptr->d_name);

			if (fd >= lowfd)
			{
				close(fd);
			}
		}

		closedir(dir);
	}

	return 0;
}

/**
 * Start an detach child process (common)
 */
static INT32 DmSrvStartProcess(INT8* path, INT8* command, INT8 *options[])
{
    INT32 lRet = DM_SUCCESS;
    INT32 pid = 0;
	INT8  processName[PATH_MAX];

	memset(processName, 0, sizeof(processName));
	
	/*create child process*/
	snprintf(processName, sizeof(processName) - 1, "%s/%s", path, command);
	pid = fork();
	if(pid < 0)
	{
		DM_ERROR(DM_ERR_PROCESSING_COMMON_ERROR, "DM Trace ------ fork error when to start %s (%d:%s)\n", command, errno, strerror(errno));
		return DM_RET_FAILED_LAUNCH_DMSRV;
	}
	else if(pid == 0)       
	{
		DmSrvClearParentFDs();
		if (0 != execv(processName, options))
		{
			DM_ERROR(DM_ERR_PROCESSING_COMMON_ERROR, "DM Trace ------ execv error when to start %s (%d:%s)\n", command, errno, strerror(errno));
			exit(1);
		}
	}

	/* parent process */
	INT32 pidStatus;
	waitpid(pid, &pidStatus, 0);
	if (!WIFEXITED(pidStatus) || WEXITSTATUS(pidStatus) != 0)
	{
		lRet = DM_RET_FAILED_LAUNCH_DMSRV;
	}

    return lRet;
}

/**
 * Start dmsrv process
 */
static INT32 DmSrvStartServiceProcess()
{
	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Start DM service(dmsrv) process.");
	
#define SCM_DMSRV_MAX_OPTION  5
    INT32  lRet = DM_SUCCESS;
	//INT8*  pProcessPath = "/oam/bins/";
	INT8*  pProcessPath = "/bin/";
	INT8*  pProcessCmd  = "dmsrv";
	INT8*  dmsrv_options[SCM_DMSRV_MAX_OPTION];
	UINT8  option_count = 0;

	INT8 *pServicePath = getenv("DM_SERVICE_PATH");	
	if (pServicePath != NULL && strlen(pServicePath) != 0)
	{
		pProcessPath = pServicePath;
	}
	else
	{
		// default : /work/oam/
	}

	memset(dmsrv_options, 0, sizeof(dmsrv_options));
	dmsrv_options[option_count++] = pProcessCmd;
	dmsrv_options[option_count++] = "-m";

	lRet = DmSrvStartProcess(pProcessPath, pProcessCmd, dmsrv_options);

	return lRet;
}

/**
 * Stop DM service(dmsrv)
 */
static INT32 DmSrvStopServiceProcess()
{
	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Stop DM service process.\n");
	kill(g_dmmonitor_pid, SIGINT);
	
	return DM_SUCCESS;
}

/**
 * if DM service is not started, start it
 */
INT32 DmEnsureStartService()
{
	INT32  lRet = DM_SUCCESS;

	if (!DmSrvCheckPidFile(DM_FALSE))
	{
		/* Start DM service */
		lRet = DmSrvStartServiceProcess();
		if (lRet != DM_SUCCESS)
		{
				DM_ERROR(DM_ERR_PROCESSING_COMMON_ERROR, "DM Trace ------ Failed to start dmsrv process.");
		}
	}
	
	return lRet;
}

/**
 * Register DM service to make self control
 */
INT32 DmSelfRegisterService()
{
	g_dmmonitor_pid = getppid();
	
	return DM_SUCCESS;
}

/**
 * Stop DM service by DM monitor processs
 */
INT32 DmSelfStopService()
{
	if (g_dmmonitor_pid != 0)
	{
		DmSrvStopServiceProcess();
	}
	
	return DM_SUCCESS;
}

/** 
 * check if dmsrv monitor client queue has created or not
 */
static BOOL8 DmSrvCheckMonitorThreadQueue(OSA_MSGQ_T* pDmSrvMsgQ)
{
    BOOL8 result = DM_TRUE;

    if (access(pDmSrvMsgQ->ftok, F_OK) != 0)
    {
        if (mkdir(pDmSrvMsgQ->ftok, 0777) != 0 )
		{
			return result;   // if failed, just not monitor dmsrv status
		}
    }

	INT32 lRet = osa_msg_queue_check(pDmSrvMsgQ);
	if (lRet != OSA_SUCCESS)
	{
		result = DM_FALSE;
	}

	return result;
}

/**
 * Notify service state monitor
 */
INT32 DmSrvServiceStateNotify(DM_SRV_STATE_NOTIFY_T* pStateMonitor)
{
	INT32  lRet = DM_SUCCESS;

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmSrvServiceStateNotify");

	if(pStateMonitor == NULL)
	{
		DM_ERROR(DM_ERR_INVALID_ARGUMENTS, "DM Trace ------ Argument error , leave DmSrvServiceStateNotify");
		return DM_RET_INVALID_ARGUMENTS;
	}
	
	OSA_MSGQ_T dmSrvMsgQ;
	osa_mem_set(&dmSrvMsgQ, 0, sizeof(OSA_MSGQ_T));
    dmSrvMsgQ.ftok = DM_SRV_MSGQ_FTOK_PATH;
    dmSrvMsgQ.used_for_processes = OSA_TRUE;

	if (DM_FALSE == DmSrvCheckMonitorThreadQueue(&dmSrvMsgQ))
	{
		/* if queue has not created , do nothing */
		return DM_SUCCESS;
	}
	
	/* obtain the queue */
	lRet = osa_msg_queue_create(&dmSrvMsgQ, sizeof(DM_SRV_STATE_NOTIFY_T), DM_SRV_MSGQ_LENGTH);
	if (lRet != OSA_SUCCESS)
	{
		DM_ERROR(DM_ERR_MSGQ_ERROR, "DM Trace ------ failed to get fm msg-q in dmSrvMsgQ, leave DmSrvServiceStateNotify");
		return lRet;
	}

    /* send state monitor message to client thread */
    lRet = osa_msg_queue_send(&dmSrvMsgQ, (void*)pStateMonitor);
    if (lRet != OSA_SUCCESS)
    {
		DM_ERROR(DM_ERR_MSGQ_ERROR, "DM Trace ------ send event (0x%X) failed, leave DmSrvServiceStateNotify", pStateMonitor->event);
        return lRet;
    }
	
	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmSrvServiceStateNotify");

	return lRet;
}

/**
 *  Thread :: DmSrvMonitorEntry
 */
static INT32 DmSrvMonitorEntry(void)
{
	INT32 lRet = DM_SUCCESS;
	BOOL8 isRunning = DM_TRUE;
	
	DM_SRV_STATE_NOTIFY_T stateMonitor;
	
	DM_TRACE(DM_TRACE_LEVEL_HIGH, "enter DmSrvMonitorEntry");
	
	while(isRunning)
	{
		osa_mem_set(&stateMonitor, 0, sizeof(DM_SRV_STATE_NOTIFY_T));
		
		lRet = osa_msg_queue_receive(&g_DmSrvMonThread.msgQ,&stateMonitor);
        if (lRet != OSA_SUCCESS)
        {
			DM_TRACE(DM_TRACE_LEVEL_WARNING, "DM Trace ------ failed to receive msg, continue");
            continue;
        }

		if(stateMonitor.event == DM_SRV_MON_THREAD_EXIT_EVENT)
		{
			osa_msg_queue_delete(&g_DmSrvMonThread.msgQ);
			osa_mem_set(&g_DmSrvMonThread, 0, sizeof(DM_SRV_MONITOR_THREAD_T));
			break;
		}		

		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ receive event id 0x%X", stateMonitor.event);
		DM_SRV_STATE_MONITOR_CALLBACK pCallback = g_DmSrvStateMonitorCB;
		if (pCallback)
		{
			pCallback(&stateMonitor);
			/* To ensure message queue is removed even if dmsrv core-dump. if not dmsrv monitor can't start again on PC */
			osa_msg_queue_delete(&g_DmSrvMonThread.msgQ);
			osa_mem_set(&g_DmSrvMonThread, 0, sizeof(DM_SRV_MONITOR_THREAD_T));
			break;
		}
	}

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "leave DmSrvMonitorEntry");
	
	return lRet;
}

/**
 *  register service monitor callback
 */
INT32 DmSrvStateMonitorRegister(DM_SRV_STATE_MONITOR_CALLBACK pCallback)
{
	INT32  lRet = DM_SUCCESS;

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmSrvStateMonitorRegister");

	if (g_DmSrvStateMonitorCB != NULL && pCallback != NULL)
	{
       DM_ERROR(DM_ERR_PROCESSING_COMMON_ERROR, "DM Trace ------ notify callback has been registered, leave DmSrvStateMonitorRegister");
       return DM_RET_REQUEST_DENIED;
	}

	if (pCallback != NULL)
	{
		OSA_MSGQ_T dmSrvMsgQ;
		osa_mem_set(&dmSrvMsgQ, 0, sizeof(OSA_MSGQ_T));
    	dmSrvMsgQ.ftok = DM_SRV_MSGQ_FTOK_PATH;
    	dmSrvMsgQ.used_for_processes = OSA_TRUE;

		if (g_DmSrvStateMonitorCB != NULL)
		{
			DM_ERROR(DM_ERR_PROCESSING_COMMON_ERROR, "DM Trace ------ notify callback has been registered by other, leave DmSrvStateMonitorRegister");
			return DM_RET_REQUEST_DENIED;
		}

		if (access(dmSrvMsgQ.ftok, F_OK) != 0 && mkdir(dmSrvMsgQ.ftok, 0777) != 0)
		{
			DM_ERROR(DM_ERR_MSGQ_ERROR, "DM Trace ------ failed to create dmsrv monitor, leave DmSrvStateMonitorRegister");
			return lRet;	 // if failed, just not monitor dmsrv status
		}

		/* if queue has not created , create it and launch monitor thread */
		osa_mem_copy((void *)&g_DmSrvMonThread.msgQ, (void *)&dmSrvMsgQ, sizeof(OSA_MSGQ_T));
		lRet = osa_msg_queue_create(&g_DmSrvMonThread.msgQ, sizeof(DM_SRV_STATE_NOTIFY_T), DM_SRV_MSGQ_LENGTH);
		if (lRet != OSA_SUCCESS)
		{
			DM_ERROR(DM_ERR_MSGQ_ERROR, "DM Trace ------ failed to get dmsrv monitor msg-q in dmSrvMsgQ, leave DmSrvStateMonitorRegister");
			return lRet;
		}
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DM state monitor queue(%x) is created DmSrvStateMonitorRegister", g_DmSrvMonThread.msgQ.mq_id);
		
        g_DmSrvMonThread.createId = getpid();

		lRet = osa_thread_create(&g_DmSrvMonThread.thread, "dmSrvM", (OSA_THREAD_PFN)DmSrvMonitorEntry, 0, 0, 
								 OSA_THREAD_SCHED_OTHER, E_THREAD_PRIO_0, OSA_THREAD_PROP_NOWAIT);
		if (lRet != OSA_SUCCESS)
		{
			DM_ERROR(DM_ERR_THREAD_CREATING_FAILED, "DM Trace ------ failed to create monitor thread, leave DmSrvStateMonitorRegister");
			return lRet;
		}
		
		g_DmSrvStateMonitorCB = pCallback;
	}
	else
	{
        g_DmSrvStateMonitorCB = NULL;

		if (g_DmSrvMonThread.createId == getpid())
		{
			DM_SRV_STATE_NOTIFY_T stateMonitor;
			osa_mem_set(&stateMonitor, 0, sizeof(DM_SRV_STATE_NOTIFY_T));
			stateMonitor.event = DM_SRV_MON_THREAD_EXIT_EVENT;
			lRet = DmSrvServiceStateNotify(&stateMonitor);
		}
	}

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmSrvStateMonitorRegister");

	return lRet;
}
