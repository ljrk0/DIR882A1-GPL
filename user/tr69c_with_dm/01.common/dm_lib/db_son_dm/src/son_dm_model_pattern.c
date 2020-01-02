

#include "dm_internal.h"
#include "son_dm_model_def.h"

#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif


/* the declaration of data-valid-check function list */
extern BOOL8 Son_dmDataValidChecker(UINT8 ucParaCType, INT8 *pValueRange, INT8 *pValue);

extern BOOL8 DmDataBoolValidCheck(INT8 *pValueRange, INT8 *pValue);
extern BOOL8 DmDataINT16ValidCheck(INT8 *pValueRange, INT8 *pValue);
extern BOOL8 DmDataINT8ValidCheck(INT8 *pValueRange, INT8 *pValue);
extern BOOL8 DmDataStringLengthValidCheck(INT8 *pValueRange, INT8 *pValue);
extern BOOL8 DmDataUINT16ValidCheck(INT8 *pValueRange, INT8 *pValue);
extern BOOL8 DmDataUINT32ValidCheck(INT8 *pValueRange, INT8 *pValue);
extern BOOL8 DmDataUINT8ArrayValidCheck(INT8 *pValueRange, INT8 *pValue);
extern BOOL8 DmDataUINT8ValidCheck(INT8 *pValueRange, INT8 *pValue);
extern BOOL8 DmEnumStringValidCheck(INT8 *pValueRange, INT8 *pValue);


/* the declaration of SON_DM data-type and c-type str-functions */
extern INT8* Son_dmGetCTypeStr(UINT8 ucParaCType);
extern INT8* Son_dmGetDataTypeStr(UINT8 ucParaType);


/* the declaration of SON_DM model register functions */
extern BOOL8 Son_dmDmPatternRegisterHandler();


/* the declaration of is-object-function */
extern BOOL8 Son_dmIsObject(UINT8 ucParaType);


