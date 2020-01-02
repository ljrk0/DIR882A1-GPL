

#include "dm_internal.h"
#include "fwcmd_model_def.h"

#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif


/* the declaration of data-valid-check function list */
extern BOOL8 FwcmdDataValidChecker(UINT8 ucParaCType, INT8 *pValueRange, INT8 *pValue);

extern BOOL8 DmDataStringLengthValidCheck(INT8 *pValueRange, INT8 *pValue);
extern BOOL8 DmDataUINT32ValidCheck(INT8 *pValueRange, INT8 *pValue);
extern BOOL8 DmEnumStringValidCheck(INT8 *pValueRange, INT8 *pValue);


/* the declaration of Fwcmd data-type and c-type str-functions */
extern INT8* FwcmdGetCTypeStr(UINT8 ucParaCType);
extern INT8* FwcmdGetDataTypeStr(UINT8 ucParaType);


/* the declaration of Fwcmd model register functions */
extern BOOL8 FwcmdDmPatternRegisterHandler();


/* the declaration of is-object-function */
extern BOOL8 FwcmdIsObject(UINT8 ucParaType);


/* the declaration of convert-fuctions */
extern BOOL8 DmConvertEnumToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
extern BOOL8 DmConvertStringToEnum(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
extern BOOL8 DmConvertCharArrayToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
extern BOOL8 DmConvertStringToCharArray(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
extern BOOL8 DmConvertUint32ToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
extern BOOL8 DmConvertStringToUint32(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);


DM_NODE_INFO_T g_FwcmdModelNodes[]=
{
    {
        /* Name */
        (INT8*)"Fwcmd.",

        /* RealName */
        (INT8*)"Fwcmd",

        /* Description */
        (INT8*)"Application configuration",

        /* ParaType */
        FWCMD_DATA_TYPE_OBJECT,

        /* CType */
        FWCMD_C_TYPE_STRUCT,

        /* CType Offset */
        0,

        /* CTypeSize */
        sizeof(FWCMD_T),

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
        5,

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
        (INT8*)"MainVersion",

        /* RealName */
        (INT8*)"MainVersion",

        /* Description */
        (INT8*)"main version",

        /* ParaType */
        FWCMD_DATA_TYPE_STRING,

        /* CType */
        FWCMD_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(FWCMD_T,MainVersion),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        FWCMD_MAINVERSION_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(FWCMD_T,MainVersionCnt),

        /* ParentIndex */
        0,

        /* SiblingIndex */
        2,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"64",

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
        (INT8*)"VendorName",

        /* RealName */
        (INT8*)"VendorName",

        /* Description */
        (INT8*)"vendor name",

        /* ParaType */
        FWCMD_DATA_TYPE_STRING,

        /* CType */
        FWCMD_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(FWCMD_T,VendorName),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        FWCMD_VENDORNAME_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(FWCMD_T,VendorNameCnt),

        /* ParentIndex */
        0,

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
        (INT8*)"64",

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
        (INT8*)"Description",

        /* RealName */
        (INT8*)"Description",

        /* Description */
        (INT8*)"the description about this version",

        /* ParaType */
        FWCMD_DATA_TYPE_STRING,

        /* CType */
        FWCMD_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(FWCMD_T,Description),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        FWCMD_DESCRIPTION_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(FWCMD_T,DescriptionCnt),

        /* ParentIndex */
        0,

        /* SiblingIndex */
        4,

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
        (INT8*)"ModuleNumberOfEntries",

        /* RealName */
        (INT8*)"ModuleNumberOfEntries",

        /* Description */
        (INT8*)"number of modules",

        /* ParaType */
        FWCMD_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        FWCMD_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(FWCMD_T,ModuleNumberOfEntries),

        /* CTypeSize */
        sizeof(UINT32),

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
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"Fwcmd.Module.{i}.",

        /* RealName */
        (INT8*)"Module",

        /* Description */
        (INT8*)"module entity info",

        /* ParaType */
        FWCMD_DATA_TYPE_OBJECT,

        /* CType */
        FWCMD_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(FWCMD_T,Module),

        /* CTypeSize */
        sizeof(FWCMD_MODULE_T),

        /* MaxElements */
        FWCMD_MODULE_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(FWCMD_T,Module_cnt),

        /* ParentIndex */
        0,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        6,

        /* ChildrenCounter */
        5,

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
        (INT8*)"the name of module",

        /* ParaType */
        FWCMD_DATA_TYPE_STRING,

        /* CType */
        FWCMD_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(FWCMD_MODULE_T,Name),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        FWCMD_MODULE_NAME_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(FWCMD_MODULE_T,NameCnt),

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
        (INT8*)"32",

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
        (INT8*)"Version",

        /* RealName */
        (INT8*)"Version",

        /* Description */
        (INT8*)"the version of this module",

        /* ParaType */
        FWCMD_DATA_TYPE_STRING,

        /* CType */
        FWCMD_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(FWCMD_MODULE_T,Version),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        FWCMD_MODULE_VERSION_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(FWCMD_MODULE_T,VersionCnt),

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
        (INT8*)"8",

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
        (INT8*)"Operation",

        /* RealName */
        (INT8*)"Operation",

        /* Description */
        (INT8*)"install/uninstall",

        /* ParaType */
        FWCMD_DATA_TYPE_STRING,

        /* CType */
        FWCMD_C_TYPE_UINT32_ENUM,

        /* CType Offset */
        DM_OFFSETOF(FWCMD_MODULE_T,Operation),

        /* CTypeSize */
        sizeof(UINT32_ENUM),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        5,

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
        (INT8*)"Install,Uninstall",

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
        DmConvertEnumToString,

        /* ToStructFunc */
        DmConvertStringToEnum
    },

    {
        /* Name */
        (INT8*)"SubModuleNumberOfEntries",

        /* RealName */
        (INT8*)"SubModuleNumberOfEntries",

        /* Description */
        (INT8*)"number of modules",

        /* ParaType */
        FWCMD_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        FWCMD_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(FWCMD_MODULE_T,SubModuleNumberOfEntries),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        5,

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
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"Fwcmd.Module.{i}.SubModule.{i}.",

        /* RealName */
        (INT8*)"SubModule",

        /* Description */
        (INT8*)"module entity info",

        /* ParaType */
        FWCMD_DATA_TYPE_OBJECT,

        /* CType */
        FWCMD_C_TYPE_STRUCT,

        /* CType Offset */
        DM_OFFSETOF(FWCMD_MODULE_T,SubModule),

        /* CTypeSize */
        sizeof(FWCMD_MODULE_SUBMODULE_T),

        /* MaxElements */
        FWCMD_MODULE_SUBMODULE_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(FWCMD_MODULE_T,SubModule_cnt),

        /* ParentIndex */
        5,

        /* SiblingIndex */
        -1,

        /* FirstChildIndex */
        11,

        /* ChildrenCounter */
        10,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"20",

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
        (INT8*)"Name",

        /* RealName */
        (INT8*)"Name",

        /* Description */
        (INT8*)"the name of module",

        /* ParaType */
        FWCMD_DATA_TYPE_STRING,

        /* CType */
        FWCMD_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(FWCMD_MODULE_SUBMODULE_T,Name),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        FWCMD_MODULE_SUBMODULE_NAME_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(FWCMD_MODULE_SUBMODULE_T,NameCnt),

        /* ParentIndex */
        10,

        /* SiblingIndex */
        12,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"32",

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
        (INT8*)"Version",

        /* RealName */
        (INT8*)"Version",

        /* Description */
        (INT8*)"the version of this module",

        /* ParaType */
        FWCMD_DATA_TYPE_STRING,

        /* CType */
        FWCMD_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(FWCMD_MODULE_SUBMODULE_T,Version),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        FWCMD_MODULE_SUBMODULE_VERSION_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(FWCMD_MODULE_SUBMODULE_T,VersionCnt),

        /* ParentIndex */
        10,

        /* SiblingIndex */
        13,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"8",

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
        (INT8*)"FileType",

        /* RealName */
        (INT8*)"FileType",

        /* Description */
        (INT8*)"file type, maybe single-file, bootdata, kernel, or bootloader",

        /* ParaType */
        FWCMD_DATA_TYPE_STRING,

        /* CType */
        FWCMD_C_TYPE_UINT32_ENUM,

        /* CType Offset */
        DM_OFFSETOF(FWCMD_MODULE_SUBMODULE_T,FileType),

        /* CTypeSize */
        sizeof(UINT32_ENUM),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        10,

        /* SiblingIndex */
        14,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"SingleFile,BootData,KernelImage,BootLoader,RootFS,OAM,TarballConf",

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
        DmConvertEnumToString,

        /* ToStructFunc */
        DmConvertStringToEnum
    },

    {
        /* Name */
        (INT8*)"Path",

        /* RealName */
        (INT8*)"Path",

        /* Description */
        (INT8*)"where to find the binary or dynamic-library of this module",

        /* ParaType */
        FWCMD_DATA_TYPE_STRING,

        /* CType */
        FWCMD_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(FWCMD_MODULE_SUBMODULE_T,Path),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        FWCMD_MODULE_SUBMODULE_PATH_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(FWCMD_MODULE_SUBMODULE_T,PathCnt),

        /* ParentIndex */
        10,

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
        (INT8*)"Command",

        /* RealName */
        (INT8*)"Command",

        /* Description */
        (INT8*)"the command to start the module. 1) thread: handle=dlopen(dynamic-lib-path);func=dlsym(handle, SmStartReq); 2) process: ./tr69c -f LAN -o",

        /* ParaType */
        FWCMD_DATA_TYPE_STRING,

        /* CType */
        FWCMD_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(FWCMD_MODULE_SUBMODULE_T,Command),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        FWCMD_MODULE_SUBMODULE_COMMAND_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(FWCMD_MODULE_SUBMODULE_T,CommandCnt),

        /* ParentIndex */
        10,

        /* SiblingIndex */
        16,

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
        (INT8*)"RunLevel",

        /* RealName */
        (INT8*)"RunLevel",

        /* Description */
        (INT8*)"Run level for this item",

        /* ParaType */
        FWCMD_DATA_TYPE_UNSIGNEDINT,

        /* CType */
        FWCMD_C_TYPE_UINT32,

        /* CType Offset */
        DM_OFFSETOF(FWCMD_MODULE_SUBMODULE_T,RunLevel),

        /* CTypeSize */
        sizeof(UINT32),

        /* MaxElements */
        0,

        /* ElementCntOffset */
        0,

        /* ParentIndex */
        10,

        /* SiblingIndex */
        17,

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
        DmConvertUint32ToString,

        /* ToStructFunc */
        DmConvertStringToUint32
    },

    {
        /* Name */
        (INT8*)"InstallMethod",

        /* RealName */
        (INT8*)"InstallMethod",

        /* Description */
        (INT8*)"Install Method for this item",

        /* ParaType */
        FWCMD_DATA_TYPE_STRING,

        /* CType */
        FWCMD_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(FWCMD_MODULE_SUBMODULE_T,InstallMethod),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        FWCMD_MODULE_SUBMODULE_INSTALLMETHOD_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(FWCMD_MODULE_SUBMODULE_T,InstallMethodCnt),

        /* ParentIndex */
        10,

        /* SiblingIndex */
        18,

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
        (INT8*)"UninstallMethod",

        /* RealName */
        (INT8*)"UninstallMethod",

        /* Description */
        (INT8*)"UnInstall Method for this item",

        /* ParaType */
        FWCMD_DATA_TYPE_STRING,

        /* CType */
        FWCMD_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(FWCMD_MODULE_SUBMODULE_T,UninstallMethod),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        FWCMD_MODULE_SUBMODULE_UNINSTALLMETHOD_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(FWCMD_MODULE_SUBMODULE_T,UninstallMethodCnt),

        /* ParentIndex */
        10,

        /* SiblingIndex */
        19,

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
        (INT8*)"SignatureAlgo",

        /* RealName */
        (INT8*)"SignatureAlgo",

        /* Description */
        (INT8*)"md5 or others",

        /* ParaType */
        FWCMD_DATA_TYPE_STRING,

        /* CType */
        FWCMD_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(FWCMD_MODULE_SUBMODULE_T,SignatureAlgo),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        FWCMD_MODULE_SUBMODULE_SIGNATUREALGO_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(FWCMD_MODULE_SUBMODULE_T,SignatureAlgoCnt),

        /* ParentIndex */
        10,

        /* SiblingIndex */
        20,

        /* FirstChildIndex */
        -1,

        /* ChildrenCounter */
        0,

        /* ReadFlag */
        0x0,

        /* WriteFlag */
        0x0,

        /* ValueRange */
        (INT8*)"8",

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
        (INT8*)"Signature",

        /* RealName */
        (INT8*)"Signature",

        /* Description */
        (INT8*)"the signature of this image",

        /* ParaType */
        FWCMD_DATA_TYPE_STRING,

        /* CType */
        FWCMD_C_TYPE_INT8_ARRAY,

        /* CType Offset */
        DM_OFFSETOF(FWCMD_MODULE_SUBMODULE_T,Signature),

        /* CTypeSize */
        sizeof(INT8),

        /* MaxElements */
        FWCMD_MODULE_SUBMODULE_SIGNATURE_CNT,

        /* ElementCntOffset */
        DM_OFFSETOF(FWCMD_MODULE_SUBMODULE_T,SignatureCnt),

        /* ParentIndex */
        10,

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
        (INT8*)"1024",

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
        FWCMD_DATA_TYPE_OBJECT,

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


/* Fwcmd model access list */
DB_ACCESS_NODE_T g_FwcmdDbAccessList[] =
{
    {"ROOT", 0xffff},
    {"TR069", 0x1},
    {"GUI", 0x2},
    {"SNMP", 0x4},
    {"PS", 0x8},
    {NULL, 0}
};



/* Fwcmd model config node */
DB_CONFIG_T g_FwcmdConfigNode =
{
    "Fwcmd",        /* DB name */
    "19529",        /* DB version */
    DM_TRUE,        /* if it is extended db model */
    "./Fwcmd.xml",         /* working path */
    "./Fwcmd.xml",         /* loading path */
    "./Fwcmd.xml",         /* backup path */
    "./Fwcmd_mapping.xml",         /* cmfile-mapping path */
    FwcmdDmPatternRegisterHandler,         /* pattern register function */
    FwcmdDataValidChecker,          /* default data-valid-check function */
    FwcmdGetCTypeStr,          /* default get c-type string function */
    FwcmdGetDataTypeStr,          /* default get data-type string function */
    FwcmdIsObject,          /* to check if it is object */
    g_FwcmdDbAccessList,         /* access list */
    NULL         /* libdl.so handler */
};

DM_ATTR_INFO_T g_FwcmdAttributes[] =
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



/* get Fwcmd model config node */
INT32 FwcmdDbLoadReq(void *pDlHandler, BOOL8 bDbReset, INT8 *pWorkPath, INT8 *pLoadPath, INT8 *pBackupPath)
{
    return DmLoadDbReq(&g_FwcmdConfigNode, pDlHandler, bDbReset, pWorkPath, pLoadPath, pBackupPath);
}


/* Fwcmd model data-valid-check function */
BOOL8 FwcmdDataValidChecker(UINT8 ucParaCType, INT8 *pValueRange, INT8 *pValue)
{
    BOOL8 bRet = DM_FALSE;

    switch(ucParaCType)
    {
        case FWCMD_C_TYPE_UINT32_ENUM:
            bRet = DmEnumStringValidCheck(pValueRange, pValue);
            break;
        case FWCMD_C_TYPE_INT8_ARRAY:
            bRet = DmDataStringLengthValidCheck(pValueRange, pValue);
            break;
        case FWCMD_C_TYPE_STRUCT:
            bRet = DM_TRUE;
            break;
        case FWCMD_C_TYPE_UINT32:
            bRet = DmDataUINT32ValidCheck(pValueRange, pValue);
            break;
        default:
            break;
    }

    return bRet;
}


/* Fwcmd model c-type string function */
INT8* FwcmdGetCTypeStr(UINT8 ucParaCType)
{
    INT8 *pTypeStr = "";

    switch(ucParaCType)
    {
        case FWCMD_C_TYPE_INT8_ARRAY:
            pTypeStr = "INT8[]";
            break;
        case FWCMD_C_TYPE_STRUCT:
            pTypeStr = "STRUCT";
            break;
        case FWCMD_C_TYPE_UINT32:
            pTypeStr = "UINT32";
            break;
        case FWCMD_C_TYPE_UINT32_ENUM:
            pTypeStr = "UINT32_ENUM";
            break;
        default:
            break;
    }

    return pTypeStr;
}


/* Fwcmd model data-type string function */
INT8* FwcmdGetDataTypeStr(UINT8 ucParaType)
{
    INT8 *pTypeStr = "";

    switch(ucParaType)
    {
        case FWCMD_DATA_TYPE_BASE64:
            pTypeStr = "base64";
            break;
        case FWCMD_DATA_TYPE_BOOLEAN:
            pTypeStr = "boolean";
            break;
        case FWCMD_DATA_TYPE_DATETIME:
            pTypeStr = "dateTime";
            break;
        case FWCMD_DATA_TYPE_HEXBINARY:
            pTypeStr = "hexBinary";
            break;
        case FWCMD_DATA_TYPE_INT:
            pTypeStr = "int";
            break;
        case FWCMD_DATA_TYPE_LONG:
            pTypeStr = "long";
            break;
        case FWCMD_DATA_TYPE_OBJECT:
            pTypeStr = "object";
            break;
        case FWCMD_DATA_TYPE_STRING:
            pTypeStr = "string";
            break;
        case FWCMD_DATA_TYPE_UNSIGNEDINT:
            pTypeStr = "unsignedInt";
            break;
        case FWCMD_DATA_TYPE_UNSIGNEDLONG:
            pTypeStr = "unsignedLong";
            break;
        default:
            break;
    }

    return pTypeStr;
}


/* Fwcmd model object-type-check function */
BOOL8 FwcmdIsObject(UINT8 ucParaType)
{
    if (ucParaType == FWCMD_DATA_TYPE_OBJECT)
    {
        return DM_TRUE;
    }
    else
    {
        return DM_FALSE;
    }
}


/* Fwcmd model pattern register function */
BOOL8 FwcmdDmPatternRegisterHandler()
{
    DM_REG_PATTERN_REQ_T            *pPara;
    BOOL8                           bRtn;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter FwcmdDmPatternRegisterHandler\n");
    pPara = (DM_REG_PATTERN_REQ_T *)malloc(sizeof(DM_REG_PATTERN_REQ_T));

    if(pPara == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ FwcmdDmPatternRegisterHandler malloc() failed for pPara\n");
        bRtn = DM_FALSE;
        return bRtn;
    }

    osa_mem_set(pPara,0,sizeof(DM_REG_PATTERN_REQ_T));

    pPara->pucRootName = (INT8 *)"Fwcmd.";
    pPara->pPatternFirstNode = (DM_NODE_INFO_T *)g_FwcmdModelNodes;
    pPara->lPatternSize = 21;
    pPara->pAttrPatternFirstNode = (DM_ATTR_INFO_T *)g_FwcmdAttributes;
    bRtn = DmRegisterPatternReq(pPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DM register handler failed!\n");
        DM_FREE(pPara);

        return DM_FALSE;
    }

    DM_FREE(pPara);

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave FwcmdDmPatternRegisterHandler\n");
    return DM_TRUE;
}

