

#include "dm_internal.h"
#include "internal_model_def.h"

#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif


/* the declaration of data-valid-check function list */
extern BOOL8 InternalDataValidChecker(UINT8 ucParaCType, INT8 *pValueRange, INT8 *pValue);

extern BOOL8 DmDataBoolValidCheck(INT8 *pValueRange, INT8 *pValue);
extern BOOL8 DmDataINT32ValidCheck(INT8 *pValueRange, INT8 *pValue);
extern BOOL8 DmDataStringLengthValidCheck(INT8 *pValueRange, INT8 *pValue);
extern BOOL8 DmDataUINT16ValidCheck(INT8 *pValueRange, INT8 *pValue);
extern BOOL8 DmDataUINT32ValidCheck(INT8 *pValueRange, INT8 *pValue);
extern BOOL8 DmDataUINT64ValidCheck(INT8 *pValueRange, INT8 *pValue);
extern BOOL8 DmDataUINT8ArrayValidCheck(INT8 *pValueRange, INT8 *pValue);
extern BOOL8 DmDataUINT8ValidCheck(INT8 *pValueRange, INT8 *pValue);
extern BOOL8 DmEnumStringValidCheck(INT8 *pValueRange, INT8 *pValue);


/* the declaration of Internal data-type and c-type str-functions */
extern INT8* InternalGetCTypeStr(UINT8 ucParaCType);
extern INT8* InternalGetDataTypeStr(UINT8 ucParaType);


/* the declaration of Internal model register functions */
extern BOOL8 InternalDmPatternRegisterHandler();


/* the declaration of is-object-function */
extern BOOL8 InternalIsObject(UINT8 ucParaType);


