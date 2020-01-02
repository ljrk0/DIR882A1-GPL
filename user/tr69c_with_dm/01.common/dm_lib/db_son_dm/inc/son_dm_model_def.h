#ifndef _SON_DM_MODEL_DEF_H_
#define _SON_DM_MODEL_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "common_type_def.h"

/* the declaration of external header files if any used */


/* the definition of data type */
#define SON_DM_DATA_TYPE_BASE64                     0x0           /* base64 */
#define SON_DM_DATA_TYPE_BOOLEAN                    0x1           /* boolean */
#define SON_DM_DATA_TYPE_DATETIME                   0x2           /* dateTime */
#define SON_DM_DATA_TYPE_HEXBINARY                  0x3           /* hexBinary */
#define SON_DM_DATA_TYPE_INT                        0x4           /* int */
#define SON_DM_DATA_TYPE_LONG                       0x5           /* long */
#define SON_DM_DATA_TYPE_OBJECT                     0x6           /* object */
#define SON_DM_DATA_TYPE_STRING                     0x7           /* string */
#define SON_DM_DATA_TYPE_UNSIGNEDINT                0x8           /* unsignedInt */
#define SON_DM_DATA_TYPE_UNSIGNEDLONG               0x9           /* unsignedLong */


/* the definition of c type */
#define SON_DM_C_TYPE_BOOL8                                         0x0           /* BOOL8 */
#define SON_DM_C_TYPE_CM_HOST_NAME_OR_IP_ADDR_T                     0x1           /* CM_HOST_NAME_OR_IP_ADDR_T */
#define SON_DM_C_TYPE_CM_HOST_NAME_OR_IP_ADDR_T_ARRAY               0x2           /* CM_HOST_NAME_OR_IP_ADDR_T[] */
#define SON_DM_C_TYPE_CM_IMSI_T                                     0x3           /* CM_IMSI_T */
#define SON_DM_C_TYPE_CM_IMSI_T_ARRAY                               0x4           /* CM_IMSI_T[] */
#define SON_DM_C_TYPE_CM_IP_ADDR_T                                  0x5           /* CM_IP_ADDR_T */
#define SON_DM_C_TYPE_CM_IP_ADDR_T_ARRAY                            0x6           /* CM_IP_ADDR_T[] */
#define SON_DM_C_TYPE_CM_PLMN_ID_T                                  0x7           /* CM_PLMN_ID_T */
#define SON_DM_C_TYPE_CM_PLMN_ID_T_ARRAY                            0x8           /* CM_PLMN_ID_T[] */
#define SON_DM_C_TYPE_DATE_TIME_T                                   0x9           /* DATE_TIME_T */
#define SON_DM_C_TYPE_INT16                                         0xa           /* INT16 */
#define SON_DM_C_TYPE_INT16_ARRAY                                   0xb           /* INT16[] */
#define SON_DM_C_TYPE_INT32                                         0xc           /* INT32 */
#define SON_DM_C_TYPE_INT64                                         0xd           /* INT64 */
#define SON_DM_C_TYPE_INT8                                          0xe           /* INT8 */
#define SON_DM_C_TYPE_INT8_ARRAY                                    0xf           /* INT8[] */
#define SON_DM_C_TYPE_INT8A_ARRAY                                   0x10          /* INT8A[] */
#define SON_DM_C_TYPE_STRUCT                                        0x11          /* STRUCT */
#define SON_DM_C_TYPE_UINT16                                        0x12          /* UINT16 */
#define SON_DM_C_TYPE_UINT16_ARRAY                                  0x13          /* UINT16[] */
#define SON_DM_C_TYPE_UINT32                                        0x14          /* UINT32 */
#define SON_DM_C_TYPE_UINT32_ARRAY                                  0x15          /* UINT32[] */
#define SON_DM_C_TYPE_UINT32_BITMAP                                 0x16          /* UINT32_BITMAP */
#define SON_DM_C_TYPE_UINT32_ENUM                                   0x17          /* UINT32_ENUM */
#define SON_DM_C_TYPE_UINT32_HEX                                    0x18          /* UINT32_HEX */
#define SON_DM_C_TYPE_UINT64                                        0x19          /* UINT64 */
#define SON_DM_C_TYPE_UINT8                                         0x1a          /* UINT8 */
#define SON_DM_C_TYPE_UINT8_ARRAY                                   0x1b          /* UINT8[] */


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
#define SON_DM_ID                                        0x0      /* SON_DM. */
#define SON_DM_VENDORDATAMODELVERSION_ID                 0x1      /* SON_DM.VendorDataModelVersion */
#define SON_DM_TRACE_ID                                  0x2      /* SON_DM.Trace */
#define SON_DM_MAXLINEPERFILE_ID                         0x3      /* SON_DM.MaxLinePerFile */
#define SON_DM_MAXFILE_ID                                0x4      /* SON_DM.MaxFile */
#define SON_DM_REM_ID                                    0x5      /* SON_DM.REM. */
#define SON_DM_REM_ENABLED_ID                            0x6      /* SON_DM.REM.Enabled */
#define SON_DM_REM_INTERVAL_ID                           0x7      /* SON_DM.REM.Interval */
#define SON_DM_REM_COUNT_ID                              0x8      /* SON_DM.REM.Count */
#define SON_DM_TPM_ID                                    0x9      /* SON_DM.TPM. */
#define SON_DM_TPM_ENABLED_ID                            0xa      /* SON_DM.TPM.Enabled */
#define SON_DM_TPM_NONCSGCHECK_ID                        0xb      /* SON_DM.TPM.NonCsgCheck */
#define SON_DM_TPM_POWERSETTINGOFFSET_ID                 0xc      /* SON_DM.TPM.PowerSettingOffset */
#define SON_DM_TPM_POLLUTIONMITIGATIONLEVEL_ID           0xd      /* SON_DM.TPM.PollutionMitigationLevel */
#define SON_DM_TPM_PINI_ID                               0xe      /* SON_DM.TPM.Pini */
#define SON_DM_TPM_PMIN_ID                               0xf      /* SON_DM.TPM.Pmin */
#define SON_DM_TPM_PMAX_ID                               0x10     /* SON_DM.TPM.Pmax */
#define SON_DM_TPM_NCELLRSCPTHRESHOLDTABLE_ID            0x11     /* SON_DM.TPM.NCellRscpThresholdTable.{i}. */
#define SON_DM_TPM_NCELLRSCPTHRESHOLDTABLE_RSRP_ID       0x12     /* SON_DM.TPM.NCellRscpThresholdTable.{i}.RSRP */
#define SON_DM_TPM_NCELLRSCPTHRESHOLDTABLE_MAXTXPOWER_ID 0x13     /* SON_DM.TPM.NCellRscpThresholdTable.{i}.MaxTxPower */
#define SON_DM_TPM_LUTPPMCORRECTION_ID                   0x14     /* SON_DM.TPM.LutPpmCorrection.{i}. */
#define SON_DM_TPM_LUTPPMCORRECTION_PPMLOW_ID            0x15     /* SON_DM.TPM.LutPpmCorrection.{i}.PPMLow */
#define SON_DM_TPM_LUTPPMCORRECTION_PPMMEDIUM_ID         0x16     /* SON_DM.TPM.LutPpmCorrection.{i}.PPMMedium */
#define SON_DM_TPM_LUTPPMCORRECTION_PPMHIGH_ID           0x17     /* SON_DM.TPM.LutPpmCorrection.{i}.PPMHigh */
#define SON_DM_TPM_LUTADJMACROCORRECTION_ID              0x18     /* SON_DM.TPM.LutAdjMacroCorrection.{i}. */
#define SON_DM_TPM_LUTADJMACROCORRECTION_RSRP_ID         0x19     /* SON_DM.TPM.LutAdjMacroCorrection.{i}.RSRP */
#define SON_DM_TPM_LUTADJMACROCORRECTION_CORRECTION_ID   0x1a     /* SON_DM.TPM.LutAdjMacroCorrection.{i}.Correction */
#define SON_DM_TPM_LUTCCMACROCORRECTION_ID               0x1b     /* SON_DM.TPM.LutCcMacroCorrection.{i}. */
#define SON_DM_TPM_LUTCCMACROCORRECTION_RSRP_ID          0x1c     /* SON_DM.TPM.LutCcMacroCorrection.{i}.RSRP */
#define SON_DM_TPM_LUTCCMACROCORRECTION_CORRECTION_ID    0x1d     /* SON_DM.TPM.LutCcMacroCorrection.{i}.Correction */
#define SON_DM_IM_ID                                     0x1e     /* SON_DM.IM. */
#define SON_DM_IM_ENABLED_ID                             0x1f     /* SON_DM.IM.Enabled */
#define SON_DM_IM_UECLASSIFICATIONENABLED_ID             0x20     /* SON_DM.IM.UeClassificationEnabled */
#define SON_DM_IM_IMFAIRLEVEL_ID                         0x21     /* SON_DM.IM.ImFairLevel */
#define SON_DM_IM_IMFAIRFACTOR_ID                        0x22     /* SON_DM.IM.ImFairFactor */
#define SON_DM_IM_IMHARDFFR_ID                           0x23     /* SON_DM.IM.ImHardFFR */
#define SON_DM_IM_IMDELTARSRPHIGH_ID                     0x24     /* SON_DM.IM.ImDeltaRsrpHigh */
#define SON_DM_IM_IMDELTARSRPLOW_ID                      0x25     /* SON_DM.IM.ImDeltaRsrpLow */
#define SON_DM_IM_IMUEMEASINTERVAL_ID                    0x26     /* SON_DM.IM.ImUeMeasInterval */
#define SON_DM_IM_IMIIRCOEFF_ID                          0x27     /* SON_DM.IM.ImIirCoeff */
#define SON_DM_IM_IMCQIAVERAGETYPE_ID                    0x28     /* SON_DM.IM.ImCqiAverageType */
#define SON_DM_IM_IMPALOW_ID                             0x29     /* SON_DM.IM.ImPaLow */
#define SON_DM_IM_IMPAHIGH_ID                            0x2a     /* SON_DM.IM.ImPaHigh */
#define SON_DM_IM_IMBASEONDYNAMICUEREPORT_ID             0x2b     /* SON_DM.IM.ImBaseOnDynamicUEReport */
#define SON_DM_IM_IMFORCEICIC_ID                         0x2c     /* SON_DM.IM.ImForceICIC */
#define SON_DM_IM_BW_ID                                  0x2d     /* SON_DM.IM.BW.{i}. */
#define SON_DM_IM_BW_IMDLPROTECTEDPRBLIST_ID             0x2e     /* SON_DM.IM.BW.{i}.ImDlProtectedPrbList.{i}. */
#define SON_DM_IM_BW_IMDLPROTECTEDPRBLIST_BITMAP_ID      0x2f     /* SON_DM.IM.BW.{i}.ImDlProtectedPrbList.{i}.Bitmap */
#define SON_DM_CCO_ID                                    0x30     /* SON_DM.CCO. */
#define SON_DM_CCO_ENABLED_ID                            0x31     /* SON_DM.CCO.Enabled */
#define SON_DM_CCO_UEMEASREPORTPERIOD_ID                 0x32     /* SON_DM.CCO.UeMeasReportPeriod */
#define SON_DM_CCO_RSRQTHRESHOLD1_ID                     0x33     /* SON_DM.CCO.RsrqThreshold1 */
#define SON_DM_CCO_RSRQTHRESHOLD2_ID                     0x34     /* SON_DM.CCO.RsrqThreshold2 */
#define SON_DM_CCO_RSRPTHRESHOLDSERVINGNOISE_ID          0x35     /* SON_DM.CCO.RsrpThresholdServingNoise */
#define SON_DM_CCO_RSRPTHRESHOLDSERVINGINF_ID            0x36     /* SON_DM.CCO.RsrpThresholdServingInf */
#define SON_DM_CCO_RSRPTHRESHOLDNEIGHBOR2CELL_ID         0x37     /* SON_DM.CCO.RsrpThresholdNeighbor2Cell */
#define SON_DM_CCO_RSRPTHRESHOLDNEIGHBOR3CELL_ID         0x38     /* SON_DM.CCO.RsrpThresholdNeighbor3Cell */
#define SON_DM_CCO_RSRQHYSTERESIS_ID                     0x39     /* SON_DM.CCO.RsrqHysteresis */
#define SON_DM_CCO_RSRPHYSTERESIS_ID                     0x3a     /* SON_DM.CCO.RsrpHysteresis */
#define SON_DM_CCO_SAMPLECOUNTTHRESHOLD_ID               0x3b     /* SON_DM.CCO.SampleCountThreshold */
#define SON_DM_CCO_STEP_ID                               0x3c     /* SON_DM.CCO.Step */
#define SON_DM_CCO_CELLEDGERATIO_ID                      0x3d     /* SON_DM.CCO.CellEdgeRatio */
#define SON_DM_CCO_WEIGHTS_ID                            0x3e     /* SON_DM.CCO.Weights. */
#define SON_DM_CCO_WEIGHTS_GOOD_ID                       0x3f     /* SON_DM.CCO.Weights.Good */
#define SON_DM_CCO_WEIGHTS_NOISEDOMINATED_ID             0x40     /* SON_DM.CCO.Weights.NoiseDominated */
#define SON_DM_CCO_WEIGHTS_INTERFERENCEDOMINATED2CELL_ID 0x41     /* SON_DM.CCO.Weights.InterferenceDominated2Cell */
#define SON_DM_CCO_WEIGHTS_INTERFERENCEDOMINATED3CELL_ID 0x42     /* SON_DM.CCO.Weights.InterferenceDominated3Cell */
#define SON_DM_MRO_ID                                    0x43     /* SON_DM.MRO. */
#define SON_DM_MRO_ENABLED_ID                            0x44     /* SON_DM.MRO.Enabled */
#define SON_DM_MRO_MROINTERFREQENABLED_ID                0x45     /* SON_DM.MRO.MroInterFreqEnabled */
#define SON_DM_MRO_MROPERIODICITY_ID                     0x46     /* SON_DM.MRO.MroPeriodicity */
#define SON_DM_MRO_MROAPERIODICENABLE_ID                 0x47     /* SON_DM.MRO.MroAperiodicEnable */
#define SON_DM_MRO_POWERCHANGETHRESHOLD_ID               0x48     /* SON_DM.MRO.PowerChangeThreshold */
#define SON_DM_MRO_PINGPONGHOTIMETHRESHOLD_ID            0x49     /* SON_DM.MRO.PingPongHoTimeThreshold */
#define SON_DM_MRO_X2STOREUECONTEXTTIME_ID               0x4a     /* SON_DM.MRO.X2StoreUEContextTime */
#define SON_DM_MRO_S1STOREUECONTEXTTIME_ID               0x4b     /* SON_DM.MRO.S1StoreUEContextTime */
#define SON_DM_MRO_TARGETHOFAILURERATE_ID                0x4c     /* SON_DM.MRO.TargetHOFailureRate */
#define SON_DM_MRO_TARGETPINGPONGRATE_ID                 0x4d     /* SON_DM.MRO.TargetPingpongRate */
#define SON_DM_MRO_MINRLFEVENT_ID                        0x4e     /* SON_DM.MRO.MinRlfEvent */
#define SON_DM_MRO_MROWEIGHTS_ID                         0x4f     /* SON_DM.MRO.MroWeights. */
#define SON_DM_MRO_MROWEIGHTS_TL_ID                      0x50     /* SON_DM.MRO.MroWeights.TL */
#define SON_DM_MRO_MROWEIGHTS_TE_ID                      0x51     /* SON_DM.MRO.MroWeights.TE */
#define SON_DM_MRO_MROWEIGHTS_WCA_ID                     0x52     /* SON_DM.MRO.MroWeights.WCA */
#define SON_DM_MRO_MROWEIGHTS_WCB_ID                     0x53     /* SON_DM.MRO.MroWeights.WCB */
#define SON_DM_MRO_MROWEIGHTS_PP_ID                      0x54     /* SON_DM.MRO.MroWeights.PP */
#define SON_DM_MLB_ID                                    0x55     /* SON_DM.MLB. */
#define SON_DM_MLB_ENABLED_ID                            0x56     /* SON_DM.MLB.Enabled */
#define SON_DM_MLB_MLBPERIODICITY_ID                     0x57     /* SON_DM.MLB.MlbPeriodicity */
#define SON_DM_MLB_REPORTINGPERIODICITY_ID               0x58     /* SON_DM.MLB.ReportingPeriodicity */
#define SON_DM_MLB_GTPECHODELAYCOFF_ID                   0x59     /* SON_DM.MLB.GtpEchoDelayCoff */
#define SON_DM_MLB_S1TNLLOWLOADTHRESHOLD_ID              0x5a     /* SON_DM.MLB.S1TnlLowLoadThreshold */
#define SON_DM_MLB_S1TNLMEDLOADTHRESHOLD_ID              0x5b     /* SON_DM.MLB.S1TnlMedLoadThreshold */
#define SON_DM_MLB_S1TNLHIGHLOADTHRESHOLD_ID             0x5c     /* SON_DM.MLB.S1TnlHighLoadThreshold */
#define SON_DM_MLB_HWLOWLOADTHRESHOLD_ID                 0x5d     /* SON_DM.MLB.HwLowLoadThreshold */
#define SON_DM_MLB_HWMEDLOADTHRESHOLD_ID                 0x5e     /* SON_DM.MLB.HwMedLoadThreshold */
#define SON_DM_MLB_HWHIGHLOADTHRESHOLD_ID                0x5f     /* SON_DM.MLB.HwHighLoadThreshold */
#define SON_DM_MLB_PRBLOWLOADTHRESHOLD_ID                0x60     /* SON_DM.MLB.PrbLowLoadThreshold */
#define SON_DM_MLB_PRBMEDLOADTHRESHOLD_ID                0x61     /* SON_DM.MLB.PrbMedLoadThreshold */
#define SON_DM_MLB_PRBHIGHLOADTHRESHOLD_ID               0x62     /* SON_DM.MLB.PrbHighLoadThreshold */
#define SON_DM_MLB_CAPACITYLOWLOADTHRESHOLD_ID           0x63     /* SON_DM.MLB.CapacityLowLoadThreshold */
#define SON_DM_MLB_CAPACITYMEDLOADTHRESHOLD_ID           0x64     /* SON_DM.MLB.CapacityMedLoadThreshold */
#define SON_DM_MLB_CAPACITYHIGHLOADTHRESHOLD_ID          0x65     /* SON_DM.MLB.CapacityHighLoadThreshold */


