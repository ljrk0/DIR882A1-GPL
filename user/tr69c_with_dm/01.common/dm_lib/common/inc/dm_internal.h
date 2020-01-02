
#ifndef _DM_EXT_H_
#define _DM_EXT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dm_interface.h"

#include <ctype.h>
#include <arpa/inet.h>
#include <sys/stat.h>


#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/xmlschemas.h>
#include <libxml/xmlversion.h>


#define MAX_MULTI_OBJ_LEVEL 3
#define MAX_OBJ_NAME_LENGTH 256
#define DM_HASH_INIT_SIZE  53

/*this size is taken from tr106 ISPLogo, if there are some size more than it,
* please change it to the big one
*/
#define MAX_VALUE_SIZE 32769
#define MAX_INSTANCE_NUMBER_SIZE 13
#define MAX_ATTR_NUMBER 10

#define RIGHT_R 0x01
#define RIGHT_W 0x02


#define DM_MALLOC(n)               		OSA_MEM_ALLOC(n)
#define DM_FREE(pMem)                   OSA_MEM_FREE(pMem)

#define DM_DECRYPT_TMP_FILE              "/tmp/tmpDecrypt.xml"

#define DM_MSG_CODE_NULL                       0x00000000
#define DM_MSG_CODE_UpdateDataModelReq         0x00000001
#define DM_MSG_CODE_UpdateDataModelCnf         0x00000002
#define DM_MSG_CODE_ExportDataModelReq         0x00000003
#define DM_MSG_CODE_ExportDataModelCnf         0x00000004
#define DM_MSG_CODE_TransactionStartReq        0x00000005
#define DM_MSG_CODE_TransactionStartCnf        0x00000006
#define DM_MSG_CODE_TransactionStopReq         0x00000007
#define DM_MSG_CODE_TransactionStopCnf         0x00000008
#define DM_MSG_CODE_GetParaCValueReq           0x00000009
#define DM_MSG_CODE_GetParaCValueCnf           0x0000000A
#define DM_MSG_CODE_SetParaCValueReq           0x0000000B
#define DM_MSG_CODE_SetParaCValueCnf           0x0000000C
#define DM_MSG_CODE_AddObjectByIdReq           0x0000000D
#define DM_MSG_CODE_AddObjectByIdCnf           0x0000000E
#define DM_MSG_CODE_DelObjectByIdReq           0x0000000F
#define DM_MSG_CODE_DelObjectByIdCnf           0x00000010
#define DM_MSG_CODE_GetParaValueReq            0x00000011
#define DM_MSG_CODE_GetParaValueCnf            0x00000012
#define DM_MSG_CODE_SetParaValueReq            0x00000013
#define DM_MSG_CODE_SetParaValueCnf            0x00000014
#define DM_MSG_CODE_GetParaNameReq             0x00000015
#define DM_MSG_CODE_GetParaNameCnf             0x00000016
#define DM_MSG_CODE_AddObjectReq               0x00000017
#define DM_MSG_CODE_AddObjectCnf               0x00000018
#define DM_MSG_CODE_DeleteObjectReq            0x00000019
#define DM_MSG_CODE_DeleteObjectCnf            0x0000001A
#define DM_MSG_CODE_SetParaAttributeReq        0x0000001B
#define DM_MSG_CODE_SetParaAttributeCnf        0x0000001C
#define DM_MSG_CODE_GetParaAttributeReq        0x0000001D
#define DM_MSG_CODE_GetParaAttributeCnf        0x0000001E
#define DM_MSG_CODE_RegisterNotifyHandlerReq   0x0000001F
#define DM_MSG_CODE_RegisterNotifyHandlerCnf   0x00000020
#define DM_MSG_CODE_DeregisterNotifyHandlerReq 0x00000021
#define DM_MSG_CODE_DeregisterNotifyHandlerCnf 0x00000022
#define DM_MSG_CODE_RegisterGetHandlerReq      0x00000023
#define DM_MSG_CODE_RegisterGetHandlerCnf      0x00000024
#define DM_MSG_CODE_DeregisterGetHandlerReq    0x00000025
#define DM_MSG_CODE_DeregisterGetHandlerCnf    0x00000026
#define DM_MSG_CODE_FlushDBfilesReq            0x00000027
#define DM_MSG_CODE_FlushDBfilesCnf            0x00000028
#define DM_MSG_CODE_ExtDbOpenReq               0x00000029
#define DM_MSG_CODE_ExtDbOpenCnf               0x0000002A
#define DM_MSG_CODE_ExtDbCloseReq              0x0000002B
#define DM_MSG_CODE_ExtDbCloseCnf              0x0000002C
#define DM_MSG_CODE_GetHandlerReq              0x0000002D
#define DM_MSG_CODE_GetHandlerCnf              0x0000002E
#define DM_MSG_CODE_NotifyHandlerReq           0x0000002F
#define DM_MSG_CODE_NotifyHandlerCnf           0x00000030
#define DM_MSG_CODE_LeaveInd                   0x00000031
#define DM_MSG_CODE_LoadDbHandlerReq           0x00000034
#define DM_MSG_CODE_LoadDbHandlerCnf           0x00000035
#define DM_MSG_CODE_UnloadDbHandlerReq         0x00000036
#define DM_MSG_CODE_UnloadDbHandlerCnf         0x00000037
#define DM_MSG_CODE_ExtDbNewParaReq            0x0000003A
#define DM_MSG_CODE_ExtDbNewParaCnf            0x0000003B
#define DM_MSG_CODE_AllocateMemoryReq          0x0000003F
#define DM_MSG_CODE_AllocateMemoryCnf          0x00000040
#define DM_MSG_CODE_ReleaseMemoryReq           0x00000041
#define DM_MSG_CODE_ReleaseMemoryCnf           0x00000042
#define DM_MSG_CODE_RegisterSetHandlerReq      0x00000043
#define DM_MSG_CODE_RegisterSetHandlerCnf      0x00000044
#define DM_MSG_CODE_DeregisterSetHandlerReq    0x00000045
#define DM_MSG_CODE_DeregisterSetHandlerCnf    0x00000046
#define DM_MSG_CODE_SetHandlerReq              0x00000047
#define DM_MSG_CODE_SetHandlerCnf              0x00000048
#define DM_MSG_CODE_AddOnControlReq            0x00000049
#define DM_MSG_CODE_AddOnControlCnf            0x00000050
#define DM_MSG_CODE_AddOnReqMsgReq             0x00000051
#define DM_MSG_CODE_AddOnReqMsgCnf             0x00000052

#define DM_SOCK_SERVER_CHECK_MAX_COUNT  8000000
#define DM_SOCK_SERVER_CHECK_INTERVAL   1000

#define DM_SOCK_SERVER_NAME  "DmSrv"

#define DM_SOCK_MSG_TIMEOUT_MS  60000
#define DM_SHM_MSG_TIMEOUT_MS   60000

