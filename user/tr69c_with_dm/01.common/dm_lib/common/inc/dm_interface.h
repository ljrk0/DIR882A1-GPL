/*******************************************************************************
*
*   Copyright (c) 2009-2015 Broadcom Corporation
*   All rights reserved
*
*   FILE NAME     : dm_interface.h
*
*   PROGRAMMER(S) :  Zhonghai
*
*   DATE CREATED  :  24-Sep-2013
*
*   DESCRIPTION   :  
*
*   CAUTIONS      :  none
*
*******************************************************************************/

#ifndef _DM_INTERFACE_H_
#define _DM_INTERFACE_H_
#ifdef __cplusplus
    extern "C" {
#endif

#include "common_type_def.h"
#include "dm_selfdefined.h"
#include "osa.h"


#define DM_TRUE    OSA_TRUE
#define DM_FALSE   OSA_FALSE

#define DM_SUCCESS      OSA_SUCCESS
#define DM_FAILURE      OSA_FAILURE

#define DM_ACCESS_ENTITY_NAME_MAX 32

#define DM_SERVICE_ENTITY_NAME     "DmSrv"

/**
* the return value used for these APIs below
*
* DmTransactionStartReq
* DmTransactionStopReq
* DmGetParaCValueReq
* DmSetParaCValueReq
* DmAddObjectByIdReq
* DmDelObjectByIdReq
* DmGetParaValueReq
* DmSetParaValueReq
* DmGetParaNameReq
* DmAddObjectReq
* DmDeleteObjectReq
* DmSetParaAttributeReq
* DmGetParaAttributeReq
* 
*/
typedef enum
{
   DM_RET_SUCCESS              = 0,     /**<Success. */
   DM_RET_METHOD_NOT_SUPPORTED = 9000,  /**<Method not supported. */
   DM_RET_REQUEST_DENIED       = 9001,  /**< Request denied (no reason specified). */
   DM_RET_INTERNAL_ERROR       = 9002,  /**< Internal error. */
   DM_RET_INVALID_ARGUMENTS    = 9003,  /**< Invalid arguments. */
   DM_RET_RESOURCE_EXCEEDED    = 9004,  /**< Resource exceeded.
                                        *  (when used in association with
                                        *  setParameterValues, this MUST not be
                                        *  used to indicate parameters in error)
                                        */
   DM_RET_INVALID_PARAM_NAME   = 9005,  /**< Invalid parameter name.
                                        *  (associated with set/getParameterValues,
                                        *  getParameterNames,set/getParameterAtrributes)
                                        */
   DM_RET_INVALID_PARAM_TYPE   = 9006,  /**< Invalid parameter type.
                                        *  (associated with set/getParameterValues)
                                        */
   DM_RET_INVALID_PARAM_VALUE  = 9007,  /**< Invalid parameter value.
                                        *  (associated with set/getParameterValues)
                                        */
   DM_RET_SET_NON_WRITABLE_PARAM = 9008,/**< Attempt to set a non-writable parameter.
                                        *  (associated with setParameterValues)
                                        */
   DM_RET_NOTIFICATION_REQ_REJECTED = 9009, /**< Notification request rejected.
                                            *  (associated with setParameterAttributes)
                                            */
   DM_RET_NO_OWNNER_PID             = 9010,  /**< Invalid caller process id. */

   DM_RET_INVALID_TRANSACTION       = 9011,  /**< Invalid transaction id. */

   DM_RET_FAILED_LAUNCH_DMSRV       = 9701,  /**< Failed to launch dmsrv process */

   DM_RET_TIMED_OUT = 9809,     /**<Operation timed out. */
   DM_RET_OBJECT_NOT_FOUND = 9840,     /** failed to find object */
   DM_RET_INVALID_FILENAME = 9850,  /**< filename was not given for download */
   DM_RET_INVALID_CONFIG_FILE = 9852,  /**< invalid config file was detected */
   DM_RET_FILENAME_EMPTY = 9857,

} DM_RET_E;

/**
 * DM Service state event id
 */
typedef enum
{
   DM_SRV_EVT_PROC_EXIT  =  0x10,      /**< exit with an error */
   DM_SRV_EVT_FATAL_ERR  =  0x11,      /**< fatal error */
   DM_SRV_EVT_SIGNAL_ERR =  0x12,      /**< signal error */
   DM_SRV_EVT_STOPED_ERR =  0x13,      /**< stopped error */
} DM_SRV_STATE_EVENT_E;

/**
 * DM_SRV_STATE_NOTIFY_T struct
 */
typedef struct {
    UINT32  event;             /**< event id, @see DM_SRV_STATE_EVENT_E */
	union {
		INT32 exitCode;        /**< used by  DM_SRV_EVT_EXIT_ERR  */
		INT32 signalNo;        /**< used by  DM_SRV_EVT_SIGNAL_ERR,  DM_SRV_EVT_STOPED_ERR */
		INT32 error;           /**< used by  DM_SRV_EVT_FATAL_ERR */
	};
} DM_SRV_STATE_NOTIFY_T;

/**
 * DM_GET_VALUE_T struct
 */
typedef struct {
    UINT32    ulParaId;               /**< id of parameter */
    INT8     *pucPath;                /**< path of parameter */
    void     *pValue;                 /**< The memory pValue points to is allocated by DM, other modules like PM need only set its real value into the memory.\n
                                                                     if bCValue is DM_TRUE, pValue is a relevant C type pointer, and callback function should copy the C value to it.\n
                                                                     if bCValue is DM_FALSE, pValue points to a string or is a NULL pointer, there are two cases:\n
				                                            if "lParaId" is an ID of item, pValue should be a string which should be filled in with parameter's value by the function.\n
				                                            if "lParaId" is an ID of object, pValue is not used and will be set to NULL, but other modules like PM should retrieve the parameters' value under this object for preparing.\n
                                                              */
    UINT32    ValueSize;
	BOOL8     bCValue;                /**< DM_TRUE means pValue is a relevant C type pointer, and DM_FALSE means pValue is a string or NULL pointer */
	UINT32    ulCnt;                  /**< valid only when bCValue is DM_TRUE and the parameter is multiple object */
}DM_GET_VALUE_T;

/**
 * DM_SET_VALUE_T struct
 */
typedef struct {
    UINT32    ulParaId;               /**< id of parameter */
    INT8     *pucPath;                /**< path of parameter */
    void     *pValue;                 /**< The memory pValue points to is allocated by DM, other modules like PM can only get its real value from the memory.\n
                                                              */
    UINT32    ValueSize;
	BOOL8     bCValue;                /**< DM_TRUE means pValue is a relevant C type pointer, and DM_FALSE means pValue is a string or NULL pointer */
	UINT32    ulCnt;                  /**< valid only when bCValue is DM_TRUE and the parameter is multiple object */
}DM_SET_VALUE_T;

/**
 * The prototype of callback function
 * @param  pPara parameter info
 * @return 0: OK, others: failure
 */
typedef INT32 (*DM_GET_CALLBACK_FUN)(DM_GET_VALUE_T *pPara);


/**
 * The prototype of callback function
 * @param  pPara parameter info
 * @return 0: OK, others: failure
 */
typedef INT32 (*DM_SET_CALLBACK_FUN)(DM_SET_VALUE_T *pPara);


/**
 * DM_REG_GET_HANDLER_PARA_T struct
 */
typedef struct {
    INT8 entity[DM_ACCESS_ENTITY_NAME_MAX];/**< no need to fill, dm will recognize it automatically */
    INT32   lParaCnt;            /**< the count of parameter IDs */
    INT8  **paParaPath;          /**< the array of parameter full path */
    DM_GET_CALLBACK_FUN pCallbackFunc; /**< the function pointer to the callback function */
}DM_REG_GET_HANDLER_PARA_T;


/**
 * DM_REG_SET_HANDLER_PARA_T struct
 */
typedef struct {
    INT8 entity[DM_ACCESS_ENTITY_NAME_MAX];/**< no need to fill, dm will recognize it automatically */
    INT32   lParaCnt;            /**< the count of parameter IDs */
    INT8  **paParaPath;          /**< the array of parameter full path */
    DM_SET_CALLBACK_FUN pCallbackFunc; /**< the function pointer to the callback function */
}DM_REG_SET_HANDLER_PARA_T;


/**
 * DM_VALID_CHECK_T struct
 */
typedef struct {
    UINT32     ulParaId;               /**< id of parameter */
    INT8      *pucPath;                /**< path of parameter */
    INT8      *pValue;                 /**< the value with which parameter will be set to  */
}DM_VALID_CHECK_T;

/**
* check the validation of the specific parameter.
*/
typedef BOOL8 (*DM_PARAM_VALID_CHECK_CALLBACK_FUN)(DM_VALID_CHECK_T *pPara);

/**
* check the validation of the specific c-type.
*/
typedef BOOL8 (*DM_CTYPE_VALID_CHECK_CALLBACK_FUN)(INT8 *pValueRange, INT8 *pValue);


/**
 * DM_NOTIFY_PARA_T struct
 */
typedef struct {
    UINT32     ulParaId;                 /**< id of parameter */
	INT8    *pucPath;                 /**< path of parameter. For multiple object type parameter, it's the path of the object\n
	                                        instance which has been added, deleted or changed, not the whole multi-object. \n
	                                        For example, a.b.2. is deleted, here we give the path as "a.b.2.", not "a.b." */

#define DM_ITEM_CHANGED             0   /**< the change of item value will use this type */
#define DM_SINGLE_OBJECT_CHANGED    1   /**< the change of any member value of single object will use this type */
#define DM_MULTI_OBJECT_ADD         2   /**< the addition of multiple object instance will use this type */
#define DM_MULTI_OBJECT_DELETE      3   /**< the deletion of multiple object instance will use this type */
#define DM_MULTI_OBJECT_CHANGED     4   /**< the change of any member value of multiple object instance will use this type */
#define DM_ATTRIBUTE_CHANGED        5   /**< the change of attributes will use this type */

    UINT32   ulParaNotifyType;         /**< notification type for change of parameter value */

    INT8   *pucChangedAttrs;               /**< changed attribute list, comma-separated */
}DM_NOTIFY_PARA_T;


/**
 * DM_NOTIFY_T struct
 */
typedef struct {
	INT8 Modifier[DM_ACCESS_ENTITY_NAME_MAX];          /**< modifier : ROOT,TR069,GUI,SNMP */
    BOOL8 bAttrNotify;                /**< attribute changed */
    UINT32 ulParaCount;
    DM_NOTIFY_PARA_T *pCont;   /**< parameter changed info, note that pCont points to a continuous memory with size equals to sizeof(DM_NOTIFY_PARA_T)*ulParaCount  */
}DM_NOTIFY_T;

typedef void (*DM_NOTIFY_CALLBACK_FUN)(DM_NOTIFY_T *pPara);

typedef enum{
	DM_UPDATE_MODE_NULL,          /**< null */
	DM_UPDATE_MODE_ALL,           /**< apply all data from external files. */
	DM_UPDATE_MODE_UPGRADED,      /**< Generate new DB file. Then transfer those parameters with 'NeedTransfer' mark to new DB files when DataModel upgraded. */
	DM_UPDATE_MODE_CONFIGFILE,    /**< Get parameters from VendorConfig file and set them to current DB according to MappingTable.xml. */
	DM_UPDATE_MODE_DEFAULTVALUE,  /**< Get those parameters with 'UseOperatorDefault' mark from VendorDefault file and set them to current DB. This case should happen after factory reset. */
	DM_UPDATE_MODE_RESET,         /**< Generate new DB file. Then transfer those parameters without 'NeedReset' mark to new DB files. It should be DM part of factory reset. */
	DM_UPDATE_MODE_RAW       /**< Drop current DB file and load specific DB file */
}DM_UPDATE_MODE_t;

typedef enum{
	DM_EXPORT_MODE_NULL,          /**< null */
	DM_EXPORT_MODE_CONFIGFILE,    /**< generate VendorConfig file from DB data according to MappingTable.xml */
	DM_EXPORT_MODE_RAW,      /**< just copy the original DB file to dest path */
}DM_EXPORT_MODE_t;

#define DM_RW_MUTEX_LOCK_PATH_LEN 512

typedef struct{
    INT8 AccessEntity[DM_ACCESS_ENTITY_NAME_MAX];          /**< Access entity : ROOT,TR069,GUI,SNMP */
    INT8 RootNodeToLock[DM_RW_MUTEX_LOCK_PATH_LEN];           /**< The path of top node to access, the node name must end with a dot,such as "Device.Time." */
    UINT32 TransactionId;            /**< DM will generate this id and return it to access entity */
    INT32 DataFlush;				/**< -1: no db data flush to flash, 0: flush db data to flash if any params with transfer flag modified, 1: flush db data to flash always. */
    BOOL8 bNotCallNotify;			/**< Not trigger notify-callbacks when current transaction ends. */
    INT32 timeout;                  /**< the time to wait for the transaction. 0 means blocking always. */
}DM_TRANSACTION_REQ_T;

typedef struct _DM_SET_NODE_ATTR_REQ_T
{
	UINT32  TransactionId;       /**< transaction id */
	UINT32  ulParaId;            /**< Internal used, the id of parameter */
	INT8    *pucPath;            /**< path of parameter */
	INT8    *attrName;           /**< attribute name */
	INT8    *attrValue;          /**< attribute value */
} DM_SET_NODE_ATTR_REQ_T;

typedef struct parameter_value_t
{
    INT8   *pucName;       /**< Parameter name */
    INT8   *pucValue;      /**< Parameter value */
    UINT8   *pucType;       /**< Parameter type */
    BOOL8   bTr69Password;  /**< If true, tr69c will ignore this value and return emtpy string to ACS */
}PARAMETER_VALUE_T;

typedef struct parameter_value_list_t
{
    UINT32              udwCnt;             /**< Parameter value number */
    PARAMETER_VALUE_T   *pstParamValue;     /**< Parameter value info */
}PARAMETER_VALUE_LIST_T;

typedef struct{
    UINT32       TransactionId;               /**< transaction id */
    UINT32           ulParaId;            /**< the id of parameter, discarded */
	INT8          *pucPath;            /**< the path of parameter */
	UINT32          ulCnt;              /**< out-value, the count of C definition to which pValue points */
	void           *pValue;             /**< the pointer to the parameter's C definition, and its memory is allocated by module who calls this API */
    UINT32          ValueSize;          /**< the size of the buffer pValue points to */
    BOOL8           bShmWay;            /**< memory from db shared memory */
}DM_GET_PARA_C_VALUE_REQ_T;


typedef struct{
    UINT32       TransactionId;               /**< transaction id */
    UINT32           ulParaId;            /**< the id of parameter, discarded */
	INT8          *pucPath;            /**< the path of parameter */
	UINT32          ulCnt;              /**< the count of C definition to which pValue points */
	void           *pValue;             /**< the pointer to the parameter's C definition (let's call it 'ABC'), and it points to a continuous memory with size equals to sizeof(ABC)*ulCnt */
    UINT32          ValueSize;          /**< the size of the buffer pValue points to */
    BOOL8           bShmWay;            /**< memory from db shared memory */
}DM_SET_PARA_C_VALUE_REQ_T;


typedef struct{
    UINT32        TransactionId;               /**< transaction id */
    UINT32           ulObjId;             /**< the id of object, discarded */
    INT8          *pucPath;            /**< the path of object */
	void           *pObject;            /**< the pointer to the object, the instance number of object should be returned in this pointer */
    UINT32          objectSize;          /**< the size of the buffer pObject points to */
    BOOL8           bShmWay;            /**< memory from db shared memory */
}DM_ADD_OBJ_BY_ID_REQ_T;


typedef struct{
    UINT32       TransactionId;               /**< transaction id */
    UINT32           ulObjId;             /**< the id of object, discarded */
	INT8          *pucPath;            /**< the path of object, if it doesn't contain instance number, DM will delete all instances */
}DM_DEL_OBJ_BY_ID_REQ_T;

/**
 * PARAMETER_NAME_T struct
 */
typedef struct parameter_name_t
{
    INT8       *pucName;       /**< Parameter name */
    BOOL8        bWritable;     /**< whether the parameter is writable, only used for getParameterName */
}PARAMETER_NAME_T;

/**
 * PARAMETER_NAME_LIST_T struct
 */
typedef struct parameter_name_list_t
{
    UINT32              udwCnt;         /**< Parameter name number */
    PARAMETER_NAME_T    *pucParamName;  /**< Parameter name info */
}PARAMETER_NAME_LIST_T;

typedef void (*MEMORY_FREE)(void *p);

typedef struct dm_get_para_value_req_t
{
    UINT32                 TransactionId;               /**< transaction id */
    PARAMETER_NAME_LIST_T      *pNameList;      /**< Parameter name list */
    PARAMETER_VALUE_LIST_T     *pvList;         /**< Parameter value list, memory should be allocated by DM */
    MEMORY_FREE                 pfnFree;        /**< Memory free pointer, Module A should free the memory of pvList allocated by DM */
}DM_GET_PARA_VALUE_REQ_T;


typedef struct dm_set_para_value_req_t
{
    UINT32      TransactionId;               /**< transaction id */
    PARAMETER_VALUE_LIST_T      *pvList;    /**< Parameter value list, memory should be allocated by Module A */
	UINT16 *pFaultCodeList; 	            /**< DM will return fault code for each parameter, memory should be allocated by external module who calls DmSetParaValueReq */
	BOOL8	bCreateIfNotExit;				/**< Create multiobject instance if not existed, used for data transfer to new DB */
}DM_SET_PARA_VALUE_REQ_T;


typedef struct dm_get_para_name_req_t
{
    UINT32         TransactionId;               /**< transaction id */
	INT8                      *pucParamName;   /**< A string containing either a complete Parameter name, or a partial path name. An empty string indicates the top of the name hierarchy */
    BOOL8                       bNextLevel;     /**< Whether next level parameter should be returned */
    PARAMETER_NAME_LIST_T      *pNameList;      /**< The returned parameter name list, memory should be allocated by DM  */
    MEMORY_FREE                 pfnFree;        /**< Memory free pointer, Module A should free the memory of pNameList allocated by DM */
}DM_GET_PARA_NAME_REQ_T;


typedef struct dm_add_object_req_t
{
    UINT32 TransactionId;               /**< transaction id */
	INT8       *pucObjName;            /**< Object name */
    UINT32       udwInstanceNum;        /**< The instance number of the added object in DB file, this is a return value */
}DM_ADD_OBJECT_REQ_T;


typedef struct dm_delete_object_req_t
{
    UINT32    TransactionId;               /**< transaction id */
    INT8       *pucObjName;                /**< Object name */
}DM_DELETE_OBJECT_REQ_T;

typedef struct dm_attr_node_t
{
	INT8 *pAttrName;      /**< attribute name */
    BOOL8      bCType;                     /**< attribute value as c-type */
	void *pAttrValue;    /**< attribute value */
    UINT32 ulAttrValueCnt; /**< attribute value count */
    UINT32 ulAttrValueSize; /**< attribute value size */
}DM_ATTR_NODE_T;

typedef struct dm_param_attr_t
{
    INT8       *pucParamName;                /**< parameter name */
    BOOL8      bOnlyCurParam;                /**< apply attribute changes to only this parameter or all parameters under it, used for set-attribute func */
	UINT32     ulAttrCnt;                    /**< attribute list count */
	DM_ATTR_NODE_T *pucAttrList;             /**< attribute list */
}DM_PARAM_ATTR_T;

typedef struct dm_set_para_attr_req_t
{
    UINT32     TransactionId;                  /**< transaction id */
	UINT32      ulParamCnt;                /**< attribute list count */
	DM_PARAM_ATTR_T *pParamAttrList;       /**< attribute list */
}DM_SET_PARA_ATTR_REQ_T;

typedef struct dm_get_para_attr_req_t
{
    UINT32                TransactionId;               /**< transaction id */
    PARAMETER_NAME_LIST_T      *pNameList;                /**< parameter name list */
    BOOL8       bCValue;                    /**< c-type attribute-value */
	UINT32      ulParamCnt;                /**< attribute list count */
	DM_PARAM_ATTR_T *pParamAttrList;       /**< attribute list */
    MEMORY_FREE                 pfnFree;   /**< used to free 'pParamAttrList' */
}DM_GET_PARA_ATTR_REQ_T;

#define DM_TRACE_LEVEL_NONE      0
#define DM_TRACE_LEVEL_LOW       1
#define DM_TRACE_LEVEL_MIDDLE    2
#define DM_TRACE_LEVEL_HIGH      3
#define DM_TRACE_LEVEL_WARNING   4
#define DM_TRACE_LEVEL_ERROR     5

#define DM_ERR_MEMORY_ALLOCATION_FAILED  0x5001
#define DM_ERR_FILE_OPERATION_ERROR      0x5002
#define DM_ERR_THREAD_CREATING_FAILED    0x5003
#define DM_ERR_TIMER_CREATING_FAILED     0x5004
#define DM_ERR_PROCESSING_COMMON_ERROR   0x5005
#define DM_ERR_LOCAL_SOCKET_ERROR        0x5006
#define DM_ERR_INVALID_ARGUMENTS         0x5007
#define DM_ERR_MSGQ_ERROR                0x5008

typedef void (*DM_TRACE_HOOKER_FUN)(const INT8 *file, INT32 line, UINT16 traceLevel, UINT32 errType, INT8 *pTraceContext);
typedef void (*DM_SRV_STATE_MONITOR_CALLBACK)(DM_SRV_STATE_NOTIFY_T* pStateMonitor);
typedef INT32 (*DM_DBFILE_ENCRYPT_FUNC)(BOOL8 encrypted, INT8 *inFile, INT8 *outFile);

/**
 * DM_NOTIFY_HANDLER_REG_PARA_T struct
 */
typedef struct{
    INT8 entity[DM_ACCESS_ENTITY_NAME_MAX];
    INT32 lParaCnt;                 /**< the count of parameter IDs */
    INT8 **paParaPath;             /**< the array of parameter full path */
    DM_NOTIFY_CALLBACK_FUN pCallbackFunc; /**< the function pointer to the callback function */
}DM_NOTIFY_HANDLER_REG_PARA_T;

#define DM_PATH_STR_MAX_LENGTH 128

typedef struct _DM_START_REQ_T
{
    INT8 entity[DM_ACCESS_ENTITY_NAME_MAX];       /* make sure first four chars unique. but not "DmSrv" which as server name */
}DM_START_REQ_T;

typedef struct _DM_LOAD_DB_HANDLER_REQ_T
{
	INT8 dbName[DM_PATH_STR_MAX_LENGTH];       /* db name, if this path is not null, dm will load db libs automatically */
    INT8 dbLibPath[DM_PATH_STR_MAX_LENGTH];    /* if this path is not null */
    INT8 workingPath[DM_PATH_STR_MAX_LENGTH];  /* working path for db xml files, if null it will be tmp */
    INT8 loadPath[DM_PATH_STR_MAX_LENGTH];     /* where to load db xml files */
    INT8 backupPath[DM_PATH_STR_MAX_LENGTH];   /* where to back up db xml files */
	
	BOOL8 bDbReset;                            /**< factory reset needed */
    BOOL8 bXmlWithComment;                     /**< xml file with comment for each node */

	UINT32 createPid;                          /* internal : create process pid */
	BOOL8  isGlobalDb;                         /* internal : global db or not */
} DM_LOAD_DB_HANDLER_REQ_T;

typedef struct _DM_UNLOAD_DB_HANDLER_REQ_T
{
    BOOL8  bAsServer;                          /* unload db by c/s side */
    UINT32 callerPid;                          /* internal : client pid */
} DM_UNLOAD_DB_HANDLER_REQ_T;

typedef struct _DM_EXT_DB_PROP_T
{
	INT8* XmlStyleSheet;                      /* XmlStyleSheet, if not NULL it will be added to XmlDoc */
	INT8* Xmlns;                              /* Xml namespace */
	INT8* XmlnsXsi;                           /* XmlStyleSheet namespace */
	INT8* XsiSchemaLocation;                  /* XsiSchemaLocation of root Node, if not NULL it will be added to XmlDoc */
} DM_EXT_DB_PROP_T;

typedef struct _DM_EXT_DB_NEW_PARA_T
{
    INT8 *pParaToRefer;/* parameter as reference */

#define DM_EXT_DB_POSITION_AS_PREV_BROTHER 0/* add new node as prev-bother of first reference node */
#define DM_EXT_DB_POSITION_AS_NEXT_BROTHER 1/* add new node as next-bother of first reference node */
#define DM_EXT_DB_POSITION_AS_LAST_CHILD   2/* add new node as last-child of first reference node */
#define DM_EXT_DB_LAST_POSITION_AS_PREV_BROTHER 3/* add new node as prev-bother of last reference node*/
#define DM_EXT_DB_LAST_POSITION_AS_NEXT_BROTHER 4/* add new node as next-bother of last reference node*/
#define DM_EXT_DB_LAST_POSITION_AS_LAST_CHILD   5/* add new node as last-child of last reference node*/
    INT32 positionToAdd;/* where to add */

    INT8 *pParaName;
    INT8 *pParaValue;

    UINT32 ulAttrCnt;
    DM_ATTR_NODE_T *pAttrValue;
}DM_EXT_DB_NEW_PARA_T;

typedef struct _DM_EXT_DB_NEW_PARA_REQ_T
{
    UINT32                 TransactionId;
    UINT32                 ulParaCnt;
    DM_EXT_DB_NEW_PARA_T  *pNewParas;
}DM_EXT_DB_NEW_PARA_REQ_T;


typedef struct _DM_ADDON_CONTROL_REQ_T
{
#define DM_ADDON_CTRL_ACTION_NULL    0
#define DM_ADDON_CTRL_ACTION_LOAD    1
#define DM_ADDON_CTRL_ACTION_UNLOAD  2
    BOOL8  action;                             /* add-on control action */

    INT8 addonPath[DM_PATH_STR_MAX_LENGTH];    /* add-on lib path */
    INT8 addonName[DM_PATH_STR_MAX_LENGTH];    /* add-on lib name*/
} DM_ADDON_CONTROL_REQ_T;

typedef struct _DM_ADDON_REQ_MESSAGE_T
{
 	INT8*  addonName;        /* add-on name*/
	INT8*  reqMsg;           /* add-on request message */
	UINT32 reqMsgSize;       /* add-on request message size */
} DM_ADDON_REQ_MESSAGE_T;

/**
* initialize DM module. Initialize dm lib. 
*
* @param[in] pStartReq 1) running type, server or client. 
*                                    2) name, used for socket communication between server and client.
* @return DM_SUCCESS or DM_FAILURE.
*
*/
INT32 DmStartReq(DM_START_REQ_T *pStartReq);



/**
* Release relative resources. 
* Save xml files for server type.
*
* @param[in] bDbReset   if DM_TRUE, fallback to default values. 
* @return DM_SUCCESS or DM_FAILURE.
*
*/
INT32 DmStopReq();



/**
* update DB. User can specify the mode to update.
* not support yet.
*
* @param[in] pSrcFile the source file
* @param[in] updateMode how to update db files
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmUpdateDataModelReq(INT8 *pSrcFile, DM_UPDATE_MODE_t updateMode);



/**
* export DB data. User can specify how to export data.
* not support yet.
*
* @param[in] pDstFile the dst file
* @param[in] exportMode how to export db files
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmExportDataModelReq(INT8 *pDstFile, DM_EXPORT_MODE_t exportMode);



/**
* start one transaction. 
* DM will keep this transaction info and genenrate one transaction id to user.
*
* @param[in/out] pTransaction the transaction config info. transaction id will be returned to user.
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmTransactionStartReq(DM_TRANSACTION_REQ_T *pTransaction);



/**
* 1)release the transaction. 
* 2) save DB file to DDR and save it to Flash if required
* 3) trigger notify-callbacks if any
*
* @param[in] transactionId the id of the transaction to release.
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmTransactionStopReq(UINT32 transactionId);



/**
* Load db handler 
* Load db handler for global
*
* @param[in] pLoadDbHandlerReq DB load information
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmLoadGlobalDbHandlerReq(DM_LOAD_DB_HANDLER_REQ_T *pLoadDbHandlerReq);



/**
* Load db handler 
* Load db handler for local
*
* @param[in] pLoadDbHandlerReq DB load information
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmLoadLocalDbHandlerReq(DM_LOAD_DB_HANDLER_REQ_T *pLoadDbHandlerReq);



/**
* Set node attribute 
*
* @param[in] pSetAttrReq Attribute information
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmSetNodeAttrReq(DM_SET_NODE_ATTR_REQ_T *pSetAttrReq);



/**
* allocate shared memory for C structure access.
*
* @param[size] the size to allocate memory
* @param[addr] the address to allocate memory
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmAllocateMemReq(UINT32 size, void **addr);



/**
* release shared memory for C structure access.
*
* @param[addr] the address to free memory
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmReleaseMemReq(void **addr);



/**
* get parameter value by C structure.
*
* @param[in] pGetPara the struct to indicate which parameter to get.
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmGetParaCValueReq(DM_GET_PARA_C_VALUE_REQ_T *pGetPara);



/**
* set parameter value by C structure. 
*
* @param[in] pSetPara the struct to indicate which parameter to set.
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmSetParaCValueReq(DM_SET_PARA_C_VALUE_REQ_T *pSetPara);



/**
* Add one object and set initial value to each memeber in this new object. 
*
* @param[in] pAddObj the struct to indicate which object to add.
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmAddObjectByIdReq(DM_ADD_OBJ_BY_ID_REQ_T *pAddObj);



/**
* Delete object by parameter id and all objects belonging to this id will be deleted. 
*
* @param[in] pDelObj the struct to indicate which object to delete.
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmDelObjectByIdReq(DM_DEL_OBJ_BY_ID_REQ_T *pDelObj);



/**
* Get parameter values by "name" string
*
* @param[in] pGetParaValueReq the struct to indicate how to get parameters' values.
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmGetParaValueReq(DM_GET_PARA_VALUE_REQ_T    *pGetParaValueReq);



/**
* Set parameter values by "name" string
*
* @param[in] pSetParaValueReq the struct to indicate how to set parameters' values.
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmSetParaValueReq(DM_SET_PARA_VALUE_REQ_T    *pSetParaValueReq);



/**
* Get parameter names by "name" string
*
* @param[in] pGetParaNameReq the struct to indicate in which branch of db tree to get parameter names. 
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmGetParaNameReq(DM_GET_PARA_NAME_REQ_T *pGetParaNameReq);



/**
* Add object by "name" string
*
* @param[in] pAddObjectReq the struct to indicate how to add object. 
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmAddObjectReq(DM_ADD_OBJECT_REQ_T *pAddObjectReq);



/**
* Delete object by "name" string
*
* @param[in] pAddObjectReq the struct to indicate how to delete object. 
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmDeleteObjectReq(DM_DELETE_OBJECT_REQ_T *pDeleteObjectReq);



/**
* Set parameter attributes
* not support yet.
*
* @param[in] pSetParaAttrReq the struct to indicate how to set parameters' attributes. 
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmSetParaAttributeReq(DM_SET_PARA_ATTR_REQ_T *pSetParaAttrReq);



/**
* Get parameter attributes
* not support yet.
*
* @param[in] pGetParaAttrReq the struct to indicate how to get parameters' attributes. 
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmGetParaAttributeReq(DM_GET_PARA_ATTR_REQ_T *pGetParaAttrReq);



/**
* If no trace hooker registered, Dm traces will be disabled.
*
* @param[in] pHooker the callback to output dm traces. 
* @return null.
*
*/
void DmTraceHookerRegister(DM_TRACE_HOOKER_FUN pHooker);



/**
* In order to register notify callback dynamically
*
* @param[in] pPara the structure. 
* @return DM_TRUE or DM_FALSE.
*
*/
BOOL8 DmRegisterNotifyHandlerReq(DM_NOTIFY_HANDLER_REG_PARA_T *pPara);



/**
* In order to release notify callback dynamically
*
* @param[in] pPara the structure. 
* @return DM_TRUE or DM_FALSE.
*
*/
BOOL8 DmDeregisterNotifyHandlerReq(DM_NOTIFY_HANDLER_REG_PARA_T *pPara);



/**
* In order to register get callback dynamically
*
* @param[in] pPara the structure. 
* @return DM_TRUE or DM_FALSE.
*
*/
BOOL8 DmRegisterGetHandlerReq(DM_REG_GET_HANDLER_PARA_T *pPara);



/**
* In order to release get callback dynamically
*
* @param[in] pPara the structure. 
* @return DM_TRUE or DM_FALSE.
*
*/
BOOL8 DmDeregisterGetHandlerReq(DM_REG_GET_HANDLER_PARA_T *pPara);



/**
* In order to register set callback dynamically
*
* @param[in] pPara the structure. 
* @return DM_TRUE or DM_FALSE.
*
*/
BOOL8 DmRegisterSetHandlerReq(DM_REG_SET_HANDLER_PARA_T *pPara);



/**
* In order to release set callback dynamically
*
* @param[in] pPara the structure. 
* @return DM_TRUE or DM_FALSE.
*
*/
BOOL8 DmDeregisterSetHandlerReq(DM_REG_SET_HANDLER_PARA_T *pPara);



/**
* Flush DB files to Flash.
*
* @param[in] null 
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmFlushDBfiles(void);



/**
* Add parameter with attributes. only used for ext db.
* the added parameters may not be find by other APIs.
*
* @param[in] pExtDbName the extened db name. 
* @param[in] pNewParas parameters to add. 
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmExtDbNewParaReq(INT8 *pExtDbName, DM_EXT_DB_NEW_PARA_REQ_T *pNewParas);



/**
* Open extended Db
*
* @param[in] pExtDbName the extened db name. 
* @param[in] pXmlFile the xml file path. 
* @param[in] pExtDbProps properties used by xml file. 
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmExtDbOpen(INT8 *pExtDbName, INT8 *pXmlFile, DM_EXT_DB_PROP_T *pExtDbProps);



/**
* Close extended Db
*
* @param[in] pExtDbName the extened db name. 
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmExtDbClose(INT8 *pExtDbName, INT8 *pXmlFile, BOOL8 bDocReleased);

/**
* Add-on control (load/unload)
*
* @param[in] pAddOnCtrl Add-on control message
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmAddOnControlReq(DM_ADDON_CONTROL_REQ_T* pAddOnCtrl);


/**
* route request message to Add-on
*
* @param[in] pAddOnReqMsg Add-on request message
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmAddOnRouteMsgReq(DM_ADDON_REQ_MESSAGE_T* pAddOnReqMsg);


/**
* Check if pXmlFile is well-formatted xml file.
* Only for Dm server mode
*
* @param[in] pXmlFile the xml file path. 
* @return the result (DM_TRUE or DM_FALSE).
*
*/
BOOL8 DmCheckXmlFile(INT8 *pXmlFile);



/**
* If callback registered, it start a thread to receive state message.
*
* @param[in] pCallback the callback to output dm srv state. 
* @return the result (refer to DM_RET_E enum).
*
*/
INT32 DmSrvStateMonitorRegister(DM_SRV_STATE_MONITOR_CALLBACK pCallback);



#ifdef __cplusplus
}
#endif

#endif /** _DM_INTERFACE_H_ */
