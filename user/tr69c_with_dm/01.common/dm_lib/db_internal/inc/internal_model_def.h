#ifndef _INTERNAL_MODEL_DEF_H_
#define _INTERNAL_MODEL_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "common_type_def.h"

/* the declaration of external header files if any used */


/* the definition of data type */
#define INTERNAL_DATA_TYPE_BASE64                       0x0           /* base64 */
#define INTERNAL_DATA_TYPE_BOOLEAN                      0x1           /* boolean */
#define INTERNAL_DATA_TYPE_DATETIME                     0x2           /* dateTime */
#define INTERNAL_DATA_TYPE_HEXBINARY                    0x3           /* hexBinary */
#define INTERNAL_DATA_TYPE_INT                          0x4           /* int */
#define INTERNAL_DATA_TYPE_LONG                         0x5           /* long */
#define INTERNAL_DATA_TYPE_OBJECT                       0x6           /* object */
#define INTERNAL_DATA_TYPE_STRING                       0x7           /* string */
#define INTERNAL_DATA_TYPE_UNSIGNEDINT                  0x8           /* unsignedInt */
#define INTERNAL_DATA_TYPE_UNSIGNEDLONG                 0x9           /* unsignedLong */


/* the definition of c type */
#define INTERNAL_C_TYPE_BOOL8                                           0x0           /* BOOL8 */
#define INTERNAL_C_TYPE_CM_HOST_NAME_OR_IP_ADDR_T                       0x1           /* CM_HOST_NAME_OR_IP_ADDR_T */
#define INTERNAL_C_TYPE_CM_HOST_NAME_OR_IP_ADDR_T_ARRAY                 0x2           /* CM_HOST_NAME_OR_IP_ADDR_T[] */
#define INTERNAL_C_TYPE_CM_IMSI_T                                       0x3           /* CM_IMSI_T */
#define INTERNAL_C_TYPE_CM_IMSI_T_ARRAY                                 0x4           /* CM_IMSI_T[] */
#define INTERNAL_C_TYPE_CM_IP_ADDR_T                                    0x5           /* CM_IP_ADDR_T */
#define INTERNAL_C_TYPE_CM_IP_ADDR_T_ARRAY                              0x6           /* CM_IP_ADDR_T[] */
#define INTERNAL_C_TYPE_CM_PLMN_ID_T                                    0x7           /* CM_PLMN_ID_T */
#define INTERNAL_C_TYPE_CM_PLMN_ID_T_ARRAY                              0x8           /* CM_PLMN_ID_T[] */
#define INTERNAL_C_TYPE_DATE_TIME_T                                     0x9           /* DATE_TIME_T */
#define INTERNAL_C_TYPE_INT16                                           0xa           /* INT16 */
#define INTERNAL_C_TYPE_INT16_ARRAY                                     0xb           /* INT16[] */
#define INTERNAL_C_TYPE_INT32                                           0xc           /* INT32 */
#define INTERNAL_C_TYPE_INT64                                           0xd           /* INT64 */
#define INTERNAL_C_TYPE_INT8                                            0xe           /* INT8 */
#define INTERNAL_C_TYPE_INT8_ARRAY                                      0xf           /* INT8[] */
#define INTERNAL_C_TYPE_INT8A_ARRAY                                     0x10          /* INT8A[] */
#define INTERNAL_C_TYPE_STRUCT                                          0x11          /* STRUCT */
#define INTERNAL_C_TYPE_UINT16                                          0x12          /* UINT16 */
#define INTERNAL_C_TYPE_UINT16_ARRAY                                    0x13          /* UINT16[] */
#define INTERNAL_C_TYPE_UINT32                                          0x14          /* UINT32 */
#define INTERNAL_C_TYPE_UINT32_ARRAY                                    0x15          /* UINT32[] */
#define INTERNAL_C_TYPE_UINT32_BITMAP                                   0x16          /* UINT32_BITMAP */
#define INTERNAL_C_TYPE_UINT32_ENUM                                     0x17          /* UINT32_ENUM */
#define INTERNAL_C_TYPE_UINT32_HEX                                      0x18          /* UINT32_HEX */
#define INTERNAL_C_TYPE_UINT64                                          0x19          /* UINT64 */
#define INTERNAL_C_TYPE_UINT8                                           0x1a          /* UINT8 */
#define INTERNAL_C_TYPE_UINT8_ARRAY                                     0x1b          /* UINT8[] */


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
#define INTERNAL_ID                                                           0x0      /* Internal. */
#define INTERNAL_VENDORDATAMODELVERSION_ID                                    0x1      /* Internal.VendorDataModelVersion */
#define INTERNAL_VENDORCONFIGLOCALIP_ID                                       0x2      /* Internal.VendorConfigLocalIp */
#define INTERNAL_SCM_ID                                                       0x3      /* Internal.Scm. */
#define INTERNAL_SCM_UPGRADEERRRETTOEME_ID                                    0x4      /* Internal.Scm.UpgradeErrRetToEme */
#define INTERNAL_SCM_DISABLESOFTDOGCHECKER_ID                                 0x5      /* Internal.Scm.DisableSoftdogChecker */
#define INTERNAL_EMA_ID                                                       0x6      /* Internal.Ema. */
#define INTERNAL_EMA_NUMOFEME_ID                                              0x7      /* Internal.Ema.NumOfEme */
#define INTERNAL_EMA_CURL_ID                                                  0x8      /* Internal.Ema.Curl. */
#define INTERNAL_EMA_CURL_CONNTIMEOUT_ID                                      0x9      /* Internal.Ema.Curl.connTimeout */
#define INTERNAL_EMA_CURL_LIMITRATE_ID                                        0xa      /* Internal.Ema.Curl.limitRate */
#define INTERNAL_EMA_CURL_RETRYMAX_ID                                         0xb      /* Internal.Ema.Curl.retryMax */
#define INTERNAL_EMA_CURL_RETRYINTERVAL_ID                                    0xc      /* Internal.Ema.Curl.retryInterval */
#define INTERNAL_EMA_SESSIONSTATUS_ID                                         0xd      /* Internal.Ema.SessionStatus. */
#define INTERNAL_EMA_SESSIONSTATUS_ENTITYID_ID                                0xe      /* Internal.Ema.SessionStatus.EntityId */
#define INTERNAL_EMA_SESSIONSTATUS_UPDATETIME_ID                              0xf      /* Internal.Ema.SessionStatus.UpdateTime */
#define INTERNAL_EMA_EMEINFO_ID                                               0x10     /* Internal.Ema.emeInfo.{i}. */
#define INTERNAL_EMA_EMEINFO_NAME_ID                                          0x11     /* Internal.Ema.emeInfo.{i}.Name */
#define INTERNAL_EMA_EMEINFO_ENABLE_ID                                        0x12     /* Internal.Ema.emeInfo.{i}.Enable */
#define INTERNAL_EMA_EMEINFO_REGISTERREQUEST_ID                               0x13     /* Internal.Ema.emeInfo.{i}.RegisterRequest */
#define INTERNAL_EMA_EMEINFO_AUTONOMOUSTRANSFERCOMPLETEIND_ID                 0x14     /* Internal.Ema.emeInfo.{i}.AutonomousTransferCompleteInd */
#define INTERNAL_WEBUI_ID                                                     0x15     /* Internal.WebUI. */
#define INTERNAL_WEBUI_USERNAME_ID                                            0x16     /* Internal.WebUI.UserName */
#define INTERNAL_WEBUI_PASSWORD_ID                                            0x17     /* Internal.WebUI.Password */
#define INTERNAL_WEBUI_LOGGINGLEVEL_ID                                        0x18     /* Internal.WebUI.loggingLevel */
#define INTERNAL_WEBUI_LOGGINGDESTINATION_ID                                  0x19     /* Internal.WebUI.loggingDestination */
#define INTERNAL_TR69C_ID                                                     0x1a     /* Internal.Tr69c. */
#define INTERNAL_TR69C_TRANSFERNEEDED_ID                                      0x1b     /* Internal.Tr69c.TransferNeeded */
#define INTERNAL_TR69C_CONFIG_ID                                              0x1c     /* Internal.Tr69c.Config. */
#define INTERNAL_TR69C_CONFIG_LOGGINGLEVEL_ID                                 0x1d     /* Internal.Tr69c.Config.loggingLevel */
#define INTERNAL_TR69C_CONFIG_LOGGINGDESTINATION_ID                           0x1e     /* Internal.Tr69c.Config.loggingDestination */
#define INTERNAL_TR69C_CONFIG_LOGGINGSOAP_ID                                  0x1f     /* Internal.Tr69c.Config.loggingSOAP */
#define INTERNAL_TR69C_CONFIG_CONNECTIONREQUESTAUTHENTICATION_ID              0x20     /* Internal.Tr69c.Config.connectionRequestAuthentication */
#define INTERNAL_TR69C_CONFIG_ENABLESELFDEFINEDEVENTCODE_ID                   0x21     /* Internal.Tr69c.Config.enableSelfdefinedEventCode */
#define INTERNAL_TR69C_CONFIG_EMESESSIONCHECKINTERVAL_ID                      0x22     /* Internal.Tr69c.Config.emeSessionCheckInterval */
#define INTERNAL_TR69C_CONFIG_EMESESSIONCHECKMAXCOUNT_ID                      0x23     /* Internal.Tr69c.Config.emeSessionCheckMaxCount */
#define INTERNAL_TR69C_CONFIG_TLS_ID                                          0x24     /* Internal.Tr69c.Config.TLS. */
#define INTERNAL_TR69C_CONFIG_TLS_CACERTREQUIRED_ID                           0x25     /* Internal.Tr69c.Config.TLS.CaCertRequired */
#define INTERNAL_TR69C_CONFIG_TLS_CIPHERS_ID                                  0x26     /* Internal.Tr69c.Config.TLS.Ciphers */
#define INTERNAL_TR69C_CONFIG_TLS_CERTDIR_ID                                  0x27     /* Internal.Tr69c.Config.TLS.CertDir */
#define INTERNAL_TR69C_CONFIG_TLS_CLIENTCERTFILE_ID                           0x28     /* Internal.Tr69c.Config.TLS.ClientCertFile */
#define INTERNAL_TR69C_CONFIG_TLS_CLIENTPRIVATEKEYFILE_ID                     0x29     /* Internal.Tr69c.Config.TLS.ClientPrivateKeyFile */
#define INTERNAL_TR69C_CONFIG_TLS_CACERTFILE_ID                               0x2a     /* Internal.Tr69c.Config.TLS.CaCertFile */
#define INTERNAL_TR69C_ACSCONFIG_ID                                           0x2b     /* Internal.Tr69c.AcsConfig. */
#define INTERNAL_TR69C_ACSCONFIG_LASTCONNECTEDURL_ID                          0x2c     /* Internal.Tr69c.AcsConfig.lastConnectedURL */
#define INTERNAL_TR69C_ACSCONFIG_BOUNDIFNAME_ID                               0x2d     /* Internal.Tr69c.AcsConfig.BoundIfName */
#define INTERNAL_TR69C_GWSTATEDATA_ID                                         0x2e     /* Internal.Tr69c.GWStateData. */
#define INTERNAL_TR69C_GWSTATEDATA_REBOOTCOMMANDKEY_ID                        0x2f     /* Internal.Tr69c.GWStateData.rebootCommandKey */
#define INTERNAL_TR69C_GWSTATEDATA_DOWNLOADCOMMANDKEY_ID                      0x30     /* Internal.Tr69c.GWStateData.downloadCommandKey */
#define INTERNAL_TR69C_GWSTATEDATA_NEWPARAMETERKEY_ID                         0x31     /* Internal.Tr69c.GWStateData.newParameterKey */
#define INTERNAL_TR69C_GWSTATEDATA_CONTACTEDSTATE_ID                          0x32     /* Internal.Tr69c.GWStateData.contactedState */
#define INTERNAL_TR69C_GWSTATEDATA_DLFAULTSTATUS_ID                           0x33     /* Internal.Tr69c.GWStateData.dlFaultStatus */
#define INTERNAL_TR69C_GWSTATEDATA_RETRYCOUNT_ID                              0x34     /* Internal.Tr69c.GWStateData.retryCount */
#define INTERNAL_TR69C_GWSTATEDATA_STARTDLTIME_ID                             0x35     /* Internal.Tr69c.GWStateData.startDLTime */
#define INTERNAL_TR69C_GWSTATEDATA_ENDDLTIME_ID                               0x36     /* Internal.Tr69c.GWStateData.endDLTime */
#define INTERNAL_TR69C_GWSTATEDATA_DLFAULTMSG_ID                              0x37     /* Internal.Tr69c.GWStateData.dlFaultMsg */
#define INTERNAL_TR69C_GWSTATEDATA_INFORMEVCOUNT_ID                           0x38     /* Internal.Tr69c.GWStateData.informEvCount */
#define INTERNAL_TR69C_GWSTATEDATA_INFORMEVLIST_ID                            0x39     /* Internal.Tr69c.GWStateData.informEvList */
#define INTERNAL_TR69C_GWSTATEDATA_DATAMODEL_ID                               0x3a     /* Internal.Tr69c.GWStateData.dataModel */
#define INTERNAL_TR69C_GWSTATEDATA_CWMPVERSION_ID                             0x3b     /* Internal.Tr69c.GWStateData.cwmpVersion */
#define INTERNAL_TR69C_TRANSFERLIST_ID                                        0x3c     /* Internal.Tr69c.TransferList. */
#define INTERNAL_TR69C_TRANSFERLIST_QUEUEENTRYCOUNT_ID                        0x3d     /* Internal.Tr69c.TransferList.queueEntryCount */
#define INTERNAL_TR69C_TRANSFERLIST_QUEUE_ID                                  0x3e     /* Internal.Tr69c.TransferList.Queue.{i}. */
#define INTERNAL_TR69C_TRANSFERLIST_QUEUE_EFILETYPE_ID                        0x3f     /* Internal.Tr69c.TransferList.Queue.{i}.efileType */
#define INTERNAL_TR69C_TRANSFERLIST_QUEUE_COMMANDKEY_ID                       0x40     /* Internal.Tr69c.TransferList.Queue.{i}.commandKey */
#define INTERNAL_TR69C_TRANSFERLIST_QUEUE_URL_ID                              0x41     /* Internal.Tr69c.TransferList.Queue.{i}.url */
#define INTERNAL_TR69C_TRANSFERLIST_QUEUE_USER_ID                             0x42     /* Internal.Tr69c.TransferList.Queue.{i}.user */
#define INTERNAL_TR69C_TRANSFERLIST_QUEUE_PWD_ID                              0x43     /* Internal.Tr69c.TransferList.Queue.{i}.pwd */
#define INTERNAL_TR69C_TRANSFERLIST_QUEUE_FILESIZE_ID                         0x44     /* Internal.Tr69c.TransferList.Queue.{i}.fileSize */
#define INTERNAL_TR69C_TRANSFERLIST_QUEUE_FILENAME_ID                         0x45     /* Internal.Tr69c.TransferList.Queue.{i}.fileName */
#define INTERNAL_TR69C_TRANSFERLIST_QUEUE_DELAYSEC_ID                         0x46     /* Internal.Tr69c.TransferList.Queue.{i}.delaySec */
#define INTERNAL_TR69C_TRANSFERLIST_QUEUE_STATE_ID                            0x47     /* Internal.Tr69c.TransferList.Queue.{i}.state */
#define INTERNAL_TR69C_TRANSFERLIST_QUEUE_RPCMETHOD_ID                        0x48     /* Internal.Tr69c.TransferList.Queue.{i}.rpcMethod */
#define INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_ID                            0x49     /* Internal.Tr69c.DownloadVendorConfigInfo. */
#define INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_NAME_ID                       0x4a     /* Internal.Tr69c.DownloadVendorConfigInfo.name */
#define INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_VERSION_ID                    0x4b     /* Internal.Tr69c.DownloadVendorConfigInfo.version */
#define INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_DATE_ID                       0x4c     /* Internal.Tr69c.DownloadVendorConfigInfo.date */
#define INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_DESCRIPTION_ID                0x4d     /* Internal.Tr69c.DownloadVendorConfigInfo.description */
#define INTERNAL_TR69C_SOAPNAMESPACES_ID                                      0x4e     /* Internal.Tr69c.SoapNameSpaces. */
#define INTERNAL_TR69C_SOAPNAMESPACES_SOAPENV_ID                              0x4f     /* Internal.Tr69c.SoapNameSpaces.SoapEnv */
#define INTERNAL_TR69C_SOAPNAMESPACES_SOAPENC_ID                              0x50     /* Internal.Tr69c.SoapNameSpaces.SoapEnc */
#define INTERNAL_TR69C_SOAPNAMESPACES_XSD_ID                                  0x51     /* Internal.Tr69c.SoapNameSpaces.Xsd */
#define INTERNAL_TR69C_SOAPNAMESPACES_XSI_ID                                  0x52     /* Internal.Tr69c.SoapNameSpaces.Xsi */
#define INTERNAL_TR69C_SOAPNAMESPACES_CWMP_ID                                 0x53     /* Internal.Tr69c.SoapNameSpaces.Cwmp */
#define INTERNAL_FM_ID                                                        0x54     /* Internal.Fm. */
#define INTERNAL_FM_RECOVERDELAY_ID                                           0x55     /* Internal.Fm.RecoverDelay */
#define INTERNAL_FM_DEVICELOG_ID                                              0x56     /* Internal.Fm.DeviceLog. */
#define INTERNAL_FM_DEVICELOG_FILEPATH_ID                                     0x57     /* Internal.Fm.DeviceLog.FilePath */
#define INTERNAL_FM_DEVICELOG_FILESIZE_ID                                     0x58     /* Internal.Fm.DeviceLog.FileSize */
#define INTERNAL_FM_DEVICELOG_PERCENTTODROP_ID                                0x59     /* Internal.Fm.DeviceLog.PercentToDrop */
#define INTERNAL_FM_TCPDUMP_ID                                                0x5a     /* Internal.Fm.Tcpdump. */
#define INTERNAL_FM_TCPDUMP_TCPDUMPPERIOD_ID                                  0x5b     /* Internal.Fm.Tcpdump.TcpdumpPeriod */
#define INTERNAL_FM_TCPDUMP_TCPDUMPSIZE_ID                                    0x5c     /* Internal.Fm.Tcpdump.TcpdumpSize */
#define INTERNAL_FM_TCPDUMP_TCPDUMPDIR_ID                                     0x5d     /* Internal.Fm.Tcpdump.TcpdumpDir */
#define INTERNAL_TM_ID                                                        0x5e     /* Internal.Tm. */
#define INTERNAL_TM_SHORTSTATUSINTERVAL_ID                                    0x5f     /* Internal.Tm.ShortStatusInterval */
#define INTERNAL_TM_LONGSTATUSINTERVAL_ID                                     0x60     /* Internal.Tm.LongStatusInterval */
#define INTERNAL_TM_SAVETIMEINTERVAL_ID                                       0x61     /* Internal.Tm.SaveTimeInterval */
#define INTERNAL_TM_ATTEMPTINTERVAL_ID                                        0x62     /* Internal.Tm.AttemptInterval */
#define INTERNAL_TM_NTPD_ID                                                   0x63     /* Internal.Tm.Ntpd. */
#define INTERNAL_TM_NTPD_NTPDATEPATH_ID                                       0x64     /* Internal.Tm.Ntpd.NtpdatePath */
#define INTERNAL_TM_NTPD_NTPQPATH_ID                                          0x65     /* Internal.Tm.Ntpd.NtpqPath */
#define INTERNAL_TM_NTPD_NTPDPATH_ID                                          0x66     /* Internal.Tm.Ntpd.NtpdPath */
#define INTERNAL_TM_NTPD_NTPDCONFIGPATH_ID                                    0x67     /* Internal.Tm.Ntpd.NtpdConfigPath */
#define INTERNAL_TM_NTPD_NTPDPIDPATH_ID                                       0x68     /* Internal.Tm.Ntpd.NtpdPidPath */
#define INTERNAL_TM_BNTPD_ID                                                  0x69     /* Internal.Tm.Bntpd. */
#define INTERNAL_TM_BNTPD_BNTPDPATH_ID                                        0x6a     /* Internal.Tm.Bntpd.BntpdPath */
#define INTERNAL_TM_BNTPD_BNTPDCNFPATH_ID                                     0x6b     /* Internal.Tm.Bntpd.BntpdCnfPath */
#define INTERNAL_TM_BNTPD_BNTPDSTATUSPATH_ID                                  0x6c     /* Internal.Tm.Bntpd.BntpdStatusPath */
#define INTERNAL_TM_PTP4L_ID                                                  0x6d     /* Internal.Tm.Ptp4l. */
#define INTERNAL_TM_PTP4L_PTP4LPATH_ID                                        0x6e     /* Internal.Tm.Ptp4l.Ptp4lPath */
#define INTERNAL_TM_PTP4L_PTP4LCNFPATH_ID                                     0x6f     /* Internal.Tm.Ptp4l.Ptp4lCnfPath */
#define INTERNAL_TM_PTP4L_PTP4LLOGGING_ID                                     0x70     /* Internal.Tm.Ptp4l.Ptp4lLogging */
#define INTERNAL_TM_PTP4L_PHC2SYSPATH_ID                                      0x71     /* Internal.Tm.Ptp4l.Phc2SysPath */
#define INTERNAL_TM_PTP4L_PMCPATH_ID                                          0x72     /* Internal.Tm.Ptp4l.PmcPath */
#define INTERNAL_GPS_ID                                                       0x73     /* Internal.GPS. */
#define INTERNAL_GPS_GPSCLIENTLIBPATH_ID                                      0x74     /* Internal.GPS.gpsClientLibPath */
#define INTERNAL_GPS_GPSDPATH_ID                                              0x75     /* Internal.GPS.gpsdPath */
#define INTERNAL_GPS_TTYNAME_ID                                               0x76     /* Internal.GPS.ttyName */
#define INTERNAL_GPS_RECVFILTER_ID                                            0x77     /* Internal.GPS.recvFilter */
#define INTERNAL_GPS_TIMEMAXDEVIATION_ID                                      0x78     /* Internal.GPS.timeMaxDeviation */
#define INTERNAL_GPS_LOCATIONMINOFFSET_ID                                     0x79     /* Internal.GPS.locationMinOffset */
#define INTERNAL_SM_ID                                                        0x7a     /* Internal.Sm. */
#define INTERNAL_SM_CERTPOOLPATH_ID                                           0x7b     /* Internal.Sm.CertPoolPath */
#define INTERNAL_SM_STROKELOGGING_ID                                          0x7c     /* Internal.Sm.StrokeLogging */
#define INTERNAL_SM_STRONGSWAN_ID                                             0x7d     /* Internal.Sm.Strongswan. */
#define INTERNAL_SM_STRONGSWAN_TARFILEPATH_ID                                 0x7e     /* Internal.Sm.Strongswan.TarFilePath */
#define INTERNAL_SM_STRONGSWAN_RUNNINGPATH_ID                                 0x7f     /* Internal.Sm.Strongswan.RunningPath */
#define INTERNAL_SM_STRONGSWAN_CHECKMAXCOUNTER_ID                             0x80     /* Internal.Sm.Strongswan.CheckMaxCounter */
#define INTERNAL_SM_STRONGSWAN_CHECKINTERVAL_ID                               0x81     /* Internal.Sm.Strongswan.CheckInterval */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_ID                                   0x82     /* Internal.Sm.Strongswan.TunnelCfg. */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_DUMPIKEKEY_ID                        0x83     /* Internal.Sm.Strongswan.TunnelCfg.DumpIkeKey */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_IKEKEYDUMPPATH_ID                    0x84     /* Internal.Sm.Strongswan.TunnelCfg.IkeKeyDumpPath */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_RETRYMAXCOUNTER_ID                   0x85     /* Internal.Sm.Strongswan.TunnelCfg.RetryMaxCounter */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_RETRYINTERVAL_ID                     0x86     /* Internal.Sm.Strongswan.TunnelCfg.RetryInterval */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_REBOOTAFTERMAXRETRIES_ID             0x87     /* Internal.Sm.Strongswan.TunnelCfg.RebootAfterMaxRetries */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_EAPTYPE_ID                           0x88     /* Internal.Sm.Strongswan.TunnelCfg.EAPType */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_EAPVENDOR_ID                         0x89     /* Internal.Sm.Strongswan.TunnelCfg.EAPVendor */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_EAPIDENTITY_ID                       0x8a     /* Internal.Sm.Strongswan.TunnelCfg.EAPIdentity */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_AAAIDENTITY_ID                       0x8b     /* Internal.Sm.Strongswan.TunnelCfg.AAAIdentity */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_XAUTHIDENTITY_ID                     0x8c     /* Internal.Sm.Strongswan.TunnelCfg.XauthIdentity */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_MODE_ID                              0x8d     /* Internal.Sm.Strongswan.TunnelCfg.Mode */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_MOBIKE_ID                            0x8e     /* Internal.Sm.Strongswan.TunnelCfg.Mobike */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_FORCEENCAP_ID                        0x8f     /* Internal.Sm.Strongswan.TunnelCfg.ForceEncap */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_IPCOMP_ID                            0x90     /* Internal.Sm.Strongswan.TunnelCfg.IPComp */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_INACTIVITY_ID                        0x91     /* Internal.Sm.Strongswan.TunnelCfg.Inactivity */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_INSTALLPOLICY_ID                     0x92     /* Internal.Sm.Strongswan.TunnelCfg.InstallPolicy */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_REQID_ID                             0x93     /* Internal.Sm.Strongswan.TunnelCfg.ReqID */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_UNIQUE_ID                            0x94     /* Internal.Sm.Strongswan.TunnelCfg.Unique */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_TIMEMARGIN_ID                        0x95     /* Internal.Sm.Strongswan.TunnelCfg.TimeMargin */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_BYTESMARGIN_ID                       0x96     /* Internal.Sm.Strongswan.TunnelCfg.BytesMargin */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_PACKETSLIFE_ID                       0x97     /* Internal.Sm.Strongswan.TunnelCfg.PacketsLife */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_PACKETSMARGIN_ID                     0x98     /* Internal.Sm.Strongswan.TunnelCfg.PacketsMargin */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_REKEYTRIES_ID                        0x99     /* Internal.Sm.Strongswan.TunnelCfg.RekeyTries */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_FUZZY_ID                             0x9a     /* Internal.Sm.Strongswan.TunnelCfg.Fuzzy */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_DPDACTION_ID                         0x9b     /* Internal.Sm.Strongswan.TunnelCfg.DPDAction */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_DPDTIMEOUT_ID                        0x9c     /* Internal.Sm.Strongswan.TunnelCfg.DPDTimeout */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_CLOSEACTION_ID                       0x9d     /* Internal.Sm.Strongswan.TunnelCfg.CloseAction */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_MARKINVALUE_ID                       0x9e     /* Internal.Sm.Strongswan.TunnelCfg.MarkInValue */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_MARKINMASK_ID                        0x9f     /* Internal.Sm.Strongswan.TunnelCfg.MarkInMask */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_MARKOUTVALUE_ID                      0xa0     /* Internal.Sm.Strongswan.TunnelCfg.MarkOutValue */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_MARKOUTMASK_ID                       0xa1     /* Internal.Sm.Strongswan.TunnelCfg.MarkOutMask */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_THREADS_ID                           0xa2     /* Internal.Sm.Strongswan.TunnelCfg.Threads */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_IKEFRAGMENT_ID                       0xa3     /* Internal.Sm.Strongswan.TunnelCfg.IKEFragment */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_RETRANSMISSION_ID                    0xa4     /* Internal.Sm.Strongswan.TunnelCfg.Retransmission. */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_RETRANSMISSION_RETRANSMIT_TRIES_ID   0xa5     /* Internal.Sm.Strongswan.TunnelCfg.Retransmission.Retransmit_tries */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_RETRANSMISSION_RETRANSMIT_TIMEOUT_ID 0xa6     /* Internal.Sm.Strongswan.TunnelCfg.Retransmission.Retransmit_timeout */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_RETRANSMISSION_RETRANSMIT_BASE_ID    0xa7     /* Internal.Sm.Strongswan.TunnelCfg.Retransmission.Retransmit_base */
#define INTERNAL_SM_STRONGSWAN_LOGLEVEL_ID                                    0xa8     /* Internal.Sm.Strongswan.LogLevel.{i}. */
#define INTERNAL_SM_STRONGSWAN_LOGLEVEL_TYPE_ID                               0xa9     /* Internal.Sm.Strongswan.LogLevel.{i}.type */
#define INTERNAL_SM_STRONGSWAN_LOGLEVEL_LEVEL_ID                              0xaa     /* Internal.Sm.Strongswan.LogLevel.{i}.level */
#define INTERNAL_SM_CMPV2_ID                                                  0xab     /* Internal.Sm.CMPv2. */
#define INTERNAL_SM_CMPV2_BNOTFIRSTUSED_ID                                    0xac     /* Internal.Sm.CMPv2.bNotFirstUsed */