/* the declaration of convert-fuctions */
extern BOOL8 DmConvertUint32ToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
extern BOOL8 DmConvertStringToUint32(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
extern BOOL8 DmConvertInt16ToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
extern BOOL8 DmConvertStringToInt(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
extern BOOL8 DmConvertCharArrayToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
extern BOOL8 DmConvertStringToCharArray(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
extern BOOL8 DmConvertBoolToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
extern BOOL8 DmConvertStringToBool(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
extern BOOL8 DmConvertUint8ToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
extern BOOL8 DmConvertStringToUint8(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
extern BOOL8 DmConvertEnumToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
extern BOOL8 DmConvertStringToEnum(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
extern BOOL8 DmConvertUint16ToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
extern BOOL8 DmConvertStringToUint16(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
extern BOOL8 DmConvertUint8ArrayToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
extern BOOL8 DmConvertStringToUint8Array(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
extern BOOL8 DmConvertInt8ToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
extern BOOL8 DmConvertStringToInt(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);


DM_NODE_INFO_T g_Son_dmModelNodes[]=
{
    {
        /* Name */
        (INT8*)"SON_DM.",

        /* RealName */
        (INT8*)"SON_DM",

        /* Description */
        (INT8*)"This object contains parameters relating to SON",

        /* ParaType */
        SON_DM_DATA_TYPE_OBJECT,

        /* CType */
        SON_DM_C_TYPE_STRUCT,

        /* CType Offset */
        0,

        /* CTypeSize */
        sizeof(SON_DM_T),

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
        0x9,

        /* WriteFlag */
        0x8,

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
        (INT8*)"Indicates the SON lib version.",

        /* ParaType */
        SON_DM_DATA_TYPE_STRING,

        /* CType */
        SON_DM_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_T,VendorDataModelVersion),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        SON_DM_VENDORDATAMODELVERSION_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(SON_DM_T,VendorDataModelVersionCnt),

        /* ParentIndex */
        0,

        /* SiblingIndex */
        2,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x8,

        /* ValueRange */
        (INT8*)"32",

        /* DefaultValue */
        (INT8*)"19371",

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
        (INT8*)"Trace",

        /* RealName */
        (INT8*)"Trace",

        /* Description */
        (INT8*)"Indicates the trace level. 0: TRACE_INFO, 1: TRACE_WARNING, 2: TRACE_ERROR, 4: TRACE_MESSAGE, 5: TRACE_CRITICAL, 6: TRACE_DISABLE",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_T,Trace),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        0,

        /* SiblingIndex */
        3,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x8,

        /* ValueRange */
        (INT8*)"[0:6]",

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
        DmConvertUint8ToString,

        /* ToStructFunc */
        DmConvertStringToUint8
    },

    {
        /* Name */
        (INT8*)"MaxLinePerFile",

        /* RealName */
        (INT8*)"MaxLinePerFile",

        /* Description */
        (INT8*)"Indicates the max number of lines in one log file.",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT16,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_T,MaxLinePerFile),

        /* CTypeSize */
        sizeof(UINT16),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        0,

        /* SiblingIndex */
        4,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x8,

        /* ValueRange */
        (INT8*)"[0:65535]",

        /* DefaultValue */
        (INT8*)"10000",

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
        DmConvertUint16ToString,

        /* ToStructFunc */
        DmConvertStringToUint16
    },

    {
        /* Name */
        (INT8*)"MaxFile",

        /* RealName */
        (INT8*)"MaxFile",

        /* Description */
        (INT8*)"Indicates the max number of log files.",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT16,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_T,MaxFile),

        /* CTypeSize */
        sizeof(UINT16),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        0,

        /* SiblingIndex */
        5,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x8,

        /* ValueRange */
        (INT8*)"[0:65535]",

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
        DmConvertUint16ToString,

        /* ToStructFunc */
        DmConvertStringToUint16
    },

    {
        /* Name */
        (INT8*)"SON_DM.REM.",

        /* RealName */
        (INT8*)"REM",

        /* Description */
        (INT8*)"This object contains parameters relating to SON REM.",

        /* ParaType */
        SON_DM_DATA_TYPE_OBJECT,

        /* CType */
        SON_DM_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_T,REM),

        /* CTypeSize */
        sizeof(SON_DM_REM_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        0,

        /* SiblingIndex */
        9,

        /* FirstChildIndex */
        6,

        /* ChildrenCounter */
        3,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x8,

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
        (INT8*)"Enabled",

        /* RealName */
        (INT8*)"Enabled",

        /* Description */
        (INT8*)"Indicates whether the periodic REM is enabled.",

        /* ParaType */
        SON_DM_DATA_TYPE_BOOLEAN,

        /* CType */
        SON_DM_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_REM_T,Enabled),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        5,

        /* SiblingIndex */
        7,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

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
        (INT8*)"Interval",

        /* RealName */
        (INT8*)"Interval",

        /* Description */
        (INT8*)"Period interval in second for REM scan. 0 means no more scan is needed",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT16,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_REM_T,Interval),

        /* CTypeSize */
        sizeof(UINT16),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        5,

        /* SiblingIndex */
        8,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:65535]",

        /* DefaultValue */
        (INT8*)"60",

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
        DmConvertUint16ToString,

        /* ToStructFunc */
        DmConvertStringToUint16
    },

    {
        /* Name */
        (INT8*)"Count",

        /* RealName */
        (INT8*)"Count",

        /* Description */
        (INT8*)"Count of REM scan to be requested. 0 means infinite",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT16,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_REM_T,Count),

        /* CTypeSize */
        sizeof(UINT16),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        5,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:65535]",

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
        DmConvertUint16ToString,

        /* ToStructFunc */
        DmConvertStringToUint16
    },

    {
        /* Name */
        (INT8*)"SON_DM.TPM.",

        /* RealName */
        (INT8*)"TPM",

        /* Description */
        (INT8*)"This object contains parameters relating to SON TPM",

        /* ParaType */
        SON_DM_DATA_TYPE_OBJECT,

        /* CType */
        SON_DM_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_T,TPM),

        /* CTypeSize */
        sizeof(SON_DM_TPM_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        0,

        /* SiblingIndex */
        30,

        /* FirstChildIndex */
        10,

        /* ChildrenCounter */
        11,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x8,

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
        (INT8*)"Enabled",

        /* RealName */
        (INT8*)"Enabled",

        /* Description */
        (INT8*)"Indicates whether the FAP supports Tx power management. ",

        /* ParaType */
        SON_DM_DATA_TYPE_BOOLEAN,

        /* CType */
        SON_DM_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_TPM_T,Enabled),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        9,

        /* SiblingIndex */
        11,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

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
        (INT8*)"NonCsgCheck",

        /* RealName */
        (INT8*)"NonCsgCheck",

        /* Description */
        (INT8*)"1 if Pout should be further conditioned when HeNB is CSG. 0 otherwise",

        /* ParaType */
        SON_DM_DATA_TYPE_BOOLEAN,

        /* CType */
        SON_DM_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_TPM_T,NonCsgCheck),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        9,

        /* SiblingIndex */
        12,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

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
        (INT8*)"PowerSettingOffset",

        /* RealName */
        (INT8*)"PowerSettingOffset",

        /* Description */
        (INT8*)"value \"X\" used in co-channel E-UTRA protection set to SONAPI_INVALID_POW_SETTING_OFFSET if not available. Range: 30..70",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_TPM_T,PowerSettingOffset),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        9,

        /* SiblingIndex */
        13,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[30:70]",

        /* DefaultValue */
        (INT8*)"33",

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
        (INT8*)"PollutionMitigationLevel",

        /* RealName */
        (INT8*)"PollutionMitigationLevel",

        /* Description */
        (INT8*)"Level of mitigation that TPM shall apply when mitigating pilot pollution situations, 0: None, 1: Low, 2: Medimum, 3: High",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_TPM_T,PollutionMitigationLevel),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        9,

        /* SiblingIndex */
        14,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:3]",

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
        DmConvertUint8ToString,

        /* ToStructFunc */
        DmConvertStringToUint8
    },

    {
        /* Name */
        (INT8*)"Pini",

        /* RealName */
        (INT8*)"Pini",

        /* Description */
        (INT8*)"Initial power level when the transmission is first-time powered on. If set to SONAPI_INVALID_POWER_OFFSET, SON will use its default value. Range: -50..24",

        /* ParaType */
        SON_DM_DATA_TYPE_INT,

        /* CType */
        SON_DM_C_TYPE_INT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_TPM_T,Pini),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        9,

        /* SiblingIndex */
        15,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[-50:24]",

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
        DmConvertInt8ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"Pmin",

        /* RealName */
        (INT8*)"Pmin",

        /* Description */
        (INT8*)"Absolute lowest power level when the transmission is on. If set to SONAPI_INVALID_POWER_OFFSET, SON will use its default value. Range: -50..24",

        /* ParaType */
        SON_DM_DATA_TYPE_INT,

        /* CType */
        SON_DM_C_TYPE_INT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_TPM_T,Pmin),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        9,

        /* SiblingIndex */
        16,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[-50:24]",

        /* DefaultValue */
        (INT8*)"-10",

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
        DmConvertInt8ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"Pmax",

        /* RealName */
        (INT8*)"Pmax",

        /* Description */
        (INT8*)"Absolute highest power level when the transmission is on. If set to SONAPI_INVALID_POWER_OFFSET, SON will use its default value. Range: -50..24",

        /* ParaType */
        SON_DM_DATA_TYPE_INT,

        /* CType */
        SON_DM_C_TYPE_INT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_TPM_T,Pmax),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        9,

        /* SiblingIndex */
        17,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[-50:24]",

        /* DefaultValue */
        (INT8*)"8",

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
        DmConvertInt8ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"SON_DM.TPM.NCellRscpThresholdTable.{i}.",

        /* RealName */
        (INT8*)"NCellRscpThresholdTable",

        /* Description */
        (INT8*)"Lookup table of tx power conditioning on neighbor cell's RSRP",

        /* ParaType */
        SON_DM_DATA_TYPE_OBJECT,

        /* CType */
        SON_DM_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_TPM_T,NCellRscpThresholdTable),

        /* CTypeSize */
        sizeof(SON_DM_TPM_NCELLRSCPTHRESHOLDTABLE_T),

        /* MaxElements */
        SON_DM_TPM_NCELLRSCPTHRESHOLDTABLE_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(SON_DM_TPM_T,NCellRscpThresholdTable_cnt),

        /* ParentIndex */
        9,

        /* SiblingIndex */
        20,

        /* FirstChildIndex */
        18,

        /* ChildrenCounter */
        2,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"16",

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
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"RSRP",

        /* RealName */
        (INT8*)"RSRP",

        /* Description */
        (INT8*)"Neighbor cell RSRP. Uint: dBm",

        /* ParaType */
        SON_DM_DATA_TYPE_STRING,

        /* CType */
        SON_DM_C_TYPE_INT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_TPM_NCELLRSCPTHRESHOLDTABLE_T,RSRP),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        17,

        /* SiblingIndex */
        19,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"[1]=-60[2]=-70[3]=-80[4]=-90[5]=-125",

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
        DmConvertInt8ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"MaxTxPower",

        /* RealName */
        (INT8*)"MaxTxPower",

        /* Description */
        (INT8*)"Max tx power. Uint: dBm",

        /* ParaType */
        SON_DM_DATA_TYPE_STRING,

        /* CType */
        SON_DM_C_TYPE_INT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_TPM_NCELLRSCPTHRESHOLDTABLE_T,MaxTxPower),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        17,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"[1]=4[2]=5[3]=6[4]=7[5]=8",

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
        DmConvertInt8ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"SON_DM.TPM.LutPpmCorrection.{i}.",

        /* RealName */
        (INT8*)"LutPpmCorrection",

        /* Description */
        (INT8*)"Pilot Pollution Mitigation lookup table",

        /* ParaType */
        SON_DM_DATA_TYPE_OBJECT,

        /* CType */
        SON_DM_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_TPM_T,LutPpmCorrection),

        /* CTypeSize */
        sizeof(SON_DM_TPM_LUTPPMCORRECTION_T),

        /* MaxElements */
        SON_DM_TPM_LUTPPMCORRECTION_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(SON_DM_TPM_T,LutPpmCorrection_cnt),

        /* ParentIndex */
        9,

        /* SiblingIndex */
        24,

        /* FirstChildIndex */
        21,

        /* ChildrenCounter */
        3,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x8,

        /* ValueRange */
        (INT8*)"3",

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
        (INT8*)"PPMLow",

        /* RealName */
        (INT8*)"PPMLow",

        /* Description */
        (INT8*)"Tx power correction when PollutionMitigationLevel is low. Uint: dB",

        /* ParaType */
        SON_DM_DATA_TYPE_STRING,

        /* CType */
        SON_DM_C_TYPE_INT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_TPM_LUTPPMCORRECTION_T,PPMLow),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        20,

        /* SiblingIndex */
        22,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"[1]=20[2]=15[3]=5",

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
        DmConvertInt8ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"PPMMedium",

        /* RealName */
        (INT8*)"PPMMedium",

        /* Description */
        (INT8*)"Tx power correction when PollutionMitigationLevel is medium. Uint: dB",

        /* ParaType */
        SON_DM_DATA_TYPE_STRING,

        /* CType */
        SON_DM_C_TYPE_INT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_TPM_LUTPPMCORRECTION_T,PPMMedium),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        20,

        /* SiblingIndex */
        23,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"[1]=15[2]=10[3]=0",

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
        DmConvertInt8ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"PPMHigh",

        /* RealName */
        (INT8*)"PPMHigh",

        /* Description */
        (INT8*)"Tx power correction when PollutionMitigationLevel is high. Uint: dB",

        /* ParaType */
        SON_DM_DATA_TYPE_STRING,

        /* CType */
        SON_DM_C_TYPE_INT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_TPM_LUTPPMCORRECTION_T,PPMHigh),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        20,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"[1]=10[2]=0[3]=-10",

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
        DmConvertInt8ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"SON_DM.TPM.LutAdjMacroCorrection.{i}.",

        /* RealName */
        (INT8*)"LutAdjMacroCorrection",

        /* Description */
        (INT8*)"Lookup table for adjacent macro cell correction",

        /* ParaType */
        SON_DM_DATA_TYPE_OBJECT,

        /* CType */
        SON_DM_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_TPM_T,LutAdjMacroCorrection),

        /* CTypeSize */
        sizeof(SON_DM_TPM_LUTADJMACROCORRECTION_T),

        /* MaxElements */
        SON_DM_TPM_LUTADJMACROCORRECTION_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(SON_DM_TPM_T,LutAdjMacroCorrection_cnt),

        /* ParentIndex */
        9,

        /* SiblingIndex */
        27,

        /* FirstChildIndex */
        25,

        /* ChildrenCounter */
        2,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"16",

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
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"RSRP",

        /* RealName */
        (INT8*)"RSRP",

        /* Description */
        (INT8*)"Neighbor cell RSRP. Uint: dBm",

        /* ParaType */
        SON_DM_DATA_TYPE_STRING,

        /* CType */
        SON_DM_C_TYPE_INT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_TPM_LUTADJMACROCORRECTION_T,RSRP),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        24,

        /* SiblingIndex */
        26,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"[1]=-50[2]=-80[3]=-100[4]=-120",

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
        DmConvertInt8ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"Correction",

        /* RealName */
        (INT8*)"Correction",

        /* Description */
        (INT8*)"Tx power correction. Uint: dB",

        /* ParaType */
        SON_DM_DATA_TYPE_STRING,

        /* CType */
        SON_DM_C_TYPE_INT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_TPM_LUTADJMACROCORRECTION_T,Correction),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        24,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"[1]=20[2]=10[3]=5[4]=0",

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
        DmConvertInt8ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"SON_DM.TPM.LutCcMacroCorrection.{i}.",

        /* RealName */
        (INT8*)"LutCcMacroCorrection",

        /* Description */
        (INT8*)"Lookup table for co-channel macro cell correction",

        /* ParaType */
        SON_DM_DATA_TYPE_OBJECT,

        /* CType */
        SON_DM_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_TPM_T,LutCcMacroCorrection),

        /* CTypeSize */
        sizeof(SON_DM_TPM_LUTCCMACROCORRECTION_T),

        /* MaxElements */
        SON_DM_TPM_LUTCCMACROCORRECTION_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(SON_DM_TPM_T,LutCcMacroCorrection_cnt),

        /* ParentIndex */
        9,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        28,

        /* ChildrenCounter */
        2,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"16",

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
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"RSRP",

        /* RealName */
        (INT8*)"RSRP",

        /* Description */
        (INT8*)"Neighbor cell RSRP. Uint: dBm",

        /* ParaType */
        SON_DM_DATA_TYPE_STRING,

        /* CType */
        SON_DM_C_TYPE_INT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_TPM_LUTCCMACROCORRECTION_T,RSRP),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        27,

        /* SiblingIndex */
        29,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"[1]=-50[2]=-80[3]=-100[4]=-120",

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
        DmConvertInt8ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"Correction",

        /* RealName */
        (INT8*)"Correction",

        /* Description */
        (INT8*)"Tx power correction. Uint: dB",

        /* ParaType */
        SON_DM_DATA_TYPE_STRING,

        /* CType */
        SON_DM_C_TYPE_INT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_TPM_LUTCCMACROCORRECTION_T,Correction),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        27,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"",

        /* DefaultValue */
        (INT8*)"[1]=30[2]=20[3]=5[4]=0",

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
        DmConvertInt8ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"SON_DM.IM.",

        /* RealName */
        (INT8*)"IM",

        /* Description */
        (INT8*)"This object contains parameters relating to SON IM",

        /* ParaType */
        SON_DM_DATA_TYPE_OBJECT,

        /* CType */
        SON_DM_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_T,IM),

        /* CTypeSize */
        sizeof(SON_DM_IM_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        0,

        /* SiblingIndex */
        48,

        /* FirstChildIndex */
        31,

        /* ChildrenCounter */
        15,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x8,

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
        (INT8*)"Enabled",

        /* RealName */
        (INT8*)"Enabled",

        /* Description */
        (INT8*)"Indicates whether ICIC functionality is enabled. ",

        /* ParaType */
        SON_DM_DATA_TYPE_BOOLEAN,

        /* CType */
        SON_DM_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_IM_T,Enabled),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        30,

        /* SiblingIndex */
        32,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

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
        (INT8*)"UeClassificationEnabled",

        /* RealName */
        (INT8*)"UeClassificationEnabled",

        /* Description */
        (INT8*)"Indicates whether UE classification is enabled. ",

        /* ParaType */
        SON_DM_DATA_TYPE_BOOLEAN,

        /* CType */
        SON_DM_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_IM_T,UeClassificationEnabled),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        30,

        /* SiblingIndex */
        33,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

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
        (INT8*)"ImFairLevel",

        /* RealName */
        (INT8*)"ImFairLevel",

        /* Description */
        (INT8*)"IM fairness level. 0: low, 1: medium, 2: high",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_IM_T,ImFairLevel),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        30,

        /* SiblingIndex */
        34,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:2]",

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
        DmConvertUint8ToString,

        /* ToStructFunc */
        DmConvertStringToUint8
    },

    {
        /* Name */
        (INT8*)"ImFairFactor",

        /* RealName */
        (INT8*)"ImFairFactor",

        /* Description */
        (INT8*)"IM fairness factor. Dimension: 3, 0: low, 1: medium, 2: high.",

        /* ParaType */
        SON_DM_DATA_TYPE_STRING,

        /* CType */
        SON_DM_C_TYPE_UINT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_IM_T,ImFairFactor),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        SON_DM_IM_IMFAIRFACTOR_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(SON_DM_IM_T,ImFairFactorCnt),

        /* ParentIndex */
        30,

        /* SiblingIndex */
        35,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"3[0:32]",

        /* DefaultValue */
        (INT8*)"16,8,4",

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
        DmConvertUint8ArrayToString,

        /* ToStructFunc */
        DmConvertStringToUint8Array
    },

    {
        /* Name */
        (INT8*)"ImHardFFR",

        /* RealName */
        (INT8*)"ImHardFFR",

        /* Description */
        (INT8*)"Mode of FFR, 1 if hard FFR is used, 0 if soft one is used.",

        /* ParaType */
        SON_DM_DATA_TYPE_BOOLEAN,

        /* CType */
        SON_DM_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_IM_T,ImHardFFR),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        30,

        /* SiblingIndex */
        36,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

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
        (INT8*)"ImDeltaRsrpHigh",

        /* RealName */
        (INT8*)"ImDeltaRsrpHigh",

        /* Description */
        (INT8*)"High threshold for delta between serving RSRP and max neighbor RSRP. It is supposed to be higher than ImDeltaRsrpLow ",

        /* ParaType */
        SON_DM_DATA_TYPE_INT,

        /* CType */
        SON_DM_C_TYPE_INT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_IM_T,ImDeltaRsrpHigh),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        30,

        /* SiblingIndex */
        37,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[-24:24]",

        /* DefaultValue */
        (INT8*)"7",

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
        DmConvertInt8ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"ImDeltaRsrpLow",

        /* RealName */
        (INT8*)"ImDeltaRsrpLow",

        /* Description */
        (INT8*)"Low threshold for delta between serving RSRP and max neighbor RSRP. It is supposed to be lower than ImDeltaRsrpHigh ",

        /* ParaType */
        SON_DM_DATA_TYPE_INT,

        /* CType */
        SON_DM_C_TYPE_INT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_IM_T,ImDeltaRsrpLow),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        30,

        /* SiblingIndex */
        38,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[-24:24]",

        /* DefaultValue */
        (INT8*)"-3",

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
        DmConvertInt8ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"ImUeMeasInterval",

        /* RealName */
        (INT8*)"ImUeMeasInterval",

        /* Description */
        (INT8*)"Period in second for UE measurement report. Not UE specific.",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT16,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_IM_T,ImUeMeasInterval),

        /* CTypeSize */
        sizeof(UINT16),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        30,

        /* SiblingIndex */
        39,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:65535]",

        /* DefaultValue */
        (INT8*)"32",

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
        DmConvertUint16ToString,

        /* ToStructFunc */
        DmConvertStringToUint16
    },

    {
        /* Name */
        (INT8*)"ImIirCoeff",

        /* RealName */
        (INT8*)"ImIirCoeff",

        /* Description */
        (INT8*)"CQI averaging coefficients exponent, not UE specific. Dimension: 2, one for WB CQI and the other for subband CQI. Default value 16, possible values are: 8, 16, 32, 64",

        /* ParaType */
        SON_DM_DATA_TYPE_STRING,

        /* CType */
        SON_DM_C_TYPE_UINT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_IM_T,ImIirCoeff),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        SON_DM_IM_IMIIRCOEFF_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(SON_DM_IM_T,ImIirCoeffCnt),

        /* ParentIndex */
        30,

        /* SiblingIndex */
        40,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"2[8,16,32,64]",

        /* DefaultValue */
        (INT8*)"16,16",

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
        DmConvertUint8ArrayToString,

        /* ToStructFunc */
        DmConvertStringToUint8Array
    },

    {
        /* Name */
        (INT8*)"ImCqiAverageType",

        /* RealName */
        (INT8*)"ImCqiAverageType",

        /* Description */
        (INT8*)"CQI Averaging Type. 0: direct, 1: efficiency",

        /* ParaType */
        SON_DM_DATA_TYPE_STRING,

        /* CType */
        SON_DM_C_TYPE_UINT32_ENUM,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_IM_T,ImCqiAverageType),

        /* CTypeSize */
        sizeof(UINT32_ENUM),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        30,

        /* SiblingIndex */
        41,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"DIRECT,EFFICIENCY",

        /* DefaultValue */
        (INT8*)"DIRECT",

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
        (INT8*)"ImPaLow",

        /* RealName */
        (INT8*)"ImPaLow",

        /* Description */
        (INT8*)"Low PA value for those specific PRBs. dB-6, dB-4dot77, dB-3, dB-1dot77, dB0, dB1, dB2, dB3, dB-inf ",

        /* ParaType */
        SON_DM_DATA_TYPE_STRING,

        /* CType */
        SON_DM_C_TYPE_UINT32_ENUM,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_IM_T,ImPaLow),

        /* CTypeSize */
        sizeof(UINT32_ENUM),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        30,

        /* SiblingIndex */
        42,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"DB_NEG_6,DB_NEG_4_77,DB_NEG_3,DB_NEG_1_77,DB_0,DB_1,DB_2,DB_3,DB_NEG_INF",

        /* DefaultValue */
        (INT8*)"DB_NEG_4_77",

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
        (INT8*)"ImPaHigh",

        /* RealName */
        (INT8*)"ImPaHigh",

        /* Description */
        (INT8*)"High PA value for those specific PRBs. dB-6, dB-4dot77, dB-3, dB-1dot77, dB0, dB1, dB2, dB3, dB-inf ",

        /* ParaType */
        SON_DM_DATA_TYPE_STRING,

        /* CType */
        SON_DM_C_TYPE_UINT32_ENUM,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_IM_T,ImPaHigh),

        /* CTypeSize */
        sizeof(UINT32_ENUM),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        30,

        /* SiblingIndex */
        43,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"DB_NEG_6,DB_NEG_4_77,DB_NEG_3,DB_NEG_1_77,DB_0,DB_1,DB_2,DB_3,DB_NEG_INF",

        /* DefaultValue */
        (INT8*)"DB_2",

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
        (INT8*)"ImBaseOnDynamicUEReport",

        /* RealName */
        (INT8*)"ImBaseOnDynamicUEReport",

        /* Description */
        (INT8*)"0 means based on PCI, 1 means based on CQIs from dynamic UE reports",

        /* ParaType */
        SON_DM_DATA_TYPE_BOOLEAN,

        /* CType */
        SON_DM_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_IM_T,ImBaseOnDynamicUEReport),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        30,

        /* SiblingIndex */
        44,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

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
        (INT8*)"ImForceICIC",

        /* RealName */
        (INT8*)"ImForceICIC",

        /* Description */
        (INT8*)"Flag to ignore neighbor cell absence and force ICIC algorithm",

        /* ParaType */
        SON_DM_DATA_TYPE_BOOLEAN,

        /* CType */
        SON_DM_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_IM_T,ImForceICIC),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        30,

        /* SiblingIndex */
        45,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

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
        (INT8*)"SON_DM.IM.BW.{i}.",

        /* RealName */
        (INT8*)"BW",

        /* Description */
        (INT8*)" 3 or 6 column by N_PRB row (max 110) matrix with binary entry N_PRB is the PRB in the HeNB bandwidth. Each column is a RNTP Mask, First vector(left-most in the matrix) is the one to be used by this HeNB. Index from top down represent increasing PRB index.",

        /* ParaType */
        SON_DM_DATA_TYPE_OBJECT,

        /* CType */
        SON_DM_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_IM_T,BW),

        /* CTypeSize */
        sizeof(SON_DM_IM_BW_T),

        /* MaxElements */
        SON_DM_IM_BW_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(SON_DM_IM_T,BW_cnt),

        /* ParentIndex */
        30,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        46,

        /* ChildrenCounter */
        1,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x8,

        /* ValueRange */
        (INT8*)"6",

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
        NULL,

        /* ToStructFunc */
        NULL
    },

    {
        /* Name */
        (INT8*)"SON_DM.IM.BW.{i}.ImDlProtectedPrbList.{i}.",

        /* RealName */
        (INT8*)"ImDlProtectedPrbList",

        /* Description */
        (INT8*)"This object contains parameters relating to SON ICIC",

        /* ParaType */
        SON_DM_DATA_TYPE_OBJECT,

        /* CType */
        SON_DM_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_IM_BW_T,ImDlProtectedPrbList),

        /* CTypeSize */
        sizeof(SON_DM_IM_BW_IMDLPROTECTEDPRBLIST_T),

        /* MaxElements */
        SON_DM_IM_BW_IMDLPROTECTEDPRBLIST_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(SON_DM_IM_BW_T,ImDlProtectedPrbList_cnt),

        /* ParentIndex */
        45,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        47,

        /* ChildrenCounter */
        1,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"6",

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
        (INT8*)"Bitmap",

        /* RealName */
        (INT8*)"Bitmap",

        /* Description */
        (INT8*)"DL protected PRB pattern for each BW.",

        /* ParaType */
        SON_DM_DATA_TYPE_STRING,

        /* CType */
        SON_DM_C_TYPE_UINT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_IM_BW_IMDLPROTECTEDPRBLIST_T,Bitmap),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        SON_DM_IM_BW_IMDLPROTECTEDPRBLIST_BITMAP_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(SON_DM_IM_BW_IMDLPROTECTEDPRBLIST_T,BitmapCnt),

        /* ParentIndex */
        46,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"14[0:255]",

        /* DefaultValue */
        (INT8*)"[1,1]=16,0,0,0,0,0,0,0,0,0,0,0,0,0[1,2]=8,0,0,0,0,0,0,0,0,0,0,0,0,0[1,3]=4,0,0,0,0,0,0,0,0,0,0,0,0,0[2,1]=0,96,0,0,0,0,0,0,0,0,0,0,0,0[2,2]=0,24,0,0,0,0,0,0,0,0,0,0,0,0[2,3]=0,6,0,0,0,0,0,0,0,0,0,0,0,0[3,1]=0,0,224,0,0,0,0,0,0,0,0,0,0,0[3,2]=0,0,28,0,0,0,0,0,0,0,0,0,0,0[3,3]=0,0,3,128,0,0,0,0,0,0,0,0,0,0[4,1]=0,0,0,0,252,0,0,0,0,0,0,0,0,0[4,2]=0,0,0,0,3,240,0,0,0,0,0,0,0,0[4,3]=0,0,0,0,0,15,192,0,0,0,0,0,0,0[5,1]=0,0,0,0,0,0,255,128,0,0,0,0,0,0[5,2]=0,0,0,0,0,0,0,127,192,0,0,0,0,0[5,3]=0,0,0,0,0,0,0,0,63,224,0,0,0,0[6,1]=0,0,0,0,0,0,0,0,255,240,0,0,0,0[6,2]=0,0,0,0,0,0,0,0,0,15,255,0,0,0[6,3]=0,0,0,0,0,0,0,0,0,0,0,255,240,0",

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
        DmConvertUint8ArrayToString,

        /* ToStructFunc */
        DmConvertStringToUint8Array
    },

    {
        /* Name */
        (INT8*)"SON_DM.CCO.",

        /* RealName */
        (INT8*)"CCO",

        /* Description */
        (INT8*)"This object contains parameters relating to SON CCO",

        /* ParaType */
        SON_DM_DATA_TYPE_OBJECT,

        /* CType */
        SON_DM_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_T,CCO),

        /* CTypeSize */
        sizeof(SON_DM_CCO_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        0,

        /* SiblingIndex */
        67,

        /* FirstChildIndex */
        49,

        /* ChildrenCounter */
        14,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x8,

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
        (INT8*)"Enabled",

        /* RealName */
        (INT8*)"Enabled",

        /* Description */
        (INT8*)"Indicates whether CCO functionality is enabled. ",

        /* ParaType */
        SON_DM_DATA_TYPE_BOOLEAN,

        /* CType */
        SON_DM_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_CCO_T,Enabled),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        48,

        /* SiblingIndex */
        50,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

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
        (INT8*)"UeMeasReportPeriod",

        /* RealName */
        (INT8*)"UeMeasReportPeriod",

        /* Description */
        (INT8*)"UE Measurement report period. Range: 0..65535, in seconds",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT16,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_CCO_T,UeMeasReportPeriod),

        /* CTypeSize */
        sizeof(UINT16),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        48,

        /* SiblingIndex */
        51,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:65535]",

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
        DmConvertUint16ToString,

        /* ToStructFunc */
        DmConvertStringToUint16
    },

    {
        /* Name */
        (INT8*)"RsrqThreshold1",

        /* RealName */
        (INT8*)"RsrqThreshold1",

        /* Description */
        (INT8*)"RSRQ thresholds1, -39..-6, representing -19.5..-3 in dB, in 0.5 dB step",

        /* ParaType */
        SON_DM_DATA_TYPE_INT,

        /* CType */
        SON_DM_C_TYPE_INT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_CCO_T,RsrqThreshold1),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        48,

        /* SiblingIndex */
        52,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[-39:-6]",

        /* DefaultValue */
        (INT8*)"-24",

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
        DmConvertInt8ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"RsrqThreshold2",

        /* RealName */
        (INT8*)"RsrqThreshold2",

        /* Description */
        (INT8*)"RSRQ thresholds2, -39..-6, representing -19.5..-3 in dB, in 0.5 dB step",

        /* ParaType */
        SON_DM_DATA_TYPE_INT,

        /* CType */
        SON_DM_C_TYPE_INT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_CCO_T,RsrqThreshold2),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        48,

        /* SiblingIndex */
        53,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[-39:-6]",

        /* DefaultValue */
        (INT8*)"-28",

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
        DmConvertInt8ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"RsrpThresholdServingNoise",

        /* RealName */
        (INT8*)"RsrpThresholdServingNoise",

        /* Description */
        (INT8*)"RSRP thresholds",

        /* ParaType */
        SON_DM_DATA_TYPE_INT,

        /* CType */
        SON_DM_C_TYPE_INT16,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_CCO_T,RsrpThresholdServingNoise),

        /* CTypeSize */
        sizeof(INT16),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        48,

        /* SiblingIndex */
        54,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[-144:-40]",

        /* DefaultValue */
        (INT8*)"-60",

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
        DmConvertInt16ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"RsrpThresholdServingInf",

        /* RealName */
        (INT8*)"RsrpThresholdServingInf",

        /* Description */
        (INT8*)"RSRP thresholds",

        /* ParaType */
        SON_DM_DATA_TYPE_INT,

        /* CType */
        SON_DM_C_TYPE_INT16,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_CCO_T,RsrpThresholdServingInf),

        /* CTypeSize */
        sizeof(INT16),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        48,

        /* SiblingIndex */
        55,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[-144:-40]",

        /* DefaultValue */
        (INT8*)"-50",

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
        DmConvertInt16ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"RsrpThresholdNeighbor2Cell",

        /* RealName */
        (INT8*)"RsrpThresholdNeighbor2Cell",

        /* Description */
        (INT8*)"RSRP thresholds",

        /* ParaType */
        SON_DM_DATA_TYPE_INT,

        /* CType */
        SON_DM_C_TYPE_INT16,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_CCO_T,RsrpThresholdNeighbor2Cell),

        /* CTypeSize */
        sizeof(INT16),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        48,

        /* SiblingIndex */
        56,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[-144:-40]",

        /* DefaultValue */
        (INT8*)"-80",

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
        DmConvertInt16ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"RsrpThresholdNeighbor3Cell",

        /* RealName */
        (INT8*)"RsrpThresholdNeighbor3Cell",

        /* Description */
        (INT8*)"RSRP thresholds",

        /* ParaType */
        SON_DM_DATA_TYPE_INT,

        /* CType */
        SON_DM_C_TYPE_INT16,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_CCO_T,RsrpThresholdNeighbor3Cell),

        /* CTypeSize */
        sizeof(INT16),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        48,

        /* SiblingIndex */
        57,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[-144:-40]",

        /* DefaultValue */
        (INT8*)"-70",

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
        DmConvertInt16ToString,

        /* ToStructFunc */
        DmConvertStringToInt
    },

    {
        /* Name */
        (INT8*)"RsrqHysteresis",

        /* RealName */
        (INT8*)"RsrqHysteresis",

        /* Description */
        (INT8*)"RSRQ Hysteresis",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_CCO_T,RsrqHysteresis),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        48,

        /* SiblingIndex */
        58,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:24]",

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
        DmConvertUint8ToString,

        /* ToStructFunc */
        DmConvertStringToUint8
    },

    {
        /* Name */
        (INT8*)"RsrpHysteresis",

        /* RealName */
        (INT8*)"RsrpHysteresis",

        /* Description */
        (INT8*)"RSRP Hysteresis",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_CCO_T,RsrpHysteresis),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        48,

        /* SiblingIndex */
        59,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:100]",

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
        DmConvertUint8ToString,

        /* ToStructFunc */
        DmConvertStringToUint8
    },

    {
        /* Name */
        (INT8*)"SampleCountThreshold",

        /* RealName */
        (INT8*)"SampleCountThreshold",

        /* Description */
        (INT8*)"Sample count threshold",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_CCO_T,SampleCountThreshold),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        48,

        /* SiblingIndex */
        60,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:255]",

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
        DmConvertUint8ToString,

        /* ToStructFunc */
        DmConvertStringToUint8
    },

    {
        /* Name */
        (INT8*)"Step",

        /* RealName */
        (INT8*)"Step",

        /* Description */
        (INT8*)"Step of the RsTxPwr change",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_CCO_T,Step),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        48,

        /* SiblingIndex */
        61,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[1:127]",

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
        DmConvertUint8ToString,

        /* ToStructFunc */
        DmConvertStringToUint8
    },

    {
        /* Name */
        (INT8*)"CellEdgeRatio",

        /* RealName */
        (INT8*)"CellEdgeRatio",

        /* Description */
        (INT8*)"Cell edge UE ratio threshold",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_CCO_T,CellEdgeRatio),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        48,

        /* SiblingIndex */
        62,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:100]",

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
        DmConvertUint8ToString,

        /* ToStructFunc */
        DmConvertStringToUint8
    },

    {
        /* Name */
        (INT8*)"SON_DM.CCO.Weights.",

        /* RealName */
        (INT8*)"Weights",

        /* Description */
        (INT8*)"Each weight is 0..100, corresponding to 0%..100%.",

        /* ParaType */
        SON_DM_DATA_TYPE_OBJECT,

        /* CType */
        SON_DM_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_CCO_T,Weights),

        /* CTypeSize */
        sizeof(SON_DM_CCO_WEIGHTS_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        48,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        63,

        /* ChildrenCounter */
        4,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x8,

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
        (INT8*)"Good",

        /* RealName */
        (INT8*)"Good",

        /* Description */
        (INT8*)"Good UE",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_CCO_WEIGHTS_T,Good),

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
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:100]",

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
        DmConvertUint8ToString,

        /* ToStructFunc */
        DmConvertStringToUint8
    },

    {
        /* Name */
        (INT8*)"NoiseDominated",

        /* RealName */
        (INT8*)"NoiseDominated",

        /* Description */
        (INT8*)"Noise dominated UE",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_CCO_WEIGHTS_T,NoiseDominated),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        62,

        /* SiblingIndex */
        65,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:100]",

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
        DmConvertUint8ToString,

        /* ToStructFunc */
        DmConvertStringToUint8
    },

    {
        /* Name */
        (INT8*)"InterferenceDominated2Cell",

        /* RealName */
        (INT8*)"InterferenceDominated2Cell",

        /* Description */
        (INT8*)"Interference dominated with 2 cells UE",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_CCO_WEIGHTS_T,InterferenceDominated2Cell),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        62,

        /* SiblingIndex */
        66,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:100]",

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
        DmConvertUint8ToString,

        /* ToStructFunc */
        DmConvertStringToUint8
    },

    {
        /* Name */
        (INT8*)"InterferenceDominated3Cell",

        /* RealName */
        (INT8*)"InterferenceDominated3Cell",

        /* Description */
        (INT8*)"Interference dominated with 2+ cells UE",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_CCO_WEIGHTS_T,InterferenceDominated3Cell),

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
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:100]",

        /* DefaultValue */
        (INT8*)"25",

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
        (INT8*)"SON_DM.MRO.",

        /* RealName */
        (INT8*)"MRO",

        /* Description */
        (INT8*)"This object contains parameters relating to SON MRO",

        /* ParaType */
        SON_DM_DATA_TYPE_OBJECT,

        /* CType */
        SON_DM_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_T,MRO),

        /* CTypeSize */
        sizeof(SON_DM_MRO_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        0,

        /* SiblingIndex */
        85,

        /* FirstChildIndex */
        68,

        /* ChildrenCounter */
        12,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x8,

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
        (INT8*)"Enabled",

        /* RealName */
        (INT8*)"Enabled",

        /* Description */
        (INT8*)"Indicates whether MRO functionality for intra-frequency LTE is enabled. ",

        /* ParaType */
        SON_DM_DATA_TYPE_BOOLEAN,

        /* CType */
        SON_DM_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MRO_T,Enabled),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        67,

        /* SiblingIndex */
        69,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

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
        (INT8*)"MroInterFreqEnabled",

        /* RealName */
        (INT8*)"MroInterFreqEnabled",

        /* Description */
        (INT8*)"Indicates whether MRO functionality for inter-frequency LTE is enabled. ",

        /* ParaType */
        SON_DM_DATA_TYPE_BOOLEAN,

        /* CType */
        SON_DM_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MRO_T,MroInterFreqEnabled),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        67,

        /* SiblingIndex */
        70,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

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
        (INT8*)"MroPeriodicity",

        /* RealName */
        (INT8*)"MroPeriodicity",

        /* Description */
        (INT8*)"MRO execution periodicity, in seconds ",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MRO_T,MroPeriodicity),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        67,

        /* SiblingIndex */
        71,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:4294967295]",

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
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"MroAperiodicEnable",

        /* RealName */
        (INT8*)"MroAperiodicEnable",

        /* Description */
        (INT8*)"Indicates RRM shall send MRO messages to SON when receiving TX power change from CCO/TPM. ",

        /* ParaType */
        SON_DM_DATA_TYPE_BOOLEAN,

        /* CType */
        SON_DM_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MRO_T,MroAperiodicEnable),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        67,

        /* SiblingIndex */
        72,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

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
        (INT8*)"PowerChangeThreshold",

        /* RealName */
        (INT8*)"PowerChangeThreshold",

        /* Description */
        (INT8*)"If tx power change exceed threshold, MRO reset. Uint: dBm",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MRO_T,PowerChangeThreshold),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        67,

        /* SiblingIndex */
        73,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:30]",

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
        DmConvertUint8ToString,

        /* ToStructFunc */
        DmConvertStringToUint8
    },

    {
        /* Name */
        (INT8*)"PingPongHoTimeThreshold",

        /* RealName */
        (INT8*)"PingPongHoTimeThreshold",

        /* Description */
        (INT8*)"PingPong Handover Time Threshold. Uint: s",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT16,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MRO_T,PingPongHoTimeThreshold),

        /* CTypeSize */
        sizeof(UINT16),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        67,

        /* SiblingIndex */
        74,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[1:60]",

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
        DmConvertUint16ToString,

        /* ToStructFunc */
        DmConvertStringToUint16
    },

    {
        /* Name */
        (INT8*)"X2StoreUEContextTime",

        /* RealName */
        (INT8*)"X2StoreUEContextTime",

        /* Description */
        (INT8*)"Time interval to check classify RLF event as X2 HO event, in miliseconds ",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MRO_T,X2StoreUEContextTime),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        67,

        /* SiblingIndex */
        75,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:4294967295]",

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
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"S1StoreUEContextTime",

        /* RealName */
        (INT8*)"S1StoreUEContextTime",

        /* Description */
        (INT8*)"Time interval to check classify RLF event as S1 HO event, in miliseconds ",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MRO_T,S1StoreUEContextTime),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        67,

        /* SiblingIndex */
        76,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:4294967295]",

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
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"TargetHOFailureRate",

        /* RealName */
        (INT8*)"TargetHOFailureRate",

        /* Description */
        (INT8*)"The percentage of target RLF ration to optimize, and the practical number would be 1/1000 of the value, value range 0..1000 ",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT16,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MRO_T,TargetHOFailureRate),

        /* CTypeSize */
        sizeof(UINT16),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        67,

        /* SiblingIndex */
        77,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:1000]",

        /* DefaultValue */
        (INT8*)"500",

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
        DmConvertUint16ToString,

        /* ToStructFunc */
        DmConvertStringToUint16
    },

    {
        /* Name */
        (INT8*)"TargetPingpongRate",

        /* RealName */
        (INT8*)"TargetPingpongRate",

        /* Description */
        (INT8*)"The percentage of target pingpong ration to optimize, and the practical number would be 1/1000 of the value, value range 0..1000 ",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT16,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MRO_T,TargetPingpongRate),

        /* CTypeSize */
        sizeof(UINT16),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        67,

        /* SiblingIndex */
        78,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:1000]",

        /* DefaultValue */
        (INT8*)"500",

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
        DmConvertUint16ToString,

        /* ToStructFunc */
        DmConvertStringToUint16
    },

    {
        /* Name */
        (INT8*)"MinRlfEvent",

        /* RealName */
        (INT8*)"MinRlfEvent",

        /* Description */
        (INT8*)"The minimum number of RLF events to execute MRO functionality ",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT16,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MRO_T,MinRlfEvent),

        /* CTypeSize */
        sizeof(UINT16),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        67,

        /* SiblingIndex */
        79,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:65535]",

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
        DmConvertUint16ToString,

        /* ToStructFunc */
        DmConvertStringToUint16
    },

    {
        /* Name */
        (INT8*)"SON_DM.MRO.MroWeights.",

        /* RealName */
        (INT8*)"MroWeights",

        /* Description */
        (INT8*)"The weight vector of each HO problem: w_TL, w_TE, w_WCA, w_WCB, w_PP. Each weight is 0..1000, corresponding to 1/1000 of the configured value  ",

        /* ParaType */
        SON_DM_DATA_TYPE_OBJECT,

        /* CType */
        SON_DM_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MRO_T,MroWeights),

        /* CTypeSize */
        sizeof(SON_DM_MRO_MROWEIGHTS_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        67,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        80,

        /* ChildrenCounter */
        5,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x8,

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
        (INT8*)"TL",

        /* RealName */
        (INT8*)"TL",

        /* Description */
        (INT8*)"TL - Too Late ",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT16,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MRO_MROWEIGHTS_T,TL),

        /* CTypeSize */
        sizeof(UINT16),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        79,

        /* SiblingIndex */
        81,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:1000]",

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
        DmConvertUint16ToString,

        /* ToStructFunc */
        DmConvertStringToUint16
    },

    {
        /* Name */
        (INT8*)"TE",

        /* RealName */
        (INT8*)"TE",

        /* Description */
        (INT8*)"TE - Too Early",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT16,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MRO_MROWEIGHTS_T,TE),

        /* CTypeSize */
        sizeof(UINT16),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        79,

        /* SiblingIndex */
        82,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:1000]",

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
        DmConvertUint16ToString,

        /* ToStructFunc */
        DmConvertStringToUint16
    },

    {
        /* Name */
        (INT8*)"WCA",

        /* RealName */
        (INT8*)"WCA",

        /* Description */
        (INT8*)"WCA - Wrong Cell After HO",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT16,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MRO_MROWEIGHTS_T,WCA),

        /* CTypeSize */
        sizeof(UINT16),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        79,

        /* SiblingIndex */
        83,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:1000]",

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
        DmConvertUint16ToString,

        /* ToStructFunc */
        DmConvertStringToUint16
    },

    {
        /* Name */
        (INT8*)"WCB",

        /* RealName */
        (INT8*)"WCB",

        /* Description */
        (INT8*)"WCB - Wrong Cell Before HO",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT16,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MRO_MROWEIGHTS_T,WCB),

        /* CTypeSize */
        sizeof(UINT16),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        79,

        /* SiblingIndex */
        84,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:1000]",

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
        DmConvertUint16ToString,

        /* ToStructFunc */
        DmConvertStringToUint16
    },

    {
        /* Name */
        (INT8*)"PP",

        /* RealName */
        (INT8*)"PP",

        /* Description */
        (INT8*)"PP - PingPong ",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT16,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MRO_MROWEIGHTS_T,PP),

        /* CTypeSize */
        sizeof(UINT16),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        79,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:1000]",

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
        DmConvertUint16ToString,

        /* ToStructFunc */
        DmConvertStringToUint16
    },

    {
        /* Name */
        (INT8*)"SON_DM.MLB.",

        /* RealName */
        (INT8*)"MLB",

        /* Description */
        (INT8*)"This object contains parameters relating to SON MLB",

        /* ParaType */
        SON_DM_DATA_TYPE_OBJECT,

        /* CType */
        SON_DM_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_T,MLB),

        /* CTypeSize */
        sizeof(SON_DM_MLB_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        0,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        86,

        /* ChildrenCounter */
        16,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x8,

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
        (INT8*)"Enabled",

        /* RealName */
        (INT8*)"Enabled",

        /* Description */
        (INT8*)"Indicates whether MLB functionality for intra-frequency LTE is enabled. ",

        /* ParaType */
        SON_DM_DATA_TYPE_BOOLEAN,

        /* CType */
        SON_DM_C_TYPE_BOOL8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MLB_T,Enabled),

        /* CTypeSize */
        sizeof(BOOL8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        85,

        /* SiblingIndex */
        87,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

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
        (INT8*)"MlbPeriodicity",

        /* RealName */
        (INT8*)"MlbPeriodicity",

        /* Description */
        (INT8*)"MLB execution periodicity, in seconds ",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MLB_T,MlbPeriodicity),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        85,

        /* SiblingIndex */
        88,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:4294967295]",

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
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"ReportingPeriodicity",

        /* RealName */
        (INT8*)"ReportingPeriodicity",

        /* Description */
        (INT8*)"X2 Reporting periodicity of ResourceStatusUpdate message, unit: ms",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT32_ENUM,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MLB_T,ReportingPeriodicity),

        /* CTypeSize */
        sizeof(UINT32_ENUM),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        85,

        /* SiblingIndex */
        89,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"1000,2000,5000,10000",

        /* DefaultValue */
        (INT8*)"10000",

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
        (INT8*)"GtpEchoDelayCoff",

        /* RealName */
        (INT8*)"GtpEchoDelayCoff",

        /* Description */
        (INT8*)"S1 TNL load percentage = Gtp echo delay/Gtp echo delay coefficient",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT16,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MLB_T,GtpEchoDelayCoff),

        /* CTypeSize */
        sizeof(UINT16),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        85,

        /* SiblingIndex */
        90,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:600]",

        /* DefaultValue */
        (INT8*)"80",

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
        DmConvertUint16ToString,

        /* ToStructFunc */
        DmConvertStringToUint16
    },

    {
        /* Name */
        (INT8*)"S1TnlLowLoadThreshold",

        /* RealName */
        (INT8*)"S1TnlLowLoadThreshold",

        /* Description */
        (INT8*)"S1Tnl Low load threshold",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MLB_T,S1TnlLowLoadThreshold),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        85,

        /* SiblingIndex */
        91,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:100]",

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
        DmConvertUint8ToString,

        /* ToStructFunc */
        DmConvertStringToUint8
    },

    {
        /* Name */
        (INT8*)"S1TnlMedLoadThreshold",

        /* RealName */
        (INT8*)"S1TnlMedLoadThreshold",

        /* Description */
        (INT8*)"S1Tnl Medium load threshold",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MLB_T,S1TnlMedLoadThreshold),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        85,

        /* SiblingIndex */
        92,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:100]",

        /* DefaultValue */
        (INT8*)"60",

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
        (INT8*)"S1TnlHighLoadThreshold",

        /* RealName */
        (INT8*)"S1TnlHighLoadThreshold",

        /* Description */
        (INT8*)"S1Tnl High load threshold",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MLB_T,S1TnlHighLoadThreshold),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        85,

        /* SiblingIndex */
        93,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:100]",

        /* DefaultValue */
        (INT8*)"90",

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
        (INT8*)"HwLowLoadThreshold",

        /* RealName */
        (INT8*)"HwLowLoadThreshold",

        /* Description */
        (INT8*)"Hardware Low load threshold",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MLB_T,HwLowLoadThreshold),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        85,

        /* SiblingIndex */
        94,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:100]",

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
        DmConvertUint8ToString,

        /* ToStructFunc */
        DmConvertStringToUint8
    },

    {
        /* Name */
        (INT8*)"HwMedLoadThreshold",

        /* RealName */
        (INT8*)"HwMedLoadThreshold",

        /* Description */
        (INT8*)"Hardware Medium load threshold",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MLB_T,HwMedLoadThreshold),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        85,

        /* SiblingIndex */
        95,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:100]",

        /* DefaultValue */
        (INT8*)"60",

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
        (INT8*)"HwHighLoadThreshold",

        /* RealName */
        (INT8*)"HwHighLoadThreshold",

        /* Description */
        (INT8*)"Hardware High load threshold",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MLB_T,HwHighLoadThreshold),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        85,

        /* SiblingIndex */
        96,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:100]",

        /* DefaultValue */
        (INT8*)"90",

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
        (INT8*)"PrbLowLoadThreshold",

        /* RealName */
        (INT8*)"PrbLowLoadThreshold",

        /* Description */
        (INT8*)"PRB Low load threshold",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MLB_T,PrbLowLoadThreshold),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        85,

        /* SiblingIndex */
        97,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:100]",

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
        DmConvertUint8ToString,

        /* ToStructFunc */
        DmConvertStringToUint8
    },

    {
        /* Name */
        (INT8*)"PrbMedLoadThreshold",

        /* RealName */
        (INT8*)"PrbMedLoadThreshold",

        /* Description */
        (INT8*)"PRB Medium load threshold",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MLB_T,PrbMedLoadThreshold),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        85,

        /* SiblingIndex */
        98,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:100]",

        /* DefaultValue */
        (INT8*)"60",

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
        (INT8*)"PrbHighLoadThreshold",

        /* RealName */
        (INT8*)"PrbHighLoadThreshold",

        /* Description */
        (INT8*)"PRB High load threshold",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MLB_T,PrbHighLoadThreshold),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        85,

        /* SiblingIndex */
        99,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:100]",

        /* DefaultValue */
        (INT8*)"90",

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
        (INT8*)"CapacityLowLoadThreshold",

        /* RealName */
        (INT8*)"CapacityLowLoadThreshold",

        /* Description */
        (INT8*)"Capacity Low load threshold",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MLB_T,CapacityLowLoadThreshold),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        85,

        /* SiblingIndex */
        100,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:100]",

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
        DmConvertUint8ToString,

        /* ToStructFunc */
        DmConvertStringToUint8
    },

    {
        /* Name */
        (INT8*)"CapacityMedLoadThreshold",

        /* RealName */
        (INT8*)"CapacityMedLoadThreshold",

        /* Description */
        (INT8*)"Capacity Medium load threshold",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MLB_T,CapacityMedLoadThreshold),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        85,

        /* SiblingIndex */
        101,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:100]",

        /* DefaultValue */
        (INT8*)"60",

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
        (INT8*)"CapacityHighLoadThreshold",

        /* RealName */
        (INT8*)"CapacityHighLoadThreshold",

        /* Description */
        (INT8*)"Capacity High load threshold",

        /* ParaType */
        SON_DM_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        SON_DM_C_TYPE_UINT8,

        /* CType Offset */
        DM_OFFSETOF(SON_DM_MLB_T,CapacityHighLoadThreshold),

        /* CTypeSize */
        sizeof(UINT8),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        85,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        -1,

        /* ReadFlag */
        0x9,

        /* WriteFlag */
        0x9,

        /* ValueRange */
        (INT8*)"[0:100]",

        /* DefaultValue */
        (INT8*)"90",

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
        NULL,

        /* RealName */
        NULL,

        /* Description */
        NULL,

        /* ParaType */
        SON_DM_DATA_TYPE_OBJECT,

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


