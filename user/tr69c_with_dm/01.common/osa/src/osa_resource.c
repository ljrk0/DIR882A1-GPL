/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:         dm_resource.c
  Description: operations for the resource group.
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

static INT32 osa_timespec_cmp(struct timeval *a, struct timeval *b)
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


INT32 osa_resource_group_create(OSA_RESOURCE_GRP_T *pResource, void *pResourceData, OSA_RESOURCE_CB_FUN pCb)
{
	INT32   lRet = OSA_SUCCESS;
    
	if(pResource)
	{
        pResource->pResourceData = pResourceData;
		pResource->cb = pCb;
        pResource->resourceGrpDeleted = 0;

        pthread_condattr_init(&(pResource->condAttr));
        //pthread_condattr_setclock(&(pResource->condAttr), CLOCK_REALTIME);

		lRet = pthread_cond_init(&pResource->cv, &(pResource->condAttr));
    	if ( lRet != OSA_SUCCESS )
    	{
    	    lRet = OSA_FAILURE;

            return lRet;
    	}
        
		lRet = pthread_mutex_init(&pResource->lock,NULL);
    	if ( lRet != OSA_SUCCESS )
    	{
    	    lRet = OSA_FAILURE;

            return lRet;
    	}
	}
	else
	{
		lRet = OSA_FAILURE;
	}
    
	return lRet;
}

INT32 osa_resource_group_delete(OSA_RESOURCE_GRP_T *pResource)
{
    INT32   lRet = OSA_SUCCESS;
    
	lRet = pthread_mutex_lock(&pResource->lock);
	if ( lRet != OSA_SUCCESS )
	{
	    lRet = OSA_FAILURE;

        return lRet;
	}

    pResource->resourceGrpDeleted = 1;
    pResource->cb(pResource->pResourceData, OSA_RESOURCE_GRP_DELETED, NULL);
    pResource->pResourceData = NULL;

	lRet = pthread_cond_broadcast(&pResource->cv);
	if ( lRet != OSA_SUCCESS )
	{
	    lRet = OSA_FAILURE;

        return lRet;
	}

	lRet = pthread_mutex_unlock(&pResource->lock);
	if ( lRet != OSA_SUCCESS )
	{
	    lRet = OSA_FAILURE;

        return lRet;
	}


	return lRet;
}

INT32 osa_resource_wait(OSA_RESOURCE_GRP_T *pResource, void *pData, UINT32 udwWaitMS)
{
	int flag = 1,ret = OSA_SUCCESS;
	int condition = 0;

	struct timespec ts;
    struct timeval tv_expired, tv_now, tv_cur, tv_expired_mono, tv_now_mono, tv_cur_mono, tv_tmp;

	if(pResource && !pResource->resourceGrpDeleted)
	{
		pthread_mutex_lock(&pResource->lock);

        if(udwWaitMS > 0)
        {
            /* use relative time in case of system time jumping for ntp sync or other reasons. */
            clock_gettime(CLOCK_REALTIME, &ts);

            tv_now.tv_sec  = ts.tv_sec;
            tv_now.tv_usec = ts.tv_nsec/1000;

            clock_gettime(CLOCK_MONOTONIC, &ts);

            tv_now_mono.tv_sec  = ts.tv_sec;
            tv_now_mono.tv_usec = ts.tv_nsec/1000;

            tv_tmp.tv_sec = udwWaitMS/1000;
            tv_tmp.tv_usec = (udwWaitMS%1000)*1000;

            timeradd(&tv_now, &tv_tmp, &tv_expired);
            timeradd(&tv_now_mono, &tv_tmp, &tv_expired_mono);
        }

		while(flag && !pResource->resourceGrpDeleted)
		{
            condition = pResource->cb(pResource->pResourceData, OSA_RESOURCE_WAIT, pData);

			if(condition == OSA_SUCCESS)
			{
				flag = 0;
			}

			while(condition != OSA_SUCCESS && !pResource->resourceGrpDeleted)
			{
                if(udwWaitMS > 0)
                {
                    clock_gettime(CLOCK_REALTIME, &ts);

                    tv_now.tv_sec  = ts.tv_sec;
                    tv_now.tv_usec = ts.tv_nsec/1000;

                    clock_gettime(CLOCK_MONOTONIC, &ts);

                    tv_now_mono.tv_sec = ts.tv_sec;
                    tv_now_mono.tv_usec = ts.tv_nsec/1000;

                    if (osa_timespec_cmp(&tv_now_mono, &tv_expired_mono) < 0)
                    {
                        /* wait for the time left */
                        timersub(&tv_expired_mono, &tv_now_mono, &tv_tmp);
                        timeradd(&tv_now, &tv_tmp, &tv_expired);

                        ts.tv_sec = tv_expired.tv_sec;
                        ts.tv_nsec = tv_expired.tv_usec*1000;

    					if(pthread_cond_timedwait(&pResource->cv,&pResource->lock,&ts)==ETIMEDOUT)
    					{
                            /* timeout */
                            clock_gettime(CLOCK_REALTIME, &ts);
                            tv_cur.tv_sec = ts.tv_sec;
                            tv_cur.tv_usec = ts.tv_nsec/1000;

                            clock_gettime(CLOCK_MONOTONIC, &ts);
                            tv_cur_mono.tv_sec = ts.tv_sec;
                            tv_cur_mono.tv_usec = ts.tv_nsec/1000;

                            if (osa_timespec_cmp(&tv_cur_mono, &tv_expired_mono) >= 0)
                            {
                                OSA_DBG_DEBUG("timeout expired(%ld.%06lds) now(%ld.%06lds) cur(%ld.%06lds)",tv_expired.tv_sec,tv_expired.tv_usec,tv_now.tv_sec,tv_now.tv_usec,tv_cur.tv_sec,tv_cur.tv_usec);
                                
                                flag = 0;
                                ret  = OSA_FAILURE;
                            }
                            else
                            {
                                OSA_DBG_DEBUG("system time jumping, not expired really, continue waiting");
                            }
    					}
                    }
                    else
                    {
                        /* timeout */
                        flag = 0;
                        ret  = OSA_FAILURE;
                    }
                }
                else
                {
                    pthread_cond_wait(&pResource->cv,&pResource->lock);

                }

                condition = OSA_SUCCESS;
			}
		}

		pthread_mutex_unlock(&pResource->lock);
	}
	else
	{
		ret = OSA_FAILURE;
	}

	return  ret;
}