/* the definition of c structure */
typedef struct internal_sm_strongswan_tunnelcfg_retransmission_t{
/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_RETRANSMISSION_RETRANSMIT_TRIES_ID 0xa5 */
    UINT32    Retransmit_tries;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_RETRANSMISSION_RETRANSMIT_TIMEOUT_ID 0xa6 */
    UINT32    Retransmit_timeout;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_RETRANSMISSION_RETRANSMIT_BASE_ID 0xa7 */
    UINT32    Retransmit_base;

}INTERNAL_SM_STRONGSWAN_TUNNELCFG_RETRANSMISSION_T;


typedef struct internal_tr69c_config_tls_t{
/* #define INTERNAL_TR69C_CONFIG_TLS_CACERTREQUIRED_ID 0x25 */
    BOOL8     CaCertRequired;

/* #define INTERNAL_TR69C_CONFIG_TLS_CIPHERS_ID 0x26 */
#define INTERNAL_TR69C_CONFIG_TLS_CIPHERS_CNT 0x41
    UINT32    CiphersCnt;
    INT8      Ciphers[INTERNAL_TR69C_CONFIG_TLS_CIPHERS_CNT];

/* #define INTERNAL_TR69C_CONFIG_TLS_CERTDIR_ID 0x27 */
#define INTERNAL_TR69C_CONFIG_TLS_CERTDIR_CNT 0x41
    UINT32    CertDirCnt;
    INT8      CertDir[INTERNAL_TR69C_CONFIG_TLS_CERTDIR_CNT];

/* #define INTERNAL_TR69C_CONFIG_TLS_CLIENTCERTFILE_ID 0x28 */
#define INTERNAL_TR69C_CONFIG_TLS_CLIENTCERTFILE_CNT 0x41
    UINT32    ClientCertFileCnt;
    INT8      ClientCertFile[INTERNAL_TR69C_CONFIG_TLS_CLIENTCERTFILE_CNT];

/* #define INTERNAL_TR69C_CONFIG_TLS_CLIENTPRIVATEKEYFILE_ID 0x29 */
#define INTERNAL_TR69C_CONFIG_TLS_CLIENTPRIVATEKEYFILE_CNT 0x41
    UINT32    ClientPrivateKeyFileCnt;
    INT8      ClientPrivateKeyFile[INTERNAL_TR69C_CONFIG_TLS_CLIENTPRIVATEKEYFILE_CNT];

/* #define INTERNAL_TR69C_CONFIG_TLS_CACERTFILE_ID 0x2a */
#define INTERNAL_TR69C_CONFIG_TLS_CACERTFILE_CNT 0x41
    UINT32    CaCertFileCnt;
    INT8      CaCertFile[INTERNAL_TR69C_CONFIG_TLS_CACERTFILE_CNT];

}INTERNAL_TR69C_CONFIG_TLS_T;


