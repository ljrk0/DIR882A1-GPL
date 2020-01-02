/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:         osa_event.c
  Description: operations for the event group.
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

static struct timeval * osa_time_realtime(struct timeval *tv)
{
    struct timespec ts;

	if (clock_gettime(CLOCK_REALTIME, &ts) == 0)
	{
		if (tv)
		{
			tv->tv_sec = ts.tv_sec;
			tv->tv_usec = ts.tv_nsec / 1000;
		}
	}

    return tv;

}

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

static INT32 osa_timeval_cmp(struct timeval *a, struct timeval *b)
{
	if (a->tv_sec > b->tv_sec)
	{
		return 1;
	}
	if (a->tv_sec < b->tv_sec)
	{
		return -1;
	}
	if (a->tv_usec > b->tv_usec)
	{
		return 1;
	}
	if (a->tv_usec < b->tv_usec)
	{
		return -1;
	}
	return 0;
}


/*
************************************************************************************************
    Function description:
(1)UINT32 osa_event_group_create(OSA_EVENT_GRP_T *pEvent)
    This function will create a event group.
*************************************************************************************************
*/
INT32 osa_event_group_create(OSA_EVENT_GRP_T *pEvent)
{
	INT32   lRet = OSA_SUCCESS;
    
	if(pEvent)
	{
		pEvent->eventBits = 0;
        pEvent->eventGrpDeleted = 0;

        pthread_condattr_init(&(pEvent->condAttr));
        //pthread_condattr_setclock(&(pEvent->condAttr), CLOCK_MONOTONIC);

		lRet = pthread_cond_init(&pEvent->cv,NULL);
    	if ( lRet != OSA_SUCCESS )
    	{
            OSA_DBG_ERROR("pthread_cond_init failed err-code=%d %s in osa_event_group_create()",lRet,strerror(lRet));
    	    lRet = OSA_FAILURE;

            return lRet;
    	}
        
		lRet = pthread_mutex_init(&pEvent->lock,NULL);
    	if ( lRet != OSA_SUCCESS )
    	{
            OSA_DBG_ERROR("pthread_mutex_init failed err-code=%d %s in osa_event_group_create()",lRet,strerror(lRet));
    	    lRet = OSA_FAILURE;

            return lRet;
    	}
	}
	else
	{
        OSA_DBG_ERROR("invalid arguments in osa_event_group_create()");
		lRet = OSA_FAILURE;
	}
    
	return lRet;
}

/*
************************************************************************************************

(2)UINT32 osa_event_group_delete(OSA_EVENT_GRP_T *pEvent)
    This function will delete a the event group. All resources associated with this evt group will be released.

*************************************************************************************************
*/
INT32 osa_event_group_delete(OSA_EVENT_GRP_T *pEvent)
{
    INT32   lRet = OSA_SUCCESS;
    
	lRet = pthread_mutex_lock(&pEvent->lock);
	if ( lRet != OSA_SUCCESS )
	{
        OSA_DBG_ERROR("pthread_mutex_lock failed err-code=%d %s in osa_event_group_delete()",lRet,strerror(lRet));
	    lRet = OSA_FAILURE;

        return lRet;
	}

    pEvent->eventGrpDeleted = 1;

	lRet = pthread_cond_broadcast(&pEvent->cv);
	if ( lRet != OSA_SUCCESS )
	{
        OSA_DBG_ERROR("pthread_cond_broadcast failed err-code=%d %s in osa_event_group_delete()",lRet,strerror(lRet));
	    lRet = OSA_FAILURE;

        return lRet;
	}

	lRet = pthread_mutex_unlock(&pEvent->lock);
	if ( lRet != OSA_SUCCESS )
	{
        OSA_DBG_ERROR("pthread_mutex_unlock failed err-code=%d %s in osa_event_group_delete()",lRet,strerror(lRet));
	    lRet = OSA_FAILURE;

        return lRet;
	}

    /* 
        Just broadcast but no resources released. Why?

        pEvent->eventBits = 0;
        pthread_cond_destroy(&pEvent->cv);
        pthread_mutex_destroy(&pEvent->lock);
    */

	return lRet;
}

/*
************************************************************************************************

(3)UINT32 osa_event_get( OSA_EVENT_GRP_T   *pEvent,
                         UINT32         udwReqEvents,
                         OSA_EVENT_OPTION_E eOption,
                         UINT32        *pudwRealEvt,
                         UINT32         udwWaitMS);

    This function will wait until event group's event match user's request.
*************************************************************************************************
*/

