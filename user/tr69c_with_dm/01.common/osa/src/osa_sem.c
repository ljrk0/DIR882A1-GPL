/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:         osa_sem.c
  Description: semaphore operations
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
  INT32 osa_sem_creat(OSA_SEM_T *pSem,INT32 key,INT32 dwInitValue)
  This function will create a sem.
 *************************************************************************************************
*/
INT32 osa_sem_create(OSA_SEM_T *pSem,INT32 key,INT32 dwInitValue)
{
    INT32   lRet = OSA_FAILURE;

	pSem->created_flag = OSA_TRUE;
	pSem->sem_id = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL);
	if(pSem->sem_id < 0)
    {
		if (errno == EEXIST) 
		{
			pSem->created_flag = OSA_FALSE;
			pSem->sem_id = semget(key, 1, 0666 | IPC_EXCL);
		}

		if(pSem->sem_id < 0)
		{
			OSA_DBG_ERROR("semget failed errno=%d %s in osa_sem_create()", errno, strerror(errno));
			return lRet;
		}
	}

	if (pSem->created_flag)
	{
		lRet = semctl(pSem->sem_id, 0, SETVAL, dwInitValue);/* set sem init-value */
		if(lRet)
		{
			OSA_DBG_ERROR("semctl SETVAL failed errno=%d %s in osa_sem_create()", errno, strerror(errno));
			return OSA_FAILURE;
		}
	}

    lRet = OSA_SUCCESS;

	return lRet;
}

/*
 ************************************************************************************************    
 INT32 INT32 osa_sem_delete(OSA_SEM_T *pSem)
 Delete a sem.
 *************************************************************************************************
*/
INT32 osa_sem_delete(OSA_SEM_T *pSem)
{
    INT32   lRet = OSA_SUCCESS;

	lRet = semctl(pSem->sem_id, 0, IPC_RMID, 0);

    if (lRet == OSA_SUCCESS)
    {
        pSem->sem_id = 0;
    }
    else
    {
        OSA_DBG_ERROR("semctl IPC_RMID failed errno=%d %s in osa_sem_delete()", errno, strerror(errno));
        lRet = OSA_FAILURE;
    }

    return lRet;
}
/*
 ************************************************************************************************
 INT32 osa_sem_wait(OSA_SEM_T *pSem)
 get/dec a sem.
 *************************************************************************************************
*/
INT32 osa_sem_wait(OSA_SEM_T *pSem)
{
    INT32   lRet = -1;
	struct sembuf operation[1];
	BOOL8 wait_more = 1;

	while(lRet == -1 && wait_more) {
		operation[0].sem_num = 0;
		operation[0].sem_op = -1; /* Decrement */
		operation[0].sem_flg = SEM_UNDO; /* Wait operation */

		lRet = semop(pSem->sem_id, operation, 1);
		if(lRet == -1 && errno == EINTR) {
			wait_more = 1;
		}
		else if (lRet == -1)
        {
            OSA_DBG_ERROR("semop failed errno=%d %s in osa_sem_wait()", errno, strerror(errno));
			wait_more = 0;
		}
        else
        {
            wait_more = 0;
        }
	}

    if (lRet == -1)
    {
        lRet = OSA_FAILURE;
    }
    else
    {
        lRet = OSA_SUCCESS;
    }

    return lRet;
}
/*
 ************************************************************************************************   
 INT32 osa_sem_post(OSA_SEM_T *pSem)
 put/add a sem.
 *************************************************************************************************
 */
INT32 osa_sem_post(OSA_SEM_T *pSem)
{
    INT32   lRet = OSA_SUCCESS;
	struct sembuf operation[1];

	operation[0].sem_num = 0;
	operation[0].sem_op = 1;	/* Increment by one */
	operation[0].sem_flg = SEM_UNDO;	/* Wait operation */

	lRet = semop(pSem->sem_id, operation, 1);

    if (lRet != -1)
    {
        lRet = OSA_SUCCESS;
    }
    else
    {
        OSA_DBG_ERROR("semop failed errno=%d %s in osa_sem_post()", errno, strerror(errno));
        lRet = OSA_FAILURE;
    }

    return lRet;
}

#ifdef __cplusplus
#if __cplusplus
        }
#endif 
#endif/* __cpluscplus */