#define DM_SHM_DEFAULT_MAX_SIZE   (5*1024*1024)

#define DM_SOCK_MAX_C_VALUE_SIZE  (2*1024)

#define DM_THREAD_POOL_MAX_NUMBER  5

typedef struct _DM_SOCK_MSG_T DM_SOCK_MSG_T;

/** the callback to handle received messages */
typedef void (*DM_SOCK_MSG_RCV_FUN)(DM_SOCK_MSG_T *pSockMsg, void *pMsg);

/** the client structure of sock message */
typedef struct _DM_SOCK_CLIENT_T{
    INT32 client_sock;
    UINT32 client_id;
}DM_SOCK_CLIENT_T;

/** the structure of sock message */
struct _DM_SOCK_MSG_T{
    INT32 sock;

#define DM_SOCK_PATH_LEN 16
    INT8 sock_path[DM_SOCK_PATH_LEN];

    OSA_THREAD_MUTEX_T sndMutex;

    OSA_THREAD_T rcvThread;/**< receive messages */

#define DM_SOCK_RCV_STATUS_STOPPED 0
#define DM_SOCK_RCV_STATUS_STOPPING 1
#define DM_SOCK_RCV_STATUS_RUNNING 2
    UINT8 rcvStatus;
    DM_SOCK_MSG_RCV_FUN pRcvFunc;

    UINT8 asServer;
    OSA_THREAD_T acptThread;/**< accept connections from clients */

#define DM_SOCK_ACPT_STATUS_STOPPED 0
#define DM_SOCK_ACPT_STATUS_STOPPING 1
#define DM_SOCK_ACPT_STATUS_RUNNING 2
    UINT8 acptStatus;

#define DM_MAX_SOCK_CLIENT 10
    DM_SOCK_CLIENT_T clients[DM_MAX_SOCK_CLIENT];
    OSA_THREAD_MUTEX_T acptMutex;
};

#define DM_SOCK_MSG_OPTION__NULL             0x0
#define DM_SOCK_MSG_OPTION__DATA_IN_SHM      0x00000001


/** the sock message header definition */
typedef struct _DM_SOCK_MSG_HEADER_T{
    UINT32 msg_id;/**< the id of identify the message */
    UINT32 sender_id;/** the id to identify client or server */
    UINT32 receiver_id;/** the id to identify client or server */
    INT32  word_data;/** different msg has different usage.  */
    INT32  caller_id;/** use to match the caller when reponse sockmsg received */
    UINT32 options;/** keep extra info */
    UINT32 msgLength;/** the bytes of the message body */
}DM_SOCK_MSG_HEADER_T;

typedef struct _DM_SOCK_MSG_PTR_T {
    DM_SOCK_MSG_HEADER_T *pHdr;
    void *pBody;
}DM_SOCK_MSG_PTR_T;




typedef BOOL8 (*DM_PATTERN_REGISTER_HANDLER_FUN)(void);
typedef BOOL8 (*DM_OPERATION_REGISTER_HANDLER_FUN)(void);
typedef BOOL8 (*DM_GET_REGISTER_HANDLER_FUN)(void);
typedef BOOL8 (*DM_NOTIFY_REGISTER_HANDLER_FUN)(void);
typedef BOOL8 (*DB_DATA_VALID_CHECK_FUN)(UINT8 ucParaCType, INT8 *pValueRange, INT8 *pValue);
typedef INT8* (*DB_GET_C_TYPE_STR_FUN)(UINT8 ucParaCType);
typedef INT8* (*DB_GET_DATA_TYPE_STR_FUN)(UINT8 ucParaType);
typedef INT32 (*DB_LOAD_CONFIG_FUN)(void *pDlHandler, BOOL8 bDbReset, INT8 *pWorkingPath, INT8 *pLoadPath, INT8 *pBackupPath);
typedef BOOL8 (*DB_IS_OBJECT_FUN)(UINT8 ucParaType);
typedef INT32 (*DB_ADDON_MESSAGE_HANDLER)(INT8* pReqMsg, UINT32 reqMsgSize);

/**
    The structure of access-node to DB
*/
typedef struct _DB_ACCESS_NODE_T {
    INT8 *AccessUserName; /**< Access Name of user */
    INT32 AccessUserId; /**< Access Id of user */
}DB_ACCESS_NODE_T;

/**
    The structure of DB config info
*/
typedef struct _DB_CONFIG_T {
    INT8 *pDbName; /**< Data Base Root Name */
    INT8 *pDbVersion; /**< Data Model Version */
    BOOL8 bExtDb; /**< If it is Extended Data Base */
    INT8 *pFilePath; /**< Location where this DB file works */
    INT8 *pLoadPath; /**< Location where this DB file loaded */
    INT8 *pBackupPath; /**< Location where this DB file saved */
    INT8 *pMappingXmlPath; /**< Location where mappingtable.xml for this DB when vendor config application */
    DM_PATTERN_REGISTER_HANDLER_FUN pDmPatternRegisterHandler; /**< pattern table register function */
    DB_DATA_VALID_CHECK_FUN pDbDataValidChecker; /**< check data-validation */
    DB_GET_C_TYPE_STR_FUN pDbGetCTypeStr; /**< Get C-type string */
    DB_GET_DATA_TYPE_STR_FUN pDbGetDataTypeStr; /**< Get data type string */
    DB_IS_OBJECT_FUN pDbIsObjectFunc; /**< the fucntion to check if it is object */
    DB_ACCESS_NODE_T *pAccessList; /**< the user list */
    void *pDlHandler; /**< libdl.so handler */
}DB_CONFIG_T;

#define DB_VERSION_PARAM_NAME "VendorDataModelVersion"
#define DB_VERSION_PARAM_VALUE_LENGTH 33


typedef BOOL8(*HANDLER_FUNC)(void *p);

typedef struct
{
    HANDLER_FUNC   pGetBefHandler;
    HANDLER_FUNC   pGetCurHandler;
    HANDLER_FUNC   pGetAftHandler;
    HANDLER_FUNC   pSetBefHandler;
    HANDLER_FUNC   pSetCurHandler;
    HANDLER_FUNC   pSetAftHandler;
    HANDLER_FUNC   pAddBefHandler;
    HANDLER_FUNC   pAddCurHandler;
    HANDLER_FUNC   pAddAftHandler;
    HANDLER_FUNC   pDelBefHandler;
    HANDLER_FUNC   pDelCurHandler;
    HANDLER_FUNC   pDelAftHandler;

    HANDLER_FUNC   pGetAttrBefHandler;
    HANDLER_FUNC   pGetAttrCurHandler;
    HANDLER_FUNC   pGetAttrAftHandler;
    HANDLER_FUNC   pSetAttrBefHandler;
    HANDLER_FUNC   pSetAttrCurHandler;
    HANDLER_FUNC   pSetAttrAftHandler;
}DM_NODE_HANDLER_T;

