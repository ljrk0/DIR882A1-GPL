#include "dm_internal.h"
#include "dmJson_com.h"
#include "json.h"

/*-----------------------------------------------------------------------------------------------------------
 * local typedef
 *-----------------------------------------------------------------------------------------------------------*/
#define call_visit(vst, type, ...) ((visitor->visit##type) ? visitor->visit##type(__VA_ARGS__) : DM_RET_SUCCESS);

/*-----------------------------------------------------------------------------------------------------------
 * function implementation
 *-----------------------------------------------------------------------------------------------------------*/
/**
 * print json object as string
 */
static JSON_BOOL DmJson_renderCallback(const char* out, void* userData)
{
	FILE* fp = (FILE*)userData;

	if(fp) {
			fprintf(fp, "%s", out);
	} else {
			printf("%s", out);
	}
	return JSON_TRUE;
}

/**
 * print json object as string
 */
INT32 DmJson_printJson(DmJsonHandle jsonObject, INT8 *pucJsonFile, BOOL8 isFormat)
{
	INT32 lRet = DM_RET_SUCCESS;
	FILE* fp = NULL;

	if(pucJsonFile != NULL)
	{
		if((fp = fopen(pucJsonFile, "w")) == NULL)
		{
			DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON : failed to open render file (%s).", pucJsonFile);
			lRet = DM_RET_INTERNAL_ERROR;
		}
	}

	if(lRet == DM_RET_SUCCESS)
	{
		int jsonRet = JSON_Render((JSON_OBJECT *)jsonObject, isFormat, DmJson_renderCallback, fp);
		if(jsonRet != JSON_SUCCESS)
		{
			DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON : failed to render json data");
			lRet = DM_RET_INTERNAL_ERROR;
		}

		if(fp) fclose(fp);
	}

	return lRet;
}

/**
 * create json object
 */
DmJsonHandle DmJson_createReferObject(INT8* pNodeName, DM_JSON_REFER_OBJECT_TYPE type)
{
	JSON_OBJECT* jsonObject = ((type == DM_JSON_REFER_OBJECT_POD) ? JSON_CreatePlainObject() : JSON_CreateArrayObject() );
	if(jsonObject == NULL)
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Failed create JSON object(type=%d,name=%s) due to err=%d in DmJson_createJsonObject.",
				 type, pNodeName, JSON_getErrno());
	}

	return (DmJsonHandle)jsonObject;
}

/**
 * destory json object
 */
void DmJson_destroyJsonObject(DmJsonHandle* jsonObject)
{
	JSON_DeleteObject(*jsonObject);
	*jsonObject = NULL;
}

/**
 * create json string
 */
DmJsonHandle DmJson_createJsonString(const INT8* pvalue)
{
	JSON_OBJECT* jsonItem = JSON_CreateStringObject(pvalue);
	if(jsonItem == NULL)
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Failed create JSON string(value=%s) due to err=%d in DmJson_createJsonString.",
				pvalue, JSON_getErrno());
	}

	return (DmJsonHandle)jsonItem;
}

/**
 * create json Number
 */
DmJsonHandle DmJson_createJsonNumber(double value)
{
	JSON_OBJECT* jsonItem = JSON_CreateNumberObject(value);
	if(jsonItem == NULL)
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Failed create JSON number(value=%lf) due to err=%d in DmJson_createJsonNumber.",
				value, JSON_getErrno());
	}

	return (DmJsonHandle)jsonItem;
}

/**
 * create json Number
 */
DmJsonHandle DmJson_createJsonBOOL(BOOL8 value)
{
	JSON_OBJECT* jsonItem = JSON_CreateBoolObject(value);
	if(jsonItem == NULL)
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Failed create JSON boolean(value=%d) due to err=%d in DmJson_createJsonBOOL.",
				value, JSON_getErrno());
	}

	return (DmJsonHandle)jsonItem;
}

/**
 * create json NULL
 */
DmJsonHandle DmJson_createJsonNULL(void)
{
	JSON_OBJECT* jsonItem = JSON_CreateNULLObject();
	if(jsonItem == NULL)
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Failed create JSON NULL due to err=%d in DmJson_createJsonNULL.", JSON_getErrno());
	}

	return (DmJsonHandle)jsonItem;
}