/* SON_DM model access list */
DB_ACCESS_NODE_T g_Son_dmDbAccessList[] =
{
    {"ROOT", 0xffff},
    {"TR069", 0x1},
    {"GUI", 0x2},
    {"SNMP", 0x4},
    {"PS", 0x8},
    {NULL, 0}
};



/* SON_DM model config node */
DB_CONFIG_T g_Son_dmConfigNode =
{
    "SON_DM",        /* DB name */
    "19371",        /* DB version */
    DM_FALSE,        /* if it is extended db model */
    "./SON_DM.xml",         /* working path */
    "./SON_DM.xml",         /* loading path */
    "./SON_DM.xml",         /* backup path */
    "./SON_DM_mapping.xml",         /* cmfile-mapping path */
    Son_dmDmPatternRegisterHandler,         /* pattern register function */
    Son_dmDataValidChecker,          /* default data-valid-check function */
    Son_dmGetCTypeStr,          /* default get c-type string function */
    Son_dmGetDataTypeStr,          /* default get data-type string function */
    Son_dmIsObject,          /* to check if it is object */
    g_Son_dmDbAccessList,         /* access list */
    NULL         /* libdl.so handler */
};

DM_ATTR_INFO_T g_Son_dmAttributes[] =
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



/* get SON_DM model config node */
INT32 Son_dmDbLoadReq(void *pDlHandler, BOOL8 bDbReset, INT8 *pWorkPath, INT8 *pLoadPath, INT8 *pBackupPath)
{
    return DmLoadDbReq(&g_Son_dmConfigNode, pDlHandler, bDbReset, pWorkPath, pLoadPath, pBackupPath);
}


