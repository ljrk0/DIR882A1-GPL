#include "dm_internal.h"
#include "dmJson_com.h"

/*-----------------------------------------------------------------------------------------------------------
 *  declare local function
 *-----------------------------------------------------------------------------------------------------------*/
static inline char* itemStrCType(DM_HANDLER_T *pDmHandler, DM_NODE_INFO_T* pNode)
{
	return pDmHandler->pCTypeStrHooker(pNode->ucParaCType);
}

static inline char* itemStrType(DM_HANDLER_T *pDmHandler, DM_NODE_INFO_T* pNode)
{
	return pDmHandler->pDataTypeStrHooker(pNode->ucParaType);
}

static inline INT32 getEntityIdByName(DM_HANDLER_T *pDmHandler, const INT8 *pEntityName)
{
    DB_ACCESS_NODE_T *pAccessNode = NULL;
    for(pAccessNode = pDmHandler->pAccessList; pAccessNode != NULL && pAccessNode->AccessUserName != NULL; pAccessNode++)
    {
        if(strcmp(pAccessNode->AccessUserName, pEntityName) == 0)
        {
            return pAccessNode->AccessUserId;
        }
    }

    return 0;
}

static INT32 DmJson_GetObject(DM_JSON_GET_OBJECT_T* pParam);
static INT32 DmJson_UpdObject(DM_JSON_UPD_OBJECT_T* pParam);

static INT32 DmJson_genJsonObjectTree(DM_HANDLER_T *pDmHandler, INT8* pucPath, const INT8* pAccessEntity, INT32 depth, DmJsonHandle* resultJson);
static INT32 DmJson_createJsonTree(DM_JSON_TRAVEL_CONEXT* travelConext, INT8* pucPath, INT32 lId, DmJsonHandle* resultJson);
static INT32 DmJson_createMetaJsonObject(DM_JSON_TRAVEL_CONEXT* travelConext, INT8* pucPath, UINT32 lId, DmJsonHandle* resultJson);
static INT32 DmJson_createValJsonObject(DM_JSON_TRAVEL_CONEXT* travelConext, INT8* pucPath, INT32 lId, DmJsonHandle* resultJson, UINT32* extralFlag);

static INT32 DmJson_travelChildOfMultiObj(DM_JSON_TRAVEL_CONEXT* travelConext, INT8* pucPath, INT32 lId, DmJsonHandle jsonArray);
static INT32 DmJson_travelChildOfSingleObj(DM_JSON_TRAVEL_CONEXT* travelConext, INT8* pucPath, INT32 lId, DmJsonHandle jsonArray);

static BOOL8 DmJson_checkIsLeafNode(DM_JSON_TRAVEL_CONEXT* travelConext, INT32 lId, BOOL8 isMultiObject);

static INT32 DmJson_analyzeUpdJson(DmJsonHandle jsonHandle, DM_JSON_UPD_CONTEXT* pUpdContext);
static INT32 DmJson_visitStringOfUpdJson(const INT8* pItemName, const INT8* value, void* userdata);
static INT32 DmJson_visitReferObjOfUpdJson(const INT8* pItemName, DmJsonHandle jsonObject, void* userdata);
static INT32 DmJson_countStringOfUpdItem(const INT8* pItemName, const INT8* value, void* userdata);
static INT32 DmJson_visitStringOfUpdItem(const INT8* pItemName, const INT8* value, void* userdata);
static INT32 DmJson_handleUpdJson(DM_JSON_UPD_CONTEXT* pUpdContext);
static INT32 DmJson_genUpdResultJson(DM_JSON_UPD_CONTEXT* pUpdContext, INT32 retCode, DmJsonHandle* resultJson);
static void  DmJson_clearUpdContext(DM_JSON_UPD_CONTEXT* pUpdContext);

/*-----------------------------------------------------------------------------------------------------------
 *  function implementation
 *-----------------------------------------------------------------------------------------------------------*/
