#include <stdio.h>
#include "dm_interface.h"
#include "osa_mem.h"
#include "DbStart.h"

static int g_dmLogEnable = 1;

void  Db_mLog(const INT8 *file, INT32 line, UINT16 traceLevel, UINT32 errType, INT8 *pTraceContext)
{
	switch(traceLevel)
	{
	case DM_TRACE_LEVEL_LOW :
		//ps_board_trace(GROUP_OAM_DM,TRACE_LEVEL_LOW, "TEST : %s", pTraceContext);
		break;
	case DM_TRACE_LEVEL_MIDDLE :
		//ps_board_trace(GROUP_OAM_DM,TRACE_LEVEL_MED, "TEST : %s", pTraceContext);
		break;
	case DM_TRACE_LEVEL_HIGH:
		//ps_board_trace(GROUP_OAM_DM,TRACE_LEVEL_HIGH, "TEST : %s", pTraceContext);
		//if (g_dmLogEnable) printf("INFO : %s \n", pTraceContext);
		break;
	case DM_TRACE_LEVEL_WARNING :
		//ps_board_trace(GROUP_OAM_DM, TRACE_LEVEL_WARNING, "TEST : %s", pTraceContext);
		printf("WARN : %s \n", pTraceContext);
		break;
	case DM_TRACE_LEVEL_ERROR :
		//ps_board_trace(GROUP_OAM_DM,TRACE_LEVEL_ERROR, "TEST : %s", pTraceContext);
		printf("ERROR : %s \n", pTraceContext);
		break;
	default :
		//ps_board_trace(GROUP_OAM_DM, TRACE_LEVEL_ERROR, "TEST : %s", pTraceContext);
		//printf("ERROR : %s \n", pTraceContext);
		break;
	}

	return;
}

void cmsOsaTraceHooker(INT32 traceLevel, const INT8 *file, INT32 line, INT8 *pTraceContext)
{
	INT8 traces[256] = {0};

	if (pTraceContext != NULL)
	{
		switch(traceLevel&OSA_DEBUG_LEVEL_MASK)
		{
			case OSA_DEBUG_LEVEL_DEBUG:
				snprintf(traces, sizeof(traces), "<Tr69c> OSA DBG --- f: %s l: %d %s", file, line, pTraceContext);
				printf("%s\n", traces);
				break;
			case OSA_DEBUG_LEVEL_TRACE:
				snprintf(traces, sizeof(traces), "<Tr69c> OSA trace --- f: %s l: %d %s", file, line, pTraceContext);
				printf("%s\n", traces);
				break;
			case OSA_DEBUG_LEVEL_WARNING:
				snprintf(traces, sizeof(traces), "<Tr69c> OSA warning --- f: %s l: %d %s", file, line, pTraceContext);
				printf("%s\n", traces);
				break;
			case OSA_DEBUG_LEVEL_ERROR:
				snprintf(traces, sizeof(traces), "<Tr69c> OSA error --- f: %s l: %d %s", file, line, pTraceContext);
				printf("%s\n", traces);
				break;
            case OSA_DEBUG_LEVEL_INFO:
				snprintf(traces, sizeof(traces), "<Tr69c> OSA info --- f: %s l: %d %s", file, line, pTraceContext);
				printf("%s\n", traces);
            	break;
			default:
				snprintf(traces, sizeof(traces), "<Tr69c> OSA DBG --- f: %s l: %d %s", file, line, pTraceContext);
				printf("%s\n", traces);
				break;
		}

	}

}