/* the definition of c structure */
typedef struct son_dm_im_bw_imdlprotectedprblist_t{
    UINT32    instanceNum;

/* #define SON_DM_IM_BW_IMDLPROTECTEDPRBLIST_BITMAP_ID 0x2f */
#define SON_DM_IM_BW_IMDLPROTECTEDPRBLIST_BITMAP_CNT 0xe
    UINT32    BitmapCnt;
    UINT8     Bitmap[SON_DM_IM_BW_IMDLPROTECTEDPRBLIST_BITMAP_CNT];

}SON_DM_IM_BW_IMDLPROTECTEDPRBLIST_T;


typedef struct son_dm_tpm_ncellrscpthresholdtable_t{
    UINT32    instanceNum;

/* #define SON_DM_TPM_NCELLRSCPTHRESHOLDTABLE_RSRP_ID 0x12 */
    INT8      RSRP;

/* #define SON_DM_TPM_NCELLRSCPTHRESHOLDTABLE_MAXTXPOWER_ID 0x13 */
    INT8      MaxTxPower;

}SON_DM_TPM_NCELLRSCPTHRESHOLDTABLE_T;


typedef struct son_dm_tpm_lutppmcorrection_t{
    UINT32    instanceNum;

/* #define SON_DM_TPM_LUTPPMCORRECTION_PPMLOW_ID 0x15 */
    INT8      PPMLow;

/* #define SON_DM_TPM_LUTPPMCORRECTION_PPMMEDIUM_ID 0x16 */
    INT8      PPMMedium;

/* #define SON_DM_TPM_LUTPPMCORRECTION_PPMHIGH_ID 0x17 */
    INT8      PPMHigh;

}SON_DM_TPM_LUTPPMCORRECTION_T;


