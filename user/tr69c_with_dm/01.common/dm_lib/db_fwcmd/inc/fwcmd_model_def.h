#ifndef _FWCMD_MODEL_DEF_H_
#define _FWCMD_MODEL_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "common_type_def.h"

/* the declaration of external header files if any used */


/* the definition of data type */
#define FWCMD_DATA_TYPE_BASE64                    0x0           /* base64 */
#define FWCMD_DATA_TYPE_BOOLEAN                   0x1           /* boolean */
#define FWCMD_DATA_TYPE_DATETIME                  0x2           /* dateTime */
#define FWCMD_DATA_TYPE_HEXBINARY                 0x3           /* hexBinary */
#define FWCMD_DATA_TYPE_INT                       0x4           /* int */
#define FWCMD_DATA_TYPE_LONG                      0x5           /* long */
#define FWCMD_DATA_TYPE_OBJECT                    0x6           /* object */
#define FWCMD_DATA_TYPE_STRING                    0x7           /* string */
#define FWCMD_DATA_TYPE_UNSIGNEDINT               0x8           /* unsignedInt */
#define FWCMD_DATA_TYPE_UNSIGNEDLONG              0x9           /* unsignedLong */


/* the definition of c type */
#define FWCMD_C_TYPE_BOOL8                                        0x0           /* BOOL8 */
#define FWCMD_C_TYPE_CM_HOST_NAME_OR_IP_ADDR_T                    0x1           /* CM_HOST_NAME_OR_IP_ADDR_T */
#define FWCMD_C_TYPE_CM_HOST_NAME_OR_IP_ADDR_T_ARRAY              0x2           /* CM_HOST_NAME_OR_IP_ADDR_T[] */
#define FWCMD_C_TYPE_CM_IMSI_T                                    0x3           /* CM_IMSI_T */
#define FWCMD_C_TYPE_CM_IMSI_T_ARRAY                              0x4           /* CM_IMSI_T[] */
#define FWCMD_C_TYPE_CM_IP_ADDR_T                                 0x5           /* CM_IP_ADDR_T */
#define FWCMD_C_TYPE_CM_IP_ADDR_T_ARRAY                           0x6           /* CM_IP_ADDR_T[] */
#define FWCMD_C_TYPE_CM_PLMN_ID_T                                 0x7           /* CM_PLMN_ID_T */
#define FWCMD_C_TYPE_CM_PLMN_ID_T_ARRAY                           0x8           /* CM_PLMN_ID_T[] */
#define FWCMD_C_TYPE_DATE_TIME_T                                  0x9           /* DATE_TIME_T */
#define FWCMD_C_TYPE_INT16                                        0xa           /* INT16 */
#define FWCMD_C_TYPE_INT16_ARRAY                                  0xb           /* INT16[] */
#define FWCMD_C_TYPE_INT32                                        0xc           /* INT32 */
#define FWCMD_C_TYPE_INT64                                        0xd           /* INT64 */
#define FWCMD_C_TYPE_INT8                                         0xe           /* INT8 */
#define FWCMD_C_TYPE_INT8_ARRAY                                   0xf           /* INT8[] */
#define FWCMD_C_TYPE_INT8A_ARRAY                                  0x10          /* INT8A[] */
#define FWCMD_C_TYPE_STRUCT                                       0x11          /* STRUCT */
#define FWCMD_C_TYPE_UINT16                                       0x12          /* UINT16 */
#define FWCMD_C_TYPE_UINT16_ARRAY                                 0x13          /* UINT16[] */
#define FWCMD_C_TYPE_UINT32                                       0x14          /* UINT32 */
#define FWCMD_C_TYPE_UINT32_ARRAY                                 0x15          /* UINT32[] */
#define FWCMD_C_TYPE_UINT32_BITMAP                                0x16          /* UINT32_BITMAP */
#define FWCMD_C_TYPE_UINT32_ENUM                                  0x17          /* UINT32_ENUM */
#define FWCMD_C_TYPE_UINT32_HEX                                   0x18          /* UINT32_HEX */
#define FWCMD_C_TYPE_UINT64                                       0x19          /* UINT64 */
#define FWCMD_C_TYPE_UINT8                                        0x1a          /* UINT8 */
#define FWCMD_C_TYPE_UINT8_ARRAY                                  0x1b          /* UINT8[] */


#ifndef _INT8A
#define _INT8A
typedef INT8 INT8A;
#endif


#ifndef _UINT32_HEX
#define _UINT32_HEX
typedef UINT32 UINT32_HEX;
#endif


#ifndef _UINT32_ENUM
#define _UINT32_ENUM
typedef UINT32 UINT32_ENUM;
#endif


#ifndef _UINT32_BITMAP
#define _UINT32_BITMAP
typedef UINT32 UINT32_BITMAP;
#endif