#if 0
INT32 ScmResetAdminStateIfNeeded(void)
{
	INT32 lRet = SCM_FAILURE;
	DM_TRANSACTION_REQ_T transReq;
	DM_GET_PARA_C_VALUE_REQ_T getCValue;
    DM_SET_PARA_C_VALUE_REQ_T setCValue;
    BOOL8 AdminState = DM_FALSE;
    BOOL8 LocationVerifiction = DM_FALSE;

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&getCValue, 0, sizeof(DM_GET_PARA_C_VALUE_REQ_T));
    memset(&setCValue, 0, sizeof(DM_SET_PARA_C_VALUE_REQ_T));
    
    strcpy(transReq.AccessEntity,"ROOT");
    strcpy(transReq.RootNodeToLock,"Device.Services.FAPService.1.FAPControl.LTE.");
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_FALSE;
    transReq.bNotCallNotify = DM_TRUE;
    transReq.timeout = 0;
    
    lRet = DmTransactionStartReq(&transReq);
    
    if (lRet != 0)
    {
        SCM_ERROR(SCM_ALARM_ID__PROCESSING_COMMON_ERROR,"SCM : DmTransactionStartReq failed lRet=%d in ScmResetAdminStateIfNeeded",lRet);
    }
    else
    {
        /* get LocationVerifiction  */
        getCValue.TransactionId = transReq.TransactionId;
        getCValue.ulParaId = DEVICE_SERVICES_FAPSERVICE_FAPCONTROL_LTE_LOCATIONVERIFICTION_ID;
        getCValue.pucPath = "Device.Services.FAPService.1.FAPControl.LTE."DM_SELFDEFINED_PREFIX"LocationVerifiction";
        getCValue.pValue = &LocationVerifiction;
        getCValue.ValueSize = sizeof(BOOL8);
        getCValue.bShmWay = DM_FALSE;

        lRet = DmGetParaCValueReq(&getCValue);
    
        if(lRet != DM_RET_SUCCESS)
        {
            SCM_ERROR(SCM_ALARM_ID__PROCESSING_COMMON_ERROR,"SCM : DmGetParaCValueReq failed, %s, lRet=%d in ScmResetAdminStateIfNeeded",getCValue.pucPath,lRet);
        }
        else if (LocationVerifiction == DM_TRUE)
        {
            SCM_TRACE_H("SCM : LocationVerifiction needed, set AdminState to false. ACS will set AdminState to true if LocationVerifiction passed...");

            setCValue.TransactionId = transReq.TransactionId;
            setCValue.ulParaId = DEVICE_SERVICES_FAPSERVICE_FAPCONTROL_LTE_ADMINSTATE_ID;
            setCValue.pucPath = "Device.Services.FAPService.1.FAPControl.LTE.AdminState";
            setCValue.pValue = &AdminState;
            setCValue.ValueSize = sizeof(BOOL8);
            setCValue.bShmWay = DM_FALSE;

            lRet = DmSetParaCValueReq(&setCValue);

            if(lRet != DM_RET_SUCCESS)
            {
                SCM_ERROR(SCM_ALARM_ID__PROCESSING_COMMON_ERROR,"SCM : DmSetParaCValueReq failed, %s, lRet=%d in ScmResetAdminStateIfNeeded",setCValue.pucPath,lRet);
            }
            else
            {
                SCM_TRACE_H("SCM : set AdminState to false...");
            }
        }
        else
        {
            SCM_TRACE_H("SCM : no need for LocationVerifiction, keep the value of AdminState");
        }

        DmTransactionStopReq(transReq.TransactionId);

        if(lRet != DM_RET_SUCCESS)
        {
            SCM_ERROR(SCM_ALARM_ID__PROCESSING_COMMON_ERROR,"SCM : DmTransactionStopReq failed, lRet=%d in ScmResetAdminStateIfNeeded",lRet);
        }
    }

    return lRet;
}

