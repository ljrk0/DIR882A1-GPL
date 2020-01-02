#ifndef _TRACECOLLECFILE_MODEL_DEF_H_
#define _TRACECOLLECFILE_MODEL_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "common_type_def.h"

/* the declaration of external header files if any used */


/* the definition of data type */
#define TRACECOLLECFILE_DATA_TYPE_BASE64                              0x0           /* base64 */
#define TRACECOLLECFILE_DATA_TYPE_BOOLEAN                             0x1           /* boolean */
#define TRACECOLLECFILE_DATA_TYPE_DATETIME                            0x2           /* dateTime */
#define TRACECOLLECFILE_DATA_TYPE_HEXBINARY                           0x3           /* hexBinary */
#define TRACECOLLECFILE_DATA_TYPE_INT                                 0x4           /* int */
#define TRACECOLLECFILE_DATA_TYPE_LONG                                0x5           /* long */
#define TRACECOLLECFILE_DATA_TYPE_OBJECT                              0x6           /* object */
#define TRACECOLLECFILE_DATA_TYPE_STRING                              0x7           /* string */
#define TRACECOLLECFILE_DATA_TYPE_UNSIGNEDINT                         0x8           /* unsignedInt */
#define TRACECOLLECFILE_DATA_TYPE_UNSIGNEDLONG                        0x9           /* unsignedLong */


/* the definition of c type */
#define TRACECOLLECFILE_C_TYPE_BOOL8                                                  0x0           /* BOOL8 */
#define TRACECOLLECFILE_C_TYPE_CM_HOST_NAME_OR_IP_ADDR_T                              0x1           /* CM_HOST_NAME_OR_IP_ADDR_T */
#define TRACECOLLECFILE_C_TYPE_CM_HOST_NAME_OR_IP_ADDR_T_ARRAY                        0x2           /* CM_HOST_NAME_OR_IP_ADDR_T[] */
#define TRACECOLLECFILE_C_TYPE_CM_IMSI_T                                              0x3           /* CM_IMSI_T */
#define TRACECOLLECFILE_C_TYPE_CM_IMSI_T_ARRAY                                        0x4           /* CM_IMSI_T[] */
#define TRACECOLLECFILE_C_TYPE_CM_IP_ADDR_T                                           0x5           /* CM_IP_ADDR_T */
#define TRACECOLLECFILE_C_TYPE_CM_IP_ADDR_T_ARRAY                                     0x6           /* CM_IP_ADDR_T[] */
#define TRACECOLLECFILE_C_TYPE_CM_PLMN_ID_T                                           0x7           /* CM_PLMN_ID_T */
#define TRACECOLLECFILE_C_TYPE_CM_PLMN_ID_T_ARRAY                                     0x8           /* CM_PLMN_ID_T[] */
#define TRACECOLLECFILE_C_TYPE_DATE_TIME_T                                            0x9           /* DATE_TIME_T */
#define TRACECOLLECFILE_C_TYPE_INT16                                                  0xa           /* INT16 */
#define TRACECOLLECFILE_C_TYPE_INT16_ARRAY                                            0xb           /* INT16[] */
#define TRACECOLLECFILE_C_TYPE_INT32                                                  0xc           /* INT32 */
#define TRACECOLLECFILE_C_TYPE_INT64                                                  0xd           /* INT64 */
#define TRACECOLLECFILE_C_TYPE_INT8                                                   0xe           /* INT8 */
#define TRACECOLLECFILE_C_TYPE_INT8_ARRAY                                             0xf           /* INT8[] */
#define TRACECOLLECFILE_C_TYPE_INT8A_ARRAY                                            0x10          /* INT8A[] */
#define TRACECOLLECFILE_C_TYPE_STRUCT                                                 0x11          /* STRUCT */
#define TRACECOLLECFILE_C_TYPE_UINT16                                                 0x12          /* UINT16 */
#define TRACECOLLECFILE_C_TYPE_UINT16_ARRAY                                           0x13          /* UINT16[] */
#define TRACECOLLECFILE_C_TYPE_UINT32                                                 0x14          /* UINT32 */
#define TRACECOLLECFILE_C_TYPE_UINT32_ARRAY                                           0x15          /* UINT32[] */
#define TRACECOLLECFILE_C_TYPE_UINT32_BITMAP                                          0x16          /* UINT32_BITMAP */
#define TRACECOLLECFILE_C_TYPE_UINT32_ENUM                                            0x17          /* UINT32_ENUM */
#define TRACECOLLECFILE_C_TYPE_UINT32_HEX                                             0x18          /* UINT32_HEX */
#define TRACECOLLECFILE_C_TYPE_UINT64                                                 0x19          /* UINT64 */
#define TRACECOLLECFILE_C_TYPE_UINT8                                                  0x1a          /* UINT8 */
#define TRACECOLLECFILE_C_TYPE_UINT8_ARRAY                                            0x1b          /* UINT8[] */


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
#define TRACECOLLECFILE_ID                                          0x0      /* traceCollecFile. */
#define TRACECOLLECFILE_FILEHEADER_ID                               0x1      /* traceCollecFile.fileHeader. */
#define TRACECOLLECFILE_FILEHEADER_FILESENDER_ID                    0x2      /* traceCollecFile.fileHeader.fileSender */
#define TRACECOLLECFILE_FILEHEADER_TRACECOLLEC_ID                   0x3      /* traceCollecFile.fileHeader.traceCollec */
#define TRACECOLLECFILE_TRACERECSESSION_ID                          0x4      /* traceCollecFile.traceRecSession. */
#define TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_ID          0x5      /* traceCollecFile.traceRecSession.traceSessionRef. */
#define TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_MCC_ID      0x6      /* traceCollecFile.traceRecSession.traceSessionRef.MCC */
#define TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_MNC_ID      0x7      /* traceCollecFile.traceRecSession.traceSessionRef.MNC */
#define TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_TRACE_ID_ID 0x8      /* traceCollecFile.traceRecSession.traceSessionRef.TRACE_ID */