/* the definition of parameter id */
#define FWCMD_ID                                  0x0      /* Fwcmd. */
#define FWCMD_MAINVERSION_ID                      0x1      /* Fwcmd.MainVersion */
#define FWCMD_VENDORNAME_ID                       0x2      /* Fwcmd.VendorName */
#define FWCMD_DESCRIPTION_ID                      0x3      /* Fwcmd.Description */
#define FWCMD_MODULENUMBEROFENTRIES_ID            0x4      /* Fwcmd.ModuleNumberOfEntries */
#define FWCMD_MODULE_ID                           0x5      /* Fwcmd.Module.{i}. */
#define FWCMD_MODULE_NAME_ID                      0x6      /* Fwcmd.Module.{i}.Name */
#define FWCMD_MODULE_VERSION_ID                   0x7      /* Fwcmd.Module.{i}.Version */
#define FWCMD_MODULE_OPERATION_ID                 0x8      /* Fwcmd.Module.{i}.Operation */
#define FWCMD_MODULE_SUBMODULENUMBEROFENTRIES_ID  0x9      /* Fwcmd.Module.{i}.SubModuleNumberOfEntries */
#define FWCMD_MODULE_SUBMODULE_ID                 0xa      /* Fwcmd.Module.{i}.SubModule.{i}. */
#define FWCMD_MODULE_SUBMODULE_NAME_ID            0xb      /* Fwcmd.Module.{i}.SubModule.{i}.Name */
#define FWCMD_MODULE_SUBMODULE_VERSION_ID         0xc      /* Fwcmd.Module.{i}.SubModule.{i}.Version */
#define FWCMD_MODULE_SUBMODULE_FILETYPE_ID        0xd      /* Fwcmd.Module.{i}.SubModule.{i}.FileType */
#define FWCMD_MODULE_SUBMODULE_PATH_ID            0xe      /* Fwcmd.Module.{i}.SubModule.{i}.Path */
#define FWCMD_MODULE_SUBMODULE_COMMAND_ID         0xf      /* Fwcmd.Module.{i}.SubModule.{i}.Command */
#define FWCMD_MODULE_SUBMODULE_RUNLEVEL_ID        0x10     /* Fwcmd.Module.{i}.SubModule.{i}.RunLevel */
#define FWCMD_MODULE_SUBMODULE_INSTALLMETHOD_ID   0x11     /* Fwcmd.Module.{i}.SubModule.{i}.InstallMethod */
#define FWCMD_MODULE_SUBMODULE_UNINSTALLMETHOD_ID 0x12     /* Fwcmd.Module.{i}.SubModule.{i}.UninstallMethod */
#define FWCMD_MODULE_SUBMODULE_SIGNATUREALGO_ID   0x13     /* Fwcmd.Module.{i}.SubModule.{i}.SignatureAlgo */
#define FWCMD_MODULE_SUBMODULE_SIGNATURE_ID       0x14     /* Fwcmd.Module.{i}.SubModule.{i}.Signature */