/* SON_DM model data-valid-check function */
BOOL8 Son_dmDataValidChecker(UINT8 ucParaCType, INT8 *pValueRange, INT8 *pValue)
{
    BOOL8 bRet = DM_FALSE;

    switch(ucParaCType)
    {
        case SON_DM_C_TYPE_UINT32:
            bRet = DmDataUINT32ValidCheck(pValueRange, pValue);
            break;
        case SON_DM_C_TYPE_INT16:
            bRet = DmDataINT16ValidCheck(pValueRange, pValue);
            break;
        case SON_DM_C_TYPE_STRUCT:
            bRet = DM_TRUE;
            break;
        case SON_DM_C_TYPE_INT8_ARRAY:
            bRet = DmDataStringLengthValidCheck(pValueRange, pValue);
            break;
        case SON_DM_C_TYPE_BOOL8:
            bRet = DmDataBoolValidCheck(pValueRange, pValue);
            break;
        case SON_DM_C_TYPE_UINT8:
            bRet = DmDataUINT8ValidCheck(pValueRange, pValue);
            break;
        case SON_DM_C_TYPE_UINT32_ENUM:
            bRet = DmEnumStringValidCheck(pValueRange, pValue);
            break;
        case SON_DM_C_TYPE_UINT16:
            bRet = DmDataUINT16ValidCheck(pValueRange, pValue);
            break;
        case SON_DM_C_TYPE_UINT8_ARRAY:
            bRet = DmDataUINT8ArrayValidCheck(pValueRange, pValue);
            break;
        case SON_DM_C_TYPE_INT8:
            bRet = DmDataINT8ValidCheck(pValueRange, pValue);
            break;
        default:
            break;
    }

    return bRet;
}