/* the declaration of convert-fuctions */
extern BOOL8 DmConvertUint64ToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
extern BOOL8 DmConvertStringToUint64(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
extern BOOL8 DmConvertInt32ToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
extern BOOL8 DmConvertStringToInt(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
extern BOOL8 DmConvertCharArrayToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
extern BOOL8 DmConvertStringToCharArray(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
extern BOOL8 DmConvertUint8ToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
extern BOOL8 DmConvertStringToUint8(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
extern BOOL8 DmConvertBoolToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
extern BOOL8 DmConvertStringToBool(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
extern BOOL8 DmConvertUint16ToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
extern BOOL8 DmConvertStringToUint16(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
extern BOOL8 DmConvertEnumToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
extern BOOL8 DmConvertStringToEnum(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
extern BOOL8 DmConvertUint8ArrayToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
extern BOOL8 DmConvertStringToUint8Array(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
extern BOOL8 DmConvertUint32ToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
extern BOOL8 DmConvertStringToUint32(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);


DM_NODE_INFO_T g_InternalModelNodes[]=
{
    {
        /* Name */
        (INT8*)"Internal.",

        /* RealName */
        (INT8*)"Internal",

        /* Description */
        (INT8*)"The top-level object for a Device.",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        0,

        /* CTypeSize */
        sizeof(INTERNAL_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        -1,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        1,

        /* ChildrenCounter */
        10,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"0",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"VendorDataModelVersion",

        /* RealName */
        (INT8*)"VendorDataModelVersion",

        /* Description */
        (INT8*)"Vendor data model version, e.g. 1.01. ",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_T,VendorDataModelVersion),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_VENDORDATAMODELVERSION_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_T,VendorDataModelVersionCnt),

        /* ParentIndex */
        0,

        /* SiblingIndex */
        2,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"32",

        /* DefaultValue */
        (INT8*)"20124",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"VendorConfigLocalIp",

        /* RealName */
        (INT8*)"VendorConfigLocalIp",

        /* Description */
        (INT8*)"IP address to recovery board with webui",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_T,VendorConfigLocalIp),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_VENDORCONFIGLOCALIP_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_T,VendorConfigLocalIpCnt),

        /* ParentIndex */
        0,

        /* SiblingIndex */
        3,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"128",

        /* DefaultValue */
        (INT8*)"192.168.200.200",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        1,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"Internal.Scm.",

        /* RealName */
        (INT8*)"Scm",

        /* Description */
        (INT8*)"This object contains CME info.",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_T,Scm),

        /* CTypeSize */
        sizeof(INTERNAL_SCM_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        0,

        /* SiblingIndex */
        6,

        /* FirstChildIndex */
        4,

        /* ChildrenCounter */
        2,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"0",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"UpgradeErrRetToEme",

        /* RealName */
        (INT8*)"UpgradeErrRetToEme",

        /* Description */
        (INT8*)"if firmware upgrading error, after reboot, need to send this content to EME in TransferComplete RPC",

        /* ParaType */
        INTERNAL_DATA_TYPE_INT,

        /* CType */
        INTERNAL_C_TYPE_INT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SCM_T,UpgradeErrRetToEme),

        /* CTypeSize */
        sizeof(INT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        3,

        /* SiblingIndex */
        5,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertInt32ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"DisableSoftdogChecker",

        /* RealName */
        (INT8*)"DisableSoftdogChecker",

        /* Description */
        (INT8*)"Disable softdog checker.",

        /* ParaType */
        INTERNAL_DATA_TYPE_BOOLEAN,

        /* CType */
        INTERNAL_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SCM_T,DisableSoftdogChecker),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        3,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertBoolToString,

        /* ToStructFunc */
        DmConvertStringToBool
    },

    {
        /* Name */
        (INT8*)"Internal.Ema.",

        /* RealName */
        (INT8*)"Ema",

        /* Description */
        (INT8*)"This object contains CME info.",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_T,Ema),

        /* CTypeSize */
        sizeof(INTERNAL_EMA_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        0,

        /* SiblingIndex */
        21,

        /* FirstChildIndex */
        7,

        /* ChildrenCounter */
        4,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"0",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"NumOfEme",

        /* RealName */
        (INT8*)"NumOfEme",

        /* Description */
        (INT8*)"Number of CME",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_EMA_T,NumOfEme),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        6,

        /* SiblingIndex */
        8,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"2",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint8ToString,

        /* ToStructFunc */
        DmConvertStringToUint8
    },

    {
        /* Name */
        (INT8*)"Internal.Ema.Curl.",

        /* RealName */
        (INT8*)"Curl",

        /* Description */
        (INT8*)"This object contains CME info.",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_EMA_T,Curl),

        /* CTypeSize */
        sizeof(INTERNAL_EMA_CURL_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        6,

        /* SiblingIndex */
        13,

        /* FirstChildIndex */
        9,

        /* ChildrenCounter */
        4,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"0",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"connTimeout",

        /* RealName */
        (INT8*)"connTimeout",

        /* Description */
        (INT8*)"timeout for curl to upload or download, 10s by default",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_EMA_CURL_T,connTimeout),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        8,

        /* SiblingIndex */
        10,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"10",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"limitRate",

        /* RealName */
        (INT8*)"limitRate",

        /* Description */
        (INT8*)"rate limit for curl to upload or download, disabled by default",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_EMA_CURL_T,limitRate),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        8,

        /* SiblingIndex */
        11,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"retryMax",

        /* RealName */
        (INT8*)"retryMax",

        /* Description */
        (INT8*)"retries if upload or download failure",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_EMA_CURL_T,retryMax),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        8,

        /* SiblingIndex */
        12,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"2",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"retryInterval",

        /* RealName */
        (INT8*)"retryInterval",

        /* Description */
        (INT8*)"retries interval if upload or download failure, unit(second)",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_EMA_CURL_T,retryInterval),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        8,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"5",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"Internal.Ema.SessionStatus.",

        /* RealName */
        (INT8*)"SessionStatus",

        /* Description */
        (INT8*)"session status",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_EMA_T,SessionStatus),

        /* CTypeSize */
        sizeof(INTERNAL_EMA_SESSIONSTATUS_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        6,

        /* SiblingIndex */
        16,

        /* FirstChildIndex */
        14,

        /* ChildrenCounter */
        2,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"EntityId",

        /* RealName */
        (INT8*)"EntityId",

        /* Description */
        (INT8*)"entity instance ID",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_EMA_SESSIONSTATUS_T,EntityId),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        13,

        /* SiblingIndex */
        15,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "integer:set-zero",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"UpdateTime",

        /* RealName */
        (INT8*)"UpdateTime",

        /* Description */
        (INT8*)"update time",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_EMA_SESSIONSTATUS_T,UpdateTime),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        13,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "integer:set-zero",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"Internal.Ema.emeInfo.{i}.",

        /* RealName */
        (INT8*)"emeInfo",

        /* Description */
        (INT8*)"This object contains parameters relating to each EME",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_EMA_T,emeInfo),

        /* CTypeSize */
        sizeof(INTERNAL_EMA_EMEINFO_T),

        /* MaxElements */
        INTERNAL_EMA_EMEINFO_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_EMA_T,emeInfo_cnt),

        /* ParentIndex */
        6,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        17,

        /* ChildrenCounter */
        4,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"2",

        /* DefaultValue */
        (INT8*)"2",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"Name",

        /* RealName */
        (INT8*)"Name",

        /* Description */
        (INT8*)"entity name",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_EMA_EMEINFO_T,Name),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_EMA_EMEINFO_NAME_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_EMA_EMEINFO_T,NameCnt),

        /* ParentIndex */
        16,

        /* SiblingIndex */
        18,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"16",

        /* DefaultValue */
        (INT8*)"[1]=Tr69c[2]=WebUI",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"Enable",

        /* RealName */
        (INT8*)"Enable",

        /* Description */
        (INT8*)"enable the entity",

        /* ParaType */
        INTERNAL_DATA_TYPE_BOOLEAN,

        /* CType */
        INTERNAL_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_EMA_EMEINFO_T,Enable),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        16,

        /* SiblingIndex */
        19,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"[1]=1[2]=1",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertBoolToString,

        /* ToStructFunc */
        DmConvertStringToBool
    },

    {
        /* Name */
        (INT8*)"RegisterRequest",

        /* RealName */
        (INT8*)"RegisterRequest",

        /* Description */
        (INT8*)"need to register itself to its server",

        /* ParaType */
        INTERNAL_DATA_TYPE_BOOLEAN,

        /* CType */
        INTERNAL_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_EMA_EMEINFO_T,RegisterRequest),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        16,

        /* SiblingIndex */
        20,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"[1]=1[2]=1",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertBoolToString,

        /* ToStructFunc */
        DmConvertStringToBool
    },

    {
        /* Name */
        (INT8*)"AutonomousTransferCompleteInd",

        /* RealName */
        (INT8*)"AutonomousTransferCompleteInd",

        /* Description */
        (INT8*)"need to know any autonomous file transfer actions.",

        /* ParaType */
        INTERNAL_DATA_TYPE_BOOLEAN,

        /* CType */
        INTERNAL_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_EMA_EMEINFO_T,AutonomousTransferCompleteInd),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        16,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"[1]=1[2]=0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertBoolToString,

        /* ToStructFunc */
        DmConvertStringToBool
    },

    {
        /* Name */
        (INT8*)"Internal.WebUI.",

        /* RealName */
        (INT8*)"WebUI",

        /* Description */
        (INT8*)"webui configuration tree",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_T,WebUI),

        /* CTypeSize */
        sizeof(INTERNAL_WEBUI_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        0,

        /* SiblingIndex */
        26,

        /* FirstChildIndex */
        22,

        /* ChildrenCounter */
        4,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"0",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"UserName",

        /* RealName */
        (INT8*)"UserName",

        /* Description */
        (INT8*)"webui user name",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_WEBUI_T,UserName),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_WEBUI_USERNAME_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_WEBUI_T,UserNameCnt),

        /* ParentIndex */
        21,

        /* SiblingIndex */
        23,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"128",

        /* DefaultValue */
        (INT8*)"admin",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"Password",

        /* RealName */
        (INT8*)"Password",

        /* Description */
        (INT8*)"webui password",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_WEBUI_T,Password),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_WEBUI_PASSWORD_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_WEBUI_T,PasswordCnt),

        /* ParentIndex */
        21,

        /* SiblingIndex */
        24,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"128",

        /* DefaultValue */
        (INT8*)"admin",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"loggingLevel",

        /* RealName */
        (INT8*)"loggingLevel",

        /* Description */
        (INT8*)"webui log level, Debug > Notice",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_WEBUI_T,loggingLevel),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_WEBUI_LOGGINGLEVEL_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_WEBUI_T,loggingLevelCnt),

        /* ParentIndex */
        21,

        /* SiblingIndex */
        25,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"16",

        /* DefaultValue */
        (INT8*)"Notice",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"loggingDestination",

        /* RealName */
        (INT8*)"loggingDestination",

        /* Description */
        (INT8*)"Where to output log: stderr, syslog or telnet clients",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_WEBUI_T,loggingDestination),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_WEBUI_LOGGINGDESTINATION_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_WEBUI_T,loggingDestinationCnt),

        /* ParentIndex */
        21,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"32",

        /* DefaultValue */
        (INT8*)"Syslog",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"Internal.Tr69c.",

        /* RealName */
        (INT8*)"Tr69c",

        /* Description */
        (INT8*)"tr69c configuration tree",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_T,Tr69c),

        /* CTypeSize */
        sizeof(INTERNAL_TR69C_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        0,

        /* SiblingIndex */
        84,

        /* FirstChildIndex */
        27,

        /* ChildrenCounter */
        7,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"0",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"TransferNeeded",

        /* RealName */
        (INT8*)"TransferNeeded",

        /* Description */
        (INT8*)"We don't use this item currently",

        /* ParaType */
        INTERNAL_DATA_TYPE_BOOLEAN,

        /* CType */
        INTERNAL_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_T,TransferNeeded),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        26,

        /* SiblingIndex */
        28,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertBoolToString,

        /* ToStructFunc */
        DmConvertStringToBool
    },

    {
        /* Name */
        (INT8*)"Internal.Tr69c.Config.",

        /* RealName */
        (INT8*)"Config",

        /* Description */
        (INT8*)"tr69c Basic configuration",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_T,Config),

        /* CTypeSize */
        sizeof(INTERNAL_TR69C_CONFIG_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        26,

        /* SiblingIndex */
        43,

        /* FirstChildIndex */
        29,

        /* ChildrenCounter */
        8,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"0",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"loggingLevel",

        /* RealName */
        (INT8*)"loggingLevel",

        /* Description */
        (INT8*)"tr69c log level, Debut > Notice",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_CONFIG_T,loggingLevel),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_CONFIG_LOGGINGLEVEL_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_CONFIG_T,loggingLevelCnt),

        /* ParentIndex */
        28,

        /* SiblingIndex */
        30,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"16",

        /* DefaultValue */
        (INT8*)"Notice",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"loggingDestination",

        /* RealName */
        (INT8*)"loggingDestination",

        /* Description */
        (INT8*)"Where to output log: stderr, syslog or telnet clients",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_CONFIG_T,loggingDestination),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_CONFIG_LOGGINGDESTINATION_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_CONFIG_T,loggingDestinationCnt),

        /* ParentIndex */
        28,

        /* SiblingIndex */
        31,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"32",

        /* DefaultValue */
        (INT8*)"Standard Error",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"loggingSOAP",

        /* RealName */
        (INT8*)"loggingSOAP",

        /* Description */
        (INT8*)"Whether log soap message",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_CONFIG_T,loggingSOAP),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        28,

        /* SiblingIndex */
        32,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertBoolToString,

        /* ToStructFunc */
        DmConvertStringToBool
    },

    {
        /* Name */
        (INT8*)"connectionRequestAuthentication",

        /* RealName */
        (INT8*)"connectionRequestAuthentication",

        /* Description */
        (INT8*)"Enable/disable authentication when connection request happened",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_CONFIG_T,connectionRequestAuthentication),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        28,

        /* SiblingIndex */
        33,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertBoolToString,

        /* ToStructFunc */
        DmConvertStringToBool
    },

    {
        /* Name */
        (INT8*)"enableSelfdefinedEventCode",

        /* RealName */
        (INT8*)"enableSelfdefinedEventCode",

        /* Description */
        (INT8*)"Enable/disable Self-defined Event Code",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_CONFIG_T,enableSelfdefinedEventCode),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        28,

        /* SiblingIndex */
        34,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"1",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertBoolToString,

        /* ToStructFunc */
        DmConvertStringToBool
    },

    {
        /* Name */
        (INT8*)"emeSessionCheckInterval",

        /* RealName */
        (INT8*)"emeSessionCheckInterval",

        /* Description */
        (INT8*)"Timeout Interval  after start eme session is sent.",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_CONFIG_T,emeSessionCheckInterval),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        28,

        /* SiblingIndex */
        35,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"5",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"emeSessionCheckMaxCount",

        /* RealName */
        (INT8*)"emeSessionCheckMaxCount",

        /* Description */
        (INT8*)"Max checkcount if eme session is used by other entities",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_CONFIG_T,emeSessionCheckMaxCount),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        28,

        /* SiblingIndex */
        36,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"3",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"Internal.Tr69c.Config.TLS.",

        /* RealName */
        (INT8*)"TLS",

        /* Description */
        (INT8*)"TLS certificate info",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_CONFIG_T,TLS),

        /* CTypeSize */
        sizeof(INTERNAL_TR69C_CONFIG_TLS_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        28,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        37,

        /* ChildrenCounter */
        6,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"0",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"CaCertRequired",

        /* RealName */
        (INT8*)"CaCertRequired",

        /* Description */
        (INT8*)"CA Certificate must be used for tls function.",

        /* ParaType */
        INTERNAL_DATA_TYPE_BOOLEAN,

        /* CType */
        INTERNAL_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_CONFIG_TLS_T,CaCertRequired),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        36,

        /* SiblingIndex */
        38,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertBoolToString,

        /* ToStructFunc */
        DmConvertStringToBool
    },

    {
        /* Name */
        (INT8*)"Ciphers",

        /* RealName */
        (INT8*)"Ciphers",

        /* Description */
        (INT8*)"Ciphers used in TLS connection.",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_CONFIG_TLS_T,Ciphers),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_CONFIG_TLS_CIPHERS_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_CONFIG_TLS_T,CiphersCnt),

        /* ParentIndex */
        36,

        /* SiblingIndex */
        39,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"64",

        /* DefaultValue */
        (INT8*)"RSA:DES:SHA+RSA:RC4:SAH+MEDIUM",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"CertDir",

        /* RealName */
        (INT8*)"CertDir",

        /* Description */
        (INT8*)"Directory of root certificate file",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_CONFIG_TLS_T,CertDir),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_CONFIG_TLS_CERTDIR_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_CONFIG_TLS_T,CertDirCnt),

        /* ParentIndex */
        36,

        /* SiblingIndex */
        40,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"64",

        /* DefaultValue */
        (INT8*)"/var/cert",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"ClientCertFile",

        /* RealName */
        (INT8*)"ClientCertFile",

        /* Description */
        (INT8*)"Full path name of client certificate file",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_CONFIG_TLS_T,ClientCertFile),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_CONFIG_TLS_CLIENTCERTFILE_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_CONFIG_TLS_T,ClientCertFileCnt),

        /* ParentIndex */
        36,

        /* SiblingIndex */
        41,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"64",

        /* DefaultValue */
        (INT8*)"/var/cert/cpecert.cert",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"ClientPrivateKeyFile",

        /* RealName */
        (INT8*)"ClientPrivateKeyFile",

        /* Description */
        (INT8*)"Full path name of private key file",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_CONFIG_TLS_T,ClientPrivateKeyFile),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_CONFIG_TLS_CLIENTPRIVATEKEYFILE_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_CONFIG_TLS_T,ClientPrivateKeyFileCnt),

        /* ParentIndex */
        36,

        /* SiblingIndex */
        42,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"64",

        /* DefaultValue */
        (INT8*)"/var/cert/cpecert.priv",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"CaCertFile",

        /* RealName */
        (INT8*)"CaCertFile",

        /* Description */
        (INT8*)"The path of saving certificate file",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_CONFIG_TLS_T,CaCertFile),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_CONFIG_TLS_CACERTFILE_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_CONFIG_TLS_T,CaCertFileCnt),

        /* ParentIndex */
        36,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"64",

        /* DefaultValue */
        (INT8*)"/var/cert/acscert.cacert",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"Internal.Tr69c.AcsConfig.",

        /* RealName */
        (INT8*)"AcsConfig",

        /* Description */
        (INT8*)"ACS configuration",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_T,AcsConfig),

        /* CTypeSize */
        sizeof(INTERNAL_TR69C_ACSCONFIG_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        26,

        /* SiblingIndex */
        46,

        /* FirstChildIndex */
        44,

        /* ChildrenCounter */
        2,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"0",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"lastConnectedURL",

        /* RealName */
        (INT8*)"lastConnectedURL",

        /* Description */
        (INT8*)"Last time connected ACS URL",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_ACSCONFIG_T,lastConnectedURL),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_ACSCONFIG_LASTCONNECTEDURL_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_ACSCONFIG_T,lastConnectedURLCnt),

        /* ParentIndex */
        43,

        /* SiblingIndex */
        45,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"256",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"BoundIfName",

        /* RealName */
        (INT8*)"BoundIfName",

        /* Description */
        (INT8*)"Bound interface Name",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_ACSCONFIG_T,BoundIfName),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_ACSCONFIG_BOUNDIFNAME_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_ACSCONFIG_T,BoundIfNameCnt),

        /* ParentIndex */
        43,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"256",

        /* DefaultValue */
        (INT8*)"LAN",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"Internal.Tr69c.GWStateData.",

        /* RealName */
        (INT8*)"GWStateData",

        /* Description */
        (INT8*)"Gateway state info",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_T,GWStateData),

        /* CTypeSize */
        sizeof(INTERNAL_TR69C_GWSTATEDATA_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        26,

        /* SiblingIndex */
        60,

        /* FirstChildIndex */
        47,

        /* ChildrenCounter */
        13,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"0",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"rebootCommandKey",

        /* RealName */
        (INT8*)"rebootCommandKey",

        /* Description */
        (INT8*)"Command key value in reboot RPC",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_GWSTATEDATA_T,rebootCommandKey),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_GWSTATEDATA_REBOOTCOMMANDKEY_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_GWSTATEDATA_T,rebootCommandKeyCnt),

        /* ParentIndex */
        46,

        /* SiblingIndex */
        48,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"33",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"downloadCommandKey",

        /* RealName */
        (INT8*)"downloadCommandKey",

        /* Description */
        (INT8*)"Command key value in download RPC",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_GWSTATEDATA_T,downloadCommandKey),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_GWSTATEDATA_DOWNLOADCOMMANDKEY_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_GWSTATEDATA_T,downloadCommandKeyCnt),

        /* ParentIndex */
        46,

        /* SiblingIndex */
        49,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"33",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"newParameterKey",

        /* RealName */
        (INT8*)"newParameterKey",

        /* Description */
        (INT8*)"the pending key",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_GWSTATEDATA_T,newParameterKey),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_GWSTATEDATA_NEWPARAMETERKEY_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_GWSTATEDATA_T,newParameterKeyCnt),

        /* ParentIndex */
        46,

        /* SiblingIndex */
        50,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"33",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"contactedState",

        /* RealName */
        (INT8*)"contactedState",

        /* Description */
        (INT8*)"tr69c inform state",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_GWSTATEDATA_T,contactedState),

        /* CTypeSize */
        sizeof(INT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        46,

        /* SiblingIndex */
        51,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertInt32ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"dlFaultStatus",

        /* RealName */
        (INT8*)"dlFaultStatus",

        /* Description */
        (INT8*)"download fault status",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_GWSTATEDATA_T,dlFaultStatus),

        /* CTypeSize */
        sizeof(INT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        46,

        /* SiblingIndex */
        52,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertInt32ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"retryCount",

        /* RealName */
        (INT8*)"retryCount",

        /* Description */
        (INT8*)"retry count when lose connection with ACS",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_GWSTATEDATA_T,retryCount),

        /* CTypeSize */
        sizeof(INT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        46,

        /* SiblingIndex */
        53,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertInt32ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"startDLTime",

        /* RealName */
        (INT8*)"startDLTime",

        /* Description */
        (INT8*)"Start download file time",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_GWSTATEDATA_T,startDLTime),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        46,

        /* SiblingIndex */
        54,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"endDLTime",

        /* RealName */
        (INT8*)"endDLTime",

        /* Description */
        (INT8*)"Download file finish time",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_GWSTATEDATA_T,endDLTime),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        46,

        /* SiblingIndex */
        55,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"dlFaultMsg",

        /* RealName */
        (INT8*)"dlFaultMsg",

        /* Description */
        (INT8*)"Download fault message, will be sent to ACS",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_GWSTATEDATA_T,dlFaultMsg),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_GWSTATEDATA_DLFAULTMSG_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_GWSTATEDATA_T,dlFaultMsgCnt),

        /* ParentIndex */
        46,

        /* SiblingIndex */
        56,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"129",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"informEvCount",

        /* RealName */
        (INT8*)"informEvCount",

        /* Description */
        (INT8*)"Inform event number",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_GWSTATEDATA_T,informEvCount),

        /* CTypeSize */
        sizeof(INT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        46,

        /* SiblingIndex */
        57,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertInt32ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"informEvList",

        /* RealName */
        (INT8*)"informEvList",

        /* Description */
        (INT8*)"Inform event list",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_UINT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_GWSTATEDATA_T,informEvList),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        INTERNAL_TR69C_GWSTATEDATA_INFORMEVLIST_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_GWSTATEDATA_T,informEvListCnt),

        /* ParentIndex */
        46,

        /* SiblingIndex */
        58,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"16",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint8ArrayToString,

        /* ToStructFunc */
        DmConvertStringToUint8Array
    },

    {
        /* Name */
        (INT8*)"dataModel",

        /* RealName */
        (INT8*)"dataModel",

        /* Description */
        (INT8*)"the data model which client communicates with ACS: TR98, TR181",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_GWSTATEDATA_T,dataModel),

        /* CTypeSize */
        sizeof(INT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        46,

        /* SiblingIndex */
        59,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"181",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertInt32ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"cwmpVersion",

        /* RealName */
        (INT8*)"cwmpVersion",

        /* Description */
        (INT8*)"default is v1.2(0x102), but can revert back to 1.0(0x100)",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_GWSTATEDATA_T,cwmpVersion),

        /* CTypeSize */
        sizeof(INT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        46,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"258",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertInt32ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"Internal.Tr69c.TransferList.",

        /* RealName */
        (INT8*)"TransferList",

        /* Description */
        (INT8*)"Transfer list info",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_T,TransferList),

        /* CTypeSize */
        sizeof(INTERNAL_TR69C_TRANSFERLIST_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        26,

        /* SiblingIndex */
        73,

        /* FirstChildIndex */
        61,

        /* ChildrenCounter */
        2,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"0",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"queueEntryCount",

        /* RealName */
        (INT8*)"queueEntryCount",

        /* Description */
        (INT8*)"Queued transfer count",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_UINT16,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_TRANSFERLIST_T,queueEntryCount),

        /* CTypeSize */
        sizeof(UINT16),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        60,

        /* SiblingIndex */
        62,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint16ToString,

        /* ToStructFunc */
        DmConvertStringToUint16
    },

    {
        /* Name */
        (INT8*)"Internal.Tr69c.TransferList.Queue.{i}.",

        /* RealName */
        (INT8*)"Queue",

        /* Description */
        (INT8*)"Detail info of queued transfer",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_TRANSFERLIST_T,Queue),

        /* CTypeSize */
        sizeof(INTERNAL_TR69C_TRANSFERLIST_QUEUE_T),

        /* MaxElements */
        INTERNAL_TR69C_TRANSFERLIST_QUEUE_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_TRANSFERLIST_T,Queue_cnt),

        /* ParentIndex */
        60,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        63,

        /* ChildrenCounter */
        10,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"16",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"efileType",

        /* RealName */
        (INT8*)"efileType",

        /* Description */
        (INT8*)"FileType in download/upload RPC",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_TRANSFERLIST_QUEUE_T,efileType),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        62,

        /* SiblingIndex */
        64,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint8ToString,

        /* ToStructFunc */
        DmConvertStringToUint8
    },

    {
        /* Name */
        (INT8*)"commandKey",

        /* RealName */
        (INT8*)"commandKey",

        /* Description */
        (INT8*)"CommandKey in download/upload RPC",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_TRANSFERLIST_QUEUE_T,commandKey),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_TRANSFERLIST_QUEUE_COMMANDKEY_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_TRANSFERLIST_QUEUE_T,commandKeyCnt),

        /* ParentIndex */
        62,

        /* SiblingIndex */
        65,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"33",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"url",

        /* RealName */
        (INT8*)"url",

        /* Description */
        (INT8*)"URL in download/upload RPC",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_TRANSFERLIST_QUEUE_T,url),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_TRANSFERLIST_QUEUE_URL_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_TRANSFERLIST_QUEUE_T,urlCnt),

        /* ParentIndex */
        62,

        /* SiblingIndex */
        66,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"256",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"user",

        /* RealName */
        (INT8*)"user",

        /* Description */
        (INT8*)"Username in download/upload RPC",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_TRANSFERLIST_QUEUE_T,user),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_TRANSFERLIST_QUEUE_USER_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_TRANSFERLIST_QUEUE_T,userCnt),

        /* ParentIndex */
        62,

        /* SiblingIndex */
        67,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"256",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"pwd",

        /* RealName */
        (INT8*)"pwd",

        /* Description */
        (INT8*)"Password in download/upload RPC",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_TRANSFERLIST_QUEUE_T,pwd),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_TRANSFERLIST_QUEUE_PWD_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_TRANSFERLIST_QUEUE_T,pwdCnt),

        /* ParentIndex */
        62,

        /* SiblingIndex */
        68,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"256",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"fileSize",

        /* RealName */
        (INT8*)"fileSize",

        /* Description */
        (INT8*)"FileSize in download/upload RPC",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_TRANSFERLIST_QUEUE_T,fileSize),

        /* CTypeSize */
        sizeof(INT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        62,

        /* SiblingIndex */
        69,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertInt32ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"fileName",

        /* RealName */
        (INT8*)"fileName",

        /* Description */
        (INT8*)"TargetFileName in download/upload RPC",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_TRANSFERLIST_QUEUE_T,fileName),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_TRANSFERLIST_QUEUE_FILENAME_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_TRANSFERLIST_QUEUE_T,fileNameCnt),

        /* ParentIndex */
        62,

        /* SiblingIndex */
        70,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"256",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"delaySec",

        /* RealName */
        (INT8*)"delaySec",

        /* Description */
        (INT8*)"DelaySeconds in download/upload RPC",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_TRANSFERLIST_QUEUE_T,delaySec),

        /* CTypeSize */
        sizeof(INT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        62,

        /* SiblingIndex */
        71,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertInt32ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"state",

        /* RealName */
        (INT8*)"state",

        /* Description */
        (INT8*)"File transfer state: Not Initialized Not Yet Started In Progress Completed Rejected",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_TRANSFERLIST_QUEUE_T,state),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        62,

        /* SiblingIndex */
        72,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint8ToString,

        /* ToStructFunc */
        DmConvertStringToUint8
    },

    {
        /* Name */
        (INT8*)"rpcMethod",

        /* RealName */
        (INT8*)"rpcMethod",

        /* Description */
        (INT8*)"Download or upload RPC",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_TRANSFERLIST_QUEUE_T,rpcMethod),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        62,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint8ToString,

        /* ToStructFunc */
        DmConvertStringToUint8
    },

    {
        /* Name */
        (INT8*)"Internal.Tr69c.DownloadVendorConfigInfo.",

        /* RealName */
        (INT8*)"DownloadVendorConfigInfo",

        /* Description */
        (INT8*)"Download vendor config info. Not used now.",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_T,DownloadVendorConfigInfo),

        /* CTypeSize */
        sizeof(INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        26,

        /* SiblingIndex */
        78,

        /* FirstChildIndex */
        74,

        /* ChildrenCounter */
        4,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"0",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"name",

        /* RealName */
        (INT8*)"name",

        /* Description */
        (INT8*)"vendor config file name",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_T,name),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_NAME_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_T,nameCnt),

        /* ParentIndex */
        73,

        /* SiblingIndex */
        75,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"64",

        /* DefaultValue */
        (INT8*)"Percello",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"version",

        /* RealName */
        (INT8*)"version",

        /* Description */
        (INT8*)"vendor config file version",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_T,version),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_VERSION_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_T,versionCnt),

        /* ParentIndex */
        73,

        /* SiblingIndex */
        76,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"16",

        /* DefaultValue */
        (INT8*)"1",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"date",

        /* RealName */
        (INT8*)"date",

        /* Description */
        (INT8*)"vendor config file downloading date",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_T,date),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_DATE_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_T,dateCnt),

        /* ParentIndex */
        73,

        /* SiblingIndex */
        77,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"64",

        /* DefaultValue */
        (INT8*)"40553",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"description",

        /* RealName */
        (INT8*)"description",

        /* Description */
        (INT8*)"vendor config file description",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_T,description),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_DESCRIPTION_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_T,descriptionCnt),

        /* ParentIndex */
        73,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"256",

        /* DefaultValue */
        (INT8*)"Test in Dalian",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"Internal.Tr69c.SoapNameSpaces.",

        /* RealName */
        (INT8*)"SoapNameSpaces",

        /* Description */
        (INT8*)"Namespaces for soap messages",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_T,SoapNameSpaces),

        /* CTypeSize */
        sizeof(INTERNAL_TR69C_SOAPNAMESPACES_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        26,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        79,

        /* ChildrenCounter */
        5,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"SoapEnv",

        /* RealName */
        (INT8*)"SoapEnv",

        /* Description */
        (INT8*)"namespaces for SoapEnv",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_SOAPNAMESPACES_T,SoapEnv),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_SOAPNAMESPACES_SOAPENV_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_SOAPNAMESPACES_T,SoapEnvCnt),

        /* ParentIndex */
        78,

        /* SiblingIndex */
        80,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"128",

        /* DefaultValue */
        (INT8*)"http://schemas.xmlsoap.org/soap/envelope/",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"SoapEnc",

        /* RealName */
        (INT8*)"SoapEnc",

        /* Description */
        (INT8*)"namespaces for SoapEnc",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_SOAPNAMESPACES_T,SoapEnc),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_SOAPNAMESPACES_SOAPENC_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_SOAPNAMESPACES_T,SoapEncCnt),

        /* ParentIndex */
        78,

        /* SiblingIndex */
        81,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"128",

        /* DefaultValue */
        (INT8*)"http://schemas.xmlsoap.org/soap/encoding/",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"Xsd",

        /* RealName */
        (INT8*)"Xsd",

        /* Description */
        (INT8*)"namespaces for Xsd",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_SOAPNAMESPACES_T,Xsd),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_SOAPNAMESPACES_XSD_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_SOAPNAMESPACES_T,XsdCnt),

        /* ParentIndex */
        78,

        /* SiblingIndex */
        82,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"128",

        /* DefaultValue */
        (INT8*)"http://www.w3.org/2001/XMLSchema",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"Xsi",

        /* RealName */
        (INT8*)"Xsi",

        /* Description */
        (INT8*)"namespaces for Xsi",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_SOAPNAMESPACES_T,Xsi),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_SOAPNAMESPACES_XSI_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_SOAPNAMESPACES_T,XsiCnt),

        /* ParentIndex */
        78,

        /* SiblingIndex */
        83,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"128",

        /* DefaultValue */
        (INT8*)"http://www.w3.org/2001/XMLSchema-instance",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"Cwmp",

        /* RealName */
        (INT8*)"Cwmp",

        /* Description */
        (INT8*)"namespaces for Cwmp",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TR69C_SOAPNAMESPACES_T,Cwmp),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TR69C_SOAPNAMESPACES_CWMP_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TR69C_SOAPNAMESPACES_T,CwmpCnt),

        /* ParentIndex */
        78,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"128",

        /* DefaultValue */
        (INT8*)"urn:dslforum-org:cwmp-1-1",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        1,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"Internal.Fm.",

        /* RealName */
        (INT8*)"Fm",

        /* Description */
        (INT8*)"Fault management configure",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_T,Fm),

        /* CTypeSize */
        sizeof(INTERNAL_FM_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        0,

        /* SiblingIndex */
        94,

        /* FirstChildIndex */
        85,

        /* ChildrenCounter */
        3,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"0",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"RecoverDelay",

        /* RealName */
        (INT8*)"RecoverDelay",

        /* Description */
        (INT8*)"the interval to take reboot, 30s by default",

        /* ParaType */
        INTERNAL_DATA_TYPE_INT,

        /* CType */
        INTERNAL_C_TYPE_INT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_FM_T,RecoverDelay),

        /* CTypeSize */
        sizeof(INT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        84,

        /* SiblingIndex */
        86,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"30",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertInt32ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"Internal.Fm.DeviceLog.",

        /* RealName */
        (INT8*)"DeviceLog",

        /* Description */
        (INT8*)"DeviceLog configure",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_FM_T,DeviceLog),

        /* CTypeSize */
        sizeof(INTERNAL_FM_DEVICELOG_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        84,

        /* SiblingIndex */
        90,

        /* FirstChildIndex */
        87,

        /* ChildrenCounter */
        3,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"FilePath",

        /* RealName */
        (INT8*)"FilePath",

        /* Description */
        (INT8*)"DeviceLogFile Location",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_FM_DEVICELOG_T,FilePath),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_FM_DEVICELOG_FILEPATH_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_FM_DEVICELOG_T,FilePathCnt),

        /* ParentIndex */
        86,

        /* SiblingIndex */
        88,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"128",

        /* DefaultValue */
        (INT8*)"/store/Devicelog",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"FileSize",

        /* RealName */
        (INT8*)"FileSize",

        /* Description */
        (INT8*)"the max bytes of DeviceLog",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_FM_DEVICELOG_T,FileSize),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        86,

        /* SiblingIndex */
        89,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"1048576",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"PercentToDrop",

        /* RealName */
        (INT8*)"PercentToDrop",

        /* Description */
        (INT8*)"the percent to drop when DeviceLog reach the max size.",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_FM_DEVICELOG_T,PercentToDrop),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        86,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"[20:50]",

        /* DefaultValue */
        (INT8*)"20",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"Internal.Fm.Tcpdump.",

        /* RealName */
        (INT8*)"Tcpdump",

        /* Description */
        (INT8*)"Tcpdump configuration",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_FM_T,Tcpdump),

        /* CTypeSize */
        sizeof(INTERNAL_FM_TCPDUMP_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        84,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        91,

        /* ChildrenCounter */
        3,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"0",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"TcpdumpPeriod",

        /* RealName */
        (INT8*)"TcpdumpPeriod",

        /* Description */
        (INT8*)"The interval when to check the dump-file size, 300s by default",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_FM_TCPDUMP_T,TcpdumpPeriod),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        90,

        /* SiblingIndex */
        92,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"300",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"TcpdumpSize",

        /* RealName */
        (INT8*)"TcpdumpSize",

        /* Description */
        (INT8*)"The size when to dump packets to file, 1Mbytes by default",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_FM_TCPDUMP_T,TcpdumpSize),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        90,

        /* SiblingIndex */
        93,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"1",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"TcpdumpDir",

        /* RealName */
        (INT8*)"TcpdumpDir",

        /* Description */
        (INT8*)"The directory where to dump packets",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_FM_TCPDUMP_T,TcpdumpDir),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_FM_TCPDUMP_TCPDUMPDIR_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_FM_TCPDUMP_T,TcpdumpDirCnt),

        /* ParentIndex */
        90,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"128",

        /* DefaultValue */
        (INT8*)"/tmp/tcpdump/",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"Internal.Tm.",

        /* RealName */
        (INT8*)"Tm",

        /* Description */
        (INT8*)"TM control info",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_T,Tm),

        /* CTypeSize */
        sizeof(INTERNAL_TM_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        0,

        /* SiblingIndex */
        115,

        /* FirstChildIndex */
        95,

        /* ChildrenCounter */
        7,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"0",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"ShortStatusInterval",

        /* RealName */
        (INT8*)"ShortStatusInterval",

        /* Description */
        (INT8*)"Timeout (milliseconds) for status timer before sync success",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TM_T,ShortStatusInterval),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        94,

        /* SiblingIndex */
        96,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"60000",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"LongStatusInterval",

        /* RealName */
        (INT8*)"LongStatusInterval",

        /* Description */
        (INT8*)"Timeout (milliseconds) for status timer after sync success",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TM_T,LongStatusInterval),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        94,

        /* SiblingIndex */
        97,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"600000",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"SaveTimeInterval",

        /* RealName */
        (INT8*)"SaveTimeInterval",

        /* Description */
        (INT8*)"Timeout (milliseconds) for saving current time timer",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TM_T,SaveTimeInterval),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        94,

        /* SiblingIndex */
        98,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"3600000",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"AttemptInterval",

        /* RealName */
        (INT8*)"AttemptInterval",

        /* Description */
        (INT8*)"Timeout (milliseconds) for sync attempt timer",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TM_T,AttemptInterval),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        94,

        /* SiblingIndex */
        99,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"60000",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"Internal.Tm.Ntpd.",

        /* RealName */
        (INT8*)"Ntpd",

        /* Description */
        (INT8*)"ntpd control info",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TM_T,Ntpd),

        /* CTypeSize */
        sizeof(INTERNAL_TM_NTPD_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        94,

        /* SiblingIndex */
        105,

        /* FirstChildIndex */
        100,

        /* ChildrenCounter */
        5,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"0",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"NtpdatePath",

        /* RealName */
        (INT8*)"NtpdatePath",

        /* Description */
        (INT8*)"Path for ntpdate binary file",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TM_NTPD_T,NtpdatePath),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TM_NTPD_NTPDATEPATH_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TM_NTPD_T,NtpdatePathCnt),

        /* ParentIndex */
        99,

        /* SiblingIndex */
        101,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"256",

        /* DefaultValue */
        (INT8*)"ntpdate",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"NtpqPath",

        /* RealName */
        (INT8*)"NtpqPath",

        /* Description */
        (INT8*)"Path for ntpq binary file",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TM_NTPD_T,NtpqPath),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TM_NTPD_NTPQPATH_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TM_NTPD_T,NtpqPathCnt),

        /* ParentIndex */
        99,

        /* SiblingIndex */
        102,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"256",

        /* DefaultValue */
        (INT8*)"ntpq",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"NtpdPath",

        /* RealName */
        (INT8*)"NtpdPath",

        /* Description */
        (INT8*)"Path for ntpd binary file",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TM_NTPD_T,NtpdPath),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TM_NTPD_NTPDPATH_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TM_NTPD_T,NtpdPathCnt),

        /* ParentIndex */
        99,

        /* SiblingIndex */
        103,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"256",

        /* DefaultValue */
        (INT8*)"ntpd",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"NtpdConfigPath",

        /* RealName */
        (INT8*)"NtpdConfigPath",

        /* Description */
        (INT8*)"Path for ntpd config file",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TM_NTPD_T,NtpdConfigPath),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TM_NTPD_NTPDCONFIGPATH_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TM_NTPD_T,NtpdConfigPathCnt),

        /* ParentIndex */
        99,

        /* SiblingIndex */
        104,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"256",

        /* DefaultValue */
        (INT8*)"/tmp/ntp.conf",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"NtpdPidPath",

        /* RealName */
        (INT8*)"NtpdPidPath",

        /* Description */
        (INT8*)"Path for ntpd pid file",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TM_NTPD_T,NtpdPidPath),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TM_NTPD_NTPDPIDPATH_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TM_NTPD_T,NtpdPidPathCnt),

        /* ParentIndex */
        99,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"256",

        /* DefaultValue */
        (INT8*)"/tmp/ntp.pid",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"Internal.Tm.Bntpd.",

        /* RealName */
        (INT8*)"Bntpd",

        /* Description */
        (INT8*)"bntpd control info",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TM_T,Bntpd),

        /* CTypeSize */
        sizeof(INTERNAL_TM_BNTPD_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        94,

        /* SiblingIndex */
        109,

        /* FirstChildIndex */
        106,

        /* ChildrenCounter */
        3,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"0",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"BntpdPath",

        /* RealName */
        (INT8*)"BntpdPath",

        /* Description */
        (INT8*)"Path for bntpd binary file",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TM_BNTPD_T,BntpdPath),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TM_BNTPD_BNTPDPATH_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TM_BNTPD_T,BntpdPathCnt),

        /* ParentIndex */
        105,

        /* SiblingIndex */
        107,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"256",

        /* DefaultValue */
        (INT8*)"/work/command/bntpd",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"BntpdCnfPath",

        /* RealName */
        (INT8*)"BntpdCnfPath",

        /* Description */
        (INT8*)"Path for bntpd config file",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TM_BNTPD_T,BntpdCnfPath),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TM_BNTPD_BNTPDCNFPATH_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TM_BNTPD_T,BntpdCnfPathCnt),

        /* ParentIndex */
        105,

        /* SiblingIndex */
        108,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"256",

        /* DefaultValue */
        (INT8*)"/tmp/bntpd.conf",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"BntpdStatusPath",

        /* RealName */
        (INT8*)"BntpdStatusPath",

        /* Description */
        (INT8*)"Path for bntpd status file",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TM_BNTPD_T,BntpdStatusPath),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TM_BNTPD_BNTPDSTATUSPATH_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TM_BNTPD_T,BntpdStatusPathCnt),

        /* ParentIndex */
        105,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"256",

        /* DefaultValue */
        (INT8*)"/tmp/bntpd.status",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"Internal.Tm.Ptp4l.",

        /* RealName */
        (INT8*)"Ptp4l",

        /* Description */
        (INT8*)"ptp4l control info",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TM_T,Ptp4l),

        /* CTypeSize */
        sizeof(INTERNAL_TM_PTP4L_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        94,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        110,

        /* ChildrenCounter */
        5,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"0",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"Ptp4lPath",

        /* RealName */
        (INT8*)"Ptp4lPath",

        /* Description */
        (INT8*)"Path for ptp4l binary file",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TM_PTP4L_T,Ptp4lPath),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TM_PTP4L_PTP4LPATH_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TM_PTP4L_T,Ptp4lPathCnt),

        /* ParentIndex */
        109,

        /* SiblingIndex */
        111,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"256",

        /* DefaultValue */
        (INT8*)"/work/command/ptp4l",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"Ptp4lCnfPath",

        /* RealName */
        (INT8*)"Ptp4lCnfPath",

        /* Description */
        (INT8*)"Path for ptp4l config file",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TM_PTP4L_T,Ptp4lCnfPath),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TM_PTP4L_PTP4LCNFPATH_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TM_PTP4L_T,Ptp4lCnfPathCnt),

        /* ParentIndex */
        109,

        /* SiblingIndex */
        112,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"256",

        /* DefaultValue */
        (INT8*)"/work/command/default.cfg",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"Ptp4lLogging",

        /* RealName */
        (INT8*)"Ptp4lLogging",

        /* Description */
        (INT8*)"enable ptp4l log",

        /* ParaType */
        INTERNAL_DATA_TYPE_BOOLEAN,

        /* CType */
        INTERNAL_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TM_PTP4L_T,Ptp4lLogging),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        109,

        /* SiblingIndex */
        113,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"1",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertBoolToString,

        /* ToStructFunc */
        DmConvertStringToBool
    },

    {
        /* Name */
        (INT8*)"Phc2SysPath",

        /* RealName */
        (INT8*)"Phc2SysPath",

        /* Description */
        (INT8*)"Path for phc2sys binary file",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TM_PTP4L_T,Phc2SysPath),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TM_PTP4L_PHC2SYSPATH_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TM_PTP4L_T,Phc2SysPathCnt),

        /* ParentIndex */
        109,

        /* SiblingIndex */
        114,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"256",

        /* DefaultValue */
        (INT8*)"/work/command/phc2sys",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"PmcPath",

        /* RealName */
        (INT8*)"PmcPath",

        /* Description */
        (INT8*)"Path for pmc binary file",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_TM_PTP4L_T,PmcPath),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_TM_PTP4L_PMCPATH_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_TM_PTP4L_T,PmcPathCnt),

        /* ParentIndex */
        109,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"256",

        /* DefaultValue */
        (INT8*)"/work/command/pmc",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"Internal.GPS.",

        /* RealName */
        (INT8*)"GPS",

        /* Description */
        (INT8*)"GPS process control info",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_T,GPS),

        /* CTypeSize */
        sizeof(INTERNAL_GPS_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        0,

        /* SiblingIndex */
        122,

        /* FirstChildIndex */
        116,

        /* ChildrenCounter */
        6,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"0",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"gpsClientLibPath",

        /* RealName */
        (INT8*)"gpsClientLibPath",

        /* Description */
        (INT8*)"Path of GPS client lib",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_GPS_T,gpsClientLibPath),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_GPS_GPSCLIENTLIBPATH_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_GPS_T,gpsClientLibPathCnt),

        /* ParentIndex */
        115,

        /* SiblingIndex */
        117,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"256",

        /* DefaultValue */
        (INT8*)"/work/libs/libgpsc.so",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"gpsdPath",

        /* RealName */
        (INT8*)"gpsdPath",

        /* Description */
        (INT8*)"Path of GPS deamon",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_GPS_T,gpsdPath),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_GPS_GPSDPATH_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_GPS_T,gpsdPathCnt),

        /* ParentIndex */
        115,

        /* SiblingIndex */
        118,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"256",

        /* DefaultValue */
        (INT8*)"/work/oam/gpsd",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"ttyName",

        /* RealName */
        (INT8*)"ttyName",

        /* Description */
        (INT8*)"Tty device file name",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_GPS_T,ttyName),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_GPS_TTYNAME_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_GPS_T,ttyNameCnt),

        /* ParentIndex */
        115,

        /* SiblingIndex */
        119,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"256",

        /* DefaultValue */
        (INT8*)"/dev/ttyS1",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"recvFilter",

        /* RealName */
        (INT8*)"recvFilter",

        /* Description */
        (INT8*)"receive data filter 0: all  1:  only GPRMC  2:GPRMC and GPGGA",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32_ENUM,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_GPS_T,recvFilter),

        /* CTypeSize */
        sizeof(UINT32_ENUM),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        115,

        /* SiblingIndex */
        120,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"ALL,ONLY_GPRMC,GPRMC_GPGGA",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertEnumToString,

        /* ToStructFunc */
        DmConvertStringToEnum
    },

    {
        /* Name */
        (INT8*)"timeMaxDeviation",

        /* RealName */
        (INT8*)"timeMaxDeviation",

        /* Description */
        (INT8*)"Max deviation when updating time (milliseconds)",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_GPS_T,timeMaxDeviation),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        115,

        /* SiblingIndex */
        121,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"1000",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"locationMinOffset",

        /* RealName */
        (INT8*)"locationMinOffset",

        /* Description */
        (INT8*)"Min offset when updating location  ( seconds)",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_GPS_T,locationMinOffset),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        115,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"50",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"Internal.Sm.",

        /* RealName */
        (INT8*)"Sm",

        /* Description */
        (INT8*)"SM configuration",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_T,Sm),

        /* CTypeSize */
        sizeof(INTERNAL_SM_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        0,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        123,

        /* ChildrenCounter */
        4,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"CertPoolPath",

        /* RealName */
        (INT8*)"CertPoolPath",

        /* Description */
        (INT8*)"the location of all certificates",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_T,CertPoolPath),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_SM_CERTPOOLPATH_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_SM_T,CertPoolPathCnt),

        /* ParentIndex */
        122,

        /* SiblingIndex */
        124,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"256",

        /* DefaultValue */
        (INT8*)"/store/certs/",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"StrokeLogging",

        /* RealName */
        (INT8*)"StrokeLogging",

        /* Description */
        (INT8*)"Output stroke message to perview",

        /* ParaType */
        INTERNAL_DATA_TYPE_BOOLEAN,

        /* CType */
        INTERNAL_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_T,StrokeLogging),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        122,

        /* SiblingIndex */
        125,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"1",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertBoolToString,

        /* ToStructFunc */
        DmConvertStringToBool
    },

    {
        /* Name */
        (INT8*)"Internal.Sm.Strongswan.",

        /* RealName */
        (INT8*)"Strongswan",

        /* Description */
        (INT8*)"Strongswan configuration",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_T,Strongswan),

        /* CTypeSize */
        sizeof(INTERNAL_SM_STRONGSWAN_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        122,

        /* SiblingIndex */
        171,

        /* FirstChildIndex */
        126,

        /* ChildrenCounter */
        6,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"TarFilePath",

        /* RealName */
        (INT8*)"TarFilePath",

        /* Description */
        (INT8*)"strongswan.tgz path",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_T,TarFilePath),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_SM_STRONGSWAN_TARFILEPATH_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_T,TarFilePathCnt),

        /* ParentIndex */
        125,

        /* SiblingIndex */
        127,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"128",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"RunningPath",

        /* RealName */
        (INT8*)"RunningPath",

        /* Description */
        (INT8*)"strongswan running path",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_T,RunningPath),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_SM_STRONGSWAN_RUNNINGPATH_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_T,RunningPathCnt),

        /* ParentIndex */
        125,

        /* SiblingIndex */
        128,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"128",

        /* DefaultValue */
        (INT8*)"/work/",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"CheckMaxCounter",

        /* RealName */
        (INT8*)"CheckMaxCounter",

        /* Description */
        (INT8*)"the max counter to Check Ipsec status",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_T,CheckMaxCounter),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        125,

        /* SiblingIndex */
        129,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"6",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"CheckInterval",

        /* RealName */
        (INT8*)"CheckInterval",

        /* Description */
        (INT8*)"the interval to Check Ipsec status, 10s by default",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_T,CheckInterval),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        125,

        /* SiblingIndex */
        130,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"10",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"Internal.Sm.Strongswan.TunnelCfg.",

        /* RealName */
        (INT8*)"TunnelCfg",

        /* Description */
        (INT8*)"strongswan configuration for tunnel",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_T,TunnelCfg),

        /* CTypeSize */
        sizeof(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        125,

        /* SiblingIndex */
        168,

        /* FirstChildIndex */
        131,

        /* ChildrenCounter */
        34,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"DumpIkeKey",

        /* RealName */
        (INT8*)"DumpIkeKey",

        /* Description */
        (INT8*)"Enable or disable to dump IKE keys",

        /* ParaType */
        INTERNAL_DATA_TYPE_BOOLEAN,

        /* CType */
        INTERNAL_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,DumpIkeKey),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        132,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertBoolToString,

        /* ToStructFunc */
        DmConvertStringToBool
    },

    {
        /* Name */
        (INT8*)"IkeKeyDumpPath",

        /* RealName */
        (INT8*)"IkeKeyDumpPath",

        /* Description */
        (INT8*)"where to save IKE keys",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,IkeKeyDumpPath),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_SM_STRONGSWAN_TUNNELCFG_IKEKEYDUMPPATH_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,IkeKeyDumpPathCnt),

        /* ParentIndex */
        130,

        /* SiblingIndex */
        133,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"128",

        /* DefaultValue */
        (INT8*)"/tmp/IkeKeys.txt",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"RetryMaxCounter",

        /* RealName */
        (INT8*)"RetryMaxCounter",

        /* Description */
        (INT8*)"How many retries to take when tunnel deleted for some error.",

        /* ParaType */
        INTERNAL_DATA_TYPE_INT,

        /* CType */
        INTERNAL_C_TYPE_INT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,RetryMaxCounter),

        /* CTypeSize */
        sizeof(INT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        134,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"10",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertInt32ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"RetryInterval",

        /* RealName */
        (INT8*)"RetryInterval",

        /* Description */
        (INT8*)"The interval to take next try when tunnel deleted for some error, 10s by default.",

        /* ParaType */
        INTERNAL_DATA_TYPE_INT,

        /* CType */
        INTERNAL_C_TYPE_INT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,RetryInterval),

        /* CTypeSize */
        sizeof(INT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        135,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"10",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertInt32ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"RebootAfterMaxRetries",

        /* RealName */
        (INT8*)"RebootAfterMaxRetries",

        /* Description */
        (INT8*)"If to take system reboot after max retries reached.",

        /* ParaType */
        INTERNAL_DATA_TYPE_BOOLEAN,

        /* CType */
        INTERNAL_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,RebootAfterMaxRetries),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        136,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"1",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertBoolToString,

        /* ToStructFunc */
        DmConvertStringToBool
    },

    {
        /* Name */
        (INT8*)"EAPType",

        /* RealName */
        (INT8*)"EAPType",

        /* Description */
        (INT8*)"Extensible Authentication Protocol type, such as AKA",

        /* ParaType */
        INTERNAL_DATA_TYPE_INT,

        /* CType */
        INTERNAL_C_TYPE_INT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,EAPType),

        /* CTypeSize */
        sizeof(INT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        137,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertInt32ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"EAPVendor",

        /* RealName */
        (INT8*)"EAPVendor",

        /* Description */
        (INT8*)"Extensible Authentication Protocol vendor",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,EAPVendor),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        138,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"EAPIdentity",

        /* RealName */
        (INT8*)"EAPIdentity",

        /* Description */
        (INT8*)"The identity the client uses to reply to an EAP Identity request.",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,EAPIdentity),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_SM_STRONGSWAN_TUNNELCFG_EAPIDENTITY_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,EAPIdentityCnt),

        /* ParentIndex */
        130,

        /* SiblingIndex */
        139,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"128",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"AAAIdentity",

        /* RealName */
        (INT8*)"AAAIdentity",

        /* Description */
        (INT8*)"The identity of the AAA backend used during IKEv2 EAP authentication.",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,AAAIdentity),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_SM_STRONGSWAN_TUNNELCFG_AAAIDENTITY_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,AAAIdentityCnt),

        /* ParentIndex */
        130,

        /* SiblingIndex */
        140,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"128",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"XauthIdentity",

        /* RealName */
        (INT8*)"XauthIdentity",

        /* Description */
        (INT8*)"defines the identity/username the client uses to reply to an XAuth request. If not defined, the IKEv1 identity will be used as XAuth identity.",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,XauthIdentity),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_SM_STRONGSWAN_TUNNELCFG_XAUTHIDENTITY_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,XauthIdentityCnt),

        /* ParentIndex */
        130,

        /* SiblingIndex */
        141,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"128",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"Mode",

        /* RealName */
        (INT8*)"Mode",

        /* Description */
        (INT8*)"The mode of the connection, we noly support tunnel mode",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32_ENUM,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,Mode),

        /* CTypeSize */
        sizeof(UINT32_ENUM),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        142,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"MODE_NONE,MODE_TRANSPORT,MODE_TUNNEL,MODE_BEET",

        /* DefaultValue */
        (INT8*)"MODE_TUNNEL",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertEnumToString,

        /* ToStructFunc */
        DmConvertStringToEnum
    },

    {
        /* Name */
        (INT8*)"Mobike",

        /* RealName */
        (INT8*)"Mobike",

        /* Description */
        (INT8*)"Enable/disable the IKEv2 MOBIKE protocol defined by RFC 4555.  Enable value is 64, disable value is 0.",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_INT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,Mobike),

        /* CTypeSize */
        sizeof(INT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        143,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertInt32ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"ForceEncap",

        /* RealName */
        (INT8*)"ForceEncap",

        /* Description */
        (INT8*)"Force UDP encapsulation for ESP packets even if no NAT situation is detected. Enable value is 1, disable value is 0.",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_INT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,ForceEncap),

        /* CTypeSize */
        sizeof(INT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        144,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertInt32ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"IPComp",

        /* RealName */
        (INT8*)"IPComp",

        /* Description */
        (INT8*)"Whether IPComp compression of content is proposed on the connection",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_INT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,IPComp),

        /* CTypeSize */
        sizeof(INT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        145,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertInt32ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"Inactivity",

        /* RealName */
        (INT8*)"Inactivity",

        /* Description */
        (INT8*)"Defines the timeout interval, after which a CHILD_SA is closed if it did not send or receive any traffic.",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,Inactivity),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        146,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"InstallPolicy",

        /* RealName */
        (INT8*)"InstallPolicy",

        /* Description */
        (INT8*)"Decides whether IPsec policies are installed in the kernel by the IKEv2 charon daemon for a given connection.",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_INT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,InstallPolicy),

        /* CTypeSize */
        sizeof(INT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        147,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"1",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertInt32ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"ReqID",

        /* RealName */
        (INT8*)"ReqID",

        /* Description */
        (INT8*)"Sets the reqid for a given connection to a pre-configured fixed value",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,ReqID),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        148,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"Unique",

        /* RealName */
        (INT8*)"Unique",

        /* Description */
        (INT8*)"Whether a particular participant ID should be kept unique, with any new (automatically keyed) connection using an ID from a different IP address deemed to replace all old ones using that ID.",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_INT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,Unique),

        /* CTypeSize */
        sizeof(INT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        149,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"1",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertInt32ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"TimeMargin",

        /* RealName */
        (INT8*)"TimeMargin",

        /* Description */
        (INT8*)"How long before connection expiry or keying-channel expiry should attempts to negotiate a replacement begin",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_INT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,TimeMargin),

        /* CTypeSize */
        sizeof(INT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        150,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"300",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertInt32ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"BytesMargin",

        /* RealName */
        (INT8*)"BytesMargin",

        /* Description */
        (INT8*)"How many bytes before IPsec SA expiry (see lifebytes) should attempts to negotiate a replacement begin ",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT64,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,BytesMargin),

        /* CTypeSize */
        sizeof(UINT64),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        151,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"52428800",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint64ToString,

        /* ToStructFunc */
        DmConvertStringToUint64
    },

    {
        /* Name */
        (INT8*)"PacketsLife",

        /* RealName */
        (INT8*)"PacketsLife",

        /* Description */
        (INT8*)"The number of packets transmitted over an IPsec SA before it expires",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT64,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,PacketsLife),

        /* CTypeSize */
        sizeof(UINT64),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        152,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"16000000",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint64ToString,

        /* ToStructFunc */
        DmConvertStringToUint64
    },

    {
        /* Name */
        (INT8*)"PacketsMargin",

        /* RealName */
        (INT8*)"PacketsMargin",

        /* Description */
        (INT8*)"How many packets before IPsec SA expiry (see lifepackets) should attempts to negotiate a replacement begin",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT64,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,PacketsMargin),

        /* CTypeSize */
        sizeof(UINT64),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        153,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"160000",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint64ToString,

        /* ToStructFunc */
        DmConvertStringToUint64
    },

    {
        /* Name */
        (INT8*)"RekeyTries",

        /* RealName */
        (INT8*)"RekeyTries",

        /* Description */
        (INT8*)"How many attempts (a positive integer or %forever) should be made to negotiate a connection, or a replacement for one, before giving up .",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,RekeyTries),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        154,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"1",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"Fuzzy",

        /* RealName */
        (INT8*)"Fuzzy",

        /* Description */
        (INT8*)"Maximum percentage by which marginbytes, marginpackets and margintime should be randomly increased to randomize rekeying intervals",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,Fuzzy),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        155,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"100",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"DPDAction",

        /* RealName */
        (INT8*)"DPDAction",

        /* Description */
        (INT8*)"controls the use of the Dead Peer Detection protocol (DPD, RFC 3706) where R_U_THERE notification messages (IKEv1) or empty INFORMATIONAL messages (IKEv2) are periodically sent in order to check the liveliness of the IPsec peer. The values clear, hold, and restart all activate DPD. If no activity is detected, all connections with a dead peer are stopped and unrouted (clear), put in the hold state (hold) or restarted (restart). The default is none which disables the active sending of DPD messages.",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32_ENUM,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,DPDAction),

        /* CTypeSize */
        sizeof(UINT32_ENUM),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        156,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"DPD_ACTION_NONE,DPD_ACTION_CLEAR,DPD_ACTION_HOLD,DPD_ACTION_RESTART,DPD_ACTION_UNKNOWN",

        /* DefaultValue */
        (INT8*)"DPD_ACTION_NONE",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertEnumToString,

        /* ToStructFunc */
        DmConvertStringToEnum
    },

    {
        /* Name */
        (INT8*)"DPDTimeout",

        /* RealName */
        (INT8*)"DPDTimeout",

        /* Description */
        (INT8*)"defines the timeout interval, after which all connections to a peer are deleted in case of inactivity. This only applies to IKEv1, in IKEv2 the default retransmission timeout applies, as every exchange is used to detect dead peers.",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,DPDTimeout),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        157,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"150",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"CloseAction",

        /* RealName */
        (INT8*)"CloseAction",

        /* Description */
        (INT8*)"defines the action to take if the remote peer unexpectedly closes a CHILD_SA (IKEv2 only, see dpdaction for meaning of values). A closeaction should not be used if the peer uses reauthentication or uniqueids checking, as these events might trigger the defined action when not desired.",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32_ENUM,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,CloseAction),

        /* CTypeSize */
        sizeof(UINT32_ENUM),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        158,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"CLOSE_ACTION_NONE,CLOSE_ACTION_CLEAR,CLOSE_ACTION_HOLD,CLOSE_ACTION_RESTART,CLOSE_ACTION_UNKNOWN",

        /* DefaultValue */
        (INT8*)"CLOSE_ACTION_NONE",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertEnumToString,

        /* ToStructFunc */
        DmConvertStringToEnum
    },

    {
        /* Name */
        (INT8*)"MarkInValue",

        /* RealName */
        (INT8*)"MarkInValue",

        /* Description */
        (INT8*)"Sets an XFRM mark in the inbound IPsec SA and policy.",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,MarkInValue),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        159,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"MarkInMask",

        /* RealName */
        (INT8*)"MarkInMask",

        /* Description */
        (INT8*)"Bits which are zero in the <mask> must be zero in the <value> too",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,MarkInMask),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        160,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"MarkOutValue",

        /* RealName */
        (INT8*)"MarkOutValue",

        /* Description */
        (INT8*)"Sets an XFRM mark in the outbound IPsec SA and policy.",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,MarkOutValue),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        161,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"MarkOutMask",

        /* RealName */
        (INT8*)"MarkOutMask",

        /* Description */
        (INT8*)"Bits which are zero in the <mask> must be zero in the <value> too",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,MarkOutMask),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        162,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"Threads",

        /* RealName */
        (INT8*)"Threads",

        /* Description */
        (INT8*)"Number of worker threads in charon",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,Threads),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        163,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"16",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"IKEFragment",

        /* RealName */
        (INT8*)"IKEFragment",

        /* Description */
        (INT8*)"Maximum size (complete IP datagram size in bytes) of a sent IKE fragment when using standardized IKEv2 fragmentation (use 0 to disable this feature). If specified this limit is used for both IPv4 and IPv6.",

        /* ParaType */
        INTERNAL_DATA_TYPE_INT,

        /* CType */
        INTERNAL_C_TYPE_INT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,IKEFragment),

        /* CTypeSize */
        sizeof(INT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        164,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"[0,576:1500]",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertInt32ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"Internal.Sm.Strongswan.TunnelCfg.Retransmission.",

        /* RealName */
        (INT8*)"Retransmission",

        /* Description */
        (INT8*)"Retransmission timeouts for IKEv2. The following algorithm is used to calculate the timeout: relative timeout = retransmit_timeout * (retransmit_base/100.0) ^ (n-1) Where n is the current retransmission count.",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_T,Retransmission),

        /* CTypeSize */
        sizeof(INTERNAL_SM_STRONGSWAN_TUNNELCFG_RETRANSMISSION_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        130,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        165,

        /* ChildrenCounter */
        3,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"Retransmit_tries",

        /* RealName */
        (INT8*)"Retransmit_tries",

        /* Description */
        (INT8*)"Number of times to retransmit a packet before giving up",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_RETRANSMISSION_T,Retransmit_tries),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        164,

        /* SiblingIndex */
        166,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"2",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"Retransmit_timeout",

        /* RealName */
        (INT8*)"Retransmit_timeout",

        /* Description */
        (INT8*)"Timeout in seconds before sending first retransmit",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_RETRANSMISSION_T,Retransmit_timeout),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        164,

        /* SiblingIndex */
        167,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"4",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"Retransmit_base",

        /* RealName */
        (INT8*)"Retransmit_base",

        /* Description */
        (INT8*)"Base to use for calculating exponential back off",

        /* ParaType */
        INTERNAL_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        INTERNAL_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_TUNNELCFG_RETRANSMISSION_T,Retransmit_base),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        164,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"180",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"Internal.Sm.Strongswan.LogLevel.{i}.",

        /* RealName */
        (INT8*)"LogLevel",

        /* Description */
        (INT8*)"log level used by Strongswan.",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_T,LogLevel),

        /* CTypeSize */
        sizeof(INTERNAL_SM_STRONGSWAN_LOGLEVEL_T),

        /* MaxElements */
        INTERNAL_SM_STRONGSWAN_LOGLEVEL_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_T,LogLevel_cnt),

        /* ParentIndex */
        125,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        169,

        /* ChildrenCounter */
        2,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"18",

        /* DefaultValue */
        (INT8*)"18",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"type",

        /* RealName */
        (INT8*)"type",

        /* Description */
        (INT8*)"Each logging message has a source from which subsystem in the daemon the log came from: app: applications other than daemons. asn: Low-level encoding/decoding (ASN.1, X.509 etc.). cfg: Configuration management and plugins. chd: CHILD_SA/IPsec SA. dmn: Main daemon setup/cleanup/signal handling. enc: Packet encoding/decoding encryption/decryption operations. esp: libipsec library messages. ike: IKE_SA/ISAKMP SA. imc: Integrity Measurement Collector. imv: Integrity Measurement Verifier. job: Jobs queuing/processing and thread pool management. knl: IPsec/Networking kernel interface. lib: libstrongwan library messages. mgr: IKE_SA manager, handling synchronization for IKE_SA access. net: IKE network communication. pts: Platform Trust Service. tls: libtls library messages. tnc: Trusted Network Connect.",

        /* ParaType */
        INTERNAL_DATA_TYPE_STRING,

        /* CType */
        INTERNAL_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_LOGLEVEL_T,type),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        INTERNAL_SM_STRONGSWAN_LOGLEVEL_TYPE_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_LOGLEVEL_T,typeCnt),

        /* ParentIndex */
        168,

        /* SiblingIndex */
        170,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"16",

        /* DefaultValue */
        (INT8*)"[1]=app[2]=asn[3]=cfg[4]=chd[5]=dmn[6]=enc[7]=esp[8]=ike[9]=imc[10]=imv[11]=job[12]=knl[13]=lib[14]=mgr[15]=net[16]=pts[17]=tls[18]=tnc",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertCharArrayToString,

        /* ToStructFunc */
        DmConvertStringToCharArray
    },

    {
        /* Name */
        (INT8*)"level",

        /* RealName */
        (INT8*)"level",

        /* Description */
        (INT8*)"The IKE daemon knows different numerical levels of logging, ranging from -1 to 2: -1: Absolutely silent. 0: Very basic auditing logs, (e.g. SA up/SA down). 1: Generic control flow with errors, a good default to see whats going on. 2: More detailed debugging control flow.",

        /* ParaType */
        INTERNAL_DATA_TYPE_INT,

        /* CType */
        INTERNAL_C_TYPE_INT32,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_STRONGSWAN_LOGLEVEL_T,level),

        /* CTypeSize */
        sizeof(INT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        168,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"[-1:2]",

        /* DefaultValue */
        (INT8*)"[1]=1[2]=1[3]=2[4]=2[5]=1[6]=1[7]=1[8]=1[9]=1[10]=1[11]=1[12]=1[13]=1[14]=1[15]=1[16]=1[17]=1[18]=1",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertInt32ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"Internal.Sm.CMPv2.",

        /* RealName */
        (INT8*)"CMPv2",

        /* Description */
        (INT8*)"CMPv2 configuration",

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        INTERNAL_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_T,CMPv2),

        /* CTypeSize */
        sizeof(INTERNAL_SM_CMPV2_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        122,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        172,

        /* ChildrenCounter */
        1,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"bNotFirstUsed",

        /* RealName */
        (INT8*)"bNotFirstUsed",

        /* Description */
        (INT8*)"Mark CMPv2 used.",

        /* ParaType */
        INTERNAL_DATA_TYPE_BOOLEAN,

        /* CType */
        INTERNAL_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(INTERNAL_SM_CMPV2_T,bNotFirstUsed),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        171,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        -1,

        /* ReadFlag */
        0x2,

        /* WriteFlag */
        0x2,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"0",

        /* supportedXmlAttrs */
        (INT8*)"",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        "",

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* UseHardwareDefault */
        0,

        /* forceActiveNotification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        DmConvertBoolToString,

        /* ToStructFunc */
        DmConvertStringToBool
    },

    {
        /* Name */
        NULL,

        /* RealName */
        NULL,

        /* Description */
        NULL,

        /* ParaType */
        INTERNAL_DATA_TYPE_OBJECT,

        /* CType */
        0,

        /* sizeOffset */
        0,

        /* TypeSize */
        0,

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        -1,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0,

        /* WriteFlag */
        0,

        /* ValueRange */
        NULL,

        /* DefaultValue */
        NULL,

        /* Supported Xml Attributes */
        NULL,

        /* Default Xml Attribute Value */
        NULL,

        /* Xml Attr-Notify Entities */
        NULL,

        /* NodeHandler */
        NULL,

        /* NotifyHandler */
        NULL,

        /* GetHandler */
        NULL,

        /* SetHandler */
        NULL,

        /* AutoSaved */
        0,

        /* UpgradeTransfer */
        0,

        /* ResetTransfer */
        0,

        /* BootAction */
        NULL,

        /* ValidityCheckHandler */
        NULL,

        /* OperatorDefault */
        0,

        /* HardwareDefault */
        0,

        /* Force Active Notification */
        0,

        /* Tr69Password */
        0,

        /* ToStringFunc */
        NULL,

        /* ToStructFunc */
        NULL

    },

};