typedef struct _DM_NOTIFY_CB_NODE_T
{
    DM_NOTIFY_CALLBACK_FUN pCb;
    struct _DM_NOTIFY_CB_NODE_T   *pNext;
}DM_NOTIFY_CB_NODE_T;

typedef struct dm_node_notify_handler_t
{
    INT8 entity[DM_ACCESS_ENTITY_NAME_MAX];    /**< notify entity */
    struct dm_node_notify_handler_t   *pNext;
}DM_NODE_NOTIFY_HANDLER_T;


typedef struct dm_node_get_handler_t
{
    INT8 entity[DM_ACCESS_ENTITY_NAME_MAX];    /**< get entity */
}DM_NODE_GET_HANDLER_T;

typedef struct dm_node_set_handler_t
{
    INT8 entity[DM_ACCESS_ENTITY_NAME_MAX];    /**< set entity */
}DM_NODE_SET_HANDLER_T;

/** hash node definition */
typedef struct _DM_HASH_NODE_T{
  INT8 *key;
  void *value;
  struct _DM_HASH_NODE_T *next;
}DM_HASH_NODE_T;

/** hash table definition */
typedef struct _DM_HASH_T{
    INT32 usedSize;

#define DM_HASH_DEFAULT_MAX_SIZE  53
    INT32 maxSize;
    DM_HASH_NODE_T **hashTable;
    OSA_THREAD_MUTEX_T mutex;
}DM_HASH_T;

typedef struct _DM_HASH_ENUM_T{
    DM_HASH_T *pHash;
	
    /** all below for enum traversing  */
    INT32           lNextEnumIndex;
    DM_HASH_NODE_T  *pNextEnumNode;	
}DM_HASH_ENUM_T;


typedef struct {
#define DM_MSG_SIZE 8
#define DM_MSGQ_LENGTH 20
        OSA_MSGQ_T msgQ;/* handle msgs from sock_msgs */
    
#define DM_THRAD_STACK_SIZE 0xf000
        OSA_THREAD_T threadId;/* handle msgs in single thread */
}DM_TRANS_CTRL_T;

/* dm sock msg list */
typedef struct _DM_SOCK_MSG_NODE_T
{
    DM_SOCK_MSG_HEADER_T *pMsgRcv;
    struct _DM_SOCK_MSG_NODE_T *pNext;
}DM_SOCK_MSG_NODE_T;

typedef struct _DM_MEM_REQ_NODE_T
{
    INT8 fileName[32];
    OSA_SHM_T shm;
}DM_MEM_REQ_NODE_T;

typedef struct _DM_ADDON_NODE_T
{
    INT8 addonName[DM_PATH_STR_MAX_LENGTH];
    void *pDlHandler;	
	DB_ADDON_MESSAGE_HANDLER msgHandler;

	struct _DM_ADDON_NODE_T *next;
}DM_ADDON_NODE_T;

/* dm control struct definition */
typedef struct _DM_CTRL_T {
    BOOL8 bAsServer;
    INT8 entity[DM_ACCESS_ENTITY_NAME_MAX];
    DM_SOCK_MSG_T sockMsgQ;/* send&recv msgs between client & server */

    DM_HASH_T transHash;/* to save the hash */

    OSA_THREAD_POOL_T threadPool;/* userd for transactions */
    OSA_RESOURCE_GRP_T rsrcGrp;/* When response received, wake up the requester */
    DM_SOCK_MSG_NODE_T *pMsgRcvList;/* keep the msg recv list temporarily */
    DM_HASH_T notifyFuncHash;/* keep notify-callback-func */
    DM_HASH_T getFuncHash;/* keep get-callback-func */
    DM_HASH_T setFuncHash;/* keep set-callback-func */
    UINT32 transIdMax;/* the max trans-id */
    OSA_THREAD_MUTEX_T transIdMutex;/* make sure unique trans-id */
	
	DM_DBFILE_ENCRYPT_FUNC encryptFunc; /* used to encrypt/decrypt db files */

    UINT32 curShmSize; /* current shm size */
    UINT32 maxShmSize; /* max shm size */
	DM_HASH_T shmInUseHash;/* shared memory hash table */
    OSA_RESOURCE_GRP_T shmGroup;/* make sure shm list correct */
    UINT32 shm_counter; /* shm counter */

	DM_ADDON_NODE_T* pAddOnList;    /* add-on list */
	OSA_THREAD_MUTEX_T addOnMutex;  /* make sure pAddOnList */

    DM_TRANS_CTRL_T nonTransCtrl; /* to handle non-transaction requests */

    DM_NOTIFY_CALLBACK_FUN pAttrNotifyCb; /*one attr-notify-cb for one process temperarily */
}DM_CTRL_T;


#define DM_BOOT_ACTION_MULTI_OBJ__DEL_ALL   "multi-obj:del-all"
#define DM_BOOT_ACTION_BOOLEAN__SET_FALSE   "boolean:set-false"
#define DM_BOOT_ACTION_BOOLEAN__SET_TRUE    "boolean:set-true"
#define DM_BOOT_ACTION_INTEGER__SET_MIN     "integer:set-min"
#define DM_BOOT_ACTION_INTEGER__SET_ZERO    "integer:set-zero"
#define DM_BOOT_ACTION_INTEGER__SET_MAX     "integer:set-max"
#define DM_BOOT_ACTION_STRING__SET_EMPTY    "string:set-empty"
#define DM_BOOT_ACTION_STRING__SET_ENUM_MIN "string:set-enum-min"
#define DM_BOOT_ACTION_STRING__SET_ENUM_MAX "string:set-enum-max"
#define DM_BOOT_ACTION_DATETIME__UNKNOWN    "datetime:unknown"

#define DM_STR__BOOLEAN_FALSE      "0"
#define DM_STR__BOOLEAN_TRUE       "1"
#define DM_STR__INTEGER_ZERO       "0"
#define DM_STR__DATETIME_UNKNOWN   "0001-01-01T00:00:00Z"
#define DM_STR__STRING_EMPTY       ""


typedef enum _DM_DATA_TRANSFER_MODE_E
{
    DM_DATA_TRANSFER_MODE__RESET = 0,
    DM_DATA_TRANSFER_MODE__UPGRADE = 1
}DM_DATA_TRANSFER_MODE_E;