INT32 ScmAutoUpgradeInd(void)
{
    INT32 lRet = SCM_FAILURE;
    DM_TRANSACTION_REQ_T transReq;
    DM_GET_PARA_C_VALUE_REQ_T getCValue;
    DEVICE_DEBUGMGMT_AUTOUPGRADE_T AutoUpgrade;

    SCM_TRACE_H("SCM : enter ScmAutoUpgradeInd");

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&getCValue, 0, sizeof(DM_GET_PARA_C_VALUE_REQ_T));
    memset(&AutoUpgrade, 0, sizeof(DEVICE_DEBUGMGMT_AUTOUPGRADE_T));

    strcpy(transReq.AccessEntity,"ROOT");
    strcpy(transReq.RootNodeToLock,"Device."DM_SELFDEFINED_PREFIX"DebugMgmt.AutoUpgrade.");
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_FALSE;
    transReq.bNotCallNotify = DM_TRUE;
    transReq.timeout = 0;

    lRet = DmTransactionStartReq(&transReq);

    if (lRet != 0)
    {
        SCM_ERROR(SCM_ALARM_ID__PROCESSING_COMMON_ERROR,"SCM : DmTransactionStartReq failed lRet=%d in ScmAutoUpgradeInd",lRet);
    }
    else
    {
        getCValue.TransactionId = transReq.TransactionId;
        getCValue.ulParaId = DEVICE_DEBUGMGMT_AUTOUPGRADE_ID;
        getCValue.pucPath = "Device."DM_SELFDEFINED_PREFIX"DebugMgmt.AutoUpgrade.";
        getCValue.pValue = &AutoUpgrade;
        getCValue.ValueSize = sizeof(DEVICE_DEBUGMGMT_AUTOUPGRADE_T);
        getCValue.bShmWay = DM_FALSE;

        lRet = DmGetParaCValueReq(&getCValue);

        if(lRet != DM_RET_SUCCESS)
        {
            SCM_ERROR(SCM_ALARM_ID__PROCESSING_COMMON_ERROR,"SCM : DmGetParaCValueReq failed, %s, lRet=%d in ScmAutoUpgradeInd",getCValue.pucPath,lRet);
        }

        if (AutoUpgrade.Enable==SCM_TRUE)
        {
            devs_update_auto_upgrade_config(AutoUpgrade.FtpIp,AutoUpgrade.FtpUser,AutoUpgrade.FtpPassword,AutoUpgrade.FtpPath,SCM_TRUE);

            SCM_TRACE_H("SCM : auto-upgrade enabled, ftp ip(%s) usr(%s) passwd(%s) path(%s)",AutoUpgrade.FtpIp,AutoUpgrade.FtpUser,AutoUpgrade.FtpPassword,AutoUpgrade.FtpPath);
        }

        DmTransactionStopReq(transReq.TransactionId);

        if(lRet != DM_RET_SUCCESS)
        {
            SCM_ERROR(SCM_ALARM_ID__PROCESSING_COMMON_ERROR,"SCM : DmTransactionStopReq failed, lRet=%d in ScmAutoUpgradeInd",lRet);
        }
    }

    SCM_TRACE_H("SCM : leave ScmAutoUpgradeInd");

    return lRet;
}

INT32 ScmUpdateAcsConfig(void)
{
	INT32 lRet = SCM_FAILURE;
	DM_TRANSACTION_REQ_T transReq;
    DM_SET_PARA_C_VALUE_REQ_T setCValue;

    if (strlen(g_ScmCtrl.acs_addr)<=0)
    {
        SCM_TRACE_H("SCM : no acs address config here");
        return lRet;
    }

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&setCValue, 0, sizeof(DM_SET_PARA_C_VALUE_REQ_T));

    strcpy(transReq.AccessEntity,"ROOT");
    strcpy(transReq.RootNodeToLock,"Device.ManagementServer.");
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_FALSE;
    transReq.bNotCallNotify = DM_TRUE;
    transReq.timeout = 0;

    lRet = DmTransactionStartReq(&transReq);

    if (lRet != 0)
    {
        SCM_ERROR(SCM_ALARM_ID__PROCESSING_COMMON_ERROR,"SCM : DmTransactionStartReq failed lRet=%d in ScmUpdateAcsConfig",lRet);
    }
    else
    {
        SCM_TRACE_H("SCM : update ACS URL to %s",g_ScmCtrl.acs_addr);

        setCValue.TransactionId = transReq.TransactionId;
        setCValue.ulParaId = DEVICE_MANAGEMENTSERVER_URL_ID;
        setCValue.pucPath = "Device.ManagementServer.URL";
        setCValue.pValue = g_ScmCtrl.acs_addr;
        setCValue.ValueSize = strlen(g_ScmCtrl.acs_addr)+1;
        setCValue.bShmWay = DM_FALSE;

        lRet = DmSetParaCValueReq(&setCValue);

        if(lRet != DM_RET_SUCCESS)
        {
            SCM_ERROR(SCM_ALARM_ID__PROCESSING_COMMON_ERROR,"SCM : DmSetParaCValueReq failed, %s, lRet=%d in ScmUpdateAcsConfig",setCValue.pucPath,lRet);
        }

        SCM_TRACE_H("SCM : update ACS User to %s",g_ScmCtrl.acs_user);

        setCValue.TransactionId = transReq.TransactionId;
        setCValue.ulParaId = DEVICE_MANAGEMENTSERVER_USERNAME_ID;
        setCValue.pucPath = "Device.ManagementServer.Username";
        setCValue.pValue = g_ScmCtrl.acs_user;
        setCValue.ValueSize = strlen(g_ScmCtrl.acs_user)+1;
        setCValue.bShmWay = DM_FALSE;

        lRet = DmSetParaCValueReq(&setCValue);

        if(lRet != DM_RET_SUCCESS)
        {
            SCM_ERROR(SCM_ALARM_ID__PROCESSING_COMMON_ERROR,"SCM : DmSetParaCValueReq failed, %s, lRet=%d in ScmUpdateAcsConfig",setCValue.pucPath,lRet);
        }

        SCM_TRACE_H("SCM : update ACS password to %s",g_ScmCtrl.acs_password);

        setCValue.TransactionId = transReq.TransactionId;
        setCValue.ulParaId = DEVICE_MANAGEMENTSERVER_PASSWORD_ID;
        setCValue.pucPath = "Device.ManagementServer.Password";
        setCValue.pValue = g_ScmCtrl.acs_password;
        setCValue.ValueSize = strlen(g_ScmCtrl.acs_password)+1;
        setCValue.bShmWay = DM_FALSE;

        lRet = DmSetParaCValueReq(&setCValue);

        if(lRet != DM_RET_SUCCESS)
        {
            SCM_ERROR(SCM_ALARM_ID__PROCESSING_COMMON_ERROR,"SCM : DmSetParaCValueReq failed, %s, lRet=%d in ScmUpdateAcsConfig",setCValue.pucPath,lRet);
        }

        DmTransactionStopReq(transReq.TransactionId);

        if(lRet != DM_RET_SUCCESS)
        {
            SCM_ERROR(SCM_ALARM_ID__PROCESSING_COMMON_ERROR,"SCM : DmTransactionStopReq failed, lRet=%d in ScmUpdateAcsConfig",lRet);
        }
    }

    return lRet;
}