/* SON_DM model c-type string function */
INT8* Son_dmGetCTypeStr(UINT8 ucParaCType)
{
    INT8 *pTypeStr = "";

    switch(ucParaCType)
    {
        case SON_DM_C_TYPE_BOOL8:
            pTypeStr = "BOOL8";
            break;
        case SON_DM_C_TYPE_INT16:
            pTypeStr = "INT16";
            break;
        case SON_DM_C_TYPE_INT8:
            pTypeStr = "INT8";
            break;
        case SON_DM_C_TYPE_INT8_ARRAY:
            pTypeStr = "INT8[]";
            break;
        case SON_DM_C_TYPE_STRUCT:
            pTypeStr = "STRUCT";
            break;
        case SON_DM_C_TYPE_UINT16:
            pTypeStr = "UINT16";
            break;
        case SON_DM_C_TYPE_UINT32:
            pTypeStr = "UINT32";
            break;
        case SON_DM_C_TYPE_UINT32_ENUM:
            pTypeStr = "UINT32_ENUM";
            break;
        case SON_DM_C_TYPE_UINT8:
            pTypeStr = "UINT8";
            break;
        case SON_DM_C_TYPE_UINT8_ARRAY:
            pTypeStr = "UINT8[]";
            break;
        default:
            break;
    }

    return pTypeStr;
}