INT32 osa_event_get(OSA_EVENT_GRP_T *pEvent,/* Event Group ID */
                        UINT32 udwReqEvents,/* Indicate that what Event that this evt-group wanted. */
                        OSA_EVENT_OPTION_E eOption,/* Wait Option.If it is OSA_EVENT_ANY_RECEIVED, any ReqEvt bit is set will match the req. 
                                                    * If it is OSA_EVENT_ALL_RECEIVED_ONLY, all need bit should be set.
                                                    * If OSA_EVENT_CLEAR_REQUIRED is added into the option, the bit will be cleared after get op.
                                                    */
                        UINT32 *pudwRealEvt,/* The real event in the event group */
                        UINT32 udwWaitMS)/* Time in mil-second. If it is 0, it will wait until event happened, or time expired after udwWaitMS. */
{
	int flag = 1,ret = OSA_SUCCESS,lRet = 0;
	int condition = 0;
	struct timespec ts;
    struct timeval tv_tmp, tv_now, tv_expired, tv_cur, tv_now_real, tv_expired_real;

	if(pEvent && !pEvent->eventGrpDeleted)
	{
		lRet = pthread_mutex_lock(&pEvent->lock);

        if (lRet != 0)
        {
            OSA_DBG_ERROR("pthread_mutex_lock failed err-code=%d %s in osa_event_get()",lRet,strerror(lRet));

            ret = OSA_FAILURE;
            return ret;
        }

        if(udwWaitMS > 0)
        {
            /* use relative time in case of system time jumping for ntp sync or other reasons. */
            osa_time_monotonic(&tv_now);

            tv_tmp.tv_sec = udwWaitMS / 1000;
            tv_tmp.tv_usec = (udwWaitMS % 1000) * 1000;

            timeradd(&tv_now, &tv_tmp, &tv_expired);
        }

		while(flag)
		{
			if(eOption & OSA_EVENT_ANY_RECEIVED)
			{
                /* check if any required event received. */
				condition = (udwReqEvents & (pEvent->eventBits) );
			}
            else
            {
                /* check if all required events received. */
				condition = ((udwReqEvents & (pEvent->eventBits)) == udwReqEvents);
            }

			if(condition)
			{
				flag = 0;
			}

			while(!condition && !pEvent->eventGrpDeleted)
			{
				if(udwWaitMS > 0)
				{
                    osa_time_monotonic(&tv_now);
                    osa_time_realtime(&tv_now_real);

                    if (osa_timeval_cmp(&tv_now, &tv_expired) < 0)
                    {
                        tv_tmp.tv_sec = 0;
                        tv_tmp.tv_usec = 0;
                        tv_expired_real.tv_sec = 0;
                        tv_expired_real.tv_usec = 0;

                        timersub(&tv_expired, &tv_now, &tv_tmp);
                        timeradd(&tv_now_real, &tv_tmp, &tv_expired_real);

                        ts.tv_sec = tv_expired_real.tv_sec;
                        ts.tv_nsec = tv_expired_real.tv_usec*1000;

    					if(pthread_cond_timedwait(&pEvent->cv,&pEvent->lock,&ts)==ETIMEDOUT)
    					{
                            clock_gettime(CLOCK_MONOTONIC, &ts);
                            tv_cur.tv_sec = ts.tv_sec;
                            tv_cur.tv_usec = ts.tv_nsec/1000;

                            if (osa_timeval_cmp(&tv_cur, &tv_expired) >= 0)
                            {
                                /* timeout */
                                flag = 0;
                                ret  = OSA_FAILURE;
                                OSA_DBG_WARNING("pthread_cond_timedwait timeout(%ums) in osa_event_get()",udwWaitMS);
                            }
                            else
                            {
                                OSA_DBG_WARNING("pthread_cond_timedwait time jumping, continue waiting in osa_event_get()");
                            }
    					}
                    }
                    else
                    {
                        /* timeout */
                        flag = 0;
                        ret  = OSA_FAILURE;
                        OSA_DBG_WARNING("timeout(%ums) in osa_event_get()",udwWaitMS);
                    }

				 }
				 else
				 {
					pthread_cond_wait(&pEvent->cv,&pEvent->lock);
				 }

				 condition = 1;
			}
		}

		*pudwRealEvt = pEvent->eventBits;

		if(eOption & OSA_EVENT_CLEAR_REQUIRED)
		{
            pEvent->eventBits &= ~udwReqEvents;
		}

		lRet = pthread_mutex_unlock(&pEvent->lock);

        if (lRet != 0)
        {
            OSA_DBG_ERROR("pthread_mutex_unlock failed err-code=%d %s in osa_event_get()",lRet,strerror(lRet));
            ret = OSA_FAILURE;
        }

	}
	else
	{
        OSA_DBG_ERROR("invalid arguments in osa_event_get()");
		ret = OSA_FAILURE;
	}

	return  ret;
}

