/*
* ==============================================================================
*   Copyright (C) 2009, Percello Ltd, Inc.
* ==============================================================================
*/
/*******************************************************************************
* File name         : db_gsad.c
* System name    : femtocell PS engine
* Module            : Database Module
* description       : Database API for R/W DB file
* 
* ------------------------------------------------------------------------------
* Author            : zhonghai
* Department        : 
* Date created      : 2015-08-28
* Update            : 
*******************************************************************************/

/*
* ==============================================================================
* Revision: 
* ==============================================================================
* date&time         | modify description                           |Author
* ------------------------------------------------------------------------------
* 
* 
* ------------------------------------------------------------------------------
*/


#include "dm_internal.h"

#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif

BOOL8 DmGetCHandler(DM_GET_PARA_C_VALUE_REQ_T *  pGetPara)
{
    DM_HANDLER_PARA_T *pDmHandlerPara = NULL;
    INT8 *pDmHandlerValue = NULL;
    UINT32 i = 0;
    UINT32 childIndex = 0;
    xmlXPathObjectPtr xpathObj = NULL;
    xmlNodePtr nodeBak = NULL;
    BOOL8 bInstance = DM_FALSE;
    INT8 acNodeName[16] = {0};
    DM_NODE_INFO_T   *pPattern = NULL;
    UINT32 *alIndex = NULL;
    INT8 *pucTmpGetParaPath = NULL;
    UINT32 *pCnt = NULL;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmGetCHandler, path=%s id=0x%x transid=%d pvalue=0x%x size=%d cnt=%d",pGetPara->pucPath,pGetPara->ulParaId,pGetPara->TransactionId,pGetPara->pValue,pGetPara->ValueSize,pGetPara->ulCnt);

    pDmHandlerPara = (DM_HANDLER_PARA_T *)DM_MALLOC(sizeof(DM_HANDLER_PARA_T));

    if (pDmHandlerPara == NULL)
    {
        DM_WARNING("memory allocated for pDmHandlerPara failed, in DmGetCHandler, path=%s",pGetPara->pucPath);
        return DM_FALSE;
    }

    memset(pDmHandlerPara, 0, sizeof(DM_HANDLER_PARA_T));

    pDmHandlerPara->pDbHandler = GetDmHandlerByName(pGetPara->pucPath);
    if (pDmHandlerPara->pDbHandler == NULL)
    {
        DM_WARNING("failed to get dm handler, in DmGetCHandler, path=%s",pGetPara->pucPath);
        DM_FREE(pDmHandlerPara);
        return DM_FALSE;
    }

    pPattern = pDmHandlerPara->pDbHandler->pPattern;

    pDmHandlerPara->TransactionId = pGetPara->TransactionId;
    pDmHandlerPara->bCInterface = DM_TRUE;

    pDmHandlerPara->pucName = (INT8 *)DM_MALLOC(sizeof(INT8)*1024);
    if (pDmHandlerPara->pucName == NULL)
    {
        DM_WARNING("memory allocated for pDmHandlerPara->pucName failed, in DmGetCHandler, path=%s",pGetPara->pucPath);
        DM_FREE(pDmHandlerPara);
        return DM_FALSE;
    }

    memset(pDmHandlerPara->pucName, 0, 1024);
    strcpy((INT8*)pDmHandlerPara->pucName, (INT8*)pGetPara->pucPath);
    pDmHandlerValue = (INT8 *)pGetPara->pValue;

    if (pDmHandlerPara->pDbHandler->pDbIsObjectFunc(pPattern[pGetPara->ulParaId].ucParaType)==DM_TRUE)
    {
        /* object */
        if (pPattern[pGetPara->ulParaId].ulMaxElement>0)
        {
            /* multi-object */
            alIndex = (UINT32 *)DM_MALLOC(sizeof(UINT32)*(pPattern[pGetPara->ulParaId].ulMaxElement+1));
            pucTmpGetParaPath = (INT8 *)DM_MALLOC(sizeof(INT8)*1024);

            if (alIndex==NULL || pucTmpGetParaPath==NULL)
            {
                DM_WARNING("memory allocated for alIndex or pucTmpGetParaPath failed, in DmGetCHandler, path=%s",pGetPara->pucPath);
                DM_FREE(alIndex);
                DM_FREE(pucTmpGetParaPath);
                DM_FREE(pDmHandlerPara->pucName);
                DM_FREE(pDmHandlerPara);
                return DM_FALSE;
            }

            memset(alIndex, 0, sizeof(UINT32)*(pPattern[pGetPara->ulParaId].ulMaxElement+1));
            memset(pucTmpGetParaPath, 0, sizeof(INT8)*1024);

            bInstance = DmIsSingleOfMultiple((INT8 *)pGetPara->pucPath);
            if(DM_TRUE == bInstance)
            {
                pGetPara->ulCnt = 1;
                alIndex[0] = 1;
                alIndex[1] = DmGetInstanceNumFromPath((INT8 *)pGetPara->pucPath);
                memset(pDmHandlerPara->pucName, 0, 1024);
                strncpy(pucTmpGetParaPath, pGetPara->pucPath, DmGetObjectLengthOfInstance(pGetPara->pucPath));
            }
            else
            {
                strcpy(pucTmpGetParaPath, pGetPara->pucPath);
                if (DM_FALSE == DbGetObjectIndexCont(pDmHandlerPara->pDbHandler, pGetPara->pucPath, alIndex))
                {
                    DM_WARNING("DbGetObjectIndexCont failed, in DmGetCHandler, path=%s",pGetPara->pucPath);
                    DM_FREE(alIndex);
                    DM_FREE(pucTmpGetParaPath);
                    DM_FREE(pDmHandlerPara->pucName);
                    DM_FREE(pDmHandlerPara);

                    return DM_FALSE;
                }

                pGetPara->ulCnt = alIndex[0];
                memset(pDmHandlerPara->pucName, 0, 1024);
            }
        }
        else
        {
            /* ordinary-object */
            pGetPara->ulCnt = 1;
        }

        for(i=0;i<pGetPara->ulCnt;i++)
        {
            if (pPattern[pGetPara->ulParaId].ulMaxElement>0)
            {
                if(DM_FALSE == bInstance)
                {
                    nodeBak = pDmHandlerPara->pDbHandler->xpathCtx->node;
                    snprintf(acNodeName,sizeof(acNodeName),"i%u",alIndex[i+1]);
                    xpathObj = DbGetXPathObjPtr(pDmHandlerPara->pDbHandler->xpathCtx, (INT8 *)acNodeName);
                }

                /* instance number */
                *((UINT32*)(pDmHandlerValue+pPattern[pGetPara->ulParaId].ulCTypeSize*i)) = alIndex[i+1];
            }

            childIndex = pPattern[pGetPara->ulParaId].ulFirstChildIdx;
            while (childIndex!=(UINT32)(-1))
            {
                pCnt = NULL;
                memset(pDmHandlerPara->pucName, 0, 1024);
                if (pPattern[pGetPara->ulParaId].ulMaxElement>0)
                {
                    snprintf((INT8*)pDmHandlerPara->pucName,sizeof(INT8)*1024,"%s%d.",pucTmpGetParaPath, alIndex[i+1]);
                }
                else
                {
                    strcpy((INT8*)pDmHandlerPara->pucName, (INT8*)pGetPara->pucPath);
                }

                pDmHandlerPara->pucName = (INT8*)strcat((INT8*)pDmHandlerPara->pucName, pPattern[childIndex].pucNodeRealName);

                if (pDmHandlerPara->pDbHandler->pDbIsObjectFunc(pPattern[childIndex].ucParaType)==DM_TRUE)
                {
                    pDmHandlerPara->pucName = (INT8*)strcat((INT8*)pDmHandlerPara->pucName, ".");
                    pDmHandlerPara->bItem = DM_FALSE;
                }
                else
                {
                    pDmHandlerPara->bItem = DM_TRUE;
                }

                DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ pDmHandlerPara->pucName=%s in DmGetCHandler",pDmHandlerPara->pucName);

                pDmHandlerPara->ulParaId = childIndex;
                if (pPattern[childIndex].ulMaxElement>0)
                {
                    /* consider the counter for this multi-obj or array-parameter */
                    pCnt = (UINT32*)(pDmHandlerValue+pPattern[pGetPara->ulParaId].ulCTypeSize*i+pPattern[childIndex].ulElementCntOffset);

                    pDmHandlerPara->pValue = (void*)(pDmHandlerValue+pPattern[pGetPara->ulParaId].ulCTypeSize*i+pPattern[childIndex].ulSizeOffset);
                    pDmHandlerPara->valueSize = pPattern[childIndex].ulCTypeSize*pPattern[childIndex].ulMaxElement;
                }
                else
                {
                    pDmHandlerPara->pValue = (void*)(pDmHandlerValue+pPattern[pGetPara->ulParaId].ulCTypeSize*i+pPattern[childIndex].ulSizeOffset);
                    pDmHandlerPara->valueSize = pPattern[childIndex].ulCTypeSize;
                }

                DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ calling DmCallDefaultGetHandler in obj-case, in DmGetCHandler, path=%s child=%s",pGetPara->pucPath,pPattern[childIndex].pucNodeRealName);
                if (DmCallDefaultGetHandler(pDmHandlerPara)==DM_FALSE)
                {
                    DM_FREE(alIndex);
                    DM_FREE(pucTmpGetParaPath);
                    DM_FREE(pDmHandlerPara->pucName);
                    DM_FREE(pDmHandlerPara);
                    DM_WARNING("call DmCallDefaultGetHandler failed for object, leave DmGetCHandler, path=%s",pGetPara->pucPath);
                    return DM_FALSE;
                }

                if (pCnt)
                {
                    *pCnt = pDmHandlerPara->ulCnt;
                }

                childIndex = pPattern[childIndex].ulNextBrotherIdx;
            }

            if(pPattern[pGetPara->ulParaId].ulMaxElement>0 && bInstance==DM_FALSE)
            {
                pDmHandlerPara->pDbHandler->xpathCtx->node = nodeBak;
                xmlXPathFreeObject(xpathObj);
            }
        }

        DM_FREE(alIndex);
        DM_FREE(pucTmpGetParaPath);
    }
    else
    {
        /* item */
        pDmHandlerPara->ulParaId = pGetPara->ulParaId;
        pDmHandlerPara->bItem = DM_TRUE;
        pDmHandlerPara->pValue = (INT8*)pGetPara->pValue;
        pDmHandlerPara->valueSize = pGetPara->ValueSize;

        DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ calling DmCallDefaultGetHandler in item-case, in DmGetCHandler, path=%s",pGetPara->pucPath);
        if (DmCallDefaultGetHandler(pDmHandlerPara)==DM_FALSE)
        {
            DM_FREE(pDmHandlerPara->pucName);
            DM_FREE(pDmHandlerPara);
            DM_WARNING("call DmCallDefaultGetHandler failed for item, leave DmGetCHandler, path=%s",pGetPara->pucPath);
            return DM_FALSE;
        }

        if (pDmHandlerPara->pDbHandler->pPattern[pGetPara->ulParaId].ulMaxElement>0)
        {
            pGetPara->ulCnt = pDmHandlerPara->ulCnt;
        }
    }

    DM_FREE(pDmHandlerPara->pucName);
    DM_FREE(pDmHandlerPara);

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmGetCHandler, path=%s id=0x%x transid=%d pvalue=0x%x size=%d cnt=%d",pGetPara->pucPath,pGetPara->ulParaId,pGetPara->TransactionId,pGetPara->pValue,pGetPara->ValueSize,pGetPara->ulCnt);

    return DM_TRUE;
}