/**
 * Add array item to array json
 */
INT32 DmJson_AddItemToArray(DmJsonHandle pArrayJson, DmJsonHandle pItemJson)
{
	DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

	INT32 targetType = ((JSON_OBJECT *)pArrayJson)->type;
	if( targetType != JSON_TYPE_ARRAY)
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Target json type(%d) is not array in DmJson_AddItemToArray.", targetType);
	}
	else
	{
		lRet = (JSON_AddObjectToArray(pArrayJson, pItemJson) == JSON_SUCCESS) ? DM_RET_SUCCESS : DM_RET_INTERNAL_ERROR;
	}

	return lRet;
}

/**
 * Add string to array json
 */
INT32 DmJson_AddStringToArray(DmJsonHandle pArrayJson, const INT8* pString)
{
	DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

	INT32 targetType = ((JSON_OBJECT *)pArrayJson)->type;
	if( targetType != JSON_TYPE_ARRAY)
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Target json type(%d) is not object in DmJson_AddStringToArray.", targetType);
	}
	else
	{
		lRet = (JSON_AddStringToArray(pArrayJson, pString) == JSON_SUCCESS) ? DM_RET_SUCCESS : DM_RET_INTERNAL_ERROR;
		if(lRet != DM_RET_SUCCESS)
		{
			DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ JSON_AddStringToArray failed(ret=%d) in DmJson_AddStringToArray.", JSON_getErrno());
		}
	}

	return lRet;
}

/**
 * Add number to array json
 */
INT32 DmJson_AddNumberToArray(DmJsonHandle pArrayJson, double number)
{
	DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

	INT32 targetType = ((JSON_OBJECT *)pArrayJson)->type;
	if( targetType != JSON_TYPE_ARRAY)
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Target json type(%d) is not object in DmJson_AddNumberToArray.", targetType);
	}
	else
	{
		lRet = (JSON_AddNumberToArray(pArrayJson, number) == JSON_SUCCESS) ? DM_RET_SUCCESS : DM_RET_INTERNAL_ERROR;
		if(lRet != DM_RET_SUCCESS)
		{
			DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ JSON_AddNumberToArray failed(ret=%d) in DmJson_AddNumberToJson.", JSON_getErrno());
		}
	}

	return lRet;
}

/**
 * Add boolean to array json
 */
INT32 DmJson_AddBoolToArray(DmJsonHandle pArrayJson, BOOL8 boolValue)
{
	DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

	INT32 targetType = ((JSON_OBJECT *)pArrayJson)->type;
	if(targetType != JSON_TYPE_ARRAY)
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Target json type(%d) is not object in DmJson_AddBoolToArray.", targetType);
	}
	else
	{
		lRet = (JSON_AddBoolToArray(pArrayJson, boolValue) == JSON_SUCCESS) ? DM_RET_SUCCESS : DM_RET_INTERNAL_ERROR;
		if(lRet != DM_RET_SUCCESS)
		{
			DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ JSON_AddBoolToArray Failed (ret=%d) in DmJson_AddBoolToArray.", JSON_getErrno());
		}
	}

	return lRet;
}

/**
 * Add null to array json
 */
INT32 DmJson_AddNULLToArray(DmJsonHandle pArrayJson)
{
	DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

	INT32 targetType = ((JSON_OBJECT *)pArrayJson)->type;
	if( targetType != JSON_TYPE_ARRAY)
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Target json type(%d) is not object in DmJson_AddNULLToArray.", targetType);
	}
	else
	{
		lRet = (JSON_AddNULLToArray(pArrayJson) == JSON_SUCCESS) ? DM_RET_SUCCESS : DM_RET_INTERNAL_ERROR;
		if(lRet != DM_RET_SUCCESS)
		{
			DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ JSON_AddNULLToArray Failed (ret=%d) in DmJson_AddNULLToArray.", JSON_getErrno());
		}
	}

	return lRet;
}

/**
 * Add object to POD json
 */