typedef BOOL8 (*DM_CONV_STRUCT_STRING_FUNC)(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
typedef BOOL8 (*DM_CONV_STRING_STRUCT_FUNC)(INT32 plStringCnt,  void *pucStringValue, UINT32 *plStructCnt, void *pStructValue, INT8 *pcValueRange);

typedef struct
{
    INT8          *pucNodeName;

    INT8          *pucNodeRealName;          /*real name in xml*/

    INT8          *pucDescription;

    UINT8           ucParaType;             /* data type */

	UINT8           ucParaCType;             /*ctype*/

    UINT32          ulSizeOffset;            /*size offset in parent structure*/
    UINT32          ulCTypeSize;             /*ctype-size*/
    UINT32          ulMaxElement;            /*max-element*/
    UINT32          ulElementCntOffset;            /*element cnt offset in parent structure*/

    UINT32          ulParentIdx;

    UINT32          ulNextBrotherIdx;

    UINT32          ulFirstChildIdx;          /*first child index*/
    UINT32          ulMaxChildren;            /*max children*/

    UINT32          ulRead;                  /* read right, each module with a bit, 0 means can't read, 1 means can read */
    UINT32          ulWrite;                 /* write right, each module with a bit, 0 means can't write, 1 mean can write */

    INT8           *pValueRange;
    INT8           *pDefaultValue;
    INT8           *pSupportedXmlAttributes;
    INT8           *pXmlAttributeDefaultValue;
    INT8           *pXmlAttributeNotificationEntities;

    DM_NODE_HANDLER_T        *pNodeHandler;
    DM_NODE_NOTIFY_HANDLER_T *pNotifyHandler;
    DM_NODE_GET_HANDLER_T    *pGetHandler;
    DM_NODE_SET_HANDLER_T    *pSetHandler;

    BOOL8           bAutoSaved; /* flush data to flash when modified */
    BOOL8           bUpgradeTransfer; /* need transfer when upgrade */
    BOOL8           bResetTransfer;    /* need transfer when factory reset */
    INT8           *pBootAction;    /* the boot action to take when db initialization */
    DM_PARAM_VALID_CHECK_CALLBACK_FUN pValidityCheckHandler;

    BOOL8           bUseOperatorDefault;/* apply operator default values when factory reset */
    BOOL8           bUseHardwareDefault;/* apply hardware default values when factory reset */
    BOOL8           bForceActiveNotification;/* force active notification to ACS */
    BOOL8           bTr69Password;/* tell tr69c to ignore this value and return emtpy string to ACS */

    DM_CONV_STRUCT_STRING_FUNC pToStringFunc;
	DM_CONV_STRING_STRUCT_FUNC pToStructFunc;
}DM_NODE_INFO_T;

typedef struct
{
    INT8           *pucAttrName;             /*attr name*/
    INT8           *pucDescription;          /*description*/
    BOOL8           bCreatedByDefault;       /*created by default */
    UINT8           ucParaCType;             /*ctype*/
    UINT32          ulCTypeSize;             /*ctype-size*/
    INT8           *pValueRange;             /*ValueRange*/
    INT8           *pDefaultValue;           /*DefaultValue*/
    DM_CTYPE_VALID_CHECK_CALLBACK_FUN pVaildCheckFunc;  /*vaild checker*/
    DM_CONV_STRUCT_STRING_FUNC pToStringFunc;
	DM_CONV_STRING_STRUCT_FUNC pToStructFunc;
}DM_ATTR_INFO_T;

typedef struct dm_notify_id_list_t
{
    INT32 lId;
	DM_NOTIFY_PARA_T notifyPara;
	struct dm_notify_id_list_t *pNext;
	struct dm_notify_id_list_t *pPrev;
}DM_NOTIFY_ID_LIST_T;



typedef struct dm_rw_mutex_t{
    INT8 lockPath[DM_RW_MUTEX_LOCK_PATH_LEN];
    INT8 AccessEntity[DM_ACCESS_ENTITY_NAME_MAX];
}DM_RW_MUTEX_T;

typedef struct dm_handler_t{
	DM_NODE_INFO_T   *pPattern;        /*pointer to pattern*/
    DM_ATTR_INFO_T   *pAttrPattern;    /*pointer to attribute pattern*/
	UINT32           ownerPid;         /*owner process pid for local db, if global db it's zero */
	UINT32           createPid;        /*create process pid */
    BOOL8             bXmlWithComment; /* xml with comment for each node */
    INT32             lPatternLength;  /*number of pattern*/
    INT8            *pucFilePath;     /*the database file path*/
    UINT8            *pucXsltPath;     /*the xslt file path*/
    UINT8            *pucNodeName;     /*the root node name*/
	INT8            *pucDbName;       /* the database name*/
    INT8            *pLoadPath;       /* Location where this DB file loaded */
    INT8            *pBackupPath;      /* Location where this DB file saved */
    INT8            *pMappingXmlPath; /* Location where mappingtable.xml for this DB when vendor config application */
	UINT8            *pucSoFilePath;   /* so file path */
	void             *pSoHandle;       /* so handle */
    INT32             lDbId;           /*the database id*/
    UINT8            *pucBackFilePath; /*the backup file database*/
	BOOL8             bNeedBak;		   /* whether need backup db file */
    xmlDocPtr         pXmlDocument;    /*the document of xml*/
	xmlXPathContextPtr xpathCtx;
	xmlXPathObjectPtr xpathParentObj;
#define DM_RW_RESOURCE_MAX 10
    DM_RW_MUTEX_T   resourceRW[DM_RW_RESOURCE_MAX];          /*mutex for read/write. only five users at the same time */
    OSA_RESOURCE_GRP_T   resourceGrp4RW;        /* resource control for mutexRW structure */
    OSA_THREAD_MUTEX_T   mutexXml;        /*mutex for xml operation*/
    OSA_THREAD_MUTEX_T   mutexNotifyList; /*mutex for notifyList*/
    DM_HASH_T            hashNameID;      /*hash for name to id*/

	DM_NOTIFY_ID_LIST_T *pNotifyIdList;/* the id list of to be notified */
    DM_NOTIFY_ID_LIST_T *pAttrNotifyIdList;/* the id list of to be notified */
    DM_HASH_T            hashTransactions; /* hash to hold transactions */

    BOOL8                bExtDb; /* If it is Extended Data Base */
    DB_ACCESS_NODE_T    *pAccessList;/* the user list */

    DB_DATA_VALID_CHECK_FUN pDataVaildChecker;/* used for data-validation */
    DB_GET_C_TYPE_STR_FUN pCTypeStrHooker;/* used for c-type string */
    DB_GET_DATA_TYPE_STR_FUN pDataTypeStrHooker;/* used for data-type string */
    DB_IS_OBJECT_FUN pDbIsObjectFunc;

    void                   *pDlHandler; /* used to close libxxxDb.so */
}DM_HANDLER_T;

/**
 * DM_HANDLER_PARA_T struct
 */
typedef struct dm_handler_para_t{
    DM_HANDLER_T     *pDbHandler;   /**< the database handler */
    UINT32   TransactionId;     /**< entity identifier:eTr069,eWebGui,eSnmp,eOam */
    UINT32            ulParaId;      /**< the id of object or item */
	UINT32            ulCnt;         /**< the count of value array */
	BOOL8             bCInterface;   /**< the call from c interface or string interface */
    BOOL8             bAttribute;    /**< only attribute modification */
	BOOL8             bItem;         /**< call from item operation, if object false, item true */
    INT8             *pucName;      /**< the fullpath of object or item */
    void             *pValue;     /**< for get: the value will be set by GetCurHandler 
				                           for set: the value should be the value of item, or null for object
				                           for add: the value should be the value of item, or null for object and out as the instance number
				                           for del: the value should be the value of item, or null for object */
    UINT32            valueSize;
}DM_HANDLER_PARA_T;


/**
 * The prototype of generated get function
 * @param  pGetPara the  parameter
 * @return INT32
 * @note   
 *
 */
typedef INT32  (*DM_GET_OPERATION_FUN) (DM_GET_PARA_C_VALUE_REQ_T *pGetPara);

/**
 * DM_REG_GET_OPERATION_REQ_T struct
 */
typedef struct dm_reg_get_operation_req_t
{
    INT8               *pucPath;         /**< the path of  item*/
	UINT32                ulParaId;         /**< the id of item */
	DM_GET_OPERATION_FUN pFunc;           /**< the get function for item */
}DM_REG_GET_OPERATION_REQ_T;

/**
* before handler for get operation.
* all the get operation should call it first, .
* @param  pPara the get parameter, 
* @return true: success, false: failure
* @note  
*/
typedef BOOL8 (*GET_BEF_HANDLER_T)(DM_HANDLER_PARA_T *pPara);
/**
* real handler for get operation.
* all the get operation should call it after called GetBefHandler.
* the value of item will be set, after call this.
* @param  pPara the get parameter, 
* @return true: success, false: failure
* @note  
*/
typedef BOOL8 (*GET_CUR_HANDLER_T)(DM_HANDLER_PARA_T *pPara);
/**
* after handler for get operation.
* all the get operation should call it after called GetCurHandler.
* the value of item be sent to notify process.
* @param  pPara the get parameter, 
* @return true: success, false: failure
* @note  
*/
typedef BOOL8 (*GET_AFT_HANDLER_T)(DM_HANDLER_PARA_T *pPara);

/**
* before handler for set operation.
* all the set operation should call it first, for the handler will check the range of value.
* @param  pPara the set parameter, 
* @return true: success, false: failure
* @note  
*/
typedef BOOL8 (*SET_BEF_HANDLER_T)(DM_HANDLER_PARA_T *pPara);
/**
* real handler for set operation.
* all the set operation should call it after called SetBefHandler.
* the value of item will be writen to db, after call this.
* @param  pPara the set parameter, 
* @return true: success, false: failure
* @note  
*/
typedef BOOL8 (*SET_CUR_HANDLER_T)(DM_HANDLER_PARA_T *pPara);
/**
* after handler for set operation.
* all the set operation should call it after called SetCurHandler.
* the value of item be sent to notify process.
* @param  pPara the set parameter, 
* @return true: success, false: failure
* @note  
*/
typedef BOOL8 (*SET_AFT_HANDLER_T)(DM_HANDLER_PARA_T *pPara);

/**
* before handler for add operation.
* all the add operation should call it first, for the handler will check the range of value.
* @param  pPara the add parameter, 
* @return true: success, false: failure
* @note  
*/
typedef BOOL8 (*ADD_BEF_HANDLER_T)(DM_HANDLER_PARA_T *pPara);
/**
* real handler for add operation.
* all the add operation should call it after called AddBefHandler.
* the value of item will be writen to db, after call this.
* if object, the instance number of object will returned in pValue.
* @param  pPara the add parameter, 
* @return true: success, false: failure
* @note  
*/
typedef BOOL8 (*ADD_CUR_HANDLER_T)(DM_HANDLER_PARA_T *pPara);
/**
* after handler for add operation.
* all the add operation should call it after called AddCurHandler.
* the value of item be sent to notify process.
* @param  pPara the add parameter, 
* @return true: success, false: failure
* @note  
*/
typedef BOOL8 (*ADD_AFT_HANDLER_T)(DM_HANDLER_PARA_T *pPara);

/**
* before handler for del operation.
* all the del operation should call it first.
* @param  pPara the del parameter, 
* @return true: success, false: failure
* @note  
*/
typedef BOOL8 (*DEL_BEF_HANDLER_T)(DM_HANDLER_PARA_T *pPara);
/**
* real handler for del operation.
* all the del operation should call it after called DelBefHandler.
* the value of item will be deleted from db, after call this.
* @param  pPara the del parameter, 
* @return true: success, false: failure
* @note  
*/
typedef BOOL8 (*DEL_CUR_HANDLER_T)(DM_HANDLER_PARA_T *pPara);
/**
* after handler for del operation.
* all the del operation should call it after called DelCurHandler.
* the value of item be sent to notify process.
* @param  pPara the del parameter, 
* @return true: success, false: failure
* @note  
*/
typedef BOOL8 (*DEL_AFT_HANDLER_T)(DM_HANDLER_PARA_T *pPara);



/**
 * DM_REG_PATTERN_REQ_T struct
 */
typedef struct dm_reg_pattern_req_t
{
    INT8           *pucRootName;         /**< the name of root name */
	DM_NODE_INFO_T *pPatternFirstNode;   /**< the pointer of first pattern node */
	INT32           lPatternSize;        /**< the size of pattern table */
    DM_ATTR_INFO_T *pAttrPatternFirstNode;   /**< the pointer of first pattern node */
}DM_REG_PATTERN_REQ_T;

/**
 * The prototype of generated set function
 * @param  pGetPara the  parameter
 * @return INT32
 * @note   
 *
 */
typedef INT32  (*DM_SET_OPERATION_FUN) (DM_SET_PARA_C_VALUE_REQ_T *pSetPara) ;

/**
 * DM_REG_SET_OPERATION_REQ_T struct
 */
typedef struct dm_reg_set_operation_req_t
{
    INT8               *pucPath;         /**< the path of  item*/
	UINT32                ulParaId;         /**< the id of item */
	DM_SET_OPERATION_FUN pFunc;           /**< the get function for item */
}DM_REG_SET_OPERATION_REQ_T;

/**
 * The prototype of generated add function
 * @param  pGetPara the  parameter
 * @return INT32
 * @note   
 *
 */
typedef INT32  (*DM_ADD_OPERATION_FUN) (DM_ADD_OBJ_BY_ID_REQ_T  *pAddPara) ;

/**
 * DM_REG_ADD_OPERATION_REQ_T struct
 */
typedef struct dm_reg_add_operation_req_t
{
    INT8               *pucPath;         /**< the path of  item*/
	UINT32                ulParaId;         /**< the id of item */
	DM_ADD_OPERATION_FUN pFunc;           /**< the get function for item */
}DM_REG_ADD_OPERATION_REQ_T;

/**
 * The prototype of generated del function
 * @param  pGetPara the  parameter
 * @return INT32
 * @note   
 *
 */
typedef INT32  (*DM_DEL_OPERATION_FUN) (DM_DEL_OBJ_BY_ID_REQ_T *pGetPara) ;

/**
 * DM_REG_DEL_OPERATION_REQ_T struct
 */
typedef struct dm_reg_del_operation_req_t
{
    INT8                 *pucPath;         /**< the path of  item*/
	UINT32                ulParaId;         /**< the id of item */
	DM_DEL_OPERATION_FUN pFunc;           /**< the get function for item */
}DM_REG_DEL_OPERATION_REQ_T;


#define MAX_XML_PATH 1024
#define DM_MAXOBJ_NUM 1024
#define MAX_NAME_LEN 512
#define MAX_STRING_VALUE_LEN 60000
#define MAX_SET_STRING_VALUE_LEN 60000

typedef struct
{
	xmlXPathObjectPtr   xpathObj;
	INT32           lIndex;
}DM_XML_OBJ_STACK;

typedef struct
{
    UINT32   ulICnt;                    /* {i} count in full path of parameter */
    UINT32  *pulIValue;                 /* the value of each {i} */
	void    *pParent;
}DM_PARA_PATH_T;


typedef struct
{
    UINT32          ulParaIdx;          /* parameter index in pattern table */
    DM_PARA_PATH_T *ParaPath;           /* parameter path related info */
    UINT8          *pucParaValue;       /* parameter value in form of string */
}DM_GET_LEAF_PARA_T;

typedef struct 
{
    DM_TRANSACTION_REQ_T transReq;
    BOOL8             bNeedBak;
    DM_NOTIFY_ID_LIST_T *pNotifyIdList;
    DM_NOTIFY_ID_LIST_T *pAttrNotifyIdList;
}DM_TRANSACTION_INFO_T;

typedef struct
{
    pid_t pid;      /* For owner permission */
} DM_CALLER_INFO_T;

typedef struct
{
    OSA_THREAD_T thread;    /* osa thread handler */
    OSA_MSGQ_T   msgQ;      /* message queue */
    pid_t        createId;  /* create process id */
} DM_SRV_MONITOR_THREAD_T;

typedef enum
{
    DM_DEFAULT_FILE__HARDWARE,
    DM_DEFAULT_FILE__OPERATOR
}DM_DEFAULT_FILE_TYPE;


#define DM_UINT32_BITMAP_MAX_VALUE      0x80000000
#define DM_UINT32_ENUM_MAX_VALUE        0xffffffff

extern INT32 DmXmlFileGenerateReq(INT8 *pucRootNode);
extern BOOL8 DmRegisterDelOperationReq(DM_REG_DEL_OPERATION_REQ_T *pPara);
extern BOOL8 DmRegisterAddOperationReq(DM_REG_ADD_OPERATION_REQ_T *pPara);
extern BOOL8 DmRegisterSetOperationReq(DM_REG_SET_OPERATION_REQ_T *pPara);
extern BOOL8 DmRegisterGetOperationReq(DM_REG_GET_OPERATION_REQ_T *pPara);
extern BOOL8 DmRegisterPatternReq(DM_REG_PATTERN_REQ_T *pPara);
extern BOOL8 DbGetCurHandler(DM_HANDLER_PARA_T *pGetPara);
extern BOOL8 DbGetCurHandlerWithFullPath(DM_HANDLER_PARA_T *pGetPara);
extern BOOL8 DbSetCurHandler(DM_HANDLER_PARA_T *pSetPara);
extern BOOL8 DbAddCurHandler(DM_HANDLER_PARA_T *pAddPara);
extern BOOL8 DbDelCurHandler(DM_HANDLER_PARA_T *pDelPara);
extern BOOL8 DbSetAttrCurHandler(DM_HANDLER_PARA_T *pSetParaAttr);
extern BOOL8 DbGetAttrCurHandler(DM_HANDLER_PARA_T *pGetParaAttr);
extern INT32 DmGetIDFromPath(DM_HASH_T *pTable, INT8 *pucPath);
extern BOOL8 isObject(DM_HANDLER_T *pHandler, UINT32 lId);
extern BOOL8 ConvertPathToXPath(INT8 *pucPath, INT8 *pucXPath);
extern INT32 DbGetObjectNum(DM_HANDLER_T *pDmHandler, INT8 *pucPath);
extern BOOL8 DbSetCurHandlerForC(DM_HANDLER_PARA_T *pSetPara);

extern BOOL8 DbSetNodeAttribute(INT8 *pucPath, INT8 *pucAttrName, INT8 *pucAttrValue);


extern DM_HANDLER_T *GetDmHandlerByTransId(UINT32 transactionId);

extern BOOL8 DefaultAddAftHandler(DM_HANDLER_PARA_T *);
extern BOOL8 DefaultAddCurHandler(DM_HANDLER_PARA_T *);
extern BOOL8 DefaultAddBefHandler(DM_HANDLER_PARA_T *);


extern BOOL8 DefaultDelAftHandler(DM_HANDLER_PARA_T *);
extern BOOL8 DefaultDelCurHandler(DM_HANDLER_PARA_T *);
extern BOOL8 DefaultDelBefHandler(DM_HANDLER_PARA_T *);

extern BOOL8 DefaultSetAftHandler(DM_HANDLER_PARA_T *);
extern BOOL8 DefaultSetCurHandler(DM_HANDLER_PARA_T *);
extern BOOL8 DefaultSetBefHandler(DM_HANDLER_PARA_T *);

extern BOOL8 DefaultGetAftHandler(DM_HANDLER_PARA_T *);
extern BOOL8 DefaultGetCurHandler(DM_HANDLER_PARA_T *);
extern BOOL8 DefaultGetBefHandler(DM_HANDLER_PARA_T *);

extern BOOL8 DefaultSetAttrAftHandler(DM_HANDLER_PARA_T *pSetParaAttr);

extern BOOL8 DbGetObjectIndexCont(DM_HANDLER_T *pDmHandler, INT8 *pucPath, UINT32 *pValue);
extern DM_HANDLER_T *GetDmHandlerByName(INT8 *pucName);
extern DM_HANDLER_T *GetDmHandlerByLibHandler(void* pLibHandler);

extern BOOL8 DmCallDefaultGetHandler(DM_HANDLER_PARA_T *pDmHandlerPara);
extern BOOL8 DmCallDefaultSetHandler(DM_HANDLER_PARA_T *pDmHandlerPara);
extern BOOL8 DmCallDefaultAddHandler(DM_HANDLER_PARA_T  *pDmHandlerPara);
extern BOOL8 DmCallDefaultDelHandler(DM_HANDLER_PARA_T *pDmHandlerPara);
extern BOOL8 DmIsSingleOfMultiple(INT8 *pucPath);
extern INT32 DmGetInstanceNumFromPath(INT8 *pucPath);
extern UINT32 DmGetObjectLengthOfInstance(INT8 *pucPath);
extern xmlXPathObjectPtr DbGetXPathObjPtr(xmlXPathContextPtr xpathCtx,INT8 *ucXPathName);


extern void DmGetAllDbPara();
extern INT32 DmCallAddHandler(DM_NODE_HANDLER_T *pNodeHandler, DM_HANDLER_PARA_T *pPara);


extern void DmCheckTransferNeed(DM_HANDLER_T *pDmHandler, UINT32 ulParaId);

INT32   DmAddObjectReqWithInstance(DM_ADD_OBJECT_REQ_T *pAddObjectReq);

extern INT8* DmGetInstanceNumSequence(const INT8 *pucParaName);
extern INT8* DmGetDefaultValue(const INT8 *pucInsNumSeq, const INT8 *pDefaultValue);
extern INT8 *DmChangeOrgNameToFullname(INT8 *pucOrgName);



extern BOOL8 DmValidityCheckCallBack(DM_VALID_CHECK_T *pPara);


extern INT32 DmSyncDBFileReq(DM_HANDLER_T* pDmHandler);

BOOL8 DmIsMultipleObject(DM_NODE_INFO_T *pPattern, UINT32 ulParaId);

BOOL8 DmGenerateDefaultValuesList(INT8 *pDirPath);
INT8* GetRootNameFromPath(INT8 *pucName);

DM_RW_MUTEX_T *DmGetRwMutexForTransaction(DM_HANDLER_T *pDmHandler, INT8 *pRootNodeToLock, INT8 *pAccessEntity);
BOOL8 DmReleaseRwMutexOfTransaction(DM_HANDLER_T *pDmHandler, INT8 *pRootNodeToLock, INT8 *pAccessEntity);
BOOL8 DmDupTransactionInfo(DM_HANDLER_T *pDmHandler, DM_TRANSACTION_REQ_T *pTransaction);
DM_TRANSACTION_INFO_T *DmDropTransactionInfo(DM_HANDLER_T *pDmHandler, UINT32 transactionId);
DM_TRANSACTION_INFO_T *DmGetTransactionInfo(DM_HANDLER_T *pDmHandler, UINT32 transactionId);
void DmUpdateTransactionInfo(DM_HANDLER_T *pDmHandler, UINT32 transactionId);

//BOOL8 DmTransferDb();
BOOL8 DmCallNotifyHandler(DM_HANDLER_T *pDmHandler, DM_TRANSACTION_INFO_T *pTransaction );
BOOL8 DmCheckRight(UINT32 lTransactionID, DM_HANDLER_T *pDmHandler, INT32 lRightType, UINT32 ulParaId);
INT8* DmGetObjectPath(INT8 *pucItemName);
void* DmHashGetFromPath(DM_HASH_T *pHash, INT8* key);
void DmFreeGetParaValueReq(void *p);
BOOL8 DbNodeExistCheck(DM_HANDLER_T *pDmHandler,INT8 *pcNodeName);
INT8* DmIntToStr(INT32 lNum);
INT8* DmUintToStr(UINT32 ulNum);

INT8 * DmGetLastNodeName(INT8 *pucObjectPath);
INT8* DmGetMulObjPathAndInstance(INT8 *pucObjectPath, UINT32 *pulInstanceNum);
INT32 DmGetChildren(DM_GET_PARA_NAME_REQ_T *pGetParaNameReq);
INT32 DmGetDescendant(DM_GET_PARA_NAME_REQ_T *pGetParaNameReq);
INT32 DmAddObject(DM_HANDLER_PARA_T *pPara);

extern BOOL8 DmIsMultipleObject(DM_NODE_INFO_T *pPattern, UINT32 ulParaId);

INT8 *DmFilterParaName(DM_HANDLER_T *pDmHandler, INT32 lId, INT8 *pucName);

INT32 DmComStartReq (BOOL8 bAsServer, DM_START_REQ_T *pStartReq);
INT32 DmStartNonTransHandler();
void  DmStopNonTransHandler();
INT32 DmNonTransReqEntry(DM_TRANS_CTRL_T *pNonTransCtrl);
INT32 DmEnsureStartService();
INT32 DmSelfRegisterService();
INT32 DmSelfStopService();
INT32 DmCheckOwnerPermission(INT8 *pucName, pid_t callerPid);
INT32 DmLoadDbHandlerReq(DM_LOAD_DB_HANDLER_REQ_T *pLoadDbHandlerReq);
INT32 DmLoadDbHandlerReqInternal(DM_LOAD_DB_HANDLER_REQ_T *pLoadDbHandlerReq);
INT32 DmUnloadDbHandlerReq(DM_UNLOAD_DB_HANDLER_REQ_T *pUnloadDbHandlerReq);
INT32 DmUnloadDbHandlerReqInternal(DM_UNLOAD_DB_HANDLER_REQ_T *pUnloadDbHandlerReq);
INT32 DmSendCnfMsgIfNoTrans(UINT32 receiver_id, UINT32 ReqMsgId, DM_SOCK_MSG_HEADER_T *pSockMsgSnd);
INT32 DmGetParaNameInternal(DM_GET_PARA_NAME_REQ_T *pGetParaNameReq);
INT32 DmGetParaCValueInternal(DM_GET_PARA_C_VALUE_REQ_T *  pGetPara);
INT32 DmSetParaCValueInternal  ( DM_SET_PARA_C_VALUE_REQ_T *  pSetPara );
INT32 DmAddObjectByIdInternal  ( DM_ADD_OBJ_BY_ID_REQ_T *  pAddPara );
INT32 DmDelObjectByIdInternal  ( DM_DEL_OBJ_BY_ID_REQ_T *  pDelPara );
INT32 DmGetParaValueInternal(DM_GET_PARA_VALUE_REQ_T    *pGetParaValueReq);
INT32 DmSetParaValueInternal(DM_SET_PARA_VALUE_REQ_T    *pSetParaValueReq);
INT32 DmAddObjectInternal(DM_ADD_OBJECT_REQ_T *pAddObjectReq);
INT32 DmDeleteObjectInternal(DM_DELETE_OBJECT_REQ_T *pDeleteObjectReq);
INT32 DmSetParaAttributeInternal(DM_SET_PARA_ATTR_REQ_T    *pSetParaAttrReq);

INT32 DmShmGroupHandler(void *pResourceData, OSA_RESOURCE_GRP_STATUS_E status, void *pExtraData);

INT32 DmHashCreate(DM_HASH_T *pHash, INT32 hashSize);
INT32 DmHashDestroy(DM_HASH_T *pHash, BOOL8 bDelValue);
void *DmHashEnumerator(DM_HASH_T *pHash, DM_HASH_ENUM_T *pEnum);
INT32 DmHashAdd(DM_HASH_T *pHash, char* key,void* value);
void* DmHashDel(DM_HASH_T *pHash, INT8* key);
void* DmHashGetKey(DM_HASH_T *pHash, void * value);
void* DmHashGetValue(DM_HASH_T *pHash, INT8* key);
INT32 DmHashGetUsedsize(DM_HASH_T *pHash);
void* DmHashFindValue(DM_HASH_T *pHash, BOOL8 (*CheckCallback)(void * current_value, void *param), void* param);

INT32 DmDataModelVersionChecker(INT8 *pDbName);
INT32 DmApplyDefaultValueFile(DM_HANDLER_T *pDmHandler, INT8 *pDefaultFile, UINT32 TransactionId, INT8 *pucRootNode, DM_DEFAULT_FILE_TYPE defaultType);
INT32 DmDataModelTransferHandler(INT8 *pDbName, DM_DATA_TRANSFER_MODE_E mode);


INT32 DmGetXpathExpression(INT8 *pFullPath, INT8 *pXpath);
BOOL8 DmResetParams(INT8 *pRootName, INT32 resetCnt, INT32 *resetArray);


extern INT32 DmTransactionGroupHandler(void *pResourceData, OSA_RESOURCE_GRP_STATUS_E status, void *pExtraData);
extern INT32 DmLoadDbReq(DB_CONFIG_T *pDbConfig, void *pDlHandler, BOOL8 bDbReset, INT8 *pWorkPath, INT8 *pLoadPath, INT8 *pBackupPath);


extern INT32 DmSockMsgServiceStartReq(DM_SOCK_MSG_T *pSockMsg, INT8 *pSockName, DM_SOCK_MSG_RCV_FUN pRcvFunc, UINT8 asServer);
extern INT32 DmSockMsgServiceStopReq(DM_SOCK_MSG_T *pSockMsg);
extern INT32 DmSockMsgSendReq(DM_SOCK_MSG_T *pSockMsg, void *pMsg);
extern INT32 DmSockMsgBroadcastReq(DM_SOCK_MSG_T *pSockMsg, void *pMsg);
extern INT32 DmWaitSockStarted(INT8 *pSockName);

extern INT32 DmOneNotifyHandlerCalling(INT8 *pEntity, DM_NOTIFY_T *pNotifyPara);
extern INT32 DmGetHandlerCalling(DM_NODE_GET_HANDLER_T *pGetHandler, DM_GET_VALUE_T *pPara);
extern INT32 DmSetHandlerCalling(DM_NODE_SET_HANDLER_T *pSetHandler, DM_SET_VALUE_T *pPara);

extern void DmClFreeGetParaNameReq(DM_GET_PARA_NAME_REQ_T *pGetParaNameReq);
extern void DmClFreeGetParaValueReq(DM_GET_PARA_VALUE_REQ_T    *pGetParaValueReq);

extern INT32 DmUpdateNumberOfEntries(INT8 *pucName, INT32 numberOfEntries, DM_HANDLER_T *pDmHandler);
extern BOOL8 ConvertPathToXPathSimple(INT8 *pucPath, INT8 *pucXPath);

extern void DmTrace(const INT8 *file, INT32 line, UINT16 usLevel, UINT32 errType, const char *string,...);

BOOL8 DmIntegerArrayValidCheck(INT8 *pValueRange, INT8 *pValue, INT32 integerType);

BOOL8 DmDataUINT64ValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmDataUINT32ValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmDataUINT16ValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmDataUINT8ValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmDataINT64ValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmDataINT32ValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmDataINT16ValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmDataINT8ValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmDataUINT64ArrayValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmDataUINT32ArrayValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmDataUINT16ArrayValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmDataUINT8ArrayValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmDataINT64ArrayValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmDataINT32ArrayValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmDataINT16ArrayValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmDataINT8ArrayValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmDataUNT32HexValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmDataBoolValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmEnumStringValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmBitmapStringValidCheck(INT8 *pValueRange, INT8 *pValue);


BOOL8 DmGetCHandler(DM_GET_PARA_C_VALUE_REQ_T *  pGetPara);
BOOL8 DmSetCHandler(DM_SET_PARA_C_VALUE_REQ_T *  pSetPara);
BOOL8 DmAddCHandler(DM_ADD_OBJ_BY_ID_REQ_T *  pAddPara);
BOOL8 DmDelCHandler(DM_DEL_OBJ_BY_ID_REQ_T *  pDelPara);

INT32 DmCreateShmKey(INT8* pTokenFile, INT32 TokenFileSize, UINT32* shm_key);
INT32 DmReleaseShmKey(INT8* pTokenFile, UINT32 shm_key);

BOOL8 DmAttrVaildCheck(DM_HANDLER_T *pDmHandler, INT32 lId, DM_ATTR_NODE_T *pAttrNode);
BOOL8 DmConvertAttributeToString(DM_HANDLER_T *pDmHandler, INT8 *pAttrName, INT8 *pAttrValue, UINT32 attrValueCnt, INT8 *pStringValue);
BOOL8 DmConvertAttributeToStruct(DM_HANDLER_T *pDmHandler, INT8 *pAttrName, INT8 *pAttrValue, UINT32 *attrValueCnt, INT8 *pStructValue, UINT32 *structValueSize);
INT8 *DmGetDefaultAttrValue(DM_HANDLER_T *pDmHandler, INT32 lDbId, INT8 *pAttrName);
INT32 DbAddDefaultAttrHandler(DM_HANDLER_T *pDmHandler, INT32 lDbId, xmlNodePtr nodePtr);
INT32 DmGetAttrId(DM_HANDLER_T *pDmHandler, INT8 *pAttrName);
void DmClFreeGetParaAttributeReq(DM_GET_PARA_ATTR_REQ_T *pGetParaAttrReq);
void DmSrFreeGetParaAttributeReq(DM_GET_PARA_ATTR_REQ_T *pGetParaAttrReq);
BOOL8 DmCheckIfAttrCreatedByDefault(DM_HANDLER_T *pDmHandler, INT32 lDbId, INT8 *pAttrName);

INT32 DmLoadAddOn(INT8* pAddonPath, INT8 *pAddOnName);
INT32 DmUnloadAddOn(INT8 *pAddOnName);
INT32 DmUnloadAllAddOn(void);

/* DM error definition */
#define DM_WARNING(...)		        DmTrace(__FILE__, __LINE__, DM_TRACE_LEVEL_WARNING, 0, __VA_ARGS__)
#define DM_ERROR(errType, ...)		DmTrace(__FILE__, __LINE__, DM_TRACE_LEVEL_ERROR, errType, __VA_ARGS__)
#define DM_TRACE(Level, ...)		DmTrace(__FILE__, __LINE__, Level, 0, __VA_ARGS__)


extern DM_HASH_T g_dmHandlerHash;
extern DM_CTRL_T g_DmCtrl;


#define DM_INTEGER_TYPE__INT8     0
#define DM_INTEGER_TYPE__INT16    1
#define DM_INTEGER_TYPE__INT32    2
#define DM_INTEGER_TYPE__INT64    3
#define DM_INTEGER_TYPE__UINT8    4
#define DM_INTEGER_TYPE__UINT16   5
#define DM_INTEGER_TYPE__UINT32   6
#define DM_INTEGER_TYPE__UINT64   7

#define DM_OFFSETOF(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)





#ifdef __cplusplus
}
#endif

#endif


