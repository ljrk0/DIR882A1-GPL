/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:        osa_sem.h
  Description:      header file for semaphore operations
  History:
    1. zhonghai    2013-10-08         creation
        
*******************************************************************************/


#ifndef OSA_SEM_H
#define OSA_SEM_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif 
#endif 

/** the structure of semaphore */
typedef struct _OSA_SEM_T{
    INT32 sem_id;
	BOOL8 created_flag;
}OSA_SEM_T;

/**
* check create_flag.
*/
#define osa_sem_created_flag(pSem) (pSem)->created_flag

/**
* create the semaphore.
*
* @param[out] pSem the struct of the semaphore
* @param[in] key use the same key can get the same semaphore.
* @param[in] dwInitValue the initial value of the semaphore.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_sem_create(OSA_SEM_T *pSem,INT32 key,INT32 dwInitValue);


/**
* delete one semaphore.
*
* @param[in] pSem the struct of the semaphore
* @return the result(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_sem_delete(OSA_SEM_T *pSem);


/**
* wait for the semaphore.
*
* @param[in] pSem the struct of the semaphore
* @return the result(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_sem_wait(OSA_SEM_T *pSem);


/**
* the semaphore post.
*
* @param[in] pSem the struct of the semaphore
* @return the result(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_sem_post(OSA_SEM_T *pSem);


#ifdef __cplusplus
#if __cplusplus
            }
#endif 
#endif/* __cpluscplus */
    

#endif