INT32 osa_resource_release(OSA_RESOURCE_GRP_T *pResource, void *pData)
{
	INT32   lRet = OSA_SUCCESS;
    
	if(pResource && !pResource->resourceGrpDeleted)
	{
		lRet = pthread_mutex_lock(&pResource->lock);
        if ( lRet != OSA_SUCCESS )
    	{
    	    lRet = OSA_FAILURE;

            return lRet;
    	}

        if (pResource->resourceGrpDeleted)
        {
            pthread_mutex_unlock(&pResource->lock);
            lRet = OSA_FAILURE;
            return lRet;
        }

		pResource->cb(pResource->pResourceData, OSA_RESOURCE_RELEASE, pData);
        
		lRet = pthread_cond_broadcast(&pResource->cv);
        if ( lRet != OSA_SUCCESS )
    	{
    	    lRet = OSA_FAILURE;

            return lRet;
    	}
        
		lRet = pthread_mutex_unlock(&pResource->lock);
        if ( lRet != OSA_SUCCESS )
    	{
    	    lRet = OSA_FAILURE;

            return lRet;
    	}
	}
	else
	{
		lRet = OSA_FAILURE;
	}

	return lRet;
}

INT32 osa_resource_cancel(OSA_RESOURCE_GRP_T *pResource, void *pData)
{
	INT32   lRet = OSA_SUCCESS;
    INT32   lResult = OSA_SUCCESS;
    
	if(pResource && !pResource->resourceGrpDeleted)
	{
		lRet = pthread_mutex_lock(&pResource->lock);
        if ( lRet != OSA_SUCCESS )
    	{
    	    lRet = OSA_FAILURE;

            return lRet;
    	}

        if (pResource->resourceGrpDeleted)
        {
            pthread_mutex_unlock(&pResource->lock);
            lRet = OSA_FAILURE;
            return lRet;
        }

		lResult = pResource->cb(pResource->pResourceData, OSA_RESOURCE_CANCEL, pData);
        
		lRet = pthread_cond_broadcast(&pResource->cv);
        if ( lRet != OSA_SUCCESS )
    	{
    	    lRet = OSA_FAILURE;

            return lRet;
    	}
        
		lRet = pthread_mutex_unlock(&pResource->lock);
        if ( lRet != OSA_SUCCESS )
    	{
    	    lRet = OSA_FAILURE;

            return lRet;
    	}
	}
	else
	{
		lRet = OSA_FAILURE;
	}

    if (lRet == OSA_SUCCESS || lResult != OSA_SUCCESS)
    {
        lRet = lResult;
    }

	return lRet;
}

#ifdef __cplusplus
#if __cplusplus
            }
#endif 
#endif/* __cpluscplus */
    