INT32 DmJsonHandler(INT8* pReqMsg, UINT32 reqMsgSize)
{
	DM_RET_E lRet = DM_RET_INVALID_ARGUMENTS;

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM JSON ------ enter DmJsonHandler");

	if (pReqMsg == NULL || reqMsgSize <= sizeof(DM_JSON_REQ_MSG_T))
    {
        DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Invalid parameter, leave DmJsonHandler");
    	return lRet;
    }

	DM_JSON_REQ_MSG_T *pJsonReqMsg = (DM_JSON_REQ_MSG_T *)pReqMsg;
	switch(pJsonReqMsg->reqId)
	{
	case DM_JSON_REQ_MSG_GET_OBJECT :
	{
		if(reqMsgSize != sizeof(DM_JSON_GET_OBJECT_T))
		{
			DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Invalid data size of message(%d), leave DmJsonHandler", pJsonReqMsg->reqId);
		}
		else
		{
			lRet = DmJson_GetObject((DM_JSON_GET_OBJECT_T *)pReqMsg);
		}
		break;
	}
	case DM_JSON_REQ_MSG_UPD_OBJECT :
	{
		if(reqMsgSize != sizeof(DM_JSON_UPD_OBJECT_T))
		{
			DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Invalid data size of message(%d), leave DmJsonHandler", pJsonReqMsg->reqId);
		}
		else
		{
			lRet = DmJson_UpdObject((DM_JSON_UPD_OBJECT_T *)pReqMsg);
		}
		break;
	}
	default :
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Invalid request msg id(%d), leave DmJsonHandler", pJsonReqMsg->reqId);
		break;
	}
	
	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM JSON ------ leave DmJsonHandler(ret=%d)", lRet);

	return lRet;
}

/**
 *  DmJson_GetObject
 */
INT32 DmJson_GetObject(DM_JSON_GET_OBJECT_T* pParam)
{
	DM_RET_E lRet = DM_RET_INVALID_ARGUMENTS;
	INT32 lId = 0;

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM JSON ------ enter DmJson_GetObject");

	/* check parameter */
	DM_HANDLER_T *pDmHandler = GetDmHandlerByName(pParam->pucPath);
    if (pDmHandler == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ failed to find DM handler, leave DmJson_GetObject");
    }
    else if ((lId = DmGetIDFromPath(&(pDmHandler->hashNameID), pParam->pucPath)) < 0)
    {
        DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ invalid pucPath (%s), in DmJson_GetObject", pParam->pucPath);
    }
    else  if(!isObject(pDmHandler, lId))
	{
        DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Not object pucPath (%s), in DmJson_GetObject", pParam->pucPath);
	}
    else if(pParam->depth <= 0)
    {
    	DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ depth must greater than 0, in DmJson_GetObject", pParam->pucPath);
    }
	else
	{
		if(DmIsMultipleObject(pDmHandler->pPattern, lId) && pDmHandler->pPattern[lId].ulMaxElement > 100 )
		{
			pParam->depth = 1;
		}

		DmJsonHandle resultJson;
		lRet = DmJson_genJsonObjectTree(pDmHandler, pParam->pucPath, pParam->accessEntity, pParam->depth, &resultJson);
		if(lRet == DM_SUCCESS)
		{
			DmJson_printJson(resultJson, pParam->pucJsonFile, pParam->isFormat);
			DmJson_destroyJsonObject(&resultJson);
		}
	}

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM JSON ------ leave DmJson_GetObject(ret=%d)", lRet);

    return lRet;
}

/**
 *  DmJson_UpdObject
 */
INT32 DmJson_UpdObject(DM_JSON_UPD_OBJECT_T* pParam)
{
	DM_RET_E lRet = DM_RET_INVALID_ARGUMENTS;

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM JSON ------ enter DmJson_UpdObject");

	/* check parameter */
	if(access(pParam->pucJsonFile, F_OK) != 0)
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ failed to find source json file %s(%d:%s), leave DmJson_UpdObject", pParam->pucJsonFile, errno, strerror(errno));
	}
	else
	{
		DmJsonHandle jsonHandle = NULL;
		lRet = DmJson_parseFile(pParam->pucJsonFile, &jsonHandle);
		if(lRet == DM_RET_SUCCESS)
		{
			DM_JSON_UPD_CONTEXT updContext;
			osa_mem_set(&updContext, 0, sizeof(DM_JSON_UPD_CONTEXT));
			updContext.accessEntity = pParam->accessEntity;

			lRet = DmJson_analyzeUpdJson(jsonHandle, &updContext);
			DmJson_destroyJsonObject(&jsonHandle);

			if(lRet == DM_RET_SUCCESS)
			{
				DmJsonHandle resultJson = NULL;
				INT32 dbUpdRet = DmJson_handleUpdJson(&updContext);
				if(dbUpdRet == DM_RET_SUCCESS && updContext.action != DM_JSON_UPD_ACTION_DELETE)
				{
					lRet = DmJson_genJsonObjectTree(updContext.pDmHandler, updContext.basicPath, pParam->accessEntity, 1, &resultJson);
					if(lRet == DM_RET_SUCCESS)
					{
						lRet = DmJson_AddNumberToPod(resultJson, "ret", dbUpdRet);
						if(lRet != DM_RET_SUCCESS)
						{
							DmJson_destroyJsonObject(&resultJson);
						}
					}
				}
				else
				{
					lRet = DmJson_genUpdResultJson(&updContext, dbUpdRet, &resultJson);
				}

				if(lRet == DM_RET_SUCCESS)
				{
					lRet = DmJson_printJson(resultJson, pParam->pucJsonFile, pParam->isFormatResult);
					DmJson_destroyJsonObject(&resultJson);
				}
			}

			DmJson_clearUpdContext(&updContext);
		}
	}

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM JSON ------ leave DmJson_UpdObject(ret=%d)", lRet);

    return lRet;
}

