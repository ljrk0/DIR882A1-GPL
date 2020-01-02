/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:         osa_getaddrinfo.c
  Description: async getaddrinfo().
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

#define OSA_GETADDRINFO_EVENT  0x1

typedef struct _OSA_ADDRINFO_T
{
    const char *node;
    const char *service;
    const struct addrinfo *hints;
    struct addrinfo **res;

    INT32 result;

    OSA_EVENT_GRP_T event;
}OSA_ADDRINFO_T;

static struct timeval * osa_time_monotonic(struct timeval *tv)
{
    struct timespec ts;

	if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
	{
		if (tv)
		{
			tv->tv_sec = ts.tv_sec;
			tv->tv_usec = ts.tv_nsec / 1000;
		}
	}

    return tv;

}

void osa_getaddrinfo_thread(OSA_ADDRINFO_T *reqNode)
{
    INT32 lRet = OSA_FAILURE;
    UINT32 eventSnt = OSA_GETADDRINFO_EVENT;
    struct timeval tv_start, tv_stop, tv_used;

    OSA_DBG_DEBUG("run getaddrinfo %s",reqNode->node);

    osa_time_monotonic(&tv_start);
    reqNode->result = getaddrinfo(reqNode->node,reqNode->service,reqNode->hints,reqNode->res);
    osa_time_monotonic(&tv_stop);

    timersub(&tv_stop, &tv_start, &tv_used);

    OSA_DBG_DEBUG("getaddrinfo %s time(%d.%06ds) result=%d %s",reqNode->node,tv_used.tv_sec,tv_used.tv_usec,reqNode->result,strerror(reqNode->result));

    lRet = osa_event_set(&reqNode->event,eventSnt);

    if (lRet != OSA_SUCCESS)
    {
        OSA_DBG_DEBUG("osa_event_clear failed in osa_getaddrinfo_thread");
    }
    else
    {
        OSA_DBG_DEBUG("osa_event_set done");
    }

    return;
}

INT32 osa_getaddrinfo(const char *node,
                            const char *service,
                            const struct addrinfo *hints,
                            struct addrinfo **res,
                            int milliseconds)
{
    INT32 lRet = OSA_FAILURE;
    INT32 result = OSA_FAILURE;
    OSA_ADDRINFO_T reqNode;
    OSA_THREAD_T reqThread;
    UINT32 eventReq = 0;
    UINT32 eventRev = 0;

    OSA_DBG_DEBUG("enter osa_getaddrinfo %s",node);

    osa_mem_set(&reqNode,0,sizeof(OSA_ADDRINFO_T));
    osa_mem_set(&reqThread,0,sizeof(OSA_THREAD_T));

    reqNode.node = node;
    reqNode.service = service;
    reqNode.hints = hints;
    reqNode.res = res;

    /* initialize pthread_cond */
    lRet = osa_event_group_create(&reqNode.event);

    if (lRet != OSA_SUCCESS)
    {
        OSA_DBG_WARNING("failed to initialize osa_event in osa_getaddrinfo");
        return lRet;
    }

    /* start thread to run getaddrinfo() */
    lRet = osa_thread_create(&reqThread,
                            "osaGetaddrinfo",
                            (OSA_THREAD_PFN)osa_getaddrinfo_thread,
                            (UINT32)&reqNode,
                            0xf000,
                            OSA_THREAD_SCHED_OTHER,
                            E_THREAD_PRIO_0,
                            OSA_THREAD_PROP_WAITABLE);

    if (lRet != OSA_SUCCESS)
    {
        OSA_DBG_WARNING("failed to create thread in osa_getaddrinfo");

        lRet = osa_event_group_delete(&reqNode.event);

        if (lRet != OSA_SUCCESS)
        {
            OSA_DBG_WARNING("osa_event_group_delete failed in osa_getaddrinfo");
        }

        return lRet;
    }

    /* wait for the result of getaddrinfo() */
    eventReq = OSA_GETADDRINFO_EVENT;
    lRet = osa_event_get(&reqNode.event,
                        eventReq,
                        OSA_EVENT_CLEAR_REQUIRED,
                        &eventRev,
                        milliseconds);

    if (lRet != OSA_SUCCESS)
    {
        OSA_DBG_WARNING("osa_event_get failed in osa_getaddrinfo");
    }
    else
    {
        result = reqNode.result;
    }

    lRet = osa_thread_try_delete(&reqThread);

    if (lRet != OSA_SUCCESS)
    {
        OSA_DBG_WARNING("osa_thread_delete failed in osa_getaddrinfo");
    }

    lRet = osa_thread_wait(&reqThread);

    if (lRet != OSA_SUCCESS)
    {
        OSA_DBG_WARNING("osa_thread_wait failed in osa_getaddrinfo");
    }

    lRet = osa_event_group_delete(&reqNode.event);

    if (lRet != OSA_SUCCESS)
    {
        OSA_DBG_WARNING("osa_event_group_delete failed in osa_getaddrinfo");
    }

    OSA_DBG_DEBUG("leave osa_getaddrinfo %s",node);

    return result;
}

#ifdef __cplusplus
#if __cplusplus
            }
#endif 
#endif/* __cpluscplus */
    