/* Internal model access list */
DB_ACCESS_NODE_T g_InternalDbAccessList[] =
{
    {"ROOT", 0xffff},
    {"TR069", 0x1},
    {"GUI", 0x2},
    {"SNMP", 0x4},
    {"PS", 0x8},
    {NULL, 0}
};



/* Internal model config node */
DB_CONFIG_T g_InternalConfigNode =
{
    "Internal",        /* DB name */
    "20124",        /* DB version */
    DM_FALSE,        /* if it is extended db model */
    "./Internal.xml",         /* working path */
    "./Internal.xml",         /* loading path */
    "./Internal.xml",         /* backup path */
    "./Internal_mapping.xml",         /* cmfile-mapping path */
    InternalDmPatternRegisterHandler,         /* pattern register function */
    InternalDataValidChecker,          /* default data-valid-check function */
    InternalGetCTypeStr,          /* default get c-type string function */
    InternalGetDataTypeStr,          /* default get data-type string function */
    InternalIsObject,          /* to check if it is object */
    g_InternalDbAccessList,         /* access list */
    NULL         /* libdl.so handler */
};

DM_ATTR_INFO_T g_InternalAttributes[] =
{
    {
        /* attribute name */
        NULL,

        /* attribute description */
        NULL,

        /* created by default */
        0,

        /* attribute ctype */
        0,

        /* attribute ctype size */
        0,

        /* attribute valuerange */
        NULL,

        /* attribute defaultvalue */
        NULL,

        /* attribute validcheck */
        NULL,

        /* attribute tostring */
        NULL,

        /* attribute tostruct */
        NULL

    }

};



