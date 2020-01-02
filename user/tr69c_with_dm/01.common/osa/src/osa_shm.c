/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:         osa_shm.c
  Description: the operations for shared memory
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

static void *osa_shm_attach(INT32 shm_id)
{
	void *shm_addr=NULL;

	shm_addr = shmat(shm_id, NULL, 0);

	if((int)shm_addr == -1)
    {
        OSA_DBG_ERROR("shmat failed errno=%d %s in osa_shm_attach()", errno, strerror(errno));
		shm_addr = NULL;
	}

	return(shm_addr);
}

INT32 osa_shm_create(OSA_SHM_T *pShm, UINT32 shmKey, UINT32 shmSize, void **pShmAddr)
{
    pShm->pshm_addr = NULL;
    pShm->key = shmKey;
    pShm->size = shmSize;
    pShm->size_bk = pShm->size^0xffffffff;
	pShm->shm_id = shmget(shmKey, shmSize, IPC_CREAT|C_IRUSR|C_IWUSR|C_IRGRP|C_IWGRP|C_IROTH|C_IWOTH);

	if(pShm->shm_id >= 0) {
		pShm->pshm_addr = osa_shm_attach(pShm->shm_id);
	}
    else
    {
        OSA_DBG_ERROR("shmget failed shmKey=0x%x shmSize=%u errno=%d %s in osa_shm_create()", shmKey, shmSize, errno, strerror(errno));
    }

    if (pShm->pshm_addr == NULL)
    {
        if (pShmAddr != NULL)
        {
            *pShmAddr = NULL;
        }

        return OSA_FAILURE;
    }
    else
    {
        if (pShmAddr != NULL)
        {
            *pShmAddr = pShm->pshm_addr;
        }

        return OSA_SUCCESS;
    }

}

INT32 osa_shm_delete(OSA_SHM_T *pShm, BOOL8 bCleanup)
{
    if (pShm->pshm_addr != NULL)
    {
        if (0 == shmdt(pShm->pshm_addr))
        {
            pShm->pshm_addr = NULL;
        }
        else
        {
            OSA_DBG_ERROR("shmdt failed errno=%d %s in osa_shm_delete()", errno, strerror(errno));
            return OSA_FAILURE;
        }
    }

    if (pShm->shm_id >= 0 && bCleanup)
    {
		if (shmctl(pShm->shm_id, IPC_RMID, 0) < 0)
		{
			OSA_DBG_ERROR("shmctl failed errno=%d %s in osa_shm_delete()", errno, strerror(errno));
			return OSA_FAILURE;
		}
		else
		{
			pShm->shm_id = -1;
		}
    }
	
    return OSA_SUCCESS;
}

