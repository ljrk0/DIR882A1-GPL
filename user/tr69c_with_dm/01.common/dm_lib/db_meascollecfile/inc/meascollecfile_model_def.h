#ifndef _MEASCOLLECFILE_MODEL_DEF_H_
#define _MEASCOLLECFILE_MODEL_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "common_type_def.h"

/* the declaration of external header files if any used */


/* the definition of data type */
#define MEASCOLLECFILE_DATA_TYPE_BASE64                             0x0           /* base64 */
#define MEASCOLLECFILE_DATA_TYPE_BOOLEAN                            0x1           /* boolean */
#define MEASCOLLECFILE_DATA_TYPE_DATETIME                           0x2           /* dateTime */
#define MEASCOLLECFILE_DATA_TYPE_HEXBINARY                          0x3           /* hexBinary */
#define MEASCOLLECFILE_DATA_TYPE_INT                                0x4           /* int */
#define MEASCOLLECFILE_DATA_TYPE_LONG                               0x5           /* long */
#define MEASCOLLECFILE_DATA_TYPE_OBJECT                             0x6           /* object */
#define MEASCOLLECFILE_DATA_TYPE_STRING                             0x7           /* string */
#define MEASCOLLECFILE_DATA_TYPE_UNSIGNEDINT                        0x8           /* unsignedInt */
#define MEASCOLLECFILE_DATA_TYPE_UNSIGNEDLONG                       0x9           /* unsignedLong */