/**
 *  generate JSON tree node for specified pucPath by DM_HANDLER_T & DB XML file
 */
INT32 DmJson_genJsonObjectTree(DM_HANDLER_T *pDmHandler, INT8* pucPath, const INT8* pAccessEntity, INT32 depth, DmJsonHandle* resultJson)
{
	DM_RET_E lRet = DM_RET_INTERNAL_ERROR;
	INT32 lId = DmGetIDFromPath(&(pDmHandler->hashNameID), pucPath);

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM JSON ------ enter DmJson_genJsonObjectTree");

	DM_JSON_TRAVEL_CONEXT travelConext = {
			.pDmHandler = pDmHandler,
			.maxDepth   = depth,
			.lEntityID  = getEntityIdByName(pDmHandler, pAccessEntity)
	};

	/* check read right */
	if(!DmJson_checkRight(travelConext.pDmHandler, travelConext.lEntityID, lId, RIGHT_R))
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM JSON ------ Failed check right for root node(%s)", pucPath);
		return lRet;
	}

	lRet = DmJson_startTransaction(pucPath, pAccessEntity, DM_FALSE, &travelConext.tractionId);
	if(lRet == DM_SUCCESS)
	{
		lRet = DmJson_createJsonTree(&travelConext, pucPath, lId, resultJson);
	}

	DmJson_stopTransaction(travelConext.tractionId);

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM JSON ------ leave DmJson_genJsonObjectTree(ret=%d)", lRet);

	return lRet;
}

/**
 * Travel DM node tree to create corresponding JSON tree, until the specified depth
 */
INT32 DmJson_createJsonTree(DM_JSON_TRAVEL_CONEXT* travelConext, INT8* pucPath, INT32 lId, DmJsonHandle* resultJson)
{
	DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM JSON ------ enter DmJson_createJsonTree(path=%s)", pucPath);

	*resultJson = NULL;

	/* create JSON tree node object { "meta" : {}, "val" : object, leaf:true/false } */
	DmJsonHandle jsonTreeNodeObject = DmJson_createReferObject(pucPath, DM_JSON_REFER_OBJECT_POD);
	if(jsonTreeNodeObject == NULL)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM JSON ------ Failed create JSON object of tree node (path=%s) in DmJson_createJsonTree.", pucPath);
		return lRet;
	}

	/* meta & instance id */
	DmJsonHandle jsonMetaObject = NULL;
	lRet = DmJson_createMetaJsonObject(travelConext, pucPath, lId, &jsonMetaObject);
	if(lRet != DM_SUCCESS)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM JSON ------ Failed create JSON meta object for tree node (path=%s) in DmJson_createJsonTree.", pucPath);
	}
	else
	{
		DmJson_AddItemToPOD(jsonTreeNodeObject, "meta", jsonMetaObject);
	}

	/* val & extral flag */
	if(lRet == DM_SUCCESS)
	{
		UINT32 extralFlag = DM_FALSE;
		DmJsonHandle jsonValObject = NULL;
		lRet = DmJson_createValJsonObject(travelConext, pucPath, lId, &jsonValObject, &extralFlag);

		if(lRet != DM_SUCCESS)
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM JSON ------ Failed create JSON value object for tree node (path=%s) in DmJson_createJsonTree.", pucPath);
		}
		else if(jsonValObject != NULL)
		{
			 DmJson_AddItemToPOD(jsonTreeNodeObject, "val", jsonValObject);
		}
		else
		{
			DmJson_AddNULLToPod(jsonTreeNodeObject, "val");
		}

		DmJsonHandle jsonFlagObject = DmJson_createJsonNumber((double)extralFlag);
		if(jsonFlagObject == NULL)
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM JSON ------ Failed create JSON leaf object for tree node (path=%s) in DmJson_createJsonTree.", pucPath);
		}
		else
		{
			DmJson_AddItemToPOD(jsonTreeNodeObject, "flag", jsonFlagObject);
		}
	}

	/* post handler */
	if(lRet != DM_SUCCESS)
	{
		DmJson_destroyJsonObject(&jsonTreeNodeObject);
	}

	*resultJson = jsonTreeNodeObject;

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM JSON ------ leave DmJson_createJsonTree(ret=%d)", lRet);

    return lRet;
}