INT32 ScmUpdateLogsrvConfig(void)
{
	INT32 lRet = SCM_FAILURE;
	DM_TRANSACTION_REQ_T transReq;
    DM_SET_PARA_C_VALUE_REQ_T setCValue;
    CM_IP_ADDR_T ipAddress;
    UINT32 ConnectingPolicy = DEVICE_DEBUGMGMT_TRACES_LOGSERVER_CONNECTINGPOLICY_ALWAYS;

    if (strlen(g_ScmCtrl.logviewer_ip)<=0)
    {
        SCM_TRACE_H("SCM : no logsrv address config here");
        return lRet;
    }

    DmConvertStringToCmIpAddr(0,g_ScmCtrl.logviewer_ip,NULL,&ipAddress,NULL);

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&setCValue, 0, sizeof(DM_SET_PARA_C_VALUE_REQ_T));

    strcpy(transReq.AccessEntity,"ROOT");
    strcpy(transReq.RootNodeToLock,"Device."DM_SELFDEFINED_PREFIX"DebugMgmt.Traces.LogServer.");
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_FALSE;
    transReq.bNotCallNotify = DM_TRUE;
    transReq.timeout = 0;

    lRet = DmTransactionStartReq(&transReq);

    if (lRet != 0)
    {
        SCM_ERROR(SCM_ALARM_ID__PROCESSING_COMMON_ERROR,"SCM : DmTransactionStartReq failed lRet=%d in ScmUpdateLogsrvConfig",lRet);
        return lRet;
    }

    SCM_TRACE_H("SCM : update logserver ip address to %s",g_ScmCtrl.logviewer_ip);

    setCValue.TransactionId = transReq.TransactionId;
    setCValue.ulParaId = DEVICE_DEBUGMGMT_TRACES_LOGSERVER_IPADDRESS_ID;
    setCValue.pucPath = "Device."DM_SELFDEFINED_PREFIX"DebugMgmt.Traces.LogServer.IPAddress";
    setCValue.pValue = &ipAddress;
    setCValue.ValueSize = sizeof(CM_IP_ADDR_T);
    setCValue.bShmWay = DM_FALSE;

    lRet = DmSetParaCValueReq(&setCValue);

    if(lRet != DM_RET_SUCCESS)
    {
        SCM_ERROR(SCM_ALARM_ID__PROCESSING_COMMON_ERROR,"SCM : DmSetParaCValueReq failed, %s, lRet=%d in ScmUpdateLogsrvConfig",setCValue.pucPath,lRet);
    }

    setCValue.TransactionId = transReq.TransactionId;
    setCValue.ulParaId = DEVICE_DEBUGMGMT_TRACES_LOGSERVER_CONNECTINGPOLICY_ID;
    setCValue.pucPath = "Device."DM_SELFDEFINED_PREFIX"DebugMgmt.Traces.LogServer.ConnectingPolicy";
    setCValue.pValue = &ConnectingPolicy;
    setCValue.ValueSize = sizeof(ConnectingPolicy);
    setCValue.bShmWay = DM_FALSE;

    lRet = DmSetParaCValueReq(&setCValue);

    if(lRet != DM_RET_SUCCESS)
    {
        SCM_ERROR(SCM_ALARM_ID__PROCESSING_COMMON_ERROR,"SCM : DmSetParaCValueReq failed, %s, lRet=%d in ScmUpdateLogsrvConfig",setCValue.pucPath,lRet);
    }

    lRet = DmTransactionStopReq(transReq.TransactionId);

    if(lRet != DM_RET_SUCCESS)
    {
        SCM_ERROR(SCM_ALARM_ID__PROCESSING_COMMON_ERROR,"SCM : DmTransactionStopReq failed, lRet=%d in ScmUpdateLogsrvConfig",lRet);
    }

    return lRet;
}
#endif
static void ScmDmSrvStateCB(DM_SRV_STATE_NOTIFY_T* pStateMonitor)
{
	SCM_TRACE_H("SCM: dmsrv status change. event=0x%X external=%d \n", pStateMonitor->event, pStateMonitor->exitCode);
	SCM_TRACE_H("Dm service exception ! event=0x%X external=%d \n", pStateMonitor->event, pStateMonitor->exitCode);

	if ( !(pStateMonitor->event == DM_SRV_EVT_PROC_EXIT && pStateMonitor->exitCode == 0))
	{
		//ScmSendMsgReq(SCM_MSG_REBOOT_REQ, (UINT32)strdup("DMSRV"));
		SCM_TRACE_H("DMSRV exit\n");
	}
}

