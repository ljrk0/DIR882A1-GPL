/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:         dm_sock_msg.c
  Description: socket message operations
  History:
*******************************************************************************/

#include "dm_internal.h"

#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif 
#endif 

static INT32 dm_sock_send(INT32 sock,void *pSndBuf,UINT32 udwLen, OSA_THREAD_MUTEX_T *pSndMutex) 
{ 
    INT32   dwByteLeft  =   0,dwByteSend = 0;
    UINT8  *pucBuffer; 

    pucBuffer   = pSndBuf; 
    dwByteLeft  = udwLen; 

    osa_thread_mutex_lock(pSndMutex);

    while(dwByteLeft > 0) 
    { 
        dwByteSend = write(sock,pucBuffer,dwByteLeft);

        if(dwByteSend <= 0)
        {        
            if((dwByteSend < 0)&&(errno == EINTR))
            {
                dwByteSend = 0; 
            }
            else
            {
                if(dwByteSend < 0)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ write socket failed, errno=%d %s",errno,strerror(errno));
                }

                osa_thread_mutex_unlock(pSndMutex);
                return(-1); 
            }
        }

        dwByteLeft  -= dwByteSend; 
        pucBuffer   += dwByteSend;
    }

    osa_thread_mutex_unlock(pSndMutex);

    return (udwLen - dwByteLeft); 

} 


static INT32 dm_sock_recv(INT32 sock,void *pRcvBuf,UINT32 udwLen) 
{ 
    INT32      dwByteLeft   =   0,dwByteRead = 0; 
    UINT8      *pucBuffer    =   NULL; 

    dwByteLeft      =   udwLen; 
    pucBuffer       =   pRcvBuf;

    
    while(dwByteLeft > 0) 
    { 
        dwByteRead = read(sock,pucBuffer,dwByteLeft);
        if(dwByteRead < 0) 
        { 
            if(errno == EINTR) 
            {
                dwByteRead = 0; 
            }
            else 
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ read socket failed, errno=%d %s",errno,strerror(errno));
                return(-1); 
            }
        } 
        else if(dwByteRead == 0) 
        {
            break; 
        }
        
        dwByteLeft  -= dwByteRead;
        pucBuffer   += dwByteRead;
    } 
    
    return (udwLen - dwByteLeft); 
}


static void dm_accept_connections_from_clients(UINT32 ptrAddr)
{
    DM_SOCK_MSG_T *pSockMsg = (DM_SOCK_MSG_T *)ptrAddr;
	INT32  sin_size = 0;
	INT32  i = 0;


    pSockMsg->acptStatus = DM_SOCK_ACPT_STATUS_RUNNING;

    while (pSockMsg->acptStatus == DM_SOCK_ACPT_STATUS_RUNNING)
    {
        INT32 client_sock = 0;
        struct sockaddr_un remote_addr;


        sin_size = sizeof(struct sockaddr_un);

        if ((client_sock = accept(pSockMsg->sock, (struct sockaddr *)&remote_addr, (socklen_t*)&sin_size)) == -1) 
    	{
             continue;
    	}
    	else
    	{
    	    osa_thread_mutex_lock(&(pSockMsg->acptMutex));

            for( i = 0; i < DM_MAX_SOCK_CLIENT; i++)
    		{
    		    if(pSockMsg->clients[i].client_sock == 0)
    	    	{
    	    	    pSockMsg->clients[i].client_sock = client_sock;

    				break;
    	    	}
    		}

			osa_thread_mutex_unlock(&(pSockMsg->acptMutex));

            if (i == DM_MAX_SOCK_CLIENT)/* exceed the max client number. */
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ exceed the max client number, close new socket, in %s",__FUNCTION__);
                close(client_sock);
            }
    	}
    }

    pSockMsg->acptStatus = DM_SOCK_ACPT_STATUS_STOPPED;


    return;
}