/* get Internal model config node */
INT32 InternalDbLoadReq(void *pDlHandler, BOOL8 bDbReset, INT8 *pWorkPath, INT8 *pLoadPath, INT8 *pBackupPath)
{
    return DmLoadDbReq(&g_InternalConfigNode, pDlHandler, bDbReset, pWorkPath, pLoadPath, pBackupPath);
}


/* Internal model data-valid-check function */
BOOL8 InternalDataValidChecker(UINT8 ucParaCType, INT8 *pValueRange, INT8 *pValue)
{
    BOOL8 bRet = DM_FALSE;

    switch(ucParaCType)
    {
        case INTERNAL_C_TYPE_UINT64:
            bRet = DmDataUINT64ValidCheck(pValueRange, pValue);
            break;
        case INTERNAL_C_TYPE_INT32:
            bRet = DmDataINT32ValidCheck(pValueRange, pValue);
            break;
        case INTERNAL_C_TYPE_STRUCT:
            bRet = DM_TRUE;
            break;
        case INTERNAL_C_TYPE_INT8_ARRAY:
            bRet = DmDataStringLengthValidCheck(pValueRange, pValue);
            break;
        case INTERNAL_C_TYPE_UINT8:
            bRet = DmDataUINT8ValidCheck(pValueRange, pValue);
            break;
        case INTERNAL_C_TYPE_BOOL8:
            bRet = DmDataBoolValidCheck(pValueRange, pValue);
            break;
        case INTERNAL_C_TYPE_UINT16:
            bRet = DmDataUINT16ValidCheck(pValueRange, pValue);
            break;
        case INTERNAL_C_TYPE_UINT32_ENUM:
            bRet = DmEnumStringValidCheck(pValueRange, pValue);
            break;
        case INTERNAL_C_TYPE_UINT8_ARRAY:
            bRet = DmDataUINT8ArrayValidCheck(pValueRange, pValue);
            break;
        case INTERNAL_C_TYPE_UINT32:
            bRet = DmDataUINT32ValidCheck(pValueRange, pValue);
            break;
        default:
            break;
    }

    return bRet;
}