/* the definition of c structure */
typedef struct fwcmd_module_submodule_t{
    UINT32    instanceNum;

/* #define FWCMD_MODULE_SUBMODULE_NAME_ID 0xb */
#define FWCMD_MODULE_SUBMODULE_NAME_CNT 0x21
    UINT32    NameCnt;
    INT8      Name[FWCMD_MODULE_SUBMODULE_NAME_CNT];

/* #define FWCMD_MODULE_SUBMODULE_VERSION_ID 0xc */
#define FWCMD_MODULE_SUBMODULE_VERSION_CNT 0x9
    UINT32    VersionCnt;
    INT8      Version[FWCMD_MODULE_SUBMODULE_VERSION_CNT];

/* #define FWCMD_MODULE_SUBMODULE_FILETYPE_ID 0xd */
#define FWCMD_MODULE_SUBMODULE_FILETYPE_SINGLEFILE  0x0 
#define FWCMD_MODULE_SUBMODULE_FILETYPE_BOOTDATA    0x1 
#define FWCMD_MODULE_SUBMODULE_FILETYPE_KERNELIMAGE 0x2 
#define FWCMD_MODULE_SUBMODULE_FILETYPE_BOOTLOADER  0x3 
#define FWCMD_MODULE_SUBMODULE_FILETYPE_ROOTFS      0x4 
#define FWCMD_MODULE_SUBMODULE_FILETYPE_OAM         0x5 
#define FWCMD_MODULE_SUBMODULE_FILETYPE_TARBALLCONF 0x6 
    UINT32    FileType;

/* #define FWCMD_MODULE_SUBMODULE_PATH_ID 0xe */
#define FWCMD_MODULE_SUBMODULE_PATH_CNT 0x81
    UINT32    PathCnt;
    INT8      Path[FWCMD_MODULE_SUBMODULE_PATH_CNT];

/* #define FWCMD_MODULE_SUBMODULE_COMMAND_ID 0xf */
#define FWCMD_MODULE_SUBMODULE_COMMAND_CNT 0x101
    UINT32    CommandCnt;
    INT8      Command[FWCMD_MODULE_SUBMODULE_COMMAND_CNT];

/* #define FWCMD_MODULE_SUBMODULE_RUNLEVEL_ID 0x10 */
    UINT32    RunLevel;

/* #define FWCMD_MODULE_SUBMODULE_INSTALLMETHOD_ID 0x11 */
#define FWCMD_MODULE_SUBMODULE_INSTALLMETHOD_CNT 0x101
    UINT32    InstallMethodCnt;
    INT8      InstallMethod[FWCMD_MODULE_SUBMODULE_INSTALLMETHOD_CNT];

/* #define FWCMD_MODULE_SUBMODULE_UNINSTALLMETHOD_ID 0x12 */
#define FWCMD_MODULE_SUBMODULE_UNINSTALLMETHOD_CNT 0x101
    UINT32    UninstallMethodCnt;
    INT8      UninstallMethod[FWCMD_MODULE_SUBMODULE_UNINSTALLMETHOD_CNT];

/* #define FWCMD_MODULE_SUBMODULE_SIGNATUREALGO_ID 0x13 */
#define FWCMD_MODULE_SUBMODULE_SIGNATUREALGO_CNT 0x9
    UINT32    SignatureAlgoCnt;
    INT8      SignatureAlgo[FWCMD_MODULE_SUBMODULE_SIGNATUREALGO_CNT];

/* #define FWCMD_MODULE_SUBMODULE_SIGNATURE_ID 0x14 */
#define FWCMD_MODULE_SUBMODULE_SIGNATURE_CNT 0x401
    UINT32    SignatureCnt;
    INT8      Signature[FWCMD_MODULE_SUBMODULE_SIGNATURE_CNT];

}FWCMD_MODULE_SUBMODULE_T;


typedef struct fwcmd_module_t{
    UINT32                      instanceNum;

/* #define FWCMD_MODULE_NAME_ID 0x6 */
#define FWCMD_MODULE_NAME_CNT 0x21
    UINT32                      NameCnt;
    INT8                        Name[FWCMD_MODULE_NAME_CNT];

/* #define FWCMD_MODULE_VERSION_ID 0x7 */
#define FWCMD_MODULE_VERSION_CNT 0x9
    UINT32                      VersionCnt;
    INT8                        Version[FWCMD_MODULE_VERSION_CNT];

/* #define FWCMD_MODULE_OPERATION_ID 0x8 */
#define FWCMD_MODULE_OPERATION_INSTALL   0x0 
#define FWCMD_MODULE_OPERATION_UNINSTALL 0x1 
    UINT32                      Operation;

/* #define FWCMD_MODULE_SUBMODULENUMBEROFENTRIES_ID 0x9 */
    UINT32                      SubModuleNumberOfEntries;

/* #define FWCMD_MODULE_SUBMODULE_ID 0xa */
#define FWCMD_MODULE_SUBMODULE_CNT 0x14
    UINT32                      SubModule_cnt;
    FWCMD_MODULE_SUBMODULE_T    SubModule[FWCMD_MODULE_SUBMODULE_CNT];

}FWCMD_MODULE_T;


typedef struct fwcmd_t{
/* #define FWCMD_MAINVERSION_ID 0x1 */
#define FWCMD_MAINVERSION_CNT 0x41
    UINT32            MainVersionCnt;
    INT8              MainVersion[FWCMD_MAINVERSION_CNT];

/* #define FWCMD_VENDORNAME_ID 0x2 */
#define FWCMD_VENDORNAME_CNT 0x41
    UINT32            VendorNameCnt;
    INT8              VendorName[FWCMD_VENDORNAME_CNT];

/* #define FWCMD_DESCRIPTION_ID 0x3 */
#define FWCMD_DESCRIPTION_CNT 0x101
    UINT32            DescriptionCnt;
    INT8              Description[FWCMD_DESCRIPTION_CNT];

/* #define FWCMD_MODULENUMBEROFENTRIES_ID 0x4 */
    UINT32            ModuleNumberOfEntries;

/* #define FWCMD_MODULE_ID 0x5 */
#define FWCMD_MODULE_CNT 0x1
    UINT32            Module_cnt;
    FWCMD_MODULE_T    Module[FWCMD_MODULE_CNT];

}FWCMD_T;




/* the declaration of Fwcmd model loading function */
extern INT32 FwcmdDbLoadReq(void *pDlHandler, BOOL8 bDbReset, INT8 *pWorkPath, INT8 *pLoadPath, INT8 *pBackupPath);


#ifdef __cplusplus
}
#endif

#endif