typedef struct internal_tr69c_transferlist_queue_t{
    UINT32    instanceNum;

/* #define INTERNAL_TR69C_TRANSFERLIST_QUEUE_EFILETYPE_ID 0x3f */
    UINT8     efileType;

/* #define INTERNAL_TR69C_TRANSFERLIST_QUEUE_COMMANDKEY_ID 0x40 */
#define INTERNAL_TR69C_TRANSFERLIST_QUEUE_COMMANDKEY_CNT 0x22
    UINT32    commandKeyCnt;
    INT8      commandKey[INTERNAL_TR69C_TRANSFERLIST_QUEUE_COMMANDKEY_CNT];

/* #define INTERNAL_TR69C_TRANSFERLIST_QUEUE_URL_ID 0x41 */
#define INTERNAL_TR69C_TRANSFERLIST_QUEUE_URL_CNT 0x101
    UINT32    urlCnt;
    INT8      url[INTERNAL_TR69C_TRANSFERLIST_QUEUE_URL_CNT];

/* #define INTERNAL_TR69C_TRANSFERLIST_QUEUE_USER_ID 0x42 */
#define INTERNAL_TR69C_TRANSFERLIST_QUEUE_USER_CNT 0x101
    UINT32    userCnt;
    INT8      user[INTERNAL_TR69C_TRANSFERLIST_QUEUE_USER_CNT];

/* #define INTERNAL_TR69C_TRANSFERLIST_QUEUE_PWD_ID 0x43 */
#define INTERNAL_TR69C_TRANSFERLIST_QUEUE_PWD_CNT 0x101
    UINT32    pwdCnt;
    INT8      pwd[INTERNAL_TR69C_TRANSFERLIST_QUEUE_PWD_CNT];

/* #define INTERNAL_TR69C_TRANSFERLIST_QUEUE_FILESIZE_ID 0x44 */
    INT32     fileSize;

/* #define INTERNAL_TR69C_TRANSFERLIST_QUEUE_FILENAME_ID 0x45 */
#define INTERNAL_TR69C_TRANSFERLIST_QUEUE_FILENAME_CNT 0x101
    UINT32    fileNameCnt;
    INT8      fileName[INTERNAL_TR69C_TRANSFERLIST_QUEUE_FILENAME_CNT];

/* #define INTERNAL_TR69C_TRANSFERLIST_QUEUE_DELAYSEC_ID 0x46 */
    INT32     delaySec;

/* #define INTERNAL_TR69C_TRANSFERLIST_QUEUE_STATE_ID 0x47 */
    UINT8     state;

/* #define INTERNAL_TR69C_TRANSFERLIST_QUEUE_RPCMETHOD_ID 0x48 */
    UINT8     rpcMethod;

}INTERNAL_TR69C_TRANSFERLIST_QUEUE_T;


