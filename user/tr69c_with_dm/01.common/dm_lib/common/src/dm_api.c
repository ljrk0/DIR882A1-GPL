#include <dirent.h>
#include <dlfcn.h>
#include <linux/limits.h>

#include <ctype.h>
#include <sys/timeb.h>

#include "dm_internal.h"

#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif

DM_HASH_T g_dmHandlerHash;

osa_thread_mutex_init(g_StartInstanceMutex);    /* Enable DmStartReq&DmStopReq multi-call, mutex */
INT32  g_StartInstanceCount = 0;                /* Enable DmStartReq&DmStopReq multi-call, instance number */

DM_CTRL_T g_DmCtrl;

static INT32 DmLoadOneDb(DM_LOAD_DB_HANDLER_REQ_T *pLoadDbHandlerReq)
{
	INT32 lRet = DM_FAILURE;
	INT8 dbNameFullPath[PATH_MAX + 1];
    INT8 *pPrefix = "lib";
    INT8 *pSuffix = "Db.so";
	
    INT8 dlFuncName[64] = {0};
    void *pDlHandler = NULL;
    DB_LOAD_CONFIG_FUN pLoadFunc = NULL;

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmLoadOneDb");
	
	osa_mem_set(dbNameFullPath, 0, sizeof(dbNameFullPath));
	snprintf(dbNameFullPath, PATH_MAX, "%s/%s%s%s", pLoadDbHandlerReq->dbLibPath, 
				pPrefix, pLoadDbHandlerReq->dbName, pSuffix);

    if (access(dbNameFullPath, F_OK) == 0)
    {
        pDlHandler = dlopen(dbNameFullPath, RTLD_NOW|RTLD_GLOBAL);
		if (pDlHandler != NULL)
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ dlopen %s successfully", dbNameFullPath);

			osa_mem_set(dlFuncName, 0, sizeof(dlFuncName));
			snprintf(dlFuncName, sizeof(dlFuncName), "%sDbLoadReq", pLoadDbHandlerReq->dbName);

			pLoadFunc = (DB_LOAD_CONFIG_FUN)dlsym(pDlHandler, dlFuncName);
			if (pLoadFunc != NULL)
			{
				DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ dlsym %s successfully", dlFuncName);
				if (pLoadFunc(pDlHandler, pLoadDbHandlerReq->bDbReset, pLoadDbHandlerReq->workingPath, pLoadDbHandlerReq->loadPath, pLoadDbHandlerReq->backupPath) != DM_SUCCESS)
				{
					DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to load %s DB in DmLoadOneDb", pLoadDbHandlerReq->dbName);
				}
				else
				{
					DM_HANDLER_T *pDmHandler = GetDmHandlerByLibHandler(pDlHandler);
					if(NULL == pDmHandler)
					{
						lRet = DM_RET_INTERNAL_ERROR;
						DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Cann't found DM handler in DmLoadOneDb(%s)",g_DmCtrl.entity);
					}
					else
					{
						pDmHandler->ownerPid = pLoadDbHandlerReq->isGlobalDb ? 0 :  pLoadDbHandlerReq->createPid;
						pDmHandler->createPid = pLoadDbHandlerReq->createPid;
						pDmHandler->bXmlWithComment = pLoadDbHandlerReq->bXmlWithComment;
						lRet = DM_SUCCESS;
					}
				}
			}
			else
			{
				DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ dlsym %s failed, %s", dlFuncName, dlerror());
			}
        }
		else
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ dlopen %s failed, %s", dlFuncName, dlerror());
		}
    }
    else
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ %s does not exist in DmLoadOneDb", dbNameFullPath);
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmLoadOneDb");
    return lRet;
}

static INT32 DmLoadAllDb(DM_LOAD_DB_HANDLER_REQ_T *pLoadDbHandlerReq)
{
    INT8 *pLoadDir = pLoadDbHandlerReq->dbLibPath;
	DIR * loadDir = NULL;
	struct dirent * fileptr = NULL;
    INT8 *pPrefix = "lib";
    INT8 *pSuffix = "Db.so";
    INT8 *pFilename = NULL;
    INT8 *pRootname = NULL;
    INT8 *pStart = NULL;
    INT8 *pEnd = NULL;

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmLoadAllDb");

    if (access(pLoadDir, F_OK) == 0)
    {
        loadDir = opendir(pLoadDir);

        while((fileptr = readdir(loadDir))!=NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ check [%s] in %s path",fileptr->d_name,pLoadDir);
            /* try to find "libDeviceDb.so" or "libDeviceDb_withdmalloc.so" */
            pStart = strstr(fileptr->d_name, pPrefix);

            /* started with "lib" */
            if (pStart != NULL && strncmp(fileptr->d_name, pPrefix, strlen(pPrefix)) == 0)
            {
                DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ prefix matched (%s)",pPrefix);
                pEnd = strstr(fileptr->d_name, pSuffix);

                /* ends with "Db.so" or "Db_withdmalloc.so" */
                if (pEnd != NULL && strcmp(pEnd, pSuffix) == 0)
                {
                    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ suffix matched %s",pSuffix);
                    pFilename = strdup(fileptr->d_name);
                    
                    pRootname = pFilename + strlen(pPrefix);
                    *(pFilename +(pEnd-pStart)) = '\0';

                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ find %s, load it",fileptr->d_name);

					osa_mem_set(pLoadDbHandlerReq->dbName, 0, sizeof(pLoadDbHandlerReq->dbName));
                    strncpy(pLoadDbHandlerReq->dbName, pRootname, sizeof(pLoadDbHandlerReq->dbName) - 1);
					
					if (DmLoadOneDb(pLoadDbHandlerReq) != DM_SUCCESS)
					{
						DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmLoadOneDb failed in DmLoadAllDb");
                        DM_FREE(pFilename);
                        closedir(loadDir);
                        return DM_FAILURE;
                    }

                    DM_FREE(pFilename);
                }
            }

        }

        closedir(loadDir);
    }
    else
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ %s does not exist in DmLoadAllDb",pLoadDir);
        return DM_FAILURE;
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmLoadAllDb");

    return DM_SUCCESS;
}

UINT32 DmConvertEntityToMsgId(INT8 *pEntity)
{
    UINT32 MsgId = (pEntity[0]<<24) | (pEntity[1]<<16) | (pEntity[2]<<8) | (pEntity[3]);

    //DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ pEntity(%s) -> MsgId(0x%x) in DmConvertEntityToMsgId",pEntity,MsgId);

    return MsgId;
}

/**
 *  Create DM non-trans-handler thread
 */
INT32 DmStartNonTransHandler()
{
	INT32 lRet = DM_FAILURE;
	OSA_TP_TASK_T task;
	DM_TRANS_CTRL_T *pNonTransCtrl = &g_DmCtrl.nonTransCtrl;
	
	osa_mem_set(pNonTransCtrl, 0, sizeof(DM_TRANS_CTRL_T));
    osa_mem_set(&task, 0, sizeof(OSA_TP_TASK_T));

	lRet = osa_msg_queue_create(&pNonTransCtrl->msgQ, DM_MSG_SIZE, DM_MSGQ_LENGTH);
	if (lRet != DM_SUCCESS)
	{
		DM_ERROR(DM_ERR_MSGQ_ERROR, "DM Trace ------ osa_msg_queue_create failed in DmStartNonTransHandler"); 
		return lRet;
	}

	task.task_id = 0;
    task.task_cb = (OSA_TP_TASK_CB_FUN)DmNonTransReqEntry;
    task.data = (void*)pNonTransCtrl;
    task.prio = OSA_TP_TASK_PRIO__MED;
    task.sched_type = OSA_TP_TASK_SCHED__DIRECT;
    task.sched_interval = 0;

	lRet = osa_thread_pool_queue_task(&g_DmCtrl.threadPool,&task);

	if (lRet != OSA_SUCCESS)
	{
		DM_ERROR(DM_ERR_THREAD_CREATING_FAILED, "DM Trace ------ failed to queue non-trans-handler task in DmStartNonTransHandler ");
		return lRet;
	}

	return lRet;
}

/**
 *  Destory/Stop DM non-trans-handler thread 
 */
void DmStopNonTransHandler()
{
	INT32 lRet = DM_FAILURE;
	UINT32 msgArray[2] = {0};
	DM_TRANS_CTRL_T *pNonTransCtrl = &g_DmCtrl.nonTransCtrl;

	msgArray[0] = DM_MSG_CODE_LeaveInd;
	msgArray[1] = 0;

	lRet = osa_msg_queue_send(&pNonTransCtrl->msgQ, (void*)msgArray);
	if (lRet != OSA_SUCCESS)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_msg_queue_send failed for DM_MSG_CODE_LeaveInd in DmStopNonTransHandler");
	}
	else
	{
		DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ tell msgq-thread leaving successfully in DmStopNonTransHandler");
	}

	return;
}

/**
 *  DM Db administrative thread
 */
INT32 DmNonTransReqEntry(DM_TRANS_CTRL_T *pNonTransCtrl)
{
    INT32 lRet = DM_FAILURE;
    UINT32 msgArray[2] = {0};
    UINT32 msgCode = 0;
    DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;
    DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;

	//BOOL8 isRunning = DM_TRUE;
	
    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmNonTransReqEntry(%s)",g_DmCtrl.entity);

    //while(isRunning)
	{
		lRet = osa_msg_queue_receive(&pNonTransCtrl->msgQ, (void*)msgArray);
        if (lRet != OSA_SUCCESS)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ recv msg failed in DmNonTransReqEntry(%s)", g_DmCtrl.entity);
            return OSA_FALSE;
        }

        msgCode = msgArray[0];
        pSockMsgRcv = (DM_SOCK_MSG_HEADER_T *)msgArray[1];
		
		switch(msgCode)
		{
            case DM_MSG_CODE_LeaveInd:
            {
                lRet = osa_msg_queue_delete(&pNonTransCtrl->msgQ);
                if (lRet != DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_msg_queue_delete failed for dm-stop-req in DmNonTransReqEntry");
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_msg_queue_delete successfully for dm-stop-req in DmNonTransReqEntry");
                }
				
                return OSA_TRUE;
            }
			case DM_MSG_CODE_LoadDbHandlerReq:
			{
				/* Calling DmLoadDbHandlerReq */
				lRet = DmLoadDbHandlerReq( (DM_LOAD_DB_HANDLER_REQ_T *)(pSockMsgRcv + 1) );

				pSockMsgSnd = pSockMsgRcv;
				pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
				pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
				/* pSockMsgSnd->caller_id  unchanged */
				pSockMsgSnd->msg_id = DM_MSG_CODE_LoadDbHandlerCnf;
				pSockMsgSnd->word_data = lRet;
			
				lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
				if (lRet != DM_SUCCESS)
				{
					DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_LoadDbHandlerCnf failed in DmNonTransReqEntry");
				}
				else
				{
					DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_LoadDbHandlerCnf successfully in DmNonTransReqEntry");
				}
			
				DM_FREE(pSockMsgRcv);
				pSockMsgSnd = NULL;
				break;
			}
			case DM_MSG_CODE_UnloadDbHandlerReq:
			{
				/* Calling DmUnloadDbHandlerReq */
				lRet = DmUnloadDbHandlerReq( (DM_UNLOAD_DB_HANDLER_REQ_T*)(pSockMsgRcv + 1) );

				pSockMsgSnd = pSockMsgRcv;
				pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
				pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
				/* pSockMsgSnd->caller_id  unchanged */
				pSockMsgSnd->msg_id = DM_MSG_CODE_UnloadDbHandlerCnf;
				pSockMsgSnd->word_data = lRet;
			
				lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
				if (lRet != DM_SUCCESS)
				{
					DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_UnloadDbHandlerCnf failed in DmNonTransReqEntry");
				}
				else
				{
					DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_UnloadDbHandlerCnf successfully in DmNonTransReqEntry");
				}
			
				DM_FREE(pSockMsgRcv);
				pSockMsgSnd = NULL;
				break;
			}
			case DM_MSG_CODE_FlushDBfilesReq:
			{
				/* Calling DmFlushDBfiles */
				lRet = DmFlushDBfiles();
			
				pSockMsgSnd = pSockMsgRcv;
				pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
				pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
				/* pSockMsgSnd->caller_id  unchanged */
				pSockMsgSnd->msg_id = DM_MSG_CODE_FlushDBfilesCnf;
				pSockMsgSnd->word_data = lRet;
				pSockMsgSnd->msgLength = 0;
			
				lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
				if (lRet != DM_SUCCESS)
				{
					DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_FlushDBfilesCnf failed in DmNonTransReqEntry");
				}
				else
				{
					DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_FlushDBfilesCnf successfully in DmNonTransReqEntry");
				}
			
				DM_FREE(pSockMsgRcv);
				pSockMsgSnd = NULL;
				break;
			}
			case DM_MSG_CODE_ExtDbOpenReq:
			{
				/* Calling DmExtDbOpenReq */

				INT8 *pExtDbName = NULL;
				INT8 *pXmlFile = NULL;
                DM_CALLER_INFO_T *pCaller = NULL;
                DM_EXT_DB_PROP_T *pExtDbProps = NULL;
			
				pExtDbName = (INT8 *)(pSockMsgRcv+1);
                if (pSockMsgRcv->word_data&0x00000001)
                {
				    pXmlFile = (INT8 *)(pExtDbName+strlen(pExtDbName)+1);
                    pCaller = (DM_CALLER_INFO_T *)(pXmlFile + strlen(pXmlFile) + 1);
                }
				else
				{
                    pXmlFile = NULL;
                    pCaller = (DM_CALLER_INFO_T *)(pExtDbName + strlen(pExtDbName) + 1);
				}

                if (pSockMsgRcv->word_data&0x00000002)
                {
                    pExtDbProps = (DM_EXT_DB_PROP_T *)(pCaller+1);

                    if (pExtDbProps)
                    {
                        pExtDbProps->XmlStyleSheet = (INT8 *)(pExtDbProps+1);
                        pExtDbProps->Xmlns = (INT8 *)(pExtDbProps->XmlStyleSheet + strlen(pExtDbProps->XmlStyleSheet) + 1);
                        pExtDbProps->XmlnsXsi = (INT8 *)(pExtDbProps->Xmlns + strlen(pExtDbProps->Xmlns) + 1);
                        pExtDbProps->XsiSchemaLocation = (INT8 *)(pExtDbProps->XmlnsXsi + strlen(pExtDbProps->XmlnsXsi) + 1);

                        if (!(pSockMsgRcv->word_data&0x00000004))
                        {
                            pExtDbProps->XmlStyleSheet = NULL;
                        }

                        if (!(pSockMsgRcv->word_data&0x00000008))
                        {
                            pExtDbProps->Xmlns = NULL;
                        }

                        if (!(pSockMsgRcv->word_data&0x00000010))
                        {
                            pExtDbProps->XmlnsXsi = NULL;
                        }

                        if (!(pSockMsgRcv->word_data&0x00000020))
                        {
                            pExtDbProps->XsiSchemaLocation = NULL;
                        }
                    }
                }
				// Check DB ownner permission
				lRet = DmCheckOwnerPermission(pExtDbName, pCaller->pid);
				if (lRet == DM_SUCCESS)
				{
					/* Calling actual process */
					lRet = DmExtDbOpen(pExtDbName, pXmlFile, pExtDbProps);
				}
			
				/* reuse the msg buffer */
				pSockMsgSnd = pSockMsgRcv;
			
				pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
				pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
				/* pSockMsgSnd->caller_id  unchanged */
				pSockMsgSnd->msg_id = DM_MSG_CODE_ExtDbOpenCnf;
				pSockMsgSnd->word_data = lRet;
			
				lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
				if (lRet != DM_SUCCESS)
				{
					DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_ExtDbOpenCnf failed in DmNonTransReqEntry");
				}
				else
				{
					DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_ExtDbOpenCnf successfully in DmNonTransReqEntry");
				}
			
				DM_FREE(pSockMsgRcv);
				pSockMsgSnd = NULL;
			
				break;
			
			}
			case DM_MSG_CODE_ExtDbCloseReq:
			{
				/* Calling DmExtDbCloseReq */
				INT8 *pExtDbName = NULL;
				INT8 *pXmlFile = NULL;	
                BOOL8 *pDocReleased = NULL;

				pExtDbName = (INT8 *)(pSockMsgRcv+1);
			    if (pSockMsgRcv->word_data==DM_TRUE)
			    {
                    pXmlFile = (INT8 *)(pExtDbName+strlen(pExtDbName)+1);
                    pDocReleased = (BOOL8 *)(pXmlFile+strlen(pXmlFile)+1);
			    }
                else
                {
                    pDocReleased = (BOOL8 *)(pExtDbName+strlen(pExtDbName)+1);
                }
				// Check DB ownner permission
				DM_CALLER_INFO_T *pCaller =  (DM_CALLER_INFO_T *)(pDocReleased + 1);
				lRet = DmCheckOwnerPermission(pExtDbName, pCaller->pid);
				if (lRet == DM_SUCCESS)
				{
					/* Calling DmExtDbFreeXmlDoc */
					lRet = DmExtDbClose(pExtDbName,pXmlFile,*pDocReleased);
				}
			
				/* reuse the msg buffer */
				pSockMsgSnd = pSockMsgRcv;
			
				pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
				pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
				/* pSockMsgSnd->caller_id  unchanged */
				pSockMsgSnd->msg_id = DM_MSG_CODE_ExtDbCloseCnf;
				pSockMsgSnd->word_data = lRet;
			
				lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
				if (lRet != DM_SUCCESS)
				{
					DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_ExtDbCloseCnf failed in DmNonTransReqEntry");
				}
				else
				{
					DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_ExtDbCloseCnf successfully in DmNonTransReqEntry");
				}
			
				DM_FREE(pSockMsgRcv);
				pSockMsgSnd = NULL;
			
				break;
			
			}
			case DM_MSG_CODE_RegisterNotifyHandlerReq:
			{
				/* Calling DmRegisterNotifyHandlerReq */
				UINT32 cnt = 0;
			
				INT8 *pTmp = NULL;
				DM_NOTIFY_HANDLER_REG_PARA_T *  pPara = (DM_NOTIFY_HANDLER_REG_PARA_T *)(pSockMsgRcv+1);
			
				pPara->paParaPath = (INT8**)(pPara+1);
			
				pTmp = (INT8*)(pPara->paParaPath + pPara->lParaCnt);
				for (cnt=0; cnt<pPara->lParaCnt; cnt++)
				{
					pPara->paParaPath[cnt] = pTmp;
					pTmp += strlen(pTmp) +1;
				}
			
				pPara->pCallbackFunc = NULL;
			
				lRet = (INT32)DmRegisterNotifyHandlerReq(pPara);
			
				/* reuse the msg buffer */
				pSockMsgSnd = pSockMsgRcv;
			
				pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
				pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
				/* pSockMsgSnd->caller_id  unchanged */
				pSockMsgSnd->msg_id = DM_MSG_CODE_RegisterNotifyHandlerCnf;
				pSockMsgSnd->word_data = lRet;
			
				lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
				if (lRet != DM_SUCCESS)
				{
					DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_RegisterNotifyHandlerCnf failed in DmNonTransReqEntry");
				}
				else
				{
					DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_RegisterNotifyHandlerCnf successfully in DmNonTransReqEntry");
				}
			
				DM_FREE(pSockMsgRcv);
				pSockMsgSnd = NULL;
			
				break;
			
			}
			case DM_MSG_CODE_DeregisterNotifyHandlerReq:
			{
				/* Calling DmDeregisterNotifyHandlerReq */
				UINT32 cnt = 0;
			
				INT8 *pTmp = NULL;
				DM_NOTIFY_HANDLER_REG_PARA_T *  pPara = (DM_NOTIFY_HANDLER_REG_PARA_T *)(pSockMsgRcv+1);
			
				pPara->paParaPath = (INT8**)(pPara+1);
			
				pTmp = (INT8*)(pPara->paParaPath + pPara->lParaCnt);
				for (cnt=0; cnt<pPara->lParaCnt; cnt++)
				{
					pPara->paParaPath[cnt] = pTmp;
					pTmp += strlen(pTmp) +1;
				}
			
				pPara->pCallbackFunc = NULL;
			
				lRet = (INT32)DmDeregisterNotifyHandlerReq(pPara);
			
				/* reuse the msg buffer */
				pSockMsgSnd = pSockMsgRcv;
			
				pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
				pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
				/* pSockMsgSnd->caller_id  unchanged */
				pSockMsgSnd->msg_id = DM_MSG_CODE_DeregisterNotifyHandlerCnf;
				pSockMsgSnd->word_data = lRet;
			
				lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
				if (lRet != DM_SUCCESS)
				{
					DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_DeregisterNotifyHandlerCnf failed in DmNonTransReqEntry");
				}
				else
				{
					DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_DeregisterNotifyHandlerCnf successfully in DmNonTransReqEntry");
				}
			
				DM_FREE(pSockMsgRcv);
				pSockMsgSnd = NULL;
			
				break;
			
			}
			case DM_MSG_CODE_RegisterGetHandlerReq:
			{
				/* Calling DmRegisterGetHandlerReq */
				UINT32 cnt = 0;
			
				INT8 *pTmp = NULL;
				DM_REG_GET_HANDLER_PARA_T *  pPara = (DM_REG_GET_HANDLER_PARA_T *)(pSockMsgRcv+1);
			
				pPara->paParaPath = (INT8**)(pPara+1);
			
				pTmp = (INT8*)(pPara->paParaPath + pPara->lParaCnt);
				for (cnt=0; cnt<pPara->lParaCnt; cnt++)
				{
					pPara->paParaPath[cnt] = pTmp;
					pTmp += strlen(pTmp) +1;
				}
			
				pPara->pCallbackFunc = NULL;
			
				lRet = (INT32)DmRegisterGetHandlerReq(pPara);
			
				/* reuse the msg buffer */
				pSockMsgSnd = pSockMsgRcv;
			
				pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
				pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
				/* pSockMsgSnd->caller_id  unchanged */
				pSockMsgSnd->msg_id = DM_MSG_CODE_RegisterGetHandlerCnf;
				pSockMsgSnd->word_data = lRet;
			
				lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
				if (lRet != DM_SUCCESS)
				{
					DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_RegisterGetHandlerCnf failed in DmNonTransReqEntry");
				}
				else
				{
					DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_RegisterGetHandlerCnf successfully in DmNonTransReqEntry");
				}
			
				DM_FREE(pSockMsgRcv);
				pSockMsgSnd = NULL;
			
				break;
			
			}
			case DM_MSG_CODE_DeregisterGetHandlerReq:
			{
				/* Calling DmDeregisterGetHandlerReq */
				UINT32 cnt = 0;
			
				INT8 *pTmp = NULL;
				DM_REG_GET_HANDLER_PARA_T *  pPara = (DM_REG_GET_HANDLER_PARA_T *)(pSockMsgRcv+1);
			
				pPara->paParaPath = (INT8**)(pPara+1);
			
				pTmp = (INT8*)(pPara->paParaPath + pPara->lParaCnt);
				for (cnt=0; cnt<pPara->lParaCnt; cnt++)
				{
					pPara->paParaPath[cnt] = pTmp;
					pTmp += strlen(pTmp) +1;
				}
			
				pPara->pCallbackFunc = NULL;
			
				lRet = (INT32)DmDeregisterGetHandlerReq(pPara);
			
				/* reuse the msg buffer */
				pSockMsgSnd = pSockMsgRcv;
			
				pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
				pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
				/* pSockMsgSnd->caller_id  unchanged */
				pSockMsgSnd->msg_id = DM_MSG_CODE_DeregisterGetHandlerCnf;
				pSockMsgSnd->word_data = lRet;
			
				lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
				if (lRet != DM_SUCCESS)
				{
					DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_DeregisterGetHandlerCnf failed in DmNonTransReqEntry");
				}
				else
				{
					DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_DeregisterGetHandlerCnf successfully in DmNonTransReqEntry");
				}
			
				DM_FREE(pSockMsgRcv);
				pSockMsgSnd = NULL;
			
				break;
			}
            case DM_MSG_CODE_AllocateMemoryReq:
            {
                OSA_SHM_T *pMemReq = NULL;
                void *addr = NULL;

                lRet = DmAllocateMemReq(pSockMsgRcv->word_data, &addr);

                pSockMsgSnd = (DM_SOCK_MSG_HEADER_T*)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+sizeof(OSA_SHM_T));
                if (pSockMsgSnd==NULL)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ memory allocation failed for pSockMsgSnd in DmNonTransReqEntry");
                    break;
                }

                osa_mem_set(pSockMsgSnd,0,sizeof(DM_SOCK_MSG_HEADER_T)+sizeof(OSA_SHM_T));

                pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
                pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
                pSockMsgSnd->msg_id = DM_MSG_CODE_AllocateMemoryCnf;
                pSockMsgSnd->word_data = lRet;
                pSockMsgSnd->caller_id = pSockMsgRcv->caller_id;
                pSockMsgSnd->msgLength = sizeof(OSA_SHM_T);

                pMemReq = addr;
				if(lRet == DM_SUCCESS)
				{
                	osa_mem_copy(pSockMsgSnd+1, pMemReq-1, sizeof(OSA_SHM_T));
				}
				else
				{
					osa_mem_set(pSockMsgSnd+1, 0, sizeof(OSA_SHM_T));
				}	

                lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);

                if (lRet != DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_AllocateMemoryCnf failed in DmNonTransReqEntry");
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_AllocateMemoryCnf successfully in DmNonTransReqEntry");
                }

                DM_FREE(pSockMsgRcv);
                DM_FREE(pSockMsgSnd);

                break;
            }
            case DM_MSG_CODE_ReleaseMemoryReq:
            {
                OSA_SHM_T *pShmReq = (OSA_SHM_T *)pSockMsgRcv->word_data;/* mapping addr in server side */
                void *addr = NULL;

                addr = (void*)(pShmReq+1);
                lRet = DmReleaseMemReq(&addr);

                pSockMsgSnd = pSockMsgRcv;

                pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
                pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
                pSockMsgSnd->msg_id = DM_MSG_CODE_ReleaseMemoryCnf;
                /* pSockMsgSnd->caller_id  unchanged */
                pSockMsgSnd->word_data = lRet;

                lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);

                if (lRet != DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_ReleaseMemoryCnf failed in DmNonTransReqEntry");
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_ReleaseMemoryCnf successfully in DmNonTransReqEntry");
                }

                DM_FREE(pSockMsgSnd);
                break;
            }
			case DM_MSG_CODE_RegisterSetHandlerReq:
			{
				/* Calling DmRegisterSetHandlerReq */
				UINT32 cnt = 0;
			
				INT8 *pTmp = NULL;
				DM_REG_SET_HANDLER_PARA_T *  pPara = (DM_REG_SET_HANDLER_PARA_T *)(pSockMsgRcv+1);
			
				pPara->paParaPath = (INT8**)(pPara+1);
			
				pTmp = (INT8*)(pPara->paParaPath + pPara->lParaCnt);
				for (cnt=0; cnt<pPara->lParaCnt; cnt++)
				{
					pPara->paParaPath[cnt] = pTmp;
					pTmp += strlen(pTmp) +1;
				}
			
				pPara->pCallbackFunc = NULL;
			
				lRet = (INT32)DmRegisterSetHandlerReq(pPara);
			
				/* reuse the msg buffer */
				pSockMsgSnd = pSockMsgRcv;
			
				pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
				pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
				/* pSockMsgSnd->caller_id  unchanged */
				pSockMsgSnd->msg_id = DM_MSG_CODE_RegisterSetHandlerCnf;
				pSockMsgSnd->word_data = lRet;
			
				lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
				if (lRet != DM_SUCCESS)
				{
					DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_RegisterSetHandlerCnf failed in DmNonTransReqEntry");
				}
				else
				{
					DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_RegisterSetHandlerCnf successfully in DmNonTransReqEntry");
				}
			
				DM_FREE(pSockMsgRcv);
				pSockMsgSnd = NULL;
			
				break;
			
			}
			case DM_MSG_CODE_DeregisterSetHandlerReq:
			{
				/* Calling DmDeregisterSetHandlerReq */
				UINT32 cnt = 0;
			
				INT8 *pTmp = NULL;
				DM_REG_SET_HANDLER_PARA_T *  pPara = (DM_REG_SET_HANDLER_PARA_T *)(pSockMsgRcv+1);
			
				pPara->paParaPath = (INT8**)(pPara+1);
			
				pTmp = (INT8*)(pPara->paParaPath + pPara->lParaCnt);
				for (cnt=0; cnt<pPara->lParaCnt; cnt++)
				{
					pPara->paParaPath[cnt] = pTmp;
					pTmp += strlen(pTmp) +1;
				}
			
				pPara->pCallbackFunc = NULL;
			
				lRet = (INT32)DmDeregisterSetHandlerReq(pPara);
			
				/* reuse the msg buffer */
				pSockMsgSnd = pSockMsgRcv;
			
				pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
				pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
				/* pSockMsgSnd->caller_id  unchanged */
				pSockMsgSnd->msg_id = DM_MSG_CODE_DeregisterSetHandlerCnf;
				pSockMsgSnd->word_data = lRet;
			
				lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
				if (lRet != DM_SUCCESS)
				{
					DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_DeregisterSetHandlerCnf failed in DmNonTransReqEntry");
				}
				else
				{
					DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_DeregisterSetHandlerCnf successfully in DmNonTransReqEntry");
				}
			
				DM_FREE(pSockMsgRcv);
				pSockMsgSnd = NULL;
			
				break;
			}
			case DM_MSG_CODE_AddOnControlReq:
			{
				/* Calling DM_MSG_CODE_AddOnControlReq */
				DM_ADDON_CONTROL_REQ_T* pAddOnCtrl = (DM_ADDON_CONTROL_REQ_T *)(pSockMsgRcv+1);
				lRet = DmAddOnControlReq(pAddOnCtrl);

				/* reuse the msg buffer */
				pSockMsgSnd = pSockMsgRcv;
				pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
				pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
				/* pSockMsgSnd->caller_id  unchanged */
				pSockMsgSnd->msg_id = DM_MSG_CODE_AddOnControlCnf;
				pSockMsgSnd->word_data = lRet;
			
				lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
				if (lRet != DM_SUCCESS)
				{
					DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_AddOnControlCnf failed in DmNonTransReqEntry");
				}
				else
				{
					DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_AddOnControlCnf successfully in DmNonTransReqEntry");
				}
			
				DM_FREE(pSockMsgRcv);
				pSockMsgSnd = NULL;
			
				break;
			}
			case DM_MSG_CODE_AddOnReqMsgReq:
			{
				/* Calling DM_MSG_CODE_AddOnControlReq */
				DM_ADDON_REQ_MESSAGE_T* pAddOnReqMsg = (DM_ADDON_REQ_MESSAGE_T *)(pSockMsgRcv+1);

				INT8 *pTmp = (INT8 *)(pAddOnReqMsg + 1);
				pAddOnReqMsg->reqMsg = pTmp;

				pTmp += pAddOnReqMsg->reqMsgSize;
				pAddOnReqMsg->addonName = pTmp;

				lRet = DmAddOnRouteMsgReq(pAddOnReqMsg);

				/* reuse the msg buffer */
				pSockMsgSnd = pSockMsgRcv;
				pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
				pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
				/* pSockMsgSnd->caller_id  unchanged */
				pSockMsgSnd->msg_id = DM_MSG_CODE_AddOnReqMsgCnf;
				pSockMsgSnd->word_data = lRet;
			
				lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
				if (lRet != DM_SUCCESS)
				{
					DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_AddOnReqMsgCnf failed in DmNonTransReqEntry");
				}
				else
				{
					DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_AddOnReqMsgCnf successfully in DmNonTransReqEntry");
				}
			
				DM_FREE(pSockMsgRcv);
				pSockMsgSnd = NULL;

				break;
			}

			default:
				DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid msg code 0x%x in DmNonTransReqEntry(%s)", msgCode, g_DmCtrl.entity);
				DM_FREE(pSockMsgRcv);
				break;
		}
	}

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmNonTransReqEntry(%s)",g_DmCtrl.entity);

    return OSA_FALSE;
}

INT32 DmServerMsgHandler(DM_TRANS_CTRL_T *pTransCtrl)
{
    INT32 lRet = DM_FAILURE;
    UINT32 msgArray[2] = {0};
    UINT32 msgCode = 0;
    DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;
    DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmServerMsgHandler(%s)",g_DmCtrl.entity);

    //for (;;)
    {
        lRet = osa_msg_queue_receive(&pTransCtrl->msgQ,(void*)msgArray);

        if (lRet != OSA_SUCCESS)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ recv msg failed in DmServerMsgHandler(%s)",g_DmCtrl.entity);
            return OSA_FALSE;
        }

        msgCode = msgArray[0];
        pSockMsgRcv = (DM_SOCK_MSG_HEADER_T *)msgArray[1];

        switch(msgCode)
        {
            case DM_MSG_CODE_LeaveInd:
            {
                lRet = osa_msg_queue_delete(&pTransCtrl->msgQ);
            
                if (lRet != DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_msg_queue_delete failed for dm-stop-req in DmServerMsgHandler");
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_msg_queue_delete successfully for dm-stop-req in DmServerMsgHandler");
                }
            
                DM_FREE(pTransCtrl);
            
                return OSA_TRUE;
            }
            case DM_MSG_CODE_TransactionStartReq:
            {
                /* Calling TransactionStartReq */
				DM_TRANSACTION_REQ_T *pTransaction = (DM_TRANSACTION_REQ_T *)(pSockMsgRcv + 1);

                UINT32 transId = 0;
                INT8 *pTransId = NULL;
				
				// Check DB ownner permission
				DM_CALLER_INFO_T *pCaller = (DM_CALLER_INFO_T *)(pTransaction + 1);
				lRet = DmCheckOwnerPermission(pTransaction->RootNodeToLock, pCaller->pid);
				if (lRet == DM_SUCCESS)
				{
					/* Calling actual process */
					lRet = DmTransactionStartReq(pTransaction);
				}

                /* save trans */
                if (lRet == DM_RET_SUCCESS)
                {
                    transId = pTransaction->TransactionId;
                    pTransId = DmUintToStr((UINT32)transId);

                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ save trans-id(%d) = %s to hash for later use, entity(%s) lock(%s) in DmServerMsgHandler",transId,pTransId,pTransaction->AccessEntity,pTransaction->RootNodeToLock);
                    DmHashAdd(&g_DmCtrl.transHash, pTransId, pTransCtrl);

                    DM_FREE(pTransId);
                }

                pSockMsgSnd = pSockMsgRcv;
                pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
                pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
                pSockMsgSnd->msg_id = DM_MSG_CODE_TransactionStartCnf;
                /* pSockMsgSnd->caller_id  unchanged */
                pSockMsgSnd->word_data = lRet;
            
                lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
            
                if (lRet != DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_TransactionStartCnf failed, transId=%d, in DmServerMsgHandler",transId);
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_TransactionStartCnf successfully, transId=%d, in DmServerMsgHandler",transId);
                }
            
                DM_FREE(pSockMsgRcv);
                pSockMsgSnd = NULL;
                break;
            }
            case DM_MSG_CODE_TransactionStopReq:
            {
                /* Calling TransactionStopReq */
                UINT32 transId = *((UINT32*)(pSockMsgRcv+1));
                INT8 *pTransId = DmUintToStr((UINT32)transId);

                lRet = DmTransactionStopReq(*((INT32*)(pSockMsgRcv+1)));

                pSockMsgSnd = pSockMsgRcv;
                pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
                pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
                pSockMsgSnd->msg_id = DM_MSG_CODE_TransactionStopCnf;
                pSockMsgSnd->word_data = lRet;
                /* pSockMsgSnd->caller_id  unchanged */
                pSockMsgSnd->msgLength = 0;
            
                lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
            
                if (lRet != DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_TransactionStopCnf failed, transId=%d, in DmServerMsgHandler",transId);
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_TransactionStopCnf successfully, transId=%d, in DmServerMsgHandler",transId);
                }

                /* release the trans */
                DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ delete trans-id(%d) = %s, trans-stop in DmServerMsgHandler",transId,pTransId);
                DmHashDel(&g_DmCtrl.transHash,pTransId);
                DM_FREE(pTransId);

                lRet = osa_msg_queue_delete(&pTransCtrl->msgQ);

                if (lRet != DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_msg_queue_delete failed for %d trans-stop-req in DmServerMsgHandler",transId);
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_msg_queue_delete successfully for %d trans-stop-req in DmServerMsgHandler",transId);
                }

                DM_FREE(pTransCtrl);

                DM_FREE(pSockMsgRcv);
                pSockMsgSnd = NULL;
                return OSA_TRUE;
            }
            case DM_MSG_CODE_ExtDbNewParaReq:
            {
                /* Calling DmExtDbNewParaReq */
                UINT32 cnt = 0;
                UINT32 iCnt = 0;
                INT8 *pTmp = NULL;
                INT8 *pExtDbName = (INT8 *)(pSockMsgRcv + 1);
                DM_EXT_DB_NEW_PARA_REQ_T *pNewParas = (DM_EXT_DB_NEW_PARA_REQ_T *)(pExtDbName + strlen(pExtDbName) + 1);

                pNewParas->pNewParas = (DM_EXT_DB_NEW_PARA_T *)(pNewParas+1);

                pTmp = (INT8 *)(pNewParas->pNewParas+pNewParas->ulParaCnt);

                for (cnt=0; cnt<pNewParas->ulParaCnt; cnt++)
                {
                    pNewParas->pNewParas[cnt].pParaToRefer = pTmp;
                    pTmp += strlen(pNewParas->pNewParas[cnt].pParaToRefer) + 1;

    				pNewParas->pNewParas[cnt].pParaName = pTmp;
                    pTmp += strlen(pNewParas->pNewParas[cnt].pParaName) + 1;

    				pNewParas->pNewParas[cnt].pParaValue = pTmp;
                    pTmp += strlen(pNewParas->pNewParas[cnt].pParaValue) + 1;

                    if (pNewParas->pNewParas[cnt].ulAttrCnt>0)
                    {
                        pNewParas->pNewParas[cnt].pAttrValue = (DM_ATTR_NODE_T *)pTmp;
                        pTmp += sizeof(DM_ATTR_NODE_T)*pNewParas->pNewParas[cnt].ulAttrCnt;

                        for (iCnt=0;iCnt<pNewParas->pNewParas[cnt].ulAttrCnt;iCnt++)
                        {
                            pNewParas->pNewParas[cnt].pAttrValue[iCnt].pAttrName = pTmp;
                            pTmp += strlen(pNewParas->pNewParas[cnt].pAttrValue[iCnt].pAttrName) + 1;

                            pNewParas->pNewParas[cnt].pAttrValue[iCnt].pAttrValue = pTmp;
                            pTmp += pNewParas->pNewParas[cnt].pAttrValue[iCnt].ulAttrValueSize;
                        }
                    }
                }

                lRet = DmExtDbNewParaReq(pExtDbName, pNewParas);

                /* reuse the message buffer */
                pSockMsgSnd = pSockMsgRcv;
                pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
                pSockMsgSnd->sender_id   = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
                pSockMsgSnd->msg_id      = DM_MSG_CODE_ExtDbNewParaCnf;
                /* pSockMsgSnd->caller_id  unchanged */
                pSockMsgSnd->word_data = lRet;
            
                lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
                if (lRet != DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_ExtDbNewParaCnf failed in DmServerMsgHandler");
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_ExtDbNewParaCnf successfully in DmServerMsgHandler");
                }
            
                DM_FREE(pSockMsgRcv);
                pSockMsgSnd = NULL;
            
                break;
            }
            case DM_MSG_CODE_GetParaCValueReq:
            {
                /* Calling DmGetParaCValueReq */
                DM_GET_PARA_C_VALUE_REQ_T *pGetPara = (DM_GET_PARA_C_VALUE_REQ_T *)(pSockMsgRcv+1);
            
                pGetPara->pucPath = (INT8*)(pGetPara+1);

                if (pSockMsgRcv->options & DM_SOCK_MSG_OPTION__DATA_IN_SHM)
                {
                    OSA_SHM_T *pShmReq = (OSA_SHM_T *)(*((UINT32*)(pGetPara->pucPath+strlen(pGetPara->pucPath)+1)));
                    pGetPara->pValue = (void*)(pShmReq+1);/* modify memory address */
                }
                else
                {
                    pGetPara->pValue = pGetPara->pucPath + strlen(pGetPara->pucPath)+1;
                }

                lRet = DmGetParaCValueReq(pGetPara);
            
                /* reuse the memory */
                pSockMsgSnd = pSockMsgRcv;
                pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
                pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
                pSockMsgSnd->msg_id = DM_MSG_CODE_GetParaCValueCnf;
                /* pSockMsgSnd->caller_id  unchanged */
                pSockMsgSnd->word_data = lRet;
            
                lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
            
                if (lRet != DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_GetParaCValueCnf failed in DmServerMsgHandler");
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_GetParaCValueCnf successfully in DmServerMsgHandler");
                }
            
                DM_FREE(pSockMsgRcv);
                pSockMsgSnd = NULL;
            
                break;
            }
            case DM_MSG_CODE_SetParaCValueReq:
            {
                /* Calling DmSetParaCValueReq */
                DM_SET_PARA_C_VALUE_REQ_T *pSetPara = (DM_SET_PARA_C_VALUE_REQ_T *)(pSockMsgRcv+1);
            
                pSetPara->pucPath = (INT8*)(pSetPara+1);

                if (pSockMsgRcv->options & DM_SOCK_MSG_OPTION__DATA_IN_SHM)
                {
                    OSA_SHM_T *pShmReq = (OSA_SHM_T *)(*((UINT32*)(pSetPara->pucPath+strlen(pSetPara->pucPath)+1)));
                    pSetPara->pValue = (void*)(pShmReq+1);/* modify memory address */
                }
                else
                {
                    pSetPara->pValue = pSetPara->pucPath + strlen(pSetPara->pucPath)+1;
                }

                lRet = DmSetParaCValueReq(pSetPara);
            
                /* reuse the memory */
                pSockMsgSnd = pSockMsgRcv;
                pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
                pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
                pSockMsgSnd->msg_id = DM_MSG_CODE_SetParaCValueCnf;
                /* pSockMsgSnd->caller_id  unchanged */
                pSockMsgSnd->word_data = lRet;
            
                lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
            
                if (lRet != DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_SetParaCValueCnf failed in DmServerMsgHandler");
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_SetParaCValueCnf successfully in DmServerMsgHandler");
                }
            
                DM_FREE(pSockMsgRcv);
                pSockMsgSnd = NULL;
            
                break;
            }
            case DM_MSG_CODE_AddObjectByIdReq:
            {
                /* Calling DmAddObjectByIdReq */
                DM_ADD_OBJ_BY_ID_REQ_T *pAddPara = (DM_ADD_OBJ_BY_ID_REQ_T *)(pSockMsgRcv+1);
            
                pAddPara->pucPath = (INT8*)(pAddPara+1);

                if (pSockMsgRcv->options & DM_SOCK_MSG_OPTION__DATA_IN_SHM)
                {
                    OSA_SHM_T *pShmReq = (OSA_SHM_T *)(*((UINT32*)(pAddPara->pucPath+strlen(pAddPara->pucPath)+1)));
                    pAddPara->pObject = (void*)(pShmReq+1);/* modify memory address */
                }
                else
                {
                    pAddPara->pObject = pAddPara->pucPath + strlen(pAddPara->pucPath)+1;
                }

                lRet = DmAddObjectByIdReq(pAddPara);
            
                /* reuse the memory */
                pSockMsgSnd = pSockMsgRcv;
                pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
                pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
                pSockMsgSnd->msg_id = DM_MSG_CODE_AddObjectByIdCnf;
                /* pSockMsgSnd->caller_id  unchanged */
                pSockMsgSnd->word_data = lRet;
            
                lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
            
                if (lRet != DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_AddObjectByIdCnf failed in DmServerMsgHandler");
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_AddObjectByIdCnf successfully in DmServerMsgHandler");
                }
            
                DM_FREE(pSockMsgRcv);
                pSockMsgSnd = NULL;
            
                break;
            }
            case DM_MSG_CODE_DelObjectByIdReq:
            {
                /* Calling DmDelObjectByIdReq */
                DM_DEL_OBJ_BY_ID_REQ_T *  pDelPara = (DM_DEL_OBJ_BY_ID_REQ_T *)(pSockMsgRcv+1);
            
                pDelPara->pucPath = (INT8*)(pDelPara+1);
            
                lRet = DmDelObjectByIdReq(pDelPara);
            
                /* reuse the memory */
                pSockMsgSnd = pSockMsgRcv;
                pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
                pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
                pSockMsgSnd->msg_id = DM_MSG_CODE_DelObjectByIdCnf;
                /* pSockMsgSnd->caller_id  unchanged */
                pSockMsgSnd->word_data = lRet;
            
                lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
            
                if (lRet != DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_DelObjectByIdCnf failed in DmServerMsgHandler");
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_DelObjectByIdCnf successfully in DmServerMsgHandler");
                }
            
                DM_FREE(pSockMsgRcv);
                pSockMsgSnd = NULL;
            
                break;
            }
            case DM_MSG_CODE_GetParaValueReq:
            {
                /* Calling DmGetParaValueReq */
                UINT32 cnt = 0;
                UINT32 sndMsgBody = 0;
                INT8 *pParaName = NULL;
                DM_GET_PARA_VALUE_REQ_T    *pGetParaValueReq = (DM_GET_PARA_VALUE_REQ_T *)(pSockMsgRcv+1);
            
                pGetParaValueReq->pNameList = (PARAMETER_NAME_LIST_T*)(pGetParaValueReq+1);
                pGetParaValueReq->pNameList->pucParamName = (PARAMETER_NAME_T*)(pGetParaValueReq->pNameList+1);
            
                pParaName = (INT8*)(pGetParaValueReq->pNameList->pucParamName+pGetParaValueReq->pNameList->udwCnt);
            
                for (cnt=0; cnt<pGetParaValueReq->pNameList->udwCnt; cnt++)
                {
                    pGetParaValueReq->pNameList->pucParamName[cnt].pucName = pParaName;
                    pParaName += strlen(pParaName)+1;
                }
            
                pGetParaValueReq->pvList = NULL;
                pGetParaValueReq->pfnFree = NULL;
            
                lRet = DmGetParaValueReq(pGetParaValueReq);
            
                /* compute the memory size of the response */
                if (lRet == DM_RET_SUCCESS)
                {
                    sndMsgBody += sizeof(PARAMETER_VALUE_LIST_T);

                    if (pGetParaValueReq->pvList->udwCnt>0)
                    {
                        sndMsgBody += sizeof(PARAMETER_VALUE_T)*(pGetParaValueReq->pvList->udwCnt);
                    }

                    for (cnt=0; cnt<pGetParaValueReq->pvList->udwCnt; cnt++)
                    {
                        if (pGetParaValueReq->pvList->pstParamValue[cnt].pucName)
                        {
                            sndMsgBody += strlen(pGetParaValueReq->pvList->pstParamValue[cnt].pucName)+1;
                        }
                        if (pGetParaValueReq->pvList->pstParamValue[cnt].pucValue)
                        {
                            sndMsgBody += strlen(pGetParaValueReq->pvList->pstParamValue[cnt].pucValue)+1;
                        }
                        if (pGetParaValueReq->pvList->pstParamValue[cnt].pucType)
                        {
                            sndMsgBody += strlen((INT8*)(pGetParaValueReq->pvList->pstParamValue[cnt].pucType))+1;
                        }
                    }
                }

                pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+sndMsgBody);

                if (pSockMsgSnd == NULL)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed for GetParaValueCnf in DmServerMsgHandler");
                    break;
                }

                osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+sndMsgBody);

                pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
                pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
                pSockMsgSnd->msg_id = DM_MSG_CODE_GetParaValueCnf;
                pSockMsgSnd->word_data = lRet;
                pSockMsgSnd->caller_id = pSockMsgRcv->caller_id;
                pSockMsgSnd->msgLength = sndMsgBody;

                if (lRet == DM_RET_SUCCESS)
                {
                    /* fill the msg body */
                    pParaName = (INT8*)(pSockMsgSnd+1);

                    osa_mem_copy(pParaName, pGetParaValueReq->pvList, sizeof(PARAMETER_VALUE_LIST_T));
                    pParaName += sizeof(PARAMETER_VALUE_LIST_T);

                    if (pGetParaValueReq->pvList->udwCnt>0)
                    {
                        osa_mem_copy(pParaName, pGetParaValueReq->pvList->pstParamValue, sizeof(PARAMETER_VALUE_T)*(pGetParaValueReq->pvList->udwCnt));
                        pParaName += sizeof(PARAMETER_VALUE_T)*(pGetParaValueReq->pvList->udwCnt);
                    }

                    for (cnt=0; cnt<pGetParaValueReq->pvList->udwCnt; cnt++)
                    {
                        if (pGetParaValueReq->pvList->pstParamValue[cnt].pucName)
                        {
                            strcpy(pParaName, pGetParaValueReq->pvList->pstParamValue[cnt].pucName);
                            pParaName += strlen(pGetParaValueReq->pvList->pstParamValue[cnt].pucName)+1;
                        }
                        if (pGetParaValueReq->pvList->pstParamValue[cnt].pucValue)
                        {
                            strcpy(pParaName, pGetParaValueReq->pvList->pstParamValue[cnt].pucValue);
                            pParaName += strlen(pGetParaValueReq->pvList->pstParamValue[cnt].pucValue)+1;
                        }
                        if (pGetParaValueReq->pvList->pstParamValue[cnt].pucType)
                        {
                            strcpy(pParaName, (INT8*)(pGetParaValueReq->pvList->pstParamValue[cnt].pucType));
                            pParaName += strlen((INT8*)(pGetParaValueReq->pvList->pstParamValue[cnt].pucType))+1;
                        }
                    }
                }

                lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
            
                if (lRet != DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_GetParaValueCnf failed in DmServerMsgHandler");
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_GetParaValueCnf successfully in DmServerMsgHandler");
                }

                if (pGetParaValueReq->pfnFree)
                {
                    pGetParaValueReq->pfnFree(pGetParaValueReq);
                }

                DM_FREE(pSockMsgRcv);
                DM_FREE(pSockMsgSnd);
            
                break;
            }
            case DM_MSG_CODE_SetParaValueReq:
            {
                /* Calling DmSetParaValueReq */
                UINT32 cnt = 0;
                INT8 *pTmp = NULL;
                DM_SET_PARA_VALUE_REQ_T    *pSetParaValueReq = (DM_SET_PARA_VALUE_REQ_T *)(pSockMsgRcv+1);
            
                pSetParaValueReq->pvList = (PARAMETER_VALUE_LIST_T*)(pSetParaValueReq+1);
                pSetParaValueReq->pvList->pstParamValue = (PARAMETER_VALUE_T*)(pSetParaValueReq->pvList+1);
                pSetParaValueReq->pFaultCodeList = (UINT16*)(pSetParaValueReq->pvList->pstParamValue + pSetParaValueReq->pvList->udwCnt);
            
                pTmp = (INT8*)(pSetParaValueReq->pFaultCodeList+pSetParaValueReq->pvList->udwCnt);
            
                for (cnt=0; cnt<pSetParaValueReq->pvList->udwCnt; cnt++)
                {
                    pSetParaValueReq->pvList->pstParamValue[cnt].pucName = pTmp;
                    pTmp += strlen(pTmp)+1;
            
                    pSetParaValueReq->pvList->pstParamValue[cnt].pucValue = pTmp;
                    pTmp += strlen(pTmp)+1;
            
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ pSetParaValueReq %d param %s = %s in DmServerMsgHandler",cnt+1,pSetParaValueReq->pvList->pstParamValue[cnt].pucName,pSetParaValueReq->pvList->pstParamValue[cnt].pucValue);
            
                }
            
                lRet = DmSetParaValueReq(pSetParaValueReq);
            
                /* reuse the message buffer */
                pSockMsgSnd = pSockMsgRcv;
            
                pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
                pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
                pSockMsgSnd->msg_id = DM_MSG_CODE_SetParaValueCnf;
                /* pSockMsgSnd->caller_id  unchanged */
                pSockMsgSnd->word_data = lRet;
            
                lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
            
                if (lRet != DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_SetParaValueCnf failed in DmServerMsgHandler");
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_SetParaValueCnf successfully in DmServerMsgHandler");
                }
            
                DM_FREE(pSockMsgRcv);
                pSockMsgSnd = NULL;
            
                break;
            
            }
            case DM_MSG_CODE_GetParaNameReq:
            {
                /* Calling DmGetParaNameReq */
                UINT32 cnt = 0;
                UINT32 sndMsgBody = 0;
                INT8 *pTmp = NULL;
                DM_GET_PARA_NAME_REQ_T *pGetParaNameReq = (DM_GET_PARA_NAME_REQ_T *)(pSockMsgRcv+1);
            
                pGetParaNameReq->pucParamName = (INT8 *)(pGetParaNameReq+1);
                pGetParaNameReq->pNameList = NULL;
                pGetParaNameReq->pfnFree = NULL;
            
                lRet = DmGetParaNameReq(pGetParaNameReq);
            
                /* compute the snd buffer */
                if (lRet == DM_RET_SUCCESS)
                {
                    sndMsgBody += sizeof(PARAMETER_NAME_LIST_T);

                    if (pGetParaNameReq->pNameList->udwCnt>0)
                    {
                        sndMsgBody += sizeof(PARAMETER_NAME_T)*(pGetParaNameReq->pNameList->udwCnt);
                    }

                    for (cnt=0; cnt<pGetParaNameReq->pNameList->udwCnt; cnt++)
                    {
                        if (pGetParaNameReq->pNameList->pucParamName[cnt].pucName)
                        {
                            sndMsgBody += strlen(pGetParaNameReq->pNameList->pucParamName[cnt].pucName)+1;
                        }
                    }
                }
            
                pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+sndMsgBody);

                osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+sndMsgBody);
            
                pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
                pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
                pSockMsgSnd->msg_id = DM_MSG_CODE_GetParaNameCnf;
                pSockMsgSnd->word_data = lRet;
                pSockMsgSnd->caller_id = pSockMsgRcv->caller_id;
                pSockMsgSnd->msgLength = sndMsgBody;
            
                if (lRet == DM_RET_SUCCESS)
                {
                    pTmp = (INT8 *)(pSockMsgSnd + 1);
            
                    osa_mem_copy(pTmp, pGetParaNameReq->pNameList, sizeof(PARAMETER_NAME_LIST_T));
                    pTmp += sizeof(PARAMETER_NAME_LIST_T);

                    if (pGetParaNameReq->pNameList->udwCnt>0)
                    {
                        osa_mem_copy(pTmp, pGetParaNameReq->pNameList->pucParamName, sizeof(PARAMETER_NAME_T)*(pGetParaNameReq->pNameList->udwCnt));
                        pTmp += sizeof(PARAMETER_NAME_T)*(pGetParaNameReq->pNameList->udwCnt);
                    }

                    for (cnt=0; cnt<pGetParaNameReq->pNameList->udwCnt; cnt++)
                    {
                        if (pGetParaNameReq->pNameList->pucParamName[cnt].pucName)
                        {
                            strcpy(pTmp, pGetParaNameReq->pNameList->pucParamName[cnt].pucName);
                            pTmp += strlen(pGetParaNameReq->pNameList->pucParamName[cnt].pucName) + 1;
                        }
                    }
                }

                lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);

                if (lRet != DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_GetParaNameCnf failed in DmServerMsgHandler");
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_GetParaNameCnf successfully in DmServerMsgHandler");
                }

                if (pGetParaNameReq->pfnFree)
                {
                    pGetParaNameReq->pfnFree(pGetParaNameReq);
                }

                DM_FREE(pSockMsgRcv);
                DM_FREE(pSockMsgSnd);

                break;
            
            }
            case DM_MSG_CODE_AddObjectReq:
            {
                /* Calling DmAddObjectReq */
                DM_ADD_OBJECT_REQ_T *pAddObjectReq = (DM_ADD_OBJECT_REQ_T *)(pSockMsgRcv+1);
            
                pAddObjectReq->pucObjName = (INT8 *)(pAddObjectReq+1);
            
                lRet = DmAddObjectReq(pAddObjectReq);
            
                /* reuse the msg buffer */
                pSockMsgSnd = pSockMsgRcv;
            
                pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
                pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
                pSockMsgSnd->msg_id = DM_MSG_CODE_AddObjectCnf;
                /* pSockMsgSnd->caller_id  unchanged */
                pSockMsgSnd->word_data = lRet;
            
                lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
            
                if (lRet != DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_AddObjectCnf failed in DmServerMsgHandler");
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_AddObjectCnf successfully in DmServerMsgHandler");
                }
            
                DM_FREE(pSockMsgRcv);
                pSockMsgSnd = NULL;
            
                break;
            
            }
            case DM_MSG_CODE_DeleteObjectReq:
            {
                /* Calling DmDeleteObjectReq */
                DM_DELETE_OBJECT_REQ_T *pDeleteObjectReq = (DM_DELETE_OBJECT_REQ_T *)(pSockMsgRcv+1);
            
                pDeleteObjectReq->pucObjName = (INT8 *)(pDeleteObjectReq+1);
            
                lRet = DmDeleteObjectReq(pDeleteObjectReq);
            
                /* reuse the msg buffer */
                pSockMsgSnd = pSockMsgRcv;
            
                pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
                pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
                pSockMsgSnd->msg_id = DM_MSG_CODE_DeleteObjectCnf;
                /* pSockMsgSnd->caller_id  unchanged */
                pSockMsgSnd->word_data = lRet;
            
                lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
            
                if (lRet != DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_DeleteObjectCnf failed in DmServerMsgHandler");
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_DeleteObjectCnf successfully in DmServerMsgHandler");
                }
            
                DM_FREE(pSockMsgRcv);
                pSockMsgSnd = NULL;
            
                break;
            
            }
            case DM_MSG_CODE_SetParaAttributeReq:
            {
                /* Calling DmSetParaAttributeReq */
                UINT32 cnt = 0;
                UINT32 iCnt = 0;
                INT8 *pTmp = NULL;
                DM_SET_PARA_ATTR_REQ_T    *pSetParaAttributeReq = (DM_SET_PARA_ATTR_REQ_T *)(pSockMsgRcv+1);
            
                pSetParaAttributeReq->pParamAttrList = (DM_PARAM_ATTR_T*)(pSetParaAttributeReq+1);

                pTmp = (INT8*)(pSetParaAttributeReq->pParamAttrList+pSetParaAttributeReq->ulParamCnt);
            
                for (cnt=0; cnt<pSetParaAttributeReq->ulParamCnt; cnt++)
                {
                    pSetParaAttributeReq->pParamAttrList[cnt].pucParamName = pTmp;
                    pTmp += strlen(pSetParaAttributeReq->pParamAttrList[cnt].pucParamName)+1;
            
                    pSetParaAttributeReq->pParamAttrList[cnt].pucAttrList = (DM_ATTR_NODE_T *)pTmp;
                    pTmp += sizeof(DM_ATTR_NODE_T)*pSetParaAttributeReq->pParamAttrList[cnt].ulAttrCnt;
            
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ pSetParaAttributeReq %d %s has %d attributes in DmServerMsgHandler",cnt+1,pSetParaAttributeReq->pParamAttrList[cnt].pucParamName,pSetParaAttributeReq->pParamAttrList[cnt].ulAttrCnt);

                    for (iCnt=0; iCnt<pSetParaAttributeReq->pParamAttrList[cnt].ulAttrCnt; iCnt++)
                    {
                        pSetParaAttributeReq->pParamAttrList[cnt].pucAttrList[iCnt].pAttrName = pTmp;
                        pTmp += strlen(pSetParaAttributeReq->pParamAttrList[cnt].pucAttrList[iCnt].pAttrName)+1;

                        pSetParaAttributeReq->pParamAttrList[cnt].pucAttrList[iCnt].pAttrValue = pTmp;
                        pTmp += pSetParaAttributeReq->pParamAttrList[cnt].pucAttrList[iCnt].ulAttrValueSize;

                        DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ pSetParaAttributeReq %d %s set %d attribute %s, in DmServerMsgHandler",cnt+1,pSetParaAttributeReq->pParamAttrList[cnt].pucParamName,iCnt+1,pSetParaAttributeReq->pParamAttrList[cnt].pucAttrList[iCnt].pAttrName);
                    }
                }

                lRet = DmSetParaAttributeReq(pSetParaAttributeReq);

                /* reuse the message buffer */
                pSockMsgSnd = pSockMsgRcv;
            
                pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
                pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
                pSockMsgSnd->msg_id = DM_MSG_CODE_SetParaAttributeCnf;
                /* pSockMsgSnd->caller_id  unchanged */
                pSockMsgSnd->word_data = lRet;
            
                lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
            
                if (lRet != DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_SetParaAttributeCnf failed in DmServerMsgHandler");
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_SetParaAttributeCnf successfully in DmServerMsgHandler");
                }
            
                DM_FREE(pSockMsgRcv);
                pSockMsgSnd = NULL;
            
                break;
            
            }
            case DM_MSG_CODE_GetParaAttributeReq:
            {
                /* Calling DmGetParaAttributeReq */
                UINT32 cnt = 0;
                UINT32 iCnt = 0;
                UINT32 sndMsgBody = 0;
                INT8 *pTmp = NULL;
                DM_GET_PARA_ATTR_REQ_T    *pGetParaAttributeReq = (DM_GET_PARA_ATTR_REQ_T *)(pSockMsgRcv+1);
            
                pGetParaAttributeReq->pNameList = (PARAMETER_NAME_LIST_T *)(pGetParaAttributeReq+1);
                pGetParaAttributeReq->pNameList->pucParamName = (PARAMETER_NAME_T *)(pGetParaAttributeReq->pNameList+1);

                pTmp = (INT8*)(pGetParaAttributeReq->pNameList->pucParamName+pGetParaAttributeReq->pNameList->udwCnt);
            
                for (cnt=0; cnt<pGetParaAttributeReq->pNameList->udwCnt; cnt++)
                {
                    pGetParaAttributeReq->pNameList->pucParamName[cnt].pucName = pTmp;
                    pTmp += strlen(pGetParaAttributeReq->pNameList->pucParamName[cnt].pucName)+1;
                }

                lRet = DmGetParaAttributeReq(pGetParaAttributeReq);

                if (lRet == DM_RET_SUCCESS)
                {
                    sndMsgBody += sizeof(UINT32);
                    sndMsgBody += sizeof(DM_PARAM_ATTR_T)*pGetParaAttributeReq->ulParamCnt;

                    for (cnt=0; cnt<pGetParaAttributeReq->ulParamCnt; cnt++)
                    {
                        sndMsgBody += strlen(pGetParaAttributeReq->pParamAttrList[cnt].pucParamName)+1;
                        sndMsgBody += sizeof(DM_ATTR_NODE_T)*pGetParaAttributeReq->pParamAttrList[cnt].ulAttrCnt;

                        for (iCnt=0; iCnt<pGetParaAttributeReq->pParamAttrList[cnt].ulAttrCnt; iCnt++)
                        {
                            sndMsgBody += strlen(pGetParaAttributeReq->pParamAttrList[cnt].pucAttrList[iCnt].pAttrName)+1;
                            sndMsgBody += pGetParaAttributeReq->pParamAttrList[cnt].pucAttrList[iCnt].ulAttrValueSize;
                        }
                    }
                }

                pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+sndMsgBody);

                osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+sndMsgBody);

                pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
                pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
                pSockMsgSnd->msg_id = DM_MSG_CODE_GetParaAttributeCnf;
                pSockMsgSnd->word_data = lRet;
                pSockMsgSnd->caller_id = pSockMsgRcv->caller_id;
                pSockMsgSnd->msgLength = sndMsgBody;

                if (lRet == DM_RET_SUCCESS)
                {
                    pTmp = (INT8 *)(pSockMsgSnd + 1);

                    osa_mem_copy(pTmp,&pGetParaAttributeReq->ulParamCnt,sizeof(UINT32));
                    pTmp += sizeof(UINT32);

                    osa_mem_copy(pTmp,pGetParaAttributeReq->pParamAttrList,sizeof(DM_PARAM_ATTR_T)*pGetParaAttributeReq->ulParamCnt);
                    pTmp += sizeof(DM_PARAM_ATTR_T)*pGetParaAttributeReq->ulParamCnt;

                    for (cnt=0; cnt<pGetParaAttributeReq->ulParamCnt; cnt++)
                    {
                        strcpy(pTmp,pGetParaAttributeReq->pParamAttrList[cnt].pucParamName);
                        pTmp += strlen(pGetParaAttributeReq->pParamAttrList[cnt].pucParamName)+1;

                        if (pGetParaAttributeReq->pParamAttrList[cnt].ulAttrCnt>0)
                        {
                            osa_mem_copy(pTmp,pGetParaAttributeReq->pParamAttrList[cnt].pucAttrList,sizeof(DM_ATTR_NODE_T)*pGetParaAttributeReq->pParamAttrList[cnt].ulAttrCnt);
                            pTmp += sizeof(DM_ATTR_NODE_T)*pGetParaAttributeReq->pParamAttrList[cnt].ulAttrCnt;
                        }

                        for (iCnt=0; iCnt<pGetParaAttributeReq->pParamAttrList[cnt].ulAttrCnt; iCnt++)
                        {
                            strcpy(pTmp,pGetParaAttributeReq->pParamAttrList[cnt].pucAttrList[iCnt].pAttrName);
                            pTmp += strlen(pGetParaAttributeReq->pParamAttrList[cnt].pucAttrList[iCnt].pAttrName)+1;

                            osa_mem_copy(pTmp,pGetParaAttributeReq->pParamAttrList[cnt].pucAttrList[iCnt].pAttrValue,pGetParaAttributeReq->pParamAttrList[cnt].pucAttrList[iCnt].ulAttrValueSize);
                            pTmp += pGetParaAttributeReq->pParamAttrList[cnt].pucAttrList[iCnt].ulAttrValueSize;
                        }
                    }
                }

                lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);

                if (lRet != DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_GetParaAttributeCnf failed in DmServerMsgHandler");
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_GetParaAttributeCnf successfully in DmServerMsgHandler");
                }

                if (pGetParaAttributeReq->pfnFree)
                {
                    pGetParaAttributeReq->pfnFree(pGetParaAttributeReq);
                }

                DM_FREE(pSockMsgRcv);
                DM_FREE(pSockMsgSnd);
                break;
            
            }
            default:
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid msg code 0x%x in DmServerMsgHandler(%s)",msgCode,g_DmCtrl.entity);
                break;
        }
    }

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmServerMsgHandler(%s)",g_DmCtrl.entity);

    return OSA_FALSE;
}

void DmClientMsgHandler(DM_TRANS_CTRL_T *pTransCtrl)
{
    INT32 lRet = DM_FAILURE;
    UINT32 msgArray[2] = {0};
    UINT32 msgCode = 0;
    DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;
    DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmClientMsgHandler(%s)",g_DmCtrl.entity);

    for (;;)
    {
        lRet = osa_msg_queue_receive(&pTransCtrl->msgQ,(void*)msgArray);

        if (lRet != OSA_SUCCESS)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ recv msg failed in DmClientMsgHandler(%s)",g_DmCtrl.entity);
            continue;
        }

        msgCode = msgArray[0];
        pSockMsgRcv = (DM_SOCK_MSG_HEADER_T *)msgArray[1];

        switch(msgCode)
        {
            case DM_MSG_CODE_LeaveInd:
            {
                lRet = osa_msg_queue_delete(&pTransCtrl->msgQ);
            
                if (lRet != DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_msg_queue_delete failed in DmClientMsgHandler(%s)",msgCode,g_DmCtrl.entity);
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_msg_queue_delete successfully in DmClientMsgHandler(%s)",g_DmCtrl.entity);
                }
            
                DM_FREE(pTransCtrl);
            
                return;
            }
            case DM_MSG_CODE_NotifyHandlerReq:
            {
                /* Calling DmOneNotifyHandlerCalling */
                UINT32 cnt = 0;
            
                INT8 *pTmp = NULL;
                DM_NOTIFY_T *pNotifyPara = (DM_NOTIFY_T *)(pSockMsgRcv+1);

                DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ recv DM_MSG_CODE_NotifyHandlerReq, %d params changed in DmClientMsgHandler(%s)",pNotifyPara->ulParaCount,g_DmCtrl.entity);

                pNotifyPara->pCont = (DM_NOTIFY_PARA_T*)(pNotifyPara+1);
            
                pTmp = (INT8*)(pNotifyPara->pCont + pNotifyPara->ulParaCount);
                for (cnt=0; cnt<pNotifyPara->ulParaCount; cnt++)
                {
                    pNotifyPara->pCont[cnt].pucPath = pTmp;
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ %s changed, need notification in DmClientMsgHandler(%s)",pNotifyPara->pCont[cnt].pucPath,g_DmCtrl.entity);
                    pTmp += strlen(pTmp) +1;

                    if (pNotifyPara->pCont[cnt].pucChangedAttrs)
                    {
                        pNotifyPara->pCont[cnt].pucChangedAttrs = pTmp;
                        pTmp += strlen(pTmp) +1;
                    }
                }

                DmOneNotifyHandlerCalling(g_DmCtrl.entity,pNotifyPara);
            
                /* no repsonse required */
            
                DM_FREE(pSockMsgRcv);
            
                break;
            
            }
            case DM_MSG_CODE_GetHandlerReq:
            {
                /* Calling DmGetHandlerCalling */
            
                DM_NODE_GET_HANDLER_T GetHandler;
                DM_GET_VALUE_T *pPara = (DM_GET_VALUE_T *)(pSockMsgRcv+1);
            
                osa_mem_set(&GetHandler, 0, sizeof(DM_NODE_GET_HANDLER_T));
                osa_mem_copy(GetHandler.entity, g_DmCtrl.entity, sizeof(GetHandler.entity));
            
                pPara->pucPath = (INT8*)(pPara+1);
                pPara->pValue = pPara->pucPath + strlen(pPara->pucPath)+1;

                DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ recv DM_MSG_CODE_GetHandlerReq, %s param in DmClientMsgHandler(%s)",pPara->pucPath,g_DmCtrl.entity);

                lRet = DmGetHandlerCalling(&GetHandler,pPara);
            
                /* reuse the msg buffer */
                pSockMsgSnd = pSockMsgRcv;
            
                pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
                pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
                pSockMsgSnd->msg_id = DM_MSG_CODE_GetHandlerCnf;
                /* pSockMsgSnd->caller_id  unchanged */
                pSockMsgSnd->word_data = lRet;
            
                lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
            
                if (lRet != DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_GetHandlerCnf failed in DmClientMsgHandler(%s)",g_DmCtrl.entity);
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_GetHandlerCnf successfully in DmClientMsgHandler(%s)",g_DmCtrl.entity);
                }
            
                DM_FREE(pSockMsgRcv);
                pSockMsgSnd = NULL;
            
            
                break;
            
            }
            case DM_MSG_CODE_SetHandlerReq:
            {
                /* Calling DmSetHandlerCalling */
            
                DM_NODE_SET_HANDLER_T SetHandler;
                DM_SET_VALUE_T *pPara = (DM_SET_VALUE_T *)(pSockMsgRcv+1);
            
                osa_mem_set(&SetHandler, 0, sizeof(DM_NODE_SET_HANDLER_T));
                osa_mem_copy(SetHandler.entity, g_DmCtrl.entity, sizeof(SetHandler.entity));
            
                pPara->pucPath = (INT8*)(pPara+1);
                pPara->pValue = pPara->pucPath + strlen(pPara->pucPath)+1;

                DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ recv DM_MSG_CODE_SetHandlerReq, %s param in DmClientMsgHandler(%s)",pPara->pucPath,g_DmCtrl.entity);

                lRet = DmSetHandlerCalling(&SetHandler,pPara);
            
                /* reuse the msg buffer */
                pSockMsgSnd = pSockMsgRcv;
            
                pSockMsgSnd->receiver_id = pSockMsgRcv->sender_id;
                pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
                pSockMsgSnd->msg_id = DM_MSG_CODE_SetHandlerCnf;
                /* pSockMsgSnd->caller_id  unchanged */
                pSockMsgSnd->word_data = lRet;
            
                lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
            
                if (lRet != DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send DM_MSG_CODE_SetHandlerCnf failed in DmClientMsgHandler(%s)",g_DmCtrl.entity);
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_SetHandlerCnf successfully in DmClientMsgHandler(%s)",g_DmCtrl.entity);
                }
            
                DM_FREE(pSockMsgRcv);
                pSockMsgSnd = NULL;
            
            
                break;
            
            }

            default:
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid msg code 0x%x in DmClientMsgHandler(%s)",msgCode,g_DmCtrl.entity);
                break;
        }
    }

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmClientMsgHandler(%s)",g_DmCtrl.entity);

    return;
}

void DmSockMsgServerRecv(DM_SOCK_MSG_T *pSockMsgHandler, void *pMsg)
{
    INT32 lRet = DM_FAILURE;
    DM_SOCK_MSG_HEADER_T *pSockMsgRcv = pMsg;
    DM_TRANS_CTRL_T *pTransCtrl = NULL;
    UINT32 msg_id = 0;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmSockMsgServerRecv");

    if (pSockMsgRcv == NULL || pMsg == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid arguments in DmSockMsgServerRecv");
        return;
    }

    if (pSockMsgRcv->receiver_id != DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME))
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ wrong recv msg-entity-id(0x%x) != 0x%x in DmSockMsgServerRecv, msg_id(0x%X)",pSockMsgRcv->receiver_id,DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME),pSockMsgRcv->msg_id);
        return;
    }

    msg_id = pSockMsgRcv->msg_id;
    switch(msg_id)
    {
        case DM_MSG_CODE_TransactionStartReq:
        {
            INT8 AccessEntity[DM_ACCESS_ENTITY_NAME_MAX];
            INT8 RootNodeToLock[DM_RW_MUTEX_LOCK_PATH_LEN];
            OSA_TP_TASK_T task;
            
            /* start a thread & msgq to handle this transaction and its later msgs */
            UINT32 msgArray[2] = {0};
            DM_TRANSACTION_REQ_T *pTransaction = (DM_TRANSACTION_REQ_T *)(pSockMsgRcv+1);
            pTransCtrl = (DM_TRANS_CTRL_T *)DM_MALLOC(sizeof(DM_TRANS_CTRL_T));
            
            if (pTransCtrl == NULL)
            {
                DM_ERROR(DM_ERR_MEMORY_ALLOCATION_FAILED, "DM Trace ------ pTransCtrl malloc failed for TransactionStartReq, req-entity=%s, lock-path=%s, leave DmSockMsgServerRecv",pTransaction->AccessEntity,pTransaction->RootNodeToLock);   
                return;
            }
            
            osa_mem_set(pTransCtrl,0,sizeof(DM_TRANS_CTRL_T));
            osa_mem_set(&task,0,sizeof(OSA_TP_TASK_T));
            
            lRet = osa_msg_queue_create(&pTransCtrl->msgQ, DM_MSG_SIZE,DM_MSGQ_LENGTH);
            
            if (lRet != DM_SUCCESS)
            {
                DM_FREE(pTransCtrl);
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_msg_queue_create failed for TransactionStartReq, req-entity=%s, lock-path=%s, leave DmSockMsgServerRecv",pTransaction->AccessEntity,pTransaction->RootNodeToLock); 
                return;
            }

            task.task_id = 0;
            task.task_cb = (OSA_TP_TASK_CB_FUN)DmServerMsgHandler;
            task.data = (void*)pTransCtrl;
            task.prio = OSA_TP_TASK_PRIO__MED;
            task.sched_type = OSA_TP_TASK_SCHED__DIRECT;
            task.sched_interval = 0;

            lRet = osa_thread_pool_queue_task(&g_DmCtrl.threadPool, &task);

            if (lRet != OSA_SUCCESS)
            {
                DM_FREE(pTransCtrl);
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_thread_pool_queue_task for cl-msg-handler failed, req-entity=%s, lock-path=%s, leave DmSockMsgServerRecv",pTransaction->AccessEntity,pTransaction->RootNodeToLock); 
                return;
            }

            /* send trans-start-req to msgq */
            msgArray[0] = msg_id;
            msgArray[1] = (UINT32)pSockMsgRcv;
            strcpy(AccessEntity, pTransaction->AccessEntity);
            strcpy(RootNodeToLock, pTransaction->RootNodeToLock);

            lRet = osa_msg_queue_send(&pTransCtrl->msgQ,(void*)msgArray);

            if (lRet != OSA_SUCCESS)
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_msg_queue_send failed for DM_MSG_CODE_TransactionStartReq, req-entity=%s, lock-path=%s in DmSockMsgServerRecv", AccessEntity, RootNodeToLock);
                DM_FREE(pSockMsgRcv);
            }
            else
            {
                DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ transfer DM_MSG_CODE_TransactionStartReq to msgQ successfully, req-entity=%s, lock-path=%s in DmSockMsgServerRecv", AccessEntity, RootNodeToLock);
            }

            break;
        }
        case DM_MSG_CODE_TransactionStopReq:
		case DM_MSG_CODE_ExtDbNewParaReq:
        case DM_MSG_CODE_GetParaCValueReq:
        case DM_MSG_CODE_SetParaCValueReq:
        case DM_MSG_CODE_AddObjectByIdReq:
        case DM_MSG_CODE_DelObjectByIdReq:
        case DM_MSG_CODE_GetParaValueReq:
        case DM_MSG_CODE_SetParaValueReq:
        case DM_MSG_CODE_GetParaNameReq:
        case DM_MSG_CODE_AddObjectReq:
        case DM_MSG_CODE_DeleteObjectReq:
        case DM_MSG_CODE_SetParaAttributeReq:
        case DM_MSG_CODE_GetParaAttributeReq:
        {
            UINT32 transId = 0;
            INT8 *pTransId = NULL;
            UINT32 msgArray[2] = {0};

            transId = (UINT32)pSockMsgRcv->word_data;
            pTransId = DmUintToStr(transId);

            pTransCtrl = DmHashGetValue(&g_DmCtrl.transHash,pTransId);
			DM_FREE(pTransId);

            if (pTransCtrl == NULL)
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to match trans-id(%u) for msg_id(0x%X) to msgQ in DmSockMsgServerRecv",transId,msg_id);
				DmSendCnfMsgIfNoTrans(pSockMsgRcv->sender_id, pSockMsgRcv->msg_id, pSockMsgRcv);
                DM_FREE(pSockMsgRcv);
                break;
            }

            msgArray[0] = msg_id;
            msgArray[1] = (UINT32)pSockMsgRcv;

            lRet = osa_msg_queue_send(&pTransCtrl->msgQ,(void*)msgArray);

            if (lRet != OSA_SUCCESS)
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_msg_queue_send failed for msg_id(0x%X) in DmSockMsgServerRecv",msg_id);
                DM_FREE(pSockMsgRcv);
            }
            else
            {
                DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ transfer msg_id(0x%X) to msgQ in DmSockMsgServerRecv",msg_id);
            }

            break;
        }
		case DM_MSG_CODE_LoadDbHandlerReq:
		case DM_MSG_CODE_UnloadDbHandlerReq:
        case DM_MSG_CODE_FlushDBfilesReq:
        case DM_MSG_CODE_ExtDbOpenReq:
        case DM_MSG_CODE_ExtDbCloseReq:
        case DM_MSG_CODE_RegisterNotifyHandlerReq:
        case DM_MSG_CODE_DeregisterNotifyHandlerReq:
        case DM_MSG_CODE_RegisterGetHandlerReq:
        case DM_MSG_CODE_DeregisterGetHandlerReq:
        case DM_MSG_CODE_AllocateMemoryReq:
        case DM_MSG_CODE_ReleaseMemoryReq:
        case DM_MSG_CODE_RegisterSetHandlerReq:
        case DM_MSG_CODE_DeregisterSetHandlerReq:
		case DM_MSG_CODE_AddOnControlReq :
		case DM_MSG_CODE_AddOnReqMsgReq :
        {
			UINT32 msgArray[2] = {0};
			
			pTransCtrl = &g_DmCtrl.nonTransCtrl;

            msgArray[0] = msg_id;
            msgArray[1] = (UINT32)pSockMsgRcv;

            lRet = osa_msg_queue_send(&pTransCtrl->msgQ, (void*)msgArray);
            if (lRet != OSA_SUCCESS)
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_msg_queue_send failed for msg_id(0x%X) in DmSockMsgServerRecv", msg_id);
                DM_FREE(pSockMsgRcv);
            }
            else
            {
                DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ transfer msg_id(0x%X) to to DB Admin thread in DmSockMsgServerRecv", msg_id);
            }

            break;
        }
        case DM_MSG_CODE_GetHandlerCnf:
        case DM_MSG_CODE_SetHandlerCnf:
        {
            lRet = osa_resource_release(&g_DmCtrl.rsrcGrp,pSockMsgRcv);

            if (lRet != DM_SUCCESS)
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_release failed for msg_id(0x%X) in DmSockMsgServerRecv",msg_id);
            }
            else
            {
                DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ recv msg_id(0x%X) successfully in DmSockMsgServerRecv",msg_id);
            }

            /* do not free pSockMsgRcv, the thread calling osa_resource_wait need it. */
            break;

        }
        default:
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid msg_id(0x%X) in DmSockMsgServerRecv",msg_id);
            DM_FREE(pSockMsgRcv);
            break;
    }

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmSockMsgServerRecv");

    return;
}

void DmSockMsgClientRecv(DM_SOCK_MSG_T *pSockMsgHandler, void *pMsg)
{
    INT32 lRet = DM_FAILURE;
    DM_SOCK_MSG_HEADER_T *pSockMsgRcv = pMsg;
    UINT32 msg_id = 0;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmSockMsgClientRecv");

    if (pSockMsgRcv == NULL || pMsg == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid arguments in DmSockMsgClientRecv");
        return;
    }

    if (pSockMsgRcv->receiver_id != DmConvertEntityToMsgId(g_DmCtrl.entity))
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ wrong recv msg-entity-id(0x%x) != 0x%x, in DmSockMsgClientRecv, msg-id=0x%x",pSockMsgRcv->receiver_id,DmConvertEntityToMsgId(g_DmCtrl.entity),pSockMsgRcv->msg_id);
        return;
    }

	msg_id = pSockMsgRcv->msg_id;
    switch(msg_id)
    {
        case DM_MSG_CODE_TransactionStartCnf:
        case DM_MSG_CODE_TransactionStopCnf:
        case DM_MSG_CODE_GetParaCValueCnf:
        case DM_MSG_CODE_SetParaCValueCnf:
        case DM_MSG_CODE_AddObjectByIdCnf:
        case DM_MSG_CODE_DelObjectByIdCnf:
        case DM_MSG_CODE_GetParaValueCnf:
        case DM_MSG_CODE_SetParaValueCnf:
        case DM_MSG_CODE_GetParaNameCnf:
        case DM_MSG_CODE_AddObjectCnf:
        case DM_MSG_CODE_DeleteObjectCnf:
        case DM_MSG_CODE_RegisterNotifyHandlerCnf:
        case DM_MSG_CODE_DeregisterNotifyHandlerCnf:
        case DM_MSG_CODE_RegisterGetHandlerCnf:
        case DM_MSG_CODE_RegisterSetHandlerCnf:
        case DM_MSG_CODE_ExtDbOpenCnf:
        case DM_MSG_CODE_ExtDbCloseCnf:
        case DM_MSG_CODE_FlushDBfilesCnf:
        case DM_MSG_CODE_LoadDbHandlerCnf:
		case DM_MSG_CODE_UnloadDbHandlerCnf:
        case DM_MSG_CODE_AllocateMemoryCnf:
        case DM_MSG_CODE_ReleaseMemoryCnf:
		case DM_MSG_CODE_AddOnControlCnf :
		case DM_MSG_CODE_AddOnReqMsgCnf :
        case DM_MSG_CODE_ExtDbNewParaCnf:
        case DM_MSG_CODE_SetParaAttributeCnf:
        case DM_MSG_CODE_GetParaAttributeCnf:
        {
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ Recv msg_id(0x%X) in DmSockMsgClientRecv",msg_id);

            lRet = osa_resource_release(&g_DmCtrl.rsrcGrp,pSockMsgRcv);

            if (lRet != DM_SUCCESS)
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_release failed for msg_id(0x%X) in DmSockMsgClientRecv",msg_id);
            }

            /* do not free pSockMsgRcv, the thread calling osa_resource_wait do it. */
            break;
        }
        case DM_MSG_CODE_NotifyHandlerReq:
        case DM_MSG_CODE_GetHandlerReq:
        case DM_MSG_CODE_SetHandlerReq:
        {
            DM_TRANS_CTRL_T *pTransCtrl = DmHashGetValue(&g_DmCtrl.transHash,g_DmCtrl.entity);
            UINT32 msgArray[2] = {0};

            msgArray[0] = msg_id;
            msgArray[1] = (UINT32)pSockMsgRcv;

            lRet = osa_msg_queue_send(&pTransCtrl->msgQ,(void*)msgArray);

            if (lRet != OSA_SUCCESS)
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_msg_queue_send failed for msg_id(0x%X) in DmSockMsgClientRecv",msg_id);
                DM_FREE(pSockMsgRcv);
            }

            break;
        }
        default:
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid msg_id(0x%X) in DmSockMsgClientRecv",msg_id);
            break;
    }

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmSockMsgClientRecv");

    return;
}

INT32 DmSockMsgRsrcGrpHandler(void *pResourceData, OSA_RESOURCE_GRP_STATUS_E status, void *pExtraData)
{
    INT32 lRet = DM_FAILURE;
    DM_CTRL_T *pDmCtrl = (DM_CTRL_T *)pResourceData;

    switch(status)
    {
        case OSA_RESOURCE_WAIT:
        {
            DM_SOCK_MSG_NODE_T *pMsgRcvNode = pDmCtrl->pMsgRcvList;
            DM_SOCK_MSG_NODE_T *pMsgRcvPerNode = NULL;
            DM_SOCK_MSG_HEADER_T **pSockMsgRecv = (DM_SOCK_MSG_HEADER_T **)pExtraData;

            while (pMsgRcvNode != NULL)
            {
                if (pMsgRcvNode->pMsgRcv
                    && pMsgRcvNode->pMsgRcv->caller_id == osa_thread_gettid()
                    && pMsgRcvNode->pMsgRcv->msg_id == (UINT32)(*pSockMsgRecv))
                {
                    *pSockMsgRecv = pMsgRcvNode->pMsgRcv;

                    lRet = DM_SUCCESS;

                    if (pMsgRcvPerNode==NULL)
                    {
                        pDmCtrl->pMsgRcvList = pMsgRcvNode->pNext;
                    }
                    else
                    {
                        pMsgRcvPerNode->pNext = pMsgRcvNode->pNext;
                    }

                    DM_FREE(pMsgRcvNode);
                    break;
                }

                pMsgRcvPerNode = pMsgRcvNode;
                pMsgRcvNode = pMsgRcvNode->pNext;
            }
            break;
        }
        case OSA_RESOURCE_RELEASE:
        {
            DM_SOCK_MSG_NODE_T *pMsgRcvNode = (DM_SOCK_MSG_NODE_T*)DM_MALLOC(sizeof(DM_SOCK_MSG_NODE_T));

            if (pMsgRcvNode)
            {
                osa_mem_set(pMsgRcvNode, 0, sizeof(DM_SOCK_MSG_NODE_T));
                pMsgRcvNode->pMsgRcv = (DM_SOCK_MSG_HEADER_T *)pExtraData;
                pMsgRcvNode->pNext = pDmCtrl->pMsgRcvList;
                pDmCtrl->pMsgRcvList = pMsgRcvNode;
                lRet = DM_SUCCESS;
            }
            else
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to allocate memory for sock msg received");
            }
            break;
        }
        case OSA_RESOURCE_GRP_DELETED:
        {
            /* nothing to do */
            lRet = DM_SUCCESS;
            break;
        }
        default:
            break;
    }

    return lRet;
}

INT32  DmServiceStartReq (DM_DBFILE_ENCRYPT_FUNC encryptFunc) 
{
	INT32 lRet = DM_FAILURE;
	DM_START_REQ_T startReq;

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmServiceStartReq(%s)", DM_SERVICE_ENTITY_NAME);

	osa_mem_set(&startReq, 0, sizeof(DM_START_REQ_T));
	strncpy(startReq.entity, DM_SERVICE_ENTITY_NAME, sizeof(startReq.entity) - 1 );

    if (access("/tmp/dmsrv/",F_OK)!=0)
    {
        mkdir("/tmp/dmsrv/",0755);
    }

	lRet = DmComStartReq(DM_TRUE, &startReq);
	if(lRet == DM_SUCCESS)
	{
		g_DmCtrl.encryptFunc = encryptFunc;
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ encryptFunc = (0x%x)", encryptFunc);
	}

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmServiceStartReq(%s)", DM_SERVICE_ENTITY_NAME);

	return lRet;
}

INT32  DmStartReq (DM_START_REQ_T *pStartReq) 
{
	INT32 lRet = DM_FAILURE;

    if (pStartReq == NULL)
    {
        DM_ERROR(DM_ERR_INVALID_ARGUMENTS, "DM Trace ------ invalid argument, leave DmStartReq");
        return DM_FAILURE;
    }
	
	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmStartReq(%s)", pStartReq->entity);
	
	lRet = DmComStartReq(DM_FALSE, pStartReq);
	
	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmStartReq(%s), ret=%d", pStartReq->entity, lRet);
	
	return lRet;
}

INT32  DmComStartReq (BOOL8 bAsServer, DM_START_REQ_T *pStartReq) 
{
    INT32 lRet = DM_FAILURE;
    INT8 threadName[32] = {0};
    DM_TRANS_CTRL_T *pTransCtrl = NULL;
	INT32 iDmStartNum = 0;

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmComStartReq(%s)",pStartReq->entity);

	osa_thread_mutex_lock(&g_StartInstanceMutex);
	g_StartInstanceCount ++;
	iDmStartNum = g_StartInstanceCount;
	osa_thread_mutex_unlock(&g_StartInstanceMutex);
	
    if (iDmStartNum > 1)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Dm Started already(num=%d), leave DmComStartReq(%s)", iDmStartNum, pStartReq->entity);
        return DM_SUCCESS;
    }

    osa_mem_set(&g_DmCtrl, 0, sizeof(DM_CTRL_T));
    g_DmCtrl.bAsServer = bAsServer;
    g_DmCtrl.transIdMax = 0;

    strncpy(g_DmCtrl.entity, pStartReq->entity, sizeof(g_DmCtrl.entity));

    lRet = osa_thread_mutex_create(&g_DmCtrl.transIdMutex);

    if (lRet != DM_SUCCESS)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ transIdMutex create entity(%s) asServer(%d) failed, leave DmComStartReq",pStartReq->entity,g_DmCtrl.bAsServer); 
        return DM_FAILURE;
    }

    lRet = DmHashCreate(&g_DmCtrl.transHash,DM_HASH_INIT_SIZE);

    if (lRet != DM_SUCCESS)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ trans-DmHashCreate entity(%s) asServer(%d) failed, leave DmComStartReq",pStartReq->entity,g_DmCtrl.bAsServer); 
        return DM_FAILURE;
    }

    lRet = DmHashCreate(&g_DmCtrl.notifyFuncHash,DM_HASH_INIT_SIZE);

    if (lRet != DM_SUCCESS)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Notify-DmHashCreate entity(%s) asServer(%d) failed, leave DmComStartReq",pStartReq->entity,g_DmCtrl.bAsServer); 
        return DM_FAILURE;
    }

    lRet = DmHashCreate(&g_DmCtrl.getFuncHash,DM_HASH_INIT_SIZE);

    if (lRet != DM_SUCCESS)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Get-DmHashCreate entity(%s) asServer(%d) failed, leave DmComStartReq",pStartReq->entity,g_DmCtrl.bAsServer); 
        return DM_FAILURE;
    }

    lRet = DmHashCreate(&g_DmCtrl.setFuncHash,DM_HASH_INIT_SIZE);

    if (lRet != DM_SUCCESS)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Set-DmHashCreate entity(%s) asServer(%d) failed, leave DmComStartReq",pStartReq->entity,g_DmCtrl.bAsServer); 
        return DM_FAILURE;
    }

    lRet = osa_resource_group_create(&g_DmCtrl.rsrcGrp,&g_DmCtrl,DmSockMsgRsrcGrpHandler);
    
    if (lRet != DM_SUCCESS)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_group_create entity(%s) asServer(%d) failed, leave DmComStartReq",g_DmCtrl.entity,g_DmCtrl.bAsServer); 
        return DM_FAILURE;
    }

    if (g_DmCtrl.bAsServer == DM_TRUE)
    {
        /* initialize thread pool */
        lRet = osa_thread_pool_create(&g_DmCtrl.threadPool,
               "trans",
               DM_FALSE,
               DM_THREAD_POOL_MAX_NUMBER,
               OSA_THREAD_SCHED_OTHER,
               E_THREAD_PRIO_0);

        if (lRet != DM_SUCCESS)
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_thread_pool_create entity(%s) asServer(%d) failed, leave DmComStartReq",g_DmCtrl.entity,g_DmCtrl.bAsServer); 
			return DM_FAILURE;
		}

    	/* create shared memory control table */
        g_DmCtrl.maxShmSize = DM_SHM_DEFAULT_MAX_SIZE;

		lRet = osa_resource_group_create(&g_DmCtrl.shmGroup,&g_DmCtrl,DmShmGroupHandler);
		if (lRet != DM_SUCCESS)
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_group_create entity(%s) asServer(%d) failed, leave DmComStartReq",g_DmCtrl.entity,g_DmCtrl.bAsServer); 
			return DM_FAILURE;
		}

		lRet = DmHashCreate(&g_DmCtrl.shmInUseHash, DM_HASH_INIT_SIZE);
		if (lRet != DM_SUCCESS)
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmHashCreate of shared memory entity(%s) asServer(%d) failed, leave DmComStartReq",pStartReq->entity,g_DmCtrl.bAsServer); 
			return DM_FAILURE;
		}

		/* Register service */
		lRet = DmSelfRegisterService();
    	if (lRet != DM_SUCCESS)
    	{
        	DM_ERROR(DM_ERR_PROCESSING_COMMON_ERROR, "DM Trace ------ DmSelfRegisterService entity(%s) failed, leave DmComStartReq",g_DmCtrl.entity);
        	return lRet;
    	}

        /* running as server */
        osa_mem_set(&g_dmHandlerHash, 0, sizeof(DM_HASH_T));
        DmHashCreate(&g_dmHandlerHash,DM_HASH_INIT_SIZE);

        //xmlKeepBlanksDefault(0);

        /* here check libxml version we used... */
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Check compiled libxml2 version (%s)",LIBXML_DOTTED_VERSION);
        LIBXML_TEST_VERSION

		// create DB admin trhead
		lRet = DmStartNonTransHandler();
        if (lRet != DM_SUCCESS)
        {
            DM_ERROR(DM_ERR_PROCESSING_COMMON_ERROR, "DM Trace ------ DmStartNonTransHandler entity(%s) asServer(%d) failed, leave DmComStartReq",g_DmCtrl.entity,g_DmCtrl.bAsServer);	
            return DM_FAILURE;
        }

        lRet = DmSockMsgServiceStartReq(&g_DmCtrl.sockMsgQ,"dm",DmSockMsgServerRecv,g_DmCtrl.bAsServer);
        if (lRet != DM_SUCCESS)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSockMsgServiceStartReq entity(%s) asServer(%d) failed, leave DmComStartReq",g_DmCtrl.entity,g_DmCtrl.bAsServer);	
            return DM_FAILURE;
        }

		osa_thread_mutex_create(&g_DmCtrl.addOnMutex);
		g_DmCtrl.pAddOnList = NULL;
    }
    else
    {
		/* Ensure DM service is starting */
		lRet = DmEnsureStartService();
    	if (lRet != DM_SUCCESS)
    	{
        	DM_ERROR(DM_ERR_PROCESSING_COMMON_ERROR, "DM Trace ------ DmEnsureStartService entity(%s) failed, leave DmComStartReq",g_DmCtrl.entity);
        	return lRet;
    	}

        pTransCtrl = (DM_TRANS_CTRL_T *)DM_MALLOC(sizeof(DM_TRANS_CTRL_T));

        if (pTransCtrl == NULL)
        {
            DM_ERROR(DM_ERR_MEMORY_ALLOCATION_FAILED, "DM Trace ------ pTransCtrl malloc failed, entity(%s) asServer(%d), leave DmComStartReq",g_DmCtrl.entity,g_DmCtrl.bAsServer);	
            return DM_FAILURE;
        }

        osa_mem_set(pTransCtrl,0,sizeof(DM_TRANS_CTRL_T));

        lRet = osa_msg_queue_create(&pTransCtrl->msgQ, DM_MSG_SIZE,DM_MSGQ_LENGTH);
        
        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pTransCtrl);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_msg_queue_create entity(%s) asServer(%d) failed, leave DmComStartReq",pStartReq->entity,g_DmCtrl.bAsServer); 
            return DM_FAILURE;
        }

        snprintf(threadName, sizeof(threadName),"dm%s",g_DmCtrl.entity);

        lRet = osa_thread_create(&pTransCtrl->threadId,
                                threadName,
                                (OSA_THREAD_PFN)DmClientMsgHandler,
                                (UINT32)pTransCtrl,
                                DM_THRAD_STACK_SIZE,
                                OSA_THREAD_SCHED_OTHER,
                                E_THREAD_PRIO_0,
                                OSA_THREAD_PROP_NOWAIT);

        if (lRet != OSA_SUCCESS)
        {
            DM_FREE(pTransCtrl);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_thread_create for cl-msg-handler entity(%s) asServer(%d) failed, leave DmComStartReq",g_DmCtrl.entity,g_DmCtrl.bAsServer); 
            return DM_FAILURE;
        }

        lRet = DmHashAdd(&g_DmCtrl.transHash,g_DmCtrl.entity,pTransCtrl);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pTransCtrl);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmHashAdd for cl-msg&thread-handler entity(%s) asServer(%d) failed, leave DmComStartReq",g_DmCtrl.entity,g_DmCtrl.bAsServer); 
            return DM_FAILURE;
        }

		/* check if socket has create or not */
		lRet = DmWaitSockStarted("dm");
        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pTransCtrl);
            DM_ERROR(DM_ERR_PROCESSING_COMMON_ERROR, "DM Trace ------ DmWaitSockStarted entity(%s) failed, leave DmComStartReq",g_DmCtrl.entity);
            return lRet;
        }

        /* running as client */
        lRet = DmSockMsgServiceStartReq(&g_DmCtrl.sockMsgQ,"dm",DmSockMsgClientRecv,g_DmCtrl.bAsServer);
        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pTransCtrl);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSockMsgServiceStartReq entity(%s) asServer(%d) failed, leave DmComStartReq",g_DmCtrl.entity,g_DmCtrl.bAsServer);	
            return DM_FAILURE;
        }
    }

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmComStartReq(%s) successfully",g_DmCtrl.entity);	

	return DM_SUCCESS;
}

INT32 DmUpdateDataModelReq(INT8 *pLoadingDir, DM_UPDATE_MODE_t UpdateMode)
{
    switch(UpdateMode)
    {
        case DM_UPDATE_MODE_ALL:
            break;
        case DM_UPDATE_MODE_UPGRADED:
            break;
        case DM_UPDATE_MODE_CONFIGFILE:
            break;
        case DM_UPDATE_MODE_DEFAULTVALUE:
            break;
        case DM_UPDATE_MODE_RESET:
            break;
        case DM_UPDATE_MODE_RAW:
            break;
        case DM_UPDATE_MODE_NULL:
        default:
            break;
    }

    return DM_SUCCESS;
}

INT32 DmExportDataModelReq(INT8 *pDstFile, DM_EXPORT_MODE_t exportMode)
{
    switch(exportMode)
    {
        case DM_EXPORT_MODE_CONFIGFILE:
            break;
        case DM_EXPORT_MODE_RAW:
            break;
        case DM_EXPORT_MODE_NULL:
        default:
            break;
    }

    return DM_SUCCESS;
}

INT32  DmServiceStopReq() 
{
	INT32 lRet = DM_FAILURE;
	
	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmServiceStopReq(%s)", g_DmCtrl.entity);

	lRet = DmStopReq();
	
	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmServiceStopReq(%s)", g_DmCtrl.entity);
	
	return lRet;
}

INT32  DmStopReq (void) 
{
    INT32 lRet = OSA_FAILURE;
    DM_HASH_ENUM_T dmHashEnum;
    DM_TRANS_CTRL_T *pTransCtrl = NULL;
    DM_NOTIFY_CB_NODE_T *pCurCbNode = NULL;
    DM_NOTIFY_CB_NODE_T *pTmpCbNode = NULL;
    UINT32 msgArray[2] = {0};
	DM_UNLOAD_DB_HANDLER_REQ_T dmUnloadDbHandlerReq;
    INT32 iDmStopNum = 0;
	
    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmStopReq(%s)",g_DmCtrl.entity);	

	osa_thread_mutex_lock(&g_StartInstanceMutex);
	iDmStopNum = g_StartInstanceCount;
	if (g_StartInstanceCount > 0) 
	{
		g_StartInstanceCount --;
	}
	osa_thread_mutex_unlock(&g_StartInstanceMutex);
	
    if (iDmStopNum > 1)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Dm will be stopped (remain num=%d), leave DmStopReq(%s)", iDmStopNum - 1, g_DmCtrl.entity);
        return DM_SUCCESS;
    }
	else if(iDmStopNum <= 0) 
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Dm stopped already, leave DmStopReq(%s)",g_DmCtrl.entity);
	}
	else 
	{
		/* only last one can stop DM */
	}

    if (g_DmCtrl.bAsServer == DM_TRUE)
    {
        /*release the handlers*/
        osa_mem_set(&dmHashEnum, 0, sizeof(DM_HASH_ENUM_T));

		osa_mem_set(&dmUnloadDbHandlerReq, 0, sizeof(DM_UNLOAD_DB_HANDLER_REQ_T));
		dmUnloadDbHandlerReq.bAsServer = g_DmCtrl.bAsServer;
        lRet = DmUnloadDbHandlerReq(&dmUnloadDbHandlerReq);
		if(lRet != DM_SUCCESS)
		{
			DM_ERROR(DM_ERR_PROCESSING_COMMON_ERROR, "DM Trace ------ Failed to unload db in DmStopReq(%s)", g_DmCtrl.entity);
		}

        /* release DM handler hash table */
        DmHashDestroy(&g_dmHandlerHash, DM_TRUE);
    }
    else
    {
        osa_mem_set(&dmUnloadDbHandlerReq, 0, sizeof(DM_UNLOAD_DB_HANDLER_REQ_T));
		dmUnloadDbHandlerReq.bAsServer = g_DmCtrl.bAsServer;
        dmUnloadDbHandlerReq.callerPid = getpid();
        lRet = DmUnloadDbHandlerReq(&dmUnloadDbHandlerReq);
		if(lRet != DM_SUCCESS)
		{
			DM_ERROR(DM_ERR_PROCESSING_COMMON_ERROR, "DM Trace ------ Failed to unload db in DmStopReq(%s)", g_DmCtrl.entity);
		}
        
        DmSrvStateMonitorRegister(NULL);
    }

    /* release g_DmCtrl */
    DmSockMsgServiceStopReq(&g_DmCtrl.sockMsgQ);
    osa_resource_group_delete(&g_DmCtrl.rsrcGrp);
    DmHashDestroy(&g_DmCtrl.getFuncHash, DM_FALSE);
    DmHashDestroy(&g_DmCtrl.setFuncHash, DM_FALSE);

    osa_mem_set(&dmHashEnum, 0, sizeof(DM_HASH_ENUM_T));
    while (NULL != (pCurCbNode=DmHashEnumerator(&g_DmCtrl.notifyFuncHash, &dmHashEnum)))
    {
        while(pCurCbNode != NULL)
        {
            pTmpCbNode = pCurCbNode;
            pCurCbNode = pCurCbNode->pNext;
            DM_FREE(pTmpCbNode);
        }
    }
    DmHashDestroy(&g_DmCtrl.notifyFuncHash, DM_FALSE);

    osa_mem_set(&dmHashEnum, 0, sizeof(DM_HASH_ENUM_T));

    while (NULL != (pTransCtrl=DmHashEnumerator(&g_DmCtrl.transHash, &dmHashEnum)))
    {
            msgArray[0] = DM_MSG_CODE_LeaveInd;
            msgArray[1] = 0;

            lRet = osa_msg_queue_send(&pTransCtrl->msgQ,(void*)msgArray);

            if (lRet != OSA_SUCCESS)
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_msg_queue_send failed for DM_MSG_CODE_LeaveInd in DmStopReq(%s)",g_DmCtrl.entity);
            }
            else
            {
                DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ tell msgq-thread leaving successfully in DmStopReq(%s)",g_DmCtrl.entity);
            }
    }

    DmHashDestroy(&g_DmCtrl.transHash,DM_FALSE);

	if (g_DmCtrl.bAsServer == DM_TRUE)
	{
		/* delete administrate thread */
		DmStopNonTransHandler();
		sleep(1);

		/* delete shared memory key */
		lRet = osa_resource_group_delete(&g_DmCtrl.shmGroup);
		if (lRet != DM_SUCCESS)
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_group_delete entity(%s) asServer(%d) failed, leave DmStopReq",g_DmCtrl.entity,g_DmCtrl.bAsServer); 
			return DM_FAILURE;
		}

		lRet = DmHashDestroy(&g_DmCtrl.shmInUseHash, DM_TRUE);
		if (lRet != DM_SUCCESS)
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmHashDestroy of shared memory entity(%s) asServer(%d) failed, leave DmStopReq",g_DmCtrl.entity,g_DmCtrl.bAsServer); 
			return DM_FAILURE;
		}

		osa_thread_mutex_delete(&g_DmCtrl.addOnMutex);
		DmUnloadAllAddOn();

        /* stop thread pool */
		lRet = osa_thread_pool_delete(&g_DmCtrl.threadPool);
		if (lRet != DM_SUCCESS)
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_thread_pool_delete entity(%s) asServer(%d) failed, leave DmStopReq",g_DmCtrl.entity,g_DmCtrl.bAsServer); 
			return DM_FAILURE;
		}
	}
	
	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmStopReq(%s)",g_DmCtrl.entity);

    return DM_SUCCESS;
}

INT32 DmLoadGlobalDbHandlerReq(DM_LOAD_DB_HANDLER_REQ_T *pLoadDbHandlerReq)
{
    INT32 lRet = DM_RET_SUCCESS;

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmLoadGlobalDbHandlerReq(%s)", g_DmCtrl.entity);

	pLoadDbHandlerReq->createPid = getpid();
	pLoadDbHandlerReq->isGlobalDb = DM_TRUE;
	lRet = DmLoadDbHandlerReq(pLoadDbHandlerReq);
	
	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmLoadGlobalDbHandlerReq(%s) lRet=%d", g_DmCtrl.entity, lRet);
	
    return lRet;
}

INT32 DmLoadLocalDbHandlerReq(DM_LOAD_DB_HANDLER_REQ_T *pLoadDbHandlerReq)
{
    INT32 lRet = DM_RET_SUCCESS;

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmLoadLocalDbHandlerReq(%s)", g_DmCtrl.entity);

	pLoadDbHandlerReq->createPid = getpid();
	pLoadDbHandlerReq->isGlobalDb = DM_FALSE;
	lRet = DmLoadDbHandlerReq(pLoadDbHandlerReq);
	
	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmLoadLocalDbHandlerReq(%s) lRet=%d", g_DmCtrl.entity, lRet);

    return lRet;
}

INT32 DmLoadDbHandlerReq(DM_LOAD_DB_HANDLER_REQ_T *pLoadDbHandlerReq)
{
    INT32 lRet = DM_RET_SUCCESS;
    DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;
    DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmLoadDbHandlerReq(%s)", g_DmCtrl.entity);

	if(NULL == pLoadDbHandlerReq)
	{
        lRet = DM_RET_INVALID_ARGUMENTS;
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid argument in DmLoadDbHandlerReq(%s)",g_DmCtrl.entity);
	    return lRet;
	}
	
    if (g_DmCtrl.bAsServer == DM_FALSE)
    {

	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pLoadDbHandlerReq->dbLibPath = (%s)",pLoadDbHandlerReq->dbLibPath);
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pLoadDbHandlerReq->loadPath = (%s)",pLoadDbHandlerReq->loadPath);
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pLoadDbHandlerReq->backupPath = (%s)",pLoadDbHandlerReq->backupPath);
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pLoadDbHandlerReq->workingPath = (%s)",pLoadDbHandlerReq->workingPath);
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pLoadDbHandlerReq->bDbReset = (%d)",pLoadDbHandlerReq->bDbReset);
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pLoadDbHandlerReq->bXmlWithComment = (%d)",pLoadDbHandlerReq->bXmlWithComment);
		
        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T) + sizeof(DM_LOAD_DB_HANDLER_REQ_T));
        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmLoadDbHandlerReq(%s)", g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T) + sizeof(DM_LOAD_DB_HANDLER_REQ_T));
        pSockMsgSnd->msg_id = DM_MSG_CODE_LoadDbHandlerReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
		pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->word_data = 0;
        pSockMsgSnd->msgLength = sizeof(DM_LOAD_DB_HANDLER_REQ_T);

        osa_mem_copy((void*)(pSockMsgSnd + 1), pLoadDbHandlerReq, pSockMsgSnd->msgLength);

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmLoadDbHandlerReq(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }
        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_LoadDbHandlerCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp, &pSockMsgRcv, DM_SOCK_MSG_TIMEOUT_MS * 3);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_LoadDbHandlerCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_LoadDbHandlerCnf)
        {
            lRet = pSockMsgRcv->word_data;
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmLoadDbHandlerReq(%s)", g_DmCtrl.entity);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmLoadDbHandlerReq(%s)", g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {
		lRet = DmLoadDbHandlerReqInternal(pLoadDbHandlerReq);
    }

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmLoadDbHandlerReqCommon(%s) lRet=%d", g_DmCtrl.entity, lRet);

    return lRet;
}

INT32 DmUnloadDbHandlerReq(DM_UNLOAD_DB_HANDLER_REQ_T *pUnloadDbHandlerReq)
{
    INT32 lRet = DM_RET_SUCCESS;
    DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;
    DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmUnloadDbHandlerReq(%s)", g_DmCtrl.entity);

	if(NULL == pUnloadDbHandlerReq)
	{
        lRet = DM_RET_INVALID_ARGUMENTS;
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid argument in DmUnloadDbHandlerReq(%s)",g_DmCtrl.entity);
	    return lRet;
	}
	
    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T) + sizeof(DM_UNLOAD_DB_HANDLER_REQ_T));
        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmUnloadDbHandlerReq(%s)", g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T) + sizeof(DM_UNLOAD_DB_HANDLER_REQ_T));
        pSockMsgSnd->msg_id = DM_MSG_CODE_UnloadDbHandlerReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
		pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->word_data = 0;
        pSockMsgSnd->msgLength = sizeof(DM_UNLOAD_DB_HANDLER_REQ_T);

        osa_mem_copy((void*)(pSockMsgSnd + 1), pUnloadDbHandlerReq, pSockMsgSnd->msgLength);

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmUnloadDbHandlerReq(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }
        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_UnloadDbHandlerCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp, &pSockMsgRcv, DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_UnloadDbHandlerCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_UnloadDbHandlerCnf)
        {
            lRet = pSockMsgRcv->word_data;
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmUnloadDbHandlerReq(%s)", g_DmCtrl.entity);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmUnloadDbHandlerReq(%s)", g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {
		lRet = DmUnloadDbHandlerReqInternal(pUnloadDbHandlerReq);
    }

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmUnloadDbHandlerReq(%s) lRet=%d", g_DmCtrl.entity, lRet);

    return lRet;
}

INT32 DmTransactionStartReq  ( DM_TRANSACTION_REQ_T *pTransaction )  
{
    DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
    DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;
    INT32 lRet = DM_RET_SUCCESS;
	DM_HANDLER_T *pDmHandler = NULL;

	if(NULL == pTransaction)
	{
        lRet = DM_RET_INVALID_ARGUMENTS;
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid argument in DmTransactionStartReq(%s)",g_DmCtrl.entity);
	    return lRet;
	}

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmTransactionStartReq(%s), %s",g_DmCtrl.entity, pTransaction->RootNodeToLock);

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
		UINT32 msgBodyLength = 0;
		INT8 *pMsgBody = NULL;

		msgBodyLength += sizeof(DM_TRANSACTION_REQ_T);
		msgBodyLength += sizeof(DM_CALLER_INFO_T);

        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T) + msgBodyLength);
        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmTransactionStartReq(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T) + msgBodyLength);

        pSockMsgSnd->msg_id = DM_MSG_CODE_TransactionStartReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->word_data = 0;
		pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = msgBodyLength;

		pMsgBody = (INT8*)(pSockMsgSnd + 1);
        osa_mem_copy(pMsgBody, pTransaction, sizeof(DM_TRANSACTION_REQ_T));
		pMsgBody += sizeof(DM_TRANSACTION_REQ_T);

		// Set caller pid for check DB ownner permission
		DM_CALLER_INFO_T* pCaller = (DM_CALLER_INFO_T*)pMsgBody;
		pCaller->pid = getpid();

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmTransactionStartReq(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_TransactionStartCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_TransactionStartCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_TransactionStartCnf)
        {
            osa_mem_copy(pTransaction, (void*)(pSockMsgRcv+1), sizeof(DM_TRANSACTION_REQ_T));
            lRet = pSockMsgRcv->word_data;
            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmTransactionStartReq(%s)",g_DmCtrl.entity);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmTransactionStartReq(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {
        /* save transaction info and generate transaction id */
        pTransaction->TransactionId =  0;

        pDmHandler = GetDmHandlerByName(pTransaction->RootNodeToLock);

        if(NULL == pDmHandler)
        {
            lRet = DM_RET_INVALID_ARGUMENTS;
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to find pDmHandler in DmTransactionStartReq(%s)",g_DmCtrl.entity);
            return lRet;
        }

        /* get rw-mutex to lock */
    	lRet = osa_resource_wait(&(pDmHandler->resourceGrp4RW), pTransaction, DM_SOCK_MSG_TIMEOUT_MS);

        if (lRet == DM_SUCCESS)
        {
            lRet = DM_RET_SUCCESS;
        }
        else
        {
            lRet = DM_RET_INTERNAL_ERROR;
        }
    }

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmTransactionStartReq(%s) lock-path=%s trans-id=%d lRet = %d",g_DmCtrl.entity,pTransaction->RootNodeToLock,pTransaction->TransactionId,lRet);	

    return lRet;
}

INT32 DmTransactionStopReq(UINT32 transactionId)  
{
    INT32 lRet = DM_RET_SUCCESS;

	/* call notify function */
	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmTransactionStopReq(%s), trans-id=%d",g_DmCtrl.entity,transactionId);

	if(0 >= transactionId)
	{
        /* invalid transaction id */
        lRet = DM_RET_INVALID_ARGUMENTS;
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmTransactionStopReq(%s), trans-id (%d) <= 0",g_DmCtrl.entity,transactionId);
		return lRet;
	}

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;

        /* client things */
        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+sizeof(INT32));

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmTransactionStopReq(%s) transactionId=%d",g_DmCtrl.entity,transactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+sizeof(INT32));

        pSockMsgSnd->msg_id = DM_MSG_CODE_TransactionStopReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->word_data = (INT32)transactionId;/* keep transaction id */
        pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = sizeof(INT32);

        osa_mem_copy((void*)(pSockMsgSnd+1), &transactionId, pSockMsgSnd->msgLength);

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmTransactionStopReq(%s) transactionId=%d",g_DmCtrl.entity,transactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_TransactionStopCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);/* 30 timeout */
        if (pSockMsgRcv == (void*)DM_MSG_CODE_TransactionStopCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_TransactionStopCnf)
        {
            lRet = pSockMsgRcv->word_data;
            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmTransactionStopReq(%s) transactionId=%d",g_DmCtrl.entity,transactionId);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmTransactionStopReq(%s) transactionId=%d",g_DmCtrl.entity,transactionId);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {
        DM_TRANSACTION_INFO_T transInfo;
        DM_TRANSACTION_INFO_T *pTransInfo = &transInfo;
        DM_HANDLER_T *pDmHandler = NULL;

        /* server things */
        pDmHandler = GetDmHandlerByTransId(transactionId);

        if(NULL == pDmHandler)
        {
            lRet = DM_RET_INVALID_ARGUMENTS;
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ fail to find DmHandler, leave DmTransactionStopReq(%s) transactionId=%d lRet=%d",g_DmCtrl.entity,transactionId,lRet);	
    		return lRet;
        }

        osa_mem_set(pTransInfo, 0, sizeof(DM_TRANSACTION_INFO_T));
        pTransInfo->transReq.TransactionId = transactionId;

        lRet = osa_resource_release(&(pDmHandler->resourceGrp4RW), (void*)pTransInfo);

        if (lRet == DM_SUCCESS)
        {
            lRet = DM_RET_SUCCESS;
        }
        else
        {
            lRet = DM_RET_INTERNAL_ERROR;
        }

    	DmCallNotifyHandler(pDmHandler, pTransInfo);
    }

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmTransactionStopReq(%s) transactionId=%d lRet=%d",g_DmCtrl.entity,transactionId,lRet);	

	return lRet;
}

INT32 DmLoadDbHandlerReqInternal(DM_LOAD_DB_HANDLER_REQ_T *pLoadDbHandlerReq)
{
	INT32 lRet = DM_SUCCESS;
	DM_HANDLER_T *pDmHandler = NULL;
	
	if(strlen(pLoadDbHandlerReq->workingPath) == 0 )
	{
		strcpy(pLoadDbHandlerReq->workingPath, "/tmp/");
	}

	if (strlen(pLoadDbHandlerReq->dbName) != 0)
	{
		pDmHandler = GetDmHandlerByName(pLoadDbHandlerReq->dbName);
		if (NULL != pDmHandler)
		{
			lRet = DM_RET_INVALID_ARGUMENTS;
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ specficed db %s has been loaded in DmLoadDbHandlerReqInternal(%s)", pLoadDbHandlerReq->dbName, g_DmCtrl.entity);
			return lRet;
		}
		
		lRet = DmLoadOneDb(pLoadDbHandlerReq);
	}
	else
	{
		lRet = DmLoadAllDb(pLoadDbHandlerReq);
	}
		
	return lRet;
}

INT32 DmUnloadDbHandlerReqInternal(DM_UNLOAD_DB_HANDLER_REQ_T *pUnloadDbHandlerReq)
{
	INT32 lRet = DM_SUCCESS;
	DM_HASH_ENUM_T dmHashEnum;
	DM_HANDLER_T *pHandler = NULL;
	UINT32  i = 0;

	/*release the handlers*/
	osa_mem_set(&dmHashEnum, 0, sizeof(DM_HASH_ENUM_T));

	while (NULL != (pHandler = DmHashEnumerator(&g_dmHandlerHash, &dmHashEnum)))
	{
		BOOL8 canUnloadDb = DM_FALSE;

		osa_thread_mutex_lock(&(pHandler->mutexXml));

		if(pHandler->createPid == pUnloadDbHandlerReq->callerPid)
		{
			canUnloadDb = DM_TRUE;
		}
		if (pUnloadDbHandlerReq->bAsServer)
		{
			canUnloadDb = DM_TRUE;    // As server, it can free all Db
		}
		if(!canUnloadDb)
		{
			osa_thread_mutex_unlock(&(pHandler->mutexXml));
			continue;
		}

		for(i = 0; i < pHandler->lPatternLength ; i++)
		{
			if(pHandler->pPattern[i].pNotifyHandler != NULL)
			{
				DM_NODE_NOTIFY_HANDLER_T *pTmpNotifyHandler = pHandler->pPattern[i].pNotifyHandler;
				DM_NODE_NOTIFY_HANDLER_T *pTmpNotifyHandlerP = pTmpNotifyHandler;
				while(pTmpNotifyHandlerP != NULL)
				{
					pTmpNotifyHandler = pTmpNotifyHandler->pNext;
					OSA_MEM_FREE(pTmpNotifyHandlerP);
					pTmpNotifyHandlerP = pTmpNotifyHandler;
				}
			}

			DM_FREE(pHandler->pPattern[i].pGetHandler);
		}

		for(i = 0; i < pHandler->lPatternLength ; i++)
		{
			DM_FREE(pHandler->pPattern[i].pNodeHandler);
		}

		DmHashDestroy(&(pHandler->hashNameID), DM_TRUE);
		DmHashDestroy(&(pHandler->hashTransactions), DM_TRUE);

		osa_resource_group_delete(&(pHandler->resourceGrp4RW));
		osa_thread_mutex_delete(&(pHandler->mutexNotifyList));

		/**
		 *	we need to synchronize DB files to Flash partition for next bootup
		*/
		if(pHandler->bExtDb==DM_FALSE)
		{
			if (DmSyncDBFileReq(pHandler)==DM_SUCCESS)
			{
				DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ sync %s to %s and %s succssfully in DmUnloadDbHandlerReqInternal(%s)",
				         pHandler->pucFilePath,pHandler->pLoadPath,pHandler->pBackupPath,g_DmCtrl.entity);
			}
			else
			{
				DM_ERROR(DM_ERR_PROCESSING_COMMON_ERROR, "DM Trace ------ sync %s to %s and %s failed in DmUnloadDbHandlerReqInternal(%s)",
				         pHandler->pucFilePath,pHandler->pLoadPath,pHandler->pBackupPath,g_DmCtrl.entity);
			}
			
			xmlFreeDoc(pHandler->pXmlDocument);
			pHandler->pXmlDocument = NULL;
		}

		// remove this Dm handler from hash table.
		if (DmHashDel(&g_dmHandlerHash, pHandler->pucDbName) == NULL)
		{
			osa_thread_mutex_unlock(&(pHandler->mutexXml));
			DM_ERROR(DM_ERR_PROCESSING_COMMON_ERROR, "DM Trace ------ Failed to delete DM handler %s from hash table(%s).", pHandler->pucDbName, g_DmCtrl.entity);
			continue;
		}
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ unload DM handler %s from hash table(%s).", pHandler->pucDbName, g_DmCtrl.entity);
		osa_thread_mutex_unlock(&(pHandler->mutexXml));

		// release resource
		if (dlclose(pHandler->pDlHandler) != 0)
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ %s dlclose failed : %s in DmUnloadDbHandlerReqInternal(%s)", pHandler->pucDbName, dlerror(),g_DmCtrl.entity);
		}
		else
		{
			DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ %s dlclose successfully in DmUnloadDbHandlerReqInternal(%s)",pHandler->pucDbName,g_DmCtrl.entity);
		}

		DM_FREE(pHandler->pucDbName);
		DM_FREE(pHandler->pucFilePath);

		DM_FREE(pHandler->pucXsltPath);
		DM_FREE(pHandler->pucBackFilePath);

		DM_FREE(pHandler->pLoadPath);
		DM_FREE(pHandler->pBackupPath);
		DM_FREE(pHandler->pMappingXmlPath);
		
		DM_FREE(pHandler);
	}
	
	return lRet;
}

/**
 * Module A use this API to get parameter name from DB file. The result will return synchronously.
 * @param pGetParaNameReq parameter for whose childs should be gotten
 * @return 0: success   other: failure
 */
INT32   DmGetParaNameInternal(DM_GET_PARA_NAME_REQ_T *pGetParaNameReq)
{
    INT32 ret = DM_RET_SUCCESS;
    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmGetParaNameInternal");

    if(NULL == pGetParaNameReq)
    {
        return DM_RET_INTERNAL_ERROR;
    }


    /* if nextLevel == true, get the next level children */
    if(DM_TRUE == pGetParaNameReq->bNextLevel)
    {
        ret = DmGetChildren(pGetParaNameReq);
    }
    /* if nextLevel == false, get the descendant children */
    else
    {
        ret = DmGetDescendant(pGetParaNameReq);
    }
    /* set all the writable flag of parameters */
	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmGetParaNameInternal");	
    return ret;
}


INT32 DmGetParaCValueInternal(DM_GET_PARA_C_VALUE_REQ_T *  pGetPara)
{
    DM_HANDLER_T *pDmHandler = NULL;

	INT32           lId      = 0;
	DM_RET_E           lRet     = DM_RET_INTERNAL_ERROR;
	xmlXPathObjectPtr   xpathObj = NULL;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmGetParaCValueInternal");

	if(NULL == pGetPara)
	{
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pGetPara is null, in DmGetParaCValueInternal");
	    return lRet;
	}

    pDmHandler = GetDmHandlerByName(pGetPara->pucPath);

    if(NULL == pDmHandler)
    {
        lRet = DM_RET_INVALID_ARGUMENTS;
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pDmHandler is null, in DmGetParaCValueInternal");
        return lRet;
    }

    /* here we adjust the param id in case that data model changed without ps_cpuh re-compiling */
    lId = DmGetIDFromPath(&(pDmHandler->hashNameID), pGetPara->pucPath);
    if (lId<0)
    {
        lRet = DM_RET_INVALID_ARGUMENTS;
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid pGetPara->pucPath (%s), in DmGetParaCValueInternal",pGetPara->pucPath);
        return lRet;
    }
	pGetPara->ulParaId = lId;

    /*check right*/
    if(!DmCheckRight(pGetPara->TransactionId, pDmHandler, RIGHT_R, lId))
	{
	    return DM_RET_REQUEST_DENIED;
	}

	if(NULL == pDmHandler->xpathCtx)
	{
		pDmHandler->xpathCtx = xmlXPathNewContext(pDmHandler->pXmlDocument);
		if(NULL == pDmHandler->xpathCtx)
		{
			return lRet;
		}
	}


	INT8 *ucXPathName = NULL;
	ucXPathName = (INT8 *)DM_MALLOC(sizeof(INT8)*MAX_XML_PATH);

	if (ucXPathName == NULL)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmGetParaCValueInternal : ucXPathName = NULL"); 

		return lRet;

	}

	osa_mem_set(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);

	if(DM_FALSE == isObject(pDmHandler,pGetPara->ulParaId))
	{
		INT8 * pTemp = DmGetObjectPath(pGetPara->pucPath);
		ConvertPathToXPath(pTemp, ucXPathName);
		DM_FREE(pTemp);
	}
	else
	{
    	ConvertPathToXPath(pGetPara->pucPath, ucXPathName);
	}

	xpathObj = DbGetXPathObjPtr(pDmHandler->xpathCtx,ucXPathName);

	DM_FREE(ucXPathName);
	
    if(DM_TRUE ==  DmGetCHandler(pGetPara))
	{
	    lRet = DM_RET_SUCCESS;
	}

	xmlXPathFreeObject(xpathObj);
	xmlXPathFreeContext(pDmHandler->xpathCtx);
	pDmHandler->xpathCtx = NULL;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmGetParaCValueInternal");	
    
	return lRet;
}

INT32 DmSetParaCValueInternal  ( DM_SET_PARA_C_VALUE_REQ_T *  pSetPara )  
{
    DM_HANDLER_T *pDmHandler = NULL;

	INT32           lId      = 0;
	DM_RET_E           lRet     = DM_RET_INTERNAL_ERROR;
	xmlXPathObjectPtr   xpathObj = NULL;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmSetParaCValueInternal");

	if(NULL == pSetPara)
	{
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pSetPara is null, in DmSetParaCValueInternal");
	    return lRet;
	}

    pDmHandler = GetDmHandlerByName(pSetPara->pucPath);

    if(NULL == pDmHandler)
    {
        lRet = DM_RET_INVALID_ARGUMENTS;
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pDmHandler is null, in DmSetParaCValueInternal");
        return lRet;
    }

    /* here we adjust the param id in case that data model changed without ps_cpuh re-compiling */
    lId = DmGetIDFromPath(&(pDmHandler->hashNameID), pSetPara->pucPath);
    if (lId<0)
    {
        lRet = DM_RET_INVALID_ARGUMENTS;
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid pSetPara->pucPath (%s), in DmSetParaCValueInternal",pSetPara->pucPath);
        return lRet;
    }
	pSetPara->ulParaId = lId;

    /*check right*/
    if(!DmCheckRight(pSetPara->TransactionId, pDmHandler, RIGHT_W, lId))
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ parameter %s can not be written by TransactionId %d",pSetPara->pucPath,pSetPara->TransactionId);
	    return DM_RET_REQUEST_DENIED;
	}


	DmCheckTransferNeed(pDmHandler,lId);


	if(NULL == pDmHandler->xpathCtx)
	{
		pDmHandler->xpathCtx = xmlXPathNewContext(pDmHandler->pXmlDocument);
		if(NULL == pDmHandler->xpathCtx)
		{
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pDmHandler->xpathCtx is null, in DmSetParaCValueInternal");
			return lRet;
		}
	}


	INT8 *ucXPathName = NULL;
	ucXPathName = (INT8 *)DM_MALLOC(sizeof(INT8)*MAX_XML_PATH);

	if (ucXPathName == NULL)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaCValueInternal : ucXPathName = NULL"); 

		return lRet;

	}

	osa_mem_set(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);


	if(DM_FALSE == isObject(pDmHandler,pSetPara->ulParaId))
	{
		INT8 * pTemp = DmGetObjectPath(pSetPara->pucPath);
		ConvertPathToXPath(pTemp, ucXPathName);
		DM_FREE(pTemp);
	}
	else
	{
    	ConvertPathToXPath(pSetPara->pucPath, ucXPathName);
	}

	xpathObj = DbGetXPathObjPtr(pDmHandler->xpathCtx,ucXPathName);

	DM_FREE(ucXPathName);

	if(DM_TRUE ==  DmSetCHandler(pSetPara))
	{
	    lRet = DM_RET_SUCCESS;
	}

	xmlXPathFreeObject(xpathObj);
	xmlXPathFreeContext(pDmHandler->xpathCtx);
	pDmHandler->xpathCtx = NULL;

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmSetParaCValueInternal");	

	return lRet;
}

INT32 DmAddObjectByIdInternal  ( DM_ADD_OBJ_BY_ID_REQ_T *  pAddPara )  
{
    DM_HANDLER_T *pDmHandler = NULL;

	INT32           lId      = 0;
	DM_RET_E			ucRet = DM_RET_SUCCESS;

	if(NULL == pAddPara || NULL == pAddPara->pucPath)
	{
	    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ DmAddObjectByIdInternal parameter error");
	    return DM_RET_INTERNAL_ERROR;
	}
	else
	{
	    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmAddObjectByIdInternal %s", pAddPara->pucPath);
	}

    pDmHandler = GetDmHandlerByName(pAddPara->pucPath);

    if(NULL == pDmHandler)
    {
        ucRet = DM_RET_INVALID_ARGUMENTS;
        return ucRet;
    }

    /* here we adjust the param id in case that data model changed without ps_cpuh re-compiling */
    lId = DmGetIDFromPath(&(pDmHandler->hashNameID), pAddPara->pucPath);
    if (lId < 0)
    {
        ucRet = DM_RET_INVALID_ARGUMENTS;
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid pAddPara->pucPath (%s), in DmAddObjectByIdInternal",pAddPara->pucPath);
        return ucRet;
    }
	pAddPara->ulObjId = lId;
	
    /*check right*/
    if(!DmCheckRight(pAddPara->TransactionId, pDmHandler, RIGHT_W, lId))
	{
	    return DM_RET_REQUEST_DENIED;
	}

	DmCheckTransferNeed(pDmHandler,lId);


	if(0 != strcmp(pDmHandler->pPattern[lId].pucNodeName+strlen(pDmHandler->pPattern[lId].pucNodeName)-strlen(".{i}."),".{i}."))
	{
	    return DM_RET_REQUEST_DENIED;
	}		

    if(DbGetObjectNum(pDmHandler,pAddPara->pucPath) >= atoi((char*)pDmHandler->pPattern[lId].pValueRange))
    {

        return DM_RET_RESOURCE_EXCEEDED;
    }
	

	INT8  aucValue[MAX_INSTANCE_NUMBER_SIZE] = {0};
	DM_HANDLER_PARA_T DmHandlerPara = {0};

    osa_mem_set(&DmHandlerPara,0,sizeof(DM_HANDLER_PARA_T));

	DmHandlerPara.TransactionId = pAddPara->TransactionId;
	DmHandlerPara.ulParaId	  = pAddPara->ulObjId;
	DmHandlerPara.pDbHandler = pDmHandler;
	DmHandlerPara.pucName = pAddPara->pucPath;
	DmHandlerPara.bCInterface = DM_TRUE;
	DmHandlerPara.bItem = DM_FALSE;
	DmHandlerPara.ulCnt = 1;
	DmHandlerPara.pValue = aucValue;
    DmHandlerPara.valueSize = pAddPara->objectSize;
	

	if(DM_RET_SUCCESS != DmCallAddHandler(pDmHandler->pPattern[lId].pNodeHandler, &DmHandlerPara)) /**< call add handler */
	{	
		DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmAddObjectByIdInternal call DmCallAddHandler failure for multiple obj %s", DmHandlerPara.pucName);
		return DM_RET_INTERNAL_ERROR;
	}

	*((UINT32 *)pAddPara->pObject) = atoi(aucValue+1);  /**<  set instance number */
    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ New instance number %d in DmAddObjectByIdInternal",*((UINT32 *)pAddPara->pObject));
	
	INT8 *pucChildObjName = DM_MALLOC((strlen((char*)(DmHandlerPara.pucName))+strlen((char*)(DmHandlerPara.pValue))+2)*sizeof(INT8));
    if(NULL == pucChildObjName)
    {
        return DM_RET_INTERNAL_ERROR;
    }

	INT8 *pucTmp = pAddPara->pucPath;
	strcpy((char*)pucChildObjName, (char*)(DmHandlerPara.pucName));
	strcat((char*)pucChildObjName,(char*)(aucValue+1));
	strcat((char*)pucChildObjName,".");
	DmHandlerPara.pucName = pucChildObjName;
	
	pAddPara->pucPath = pucChildObjName;

	xmlXPathContextPtr	xpathCtx = NULL;

	xpathCtx = xmlXPathNewContext(pDmHandler->pXmlDocument);
	if(NULL == xpathCtx)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmAddObjectByIdInternal can't get xpathCtx");
		return DM_RET_INTERNAL_ERROR;
	}

	INT8 *ucXPathName = NULL;
	ucXPathName = (INT8 *)DM_MALLOC(sizeof(INT8)*MAX_XML_PATH);

	if (ucXPathName == NULL)
	{

		return DM_RET_INTERNAL_ERROR;
	}

	osa_mem_set(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);

	ConvertPathToXPath(pucChildObjName, ucXPathName);
	pDmHandler->xpathParentObj = xmlXPathEvalExpression((const xmlChar *)ucXPathName, xpathCtx);

	DM_FREE(ucXPathName);
	
	if(DM_FALSE == DmAddCHandler(pAddPara))
	{
		ucRet = DM_RET_INTERNAL_ERROR;
	}

	pAddPara->pucPath = pucTmp;
	DM_FREE(pucChildObjName);

	if(NULL != xpathCtx)
	{
		xmlXPathFreeObject(pDmHandler->xpathParentObj);
		xmlXPathFreeContext(xpathCtx);
		pDmHandler->xpathParentObj = NULL;
	}

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmAddObjectByIdInternal");

    return ucRet;
}




INT32 DmDelObjectByIdInternal  ( DM_DEL_OBJ_BY_ID_REQ_T *  pDelPara )  
{
    DM_HANDLER_T *pDmHandler = NULL;

	INT32           lId      = 0;
    DM_RET_E    	lRet = DM_RET_SUCCESS;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmDelObjectByIdInternal");

	if(NULL == pDelPara)
	{
	    return DM_RET_INTERNAL_ERROR;
	}

    pDmHandler = GetDmHandlerByName(pDelPara->pucPath);

    if(NULL == pDmHandler)
    {
        lRet = DM_RET_INVALID_ARGUMENTS;
        return lRet;
    }

    /* here we adjust the param id in case that data model changed without ps_cpuh re-compiling */
    lId = DmGetIDFromPath(&(pDmHandler->hashNameID), pDelPara->pucPath);
    if (lId < 0)
    {
        lRet = DM_RET_INVALID_ARGUMENTS;
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid pDelPara->pucPath (%s), in DmDelObjectByIdInternal",pDelPara->pucPath);
        return lRet;
    }
	pDelPara->ulObjId = lId;

    /*check right*/
    if(!DmCheckRight(pDelPara->TransactionId, pDmHandler, RIGHT_W, lId))
	{
	    return DM_RET_REQUEST_DENIED;
	}

	DmCheckTransferNeed(pDmHandler,lId);

    DM_NODE_HANDLER_T *pNodeHandler = pDmHandler->pPattern[lId].pNodeHandler;

    DM_HANDLER_PARA_T DmHandlerPara = {0};

    osa_mem_set(&DmHandlerPara,0,sizeof(DM_HANDLER_PARA_T));

    DmHandlerPara.TransactionId = pDelPara->TransactionId;
    DmHandlerPara.ulParaId    = lId;
    DmHandlerPara.pDbHandler = pDmHandler;
    DmHandlerPara.pucName     = pDelPara->pucPath;
    DmHandlerPara.ulCnt = 0;
    DmHandlerPara.pValue = NULL;
	DmHandlerPara.bCInterface = DM_TRUE;
	DmHandlerPara.bItem = DM_FALSE;
    DmHandlerPara.valueSize = 0;

	if(0 == DbGetObjectNum(pDmHandler,pDelPara->pucPath))
	{
		return lRet;
	}
    
    if(pNodeHandler->pDelBefHandler != NULL)
    {
        if(!pNodeHandler->pDelBefHandler(&DmHandlerPara))
        {

            return DM_RET_INTERNAL_ERROR;
        }
    }
    
    if(pNodeHandler->pDelCurHandler != NULL)
    {
        if(!pNodeHandler->pDelCurHandler(&DmHandlerPara))
        {
            return DM_RET_INVALID_PARAM_NAME;
        }
    }
    else
    {
        return DM_RET_INTERNAL_ERROR;
    }
    
    if(pNodeHandler->pDelAftHandler!= NULL)
    {
        if(!pNodeHandler->pDelAftHandler(&DmHandlerPara))
        {
            return DM_RET_INTERNAL_ERROR;
        }
    }

	
	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmDelObjectByIdInternal");	

	return lRet;
}


INT32   DmGetParaValueInternal(DM_GET_PARA_VALUE_REQ_T    *pGetParaValueReq)
{
    DM_HANDLER_T   *pDmHandler = NULL;
    INT32           lIndex     = -1;
    UINT32           i          = 0;
    INT32           j          = 0;
    UINT32           k          = 0;
    UINT32           z          = 0;
    UINT32          ulParaCnt  = 0;
	DM_HASH_T          calledFuncHash = {0};
	xmlXPathObjectPtr   xpathObj = NULL;
	DM_XML_OBJ_STACK xmlStack[16];
	UINT8 			cStackTop = 0;

    
    if(NULL == pGetParaValueReq)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmGetParaValueInternal parameter error");
        return DM_RET_INTERNAL_ERROR;
    }
	else
	{
	    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmGetParaValueInternal");
	}

	pGetParaValueReq->pfnFree = DmFreeGetParaValueReq;

    /* get the dm handler */
    pDmHandler = GetDmHandlerByName(pGetParaValueReq->pNameList->pucParamName->pucName);

    if(NULL == pDmHandler)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to find pDmHandler for %s in DmGetParaValueInternal",pGetParaValueReq->pNameList->pucParamName->pucName);
        return DM_RET_INVALID_PARAM_NAME;
    }

    /* check Name List */
    for(i = 0; i < pGetParaValueReq->pNameList->udwCnt; i++)
    {
        lIndex = DmGetIDFromPath(&(pDmHandler->hashNameID), pGetParaValueReq->pNameList->pucParamName[i].pucName);
        if(lIndex < 0)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to find lIndex for %s in DmGetParaValueInternal",pGetParaValueReq->pNameList->pucParamName[i].pucName);
            return DM_RET_INVALID_PARAM_NAME;
        }
		else if(!DmCheckRight(pGetParaValueReq->TransactionId, pDmHandler, RIGHT_R, lIndex))
		{
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ %s not readable in DmGetParaValueInternal",pGetParaValueReq->pNameList->pucParamName[i].pucName);
			return DM_RET_REQUEST_DENIED;
		}
    }

    /* get parameter count */
    j = 0;

	DM_GET_PARA_NAME_REQ_T *pGetParaNameReq = (DM_GET_PARA_NAME_REQ_T *)DM_MALLOC(sizeof(DM_GET_PARA_NAME_REQ_T)*pGetParaValueReq->pNameList->udwCnt);

	if(NULL == pGetParaNameReq)
	{
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed for pGetParaNameReq in DmGetParaValueInternal");
		return DM_RET_INTERNAL_ERROR;
	}
	
	osa_mem_set(pGetParaNameReq,0,sizeof(DM_GET_PARA_NAME_REQ_T)*pGetParaValueReq->pNameList->udwCnt);

    for(i = 0; i < pGetParaValueReq->pNameList->udwCnt; i++)
    {
		
		if(DM_FALSE == DbNodeExistCheck(pDmHandler,pGetParaValueReq->pNameList->pucParamName[i].pucName))
		{
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ %s not exist in DmGetParaValueInternal",pGetParaValueReq->pNameList->pucParamName[i].pucName);
			continue;
		}

        pGetParaNameReq[i].TransactionId = pGetParaValueReq->TransactionId;
        pGetParaNameReq[i].bNextLevel = 0;
        pGetParaNameReq[i].pucParamName = strdup((char*)pGetParaValueReq->pNameList->pucParamName[i].pucName);
        DmGetParaNameInternal(&(pGetParaNameReq[i]));

        for(k = 0; k < pGetParaNameReq[i].pNameList->udwCnt; k++)
        {

            if(pGetParaNameReq[i].pNameList->pucParamName[k].pucName[strlen((char*)pGetParaNameReq[i].pNameList->pucParamName[k].pucName)-1]
                  != '.')
            {
                lIndex = DmGetIDFromPath(&(pDmHandler->hashNameID), pGetParaNameReq[i].pNameList->pucParamName[k].pucName);
                
                /*check right*/
                if(!DmCheckRight(pGetParaValueReq->TransactionId, pDmHandler, RIGHT_R, lIndex))
                {
                    continue;
                }
                j++;
            }
        }
        

    }

    ulParaCnt = j;

    /* allocate memory for each parameter */
    pGetParaValueReq->pvList = (PARAMETER_VALUE_LIST_T *)DM_MALLOC(sizeof(PARAMETER_VALUE_LIST_T));
    if(NULL == pGetParaValueReq->pvList)
    {

		for(i = 0; i < pGetParaValueReq->pNameList->udwCnt; i++)
		{
            if (pGetParaNameReq[i].pfnFree)
            {
	            pGetParaNameReq[i].pfnFree(&(pGetParaNameReq[i]));
            }
			DM_FREE(pGetParaNameReq[i].pucParamName);
    	}
		DM_FREE(pGetParaNameReq);
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed for pGetParaValueReq->pvList in DmGetParaValueInternal");
        return DM_RET_INTERNAL_ERROR;
    }

    osa_mem_set(pGetParaValueReq->pvList,0,sizeof(PARAMETER_VALUE_LIST_T));

    pGetParaValueReq->pvList->udwCnt = ulParaCnt;

	if(0 == ulParaCnt)
	{
		for(i = 0; i < pGetParaValueReq->pNameList->udwCnt; i++)
		{
            if (pGetParaNameReq[i].pfnFree)
            {
	            pGetParaNameReq[i].pfnFree(&(pGetParaNameReq[i]));
            }
			DM_FREE(pGetParaNameReq[i].pucParamName);
    	}
		DM_FREE(pGetParaNameReq);
        return DM_RET_SUCCESS;
	}

	
    pGetParaValueReq->pvList->pstParamValue = (PARAMETER_VALUE_T *)DM_MALLOC(sizeof(PARAMETER_VALUE_T)*ulParaCnt);
    if(NULL == pGetParaValueReq->pvList->pstParamValue)
    {
		for(i = 0; i < pGetParaValueReq->pNameList->udwCnt; i++)
		{
            if (pGetParaNameReq[i].pfnFree)
            {
	            pGetParaNameReq[i].pfnFree(&(pGetParaNameReq[i]));
            }
			DM_FREE(pGetParaNameReq[i].pucParamName);
    	}
		DM_FREE(pGetParaNameReq);
		DM_FREE(pGetParaValueReq->pvList);
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed for pGetParaValueReq->pvList->pstParamValue in DmGetParaValueInternal");
        return DM_RET_INTERNAL_ERROR;
    }

    osa_mem_set(pGetParaValueReq->pvList->pstParamValue,0,sizeof(PARAMETER_VALUE_T)*ulParaCnt);

    /* get parameter value */
    j = 0;
    UINT8 *pucValue = DM_MALLOC(sizeof(UINT8)*MAX_VALUE_SIZE);
    if(NULL == pucValue)
    {
		for(i = 0; i < pGetParaValueReq->pNameList->udwCnt; i++)
		{
            if (pGetParaNameReq[i].pfnFree)
            {
	            pGetParaNameReq[i].pfnFree(&(pGetParaNameReq[i]));
            }
			DM_FREE(pGetParaNameReq[i].pucParamName);
    	}
		DM_FREE(pGetParaNameReq);
		DM_FREE(pGetParaValueReq->pvList);
		DM_FREE(pGetParaValueReq->pvList->pstParamValue);
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed for pucValue in DmGetParaValueInternal");
        return DM_RET_INTERNAL_ERROR;
    }

    osa_mem_set(pucValue,0,sizeof(UINT8)*MAX_VALUE_SIZE);

	DmHashCreate(&calledFuncHash, DM_HASH_INIT_SIZE);

	if(NULL == pDmHandler->xpathCtx)
	{
		pDmHandler->xpathCtx = xmlXPathNewContext(pDmHandler->pXmlDocument);
		if(NULL == pDmHandler->xpathCtx)
		{
			if (pucValue != NULL)
			{
				DM_FREE(pucValue);
			}
			for(i = 0; i < pGetParaValueReq->pNameList->udwCnt; i++)
			{
                if (pGetParaNameReq[i].pfnFree)
                {
				    pGetParaNameReq[i].pfnFree(&(pGetParaNameReq[i]));
                }
				DM_FREE(pGetParaNameReq[i].pucParamName);
			}
			DM_FREE(pGetParaNameReq);
			DM_FREE(pGetParaValueReq->pvList);
			DM_FREE(pGetParaValueReq->pvList->pstParamValue);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to get xpath new content in DmGetParaValueInternal");
			return DM_RET_INTERNAL_ERROR;
		}
	}

	INT8 *ucXPathName = NULL;
	ucXPathName = (INT8 *)DM_MALLOC(sizeof(INT8)*MAX_XML_PATH);

	if (ucXPathName == NULL)
	{
		if (pucValue != NULL)
		{
			DM_FREE(pucValue);
		}

		for(i = 0; i < pGetParaValueReq->pNameList->udwCnt; i++)
		{
            if (pGetParaNameReq[i].pfnFree)
            {
			    pGetParaNameReq[i].pfnFree(&(pGetParaNameReq[i]));
            }
			DM_FREE(pGetParaNameReq[i].pucParamName);
		}
		DM_FREE(pGetParaNameReq);
		DM_FREE(pGetParaValueReq->pvList);
		DM_FREE(pGetParaValueReq->pvList->pstParamValue);
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to malloc for ucXPathName in DmGetParaValueInternal");
        return DM_RET_INTERNAL_ERROR;

	}

    osa_mem_set(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);

    for(i = 0; i < pGetParaValueReq->pNameList->udwCnt; i++)
    {
		lIndex = DmGetIDFromPath(&(pDmHandler->hashNameID), pGetParaValueReq->pNameList->pucParamName[i].pucName);

        if (lIndex<0)
        {
            continue;
        }

        /* no children found, skip it */
        if (pGetParaNameReq[i].pNameList==NULL)
        {
            continue;
        }

		osa_mem_set(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);

		UINT8 ucStartIndex = 0;
		
		if(DM_FALSE == isObject(pDmHandler,lIndex))
		{
			INT8 * pTemp = DmGetObjectPath(pGetParaValueReq->pNameList->pucParamName[i].pucName);
			ConvertPathToXPath(pTemp, ucXPathName);
			DM_FREE(pTemp);
			ucStartIndex = 0;
		}
		else
		{
			ConvertPathToXPath(pGetParaValueReq->pNameList->pucParamName[i].pucName, ucXPathName);
			ucStartIndex = 1;
		}
		
		xpathObj = DbGetXPathObjPtr(pDmHandler->xpathCtx,ucXPathName);

		cStackTop = 0;
		xmlStack[cStackTop].xpathObj = xpathObj;
		xmlStack[cStackTop].lIndex = lIndex;

        for(k = ucStartIndex; k < pGetParaNameReq[i].pNameList->udwCnt; k++)
        {
            if(pGetParaNameReq[i].pNameList->pucParamName[k].pucName[strlen((char*)pGetParaNameReq[i].pNameList->pucParamName[k].pucName)-1]
                  != '.')
            {
                BOOL8 bNodeExist = DM_FALSE;
                lIndex = DmGetIDFromPath(&(pDmHandler->hashNameID), pGetParaNameReq[i].pNameList->pucParamName[k].pucName);

                if (lIndex<0)
                {
                    continue;
                }

                /*check right*/
                if(!DmCheckRight(pGetParaValueReq->TransactionId, pDmHandler, RIGHT_R, lIndex))
                {
                    continue;
                }

				UINT32 ulParentIdx = pDmHandler->pPattern[lIndex].ulParentIdx;
				if(NULL != pDmHandler->pPattern[ulParentIdx].pGetHandler)
				{
					/*if called, next*/
					INT8 *pucFuncAddress = DmUintToStr((UINT32)(pDmHandler->pPattern[ulParentIdx].pGetHandler));
					if(NULL == DmHashGetValue(&calledFuncHash, pucFuncAddress))
					{
						DM_GET_VALUE_T Para = {0};
						Para.ulParaId = ulParentIdx;
						Para.pucPath = DmGetObjectPath(pGetParaNameReq[i].pNameList->pucParamName[k].pucName);
						Para.pValue  = NULL;
                        Para.ValueSize = 0;

						DmGetHandlerCalling(pDmHandler->pPattern[ulParentIdx].pGetHandler,&Para);					
						DmHashAdd(&calledFuncHash,pucFuncAddress,(void *)1);

						DM_FREE(Para.pucPath);
					}
					DM_FREE(pucFuncAddress);
				}
				
                
                osa_mem_set(pucValue, 0, sizeof(UINT8)*MAX_VALUE_SIZE);
                pGetParaValueReq->pvList->pstParamValue[j].pucName = strdup((char*)pGetParaNameReq[i].pNameList->pucParamName[k].pucName);

                DM_HANDLER_PARA_T *pPara = (DM_HANDLER_PARA_T*)DM_MALLOC(sizeof(DM_HANDLER_PARA_T));

				if (pPara == NULL)
				{
					if (pucValue != NULL)
					{
						DM_FREE(pucValue);
					}

					if (ucXPathName != NULL)
					{
						DM_FREE(ucXPathName);
					}

					for(z = 0; z < pGetParaValueReq->pNameList->udwCnt; z++)
					{
                        if (pGetParaNameReq[z].pfnFree)
                        {
						    pGetParaNameReq[z].pfnFree(&(pGetParaNameReq[z]));
                        }
						DM_FREE(pGetParaNameReq[z].pucParamName);
					}
					DM_FREE(pGetParaNameReq);
					DM_FREE(pGetParaValueReq->pvList);
					DM_FREE(pGetParaValueReq->pvList->pstParamValue);

                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to malloc for pPara in DmGetParaValueInternal");
					return DM_RET_INTERNAL_ERROR;
				}

                osa_mem_set(pPara,0,sizeof(DM_HANDLER_PARA_T));

                pPara->TransactionId = pGetParaValueReq->TransactionId;
                pPara->ulParaId   = lIndex;
                pPara->pDbHandler = pDmHandler;
                pPara->pucName    = pGetParaNameReq[i].pNameList->pucParamName[k].pucName;
				pPara->ulCnt = 1;
                pPara->pValue= pucValue;
				pPara->bCInterface = DM_FALSE;
                pPara->valueSize = sizeof(UINT8)*MAX_VALUE_SIZE;
                
                if(pDmHandler->pPattern[lIndex].pNodeHandler->pGetBefHandler != NULL)
                {
                    pDmHandler->pPattern[lIndex].pNodeHandler->pGetBefHandler(pPara);
                }
                /*call the default one*/
                if(pDmHandler->pPattern[lIndex].pNodeHandler->pGetCurHandler != NULL)
                {
                    bNodeExist = pDmHandler->pPattern[lIndex].pNodeHandler->pGetCurHandler(pPara);
                }
                /*call the getHandler*/
                else
                {
                    bNodeExist = DM_FALSE;
                }
                
                if(pDmHandler->pPattern[lIndex].pNodeHandler->pGetAftHandler!= NULL)
                {
                    pDmHandler->pPattern[lIndex].pNodeHandler->pGetAftHandler(pPara);
                }

                if (bNodeExist==DM_TRUE)
                {
                    pGetParaValueReq->pvList->pstParamValue[j].pucValue= strdup((char*)pPara->pValue);
                }
                else
                {
                    pGetParaValueReq->pvList->pstParamValue[j].pucValue= NULL;
                }
                pGetParaValueReq->pvList->pstParamValue[j].pucType = (UINT8*)strdup((char*)pDmHandler->pDataTypeStrHooker(pDmHandler->pPattern[lIndex].ucParaType));
                pGetParaValueReq->pvList->pstParamValue[j].bTr69Password = pDmHandler->pPattern[lIndex].bTr69Password;
                DM_FREE(pPara);
                j++;
            }
			else /* it's an object */
			{
                lIndex = DmGetIDFromPath(&(pDmHandler->hashNameID), pGetParaNameReq[i].pNameList->pucParamName[k].pucName);

                if (lIndex<0)
                {
                    continue;
                }

				if(lIndex != xmlStack[cStackTop].lIndex)
				{
					while(pDmHandler->pPattern[lIndex].ulParentIdx != xmlStack[cStackTop].lIndex)
					{
						if((lIndex != xmlStack[cStackTop].lIndex)||((cStackTop>0)&&(xmlStack[cStackTop].lIndex == xmlStack[cStackTop-1].lIndex)))
						{
							xmlXPathFreeObject(xmlStack[cStackTop].xpathObj);
							xmlStack[cStackTop].xpathObj = NULL;
                            if(cStackTop > 0)
                            {
                                cStackTop--;
                            }
						}
						else
						{
							break;
						}
					}
				}
				else
				{/* else branch means multiple object case, for example, a.b, a.b.1, they need both be pushed into stack */
					if((cStackTop>0)&&(xmlStack[cStackTop].lIndex == xmlStack[cStackTop-1].lIndex))
					{/* But for instance which not 1st, we should pop up the one before it */
						xmlXPathFreeObject(xmlStack[cStackTop].xpathObj);
						xmlStack[cStackTop].xpathObj = NULL;
                        if(cStackTop > 0)
                        {
                            cStackTop--;
                        }
					}
				}

				INT8 *pNodeName = NULL;

				if(NULL != xmlStack[cStackTop].xpathObj)
				{
					pDmHandler->xpathCtx->node = xmlStack[cStackTop].xpathObj->nodesetval->nodeTab[0];
					pNodeName = DmGetLastNodeName(pGetParaNameReq[i].pNameList->pucParamName[k].pucName);
					xpathObj = DbGetXPathObjPtr(pDmHandler->xpathCtx, pNodeName);
				}
				else
				{
					xpathObj = NULL;
				}

                if (xpathObj == NULL)
                {
                    pDmHandler->xpathCtx->node = NULL;
                }

				cStackTop++;
				xmlStack[cStackTop].xpathObj = xpathObj;
				xmlStack[cStackTop].lIndex = lIndex;

				DM_FREE(pNodeName);

                /*check right*/
                if(!DmCheckRight(pGetParaValueReq->TransactionId, pDmHandler, RIGHT_R, lIndex))
                {
                    continue;
                }
				
				if(NULL != pDmHandler->pPattern[lIndex].pGetHandler)
				{
					/*if called, next*/
					INT8 *pucFuncAddress = DmUintToStr((UINT32)(pDmHandler->pPattern[lIndex].pGetHandler));
					if(NULL == DmHashGetValue(&calledFuncHash, pucFuncAddress))
					{
						DM_GET_VALUE_T Para = {0};
						Para.ulParaId = lIndex;
						Para.pucPath = pGetParaNameReq[i].pNameList->pucParamName[k].pucName;
						Para.pValue  = NULL;
                        Para.ValueSize = 0;

						DmGetHandlerCalling(pDmHandler->pPattern[lIndex].pGetHandler,&Para);					
						DmHashAdd(&calledFuncHash,pucFuncAddress,(void*)1);
					}
					DM_FREE(pucFuncAddress);
				}
			}
        }

		while(cStackTop > 0)
		{
			xmlXPathFreeObject(xmlStack[cStackTop].xpathObj);
			xmlStack[cStackTop].xpathObj = 0;
		
			cStackTop--;
		}
		xmlXPathFreeObject(xmlStack[0].xpathObj);
		xmlStack[0].xpathObj = 0;

        if (pGetParaNameReq[i].pfnFree)
        {
            pGetParaNameReq[i].pfnFree(&(pGetParaNameReq[i]));
        }
		DM_FREE(pGetParaNameReq[i].pucParamName);
    }


	DM_FREE(pGetParaNameReq);
	DM_FREE(ucXPathName);
	
	xmlXPathFreeContext(pDmHandler->xpathCtx);
	pDmHandler->xpathCtx = NULL;
    
	DmHashDestroy(&calledFuncHash, DM_FALSE);
	
	DM_FREE(pucValue);

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmGetParaValueInternal");		
	return DM_RET_SUCCESS;
}


/**
 * Module A use this API to set single or multi parameter values to DB file. The result will return synchronously.
 * @param pSetParaValueReq parameters to be set
 * @return 0: success   other: failure
 */
INT32   DmSetParaValueInternal(DM_SET_PARA_VALUE_REQ_T    *pSetParaValueReq)
{
	INT32 lRet = DM_RET_SUCCESS;
    DM_HANDLER_T   *pDmHandler = NULL;
    INT32           lId      = -1;
    UINT32           i        = 0;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmSetParaValueInternal");

    if(NULL == pSetParaValueReq || 
		NULL == pSetParaValueReq->pvList ||
		NULL == pSetParaValueReq->pvList->pstParamValue ||
		NULL == pSetParaValueReq->pvList->pstParamValue->pucName
		)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaValueInternal : null parameters");
        return DM_RET_INVALID_ARGUMENTS;
    }

    /*get the dm handler*/
    pDmHandler = GetDmHandlerByName(pSetParaValueReq->pvList->pstParamValue->pucName);

    if(NULL == pDmHandler)
    {
		pSetParaValueReq->pFaultCodeList[0] = DM_RET_INVALID_PARAM_NAME;
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaValueInternal : failed to get dm-handler");
		return DM_RET_INVALID_ARGUMENTS;
    }

    for(i = 0; i < pSetParaValueReq->pvList->udwCnt; i++)
    {
        /* check if valid parameter path */
        lId = DmGetIDFromPath(&(pDmHandler->hashNameID), pSetParaValueReq->pvList->pstParamValue[i].pucName);

        if(lId < 0)
        {
            pSetParaValueReq->pFaultCodeList[i] = DM_RET_INVALID_PARAM_NAME;
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaValueInternal : %d parameter %s invalid",i+1,pSetParaValueReq->pvList->pstParamValue[i].pucName);
			lRet = DM_RET_INVALID_ARGUMENTS;
            continue;
        }

        /* check if the parameter exists in db */
        if (DM_FALSE == pSetParaValueReq->bCreateIfNotExit
            && DM_FALSE == DbNodeExistCheck(pDmHandler, pSetParaValueReq->pvList->pstParamValue[i].pucName))
        {
            pSetParaValueReq->pFaultCodeList[i] = DM_RET_INVALID_PARAM_NAME;
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaValueInternal : %d parameter %s not exists in db",i+1,pSetParaValueReq->pvList->pstParamValue[i].pucName);
			lRet = DM_RET_INVALID_PARAM_NAME;
            continue;
        }

        /*check right*/
        if(!DmCheckRight(pSetParaValueReq->TransactionId, pDmHandler, RIGHT_W, lId))
        {
            pSetParaValueReq->pFaultCodeList[i] = DM_RET_SET_NON_WRITABLE_PARAM;
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaValueInternal : %d parameter %s not-writable",i+1,pSetParaValueReq->pvList->pstParamValue[i].pucName);
			lRet = DM_RET_INVALID_ARGUMENTS;
            continue;
        }

		/* check value */
		if(DM_FALSE == pDmHandler->pDataVaildChecker(pDmHandler->pPattern[lId].ucParaCType,pDmHandler->pPattern[lId].pValueRange,(INT8 *)pSetParaValueReq->pvList->pstParamValue[i].pucValue))
		{
            pSetParaValueReq->pFaultCodeList[i] = DM_RET_INVALID_PARAM_VALUE;
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaValueInternal : %d parameter %s invalid value",i+1,pSetParaValueReq->pvList->pstParamValue[i].pucName);
			lRet = DM_RET_INVALID_ARGUMENTS;
            continue;
		}

		if(NULL != pDmHandler->pPattern[lId].pValidityCheckHandler)
		{
			DM_VALID_CHECK_T ParaCheck = {0};
			ParaCheck.ulParaId = lId;
			ParaCheck.pucPath = pSetParaValueReq->pvList->pstParamValue[i].pucName;
			ParaCheck.pValue = pSetParaValueReq->pvList->pstParamValue[i].pucValue;
			if(DM_FALSE == pDmHandler->pPattern[lId].pValidityCheckHandler(&ParaCheck))
			{
	            pSetParaValueReq->pFaultCodeList[i] = DM_RET_INVALID_PARAM_VALUE;
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaValueInternal : %d parameter %s validity not passed",i+1,pSetParaValueReq->pvList->pstParamValue[i].pucName);
				lRet = DM_RET_INVALID_ARGUMENTS;
	            continue;				
			}
		}

		DmCheckTransferNeed(pDmHandler,lId);
    }

	if(DM_RET_SUCCESS != lRet)
	{
		return lRet;
	}

    
    for(i = 0; i < pSetParaValueReq->pvList->udwCnt; i++)
    {
        lId = DmGetIDFromPath(&(pDmHandler->hashNameID), pSetParaValueReq->pvList->pstParamValue[i].pucName);

        DM_HANDLER_PARA_T *pPara = (DM_HANDLER_PARA_T*)calloc(1, sizeof(DM_HANDLER_PARA_T));

		if (pPara == NULL)
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaValueInternal : pPara = NULL");
			return DM_RET_INTERNAL_ERROR;

		}

        osa_mem_set(pPara,0,sizeof(DM_HANDLER_PARA_T));

        pPara->TransactionId = pSetParaValueReq->TransactionId;
        pPara->ulParaId   = lId;
        pPara->pDbHandler = pDmHandler;
        pPara->pucName    = pSetParaValueReq->pvList->pstParamValue[i].pucName;
		pPara->ulCnt = 1;
        pPara->pValue   = pSetParaValueReq->pvList->pstParamValue[i].pucValue;
		pPara->bCInterface = DM_FALSE;
        pPara->valueSize = strlen(pSetParaValueReq->pvList->pstParamValue[i].pucValue)+1;

        if(pDmHandler->pPattern[lId].pNodeHandler->pSetBefHandler != NULL)
        {
            if(!pDmHandler->pPattern[lId].pNodeHandler->pSetBefHandler(pPara))
            {
                pSetParaValueReq->pFaultCodeList[i] = DM_RET_INVALID_PARAM_VALUE;
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaValueInternal : pPara %s pSetBefHandler failed",pPara->pucName);
                DM_FREE(pPara);
                return DM_RET_INVALID_ARGUMENTS;
            }
        }

		BOOL8 bRet = DM_FALSE;
        
        if(pDmHandler->pPattern[lId].pNodeHandler->pSetCurHandler != NULL)
        {
        	bRet = pDmHandler->pPattern[lId].pNodeHandler->pSetCurHandler(pPara);
            if(DM_FALSE == bRet)
            {
				if(DM_TRUE == pSetParaValueReq->bCreateIfNotExit)
				{
					DM_ADD_OBJECT_REQ_T AddObjectReq = {0};
					AddObjectReq.TransactionId = pSetParaValueReq->TransactionId;
					AddObjectReq.pucObjName = DmGetMulObjPathAndInstance(pPara->pucName,&AddObjectReq.udwInstanceNum);
					DmAddObjectReqWithInstance(&AddObjectReq);
					DM_FREE(AddObjectReq.pucObjName);
					pDmHandler->pPattern[lId].pNodeHandler->pSetCurHandler(pPara);
				}
				else
				{
					pSetParaValueReq->pFaultCodeList[i] = DM_RET_INVALID_PARAM_NAME;
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaValueInternal : pPara %s pSetCurHandler failed",pPara->pucName);
					DM_FREE(pPara);
					return DM_RET_INVALID_ARGUMENTS;
				}
            }
        }
        else
        {
        }

		if(DM_TRUE == bRet)
		{
	        if(pDmHandler->pPattern[lId].pNodeHandler->pSetAftHandler!= NULL)
	        {
	            pDmHandler->pPattern[lId].pNodeHandler->pSetAftHandler(pPara);
	        }
		}
        DM_FREE(pPara);
    }
	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmSetParaValueInternal");

	return DM_RET_SUCCESS;
}


INT32   DmSetParaAttributeInternal(DM_SET_PARA_ATTR_REQ_T    *pSetParaAttrReq)
{
	INT32 lRet = DM_RET_SUCCESS;
    DM_HANDLER_T   *pDmHandler = NULL;
    INT32           lId      = -1;
    UINT32           i        = 0;
    UINT32           j        = 0;
    UINT32           k        = 0;
    DM_GET_PARA_NAME_REQ_T *pGetNameReq = NULL;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmSetParaAttributeInternal");

    if(NULL == pSetParaAttrReq || 
        0 == pSetParaAttrReq->ulParamCnt ||
		NULL == pSetParaAttrReq->pParamAttrList ||
		NULL == pSetParaAttrReq->pParamAttrList->pucParamName)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaAttributeInternal : null parameters");
        return DM_RET_INVALID_ARGUMENTS;
    }

    /*get the dm handler*/
    pDmHandler = GetDmHandlerByName(pSetParaAttrReq->pParamAttrList->pucParamName);

    if(NULL == pDmHandler)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaAttributeInternal : failed to get dm-handler");
		return DM_RET_INVALID_PARAM_NAME;
    }

    /* prepare the array in case of partial name */
    pGetNameReq = (DM_GET_PARA_NAME_REQ_T *)DM_MALLOC(sizeof(DM_GET_PARA_NAME_REQ_T)*pSetParaAttrReq->ulParamCnt);

    if (pGetNameReq == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaAttributeInternal : memory allocated for pGetNameReq failed");
        return DM_RET_INTERNAL_ERROR;

    }

    osa_mem_set(pGetNameReq, 0, sizeof(DM_GET_PARA_NAME_REQ_T)*pSetParaAttrReq->ulParamCnt);

    for(i = 0; i < pSetParaAttrReq->ulParamCnt; i++)
    {
        /* check if valid parameter path */
        lId = DmGetIDFromPath(&(pDmHandler->hashNameID), pSetParaAttrReq->pParamAttrList[i].pucParamName);

        if(lId < 0)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaAttributeInternal : parameter%d %s invalid",i+1,pSetParaAttrReq->pParamAttrList[i].pucParamName);
			lRet = DM_RET_INVALID_ARGUMENTS;
            break;
        }

        /* check if the parameter exists in db */
        if (DM_FALSE == DbNodeExistCheck(pDmHandler, pSetParaAttrReq->pParamAttrList[i].pucParamName))
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaAttributeInternal : parameter%d %s not exists in db",i+1,pSetParaAttrReq->pParamAttrList[i].pucParamName);
			lRet = DM_RET_INVALID_PARAM_NAME;
            break;
        }

		/* check attribute */
        if (isObject(pDmHandler, lId))
        {
            if (pSetParaAttrReq->pParamAttrList[i].bOnlyCurParam==DM_TRUE)
            {
                if(DM_FALSE == DmAttrVaildCheck(pDmHandler,lId,&pSetParaAttrReq->pParamAttrList[i].pucAttrList[j]))
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaAttributeInternal : parameter%d %s attribute%d %s is invalid",i+1,pSetParaAttrReq->pParamAttrList[i].pucParamName,j+1,pSetParaAttrReq->pParamAttrList[i].pucAttrList[j].pAttrName);
                    lRet = DM_RET_INVALID_ARGUMENTS;
                    break;
                }
            }
            else
            {
                /* check all parameters under this object */
                pGetNameReq[i].TransactionId = pSetParaAttrReq->TransactionId;
                pGetNameReq[i].pucParamName = strdup(pSetParaAttrReq->pParamAttrList[i].pucParamName);
                pGetNameReq[i].bNextLevel = DM_FALSE;
                pGetNameReq[i].pNameList = NULL;
                pGetNameReq[i].pfnFree = NULL;

                DmGetParaNameInternal(&(pGetNameReq[i]));

                for(k = 0; k < pGetNameReq[i].pNameList->udwCnt; k++)
                {
                    INT32 lChildId = DmGetIDFromPath(&(pDmHandler->hashNameID), pGetNameReq[i].pNameList->pucParamName[k].pucName);

                    if(lChildId < 0)
                    {
                        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaAttributeInternal : parameter%d %s child%d %s invalid",i+1,pSetParaAttrReq->pParamAttrList[i].pucParamName,k+1,pGetNameReq[i].pNameList->pucParamName[k].pucName);
            			lRet = DM_RET_INVALID_ARGUMENTS;
                        break;
                    }

                    for(j = 0; j < pSetParaAttrReq->pParamAttrList[i].ulAttrCnt; j++)
                    {
                        if(DM_FALSE == DmAttrVaildCheck(pDmHandler,lChildId,&pSetParaAttrReq->pParamAttrList[i].pucAttrList[j]))
                        {
                            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaAttributeInternal : parameter%d %s  child%d %s attribute%d %s is invalid",i+1,pSetParaAttrReq->pParamAttrList[i].pucParamName,k+1,pGetNameReq[i].pNameList->pucParamName[k].pucName,j+1,pSetParaAttrReq->pParamAttrList[i].pucAttrList[j].pAttrName);
                            lRet = DM_RET_INVALID_ARGUMENTS;
                            break;
                        }
                    }
                }
            }

            if (lRet == DM_RET_INVALID_ARGUMENTS)
            {
                break;
            }
        }
        else
        {
            if(DM_FALSE == DmAttrVaildCheck(pDmHandler,lId,&pSetParaAttrReq->pParamAttrList[i].pucAttrList[j]))
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaAttributeInternal : parameter%d %s attribute%d %s is invalid",i+1,pSetParaAttrReq->pParamAttrList[i].pucParamName,j+1,pSetParaAttrReq->pParamAttrList[i].pucAttrList[j].pAttrName);
                lRet = DM_RET_INVALID_ARGUMENTS;
                break;
            }
        }

        if (lRet != DM_RET_SUCCESS)
        {
            break;
        }

		DmCheckTransferNeed(pDmHandler,lId);
    }

	if(DM_RET_SUCCESS != lRet)
	{
        /* release pGetNameReq array */
        for(k = 0; k < pSetParaAttrReq->ulParamCnt; k++)
        {
            if (pGetNameReq && pGetNameReq[k].pfnFree)
            {
                pGetNameReq[k].pfnFree(&(pGetNameReq[k]));
                DM_FREE(pGetNameReq[k].pucParamName);
            }
        }
        DM_FREE(pGetNameReq);
		return lRet;
	}

    for(i = 0; i < pSetParaAttrReq->ulParamCnt; i++)
    {
        lId = DmGetIDFromPath(&(pDmHandler->hashNameID), pSetParaAttrReq->pParamAttrList[i].pucParamName);

        DM_HANDLER_PARA_T *pPara = (DM_HANDLER_PARA_T*)DM_MALLOC(sizeof(DM_HANDLER_PARA_T));

		if (pPara == NULL)
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaAttributeInternal : pPara = NULL");
			lRet = DM_RET_INTERNAL_ERROR;
            break;
		}

        if (isObject(pDmHandler, lId) && pSetParaAttrReq->pParamAttrList[i].bOnlyCurParam==DM_FALSE)
        {
            /* if the name is partial path, if so we should set all parameters under it */
            for(k = 0; pGetNameReq && pGetNameReq[i].pNameList && (k < pGetNameReq[i].pNameList->udwCnt); k++)
            {
                osa_mem_set(pPara,0,sizeof(DM_HANDLER_PARA_T));

                pPara->TransactionId = pSetParaAttrReq->TransactionId;
                pPara->ulParaId   = lId;
                pPara->pDbHandler = pDmHandler;
                pPara->pucName    = pGetNameReq[i].pNameList->pucParamName[k].pucName;
        		pPara->ulCnt = pSetParaAttrReq->pParamAttrList[i].ulAttrCnt;
                pPara->pValue   = pSetParaAttrReq->pParamAttrList[i].pucAttrList;
        		pPara->bCInterface = DM_FALSE;
                pPara->bAttribute = DM_TRUE;
                pPara->valueSize = sizeof(DM_ATTR_NODE_T)*(pSetParaAttrReq->pParamAttrList[i].ulAttrCnt);

                if(pDmHandler->pPattern[lId].pNodeHandler->pSetAttrBefHandler != NULL)
                {
                    if(!pDmHandler->pPattern[lId].pNodeHandler->pSetAttrBefHandler(pPara))
                    {
                        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaAttributeInternal : pPara %s pSetAttrBefHandler failed",pPara->pucName);
                        lRet = DM_RET_INVALID_ARGUMENTS;
                        break;
                    }
                }

        		BOOL8 bRet = DM_FALSE;
                
                if(pDmHandler->pPattern[lId].pNodeHandler->pSetAttrCurHandler != NULL)
                {
                	bRet = pDmHandler->pPattern[lId].pNodeHandler->pSetAttrCurHandler(pPara);
                    if(DM_FALSE == bRet)
                    {
                        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaAttributeInternal : pPara %s pSetAttrBefHandler failed",pPara->pucName);
        				lRet = DM_RET_INVALID_ARGUMENTS;
                    }
                }
                else
                {
                }

        		if(DM_TRUE == bRet)
        		{
        	        if(pDmHandler->pPattern[lId].pNodeHandler->pSetAttrAftHandler!= NULL)
        	        {
        	            pDmHandler->pPattern[lId].pNodeHandler->pSetAttrAftHandler(pPara);
        	        }
        		}
            }
        }
        else
        {
            /* item */
            osa_mem_set(pPara,0,sizeof(DM_HANDLER_PARA_T));

            pPara->TransactionId = pSetParaAttrReq->TransactionId;
            pPara->ulParaId   = lId;
            pPara->pDbHandler = pDmHandler;
            pPara->pucName    = pSetParaAttrReq->pParamAttrList[i].pucParamName;
    		pPara->ulCnt = pSetParaAttrReq->pParamAttrList[i].ulAttrCnt;
            pPara->pValue   = pSetParaAttrReq->pParamAttrList[i].pucAttrList;
    		pPara->bCInterface = DM_FALSE;
            pPara->bAttribute = DM_TRUE;
            pPara->valueSize = sizeof(DM_ATTR_NODE_T)*(pSetParaAttrReq->pParamAttrList[i].ulAttrCnt);

            if(pDmHandler->pPattern[lId].pNodeHandler->pSetAttrBefHandler != NULL)
            {
                if(!pDmHandler->pPattern[lId].pNodeHandler->pSetAttrBefHandler(pPara))
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaAttributeInternal : pPara %s pSetAttrBefHandler failed",pPara->pucName);
                    DM_FREE(pPara);
                    lRet = DM_RET_INVALID_ARGUMENTS;
                }
            }

    		BOOL8 bRet = DM_FALSE;

            if(pDmHandler->pPattern[lId].pNodeHandler->pSetAttrCurHandler != NULL)
            {
            	bRet = pDmHandler->pPattern[lId].pNodeHandler->pSetAttrCurHandler(pPara);
                if(DM_FALSE == bRet)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmSetParaAttributeInternal : pPara %s pSetAttrBefHandler failed",pPara->pucName);
    				DM_FREE(pPara);
    				lRet = DM_RET_INVALID_ARGUMENTS;
                }
            }
            else
            {
            }

    		if(DM_TRUE == bRet)
    		{
    	        if(pDmHandler->pPattern[lId].pNodeHandler->pSetAttrAftHandler!= NULL)
    	        {
    	            pDmHandler->pPattern[lId].pNodeHandler->pSetAttrAftHandler(pPara);
    	        }
    		}
        }

        DM_FREE(pPara);
        
        if (lRet != DM_RET_SUCCESS)
        {
            break;
        }
    }

    /* release pGetNameReq array */
    for(k = 0; k < pSetParaAttrReq->ulParamCnt; k++)
	{
        if (pGetNameReq && pGetNameReq[k].pfnFree)
        {
            pGetNameReq[k].pfnFree(&(pGetNameReq[k]));
            DM_FREE(pGetNameReq[k].pucParamName);
        }
	}
	DM_FREE(pGetNameReq);

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmSetParaAttributeInternal");

	return DM_RET_SUCCESS;
}


INT32   DmGetParaAttributeInternal(DM_GET_PARA_ATTR_REQ_T    *pGetParaAttrReq)
{
	INT32 lRet = DM_RET_SUCCESS;
    DM_HANDLER_T   *pDmHandler = NULL;
    INT32           lId      = -1;
    UINT32           i        = 0;
    UINT32           j        = 0;
    UINT32           k        = 0;
    DM_GET_PARA_NAME_REQ_T *pGetNameReq = NULL;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmGetParaAttributeInternal");

    if(NULL == pGetParaAttrReq || 
        NULL == pGetParaAttrReq->pNameList ||
		0 == pGetParaAttrReq->pNameList->udwCnt ||
		NULL == pGetParaAttrReq->pNameList->pucParamName ||
		NULL == pGetParaAttrReq->pNameList->pucParamName[0].pucName )
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmGetParaAttributeInternal : null parameters");
        return DM_RET_INVALID_ARGUMENTS;
    }

    /*get the dm handler*/
    pDmHandler = GetDmHandlerByName(pGetParaAttrReq->pNameList->pucParamName[0].pucName);

    if(NULL == pDmHandler)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmGetParaAttributeInternal : failed to get dm-handler");
		return DM_RET_INVALID_PARAM_NAME;
    }

    pGetParaAttrReq->pfnFree = (MEMORY_FREE)DmSrFreeGetParaAttributeReq;

    for(i = 0; i < pGetParaAttrReq->pNameList->udwCnt; i++)
    {
        /* check if valid parameter path */
        lId = DmGetIDFromPath(&(pDmHandler->hashNameID), pGetParaAttrReq->pNameList->pucParamName[i].pucName);

        if(lId < 0)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmGetParaAttributeInternal : parameter%d %s invalid",i+1,pGetParaAttrReq->pNameList->pucParamName[i].pucName);
			lRet = DM_RET_INVALID_ARGUMENTS;
            break;
        }

        if(!DmCheckRight(pGetParaAttrReq->TransactionId, pDmHandler, RIGHT_R, lId))
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ %s not readable in DmGetParaAttributeInternal",pGetParaAttrReq->pNameList->pucParamName[i].pucName);
            return DM_RET_REQUEST_DENIED;
        }
    }

    /* prepare the array in case of partial name */
    pGetNameReq = (DM_GET_PARA_NAME_REQ_T *)DM_MALLOC(sizeof(DM_GET_PARA_NAME_REQ_T)*pGetParaAttrReq->pNameList->udwCnt);

    if (pGetNameReq == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmGetParaAttributeInternal : memory allocated for pGetNameReq failed");
        return DM_RET_INTERNAL_ERROR;

    }

    osa_mem_set(pGetNameReq, 0, sizeof(DM_GET_PARA_NAME_REQ_T)*pGetParaAttrReq->pNameList->udwCnt);

    pGetParaAttrReq->ulParamCnt = 0;

    for(i = 0; i < pGetParaAttrReq->pNameList->udwCnt; i++)
    {
        /* check if valid parameter path */
        lId = DmGetIDFromPath(&(pDmHandler->hashNameID), pGetParaAttrReq->pNameList->pucParamName[i].pucName);
        if(lId < 0)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmGetParaAttributeInternal : parameter%d %s invalid",i+1,pGetParaAttrReq->pNameList->pucParamName[i].pucName);
			lRet = DM_RET_INVALID_ARGUMENTS;
            break;
        }

        /* check if the parameter exists in db */
        if (DM_FALSE == DbNodeExistCheck(pDmHandler, pGetParaAttrReq->pNameList->pucParamName[i].pucName))
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmGetParaAttributeInternal : parameter%d %s not exists in db",i+1,pGetParaAttrReq->pNameList->pucParamName[i].pucName);
			lRet = DM_RET_INVALID_PARAM_NAME;
            break;
        }

		/* check attribute */
        if (isObject(pDmHandler, lId))
        {
            /* check all parameters under this object */
            pGetNameReq[i].TransactionId = pGetParaAttrReq->TransactionId;
            pGetNameReq[i].pucParamName = strdup(pGetParaAttrReq->pNameList->pucParamName[i].pucName);
            pGetNameReq[i].bNextLevel = DM_FALSE;
            pGetNameReq[i].pNameList = NULL;
            pGetNameReq[i].pfnFree = NULL;
        
            lRet = DmGetParaNameInternal(&(pGetNameReq[i]));

            if (lRet == DM_RET_SUCCESS)
            {
                for(k = 0; k < pGetNameReq[i].pNameList->udwCnt; k++)
                {

                    if(pGetNameReq[i].pNameList->pucParamName[k].pucName[strlen((char*)pGetNameReq[i].pNameList->pucParamName[k].pucName)-1]
                          != '.')
                    {
                        INT32 lIndex = DmGetIDFromPath(&(pDmHandler->hashNameID), pGetNameReq[i].pNameList->pucParamName[k].pucName);
                        
                        /*check right*/
                        if(!DmCheckRight(pGetNameReq->TransactionId, pDmHandler, RIGHT_R, lIndex))
                        {
                            continue;
                        }

                        pGetParaAttrReq->ulParamCnt++;
                    }
                }
            }
        }
        else
        {
            pGetParaAttrReq->ulParamCnt++;
        }
    }

	if(DM_RET_SUCCESS != lRet)
	{
        /* release pGetNameReq array */
        for(k = 0; k < pGetParaAttrReq->pNameList->udwCnt; k++)
        {
            if (pGetNameReq && pGetNameReq[k].pfnFree)
            {
                pGetNameReq[k].pfnFree(&(pGetNameReq[k]));
                DM_FREE(pGetNameReq[k].pucParamName);
            }
        }
        DM_FREE(pGetNameReq);
		return lRet;
	}

    if(0 == pGetParaAttrReq->ulParamCnt)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmGetParaAttributeInternal : no parameter found");
        pGetParaAttrReq->pParamAttrList = NULL;

        /* release pGetNameReq array */
        for(k = 0; k < pGetParaAttrReq->pNameList->udwCnt; k++)
        {
            if (pGetNameReq && pGetNameReq[k].pfnFree)
            {
                pGetNameReq[k].pfnFree(&(pGetNameReq[k]));
                DM_FREE(pGetNameReq[k].pucParamName);
            }
        }
        DM_FREE(pGetNameReq);
        return DM_RET_SUCCESS;
    }

    pGetParaAttrReq->pParamAttrList = (DM_PARAM_ATTR_T *)DM_MALLOC(sizeof(DM_PARAM_ATTR_T)*pGetParaAttrReq->ulParamCnt);

    if(NULL == pGetParaAttrReq->pParamAttrList)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmGetParaAttributeInternal : memory allocated for pGetParaAttrReq->pParamAttrList failed");

        /* release pGetNameReq array */
        for(k = 0; k < pGetParaAttrReq->pNameList->udwCnt; k++)
        {
            if (pGetNameReq && pGetNameReq[k].pfnFree)
            {
                pGetNameReq[k].pfnFree(&(pGetNameReq[k]));
                DM_FREE(pGetNameReq[k].pucParamName);
            }
        }
        DM_FREE(pGetNameReq);
		return DM_RET_INTERNAL_ERROR;
    }

    osa_mem_set(pGetParaAttrReq->pParamAttrList,0,sizeof(DM_PARAM_ATTR_T)*pGetParaAttrReq->ulParamCnt);

    /* get attribute value */
    DM_ATTR_NODE_T *pAttrValue = (DM_ATTR_NODE_T *)DM_MALLOC(sizeof(DM_ATTR_NODE_T)*MAX_ATTR_NUMBER);
    if(NULL == pAttrValue)
    {
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmGetParaAttributeInternal : memory allocated for pucAttrValue failed");

        /* release pGetNameReq array */
        for(k = 0; k < pGetParaAttrReq->pNameList->udwCnt; k++)
        {
            if (pGetNameReq && pGetNameReq[k].pfnFree)
            {
                pGetNameReq[k].pfnFree(&(pGetNameReq[k]));
                DM_FREE(pGetNameReq[k].pucParamName);
            }
        }
        DM_FREE(pGetNameReq);
        DM_FREE(pGetParaAttrReq->pParamAttrList);
		return DM_RET_INTERNAL_ERROR;
    }

    osa_mem_set(pAttrValue,0,sizeof(DM_ATTR_NODE_T)*MAX_ATTR_NUMBER);

	if(NULL == pDmHandler->xpathCtx)
	{
		pDmHandler->xpathCtx = xmlXPathNewContext(pDmHandler->pXmlDocument);
		if(NULL == pDmHandler->xpathCtx)
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmGetParaAttributeInternal : failed to get new xpath context");

            /* release pGetNameReq array */
            for(k = 0; k < pGetParaAttrReq->pNameList->udwCnt; k++)
            {
                if (pGetNameReq && pGetNameReq[k].pfnFree)
                {
                    pGetNameReq[k].pfnFree(&(pGetNameReq[k]));
                    DM_FREE(pGetNameReq[k].pucParamName);
                }
            }
            DM_FREE(pGetNameReq);
            DM_FREE(pGetParaAttrReq->pParamAttrList);
            DM_FREE(pAttrValue);
            return DM_RET_INTERNAL_ERROR;
		}
	}

    DM_HANDLER_PARA_T *pPara = (DM_HANDLER_PARA_T*)DM_MALLOC(sizeof(DM_HANDLER_PARA_T));
    
    if (pPara == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmGetParaAttributeInternal :  memory allocated for pPara failed");
        return DM_RET_INTERNAL_ERROR;
    }
    
    j = 0;
    for(i = 0; i < pGetParaAttrReq->pNameList->udwCnt; i++)
    {
        lId = DmGetIDFromPath(&(pDmHandler->hashNameID), pGetParaAttrReq->pNameList->pucParamName[i].pucName);

        if(lId < 0)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmGetParaAttributeInternal : parameter%d %s invalid",i+1,pGetParaAttrReq->pNameList->pucParamName[i].pucName);
            lRet = DM_RET_INVALID_ARGUMENTS;
            break;
        }

        if (isObject(pDmHandler, lId))
        {
            if (pGetNameReq[i].pNameList==NULL)
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmGetParaAttributeInternal : parameter%d %s namelist is null",i+1,pGetParaAttrReq->pNameList->pucParamName[i].pucName);
                continue;
            }

            /* if the name is partial path, if so we should set all parameters under it */
            for(k = 0; k < pGetNameReq[i].pNameList->udwCnt; k++)
            {
                if(pGetNameReq[i].pNameList->pucParamName[k].pucName[strlen((char*)pGetNameReq[i].pNameList->pucParamName[k].pucName)-1] != '.')
                {
                    INT32 lIndex = DmGetIDFromPath(&(pDmHandler->hashNameID), pGetNameReq[i].pNameList->pucParamName[k].pucName);

                    osa_mem_set(pPara,0,sizeof(DM_HANDLER_PARA_T));
                    osa_mem_set(pAttrValue,0,sizeof(DM_ATTR_NODE_T)*MAX_ATTR_NUMBER);

                    pPara->TransactionId = pGetParaAttrReq->TransactionId;
                    pPara->ulParaId   = lIndex;
                    pPara->pDbHandler = pDmHandler;
                    pPara->pucName    = pGetNameReq[i].pNameList->pucParamName[k].pucName;
            		pPara->ulCnt = MAX_ATTR_NUMBER;
                    pPara->pValue   = pAttrValue;
            		pPara->bCInterface = pGetParaAttrReq->bCValue;
                    pPara->bAttribute = DM_TRUE;
                    pPara->valueSize = sizeof(DM_ATTR_NODE_T)*MAX_ATTR_NUMBER;

                    if(pDmHandler->pPattern[lId].pNodeHandler->pGetAttrBefHandler != NULL)
                    {
                        if(!pDmHandler->pPattern[lId].pNodeHandler->pGetAttrBefHandler(pPara))
                        {
                            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmGetParaAttributeInternal : pPara %s pGetAttrBefHandler failed",pPara->pucName);
                            lRet = DM_RET_INVALID_ARGUMENTS;
                            break;
                        }
                    }

            		BOOL8 bRet = DM_FALSE;
                    
                    if(pDmHandler->pPattern[lId].pNodeHandler->pGetAttrCurHandler != NULL)
                    {
                    	bRet = pDmHandler->pPattern[lId].pNodeHandler->pGetAttrCurHandler(pPara);
                        if(DM_FALSE == bRet)
                        {
                            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmGetParaAttributeInternal : pPara %s pGetAttrCurHandler failed",pPara->pucName);
            				lRet = DM_RET_INVALID_ARGUMENTS;
                        }
                    }
                    else
                    {
                    }

            		if(DM_TRUE == bRet)
            		{
            	        if(pDmHandler->pPattern[lId].pNodeHandler->pGetAttrAftHandler!= NULL)
            	        {
            	            pDmHandler->pPattern[lId].pNodeHandler->pGetAttrAftHandler(pPara);
            	        }
            		}

                    pGetParaAttrReq->pParamAttrList[j].pucParamName = strdup(pGetNameReq[i].pNameList->pucParamName[k].pucName);
                    pGetParaAttrReq->pParamAttrList[j].ulAttrCnt = pPara->ulCnt;
                    pGetParaAttrReq->pParamAttrList[j].pucAttrList = (DM_ATTR_NODE_T *)DM_MALLOC(sizeof(DM_ATTR_NODE_T)*pPara->ulCnt);
                    osa_mem_copy(pGetParaAttrReq->pParamAttrList[j].pucAttrList,pAttrValue,sizeof(DM_ATTR_NODE_T)*pPara->ulCnt);
                    j++;
                }
            }
        }
        else
        {
            /* item */
            osa_mem_set(pPara,0,sizeof(DM_HANDLER_PARA_T));
            osa_mem_set(pAttrValue,0,sizeof(DM_ATTR_NODE_T)*MAX_ATTR_NUMBER);

            pPara->TransactionId = pGetParaAttrReq->TransactionId;
            pPara->ulParaId   = lId;
            pPara->pDbHandler = pDmHandler;
            pPara->pucName    = pGetParaAttrReq->pParamAttrList[i].pucParamName;
    		pPara->ulCnt = MAX_ATTR_NUMBER;
            pPara->pValue   = pAttrValue;
    		pPara->bCInterface = pGetParaAttrReq->bCValue;
            pPara->bAttribute = DM_TRUE;
            pPara->valueSize = sizeof(DM_ATTR_NODE_T)*MAX_ATTR_NUMBER;

            if(pDmHandler->pPattern[lId].pNodeHandler->pGetAttrBefHandler != NULL)
            {
                if(!pDmHandler->pPattern[lId].pNodeHandler->pGetAttrBefHandler(pPara))
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmGetParaAttributeInternal : pPara %s pGetAttrBefHandler failed",pPara->pucName);
                    DM_FREE(pPara);
                    lRet = DM_RET_INVALID_ARGUMENTS;
                }
            }

    		BOOL8 bRet = DM_FALSE;

            if(pDmHandler->pPattern[lId].pNodeHandler->pGetAttrCurHandler != NULL)
            {
            	bRet = pDmHandler->pPattern[lId].pNodeHandler->pGetAttrCurHandler(pPara);
                if(DM_FALSE == bRet)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmGetParaAttributeInternal : pPara %s pGetAttrBefHandler failed",pPara->pucName);
    				DM_FREE(pPara);
    				lRet = DM_RET_INVALID_ARGUMENTS;
                }
            }
            else
            {
            }

    		if(DM_TRUE == bRet)
    		{
    	        if(pDmHandler->pPattern[lId].pNodeHandler->pGetAttrAftHandler!= NULL)
    	        {
    	            pDmHandler->pPattern[lId].pNodeHandler->pGetAttrAftHandler(pPara);
    	        }
    		}

            pGetParaAttrReq->pParamAttrList[j].pucParamName = strdup(pGetParaAttrReq->pParamAttrList[i].pucParamName);
            if(DM_TRUE == bRet && pPara->ulCnt>0)
            {
                pGetParaAttrReq->pParamAttrList[j].ulAttrCnt = pPara->ulCnt;
                pGetParaAttrReq->pParamAttrList[j].pucAttrList = (DM_ATTR_NODE_T *)DM_MALLOC(sizeof(DM_ATTR_NODE_T)*pPara->ulCnt);
                osa_mem_copy(pGetParaAttrReq->pParamAttrList[j].pucAttrList,pAttrValue,sizeof(DM_ATTR_NODE_T)*pPara->ulCnt);
            }
            else
            {
                pGetParaAttrReq->pParamAttrList[j].ulAttrCnt = 0;
                pGetParaAttrReq->pParamAttrList[j].pucAttrList = NULL;
            }
            j++;
        }

        if (lRet != DM_RET_SUCCESS)
        {
            break;
        }
    }

    /* release pGetNameReq array */
    for(k = 0; k < pGetParaAttrReq->pNameList->udwCnt; k++)
	{
        if (pGetNameReq && pGetNameReq[k].pfnFree)
        {
            pGetNameReq[k].pfnFree(&(pGetNameReq[k]));
            DM_FREE(pGetNameReq[k].pucParamName);
        }
	}
	DM_FREE(pGetNameReq);
    DM_FREE(pAttrValue);
    DM_FREE(pPara);

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmGetParaAttributeInternal");

	return DM_RET_SUCCESS;

}


/**
 * Module A can use this API to add an object to DB file. The result will return synchronously.
 * @param pAddObjectReq the object instance to be added
 * @return 0: success   other: failure
 */
INT32   DmAddObjectInternal(DM_ADD_OBJECT_REQ_T *pAddObjectReq)
{

    DM_HANDLER_T   *pDmHandler = NULL;
    INT32           lId      = -1;
    
    UINT8 aucValue[MAX_INSTANCE_NUMBER_SIZE] = {0};
    
    if(NULL == pAddObjectReq || NULL == pAddObjectReq->pucObjName)
    {
        DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ DmAddObjectInternal parameter error");
        return -1;
    }
	else
	{
	    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmAddObjectInternal %s", pAddObjectReq->pucObjName);
	}

    /*get the dm handler*/
    pDmHandler = GetDmHandlerByName(pAddObjectReq->pucObjName);

    if(NULL == pDmHandler)
    {
        return DM_RET_INVALID_PARAM_NAME;
    }

    lId = DmGetIDFromPath(&(pDmHandler->hashNameID), pAddObjectReq->pucObjName);
    if(lId < 0)
    {
        return DM_RET_INVALID_PARAM_NAME;
    }

    /*check right*/
    if(!DmCheckRight(pAddObjectReq->TransactionId, pDmHandler, RIGHT_W, lId))
    {
        return DM_RET_REQUEST_DENIED;
    }

	DmCheckTransferNeed(pDmHandler,lId);

    if(DbGetObjectNum(pDmHandler,pAddObjectReq->pucObjName) >= atoi((char*)pDmHandler->pPattern[lId].pValueRange))
    {
        return DM_RET_RESOURCE_EXCEEDED;
    }


    DM_HANDLER_PARA_T *pPara = (DM_HANDLER_PARA_T*)calloc(1, sizeof(DM_HANDLER_PARA_T));

	if (pPara == NULL)
	{
        DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ DmAddObjectInternal : pPara = NULL");
		return DM_RET_INTERNAL_ERROR;

	}

    osa_mem_set(pPara,0,sizeof(DM_HANDLER_PARA_T));

    pPara->TransactionId = pAddObjectReq->TransactionId;
    pPara->ulParaId   = lId;
    pPara->pDbHandler = pDmHandler;
    pPara->pucName    = pAddObjectReq->pucObjName;
	pPara->ulCnt = 1;
    pPara->pValue   = aucValue;
	pPara->bCInterface = DM_FALSE;
    pPara->valueSize = sizeof(aucValue);


    if(DM_RET_SUCCESS == DmAddObject(pPara))
    {
        UINT8 *pucInstanceNum = (UINT8*)pPara->pValue;
        pAddObjectReq->udwInstanceNum = atoi((char*)(pucInstanceNum+1));
        DM_FREE(pPara);
		DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmAddObjectInternal");			
        return DM_RET_SUCCESS;
    }
    else
    {
        DM_FREE(pPara);
		DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmAddObjectInternal");			
        return DM_RET_INTERNAL_ERROR;
    }
}


/**
 * Module A use this API to delete an object from DB file. The result will return synchronously.
 * @param pDeleteObjectReq the object instance to be deleted
 * @return 0: success   other: failure
 */
INT32   DmDeleteObjectInternal(DM_DELETE_OBJECT_REQ_T *pDeleteObjectReq)
{

    DM_HANDLER_T   *pDmHandler = NULL;
    INT32           lId      = -1;
    
    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmDeleteObjectInternal");
    if(NULL == pDeleteObjectReq)
    {
        return DM_RET_INTERNAL_ERROR;
    }

    /*get the dm handler*/
    pDmHandler = GetDmHandlerByName(pDeleteObjectReq->pucObjName);

    if(NULL == pDmHandler)
    {
        return DM_RET_INVALID_PARAM_NAME;
    }

    lId = DmGetIDFromPath(&(pDmHandler->hashNameID), pDeleteObjectReq->pucObjName);
    if(lId < 0)
    {
        return DM_RET_INVALID_PARAM_NAME;
    }

    /*check right*/
    if(!DmCheckRight(pDeleteObjectReq->TransactionId, pDmHandler, RIGHT_W, lId))
    {
        return DM_RET_REQUEST_DENIED;
    }

	DmCheckTransferNeed(pDmHandler,lId);

    DM_NODE_HANDLER_T *pNodeHandler = pDmHandler->pPattern[lId].pNodeHandler;

    DM_HANDLER_PARA_T DmHandlerPara = {0};

    osa_mem_set(&DmHandlerPara,0,sizeof(DM_HANDLER_PARA_T));

    DmHandlerPara.TransactionId = pDeleteObjectReq->TransactionId;
    DmHandlerPara.ulParaId    = lId;
    DmHandlerPara.pDbHandler = pDmHandler;
    DmHandlerPara.pucName     = pDeleteObjectReq->pucObjName;
    DmHandlerPara.pValue = NULL;
	DmHandlerPara.bCInterface = DM_FALSE;
    DmHandlerPara.valueSize = 0;

    
    if(pNodeHandler->pDelBefHandler != NULL)
    {
        if(!pNodeHandler->pDelBefHandler(&DmHandlerPara))
        {
            return DM_RET_INTERNAL_ERROR;
        }
    }
    
    if(pNodeHandler->pDelCurHandler != NULL)
    {
        if(!pNodeHandler->pDelCurHandler(&DmHandlerPara))
        {
            return DM_RET_INVALID_PARAM_NAME;
        }
    }
    else
    {
        return DM_RET_INTERNAL_ERROR;
    }
    
    if(pNodeHandler->pDelAftHandler!= NULL)
    {
        if(!pNodeHandler->pDelAftHandler(&DmHandlerPara))
        {
            return DM_RET_INTERNAL_ERROR;
        }
    }
	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmDeleteObjectInternal");	
	return DM_RET_SUCCESS;
}

INT32 DmSetParaAttributeReq(DM_SET_PARA_ATTR_REQ_T *pSetParaAttrReq)
{
    DM_RET_E lRet = DM_RET_SUCCESS;

    if (pSetParaAttrReq==NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pSetParaAttrReq is null in DmSetParaAttributeReq(%s)",g_DmCtrl.entity);
        return lRet;
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmSetParaAttributeReq(%s) transactionId=%d",g_DmCtrl.entity,pSetParaAttrReq->TransactionId);

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        UINT32 cnt = 0;
        UINT32 iCnt = 0;
        UINT32 msgBodyLength = 0;
        INT8 *pMsgBody = NULL;

        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;

        /* client things */
        /* compute the size of msg-body */
        msgBodyLength += sizeof(DM_SET_PARA_ATTR_REQ_T);
        msgBodyLength += sizeof(DM_PARAM_ATTR_T)*(pSetParaAttrReq->ulParamCnt);

        for (cnt=0; cnt<pSetParaAttrReq->ulParamCnt; cnt++)
        {
            msgBodyLength += strlen(pSetParaAttrReq->pParamAttrList[cnt].pucParamName)+1;
            msgBodyLength += sizeof(DM_ATTR_NODE_T)*(pSetParaAttrReq->pParamAttrList[cnt].ulAttrCnt);

            for (iCnt=0; iCnt<pSetParaAttrReq->pParamAttrList[cnt].ulAttrCnt; iCnt++)
            {
                msgBodyLength += strlen(pSetParaAttrReq->pParamAttrList[cnt].pucAttrList[iCnt].pAttrName)+1;
                msgBodyLength += pSetParaAttrReq->pParamAttrList[cnt].pucAttrList[iCnt].ulAttrValueSize;
            }
        }

        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmSetParaAttributeReq(%s) transactionId=%d",g_DmCtrl.entity,pSetParaAttrReq->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        pSockMsgSnd->msg_id = DM_MSG_CODE_SetParaAttributeReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->word_data = pSetParaAttrReq->TransactionId;/* keep trans-id */
        pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = msgBodyLength;

        /* fill the msg-body */
        pMsgBody = (INT8*)(pSockMsgSnd+1);

        osa_mem_copy(pMsgBody,pSetParaAttrReq, sizeof(DM_SET_PARA_ATTR_REQ_T));
        pMsgBody += sizeof(DM_SET_PARA_ATTR_REQ_T);

        osa_mem_copy(pMsgBody,pSetParaAttrReq->pParamAttrList,sizeof(DM_PARAM_ATTR_T)*(pSetParaAttrReq->ulParamCnt));
        pMsgBody += sizeof(DM_PARAM_ATTR_T)*(pSetParaAttrReq->ulParamCnt);

        for (cnt=0; cnt<pSetParaAttrReq->ulParamCnt; cnt++)
        {
            strcpy(pMsgBody, pSetParaAttrReq->pParamAttrList[cnt].pucParamName);
            pMsgBody += strlen(pSetParaAttrReq->pParamAttrList[cnt].pucParamName) + 1;

            osa_mem_copy(pMsgBody,pSetParaAttrReq->pParamAttrList[cnt].pucAttrList,sizeof(DM_ATTR_NODE_T)*(pSetParaAttrReq->pParamAttrList[cnt].ulAttrCnt));
            pMsgBody += sizeof(DM_ATTR_NODE_T)*(pSetParaAttrReq->pParamAttrList[cnt].ulAttrCnt);

            for (iCnt=0; iCnt<pSetParaAttrReq->pParamAttrList[cnt].ulAttrCnt; iCnt++)
            {
                strcpy(pMsgBody, pSetParaAttrReq->pParamAttrList[cnt].pucAttrList[iCnt].pAttrName);
                pMsgBody += strlen(pSetParaAttrReq->pParamAttrList[cnt].pucAttrList[iCnt].pAttrName)+1;

                osa_mem_copy(pMsgBody,pSetParaAttrReq->pParamAttrList[cnt].pucAttrList[iCnt].pAttrValue,pSetParaAttrReq->pParamAttrList[cnt].pucAttrList[iCnt].ulAttrValueSize);
                pMsgBody += pSetParaAttrReq->pParamAttrList[cnt].pucAttrList[iCnt].ulAttrValueSize;
            }
        }

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmSetParaAttributeReq(%s) transactionId=%d",g_DmCtrl.entity,pSetParaAttrReq->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_SetParaAttributeCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_SetParaAttributeCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_SetParaAttributeCnf)
        {
            lRet = pSockMsgRcv->word_data;
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ lRet=%d in DmSetParaAttributeReq(%s) transactionId=%d",lRet,g_DmCtrl.entity,pSetParaAttrReq->TransactionId);
            
            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmSetParaAttributeReq(%s) transactionId=%d",g_DmCtrl.entity,pSetParaAttrReq->TransactionId);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmSetParaAttributeReq(%s) transactionId=%d",g_DmCtrl.entity,pSetParaAttrReq->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);

    }
    else
    {
        DM_HANDLER_T *pDmHandler = GetDmHandlerByName(pSetParaAttrReq->pParamAttrList[0].pucParamName);

        if (pDmHandler == NULL)
        {
            lRet = DM_RET_INVALID_PARAM_NAME;
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to find dmhandler, leave DmSetParaAttributeReq(%s) lRet=%d transactionId=%d",g_DmCtrl.entity,lRet,pSetParaAttrReq->TransactionId);
            return lRet;
        }

        osa_thread_mutex_lock(&(pDmHandler->mutexXml));

        lRet = DmSetParaAttributeInternal(pSetParaAttrReq);

        DmUpdateTransactionInfo(pDmHandler, pSetParaAttrReq->TransactionId);

        osa_thread_mutex_unlock(&(pDmHandler->mutexXml));
    }
    
    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmSetParaAttributeReq(%s) lRet=%d transactionId=%d",g_DmCtrl.entity,lRet,pSetParaAttrReq->TransactionId);    

    return lRet;
}

INT32 DmGetParaAttributeReq(DM_GET_PARA_ATTR_REQ_T *pGetParaAttrReq)
{
    DM_RET_E lRet = DM_RET_SUCCESS;

    if (pGetParaAttrReq==NULL || pGetParaAttrReq->pNameList==NULL || pGetParaAttrReq->pNameList->udwCnt==0 || pGetParaAttrReq->pNameList->pucParamName==NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ null arguments in DmGetParaAttributeReq(%s)",g_DmCtrl.entity);
        return lRet;
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmGetParaAttributeReq(%s) transactionId=%d",g_DmCtrl.entity,pGetParaAttrReq->TransactionId);

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        UINT32 cnt = 0;
        UINT32 iCnt = 0;
        UINT32 msgBodyLength = 0;
        INT8 *pMsgBody = NULL;

        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;

        /* client things */
        /* compute the size of msg-body */
        msgBodyLength += sizeof(DM_GET_PARA_ATTR_REQ_T);
        msgBodyLength += sizeof(PARAMETER_NAME_LIST_T);
        msgBodyLength += sizeof(PARAMETER_NAME_T)*(pGetParaAttrReq->pNameList->udwCnt);

        for (cnt=0; cnt<pGetParaAttrReq->pNameList->udwCnt; cnt++)
        {
            msgBodyLength += strlen(pGetParaAttrReq->pNameList->pucParamName[cnt].pucName)+1;
        }

        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmGetParaAttributeReq(%s) transactionId=%d",g_DmCtrl.entity,pGetParaAttrReq->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        pSockMsgSnd->msg_id = DM_MSG_CODE_GetParaAttributeReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->word_data = pGetParaAttrReq->TransactionId;/* keep trans-id */
        pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = msgBodyLength;

        /* fill the msg-body */
        pMsgBody = (INT8*)(pSockMsgSnd+1);

        osa_mem_copy(pMsgBody,pGetParaAttrReq, sizeof(DM_GET_PARA_ATTR_REQ_T));
        pMsgBody += sizeof(DM_GET_PARA_ATTR_REQ_T);

        osa_mem_copy(pMsgBody,pGetParaAttrReq->pNameList, sizeof(PARAMETER_NAME_LIST_T));
        pMsgBody += sizeof(PARAMETER_NAME_LIST_T);

        osa_mem_copy(pMsgBody,pGetParaAttrReq->pNameList->pucParamName,sizeof(PARAMETER_NAME_T)*(pGetParaAttrReq->pNameList->udwCnt));
        pMsgBody += sizeof(PARAMETER_NAME_T)*(pGetParaAttrReq->pNameList->udwCnt);

        for (cnt=0; cnt<pGetParaAttrReq->pNameList->udwCnt; cnt++)
        {
            strcpy(pMsgBody, pGetParaAttrReq->pNameList->pucParamName[cnt].pucName);
            pMsgBody += strlen(pGetParaAttrReq->pNameList->pucParamName[cnt].pucName) + 1;
        }

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmGetParaAttributeReq(%s) transactionId=%d",g_DmCtrl.entity,pGetParaAttrReq->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_GetParaAttributeCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_GetParaAttributeCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_GetParaAttributeCnf)
        {
            lRet = pSockMsgRcv->word_data;
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ result=%d from db-server in DmGetParaAttributeReq(%s) transactionId=%d",lRet,g_DmCtrl.entity,pGetParaAttrReq->TransactionId);

            if (pSockMsgRcv->msgLength > 0)
            {
                /* copy the result */
                INT8 *pTmp = NULL;
                UINT32 *pParamCnt = NULL;
                pParamCnt = (UINT32 *)DM_MALLOC(pSockMsgRcv->msgLength);

                if (pParamCnt == NULL)
                {
                    lRet = DM_RET_INTERNAL_ERROR;
                    DM_FREE(pSockMsgRcv);
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmGetParaAttributeReq(%s) transactionId=%d",g_DmCtrl.entity,pGetParaAttrReq->TransactionId);
                    return lRet;
                }

                osa_mem_copy(pParamCnt, pSockMsgRcv+1, pSockMsgRcv->msgLength);

                pGetParaAttrReq->ulParamCnt = *pParamCnt;
                pGetParaAttrReq->pParamAttrList = (DM_PARAM_ATTR_T *)(pParamCnt+1);

                pTmp = (INT8 *)(pGetParaAttrReq->pParamAttrList + pGetParaAttrReq->ulParamCnt);

                for (cnt=0;cnt<pGetParaAttrReq->ulParamCnt;cnt++)
                {
                    pGetParaAttrReq->pParamAttrList[cnt].pucParamName = pTmp;
                    pTmp += strlen(pGetParaAttrReq->pParamAttrList[cnt].pucParamName)+1;

                    if (pGetParaAttrReq->pParamAttrList[cnt].ulAttrCnt>0)
                    {
                        pGetParaAttrReq->pParamAttrList[cnt].pucAttrList = (DM_ATTR_NODE_T *)pTmp;
                        pTmp = (INT8*)(pGetParaAttrReq->pParamAttrList[cnt].pucAttrList + pGetParaAttrReq->pParamAttrList[cnt].ulAttrCnt);
                    }

                    for (iCnt=0;iCnt<pGetParaAttrReq->pParamAttrList[cnt].ulAttrCnt;iCnt++)
                    {
                        pGetParaAttrReq->pParamAttrList[cnt].pucAttrList[iCnt].pAttrName = pTmp;
                        pTmp += strlen(pGetParaAttrReq->pParamAttrList[cnt].pucAttrList[iCnt].pAttrName)+1;

                        pGetParaAttrReq->pParamAttrList[cnt].pucAttrList[iCnt].pAttrValue = pTmp;
                        pTmp += pGetParaAttrReq->pParamAttrList[cnt].pucAttrList[iCnt].ulAttrValueSize;
                    }
                }
            }

            pGetParaAttrReq->pfnFree = (MEMORY_FREE)DmClFreeGetParaAttributeReq;

            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmGetParaAttributeReq(%s) transactionId=%d",g_DmCtrl.entity,pGetParaAttrReq->TransactionId);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmGetParaAttributeReq(%s) transactionId=%d",g_DmCtrl.entity,pGetParaAttrReq->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);

    }
    else
    {
        DM_HANDLER_T *pDmHandler = GetDmHandlerByName(pGetParaAttrReq->pNameList->pucParamName[0].pucName);

        if (pDmHandler == NULL)
        {
            lRet = DM_RET_INVALID_PARAM_NAME;
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to find dmhandler, leave DmGetParaAttributeReq(%s) lRet=%d transactionId=%d",g_DmCtrl.entity,lRet,pGetParaAttrReq->TransactionId);
            return lRet;
        }

        osa_thread_mutex_lock(&(pDmHandler->mutexXml));

        lRet = DmGetParaAttributeInternal(pGetParaAttrReq);

        DmUpdateTransactionInfo(pDmHandler, pGetParaAttrReq->TransactionId);

        osa_thread_mutex_unlock(&(pDmHandler->mutexXml));
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmGetParaAttributeReq(%s) lRet=%d transactionId=%d pGetParaAttrReq->ulParamCnt=%u",g_DmCtrl.entity,lRet,pGetParaAttrReq->TransactionId,pGetParaAttrReq->ulParamCnt);    

    return lRet;

}

void DmSrFreeGetParaAttributeReq(DM_GET_PARA_ATTR_REQ_T *pGetParaAttrReq)
{
    if (g_DmCtrl.bAsServer == DM_TRUE && pGetParaAttrReq != NULL && pGetParaAttrReq->pParamAttrList != NULL)
    {
        UINT32 cnt = 0;
        UINT32 iCnt = 0;

        for (cnt=0; cnt<pGetParaAttrReq->ulParamCnt; cnt++)
        {
            DM_FREE(pGetParaAttrReq->pParamAttrList[cnt].pucParamName);

            for (iCnt=0; iCnt<pGetParaAttrReq->pParamAttrList[cnt].ulAttrCnt; iCnt++)
            {
                DM_FREE(pGetParaAttrReq->pParamAttrList[cnt].pucAttrList[iCnt].pAttrName);
                DM_FREE(pGetParaAttrReq->pParamAttrList[cnt].pucAttrList[iCnt].pAttrValue);
            }

            DM_FREE(pGetParaAttrReq->pParamAttrList[cnt].pucAttrList);
        }
    }

    return;
}

void DmClFreeGetParaAttributeReq(DM_GET_PARA_ATTR_REQ_T *pGetParaAttrReq)
{
    if (g_DmCtrl.bAsServer == DM_FALSE && pGetParaAttrReq != NULL && pGetParaAttrReq->pParamAttrList != NULL)
    {
        UINT32 *pTmp = (UINT32 *)pGetParaAttrReq->pParamAttrList;
        pTmp--;
        DM_FREE(pTmp);
    }

    return;
}

INT32 DmFlushDBfiles(void)
{
    DM_RET_E lRet = DM_RET_SUCCESS;

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmFlushDBfiles(%s)",g_DmCtrl.entity);

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;

        /* client things */
        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T));

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmFlushDBfiles(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T));

        pSockMsgSnd->msg_id = DM_MSG_CODE_FlushDBfilesReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->word_data = 0;
        pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = 0;

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmFlushDBfiles(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_FlushDBfilesCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_FlushDBfilesCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_FlushDBfilesCnf)
        {
            lRet = pSockMsgRcv->word_data;
            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmFlushDBfiles(%s)",g_DmCtrl.entity);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmFlushDBfiles(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {
        DM_HANDLER_T *pDmHandler = NULL;
        DM_HASH_ENUM_T dmHashEnum;

        osa_mem_set(&dmHashEnum, 0, sizeof(DM_HASH_ENUM_T));

        while (NULL != (pDmHandler=DmHashEnumerator(&g_dmHandlerHash, &dmHashEnum)))
        {
            if (pDmHandler->bExtDb == DM_FALSE)
            {
                osa_thread_mutex_lock(&(pDmHandler->mutexXml));

                if(DmSyncDBFileReq(pDmHandler) == DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ sync %s to %s and %s succssfully in DmFlushDBfiles(%s)",pDmHandler->pucFilePath,pDmHandler->pLoadPath,pDmHandler->pBackupPath,g_DmCtrl.entity);
                }
                else
                {
                    lRet = DM_RET_INTERNAL_ERROR;
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ sync %s to %s and %s failed in DmFlushDBfiles(%s)",pDmHandler->pucFilePath,pDmHandler->pLoadPath,pDmHandler->pBackupPath,g_DmCtrl.entity);
                }

                osa_thread_mutex_unlock(&(pDmHandler->mutexXml));
            }
        }
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmFlushDBfiles(%s) lRet=%d",g_DmCtrl.entity,lRet);

    return lRet;

}

INT32 DmGetParaCValueReq(DM_GET_PARA_C_VALUE_REQ_T *  pGetPara)
{
    DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

    if (pGetPara==NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pGetPara is null in DmGetParaCValueReq(%s)",g_DmCtrl.entity);
        return lRet;
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmGetParaCValueReq(%s) transactionId=%d path=%s ValueSize=%ubytes bShmWay=%d",g_DmCtrl.entity,pGetPara->TransactionId,pGetPara->pucPath,pGetPara->ValueSize,pGetPara->bShmWay);

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;
        INT8 *pTmp = NULL;
        OSA_SHM_T *pShmReq = (OSA_SHM_T *)pGetPara->pValue;
        UINT32 memSize = pGetPara->ValueSize;

        /* client things */
        pShmReq--;

        if (pGetPara->bShmWay == DM_TRUE)
        {
            if (pShmReq->size == (pShmReq->size_bk^0xffffffff)
                && pShmReq->size == memSize+sizeof(OSA_SHM_T))
            {
                DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ use shm way to call DmGetParaCValueReq(%s) transactionId=%d",g_DmCtrl.entity,pGetPara->TransactionId);
                memSize = sizeof(UINT32);/* shm key value */
            }
            else
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ memory of pGetPara->pValue is not from shared-memory in DmGetParaCValueReq(%s) transactionId=%d",g_DmCtrl.entity,pGetPara->TransactionId);
                return lRet;
            }
        }
        else if (pGetPara->ValueSize>DM_SOCK_MAX_C_VALUE_SIZE)
        {
            DM_WARNING("consider shm way please, value memory %ubytes > %dbytes when to call DmGetParaCValueReq(%s) transactionId=%d  path=%s",pGetPara->ValueSize,DM_SOCK_MAX_C_VALUE_SIZE,g_DmCtrl.entity,pGetPara->TransactionId,pGetPara->pucPath);
        }

        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+sizeof(DM_GET_PARA_C_VALUE_REQ_T)+strlen(pGetPara->pucPath)+1+memSize);

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmGetParaCValueReq(%s) transactionId=%d",g_DmCtrl.entity,pGetPara->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+sizeof(DM_GET_PARA_C_VALUE_REQ_T)+strlen(pGetPara->pucPath)+1+memSize);

        pSockMsgSnd->msg_id = DM_MSG_CODE_GetParaCValueReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->word_data = pGetPara->TransactionId;/* keep trans-id */
        pSockMsgSnd->caller_id = osa_thread_gettid();
        if (pGetPara->bShmWay == DM_TRUE)
        {
            pSockMsgSnd->options |= DM_SOCK_MSG_OPTION__DATA_IN_SHM;
        }
        pSockMsgSnd->msgLength = sizeof(DM_GET_PARA_C_VALUE_REQ_T)+strlen(pGetPara->pucPath)+1+memSize;

        pTmp = (INT8*)(pSockMsgSnd+1);

        osa_mem_copy(pTmp, pGetPara, sizeof(DM_GET_PARA_C_VALUE_REQ_T));
        pTmp += sizeof(DM_GET_PARA_C_VALUE_REQ_T);

        strcpy(pTmp,pGetPara->pucPath);
        pTmp += strlen(pTmp)+1;

        if (pGetPara->bShmWay == DM_TRUE)
        {
            *((UINT32*)pTmp) = (UINT32)pShmReq->pshm_addr;/* pass shm key to server */
        }

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmGetParaCValueReq(%s) transactionId=%d",g_DmCtrl.entity,pGetPara->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_GetParaCValueCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_GetParaCValueCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_GetParaCValueCnf)
        {
            lRet = pSockMsgRcv->word_data;
            pGetPara->ulCnt = ((DM_GET_PARA_C_VALUE_REQ_T *)(pSockMsgRcv+1))->ulCnt;

            if (pGetPara->bShmWay == DM_FALSE)
            {
                osa_mem_copy(pGetPara->pValue, (INT8*)((DM_GET_PARA_C_VALUE_REQ_T *)(pSockMsgRcv+1)+1)+strlen(pGetPara->pucPath)+1, pGetPara->ValueSize);
            }
            else
            {
                DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ check shm(id=0x%x,key=0x%x,size=0x%x(%d),size_bk=0x%x,addr=0x%x) in DmGetParaCValueReq(%s) transactionId=%d",pShmReq->shm_id,pShmReq->key,pShmReq->size,pShmReq->size,pShmReq->size_bk,pShmReq->pshm_addr,g_DmCtrl.entity,pGetPara->TransactionId);
            }

            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmGetParaCValueReq(%s) transactionId=%d",g_DmCtrl.entity,pGetPara->TransactionId);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmGetParaCValueReq(%s) transactionId=%d",g_DmCtrl.entity,pGetPara->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {
        DM_HANDLER_T *pDmHandler = GetDmHandlerByName(pGetPara->pucPath);

        if (pDmHandler == NULL)
        {
            lRet = DM_RET_INVALID_PARAM_NAME;
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to find dmhandler, leave DmGetParaCValueReq(%s) lRet=%d transactionId=%d",g_DmCtrl.entity,lRet,pGetPara->TransactionId);
            return lRet;
        }

        osa_thread_mutex_lock(&(pDmHandler->mutexXml));

        lRet = DmGetParaCValueInternal(pGetPara);

        DmUpdateTransactionInfo(pDmHandler, pGetPara->TransactionId);

        osa_thread_mutex_unlock(&(pDmHandler->mutexXml));
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmGetParaCValueReq(%s) lRet=%d transactionId=%d",g_DmCtrl.entity,lRet,pGetPara->TransactionId);

    return lRet;
}

INT32 DmSetParaCValueReq  ( DM_SET_PARA_C_VALUE_REQ_T *  pSetPara )  
{
    DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

    if (pSetPara==NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pSetPara is null in DmSetParaCValueReq(%s)",g_DmCtrl.entity);
        return lRet;
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmSetParaCValueReq(%s) transactionId=%d path=%s ValueSize=%ubytes",g_DmCtrl.entity,pSetPara->TransactionId,pSetPara->pucPath,pSetPara->ValueSize);

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;
        INT8 *pTmp = NULL;
        OSA_SHM_T *pShmReq = NULL;
        UINT32 memSize = pSetPara->ValueSize;

        /* client things */
        pShmReq = (OSA_SHM_T *)pSetPara->pValue-1;

        if (pSetPara->bShmWay == DM_TRUE)
        {
            if (pShmReq->size == (pShmReq->size_bk^0xffffffff)
                && pShmReq->size == memSize+sizeof(OSA_SHM_T))
            {
                DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ use shm way to call DmSetParaCValueReq(%s) transactionId=%d",g_DmCtrl.entity,pSetPara->TransactionId);
                memSize = sizeof(UINT32);/* shm key value */
            }
            else
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ memory of pSetPara->pValue is not from shared-memory in DmSetParaCValueReq(%s) transactionId=%d",g_DmCtrl.entity,pSetPara->TransactionId);
                return lRet;
            }
        }
        else if (pSetPara->ValueSize>DM_SOCK_MAX_C_VALUE_SIZE)
        {
            DM_WARNING("consider shm way please, value memory %ubytes > %dbytes when to call DmSetParaCValueReq(%s) transactionId=%d  path=%s",pSetPara->ValueSize,DM_SOCK_MAX_C_VALUE_SIZE,g_DmCtrl.entity,pSetPara->TransactionId,pSetPara->pucPath);
        }

        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+sizeof(DM_SET_PARA_C_VALUE_REQ_T)+strlen(pSetPara->pucPath)+1+memSize);

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmSetParaCValueReq(%s) transactionId=%d",g_DmCtrl.entity,pSetPara->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+sizeof(DM_SET_PARA_C_VALUE_REQ_T)+strlen(pSetPara->pucPath)+1+memSize);

        pSockMsgSnd->msg_id = DM_MSG_CODE_SetParaCValueReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->word_data = pSetPara->TransactionId;/* keep trans-id */
        pSockMsgSnd->caller_id = osa_thread_gettid();
        if (pSetPara->bShmWay == DM_TRUE)
        {
            pSockMsgSnd->options |= DM_SOCK_MSG_OPTION__DATA_IN_SHM;
        }
        pSockMsgSnd->msgLength = sizeof(DM_SET_PARA_C_VALUE_REQ_T)+strlen(pSetPara->pucPath)+1+memSize;

        pTmp = (INT8*)(pSockMsgSnd+1);

        osa_mem_copy(pTmp, pSetPara, sizeof(DM_SET_PARA_C_VALUE_REQ_T));
        pTmp += sizeof(DM_SET_PARA_C_VALUE_REQ_T);

        strcpy(pTmp,pSetPara->pucPath);
        pTmp += strlen(pTmp)+1;

        if (pSetPara->bShmWay == DM_TRUE)
        {
            *((UINT32*)pTmp) = (UINT32)pShmReq->pshm_addr;/* pass shm key to server */
        }
        else
        {
            osa_mem_copy(pTmp, pSetPara->pValue, pSetPara->ValueSize);
        }

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmSetParaCValueReq(%s) transactionId=%d",g_DmCtrl.entity,pSetPara->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_SetParaCValueCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_SetParaCValueCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_SetParaCValueCnf)
        {
            lRet = pSockMsgRcv->word_data;

            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmSetParaCValueReq(%s) transactionId=%d",g_DmCtrl.entity,pSetPara->TransactionId);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmSetParaCValueReq(%s) transactionId=%d",g_DmCtrl.entity,pSetPara->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);

    }
    else
    {
        DM_HANDLER_T *pDmHandler = GetDmHandlerByName(pSetPara->pucPath);

        if (pDmHandler == NULL)
        {
            lRet = DM_RET_INVALID_PARAM_NAME;
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to find dmhandler, leave DmSetParaCValueReq(%s) lRet=%d transactionId=%d",g_DmCtrl.entity,lRet,pSetPara->TransactionId);
            return lRet;
        }

        osa_thread_mutex_lock(&(pDmHandler->mutexXml));

        lRet = DmSetParaCValueInternal(pSetPara);

        DmUpdateTransactionInfo(pDmHandler, pSetPara->TransactionId);

        osa_thread_mutex_unlock(&(pDmHandler->mutexXml));
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmSetParaCValueReq(%s) lRet=%d transactionId=%d",g_DmCtrl.entity,lRet,pSetPara->TransactionId);

    return lRet;


}

INT32 DmAddObjectByIdReq  ( DM_ADD_OBJ_BY_ID_REQ_T *  pAddPara )  
{
    DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

    if (pAddPara==NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pAddPara is null in DmAddObjectByIdReq(%s)",g_DmCtrl.entity);
        return lRet;
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmAddObjectByIdReq(%s) transactionId=%d path=%s objectSize=%ubytes",g_DmCtrl.entity,pAddPara->TransactionId,pAddPara->pucPath,pAddPara->objectSize);

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;
        INT8 *pTmp = NULL;
        OSA_SHM_T *pShmReq = NULL;
        UINT32 memSize = pAddPara->objectSize;

        /* client things */
        pShmReq = (OSA_SHM_T *)pAddPara->pObject-1;

        if (pAddPara->bShmWay == DM_TRUE)
        {
            if (pShmReq->size == (pShmReq->size_bk^0xffffffff)
                && pShmReq->size == memSize+sizeof(OSA_SHM_T))
            {
                DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ use shm way to call DmAddObjectByIdReq(%s) transactionId=%d",g_DmCtrl.entity,pAddPara->TransactionId);
                memSize = sizeof(UINT32);/* shm key value */
            }
            else
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ memory of pAddPara->pObject is not from shared-memory in DmAddObjectByIdReq(%s) transactionId=%d",g_DmCtrl.entity,pAddPara->TransactionId);
                return lRet;
            }
        }
        else if (pAddPara->objectSize>DM_SOCK_MAX_C_VALUE_SIZE)
        {
            DM_WARNING("consider shm way please, value memory %ubytes > %dbytes when to call DmAddObjectByIdReq(%s) transactionId=%d  path=%s",pAddPara->objectSize,DM_SOCK_MAX_C_VALUE_SIZE,g_DmCtrl.entity,pAddPara->TransactionId,pAddPara->pucPath);
        }

        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+sizeof(DM_ADD_OBJ_BY_ID_REQ_T)+strlen(pAddPara->pucPath)+1+memSize);

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmAddObjectByIdReq(%s) transactionId=%d",g_DmCtrl.entity,pAddPara->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+sizeof(DM_ADD_OBJ_BY_ID_REQ_T)+strlen(pAddPara->pucPath)+1+memSize);

        pSockMsgSnd->msg_id = DM_MSG_CODE_AddObjectByIdReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->word_data = pAddPara->TransactionId;/* keep trans-id */
        pSockMsgSnd->caller_id = osa_thread_gettid();
        if (pAddPara->bShmWay == DM_TRUE)
        {
            pSockMsgSnd->options |= DM_SOCK_MSG_OPTION__DATA_IN_SHM;
        }
        pSockMsgSnd->msgLength = sizeof(DM_ADD_OBJ_BY_ID_REQ_T)+strlen(pAddPara->pucPath)+1+memSize;

        pTmp = (INT8*)(pSockMsgSnd+1);

        osa_mem_copy(pTmp,pAddPara,sizeof(DM_ADD_OBJ_BY_ID_REQ_T));
        pTmp += sizeof(DM_ADD_OBJ_BY_ID_REQ_T);

        strcpy(pTmp,pAddPara->pucPath);
        pTmp += strlen(pTmp)+1;

        if (pAddPara->bShmWay == DM_TRUE)
        {
            *((UINT32*)pTmp) = (UINT32)pShmReq->pshm_addr;/* pass shm key to server */
            pTmp += sizeof(UINT32);
        }
        else
        {
            osa_mem_copy(pTmp, pAddPara->pObject, pAddPara->objectSize);
        }

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmAddObjectByIdReq(%s) transactionId=%d",g_DmCtrl.entity,pAddPara->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_AddObjectByIdCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_AddObjectByIdCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_AddObjectByIdCnf)
        {
            lRet = pSockMsgRcv->word_data;

            if (pAddPara->bShmWay == DM_FALSE)
            {
                pTmp = (INT8*)(pSockMsgRcv+1);
                pTmp += sizeof(DM_ADD_OBJ_BY_ID_REQ_T) + strlen(pAddPara->pucPath) + 1;

                osa_mem_copy(pAddPara->pObject, pTmp, pAddPara->objectSize);
            }

            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmAddObjectByIdReq(%s) transactionId=%d",g_DmCtrl.entity,pAddPara->TransactionId);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmAddObjectByIdReq(%s) transactionId=%d",g_DmCtrl.entity,pAddPara->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {
        DM_HANDLER_T *pDmHandler = GetDmHandlerByName(pAddPara->pucPath);

        if (pDmHandler == NULL)
        {
            lRet = DM_RET_INVALID_PARAM_NAME;
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to find dmhandler, leave DmAddObjectByIdReq(%s) lRet=%d transactionId=%d",g_DmCtrl.entity,lRet,pAddPara->TransactionId);
            return lRet;
        }

        osa_thread_mutex_lock(&(pDmHandler->mutexXml));

        lRet = DmAddObjectByIdInternal(pAddPara);

        DmUpdateTransactionInfo(pDmHandler, pAddPara->TransactionId);

        osa_thread_mutex_unlock(&(pDmHandler->mutexXml));
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmAddObjectByIdReq(%s) lRet=%d transactionId=%d",g_DmCtrl.entity,lRet,pAddPara->TransactionId);

    return lRet;

}

INT32 DmDelObjectByIdReq  ( DM_DEL_OBJ_BY_ID_REQ_T *  pDelPara )  
{
    DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

    if (pDelPara==NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pDelPara is null in DmDelObjectByIdReq(%s)",g_DmCtrl.entity);
        return lRet;
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmDelObjectByIdReq(%s) transactionId=%d path=%s",g_DmCtrl.entity,pDelPara->TransactionId,pDelPara->pucPath);

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;
        INT8 *pTmp = NULL;

        /* client things */
        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+sizeof(DM_DEL_OBJ_BY_ID_REQ_T)+strlen(pDelPara->pucPath)+1);

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmDelObjectByIdReq(%s) transactionId=%d",g_DmCtrl.entity,pDelPara->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+sizeof(DM_DEL_OBJ_BY_ID_REQ_T)+strlen(pDelPara->pucPath)+1);

        pSockMsgSnd->msg_id = DM_MSG_CODE_DelObjectByIdReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->word_data = pDelPara->TransactionId;/* keep trans-id */
        pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = sizeof(DM_DEL_OBJ_BY_ID_REQ_T)+strlen(pDelPara->pucPath)+1;

        pTmp = (INT8*)(pSockMsgSnd+1);

        osa_mem_copy(pTmp, pDelPara, sizeof(DM_DEL_OBJ_BY_ID_REQ_T));
        pTmp += sizeof(DM_DEL_OBJ_BY_ID_REQ_T);

        strcpy(pTmp,pDelPara->pucPath);

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmDelObjectByIdReq(%s) transactionId=%d",g_DmCtrl.entity,pDelPara->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_DelObjectByIdCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_DelObjectByIdCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_DelObjectByIdCnf)
        {
            lRet = pSockMsgRcv->word_data;

            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmDelObjectByIdReq(%s) transactionId=%d",g_DmCtrl.entity,pDelPara->TransactionId);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmDelObjectByIdReq(%s) transactionId=%d",g_DmCtrl.entity,pDelPara->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {
        DM_HANDLER_T *pDmHandler = GetDmHandlerByName(pDelPara->pucPath);

        if (pDmHandler == NULL)
        {
            lRet = DM_RET_INVALID_PARAM_NAME;
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to find dmhandler, leave DmDelObjectByIdReq(%s) lRet=%d transactionId=%d",g_DmCtrl.entity,lRet,pDelPara->TransactionId);
            return lRet;
        }

        osa_thread_mutex_lock(&(pDmHandler->mutexXml));

        lRet = DmDelObjectByIdInternal(pDelPara);

        DmUpdateTransactionInfo(pDmHandler, pDelPara->TransactionId);

        osa_thread_mutex_unlock(&(pDmHandler->mutexXml));
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmDelObjectByIdReq(%s) lRet=%d transactionId=%d",g_DmCtrl.entity,lRet,pDelPara->TransactionId);

    return lRet;

}

void DmClFreeGetParaValueReq(DM_GET_PARA_VALUE_REQ_T    *pGetParaValueReq)
{
    if (g_DmCtrl.bAsServer == DM_FALSE && pGetParaValueReq != NULL && pGetParaValueReq->pvList != NULL)
    {
        DM_FREE(pGetParaValueReq->pvList);
    }

    return;
}

INT32   DmGetParaValueReq(DM_GET_PARA_VALUE_REQ_T    *pGetParaValueReq)
{
    DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

    if (pGetParaValueReq==NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pGetParaValueReq is null in DmGetParaValueReq(%s)",g_DmCtrl.entity);
        return lRet;
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmGetParaValueReq(%s) transactionId=%d",g_DmCtrl.entity,pGetParaValueReq->TransactionId);

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        UINT32 cnt = 0;
        UINT32 msgBodyLength = 0;
        INT8 *pMsgBody = NULL;
        INT8 *pTmp = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;

        /* client things */
        /* compute the size of msg-body */
        msgBodyLength += sizeof(DM_GET_PARA_VALUE_REQ_T);
        msgBodyLength += sizeof(PARAMETER_NAME_LIST_T);
        msgBodyLength += sizeof(PARAMETER_NAME_T)*(pGetParaValueReq->pNameList->udwCnt);

        for (cnt=0; cnt<pGetParaValueReq->pNameList->udwCnt; cnt++)
        {
            msgBodyLength += strlen(pGetParaValueReq->pNameList->pucParamName[cnt].pucName)+1;
        }

        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmGetParaValueReq(%s) transactionId=%d",g_DmCtrl.entity,pGetParaValueReq->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        pSockMsgSnd->msg_id = DM_MSG_CODE_GetParaValueReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->word_data = pGetParaValueReq->TransactionId;/* keep trans-id */
        pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = msgBodyLength;

        /* fill the msg-body */
        pMsgBody = (INT8*)(pSockMsgSnd+1);

        osa_mem_copy(pMsgBody,pGetParaValueReq, sizeof(DM_GET_PARA_VALUE_REQ_T));
        pMsgBody += sizeof(DM_GET_PARA_VALUE_REQ_T);

        osa_mem_copy(pMsgBody,pGetParaValueReq->pNameList, sizeof(PARAMETER_NAME_LIST_T));
        pMsgBody += sizeof(PARAMETER_NAME_LIST_T);

        osa_mem_copy(pMsgBody,pGetParaValueReq->pNameList->pucParamName, sizeof(PARAMETER_NAME_T)*(pGetParaValueReq->pNameList->udwCnt));
        pMsgBody += sizeof(PARAMETER_NAME_LIST_T)*(pGetParaValueReq->pNameList->udwCnt);

        for (cnt=0; cnt<pGetParaValueReq->pNameList->udwCnt; cnt++)
        {
            strcpy(pMsgBody, pGetParaValueReq->pNameList->pucParamName[cnt].pucName);
            pMsgBody += strlen(pGetParaValueReq->pNameList->pucParamName[cnt].pucName) + 1;
        }

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmGetParaValueReq(%s) transactionId=%d",g_DmCtrl.entity,pGetParaValueReq->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_GetParaValueCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_GetParaValueCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_GetParaValueCnf)
        {
            lRet = pSockMsgRcv->word_data;

            if (lRet == DM_RET_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msgLength > 0)
            {
                /* copy the result */
                pGetParaValueReq->pvList = (PARAMETER_VALUE_LIST_T *)DM_MALLOC(pSockMsgRcv->msgLength);

                if (pGetParaValueReq->pvList == NULL)
                {
                    lRet = DM_RET_INTERNAL_ERROR;
                    DM_FREE(pSockMsgRcv);
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmGetParaValueReq(%s) transactionId=%d",g_DmCtrl.entity,pGetParaValueReq->TransactionId);
                    return lRet;
                }

                osa_mem_set(pGetParaValueReq->pvList, 0, pSockMsgRcv->msgLength);
                osa_mem_copy(pGetParaValueReq->pvList, pSockMsgRcv+1, pSockMsgRcv->msgLength);

                /* set pointer */
                pGetParaValueReq->pvList->pstParamValue = (PARAMETER_VALUE_T *)(pGetParaValueReq->pvList+1);

                pTmp = (INT8*)(pGetParaValueReq->pvList->pstParamValue + pGetParaValueReq->pvList->udwCnt);

                for (cnt=0; cnt<pGetParaValueReq->pvList->udwCnt; cnt++)
                {
                    if (pGetParaValueReq->pvList->pstParamValue[cnt].pucName)
                    {
                        pGetParaValueReq->pvList->pstParamValue[cnt].pucName = pTmp;
                        pTmp += strlen(pTmp) + 1;
                    }
                    if (pGetParaValueReq->pvList->pstParamValue[cnt].pucValue)
                    {
                        pGetParaValueReq->pvList->pstParamValue[cnt].pucValue = pTmp;
                        pTmp += strlen(pTmp) + 1;
                    }
                    if (pGetParaValueReq->pvList->pstParamValue[cnt].pucType)
                    {
                        pGetParaValueReq->pvList->pstParamValue[cnt].pucType = (UINT8*)pTmp;
                        pTmp += strlen(pTmp) + 1;
                    }
                }
            }

            pGetParaValueReq->pfnFree = (MEMORY_FREE)DmClFreeGetParaValueReq;

            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmGetParaValueReq(%s) transactionId=%d",g_DmCtrl.entity,pGetParaValueReq->TransactionId);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmGetParaValueReq(%s) transactionId=%d",g_DmCtrl.entity,pGetParaValueReq->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {
        DM_HANDLER_T *pDmHandler = GetDmHandlerByName(pGetParaValueReq->pNameList->pucParamName->pucName);

        if (pDmHandler == NULL)
        {
            lRet = DM_RET_INVALID_PARAM_NAME;
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to find dmhandler, leave DmGetParaValueReq(%s) lRet=%d transactionId=%d",g_DmCtrl.entity,lRet,pGetParaValueReq->TransactionId);
            return lRet;
        }

        osa_thread_mutex_lock(&(pDmHandler->mutexXml));

        lRet = DmGetParaValueInternal(pGetParaValueReq);

        DmUpdateTransactionInfo(pDmHandler, pGetParaValueReq->TransactionId);

        osa_thread_mutex_unlock(&(pDmHandler->mutexXml));
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmGetParaValueReq(%s) lRet=%d transactionId=%d",g_DmCtrl.entity,lRet,pGetParaValueReq->TransactionId);

    return lRet;

}

INT32   DmSetParaValueReq(DM_SET_PARA_VALUE_REQ_T    *pSetParaValueReq)
{
    DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

    if (pSetParaValueReq==NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pSetParaValueReq is null in DmSetParaValueReq(%s)",g_DmCtrl.entity);
        return lRet;
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmSetParaValueReq(%s) transactionId=%d",g_DmCtrl.entity,pSetParaValueReq->TransactionId);

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        UINT32 cnt = 0;
        UINT32 msgBodyLength = 0;
        INT8 *pMsgBody = NULL;

        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;

        /* client things */
        /* compute the size of msg-body */
        msgBodyLength += sizeof(DM_SET_PARA_VALUE_REQ_T);
        msgBodyLength += sizeof(PARAMETER_VALUE_LIST_T);
        msgBodyLength += sizeof(PARAMETER_VALUE_T)*(pSetParaValueReq->pvList->udwCnt);
        msgBodyLength += sizeof(UINT16)*(pSetParaValueReq->pvList->udwCnt);

        for (cnt=0; cnt<pSetParaValueReq->pvList->udwCnt; cnt++)
        {
            msgBodyLength += strlen(pSetParaValueReq->pvList->pstParamValue[cnt].pucName)+1;

            if (pSetParaValueReq->pvList->pstParamValue[cnt].pucValue)
            {
                msgBodyLength += strlen(pSetParaValueReq->pvList->pstParamValue[cnt].pucValue)+1;
            }
        }

        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmSetParaValueReq(%s) transactionId=%d",g_DmCtrl.entity,pSetParaValueReq->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        pSockMsgSnd->msg_id = DM_MSG_CODE_SetParaValueReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->word_data = pSetParaValueReq->TransactionId;/* keep trans-id */
        pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = msgBodyLength;

        /* fill the msg-body */
        pMsgBody = (INT8*)(pSockMsgSnd+1);

        osa_mem_copy(pMsgBody,pSetParaValueReq, sizeof(DM_SET_PARA_VALUE_REQ_T));
        pMsgBody += sizeof(DM_SET_PARA_VALUE_REQ_T);

        osa_mem_copy(pMsgBody,pSetParaValueReq->pvList, sizeof(PARAMETER_VALUE_LIST_T));
        pMsgBody += sizeof(PARAMETER_VALUE_LIST_T);

        osa_mem_copy(pMsgBody,pSetParaValueReq->pvList->pstParamValue, sizeof(PARAMETER_VALUE_T)*(pSetParaValueReq->pvList->udwCnt));
        pMsgBody += sizeof(PARAMETER_VALUE_T)*(pSetParaValueReq->pvList->udwCnt);

        osa_mem_copy(pMsgBody,pSetParaValueReq->pFaultCodeList, sizeof(UINT16)*(pSetParaValueReq->pvList->udwCnt));
        pMsgBody += sizeof(UINT16)*(pSetParaValueReq->pvList->udwCnt);

        for (cnt=0; cnt<pSetParaValueReq->pvList->udwCnt; cnt++)
        {
            strcpy(pMsgBody, pSetParaValueReq->pvList->pstParamValue[cnt].pucName);
            pMsgBody += strlen(pSetParaValueReq->pvList->pstParamValue[cnt].pucName) + 1;

            if (pSetParaValueReq->pvList->pstParamValue[cnt].pucValue)
            {
                strcpy(pMsgBody, pSetParaValueReq->pvList->pstParamValue[cnt].pucValue);
                pMsgBody += strlen(pSetParaValueReq->pvList->pstParamValue[cnt].pucValue) + 1;
            }
            else
            {
                pMsgBody ++;
            }
        }

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmSetParaValueReq(%s) transactionId=%d",g_DmCtrl.entity,pSetParaValueReq->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_SetParaValueCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_SetParaValueCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_SetParaValueCnf)
        {
            lRet = pSockMsgRcv->word_data;
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ lRet=%d in DmSetParaValueReq(%s) transactionId=%d",lRet,g_DmCtrl.entity,pSetParaValueReq->TransactionId);
            
            /* copy the faultcodelist */
            pMsgBody = (INT8*)(pSockMsgRcv+1);
            pMsgBody += sizeof(DM_SET_PARA_VALUE_REQ_T);
            pMsgBody += sizeof(PARAMETER_VALUE_LIST_T);
            pMsgBody += sizeof(PARAMETER_VALUE_T)*(pSetParaValueReq->pvList->udwCnt);

            osa_mem_copy(pSetParaValueReq->pFaultCodeList,pMsgBody,sizeof(UINT16)*(pSetParaValueReq->pvList->udwCnt));

            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmSetParaValueReq(%s) transactionId=%d",g_DmCtrl.entity,pSetParaValueReq->TransactionId);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmSetParaValueReq(%s) transactionId=%d",g_DmCtrl.entity,pSetParaValueReq->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {
        DM_HANDLER_T *pDmHandler = GetDmHandlerByName(pSetParaValueReq->pvList->pstParamValue->pucName);

        if (pDmHandler == NULL)
        {
            lRet = DM_RET_INVALID_PARAM_NAME;
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to find dmhandler, leave DmSetParaValueReq(%s) lRet=%d transactionId=%d",g_DmCtrl.entity,lRet,pSetParaValueReq->TransactionId);
            return lRet;
        }

        osa_thread_mutex_lock(&(pDmHandler->mutexXml));

        lRet = DmSetParaValueInternal(pSetParaValueReq);

        DmUpdateTransactionInfo(pDmHandler, pSetParaValueReq->TransactionId);

        osa_thread_mutex_unlock(&(pDmHandler->mutexXml));
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmSetParaValueReq(%s) lRet=%d transactionId=%d",g_DmCtrl.entity,lRet,pSetParaValueReq->TransactionId);

    return lRet;

}

void DmClFreeGetParaNameReq(DM_GET_PARA_NAME_REQ_T *pGetParaNameReq)
{
    if (g_DmCtrl.bAsServer == DM_FALSE && pGetParaNameReq != NULL && pGetParaNameReq->pNameList != NULL)
    {
        DM_FREE(pGetParaNameReq->pNameList);
    }

    return;
}

INT32   DmGetParaNameReq(DM_GET_PARA_NAME_REQ_T *pGetParaNameReq)
{
    DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

    if (pGetParaNameReq==NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pGetParaNameReq is null in DmGetParaNameReq(%s)",g_DmCtrl.entity);
        return lRet;
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmGetParaNameReq(%s) transactionId=%d",g_DmCtrl.entity,pGetParaNameReq->TransactionId);

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        UINT32 cnt = 0;
        UINT32 msgBodyLength = 0;
        INT8 *pMsgBody = NULL;
        INT8 *pTmp = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;

        /* client things */
        /* compute the size of msg-body */
        msgBodyLength += sizeof(DM_GET_PARA_NAME_REQ_T);
        msgBodyLength += strlen(pGetParaNameReq->pucParamName) + 1;

        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmGetParaNameReq(%s) transactionId=%d",g_DmCtrl.entity,pGetParaNameReq->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        pSockMsgSnd->msg_id = DM_MSG_CODE_GetParaNameReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->word_data = pGetParaNameReq->TransactionId;/* keep trans-id */
        pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = msgBodyLength;

        /* fill the msg-body */
        pMsgBody = (INT8*)(pSockMsgSnd+1);

        osa_mem_copy(pMsgBody,pGetParaNameReq,sizeof(DM_GET_PARA_NAME_REQ_T));
        pMsgBody += sizeof(DM_GET_PARA_NAME_REQ_T);

        strcpy(pMsgBody,pGetParaNameReq->pucParamName);
        pMsgBody += strlen(pGetParaNameReq->pucParamName) + 1;

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmSetParaValueReq(%s) transactionId=%d",g_DmCtrl.entity,pGetParaNameReq->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_GetParaNameCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_GetParaNameCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_GetParaNameCnf)
        {
            lRet = pSockMsgRcv->word_data;

            if (lRet == DM_RET_SUCCESS && pSockMsgRcv->msgLength > 0)
            {
                /* copy the results */
                pGetParaNameReq->pNameList = (PARAMETER_NAME_LIST_T *)DM_MALLOC(pSockMsgRcv->msgLength);

                if (pGetParaNameReq->pNameList == NULL)
                {
                    DM_FREE(pSockMsgRcv);
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmGetParaNameReq(%s) transactionId=%d",g_DmCtrl.entity,pGetParaNameReq->TransactionId);
                    lRet = DM_RET_INTERNAL_ERROR;
                    return lRet;
                }

                osa_mem_set(pGetParaNameReq->pNameList, 0, pSockMsgRcv->msgLength);
                osa_mem_copy(pGetParaNameReq->pNameList, pSockMsgRcv+1, pSockMsgRcv->msgLength);

                pGetParaNameReq->pNameList->pucParamName = (PARAMETER_NAME_T *)(pGetParaNameReq->pNameList + 1);

                pTmp = (INT8*)(pGetParaNameReq->pNameList->pucParamName + pGetParaNameReq->pNameList->udwCnt);

                for (cnt=0; cnt<pGetParaNameReq->pNameList->udwCnt; cnt++)
                {
                    if (pGetParaNameReq->pNameList->pucParamName[cnt].pucName)
                    {
                        pGetParaNameReq->pNameList->pucParamName[cnt].pucName = pTmp;
                        pTmp += strlen(pTmp) + 1;
                    }
                }
            }

            pGetParaNameReq->pfnFree = (MEMORY_FREE)DmClFreeGetParaNameReq;

            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmSetParaValueReq(%s) transactionId=%d",g_DmCtrl.entity,pGetParaNameReq->TransactionId);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmSetParaValueReq(%s) transactionId=%d",g_DmCtrl.entity,pGetParaNameReq->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {
        DM_HANDLER_T *pDmHandler = GetDmHandlerByName(pGetParaNameReq->pucParamName);

        if (pDmHandler == NULL)
        {
            lRet = DM_RET_INVALID_PARAM_NAME;
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to find dmhandler, leave DmGetParaNameReq(%s) lRet=%d transactionId=%d",g_DmCtrl.entity,lRet,pGetParaNameReq->TransactionId);
            return lRet;
        }

        osa_thread_mutex_lock(&(pDmHandler->mutexXml));

        lRet = DmGetParaNameInternal(pGetParaNameReq);

        DmUpdateTransactionInfo(pDmHandler, pGetParaNameReq->TransactionId);

        osa_thread_mutex_unlock(&(pDmHandler->mutexXml));
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmGetParaNameReq(%s) lRet=%d transactionId=%d",g_DmCtrl.entity,lRet,pGetParaNameReq->TransactionId);

    return lRet;

}

INT32   DmAddObjectReq(DM_ADD_OBJECT_REQ_T *pAddObjectReq)
{
    DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

    if (pAddObjectReq==NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pAddObjectReq is null in DmAddObjectReq(%s)",g_DmCtrl.entity);
        return lRet;
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmAddObjectReq(%s) transactionId=%d",g_DmCtrl.entity,pAddObjectReq->TransactionId);

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {

        UINT32 msgBodyLength = 0;
        INT8 *pMsgBody = NULL;

        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;

        /* client things */
        /* compute the size of msg-body */
        msgBodyLength += sizeof(DM_ADD_OBJECT_REQ_T);
        msgBodyLength += strlen(pAddObjectReq->pucObjName) + 1;

        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmAddObjectReq(%s) transactionId=%d",g_DmCtrl.entity,pAddObjectReq->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        pSockMsgSnd->msg_id = DM_MSG_CODE_AddObjectReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
		pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->word_data = pAddObjectReq->TransactionId;/* keep trans-id */
        pSockMsgSnd->msgLength = msgBodyLength;

        /* fill the msg-body */
        pMsgBody = (INT8*)(pSockMsgSnd+1);

        osa_mem_copy(pMsgBody,pAddObjectReq,sizeof(DM_ADD_OBJECT_REQ_T));
        pMsgBody += sizeof(DM_ADD_OBJECT_REQ_T);

        strcpy(pMsgBody,pAddObjectReq->pucObjName);
        pMsgBody += strlen(pAddObjectReq->pucObjName) + 1;

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmSetParaValueReq(%s) transactionId=%d",g_DmCtrl.entity,pAddObjectReq->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_AddObjectCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_AddObjectCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_AddObjectCnf)
        {
            lRet = pSockMsgRcv->word_data;
            
            pAddObjectReq->udwInstanceNum = ((DM_ADD_OBJECT_REQ_T*)(pSockMsgRcv+1))->udwInstanceNum;

            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmSetParaValueReq(%s) transactionId=%d",g_DmCtrl.entity,pAddObjectReq->TransactionId);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmSetParaValueReq(%s) transactionId=%d",g_DmCtrl.entity,pAddObjectReq->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);

    }
    else
    {
        DM_HANDLER_T *pDmHandler = GetDmHandlerByName(pAddObjectReq->pucObjName);

        if (pDmHandler == NULL)
        {
            lRet = DM_RET_INVALID_PARAM_NAME;
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to find dmhandler, leave DmAddObjectReq(%s) lRet=%d transactionId=%d",g_DmCtrl.entity,lRet,pAddObjectReq->TransactionId);
            return lRet;
        }

        osa_thread_mutex_lock(&(pDmHandler->mutexXml));

        lRet = DmAddObjectInternal(pAddObjectReq);

        DmUpdateTransactionInfo(pDmHandler, pAddObjectReq->TransactionId);

        osa_thread_mutex_unlock(&(pDmHandler->mutexXml));
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmAddObjectReq(%s) lRet=%d transactionId=%d",g_DmCtrl.entity,lRet,pAddObjectReq->TransactionId);

    return lRet;

}

INT32   DmDeleteObjectReq(DM_DELETE_OBJECT_REQ_T *pDeleteObjectReq)
{
    DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

    if (pDeleteObjectReq==NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pDeleteObjectReq is null in DmDeleteObjectReq(%s)",g_DmCtrl.entity);
        return lRet;
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmDeleteObjectReq(%s) transactionId=%d",g_DmCtrl.entity,pDeleteObjectReq->TransactionId);

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {

        UINT32 msgBodyLength = 0;
        INT8 *pMsgBody = NULL;

        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;

        /* client things */

        /* compute the size of msg-body */
        msgBodyLength += sizeof(DM_DELETE_OBJECT_REQ_T);
        msgBodyLength += strlen(pDeleteObjectReq->pucObjName) + 1;

        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmDeleteObjectReq(%s) transactionId=%d",g_DmCtrl.entity,pDeleteObjectReq->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        pSockMsgSnd->msg_id = DM_MSG_CODE_DeleteObjectReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
		pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->word_data = pDeleteObjectReq->TransactionId;/* keep trans-id */
        pSockMsgSnd->msgLength = msgBodyLength;

        /* fill the msg-body */
        pMsgBody = (INT8*)(pSockMsgSnd+1);

        osa_mem_copy(pMsgBody,pDeleteObjectReq,sizeof(DM_DELETE_OBJECT_REQ_T));
        pMsgBody += sizeof(DM_DELETE_OBJECT_REQ_T);

        strcpy(pMsgBody,pDeleteObjectReq->pucObjName);
        pMsgBody += strlen(pDeleteObjectReq->pucObjName) + 1;

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmDeleteObjectReq(%s) transactionId=%d",g_DmCtrl.entity,pDeleteObjectReq->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_DeleteObjectCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_DeleteObjectCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_DeleteObjectCnf)
        {
            lRet = pSockMsgRcv->word_data;
            
            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmSetParaValueReq(%s) transactionId=%d",g_DmCtrl.entity,pDeleteObjectReq->TransactionId);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmSetParaValueReq(%s) transactionId=%d",g_DmCtrl.entity,pDeleteObjectReq->TransactionId);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {
        DM_HANDLER_T *pDmHandler = GetDmHandlerByName(pDeleteObjectReq->pucObjName);

        if (pDmHandler == NULL)
        {
            lRet = DM_RET_INVALID_PARAM_NAME;
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to find dmhandler, leave DmDeleteObjectReq(%s) lRet=%d transactionId=%d",g_DmCtrl.entity,lRet,pDeleteObjectReq->TransactionId);
            return lRet;
        }

        osa_thread_mutex_lock(&(pDmHandler->mutexXml));

        lRet = DmDeleteObjectInternal(pDeleteObjectReq);

        DmUpdateTransactionInfo(pDmHandler, pDeleteObjectReq->TransactionId);

        osa_thread_mutex_unlock(&(pDmHandler->mutexXml));
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmDeleteObjectReq(%s) lRet=%d transactionId=%d",g_DmCtrl.entity,lRet,pDeleteObjectReq->TransactionId);

    return lRet;

}

INT32 DmExtDbNewParaReq(INT8 *pExtDbName, DM_EXT_DB_NEW_PARA_REQ_T *pNewParas)
{
    INT32 lRet = DM_RET_SUCCESS;
    INT32 cnt = 0;
    INT32 iCnt = 0;

	if (pExtDbName == NULL || pNewParas == NULL || pNewParas->ulParaCnt == 0 || pNewParas->pNewParas == NULL)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ null arguments in DmExtDbCreateXmlDoc(%s)", g_DmCtrl.entity);
		return DM_RET_INVALID_ARGUMENTS;
	}

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmExtDbNewParaReq(%s) dbName(%s)", g_DmCtrl.entity, pExtDbName);

    for (cnt=0;cnt<pNewParas->ulParaCnt;cnt++)
    {
        if (pNewParas->pNewParas[cnt].pParaName==NULL
            || pNewParas->pNewParas[cnt].pParaToRefer==NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ null pParaName/pParaToRefer in DmExtDbCreateXmlDoc(%s)", g_DmCtrl.entity);
    		return DM_RET_INVALID_ARGUMENTS;
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ add new para %s, refered %s, position %d(0:prev-bro,1:next-bro,2:last-child) in DmExtDbCreateXmlDoc(%s)",pNewParas->pNewParas[cnt].pParaName,pNewParas->pNewParas[cnt].pParaToRefer,pNewParas->pNewParas[cnt].positionToAdd,g_DmCtrl.entity);
        }
    }

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        UINT32 msgBodyLength = 0;
        INT8   *pMsgBody = NULL;

        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;

        /* client things */
        /* compute the size of msg-body */
        msgBodyLength += strlen(pExtDbName) + 1;
        msgBodyLength += sizeof(DM_EXT_DB_NEW_PARA_REQ_T);
        msgBodyLength += sizeof(DM_EXT_DB_NEW_PARA_T)*pNewParas->ulParaCnt;

        for (cnt=0;cnt<pNewParas->ulParaCnt;cnt++)
        {
    		if (pNewParas->pNewParas[cnt].pParaToRefer != NULL)
    		{
    			msgBodyLength += strlen(pNewParas->pNewParas[cnt].pParaToRefer) + 1;
    		}
            else
            {
                msgBodyLength++;
            }
    		
    		if (pNewParas->pNewParas[cnt].pParaName != NULL)
    		{
    			msgBodyLength += strlen(pNewParas->pNewParas[cnt].pParaName) + 1;
    		}
            else
            {
                msgBodyLength++;
            }
    		
    		if (pNewParas->pNewParas[cnt].pParaValue != NULL)
    		{
    			msgBodyLength += strlen(pNewParas->pNewParas[cnt].pParaValue) + 1;
    		}
            else
            {
                msgBodyLength++;
            }

    		if (pNewParas->pNewParas[cnt].ulAttrCnt > 0)
    		{
                msgBodyLength += sizeof(DM_ATTR_NODE_T)*pNewParas->pNewParas[cnt].ulAttrCnt;

                for (iCnt=0; iCnt<pNewParas->pNewParas[cnt].ulAttrCnt; iCnt++)
                {
                    if (pNewParas->pNewParas[cnt].pAttrValue[iCnt].pAttrName)
                    {
                        msgBodyLength += strlen(pNewParas->pNewParas[cnt].pAttrValue[iCnt].pAttrName) + 1;
                    }

                    if (pNewParas->pNewParas[cnt].pAttrValue[iCnt].pAttrValue)
                    {
                        msgBodyLength += pNewParas->pNewParas[cnt].pAttrValue[iCnt].ulAttrValueSize;
                    }
                }
    		}
        }

		// Set caller pid for check DB ownner permission
		msgBodyLength += sizeof(DM_CALLER_INFO_T);

        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC( sizeof(DM_SOCK_MSG_HEADER_T) + msgBodyLength);
		if (pSockMsgSnd == NULL)
        {
            DM_ERROR(DM_ERR_MEMORY_ALLOCATION_FAILED, "DM Trace ------ malloc failed in DmExtDbNewParaReq(%s)", g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T) + msgBodyLength);
        pSockMsgSnd->msg_id = DM_MSG_CODE_ExtDbNewParaReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->word_data = pNewParas->TransactionId;
		pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = msgBodyLength;

        /* fill the msg-body */
        pMsgBody = (INT8*)(pSockMsgSnd + 1);

        strcpy(pMsgBody, pExtDbName);
        pMsgBody += strlen(pExtDbName) + 1;

		osa_mem_copy(pMsgBody, pNewParas, sizeof(DM_EXT_DB_NEW_PARA_REQ_T));
        pMsgBody += sizeof(DM_EXT_DB_NEW_PARA_REQ_T);

        osa_mem_copy(pMsgBody, pNewParas->pNewParas, sizeof(DM_EXT_DB_NEW_PARA_T)*pNewParas->ulParaCnt);
        pMsgBody += sizeof(DM_EXT_DB_NEW_PARA_T)*pNewParas->ulParaCnt;

        for (cnt=0;cnt<pNewParas->ulParaCnt;cnt++)
        {
            if (pNewParas->pNewParas[cnt].pParaToRefer != NULL)
    		{
                strcpy(pMsgBody, pNewParas->pNewParas[cnt].pParaToRefer);
    			pMsgBody += strlen(pNewParas->pNewParas[cnt].pParaToRefer) + 1;
    		}
            else
            {
                pMsgBody++;
            }
    		
    		if (pNewParas->pNewParas[cnt].pParaName != NULL)
    		{
                strcpy(pMsgBody, pNewParas->pNewParas[cnt].pParaName);
    			pMsgBody += strlen(pNewParas->pNewParas[cnt].pParaName) + 1;
    		}
    		else
            {
                pMsgBody++;
            }

    		if (pNewParas->pNewParas[cnt].pParaValue != NULL)
    		{
                strcpy(pMsgBody, pNewParas->pNewParas[cnt].pParaValue);
    			pMsgBody += strlen(pNewParas->pNewParas[cnt].pParaValue) + 1;
    		}
            else
            {
                pMsgBody++;
            }

            if (pNewParas->pNewParas[cnt].ulAttrCnt > 0)
            {
                osa_mem_copy(pMsgBody, pNewParas->pNewParas[cnt].pAttrValue, sizeof(DM_ATTR_NODE_T)*pNewParas->pNewParas[cnt].ulAttrCnt);
                pMsgBody += sizeof(DM_ATTR_NODE_T)*pNewParas->pNewParas[cnt].ulAttrCnt;

                for (iCnt=0; iCnt<pNewParas->pNewParas[cnt].ulAttrCnt; iCnt++)
                {
                    if (pNewParas->pNewParas[cnt].pAttrValue[iCnt].pAttrName)
                    {
                        strcpy(pMsgBody, pNewParas->pNewParas[cnt].pAttrValue[iCnt].pAttrName);
                        pMsgBody += strlen(pNewParas->pNewParas[cnt].pAttrValue[iCnt].pAttrName) + 1;
                    }

                    if (pNewParas->pNewParas[cnt].pAttrValue[iCnt].pAttrValue)
                    {
                        osa_mem_copy(pMsgBody, pNewParas->pNewParas[cnt].pAttrValue[iCnt].pAttrValue, pNewParas->pNewParas[cnt].pAttrValue[iCnt].ulAttrValueSize);
                        pMsgBody += pNewParas->pNewParas[cnt].pAttrValue[iCnt].ulAttrValueSize;
                    }
                }
            }
        }

		// Set caller pid for check DB ownner permission
		DM_CALLER_INFO_T* pCaller = (DM_CALLER_INFO_T*)pMsgBody;
		pCaller->pid = getpid();

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);
        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmExtDbNewParaReq(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }
        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_ExtDbNewParaCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp, &pSockMsgRcv, DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_ExtDbNewParaCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_ExtDbNewParaCnf)
        {
            lRet = pSockMsgRcv->word_data;
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmExtDbNewParaReq(%s)", g_DmCtrl.entity);
        }
        else
        {
            DM_ERROR(DM_ERR_PROCESSING_COMMON_ERROR, "DM Trace ------ osa_resource_wait failed in DmExtDbNewParaReq(%s)", g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {
        INT8 *ucXPathName = NULL;
        xmlXPathContextPtr	xpathCtx = NULL;
        DM_HANDLER_T *pDmHandler = GetDmHandlerByName(pExtDbName);

        if (NULL == pDmHandler || !pDmHandler->bExtDb )
        {
            lRet = DM_RET_INVALID_ARGUMENTS;
            DM_ERROR(DM_ERR_INVALID_ARGUMENTS, "DM Trace ------ failed to find dm-handler of %s, leave DmExtDbNewParaReq(%s)", pExtDbName, g_DmCtrl.entity);
            return lRet;
        }

        osa_thread_mutex_lock(&(pDmHandler->mutexXml));

        xpathCtx = xmlXPathNewContext(pDmHandler->pXmlDocument);
		if(NULL == xpathCtx)
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DmExtDbNewParaReq can't get xpathCtx");
            osa_thread_mutex_unlock(&(pDmHandler->mutexXml));
			return DM_RET_INTERNAL_ERROR;
		}
        ucXPathName = (INT8 *)DM_MALLOC(sizeof(INT8)*MAX_XML_PATH);

        if (ucXPathName == NULL)
        {
            DM_ERROR(DM_ERR_MEMORY_ALLOCATION_FAILED, "DB Trace ------ DmExtDbNewParaReq : ucXPathName = NULL");
            osa_thread_mutex_unlock(&(pDmHandler->mutexXml));
            return DM_RET_INTERNAL_ERROR;

        }

        for (cnt=0;cnt<pNewParas->ulParaCnt;cnt++)
        {
            memset(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);

            ConvertPathToXPath(pNewParas->pNewParas[cnt].pParaToRefer, ucXPathName);
            pDmHandler->xpathParentObj = xmlXPathEvalExpression((const xmlChar *)ucXPathName, xpathCtx);

            if (pDmHandler->xpathParentObj==NULL)
            {
                DM_WARNING("DB Trace ------ DmExtDbNewParaReq : pDmHandler->xpathParentObj = NULL ucXPathName=%s",ucXPathName);
                osa_thread_mutex_unlock(&(pDmHandler->mutexXml));
                DM_FREE(ucXPathName);
                return DM_RET_INVALID_ARGUMENTS;
            }

            /* new the xml node */
            xmlNodePtr node = xmlNewNode(NULL, (const xmlChar *)pNewParas->pNewParas[cnt].pParaName);
            xmlNodeSetContent(node, BAD_CAST pNewParas->pNewParas[cnt].pParaValue);

            /* set attributes if any */
            for (iCnt=0; iCnt<pNewParas->pNewParas[cnt].ulAttrCnt; iCnt++)
            {
                INT8 strValue[1024] = {0};
                void *pAttrValue = pNewParas->pNewParas[cnt].pAttrValue[iCnt].pAttrValue;

                if (pNewParas->pNewParas[cnt].pAttrValue[iCnt].bCType==DM_TRUE)
                {
                    DmConvertAttributeToString(pDmHandler,pNewParas->pNewParas[cnt].pAttrValue[iCnt].pAttrName,pNewParas->pNewParas[cnt].pAttrValue[iCnt].pAttrValue,pNewParas->pNewParas[cnt].pAttrValue[iCnt].ulAttrValueCnt,strValue);
                    pAttrValue = strValue;
                }

                xmlNewProp(node, (const xmlChar *)pNewParas->pNewParas[cnt].pAttrValue[iCnt].pAttrName, (const xmlChar *)pAttrValue);
            }

            /* insert the xml node */
            switch(pNewParas->pNewParas[cnt].positionToAdd)
            {
                case DM_EXT_DB_POSITION_AS_PREV_BROTHER:
                    xmlAddPrevSibling(pDmHandler->xpathParentObj->nodesetval->nodeTab[0], node);
                    break;
                case DM_EXT_DB_POSITION_AS_NEXT_BROTHER:
                    xmlAddNextSibling(pDmHandler->xpathParentObj->nodesetval->nodeTab[0], node);
                    break;
                case DM_EXT_DB_POSITION_AS_LAST_CHILD:
                    xmlAddChild(pDmHandler->xpathParentObj->nodesetval->nodeTab[0], node);
                    break;
                case DM_EXT_DB_LAST_POSITION_AS_PREV_BROTHER:
                    xmlAddPrevSibling(pDmHandler->xpathParentObj->nodesetval->nodeTab[pDmHandler->xpathParentObj->nodesetval->nodeNr-1], node);
                    break;
                case DM_EXT_DB_LAST_POSITION_AS_NEXT_BROTHER:
                    xmlAddNextSibling(pDmHandler->xpathParentObj->nodesetval->nodeTab[pDmHandler->xpathParentObj->nodesetval->nodeNr-1], node);
                    break;
                case DM_EXT_DB_LAST_POSITION_AS_LAST_CHILD:
                    xmlAddChild(pDmHandler->xpathParentObj->nodesetval->nodeTab[pDmHandler->xpathParentObj->nodesetval->nodeNr-1], node);
                    break;
                default:
                    DM_WARNING("DB Trace ------ DmExtDbNewParaReq : invalid position(%d) to add new parameter",pNewParas->pNewParas[cnt].positionToAdd);
                    break;
            }
        }

        DM_FREE(ucXPathName);

        osa_thread_mutex_unlock(&(pDmHandler->mutexXml));
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmExtDbNewParaReq(%s) lRet=%d",g_DmCtrl.entity,lRet);

    return lRet;
}

INT32 DmExtDbOpen(INT8 *pExtDbName, INT8 *pXmlFile, DM_EXT_DB_PROP_T *pExtDbProps)
{
    INT32 lRet = DM_RET_SUCCESS;

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmExtDbOpen(%s) dbName(%s) xmlFile(%s)",g_DmCtrl.entity,pExtDbName,pXmlFile?pXmlFile:"null");

    if (pExtDbProps)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmExtDbOpen(%s) dbName(%s) pExtDbProps->XmlStyleSheet(%s)",g_DmCtrl.entity,pExtDbName,pExtDbProps->XmlStyleSheet?pExtDbProps->XmlStyleSheet:"null");
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmExtDbOpen(%s) dbName(%s) pExtDbProps->Xmlns(%s)",g_DmCtrl.entity,pExtDbName,pExtDbProps->Xmlns?pExtDbProps->Xmlns:"null");
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmExtDbOpen(%s) dbName(%s) pExtDbProps->XmlnsXsi(%s)",g_DmCtrl.entity,pExtDbName,pExtDbProps->XmlnsXsi?pExtDbProps->XmlnsXsi:"null");
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmExtDbOpen(%s) dbName(%s) pExtDbProps->XsiSchemaLocation(%s)",g_DmCtrl.entity,pExtDbName,pExtDbProps->XsiSchemaLocation?pExtDbProps->XsiSchemaLocation:"null");
    }
    else
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmExtDbOpen(%s) dbName(%s) pExtDbProps(null)",g_DmCtrl.entity,pExtDbName);
    }

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        UINT32 msgBodyLength = 0;
        INT8 *pMsgBody = NULL;
        UINT32 arguFlags = 0;

        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;

        /* client things */
        /* compute the size of msg-body */
        msgBodyLength += strlen(pExtDbName) + 1;
        if (pXmlFile)
        {
            arguFlags |= 0x00000001;
            msgBodyLength += strlen(pXmlFile) + 1;
        }
		msgBodyLength += sizeof(DM_CALLER_INFO_T);
        if (pExtDbProps)
        {
            arguFlags |= 0x00000002;
            msgBodyLength += sizeof(DM_EXT_DB_PROP_T);

            if (pExtDbProps->XmlStyleSheet)
            {
                arguFlags |= 0x00000004;
                msgBodyLength += strlen(pExtDbProps->XmlStyleSheet) + 1;
            }
            else
            {
                msgBodyLength++;
            }

            if (pExtDbProps->Xmlns)
            {
                arguFlags |= 0x00000008;
                msgBodyLength += strlen(pExtDbProps->Xmlns) + 1;
            }
            else
            {
                msgBodyLength++;
            }

            if (pExtDbProps->XmlnsXsi)
            {
                arguFlags |= 0x00000010;
                msgBodyLength += strlen(pExtDbProps->XmlnsXsi) + 1;
            }
            else
            {
                msgBodyLength++;
            }

            if (pExtDbProps->XsiSchemaLocation)
            {
                arguFlags |= 0x00000020;
                msgBodyLength += strlen(pExtDbProps->XsiSchemaLocation) + 1;
            }
            else
            {
                msgBodyLength++;
            }
        }

        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmExtDbOpen(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        pSockMsgSnd->msg_id = DM_MSG_CODE_ExtDbOpenReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->word_data = arguFlags;
        pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = msgBodyLength;

        /* fill the msg-body */
        pMsgBody = (INT8*)(pSockMsgSnd+1);

        strcpy(pMsgBody,pExtDbName);
        pMsgBody += strlen(pExtDbName) + 1;

        if (pXmlFile)
        {
            strcpy(pMsgBody,pXmlFile);
            pMsgBody += strlen(pXmlFile) + 1;
        }

		// Set caller pid for check DB ownner permission
		DM_CALLER_INFO_T* pCaller = (DM_CALLER_INFO_T*)pMsgBody;
		pCaller->pid = getpid();
        pMsgBody += sizeof(DM_CALLER_INFO_T);

        if (pExtDbProps)
        {
            osa_mem_copy(pMsgBody,pExtDbProps,sizeof(DM_EXT_DB_PROP_T));
            pMsgBody += sizeof(DM_EXT_DB_PROP_T);

            if (pExtDbProps->XmlStyleSheet)
            {
                strcpy(pMsgBody,pExtDbProps->XmlStyleSheet);
                pMsgBody += strlen(pExtDbProps->XmlStyleSheet) + 1;
            }
            else
            {
                pMsgBody++;
            }

            if (pExtDbProps->Xmlns)
            {
                strcpy(pMsgBody,pExtDbProps->Xmlns);
                pMsgBody += strlen(pExtDbProps->Xmlns) + 1;
            }
            else
            {
                pMsgBody++;
            }

            if (pExtDbProps->XmlnsXsi)
            {
                strcpy(pMsgBody,pExtDbProps->XmlnsXsi);
                pMsgBody += strlen(pExtDbProps->XmlnsXsi) + 1;
            }
            else
            {
                pMsgBody++;
            }

            if (pExtDbProps->XsiSchemaLocation)
            {
                strcpy(pMsgBody,pExtDbProps->XsiSchemaLocation);
                pMsgBody += strlen(pExtDbProps->XsiSchemaLocation) + 1;
            }
            else
            {
                pMsgBody++;
            }
        }

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmExtDbOpen(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_ExtDbOpenCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_ExtDbOpenCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_ExtDbOpenCnf)
        {
            lRet = pSockMsgRcv->word_data;
            
            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmExtDbOpen(%s)",g_DmCtrl.entity);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmExtDbOpen(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {
        DM_HANDLER_T *pDmHandler = NULL;

        pDmHandler = GetDmHandlerByName(pExtDbName);
        if(NULL == pDmHandler || !pDmHandler->bExtDb)
        {
            lRet = DM_RET_INVALID_ARGUMENTS;
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to find dm-handler, leave DmExtDbOpen(%s)",g_DmCtrl.entity);
            return lRet;
        }

        //osa_thread_mutex_lock(&(pDmHandler->mutexXml));

        if (pXmlFile != NULL)
        {
            pDmHandler->pucFilePath = strdup(pXmlFile);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pXmlFile is empty, in DmExtDbOpen(%s)",g_DmCtrl.entity);
            pDmHandler->pucFilePath = NULL;
        }

        /*if xml file exist, load it. else, create one and load it*/
        if(pDmHandler->pucFilePath && access(pDmHandler->pucFilePath, F_OK) == 0)
        {

            pDmHandler->pXmlDocument = xmlReadFile(pDmHandler->pucFilePath, NULL, XML_PARSE_NOBLANKS);

            if(NULL != pDmHandler->pXmlDocument)
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmExtDbOpen(%s), parser file %s for db %s",g_DmCtrl.entity, pDmHandler->pucFilePath, pDmHandler->pucDbName);
                lRet = DM_RET_SUCCESS;
            }
            else
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmExtDbOpen(%s) parser file %s for db %s error",g_DmCtrl.entity, pDmHandler->pucFilePath, pDmHandler->pucDbName);

                lRet = DM_RET_INVALID_CONFIG_FILE;

                DM_FREE(pDmHandler->pucFilePath);
            }
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ %s does not exist, create %s dbfile, in DmExtDbOpen(%s)", pDmHandler->pucFilePath?pDmHandler->pucFilePath:"null",pDmHandler->pucDbName,g_DmCtrl.entity);

            pDmHandler->pXmlDocument = xmlNewDoc(BAD_CAST"1.0");
            if(DM_SUCCESS != DmXmlFileGenerateReq((INT8 *)(pDmHandler->pucDbName)))
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ create xml file failure: %s in DmExtDbOpen(%s)", pDmHandler->pucDbName,g_DmCtrl.entity);

                lRet = DM_RET_INTERNAL_ERROR;

                DM_FREE(pDmHandler->pucFilePath);
            }
        }

        if (pDmHandler->pXmlDocument)
        {
            xmlNodePtr pRoot = xmlDocGetRootElement(pDmHandler->pXmlDocument);

    		if (pExtDbProps && pExtDbProps->XmlStyleSheet)
    		{
    			xmlNodePtr  pPI = xmlNewDocPI(pDmHandler->pXmlDocument, (xmlChar *)"xml-stylesheet",  (xmlChar *)pExtDbProps->XmlStyleSheet);
    			xmlAddPrevSibling(pRoot, pPI);
    		}

    		if (pExtDbProps && pExtDbProps->Xmlns)
    		{
    			xmlNewNs(pRoot, (xmlChar *)pExtDbProps->Xmlns, NULL);
    		}

    		if (pExtDbProps && pExtDbProps->XmlnsXsi)
    		{
    			xmlNewNs(pRoot, (xmlChar *)pExtDbProps->XmlnsXsi, (xmlChar *)"xsi");
    		}

    		if (pExtDbProps && pExtDbProps->XsiSchemaLocation)
    		{
    			xmlSetProp(pRoot, (xmlChar *)"xsi:schemaLocation", (xmlChar *)pExtDbProps->XsiSchemaLocation);
    		}
        }

        //osa_thread_mutex_unlock(&(pDmHandler->mutexXml));
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmExtDbOpen(%s) lRet=%d",g_DmCtrl.entity,lRet);

    return lRet;
}

INT32 DmExtDbClose(INT8 *pExtDbName, INT8 *pXmlFile, BOOL8 bDocReleased)
{
    INT32 lRet = DM_RET_SUCCESS;

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmExtDbClose(%s) dbName(%s) pXmlFile(%s) bDocReleased(%d)",g_DmCtrl.entity,pExtDbName,pXmlFile?pXmlFile:"null",bDocReleased);

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        UINT32 msgBodyLength = 0;
        INT8 *pMsgBody = NULL;

        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;

        /* client things */
        /* compute the size of msg-body */
        msgBodyLength = strlen(pExtDbName) + 1;
        if (pXmlFile)
        {
            msgBodyLength += strlen(pXmlFile) + 1;
        }
        msgBodyLength += sizeof(BOOL8);
		msgBodyLength += sizeof(DM_CALLER_INFO_T);

        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmExtDbClose(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        pSockMsgSnd->msg_id = DM_MSG_CODE_ExtDbCloseReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->word_data = pXmlFile?DM_TRUE:DM_FALSE;
        pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = msgBodyLength;

        /* fill the msg-body */
        pMsgBody = (INT8*)(pSockMsgSnd+1);

        strcpy(pMsgBody,pExtDbName);
        pMsgBody += strlen(pExtDbName) + 1;

        if (pXmlFile)
        {
            strcpy(pMsgBody,pXmlFile);
            pMsgBody += strlen(pXmlFile) + 1;
        }

        osa_mem_copy(pMsgBody,&bDocReleased,sizeof(BOOL8));
        pMsgBody += sizeof(BOOL8);

		// Set caller pid for check DB ownner permission
		DM_CALLER_INFO_T* pCaller = (DM_CALLER_INFO_T*)pMsgBody;
		pCaller->pid = getpid();

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmExtDbClose(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_ExtDbCloseCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_ExtDbCloseCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_ExtDbCloseCnf)
        {
            lRet = pSockMsgRcv->word_data;
            
            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmExtDbClose(%s)",g_DmCtrl.entity);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmExtDbClose(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {
        DM_HANDLER_T *pDmHandler = NULL;

        pDmHandler = GetDmHandlerByName(pExtDbName);

        if(NULL == pDmHandler || !pDmHandler->bExtDb)
        {
            lRet = DM_RET_INVALID_ARGUMENTS;
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to find dm-handler, leave DmExtDbClose(%s)",g_DmCtrl.entity);
            return lRet;
        }

        osa_thread_mutex_lock(&(pDmHandler->mutexXml));

        if(NULL != pDmHandler->pXmlDocument)
        {
            if (pXmlFile)
            {
                xmlSaveFormatFileEnc(pXmlFile, pDmHandler->pXmlDocument, (const char *)"UTF-8", 1);
            }
            else if (pDmHandler->pucFilePath)
            {
                xmlSaveFormatFileEnc(pDmHandler->pucFilePath, pDmHandler->pXmlDocument, (const char *)"UTF-8", 1);
            }

            if (bDocReleased==DM_TRUE)
            {
                xmlFreeDoc(pDmHandler->pXmlDocument);
                pDmHandler->pXmlDocument = NULL;

                DM_FREE(pDmHandler->pucFilePath);
            }
        }

        osa_thread_mutex_unlock(&(pDmHandler->mutexXml));
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmExtDbClose(%s) lRet=%d",g_DmCtrl.entity,lRet);

    return lRet;
}

BOOL8 DmRegisterNotifyHandlerReq  ( DM_NOTIFY_HANDLER_REG_PARA_T *  pPara )  
{
    INT32 lRet = DM_RET_SUCCESS;
    BOOL8 bRet = DM_FALSE;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmRegisterNotifyHandlerReq(%s)",g_DmCtrl.entity);

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        UINT32 cnt = 0;
        UINT32 msgBodyLength = 0;
        INT8 *pMsgBody = NULL;

        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;
        INT8 *pFullPath = NULL;

        DM_NOTIFY_CB_NODE_T       *pCbNodeList       = NULL;
        DM_NOTIFY_CB_NODE_T       *pNewCbNode       = NULL;

        /* client things */
        /*
                here the client should keep the call-back functions,
                and just inform which params should be notified it.

                when param changed, server will notify the client of these params.
                and the client will lookup the notify-hash-table for the callback function to call.
            */

        /* compute the size of msg-body */
        msgBodyLength = sizeof(DM_NOTIFY_HANDLER_REG_PARA_T);
        msgBodyLength += sizeof(INT8*)*(pPara->lParaCnt);

        for (cnt=0; cnt<pPara->lParaCnt; cnt++)
        {
            msgBodyLength += strlen(pPara->paParaPath[cnt])+1;
        }

        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmRegisterNotifyHandlerReq(%s)",g_DmCtrl.entity);
            bRet = DM_FALSE;
            return bRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        pSockMsgSnd->msg_id = DM_MSG_CODE_RegisterNotifyHandlerReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->word_data = 0;
        pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = msgBodyLength;

        /* fill the msg-body */
        pMsgBody = (INT8*)(pSockMsgSnd+1);

        osa_mem_copy(pMsgBody,pPara,sizeof(DM_NOTIFY_HANDLER_REG_PARA_T));

        osa_mem_copy(pMsgBody, g_DmCtrl.entity, sizeof(g_DmCtrl.entity));/* fill the entity */

        pMsgBody += sizeof(DM_NOTIFY_HANDLER_REG_PARA_T);

        osa_mem_copy(pMsgBody,pPara->paParaPath,sizeof(INT8*)*(pPara->lParaCnt));
        pMsgBody += sizeof(INT8*)*(pPara->lParaCnt);

        for (cnt=0; cnt<pPara->lParaCnt; cnt++)
        {
            strcpy(pMsgBody, pPara->paParaPath[cnt]);
            pMsgBody += strlen(pPara->paParaPath[cnt])+1;
        }

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmRegisterNotifyHandlerReq(%s)",g_DmCtrl.entity);
            bRet = DM_FALSE;
            return bRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_RegisterNotifyHandlerCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_RegisterNotifyHandlerCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_RegisterNotifyHandlerCnf)
        {
            bRet = (BOOL8)(pSockMsgRcv->word_data);

            /* update only if server ok */
            if (bRet == DM_TRUE)
            {
                for (cnt=0; cnt<pPara->lParaCnt; cnt++)
                {
                    /* keep the notify-cb in notify-hash-table */
                    pFullPath = DmChangeOrgNameToFullname(pPara->paParaPath[cnt]);

                    pNewCbNode = (DM_NOTIFY_CB_NODE_T*)DM_MALLOC(sizeof(DM_NOTIFY_CB_NODE_T));

                    if (pNewCbNode == NULL)
                    {
                        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pNewCbNode is null for %s in DmRegisterNotifyHandlerReq(%s)",pPara->paParaPath[cnt],g_DmCtrl.entity);
                        DM_FREE(pFullPath);
                        return DM_FALSE;
                    }

                    osa_mem_set(pNewCbNode,0,sizeof(DM_NOTIFY_CB_NODE_T));

                    pNewCbNode->pCb = pPara->pCallbackFunc;
                    pNewCbNode->pNext = NULL;

                    pCbNodeList = DmHashGetValue(&g_DmCtrl.notifyFuncHash,pFullPath);

                    if (pCbNodeList == NULL)
                    {
                        pCbNodeList = pNewCbNode;
                    }
                    else
                    {
                        pNewCbNode->pNext = pCbNodeList;
                        pCbNodeList = pNewCbNode;
                    }

                    DmHashAdd(&g_DmCtrl.notifyFuncHash,pFullPath,(void *)pCbNodeList);

                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ pPara %s added to notifyFuncHash, cb-addr=0x%x in DmRegisterNotifyHandlerReq(%s)",pFullPath,pPara->pCallbackFunc,g_DmCtrl.entity);
                    DM_FREE(pFullPath);
                }
            }

            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmRegisterNotifyHandlerReq(%s)",g_DmCtrl.entity);

        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmRegisterNotifyHandlerReq(%s)",g_DmCtrl.entity);
            bRet = DM_FALSE;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {
        DM_HANDLER_T             *pDmHandler    = NULL;
    	INT32                     lIndex        = 0;
    	INT32                     i             = 0;
    	DM_NODE_NOTIFY_HANDLER_T *pCallbackPtr = NULL;
        INT8                     *pFullPath     = NULL;
        DM_NOTIFY_CB_NODE_T       *pCbNodeList       = NULL;
        DM_NOTIFY_CB_NODE_T       *pNewCbNode       = NULL;

    	if(NULL == pPara)
    	{
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pPara is null in DmRegisterNotifyHandlerReq(%s)",g_DmCtrl.entity);
    	    return DM_FALSE;
    	}


        /* if entity is server, here keep notify-cb function */
        if (strlen(pPara->entity)==0 || strcmp(pPara->entity,g_DmCtrl.entity)==0)
        {
            for(i = 0; i < pPara->lParaCnt; i++)
            {
                pFullPath = DmChangeOrgNameToFullname(pPara->paParaPath[i]);

                pNewCbNode = (DM_NOTIFY_CB_NODE_T*)DM_MALLOC(sizeof(DM_NOTIFY_CB_NODE_T));

                if (pNewCbNode == NULL)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pNewCbNode is null for %s in DmRegisterNotifyHandlerReq(%s)",pPara->paParaPath[i],g_DmCtrl.entity);
                    DM_FREE(pFullPath);
                    return DM_FALSE;
                }

                osa_mem_set(pNewCbNode,0,sizeof(DM_NOTIFY_CB_NODE_T));

                pNewCbNode->pCb = pPara->pCallbackFunc;
                pNewCbNode->pNext = NULL;

                pCbNodeList = DmHashGetValue(&g_DmCtrl.notifyFuncHash,pFullPath);

                if (pCbNodeList == NULL)
                {
                    pCbNodeList = pNewCbNode;
                }
                else
                {
                    pNewCbNode->pNext = pCbNodeList;
                    pCbNodeList = pNewCbNode;
                }

                DmHashAdd(&g_DmCtrl.notifyFuncHash,pFullPath,(void *)pCbNodeList);

                DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ pPara %s added to notifyFuncHash, cb-addr=0x%x, in DmRegisterNotifyHandlerReq(%s)",pFullPath,pPara->pCallbackFunc,g_DmCtrl.entity);
                DM_FREE(pFullPath);
            }

            /* indicate server entity */
            osa_mem_copy(pPara->entity,g_DmCtrl.entity,sizeof(pPara->entity));
        }

    	for(i = 0; i < pPara->lParaCnt; i++)
    	{
    	    /*get the database handler by name*/
    		pDmHandler = GetDmHandlerByName(pPara->paParaPath[i]);
    		if(NULL == pDmHandler)
    		{
                DM_ERROR(DM_ERR_PROCESSING_COMMON_ERROR, "DM Trace ------ DmRegisterNotifyHandlerReq(%s) : pDmHandler is null for %s",g_DmCtrl.entity,pPara->paParaPath[i]); 
    		    return DM_FALSE;
    		}
    		/*get the index of node*/
    		lIndex = DmGetIDFromPath(&(pDmHandler->hashNameID), pPara->paParaPath[i]);
    		if(lIndex < 0)
    		{
                DM_ERROR(DM_ERR_PROCESSING_COMMON_ERROR, "DM Trace ------ DmRegisterNotifyHandlerReq(%s) : cannot find the lIndex for %s",g_DmCtrl.entity,pPara->paParaPath[i]); 
    		    return DM_FALSE;
    		}
    		/*search the list of handler and check if registered*/
    		/*register the handler to database*/
            pCallbackPtr = (DM_NODE_NOTIFY_HANDLER_T*)calloc(1, sizeof(DM_NODE_NOTIFY_HANDLER_T));
            
            if (pCallbackPtr == NULL)
            {
            
                DM_ERROR(DM_ERR_MEMORY_ALLOCATION_FAILED, "DM Trace ------ DmRegisterNotifyHandlerReq(%s) : pCallbackPtr = NULL for %s",g_DmCtrl.entity,pPara->paParaPath[i]); 
            
                return DM_FALSE;
            }

            osa_mem_set(pCallbackPtr, 0, sizeof(DM_NODE_NOTIFY_HANDLER_T));
            
            strcpy(pCallbackPtr->entity, pPara->entity);

            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ prepending notify-node, notify-%s for %s in DmRegisterNotifyHandlerReq(%s)",pPara->entity,pPara->paParaPath[i],g_DmCtrl.entity);

    		pCallbackPtr->pNext = pDmHandler->pPattern[lIndex].pNotifyHandler;            
            pDmHandler->pPattern[lIndex].pNotifyHandler = pCallbackPtr;

    	}
	}

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmRegisterNotifyHandlerReq(%s)",g_DmCtrl.entity);	

    return DM_TRUE;
}
BOOL8  DmDeregisterNotifyHandlerReq (DM_NOTIFY_HANDLER_REG_PARA_T *pPara) 
{
    INT32 lRet = DM_RET_SUCCESS;
    BOOL8 bRet = DM_FALSE;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmDeregisterNotifyHandlerReq(%s)",g_DmCtrl.entity); 

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        UINT32 cnt = 0;
        UINT32 msgBodyLength = 0;
        INT8 *pMsgBody = NULL;

        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;
        INT8 *pFullPath = NULL;

        /* client things */
        /* compute the size of msg-body */
        msgBodyLength = sizeof(DM_NOTIFY_HANDLER_REG_PARA_T);
        msgBodyLength += sizeof(INT8*)*(pPara->lParaCnt);

        for (cnt=0; cnt<pPara->lParaCnt; cnt++)
        {
            msgBodyLength += strlen(pPara->paParaPath[cnt])+1;
        }

        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmDeregisterNotifyHandlerReq(%s)",g_DmCtrl.entity);
            bRet = DM_FALSE;
            return bRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        pSockMsgSnd->msg_id = DM_MSG_CODE_DeregisterNotifyHandlerReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->word_data = 0;
        pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = msgBodyLength;

        /* fill the msg-body */
        pMsgBody = (INT8*)(pSockMsgSnd+1);

        osa_mem_copy(pMsgBody,pPara,sizeof(DM_NOTIFY_HANDLER_REG_PARA_T));

        osa_mem_copy(pMsgBody, g_DmCtrl.entity, sizeof(g_DmCtrl.entity));/* fill the entity */

        pMsgBody += sizeof(DM_NOTIFY_HANDLER_REG_PARA_T);

        osa_mem_copy(pMsgBody,pPara->paParaPath,sizeof(INT8*)*(pPara->lParaCnt));
        pMsgBody += sizeof(INT8*)*(pPara->lParaCnt);

        for (cnt=0; cnt<pPara->lParaCnt; cnt++)
        {
            strcpy(pMsgBody, pPara->paParaPath[cnt]);
            pMsgBody += strlen(pPara->paParaPath[cnt])+1;
        }

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmDeregisterNotifyHandlerReq(%s)",g_DmCtrl.entity);
            bRet = DM_FALSE;
            return bRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_DeregisterNotifyHandlerCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_DeregisterNotifyHandlerCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_DeregisterNotifyHandlerCnf)
        {
            bRet = (BOOL8)(pSockMsgRcv->word_data);

            /* update only if server ok */
            if (bRet == DM_TRUE)
            {
                for (cnt=0; cnt<pPara->lParaCnt; cnt++)
                {
                    /* Delete the notify-cb in notify-hash-table */
                    pFullPath = DmChangeOrgNameToFullname(pPara->paParaPath[cnt]);
                    DmHashDel(&g_DmCtrl.notifyFuncHash,pFullPath);
                    DM_FREE(pFullPath);
                }
            }

            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmDeregisterNotifyHandlerReq(%s)",g_DmCtrl.entity);

        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmDeregisterNotifyHandlerReq(%s)",g_DmCtrl.entity);
            bRet = DM_FALSE;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {
        DM_HANDLER_T             *pDmHandler    = NULL;
    	INT32                     lIndex        = 0;
    	INT32                     i             = 0;
    	DM_NODE_NOTIFY_HANDLER_T *pCallbackPtr = NULL;
    	DM_NODE_NOTIFY_HANDLER_T *pCallbackPtrB= NULL;
        INT8                     *pFullPath     = NULL;

    	if(NULL == pPara)
    	{
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ NULL == pPara in DmDeregisterNotifyHandlerReq(%s)",g_DmCtrl.entity);
    	    return DM_FALSE;
    	}

        /* if it is the request from the server */
        if (strlen(pPara->entity)==0)
        {
            strcpy(pPara->entity,g_DmCtrl.entity);
        }

    	for(i = 0; i < pPara->lParaCnt; i++)
    	{
    	    /*get the database handler by name*/
    		pDmHandler = GetDmHandlerByName(pPara->paParaPath[i]);
    		if(NULL == pDmHandler)
    		{
    		    return DM_FALSE;
    		}
    		/*get the index of node*/
    		lIndex = DmGetIDFromPath(&(pDmHandler->hashNameID), pPara->paParaPath[i]);
    		if(lIndex < 0)
    		{
    		    return DM_FALSE;
    		}
    		/*search the list of handler and check if registered*/
    		/*deregister the handler to database*/
    		pCallbackPtr = pDmHandler->pPattern[lIndex].pNotifyHandler;
    		if(NULL == pCallbackPtr)
    		{
    		    continue;
    		}
    		else
    		{
    		    /*free the callback node in the list*/
    		    pCallbackPtrB = pCallbackPtr->pNext;
    		    while(pCallbackPtr->pNext != NULL)
    	    	{
    	    	    if(strcmp(pCallbackPtr->entity,pPara->entity)==0)
        	    	{
        	    	    pCallbackPtrB->pNext = pCallbackPtr->pNext;
    					DM_FREE(pCallbackPtr);
        	    	    break;
        	    	}
    				pCallbackPtrB = pCallbackPtr;
    				pCallbackPtr = pCallbackPtr->pNext;
    	    	}
    			
    		}
            
    	}

        /* it is the server */
        if (strcmp(pPara->entity,g_DmCtrl.entity)==0)
        {
            /* update only if notify-handler deleted */
            for (i=0; i<pPara->lParaCnt; i++)
            {
                /* Delete the notify-cb in notify-hash-table */
                pFullPath = DmChangeOrgNameToFullname(pPara->paParaPath[i]);
                DmHashDel(&g_DmCtrl.notifyFuncHash,pFullPath);
                DM_FREE(pFullPath);
            }
        }
    }

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmDeregisterNotifyHandlerReq(%s)",g_DmCtrl.entity);	

    return DM_TRUE;
}

BOOL8 DmRegisterGetHandlerReq  ( DM_REG_GET_HANDLER_PARA_T *  pPara )  
{
    INT32 lRet = DM_RET_SUCCESS;
    BOOL8 bRet = DM_FALSE;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmRegisterGetHandlerReq(%s)",g_DmCtrl.entity); 

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        UINT32 cnt = 0;
        UINT32 msgBodyLength = 0;
        INT8 *pMsgBody = NULL;

        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;
        INT8         *pFullPath  = NULL;

        /* client things */
        /* compute the size of msg-body */
        msgBodyLength = sizeof(DM_REG_GET_HANDLER_PARA_T);
        msgBodyLength += sizeof(INT8*)*(pPara->lParaCnt);

        for (cnt=0; cnt<pPara->lParaCnt; cnt++)
        {
            msgBodyLength += strlen(pPara->paParaPath[cnt])+1;
        }

        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmRegisterGetHandlerReq(%s)",g_DmCtrl.entity);
            bRet = DM_FALSE;
            return bRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        pSockMsgSnd->msg_id = DM_MSG_CODE_RegisterGetHandlerReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->word_data = 0;
        pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = msgBodyLength;

        /* fill the msg-body */
        pMsgBody = (INT8*)(pSockMsgSnd+1);

        osa_mem_copy(pMsgBody,pPara,sizeof(DM_REG_GET_HANDLER_PARA_T));

        osa_mem_copy(pMsgBody, g_DmCtrl.entity, sizeof(g_DmCtrl.entity));/* fill the entity */

        pMsgBody += sizeof(DM_REG_GET_HANDLER_PARA_T);

        osa_mem_copy(pMsgBody,pPara->paParaPath,sizeof(INT8*)*(pPara->lParaCnt));
        pMsgBody += sizeof(INT8*)*(pPara->lParaCnt);

        for (cnt=0; cnt<pPara->lParaCnt; cnt++)
        {
            strcpy(pMsgBody, pPara->paParaPath[cnt]);
            pMsgBody += strlen(pPara->paParaPath[cnt])+1;
        }

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmRegisterGetHandlerReq(%s)",g_DmCtrl.entity);
            bRet = DM_FALSE;
            return bRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_RegisterGetHandlerCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_RegisterGetHandlerCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_RegisterGetHandlerCnf)
        {
            bRet = (BOOL8)(pSockMsgRcv->word_data);

            /* update only if server ok */
            if (bRet == DM_TRUE)
            {
                for (cnt=0; cnt<pPara->lParaCnt; cnt++)
                {
                    /* Add the notify-cb to notify-hash-table */
                    pFullPath = DmChangeOrgNameToFullname(pPara->paParaPath[cnt]);
                    DmHashAdd(&g_DmCtrl.getFuncHash,pFullPath,(void*)pPara->pCallbackFunc);
                    DM_FREE(pFullPath);
                }
            }

            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmRegisterGetHandlerReq(%s)",g_DmCtrl.entity);

        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmRegisterGetHandlerReq(%s)",g_DmCtrl.entity);
            bRet = DM_FALSE;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {

    	DM_HANDLER_T *pDmHandler = NULL;
    	INT32         lIndex     = 0;
    	INT32         i          = 0;
        INT8         *pFullPath  = NULL;

    	if(NULL == pPara)
    	{
    	    return DM_FALSE;
    	}

        DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ handle get-register from %s in DmRegisterGetHandlerReq(%s)",pPara->entity,g_DmCtrl.entity);

        /* keep the get-cb if server */
        if (strlen(pPara->entity)==0 || strcmp(pPara->entity,g_DmCtrl.entity)==0)
        {
            for(i = 0; i < pPara->lParaCnt; i++)
            {
                pFullPath = DmChangeOrgNameToFullname(pPara->paParaPath[i]);
                DmHashAdd(&g_DmCtrl.getFuncHash,pFullPath,(void*)pPara->pCallbackFunc);
                DM_FREE(pFullPath);
            }

            /* indicate server entity */
            osa_mem_copy(pPara->entity,g_DmCtrl.entity,sizeof(pPara->entity));
        }

    	for(i = 0; i < pPara->lParaCnt; i++)
    	{
    		/*get the database handler by name*/
    		pDmHandler = GetDmHandlerByName(pPara->paParaPath[i]);
    		if(NULL == pDmHandler)
    		{
    		    return DM_FALSE;
    		}

    		/*get the index of node*/
    		lIndex = DmGetIDFromPath(&(pDmHandler->hashNameID), pPara->paParaPath[i]);
    		if(lIndex < 0)
    		{
    		    return DM_FALSE;
    		}
    		/*register the handler to database*/
    		pDmHandler->pPattern[lIndex].pGetHandler = (DM_NODE_GET_HANDLER_T*)DM_MALLOC(sizeof(DM_NODE_GET_HANDLER_T));

            if (pDmHandler->pPattern[lIndex].pGetHandler == NULL)
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pGetHandler malloc failed in DmRegisterGetHandlerReq(%s)",g_DmCtrl.entity);
                bRet = DM_FALSE;
            }
            else
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ register Get-from-%s for %s in DmRegisterGetHandlerReq(%s)", pPara->entity,pPara->paParaPath[i],g_DmCtrl.entity);	
            }

            osa_mem_copy(pDmHandler->pPattern[lIndex].pGetHandler->entity, pPara->entity, sizeof(pPara->entity));
    	}
    }

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmRegisterGetHandlerReq(%s)",g_DmCtrl.entity);	

    return DM_TRUE;
}

BOOL8 DmDeregisterGetHandlerReq (DM_REG_GET_HANDLER_PARA_T *pPara)  
{
    INT32 lRet = DM_RET_SUCCESS;
    BOOL8 bRet = DM_FALSE;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmDeregisterGetHandlerReq(%s)",g_DmCtrl.entity);

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        UINT32 cnt = 0;
        UINT32 msgBodyLength = 0;
        INT8 *pMsgBody = NULL;

        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;
        INT8 *pFullPath = NULL;

        /* client things */
        /* compute the size of msg-body */
        msgBodyLength = sizeof(DM_REG_GET_HANDLER_PARA_T);
        msgBodyLength += sizeof(INT8*)*(pPara->lParaCnt);

        for (cnt=0; cnt<pPara->lParaCnt; cnt++)
        {
            msgBodyLength += strlen(pPara->paParaPath[cnt])+1;
        }

        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmDeregisterGetHandlerReq(%s)",g_DmCtrl.entity);
            bRet = DM_FALSE;
            return bRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        pSockMsgSnd->msg_id = DM_MSG_CODE_DeregisterGetHandlerReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->word_data = 0;
        pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = msgBodyLength;

        /* fill the msg-body */
        pMsgBody = (INT8*)(pSockMsgSnd+1);

        osa_mem_copy(pMsgBody,pPara,sizeof(DM_REG_GET_HANDLER_PARA_T));

        osa_mem_copy(pMsgBody, g_DmCtrl.entity, sizeof(g_DmCtrl.entity));/* fill the entity */

        pMsgBody += sizeof(DM_REG_GET_HANDLER_PARA_T);

        osa_mem_copy(pMsgBody,pPara->paParaPath,sizeof(INT8*)*(pPara->lParaCnt));
        pMsgBody += sizeof(INT8*)*(pPara->lParaCnt);

        for (cnt=0; cnt<pPara->lParaCnt; cnt++)
        {
            strcpy(pMsgBody, pPara->paParaPath[cnt]);
            pMsgBody += strlen(pPara->paParaPath[cnt])+1;
        }

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmDeregisterGetHandlerReq(%s)",g_DmCtrl.entity);
            bRet = DM_FALSE;
            return bRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_DeregisterGetHandlerCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_DeregisterGetHandlerCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_DeregisterGetHandlerCnf)
        {
            bRet = (BOOL8)(pSockMsgRcv->word_data);

            /* update only if server ok */
            if (bRet == DM_TRUE)
            {
                for (cnt=0; cnt<pPara->lParaCnt; cnt++)
                {
                    /* Delete the notify-cb from notify-hash-table */
                    pFullPath = DmChangeOrgNameToFullname(pPara->paParaPath[cnt]);
                    DmHashDel(&g_DmCtrl.getFuncHash,pFullPath);
                    DM_FREE(pFullPath);
                }
            }

            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmDeregisterGetHandlerReq(%s)",g_DmCtrl.entity);

        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmDeregisterGetHandlerReq(%s)",g_DmCtrl.entity);
            bRet = DM_FALSE;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {


        DM_HANDLER_T *pDmHandler = NULL;
    	INT32         lIndex     = 0;
    	INT32         i          = 0;
        INT8         *pFullPath  = NULL;
	
    	if(NULL == pPara)
    	{
    	    return DM_FALSE;
    	}

        /* in case it is the request from server */
        if (strlen(pPara->entity)==0)
        {
            strcpy(pPara->entity,g_DmCtrl.entity);
        }

        DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ handle get-de-register from %s in DmDeregisterGetHandlerReq(%s)",pPara->entity,g_DmCtrl.entity);

    	for(i = 0; i < pPara->lParaCnt; i++)
    	{
    		/*get the database handler by name*/
    		pDmHandler = GetDmHandlerByName(pPara->paParaPath[i]);
    		if(NULL == pDmHandler)
    		{
    		    return DM_FALSE;
    		}

    		/*get the index of node*/
    		lIndex = DmGetIDFromPath(&(pDmHandler->hashNameID), pPara->paParaPath[i]);
    		if(lIndex < 0)
    		{
    		    return DM_FALSE;
    		}
    		/*deregister the handler to database*/
    		if(pDmHandler->pPattern[lIndex].pGetHandler != NULL && strcmp(pDmHandler->pPattern[lIndex].pGetHandler->entity, pPara->entity)==0)
    		{
                DM_FREE(pDmHandler->pPattern[lIndex].pGetHandler);
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ de-register Get-from-%s for %s in DmRegisterGetHandlerReq(%s)", pPara->entity,pPara->paParaPath[i],g_DmCtrl.entity);	
    		}
    	}

        if (strcmp(pPara->entity,g_DmCtrl.entity)==0)
        {
            for (i=0; i<pPara->lParaCnt; i++)
            {
                /* Delete the notify-cb from notify-hash-table */
                pFullPath = DmChangeOrgNameToFullname(pPara->paParaPath[i]);
                DmHashDel(&g_DmCtrl.getFuncHash,pFullPath);
                DM_FREE(pFullPath);
            }
        }
    }

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmDeregisterGetHandlerReq(%s)",g_DmCtrl.entity);	

    return DM_TRUE;
}

BOOL8 DmRegisterSetHandlerReq  ( DM_REG_SET_HANDLER_PARA_T *  pPara )  
{
    INT32 lRet = DM_RET_SUCCESS;
    BOOL8 bRet = DM_FALSE;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmRegisterSetHandlerReq(%s)",g_DmCtrl.entity); 

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        UINT32 cnt = 0;
        UINT32 msgBodyLength = 0;
        INT8 *pMsgBody = NULL;

        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;
        INT8         *pFullPath  = NULL;

        /* client things */
        /* compute the size of msg-body */
        msgBodyLength = sizeof(DM_REG_SET_HANDLER_PARA_T);
        msgBodyLength += sizeof(INT8*)*(pPara->lParaCnt);

        for (cnt=0; cnt<pPara->lParaCnt; cnt++)
        {
            msgBodyLength += strlen(pPara->paParaPath[cnt])+1;
        }

        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmRegisterSetHandlerReq(%s)",g_DmCtrl.entity);
            bRet = DM_FALSE;
            return bRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        pSockMsgSnd->msg_id = DM_MSG_CODE_RegisterSetHandlerReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->word_data = 0;
        pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = msgBodyLength;

        /* fill the msg-body */
        pMsgBody = (INT8*)(pSockMsgSnd+1);

        osa_mem_copy(pMsgBody,pPara,sizeof(DM_REG_SET_HANDLER_PARA_T));

        osa_mem_copy(pMsgBody, g_DmCtrl.entity, sizeof(g_DmCtrl.entity));/* fill the entity */

        pMsgBody += sizeof(DM_REG_SET_HANDLER_PARA_T);

        osa_mem_copy(pMsgBody,pPara->paParaPath,sizeof(INT8*)*(pPara->lParaCnt));
        pMsgBody += sizeof(INT8*)*(pPara->lParaCnt);

        for (cnt=0; cnt<pPara->lParaCnt; cnt++)
        {
            strcpy(pMsgBody, pPara->paParaPath[cnt]);
            pMsgBody += strlen(pPara->paParaPath[cnt])+1;
        }

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmRegisterSetHandlerReq(%s)",g_DmCtrl.entity);
            bRet = DM_FALSE;
            return bRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_RegisterSetHandlerCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_RegisterSetHandlerCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_RegisterSetHandlerCnf)
        {
            bRet = (BOOL8)(pSockMsgRcv->word_data);

            /* update only if server ok */
            if (bRet == DM_TRUE)
            {
                for (cnt=0; cnt<pPara->lParaCnt; cnt++)
                {
                    /* Add the notify-cb to notify-hash-table */
                    pFullPath = DmChangeOrgNameToFullname(pPara->paParaPath[cnt]);
                    DmHashAdd(&g_DmCtrl.setFuncHash,pFullPath,(void*)pPara->pCallbackFunc);
                    DM_FREE(pFullPath);
                }
            }

            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmRegisterSetHandlerReq(%s)",g_DmCtrl.entity);

        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmRegisterSetHandlerReq(%s)",g_DmCtrl.entity);
            bRet = DM_FALSE;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {

    	DM_HANDLER_T *pDmHandler = NULL;
    	INT32         lIndex     = 0;
    	INT32         i          = 0;
        INT8         *pFullPath  = NULL;

    	if(NULL == pPara)
    	{
    	    return DM_FALSE;
    	}

        DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ handle set-register from %s in DmRegisterSetHandlerReq(%s)",pPara->entity,g_DmCtrl.entity);

        /* keep the get-cb if server */
        if (strlen(pPara->entity)==0 || strcmp(pPara->entity,g_DmCtrl.entity)==0)
        {
            for(i = 0; i < pPara->lParaCnt; i++)
            {
                pFullPath = DmChangeOrgNameToFullname(pPara->paParaPath[i]);
                DmHashAdd(&g_DmCtrl.setFuncHash,pFullPath,(void*)pPara->pCallbackFunc);
                DM_FREE(pFullPath);
            }

            /* indicate server entity */
            osa_mem_copy(pPara->entity,g_DmCtrl.entity,sizeof(pPara->entity));
        }

    	for(i = 0; i < pPara->lParaCnt; i++)
    	{
    		/*get the database handler by name*/
    		pDmHandler = GetDmHandlerByName(pPara->paParaPath[i]);
    		if(NULL == pDmHandler)
    		{
    		    return DM_FALSE;
    		}

    		/*get the index of node*/
    		lIndex = DmGetIDFromPath(&(pDmHandler->hashNameID), pPara->paParaPath[i]);
    		if(lIndex < 0)
    		{
    		    return DM_FALSE;
    		}
    		/*register the handler to database*/
    		pDmHandler->pPattern[lIndex].pSetHandler = (DM_NODE_SET_HANDLER_T*)DM_MALLOC(sizeof(DM_NODE_SET_HANDLER_T));

            if (pDmHandler->pPattern[lIndex].pSetHandler == NULL)
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pSetHandler malloc failed in DmRegisterSetHandlerReq(%s)",g_DmCtrl.entity);
                bRet = DM_FALSE;
            }
            else
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ register Set-from-%s for %s in DmRegisterSetHandlerReq(%s)", pPara->entity,pPara->paParaPath[i],g_DmCtrl.entity);	
            }

            osa_mem_copy(pDmHandler->pPattern[lIndex].pSetHandler->entity, pPara->entity, sizeof(pPara->entity));
    	}
    }

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmRegisterSetHandlerReq(%s)",g_DmCtrl.entity);	

    return DM_TRUE;
}

BOOL8 DmDeregisterSetHandlerReq (DM_REG_SET_HANDLER_PARA_T *pPara)  
{
    INT32 lRet = DM_RET_SUCCESS;
    BOOL8 bRet = DM_FALSE;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmDeregisterSetHandlerReq(%s)",g_DmCtrl.entity);

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        UINT32 cnt = 0;
        UINT32 msgBodyLength = 0;
        INT8 *pMsgBody = NULL;

        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;
        INT8 *pFullPath = NULL;

        /* client things */
        /* compute the size of msg-body */
        msgBodyLength = sizeof(DM_REG_SET_HANDLER_PARA_T);
        msgBodyLength += sizeof(INT8*)*(pPara->lParaCnt);

        for (cnt=0; cnt<pPara->lParaCnt; cnt++)
        {
            msgBodyLength += strlen(pPara->paParaPath[cnt])+1;
        }

        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmDeregisterSetHandlerReq(%s)",g_DmCtrl.entity);
            bRet = DM_FALSE;
            return bRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        pSockMsgSnd->msg_id = DM_MSG_CODE_DeregisterSetHandlerReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->word_data = 0;
        pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = msgBodyLength;

        /* fill the msg-body */
        pMsgBody = (INT8*)(pSockMsgSnd+1);

        osa_mem_copy(pMsgBody,pPara,sizeof(DM_REG_SET_HANDLER_PARA_T));

        osa_mem_copy(pMsgBody, g_DmCtrl.entity, sizeof(g_DmCtrl.entity));/* fill the entity */

        pMsgBody += sizeof(DM_REG_SET_HANDLER_PARA_T);

        osa_mem_copy(pMsgBody,pPara->paParaPath,sizeof(INT8*)*(pPara->lParaCnt));
        pMsgBody += sizeof(INT8*)*(pPara->lParaCnt);

        for (cnt=0; cnt<pPara->lParaCnt; cnt++)
        {
            strcpy(pMsgBody, pPara->paParaPath[cnt]);
            pMsgBody += strlen(pPara->paParaPath[cnt])+1;
        }

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmDeregisterSetHandlerReq(%s)",g_DmCtrl.entity);
            bRet = DM_FALSE;
            return bRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_DeregisterSetHandlerCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_DeregisterSetHandlerCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_DeregisterSetHandlerCnf)
        {
            bRet = (BOOL8)(pSockMsgRcv->word_data);

            /* update only if server ok */
            if (bRet == DM_TRUE)
            {
                for (cnt=0; cnt<pPara->lParaCnt; cnt++)
                {
                    /* Delete the notify-cb from notify-hash-table */
                    pFullPath = DmChangeOrgNameToFullname(pPara->paParaPath[cnt]);
                    DmHashDel(&g_DmCtrl.setFuncHash,pFullPath);
                    DM_FREE(pFullPath);
                }
            }

            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmDeregisterSetHandlerReq(%s)",g_DmCtrl.entity);

        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmDeregisterSetHandlerReq(%s)",g_DmCtrl.entity);
            bRet = DM_FALSE;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {


        DM_HANDLER_T *pDmHandler = NULL;
    	INT32         lIndex     = 0;
    	INT32         i          = 0;
        INT8         *pFullPath  = NULL;
	
    	if(NULL == pPara)
    	{
    	    return DM_FALSE;
    	}

        /* in case it is the request from server */
        if (strlen(pPara->entity)==0)
        {
            strcpy(pPara->entity,g_DmCtrl.entity);
        }

        DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ handle set-de-register from %s in DmDeregisterSetHandlerReq(%s)",pPara->entity,g_DmCtrl.entity);

    	for(i = 0; i < pPara->lParaCnt; i++)
    	{
    		/*get the database handler by name*/
    		pDmHandler = GetDmHandlerByName(pPara->paParaPath[i]);
    		if(NULL == pDmHandler)
    		{
    		    return DM_FALSE;
    		}

    		/*get the index of node*/
    		lIndex = DmGetIDFromPath(&(pDmHandler->hashNameID), pPara->paParaPath[i]);
    		if(lIndex < 0)
    		{
    		    return DM_FALSE;
    		}
    		/*deregister the handler to database*/
    		if(pDmHandler->pPattern[lIndex].pSetHandler != NULL && strcmp(pDmHandler->pPattern[lIndex].pSetHandler->entity, pPara->entity)==0)
    		{
                DM_FREE(pDmHandler->pPattern[lIndex].pSetHandler);
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ de-register Set-from-%s for %s in DmRegisterSetHandlerReq(%s)", pPara->entity,pPara->paParaPath[i],g_DmCtrl.entity);	
    		}
    	}

        if (strcmp(pPara->entity,g_DmCtrl.entity)==0)
        {
            for (i=0; i<pPara->lParaCnt; i++)
            {
                /* Delete the notify-cb from notify-hash-table */
                pFullPath = DmChangeOrgNameToFullname(pPara->paParaPath[i]);
                DmHashDel(&g_DmCtrl.setFuncHash,pFullPath);
                DM_FREE(pFullPath);
            }
        }
    }

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmDeregisterSetHandlerReq(%s)",g_DmCtrl.entity);	

    return DM_TRUE;
}

INT32 DmOneNotifyHandlerCalling(INT8 *pEntity, DM_NOTIFY_T *pNotifyPara)
{
    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmOneNotifyHandlerCalling(%s) to notify %s ",g_DmCtrl.entity,pEntity);

    /* send params to client */
    if (strcmp(pEntity,g_DmCtrl.entity)!=0)
    {
        INT32 lRet = DM_RET_SUCCESS;

        UINT32 cnt = 0;
        UINT32 msgBodyLength = 0;
        INT8 *pMsgBody = NULL;

        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;


        /* client things */
        /* compute the size of msg-body */
        msgBodyLength = sizeof(DM_NOTIFY_T);
        msgBodyLength += sizeof(DM_NOTIFY_PARA_T)*(pNotifyPara->ulParaCount);

        for (cnt=0; cnt<pNotifyPara->ulParaCount; cnt++)
        {
            msgBodyLength += strlen(pNotifyPara->pCont[cnt].pucPath)+1;

            if (pNotifyPara->pCont[cnt].pucChangedAttrs)
            {
                msgBodyLength += strlen(pNotifyPara->pCont[cnt].pucChangedAttrs)+1;
            }
        }

        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc pSockMsgSnd failed in DmOneNotifyHandlerCalling(%s)",g_DmCtrl.entity);
            return DM_RET_INTERNAL_ERROR;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        pSockMsgSnd->msg_id = DM_MSG_CODE_NotifyHandlerReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(pEntity);
        pSockMsgSnd->word_data = 0;
        pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = msgBodyLength;

        /* fill the msg-body */
        pMsgBody = (INT8*)(pSockMsgSnd+1);

        osa_mem_copy(pMsgBody,pNotifyPara,sizeof(DM_NOTIFY_T));
        pMsgBody += sizeof(DM_NOTIFY_T);

        osa_mem_copy(pMsgBody,pNotifyPara->pCont,sizeof(DM_NOTIFY_PARA_T)*(pNotifyPara->ulParaCount));
        pMsgBody += sizeof(DM_NOTIFY_PARA_T)*(pNotifyPara->ulParaCount);

        for (cnt=0; cnt<pNotifyPara->ulParaCount; cnt++)
        {
            strcpy(pMsgBody, pNotifyPara->pCont[cnt].pucPath);
            pMsgBody += strlen(pNotifyPara->pCont[cnt].pucPath)+1;

            if (pNotifyPara->pCont[cnt].pucChangedAttrs)
            {
                strcpy(pMsgBody, pNotifyPara->pCont[cnt].pucChangedAttrs);
                pMsgBody += strlen(pNotifyPara->pCont[cnt].pucChangedAttrs)+1;
            }
        }

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to %s failed in DmOneNotifyHandlerCalling(%s)",pEntity,g_DmCtrl.entity);
            return DM_RET_INTERNAL_ERROR;
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send DM_MSG_CODE_NotifyHandlerReq successfully in DmOneNotifyHandlerCalling(%s)",g_DmCtrl.entity);
        }

        DM_FREE(pSockMsgSnd);

        /* no need to wait for the response */
    }
    else
    {
        if (pNotifyPara->bAttrNotify==DM_TRUE)
        {
            if (g_DmCtrl.pAttrNotifyCb)
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ call attr-notify-cb to handle notify-list in DmOneNotifyHandlerCalling(%s)",g_DmCtrl.entity);
                g_DmCtrl.pAttrNotifyCb(pNotifyPara);
            }
            else
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ no attr-notify-cb here, ingored, in DmOneNotifyHandlerCalling(%s)",g_DmCtrl.entity);
            }
        }
        else
        {
            UINT32 cnt = 0;
            
            DM_HASH_T tmpHash;
            DM_HASH_ENUM_T tmpEnum;
            DM_NOTIFY_T tmpNotifyPara;
            DM_NOTIFY_CALLBACK_FUN pNotifyCb = NULL;
            INT8 *pNotifyCbStr = NULL;
            INT32 paraCnt = 0;
            DM_NOTIFY_CB_NODE_T *pCbNode = NULL;
            
            osa_mem_set(&tmpHash, 0, sizeof(DM_HASH_T));
            osa_mem_set(&tmpEnum, 0, sizeof(DM_HASH_ENUM_T));

            DmHashCreate(&tmpHash,DM_HASH_INIT_SIZE);

            /* counting notify-CBs */
            for (cnt=0; cnt<pNotifyPara->ulParaCount; cnt++)
            {
                pCbNode = DmHashGetFromPath(&g_DmCtrl.notifyFuncHash, pNotifyPara->pCont[cnt].pucPath);

                while (pCbNode != NULL)
                {
                    pNotifyCbStr = DmUintToStr((UINT32)pCbNode->pCb);
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ find 1 cb addr=0x%x str=%s in DmOneNotifyHandlerCalling(%s)",pCbNode->pCb,pNotifyCbStr,g_DmCtrl.entity);

                    if (NULL == DmHashGetValue(&tmpHash,pNotifyCbStr))
                    {
                        DmHashAdd(&tmpHash,pNotifyCbStr,pCbNode->pCb);
                    }

                    DM_FREE(pNotifyCbStr);

                    pCbNode = pCbNode->pNext;
                }
            }

            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ %d notify-CBs to be called in DmOneNotifyHandlerCalling(%s)",DmHashGetUsedsize(&tmpHash),g_DmCtrl.entity);

            /* run notify-CBs */
            while(NULL != (pNotifyCb = DmHashEnumerator(&tmpHash,&tmpEnum)))
            {
                paraCnt = 0;
                osa_mem_set(&tmpNotifyPara, 0, sizeof(DM_NOTIFY_T));

                for (cnt=0; cnt<pNotifyPara->ulParaCount; cnt++)
                {
                    pCbNode = DmHashGetFromPath(&g_DmCtrl.notifyFuncHash, pNotifyPara->pCont[cnt].pucPath);

                    while (pCbNode != NULL)
                    {
                        if (pCbNode->pCb == pNotifyCb)
                        {
                            paraCnt++;
                            break;
                        }

                        pCbNode = pCbNode->pNext;
                    }
                }

                DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ %d params need to call notify-cb 0x%x in DmOneNotifyHandlerCalling(%s)",paraCnt,pNotifyCb,g_DmCtrl.entity);

                osa_mem_copy(tmpNotifyPara.Modifier,pNotifyPara->Modifier, sizeof(tmpNotifyPara.Modifier));
                tmpNotifyPara.ulParaCount = paraCnt;
                tmpNotifyPara.pCont = (DM_NOTIFY_PARA_T*)DM_MALLOC(sizeof(DM_NOTIFY_PARA_T)*paraCnt);

                if (tmpNotifyPara.pCont == NULL)
                {
                    /* malloc error */
                    DmHashDestroy(&tmpHash, DM_TRUE);
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc tmpNotifyPara.pCont failed in DmOneNotifyHandlerCalling(%s)",g_DmCtrl.entity);
                    return DM_RET_INTERNAL_ERROR;
                }

                osa_mem_set(tmpNotifyPara.pCont,0,sizeof(DM_NOTIFY_PARA_T));

                paraCnt = 0;
                for (cnt=0; cnt<pNotifyPara->ulParaCount; cnt++)
                {
                    pCbNode =  DmHashGetFromPath(&g_DmCtrl.notifyFuncHash, pNotifyPara->pCont[cnt].pucPath);

                    while (pCbNode != NULL)
                    {
                        if (pCbNode->pCb == pNotifyCb)
                        {
                            tmpNotifyPara.pCont[paraCnt] = pNotifyPara->pCont[cnt];
                            paraCnt++;
                            break;
                        }

                        pCbNode = pCbNode->pNext;
                    }
                }

                /* run notify-callback */
                DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ calling one notify-hanlder 0x%x in DmOneNotifyHandlerCalling(%s)",pNotifyCb,g_DmCtrl.entity);
                pNotifyCb(&tmpNotifyPara);

                DM_FREE(tmpNotifyPara.pCont);
            }

            DmHashDestroy(&tmpHash, DM_FALSE);
        }
    }

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmOneNotifyHandlerCalling(%s)",g_DmCtrl.entity);

    return DM_RET_SUCCESS;
}

INT32 DmGetHandlerCalling(DM_NODE_GET_HANDLER_T *pGetHandler, DM_GET_VALUE_T *pPara)
{
    INT32 lRet = DM_RET_SUCCESS;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmGetHandlerCalling(%s) for get-entity(%s) param (%s)",g_DmCtrl.entity,pGetHandler->entity,pPara->pucPath);

    /* send params to required entity */
    if (strcmp(pGetHandler->entity,g_DmCtrl.entity)!=0)
    {

        UINT32 msgBodyLength = 0;
        INT8 *pMsgBody = NULL;
        INT8 *pTmp = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;


        /* client things */
        /* compute the size of msg-body */
        msgBodyLength = sizeof(DM_GET_VALUE_T) + pPara->ValueSize;
        msgBodyLength += strlen(pPara->pucPath) + 1;

        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc pSockMsgSnd failed in DmGetHandlerCalling(%s)",g_DmCtrl.entity);
            return DM_RET_INTERNAL_ERROR;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        pSockMsgSnd->msg_id = DM_MSG_CODE_GetHandlerReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(pGetHandler->entity);
        pSockMsgSnd->word_data = 0;
        pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = msgBodyLength;

        /* fill the msg-body */
        pMsgBody = (INT8*)(pSockMsgSnd+1);

        osa_mem_copy(pMsgBody,pPara,sizeof(DM_GET_VALUE_T));
        pMsgBody += sizeof(DM_GET_VALUE_T);

        strcpy(pMsgBody,pPara->pucPath);
        pMsgBody += strlen(pPara->pucPath) + 1;

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to %s failed in DmGetHandlerCalling(%s)",pGetHandler->entity,g_DmCtrl.entity);
            return DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_GetHandlerCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_GetHandlerCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_GetHandlerCnf)
        {
            lRet = pSockMsgRcv->word_data;

            /* copy the results */
            pTmp = (INT8*)((DM_GET_VALUE_T *)(pSockMsgRcv+1)+1);
            pTmp += strlen(pPara->pucPath)+1;

            osa_mem_copy(pPara->pValue, pTmp, pPara->ValueSize);

            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmGetHandlerCalling(%s)",g_DmCtrl.entity);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmGetHandlerCalling(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);

    }
    else
    {
        DM_GET_CALLBACK_FUN pGetCb = NULL;

        /* counting notify-CBs */
        pGetCb = DmHashGetFromPath(&g_DmCtrl.getFuncHash, pPara->pucPath);

        if (pGetCb != NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ Get-hooker for %s found in DmGetHandlerCalling(%s)",pPara->pucPath,g_DmCtrl.entity);

            lRet = pGetCb(pPara);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to find Get-hooker for %s in DmGetHandlerCalling(%s)",pPara->pucPath,g_DmCtrl.entity);
        }
    }

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmGetHandlerCalling(%s) lRet=%d",g_DmCtrl.entity,lRet);

    return lRet;

}

INT32 DmSetHandlerCalling(DM_NODE_SET_HANDLER_T *pSetHandler, DM_SET_VALUE_T *pPara)
{
    INT32 lRet = DM_RET_SUCCESS;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmSetHandlerCalling(%s) for set-entity(%s) param (%s)",g_DmCtrl.entity,pSetHandler->entity,pPara->pucPath);

    /* send params to required entity */
    if (strcmp(pSetHandler->entity,g_DmCtrl.entity)!=0)
    {

        UINT32 msgBodyLength = 0;
        INT8 *pMsgBody = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;


        /* client things */
        /* compute the size of msg-body */
        msgBodyLength = sizeof(DM_SET_VALUE_T) + pPara->ValueSize;
        msgBodyLength += strlen(pPara->pucPath) + 1;

        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc pSockMsgSnd failed in DmSetHandlerCalling(%s)",g_DmCtrl.entity);
            return DM_RET_INTERNAL_ERROR;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T)+msgBodyLength);

        pSockMsgSnd->msg_id = DM_MSG_CODE_SetHandlerReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(pSetHandler->entity);
        pSockMsgSnd->word_data = 0;
        pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = msgBodyLength;

        /* fill the msg-body */
        pMsgBody = (INT8*)(pSockMsgSnd+1);

        osa_mem_copy(pMsgBody,pPara,sizeof(DM_SET_VALUE_T));
        pMsgBody += sizeof(DM_SET_VALUE_T);

        strcpy(pMsgBody,pPara->pucPath);
        pMsgBody += strlen(pPara->pucPath) + 1;

        osa_mem_copy(pMsgBody,pPara->pValue,pPara->ValueSize);

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to %s failed in DmSetHandlerCalling(%s)",pSetHandler->entity,g_DmCtrl.entity);
            return DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_SetHandlerCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_SetHandlerCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_SetHandlerCnf)
        {
            lRet = pSockMsgRcv->word_data;

            DM_FREE(pSockMsgRcv);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmSetHandlerCalling(%s)",g_DmCtrl.entity);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmSetHandlerCalling(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);

    }
    else
    {
        DM_SET_CALLBACK_FUN pSetCb = NULL;

        /* counting notify-CBs */
        pSetCb = DmHashGetFromPath(&g_DmCtrl.setFuncHash, pPara->pucPath);

        if (pSetCb != NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ Set-hooker for %s found in DmSetHandlerCalling(%s)",pPara->pucPath,g_DmCtrl.entity);

            lRet = pSetCb(pPara);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to find Set-hooker for %s in DmSetHandlerCalling(%s)",pPara->pucPath,g_DmCtrl.entity);
        }
    }

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmSetHandlerCalling(%s) lRet=%d",g_DmCtrl.entity,lRet);

    return lRet;

}

BOOL8 DmCheckXmlFile(INT8 *pXmlFile)
{
    if (pXmlFile && access(pXmlFile, F_OK)==0)
    {
        xmlDocPtr pXmlDocument = xmlReadFile(pXmlFile, NULL, XML_PARSE_NOBLANKS);

        if (pXmlDocument != NULL)
        {
            xmlFreeDoc(pXmlDocument);
            pXmlDocument = NULL;

            return DM_TRUE;
        }
    }

    return DM_FALSE;
}

/* send the request to the server, let server allocate memory and return shm-id */
INT32 DmAllocateMemReq(UINT32 size, void **addr)
{
    DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

    if (size<=0)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid parameters, size(%d), in DmAllocateMemReq(%s)",size,g_DmCtrl.entity);
        return lRet;
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmAllocateMemReq(%s) size(%d)",g_DmCtrl.entity,size);

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;

        /* client things */
        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T));

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmAllocateMemReq(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T));

        pSockMsgSnd->msg_id = DM_MSG_CODE_AllocateMemoryReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->word_data = size;/* keep memory size */
        pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = 0;

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmReleaseMemReq(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_AllocateMemoryCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_AllocateMemoryCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_AllocateMemoryCnf)
        {
            OSA_SHM_T *pShmReq = (OSA_SHM_T *)(pSockMsgRcv+1);

			/* check allocated result from server side */
            lRet = pSockMsgRcv->word_data;
			if(lRet != DM_RET_SUCCESS)
			{
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to allocate memory in server side, in DmAllocateMemReq(%s)",g_DmCtrl.entity);
                lRet = DM_RET_INTERNAL_ERROR;
                return lRet;			
			}

            /* udpate shm addr in current process */
            lRet = osa_shm_create(pShmReq,pShmReq->key,pShmReq->size,NULL);

            if (lRet == OSA_FAILURE)
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ shared memory error, in DmAllocateMemReq(%s)",g_DmCtrl.entity);
                lRet = DM_RET_INTERNAL_ERROR;
                return lRet;
            }

            if (addr)
            {
                /* skip the header */
                *addr = (void*)((OSA_SHM_T *)(pShmReq->pshm_addr)+1);
                DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ *addr(0x%x) assigned, shm(id=0x%x,key=0x%x,size=0x%x(%d),size_bk=0x%x,addr=0x%x) in DmAllocateMemReq(%s)",(*addr),pShmReq->shm_id,pShmReq->key,pShmReq->size,pShmReq->size,pShmReq->size_bk,pShmReq->pshm_addr,g_DmCtrl.entity);

                pShmReq = (OSA_SHM_T *)pShmReq->pshm_addr;
                DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ check shm(id=0x%x,key=0x%x,size=0x%x(%d),size_bk=0x%x,addr=0x%x) in DmAllocateMemReq(%s)",pShmReq->shm_id,pShmReq->key,pShmReq->size,pShmReq->size,pShmReq->size_bk,pShmReq->pshm_addr,g_DmCtrl.entity);
            }

            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmAllocateMemReq(%s)",g_DmCtrl.entity);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmAllocateMemReq(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {
        OSA_SHM_T shmReq;

        osa_mem_set(&shmReq,0,sizeof(OSA_SHM_T));
        shmReq.size = size;

        lRet = osa_resource_wait(&g_DmCtrl.shmGroup, &shmReq, DM_SHM_MSG_TIMEOUT_MS);

        if (lRet == DM_SUCCESS)
        {
            lRet = DM_RET_SUCCESS;
            if (addr)
            {
                /* skip the header */
                *addr = (void*)((OSA_SHM_T *)(shmReq.pshm_addr)+1);
                DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ *addr(0x%x) assigned, shm(id=0x%x,key=0x%x,size=0x%x(%d),size_bk=0x%x,addr=0x%x) in DmAllocateMemReq(%s)",(*addr),shmReq.shm_id,shmReq.key,shmReq.size,shmReq.size,shmReq.size_bk,shmReq.pshm_addr,g_DmCtrl.entity);
            }
        }
        else
        {
            lRet = DM_RET_INTERNAL_ERROR;
        }
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmAllocateMemReq(%s) lRet=%d",g_DmCtrl.entity,lRet);

    return lRet;
}

/* tell the server shm-id to let server release memory */
INT32 DmReleaseMemReq(void **addr)
{
    DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

    if (addr==NULL || (*addr)==NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid parameters, addr(0x%x) *addr(0x%x), in DmReleaseMemReq(%s)",addr,(*addr),g_DmCtrl.entity);
        return lRet;
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmReleaseMemReq(%s) *addr(0x%x)",g_DmCtrl.entity,(*addr));

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;
        OSA_SHM_T *pShmReq = (OSA_SHM_T *)(*addr);
        pShmReq--;

        if (pShmReq->size != (pShmReq->size_bk^0xffffffff))
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid shm address 0x%x, shm(id=0x%x,key=0x%x,size=0x%x(%d),size_bk=0x%x,addr=0x%x) in DmReleaseMemReq(%s)",pShmReq,pShmReq->shm_id,pShmReq->key,pShmReq->size,pShmReq->size,pShmReq->size_bk,pShmReq->pshm_addr,g_DmCtrl.entity);
            return lRet;
        }

        /* client things */
        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sizeof(DM_SOCK_MSG_HEADER_T));

        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmReleaseMemReq(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sizeof(DM_SOCK_MSG_HEADER_T));

        pSockMsgSnd->msg_id = DM_MSG_CODE_ReleaseMemoryReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->word_data = (UINT32)pShmReq->pshm_addr;/* keep memory key */
        pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = 0;

		/* first detach this shm in client side */
		OSA_SHM_T clientShm = *pShmReq;
		clientShm.pshm_addr = (void *)pShmReq;
		lRet = osa_shm_delete(&clientShm, OSA_FALSE);
        if (lRet != DM_SUCCESS)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmShmGroupHandler(%s) failed to remove shm key=%d",g_DmCtrl.entity,pShmReq->key);
            return lRet;
        }

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ,pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmReleaseMemReq(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_ReleaseMemoryCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_ReleaseMemoryCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_ReleaseMemoryCnf)
        {
            lRet = pSockMsgRcv->word_data;

            if (lRet==DM_RET_SUCCESS && addr)
            {
                *addr = NULL;
            }

            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmReleaseMemReq(%s)",g_DmCtrl.entity);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmReleaseMemReq(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {
        OSA_SHM_T *pShmReq = (OSA_SHM_T *)(*addr)-1;

        lRet = osa_resource_release(&g_DmCtrl.shmGroup, pShmReq);

        if (lRet == DM_SUCCESS)
        {
            lRet = DM_RET_SUCCESS;
            if (addr)
            {
                *addr = NULL;
            }
        }
        else
        {
            lRet = DM_RET_INTERNAL_ERROR;
        }
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmReleaseMemReq(%s) lRet=%d",g_DmCtrl.entity,lRet);

    return lRet;
}

INT32 DmShmGroupHandler(void *pResourceData, OSA_RESOURCE_GRP_STATUS_E status, void *pExtraData)
{
    INT32 lRet = DM_FAILURE;
    DM_CTRL_T *pDmCtrl = (DM_CTRL_T *)pResourceData;
	INT8   shmNodeKey[24];

    switch(status)
    {
        case OSA_RESOURCE_WAIT:
        {
            OSA_SHM_T *pShmReq = (OSA_SHM_T *)pExtraData;
			UINT32 shm_key = 0;

            if (pShmReq)
            {
                if (pShmReq->size>0)
                {
                    if ((pShmReq->size+pDmCtrl->curShmSize) <= pDmCtrl->maxShmSize)
                    {
                        /* allocate shm key */
                        DM_MEM_REQ_NODE_T *pNode = (DM_MEM_REQ_NODE_T *)DM_MALLOC(sizeof(DM_MEM_REQ_NODE_T));
                        if (pNode==NULL)
                        {
                            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmShmGroupHandler(%s) status=%d pNode=null",g_DmCtrl.entity,status);
                            break;
                        }

                        osa_mem_set(pNode, 0, sizeof(DM_MEM_REQ_NODE_T));
			
						lRet = DmCreateShmKey(pNode->fileName, sizeof(pNode->fileName), &shm_key);
						if (lRet != DM_SUCCESS)
						{
							DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to create shared memory key in DmShmGroupHandler(%s)", g_DmCtrl.entity);
							DM_FREE(pNode);
							break;
						}

                        /* create shared memory */
                        pNode->shm.key = shm_key;
                        pNode->shm.size = pShmReq->size+sizeof(OSA_SHM_T);/* fill shm control info as header */

                        lRet = osa_shm_create(&pNode->shm,pNode->shm.key,pNode->shm.size,NULL);
                        if (lRet == OSA_FAILURE)
                        {
                            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to create shared memory with key=0x%x size=0x%x(%u) in DmShmGroupHandler(%s)",pNode->shm.key,pNode->shm.size,pNode->shm.size,g_DmCtrl.entity);
                            DM_FREE(pNode);
                            break;
                        }
                        else
                        {
                            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ shared memory created, shm(id=0x%x,key=0x%x,size=0x%x(%u),size_bk=0x%x,addr=0x%x), mem-status (%u/%u) in DmShmGroupHandler(%s)",pNode->shm.shm_id,pNode->shm.key,pNode->shm.size,pNode->shm.size,pNode->shm.size_bk,pNode->shm.pshm_addr,(g_DmCtrl.curShmSize+pNode->shm.size),g_DmCtrl.maxShmSize,g_DmCtrl.entity);

                            /* write shm info at the shm header */
                            osa_mem_copy(pNode->shm.pshm_addr,&pNode->shm,sizeof(OSA_SHM_T));
                        }

                        /* Add allocated shm to manage hash table */
						snprintf(shmNodeKey, sizeof(shmNodeKey), "%u", shm_key);
						DmHashAdd(&g_DmCtrl.shmInUseHash, shmNodeKey, pNode);
                        g_DmCtrl.curShmSize += pNode->shm.size;

                        /* copy shm info to req */
                        *pShmReq = pNode->shm;
                    }
                    else
                    {
                        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmShmGroupHandler(%s) not enough shared-memory left, pShmReq->size=%u pDmCtrl->curShmSize=%u pDmCtrl->maxShmSize=%u",g_DmCtrl.entity,pShmReq->size,pDmCtrl->curShmSize,pDmCtrl->maxShmSize);
                    }
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmShmGroupHandler(%s) status=%d pMemReq->size=0",g_DmCtrl.entity,status);
                }
            }
            else
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmShmGroupHandler(%s) status=%d pMemReq=null",g_DmCtrl.entity,status);
            }
            break;
        }
        case OSA_RESOURCE_RELEASE:
        {
            OSA_SHM_T *pShmReq = (OSA_SHM_T *)pExtraData;

            if (pShmReq)
            {
            	snprintf(shmNodeKey, sizeof(shmNodeKey), "%u", pShmReq->key);
                DM_MEM_REQ_NODE_T *pNode = DmHashDel(&g_DmCtrl.shmInUseHash, shmNodeKey);

                DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ try to release shared memory, shm(id=0x%x,key=0x%x,size=0x%x(%d),size_bk=0x%x,addr=0x%x) in DmShmGroupHandler(%s)",pShmReq->shm_id,pShmReq->key,pShmReq->size,pShmReq->size,pShmReq->size_bk,pShmReq->pshm_addr,g_DmCtrl.entity);

				if(pNode != NULL)
				{
	                /* remove shm */
					OSA_SHM_T serverShm = pNode->shm;
	                lRet = osa_shm_delete(&serverShm, OSA_TRUE);

	                if (lRet == OSA_FAILURE)
	                {
	                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmShmGroupHandler(%s) failed to remove shm key=%d",g_DmCtrl.entity,pNode->shm.key);
	                }
	                else
	                {
						g_DmCtrl.curShmSize -= pNode->shm.size;
						
	                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ shared memory removed (id=0x%x,key=0x%x,size=0x%x(%u),size_bk=0x%x,addr=0x%x), mem-status (%u/%u) in DmShmGroupHandler(%s)",pNode->shm.shm_id,pNode->shm.key,pNode->shm.size,pNode->shm.size,pNode->shm.size_bk,pNode->shm.pshm_addr,(g_DmCtrl.curShmSize-pNode->shm.size),g_DmCtrl.maxShmSize,g_DmCtrl.entity);

						/* remove the token file */
						DmReleaseShmKey(pNode->fileName, pNode->shm.key);
						DM_FREE(pNode);
	                    lRet = DM_SUCCESS;
	                }
				}
            }
            else
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmShmGroupHandler(%s) status=%d pMemReq=null",g_DmCtrl.entity,status);
            }
            break;
        }
        case OSA_RESOURCE_GRP_DELETED:
        {
            /* nothing to do */
            lRet = DM_SUCCESS;
            break;
        }
        default:
            break;
    }

    return lRet;
}


/* Add-on control (load/unload) */
INT32 DmAddOnControlReq(DM_ADDON_CONTROL_REQ_T* pAddOnCtrl)
{
    DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

    if (pAddOnCtrl == NULL || pAddOnCtrl->action == DM_ADDON_CTRL_ACTION_NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid parameters, pAddOnCtrl is null or action is invlaid in DmAddOnControlReq(%s)", g_DmCtrl.entity);
        return lRet;
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmAddOnControlReq(%s) action(%d) path(%s) add-on name(%s)",
		     g_DmCtrl.entity, pAddOnCtrl->action, pAddOnCtrl->addonPath, pAddOnCtrl->addonName);

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;

        /* client things */
		UINT32 sockMsgSize = sizeof(DM_SOCK_MSG_HEADER_T) + sizeof(DM_ADDON_CONTROL_REQ_T);
        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sockMsgSize);
        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmAddOnControlReq(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sockMsgSize);
        pSockMsgSnd->msg_id = DM_MSG_CODE_AddOnControlReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = sizeof(DM_ADDON_CONTROL_REQ_T);

		INT8 *pTmp = (INT8 *)(pSockMsgSnd + 1);
		osa_mem_copy(pTmp, pAddOnCtrl, sizeof(DM_ADDON_CONTROL_REQ_T));

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmAddOnControlReq(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_AddOnControlCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_AddOnControlCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_AddOnControlCnf)
        {
            lRet = pSockMsgRcv->word_data;
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmAddOnControlReq(%s)",g_DmCtrl.entity);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmAddOnControlReq(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {
    	osa_thread_mutex_lock(&g_DmCtrl.addOnMutex);
	
     	if(pAddOnCtrl->action == DM_ADDON_CTRL_ACTION_LOAD)
     	{
     		lRet = DmLoadAddOn(pAddOnCtrl->addonPath, pAddOnCtrl->addonName);
     	}
		else
		{
			lRet = DmUnloadAddOn(pAddOnCtrl->addonName);
		}

     	osa_thread_mutex_unlock(&g_DmCtrl.addOnMutex);
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmAddOnControlReq(%s) lRet=%d",g_DmCtrl.entity,lRet);

    return lRet;
}

/* route request message to Add-on */
INT32 DmAddOnRouteMsgReq(DM_ADDON_REQ_MESSAGE_T* pAddOnReqMsg)
{
    DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

    if (pAddOnReqMsg == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid parameters, pAddOnReqMsg is null in DmAddOnRouteMsgReq(%s)", g_DmCtrl.entity);
        return lRet;
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmAddOnRouteMsgReq(%s) addon name(%s)", g_DmCtrl.entity, pAddOnReqMsg->addonName);

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        DM_SOCK_MSG_HEADER_T *pSockMsgSnd = NULL;
        DM_SOCK_MSG_HEADER_T *pSockMsgRcv = NULL;

        /* client things */
		UINT32 msgBodySize = sizeof(DM_ADDON_REQ_MESSAGE_T) + pAddOnReqMsg->reqMsgSize + strlen(pAddOnReqMsg->addonName) + 1;
		UINT32 sockMsgSize = sizeof(DM_SOCK_MSG_HEADER_T) + msgBodySize;
        pSockMsgSnd = (DM_SOCK_MSG_HEADER_T *)DM_MALLOC(sockMsgSize);
        if (pSockMsgSnd == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed in DmAddOnControlReq(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        osa_mem_set(pSockMsgSnd, 0, sockMsgSize);
        pSockMsgSnd->msg_id = DM_MSG_CODE_AddOnReqMsgReq;
        pSockMsgSnd->sender_id = DmConvertEntityToMsgId(g_DmCtrl.entity);
        pSockMsgSnd->receiver_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
        pSockMsgSnd->caller_id = osa_thread_gettid();
        pSockMsgSnd->msgLength = msgBodySize;

		INT8 *pTmp = (INT8 *)(pSockMsgSnd + 1);
		osa_mem_copy(pTmp, pAddOnReqMsg, sizeof(DM_ADDON_REQ_MESSAGE_T));

		pTmp += sizeof(DM_ADDON_REQ_MESSAGE_T);
		osa_mem_copy(pTmp, pAddOnReqMsg->reqMsg, pAddOnReqMsg->reqMsgSize);

		pTmp += pAddOnReqMsg->reqMsgSize;
		strcpy(pTmp, pAddOnReqMsg->addonName);

        /* send request to server */
        lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);

        if (lRet != DM_SUCCESS)
        {
            DM_FREE(pSockMsgSnd);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send sock msg to server failed in DmAddOnControlReq(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
            return lRet;
        }

        DM_FREE(pSockMsgSnd);

        /* wait for the response from server */
        pSockMsgRcv = (void*)DM_MSG_CODE_AddOnReqMsgCnf;
        lRet = osa_resource_wait(&g_DmCtrl.rsrcGrp,&pSockMsgRcv,DM_SOCK_MSG_TIMEOUT_MS);
        if (pSockMsgRcv == (void*)DM_MSG_CODE_AddOnReqMsgCnf)
        {
            pSockMsgRcv = NULL;
        }

        if (lRet == DM_SUCCESS && pSockMsgRcv != NULL && pSockMsgRcv->msg_id == DM_MSG_CODE_AddOnReqMsgCnf)
        {
            lRet = pSockMsgRcv->word_data;
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ osa_resource_wait successfully in DmAddOnRouteMsgReq(%s)",g_DmCtrl.entity);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_resource_wait failed in DmAddOnRouteMsgReq(%s)",g_DmCtrl.entity);
            lRet = DM_RET_INTERNAL_ERROR;
        }

        DM_FREE(pSockMsgRcv);
    }
    else
    {
    	osa_thread_mutex_lock(&g_DmCtrl.addOnMutex);

        DM_ADDON_NODE_T* pAddOnNode = g_DmCtrl.pAddOnList;
		for(; pAddOnNode; pAddOnNode = pAddOnNode->next)
		{
			if(strcmp(pAddOnNode->addonName, pAddOnReqMsg->addonName) == 0 && pAddOnNode->msgHandler)
			{
				lRet = pAddOnNode->msgHandler(pAddOnReqMsg->reqMsg, pAddOnReqMsg->reqMsgSize);
				break;
			}	
		}

		osa_thread_mutex_unlock(&g_DmCtrl.addOnMutex);
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmAddOnRouteMsgReq(%s) lRet=%d",g_DmCtrl.entity,lRet);

    return lRet;
}