/* the definition of c type */
#define MEASCOLLECFILE_C_TYPE_BOOL8                                                 0x0           /* BOOL8 */
#define MEASCOLLECFILE_C_TYPE_CM_HOST_NAME_OR_IP_ADDR_T                             0x1           /* CM_HOST_NAME_OR_IP_ADDR_T */
#define MEASCOLLECFILE_C_TYPE_CM_HOST_NAME_OR_IP_ADDR_T_ARRAY                       0x2           /* CM_HOST_NAME_OR_IP_ADDR_T[] */
#define MEASCOLLECFILE_C_TYPE_CM_IMSI_T                                             0x3           /* CM_IMSI_T */
#define MEASCOLLECFILE_C_TYPE_CM_IMSI_T_ARRAY                                       0x4           /* CM_IMSI_T[] */
#define MEASCOLLECFILE_C_TYPE_CM_IP_ADDR_T                                          0x5           /* CM_IP_ADDR_T */
#define MEASCOLLECFILE_C_TYPE_CM_IP_ADDR_T_ARRAY                                    0x6           /* CM_IP_ADDR_T[] */
#define MEASCOLLECFILE_C_TYPE_CM_PLMN_ID_T                                          0x7           /* CM_PLMN_ID_T */
#define MEASCOLLECFILE_C_TYPE_CM_PLMN_ID_T_ARRAY                                    0x8           /* CM_PLMN_ID_T[] */
#define MEASCOLLECFILE_C_TYPE_DATE_TIME_T                                           0x9           /* DATE_TIME_T */
#define MEASCOLLECFILE_C_TYPE_INT16                                                 0xa           /* INT16 */
#define MEASCOLLECFILE_C_TYPE_INT16_ARRAY                                           0xb           /* INT16[] */
#define MEASCOLLECFILE_C_TYPE_INT32                                                 0xc           /* INT32 */
#define MEASCOLLECFILE_C_TYPE_INT64                                                 0xd           /* INT64 */
#define MEASCOLLECFILE_C_TYPE_INT8                                                  0xe           /* INT8 */
#define MEASCOLLECFILE_C_TYPE_INT8_ARRAY                                            0xf           /* INT8[] */
#define MEASCOLLECFILE_C_TYPE_INT8A_ARRAY                                           0x10          /* INT8A[] */
#define MEASCOLLECFILE_C_TYPE_STRUCT                                                0x11          /* STRUCT */
#define MEASCOLLECFILE_C_TYPE_UINT16                                                0x12          /* UINT16 */
#define MEASCOLLECFILE_C_TYPE_UINT16_ARRAY                                          0x13          /* UINT16[] */
#define MEASCOLLECFILE_C_TYPE_UINT32                                                0x14          /* UINT32 */
#define MEASCOLLECFILE_C_TYPE_UINT32_ARRAY                                          0x15          /* UINT32[] */
#define MEASCOLLECFILE_C_TYPE_UINT32_BITMAP                                         0x16          /* UINT32_BITMAP */
#define MEASCOLLECFILE_C_TYPE_UINT32_ENUM                                           0x17          /* UINT32_ENUM */
#define MEASCOLLECFILE_C_TYPE_UINT32_HEX                                            0x18          /* UINT32_HEX */
#define MEASCOLLECFILE_C_TYPE_UINT64                                                0x19          /* UINT64 */
#define MEASCOLLECFILE_C_TYPE_UINT8                                                 0x1a          /* UINT8 */
#define MEASCOLLECFILE_C_TYPE_UINT8_ARRAY                                           0x1b          /* UINT8[] */


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
#define MEASCOLLECFILE_ID                                         0x0      /* measCollecFile. */
#define MEASCOLLECFILE_FILEHEADER_ID                              0x1      /* measCollecFile.fileHeader. */
#define MEASCOLLECFILE_FILEHEADER_FILESENDER_ID                   0x2      /* measCollecFile.fileHeader.fileSender */
#define MEASCOLLECFILE_FILEHEADER_MEASCOLLEC_ID                   0x3      /* measCollecFile.fileHeader.measCollec */
#define MEASCOLLECFILE_MEASDATA_ID                                0x4      /* measCollecFile.measData. */
#define MEASCOLLECFILE_MEASDATA_MANAGEDELEMENT_ID                 0x5      /* measCollecFile.measData.managedElement */
#define MEASCOLLECFILE_MEASDATA_MEASINFO_ID                       0x6      /* measCollecFile.measData.measInfo. */
#define MEASCOLLECFILE_MEASDATA_MEASINFO_GRANPERIOD_ID            0x7      /* measCollecFile.measData.measInfo.granPeriod */
#define MEASCOLLECFILE_MEASDATA_MEASINFO_REPPERIOD_ID             0x8      /* measCollecFile.measData.measInfo.repPeriod */
#define MEASCOLLECFILE_MEASDATA_MEASINFO_MEASTYPES_ID             0x9      /* measCollecFile.measData.measInfo.measTypes */
#define MEASCOLLECFILE_MEASDATA_MEASINFO_MEASVALUE_ID             0xa      /* measCollecFile.measData.measInfo.measValue. */
#define MEASCOLLECFILE_MEASDATA_MEASINFO_MEASVALUE_MEASRESULTS_ID 0xb      /* measCollecFile.measData.measInfo.measValue.measResults */
#define MEASCOLLECFILE_FILEFOOTER_ID                              0xc      /* measCollecFile.fileFooter. */
#define MEASCOLLECFILE_FILEFOOTER_MEASCOLLEC_ID                   0xd      /* measCollecFile.fileFooter.measCollec */


/* the definition of c structure */
typedef struct meascollecfile_measdata_measinfo_measvalue_t{
/* #define MEASCOLLECFILE_MEASDATA_MEASINFO_MEASVALUE_MEASRESULTS_ID 0xb */
#define MEASCOLLECFILE_MEASDATA_MEASINFO_MEASVALUE_MEASRESULTS_CNT 0x961
    UINT32    measResultsCnt;
    INT8      measResults[MEASCOLLECFILE_MEASDATA_MEASINFO_MEASVALUE_MEASRESULTS_CNT];

}MEASCOLLECFILE_MEASDATA_MEASINFO_MEASVALUE_T;