/* SON_DM model data-type string function */
INT8* Son_dmGetDataTypeStr(UINT8 ucParaType)
{
    INT8 *pTypeStr = "";

    switch(ucParaType)
    {
        case SON_DM_DATA_TYPE_BASE64:
            pTypeStr = "base64";
            break;
        case SON_DM_DATA_TYPE_BOOLEAN:
            pTypeStr = "boolean";
            break;
        case SON_DM_DATA_TYPE_DATETIME:
            pTypeStr = "dateTime";
            break;
        case SON_DM_DATA_TYPE_HEXBINARY:
            pTypeStr = "hexBinary";
            break;
        case SON_DM_DATA_TYPE_INT:
            pTypeStr = "int";
            break;
        case SON_DM_DATA_TYPE_LONG:
            pTypeStr = "long";
            break;
        case SON_DM_DATA_TYPE_OBJECT:
            pTypeStr = "object";
            break;
        case SON_DM_DATA_TYPE_STRING:
            pTypeStr = "string";
            break;
        case SON_DM_DATA_TYPE_UNSIGNEDINT:
            pTypeStr = "unsignedInt";
            break;
        case SON_DM_DATA_TYPE_UNSIGNEDLONG:
            pTypeStr = "unsignedLong";
            break;
        default:
            break;
    }

    return pTypeStr;
}