typedef struct son_dm_tpm_lutadjmacrocorrection_t{
    UINT32    instanceNum;

/* #define SON_DM_TPM_LUTADJMACROCORRECTION_RSRP_ID 0x19 */
    INT8      RSRP;

/* #define SON_DM_TPM_LUTADJMACROCORRECTION_CORRECTION_ID 0x1a */
    INT8      Correction;

}SON_DM_TPM_LUTADJMACROCORRECTION_T;


typedef struct son_dm_tpm_lutccmacrocorrection_t{
    UINT32    instanceNum;

/* #define SON_DM_TPM_LUTCCMACROCORRECTION_RSRP_ID 0x1c */
    INT8      RSRP;

/* #define SON_DM_TPM_LUTCCMACROCORRECTION_CORRECTION_ID 0x1d */
    INT8      Correction;

}SON_DM_TPM_LUTCCMACROCORRECTION_T;


typedef struct son_dm_im_bw_t{
    UINT32                                 instanceNum;

/* #define SON_DM_IM_BW_IMDLPROTECTEDPRBLIST_ID 0x2e */
#define SON_DM_IM_BW_IMDLPROTECTEDPRBLIST_CNT 0x6
    UINT32                                 ImDlProtectedPrbList_cnt;
    SON_DM_IM_BW_IMDLPROTECTEDPRBLIST_T    ImDlProtectedPrbList[SON_DM_IM_BW_IMDLPROTECTEDPRBLIST_CNT];

}SON_DM_IM_BW_T;