typedef struct meascollecfile_measdata_measinfo_t{
/* #define MEASCOLLECFILE_MEASDATA_MEASINFO_GRANPERIOD_ID 0x7 */
#define MEASCOLLECFILE_MEASDATA_MEASINFO_GRANPERIOD_CNT 0x2
    UINT32                                          granPeriodCnt;
    INT8                                            granPeriod[MEASCOLLECFILE_MEASDATA_MEASINFO_GRANPERIOD_CNT];

/* #define MEASCOLLECFILE_MEASDATA_MEASINFO_REPPERIOD_ID 0x8 */
#define MEASCOLLECFILE_MEASDATA_MEASINFO_REPPERIOD_CNT 0x2
    UINT32                                          repPeriodCnt;
    INT8                                            repPeriod[MEASCOLLECFILE_MEASDATA_MEASINFO_REPPERIOD_CNT];

/* #define MEASCOLLECFILE_MEASDATA_MEASINFO_MEASTYPES_ID 0x9 */
#define MEASCOLLECFILE_MEASDATA_MEASINFO_MEASTYPES_CNT 0x15e1
    UINT32                                          measTypesCnt;
    INT8                                            measTypes[MEASCOLLECFILE_MEASDATA_MEASINFO_MEASTYPES_CNT];

/* #define MEASCOLLECFILE_MEASDATA_MEASINFO_MEASVALUE_ID 0xa */
    MEASCOLLECFILE_MEASDATA_MEASINFO_MEASVALUE_T    measValue;

}MEASCOLLECFILE_MEASDATA_MEASINFO_T;


typedef struct meascollecfile_fileheader_t{
/* #define MEASCOLLECFILE_FILEHEADER_FILESENDER_ID 0x2 */
#define MEASCOLLECFILE_FILEHEADER_FILESENDER_CNT 0x2
    UINT32    fileSenderCnt;
    INT8      fileSender[MEASCOLLECFILE_FILEHEADER_FILESENDER_CNT];

/* #define MEASCOLLECFILE_FILEHEADER_MEASCOLLEC_ID 0x3 */
#define MEASCOLLECFILE_FILEHEADER_MEASCOLLEC_CNT 0x2
    UINT32    measCollecCnt;
    INT8      measCollec[MEASCOLLECFILE_FILEHEADER_MEASCOLLEC_CNT];

}MEASCOLLECFILE_FILEHEADER_T;


typedef struct meascollecfile_measdata_t{
/* #define MEASCOLLECFILE_MEASDATA_MANAGEDELEMENT_ID 0x5 */
#define MEASCOLLECFILE_MEASDATA_MANAGEDELEMENT_CNT 0x2
    UINT32                                managedElementCnt;
    INT8                                  managedElement[MEASCOLLECFILE_MEASDATA_MANAGEDELEMENT_CNT];

/* #define MEASCOLLECFILE_MEASDATA_MEASINFO_ID 0x6 */
    MEASCOLLECFILE_MEASDATA_MEASINFO_T    measInfo;

}MEASCOLLECFILE_MEASDATA_T;


typedef struct meascollecfile_filefooter_t{
/* #define MEASCOLLECFILE_FILEFOOTER_MEASCOLLEC_ID 0xd */
#define MEASCOLLECFILE_FILEFOOTER_MEASCOLLEC_CNT 0x2
    UINT32    measCollecCnt;
    INT8      measCollec[MEASCOLLECFILE_FILEFOOTER_MEASCOLLEC_CNT];

}MEASCOLLECFILE_FILEFOOTER_T;


typedef struct meascollecfile_t{
/* #define MEASCOLLECFILE_FILEHEADER_ID 0x1 */
    MEASCOLLECFILE_FILEHEADER_T    fileHeader;

/* #define MEASCOLLECFILE_MEASDATA_ID 0x4 */
    MEASCOLLECFILE_MEASDATA_T      measData;

/* #define MEASCOLLECFILE_FILEFOOTER_ID 0xc */
    MEASCOLLECFILE_FILEFOOTER_T    fileFooter;

}MEASCOLLECFILE_T;




/* the declaration of measCollecFile model loading function */
extern INT32 MeascollecfileDbLoadReq(void *pDlHandler, BOOL8 bDbReset, INT8 *pWorkPath, INT8 *pLoadPath, INT8 *pBackupPath);


#ifdef __cplusplus
}
#endif

#endif