/* Internal model c-type string function */
INT8* InternalGetCTypeStr(UINT8 ucParaCType)
{
    INT8 *pTypeStr = "";

    switch(ucParaCType)
    {
        case INTERNAL_C_TYPE_BOOL8:
            pTypeStr = "BOOL8";
            break;
        case INTERNAL_C_TYPE_INT32:
            pTypeStr = "INT32";
            break;
        case INTERNAL_C_TYPE_INT8_ARRAY:
            pTypeStr = "INT8[]";
            break;
        case INTERNAL_C_TYPE_STRUCT:
            pTypeStr = "STRUCT";
            break;
        case INTERNAL_C_TYPE_UINT16:
            pTypeStr = "UINT16";
            break;
        case INTERNAL_C_TYPE_UINT32:
            pTypeStr = "UINT32";
            break;
        case INTERNAL_C_TYPE_UINT32_ENUM:
            pTypeStr = "UINT32_ENUM";
            break;
        case INTERNAL_C_TYPE_UINT64:
            pTypeStr = "UINT64";
            break;
        case INTERNAL_C_TYPE_UINT8:
            pTypeStr = "UINT8";
            break;
        case INTERNAL_C_TYPE_UINT8_ARRAY:
            pTypeStr = "UINT8[]";
            break;
        default:
            break;
    }

    return pTypeStr;
}