typedef struct son_dm_cco_weights_t{
/* #define SON_DM_CCO_WEIGHTS_GOOD_ID 0x3f */
    UINT8    Good;

/* #define SON_DM_CCO_WEIGHTS_NOISEDOMINATED_ID 0x40 */
    UINT8    NoiseDominated;

/* #define SON_DM_CCO_WEIGHTS_INTERFERENCEDOMINATED2CELL_ID 0x41 */
    UINT8    InterferenceDominated2Cell;

/* #define SON_DM_CCO_WEIGHTS_INTERFERENCEDOMINATED3CELL_ID 0x42 */
    UINT8    InterferenceDominated3Cell;

}SON_DM_CCO_WEIGHTS_T;


typedef struct son_dm_mro_mroweights_t{
/* #define SON_DM_MRO_MROWEIGHTS_TL_ID 0x50 */
    UINT16    TL;

/* #define SON_DM_MRO_MROWEIGHTS_TE_ID 0x51 */
    UINT16    TE;

/* #define SON_DM_MRO_MROWEIGHTS_WCA_ID 0x52 */
    UINT16    WCA;

/* #define SON_DM_MRO_MROWEIGHTS_WCB_ID 0x53 */
    UINT16    WCB;

/* #define SON_DM_MRO_MROWEIGHTS_PP_ID 0x54 */
    UINT16    PP;

}SON_DM_MRO_MROWEIGHTS_T;