/**
 *  Create JSON meta data for current tree node.
 */
INT32 DmJson_createMetaJsonObject(DM_JSON_TRAVEL_CONEXT* travelConext, INT8* pucPath, UINT32 lId, DmJsonHandle* resultJson)
{
	DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

	DM_NODE_INFO_T* pNode = &travelConext->pDmHandler->pPattern[lId];
	DmJsonHandle metaJson = DmJson_createReferObject(pNode->pucNodeName, DM_JSON_REFER_OBJECT_POD);
	if(metaJson == NULL)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM JSON ------ Failed create JSON meta object for node (%s) in DmJson_createMetaJsonObject.", pNode->pucNodeName);
		goto FINAL;
	}
	else
	{
		UINT32   instanceNum = 0;

		// set meta item
		lRet = DmJson_AddStringToPod(metaJson, "path",  pucPath);
		if(lRet != DM_SUCCESS) goto FINAL;

		if(DmIsMultipleObject(travelConext->pDmHandler->pPattern, lId))
		{
			instanceNum = DmGetInstanceNumFromPath(pucPath);
			if( instanceNum == 0)
			{
				lRet = DmJson_AddStringToPod(metaJson, "type",  "multi");
				if(lRet != DM_SUCCESS) goto FINAL;

				INT8 *pCType = itemStrCType(travelConext->pDmHandler, pNode);
				lRet = DmJson_AddStringToPod(metaJson, "cType", pCType);
				if(lRet != DM_SUCCESS) goto FINAL;

				lRet = DmJson_AddNumberToPod(metaJson, "max", pNode->ulMaxElement);
				if(lRet != DM_SUCCESS) goto FINAL;
			}
			else
			{
				lRet = DmJson_AddStringToPod(metaJson, "type",  "instance");
				if(lRet != DM_SUCCESS) goto FINAL;

				lRet = DmJson_AddNumberToPod(metaJson, "instNum", instanceNum);
				if(lRet != DM_SUCCESS) goto FINAL;
			}
		}
		else if(isObject(travelConext->pDmHandler, lId))
		{
			// set meta item
			lRet = DmJson_AddStringToPod(metaJson, "path",  pucPath);
			if(lRet != DM_SUCCESS) goto FINAL;

			INT8 *pObjType = itemStrType(travelConext->pDmHandler, pNode);
			lRet = DmJson_AddStringToPod(metaJson, "type",  pObjType);
			if(lRet != DM_SUCCESS) goto FINAL;

			INT8 *pCType = itemStrCType(travelConext->pDmHandler, pNode);
			lRet = DmJson_AddStringToPod(metaJson, "cType", pCType);
			if(lRet != DM_SUCCESS) goto FINAL;
		}
		else
		{
			// set meta item
			lRet = DmJson_AddStringToPod(metaJson, "path",  pucPath);
			if(lRet != DM_SUCCESS) goto FINAL;

			INT8 *pObjType = itemStrType(travelConext->pDmHandler, pNode);
			lRet = DmJson_AddStringToPod(metaJson, "type",  pObjType);
			if(lRet != DM_SUCCESS) goto FINAL;

			INT8 *pCType = itemStrCType(travelConext->pDmHandler, pNode);
			lRet = DmJson_AddStringToPod(metaJson, "cType", pCType);
			if(lRet != DM_SUCCESS) goto FINAL;

			lRet = DmJson_AddStringToPod(metaJson, "range",  pNode->pValueRange);
			if(lRet != DM_SUCCESS) goto FINAL;
		}
	}

FINAL:
	if(lRet != DM_SUCCESS)
	{
		DmJson_destroyJsonObject(&metaJson);
	}
	*resultJson = metaJson;

	return lRet;
}

/**
 * Create JSON value data for current tree node.
 */