static void dm_receive_messages_from_clients(UINT32 ptrAddr)
{
    DM_SOCK_MSG_T *pSockMsg = (DM_SOCK_MSG_T *)ptrAddr;
    fd_set        watchset;
	INT32         i = 0;
	INT32         lRet = 0;
	INT32         lMaxFd = 0;
	struct timeval tv = {0};
	DM_SOCK_MSG_HEADER_T msgHeader = {0};
	INT32         len  = 0;


    pSockMsg->rcvStatus = DM_SOCK_RCV_STATUS_RUNNING;

	while(pSockMsg->rcvStatus == DM_SOCK_RCV_STATUS_RUNNING)
    {

	    /*lock mutex*/
		osa_thread_mutex_lock(&(pSockMsg->acptMutex));

		lMaxFd = 0;

		/*set the fdset*/
		FD_ZERO(&watchset);

		for(i = 0; i < DM_MAX_SOCK_CLIENT; i++)
		{
		    if(pSockMsg->clients[i].client_sock != 0)
	    	{
	    	    lMaxFd = lMaxFd > (pSockMsg->clients[i].client_sock) ? lMaxFd : (pSockMsg->clients[i].client_sock);
	    	    FD_SET((unsigned int)(pSockMsg->clients[i].client_sock), &watchset);
	    	}
		}

		/*unlock mutex*/
		osa_thread_mutex_unlock(&(pSockMsg->acptMutex));

		/*select the fdset*/
		if(lMaxFd == 0 )
		{
		    sleep(1);
		    continue;
		}

		tv.tv_sec = 1;


		lRet = select(lMaxFd+1, &watchset, NULL, NULL, &tv);

		if (lRet < 0 && errno != EINTR)
        {
            continue;
        }
        else if (lRet < 0 && errno == EINTR)
        {
            /* interrupted by some signal, just take next select. */
            continue;
        }        

		/* received message, call the callback to handle it */
		INT32 closedSockNum = DM_MAX_SOCK_CLIENT;
		for(i = 0; i < DM_MAX_SOCK_CLIENT; i++)
		{
		    if(pSockMsg->clients[i].client_sock != 0)
	    	{
    		    if(FD_ISSET((unsigned int)(pSockMsg->clients[i].client_sock), &watchset))
    	    	{
    	    	    FD_CLR((unsigned int)(pSockMsg->clients[i].client_sock), &watchset);

    	    	    memset(&msgHeader, 0, sizeof(DM_SOCK_MSG_HEADER_T));

    	    	    len = dm_sock_recv(pSockMsg->clients[i].client_sock, &msgHeader, sizeof(DM_SOCK_MSG_HEADER_T));

    				if(len != sizeof(DM_SOCK_MSG_HEADER_T))
    				{
    				    if (0 == len)/* peer has closed this socket. we should close it. */
    				    {
        				    osa_thread_mutex_lock(&(pSockMsg->acptMutex));

                            if(0 == shutdown(pSockMsg->clients[i].client_sock, 2))
                            {
								closedSockNum --;
                                close(pSockMsg->clients[i].client_sock);
                            }

				    		pSockMsg->clients[i].client_sock = 0;
                    		pSockMsg->clients[i].client_id = 0;

        				    osa_thread_mutex_unlock(&(pSockMsg->acptMutex));

        				    continue;
    				    }
    				}
    				else
    				{
    				    if(pSockMsg->clients[i].client_id == 0)
    			    	{
    			    	    osa_thread_mutex_lock(&(pSockMsg->acptMutex));
    						pSockMsg->clients[i].client_id = msgHeader.sender_id;
    						osa_thread_mutex_unlock(&(pSockMsg->acptMutex));
    			    	}

    				    void *pMsg = OSA_MEM_ALLOC(sizeof(DM_SOCK_MSG_HEADER_T) + msgHeader.msgLength);

						if(pMsg == NULL)
						{
                            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to malloc() in %s",__FUNCTION__);
							continue;
						}

                        memset(pMsg, 0, sizeof(DM_SOCK_MSG_HEADER_T)+msgHeader.msgLength);
    					memcpy(pMsg, &msgHeader, sizeof(DM_SOCK_MSG_HEADER_T));

    					len = dm_sock_recv(pSockMsg->clients[i].client_sock, (INT8*)pMsg + sizeof(DM_SOCK_MSG_HEADER_T), msgHeader.msgLength);

    					if(len == msgHeader.msgLength && NULL != pSockMsg->pRcvFunc)
    					{
                            /*
                                valid message, so call callback function to handle it.
                             */
                            pSockMsg->pRcvFunc(pSockMsg, pMsg);
    					}

                        //OSA_MEM_FREE(pMsg);
    				}
    	    	}
	    	}
			else
			{
				closedSockNum --;
			}
		}

		if (closedSockNum == 0) /* stop dm service if all client is closed */
		{
			DmSelfStopService();
		}
    }

    pSockMsg->rcvStatus = DM_SOCK_RCV_STATUS_STOPPED;


    return;
}