typedef struct son_dm_rem_t{
/* #define SON_DM_REM_ENABLED_ID 0x6 */
    BOOL8     Enabled;

/* #define SON_DM_REM_INTERVAL_ID 0x7 */
    UINT16    Interval;

/* #define SON_DM_REM_COUNT_ID 0x8 */
    UINT16    Count;

}SON_DM_REM_T;


typedef struct son_dm_tpm_t{
/* #define SON_DM_TPM_ENABLED_ID 0xa */
    BOOL8                                   Enabled;

/* #define SON_DM_TPM_NONCSGCHECK_ID 0xb */
    BOOL8                                   NonCsgCheck;

/* #define SON_DM_TPM_POWERSETTINGOFFSET_ID 0xc */
    UINT8                                   PowerSettingOffset;

/* #define SON_DM_TPM_POLLUTIONMITIGATIONLEVEL_ID 0xd */
    UINT8                                   PollutionMitigationLevel;

/* #define SON_DM_TPM_PINI_ID 0xe */
    INT8                                    Pini;

/* #define SON_DM_TPM_PMIN_ID 0xf */
    INT8                                    Pmin;

/* #define SON_DM_TPM_PMAX_ID 0x10 */
    INT8                                    Pmax;

/* #define SON_DM_TPM_NCELLRSCPTHRESHOLDTABLE_ID 0x11 */
#define SON_DM_TPM_NCELLRSCPTHRESHOLDTABLE_CNT 0x10
    UINT32                                  NCellRscpThresholdTable_cnt;
    SON_DM_TPM_NCELLRSCPTHRESHOLDTABLE_T    NCellRscpThresholdTable[SON_DM_TPM_NCELLRSCPTHRESHOLDTABLE_CNT];

/* #define SON_DM_TPM_LUTPPMCORRECTION_ID 0x14 */
#define SON_DM_TPM_LUTPPMCORRECTION_CNT 0x3
    UINT32                                  LutPpmCorrection_cnt;
    SON_DM_TPM_LUTPPMCORRECTION_T           LutPpmCorrection[SON_DM_TPM_LUTPPMCORRECTION_CNT];

/* #define SON_DM_TPM_LUTADJMACROCORRECTION_ID 0x18 */
#define SON_DM_TPM_LUTADJMACROCORRECTION_CNT 0x10
    UINT32                                  LutAdjMacroCorrection_cnt;
    SON_DM_TPM_LUTADJMACROCORRECTION_T      LutAdjMacroCorrection[SON_DM_TPM_LUTADJMACROCORRECTION_CNT];

/* #define SON_DM_TPM_LUTCCMACROCORRECTION_ID 0x1b */
#define SON_DM_TPM_LUTCCMACROCORRECTION_CNT 0x10
    UINT32                                  LutCcMacroCorrection_cnt;
    SON_DM_TPM_LUTCCMACROCORRECTION_T       LutCcMacroCorrection[SON_DM_TPM_LUTCCMACROCORRECTION_CNT];

}SON_DM_TPM_T;