INT32 DmJson_createValJsonObject(DM_JSON_TRAVEL_CONEXT* travelConext, INT8* pucPath, INT32 lId, DmJsonHandle* resultJson, UINT32* extralFlag)
{
	DM_RET_E lRet = DM_RET_INTERNAL_ERROR;
	BOOL8 isleafFlag = DM_TRUE;

	DM_NODE_INFO_T *pNode = travelConext->pDmHandler->pPattern + lId;
	*resultJson = NULL;

	// create value JSON node
	if(DmIsMultipleObject(travelConext->pDmHandler->pPattern, lId) &&
	   DmGetInstanceNumFromPath(pucPath) == 0)
	{
		if(travelConext->maxDepth > 0)
		{
			/* create ARRAY JSON to contain all items  */
			DmJsonHandle jsonValueArray = DmJson_createReferObject(pucPath, DM_JSON_REFER_OBJECT_ARRAY);
			if(jsonValueArray == NULL)
			{
				DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM JSON ------ Failed create JSON meta object for node (%s) in DmJson_createValJsonObject.", pNode->pucNodeName);
			}
			else
			{
				travelConext->maxDepth --;
				lRet = DmJson_travelChildOfMultiObj(travelConext, pucPath, lId, jsonValueArray);
				travelConext->maxDepth ++;
				*resultJson = jsonValueArray;
			}
		}
		else
		{
			lRet = DM_RET_SUCCESS;
		}
		isleafFlag =  DmJson_checkIsLeafNode(travelConext, lId, DM_TRUE);
	}
	else if(isObject(travelConext->pDmHandler, lId))
	{
		/* create ARRAY JSON to contain all items  */
		DmJsonHandle jsonValueArray = DmJson_createReferObject(pucPath, DM_JSON_REFER_OBJECT_ARRAY);
		if(jsonValueArray == NULL)
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM JSON ------ Failed create JSON meta object for node (%s) in DmJson_createValJsonObject.", pNode->pucNodeName);
		}
		else if(travelConext->maxDepth > 0)
		{
			travelConext->maxDepth --;
			lRet = DmJson_travelChildOfSingleObj(travelConext, pucPath, lId, jsonValueArray);
			travelConext->maxDepth ++;
			*resultJson = jsonValueArray;
		}
		else
		{
			lRet = DM_RET_SUCCESS;
		}
		isleafFlag =  DmJson_checkIsLeafNode(travelConext, lId, DM_FALSE);
	}
	else
	{
		/* basic NODE : fetch value from datebase */
	    lRet = DmJson_readNodeAsJsonString(travelConext->tractionId, pucPath, resultJson);
	    isleafFlag = DM_TRUE;
	}

	// create extral flag
	BOOL8 writableFlag = DmJson_checkRight(travelConext->pDmHandler, travelConext->lEntityID, lId, RIGHT_W);
	*extralFlag = ((writableFlag << 1) | isleafFlag);

    return lRet;
}

/**
 * travel child item for multi object
 */
INT32 DmJson_travelChildOfMultiObj(DM_JSON_TRAVEL_CONEXT* travelConext, INT8* pucPath, INT32 lId, DmJsonHandle jsonArray)
{
	DM_RET_E lRet = DM_RET_INTERNAL_ERROR;
	UINT32  i, childNodeCount = 0;

	/* check read right */
	if(!DmJson_checkRight(travelConext->pDmHandler, travelConext->lEntityID, lId, RIGHT_R))
	{
		return DM_RET_SUCCESS;
	}

	/* get each child path from DB.xml & trval each item */
	DM_JSON_NODE_PATH *pChildNodePaths = NULL;
	lRet = DmJson_getChildNodePaths(travelConext->tractionId, pucPath, &pChildNodePaths, &childNodeCount);

	for(i = 0; lRet == DM_SUCCESS && i < childNodeCount; i++)
	{
		DmJsonHandle childJsonObject = NULL;
		lRet = DmJson_createJsonTree(travelConext, pChildNodePaths[i].nodePath, lId, &childJsonObject);
        if(lRet != DM_SUCCESS)
        {
            break;
        }

        DmJson_AddItemToArray(jsonArray, childJsonObject);
	}

    if(pChildNodePaths != NULL)
    {
    	DM_FREE(pChildNodePaths);
    }

    return lRet;
}

/**
 * travel child item for single object
 */
INT32 DmJson_travelChildOfSingleObj(DM_JSON_TRAVEL_CONEXT* travelConext, INT8* pucParentPath, INT32 lId, DmJsonHandle jsonArray)
{
	DM_RET_E lRet = DM_RET_INTERNAL_ERROR;
	INT32 i;
	INT8 pucChildPath[DM_JSON_MAX_NODE_PATH];

    DM_NODE_INFO_T* pCurrentNode = &travelConext->pDmHandler->pPattern[lId];
    INT32 lChildId = pCurrentNode->ulFirstChildIdx;

    for(i = 0; i < pCurrentNode->ulMaxChildren; i ++)
    {
        DM_NODE_INFO_T* pChildNode = &travelConext->pDmHandler->pPattern[lChildId];

    	/* get child path */
        if(isObject(travelConext->pDmHandler, lChildId))
        {
        	snprintf(pucChildPath, sizeof(pucChildPath), "%s%s.", pucParentPath, pChildNode->pucNodeRealName);
        }
        else
        {
        	snprintf(pucChildPath, sizeof(pucChildPath), "%s%s", pucParentPath, pChildNode->pucNodeRealName);
        }

    	/* check read right */
    	if(DmJson_checkRight(travelConext->pDmHandler, travelConext->lEntityID, lChildId, RIGHT_R))
    	{
			/* travel current child node detail for sub tree or value */
			DmJsonHandle childJsonObject = NULL;
			lRet = DmJson_createJsonTree(travelConext, pucChildPath, lChildId, &childJsonObject);
			if(lRet != DM_SUCCESS)
			{
				break;
			}

			DmJson_AddItemToArray(jsonArray, childJsonObject);
    	}

        lChildId = pChildNode->ulNextBrotherIdx;
    }

    return lRet;
}