BOOL8 DmSetCHandler(DM_SET_PARA_C_VALUE_REQ_T *  pSetPara)
{
    DM_HANDLER_PARA_T          *pDmHandlerPara = NULL;
    INT8                       *pDmHandlerValue = NULL;
    UINT32                     i = 0;
    UINT32                     childIndex = 0;
    xmlXPathObjectPtr          xpathObj = NULL;
    xmlNodePtr                 nodeBak = NULL;
    BOOL8                      bInstance = DM_FALSE;
    INT8                       acNodeName[16] = {0};
    DM_NODE_INFO_T             *pPattern = NULL;
    UINT32                     instanceNum = 0;
    INT8                       *pucTmpParaPath = NULL;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmSetCHandler, path=%s id=0x%x transid=%d pvalue=0x%x size=%d cnt=%d",pSetPara->pucPath,pSetPara->ulParaId,pSetPara->TransactionId,pSetPara->pValue,pSetPara->ValueSize,pSetPara->ulCnt);

    pDmHandlerPara = (DM_HANDLER_PARA_T *)DM_MALLOC(sizeof(DM_HANDLER_PARA_T));

    if (pDmHandlerPara == NULL)
    {
        DM_WARNING("memory allocated for pDmHandlerPara failed, in DmSetCHandler, path=%s",pSetPara->pucPath);
        return DM_FALSE;
    }

    memset(pDmHandlerPara, 0, sizeof(DM_HANDLER_PARA_T));

    pDmHandlerPara->pDbHandler = GetDmHandlerByName(pSetPara->pucPath);

    if (pDmHandlerPara->pDbHandler == NULL)
    {
        DM_WARNING("failed to get dm handler, in DmSetCHandler, path=%s",pSetPara->pucPath);
        DM_FREE(pDmHandlerPara);
        return DM_FALSE;
    }

    pPattern = pDmHandlerPara->pDbHandler->pPattern;

    pDmHandlerPara->TransactionId = pSetPara->TransactionId;
    pDmHandlerPara->bCInterface = DM_TRUE;
    pDmHandlerPara->ulCnt = 1;

    pDmHandlerPara->pucName = (INT8 *)DM_MALLOC(sizeof(INT8)*1024);
    if (pDmHandlerPara->pucName == NULL)
    {
        DM_WARNING("memory allocated for pDmHandlerPara->pucName failed, in DmSetCHandler, path=%s",pSetPara->pucPath);
        DM_FREE(pDmHandlerPara);
        return DM_FALSE;
    }

    memset(pDmHandlerPara->pucName, 0, sizeof(INT8)*1024);

    if (DmIsMultipleObject(pPattern,pSetPara->ulParaId)==DM_FALSE)
    {
        strcpy((INT8*)pDmHandlerPara->pucName, (INT8*)pSetPara->pucPath);
    }

    pDmHandlerValue = (INT8 *)pSetPara->pValue;

    if (pDmHandlerPara->pDbHandler->pDbIsObjectFunc(pPattern[pSetPara->ulParaId].ucParaType)==DM_TRUE)
    {
        /* set object */
        pDmHandlerPara->bItem = DM_FALSE;

        if (pPattern[pSetPara->ulParaId].ulMaxElement>0)
        {
            /* multi-object */
            pucTmpParaPath = (INT8*)DM_MALLOC(sizeof(INT8)*1024);
            if (pucTmpParaPath == NULL)
            {
                DM_FREE(pDmHandlerPara);
                DM_WARNING("memory allocated for pucTmpParaPath failed, leave DmSetCHandler, path=%s",pSetPara->pucPath);
                return DM_FALSE;
            }

            memset(pucTmpParaPath, 0, sizeof(INT8)*1024);

            bInstance = DmIsSingleOfMultiple(pSetPara->pucPath);
            if(DM_TRUE == bInstance)
            {
                strncpy(pucTmpParaPath, pSetPara->pucPath, DmGetObjectLengthOfInstance(pSetPara->pucPath));
            }
            else
            {
                strcpy(pucTmpParaPath, pSetPara->pucPath);
            }
        }

        /* could set more than one object */
        for(i=0;i<pSetPara->ulCnt;i++)
        {
            if(pPattern[pSetPara->ulParaId].ulMaxElement>0)
            {
                /* skip instanceNum length */
                instanceNum = *((UINT32*)(pDmHandlerValue+pPattern[pSetPara->ulParaId].ulCTypeSize*i));

                if (bInstance==DM_FALSE)
                {
                    nodeBak = pDmHandlerPara->pDbHandler->xpathCtx->node;
                    memset(acNodeName,0,sizeof(INT8)*16);
                    snprintf(acNodeName,sizeof(acNodeName),"i%u",instanceNum);
                    xpathObj = DbGetXPathObjPtr(pDmHandlerPara->pDbHandler->xpathCtx, (INT8 *)acNodeName);
                }
            }

            /* set children */
            childIndex = pPattern[pSetPara->ulParaId].ulFirstChildIdx;
            while (childIndex!=(UINT32)(-1))
            {
                if (DmIsMultipleObject(pPattern,childIndex)==DM_TRUE)
                {
                    pDmHandlerPara->ulCnt = *((UINT32*)(pDmHandlerValue + pPattern[pSetPara->ulParaId].ulCTypeSize*i + pPattern[childIndex].ulSizeOffset - sizeof(UINT32)));

                    if (pDmHandlerPara->ulCnt != 0)
                    {
                        if (DmIsMultipleObject(pPattern,pSetPara->ulParaId)==DM_TRUE)
                        {
                            snprintf((INT8*)pDmHandlerPara->pucName,sizeof(INT8)*1024,"%s%u.",(INT8*)pucTmpParaPath,instanceNum);
                        }

                        pDmHandlerPara->pucName = (INT8*)strcat((INT8*)pDmHandlerPara->pucName,pPattern[childIndex].pucNodeRealName);
                        pDmHandlerPara->pucName = (INT8*)strcat((INT8*)pDmHandlerPara->pucName, ".");
                        pDmHandlerPara->bItem = DM_FALSE;
                        pDmHandlerPara->ulParaId = childIndex;
                        pDmHandlerPara->pValue = (void *)(pDmHandlerValue + pPattern[pSetPara->ulParaId].ulCTypeSize*i + pPattern[childIndex].ulSizeOffset);
                        pDmHandlerPara->valueSize = pPattern[childIndex].ulCTypeSize*pPattern[childIndex].ulMaxElement;

                        DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ calling DmCallDefaultSetHandler in multi-obj-case, in DmSetCHandler, path=%s",pSetPara->pucPath);
                        if (DmCallDefaultSetHandler(pDmHandlerPara)==DM_FALSE)
                        {
                            DM_FREE(pucTmpParaPath);
                            DM_FREE(pDmHandlerPara);
                            DM_WARNING("call DmCallDefaultSetHandler failed for multi-obj child %s, leave DmSetCHandler, path=%s",pPattern[childIndex].pucNodeRealName,pSetPara->pucPath);
                            return DM_FALSE;
                        }

                        memset(pDmHandlerPara->pucName, 0, 1024);

                        if (DmIsMultipleObject(pPattern,pSetPara->ulParaId)==DM_FALSE)
                        {
                            strcpy((INT8*)pDmHandlerPara->pucName, (INT8*)pSetPara->pucPath);
                        }
                    }

                }
                else
                {
                    if (DmIsMultipleObject(pPattern,pSetPara->ulParaId)==DM_TRUE)
                    {
                        snprintf((INT8*)pDmHandlerPara->pucName,sizeof(INT8)*1024,"%s%u.",pucTmpParaPath,instanceNum);
                    }

                    pDmHandlerPara->ulParaId = childIndex;
                    pDmHandlerPara->pucName = (INT8*)strcat((INT8*)pDmHandlerPara->pucName,pPattern[childIndex].pucNodeRealName);

                    if (pDmHandlerPara->pDbHandler->pDbIsObjectFunc(pPattern[childIndex].ucParaType)==DM_TRUE)
                    {
                        pDmHandlerPara->pucName = (INT8*)strcat((INT8*)pDmHandlerPara->pucName, ".");
                        pDmHandlerPara->bItem = DM_FALSE;
                        pDmHandlerPara->ulCnt = 1;
                        pDmHandlerPara->pValue = (void *)(pDmHandlerValue + pPattern[pSetPara->ulParaId].ulCTypeSize*i + pPattern[childIndex].ulSizeOffset);
                        pDmHandlerPara->valueSize = pPattern[childIndex].ulCTypeSize;

                    }
                    else
                    {
                        pDmHandlerPara->bItem = DM_TRUE;

                        if (pPattern[childIndex].ulMaxElement)
                        {
                            /* array-parameter */
                            pDmHandlerPara->ulCnt = *((UINT32*)(pDmHandlerValue + pPattern[pSetPara->ulParaId].ulCTypeSize*i + pPattern[childIndex].ulSizeOffset - sizeof(UINT32)));
                            pDmHandlerPara->valueSize = pPattern[childIndex].ulCTypeSize*pPattern[childIndex].ulMaxElement;

                        }
                        else
                        {
                            pDmHandlerPara->ulCnt = 1;
                            pDmHandlerPara->valueSize = pPattern[childIndex].ulCTypeSize;

                        }

                        pDmHandlerPara->pValue = (void *)(pDmHandlerValue + pPattern[pSetPara->ulParaId].ulCTypeSize*i + pPattern[childIndex].ulSizeOffset);
                    }

                    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ calling DmCallDefaultSetHandler in obj/item-case, in DmSetCHandler, path=%s",pSetPara->pucPath);
                    if (DmCallDefaultSetHandler(pDmHandlerPara)==DM_FALSE)
                    {
                        DM_FREE(pucTmpParaPath);
                        DM_FREE(pDmHandlerPara);
                        DM_WARNING("call DmCallDefaultSetHandler failed for simple-obj/item child %s, leave DmSetCHandler, path=%s",pPattern[childIndex].pucNodeRealName,pSetPara->pucPath);
                        return DM_FALSE;
                    }

                    if (DmIsMultipleObject(pPattern,pSetPara->ulParaId)==DM_TRUE)
                    {
                        memset(pDmHandlerPara->pucName, 0, 1024);
                    }
                    else
                    {
                        memset(pDmHandlerPara->pucName, 0, 1024);
                        strcpy((INT8*)pDmHandlerPara->pucName, (INT8*)pSetPara->pucPath);
                    }
                }

                childIndex = pPattern[childIndex].ulNextBrotherIdx;
            }

            if (pPattern[pSetPara->ulParaId].ulMaxElement>0 && bInstance==DM_FALSE)
            {
                pDmHandlerPara->pDbHandler->xpathCtx->node = nodeBak;
                xmlXPathFreeObject(xpathObj);
            }
        }

        DM_FREE(pucTmpParaPath);
    }
    else
    {
        /* set item */
        pDmHandlerPara->ulParaId = pSetPara->ulParaId;
        pDmHandlerPara->bItem = DM_TRUE;
        pDmHandlerPara->pValue = (INT8*)pSetPara->pValue;
        pDmHandlerPara->valueSize = pSetPara->ValueSize;
        pDmHandlerPara->ulCnt = pSetPara->ulCnt;

        DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ calling DmCallDefaultSetHandler in item-case, in DmSetCHandler, path=%s",pSetPara->pucPath);
        if (DmCallDefaultSetHandler(pDmHandlerPara)==DM_FALSE)
        {
            DM_FREE(pDmHandlerPara->pucName);
            DM_FREE(pDmHandlerPara);
            DM_WARNING("call DmCallDefaultSetHandler failed for item, leave DmSetCHandler, path=%s",pSetPara->pucPath);
            return DM_FALSE;
        }
    }

    DM_FREE(pDmHandlerPara->pucName);
    DM_FREE(pDmHandlerPara);

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmSetCHandler, path=%s",pSetPara->pucPath);
    return DM_TRUE;
}