/* Internal model data-type string function */
INT8* InternalGetDataTypeStr(UINT8 ucParaType)
{
    INT8 *pTypeStr = "";

    switch(ucParaType)
    {
        case INTERNAL_DATA_TYPE_BASE64:
            pTypeStr = "base64";
            break;
        case INTERNAL_DATA_TYPE_BOOLEAN:
            pTypeStr = "boolean";
            break;
        case INTERNAL_DATA_TYPE_DATETIME:
            pTypeStr = "dateTime";
            break;
        case INTERNAL_DATA_TYPE_HEXBINARY:
            pTypeStr = "hexBinary";
            break;
        case INTERNAL_DATA_TYPE_INT:
            pTypeStr = "int";
            break;
        case INTERNAL_DATA_TYPE_LONG:
            pTypeStr = "long";
            break;
        case INTERNAL_DATA_TYPE_OBJECT:
            pTypeStr = "object";
            break;
        case INTERNAL_DATA_TYPE_STRING:
            pTypeStr = "string";
            break;
        case INTERNAL_DATA_TYPE_UNSIGNEDINT:
            pTypeStr = "unsignedInt";
            break;
        case INTERNAL_DATA_TYPE_UNSIGNEDLONG:
            pTypeStr = "unsignedLong";
            break;
        default:
            break;
    }

    return pTypeStr;
}