static INT32 dm_start_server_service(DM_SOCK_MSG_T *pSockMsg, INT8 *pSockName)
{
    INT32 lRet = DM_FAILURE;
    struct sockaddr_un my_addr;	
    INT8 thread_name[16];

    /* create server socket */
    if ((pSockMsg->sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to create socket in %s",__FUNCTION__);
        return lRet;
    }

    my_addr.sun_family=AF_UNIX;
    strcpy(my_addr.sun_path, pSockMsg->sock_path);

    unlink(pSockMsg->sock_path);

    /* bind the socket */
    if (bind(pSockMsg->sock, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1) 
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to bind() errno %d %s in %s",errno,strerror(errno),__FUNCTION__);
        return lRet;
    }

    /* listen on this socket */
    if (listen(pSockMsg->sock, DM_MAX_SOCK_CLIENT) == -1)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to listen() errno %d %s in %s",errno,strerror(errno),__FUNCTION__);
        return lRet;
    }

    osa_thread_mutex_create(&(pSockMsg->acptMutex));

    snprintf(thread_name, sizeof(thread_name), "%s_sc",pSockName);

    /* start one thread to accept connections from clients */
    lRet = osa_thread_create(&(pSockMsg->acptThread), 
                                thread_name,
                                (OSA_THREAD_PFN)dm_accept_connections_from_clients, 
                                (UINT32)pSockMsg, 
                                0xF000, 
                                OSA_THREAD_SCHED_OTHER, 
                                E_THREAD_PRIO_0, 
                                OSA_THREAD_PROP_WAITABLE);

    if (lRet != DM_SUCCESS)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_thread_create for acptThread failed in %s",__FUNCTION__);
        return lRet;
    }

    snprintf(thread_name, sizeof(thread_name), "%s_sr",pSockName);

    /* start one thread to receive messages from clients */
    lRet = osa_thread_create(&(pSockMsg->rcvThread), 
                                thread_name,
                                (OSA_THREAD_PFN)dm_receive_messages_from_clients, 
                                (UINT32)pSockMsg, 
                                0xF000, 
                                OSA_THREAD_SCHED_FIFO, 
                                E_THREAD_PRIO_0, 
                                OSA_THREAD_PROP_NOWAIT);


    if (lRet != DM_SUCCESS)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ osa_thread_create for rcvThread failed in %s",__FUNCTION__);
    }

    return lRet;

}

