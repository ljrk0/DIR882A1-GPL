/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:         osa_msg_queue.c
  Description: the operations for message queue
  History:
*******************************************************************************/

#include "osa.h"

#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif 
#endif 

/*
 ************************************************************************************************
 
 INT32 osa_msg_queue_check(OSA_MSGQ_T *pMsgQueue);
 This function will check if the msg-queue exists.
 *************************************************************************************************
*/
INT32 osa_msg_queue_check(OSA_MSGQ_T *pMsgQueue)
{
    INT32 lRet = OSA_FAILURE;

    if (pMsgQueue->used_for_processes == OSA_TRUE && pMsgQueue->ftok != NULL)
    {
        lRet = msgget(ftok(pMsgQueue->ftok, 0), C_IRUSR|C_IWUSR|C_IRGRP|C_IWGRP|C_IROTH|C_IWOTH);

        if (lRet > 0)/* msgq exists */
        {
            lRet = OSA_SUCCESS;
        }
    }
    else if (pMsgQueue->used_for_processes == OSA_FALSE && pMsgQueue->mq_id > 0)
    {
        lRet = OSA_SUCCESS;
    }

    return lRet;
}

/*
 ************************************************************************************************
 
 INT32 osa_msg_queue_create(   OSA_MSGQ_T *pMsgQueue,UINT32 udwMsgSize,UINT32 udwQueueLen);
 This function will create a msg queue.udwMsgSize indicate that msg element size in bytes and udwQueueLen is queue's length(how many 
 msg items can be through into the queue)
 *************************************************************************************************
*/
INT32 osa_msg_queue_create( OSA_MSGQ_T *pMsgQueue,              /* MsgQueue ID */
                                         UINT32      udwMsgSize,         /* Msg Element Size */
                                         UINT32      udwQueueLen)        /* Queue Len,It means how many msgs can be through into the queue. */
{
	INT32  rc = 0;
    UINT32 mq_size = 0;
	struct msqid_ds ds;
    
    if ((udwMsgSize <= 0) || (udwQueueLen <= 0))
    {
        OSA_DBG_ERROR("invalid parameters in osa_msg_queue_create()");
        return OSA_FAILURE;
    }

	mq_size = udwMsgSize * udwQueueLen;  /* compute the required queue size */
	pMsgQueue->mq_msgSize = udwMsgSize;  /* remember element size to be used later in send/receive */

    if (pMsgQueue->ftok != NULL)
    {
        pMsgQueue->mq_id = msgget(ftok(pMsgQueue->ftok, 0), IPC_CREAT|C_IRUSR|C_IWUSR|C_IRGRP|C_IWGRP|C_IROTH|C_IWOTH);

        if (pMsgQueue->mq_id==0)
        {
            /* drop id=0 msgq since it is easy to make a mistake. */
            msgctl(pMsgQueue->mq_id, IPC_RMID, (struct msqid_ds *)NULL);
            pMsgQueue->mq_id = msgget(IPC_PRIVATE, IPC_CREAT|C_IRUSR|C_IWUSR|C_IRGRP|C_IWGRP|C_IROTH|C_IWOTH);
            pMsgQueue->mq_id = msgget(ftok(pMsgQueue->ftok, 0), IPC_CREAT|C_IRUSR|C_IWUSR|C_IRGRP|C_IWGRP|C_IROTH|C_IWOTH);
        }

        pMsgQueue->used_for_processes = OSA_TRUE;
    }
    else
    {
        pMsgQueue->mq_id = msgget(IPC_PRIVATE, IPC_CREAT|C_IRUSR|C_IWUSR|C_IRGRP|C_IWGRP|C_IROTH|C_IWOTH);
        if (pMsgQueue->mq_id==0)
        {
            /* drop id=0 msgq since it is easy to make a mistake. */
            pMsgQueue->mq_id = msgget(IPC_PRIVATE, IPC_CREAT|C_IRUSR|C_IWUSR|C_IRGRP|C_IWGRP|C_IROTH|C_IWOTH);
        }
    }


	if(pMsgQueue->mq_id >= 0)
    {
		rc = msgctl(pMsgQueue->mq_id, IPC_STAT, &ds);
        if (rc < 0)
        {
            OSA_DBG_ERROR("msgctl IPC_STAT failed errno=%d %s in osa_msg_queue_create()",errno, strerror(errno));
            return OSA_FAILURE;
        }

        if (ds.msg_qbytes != mq_size)
        {
    		ds.msg_qbytes = mq_size;
    		rc = msgctl(pMsgQueue->mq_id, IPC_SET, &ds);
            if (rc < 0)
            {
                OSA_DBG_ERROR("msgctl IPC_SET failed errno=%d %s in osa_msg_queue_create()",errno, strerror(errno));
                return OSA_FAILURE;
            }
        }
	}
	else
	{
        OSA_DBG_ERROR("msgget failed errno=%d %s in osa_msg_queue_create()",errno, strerror(errno));
		rc = OSA_FAILURE;
	}

	return(rc);
}

