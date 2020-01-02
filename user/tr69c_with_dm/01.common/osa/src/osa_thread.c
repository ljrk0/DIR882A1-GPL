/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:         osa_thread.c
  Description: thread operations
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

static void osa_thread_handler(OSA_THREAD_T *pThread)
{
    INT32 rc=0;
	struct sched_param s_param;
	int policy = 0;

	rc = pthread_setschedparam(pThread->threadId,pThread->policy,&pThread->s_param);	
	if (rc != 0)
	{
		OSA_DBG_ERROR("pthread_setschedparam failed err-code=%d %s in osa_thread_create()", rc, strerror(rc));
	}

	if (strlen(pThread->name)>0)
	{
		prctl(PR_SET_NAME, pThread->name, 0, 0, 0);

		pthread_getschedparam(pthread_self(),&policy,&s_param);
		if(policy == SCHED_OTHER)
		{
			OSA_DBG_TRACE("pthread name : %s ,SCHED_OTHER,sched_priority:%d",pThread->name,s_param.sched_priority);
		}
		else if(policy == SCHED_RR)
		{
			OSA_DBG_TRACE("pthread name : %s ,SCHED_RR,sched_priority:%d",pThread->name,s_param.sched_priority);
		}
		else  if(policy==SCHED_FIFO)
	  	{
			OSA_DBG_TRACE("pthread name : %s ,SCHED_FIFO,sched_priority:%d",pThread->name,s_param.sched_priority);
		}
	}

	pThread->pfnEntry(pThread->udwInitDat);

    	return;
}

typedef void *(*OSA_P_FUNC_T) (void *);

/*
 ************************************************************************************************
 INT32 osa_thread_create( OSA_THREAD_T       *pThread,
                            INT8 *pName, 
                            OSA_THREAD_PFN           pfnEntry,
                            UINT32              udwInitDat,
                            UINT32              udwStackSize,
                            OSA_THREAD_SCHED_E eSched,
                            OSA_THREAD_PRIO_E   ePri,
                            OSA_THREAD_PROP_E eProp
                            );
  This function will create a thread.
  *************************************************************************************************
 */
