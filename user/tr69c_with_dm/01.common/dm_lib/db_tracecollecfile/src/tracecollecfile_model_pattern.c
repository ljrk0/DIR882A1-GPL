

#include "dm_internal.h"
#include "tracecollecfile_model_def.h"

#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif


/* the declaration of data-valid-check function list */
extern BOOL8 TracecollecfileDataValidChecker(UINT8 ucParaCType, INT8 *pValueRange, INT8 *pValue);

extern BOOL8 DmDataStringLengthValidCheck(INT8 *pValueRange, INT8 *pValue);


/* the declaration of traceCollecFile data-type and c-type str-functions */
extern INT8* TracecollecfileGetCTypeStr(UINT8 ucParaCType);
extern INT8* TracecollecfileGetDataTypeStr(UINT8 ucParaType);


/* the declaration of traceCollecFile model register functions */
extern BOOL8 TracecollecfileDmPatternRegisterHandler();


/* the declaration of is-object-function */
extern BOOL8 TracecollecfileIsObject(UINT8 ucParaType);


/* the declaration of convert-fuctions */
extern BOOL8 DmConvertCharArrayToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
extern BOOL8 DmConvertStringToCharArray(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);


DM_NODE_INFO_T g_TracecollecfileModelNodes[]=
{
    {
        /* Name */
        (INT8*)"traceCollecFile.",

        /* RealName */
        (INT8*)"traceCollecFile",

        /* Description */
        (INT8*)"",

        /* ParaType */
        TRACECOLLECFILE_DATA_TYPE_OBJECT,

        /* CType */
        TRACECOLLECFILE_C_TYPE_STRUCT,

        /* CType Offset */
        0,

        /* CTypeSize */
        sizeof(TRACECOLLECFILE_T),

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
        (INT8*)"traceCollecFile.fileHeader.",

        /* RealName */
        (INT8*)"fileHeader",

        /* Description */
        (INT8*)"",

        /* ParaType */
        TRACECOLLECFILE_DATA_TYPE_OBJECT,

        /* CType */
        TRACECOLLECFILE_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(TRACECOLLECFILE_T,fileHeader),

        /* CTypeSize */
        sizeof(TRACECOLLECFILE_FILEHEADER_T),

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
        (INT8*)"fileFormatVersion",

        /* defaultXmlAttrValue */
        (INT8*)"fileFormatVersion=\"32.423 V6.0\"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

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
        TRACECOLLECFILE_DATA_TYPE_STRING,

        /* CType */
        TRACECOLLECFILE_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(TRACECOLLECFILE_FILEHEADER_T,fileSender),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        TRACECOLLECFILE_FILEHEADER_FILESENDER_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(TRACECOLLECFILE_FILEHEADER_T,fileSenderCnt),

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
        (INT8*)"traceCollec",

        /* RealName */
        (INT8*)"traceCollec",

        /* Description */
        (INT8*)"",

        /* ParaType */
        TRACECOLLECFILE_DATA_TYPE_STRING,

        /* CType */
        TRACECOLLECFILE_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(TRACECOLLECFILE_FILEHEADER_T,traceCollec),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        TRACECOLLECFILE_FILEHEADER_TRACECOLLEC_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(TRACECOLLECFILE_FILEHEADER_T,traceCollecCnt),

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
        (INT8*)"traceCollecFile.traceRecSession.",

        /* RealName */
        (INT8*)"traceRecSession",

        /* Description */
        (INT8*)"",

        /* ParaType */
        TRACECOLLECFILE_DATA_TYPE_OBJECT,

        /* CType */
        TRACECOLLECFILE_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(TRACECOLLECFILE_T,traceRecSession),

        /* CTypeSize */
        sizeof(TRACECOLLECFILE_TRACERECSESSION_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        0,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        5,

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
        (INT8*)"traceRecSessionRef",

        /* defaultXmlAttrValue */
        (INT8*)"",

        /* XmlAttrNotifyEntities */
        (INT8*)"",

        /* NodeHandler */
        NULL,

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
        (INT8*)"traceCollecFile.traceRecSession.traceSessionRef.",

        /* RealName */
        (INT8*)"traceSessionRef",

        /* Description */
        (INT8*)"",

        /* ParaType */
        TRACECOLLECFILE_DATA_TYPE_OBJECT,

        /* CType */
        TRACECOLLECFILE_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(TRACECOLLECFILE_TRACERECSESSION_T,traceSessionRef),

        /* CTypeSize */
        sizeof(TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_T),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        4,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        6,

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
        (INT8*)"MCC",

        /* RealName */
        (INT8*)"MCC",

        /* Description */
        (INT8*)"",

        /* ParaType */
        TRACECOLLECFILE_DATA_TYPE_STRING,

        /* CType */
        TRACECOLLECFILE_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_T,MCC),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_MCC_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_T,MCCCnt),

        /* ParentIndex */
        5,

        /* SiblingIndex */
        7,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"3",

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
        (INT8*)"MNC",

        /* RealName */
        (INT8*)"MNC",

        /* Description */
        (INT8*)"",

        /* ParaType */
        TRACECOLLECFILE_DATA_TYPE_STRING,

        /* CType */
        TRACECOLLECFILE_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_T,MNC),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_MNC_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_T,MNCCnt),

        /* ParentIndex */
        5,

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
        (INT8*)"3",

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
        (INT8*)"TRACE_ID",

        /* RealName */
        (INT8*)"TRACE_ID",

        /* Description */
        (INT8*)"",

        /* ParaType */
        TRACECOLLECFILE_DATA_TYPE_STRING,

        /* CType */
        TRACECOLLECFILE_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_T,TRACE_ID),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_TRACE_ID_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_T,TRACE_IDCnt),

        /* ParentIndex */
        5,

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
        (INT8*)"6",

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
        NULL,

        /* RealName */
        NULL,

        /* Description */
        NULL,

        /* ParaType */
        TRACECOLLECFILE_DATA_TYPE_OBJECT,

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