/*
 ************************************************************************************************
 INT32 osa_msg_queue_delete(OSA_MSGQ_T *pMsgQueue)
    This function will delete the msg queue, release all resources associated with this queue, maybe cond, mutex,etc.
 *************************************************************************************************
*/
INT32 osa_msg_queue_delete(OSA_MSGQ_T *pMsgQueue)
{
    INT32   lRet = OSA_SUCCESS;

    if (pMsgQueue->mq_id==0)
    {
        /* id=0 msgq dropped, all msgq should start from 1 */
        return lRet;
    }

    lRet = msgctl(pMsgQueue->mq_id, IPC_RMID, (struct msqid_ds *)NULL);
	if ( lRet != OSA_SUCCESS )
	{
        OSA_DBG_ERROR("msgctl IPC_RMID failed errno=%d %s in osa_msg_queue_delete()",errno, strerror(errno));
	    lRet = OSA_FAILURE;
	}
    else
    {
        pMsgQueue->mq_id = -1;
    }
    
    return lRet;
}

/*
 ************************************************************************************************
 INT32 osa_msg_queue_send(OSA_MSGQ_T *pMsgQueue, void *pMsg)
 This func will put the pMsg into the queue if queue is not full. The msg item's prop should match to the pMsgQueue, such as msg item's length,
 it's data stucture,etc.
 *************************************************************************************************
*/
INT32 osa_msg_queue_send(OSA_MSGQ_T *pMsgQueue, void *pMsg)
{
    INT32   lRet = OSA_SUCCESS;

    if (pMsgQueue->mq_id==0)
    {
        /* id=0 msgq dropped, all msgq should start from 1 */
        OSA_DBG_ERROR("msgsnd failed mq_id=0x0");
        lRet = OSA_FAILURE;
        return lRet;
    }

    lRet = msgsnd(pMsgQueue->mq_id, (const void *)pMsg, (pMsgQueue->mq_msgSize-sizeof(long)), IPC_NOWAIT);

	if ( lRet == -1 )
	{
        OSA_DBG_ERROR("msgsnd failed mq_id=0x%x msgid=0x%x errno=%d %s in osa_msg_queue_send()",pMsgQueue->mq_id,*((INT32*)pMsg),errno, strerror(errno));
	    lRet = OSA_FAILURE;
	}
    
    return lRet;
}


/*
 ************************************************************************************************
 INT32 osa_msg_queue_receive(OSA_MSGQ_T *pMsgQueue, void *pMsg)
 This func will get a msg from pMsgQueue if MsgQ is not empty and copy this msg item to the buffer of pMsg.
 If MsgQ is empty, the thread will hang up on it until this MsgQ is not empty.    
 *************************************************************************************************
*/ 
INT32 osa_msg_queue_receive(OSA_MSGQ_T *pMsgQueue, void *pMsg)
{
	INT32   lRet = OSA_SUCCESS;

    if (pMsgQueue->mq_id==0)
    {
        /* id=0 msgq dropped, all msgq should start from 1 */
        OSA_DBG_ERROR("msgrcv failed mq_id=0x0");
        lRet = OSA_FAILURE;
        return lRet;
    }

	lRet = msgrcv(pMsgQueue->mq_id, pMsg, (pMsgQueue->mq_msgSize-sizeof(long)), 0, 0);

    if (lRet == -1)
    {
        OSA_DBG_ERROR("msgrcv failed mq_id=0x%x errno=%d %s in osa_msg_queue_receive()",pMsgQueue->mq_id,errno, strerror(errno));
        lRet = OSA_FAILURE;
    }
	else if ( lRet != (pMsgQueue->mq_msgSize-sizeof(long)))
	{
        OSA_DBG_ERROR("msgrcv failed : invalid msg length : received(%d bytes) required(%d bytes) in osa_msg_queue_receive()",(lRet+sizeof(long)), pMsgQueue->mq_msgSize);
	    lRet = OSA_FAILURE;
	}
    else
    {
        lRet = OSA_SUCCESS;
    }
	
    return lRet;
}

