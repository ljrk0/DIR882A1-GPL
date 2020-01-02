/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:        osa_resource.h
  Description:      header file for event operations
  History:
    1. zhonghai    2016-04-28         creation
        
*******************************************************************************/


#ifndef OSA_RESOURCE_H
#define OSA_RESOURCE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif 
#endif 

/** osa resource group status definition */
typedef enum _OSA_RESOURCE_GRP_STATUS_E
{
    OSA_RESOURCE_WAIT                = 0,
    OSA_RESOURCE_RELEASE             = 1,
    OSA_RESOURCE_CANCEL              = 2,
    OSA_RESOURCE_GRP_DELETED         = 3
}OSA_RESOURCE_GRP_STATUS_E;

/** osa resource callback definition */
typedef INT32 (*OSA_RESOURCE_CB_FUN)(void *pResourceData, OSA_RESOURCE_GRP_STATUS_E status, void *pExtraData);

/** osa resource group definition */
typedef struct _OSA_RESOURCE_GRP_T
{
    void *pResourceData;
    OSA_RESOURCE_CB_FUN cb;
    UINT8             resourceGrpDeleted;
    pthread_condattr_t condAttr;
    pthread_cond_t    cv;
    pthread_mutex_t   lock;
}OSA_RESOURCE_GRP_T;


/**
* create the resource group.
*
* @param[out] pResource the struct of the resource group
* @param[in] pResourceData control structure passed to pCb.
* @param[in] pCb callback function.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_resource_group_create(OSA_RESOURCE_GRP_T *pResource, void *pResourceData, OSA_RESOURCE_CB_FUN pCb);


/**
* delete the resource group.
*
* @param[in] pResource the struct of the resource group
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_resource_group_delete(OSA_RESOURCE_GRP_T *pResource);


/**
* wait for the resource.
*
* @param[in] pResource the struct of the resource group
* @param[in] pData extra argument passed to pCb.
* @param[in] udwWaitMS extra argument passed to pCb.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_resource_wait(OSA_RESOURCE_GRP_T *pResource, void *pData, UINT32 udwWaitMS);

/**
* release the resource.
*
* @param[in] pResource the struct of the resource group
* @param[in] pData extra argument passed to pCb.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_resource_release(OSA_RESOURCE_GRP_T *pResource, void *pData);

/**
* cancel the resource.
* only works before resource used
*
* @param[in] pResource the struct of the resource group
* @param[in] pData extra argument passed to pCb.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_resource_cancel(OSA_RESOURCE_GRP_T *pResource, void *pData);


#ifdef __cplusplus
#if __cplusplus
            }
#endif 
#endif/* __cpluscplus */
    

#endif