INT32 ScmStartConnectServer()
{
	SCM_TRACE_H("SCM Start DM Req\n");
	DM_START_REQ_T startReq;
	//DEVS_REBOOT_REASON_TYPE rebootreason = DEVS_REBOOT_REASON__normal;
	DM_LOAD_DB_HANDLER_REQ_T LoadDbHandlerReq;

	INT32 lRet = DmSrvStateMonitorRegister(ScmDmSrvStateCB); 
	if (SCM_SUCCESS != lRet)
	{
		SCM_TRACE_H("Failed to register dmsrv monitor\n");
		printf("[OAM FATAL], Failed to register DM monitor callback.\n");
		return SCM_FAILURE;
	}

	osa_mem_set(&startReq, 0, sizeof(DM_START_REQ_T));
	strncpy(startReq.entity, "Oam",sizeof(startReq.entity) - 1);
	lRet = DmStartReq(&startReq);
	if (SCM_SUCCESS != lRet)
	{
		SCM_TRACE_H("SCM start DM failed\n");
		printf("[OAM FATAL], SCM start DM failed\n");
		return SCM_FAILURE;
	}
#if 0
	devs_get_reboot_reason(&rebootreason);
    if (rebootreason&DEVS_REBOOT_REASON__upgrade_fail_need_recover)
    {
        /* roll back db files from old ones */
        ScmCopyFile(DEVICE_XML_FOR_UPGRADE_BACKUP, DEVICE_XML);
		ScmCopyFile(DEVICE_XML_FOR_UPGRADE_BACKUP, DEVICE_XML_FOR_BACKUP);
		ScmCopyFile(INTERNAL_XML_FOR_UPGRADE_BACKUP, INTERNAL_XML);
		ScmCopyFile(INTERNAL_XML_FOR_UPGRADE_BACKUP, INTERNAL_XML_FOR_BACKUP);
    }
#endif
	/* load db xml */
	osa_mem_set(&LoadDbHandlerReq, 0, sizeof(DM_LOAD_DB_HANDLER_REQ_T));
	LoadDbHandlerReq.bDbReset = 0;
    LoadDbHandlerReq.bXmlWithComment = DM_FALSE;
	if(0 != access("/tmp/xml", F_OK))
	{
		mkdir("/tmp/xml", 0777);
		mkdir("/tmp/xml/tmp", 0777);
		mkdir("/tmp/xml/data", 0777);
		mkdir("/tmp/xml/store", 0777);
	}
	snprintf(LoadDbHandlerReq.dbLibPath,   sizeof(LoadDbHandlerReq.dbLibPath),   "/lib/");
    snprintf(LoadDbHandlerReq.workingPath, sizeof(LoadDbHandlerReq.workingPath), "/tmp/xml/tmp/");
    snprintf(LoadDbHandlerReq.loadPath,    sizeof(LoadDbHandlerReq.loadPath),    "/tmp/xml/data/");
    snprintf(LoadDbHandlerReq.backupPath,  sizeof(LoadDbHandlerReq.backupPath),  "/tmp/xml/store/");
	/* If LoadDbHandlerReq.dbName is null, all db will be loade in /oam/libs */
	
	lRet = DmLoadGlobalDbHandlerReq(&LoadDbHandlerReq);
	if (lRet != SCM_SUCCESS)
	{
		SCM_TRACE_H("Failed to load db. \n");
		printf("[OAM FATAL], Failed to loade db.\n");
		return lRet;
	}

	return lRet;
}