/*
************************************************************************************************
(4)UINT32 osa_event_set(OSA_EVENT_GRP_T *pEvent, UINT32 udwEvents)
    This function will set some evt flag into the event group pEvent. If another thread is waiting for this event, that thread should know this and be wake up.
*************************************************************************************************
*/
INT32 osa_event_set(OSA_EVENT_GRP_T *pEvent, UINT32 udwEvents)
{
	INT32   lRet = OSA_SUCCESS;
    
	if(pEvent && !pEvent->eventGrpDeleted)
	{
		lRet = pthread_mutex_lock(&pEvent->lock);
        if ( lRet != OSA_SUCCESS )
    	{
            OSA_DBG_ERROR("pthread_mutex_lock failed err-code=%d %s in osa_event_set()",lRet,strerror(lRet));
    	    lRet = OSA_FAILURE;

            return lRet;
    	}
        
		pEvent->eventBits |= udwEvents;
        
		lRet = pthread_cond_broadcast(&pEvent->cv);
        if ( lRet != OSA_SUCCESS )
    	{
            OSA_DBG_ERROR("pthread_cond_broadcast failed err-code=%d %s in osa_event_set()",lRet,strerror(lRet));
    	    lRet = OSA_FAILURE;

            return lRet;
    	}
        
		lRet = pthread_mutex_unlock(&pEvent->lock);
        if ( lRet != OSA_SUCCESS )
    	{
            OSA_DBG_ERROR("pthread_mutex_unlock failed err-code=%d %s in osa_event_set()",lRet,strerror(lRet));
    	    lRet = OSA_FAILURE;

            return lRet;
    	}
	}
	else
	{
        OSA_DBG_ERROR("invalid arguments in osa_event_set()");
		lRet = OSA_FAILURE;
	}

	return lRet;

}

/*
************************************************************************************************
(5)UINT32 osa_event_clear(OSA_EVENT_GRP_T *pEvent, UINT32 udwEvents)
    This function will clear the special evt-flag in the event group.
*************************************************************************************************
*/
INT32 osa_event_clear(OSA_EVENT_GRP_T *pEvent, UINT32 udwEvents)
{
	INT32   lRet = OSA_SUCCESS;
    
	if(pEvent && !pEvent->eventGrpDeleted)
	{
		lRet = pthread_mutex_lock(&pEvent->lock);
        if ( lRet != OSA_SUCCESS )
    	{
            OSA_DBG_ERROR("pthread_mutex_lock failed err-code=%d %s in osa_event_clear()",lRet,strerror(lRet));
    	    lRet = OSA_FAILURE;

            return lRet;
    	}
        
        pEvent->eventBits &= ~udwEvents;

        lRet = pthread_cond_broadcast(&pEvent->cv);
        if ( lRet != OSA_SUCCESS )
    	{
            OSA_DBG_ERROR("pthread_cond_broadcast failed err-code=%d %s in osa_event_clear()",lRet,strerror(lRet));
    	    lRet = OSA_FAILURE;

            return lRet;
    	}
        
        lRet = pthread_mutex_unlock(&pEvent->lock);
        if ( lRet != OSA_SUCCESS )
    	{
            OSA_DBG_ERROR("pthread_mutex_unlock failed err-code=%d %s in osa_event_clear()",lRet,strerror(lRet));
    	    lRet = OSA_FAILURE;

            return lRet;
    	}
	}
	else
	{
        OSA_DBG_ERROR("invalid arguments in osa_event_clear()");
		lRet = OSA_FAILURE;
	}

	return lRet;
}

#ifdef __cplusplus
#if __cplusplus
            }
#endif 
#endif/* __cpluscplus */
    