static INT32 dm_stop_server_service(DM_SOCK_MSG_T *pSockMsg)
{
    INT32 i = 0;

    if (pSockMsg != NULL)
    {
        if (pSockMsg->acptStatus != DM_SOCK_ACPT_STATUS_STOPPED)
        {
            osa_thread_delete(&(pSockMsg->acptThread));
            osa_thread_wait(&(pSockMsg->acptThread));
            pSockMsg->acptStatus = DM_SOCK_ACPT_STATUS_STOPPED;
        }

        if (pSockMsg->rcvStatus != DM_SOCK_RCV_STATUS_STOPPED)
        {
            pSockMsg->rcvStatus = DM_SOCK_RCV_STATUS_STOPPING;
        }

        while (pSockMsg->acptStatus != DM_SOCK_ACPT_STATUS_STOPPED 
            || pSockMsg->rcvStatus != DM_SOCK_RCV_STATUS_STOPPED)
        {
            sleep(1);
        }

        osa_thread_mutex_lock(&(pSockMsg->acptMutex));

        for (i = 0; i < DM_MAX_SOCK_CLIENT; i++)
        {
            if (pSockMsg->clients[i].client_sock != 0)
            {
                close (pSockMsg->clients[i].client_sock);

                pSockMsg->clients[i].client_sock = 0;
                pSockMsg->clients[i].client_id = 0;
            }
        }

        osa_thread_mutex_unlock(&(pSockMsg->acptMutex));

        close (pSockMsg->sock);
        pSockMsg->sock = 0;

        unlink(pSockMsg->sock_path);

        osa_thread_mutex_delete(&(pSockMsg->acptMutex));
        osa_thread_mutex_delete(&(pSockMsg->sndMutex));


        return DM_SUCCESS;
    }


    return DM_FAILURE;
}

static INT32 dm_send_messages_to_clients(DM_SOCK_MSG_T *pSockMsg, void *pMsg)
{
    INT32  i = 0;
	INT32  lRet = DM_FAILURE;
	DM_SOCK_MSG_HEADER_T *pMsgHeader = (DM_SOCK_MSG_HEADER_T*)pMsg;

    /*check message*/
	if(NULL == pMsg || NULL == pMsgHeader)
	{
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid arguments in %s",__FUNCTION__);
	    return -1;
	}

	/*choose the client, and send message*/
	for(i = 0; i < DM_MAX_SOCK_CLIENT; i++)
	{
	    if((pSockMsg->clients[i].client_id != 0) 
            && (pMsgHeader->receiver_id == pSockMsg->clients[i].client_id))
    	{
            /* here we should consider the socket status. if it has been closed by peer, we should hanlde this exception. */
            lRet = dm_sock_send(pSockMsg->clients[i].client_sock, pMsg, sizeof(DM_SOCK_MSG_HEADER_T)+pMsgHeader->msgLength, &(pSockMsg->sndMutex));

            if(lRet == sizeof(DM_SOCK_MSG_HEADER_T)+pMsgHeader->msgLength)
            {
                lRet = DM_SUCCESS;
            }

            break;
    	}
	}

    return lRet;

}

static INT32 dm_broadcast_messages_to_clients(DM_SOCK_MSG_T *pSockMsg, void *pMsg)
{
    INT32  i = 0;
	INT32  lRet = DM_FAILURE;
	DM_SOCK_MSG_HEADER_T *pMsgHeader = (DM_SOCK_MSG_HEADER_T*)pMsg;

    /*check message*/
	if(NULL == pMsg || NULL == pMsgHeader)
	{
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid arguments in %s",__FUNCTION__);
	    return -1;
	}

	/* send message to all clients */
	for(i = 0; i < DM_MAX_SOCK_CLIENT; i++)
	{
        if (pSockMsg->clients[i].client_id != 0)
        {
    	    pMsgHeader->receiver_id = pSockMsg->clients[i].client_id;

            /* here we should consider the socket status. if it has been closed by peer, we should hanlde this exception. */
            lRet = dm_sock_send(pSockMsg->clients[i].client_sock, pMsg, sizeof(DM_SOCK_MSG_HEADER_T)+pMsgHeader->msgLength, &(pSockMsg->sndMutex));

            if(lRet == sizeof(DM_SOCK_MSG_HEADER_T)+pMsgHeader->msgLength)
            {
                lRet = DM_SUCCESS;
            }
        }

	}

    return lRet;

}