BOOL8 DmAddCHandler(DM_ADD_OBJ_BY_ID_REQ_T *  pAddPara)
{
    DM_HANDLER_PARA_T *pDmHandlerPara = NULL;
    INT8 *pDmHandlerValue = NULL;
    UINT32 childIndex = 0;
    UINT32 cnt = 0;
    DM_NODE_INFO_T   *pPattern = NULL;
    UINT32 *pCnt = NULL;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmAddCHandler, path=%s id=0x%x transid=%d pvalue=0x%x size=%d",pAddPara->pucPath,pAddPara->ulObjId,pAddPara->TransactionId,pAddPara->pObject,pAddPara->objectSize);

    pDmHandlerPara = (DM_HANDLER_PARA_T *)DM_MALLOC(sizeof(DM_HANDLER_PARA_T));

    if (pDmHandlerPara == NULL)
    {
        DM_WARNING("memory allocated for pDmHandlerPara failed, in DmAddCHandler, path=%s",pAddPara->pucPath);
        return DM_FALSE;
    }

    memset(pDmHandlerPara, 0, sizeof(DM_HANDLER_PARA_T));

    pDmHandlerPara->pDbHandler = GetDmHandlerByName(pAddPara->pucPath);

    if (pDmHandlerPara->pDbHandler == NULL)
    {
        DM_WARNING("failed to get dm handler, in DmAddCHandler, path=%s",pAddPara->pucPath);
        DM_FREE(pDmHandlerPara);
        return DM_FALSE;
    }

    pPattern = pDmHandlerPara->pDbHandler->pPattern;

    pDmHandlerPara->TransactionId = pAddPara->TransactionId;
    pDmHandlerPara->bCInterface = DM_TRUE;
    pDmHandlerPara->ulCnt = 1;

    pDmHandlerPara->pucName = (INT8 *)DM_MALLOC(sizeof(INT8)*1024);

    if (pDmHandlerPara->pucName == NULL)
    {
        DM_WARNING("memory allocated for pDmHandlerPara->pucName failed, in DmAddCHandler, path=%s",pAddPara->pucPath);
        DM_FREE(pDmHandlerPara);
        return DM_FALSE;
    }

    memset(pDmHandlerPara->pucName, 0, sizeof(INT8)*1024);
    strcpy((INT8*)pDmHandlerPara->pucName, (INT8*)pAddPara->pucPath);

    pDmHandlerValue = (INT8 *)pAddPara->pObject;

    /* add its children */
    childIndex = pPattern[pAddPara->ulObjId].ulFirstChildIdx;
    while(childIndex!=(UINT32)(-1))
    {
        pDmHandlerPara->ulParaId = childIndex;
        pDmHandlerPara->pucName = (INT8*)strcat((INT8*)pDmHandlerPara->pucName, pPattern[childIndex].pucNodeRealName);

        if (pDmHandlerPara->pDbHandler->pDbIsObjectFunc(pPattern[childIndex].ucParaType)==DM_TRUE)
        {
            /* add sub-object */
            pDmHandlerPara->pucName = (INT8*)strcat((INT8*)pDmHandlerPara->pucName, ".");
            pDmHandlerPara->bItem = DM_FALSE;
            pDmHandlerPara->ulCnt = 1;

            if (pPattern[childIndex].ulMaxElement>0)
            {
                /* multi-object */

                /* skip multi-object counter */
                pCnt = (UINT32*)(pDmHandlerValue+pPattern[childIndex].ulElementCntOffset);
                DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ multi-obj child %s counter=%d, in DmAddCHandler, path=%s",pPattern[childIndex].pucNodeRealName,*pCnt,pAddPara->pucPath);

                for (cnt=0; cnt<*pCnt; cnt++)
                {
                    pDmHandlerPara->pValue = (void *)(pDmHandlerValue+pPattern[childIndex].ulCTypeSize*cnt+pPattern[childIndex].ulSizeOffset);
                    pDmHandlerPara->valueSize = pPattern[childIndex].ulCTypeSize;

                    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ calling DmCallDefaultAddHandler for multi-obj-child path=%s%s",pAddPara->pucPath,pPattern[childIndex].pucNodeRealName);
                    if (DmCallDefaultAddHandler(pDmHandlerPara)==DM_FALSE)
                    {
                        DM_FREE(pDmHandlerPara->pucName);
                        DM_FREE(pDmHandlerPara);
                        DM_WARNING("call DmCallDefaultAddHandler failed for multi-obj-child %s, leave DmAddCHandler, path=%s",pPattern[childIndex].pucNodeRealName,pAddPara->pucPath);
                        return DM_FALSE;
                    }
                }

            }
            else
            {
                /* ordinary object */
                pDmHandlerPara->pValue = (void *)(pDmHandlerValue+pPattern[childIndex].ulSizeOffset);
                pDmHandlerPara->valueSize = pPattern[childIndex].ulCTypeSize;

                DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ calling DmCallDefaultAddHandler for simple-obj-child path=%s%s",pAddPara->pucPath,pPattern[childIndex].pucNodeRealName);
                if (DmCallDefaultAddHandler(pDmHandlerPara)==DM_FALSE)
                {
                    DM_FREE(pDmHandlerPara->pucName);
                    DM_FREE(pDmHandlerPara);
                    DM_WARNING("call DmCallDefaultAddHandler failed for simple-obj-child %s, leave DmAddCHandler, path=%s",pPattern[childIndex].pucNodeRealName,pAddPara->pucPath);
                    return DM_FALSE;
                }
            }
        }
        else
        {
            /* add sub-item */
            pDmHandlerPara->bItem = DM_TRUE;

            if (pPattern[childIndex].ulMaxElement>0)
            {
                /* array-item */
                pDmHandlerPara->ulCnt = *((UINT32*)(pDmHandlerValue+pPattern[childIndex].ulSizeOffset-sizeof(UINT32)));
                pDmHandlerPara->pValue = (void *)(pDmHandlerValue+pPattern[childIndex].ulSizeOffset);
                pDmHandlerPara->valueSize = pPattern[childIndex].ulCTypeSize*pPattern[childIndex].ulMaxElement;
            }
            else
            {
                /* other-item */
                pDmHandlerPara->ulCnt = 1;
                pDmHandlerPara->pValue = (void *)(pDmHandlerValue+pPattern[childIndex].ulSizeOffset);
                pDmHandlerPara->valueSize = pPattern[childIndex].ulCTypeSize;
            }

            DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ calling DmCallDefaultAddHandler for item-child path=%s",pAddPara->pucPath,pPattern[childIndex].pucNodeRealName);
            if (DmCallDefaultAddHandler(pDmHandlerPara)==DM_FALSE)
            {
                DM_FREE(pDmHandlerPara->pucName);
                DM_FREE(pDmHandlerPara);
                DM_WARNING("call DmCallDefaultAddHandler failed for item-child %s, leave DmAddCHandler, path=%s",pPattern[childIndex].pucNodeRealName,pAddPara->pucPath);
                return DM_FALSE;
            }
        }

        memset(pDmHandlerPara->pucName, 0, 1024);
        strcpy((INT8*)pDmHandlerPara->pucName, (INT8*)pAddPara->pucPath);

        childIndex = pPattern[childIndex].ulNextBrotherIdx;
    }

    DM_FREE(pDmHandlerPara->pucName);
    DM_FREE(pDmHandlerPara);

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmAddCHandler, path=%s",pAddPara->pucPath);

    return DM_TRUE;
}

BOOL8 DmDelCHandler(DM_DEL_OBJ_BY_ID_REQ_T *  pDelPara)
{
    /* no use */
    return DM_TRUE;
}