/* traceCollecFile model access list */
DB_ACCESS_NODE_T g_TracecollecfileDbAccessList[] =
{
    {"ROOT", 0xffff},
    {"TR069", 0x1},
    {"GUI", 0x2},
    {"SNMP", 0x4},
    {"PS", 0x8},
    {NULL, 0}
};



/* traceCollecFile model config node */
DB_CONFIG_T g_TracecollecfileConfigNode =
{
    "traceCollecFile",        /* DB name */
    "18892",        /* DB version */
    DM_TRUE,        /* if it is extended db model */
    "./traceCollecFile.xml",         /* working path */
    "./traceCollecFile.xml",         /* loading path */
    "./traceCollecFile.xml",         /* backup path */
    "./traceCollecFile_mapping.xml",         /* cmfile-mapping path */
    TracecollecfileDmPatternRegisterHandler,         /* pattern register function */
    TracecollecfileDataValidChecker,          /* default data-valid-check function */
    TracecollecfileGetCTypeStr,          /* default get c-type string function */
    TracecollecfileGetDataTypeStr,          /* default get data-type string function */
    TracecollecfileIsObject,          /* to check if it is object */
    g_TracecollecfileDbAccessList,         /* access list */
    NULL         /* libdl.so handler */
};

DM_ATTR_INFO_T g_TracecollecfileAttributes[] =
{
    {
        /* attribute name */
        "fileFormatVersion",

        /* attribute description */
        "",

        /* created by default */
        1,

        /* attribute ctype */
        TRACECOLLECFILE_C_TYPE_INT8_ARRAY,

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
        TRACECOLLECFILE_C_TYPE_INT8_ARRAY,

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
        "traceRecSessionRef",

        /* attribute description */
        "",

        /* created by default */
        1,

        /* attribute ctype */
        TRACECOLLECFILE_C_TYPE_INT8_ARRAY,

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



/* get traceCollecFile model config node */
INT32 TracecollecfileDbLoadReq(void *pDlHandler, BOOL8 bDbReset, INT8 *pWorkPath, INT8 *pLoadPath, INT8 *pBackupPath)
{
    return DmLoadDbReq(&g_TracecollecfileConfigNode, pDlHandler, bDbReset, pWorkPath, pLoadPath, pBackupPath);
}


/* traceCollecFile model data-valid-check function */
BOOL8 TracecollecfileDataValidChecker(UINT8 ucParaCType, INT8 *pValueRange, INT8 *pValue)
{
    BOOL8 bRet = DM_FALSE;

    switch(ucParaCType)
    {
        case TRACECOLLECFILE_C_TYPE_INT8_ARRAY:
            bRet = DmDataStringLengthValidCheck(pValueRange, pValue);
            break;
        case TRACECOLLECFILE_C_TYPE_STRUCT:
            bRet = DM_TRUE;
            break;
        default:
            break;
    }

    return bRet;
}


/* traceCollecFile model c-type string function */
INT8* TracecollecfileGetCTypeStr(UINT8 ucParaCType)
{
    INT8 *pTypeStr = "";

    switch(ucParaCType)
    {
        case TRACECOLLECFILE_C_TYPE_INT8_ARRAY:
            pTypeStr = "INT8[]";
            break;
        case TRACECOLLECFILE_C_TYPE_STRUCT:
            pTypeStr = "STRUCT";
            break;
        default:
            break;
    }

    return pTypeStr;
}


/* traceCollecFile model data-type string function */
INT8* TracecollecfileGetDataTypeStr(UINT8 ucParaType)
{
    INT8 *pTypeStr = "";

    switch(ucParaType)
    {
        case TRACECOLLECFILE_DATA_TYPE_BASE64:
            pTypeStr = "base64";
            break;
        case TRACECOLLECFILE_DATA_TYPE_BOOLEAN:
            pTypeStr = "boolean";
            break;
        case TRACECOLLECFILE_DATA_TYPE_DATETIME:
            pTypeStr = "dateTime";
            break;
        case TRACECOLLECFILE_DATA_TYPE_HEXBINARY:
            pTypeStr = "hexBinary";
            break;
        case TRACECOLLECFILE_DATA_TYPE_INT:
            pTypeStr = "int";
            break;
        case TRACECOLLECFILE_DATA_TYPE_LONG:
            pTypeStr = "long";
            break;
        case TRACECOLLECFILE_DATA_TYPE_OBJECT:
            pTypeStr = "object";
            break;
        case TRACECOLLECFILE_DATA_TYPE_STRING:
            pTypeStr = "string";
            break;
        case TRACECOLLECFILE_DATA_TYPE_UNSIGNEDINT:
            pTypeStr = "unsignedInt";
            break;
        case TRACECOLLECFILE_DATA_TYPE_UNSIGNEDLONG:
            pTypeStr = "unsignedLong";
            break;
        default:
            break;
    }

    return pTypeStr;
}


/* traceCollecFile model object-type-check function */
BOOL8 TracecollecfileIsObject(UINT8 ucParaType)
{
    if (ucParaType == TRACECOLLECFILE_DATA_TYPE_OBJECT)
    {
        return DM_TRUE;
    }
    else
    {
        return DM_FALSE;
    }
}


/* traceCollecFile model pattern register function */
BOOL8 TracecollecfileDmPatternRegisterHandler()
{
    DM_REG_PATTERN_REQ_T            *pPara;
    BOOL8                           bRtn;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter TracecollecfileDmPatternRegisterHandler\n");
    pPara = (DM_REG_PATTERN_REQ_T *)malloc(sizeof(DM_REG_PATTERN_REQ_T));

    if(pPara == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ TracecollecfileDmPatternRegisterHandler malloc() failed for pPara\n");
        bRtn = DM_FALSE;
        return bRtn;
    }

    osa_mem_set(pPara,0,sizeof(DM_REG_PATTERN_REQ_T));

    pPara->pucRootName = (INT8 *)"traceCollecFile.";
    pPara->pPatternFirstNode = (DM_NODE_INFO_T *)g_TracecollecfileModelNodes;
    pPara->lPatternSize = 9;
    pPara->pAttrPatternFirstNode = (DM_ATTR_INFO_T *)g_TracecollecfileAttributes;
    bRtn = DmRegisterPatternReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DM register handler failed!\n");
        DM_FREE(pPara);

        return DM_FALSE;
    }

    DM_FREE(pPara);

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave TracecollecfileDmPatternRegisterHandler\n");
    return DM_TRUE;
}