static void dm_receive_messages_from_server(UINT32 ptrAddr)
{
    DM_SOCK_MSG_T *pSockMsg = (DM_SOCK_MSG_T *)ptrAddr;
    fd_set        watchset;
	INT32         lRet = 0;
	INT32         lMaxFd = 0;
	struct timeval tv = {0};
	DM_SOCK_MSG_HEADER_T msgHeader = {0};
	INT32         len  = 0;

    pSockMsg->rcvStatus = DM_SOCK_RCV_STATUS_RUNNING;

	while(pSockMsg->rcvStatus == DM_SOCK_RCV_STATUS_RUNNING)
    {
		/*set the fdset*/
		FD_ZERO(&watchset);

    	FD_SET((unsigned int)(pSockMsg->sock), &watchset);

        lMaxFd = pSockMsg->sock + 1;

		/*select the fdset*/
		if(lMaxFd == 0 )
		{
		    sleep(1);
		    continue;
		}

		tv.tv_sec = 1;

		lRet = select(lMaxFd+1, &watchset, NULL, NULL, &tv);

        /* any messages received. */
	    if((lRet==1) && (FD_ISSET((unsigned int)(pSockMsg->sock), &watchset)))
    	{
    	    FD_CLR((unsigned int)(pSockMsg->sock), &watchset);

    	    memset(&msgHeader, 0, sizeof(DM_SOCK_MSG_HEADER_T));

    	    len = dm_sock_recv(pSockMsg->sock, &msgHeader, sizeof(DM_SOCK_MSG_HEADER_T));

			if(len != sizeof(DM_SOCK_MSG_HEADER_T))
			{
			    if (0 == len)/* peer has closed this socket. we close it too. */
			    {
                    if(0 == shutdown(pSockMsg->sock, 2))
                    {

                        close(pSockMsg->sock);
                    }

		    		pSockMsg->sock = 0;

				    break;/* the receive thread quit */
			    }
			}
			else
			{
			    void *pMsg = OSA_MEM_ALLOC(sizeof(DM_SOCK_MSG_HEADER_T) + msgHeader.msgLength);

				if(pMsg == NULL)
				{
					continue;
				}

                memset(pMsg, 0, sizeof(DM_SOCK_MSG_HEADER_T)+msgHeader.msgLength);
				memcpy(pMsg, &msgHeader, sizeof(DM_SOCK_MSG_HEADER_T));

				len = dm_sock_recv(pSockMsg->sock, (INT8*)pMsg + sizeof(DM_SOCK_MSG_HEADER_T), msgHeader.msgLength);

				if(len == msgHeader.msgLength && NULL != pSockMsg->pRcvFunc)
				{
                    /*
                        valid message, so call callback function to handle it.
                     */
                    pSockMsg->pRcvFunc(pSockMsg, pMsg);
				}

                //OSA_MEM_FREE(pMsg);
			}
    	}

    }

    pSockMsg->rcvStatus = DM_SOCK_RCV_STATUS_STOPPED;

    return;
}