typedef struct internal_sm_strongswan_tunnelcfg_t{
/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_DUMPIKEKEY_ID 0x83 */
    BOOL8                                                DumpIkeKey;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_IKEKEYDUMPPATH_ID 0x84 */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_IKEKEYDUMPPATH_CNT 0x81
    UINT32                                               IkeKeyDumpPathCnt;
    INT8                                                 IkeKeyDumpPath[INTERNAL_SM_STRONGSWAN_TUNNELCFG_IKEKEYDUMPPATH_CNT];

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_RETRYMAXCOUNTER_ID 0x85 */
    INT32                                                RetryMaxCounter;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_RETRYINTERVAL_ID 0x86 */
    INT32                                                RetryInterval;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_REBOOTAFTERMAXRETRIES_ID 0x87 */
    BOOL8                                                RebootAfterMaxRetries;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_EAPTYPE_ID 0x88 */
    INT32                                                EAPType;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_EAPVENDOR_ID 0x89 */
    UINT32                                               EAPVendor;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_EAPIDENTITY_ID 0x8a */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_EAPIDENTITY_CNT 0x81
    UINT32                                               EAPIdentityCnt;
    INT8                                                 EAPIdentity[INTERNAL_SM_STRONGSWAN_TUNNELCFG_EAPIDENTITY_CNT];

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_AAAIDENTITY_ID 0x8b */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_AAAIDENTITY_CNT 0x81
    UINT32                                               AAAIdentityCnt;
    INT8                                                 AAAIdentity[INTERNAL_SM_STRONGSWAN_TUNNELCFG_AAAIDENTITY_CNT];

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_XAUTHIDENTITY_ID 0x8c */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_XAUTHIDENTITY_CNT 0x81
    UINT32                                               XauthIdentityCnt;
    INT8                                                 XauthIdentity[INTERNAL_SM_STRONGSWAN_TUNNELCFG_XAUTHIDENTITY_CNT];

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_MODE_ID 0x8d */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_MODE_MODE_NONE      0x0 
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_MODE_MODE_TRANSPORT 0x1 
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_MODE_MODE_TUNNEL    0x2 
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_MODE_MODE_BEET      0x3 
    UINT32                                               Mode;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_MOBIKE_ID 0x8e */
    INT32                                                Mobike;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_FORCEENCAP_ID 0x8f */
    INT32                                                ForceEncap;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_IPCOMP_ID 0x90 */
    INT32                                                IPComp;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_INACTIVITY_ID 0x91 */
    UINT32                                               Inactivity;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_INSTALLPOLICY_ID 0x92 */
    INT32                                                InstallPolicy;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_REQID_ID 0x93 */
    UINT32                                               ReqID;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_UNIQUE_ID 0x94 */
    INT32                                                Unique;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_TIMEMARGIN_ID 0x95 */
    INT32                                                TimeMargin;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_BYTESMARGIN_ID 0x96 */
    UINT64                                               BytesMargin;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_PACKETSLIFE_ID 0x97 */
    UINT64                                               PacketsLife;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_PACKETSMARGIN_ID 0x98 */
    UINT64                                               PacketsMargin;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_REKEYTRIES_ID 0x99 */
    UINT32                                               RekeyTries;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_FUZZY_ID 0x9a */
    UINT32                                               Fuzzy;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_DPDACTION_ID 0x9b */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_DPDACTION_DPD_ACTION_NONE    0x0 
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_DPDACTION_DPD_ACTION_CLEAR   0x1 
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_DPDACTION_DPD_ACTION_HOLD    0x2 
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_DPDACTION_DPD_ACTION_RESTART 0x3 
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_DPDACTION_DPD_ACTION_UNKNOWN 0x4 
    UINT32                                               DPDAction;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_DPDTIMEOUT_ID 0x9c */
    UINT32                                               DPDTimeout;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_CLOSEACTION_ID 0x9d */
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_CLOSEACTION_CLOSE_ACTION_NONE    0x0 
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_CLOSEACTION_CLOSE_ACTION_CLEAR   0x1 
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_CLOSEACTION_CLOSE_ACTION_HOLD    0x2 
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_CLOSEACTION_CLOSE_ACTION_RESTART 0x3 
#define INTERNAL_SM_STRONGSWAN_TUNNELCFG_CLOSEACTION_CLOSE_ACTION_UNKNOWN 0x4 
    UINT32                                               CloseAction;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_MARKINVALUE_ID 0x9e */
    UINT32                                               MarkInValue;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_MARKINMASK_ID 0x9f */
    UINT32                                               MarkInMask;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_MARKOUTVALUE_ID 0xa0 */
    UINT32                                               MarkOutValue;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_MARKOUTMASK_ID 0xa1 */
    UINT32                                               MarkOutMask;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_THREADS_ID 0xa2 */
    UINT32                                               Threads;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_IKEFRAGMENT_ID 0xa3 */
    INT32                                                IKEFragment;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_RETRANSMISSION_ID 0xa4 */
    INTERNAL_SM_STRONGSWAN_TUNNELCFG_RETRANSMISSION_T    Retransmission;

}INTERNAL_SM_STRONGSWAN_TUNNELCFG_T;


