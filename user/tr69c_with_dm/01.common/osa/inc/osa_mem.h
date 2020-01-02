/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:        osa_mem.h
  Description:      header file for memory operations
  History:
    1. zhonghai    2013-10-08         creation
        
*******************************************************************************/


#ifndef OSA_MEM_H
#define OSA_MEM_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif 
#endif 

/** memory operations */
#define OSA_MEM_ALLOC(bytes) osa_mem_alloc(bytes)
#define OSA_MEM_ALLOC_ST(type, count) (type*)osa_mem_alloc(sizeof(type)*(count))
#define OSA_MEM_FREE(p) {if((p)!=NULL){osa_mem_free(p);(p)=NULL;}}

#ifndef DMALLOC_CHECK_ENABLED

/**
* allocate memory.
*
* @param[in] udwSize bytes to allocate.
* @return the address of allocated memory.
*
*/
void *osa_mem_alloc(UINT32 udwSize);


/**
* free memory.
*
* @param[in] p the memory address.
* @return null
*
*/
void  osa_mem_free(void *p);
#else
#define osa_mem_alloc malloc
#define osa_mem_free free
#endif


/**
* copy memory.
*
* @param[in] pDes dest address.
* @param[in] pSrc source address.
* @param[in] ulNum bytes to copy.
* @return dest address.
*
*/
void *osa_mem_copy(void *pDes, void *pSrc, UINT32 ulNum );


/**
* reset memory with specified value.
*
* @param[in] p memory address.
* @param[in] ucVal the value to set
* @param[in] ulCnt bytes to reset.
* @return the address of allocated memory.
*
*/
void *osa_mem_set(void *p, UINT8 ucVal, UINT32 ulCnt);


/**
* compare memory data.
*
* @param[in] buf1 the first buffer to compare.
* @param[in] buf2 the second buffer to compare
* @param[in] len bytes to compare.
* @return the result. "<0" means buf1<buf2; "=0" means buf1=buf2; ">0" means buf1>buf2;
*
*/
INT32 osa_mem_cmp(const void *buf1, const void *buf2, UINT32 len);


#ifdef __cplusplus
#if __cplusplus
            }
#endif 
#endif/* __cpluscplus */
    

#endif