/** user defined msg queue functions */
INT32 osa_msg_queue_create_user(OSA_MSGQ_USER_T *pMsgQueue,              //MsgQueue ID
                                         UINT32      udwMsgSize,         //Msg Element Size
                                         UINT32      udwQueueLen)        //Queue Len,It means how many msgs can be through into the queue.
{
	INT32   rc      = 0;
    
    if ((udwMsgSize <= 0) || (udwQueueLen <= 0))
    {
        return OSA_FAILURE;
    }

    if ((udwMsgSize != RX_1_UINT32) && (udwMsgSize != RX_2_UINT32)
        && (udwMsgSize != RX_4_UINT32) && (udwMsgSize != RX_8_UINT32))
    {
        return OSA_FAILURE;
    }

    rc = osa_thread_mutex_create(&pMsgQueue->MsgQueueMutex);
    if (OSA_SUCCESS != rc)
    {
        return OSA_FAILURE;
    }

    osa_thread_mutex_lock(&pMsgQueue->MsgQueueMutex);

	pMsgQueue->mq_msgSize               = udwMsgSize;  // remember element size to be used later in send/receive

    pMsgQueue->mq_msg_buf_idx           = MQ_RX_1;
    pMsgQueue->mq_msg_buf_convert_flg   = 0;

    pMsgQueue->MsgBuf1.ucWrittingFlg    = 0;
    pMsgQueue->MsgBuf1.mq_capacity      = udwQueueLen;
    pMsgQueue->MsgBuf1.mq_rx_start      = (UINT32 *)osa_mem_alloc(udwMsgSize * 4 * udwQueueLen);
    if (NULL == pMsgQueue->MsgBuf1.mq_rx_start)
    {
        return OSA_FAILURE;
    }
    pMsgQueue->MsgBuf1.mq_rx_end        = pMsgQueue->MsgBuf1.mq_rx_start + udwMsgSize * udwQueueLen;
    pMsgQueue->MsgBuf1.mq_rx_read       = pMsgQueue->MsgBuf1.mq_rx_start;
    pMsgQueue->MsgBuf1.mq_rx_write      = pMsgQueue->MsgBuf1.mq_rx_start;
    pMsgQueue->MsgBuf1.mq_enqueued      = 0;
    pMsgQueue->MsgBuf1.mq_available_storage = udwQueueLen;

    pMsgQueue->MsgBuf2.ucWrittingFlg    = 0;
    pMsgQueue->MsgBuf2.mq_capacity      = udwQueueLen;
    pMsgQueue->MsgBuf2.mq_rx_start      = (UINT32 *)osa_mem_alloc(udwMsgSize * 4 * udwQueueLen);
    if (NULL == pMsgQueue->MsgBuf2.mq_rx_start)
    {
        return OSA_FAILURE;
    }
    pMsgQueue->MsgBuf2.mq_rx_end        = pMsgQueue->MsgBuf2.mq_rx_start + udwMsgSize * udwQueueLen;
    pMsgQueue->MsgBuf2.mq_rx_read       = pMsgQueue->MsgBuf2.mq_rx_start;
    pMsgQueue->MsgBuf2.mq_rx_write      = pMsgQueue->MsgBuf2.mq_rx_start;
    pMsgQueue->MsgBuf2.mq_enqueued      = 0;
    pMsgQueue->MsgBuf2.mq_available_storage = udwQueueLen;

    osa_thread_mutex_unlock(&pMsgQueue->MsgQueueMutex);

	return(rc);
}

INT32 osa_msg_queue_delete_user(OSA_MSGQ_USER_T *pMsgQueue)
{
    INT32   lRet = OSA_SUCCESS;

    osa_thread_mutex_delete(&pMsgQueue->MsgQueueMutex);
    osa_mem_free(pMsgQueue->MsgBuf1.mq_rx_start);
    osa_mem_free(pMsgQueue->MsgBuf2.mq_rx_start);
    
    return lRet;
}