typedef struct internal_sm_strongswan_loglevel_t{
    UINT32    instanceNum;

/* #define INTERNAL_SM_STRONGSWAN_LOGLEVEL_TYPE_ID 0xa9 */
#define INTERNAL_SM_STRONGSWAN_LOGLEVEL_TYPE_CNT 0x11
    UINT32    typeCnt;
    INT8      type[INTERNAL_SM_STRONGSWAN_LOGLEVEL_TYPE_CNT];

/* #define INTERNAL_SM_STRONGSWAN_LOGLEVEL_LEVEL_ID 0xaa */
    INT32     level;

}INTERNAL_SM_STRONGSWAN_LOGLEVEL_T;


typedef struct internal_ema_curl_t{
/* #define INTERNAL_EMA_CURL_CONNTIMEOUT_ID 0x9 */
    UINT32    connTimeout;

/* #define INTERNAL_EMA_CURL_LIMITRATE_ID 0xa */
    UINT32    limitRate;

/* #define INTERNAL_EMA_CURL_RETRYMAX_ID 0xb */
    UINT32    retryMax;

/* #define INTERNAL_EMA_CURL_RETRYINTERVAL_ID 0xc */
    UINT32    retryInterval;

}INTERNAL_EMA_CURL_T;


typedef struct internal_ema_sessionstatus_t{
/* #define INTERNAL_EMA_SESSIONSTATUS_ENTITYID_ID 0xe */
    UINT32    EntityId;

/* #define INTERNAL_EMA_SESSIONSTATUS_UPDATETIME_ID 0xf */
    UINT32    UpdateTime;

}INTERNAL_EMA_SESSIONSTATUS_T;


typedef struct internal_ema_emeinfo_t{
    UINT32    instanceNum;

/* #define INTERNAL_EMA_EMEINFO_NAME_ID 0x11 */
#define INTERNAL_EMA_EMEINFO_NAME_CNT 0x11
    UINT32    NameCnt;
    INT8      Name[INTERNAL_EMA_EMEINFO_NAME_CNT];

/* #define INTERNAL_EMA_EMEINFO_ENABLE_ID 0x12 */
    BOOL8     Enable;

/* #define INTERNAL_EMA_EMEINFO_REGISTERREQUEST_ID 0x13 */
    BOOL8     RegisterRequest;

/* #define INTERNAL_EMA_EMEINFO_AUTONOMOUSTRANSFERCOMPLETEIND_ID 0x14 */
    BOOL8     AutonomousTransferCompleteInd;

}INTERNAL_EMA_EMEINFO_T;