INT32 osa_thread_create( OSA_THREAD_T       *pThread,            /* Thread ID */
                                        INT8 *pName, /* Thread name */
                                        OSA_THREAD_PFN           pfnEntry,           /* thread entry function */
                                        UINT32              udwInitDat,         /* thread initial parameter */
                                        UINT32              udwStackSize,       /* thread stack size */
                                        OSA_THREAD_SCHED_E eSched,              /* thread schedule */
                                        OSA_THREAD_PRIO_E   ePri,               /* thread priority */
                                        OSA_THREAD_PROP_E eProp             /* thread priority,currently it can be OSA_THREAD_PROP_WAITABLE or OSA_THREAD_PROP_NOWAIT */
                                        )
{

	INT32 rc=0;
    INT32 policy = SCHED_OTHER;
	INT32 max_priority, min_priority;
	pthread_attr_t attr;
	struct sched_param s_param;
	struct rlimit r;

    memset(&r, 0, sizeof(struct rlimit));
    
	r.rlim_max=99;
	setrlimit(RLIMIT_RTPRIO, &r);

    memset(pThread, 0, sizeof(OSA_THREAD_T));

    pThread->pfnEntry = pfnEntry;
    pThread->udwInitDat = udwInitDat;

    if (pName != NULL)
    {
        snprintf(pThread->name, sizeof(pThread->name), "%s", pName);
    }

	/* check the priority is in range */
    switch(eSched)
    {
        case OSA_THREAD_SCHED_FIFO:
            policy = SCHED_FIFO;
            break;
        case OSA_THREAD_SCHED_RR:
            policy = SCHED_RR;
            break;
        default:
            policy = SCHED_OTHER;
            break;
    }

	/* check the priority is in range */
	max_priority = sched_get_priority_max(policy);
	min_priority = sched_get_priority_min(policy);

	if(ePri < min_priority)
	{
		ePri = min_priority;
	}
    else if(ePri > max_priority)
    {
		ePri = max_priority;
    }

    /* initialize the attribute object */
	memset(&attr, 0, sizeof(attr));
    rc = pthread_attr_init(&attr);
    if (rc != 0)
    {
        OSA_DBG_ERROR("pthread_create failed err-code=%d %s in osa_thread_create()", rc, strerror(rc));
        return OSA_FAILURE;
    }

    /* set schedule policy */
	rc = pthread_attr_setschedpolicy(&attr, policy);
    if (rc != 0)
    {
        OSA_DBG_ERROR("pthread_create failed err-code=%d %s in osa_thread_create()", rc, strerror(rc));
        return OSA_FAILURE;
    }

	/* check the stack size is in range */
	if(udwStackSize < PTHREAD_STACK_MIN)
	{
		udwStackSize = PTHREAD_STACK_MIN;
	}

	rc = pthread_attr_setstacksize(&attr, udwStackSize);
    if (rc != 0)
    {
        OSA_DBG_ERROR("pthread_attr_setstacksize failed err-code=%d %s in osa_thread_create()", rc, strerror(rc));
        return OSA_FAILURE;
    }

    /* set thread priority */
	s_param.sched_priority = ePri;
	rc = pthread_attr_setschedparam(&attr, &s_param);
    if (rc != 0)
    {
        OSA_DBG_ERROR("pthread_attr_setschedparam failed err-code=%d %s in osa_thread_create()", rc, strerror(rc));
        return OSA_FAILURE;
    }

    /* set detached attribute. joinable by default. */
    if ( OSA_THREAD_PROP_NOWAIT == eProp )
    {
        rc = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        if (rc != 0)
        {
            OSA_DBG_ERROR("pthread_attr_setdetachstate failed err-code=%d %s in osa_thread_create()", rc, strerror(rc));
            return OSA_FAILURE;
        }
    }

    pThread->policy = policy;
    pThread->s_param = s_param;

    /* create the thread. */
	rc = pthread_create(&(pThread->threadId), &attr, (OSA_P_FUNC_T)osa_thread_handler, (void*)pThread);
    if (rc != 0)
    {
        OSA_DBG_ERROR("pthread_create failed err-code=%d %s in osa_thread_create()", rc, strerror(rc));
        return OSA_FAILURE;
    }

    /* release the attribute object */
    rc = pthread_attr_destroy(&attr);
    if (rc != 0)
    {
        OSA_DBG_ERROR("pthread_attr_destroy failed err-code=%d %s in osa_thread_create()", rc, strerror(rc));
        return OSA_FAILURE;
    }

	return(rc);
}

/*
 ************************************************************************************************
 INT32    osa_thread_delete(OSA_THREAD_T *pThread); 
    This function will stop/delete a thread.
  *************************************************************************************************
*/
INT32    osa_thread_delete(OSA_THREAD_T *pThread)
{
    INT32   lRet = OSA_SUCCESS;

    lRet = pthread_cancel(pThread->threadId);
    if ( lRet != OSA_SUCCESS )
	{
        OSA_DBG_ERROR("pthread_cancel failed err-code=%d %s in osa_thread_delete()", lRet, strerror(lRet));
	    lRet = OSA_FAILURE;

        return lRet;
	}
    
	return lRet;

}

/*
 ************************************************************************************************
 INT32    osa_thread_delete(OSA_THREAD_T *pThread); 
    This function will stop/delete a thread if it running.
  *************************************************************************************************
*/
INT32    osa_thread_try_delete(OSA_THREAD_T *pThread)
{
    INT32   lRet = OSA_SUCCESS;

    if (pthread_kill(pThread->threadId,0)==0)
    {
        lRet = pthread_cancel(pThread->threadId);
        if ( lRet != OSA_SUCCESS )
    	{
            OSA_DBG_ERROR("pthread_cancel failed err-code=%d %s in osa_thread_try_delete()", lRet, strerror(lRet));
    	    lRet = OSA_FAILURE;

            return lRet;
    	}
    }
    else
    {
        OSA_DBG_DEBUG("pthread %d not exists, in osa_thread_try_delete()", pThread->threadId);
    }

	return lRet;

}