INT32 DmJson_AddItemToPOD(DmJsonHandle pPodJson, const INT8 *pName, DmJsonHandle pItemJson)
{
	DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

	INT32 targetType = ((JSON_OBJECT *)pPodJson)->type;
	if( targetType != JSON_TYPE_POD)
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Target json type(%d) is not object in DmJson_AddItemToPOD.", targetType);
	}
	else
	{
		lRet = (JSON_AddObjectToPlainObject(pPodJson, pName, pItemJson) == JSON_SUCCESS) ? DM_RET_SUCCESS : DM_RET_INTERNAL_ERROR;
	}

	return lRet;
}

/**
 * Add string to POD json
 */
INT32 DmJson_AddStringToPod(DmJsonHandle pPodJson, const INT8 *pName, const INT8* pString)
{
	DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

	INT32 targetType = ((JSON_OBJECT *)pPodJson)->type;
	if( targetType != JSON_TYPE_POD)
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Target json type(%d) is not object in DmJson_AddStringToPod.", targetType);
	}
	else
	{
		lRet = (JSON_AddStringToPlainObject(pPodJson, pName, pString) == JSON_SUCCESS) ? DM_RET_SUCCESS : DM_RET_INTERNAL_ERROR;
		if(lRet != DM_RET_SUCCESS)
		{
			DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ JSON_AddStringToPlainObject failed(item=%s, ret=%d) in DmJson_AddStringToPod.",
					pName, JSON_getErrno());
		}
	}

	return lRet;
}

/**
 * Add number to POD json
 */
INT32 DmJson_AddNumberToPod(DmJsonHandle pPodJson, const INT8 *pName, double number)
{
	DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

	INT32 targetType = ((JSON_OBJECT *)pPodJson)->type;
	if( targetType != JSON_TYPE_POD)
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Target json type(%d) is not object in DmJson_AddNumberToPod.", targetType);
	}
	else
	{
		lRet = (JSON_AddNumberToPlainObject(pPodJson, pName, number) == JSON_SUCCESS) ? DM_RET_SUCCESS : DM_RET_INTERNAL_ERROR;
		if(lRet != DM_RET_SUCCESS)
		{
			DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ JSON_AddNumberToPlainObject failed(item=%s, ret=%d) in DmJson_AddNumberToPod.",
					pName, JSON_getErrno());
		}
	}

	return lRet;
}

/**
 * Add boolean to POD json
 */
INT32 DmJson_AddBoolToPod(DmJsonHandle pPodJson, const INT8 *pName,BOOL8 boolValue)
{
	DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

	INT32 targetType = ((JSON_OBJECT *)pPodJson)->type;
	if( targetType != JSON_TYPE_POD)
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Target json type(%d) is not object in DmJson_AddBoolToPod.", targetType);
	}
	else
	{
		lRet = (JSON_AddBoolToPlainObject(pPodJson, pName, boolValue) == JSON_SUCCESS) ? DM_RET_SUCCESS : DM_RET_INTERNAL_ERROR;
		if(lRet != DM_RET_SUCCESS)
		{
			DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ JSON_AddBoolToPlainObject failed(item=%s, ret=%d) in DmJson_AddBoolToPod.",
					pName, JSON_getErrno());
		}
	}

	return lRet;
}

/**
 * Add null to POD json
 */
INT32 DmJson_AddNULLToPod(DmJsonHandle pPodJson, const INT8 *pName)
{
	DM_RET_E lRet = DM_RET_INTERNAL_ERROR;

	INT32 targetType = ((JSON_OBJECT *)pPodJson)->type;
	if( targetType != JSON_TYPE_POD)
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Target json type(%d) is not object in DmJson_AddNULLToPod.", targetType);
	}
	else
	{
		lRet = (JSON_AddNULLToPlainObject(pPodJson, pName) == JSON_SUCCESS) ? DM_RET_SUCCESS : DM_RET_INTERNAL_ERROR;
		if(lRet != DM_RET_SUCCESS)
		{
			DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ JSON_AddNULLToPlainObject failed(item=%s, ret=%d) in DmJson_AddNULLToPod.",
					pName, JSON_getErrno());
		}
	}

	return lRet;
}

/**
 * Parse JSON file
 */