typedef struct internal_tr69c_config_t{
/* #define INTERNAL_TR69C_CONFIG_LOGGINGLEVEL_ID 0x1d */
#define INTERNAL_TR69C_CONFIG_LOGGINGLEVEL_CNT 0x11
    UINT32                         loggingLevelCnt;
    INT8                           loggingLevel[INTERNAL_TR69C_CONFIG_LOGGINGLEVEL_CNT];

/* #define INTERNAL_TR69C_CONFIG_LOGGINGDESTINATION_ID 0x1e */
#define INTERNAL_TR69C_CONFIG_LOGGINGDESTINATION_CNT 0x21
    UINT32                         loggingDestinationCnt;
    INT8                           loggingDestination[INTERNAL_TR69C_CONFIG_LOGGINGDESTINATION_CNT];

/* #define INTERNAL_TR69C_CONFIG_LOGGINGSOAP_ID 0x1f */
    BOOL8                          loggingSOAP;

/* #define INTERNAL_TR69C_CONFIG_CONNECTIONREQUESTAUTHENTICATION_ID 0x20 */
    BOOL8                          connectionRequestAuthentication;

/* #define INTERNAL_TR69C_CONFIG_ENABLESELFDEFINEDEVENTCODE_ID 0x21 */
    BOOL8                          enableSelfdefinedEventCode;

/* #define INTERNAL_TR69C_CONFIG_EMESESSIONCHECKINTERVAL_ID 0x22 */
    UINT32                         emeSessionCheckInterval;

/* #define INTERNAL_TR69C_CONFIG_EMESESSIONCHECKMAXCOUNT_ID 0x23 */
    UINT32                         emeSessionCheckMaxCount;

/* #define INTERNAL_TR69C_CONFIG_TLS_ID 0x24 */
    INTERNAL_TR69C_CONFIG_TLS_T    TLS;

}INTERNAL_TR69C_CONFIG_T;


typedef struct internal_tr69c_acsconfig_t{
/* #define INTERNAL_TR69C_ACSCONFIG_LASTCONNECTEDURL_ID 0x2c */
#define INTERNAL_TR69C_ACSCONFIG_LASTCONNECTEDURL_CNT 0x101
    UINT32    lastConnectedURLCnt;
    INT8      lastConnectedURL[INTERNAL_TR69C_ACSCONFIG_LASTCONNECTEDURL_CNT];

/* #define INTERNAL_TR69C_ACSCONFIG_BOUNDIFNAME_ID 0x2d */
#define INTERNAL_TR69C_ACSCONFIG_BOUNDIFNAME_CNT 0x101
    UINT32    BoundIfNameCnt;
    INT8      BoundIfName[INTERNAL_TR69C_ACSCONFIG_BOUNDIFNAME_CNT];

}INTERNAL_TR69C_ACSCONFIG_T;


typedef struct internal_tr69c_gwstatedata_t{
/* #define INTERNAL_TR69C_GWSTATEDATA_REBOOTCOMMANDKEY_ID 0x2f */
#define INTERNAL_TR69C_GWSTATEDATA_REBOOTCOMMANDKEY_CNT 0x22
    UINT32    rebootCommandKeyCnt;
    INT8      rebootCommandKey[INTERNAL_TR69C_GWSTATEDATA_REBOOTCOMMANDKEY_CNT];

/* #define INTERNAL_TR69C_GWSTATEDATA_DOWNLOADCOMMANDKEY_ID 0x30 */
#define INTERNAL_TR69C_GWSTATEDATA_DOWNLOADCOMMANDKEY_CNT 0x22
    UINT32    downloadCommandKeyCnt;
    INT8      downloadCommandKey[INTERNAL_TR69C_GWSTATEDATA_DOWNLOADCOMMANDKEY_CNT];

/* #define INTERNAL_TR69C_GWSTATEDATA_NEWPARAMETERKEY_ID 0x31 */
#define INTERNAL_TR69C_GWSTATEDATA_NEWPARAMETERKEY_CNT 0x22
    UINT32    newParameterKeyCnt;
    INT8      newParameterKey[INTERNAL_TR69C_GWSTATEDATA_NEWPARAMETERKEY_CNT];

/* #define INTERNAL_TR69C_GWSTATEDATA_CONTACTEDSTATE_ID 0x32 */
    INT32     contactedState;

/* #define INTERNAL_TR69C_GWSTATEDATA_DLFAULTSTATUS_ID 0x33 */
    INT32     dlFaultStatus;

/* #define INTERNAL_TR69C_GWSTATEDATA_RETRYCOUNT_ID 0x34 */
    INT32     retryCount;

/* #define INTERNAL_TR69C_GWSTATEDATA_STARTDLTIME_ID 0x35 */
    UINT32    startDLTime;

/* #define INTERNAL_TR69C_GWSTATEDATA_ENDDLTIME_ID 0x36 */
    UINT32    endDLTime;

/* #define INTERNAL_TR69C_GWSTATEDATA_DLFAULTMSG_ID 0x37 */
#define INTERNAL_TR69C_GWSTATEDATA_DLFAULTMSG_CNT 0x82
    UINT32    dlFaultMsgCnt;
    INT8      dlFaultMsg[INTERNAL_TR69C_GWSTATEDATA_DLFAULTMSG_CNT];

/* #define INTERNAL_TR69C_GWSTATEDATA_INFORMEVCOUNT_ID 0x38 */
    INT32     informEvCount;

/* #define INTERNAL_TR69C_GWSTATEDATA_INFORMEVLIST_ID 0x39 */
#define INTERNAL_TR69C_GWSTATEDATA_INFORMEVLIST_CNT 0x10
    UINT32    informEvListCnt;
    UINT8     informEvList[INTERNAL_TR69C_GWSTATEDATA_INFORMEVLIST_CNT];

/* #define INTERNAL_TR69C_GWSTATEDATA_DATAMODEL_ID 0x3a */
    INT32     dataModel;

/* #define INTERNAL_TR69C_GWSTATEDATA_CWMPVERSION_ID 0x3b */
    INT32     cwmpVersion;

}INTERNAL_TR69C_GWSTATEDATA_T;


typedef struct internal_tr69c_transferlist_t{
/* #define INTERNAL_TR69C_TRANSFERLIST_QUEUEENTRYCOUNT_ID 0x3d */
    UINT16                                 queueEntryCount;

/* #define INTERNAL_TR69C_TRANSFERLIST_QUEUE_ID 0x3e */
#define INTERNAL_TR69C_TRANSFERLIST_QUEUE_CNT 0x10
    UINT32                                 Queue_cnt;
    INTERNAL_TR69C_TRANSFERLIST_QUEUE_T    Queue[INTERNAL_TR69C_TRANSFERLIST_QUEUE_CNT];

}INTERNAL_TR69C_TRANSFERLIST_T;


typedef struct internal_tr69c_downloadvendorconfiginfo_t{
/* #define INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_NAME_ID 0x4a */
#define INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_NAME_CNT 0x41
    UINT32    nameCnt;
    INT8      name[INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_NAME_CNT];

/* #define INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_VERSION_ID 0x4b */
#define INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_VERSION_CNT 0x11
    UINT32    versionCnt;
    INT8      version[INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_VERSION_CNT];

/* #define INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_DATE_ID 0x4c */
#define INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_DATE_CNT 0x41
    UINT32    dateCnt;
    INT8      date[INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_DATE_CNT];

/* #define INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_DESCRIPTION_ID 0x4d */
#define INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_DESCRIPTION_CNT 0x101
    UINT32    descriptionCnt;
    INT8      description[INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_DESCRIPTION_CNT];

}INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_T;


typedef struct internal_tr69c_soapnamespaces_t{
/* #define INTERNAL_TR69C_SOAPNAMESPACES_SOAPENV_ID 0x4f */
#define INTERNAL_TR69C_SOAPNAMESPACES_SOAPENV_CNT 0x81
    UINT32    SoapEnvCnt;
    INT8      SoapEnv[INTERNAL_TR69C_SOAPNAMESPACES_SOAPENV_CNT];

/* #define INTERNAL_TR69C_SOAPNAMESPACES_SOAPENC_ID 0x50 */
#define INTERNAL_TR69C_SOAPNAMESPACES_SOAPENC_CNT 0x81
    UINT32    SoapEncCnt;
    INT8      SoapEnc[INTERNAL_TR69C_SOAPNAMESPACES_SOAPENC_CNT];

/* #define INTERNAL_TR69C_SOAPNAMESPACES_XSD_ID 0x51 */
#define INTERNAL_TR69C_SOAPNAMESPACES_XSD_CNT 0x81
    UINT32    XsdCnt;
    INT8      Xsd[INTERNAL_TR69C_SOAPNAMESPACES_XSD_CNT];

/* #define INTERNAL_TR69C_SOAPNAMESPACES_XSI_ID 0x52 */
#define INTERNAL_TR69C_SOAPNAMESPACES_XSI_CNT 0x81
    UINT32    XsiCnt;
    INT8      Xsi[INTERNAL_TR69C_SOAPNAMESPACES_XSI_CNT];

/* #define INTERNAL_TR69C_SOAPNAMESPACES_CWMP_ID 0x53 */
#define INTERNAL_TR69C_SOAPNAMESPACES_CWMP_CNT 0x81
    UINT32    CwmpCnt;
    INT8      Cwmp[INTERNAL_TR69C_SOAPNAMESPACES_CWMP_CNT];

}INTERNAL_TR69C_SOAPNAMESPACES_T;