BOOL8 DmJson_checkIsLeafNode(DM_JSON_TRAVEL_CONEXT* travelConext, INT32 lId, BOOL8 isMultiObject)
{
	BOOL8 leafFlag = DM_TRUE;
	INT32 i;

	DM_NODE_INFO_T* pCurrentNode = &travelConext->pDmHandler->pPattern[lId];

	if(isMultiObject)
	{
		INT8 tmp[DM_JSON_MAX_NODE_PATH];
		snprintf(tmp, sizeof(tmp), "%s", pCurrentNode->pucNodeName);
		tmp[strlen(tmp)-4] = 0;
		leafFlag = (DbGetObjectNum(travelConext->pDmHandler, tmp) == 0);
	}
	else
	{
		INT32 lChildId = pCurrentNode->ulFirstChildIdx;

		for(i = 0; i < pCurrentNode->ulMaxChildren; i ++)
		{
			DM_NODE_INFO_T* pChildNode = &travelConext->pDmHandler->pPattern[lChildId];
			if(isObject(travelConext->pDmHandler, lChildId))
			{
				leafFlag = DM_FALSE;
				break;
			}
			lChildId = pChildNode->ulNextBrotherIdx;
		}
	}

    return leafFlag;
}

INT32 DmJson_analyzeUpdJson(DmJsonHandle jsonHandle, DM_JSON_UPD_CONTEXT* pUpdContext)
{
	INT32 lRet = DM_RET_INTERNAL_ERROR;

	DM_JSON_POD_VISITOR vistor;
	osa_mem_set(&vistor, 0, sizeof(vistor));
	vistor.visitString = DmJson_visitStringOfUpdJson;
	vistor.visitReferObj = DmJson_visitReferObjOfUpdJson;

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM JSON ------ enter DmJson_analyzeUpdJson");

	/* fetch update item from JSON data */
	lRet = DmJson_IterPlainObject(jsonHandle, &vistor, (void *)pUpdContext);
	if(lRet == DM_RET_SUCCESS)
	{
		if(pUpdContext->action == DM_JSON_UPD_ACTION_MODIFY && pUpdContext->udwUpdCnt == 0)
		{
			DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ modified item list is null DmJson_analyzeUpdJson");
			lRet = DM_RET_INTERNAL_ERROR;
		}
		else if(pUpdContext->basicPath[0] == '\0')
		{
			DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ target path list is null DmJson_analyzeUpdJson");
			lRet = DM_RET_INTERNAL_ERROR;
		}
		else
		{
			if(pUpdContext->action == DM_JSON_UPD_ACTION_ADD || pUpdContext->action == DM_JSON_UPD_ACTION_DELETE)
			{
				UINT32 lId = DmGetIDFromPath(&(pUpdContext->pDmHandler->hashNameID), pUpdContext->basicPath);
				if(!DmJson_checkRight(pUpdContext->pDmHandler, pUpdContext->lAccessEntityId, lId, RIGHT_W))
				{
					DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ %s is denied to add/delete instance to %s", 
													pUpdContext->accessEntity, pUpdContext->basicPath);
					lRet = DM_RET_REQUEST_DENIED;
				}
			}
		}
	}

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM JSON ------ leave DmJson_analyzeUpdJson(ret=%d)", lRet);

	return lRet;
}

