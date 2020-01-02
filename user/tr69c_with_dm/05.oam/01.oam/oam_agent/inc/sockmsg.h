
#ifndef _SOCK_MSG_H_
#define _SOCK_MSG_H_

#include "osa.h"
//#include "prc_ps_log.h"

typedef struct _SOCK_MSG_T SOCK_MSG_T;

/** the callback to handle received messages */
typedef void (*SOCK_MSG_RCV_FUN)(SOCK_MSG_T *pSockMsg, void *pMsg);

/** the client structure of sock message */
typedef struct _SOCK_CLIENT_T{
    INT32 client_sock;
    UINT32 client_id;
}SOCK_CLIENT_T;

/** the structure of sock message */
struct _SOCK_MSG_T{
    INT32 sock;

#define SOCK_PATH_LEN 16
    INT8 sock_path[SOCK_PATH_LEN];

    OSA_THREAD_MUTEX_T sndMutex;

    OSA_THREAD_T rcvThread;/**< receive messages */

#define SOCK_RCV_STATUS_STOPPED 0
#define SOCK_RCV_STATUS_STOPPING 1
#define SOCK_RCV_STATUS_RUNNING 2
    UINT8 rcvStatus;
    SOCK_MSG_RCV_FUN pRcvFunc;

    UINT8 asServer;
    OSA_THREAD_T acptThread;/**< accept connections from clients */

#define SOCK_ACPT_STATUS_STOPPED 0
#define SOCK_ACPT_STATUS_STOPPING 1
#define SOCK_ACPT_STATUS_RUNNING 2
    UINT8 acptStatus;

#define MAX_SOCK_CLIENT 10
    SOCK_CLIENT_T clients[MAX_SOCK_CLIENT];
    OSA_THREAD_MUTEX_T acptMutex;
};

/** the sock message header definition */
typedef struct _SOCK_MSG_HEADER_T{
    UINT32 msg_id;/**< the id of identify the message */
    UINT32 sender_id;/** the id to identify client or server */
    UINT32 receiver_id;/** the id to identify client or server */
    INT32  word_data;/** different msg has different usage.  */
    UINT32 msgLength;/** the bytes of the message body */
}SOCK_MSG_HEADER_T;

typedef struct _SOCK_MSG_PTR_T {
    SOCK_MSG_HEADER_T *pHdr;
    void *pBody;
}SOCK_MSG_PTR_T;



typedef INT32 (*SOCK_MSG_HANDLER_FUN)(UINT32   ulMsgId,UINT32   ulMsgAddress); 



//#define OAMAGENT_ERROR(pString,...)           ps_board_trace(GROUP_OAM_SCM, TRACE_LEVEL_ERROR, pString, ##__VA_ARGS__)
//#define OAMAGENT_WARNING(pString,...)         ps_board_trace(GROUP_OAM_SCM, TRACE_LEVEL_WARNING, pString, ##__VA_ARGS__)
//#define OAMAGENT_TRACE_H(pString,...)         ps_board_trace(GROUP_OAM_SCM, TRACE_LEVEL_HIGH, pString, ##__VA_ARGS__)
//#define OAMAGENT_TRACE_M(pString,...)         ps_board_trace(GROUP_OAM_SCM, TRACE_LEVEL_MED, pString, ##__VA_ARGS__)
//#define OAMAGENT_TRACE_L(pString,...)         ps_board_trace(GROUP_OAM_SCM, TRACE_LEVEL_LOW, pString, ##__VA_ARGS__)
#define OAMAGENT_TEST_PRINT                        printf



#ifdef DMALLOC_CHECK
#define OAMAGENT_MEM_ALLOC(size)          dmalloc_malloc(__FILE__, __LINE__, (size), DMALLOC_FUNC_MALLOC, 0, 0)
#define OAMAGENT_MEM_FREE(pMem) \
	{ \
		if (NULL != pMem) \
		{ \
			dmalloc_free(__FILE__, __LINE__, (pMem), DMALLOC_FUNC_FREE); \
			pMem = NULL; \
		} \
	}
#else
#define OAMAGENT_MEM_ALLOC         	osa_mem_alloc 
#define OAMAGENT_MEM_FREE(pMem) \
	{ \
		if (NULL != pMem) \
		{ \
			osa_mem_free(pMem); \
			pMem = NULL; \
		} \
	}
#endif



INT32 sockmsg_service_start_req(SOCK_MSG_T *pSockMsg, INT8 *pSockName, SOCK_MSG_RCV_FUN pRcvFunc, UINT8 asServer);
INT32 sockmsg_service_stop_req(SOCK_MSG_T *pSockMsg);
INT32 sockmsg_send_req(SOCK_MSG_T *pSockMsg, void *pMsg);
INT32 sockmsg_broadcast_req(SOCK_MSG_T *pSockMsg, void *pMsg);

#endif