static INT32 dm_start_client_service(DM_SOCK_MSG_T *pSockMsg, INT8 *pSockName)
{
    INT32 lRet = DM_FAILURE;
	struct sockaddr_un serverAddr;
    INT8 thread_name[16];

    /* Create a unix domain socket.*/
	pSockMsg->sock = socket(AF_LOCAL, SOCK_STREAM, 0);

	if (pSockMsg->sock < 0)
	{
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to create socket in %s",__FUNCTION__);
		return DM_FAILURE;
	}

    /*
    * Connect to server.
    */
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sun_family = AF_LOCAL;/* AF_UNIX */
	strncpy(serverAddr.sun_path, pSockMsg->sock_path, sizeof(serverAddr.sun_path));

	if (connect(pSockMsg->sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) != 0)
	{
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to connect() errno=%d %s in %s",errno,strerror(errno),__FUNCTION__);

		close(pSockMsg->sock);
		pSockMsg->sock = 0;

		return DM_FAILURE;
	}

    /* start one thread to receive messages from server */
    snprintf(thread_name, sizeof(thread_name), "%s_cr",pSockName);

    lRet = osa_thread_create(&(pSockMsg->rcvThread), 
                                thread_name,
                                (OSA_THREAD_PFN)dm_receive_messages_from_server, 
                                (UINT32)pSockMsg, 
                                0xF000, 
                                OSA_THREAD_SCHED_FIFO,
                                E_THREAD_PRIO_0, 
                                OSA_THREAD_PROP_NOWAIT);


    if (lRet != DM_SUCCESS)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to create thread for rcvThread in %s",__FUNCTION__);
    }
	return lRet;
}

static INT32 dm_stop_client_service(DM_SOCK_MSG_T *pSockMsg)
{
    if (pSockMsg != NULL)
    {
        if (pSockMsg->rcvStatus != DM_SOCK_RCV_STATUS_STOPPED)
        {
            pSockMsg->rcvStatus = DM_SOCK_RCV_STATUS_STOPPING;
        }

        while (pSockMsg->rcvStatus != DM_SOCK_RCV_STATUS_STOPPED)
        {
            sleep(1);
        }

        close (pSockMsg->sock);
        pSockMsg->sock = 0;

        //unlink(pSockMsg->sock_path);

        osa_thread_mutex_delete(&(pSockMsg->sndMutex));

        return DM_SUCCESS;
    }

    return DM_FAILURE;
}

static INT32 dm_send_messages_to_server(DM_SOCK_MSG_T *pSockMsg, void *pMsg)
{
	INT32  lRet = DM_FAILURE;
	DM_SOCK_MSG_HEADER_T *pMsgHeader = (DM_SOCK_MSG_HEADER_T*)pMsg;

    /* check message */
	if(NULL == pMsg || NULL == pMsgHeader)
	{
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid arguments in %s",__FUNCTION__);
	    return -1;
	}

	/* send message */

    if(pSockMsg->sock != 0)
	{
        /* here we should consider the socket status. if it has been closed by peer, we should hanlde this exception. */
        lRet = dm_sock_send(pSockMsg->sock, pMsg, sizeof(DM_SOCK_MSG_HEADER_T)+pMsgHeader->msgLength, &(pSockMsg->sndMutex));

        if(lRet == sizeof(DM_SOCK_MSG_HEADER_T)+pMsgHeader->msgLength)
        {
            lRet = DM_SUCCESS;
        }

    }    	

    return lRet;

}


/*
 ************************************************************************************************
  INT32 DmSockMsgServiceStartReq(DM_SOCK_MSG_T *pSockMsg, INT8 *pSockName, DM_SOCK_MSG_RCV_FUN pRcvFunc, UINT8 asServer)
  This function will create a socket message service.
 *************************************************************************************************
*/
INT32 DmSockMsgServiceStartReq(DM_SOCK_MSG_T *pSockMsg, INT8 *pSockName, DM_SOCK_MSG_RCV_FUN pRcvFunc, UINT8 asServer)
{
	INT32 lRet = DM_FAILURE;

    memset(pSockMsg, 0, sizeof(DM_SOCK_MSG_T));

    snprintf(pSockMsg->sock_path, sizeof(pSockMsg->sock_path), "/tmp/sock_%s", pSockName);

    pSockMsg->asServer = asServer;
    pSockMsg->pRcvFunc = pRcvFunc;

    osa_thread_mutex_create(&(pSockMsg->sndMutex));

    if (pSockMsg->asServer != 0)
    {
        lRet = dm_start_server_service(pSockMsg,pSockName);
    }
    else
    {
        lRet = dm_start_client_service(pSockMsg,pSockName);
    }

    return lRet;
}

