#include "dm_internal.h"
#include "dmJson_com.h"

/*-----------------------------------------------------------------------------------------------------------
 * function implementation
 *-----------------------------------------------------------------------------------------------------------*/
/**
 *  DmJson_startTransaction
 */
INT32 DmJson_startTransaction(char *rootNodeToLock, const INT8* pEntity, BOOL8 dataFlush, UINT32* tractionId)
{
    INT32 lRet = DM_RET_INTERNAL_ERROR;

    DM_TRANSACTION_REQ_T transReq;

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));

    strcpy(transReq.AccessEntity, pEntity);
    strncpy(transReq.RootNodeToLock, rootNodeToLock, sizeof(transReq.RootNodeToLock) - 1);
    transReq.TransactionId = 0;
    transReq.DataFlush = dataFlush;
    transReq.bNotCallNotify = DM_FALSE;
    transReq.timeout = 0;

    lRet = DmTransactionStartReq(&transReq);
    if (lRet != DM_RET_SUCCESS)
    {
    	DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON : DmTransactionStartReq failed lRet=%d", lRet);
        *tractionId = 0;
    }
    else
    {
        *tractionId = transReq.TransactionId;
    }

    return lRet;
}

/**
 *  DmJson_stopTransaction
 */
INT32 DmJson_stopTransaction(UINT32 tractionId)
{
    INT32 lRet = DM_RET_INTERNAL_ERROR;

    lRet = DmTransactionStopReq(tractionId);
    if (lRet != DM_RET_SUCCESS)
    {
    	DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON : DmTransactionStartReq failed lRet=%d", lRet);
    }

    return lRet;
}


/**
 *  DmJson_getChildNodePaths
 */
INT32 DmJson_getChildNodePaths(UINT32 tractionId, INT8 *pucPath, DM_JSON_NODE_PATH **pChildNodePaths, UINT32 *pChildNodeCount)
{
    INT32 lRet = DM_RET_INTERNAL_ERROR;

    DM_JSON_NODE_PATH *pNodePathList = NULL;
    UINT32 nodePathCount = 0;
	INT32 i;

    DM_GET_PARA_NAME_REQ_T getName;

    osa_mem_set(&getName, 0, sizeof(DM_GET_PARA_NAME_REQ_T));

    getName.TransactionId = tractionId;
    getName.pucParamName = pucPath;
    getName.bNextLevel   = DM_TRUE;
    getName.pfnFree      = NULL;
    getName.pNameList    = NULL;
    lRet = DmGetParaNameReq(&getName);
    if (lRet != DM_RET_SUCCESS)
    {
    	DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON : DmGetParaNameReq failed lRet=%d", lRet);
    }
    else if(getName.pNameList->udwCnt > 0)
    {
    	pNodePathList = DM_MALLOC(sizeof(DM_JSON_NODE_PATH) * getName.pNameList->udwCnt);
    	if(pNodePathList == NULL)
    	{
    		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON : failed to allocate memory for DM_JSON_NODE_PATH ");
    		lRet = DM_RET_INTERNAL_ERROR;
    	}
    	else
    	{
			nodePathCount = getName.pNameList->udwCnt;

			for(i = 0; i < nodePathCount; i++)
			{
				snprintf(pNodePathList[i].nodePath,  sizeof(pNodePathList[i].nodePath), "%s", getName.pNameList->pucParamName[i].pucName);
			}
			getName.pfnFree(&getName);
    	}
    }
    else
    {
    	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM JSON : Not found child node of %d in XML file", pucPath);
    }

    *pChildNodePaths = pNodePathList;
    *pChildNodeCount = nodePathCount;

    return lRet;
}

/**
 *  DmJson_readNodeAsJsonString
 */
INT32 DmJson_readNodeAsJsonString(UINT32 tractionId, INT8 *pucPath, DmJsonHandle* strJson)
{
    INT32 lRet = DM_RET_SUCCESS;

    PARAMETER_NAME_T        nameItem;
    PARAMETER_NAME_LIST_T   nameList;
    DM_GET_PARA_VALUE_REQ_T getValue;

    osa_mem_set(&nameItem, 0, sizeof(PARAMETER_NAME_T));
    osa_mem_set(&nameList, 0, sizeof(PARAMETER_NAME_LIST_T));
    osa_mem_set(&getValue, 0, sizeof(DM_GET_PARA_VALUE_REQ_T));

    nameItem.pucName = pucPath;

    nameList.udwCnt = 1;
    nameList.pucParamName = &nameItem;

    getValue.TransactionId = tractionId;
    getValue.pNameList = &nameList;

    lRet = DmGetParaValueReq(&getValue);
    if (lRet != DM_RET_SUCCESS || getValue.pvList->udwCnt != 1)
    {
    	DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON : DmGetParaValueReq failed lRet=%d, pucPath=%s,leave DmJson_readNodeAsJsonString", lRet, pucPath);
        lRet = DM_RET_INTERNAL_ERROR;
    }
    else 
    {
        INT8* pucValue = getValue.pvList->pstParamValue->pucValue;

        // change non-print char to space
    	INT8* pCursor;
    	for(pCursor = pucValue; *pCursor != '\0'; pCursor++) 
        {
            if(!isprint(*pCursor))
            {
                *pCursor = 0x20;
            }
        }

        if((*strJson = DmJson_createJsonString(pucValue)) == NULL)
        {
    	    DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON : Failed to create JSON string for pucPath=%s,leave DmJson_readNodeAsJsonString", lRet, pucPath);
            lRet = DM_RET_INTERNAL_ERROR;
        }
    }
    getValue.pfnFree(&getValue);

    return lRet;
}