INT32 osa_msg_queue_send_user(OSA_MSGQ_USER_T *pMsgQueue, void *pMsg)
{
    INT32           lRet            = OSA_SUCCESS;
    UINT32          *source         = NULL;             /* Source pointer          */
    UINT32          *destination    = NULL;             /* Destination pointer     */
    OSA_MSG_BUF_T   *pMsgBuf        = NULL;

    osa_thread_mutex_lock(&pMsgQueue->MsgQueueMutex);
    
    pMsgBuf = &pMsgQueue->MsgBuf1;

    pMsgBuf->ucWrittingFlg  = 1;

    if (pMsgBuf->mq_available_storage)
    {
        /* Setup source and destination pointers.  */
        source      =  (UINT32 *)pMsg;
        destination =  (UINT32 *)(pMsgBuf->mq_rx_write);

        /* Copy the message into the queue.  */
        if (pMsgQueue->mq_msgSize == RX_1_UINT32)
        {
            /* Copy single longword message into the queue.  */
            *destination =  *source;
        }
        else if (pMsgQueue->mq_msgSize == RX_2_UINT32)
        {
            /* Copy double longword message into the queue.  */
            *destination++ =  *source++;
            *destination =    *source;
        }
        else if (pMsgQueue->mq_msgSize == RX_4_UINT32)
        {
            /* Copy a four longword message into the queue.  */
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination =    *source;
        }
        else
        {
            /* Copy an eight longword message into the queue.  */
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination =    *source;
        }

        /* Adjust the write pointer.  */
        pMsgBuf->mq_rx_write    =  pMsgBuf->mq_rx_write + pMsgQueue->mq_msgSize;

        /* Determine if we are at the end.  */
        if (pMsgBuf->mq_rx_write == pMsgBuf->mq_rx_end)
        {
            /* Yes, wrap around to the beginning.  */
            pMsgBuf->mq_rx_write    =  pMsgBuf->mq_rx_start;
        }

        pMsgBuf->mq_available_storage--;
        pMsgBuf->mq_enqueued++;

        pMsgBuf->ucWrittingFlg  = 0;
    }
    else
    {
        lRet    = OSA_FAILURE;
    }

    osa_thread_mutex_unlock(&pMsgQueue->MsgQueueMutex);
    
    return lRet;
}

UINT32 osa_msg_queue_send_multi_user(OSA_MSGQ_USER_T *pMsgQueue, void *pMsg, UINT16 usMsgCnt)
{
    UINT16          usSendMsgCnt    = 0;
    UINT32          *source         = NULL;             /* Source pointer          */
    UINT32          *destination    = NULL;             /* Destination pointer     */
    OSA_MSG_BUF_T   *pMsgBuf        = NULL;

    osa_thread_mutex_lock(&pMsgQueue->MsgQueueMutex);
    
    pMsgBuf = &pMsgQueue->MsgBuf1;

    pMsgBuf->ucWrittingFlg  = 1;

    /* Setup source and destination pointers.  */
    source      =  (UINT32 *)pMsg;

    while (0 != pMsgBuf->mq_available_storage)
    {
        destination =  (UINT32 *)(pMsgBuf->mq_rx_write);

        /* Copy the message into the queue.  */
        if (pMsgQueue->mq_msgSize == RX_1_UINT32)
        {
            /* Copy single longword message into the queue.  */
            *destination =  *source++;
        }
        else if (pMsgQueue->mq_msgSize == RX_2_UINT32)
        {
            /* Copy double longword message into the queue.  */
            *destination++ =  *source++;
            *destination =    *source++;
        }
        else if (pMsgQueue->mq_msgSize == RX_4_UINT32)
        {
            /* Copy a four longword message into the queue.  */
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination =    *source++;
        }
        else
        {
            /* Copy an eight longword message into the queue.  */
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination =    *source++;
        }
    
        /* Adjust the write pointer.  */
        pMsgBuf->mq_rx_write    =  pMsgBuf->mq_rx_write + pMsgQueue->mq_msgSize;
    
        /* Determine if we are at the end.  */
        if (pMsgBuf->mq_rx_write == pMsgBuf->mq_rx_end)
        {
            /* Yes, wrap around to the beginning.  */
            pMsgBuf->mq_rx_write    =  pMsgBuf->mq_rx_start;
        }
    
        pMsgBuf->mq_available_storage--;
        pMsgBuf->mq_enqueued++;
    
        pMsgBuf->ucWrittingFlg  = 0;

        usSendMsgCnt++;

        if (usSendMsgCnt >= usMsgCnt)
        {
            break;
        }
    }

    osa_thread_mutex_unlock(&pMsgQueue->MsgQueueMutex);
    
    return usSendMsgCnt;
}

