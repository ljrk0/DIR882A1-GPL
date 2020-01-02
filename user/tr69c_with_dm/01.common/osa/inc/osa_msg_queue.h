/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:        osa_msg_queue.h
  Description:      header file for message queue operations
  History:
    1. zhonghai    2013-10-08         creation
        
*******************************************************************************/


#ifndef OSA_MSG_QUEUE_H
#define OSA_MSG_QUEUE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif 
#endif 

/** the structure of message queue */
typedef struct  _OSA_MSGQ_T
{
    INT32 mq_id;
	UINT32 mq_msgSize;
    INT8 *ftok;/* used between processes. */
    BOOL8 used_for_processes;
}OSA_MSGQ_T;

#define MQ_RX_1     1
#define MQ_RX_2     2

/**< message size define */
#define RX_1_UINT32     1
#define RX_2_UINT32     2
#define RX_4_UINT32     4
#define RX_8_UINT32     8

typedef struct _OSA_MSG_BUF_T
{
    UINT8   ucWrittingFlg;      /**< indicate the buffer are in writing state */
    UINT32  mq_capacity;
    UINT32  mq_enqueued;
    UINT32  mq_available_storage;

    UINT32  *mq_rx_start;
    UINT32  *mq_rx_end;
    UINT32  *mq_rx_read;
    UINT32  *mq_rx_write;
}OSA_MSG_BUF_T;

typedef struct  _OSA_MSGQ_USER_T
{
    INT32   mq_id;
	UINT32  mq_msgSize;
    
    UINT8   mq_msg_buf_idx;             /**< use which message buffer to receive message */

    UINT8   mq_msg_buf_convert_flg;     /**< indicate if need convert the Buf1 and Buf2 after the receiving function */

    OSA_MSG_BUF_T   MsgBuf1;
    OSA_MSG_BUF_T   MsgBuf2;
    OSA_THREAD_MUTEX_T     MsgQueueMutex;
}OSA_MSGQ_USER_T;


/**
* check if message queue exists.
*
* @param[out] pMsgQueue the struct of the message queue.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_msg_queue_check(OSA_MSGQ_T *pMsgQueue);


/**
* create message queue.
*
* @param[out] pMsgQueue the struct of the message queue.
* @param[in] udwMsgSize message element size
* @param[in] udwQueueLen the length of message queue, it means how many messages can be through into the queue..
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_msg_queue_create(OSA_MSGQ_T *pMsgQueue, UINT32 udwMsgSize, UINT32 udwQueueLen);


/**
* release message queue.
*
* @param[in] pMsgQueue the struct of the message queue.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_msg_queue_delete(OSA_MSGQ_T *pMsgQueue);


/**
* try to send one message (not blocking).
*
* @param[in] pMsgQueue the struct of the message queue.
* @param[in] pMsg the message to send.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_msg_queue_send(OSA_MSGQ_T *pMsgQueue, void *pMsg);


/**
* receive one message(blocking).
*
* @param[in] pMsgQueue the struct of the message queue.
* @param[out] pMsg the message to receive.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_msg_queue_receive(OSA_MSGQ_T *pMsgQueue, void *pMsg);


/** user defined msg queue functions, use link table */
INT32 osa_msg_queue_create_user(OSA_MSGQ_USER_T *pMsgQueue,         
                                         UINT32      udwMsgSize,    
                                         UINT32      udwQueueLen);  
INT32 osa_msg_queue_delete_user(OSA_MSGQ_USER_T *pMsgQueue);
INT32 osa_msg_queue_send_user(OSA_MSGQ_USER_T *pMsgQueue, void *pMsg);
UINT32 osa_msg_queue_send_multi_user(OSA_MSGQ_USER_T *pMsgQueue, void *pMsg, UINT16 usMsgCnt);
INT32 osa_msg_queue_receive_user(OSA_MSGQ_USER_T *pMsgQueue, void *pMsg, UINT8 ucMsgFlg);
INT32 osa_msg_queue_receive_multi_user(OSA_MSGQ_USER_T *pMsgQueue, void *pMsg, UINT16 usMsgCnt, UINT8 ucMsgFlg);

#ifdef __cplusplus
#if __cplusplus
            }
#endif 
#endif/* __cpluscplus */
    

#endif


