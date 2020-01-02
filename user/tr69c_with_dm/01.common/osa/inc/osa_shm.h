/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:        osa_shm.h
  Description:      header file for shared memory operations
  History:
    1. zhonghai    2013-10-08         creation
        
*******************************************************************************/


#ifndef OSA_SHM_H
#define OSA_SHM_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif 
#endif 

/** the structure of shared memory */
typedef struct _OSA_SHM_T{
    UINT32 size;
    UINT32 size_bk;
    INT32 shm_id;
    void* pshm_addr;
    UINT32 key;
}OSA_SHM_T;

/**
* create or get a shared-memory segment.
* if other processes have called this function with the same arguments before, the action here is just "get".
* if not, creation is done.
*
* @param[out] pShm the struct of the shared memory.
* @param[in] shmKey the key to indicate shared memory.
* @param[in] shmSize the message to receive.
* @param[out] pShmAddr return the mapping address in the address space of the calling process.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
* Notice, if you want to shared this shm between different processes, you should use the same shmKey and shmSize for them.
*
*/
INT32 osa_shm_create(OSA_SHM_T *pShm, UINT32 shmKey, UINT32 shmSize, void **pShmAddr);


/**
* release the shared-memory segment from the address space of the current process.
* if it is the last caller of this shm segment, the shm segment will be deleted.
*
* @param[in] pShm  the struct of the shared memory.
* @param[in] bCleanup OSA_TRUE: cleanup from OS.  OSA_FALSE : detach only
*
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_shm_delete(OSA_SHM_T *pShm, BOOL8 bCleanup);

#ifdef __cplusplus
#if __cplusplus
            }
#endif 
#endif/* __cpluscplus */
    

#endif


