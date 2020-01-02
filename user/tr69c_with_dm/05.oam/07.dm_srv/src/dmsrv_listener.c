#include <unistd.h>

#include "dmsrv_common.h"
//#include "devs_interface.h"

/**
 *  static area
 */
static INT32 DmListener_Start();
static void  DmListener_Stop();
static INT32 DmListener_Run();

/**
 *  DM service main process
 */
INT32 DmListener_Main()
{
	INT32  lRet = DM_SRV_FAILURE;

//	if(devs_start_req() != DEVS_SUCCESS)
//	{
//		DM_SRV_ERROR("DM Service - failed start devs req");
//		return lRet;
//	}
	
	lRet = DmListener_Start();
	if (lRet == DM_SRV_SUCCESS)
	{
		lRet = DmListener_Run();
		
		DmListener_Stop();
	}

//	devs_stop_req();

	return lRet;
}

/**
 *  DM service start
 */
static INT32 DmListener_Start()
{
	DM_SRV_INFO("DM Service - DM Listener start DM request");
	
	DM_DBFILE_ENCRYPT_FUNC encryptFunc = NULL;
	
//	if(devs_data_cipher_needed())
//	{
//		encryptFunc = devs_do_data_cipher;
//	}

    /* Start DM model */
	if (DM_SUCCESS != DmServiceStartReq(encryptFunc))
	{
		DM_SRV_ERROR("DM Service - DM Listener start DM failed.");
		return DM_SRV_FAILURE;
	}

	return DM_SRV_SUCCESS;
}

/**
 *  DM service stop
 */
static void  DmListener_Stop()
{
	DM_SRV_INFO("DM Service - DM Listener stop DM request.");

	DmServiceStopReq();
}

/**
 *  DM service run loop
 */
static INT32 DmListener_Run()
{
	INT32  lRet = DM_SRV_FAILURE;
	sigset_t set;

	DM_SRV_INFO("DM Service - DM Listener is running. ver %s ", BRCM_DMSRV_REVISION);

	/* handle SIGINT, SIGHUP ans SIGTERM in this handler */
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGTERM);
	sigaddset(&set, SIGQUIT);
	sigaddset(&set, SIGHUP);
	sigprocmask(SIG_BLOCK, &set, NULL);

	while (!g_DmSrvCtrl.isTerminated)
	{
		int sig;
		int error = sigwait(&set, &sig);
		if (error)
		{
			DM_SRV_ERROR("DM Service - Listener error %d(%s) while waiting for a signal", errno, strerror(errno));
			break;
		}

		DM_SRV_INFO("DM Service - DM Listener receive signal %d, SIGTERM(%d) SIGINT(%d) SIGQUIT(%d)", 
						sig,SIGTERM,SIGINT,SIGQUIT);		
		switch (sig)
		{
        case SIGINT:    /* 2 */
			DM_SRV_INFO("DM Service - Notify DM Listener to stop...");
            g_DmSrvCtrl.isTerminated = DM_SRV_TRUE;
			lRet = DM_SRV_SUCCESS;
            break;
        case SIGTERM:   /* 15 */
        case SIGQUIT:   /* 3 */
			DM_SRV_TRACE_L("DM Service - Listener ignore this signal ...");
            break;
        case SIGHUP:
			DmSrv_HandleHangup();
            break;
		default:
            break;
		}
	}
	
	sigprocmask(SIG_UNBLOCK, &set, NULL);
	
	return lRet;
}