typedef struct internal_fm_devicelog_t{
/* #define INTERNAL_FM_DEVICELOG_FILEPATH_ID 0x57 */
#define INTERNAL_FM_DEVICELOG_FILEPATH_CNT 0x81
    UINT32    FilePathCnt;
    INT8      FilePath[INTERNAL_FM_DEVICELOG_FILEPATH_CNT];

/* #define INTERNAL_FM_DEVICELOG_FILESIZE_ID 0x58 */
    UINT32    FileSize;

/* #define INTERNAL_FM_DEVICELOG_PERCENTTODROP_ID 0x59 */
    UINT32    PercentToDrop;

}INTERNAL_FM_DEVICELOG_T;


typedef struct internal_fm_tcpdump_t{
/* #define INTERNAL_FM_TCPDUMP_TCPDUMPPERIOD_ID 0x5b */
    UINT32    TcpdumpPeriod;

/* #define INTERNAL_FM_TCPDUMP_TCPDUMPSIZE_ID 0x5c */
    UINT32    TcpdumpSize;

/* #define INTERNAL_FM_TCPDUMP_TCPDUMPDIR_ID 0x5d */
#define INTERNAL_FM_TCPDUMP_TCPDUMPDIR_CNT 0x81
    UINT32    TcpdumpDirCnt;
    INT8      TcpdumpDir[INTERNAL_FM_TCPDUMP_TCPDUMPDIR_CNT];

}INTERNAL_FM_TCPDUMP_T;


typedef struct internal_tm_ntpd_t{
/* #define INTERNAL_TM_NTPD_NTPDATEPATH_ID 0x64 */
#define INTERNAL_TM_NTPD_NTPDATEPATH_CNT 0x101
    UINT32    NtpdatePathCnt;
    INT8      NtpdatePath[INTERNAL_TM_NTPD_NTPDATEPATH_CNT];

/* #define INTERNAL_TM_NTPD_NTPQPATH_ID 0x65 */
#define INTERNAL_TM_NTPD_NTPQPATH_CNT 0x101
    UINT32    NtpqPathCnt;
    INT8      NtpqPath[INTERNAL_TM_NTPD_NTPQPATH_CNT];

/* #define INTERNAL_TM_NTPD_NTPDPATH_ID 0x66 */
#define INTERNAL_TM_NTPD_NTPDPATH_CNT 0x101
    UINT32    NtpdPathCnt;
    INT8      NtpdPath[INTERNAL_TM_NTPD_NTPDPATH_CNT];

/* #define INTERNAL_TM_NTPD_NTPDCONFIGPATH_ID 0x67 */
#define INTERNAL_TM_NTPD_NTPDCONFIGPATH_CNT 0x101
    UINT32    NtpdConfigPathCnt;
    INT8      NtpdConfigPath[INTERNAL_TM_NTPD_NTPDCONFIGPATH_CNT];

/* #define INTERNAL_TM_NTPD_NTPDPIDPATH_ID 0x68 */
#define INTERNAL_TM_NTPD_NTPDPIDPATH_CNT 0x101
    UINT32    NtpdPidPathCnt;
    INT8      NtpdPidPath[INTERNAL_TM_NTPD_NTPDPIDPATH_CNT];

}INTERNAL_TM_NTPD_T;


typedef struct internal_tm_bntpd_t{
/* #define INTERNAL_TM_BNTPD_BNTPDPATH_ID 0x6a */
#define INTERNAL_TM_BNTPD_BNTPDPATH_CNT 0x101
    UINT32    BntpdPathCnt;
    INT8      BntpdPath[INTERNAL_TM_BNTPD_BNTPDPATH_CNT];

/* #define INTERNAL_TM_BNTPD_BNTPDCNFPATH_ID 0x6b */
#define INTERNAL_TM_BNTPD_BNTPDCNFPATH_CNT 0x101
    UINT32    BntpdCnfPathCnt;
    INT8      BntpdCnfPath[INTERNAL_TM_BNTPD_BNTPDCNFPATH_CNT];

/* #define INTERNAL_TM_BNTPD_BNTPDSTATUSPATH_ID 0x6c */
#define INTERNAL_TM_BNTPD_BNTPDSTATUSPATH_CNT 0x101
    UINT32    BntpdStatusPathCnt;
    INT8      BntpdStatusPath[INTERNAL_TM_BNTPD_BNTPDSTATUSPATH_CNT];

}INTERNAL_TM_BNTPD_T;


typedef struct internal_tm_ptp4l_t{
/* #define INTERNAL_TM_PTP4L_PTP4LPATH_ID 0x6e */
#define INTERNAL_TM_PTP4L_PTP4LPATH_CNT 0x101
    UINT32    Ptp4lPathCnt;
    INT8      Ptp4lPath[INTERNAL_TM_PTP4L_PTP4LPATH_CNT];

/* #define INTERNAL_TM_PTP4L_PTP4LCNFPATH_ID 0x6f */
#define INTERNAL_TM_PTP4L_PTP4LCNFPATH_CNT 0x101
    UINT32    Ptp4lCnfPathCnt;
    INT8      Ptp4lCnfPath[INTERNAL_TM_PTP4L_PTP4LCNFPATH_CNT];

/* #define INTERNAL_TM_PTP4L_PTP4LLOGGING_ID 0x70 */
    BOOL8     Ptp4lLogging;

/* #define INTERNAL_TM_PTP4L_PHC2SYSPATH_ID 0x71 */
#define INTERNAL_TM_PTP4L_PHC2SYSPATH_CNT 0x101
    UINT32    Phc2SysPathCnt;
    INT8      Phc2SysPath[INTERNAL_TM_PTP4L_PHC2SYSPATH_CNT];

/* #define INTERNAL_TM_PTP4L_PMCPATH_ID 0x72 */
#define INTERNAL_TM_PTP4L_PMCPATH_CNT 0x101
    UINT32    PmcPathCnt;
    INT8      PmcPath[INTERNAL_TM_PTP4L_PMCPATH_CNT];

}INTERNAL_TM_PTP4L_T;


typedef struct internal_sm_strongswan_t{
/* #define INTERNAL_SM_STRONGSWAN_TARFILEPATH_ID 0x7e */
#define INTERNAL_SM_STRONGSWAN_TARFILEPATH_CNT 0x81
    UINT32                                TarFilePathCnt;
    INT8                                  TarFilePath[INTERNAL_SM_STRONGSWAN_TARFILEPATH_CNT];

/* #define INTERNAL_SM_STRONGSWAN_RUNNINGPATH_ID 0x7f */
#define INTERNAL_SM_STRONGSWAN_RUNNINGPATH_CNT 0x81
    UINT32                                RunningPathCnt;
    INT8                                  RunningPath[INTERNAL_SM_STRONGSWAN_RUNNINGPATH_CNT];

/* #define INTERNAL_SM_STRONGSWAN_CHECKMAXCOUNTER_ID 0x80 */
    UINT32                                CheckMaxCounter;

/* #define INTERNAL_SM_STRONGSWAN_CHECKINTERVAL_ID 0x81 */
    UINT32                                CheckInterval;

/* #define INTERNAL_SM_STRONGSWAN_TUNNELCFG_ID 0x82 */
    INTERNAL_SM_STRONGSWAN_TUNNELCFG_T    TunnelCfg;

/* #define INTERNAL_SM_STRONGSWAN_LOGLEVEL_ID 0xa8 */
#define INTERNAL_SM_STRONGSWAN_LOGLEVEL_CNT 0x12
    UINT32                                LogLevel_cnt;
    INTERNAL_SM_STRONGSWAN_LOGLEVEL_T     LogLevel[INTERNAL_SM_STRONGSWAN_LOGLEVEL_CNT];

}INTERNAL_SM_STRONGSWAN_T;


typedef struct internal_sm_cmpv2_t{
/* #define INTERNAL_SM_CMPV2_BNOTFIRSTUSED_ID 0xac */
    BOOL8    bNotFirstUsed;

}INTERNAL_SM_CMPV2_T;


typedef struct internal_scm_t{
/* #define INTERNAL_SCM_UPGRADEERRRETTOEME_ID 0x4 */
    INT32    UpgradeErrRetToEme;

/* #define INTERNAL_SCM_DISABLESOFTDOGCHECKER_ID 0x5 */
    BOOL8    DisableSoftdogChecker;

}INTERNAL_SCM_T;


