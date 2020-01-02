/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:         osa_mutex.c
  Description: the mutex operations
  History:
*******************************************************************************/


#include "osa.h"
#include <assert.h>

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
  Function description:
  INT32 osa_thread_mutex_create(OSA_THREAD_MUTEX_T *pMutex)
  This function will create a mutex. mutex should system-scope.
  ************************************************************************************************
*/
INT32 osa_thread_mutex_create(OSA_THREAD_MUTEX_T *pMutex)
{
    INT32   lRet = OSA_SUCCESS;

    lRet = pthread_mutex_init(&(pMutex->mutex),NULL);
    if ( lRet != OSA_SUCCESS )
	{
        OSA_DBG_ERROR("pthread_mutex_init failed err-code=%d %s in osa_thread_mutex_create()",lRet,strerror(lRet));
	    lRet = OSA_FAILURE;

        return lRet;
	}
    
    return lRet;
}

/*
  ************************************************************************************************
  INT32 osa_thread_mutex_delete(OSA_THREAD_MUTEX_T *pMutex)
   Delete a mutex.
  ************************************************************************************************
*/
INT32 osa_thread_mutex_delete(OSA_THREAD_MUTEX_T *pMutex)
{
    INT32   lRet = OSA_SUCCESS;

    lRet = pthread_mutex_destroy(&(pMutex->mutex));
    if ( lRet != OSA_SUCCESS )
	{
        OSA_DBG_ERROR("pthread_mutex_destroy failed err-code=%d %s in osa_thread_mutex_delete()",lRet,strerror(lRet));
	    lRet = OSA_FAILURE;

        return lRet;
	}
    
    return lRet;
}
/*
  ************************************************************************************************
  INT32 osa_thread_mutex_lock(OSA_THREAD_MUTEX_T * pMutex)
  Lock a mutex
  ************************************************************************************************
*/
INT32 osa_thread_mutex_lock(OSA_THREAD_MUTEX_T *pMutex)
{
    INT32   lRet = OSA_SUCCESS;

    lRet = pthread_mutex_lock(&(pMutex->mutex));
    if ( lRet != OSA_SUCCESS )
	{
        OSA_DBG_ERROR("pthread_mutex_lock failed err-code=%d %s in osa_thread_mutex_lock()",lRet,strerror(lRet));
        assert(0);
	    lRet = OSA_FAILURE;

        return lRet;
	}
    
    return lRet;
}

/*
  ************************************************************************************************
  INT32 osa_mutex_trylock(OSA_MUTEX_T * pMutex)
  try to lock a mutex
  ************************************************************************************************
*/
INT32 osa_thread_mutex_trylock(OSA_THREAD_MUTEX_T *pMutex)
{
    INT32   lRet = OSA_SUCCESS;

    lRet = pthread_mutex_trylock(&(pMutex->mutex));
    if (EBUSY == lRet)
	{
        OSA_DBG_ERROR("pthread_mutex_trylock failed err-code=%d %s in osa_thread_mutex_trylock()",lRet,strerror(lRet));
	    lRet = OSA_FAILURE;

        return lRet;
	}
    
    return lRet;
}

/*
  ************************************************************************************************
   INT32 osa_thread_mutex_unlock(OSA_THREAD_MUTEX_T * pMutex)
   Unlock a mutex
  ************************************************************************************************
*/
INT32 osa_thread_mutex_unlock(OSA_THREAD_MUTEX_T *pMutex)
{
    INT32   lRet = OSA_SUCCESS;

    lRet = pthread_mutex_unlock(&(pMutex->mutex));
    if ( lRet != OSA_SUCCESS )
	{
        OSA_DBG_ERROR("pthread_mutex_unlock failed err-code=%d %s in osa_thread_mutex_unlock()",lRet,strerror(lRet));
        assert(0);
	    lRet = OSA_FAILURE;

        return lRet;
	}
    
	return lRet;
}


#ifdef __cplusplus
#if __cplusplus
        }
#endif 
#endif/* __cpluscplus */