#define SMD_MESSAGE_ADDR  "/tmp/sock_oam"
#define MAXPENDING 5
void SMD_loop()
{
	INT32 commFd = -1, connect_fd = -1;
	INT32 receiver_id, sender_id;
	struct sockaddr_un serverAddr;
	INT32 rc, length;
	fd_set fdR;
	SOCK_MSG_HEADER_T *soket_Msg_header;
	char buf[256] = { 0 };
	/*
	* Create a unix domain socket.
	*/
	commFd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (commFd < 0)
	{
	   SCM_TRACE_H("Could not create socket\n");
	   return -1;
	}

	/*
	* Connect to smd.
	*/
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sun_family = AF_LOCAL;
	strncpy(serverAddr.sun_path, SMD_MESSAGE_ADDR, sizeof(serverAddr.sun_path));

	rc = bind(commFd, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
	if (rc != 0)
	{
	   SCM_TRACE_H("bind to %s failed, rc=%d errno=%d\n", SMD_MESSAGE_ADDR, rc, errno);
	   close(commFd);
	   return -1;
	}

	rc = listen(commFd, MAXPENDING);
	if (rc < 0)
	{
	   SCM_TRACE_H("list to %s failed, rc=%d errno=%d\n", SMD_MESSAGE_ADDR, rc, errno);
	   close(commFd);
	   return -1;
	}

	while (1)
	{
		FD_ZERO(&fdR); 
		FD_SET(commFd, &fdR);
		
		switch (select(commFd + 1, &fdR, NULL, NULL, NULL)) { 
		case -1: 
			SCM_TRACE_H("select to %s failed, rc=%d errno=%d\n", SMD_MESSAGE_ADDR, rc, errno);
		case 0: 
			SCM_TRACE_H("timeout to %s rc=%d errno=%d\n", SMD_MESSAGE_ADDR, rc, errno);
		default: 
			if (FD_ISSET(commFd, &fdR)) 
			{
				if( (connect_fd = accept(commFd, (struct sockaddr*)NULL, NULL)) == -1)
				{  
					SCM_TRACE_H("accept socket error: %s(errno: %d)\n",strerror(errno),errno);  
					continue;
				}
				
				length = recv(connect_fd, buf, sizeof(buf), 0);
				soket_Msg_header = (SOCK_MSG_HEADER_T *)&buf;

				SCM_TRACE_H("recv from tr69 msg id = %x\n", soket_Msg_header->msg_id);

				receiver_id = soket_Msg_header->sender_id;
				sender_id = soket_Msg_header->receiver_id;
				if(0x10000001 == soket_Msg_header->msg_id)//EME_MSG_READY_IND
				{
					sleep(15);
					memset(buf, 0, sizeof(buf));
					soket_Msg_header->msg_id = 0x10000003;//EME_MSG_REGISTER_IND
					soket_Msg_header->receiver_id = receiver_id;
					soket_Msg_header->sender_id = sender_id;
					soket_Msg_header->word_data = getpid();
					soket_Msg_header->msgLength = 0;
					send(connect_fd, buf, sizeof(SOCK_MSG_HEADER_T), 0);
					SCM_TRACE_H("sent tr69 start\n");	
				}
							
			}
		}
	}
	close(connect_fd);
	close(commFd);
	
	return 0;
}

INT32 main(int argc, char **argv)
{
	SCM_TRACE_H("enter ScmDbServiceStartReq\n");
	//osa_debug_callback(cmsOsaTraceHooker);
	//DmTraceHookerRegister(Db_mLog);

	INT32 lRet = ScmStartConnectServer();
	if (SCM_SUCCESS != lRet)
	{
		return lRet;
	}
#if 0
    /* update acs url */
    ScmUpdateAcsConfig();

    /* update logsrv addr */
    ScmUpdateLogsrvConfig();

    /* check "LocationVerifiction", if true, reset "AdminState" */
    ScmResetAdminStateIfNeeded();
    /* register db-callbacks here */
    ScmRegisterDbCbsHandler();
#endif
    SCM_TRACE_H("leave ScmDbServiceStartReq\n");

	SMD_loop();

	return SCM_SUCCESS;
}

