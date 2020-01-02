/*******************************************************************************
 *
 *   Copyright (c) 2009-2016 Broadcom Corporation
 *   All rights reserved
 *
 *   FILE NAME     : dmJson_Addon.h
 *   PROGRAMMER(S) :
 *   DATE CREATED  : 11-APR-2016
 *   DESCRIPTION   :
 *   CAUTIONS      : none
 *
 *******************************************************************************/

#ifndef _DM_JSON_ADDON_H_
#define _DM_JSON_ADDON_H_
#ifdef __cplusplus
    extern "C" {
#endif

#include "common_type_def.h"
#include "osa.h"

#define DM_JSON_MAX_NODE_PATH   256
#define DM_JSON_MAX_FILE_NAME   32

/**
 * type definition
 */
typedef INT32 (*DB_JSON_MESSAGE_HANDLER)(INT8* pReqMsg, UINT32 reqMsgSize);

/**
 * struct & enum
 */
typedef enum {
	DM_JSON_REQ_MSG_INVLAID    = 0x00,
	DM_JSON_REQ_MSG_GET_OBJECT,
	DM_JSON_REQ_MSG_UPD_OBJECT,
} DM_JSON_REQ_MSG_ID;

typedef struct _DM_JSON_REQ_MSG_T{
	INT32  reqId;
} DM_JSON_REQ_MSG_T;

typedef struct {
	DM_JSON_REQ_MSG_T msg;

	INT8  accessEntity[DM_ACCESS_ENTITY_NAME_MAX];
	INT8  pucPath[DM_JSON_MAX_NODE_PATH];
	INT8  pucJsonFile[DM_JSON_MAX_FILE_NAME];
	BOOL8 isFormat;
	INT32 depth;
} DM_JSON_GET_OBJECT_T;

typedef struct {
	DM_JSON_REQ_MSG_T msg;

	INT8  accessEntity[DM_ACCESS_ENTITY_NAME_MAX];
	INT8  pucJsonFile[DM_JSON_MAX_FILE_NAME];
	BOOL8 isFormatResult;
} DM_JSON_UPD_OBJECT_T;

#ifdef __cplusplus
}
#endif

#endif /** _DM_JSON_ADDON_H_ */
