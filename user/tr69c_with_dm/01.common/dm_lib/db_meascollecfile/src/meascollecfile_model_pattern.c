

#include "dm_internal.h"
#include "meascollecfile_model_def.h"

#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif


/* the declaration of data-valid-check function list */
extern BOOL8 MeascollecfileDataValidChecker(UINT8 ucParaCType, INT8 *pValueRange, INT8 *pValue);

extern BOOL8 DmDataStringLengthValidCheck(INT8 *pValueRange, INT8 *pValue);


/* the declaration of measCollecFile data-type and c-type str-functions */
extern INT8* MeascollecfileGetCTypeStr(UINT8 ucParaCType);
extern INT8* MeascollecfileGetDataTypeStr(UINT8 ucParaType);


/* the declaration of measCollecFile model register functions */
extern BOOL8 MeascollecfileDmPatternRegisterHandler();


/* the declaration of is-object-function */
extern BOOL8 MeascollecfileIsObject(UINT8 ucParaType);


/* the declaration of convert-fuctions */
extern BOOL8 DmConvertCharArrayToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
extern BOOL8 DmConvertStringToCharArray(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);


DM_NODE_INFO_T g_MeascollecfileModelNodes[]=
{
    {
        /* Name */
        (INT8*)"measCollecFile.",

        /* RealName */
        (INT8*)"measCollecFile",

        /* Description */
        (INT8*)"",

        /* ParaType */
        MEASCOLLECFILE_DATA_TYPE_OBJECT,

        /* CType */
        MEASCOLLECFILE_C_TYPE_STRUCT,

        /* CType Offset */
        0,

        /* CTypeSize */
        sizeof(MEASCOLLECFILE_T),

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
        3,

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
        (INT8*)"measCollecFile.fileHeader.",

        /* RealName */
        (INT8*)"fileHeader",

        /* Description */
        (INT8*)"",

        /* ParaType */
        MEASCOLLECFILE_DATA_TYPE_OBJECT,

        /* CType */
        MEASCOLLECFILE_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(MEASCOLLECFILE_T,fileHeader),

        /* CTypeSize */
        sizeof(MEASCOLLECFILE_FILEHEADER_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        0,

        /* SiblingIndex */
        4,

        /* FirstChildIndex */
        2,

        /* ChildrenCounter */
        2,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"0",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"fileFormatVersion,dnPrefix",

        /* defaultXmlAttrValue */
        (INT8*)"fileFormatVersion=\"32.435 V7.0\"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

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
        (INT8*)"fileSender",

        /* RealName */
        (INT8*)"fileSender",

        /* Description */
        (INT8*)"",

        /* ParaType */
        MEASCOLLECFILE_DATA_TYPE_STRING,

        /* CType */
        MEASCOLLECFILE_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(MEASCOLLECFILE_FILEHEADER_T,fileSender),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        MEASCOLLECFILE_FILEHEADER_FILESENDER_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(MEASCOLLECFILE_FILEHEADER_T,fileSenderCnt),

        /* ParentIndex */
        1,

        /* SiblingIndex */
        3,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"1",

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
        (INT8*)"measCollec",

        /* RealName */
        (INT8*)"measCollec",

        /* Description */
        (INT8*)"",

        /* ParaType */
        MEASCOLLECFILE_DATA_TYPE_STRING,

        /* CType */
        MEASCOLLECFILE_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(MEASCOLLECFILE_FILEHEADER_T,measCollec),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        MEASCOLLECFILE_FILEHEADER_MEASCOLLEC_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(MEASCOLLECFILE_FILEHEADER_T,measCollecCnt),

        /* ParentIndex */
        1,

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
        (INT8*)"1",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"beginTime",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

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
        (INT8*)"measCollecFile.measData.",

        /* RealName */
        (INT8*)"measData",

        /* Description */
        (INT8*)"",

        /* ParaType */
        MEASCOLLECFILE_DATA_TYPE_OBJECT,

        /* CType */
        MEASCOLLECFILE_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(MEASCOLLECFILE_T,measData),

        /* CTypeSize */
        sizeof(MEASCOLLECFILE_MEASDATA_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        0,

        /* SiblingIndex */
        12,

        /* FirstChildIndex */
        5,

        /* ChildrenCounter */
        2,

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
        (INT8*)"managedElement",

        /* RealName */
        (INT8*)"managedElement",

        /* Description */
        (INT8*)"",

        /* ParaType */
        MEASCOLLECFILE_DATA_TYPE_STRING,

        /* CType */
        MEASCOLLECFILE_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(MEASCOLLECFILE_MEASDATA_T,managedElement),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        MEASCOLLECFILE_MEASDATA_MANAGEDELEMENT_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(MEASCOLLECFILE_MEASDATA_T,managedElementCnt),

        /* ParentIndex */
        4,

        /* SiblingIndex */
        6,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"1",

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
        (INT8*)"measCollecFile.measData.measInfo.",

        /* RealName */
        (INT8*)"measInfo",

        /* Description */
        (INT8*)"",

        /* ParaType */
        MEASCOLLECFILE_DATA_TYPE_OBJECT,

        /* CType */
        MEASCOLLECFILE_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(MEASCOLLECFILE_MEASDATA_T,measInfo),

        /* CTypeSize */
        sizeof(MEASCOLLECFILE_MEASDATA_MEASINFO_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        4,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        7,

        /* ChildrenCounter */
        4,

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
        (INT8*)"granPeriod",

        /* RealName */
        (INT8*)"granPeriod",

        /* Description */
        (INT8*)"",

        /* ParaType */
        MEASCOLLECFILE_DATA_TYPE_STRING,

        /* CType */
        MEASCOLLECFILE_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(MEASCOLLECFILE_MEASDATA_MEASINFO_T,granPeriod),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        MEASCOLLECFILE_MEASDATA_MEASINFO_GRANPERIOD_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(MEASCOLLECFILE_MEASDATA_MEASINFO_T,granPeriodCnt),

        /* ParentIndex */
        6,

        /* SiblingIndex */
        8,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"1",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"duration,endTime",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

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
        (INT8*)"repPeriod",

        /* RealName */
        (INT8*)"repPeriod",

        /* Description */
        (INT8*)"",

        /* ParaType */
        MEASCOLLECFILE_DATA_TYPE_STRING,

        /* CType */
        MEASCOLLECFILE_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(MEASCOLLECFILE_MEASDATA_MEASINFO_T,repPeriod),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        MEASCOLLECFILE_MEASDATA_MEASINFO_REPPERIOD_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(MEASCOLLECFILE_MEASDATA_MEASINFO_T,repPeriodCnt),

        /* ParentIndex */
        6,

        /* SiblingIndex */
        9,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"1",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"duration",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

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
        (INT8*)"measTypes",

        /* RealName */
        (INT8*)"measTypes",

        /* Description */
        (INT8*)"",

        /* ParaType */
        MEASCOLLECFILE_DATA_TYPE_STRING,

        /* CType */
        MEASCOLLECFILE_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(MEASCOLLECFILE_MEASDATA_MEASINFO_T,measTypes),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        MEASCOLLECFILE_MEASDATA_MEASINFO_MEASTYPES_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(MEASCOLLECFILE_MEASDATA_MEASINFO_T,measTypesCnt),

        /* ParentIndex */
        6,

        /* SiblingIndex */
        10,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"5600",

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
        (INT8*)"measCollecFile.measData.measInfo.measValue.",

        /* RealName */
        (INT8*)"measValue",

        /* Description */
        (INT8*)"",

        /* ParaType */
        MEASCOLLECFILE_DATA_TYPE_OBJECT,

        /* CType */
        MEASCOLLECFILE_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(MEASCOLLECFILE_MEASDATA_MEASINFO_T,measValue),

        /* CTypeSize */
        sizeof(MEASCOLLECFILE_MEASDATA_MEASINFO_MEASVALUE_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        6,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        11,

        /* ChildrenCounter */
        1,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"0",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"measObjLdn",

        /* defaultXmlAttrValue */
        (INT8*)"measObjLdn=\"RncFunction=RF-1,UtranCell=Gbg-997\"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

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
        (INT8*)"measResults",

        /* RealName */
        (INT8*)"measResults",

        /* Description */
        (INT8*)"",

        /* ParaType */
        MEASCOLLECFILE_DATA_TYPE_STRING,

        /* CType */
        MEASCOLLECFILE_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(MEASCOLLECFILE_MEASDATA_MEASINFO_MEASVALUE_T,measResults),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        MEASCOLLECFILE_MEASDATA_MEASINFO_MEASVALUE_MEASRESULTS_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(MEASCOLLECFILE_MEASDATA_MEASINFO_MEASVALUE_T,measResultsCnt),

        /* ParentIndex */
        10,

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
        (INT8*)"2400",

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
        (INT8*)"measCollecFile.fileFooter.",

        /* RealName */
        (INT8*)"fileFooter",

        /* Description */
        (INT8*)"",

        /* ParaType */
        MEASCOLLECFILE_DATA_TYPE_OBJECT,

        /* CType */
        MEASCOLLECFILE_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(MEASCOLLECFILE_T,fileFooter),

        /* CTypeSize */
        sizeof(MEASCOLLECFILE_FILEFOOTER_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        0,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        13,

        /* ChildrenCounter */
        1,

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
        (INT8*)"measCollec",

        /* RealName */
        (INT8*)"measCollec",

        /* Description */
        (INT8*)"",

        /* ParaType */
        MEASCOLLECFILE_DATA_TYPE_STRING,

        /* CType */
        MEASCOLLECFILE_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(MEASCOLLECFILE_FILEFOOTER_T,measCollec),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        MEASCOLLECFILE_FILEFOOTER_MEASCOLLEC_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(MEASCOLLECFILE_FILEFOOTER_T,measCollecCnt),

        /* ParentIndex */
        12,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        -1,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"1",

        /* DefaultValue */
        (INT8*)"",

        /* supportedXmlAttrs */
        (INT8*)"endTime",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

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
        NULL,

        /* RealName */
        NULL,

        /* Description */
        NULL,

        /* ParaType */
        MEASCOLLECFILE_DATA_TYPE_OBJECT,

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


/* measCollecFile model access list */
DB_ACCESS_NODE_T g_MeascollecfileDbAccessList[] =
{
    {"ROOT", 0xffff},
    {"TR069", 0x1},
    {"GUI", 0x2},
    {"SNMP", 0x4},
    {"PS", 0x8},
    {NULL, 0}
};



/* measCollecFile model config node */
DB_CONFIG_T g_MeascollecfileConfigNode =
{
    "measCollecFile",        /* DB name */
    "17971",        /* DB version */
    DM_TRUE,        /* if it is extended db model */
    "./measCollecFile.xml",         /* working path */
    "./measCollecFile.xml",         /* loading path */
    "./measCollecFile.xml",         /* backup path */
    "./measCollecFile_mapping.xml",         /* cmfile-mapping path */
    MeascollecfileDmPatternRegisterHandler,         /* pattern register function */
    MeascollecfileDataValidChecker,          /* default data-valid-check function */
    MeascollecfileGetCTypeStr,          /* default get c-type string function */
    MeascollecfileGetDataTypeStr,          /* default get data-type string function */
    MeascollecfileIsObject,          /* to check if it is object */
    g_MeascollecfileDbAccessList,         /* access list */
    NULL         /* libdl.so handler */
};

DM_ATTR_INFO_T g_MeascollecfileAttributes[] =
{
    {
        /* attribute name */
        "fileFormatVersion",

        /* attribute description */
        "",

        /* created by default */
        1,

        /* attribute ctype */
        MEASCOLLECFILE_C_TYPE_INT8_ARRAY,

        /* attribute ctype size */
        sizeof(INT8),

        /* attribute valuerange */
        "64",

        /* attribute defaultvalue */
        "",

        /* attribute validcheck */
        DmDataStringLengthValidCheck,

        /* attribute tostring */
        DmConvertCharArrayToString,

        /* attribute tostruct */
        DmConvertStringToCharArray

    },

    {
        /* attribute name */
        "dnPrefix",

        /* attribute description */
        "",

        /* created by default */
        0,

        /* attribute ctype */
        MEASCOLLECFILE_C_TYPE_INT8_ARRAY,

        /* attribute ctype size */
        sizeof(INT8),

        /* attribute valuerange */
        "64",

        /* attribute defaultvalue */
        "",

        /* attribute validcheck */
        DmDataStringLengthValidCheck,

        /* attribute tostring */
        DmConvertCharArrayToString,

        /* attribute tostruct */
        DmConvertStringToCharArray

    },

    {
        /* attribute name */
        "duration",

        /* attribute description */
        "",

        /* created by default */
        1,

        /* attribute ctype */
        MEASCOLLECFILE_C_TYPE_INT8_ARRAY,

        /* attribute ctype size */
        sizeof(INT8),

        /* attribute valuerange */
        "64",

        /* attribute defaultvalue */
        "",

        /* attribute validcheck */
        DmDataStringLengthValidCheck,

        /* attribute tostring */
        DmConvertCharArrayToString,

        /* attribute tostruct */
        DmConvertStringToCharArray

    },

    {
        /* attribute name */
        "beginTime",

        /* attribute description */
        "",

        /* created by default */
        1,

        /* attribute ctype */
        MEASCOLLECFILE_C_TYPE_INT8_ARRAY,

        /* attribute ctype size */
        sizeof(INT8),

        /* attribute valuerange */
        "64",

        /* attribute defaultvalue */
        "",

        /* attribute validcheck */
        DmDataStringLengthValidCheck,

        /* attribute tostring */
        DmConvertCharArrayToString,

        /* attribute tostruct */
        DmConvertStringToCharArray

    },

    {
        /* attribute name */
        "endTime",

        /* attribute description */
        "",

        /* created by default */
        1,

        /* attribute ctype */
        MEASCOLLECFILE_C_TYPE_INT8_ARRAY,

        /* attribute ctype size */
        sizeof(INT8),

        /* attribute valuerange */
        "64",

        /* attribute defaultvalue */
        "",

        /* attribute validcheck */
        DmDataStringLengthValidCheck,

        /* attribute tostring */
        DmConvertCharArrayToString,

        /* attribute tostruct */
        DmConvertStringToCharArray

    },

    {
        /* attribute name */
        "measObjLdn",

        /* attribute description */
        "",

        /* created by default */
        1,

        /* attribute ctype */
        MEASCOLLECFILE_C_TYPE_INT8_ARRAY,

        /* attribute ctype size */
        sizeof(INT8),

        /* attribute valuerange */
        "64",

        /* attribute defaultvalue */
        "",

        /* attribute validcheck */
        DmDataStringLengthValidCheck,

        /* attribute tostring */
        DmConvertCharArrayToString,

        /* attribute tostruct */
        DmConvertStringToCharArray

    },

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



/* get measCollecFile model config node */
INT32 MeascollecfileDbLoadReq(void *pDlHandler, BOOL8 bDbReset, INT8 *pWorkPath, INT8 *pLoadPath, INT8 *pBackupPath)
{
    return DmLoadDbReq(&g_MeascollecfileConfigNode, pDlHandler, bDbReset, pWorkPath, pLoadPath, pBackupPath);
}


/* measCollecFile model data-valid-check function */
BOOL8 MeascollecfileDataValidChecker(UINT8 ucParaCType, INT8 *pValueRange, INT8 *pValue)
{
    BOOL8 bRet = DM_FALSE;

    switch(ucParaCType)
    {
        case MEASCOLLECFILE_C_TYPE_INT8_ARRAY:
            bRet = DmDataStringLengthValidCheck(pValueRange, pValue);
            break;
        case MEASCOLLECFILE_C_TYPE_STRUCT:
            bRet = DM_TRUE;
            break;
        default:
            break;
    }

    return bRet;
}


/* measCollecFile model c-type string function */
INT8* MeascollecfileGetCTypeStr(UINT8 ucParaCType)
{
    INT8 *pTypeStr = "";

    switch(ucParaCType)
    {
        case MEASCOLLECFILE_C_TYPE_INT8_ARRAY:
            pTypeStr = "INT8[]";
            break;
        case MEASCOLLECFILE_C_TYPE_STRUCT:
            pTypeStr = "STRUCT";
            break;
        default:
            break;
    }

    return pTypeStr;
}


/* measCollecFile model data-type string function */
INT8* MeascollecfileGetDataTypeStr(UINT8 ucParaType)
{
    INT8 *pTypeStr = "";

    switch(ucParaType)
    {
        case MEASCOLLECFILE_DATA_TYPE_BASE64:
            pTypeStr = "base64";
            break;
        case MEASCOLLECFILE_DATA_TYPE_BOOLEAN:
            pTypeStr = "boolean";
            break;
        case MEASCOLLECFILE_DATA_TYPE_DATETIME:
            pTypeStr = "dateTime";
            break;
        case MEASCOLLECFILE_DATA_TYPE_HEXBINARY:
            pTypeStr = "hexBinary";
            break;
        case MEASCOLLECFILE_DATA_TYPE_INT:
            pTypeStr = "int";
            break;
        case MEASCOLLECFILE_DATA_TYPE_LONG:
            pTypeStr = "long";
            break;
        case MEASCOLLECFILE_DATA_TYPE_OBJECT:
            pTypeStr = "object";
            break;
        case MEASCOLLECFILE_DATA_TYPE_STRING:
            pTypeStr = "string";
            break;
        case MEASCOLLECFILE_DATA_TYPE_UNSIGNEDINT:
            pTypeStr = "unsignedInt";
            break;
        case MEASCOLLECFILE_DATA_TYPE_UNSIGNEDLONG:
            pTypeStr = "unsignedLong";
            break;
        default:
            break;
    }

    return pTypeStr;
}


/* measCollecFile model object-type-check function */
BOOL8 MeascollecfileIsObject(UINT8 ucParaType)
{
    if (ucParaType == MEASCOLLECFILE_DATA_TYPE_OBJECT)
    {
        return DM_TRUE;
    }
    else
    {
        return DM_FALSE;
    }
}


/* measCollecFile model pattern register function */
BOOL8 MeascollecfileDmPatternRegisterHandler()
{
    DM_REG_PATTERN_REQ_T            *pPara;
    BOOL8                           bRtn;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter MeascollecfileDmPatternRegisterHandler\n");
    pPara = (DM_REG_PATTERN_REQ_T *)malloc(sizeof(DM_REG_PATTERN_REQ_T));

    if(pPara == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ MeascollecfileDmPatternRegisterHandler malloc() failed for pPara\n");
        bRtn = DM_FALSE;
        return bRtn;
    }

    osa_mem_set(pPara,0,sizeof(DM_REG_PATTERN_REQ_T));

    pPara->pucRootName = (INT8 *)"measCollecFile.";
    pPara->pPatternFirstNode = (DM_NODE_INFO_T *)g_MeascollecfileModelNodes;
    pPara->lPatternSize = 14;
    pPara->pAttrPatternFirstNode = (DM_ATTR_INFO_T *)g_MeascollecfileAttributes;
    bRtn = DmRegisterPatternReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DM register handler failed!\n");
        DM_FREE(pPara);

        return DM_FALSE;
    }

    DM_FREE(pPara);

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave MeascollecfileDmPatternRegisterHandler\n");
    return DM_TRUE;
}