/* SON_DM model object-type-check function */
BOOL8 Son_dmIsObject(UINT8 ucParaType)
{
    if (ucParaType == SON_DM_DATA_TYPE_OBJECT)
    {
        return DM_TRUE;
    }
    else
    {
        return DM_FALSE;
    }
}


/* SON_DM model pattern register function */
BOOL8 Son_dmDmPatternRegisterHandler()
{
    DM_REG_PATTERN_REQ_T            *pPara;
    BOOL8                           bRtn;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter Son_dmDmPatternRegisterHandler\n");
    pPara = (DM_REG_PATTERN_REQ_T *)malloc(sizeof(DM_REG_PATTERN_REQ_T));

    if(pPara == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Son_dmDmPatternRegisterHandler malloc() failed for pPara\n");
        bRtn = DM_FALSE;
        return bRtn;
    }

    osa_mem_set(pPara,0,sizeof(DM_REG_PATTERN_REQ_T));

    pPara->pucRootName = (INT8 *)"SON_DM.";
    pPara->pPatternFirstNode = (DM_NODE_INFO_T *)g_Son_dmModelNodes;
    pPara->lPatternSize = 102;
    pPara->pAttrPatternFirstNode = (DM_ATTR_INFO_T *)g_Son_dmAttributes;
    bRtn = DmRegisterPatternReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DM register handler failed!\n");
        DM_FREE(pPara);

        return DM_FALSE;
    }

    DM_FREE(pPara);

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave Son_dmDmPatternRegisterHandler\n");
    return DM_TRUE;
}