typedef struct son_dm_im_t{
/* #define SON_DM_IM_ENABLED_ID 0x1f */
    BOOL8             Enabled;

/* #define SON_DM_IM_UECLASSIFICATIONENABLED_ID 0x20 */
    BOOL8             UeClassificationEnabled;

/* #define SON_DM_IM_IMFAIRLEVEL_ID 0x21 */
    UINT8             ImFairLevel;

/* #define SON_DM_IM_IMFAIRFACTOR_ID 0x22 */
#define SON_DM_IM_IMFAIRFACTOR_CNT 0x3
    UINT32            ImFairFactorCnt;
    UINT8             ImFairFactor[SON_DM_IM_IMFAIRFACTOR_CNT];

/* #define SON_DM_IM_IMHARDFFR_ID 0x23 */
    BOOL8             ImHardFFR;

/* #define SON_DM_IM_IMDELTARSRPHIGH_ID 0x24 */
    INT8              ImDeltaRsrpHigh;

/* #define SON_DM_IM_IMDELTARSRPLOW_ID 0x25 */
    INT8              ImDeltaRsrpLow;

/* #define SON_DM_IM_IMUEMEASINTERVAL_ID 0x26 */
    UINT16            ImUeMeasInterval;

/* #define SON_DM_IM_IMIIRCOEFF_ID 0x27 */
#define SON_DM_IM_IMIIRCOEFF_CNT 0x2
    UINT32            ImIirCoeffCnt;
    UINT8             ImIirCoeff[SON_DM_IM_IMIIRCOEFF_CNT];

/* #define SON_DM_IM_IMCQIAVERAGETYPE_ID 0x28 */
#define SON_DM_IM_IMCQIAVERAGETYPE_DIRECT     0x0 
#define SON_DM_IM_IMCQIAVERAGETYPE_EFFICIENCY 0x1 
    UINT32            ImCqiAverageType;

/* #define SON_DM_IM_IMPALOW_ID 0x29 */
#define SON_DM_IM_IMPALOW_DB_NEG_6    0x0 
#define SON_DM_IM_IMPALOW_DB_NEG_4_77 0x1 
#define SON_DM_IM_IMPALOW_DB_NEG_3    0x2 
#define SON_DM_IM_IMPALOW_DB_NEG_1_77 0x3 
#define SON_DM_IM_IMPALOW_DB_0        0x4 
#define SON_DM_IM_IMPALOW_DB_1        0x5 
#define SON_DM_IM_IMPALOW_DB_2        0x6 
#define SON_DM_IM_IMPALOW_DB_3        0x7 
#define SON_DM_IM_IMPALOW_DB_NEG_INF  0x8 
    UINT32            ImPaLow;

/* #define SON_DM_IM_IMPAHIGH_ID 0x2a */
#define SON_DM_IM_IMPAHIGH_DB_NEG_6    0x0 
#define SON_DM_IM_IMPAHIGH_DB_NEG_4_77 0x1 
#define SON_DM_IM_IMPAHIGH_DB_NEG_3    0x2 
#define SON_DM_IM_IMPAHIGH_DB_NEG_1_77 0x3 
#define SON_DM_IM_IMPAHIGH_DB_0        0x4 
#define SON_DM_IM_IMPAHIGH_DB_1        0x5 
#define SON_DM_IM_IMPAHIGH_DB_2        0x6 
#define SON_DM_IM_IMPAHIGH_DB_3        0x7 
#define SON_DM_IM_IMPAHIGH_DB_NEG_INF  0x8 
    UINT32            ImPaHigh;

/* #define SON_DM_IM_IMBASEONDYNAMICUEREPORT_ID 0x2b */
    BOOL8             ImBaseOnDynamicUEReport;

/* #define SON_DM_IM_IMFORCEICIC_ID 0x2c */
    BOOL8             ImForceICIC;

/* #define SON_DM_IM_BW_ID 0x2d */
#define SON_DM_IM_BW_CNT 0x6
    UINT32            BW_cnt;
    SON_DM_IM_BW_T    BW[SON_DM_IM_BW_CNT];

}SON_DM_IM_T;