/* Internal model object-type-check function */
BOOL8 InternalIsObject(UINT8 ucParaType)
{
    if (ucParaType == INTERNAL_DATA_TYPE_OBJECT)
    {
        return DM_TRUE;
    }
    else
    {
        return DM_FALSE;
    }
}


/* Internal model pattern register function */
BOOL8 InternalDmPatternRegisterHandler()
{
    DM_REG_PATTERN_REQ_T            *pPara;
    BOOL8                           bRtn;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter InternalDmPatternRegisterHandler\n");
    pPara = (DM_REG_PATTERN_REQ_T *)malloc(sizeof(DM_REG_PATTERN_REQ_T));

    if(pPara == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ InternalDmPatternRegisterHandler malloc() failed for pPara\n");
        bRtn = DM_FALSE;
        return bRtn;
    }

    osa_mem_set(pPara,0,sizeof(DM_REG_PATTERN_REQ_T));

    pPara->pucRootName = (INT8 *)"Internal.";
    pPara->pPatternFirstNode = (DM_NODE_INFO_T *)g_InternalModelNodes;
    pPara->lPatternSize = 173;
    pPara->pAttrPatternFirstNode = (DM_ATTR_INFO_T *)g_InternalAttributes;
    bRtn = DmRegisterPatternReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DM register handler failed!\n");
        DM_FREE(pPara);

        return DM_FALSE;
    }

    DM_FREE(pPara);

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave InternalDmPatternRegisterHandler\n");
    return DM_TRUE;
}

