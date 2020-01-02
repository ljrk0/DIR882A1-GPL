/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:         osa_mem.c
  Description: memory operations.
  History:
*******************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif 
#endif

#include "osa.h"

#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif

#ifndef DMALLOC_CHECK_ENABLED
void *osa_mem_alloc(UINT32 udwSize)
{
    if (0 == udwSize)
    {
        OSA_DBG_ERROR("invalid parameters in osa_mem_alloc()");
        return NULL;
    }
    else
    {
	    return malloc(udwSize);
    }
}

void  osa_mem_free(void *p)
{
    if( NULL != p )
    {
	    free(p);
	}

    return;
}
#endif

void *osa_mem_copy(void *pDes, void *pSrc, UINT32 ulNum )
{
    if ((NULL == pDes) || (NULL == pSrc) || (0 == ulNum))
    {
        OSA_DBG_ERROR("invalid parameters, pDes=0x%x pSrc=0x%x ulNum=%u in osa_mem_copy()",(UINT32)pDes,(UINT32)pSrc,ulNum);
        return NULL;
    }
    else
    {
	    return memcpy((void *)pDes, (void *)pSrc, (UINT32)ulNum);
    }
}

void *osa_mem_set(void *p, UINT8 ucVal, UINT32 ulCnt)
{
    if ((NULL == p) || (0 == ulCnt))
    {
        OSA_DBG_ERROR("invalid parameters, p=0x%x ulCnt=%u in osa_mem_set()",p,ulCnt);
        return NULL;
    }
    else
    {
	    return memset((void *)p,(UINT8)ucVal,(UINT32)ulCnt);
    }
}

INT32 osa_mem_cmp(const void *buf1, const void *buf2, UINT32 len)
{
    return memcmp(buf1,buf2,len);
}

#ifdef __cplusplus
#if __cplusplus
        }
#endif 
#endif/* __cpluscplus */