INT32 DmJson_visitStringOfUpdJson(const INT8* pItemName, const INT8* value, void* userdata)
{
	INT32 lRet = DM_RET_SUCCESS;
	DM_JSON_UPD_CONTEXT* pUpdContext = (DM_JSON_UPD_CONTEXT*)userdata;

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM JSON ------ enter DmJson_visitStringOfUpdJson");

	if(strcmp(pItemName, "path") == 0)
	{
		if(value[0] == '\0')
		{
			DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ pucPath is null in DmJson_visitStringOfUpdJson.");
			lRet = DM_RET_INVALID_ARGUMENTS;
		}
		else
		{
			snprintf(pUpdContext->basicPath, sizeof(pUpdContext->basicPath), "%s", value);
			
			pUpdContext->pDmHandler = GetDmHandlerByName(pUpdContext->basicPath);
			if(pUpdContext->pDmHandler == NULL)
			{
				DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ failed to find DM handler in DmJson_visitStringOfUpdJson");
				lRet = DM_RET_INVALID_ARGUMENTS;
			}
			else 
			{
				pUpdContext->lAccessEntityId = getEntityIdByName(pUpdContext->pDmHandler, pUpdContext->accessEntity);
			}
		}
	}
	else if(strcmp(pItemName, "action") == 0)
	{
		if(value[0] == 'M')
		{
			pUpdContext->action = DM_JSON_UPD_ACTION_MODIFY;
		}
		else if(value[0] == 'A')
		{
			pUpdContext->action = DM_JSON_UPD_ACTION_ADD;
		}
		else if(value[0] == 'D')
		{
			pUpdContext->action = DM_JSON_UPD_ACTION_DELETE;
		}
		else
		{
			DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ action is null in DmJson_visitStringOfUpdJson.");
			pUpdContext->action = DM_JSON_UPD_ACTION_NULL;
			lRet = DM_RET_INVALID_ARGUMENTS;
		}
	}

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM JSON ------ leave DmJson_visitStringOfUpdJson(ret=%d)", lRet);

	return lRet;
}

INT32 DmJson_visitReferObjOfUpdJson(const INT8* pItemName, DmJsonHandle jsonObject, void* userdata)
{
	INT32 lRet = DM_RET_SUCCESS;
	DM_JSON_UPD_CONTEXT* pUpdContext = (DM_JSON_UPD_CONTEXT*)userdata;

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM JSON ------ enter DmJson_visitReferObjOfUpdJson");

	if(strcmp(pItemName, "list") == 0)
	{
		UINT32 udwUpdCnt = 0;
		DM_JSON_POD_VISITOR itemVistor;
		osa_mem_set(&itemVistor, 0, sizeof(itemVistor));

		itemVistor.visitString = DmJson_countStringOfUpdItem;
		lRet = DmJson_IterPlainObject(jsonObject, &itemVistor, (void *)&udwUpdCnt);  // count update item
		if(lRet == DM_RET_SUCCESS && udwUpdCnt > 0)
		{
			pUpdContext->updValueList = DM_MALLOC(sizeof(PARAMETER_VALUE_T) * udwUpdCnt);
			pUpdContext->faultCodeList = DM_MALLOC(sizeof(UINT16) * udwUpdCnt);
			if(pUpdContext->updValueList == NULL || pUpdContext->faultCodeList == NULL)
			{
				DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON : failed to allocate memory for updValueList or faultCodeList in DmJson_visitReferObjOfUpdJson");
				lRet = DM_RET_INTERNAL_ERROR;
			}
			else
			{
				// fetch update item to update list
				osa_mem_set(pUpdContext->updValueList, 0, sizeof(PARAMETER_VALUE_T) * udwUpdCnt);
				osa_mem_set(pUpdContext->faultCodeList, 0, sizeof(UINT16) * udwUpdCnt);

				itemVistor.visitString = DmJson_visitStringOfUpdItem;
				lRet = DmJson_IterPlainObject(jsonObject, &itemVistor, (void *)pUpdContext);
			}
		}
	}

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM JSON ------ leave DmJson_visitReferObjOfUpdJson(ret=%d)", lRet);

	return lRet;
}

INT32 DmJson_countStringOfUpdItem(const INT8* pItemName, const INT8* value, void* userdata)
{
	(*(UINT32*)userdata) ++;
	return DM_RET_SUCCESS;
}