typedef struct son_dm_cco_t{
/* #define SON_DM_CCO_ENABLED_ID 0x31 */
    BOOL8                   Enabled;

/* #define SON_DM_CCO_UEMEASREPORTPERIOD_ID 0x32 */
    UINT16                  UeMeasReportPeriod;

/* #define SON_DM_CCO_RSRQTHRESHOLD1_ID 0x33 */
    INT8                    RsrqThreshold1;

/* #define SON_DM_CCO_RSRQTHRESHOLD2_ID 0x34 */
    INT8                    RsrqThreshold2;

/* #define SON_DM_CCO_RSRPTHRESHOLDSERVINGNOISE_ID 0x35 */
    INT16                   RsrpThresholdServingNoise;

/* #define SON_DM_CCO_RSRPTHRESHOLDSERVINGINF_ID 0x36 */
    INT16                   RsrpThresholdServingInf;

/* #define SON_DM_CCO_RSRPTHRESHOLDNEIGHBOR2CELL_ID 0x37 */
    INT16                   RsrpThresholdNeighbor2Cell;

/* #define SON_DM_CCO_RSRPTHRESHOLDNEIGHBOR3CELL_ID 0x38 */
    INT16                   RsrpThresholdNeighbor3Cell;

/* #define SON_DM_CCO_RSRQHYSTERESIS_ID 0x39 */
    UINT8                   RsrqHysteresis;

/* #define SON_DM_CCO_RSRPHYSTERESIS_ID 0x3a */
    UINT8                   RsrpHysteresis;

/* #define SON_DM_CCO_SAMPLECOUNTTHRESHOLD_ID 0x3b */
    UINT8                   SampleCountThreshold;

/* #define SON_DM_CCO_STEP_ID 0x3c */
    UINT8                   Step;

/* #define SON_DM_CCO_CELLEDGERATIO_ID 0x3d */
    UINT8                   CellEdgeRatio;

/* #define SON_DM_CCO_WEIGHTS_ID 0x3e */
    SON_DM_CCO_WEIGHTS_T    Weights;

}SON_DM_CCO_T;


typedef struct son_dm_mro_t{
/* #define SON_DM_MRO_ENABLED_ID 0x44 */
    BOOL8                      Enabled;

/* #define SON_DM_MRO_MROINTERFREQENABLED_ID 0x45 */
    BOOL8                      MroInterFreqEnabled;

/* #define SON_DM_MRO_MROPERIODICITY_ID 0x46 */
    UINT32                     MroPeriodicity;

/* #define SON_DM_MRO_MROAPERIODICENABLE_ID 0x47 */
    BOOL8                      MroAperiodicEnable;

/* #define SON_DM_MRO_POWERCHANGETHRESHOLD_ID 0x48 */
    UINT8                      PowerChangeThreshold;

/* #define SON_DM_MRO_PINGPONGHOTIMETHRESHOLD_ID 0x49 */
    UINT16                     PingPongHoTimeThreshold;

/* #define SON_DM_MRO_X2STOREUECONTEXTTIME_ID 0x4a */
    UINT32                     X2StoreUEContextTime;

/* #define SON_DM_MRO_S1STOREUECONTEXTTIME_ID 0x4b */
    UINT32                     S1StoreUEContextTime;

/* #define SON_DM_MRO_TARGETHOFAILURERATE_ID 0x4c */
    UINT16                     TargetHOFailureRate;

/* #define SON_DM_MRO_TARGETPINGPONGRATE_ID 0x4d */
    UINT16                     TargetPingpongRate;

/* #define SON_DM_MRO_MINRLFEVENT_ID 0x4e */
    UINT16                     MinRlfEvent;

/* #define SON_DM_MRO_MROWEIGHTS_ID 0x4f */
    SON_DM_MRO_MROWEIGHTS_T    MroWeights;

}SON_DM_MRO_T;