typedef struct internal_ema_t{
/* #define INTERNAL_EMA_NUMOFEME_ID 0x7 */
    UINT8                           NumOfEme;

/* #define INTERNAL_EMA_CURL_ID 0x8 */
    INTERNAL_EMA_CURL_T             Curl;

/* #define INTERNAL_EMA_SESSIONSTATUS_ID 0xd */
    INTERNAL_EMA_SESSIONSTATUS_T    SessionStatus;

/* #define INTERNAL_EMA_EMEINFO_ID 0x10 */
#define INTERNAL_EMA_EMEINFO_CNT 0x2
    UINT32                          emeInfo_cnt;
    INTERNAL_EMA_EMEINFO_T          emeInfo[INTERNAL_EMA_EMEINFO_CNT];

}INTERNAL_EMA_T;


typedef struct internal_webui_t{
/* #define INTERNAL_WEBUI_USERNAME_ID 0x16 */
#define INTERNAL_WEBUI_USERNAME_CNT 0x81
    UINT32    UserNameCnt;
    INT8      UserName[INTERNAL_WEBUI_USERNAME_CNT];

/* #define INTERNAL_WEBUI_PASSWORD_ID 0x17 */
#define INTERNAL_WEBUI_PASSWORD_CNT 0x81
    UINT32    PasswordCnt;
    INT8      Password[INTERNAL_WEBUI_PASSWORD_CNT];

/* #define INTERNAL_WEBUI_LOGGINGLEVEL_ID 0x18 */
#define INTERNAL_WEBUI_LOGGINGLEVEL_CNT 0x11
    UINT32    loggingLevelCnt;
    INT8      loggingLevel[INTERNAL_WEBUI_LOGGINGLEVEL_CNT];

/* #define INTERNAL_WEBUI_LOGGINGDESTINATION_ID 0x19 */
#define INTERNAL_WEBUI_LOGGINGDESTINATION_CNT 0x21
    UINT32    loggingDestinationCnt;
    INT8      loggingDestination[INTERNAL_WEBUI_LOGGINGDESTINATION_CNT];

}INTERNAL_WEBUI_T;


typedef struct internal_tr69c_t{
/* #define INTERNAL_TR69C_TRANSFERNEEDED_ID 0x1b */
    BOOL8                                        TransferNeeded;

/* #define INTERNAL_TR69C_CONFIG_ID 0x1c */
    INTERNAL_TR69C_CONFIG_T                      Config;

/* #define INTERNAL_TR69C_ACSCONFIG_ID 0x2b */
    INTERNAL_TR69C_ACSCONFIG_T                   AcsConfig;

/* #define INTERNAL_TR69C_GWSTATEDATA_ID 0x2e */
    INTERNAL_TR69C_GWSTATEDATA_T                 GWStateData;

/* #define INTERNAL_TR69C_TRANSFERLIST_ID 0x3c */
    INTERNAL_TR69C_TRANSFERLIST_T                TransferList;

/* #define INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_ID 0x49 */
    INTERNAL_TR69C_DOWNLOADVENDORCONFIGINFO_T    DownloadVendorConfigInfo;

/* #define INTERNAL_TR69C_SOAPNAMESPACES_ID 0x4e */
    INTERNAL_TR69C_SOAPNAMESPACES_T              SoapNameSpaces;

}INTERNAL_TR69C_T;


typedef struct internal_fm_t{
/* #define INTERNAL_FM_RECOVERDELAY_ID 0x55 */
    INT32                      RecoverDelay;

/* #define INTERNAL_FM_DEVICELOG_ID 0x56 */
    INTERNAL_FM_DEVICELOG_T    DeviceLog;

/* #define INTERNAL_FM_TCPDUMP_ID 0x5a */
    INTERNAL_FM_TCPDUMP_T      Tcpdump;

}INTERNAL_FM_T;


typedef struct internal_tm_t{
/* #define INTERNAL_TM_SHORTSTATUSINTERVAL_ID 0x5f */
    UINT32                 ShortStatusInterval;

/* #define INTERNAL_TM_LONGSTATUSINTERVAL_ID 0x60 */
    UINT32                 LongStatusInterval;

/* #define INTERNAL_TM_SAVETIMEINTERVAL_ID 0x61 */
    UINT32                 SaveTimeInterval;

/* #define INTERNAL_TM_ATTEMPTINTERVAL_ID 0x62 */
    UINT32                 AttemptInterval;

/* #define INTERNAL_TM_NTPD_ID 0x63 */
    INTERNAL_TM_NTPD_T     Ntpd;

/* #define INTERNAL_TM_BNTPD_ID 0x69 */
    INTERNAL_TM_BNTPD_T    Bntpd;

/* #define INTERNAL_TM_PTP4L_ID 0x6d */
    INTERNAL_TM_PTP4L_T    Ptp4l;

}INTERNAL_TM_T;


typedef struct internal_gps_t{
/* #define INTERNAL_GPS_GPSCLIENTLIBPATH_ID 0x74 */
#define INTERNAL_GPS_GPSCLIENTLIBPATH_CNT 0x101
    UINT32    gpsClientLibPathCnt;
    INT8      gpsClientLibPath[INTERNAL_GPS_GPSCLIENTLIBPATH_CNT];

/* #define INTERNAL_GPS_GPSDPATH_ID 0x75 */
#define INTERNAL_GPS_GPSDPATH_CNT 0x101
    UINT32    gpsdPathCnt;
    INT8      gpsdPath[INTERNAL_GPS_GPSDPATH_CNT];

/* #define INTERNAL_GPS_TTYNAME_ID 0x76 */
#define INTERNAL_GPS_TTYNAME_CNT 0x101
    UINT32    ttyNameCnt;
    INT8      ttyName[INTERNAL_GPS_TTYNAME_CNT];

/* #define INTERNAL_GPS_RECVFILTER_ID 0x77 */
#define INTERNAL_GPS_RECVFILTER_ALL         0x0 
#define INTERNAL_GPS_RECVFILTER_ONLY_GPRMC  0x1 
#define INTERNAL_GPS_RECVFILTER_GPRMC_GPGGA 0x2 
    UINT32    recvFilter;

/* #define INTERNAL_GPS_TIMEMAXDEVIATION_ID 0x78 */
    UINT32    timeMaxDeviation;

/* #define INTERNAL_GPS_LOCATIONMINOFFSET_ID 0x79 */
    UINT32    locationMinOffset;

}INTERNAL_GPS_T;


typedef struct internal_sm_t{
/* #define INTERNAL_SM_CERTPOOLPATH_ID 0x7b */
#define INTERNAL_SM_CERTPOOLPATH_CNT 0x101
    UINT32                      CertPoolPathCnt;
    INT8                        CertPoolPath[INTERNAL_SM_CERTPOOLPATH_CNT];

/* #define INTERNAL_SM_STROKELOGGING_ID 0x7c */
    BOOL8                       StrokeLogging;

/* #define INTERNAL_SM_STRONGSWAN_ID 0x7d */
    INTERNAL_SM_STRONGSWAN_T    Strongswan;

/* #define INTERNAL_SM_CMPV2_ID 0xab */
    INTERNAL_SM_CMPV2_T         CMPv2;

}INTERNAL_SM_T;


typedef struct internal_t{
/* #define INTERNAL_VENDORDATAMODELVERSION_ID 0x1 */
#define INTERNAL_VENDORDATAMODELVERSION_CNT 0x21
    UINT32              VendorDataModelVersionCnt;
    INT8                VendorDataModelVersion[INTERNAL_VENDORDATAMODELVERSION_CNT];

/* #define INTERNAL_VENDORCONFIGLOCALIP_ID 0x2 */
#define INTERNAL_VENDORCONFIGLOCALIP_CNT 0x81
    UINT32              VendorConfigLocalIpCnt;
    INT8                VendorConfigLocalIp[INTERNAL_VENDORCONFIGLOCALIP_CNT];

/* #define INTERNAL_SCM_ID 0x3 */
    INTERNAL_SCM_T      Scm;

/* #define INTERNAL_EMA_ID 0x6 */
    INTERNAL_EMA_T      Ema;

/* #define INTERNAL_WEBUI_ID 0x15 */
    INTERNAL_WEBUI_T    WebUI;

/* #define INTERNAL_TR69C_ID 0x1a */
    INTERNAL_TR69C_T    Tr69c;

/* #define INTERNAL_FM_ID 0x54 */
    INTERNAL_FM_T       Fm;

/* #define INTERNAL_TM_ID 0x5e */
    INTERNAL_TM_T       Tm;

/* #define INTERNAL_GPS_ID 0x73 */
    INTERNAL_GPS_T      GPS;

/* #define INTERNAL_SM_ID 0x7a */
    INTERNAL_SM_T       Sm;

}INTERNAL_T;




/* the declaration of Internal model loading function */
extern INT32 InternalDbLoadReq(void *pDlHandler, BOOL8 bDbReset, INT8 *pWorkPath, INT8 *pLoadPath, INT8 *pBackupPath);


#ifdef __cplusplus
}
#endif

#endif