INT32 DmJson_visitStringOfUpdItem(const INT8* pItemName, const INT8* value, void* userdata)
{
	INT32 lRet = DM_RET_SUCCESS;
	DM_JSON_UPD_CONTEXT* pUpdContext = (DM_JSON_UPD_CONTEXT*)userdata;
	INT8  pucPath[DM_JSON_MAX_NODE_PATH];

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM JSON ------ enter DmJson_visitStringOfUpdItem(%s:%s)", pItemName, value);

	snprintf(pucPath, sizeof(pucPath), "%s%s", pUpdContext->basicPath, pItemName);

	UINT32 lId = DmGetIDFromPath(&(pUpdContext->pDmHandler->hashNameID), pucPath);
	if(!DmJson_checkRight(pUpdContext->pDmHandler, pUpdContext->lAccessEntityId, lId, RIGHT_W))
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ %s is denied to modify item %s", pUpdContext->accessEntity, pucPath);
		lRet = DM_RET_REQUEST_DENIED;
		return lRet;
	}

	PARAMETER_VALUE_T* pCurValue = &pUpdContext->updValueList[pUpdContext->udwUpdCnt];
	pUpdContext->udwUpdCnt ++;

	pCurValue->pucName = strdup(pucPath);
	pCurValue->pucValue = strdup(value);

	if(pCurValue->pucName == NULL || pCurValue->pucValue == NULL)
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON : failed to allocate memory for pucName or pucValue in DmJson_visitStringOfUpdItem");
		if(pCurValue->pucName != NULL) DM_FREE(pCurValue->pucName);
		if(pCurValue->pucValue != NULL) DM_FREE(pCurValue->pucValue);

		lRet = DM_RET_INTERNAL_ERROR;
	}

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM JSON ------ leave DmJson_visitStringOfUpdItem(ret=%d)", lRet);

	return lRet;
}

INT32 DmJson_handleUpdJson(DM_JSON_UPD_CONTEXT* pUpdContext)
{
	INT32 lRet = DM_RET_INTERNAL_ERROR;
	UINT32 tractionId;
	UINT32 newInstanceNum = 0;

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM JSON ------ enter DmJson_handleUpdJson");

	/* Update each item */
	lRet = DmJson_startTransaction(pUpdContext->basicPath, pUpdContext->accessEntity, DM_TRUE, &tractionId);
	if(lRet == DM_RET_SUCCESS)
	{
		switch(pUpdContext->action)
		{
		case DM_JSON_UPD_ACTION_ADD :
			lRet = DmJson_addNode(tractionId, pUpdContext->basicPath, &newInstanceNum);
			if(lRet == DM_RET_SUCCESS) {
				snprintf(pUpdContext->basicPath + strlen(pUpdContext->basicPath), sizeof(pUpdContext->basicPath) - strlen(pUpdContext->basicPath), "%d.", newInstanceNum);
			}
			break;
		case DM_JSON_UPD_ACTION_DELETE :
			lRet = DmJson_delNode(tractionId, pUpdContext->basicPath);
			break;
		case DM_JSON_UPD_ACTION_MODIFY:
			lRet = DmJson_saveNodeList(tractionId, pUpdContext->basicPath, pUpdContext->updValueList, pUpdContext->udwUpdCnt, pUpdContext->faultCodeList);
			break;
		default :
			break;
		}
	}

	DmJson_stopTransaction(tractionId);

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM JSON ------ leave DmJson_handleUpdJson(ret=%d)", lRet);

	return lRet;
}

INT32 DmJson_genUpdResultJson(DM_JSON_UPD_CONTEXT* pUpdContext, INT32 retCode, DmJsonHandle* resultJson)
{
	INT32 lRet = DM_RET_INTERNAL_ERROR;
	UINT32 i;

	DmJsonHandle errJson = DmJson_createReferObject(pUpdContext->basicPath, DM_JSON_REFER_OBJECT_POD);
	if(errJson != NULL)
	{
		lRet = DmJson_AddNumberToPod(errJson, "ret", retCode);

		if(lRet == DM_RET_SUCCESS && pUpdContext->udwUpdCnt > 0)
		{
			DmJsonHandle faultJson = DmJson_createReferObject(pUpdContext->basicPath, DM_JSON_REFER_OBJECT_POD);
			if(faultJson != NULL)
			{
				lRet = DmJson_AddItemToPOD(errJson, "fault", faultJson);

				for(i = 0; lRet == DM_RET_SUCCESS && i < pUpdContext->udwUpdCnt; i++)
				{
					lRet = DmJson_AddNumberToPod(faultJson, pUpdContext->updValueList[i].pucName, pUpdContext->faultCodeList[i]);
				}
			}
		}

		if(lRet != DM_RET_SUCCESS)
		{
			DmJson_destroyJsonObject(&errJson);
		}
	}

	*resultJson = errJson;

	return lRet;
}

void DmJson_clearUpdContext(DM_JSON_UPD_CONTEXT* pUpdContext)
{
	if(pUpdContext->action == DM_JSON_UPD_ACTION_MODIFY)
	{
		UINT32 i;
		for(i = 0; i < pUpdContext->udwUpdCnt; i++)
		{
			DM_FREE(pUpdContext->updValueList[i].pucName);
			DM_FREE(pUpdContext->updValueList[i].pucValue);
		}
		DM_FREE(pUpdContext->updValueList);
		DM_FREE(pUpdContext->faultCodeList);
	}
}