INT32 osa_msg_queue_receive_user(OSA_MSGQ_USER_T *pMsgQueue, void *pMsg, UINT8 ucMsgFlg)
{
	INT32           lRet            = OSA_SUCCESS;
    UINT32          *source         = NULL;             /* Source pointer          */
    UINT32          *destination    = NULL;             /* Destination pointer     */
    OSA_MSG_BUF_T   *pMsgBuf        = NULL;

    osa_thread_mutex_lock(&pMsgQueue->MsgQueueMutex);
    
    pMsgBuf = &pMsgQueue->MsgBuf1;

    /* Determine if there is anything in the queue.  */
    if (pMsgBuf->mq_enqueued)
    {
        /* Setup source and destination pointers.  */
        source      =  (UINT32 *)(pMsgBuf->mq_rx_read);
        destination =  (UINT32 *)pMsg;

        /* Yes, there is something in the queue.  Place the oldest message in the 
           queue into the thread's area.  */
        if (pMsgQueue->mq_msgSize == RX_1_UINT32)
        {                
            /* Copy single longword message from the queue.  */
            *destination =  *source;
        }
        else if (pMsgQueue->mq_msgSize == RX_2_UINT32)
        {
            /* Copy double longword message from the queue.  */
            *destination++ =  *source++;
            *destination =    *source;
        }
        else if (pMsgQueue->mq_msgSize == RX_4_UINT32)
        {
            /* Copy a four longword message from the queue.  */
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination =    *source;
        }
        else
        {
            /* Copy an eight longword message from the queue.  */
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination =    *source;
        }

        /* Adjust the read pointer.  */
        pMsgBuf->mq_rx_read = pMsgBuf->mq_rx_read + pMsgQueue->mq_msgSize;
        pMsgBuf->mq_enqueued--;
        pMsgBuf->mq_available_storage++;

        /* Determine if we are at the end.  */
        if (pMsgBuf->mq_rx_read == pMsgBuf->mq_rx_end)
        {
            /* Yes, wrap around to the beginning.  */
            pMsgBuf->mq_rx_read =  pMsgBuf->mq_rx_start;
        }
    }
    else
    {
        lRet    = OSA_QUEUE_EMPTY;
    }

    osa_thread_mutex_unlock(&pMsgQueue->MsgQueueMutex);

    return lRet;
}

INT32 osa_msg_queue_receive_multi_user(OSA_MSGQ_USER_T *pMsgQueue, void *pMsg, UINT16 usMsgCnt, UINT8 ucMsgFlg)
{
	INT32           lRet            = 0;
    UINT32          *source         = NULL;             /* Source pointer          */
    UINT32          *destination    = NULL;             /* Destination pointer     */
    OSA_MSG_BUF_T   *pMsgBuf        = NULL;
    UINT16          usReadMsg = 0;

    osa_thread_mutex_lock(&pMsgQueue->MsgQueueMutex);
    
    pMsgBuf = &pMsgQueue->MsgBuf1;

    /* Determine if there is anything in the queue.  */
    /* Setup source and destination pointers.  */
    source      =  (UINT32 *)(pMsgBuf->mq_rx_read);
    destination =  (UINT32 *)pMsg;
    
    while (pMsgBuf->mq_enqueued)
    {
        source      =  (UINT32 *)(pMsgBuf->mq_rx_read);
        /* Yes, there is something in the queue.  Place the oldest message in the 
            queue into the thread's area.  */
        if (pMsgQueue->mq_msgSize == RX_1_UINT32)
        {                
            /* Copy single longword message from the queue.  */
            *destination++ =  *source++;
        }
        else if (pMsgQueue->mq_msgSize == RX_2_UINT32)
        {
            /* Copy double longword message from the queue.  */
            *destination++ =  *source++;
            *destination++ =  *source;
        }
        else if (pMsgQueue->mq_msgSize == RX_4_UINT32)
        {
            /* Copy a four longword message from the queue.  */
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source;
        }
        else
        {
            /* Copy an eight longword message from the queue.  */
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source;
        }

        /* Adjust the read pointer.  */
        pMsgBuf->mq_rx_read = pMsgBuf->mq_rx_read + pMsgQueue->mq_msgSize;
        pMsgBuf->mq_enqueued--;
        pMsgBuf->mq_available_storage++;

        /* Determine if we are at the end.  */
        if (pMsgBuf->mq_rx_read == pMsgBuf->mq_rx_end)
        {
            /* Yes, wrap around to the beginning.  */
            pMsgBuf->mq_rx_read =  pMsgBuf->mq_rx_start;
        }

        usReadMsg++;

        if (usReadMsg >= usMsgCnt)
        {
            break;
        }
    }

    lRet =  usReadMsg;

    osa_thread_mutex_unlock(&pMsgQueue->MsgQueueMutex);

    return lRet;
}

#ifdef __cplusplus
#if __cplusplus
        }
#endif 
#endif/* __cpluscplus */



