/*******************************************************************************
 *
 *   Copyright (c) 2009-2016 Broadcom Corporation
 *   All rights reserved
 *
 *   FILE NAME     : dm_json.h
 *   PROGRAMMER(S) :
 *   DATE CREATED  : 11-APR-2016
 *   DESCRIPTION   :
 *   CAUTIONS      : none
 *
 *******************************************************************************/

#ifndef _DM_JSON_COM_H_
#define _DM_JSON_COM_H_
#ifdef __cplusplus
    extern "C" {
#endif

#include "common_type_def.h"
#include "osa.h"
#include "dmJson_addon.h"
#include "device_external_type.h"

/**
 * definition
 */
typedef enum {
	DM_JSON_REFER_OBJECT_POD,
	DM_JSON_REFER_OBJECT_ARRAY
} DM_JSON_REFER_OBJECT_TYPE;

typedef VOID* DmJsonHandle;

/**
 * structure
 */
typedef struct DmJsonTravelContext
{
	DM_HANDLER_T *pDmHandler;
	UINT32       tractionId;
	INT32        lEntityID;
	INT32        maxDepth;
} DM_JSON_TRAVEL_CONEXT;

typedef struct DmJsonNodePath
{
	INT8         nodePath[DM_JSON_MAX_NODE_PATH];
} DM_JSON_NODE_PATH;

typedef struct DmJsonUpdContext
{
	DM_HANDLER_T *pDmHandler;
	INT8         *accessEntity;
	INT32        lAccessEntityId;

#define DM_JSON_UPD_ACTION_NULL    0
#define DM_JSON_UPD_ACTION_ADD     1
#define DM_JSON_UPD_ACTION_DELETE  2
#define DM_JSON_UPD_ACTION_MODIFY  3
	INT32 action;
	INT8  basicPath[DM_JSON_MAX_NODE_PATH];

	UINT32 udwUpdCnt;
	PARAMETER_VALUE_T*   updValueList;
	UINT16*              faultCodeList;
} DM_JSON_UPD_CONTEXT;

typedef struct DmJsonPodVistor
{
	INT32 (*visitString)(const INT8* pItemName, const INT8* value, void* userdata);
	INT32 (*visitNumber)(const INT8* pItemName, double value, void* userdata);
	INT32 (*visitBOOL)(const INT8* pItemName, BOOL8 value, void* userdata);
	INT32 (*visitNULL)(const INT8* pItemName, void* userdata);
	INT32 (*visitReferObj)(const INT8* pItemName, DmJsonHandle jsonObject, void* userdata);
} DM_JSON_POD_VISITOR;

typedef struct DmJsonArrayVistor
{
	INT32 (*visitString)(INT32 index, const INT8* value, void* userdata);
	INT32 (*visitNumber)(INT32 index, double value, void* userdata);
	INT32 (*visitBOOL)(INT32 index, BOOL8 value, void* userdata);
	INT32 (*visitNULL)(INT32 index, void* userdata);
	INT32 (*visitReferObj)(INT32 index, DmJsonHandle jsonObject, void* userdata);
} DM_JSON_ARRAY_VISITOR;

/**
 * function (Main)
 */
extern INT32 DmJsonHandler(INT8* pReqMsg, UINT32 reqMsgSize);

/**
 * function (DB utility)
 */
extern INT32 DmJson_startTransaction(char *rootNodeToLock, const INT8* pEntity, BOOL8 dataFlush, UINT32* tractionId);
extern INT32 DmJson_stopTransaction(UINT32 tractionId);
extern INT32 DmJson_getChildNodePaths(UINT32 tractionId, INT8 *pucPath, DM_JSON_NODE_PATH **pChildNodePaths, UINT32 *pChildNodeCount);
extern INT32 DmJson_readNodeAsJsonString(UINT32 tractionId, INT8 *pucPath, DmJsonHandle* strJson);
extern INT32 DmJson_saveNodeList(UINT32 tractionId, INT8 *pucBasePath, PARAMETER_VALUE_T* updValueList, UINT32 udwUpdCnt, UINT16* faultCodeList);
extern INT32 DmJson_addNode(UINT32 tractionId, INT8 *pucPath, UINT32* newInstanceNum);
extern INT32 DmJson_delNode(UINT32 tractionId, INT8 *pucPath);

extern BOOL8 DmJson_checkRight(DM_HANDLER_T *pDmHandler, INT32 lEntityID, INT32 lId, INT32 lRightType);

/**
 * interface function (JSON utility)
 */
extern INT32 DmJson_printJson(DmJsonHandle jsonObject, INT8 *pucJsonFile, BOOL8 isFormat);

extern DmJsonHandle DmJson_createReferObject(INT8* pNodeName,  DM_JSON_REFER_OBJECT_TYPE type);
extern void         DmJson_destroyJsonObject(DmJsonHandle* jsonObject);

extern DmJsonHandle DmJson_createJsonString(const INT8* pvalue);
extern DmJsonHandle DmJson_createJsonNumber(double value);
extern DmJsonHandle DmJson_createJsonBOOL(BOOL8 value);
extern DmJsonHandle DmJson_createJsonNULL(void);
extern void         DmJson_destroyBasicItem(DmJsonHandle* jsonItem);

extern INT32 DmJson_AddItemToArray(DmJsonHandle pArrayJson, DmJsonHandle pItemJson);
extern INT32 DmJson_AddStringToArray(DmJsonHandle pArrayJson, const INT8* pString);
extern INT32 DmJson_AddNumberToArray(DmJsonHandle pArrayJson, double number);
extern INT32 DmJson_AddBoolToArray(DmJsonHandle pArrayJson, BOOL8 boolValue);
extern INT32 DmJson_AddNULLToArray(DmJsonHandle pArrayJson);

extern INT32 DmJson_AddItemToPOD(DmJsonHandle pPodJson, const INT8 *pName, DmJsonHandle pItemJson);
extern INT32 DmJson_AddStringToPod(DmJsonHandle pPodJson, const INT8 *pName, const INT8* pString);
extern INT32 DmJson_AddNumberToPod(DmJsonHandle pPodJson, const INT8 *pName, double number);
extern INT32 DmJson_AddBoolToPod(DmJsonHandle pPodJson, const INT8 *pName,BOOL8 boolValue);
extern INT32 DmJson_AddNULLToPod(DmJsonHandle pPodJson, const INT8 *pName);

extern INT32 DmJson_parseFile(INT8* pJsonFile, DmJsonHandle* pRootJson);

extern INT32 DmJson_IterPlainObject(DmJsonHandle rootJson, DM_JSON_POD_VISITOR* visitor, void* userdata);
extern INT32 DmJson_IterArray(DmJsonHandle rootJson, DM_JSON_ARRAY_VISITOR* visitor, void* userdata);

#ifdef __cplusplus
}
#endif

#endif /** _DM_JSON_H_ */