/*
 ************************************************************************************************
  INT32 DmSockMsgServiceStopReq(DM_SOCK_MSG_T *pSockMsg)
  This function will delete a socket message service.
 *************************************************************************************************
*/
INT32 DmSockMsgServiceStopReq(DM_SOCK_MSG_T *pSockMsg)
{
	INT32 lRet = DM_FAILURE;

    if (pSockMsg->asServer != 0)
    {
        lRet = dm_stop_server_service(pSockMsg);
    }
    else
    {
        lRet = dm_stop_client_service(pSockMsg);
    }

    memset((void*)pSockMsg, 0, sizeof(DM_SOCK_MSG_T));

    return lRet;
}

/*
 ************************************************************************************************
  INT32 DmSockMsgSendReq(DM_SOCK_MSG_T *pSockMsg, void *pMsg)
  This function will send a socket message to peer(server or client).
 *************************************************************************************************
*/
INT32 DmSockMsgSendReq(DM_SOCK_MSG_T *pSockMsg, void *pMsg)
{
	INT32 lRet = DM_FAILURE;

    if (pSockMsg->sock <= 0)
    {
        /* invalid socket */
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid socket in %s",__FUNCTION__);
        return lRet;
    }

    if (pSockMsg->asServer != 0)
    {
        lRet = dm_send_messages_to_clients(pSockMsg, pMsg);
    }
    else
    {
        lRet = dm_send_messages_to_server(pSockMsg, pMsg);
    }

    return lRet;
}

/*
 ************************************************************************************************
  INT32 DmSockMsgBroadcastReq(DM_SOCK_MSG_T *pSockMsg, void *pMsg)
  This function will broadcast a socket message to all clients.
 *************************************************************************************************
*/
INT32 DmSockMsgBroadcastReq(DM_SOCK_MSG_T *pSockMsg, void *pMsg)
{
    INT32 lRet = DM_FAILURE;

    if (pSockMsg->sock <= 0)
    {
        /* invalid socket */
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid socket in %s",__FUNCTION__);
        return lRet;
    }

    if (pSockMsg->asServer != 0)
    {
        lRet = dm_broadcast_messages_to_clients(pSockMsg, pMsg);
    }

    return lRet;
}

/*
 ************************************************************************************************
  INT32 DmWaitSockStarted(INT8 *pSockName)
  This function wait sock created
 *************************************************************************************************
*/
INT32 DmWaitSockStarted(INT8 *pSockName)
{
	INT32 lRet = DM_SUCCESS;
	char sock_path[16];
	struct stat st;
	INT32 i, maxCount = DM_SOCK_SERVER_CHECK_MAX_COUNT/DM_SOCK_SERVER_CHECK_INTERVAL;
	
	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmWaitServiceStarted(%s) ",g_DmCtrl.entity);	
	
	osa_mem_set(sock_path, 0, sizeof(sock_path));
	snprintf(sock_path, sizeof(sock_path), "/tmp/sock_%s", pSockName);
	
	for (i = 0; i < maxCount; i++)
	{
		if( stat(sock_path, &st) == 0)
		{
			usleep(DM_SOCK_SERVER_CHECK_INTERVAL);
			break;
		}
		usleep(DM_SOCK_SERVER_CHECK_INTERVAL);
	}

	if (i >= maxCount)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Failed to check socket started status of DmSrv(%s).",g_DmCtrl.entity);
        printf("DM Trace ------ Failed to check socket started status count=%d", i);
		lRet = DM_FAILURE;
	}

    if( !S_ISSOCK(st.st_mode)) 
    {
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Failed to check socket started status not socket file");
        printf("DM Trace ------ Failed to check socket started status. not socket file");
		lRet = DM_FAILURE;
    }

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmWaitServiceStarted(%s) successfully",g_DmCtrl.entity);	
	
	return lRet;
}