/* the definition of c structure */
typedef struct tracecollecfile_tracerecsession_tracesessionref_t{
/* #define TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_MCC_ID 0x6 */
#define TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_MCC_CNT 0x4
    UINT32    MCCCnt;
    INT8      MCC[TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_MCC_CNT];

/* #define TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_MNC_ID 0x7 */
#define TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_MNC_CNT 0x4
    UINT32    MNCCnt;
    INT8      MNC[TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_MNC_CNT];

/* #define TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_TRACE_ID_ID 0x8 */
#define TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_TRACE_ID_CNT 0x7
    UINT32    TRACE_IDCnt;
    INT8      TRACE_ID[TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_TRACE_ID_CNT];

}TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_T;


typedef struct tracecollecfile_fileheader_t{
/* #define TRACECOLLECFILE_FILEHEADER_FILESENDER_ID 0x2 */
#define TRACECOLLECFILE_FILEHEADER_FILESENDER_CNT 0x2
    UINT32    fileSenderCnt;
    INT8      fileSender[TRACECOLLECFILE_FILEHEADER_FILESENDER_CNT];

/* #define TRACECOLLECFILE_FILEHEADER_TRACECOLLEC_ID 0x3 */
#define TRACECOLLECFILE_FILEHEADER_TRACECOLLEC_CNT 0x2
    UINT32    traceCollecCnt;
    INT8      traceCollec[TRACECOLLECFILE_FILEHEADER_TRACECOLLEC_CNT];

}TRACECOLLECFILE_FILEHEADER_T;


typedef struct tracecollecfile_tracerecsession_t{
/* #define TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_ID 0x5 */
    TRACECOLLECFILE_TRACERECSESSION_TRACESESSIONREF_T    traceSessionRef;

}TRACECOLLECFILE_TRACERECSESSION_T;


typedef struct tracecollecfile_t{
/* #define TRACECOLLECFILE_FILEHEADER_ID 0x1 */
    TRACECOLLECFILE_FILEHEADER_T         fileHeader;

/* #define TRACECOLLECFILE_TRACERECSESSION_ID 0x4 */
    TRACECOLLECFILE_TRACERECSESSION_T    traceRecSession;

}TRACECOLLECFILE_T;




/* the declaration of traceCollecFile model loading function */
extern INT32 TracecollecfileDbLoadReq(void *pDlHandler, BOOL8 bDbReset, INT8 *pWorkPath, INT8 *pLoadPath, INT8 *pBackupPath);


#ifdef __cplusplus
}
#endif

#endif
