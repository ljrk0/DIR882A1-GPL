#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>

#include "dmsrv_common.h"

static void  DmMonitor_SetRLimit();
static INT32 DmMonitor_WaitListenerProcess();
static INT32 DmMonitor_StartListenerWithMonitor();
static INT32 DmMonitor_StartSingleListener();
static INT32 DmMonitor_ClearParentFDs();

/**
 *  main
 */
int main(int argc, char** argv)
{
	INT32 lRet = DM_SRV_FAILURE;
	BOOL8 bStartMonitor = DM_SRV_FALSE; 
	
	osa_mem_set(&g_DmSrvCtrl, 0, sizeof(DMSRV_CTRL_T));
   
	int i = 0;
	for(i = 1; i < argc; i ++)
	{
		if (strcmp(argv[i], "-m") == 0)
		{
			bStartMonitor = DM_SRV_TRUE;
		} 
	}
   
	DmMonitor_SetRLimit();

	if(bStartMonitor)
	{		
		lRet = DmMonitor_StartListenerWithMonitor();
	}
	else
	{
		lRet = DmMonitor_StartSingleListener();
	}

	return lRet;
}

/**
 * setrlimit
 */
static void  DmMonitor_SetRLimit()
{
	struct rlimit r;

	getrlimit(RLIMIT_CORE, &r);
	r.rlim_cur = r.rlim_max;
	setrlimit(RLIMIT_CORE, &r);

	r.rlim_max=99;
	setrlimit(RLIMIT_RTPRIO, &r);
}

/**
 *  clear all the parent fd.
 */
static INT32 DmMonitor_ClearParentFDs(void)
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
 *  Start monitor process & dm listener process
 */
static INT32 DmMonitor_StartListenerWithMonitor()
{
	INT32  lRet = DM_SRV_FAILURE;

	g_DmSrvCtrl.process = DM_SRV_PROCESS_MONITOR;
	
	DmSrv_RegSignalHandler();
	
	DmSrv_Initialize();
	
	DM_SRV_TRACE_H("DM Service - Start DM monitor");
	
	pid_t pid = fork();
	if (pid < 0) 
	{
		DM_SRV_ERROR("DM Service - Failed to fork DM handler process!! errno=%d errmsg=%s", errno, strerror(errno));
	}
	else if (pid == 0)
	{
		g_DmSrvCtrl.servicePid = getpid();

		DmMonitor_ClearParentFDs();	
		DM_SRV_TRACE_H("DM Service - Start DM listener.");
		lRet = DmListener_Main();
		
		DM_SRV_TRACE_H("DM Service - Stop DM listener(%d).", lRet);
		DmSrv_Cleanup(lRet);
		exit(lRet);
	}
	else
	{
		g_DmSrvCtrl.servicePid = pid;

		lRet = DmMonitor_WaitListenerProcess(pid);
		DM_SRV_TRACE_H("DM Service - Stop DM monitor(%d).", lRet);
		DmSrv_Cleanup(lRet);
	}
	
	return lRet;
}

/**
 *  Start monitor process & dm service process
 */
static INT32 DmMonitor_StartSingleListener()
{
	INT32  lRet = DM_SRV_FAILURE;

	g_DmSrvCtrl.process = DM_SRV_PROCESS_LISTENER;
	
	DmSrv_RegSignalHandler();
	DmSrv_Initialize();
	DM_SRV_TRACE_H("DM Service - Start DM listener.");
	
	lRet = DmListener_Main();
	
	DM_SRV_TRACE_H("DM Service - Stop DM listener(%d).", lRet);
	DmSrv_Cleanup(0);
	
	return lRet;
}

/**
 *  Wait for dm listener process status
 */
static INT32 DmMonitor_WaitListenerProcess(pid_t pid)
{
	INT32  lRet = DM_SRV_FAILURE;
    DM_SRV_STATE_NOTIFY_T stateNotify;
	
	while (!g_DmSrvCtrl.isTerminated)
	{
		INT32 pidStatus;	
		pid_t retPid = waitpid(pid, &pidStatus, 0);
		if (retPid == -1)
		{
			if (!g_DmSrvCtrl.isTerminated)
			{
				if (errno == EINTR)
				{
					continue;
				}

				DM_SRV_ERROR("DM Service --- Failed to monitor DM listener process (pid=%d) errmsg(%s)", pid, strerror(errno));
				DmSrv_NotifyDmSrvFatalError(stateNotify, errno);
				lRet = errno;
				g_DmSrvCtrl.isTerminated = DM_SRV_TRUE;
			}
			else
			{
				DM_SRV_TRACE_H("DM Service - DM listener will be stopped by monitor");
				DmSrv_NotifyDmSrvExit(stateNotify, 0);
				kill(pid, SIGINT);
				lRet = DM_SRV_SUCCESS;
			}
		}
		else if(retPid == pid)
		{
			if (WIFEXITED(pidStatus))
			{
				DM_SRV_TRACE_L("DM Service - DM listener process(pid=%d) quit by exit(%d)",  pid, WEXITSTATUS(pidStatus));
				DmSrv_NotifyDmSrvExit(stateNotify, WEXITSTATUS(pidStatus));
			}
			else if (WIFSIGNALED(pidStatus))
			{
				DM_SRV_ERROR("DM Service - DM listener process(pid=%d) killed by signal(%d) unexpectedly", pid, WTERMSIG(pidStatus));
				DmSrv_NotifyDmSrvSignal(stateNotify, WTERMSIG(pidStatus));
			}
			else if (WIFSTOPPED(pidStatus))
			{
				DM_SRV_ERROR("DM Service - DM listener process(pid=%d) stopped by signal(%d) unexpectedly", pid, WSTOPSIG(pidStatus));
				DmSrv_NotifyDmSrvStopError(stateNotify, WSTOPSIG(pidStatus));
			}
			else
			{
				DM_SRV_ERROR("DM Service - DM listener process(pid=%d) quit unexpectedly", pid);
				DmSrv_NotifyDmSrvFatalError(stateNotify, -1);
			}

			lRet = DM_SRV_SUCCESS;
			g_DmSrvCtrl.isTerminated = DM_SRV_TRUE;
		}	
	}

	return lRet;
}