INT32 DmJson_parseFile(INT8* pJsonFile, DmJsonHandle* pRootJson)
{
	DM_RET_E lRet = DM_RET_INTERNAL_ERROR;
	FILE* fp = NULL;
	JSON_OBJECT* parseJson = NULL;

	*pRootJson = NULL;

	if( (fp = fopen(pJsonFile, "r")) == NULL)
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Failed open JSON file(%s) due to err=%d(%s) in DmJson_parseFile.",
				pJsonFile, errno, strerror(errno));
		return lRet;
	}

	// calculate data buffer
	fseek(fp, 0, SEEK_END);
	INT32 fileSize = ftell(fp);
	INT8 *jsonBuff = DM_MALLOC(fileSize + 1);
	if(jsonBuff == NULL)
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Failed to allocate memory for json data in DmJson_parseFile.");
	}
	else
	{
		// read data buffer from file
		fseek(fp, 0, SEEK_SET);
		if(fread(jsonBuff, 1, fileSize, fp) < fileSize)
		{
			DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Failed to read json data from file in DmJson_parseFile.");
		}
		else if( (parseJson = JSON_Parse(jsonBuff)) == NULL)
		{
			DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM JSON ------ Failed parse JSON data due to err=%d:%s in DmJson_parseFile.",
					 JSON_getErrno(), JSON_GetParseError());
		}
		else
		{
			lRet = DM_RET_SUCCESS;
			*pRootJson = parseJson;

		}
	}

	fclose(fp);

	return lRet;
}

INT32 DmJson_IterPlainObject(DmJsonHandle rootJson, DM_JSON_POD_VISITOR* visitor, void* userdata)
{
	DM_RET_E lRet = DM_RET_INTERNAL_ERROR;
	JSON_ITERATOR iterator;

	int jsonret = JSON_InitIterator((JSON_OBJECT* )rootJson, &iterator);
	if(jsonret == JSON_SUCCESS && iterator.type == JSON_TYPE_POD && visitor)
	{
		while(JSON_IterNextChild(&iterator))
		{
			switch(iterator.current->type)
			{
			case JSON_TYPE_STRING:
				lRet = call_visit(visitor, String, iterator.name, JSON_GetValueOfStringObject(iterator.current), userdata);
				break;
			case JSON_TYPE_NUMBER:
				lRet = call_visit(visitor, Number, iterator.name, JSON_GetValueOfNumberObject(iterator.current), userdata);
				break;
			case JSON_TYPE_BOOLEAN:
				lRet = call_visit(visitor, BOOL, iterator.name, JSON_GetValueOfBoolObject(iterator.current), userdata);
				break;
			case JSON_TYPE_NULL:
				lRet = call_visit(visitor, NULL, iterator.name, userdata);
				break;
			default :
				lRet = call_visit(visitor, ReferObj, iterator.name, (DmJsonHandle)iterator.current, userdata);
				break;
			}

			if(lRet != DM_RET_SUCCESS)
			{
				break;
			}
		}
	}

	return lRet;
}

INT32 DmJson_IterArray(DmJsonHandle rootJson, DM_JSON_ARRAY_VISITOR* visitor, void* userdata)
{
	DM_RET_E lRet = DM_RET_INTERNAL_ERROR;
	JSON_ITERATOR iterator;

	int jsonret = JSON_InitIterator((JSON_OBJECT* )rootJson, &iterator);
	if(jsonret == JSON_SUCCESS && iterator.type == JSON_TYPE_ARRAY && visitor)
	{
		INT32 i;
		for(i = 0; JSON_IterNextChild(&iterator); i ++)
		{
			switch(iterator.current->type)
			{
			case JSON_TYPE_STRING:
				lRet = call_visit(visitor, String, i, JSON_GetValueOfStringObject(iterator.current), userdata);
				break;
			case JSON_TYPE_NUMBER:
				lRet = call_visit(visitor, Number, i, JSON_GetValueOfNumberObject(iterator.current), userdata);
				break;
			case JSON_TYPE_BOOLEAN:
				lRet = call_visit(visitor, BOOL, i, JSON_GetValueOfBoolObject(iterator.current), userdata);
				break;
			case JSON_TYPE_NULL:
				lRet = call_visit(visitor, NULL, i, userdata);
				break;
			default :
				lRet = call_visit(visitor, ReferObj, i, (DmJsonHandle)iterator.current, userdata);
				break;
			}

			if(lRet != DM_RET_SUCCESS)
			{
				break;
			}
		}
	}

	return lRet;
}
