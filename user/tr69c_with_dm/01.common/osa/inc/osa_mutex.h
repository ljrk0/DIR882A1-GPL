/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:        osa_debug.h
  Description:      header file for mutex operations
  History:
    1. zhonghai    2013-10-08         creation
        
*******************************************************************************/


#ifndef OSA_MUTEX_H
#define OSA_MUTEX_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif 
#endif 

/** OSA mutex type definition */
typedef struct _OSA_THREAD_MUTEX_T{
    pthread_mutex_t mutex;
}OSA_THREAD_MUTEX_T;

/**
* initialize one mutex with static mode
*
* @param[out] staticMutex the variable name of the mutex.
*/
#define osa_thread_mutex_init(staticMutex)  \
OSA_THREAD_MUTEX_T staticMutex = {  \
    .mutex = PTHREAD_MUTEX_INITIALIZER \
}

/**
* initialize one mutex.
*
* @param[out] pMutex the struct of the mutex.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_thread_mutex_create(OSA_THREAD_MUTEX_T *pMutex);


/**
* destroy the mutex.
*
* @param[in] pMutex the struct of the mutex.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_thread_mutex_delete(OSA_THREAD_MUTEX_T *pMutex);


/**
* lock the mutex.
*
* @param[in] pMutex the struct of the mutex.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_thread_mutex_lock(OSA_THREAD_MUTEX_T *pMutex);


/**
* lock the mutex.
*
* @param[in] pMutex the struct of the mutex.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_thread_mutex_trylock(OSA_THREAD_MUTEX_T *pMutex);


/**
* unlock the mutex.
*
* @param[in] pMutex the struct of the mutex.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_thread_mutex_unlock(OSA_THREAD_MUTEX_T *pMutex);


#ifdef __cplusplus
#if __cplusplus
            }
#endif 
#endif/* __cpluscplus */
    

#endif