/*
************************************************************************************************
(3)INT32    osa_thread_wait(OSA_THREAD_T *pThread)
    This function will wait until "pThread" exit if pThread is OSA_THREAD_PROP_WAITABLE. Error code will be returned if error occured.
    For example:
 *************************************************************************************************
 */
INT32    osa_thread_wait(OSA_THREAD_T *pThread)
{
    INT32   lRet = OSA_SUCCESS;

    lRet = pthread_join(pThread->threadId, NULL);
    if ( lRet != OSA_SUCCESS )
	{
        OSA_DBG_ERROR("pthread_join failed err-code=%d %s in osa_thread_wait()", lRet, strerror(lRet));
	    lRet = OSA_FAILURE;

        return lRet;
	}
    
    return lRet; 	
}

/*
 ************************************************************************************************
 void     osa_thread_sleep(UINT32 udwMs);
 This function will set current thread in sleep mode and be wake up after udwMS.
    Other threads will be re-sched when this function is called.                                            
 *************************************************************************************************
*/
void  osa_thread_sleep(UINT32 udwMs)
{
	usleep(udwMs * 1000);

    return;
}

/*
 ************************************************************************************************
 pid_t osa_thread_gettid(void);
 get thread id.                                            
 *************************************************************************************************
*/
pid_t osa_thread_gettid(void)
{
	return syscall(SYS_gettid);
}

/*
 ************************************************************************************************
 INT32 osa_thread_set_name(const char *pName, ...);
 set name for current thread.                                            
 *************************************************************************************************
*/
INT32 osa_thread_set_name(const char *pName, ...)
{
    INT32 ret = OSA_SUCCESS;
    char title [16] ={0};
    va_list ap;

    va_start(ap, pName);
    vsnprintf (title, sizeof (title) , pName, ap);
    va_end (ap);

    ret = prctl(PR_SET_NAME,title,0,0,0);

    if (ret != OSA_SUCCESS)
    {
        OSA_DBG_ERROR("prctl failed errno=%d %s in osa_thread_set_name()", errno, strerror(errno));
	    ret = OSA_FAILURE;
    }

    return ret;
}

INT32 osa_thread_increase_priority(pthread_t threadId)
{ 
    INT32 priority_policy = 0;
    struct sched_param para_struct;
    
    pthread_getschedparam(threadId, &priority_policy, &para_struct); 
    
    if (priority_policy == SCHED_FIFO) 
    {
        para_struct.sched_priority += 1; 
        pthread_setschedparam(threadId, priority_policy, &para_struct);
    }

    return 0;
}

INT32 osa_thread_decrease_priority(pthread_t threadId)
{
    INT32 priority_policy = 0;
    struct sched_param para_struct;
    
    pthread_getschedparam(threadId, &priority_policy, &para_struct); 
    
    if (priority_policy == SCHED_FIFO) 
    {
        para_struct.sched_priority -= 1; 
        pthread_setschedparam(threadId, priority_policy, &para_struct);
    }

    return 0;
}    

INT32 osa_get_thread_priority(OSA_THREAD_T  *pThread)
{
    INT32 priority_policy = 0;
    struct sched_param para_struct;
    
    pthread_getschedparam(pThread->threadId, &priority_policy, &para_struct); 

    if (priority_policy == SCHED_FIFO) 
    {
        OSA_DBG_DEBUG("osa_get_thread_priority %d", para_struct.sched_priority); 
    }

    return 0;
}

INT32 osa_thread_set_priority(OSA_THREAD_T *pThread, UINT32 ulPriority)
{ 
    INT32 priority_policy = 0;
    struct sched_param para_struct;
    
    
    pthread_getschedparam(pThread->threadId, &priority_policy, &para_struct); 
    
    if (priority_policy == SCHED_FIFO) 
    {
        para_struct.sched_priority = ulPriority; 
        pthread_setschedparam(pThread->threadId, priority_policy, &para_struct);
    }

    return 0;
}

 
#ifdef __cplusplus
#if __cplusplus
        }
#endif 
#endif/* __cpluscplus */


