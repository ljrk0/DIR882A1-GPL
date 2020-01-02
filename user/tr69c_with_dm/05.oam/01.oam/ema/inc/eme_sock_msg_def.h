#ifndef __EME_SOCK_MSG_DEF_H__
#define __EME_SOCK_MSG_DEF_H__

#ifdef __cplusplus
    extern "C" {
#endif

/* msg code definition */
#define EME_MSG_BASE                       0x10000000

#define EME_MSG_READY_IND                  (EME_MSG_BASE + 0x01)
#define EME_MSG_DESTROY_IND                (EME_MSG_BASE + 0x02)

#define EME_MSG_REGISTER_IND               (EME_MSG_BASE + 0x03)
#define EME_MSG_REGISTER_RSP               (EME_MSG_BASE + 0x04)

#define EME_MSG_FILE_TRANS_REQ             (EME_MSG_BASE + 0x09)
#define EME_MSG_TRANS_COMPLETE_IND         (EME_MSG_BASE + 0x0a)

#define EME_MSG_AUTO_TRANS_COMPLETE_IND    (EME_MSG_BASE + 0x0b)
#define EME_MSG_AUTO_TRANS_COMPLETE_RSP    (EME_MSG_BASE + 0x0c)

#define EME_MSG_ALARM_IND                  (EME_MSG_BASE + 0x0d)

#define EME_MSG_PARAM_CHANGE_IND           (EME_MSG_BASE + 0x0e)

#define EME_MSG_SYSTEM_REBOOT_REQ          (EME_MSG_BASE + 0x0f)
#define EME_MSG_FACTORY_RESET_REQ          (EME_MSG_BASE + 0x10)

#define EME_MSG_CMPV2_FORCE_REQ            (EME_MSG_BASE + 0x11)
#define EME_MSG_CMPV2_FORCE_CNF            (EME_MSG_BASE + 0x12)

#define EME_MSG_DELETE_CERT_REQ            (EME_MSG_BASE + 0x13)
#define EME_MSG_DELETE_CERT_IND            (EME_MSG_BASE + 0x14)

/* entity name definition */
#define EMA_ENTITY_NAME "Ema"
#define EME_ENTITY_TR69C_NAME "Tr69c"
#define EME_ENTITY_WEBUI_NAME "WebUI"


/*!\enum CmsRet
 * \brief Return codes for all external functions, and some internal functions too.
 *
 * Codes from 9000-9799 are reserved for TR69C return values.
 * All Broadcom return codes should start at 9800.
 */
typedef enum
{
	EMERET_SUCCESS              = 0,     /**<Success. */
	EMERET_METHOD_NOT_SUPPORTED = 9000,  /**<Method not supported. */
	EMERET_REQUEST_DENIED       = 9001,  /**< Request denied (no reason specified). */
	EMERET_INTERNAL_ERROR       = 9002,  /**< Internal error. */
	EMERET_INVALID_ARGUMENTS    = 9003,  /**< Invalid arguments. */
	EMERET_RESOURCE_EXCEEDED    = 9004,  /**< Resource exceeded.
                                        *  (when used in association with
                                        *  setParameterValues, this MUST not be
                                        *  used to indicate parameters in error)
                                        */
	EMERET_INVALID_PARAM_NAME   = 9005,  /**< Invalid parameter name.
                                        *  (associated with set/getParameterValues,
                                        *  getParameterNames,set/getParameterAtrributes)
                                        */
	EMERET_INVALID_PARAM_TYPE   = 9006,  /**< Invalid parameter type.
                                        *  (associated with set/getParameterValues)
                                        */
	EMERET_INVALID_PARAM_VALUE  = 9007,  /**< Invalid parameter value.
                                        *  (associated with set/getParameterValues)
                                        */
	EMERET_SET_NON_WRITABLE_PARAM = 9008,/**< Attempt to set a non-writable parameter.
                                        *  (associated with setParameterValues)
                                        */
	EMERET_NOTIFICATION_REQ_REJECTED = 9009, /**< Notification request rejected.
                                            *  (associated with setParameterAttributes)
                                            */
	EMERET_DOWNLOAD_FAILURE     = 9010,  /**< Download failure.
                                         *  (associated with download or transferComplete)
                                         */
	EMERET_UPLOAD_FAILURE       = 9011,  /**< Upload failure.
                                        *  (associated with upload or transferComplete)
                                        */
	EMERET_FILE_TRANSFER_AUTH_FAILURE = 9012,  /**< File transfer server authentication
                                              *  failure.
                                              *  (associated with upload, download
                                              *  or transferComplete)
                                              */
	EMERET_UNSUPPORTED_FILE_TRANSFER_PROTOCOL = 9013,/**< Unsupported protocol for file
                                                    *  transfer.
                                                    *  (associated with upload or
                                                    *  download)
                                                    */
    EMERET_UNABLE_CONTACT_FILE_SERVER = 9015,
	EMERET_UNABLE_ACCESS_FILE = 9016,
	EMERET_CORRUPTED_FILE = 9018,
	EMERET_AUTHENTICATION_FAILED_FILE = 9019, /**< authentication failed file */
	EMERET_SUCCESS_REBOOT_REQUIRED = 9800, /**< Config successful, but requires reboot to take effect. */
	EMERET_SUCCESS_UNRECOGNIZED_DATA_IGNORED = 9801,  /**<Success, but some unrecognized data was ignored. */
	EMERET_SUCCESS_OBJECT_UNCHANGED = 9802,  /**<Success, furthermore object has not changed, returned by STL handler functions. */
	EMERET_FAIL_REBOOT_REQUIRED = 9803,  /**<Config failed, and now system is in a bad state requiring reboot. */
	EMERET_NO_MORE_INSTANCES = 9804,     /**<getnext operation cannot find any more instances to return. */
	EMERET_MDM_TREE_ERROR = 9805,         /**<Error during MDM tree traversal */
	EMERET_WOULD_DEADLOCK = 9806, /**< Caller is requesting a lock while holding the same lock or a different one. */
	EMERET_LOCK_REQUIRED = 9807,  /**< The MDM lock is required for this operation. */
	EMERET_OP_INTR = 9808,      /**<Operation was interrupted, most likely by a Linux signal. */
	EMERET_TIMED_OUT = 9809,     /**<Operation timed out. */
	EMERET_DISCONNECTED = 9810,  /**< Communications link is disconnected. */
	EMERET_MSG_BOUNCED = 9811,   /**< Msg was sent to a process not running, and the
                                 *   bounceIfNotRunning flag was set on the header.  */
	EMERET_OP_ABORTED_BY_USER = 9812,  /**< Operation was aborted/discontinued by the user */
	EMERET_RECURSION_ERROR = 9817,     /**< too many levels of recursion */
	EMERET_OPEN_FILE_ERROR = 9818,     /**< open file error */
	EMERET_KEY_GENERATION_ERROR = 9830,     /** certificate key generation error */
	EMERET_INVALID_CERT_REQ = 9831,     /** requested certificate does not match with issued certificate */
	EMERET_INVALID_CERT_SUBJECT = 9832,     /** certificate has invalid subject information */
	EMERET_OBJECT_NOT_FOUND = 9840,     /** failed to find object */

	EMERET_INVALID_FILENAME = 9850,  /**< filename was not given for download */
	EMERET_INVALID_IMAGE = 9851,     /**< bad image was given for download */
	EMERET_INVALID_CONFIG_FILE = 9852,  /**< invalid config file was detected */
	EMERET_CONFIG_PSI = 9853,         /**< old PSI/3.x config file was detected */
	EMERET_IMAGE_FLASH_FAILED = 9854, /**< could not write the image to flash */

	EMERET_CHECK_SIGNATURE_FAILED = 9855, /**< check image signature  failed */
	EMERET_CHECK_FIRMWARE_UPGRADE_FAILED = 9856, /**< firmware upgrade failed */
	EMERET_EME_DISABLE = 9857,
	EMERET_IN_SESSION = 9858,
} E_EMERET_T;

typedef struct {
#define EME_FILE_COMPLETE_PATH_LEN 256
	INT8 fileName[EME_FILE_COMPLETE_PATH_LEN];/* filename without path */
    INT8 url[EME_FILE_COMPLETE_PATH_LEN];/* the url used by curl */
    UINT32 fileSize;/* bytes */
	INT32 result;
	time_t startTime;
	time_t completeTime;
}EME_FILE_COMPLETE_IND;

typedef enum{
		EME_FILE_TYPE_NULL = 0,/**< Null */
		EME_FILE_TYPE_FIRMWARE = 1,/**< Firmware image file. Download only. */
		EME_FILE_TYPE_WEBCONTENT = 2,/**< Web content, currently not support. Download only */
		EME_FILE_TYPE_VENDOR_CONFIG = 3,/**< vendor config file. Download or upload. */
		EME_FILE_TYPE_VENDOR_LOG = 4,/**< vendor log file. Upload only. */
		EME_FILE_TYPE_VENDOR_CALIBRATION = 5,/**< Calibration file. Download or Upload only. */
		EME_FILE_TYPE_VENDOR_ALARMREPORT = 11,/**< Alarm file. Upload only. */
		EME_FILE_TYPE_VENDOR_PMREPORT = 12,/**< Performance report files. Upload only. */
		EME_FILE_TYPE_VENDOR_TRACES = 13,/**< Flush-traces files. Upload only. */
		EME_FILE_TYPE_VENDOR_CERTREQ = 14,/**< PKCS10 cert-req file. Upload only. */
		EME_FILE_TYPE_VENDOR_CERTIFICATE = 15,/**< Certificate file. Download only. */
		EME_FILE_TYPE_VENDOR_DBRAWFILE = 16,/**< DB RAW files(Device.xml). Download or upload. */
		EME_FILE_TYPE_VENDOR_DEFAULTVALUE = 17,/**< Operator default files(Internal.xml). Download only. */
		EME_FILE_TYPE_VENDOR_COREFILE = 18,/**< Core-dump files. Upload only. */
		EME_FILE_TYPE_VENDOR_INNER_DBRAWFILE = 19,/**< DB RAW files(Internal.xml). Download or upload. */
		EME_FILE_TYPE_VENDOR_INNER_DEFAULTVALUE = 20,/**< Operator default files(Internal.xml). Download only. */
		EME_FILE_TYPE_VENDOR_TCPDUMP_FILE = 21,/**< tcpdump files. Upload only. */
		EME_FILE_TYPE_VENDOR_KERNELLOG_FILE = 22,/**< kernel log files. Upload only. */
		EME_FILE_TYPE_VENDOR_CA_CERTIFICATE = 23,/**< CA Certificate file. Download only. */
		EME_FILE_TYPE_DU_CHANGE = 24,/**< DU operation. Download only. */
		EME_FILE_TYPE_VENDOR_FIRMWARE = 25,/**< vendor firmware. Download only. */
		EME_FILE_TYPE_VENDOR_DEVICE_PARAMS = 26,/**< vendor device parameters. Download only. */
		EME_FILE_TYPE_VENDOR_DB_CNDATA = 27,/**< CnData db file(CnData.xml). Download only. */
		EME_FILE_TYPE_VENDOR_DB_CNDATA_DEFAULT = 28,/**< CnData db default file(CnData.xml). Download only. */
		EME_FILE_TYPE_VENDOR_DB_SONDATA = 29,/**< SON Data db file(CnData.xml). Download only. */
		EME_FILE_TYPE_VENDOR_DB_SONDATA_DEFAULT = 30,/**< SON Data db default file(SON_DM.xml). Download only. */
		EME_FILE_TYPE_VENDOR_TRACE_MDT_FILE = 31/**< Trace&Mdt files. Upload only. */
}E_FILE_TYPE_T;

typedef enum{
		EME_FILE_OP_NULL = 0,/**< Null */
		EME_FILE_OP_UPLOAD_REQ = 1,/**< It means EMA should upload files specified by external entity. */
		EME_FILE_OP_DOWNLOAD_REQ = 2,/**< It means EMA should download files specified by external entity and then apply them. */
		EME_FILE_OP_DU_INSTALL_REQ = 4,/**< It means EMA should download du file specified by external entity and then apply them. */
		EME_FILE_OP_DU_UPDATE_REQ = 8,/**< It means EMA should download du file specified by external entity and then update it. */
		EME_FILE_OP_DU_UNINSTALL_REQ = 16 /**< It means EMA has no need to download du file specified by external entity, just delete it. */
}E_OP_TYPE_T;

typedef enum{
		EME_FILE_URL_REMOTE_FILE = 0,  /**< remote file */
		EME_FILE_URL_LOCAL_FILE = 1,   /**< local file */
}E_FILE_URL_TYPE;

typedef enum{
				eForceUpdate,
				eWaitUntilIdle,
				eDelayUpdate,
				eNextPowerOn
}E_UPDATE_MODE_T;


typedef struct {
#define EME_DOWNLOAD_URL_USRNAME_LEN 32
				INT8 usrname[EME_DOWNLOAD_URL_USRNAME_LEN];

#define EME_DOWNLOAD_URL_PASSWD_LEN 32
				INT8 passwd[EME_DOWNLOAD_URL_PASSWD_LEN];

				UINT32 delaySecondsToDownload;
}EME_DOWNLOAD_EXTENSION_T;

typedef struct {
#define EME_DOWNLOAD_URL_LEN 1024
	INT8 url[EME_DOWNLOAD_URL_LEN];/**< the required name: remote or local path  */
	E_UPDATE_MODE_T updateMode;
	UINT32 delaySecondsToUpdate;
	EME_DOWNLOAD_EXTENSION_T extension;
				
}EME_DOWNLOAD_T;


typedef struct {
#define EME_UPLOAD_URL_USRNAME_LEN 32
	INT8 usrname[EME_UPLOAD_URL_USRNAME_LEN];

#define EME_UPLOAD_URL_PASSWD_LEN 32
	INT8 passwd[EME_UPLOAD_URL_PASSWD_LEN];

	UINT32 delaySecondsToUpload;
}EME_UPLOAD_EXTENSION_T;

typedef struct {
#define EME_UPLOAD_URL_LEN 1024
	INT8 url[EME_UPLOAD_URL_LEN];/**< specify the dst path of required file, remote or local path */

	EME_UPLOAD_EXTENSION_T 	extension;
}EME_UPLOAD_T;

#define EME_DU_CHANGE_VER_LENGTH  32
#define EME_DU_CHANGE_UUID_LENGTH 40
#define EME_DU_CHANGE_STR_LENGTH  256
#define EME_DU_CHANGE_EE_LENGTH   1024

typedef struct {
    INT8 operation[EME_DU_CHANGE_VER_LENGTH];   /**< Required: Install, Update, Uninstall, install_at_bootup */
    INT8 URL[EME_DU_CHANGE_EE_LENGTH]; /**< Optional: Location of DU to be installed/update */
    INT8 UUID[EME_DU_CHANGE_UUID_LENGTH];    /**< Required: Unique ID to describe DU,36 bytes */
    INT8 username[EME_DU_CHANGE_STR_LENGTH]; /**< Optional username for file server */
    INT8 password[EME_DU_CHANGE_STR_LENGTH]; /**< Optional password for file server */
    INT8 execEnvFullPath[EME_DU_CHANGE_STR_LENGTH]; /**< Required for install: FullPath to the Exec Env */
    INT8 version[EME_DU_CHANGE_VER_LENGTH]; /**< Optional: Version of DU */
    UINT16 reqId;               /**< Optional: track req number */
}EME_DU_CHANGE_T;

typedef struct{

	E_FILE_TYPE_T filetype;
	E_OP_TYPE_T   optype;
    E_FILE_URL_TYPE  urltype;

	union{
		
		EME_DOWNLOAD_T download;
		
		EME_UPLOAD_T upload;

        EME_DU_CHANGE_T duOp;
	};

	// extral data
    union {
        INT8 certPasswd[64];
    };
}EME_FILE_TRANSMISSION_T;

typedef struct{
   INT8 operation[32];
   INT8 URL[1024];
   INT8 UUID[40];
   INT8 version[32];
   INT8 EUlist[1024];
   INT8 DUlist[256];
   INT32 faultCode;
   UINT16 reqId;
   INT8 startTime[256];
   INT8 completeTime[256];
   INT32 currentState;
   BOOL8 resolved;
} EME_DU_CHANGE_COMPLETE_IND;

typedef struct {
	E_FILE_TYPE_T filetype;
	E_OP_TYPE_T   optype;

    union{
	    EME_FILE_COMPLETE_IND file;
        EME_DU_CHANGE_COMPLETE_IND du;
    };
}EME_FILE_TRANSFER_COMPLETE_IND_T;

typedef struct {
#define CMPV2_FLOW_FORCE_IR 1
#define CMPV2_FLOW_FORCE_KUR 2
	INT32  forceType;
}EME_FORCE_CMPV2_FLOW_REQ_T;

typedef struct {
#define CMPV2_FLOW_FORCE_IR 1
#define CMPV2_FLOW_FORCE_KUR 2
	INT32  forceType;
	UINT32 result;
}EME_FORCE_CMPV2_FLOW_IND_T;

typedef struct{
	E_FILE_TYPE_T filetype;
	UINT32        certId;
}EME_DELETE_CERT_FILE_REQ_T;

typedef struct{
	E_FILE_TYPE_T filetype;
	UINT32        certId;
	UINT32        result;
}EME_DELETE_CERT_FILE_IND_T;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EME_SOCK_MSG_DEF_H__ */