typedef struct son_dm_mlb_t{
/* #define SON_DM_MLB_ENABLED_ID 0x56 */
    BOOL8     Enabled;

/* #define SON_DM_MLB_MLBPERIODICITY_ID 0x57 */
    UINT32    MlbPeriodicity;

/* #define SON_DM_MLB_REPORTINGPERIODICITY_ID 0x58 */
#define SON_DM_MLB_REPORTINGPERIODICITY_1000  0x0 
#define SON_DM_MLB_REPORTINGPERIODICITY_2000  0x1 
#define SON_DM_MLB_REPORTINGPERIODICITY_5000  0x2 
#define SON_DM_MLB_REPORTINGPERIODICITY_10000 0x3 
    UINT32    ReportingPeriodicity;

/* #define SON_DM_MLB_GTPECHODELAYCOFF_ID 0x59 */
    UINT16    GtpEchoDelayCoff;

/* #define SON_DM_MLB_S1TNLLOWLOADTHRESHOLD_ID 0x5a */
    UINT8     S1TnlLowLoadThreshold;

/* #define SON_DM_MLB_S1TNLMEDLOADTHRESHOLD_ID 0x5b */
    UINT8     S1TnlMedLoadThreshold;

/* #define SON_DM_MLB_S1TNLHIGHLOADTHRESHOLD_ID 0x5c */
    UINT8     S1TnlHighLoadThreshold;

/* #define SON_DM_MLB_HWLOWLOADTHRESHOLD_ID 0x5d */
    UINT8     HwLowLoadThreshold;

/* #define SON_DM_MLB_HWMEDLOADTHRESHOLD_ID 0x5e */
    UINT8     HwMedLoadThreshold;

/* #define SON_DM_MLB_HWHIGHLOADTHRESHOLD_ID 0x5f */
    UINT8     HwHighLoadThreshold;

/* #define SON_DM_MLB_PRBLOWLOADTHRESHOLD_ID 0x60 */
    UINT8     PrbLowLoadThreshold;

/* #define SON_DM_MLB_PRBMEDLOADTHRESHOLD_ID 0x61 */
    UINT8     PrbMedLoadThreshold;

/* #define SON_DM_MLB_PRBHIGHLOADTHRESHOLD_ID 0x62 */
    UINT8     PrbHighLoadThreshold;

/* #define SON_DM_MLB_CAPACITYLOWLOADTHRESHOLD_ID 0x63 */
    UINT8     CapacityLowLoadThreshold;

/* #define SON_DM_MLB_CAPACITYMEDLOADTHRESHOLD_ID 0x64 */
    UINT8     CapacityMedLoadThreshold;

/* #define SON_DM_MLB_CAPACITYHIGHLOADTHRESHOLD_ID 0x65 */
    UINT8     CapacityHighLoadThreshold;

}SON_DM_MLB_T;


typedef struct son_dm_t{
/* #define SON_DM_VENDORDATAMODELVERSION_ID 0x1 */
#define SON_DM_VENDORDATAMODELVERSION_CNT 0x21
    UINT32          VendorDataModelVersionCnt;
    INT8            VendorDataModelVersion[SON_DM_VENDORDATAMODELVERSION_CNT];

/* #define SON_DM_TRACE_ID 0x2 */
    UINT8           Trace;

/* #define SON_DM_MAXLINEPERFILE_ID 0x3 */
    UINT16          MaxLinePerFile;

/* #define SON_DM_MAXFILE_ID 0x4 */
    UINT16          MaxFile;

/* #define SON_DM_REM_ID 0x5 */
    SON_DM_REM_T    REM;

/* #define SON_DM_TPM_ID 0x9 */
    SON_DM_TPM_T    TPM;

/* #define SON_DM_IM_ID 0x1e */
    SON_DM_IM_T     IM;

/* #define SON_DM_CCO_ID 0x30 */
    SON_DM_CCO_T    CCO;

/* #define SON_DM_MRO_ID 0x43 */
    SON_DM_MRO_T    MRO;

/* #define SON_DM_MLB_ID 0x55 */
    SON_DM_MLB_T    MLB;

}SON_DM_T;




/* the declaration of SON_DM model loading function */
extern INT32 Son_dmDbLoadReq(void *pDlHandler, BOOL8 bDbReset, INT8 *pWorkPath, INT8 *pLoadPath, INT8 *pBackupPath);


#ifdef __cplusplus
}
#endif

#endif