/**
 *  DmJson_saveNodeList
 */
INT32 DmJson_saveNodeList(UINT32 tractionId, INT8 *pucBasePath, PARAMETER_VALUE_T* updValueList, UINT32 udwUpdCnt, UINT16* faultCodeList)
{
    INT32 lRet = DM_RET_SUCCESS;

    PARAMETER_VALUE_LIST_T  valList;
    DM_SET_PARA_VALUE_REQ_T setValue;

    osa_mem_set(&valList, 0, sizeof(PARAMETER_VALUE_LIST_T));
    osa_mem_set(&setValue, 0, sizeof(DM_SET_PARA_VALUE_REQ_T));

    valList.pstParamValue = updValueList;
    valList.udwCnt = udwUpdCnt;

    setValue.TransactionId = tractionId;
    setValue.bCreateIfNotExit = DM_TRUE;
    setValue.pvList = &valList;
    setValue.pFaultCodeList = faultCodeList;

    lRet = DmSetParaValueReq(&setValue);
    if (lRet != DM_RET_SUCCESS)
    {
    	DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON : DmSetParaValueReq failed lRet=%d, pucPath=%s,leave DmJson_saveNodeValue", lRet, pucBasePath);
    }

    return lRet;
}

/**
 *  DmJson_addNodeValue
 */
INT32 DmJson_addNode(UINT32 tractionId, INT8 *pucPath, UINT32* newInstanceNum)
{
    INT32 ret = DM_RET_SUCCESS;

    DM_ADD_OBJECT_REQ_T addObj;

    memset(&addObj, 0, sizeof(DM_ADD_OBJECT_REQ_T));

    addObj.TransactionId = tractionId;
    addObj.pucObjName = pucPath;

    ret = DmAddObjectReq(&addObj);
    if (ret != DM_RET_SUCCESS)
    {
        DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON :DmAddObjectByIdReq failed lRet=%d, pucPath=%s, leave DmJson_addNodeValue",ret, pucPath);
    }
    else
    {
    	*newInstanceNum = addObj.udwInstanceNum;
    }

    return ret;
}

/**
 *  DmJson_delNode
 */
INT32 DmJson_delNode(UINT32 tractionId, INT8 *pucPath)
{
    INT32 ret = DM_RET_SUCCESS;

    DM_DEL_OBJ_BY_ID_REQ_T delObj;

    memset(&delObj, 0, sizeof(DM_DEL_OBJ_BY_ID_REQ_T));

    delObj.TransactionId = tractionId;
    delObj.pucPath = pucPath;
    ret = DmDelObjectByIdReq(&delObj);
    if (ret != DM_RET_SUCCESS)
    {
        DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON :DmDelObjectByIdReq failed lRet=%d, pucPath=%s, leave DmJson_delNodeValue",ret, pucPath);
    }

    return ret;
}

/**
 *  DmJson_checkRight
 */
BOOL8 DmJson_checkRight(DM_HANDLER_T *pDmHandler, INT32 lEntityID, INT32 lId, INT32 lRightType)
{
	BOOL8 checkResult = DM_FALSE;

    if (lEntityID == 0xffff)
    {
        DM_TRACE(DM_TRACE_LEVEL_LOW, "DM JSON ------ root rights, leave DmJson_checkRight TRUE");
        checkResult = DM_TRUE;
    }
    else if (lEntityID <= 0)
    {
        DM_TRACE(DM_TRACE_LEVEL_LOW, "DM JSON ------ invalid entity(%d), leave DmJson_checkRight FALSE",lEntityID);
    }
    else
    {
    	UINT32 ulRightBitMap = lRightType == RIGHT_R ? pDmHandler->pPattern[lId].ulRead : pDmHandler->pPattern[lId].ulWrite;
		if((ulRightBitMap & lEntityID))
		{
			checkResult = DM_TRUE;
		}
    }

	return checkResult;
}
