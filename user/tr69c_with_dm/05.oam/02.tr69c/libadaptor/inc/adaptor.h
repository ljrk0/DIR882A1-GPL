#ifndef ADAPTOR_H
#define ADAPTOR_H

#include "common_type_def.h"
#include "sockmsg.h"
//#include "debug_def.h"
#include "eme_sock_msg_def.h"

/** data type definition **/
typedef INT64    SINT64;
typedef INT32    SINT32;
typedef INT16    SINT16;
typedef INT8     SINT8;
typedef UINT8    UBOOL8;
typedef char *     BASE64;
typedef char *     HEXBINARY;
typedef char *     DATETIME;
typedef UINT16 MdmObjectId;
typedef UINT32 CmsEntityId;

#include "mdm_objectid.h"
#include "mdm_object.h"

#define MAX_UINT64 18446744073709551615ULL
#define MAX_SINT64 9223372036854775807LL
#define MIN_SINT64 (-1 * MAX_SINT64 - 1)
#define MAX_UINT32 4294967295U
#define MAX_SINT32 2147483647
#define MIN_SINT32 (-2147483648)
#define MAX_UINT16  65535
#define MAX_SINT16  32767
#define MIN_SINT16  (-32768)

#define EID_INVALID       0
#define EID_TR69C         1
#define EID_SMD          20
#define EID_SSK          21

#define NDA_ACCESS_TR69C              0x0001
#define NDA_ACCESS_TR64C              0x0002
#define NDA_ACCESS_HTTPD              0x0004
#define NDA_ACCESS_SNMPD              0x0008
#define NDA_ACCESS_CONSOLED           0x0010
#define NDA_ACCESS_TELNETD            0x0020
#define NDA_ACCESS_SSHD               0x0040
#define NDA_ACCESS_UPNP               0x0080
#define NDA_ACCESS_AVAILABLE1         0x0100
#define NDA_ACCESS_AVAILABLE10        0x0200
#define NDA_ACCESS_AVAILABLE11        0x0400
#define NDA_ACCESS_AVAILABLE12        0x0800
#define NDA_ACCESS_AVAILABLE2         0x1000
#define NDA_ACCESS_AVAILABLE14        0x2000
#define NDA_ACCESS_AVAILABLE15        0x4000

#define NDA_ACCESS_SUBSCRIBER  (NDA_ACCESS_TR64C        |                         \
                                NDA_ACCESS_HTTPD        | NDA_ACCESS_SNMPD      | \
                                NDA_ACCESS_CONSOLED     | NDA_ACCESS_TELNETD    | \
                                NDA_ACCESS_SSHD         | NDA_ACCESS_UPNP       | \
                       NDA_ACCESS_AVAILABLE1   | NDA_ACCESS_AVAILABLE10 | \
                       NDA_ACCESS_AVAILABLE11  | NDA_ACCESS_AVAILABLE12 | \
                       NDA_ACCESS_AVAILABLE2   | \
                       NDA_ACCESS_AVAILABLE14  | NDA_ACCESS_AVAILABLE15)

#define CMS_INVALID_FD  (-1)
#define CMS_INVALID_PID   0

#define CMS_IFNAME_LENGTH  BUFLEN_32   //!< broadcom interface name length

#ifndef TRUE
/** TRUE = 1
 */
#define TRUE  1
#endif

#ifndef FALSE
/** FALSE = 0
 */
#define FALSE 0
#endif

#define BUFLEN_4        4     //!< buffer length 4
#define BUFLEN_8        8     //!< buffer length 8
#define BUFLEN_16       16    //!< buffer length 16
#define BUFLEN_18       18    //!< buffer length 18 -- for ppp session id
#define BUFLEN_24       24    //!< buffer length 24 -- mostly for password
#define BUFLEN_32       32    //!< buffer length 32
#define BUFLEN_40       40    //!< buffer length 40
#define BUFLEN_48       48    //!< buffer length 48
#define BUFLEN_64       64    //!< buffer length 64
#define BUFLEN_80       80    //!< buffer length 80
#define BUFLEN_128      128   //!< buffer length 128
#define BUFLEN_256      256   //!< buffer length 256
#define BUFLEN_264      264   //!< buffer length 264
#define BUFLEN_512      512   //!< buffer length 512
#define BUFLEN_1024     1024  //!< buffer length 1024

#define NSECS_IN_SEC 1000000000
#define NSECS_IN_MSEC 1000000
#define NSECS_IN_USEC 1000
#define USECS_IN_SEC  1000000
#define USECS_IN_MSEC 1000
#define MSECS_IN_SEC  1000
#define SECS_IN_MINUTE   60
#define SECS_IN_HOUR     (SECS_IN_MINUTE * 60)
#define SECS_IN_DAY      (SECS_IN_HOUR * 24)

#define MAC_ADDR_LEN    6     //!< Mac address len in an array of 6 bytes
#define MAC_STR_LEN     17    //!< Mac String len with ":". eg: xx:xx:xx:xx:xx:xx

#define CMS_AF_SELECT_IPV4       0x0001
#define CMS_AF_SELECT_IPV6       0x0002

#define CMS_AF_SELECT_IPVX       (CMS_AF_SELECT_IPV4|CMS_AF_SELECT_IPV6)

#define CMS_MAX_FULLPATH_LENGTH     1024
#define CMS_MAX_FILENAME_LENGTH     100

#define UNKNOWN_DATETIME_STRING "0001-01-01T00:00:00Z"

#define UINT32_MAX (4294967295U)

#define TR69C_CONN_REQ_PORT      30005
#define TR69C_CONN_REQ_PATH      "/"

#define TR69C_EXIT_ON_IDLE_TIMEOUT       (2160356) 

#define SMD_SHUTDOWN_IN_PROGRESS "/tmp/smd_shutdown_in_progress"

#define UNINITIALIZED_SHM_ID  -1

#define MDMVS_LAN            "LAN"
#define MDMVS_LOOPBACK       "Loopback"
#define MDMVS_ANY_WAN        "Any_WAN"
#define MDMVS_COMPLETE       "Complete"
#define MDMVS_STATIC         "Static"
#define MDMVS_DHCP           "DHCP"
#define MDMVS_CONNECTED      "Connected"
#define MDMVS_INSTALLED      "Installed"
#define MDMVS_UNINSTALLED    "Uninstalled"
#define MDMVS_ERROR          "Error"
#define MDMVS_NOTICE         "Notice"
#define MDMVS_DEBUG          "Debug"
#define MDMVS_STANDARD_ERROR "Standard Error"
#define MDMVS_SYSLOG         "Syslog"
#define MDMVS_TELNET         "Telnet"
#define MDMVS_SYNCHRONIZED   "Synchronized"
#define MDMVS_UNSYNCHRONIZED "Unsynchronized"

#define MAX_MDM_INSTANCE_DEPTH    6
#define MAX_MDM_PARAM_NAME_LENGTH   255
#define MAX_MDM_FULL_PATH_LENGTH   255

#define EMPTY_INSTANCE_ID_STACK {0, {0,0,0,0}}

#define CMS_DYNAMIC_LAUNCH_SERVER_FD  3

#ifdef SUPPORT_IPV6
#define CMS_IPADDR_LENGTH  46          //!< IP address length to hold IPv6 in CIDR notation (match INET6_ADDRSTRLEN)
#else
#define CMS_IPADDR_LENGTH  BUFLEN_16   //!< IP address length to hold IPv4 in ASCII
#endif /* SUPPORT_IPV6 */

typedef struct
{
   UINT8 currentDepth;                     /**< next index in the instance array 
                                            *   to fill.  0 means empty. */
   UINT32 instance[MAX_MDM_INSTANCE_DEPTH];/**< Array of instance id's. */
} InstanceIdStack;

#define INIT_INSTANCE_ID_STACK(s)  (memset((void *)(s), 0, sizeof(InstanceIdStack)))

typedef struct dlist_node {
	struct dlist_node *next;   /**< next pointer */
	struct dlist_node *prev;   /**< previous pointer */
} DlistNode;

typedef struct
{
   DlistNode dlist;    /**< internal use only, must be first field */
   CmsEntityId eid;    /**< The entity id for this entry. */
   UINT16 accessBit;   /**< This entity's bit position in MdmNodeAttributes.accessBitMask (only 15 bits available) */
   char *name;         /**< Short name for this entity. */
   char *path;         /**< Full path to the executable. */
   char *altPath;      /**< Alternative path to exe, used for TR181 mode */
   char *runArgs;      /**< Args to pass to this executable on startup */
   UINT32 flags;       /**< Additional info about this entity, see EIF_xxx */
   UINT8  backLog;     /**< Backlog arg for the server socket */
   UINT16 port;        /**< If server, port number for the server socket. */
   UINT32 maxMemUsage;    /**< Maximum memory usage, in KB, 0 if unknown. */
   UINT32 normalMemUsage; /**< Typical memory usage, in KB, 0 if unknown. */
   UINT32 minMemUsage;    /**< Minimum memory usage, in KB, 0 if unknown. */
   UINT32 schedPolicy;   /**< (use vals from sched.h) 0=NORMAL, 1=FIFO, 2=RR */
   UINT32 schedPriority; /**< 99 is highest, 1 is lowest, but see bcm_realtime.h */
   UINT32 cpuMask;    /**< bitmask of CPU's this thread is allowed to run on.  1=TP0, 2=TP1, 3=both (0=both) */
   char *cpuGroupName;  /**< name of cpugroup this belongs to (leave blank for default) */
} CmsEntityInfo;


typedef struct
{
   UINT32 sec;   /**< Number of seconds since some arbitrary point. */
   UINT32 nsec;  /**< Number of nanoseconds since some arbitrary point. */
} CmsTimestamp;

typedef enum 
{
   URL_PROTO_HTTP=0, /**< http */
   URL_PROTO_HTTPS=1, /**< https */
   URL_PROTO_FTP=2,   /**< ftp */
   URL_PROTO_TFTP=3   /**< tftp */
} UrlProto;

typedef enum
{
   CMS_CONNECTION_MODE_DEFAULT=0,      /**< Default connection mdoe - single wan service over 1 connection */          
   CMS_CONNECTION_MODE_VLANMUX=1,      /**< Vlan mux connection mdoe - multiple vlan service over 1 connection */          
   CMS_CONNECTION_MODE_MSC=2,          /**< MSC connection mdoe - multiple wan service over 1 connection */           
} ConnectionModeType;

typedef enum
{
   CMSRET_SUCCESS              = 0,     /**<Success. */
   CMSRET_METHOD_NOT_SUPPORTED = 9000,  /**<Method not supported. */
   CMSRET_REQUEST_DENIED       = 9001,  /**< Request denied (no reason specified). */
   CMSRET_INTERNAL_ERROR       = 9002,  /**< Internal error. */
   CMSRET_INVALID_ARGUMENTS    = 9003,  /**< Invalid arguments. */
   CMSRET_RESOURCE_EXCEEDED    = 9004,  /**< Resource exceeded.
                                        *  (when used in association with
                                        *  setParameterValues, this MUST not be
                                        *  used to indicate parameters in error)
                                        */
   CMSRET_INVALID_PARAM_NAME   = 9005,  /**< Invalid parameter name.
                                        *  (associated with set/getParameterValues,
                                        *  getParameterNames,set/getParameterAtrributes)
                                        */
   CMSRET_INVALID_PARAM_TYPE   = 9006,  /**< Invalid parameter type.
                                        *  (associated with set/getParameterValues)
                                        */
   CMSRET_INVALID_PARAM_VALUE  = 9007,  /**< Invalid parameter value.
                                        *  (associated with set/getParameterValues)
                                        */
   CMSRET_SET_NON_WRITABLE_PARAM = 9008,/**< Attempt to set a non-writable parameter.
                                        *  (associated with setParameterValues)
                                        */
   CMSRET_NOTIFICATION_REQ_REJECTED = 9009, /**< Notification request rejected.
                                            *  (associated with setParameterAttributes)
                                            */
   CMSRET_DOWNLOAD_FAILURE     = 9010,  /**< Download failure.
                                         *  (associated with download or transferComplete)
                                         */
   CMSRET_UPLOAD_FAILURE       = 9011,  /**< Upload failure.
                                        *  (associated with upload or transferComplete)
                                        */
   CMSRET_FILE_TRANSFER_AUTH_FAILURE = 9012,  /**< File transfer server authentication
                                              *  failure.
                                              *  (associated with upload, download
                                              *  or transferComplete)
                                              */
   CMSRET_UNSUPPORTED_FILE_TRANSFER_PROTOCOL = 9013,/**< Unsupported protocol for file
                                                    *  transfer.
                                                    *  (associated with upload or
                                                    *  download)
                                                    */
   CMSRET_FILE_TRANSFER_UNABLE_JOIN_MULTICAST = 9014,/**< File transfer failure,
                                                    *  unable to join multicast
                                                    *  group.
                                                    */
   CMSRET_FILE_TRANSFER_UNABLE_CONTACT_FILE_SERVER = 9015,/**< File transfer failure,
                                                    *  unable to contact file server.
                                                    */
   CMSRET_FILE_TRANSFER_UNABLE_ACCESS_FILE = 9016,/**< File transfer failure,
                                                    *  unable to access file.
                                                    */
   CMSRET_FILE_TRANSFER_UNABLE_COMPLETE = 9017,/**< File transfer failure,
                                                    *  unable to complete download.
                                                    */
   CMSRET_FILE_TRANSFER_FILE_CORRUPTED = 9018,/**< File transfer failure,
                                                    *  file corrupted.
                                                    */
   CMSRET_FILE_TRANSFER_FILE_AUTHENTICATION_ERROR = 9019,/**< File transfer failure,
                                                    *  file authentication error.
                                                    */
   CMSRET_FILE_TRANSFER_FILE_TIMEOUT = 9020,/**< File transfer failure,
                                                    *  download timeout.
                                                    */
   CMSRET_FILE_TRANSFER_FILE_CANCELLATION_NOT_ALLOW = 9021,/**< File transfer failure,
                                                    *  cancellation not permitted.
                                                    */
   CMSRET_INVALID_UUID_FORMAT = 9022,/**< Invalid UUID Format
                                                    * (associated with ChangeDUState)
                                                    */
   CMSRET_UNKNOWN_EE = 9023,/**< Unknown Execution Environment
                                                    * (associated with ChangeDUState)
                                                    */

   CMSRET_EE_DISABLED = 9024,/**< Execution Environment disabled
                                                    * (associated with ChangeDUState)
                                                    */
   CMSRET_DU_EE_MISMATCH = 9025,/**< Execution Environment and Deployment Unit mismatch
                                                    * (associated with ChangeDUState:install/update)
                                                    */
   CMSRET_DU_DUPLICATE = 9026,/**< Duplicate Deployment Unit
                                                    * (associated with ChangeDUState:install/update)
                                                    */
   CMSRET_SW_MODULE_SYSTEM_RESOURCE_EXCEEDED = 9027,/**< System resources exceeded
                                                    * (associated with ChangeDUState:install/update)
                                                    */
   CMSRET_DU_UNKNOWN = 9028,/**< Unknown Deployment Unit
                                                    * (associated with ChangeDUState:update/uninstall)
                                                    */
   CMSRET_DU_STATE_INVALID = 9029,/**< Invalid Deployment Unit State
                                                    * (associated with ChangeDUState:update)
                                                    */
   CMSRET_DU_UPDATE_DOWNGRADE_NOT_ALLOWED = 9030,/**< Invalid Deployment Unit Update, downgrade not permitted
                                                    * (associated with ChangeDUState:update)
                                                    */
   CMSRET_DU_UPDATE_VERSION_NOT_SPECIFIED = 9031,/**< Invalid Deployment Unit Update, version not specified
                                                    * (associated with ChangeDUState:update)
                                                    */

   CMSRET_DU_UPDATE_VERSION_EXISTED= 9032,/**< Invalid Deployment Unit Update, version already exists
                                                    * (associated with ChangeDUState:update)
                                                    */
   CMSRET_SUCCESS_REBOOT_REQUIRED = 9800, /**< Config successful, but requires reboot to take effect. */
   CMSRET_SUCCESS_UNRECOGNIZED_DATA_IGNORED = 9801,  /**<Success, but some unrecognized data was ignored. */
   CMSRET_SUCCESS_OBJECT_UNCHANGED = 9802,  /**<Success, furthermore object has not changed, returned by STL handler functions. */
   CMSRET_SUCCESS_APPLY_NOT_COMPLETE = 9803, /**< Config validated/commited, but requires more action to take effect. */
   CMSRET_NO_MORE_INSTANCES = 9804,     /**<getnext operation cannot find any more instances to return. */
   CMSRET_MDM_TREE_ERROR = 9805,         /**<Error during MDM tree traversal */
   CMSRET_WOULD_DEADLOCK = 9806, /**< Caller is requesting a lock while holding the same lock or a different one. */
   CMSRET_LOCK_REQUIRED = 9807,  /**< The MDM lock is required for this operation. */
   CMSRET_OP_INTR = 9808,      /**<Operation was interrupted, most likely by a Linux signal. */
   CMSRET_TIMED_OUT = 9809,     /**<Operation timed out. */
   CMSRET_DISCONNECTED = 9810,  /**< Communications link is disconnected. */
   CMSRET_MSG_BOUNCED = 9811,   /**< Msg was sent to a process not running, and the
                                 *   bounceIfNotRunning flag was set on the header.  */
   CMSRET_OP_ABORTED_BY_USER = 9812,  /**< Operation was aborted/discontinued by the user */
   CMSRET_FAIL_REBOOT_REQUIRED = 9813,  /**<Config failed, and now system is in a bad state requiring reboot. */
   CMSRET_RECURSION_ERROR = 9817,     /**< too many levels of recursion */
   CMSRET_OPEN_FILE_ERROR = 9818,     /**< open file error */
   CMSRET_EAGAIN_ERROR = 9820,        /**< socket write EAGAIN error */
   CMSRET_SOCKET_ERROR = 9821,        /**< socket error */
   CMSRET_KEY_GENERATION_ERROR = 9830,     /** certificate key generation error */
   CMSRET_INVALID_CERT_REQ = 9831,     /** requested certificate does not match with issued certificate */
   CMSRET_INVALID_CERT_SUBJECT = 9832,     /** certificate has invalid subject information */
   CMSRET_OBJECT_NOT_FOUND = 9840,     /** failed to find object */

   CMSRET_INVALID_FILENAME = 9850,  /**< filename was not given for download */
   CMSRET_INVALID_IMAGE = 9851,     /**< bad image was given for download */
   CMSRET_INVALID_CONFIG_FILE = 9852,  /**< invalid config file was detected */
   CMSRET_CONFIG_PSI = 9853,         /**< old PSI/3.x config file was detected */
   CMSRET_IMAGE_FLASH_FAILED = 9854, /**< could not write the image to flash */
   CMSRET_RESOURCE_NOT_CONFIGURED = 9855, /**< requested resource is not configured/found */
} CmsRet;

typedef struct _uuid_t
{
   UINT32 time_low;
   UINT16 time_mid;
   UINT16 time_hi_and_version;
   UINT16 clock_seq;
   char node[6];
} uuid_t;

#define MAX_ADJUSTMENT 10
#define MAXFDS	128


/** cms_msg.h **/
typedef struct
{
   UINT32  delayMs; /**< Number of milliseconds in the future to deliver this message. */

} RegisterDelayedMsgBody;

typedef struct
{
   char name[BUFLEN_64];              /**< name of configuration file */
   char version[BUFLEN_16];           /**< version of configuration file */
   char date[BUFLEN_64];              /**< date when config is updated */
   char description[BUFLEN_256];      /**< description of config file */
} vendorConfigUpdateMsgBody;

typedef struct
{
   char diagnosticsState[BUFLEN_32];  /**< Ping state: requested, none, completed... */
   char interface[BUFLEN_32];   /**< interface on which ICMP request is sent */
   char host[BUFLEN_32]; /**< Host -- either IP address form or hostName to send ICMP request to */
   UINT32 numberOfRepetitions; /**< Number of ICMP requests to send */
   UINT32    timeout;	/**< Timeout in seconds */
   UINT32    dataBlockSize;	/**< DataBlockSize  */
   UINT32    DSCP;	/**< DSCP */
   UINT32    successCount;	/**< SuccessCount */
   UINT32    failureCount;	/**< FailureCount */
   UINT32    averageResponseTime;	/**< AverageResponseTime */
   UINT32    minimumResponseTime;	/**< MinimumResponseTime */
   UINT32    maximumResponseTime;	/**< MaximumResponseTime */
   CmsEntityId requesterId;
}PingDataMsgBody;

typedef enum 
{
   CMS_MSG_SYSTEM_BOOT                          = 0x10000250, /**< system has booted, delivered to apps
                                                                 *   EIF_LAUNCH_ON_STARTUP set in their
                                                                 *   CmsEntityInfo.flags structure.
                                                                 */
   CMS_MSG_APP_LAUNCHED                         = 0x10000251, /**< Used by apps to confirm that launch succeeded.
                                                                 *   Sent from app to smd in cmsMsg_init.
                                                                 */
   CMS_MSG_WAN_LINK_UP                          = 0x10000252, /**< wan link is up (includes dsl, ethernet, etc) */
   CMS_MSG_WAN_LINK_DOWN                        = 0x10000253, /**< wan link is down */
   CMS_MSG_WAN_CONNECTION_UP                    = 0x10000254, /**< WAN connection is up (got IP address) */
   CMS_MSG_WAN_CONNECTION_DOWN                  = 0x10000255, /**< WAN connection is down (lost IP address) */
   CMS_MSG_ETH_LINK_UP                          = 0x10000256, /**< eth link is up (only if eth is used as LAN interface) */
   CMS_MSG_ETH_LINK_DOWN                        = 0x10000257, /**< eth link is down (only if eth is used as LAN interface) */
   CMS_MSG_USB_LINK_UP                          = 0x10000258, /**< usb link is up (only if eth is used as LAN interface) */
   CMS_MSG_USB_LINK_DOWN                        = 0x10000259, /**< usb link is down (only if eth is used as LAN interface) */
   CMS_MSG_ACS_CONFIG_CHANGED                   = 0x1000025A, /**< ACS configuration has changed. */
   CMS_MSG_DELAYED_MSG                          = 0x1000025B, /**< This message is delivered to when delayed msg timer expires. */
   CMS_MSG_TR69_ACTIVE_NOTIFICATION             = 0x1000025C, /**< This message is sent to tr69c when one or more
                                                       *   parameters with active notification attribute
                                                       *   has had their value changed.
                                                       */
   CMS_MSG_WAN_ERRORSAMPLES_AVAILABLE           = 0x1000025D,/**< WAN connection has vectoring error samples available */
   CMS_MSG_WAN_ERRORSAMPLES_AVAILABLE_LINE1     = 0x1000025E,/**< WAN connection has vectoring error samples available for line 1, keep CMS_MSG_WAN_ERRORSAMPLES_AVAILABLE+1*/
   CMS_MSG_GET_SHMID                            = 0x10000261, /**< Used by apps to get shmid from smd. */
   CMS_MSG_SHMID                                = 0x10000262, /**< Sent from ssk to smd when shmid is obtained. */
   CMS_MSG_MDM_INITIALIZED                      = 0x10000263, /**< Sent from ssk to smd when MDM has been initialized. */
   CMS_MSG_DHCPC_STATE_CHANGED                  = 0x10000264, /**< Sent from dhcp client when state changes, see also DhcpcStateChangeMsgBody */
   CMS_MSG_PPPOE_STATE_CHANGED                  = 0x10000265, /**< Sent from pppoe when state changes, see also PppoeStateChangeMsgBody */
   CMS_MSG_DHCP6C_STATE_CHANGED                 = 0x10000266, /**< Sent from dhcpv6 client when state changes, see also Dhcp6cStateChangeMsgBody */
   CMS_MSG_PING_STATE_CHANGED                   = 0x10000267, /**< Ping state changed (completed, or stopped) */
   CMS_MSG_DHCPD_RELOAD		                    = 0x10000268, /**< Sent to dhcpd to force it reload config file without restart */
   CMS_MSG_DHCPD_DENY_VENDOR_ID	                = 0x10000269, /**< Sent from dhcpd to notify a denied request with some vendor ID */
   CMS_MSG_DHCPD_HOST_INFO                      = 0x1000026A, /**< Sent from dhcpd to ssk to inform of lan host add/delete */
   CMS_MSG_TRACERT_STATE_CHANGED                = 0x1000026B, /**< Traceroute state changed (completed, or stopped) */
   CMS_MSG_DNSPROXY_RELOAD	                    = 0x10000270, /**< Sent to dnsproxy to force it reload config file without restart */
   CMS_MSG_SNTP_STATE_CHANGED 	                = 0x10000271, /**< SNTP state changed */
   CMS_MSG_DNSPROXY_IPV6_CHANGED                = 0x10000272, /**< Sent to dnsproxy to inform the DProxy IPv6 DNS server address */
   CMS_MSG_DNSPROXY_GET_STATS	                = 0x10000273, /**< Sent to dnsproxy to get DNS query error statistic */
   CMS_MSG_MCPD_RELOAD	                        = 0x10000276, /**< Sent to mcpd to force it reload config file without restart */
   CMS_MSG_MCPD_CTL   	                        = 0x10000277, /**< Sent to mcpd to force it reload config file without restart */
   CMS_MSG_CONFIG_WRITTEN                       = 0x10000280, /**< Event sent when a config file is written. */
   CMS_MSG_CONFIG_UPLOAD_COMPLETE               = 0x10000281, /**< Event sent when a remote configuration cycle has ended. */
   CMS_MSG_DHCPC_GATEWAY_INFO                   = 0x10000282, /**< Sent from dhcp client when information associated with a connected Internet 
                                                                   Gateway Device is included in dhcp ack packet, see also DhcpcGatewayInfoMsgBody */

   CMS_MSG_SET_PPP_UP                           = 0x10000290, /* Sent to ppp when set ppp up manually */
   CMS_MSG_SET_PPP_DOWN                         = 0x10000291, /* Sent to ppp when set ppp down manually */  

   CMS_MSG_DNSPROXY_DUMP_STATUS                 = 0x100002A1, /* Tell dnsproxy to dump its current status */
   CMS_MSG_DNSPROXY_DUMP_STATS                  = 0x100002A2, /* Tell dnsproxy to dump its statistics */
   CMS_MSG_RASTATUS6_INFO                       = 0x100002A3, /**< Sent from rastatus6 when RA is received, see also RAStatus6MsgBody */
   CMS_MSG_GET_TR69C_SESSION_STATUS             = 0x100002B0, /* get TR69C session status (active[1], ended[0]) */
   
   CMS_MSG_WLAN_CHANGED          	            = 0x10000300,  /**< wlmngr jhc*/

   CMS_MSG_SNMPD_CONFIG_CHANGED                 = 0x1000301, /**< ACS configuration has changed. XXX Ugh, missing a 0 */
   CMS_MSG_MANAGEABLE_DEVICE_NOTIFICATION_LIMIT_CHANGED = 0x1000302, /**< Notification Limit of number of management device. XXX Ugh, missing a 0 */

   CMS_MSG_STORAGE_ADD_PHYSICAL_MEDIUM          = 0x1000310, /**< XXX Ugh, missing a 0 */
   CMS_MSG_STORAGE_REMOVE_PHYSICAL_MEDIUM       = 0x1000311, /**< XXX Ugh, missing a 0 */
   CMS_MSG_STORAGE_ADD_LOGICAL_VOLUME           = 0x1000312, /**< XXX Ugh, missing a 0 */
   CMS_MSG_STORAGE_REMOVE_LOGICAL_VOLUME        = 0x1000313, /**< XXX Ugh, missing a 0 */

   CMS_MSG_INTFSTACK_LOWERLAYERS_CHANGED        = 0x10000320,  /**< tell ssk about change in LowerLayers param */
   CMS_MSG_INTFSTACK_ALIAS_CHANGED              = 0x10000321,  /**< tell ssk about change in Alias param */
   CMS_MSG_INTFSTACK_OBJECT_DELETED             = 0x10000322,  /**< tell ssk about object delete */
   CMS_MSG_INTFSTACK_STATIC_ADDRESS_CONFIG      = 0x10000323,  /**< tell ssk about static address configuration */
   CMS_MSG_INTFSTACK_RESERVED_END               = 0x1000032F,  /**< End of reserved range for interface stack messages */

   CMS_MSG_DIAG                                 = 0x10000790, /**< request diagnostic to be run */
   CMS_MSG_TR69_GETRPCMETHODS_DIAG              = 0x10000791, /**< request tr69c send out a GetRpcMethods */
   CMS_MSG_DSL_LOOP_DIAG_COMPLETE               = 0x10000792, /**< dsl loop diagnostic completes */
   CMS_MSG_DSL_SELT_DIAG_COMPLETE               = 0x10000793, /**< dsl SELT diagnostic completes */
   CMS_MSG_SPDSVC_DIAG_COMPLETE                 = 0x10000794, /** <speed service test completes */
   CMS_MSG_DIAG_RESERVED_END                    = 0x100007BF,

   CMS_MSG_INTERNAL_NOOP                        = 0x100007C0, /**< used internally by CMS MSG sub-system.  Apps which get this should just ignore it and free it. */

   CMS_MSG_REGISTER_DELAYED_MSG                 = 0x10000800, /**< request a message sometime in the future. */
   CMS_MSG_UNREGISTER_DELAYED_MSG               = 0x10000801, /**< cancel future message delivery. */
   CMS_MSG_REGISTER_EVENT_INTEREST              = 0x10000802, /**< request receipt of the specified event msg. */
   CMS_MSG_UNREGISTER_EVENT_INTEREST            = 0x10000803, /**< cancel receipt of the specified event msg. */

   CMS_MSG_START_APP                            = 0x10000807, /**< request smd to start an app; pid is returned in the wordData */
   CMS_MSG_RESTART_APP                          = 0x10000809, /**< request smd to stop and then start an app; pid is returned in the wordData */
   CMS_MSG_STOP_APP                             = 0x1000080A, /**< request smd to stop an app */
   CMS_MSG_IS_APP_RUNNING                       = 0x1000080B, /**< request to check if the the application is running or not */
   CMS_MSG_APP_TERMINATED                       = 0x1000080C, /**< register to smd for application termination info. */
   CMS_MSG_TERMINATE                            = 0x1000080D,  /**< request app to terminate, a response means action has started. */

   CMS_MSG_REBOOT_SYSTEM                        = 0x10000850,  /**< request smd to reboot, a response means reboot sequence has started. */

   CMS_MSG_DUMP_EID_INFO                        = 0x1000085A,  /**< request smd to dump its eid info DB */
   CMS_MSG_RESCAN_EID_INFO                      = 0x1000085B,  /**< request smd to rescan the eid info files (future) */
   CMS_MSG_ADD_EID_INFO                         = 0x1000085C,  /**< request smd to add given eid info to its DB (future) */
   CMS_MSG_APPLY_EID_INFO                       = 0x1000085D,  /**< request smd to apply EID settings (future) */

   CMS_MSG_SET_LOG_LEVEL                        = 0x10000860,  /**< request app to set its log level. */
   CMS_MSG_SET_LOG_DESTINATION                  = 0x10000861,  /**< request app to set its log destination. */

   CMS_MSG_MEM_DUMP_STATS                       = 0x1000086A,  /**< request app to dump its memstats */
   CMS_MSG_MEM_DUMP_TRACEALL                    = 0x1000086B,  /**< request app to dump all of its mem leak traces */
   CMS_MSG_MEM_DUMP_TRACE50                     = 0x1000086C,  /**< request app to its last 50 mem leak traces */
   CMS_MSG_MEM_DUMP_TRACECLONES                 = 0x1000086D,  /**< request app to dump mem leak traces with clones */

   CMS_MSG_LOAD_IMAGE_STARTING                  = 0x10000870,  /**< notify smd that image network loading is starting. */
   CMS_MSG_LOAD_IMAGE_DONE                      = 0x10000871,  /**< notify smd that image network loading is done. */
   CMS_MSG_GET_CONFIG_FILE                      = 0x10000872,  /**< ask smd for a copy of the config file. */
   CMS_MSG_VALIDATE_CONFIG_FILE                 = 0x10000873,  /**< ask smd to validate the given config file. */
   CMS_MSG_WRITE_CONFIG_FILE                    = 0x10000874,  /**< ask smd to write the config file. */
   CMS_MSG_VENDOR_CONFIG_UPDATE                 = 0x10000875,  /**<  the config file. */

   CMS_MSG_GET_WAN_LINK_STATUS                  = 0x10000880,  /**< request current WAN LINK status. */
   CMS_MSG_GET_WAN_CONN_STATUS                  = 0x10000881,  /**< request current WAN Connection status. */
   CMS_MSG_GET_LAN_LINK_STATUS                  = 0x10000882,  /**< request current LAN LINK status. */
  
   CMS_MSG_WATCH_WAN_CONNECTION                 = 0x10000890,  /**< request ssk to watch the dsl link status and then change the connectionStatus for bridge, static MER and ipoa */
   CMS_MSG_WATCH_DSL_LOOP_DIAG                  = 0x10000891,  /**< request ssk to watch the dsl loop diag and then update the stats */
   CMS_MSG_WATCH_DSL_SELT_DIAG                  = 0x10000892,  /**< request ssk to watch the dsl SELT diag and then update the stats */

   CMS_MSG_GET_LEASE_TIME_REMAINING             = 0x100008A0,  /**< ask dhcpd how much time remains on lease for particular LAN host */
   CMS_MSG_GET_DEVICE_INFO                      = 0x100008A1,  /**< request system/device's info */
   CMS_MSG_REQUEST_FOR_PPP_CHANGE               = 0x100008A2,  /**< request for disconnect/connect ppp  */
   CMS_MSG_EVENT_SNTP_SYNC                      = 0x100008A3,  /**< sntp send sync delta value */

   CMS_MSG_MOCA_RESERVED_BEGIN                  = 0x100008B0, /**< ==== Begin Moca Msg reserved range */
   CMS_MSG_MOCA_WRITE_LOF                       = 0x100008B1, /**< mocad reporting last operational frequency */
   CMS_MSG_MOCA_READ_LOF                        = 0x100008B2, /**< mocad reporting last operational frequency */
   CMS_MSG_MOCA_WRITE_MRNONDEFSEQNUM            = 0x100008B3, /**< mocad reporting moca reset non-def sequence number */
   CMS_MSG_MOCA_READ_MRNONDEFSEQNUM             = 0x100008B4, /**< mocad reporting moca reset non-def sequence number */
   CMS_MSG_MOCA_NOTIFICATION                    = 0x100008B5, /**< application reporting that it has updated moca parameters */
   CMS_MSG_MOCA_LAN_LINK_UP                     = 0x100008BA, /**< smd reporting MOCA LAN link up */
   CMS_MSG_MOCA_LAN_LINK_DOWN                   = 0x100008BB, /**< smd reporting MOCA LAN link down */
   CMS_MSG_MOCA_RESERVED_END                    = 0x100008BF, /**< ==== End of Moca Msg reserved range */


   CMS_MSG_QOS_DHCP_OPT60_COMMAND               = 0x100008C0, /**< QoS Vendor Class ID classification command */
   CMS_MSG_QOS_DHCP_OPT77_COMMAND               = 0x100008C1, /**< QoS User   Class ID classification command */

   CMS_MSG_EPONMAC_BOOT_COMPLETE                = 0x10001F00, /**< notification from eponapp that EPON mac has booted. */
   CMS_MSG_PORT_LOOP_DETECT                     = 0x10001F20, /**< EPON ONU port loop detect oam */
   CMS_MSG_PORT_DISABLE_LOOPED                  = 0x10001F21, /**< EPON ONU port disable looped oam */
   CMS_MSG_EPON_LINK_STATUS_CHANGED             = 0x10001F22, /**< EPON ONU link status changed */
   CMS_MSG_EPON_GET_LINK_STATUS                 = 0x10001F23, /**< EPON ONU link status */
   
   CMS_MSG_VOICE_CONFIG_CHANGED                 = 0x10002000, /**< Voice Configuration parameter changed private event msg. */
   CMS_MSG_VODSL_BOUNDIFNAME_CHANGED            = 0x10002001, /**< vodsl BoundIfName param has changed. */
   CMS_MSG_SHUTDOWN_VODSL                       = 0x10002002, /**< Voice shutdown request. */
   CMS_MSG_START_VODSL                          = 0x10002003, /**< Voice start request. */
   CMS_MSG_REBOOT_VODSL                         = 0x10002004, /**< Voice reboot request. This is for the voice reboot command */
   CMS_MSG_RESTART_VODSL                        = 0x10002005, /**< Voice re-start request. This is to restart the call manager when the IP address changes*/
   CMS_MSG_INIT_VODSL                           = 0x10002006, /**< Voice init request. */
   CMS_MSG_DEINIT_VODSL                         = 0x10002007, /**< Voice init request. */
   CMS_MSG_RESTART_VODSL_CALLMGR                = 0x10002008, /**< Voice call manager re-start request. */
   CMS_MSG_DEFAULT_VODSL                        = 0x10002009, /**< Voice call manager set defaults request. */
   CMS_MSG_VOICE_NTR_CONFIG_CHANGED             = 0x10002009, /**< Voice NTR Configuration parameter changed private event msg. */
   CMS_MSG_VOICE_GET_RTP_STATS                  = 0x10002010, /**< Voice get RTP PM stats msg (OMCI). */
   CMS_MSG_ROUTING_UPDATE                       = 0x10002012, /**< Voice routing-specific element changed. */
   CMS_MSG_VOICE_LOGLVL_CHANGED                 = 0x10002013, /**< Voice logging level changed. */

   CMS_MSG_VOICE_DIAG                           = 0x10002100, /**< request voice diagnostic to be run */
   CMS_MSG_VOICE_STATISTICS_REQUEST             = 0x10002101, /**< request for Voice call statistics */
   CMS_MSG_VOICE_STATISTICS_RESPONSE            = 0x10002102, /**< response for Voice call statistics */
   CMS_MSG_VOICE_STATISTICS_RESET               = 0x10002103, /**< request to reset Voice call statistics */
   CMS_MSG_VOICE_CM_ENDPT_STATUS                = 0x10002104, /**< request for Voice Line obj */
   CMS_MSG_VODSL_IS_READY_FOR_DEINIT            = 0x10002105, /**< query if the voice app is ready to deinit */

   CMS_MSG_VOICE_DECT_START                     = 0x100021A0, /**< request for number of DECT endpoints */
   CMS_MSG_VOICE_DECT_STOP                      = 0x100021A1, /**< request for number of DECT endpoints */
   CMS_MSG_VOICE_DECT_MEM_SET                   = 0x100021A2, /**< request for number of DECT endpoints */
   CMS_MSG_VOICE_DECT_MEM_GET                   = 0x100021A3, /**< request for number of DECT endpoints */
   CMS_MSG_VOICE_DECT_MODE_SET                  = 0x100021A4, /**< request for number of DECT endpoints */
   CMS_MSG_VOICE_DECT_MODE_GET                  = 0x100021A5, /**< request for number of DECT endpoints */
   CMS_MSG_VOICE_DECT_EVENT                     = 0x100021A6, /**< DECT EVENT raised by DECT endpoints */
   CMS_MSG_VOICE_DECT_READY                     = 0x100021A7, /**< DECT Module is ready */
   CMS_MSG_VOICE_DECT_CM_EVENT                  = 0x100021A8, /**< Call manager events or states */
   CMS_MSG_VOICE_DECT_CALL_CTL_CMD              = 0x100021A9, /**< DECT Call Control commands*/
   CMS_MSG_VOICE_DECT_LIST_UPDATE               = 0x100021AA, /**< DECT List update event */

   CMS_MSG_VOICE_DECT_OPEN_REG_WND              = 0x100021F0, /**< request for opening DECT registration window */
   CMS_MSG_VOICE_DECT_CLOSE_REG_WND             = 0x100021F1, /**< request for closing DECT registration window */
   CMS_MSG_VOICE_DECT_INFO_REQ                  = 0x100021F2, /**< request for Voice DECT status information */
   CMS_MSG_VOICE_DECT_INFO_RSP                  = 0x100021F3, /**< response for Voice DECT status information */
   CMS_MSG_VOICE_DECT_AC_SET                    = 0x100021F4, /**< request for Voice DECT Access Code Set */
   CMS_MSG_VOICE_DECT_HS_INFO_REQ               = 0x100021F5, /**< request for Voice DECT handset status information */
   CMS_MSG_VOICE_DECT_HS_INFO_RSP               = 0x100021F6, /**< response for Voice DECT handset status information */
   CMS_MSG_VOICE_DECT_HS_DELETE                 = 0x100021F7, /**< request for deleting a handset from DECT module */
   CMS_MSG_VOICE_DECT_HS_PING                   = 0x100021F8, /**< request for pinging a handset from DECT module */
   CMS_MSG_VOICE_DECT_NUM_ENDPT                 = 0x100021F9, /**< request for number of DECT endpoints */
   CMS_MSG_VOICE_DECT_REGHSETLIST_UPDATE        = 0x100021FA, /**< Event indicating change in number of registered dect handset */
   CMS_MSG_VOICE_DECT_SYNC_DATE_TIME            = 0x100021FB, /**< request for date and time synchronization */

   CMS_MSG_GET_GPON_OMCI_STATS                  = 0x10002200, /**< request GPON OMCI statistics */
   CMS_MSG_OMCI_COMMAND_REQUEST                 = 0x10002201, /**< GPON OMCI command message request */
   CMS_MSG_OMCI_COMMAND_RESPONSE                = 0x10002202, /**< GPON OMCI command message response */
   CMS_MSG_OMCI_DEBUG_GET_REQUEST               = 0x10002203, /**< GPON OMCI debug get message request */
   CMS_MSG_OMCI_DEBUG_GET_RESPONSE              = 0x10002204, /**< GPON OMCI debug get message response */
   CMS_MSG_OMCI_DEBUG_SET_REQUEST               = 0x10002205, /**< GPON OMCI debug set message request */
   CMS_MSG_OMCI_DEBUG_SET_RESPONSE              = 0x10002206, /**< GPON OMCI debug set message response */
   CMS_MSG_OMCI_DEBUG_MKERR_SWDLERR1            = 0x10002207, /**< GPON OMCI debug drop next section to cause missing section error */
   CMS_MSG_OMCI_DEBUG_MKERR_SWDLERR2            = 0x10002208, /**< GPON OMCI debug drop final section of next window to cause no response on final window section error */
   CMS_MSG_OMCI_DEBUG_MKERR_SWDLERR3            = 0x10002209, /**< GPON OMCI debug corrupt next section to cause CRC error on SW DL image */


   CMS_MSG_OMCI_IGMP_ADMISSION_CONTROL          = 0x10002220, /**< mcpd request admission control from omcid */
   CMS_MSG_OMCI_MLD_ADMISSION_CONTROL           = 0x10002221, /**< mcpd request admission control from omcid */
   CMS_MSG_OMCI_CAPTURE_STATE_ON                = 0x10002230, /**< Start the capture of OMCI msgs from OLT */
   CMS_MSG_OMCI_CAPTURE_STATE_OFF               = 0x10002231, /**< Stop the capture of OMCI msgs from OLT */
   CMS_MSG_OMCI_CAPTURE_REPLAY_ON               = 0x10002232, /**< Start the playback of OMCI msgs */
   CMS_MSG_OMCI_CAPTURE_REPLAY_OFF              = 0x10002233, /**< Start the playback of OMCI msgs */
   CMS_MSG_OMCI_CAPTURE_VIEW                    = 0x10002234, /**< Start the display OMCI msgs from a file */
   CMS_MSG_OMCI_CAPTURE_DOWNLOAD                = 0x10002235, /**< Download internal OMCI msg capture file */
   CMS_MSG_OMCI_CAPTURE_UPLOAD                  = 0x10002236, /**< Upload a file of OMCI msgs to replace internal file */
   CMS_MSG_OMCI_PROMISC_SET_REQUEST             = 0x10002240, /**< GPON OMCI Promisc set message request */
   CMS_MSG_OMCI_PROMISC_SET_RESPONSE            = 0x10002241, /**< GPON OMCI Promisc set message response */
   CMS_MSG_OMCI_ETH_PORT_TYPE_SET_REQUEST       = 0x10002245, /**< GPON OMCI Eth Port Type set message request */
   CMS_MSG_OMCI_ETH_PORT_TYPE_GET_REQUEST       = 0x10002246, /**< GPON OMCI Eth Port Type get message request */
   CMS_MSG_OMCI_ETH_PORT_TYPE_SET_RESPONSE      = 0x10002247, /**< GPON OMCI Eth Port Type set message response */
   CMS_MSG_OMCI_GPON_WAN_SERVICE_STATUS_CHANGE  = 0x10002250, /**< OMCI-->RG - Wan service status change notification */
   CMS_MSG_OMCI_RG_WAN_SERVICE_STAUTS_CHANGE    = 0x10002251, /**< RG-->OMCI - WAN service status change notification */
   CMS_MSG_OMCI_MCPD_MIB_RESET                  = 0x10002252, /**< OMCID to MCPD Mib Reset message to clean up  VLANCTL rules and CMF flows */
   CMS_MSG_OMCI_VOIP_MIB_RESET                  = 0x10002253, /**< OMCID to VOIP Mib Reset message to clean up Voice stack */

   CMS_MSG_CMF_SEND_REQUEST                     = 0x10002301, /**< CMF File Send message request */
   CMS_MSG_CMF_SEND_RESPONSE                    = 0x10002302, /**< CMF File Send message response */

   CMS_MSG_OMCIPMD_SYNC                         = 0x10002400, /**< OMCIPMD command message request to re-sync timers and PMs. */
   CMS_MSG_OMCIPMD_DEBUG                        = 0x10002401, /**< OMCIPMD command to turn debug on/off for specific PMs. */
   CMS_MSG_OMCIPMD_MIB_RESET                    = 0x10002402, /**< OMCIPMD command to execute MIB_RESET. */
   CMS_MSG_OMCIPMD_ALARM_SEQ_SET                = 0x10002403, /**< OMCIPMD command to set OMCI Alarm Sequence Number. */
   CMS_MSG_OMCIPMD_ALARM_SEQ_GET                = 0x10002404, /**< OMCIPMD command to get OMCI Alarm Sequence Number. */
   CMS_MSG_OMCIPMD_SYNC_MIB_ADMINSTATE          = 0x10002405, /**< Make OMCIPMD to sync the admin states from CMS. */
   CMS_MSG_OMCIPMD_GET_STATS                    = 0x10002406, /**< OMCIPMD command to get OMCI PM stats for CLI. */

   CMS_MSG_OMCIPMD_MCAST_COUNTER_REQ            = 0x10002410, /**< OMCIPMD command message request for multicast RX octets. */
   CMS_MSG_OMCIPMD_MCAST_COUNTER_RSP            = 0x10002411, /**< OMCIPMD command message response for multicast RX octets. */

   CMS_MSG_OMCID_OMCIPMD_REQUEST                = 0x10002420, /**< OMCIPMD command message request. */
   CMS_MSG_ENCAPSULATED_OMCI                    = 0x10002421, /**< OMCIPMD encapsulated OMCI raw frame. */

   CMS_MSG_GET_NTH_GPON_WAN_LINK_INFO           = 0x10002440,  /**< Get GPON WAN LINK connection info */
   CMS_MSG_GPON_LINK_STATUS_CHANGE              = 0x10002441, /**< GPON Link Status Change */

   CMS_MSG_MODSW_BEGIN         = 0x10002500,  /**< start of Modular Software Message types */
   CMS_MSG_MODSW_RESERVIED     = 0x10002501,  /**< This range is reserved for Modular Software, do not add your messages here! */
   CMS_MSG_MODSW_END           = 0x100025FF,  /**< end of Modular Software Message types */

   CMS_MSG_BMU_BEGIN                    = 0x10002600,  /**< start of Battery Management Unit Message types */
   CMS_MSG_BMU_END                      = 0x1000261F,  /**< end of Battery Management Unit message types */

   CMS_MSG_WLAN_LINK_STATUS_CHANGED     = 0x10002655, /**<APSTA link event to ssk to update Wifi Wan status (obsolete) */
   CMS_MSG_WIFI_LINK_UP                 = 0x1000265A, /**< LAN side Wifi SSID link is up */
   CMS_MSG_WIFI_LINK_DOWN               = 0x1000265B, /**< LAN side Wifi SSID link is down */

   CMS_MSG_HOMEPLUG_LINK_STATUS_CHANGED = 0x10002700, /**< Homeplug link status changed; Begin homeplug reserved msg range */
   CMS_MSG_HOMEPLUG_LINK_UP             = 0x10002701, /**< Homeplug link up, ifname in body of msg */
   CMS_MSG_HOMEPLUG_LINK_DOWN           = 0x10002702, /**< Homeplug link down, ifname in body of msg */
   CMS_MSG_HOMEPLUG_RESERVED            = 0x10002703, /**< This range is reserved for Homeplug related messages, do not add your message here! */
   CMS_MSG_HOMEPLUG_END                 = 0x100027FF, /**< End of Homeplug reserved range */

   CMS_MSG_TRANSFER_COMPLETE_IND        = 0x10002800, /**< TransferComplete */
   CMS_MSG_FILE_TRANSFER_REQ            = 0x10002801, /**< download-request */
   CMS_MSG_REGISTERED_IND               = 0x10002802, /**< registered ind */
   CMS_MSG_APP_STOPPED                  = 0x10002803, /**< tr69c stop */

   CMS_MSG_CUSTOMER_RESERVED_BEGIN      = 0x20000000, /**< This range of messages are reserved for customer use */
   CMS_MSG_CUSTOMER_RESERVED_END        = 0x2FFFFFFF, /**< End of customer reserved range */

   CMS_MSG_FUTURE_RESERVED_BEGIN        = 0x30000000 /**< All messages from this point are reserved for future use */

} CmsMsgType;

typedef struct cms_msg_header
{
   CmsMsgType  type;  /**< specifies what message this is. */
   CmsEntityId src;   /**< CmsEntityId of the sender; for apps that can have
                       *   multiple instances, use the MAKE_SPECIFI_EID macro. */
   CmsEntityId dst;   /**< CmsEntityId of the receiver; for apps that can have
                       *   multiple instances, use the MAKE_SPECIFI_EID macro. */
   union {
      UINT16 all;     /**< All 16 bits of the flags at once. */
      struct {
         UINT16 event:1;    /**< This is a event msg. */
         UINT16 request:1;  /**< This is a request msg. */
         UINT16 response:1; /**< This is a response msg. */
         UINT16 available1:1;  /**< was requeue, no longer used */
         UINT16 bounceIfNotRunning:1; /**< Do not launch the app to receive this message if
                                       *  it is not already running. */
         UINT16 unused:11;  /**< For future expansion. */
      } bits;
   } flags;  /**< Modifiers to the type of message. */
   UINT16 sequenceNumber;     /**< "Optional", but read the explanation below.
                               *
                               * Senders of request or event message types
                               * are free to set this to whatever
                               * they want, or leave it unitialized.  Senders
                               * are not required to increment the sequence
                               * number with every new message sent.
                               * However, response messages must 
                               * return the same sequence number as the
                               * request message.
                               * 
                               */
   struct cms_msg_header *next;   /**< Allows CmsMsgHeaders to be chained. */
   UINT32 wordData;   /**< As an optimization, allow one word of user
                       *   data in msg hdr.
                       *
                       * For messages that have only one word of data,
                       * we can just put the data in this field.
                       * One good use is for response messages that just
                       * need to return a status code.  The message type
                       * determines whether this field is used or not.
                       */
   UINT32 dataLength; /**< Amount of data following the header.  0 if no additional data. */
} CmsMsgHeader;

#define flags_event        flags.bits.event      /**< Convenience macro for accessing event bit in msg hdr */
#define flags_request      flags.bits.request    /**< Convenience macro for accessing request bit in msg hdr */
#define flags_response     flags.bits.response   /**< Convenience macro for accessing response bit in msg hdr */
#define flags_bounceIfNotRunning flags.bits.bounceIfNotRunning   /**< Convenience macro for accessing bounceIfNotRunning bit in msg hdr */

#define EMPTY_MSG_HEADER   {0, 0, 0, {0}, 0, 0, 0, 0} /**< Initialize msg header to empty */

CmsRet cmsMsg_initWithFlags(CmsEntityId eid, UINT32 flags, void **msgHandler);
CmsRet cmsMsg_init(CmsEntityId eid, void **msgHandler);
void cmsMsg_cleanup(void **msgHandler);
CmsRet cmsMsg_send(void *msgHandler, const CmsMsgHeader *buf);
CmsRet cmsMsg_sendReply(void *msgHandler, const CmsMsgHeader *msg, CmsRet retCode);
CmsRet cmsMsg_sendAndGetReply(void *msgHandler, const CmsMsgHeader *buf);
CmsRet cmsMsg_sendAndGetReplyWithTimeout(void *msgHandler,
                                         const CmsMsgHeader *buf,
                                         UINT32 timeoutMilliSeconds);
CmsRet cmsMsg_sendAndGetReplyBuf(void *msgHandler, 
                                 const CmsMsgHeader *buf,
                                 CmsMsgHeader **replyBuf);
CmsRet cmsMsg_sendAndGetReplyBufWithTimeout(void *msgHandler, 
                                            const CmsMsgHeader *buf,
                                            CmsMsgHeader **replyBuf,
                                            UINT32 timeoutMilliSeconds);
CmsRet cmsMsg_receive(void *msgHandler, CmsMsgHeader **buf);
CmsRet cmsMsg_receiveWithTimeout(void *msgHandler,
                                 CmsMsgHeader **buf,
                                 UINT32 timeoutMilliSeconds);
void cmsMsg_putBack(void *msgHandler, CmsMsgHeader **buf);
void cmsMsg_sendNoop(void *msgHandler);
CmsMsgHeader *cmsMsg_duplicate(const CmsMsgHeader *buf);
CmsRet cmsMsg_getEventHandle(const void *msgHandler, void *eventHandle);
CmsEntityId cmsMsg_getHandleEid(const void *msgHandler);
UBOOL8 cmsMsg_isServiceReady(void);

/** cms_mem.h **/
#define ALLOC_ZEROIZE          0x01
#define ALLOC_SHARED_MEM       0x02

void *cmsMem_alloc(UINT32 size, UINT32 allocFlags);
void *cmsMem_realloc(void *buf, UINT32 size);
void cmsMem_free(void *buf);
char *cmsMem_strdup(const char *str);
char *cmsMem_strdupFlags(const char *str, UINT32 flags);
char *cmsMem_strndup(const char *str, UINT32 maxlen);
char *cmsMem_strndupFlags(const char *str, UINT32 maxlen, UINT32 flags);
UINT32 cmsMem_strnlen(const char *str, UINT32 maxlen, UBOOL8 *isTerminated);
void cmsMem_cleanup(void);

#define CMSMEM_FREE_BUF_AND_NULL_PTR(p) \
    do { \
      if ((p) != NULL) {cmsMem_free((p)); (p) = NULL;}   \
    } while (0)

#define CMSMEM_REPLACE_STRING(p, s) \
    do {\
    if ((p) != NULL) {cmsMem_free((p));} \
    (p) = cmsMem_strdup((s)); \
    } while (0)

#define CMSMEM_REPLACE_STRING_FLAGS(p, s, f) \
    do {\
      if ((p) != NULL) {cmsMem_free((p));} \
      (p) = cmsMem_strdupFlags((s), (f)); \
    } while (0)

#define REPLACE_STRING_IF_NOT_EQUAL(p, s)    \
    if ((p) != NULL) { \
    if (strcmp((p), (s))) { cmsMem_free((p)); (p) = cmsMem_strdup((s)); } \
    } else { \
      (p) = cmsMem_strdup((s));                 \
    } 

#define REPLACE_STRING_IF_NOT_EQUAL_FLAGS(p, s, f)     \
	if ((p) != NULL) { \
      if (strcmp((p), (s))) {cmsMem_free((p)); (p) = cmsMem_strdupFlags((s), (f)); } \
	} else { \
		(p) = cmsMem_strdupFlags((s), (f));       \
	} 

/** cms_tmr.h **/
typedef void (*CmsEventHandler)(void*);

#define CMS_EVENT_TIMER_NAME_LENGTH  32

CmsRet cmsTmr_init(void **tmrHandle);
void cmsTmr_cleanup(void **tmrHandle);
CmsRet cmsTmr_set(void *tmrHandle, CmsEventHandler func, void *ctxData, UINT32 ms, const char *name);
void cmsTmr_cancel(void *tmrHandle, CmsEventHandler func, void *ctxData);
CmsRet cmsTmr_pause(void *handle, CmsEventHandler func, void *ctxData);
CmsRet cmsTmr_resume(void *handle, CmsEventHandler func, void *ctxData);
CmsRet cmsTmr_replaceIfSooner(void *tmrHandle, CmsEventHandler func, void *ctxData, UINT32 ms, const char *name);
CmsRet cmsTmr_getTimeToNextEvent(const void *tmrHandle, UINT32 *ms);
UINT32 cmsTmr_getNumberOfEvents(const void *tmrHandle);
void cmsTmr_executeExpiredEvents(void *tmrHandle);
CmsRet cmsTmr_getTimeRemaining(const void *handle, CmsEventHandler func, void *ctxData, UINT32 *ms);
UBOOL8 cmsTmr_isEventPresent(const void *tmrHandle, CmsEventHandler func, void *ctxData);
void cmsTmr_dumpEvents(const void *tmrHandle);


/** cms_log.h **/
#define CMSLOG_HDRMASK_APPNAME    0x0001 
#define CMSLOG_HDRMASK_LEVEL      0x0002 
#define CMSLOG_HDRMASK_TIMESTAMP  0x0004
#define CMSLOG_HDRMASK_LOCATION   0x0008 
#define DEFAULT_LOG_LEVEL        LOG_LEVEL_ERR
#define DEFAULT_LOG_DESTINATION  LOG_DEST_STDERR
#define DEFAULT_LOG_HEADER_MASK (CMSLOG_HDRMASK_APPNAME|CMSLOG_HDRMASK_LEVEL|CMSLOG_HDRMASK_TIMESTAMP|CMSLOG_HDRMASK_LOCATION)
#define MAX_LOG_LINE_LENGTH      512

#define BCM_SYSLOG_FIRST_READ           -2
#define BCM_SYSLOG_READ_BUFFER_ERROR    -1
#define BCM_SYSLOG_READ_BUFFER_END      -3
#define BCM_SYSLOG_MAX_LINE_SIZE        255

#define LOG_LOGGER      1       /* error conditions */
#define LOG_ERR         3       /* error conditions */
#define LOG_NOTICE      5       /* error conditions */
#define LOG_DEBUG       7       /* error conditions */

typedef enum
{
   LOG_LEVEL_LOGGER = 1, /**< Message at log level. */
   LOG_LEVEL_ERR    = 3, /**< Message at error level. */
   LOG_LEVEL_NOTICE = 5, /**< Message at notice level. */
   LOG_LEVEL_DEBUG  = 7  /**< Message at debug level. */
} CmsLogLevel;

typedef enum
{
   LOG_DEST_STDERR  = 1,  /**< Message output to stderr. */
   LOG_DEST_SYSLOG  = 2,  /**< Message output to syslog. */
   LOG_DEST_TELNET  = 3   /**< Message output to telnet clients. */
} CmsLogDestination;

/* alarm list used by tr69c */
#define TR069_ALARM_PARAMETER_ERROR                 0//(REPORTER_ID_TR69<<24)|(ALARM_SEVERITY_MAJOR<<16)|(ALARM_GROUP_PROCESSING<<8)
#define TR069_ALARM_MEMORY_ALLOC_FAILED             0//(REPORTER_ID_TR69<<24)|(ALARM_SEVERITY_CRITICAL<<16)|(ALARM_GROUP_OSA<<8)
#define TR069_ALARM_TIMER_ERROR                     0//(REPORTER_ID_TR69<<24)|(ALARM_SEVERITY_CRITICAL<<16)|(ALARM_GROUP_OSA<<8)
#define TR069_ALARM_RPC_ERROR                       0//(REPORTER_ID_TR69<<24)|(ALARM_SEVERITY_MAJOR<<16)|(ALARM_GROUP_COMMUNICATION<<8)
#define TR069_ALARM_SND_RCV_MSG_ERROR               0//(REPORTER_ID_TR69<<24)|(ALARM_SEVERITY_MAJOR<<16)|(ALARM_GROUP_PROCESSING<<8)
#define TR069_ALARM_INTERNAL_ERROR                  0//(REPORTER_ID_TR69<<24)|(ALARM_SEVERITY_MAJOR<<16)|(ALARM_GROUP_PROCESSING<<8)
#define TR069_ALARM_SOCKET_ERROR                    0//(REPORTER_ID_TR69<<24)|(ALARM_SEVERITY_MAJOR<<16)|(ALARM_GROUP_COMMUNICATION<<8)
#define TR069_ALARM_ACS_CONNECT_FAILED              0//(REPORTER_ID_TR69<<24)|(ALARM_SEVERITY_MAJOR<<16)|(ALARM_GROUP_COMMUNICATION<<8)
#define TR069_ALARM_FILE_TRANMIT_FAILED             0//(REPORTER_ID_TR69<<24)|(ALARM_SEVERITY_MAJOR<<16)|(ALARM_GROUP_COMMUNICATION<<8)
#define TR069_ALARM_DB_ACCESS_ERROR                 0//(REPORTER_ID_TR69<<24)|(ALARM_SEVERITY_MAJOR<<16)|(ALARM_GROUP_PROCESSING<<8)
#define TR069_ALARM_PROCESS_ERROR                   0//(REPORTER_ID_TR69<<24)|(ALARM_SEVERITY_MAJOR<<16)|(ALARM_GROUP_PROCESSING<<8)

#define cmsLog_log(args...)  log_log((CmsLogLevel)LOG_LOGGER, __FILE__, __FUNCTION__, __LINE__, (UINT32)-1, args)
#define cmsLog_error(alarmid,args...)  log_log((CmsLogLevel)LOG_ERR, __FILE__, __FUNCTION__, __LINE__, alarmid, args)
#define cmsLog_notice(args...) log_log((CmsLogLevel)LOG_NOTICE, __FILE__, __FUNCTION__, __LINE__, 0, args)
#define cmsLog_debug(args...)  log_log((CmsLogLevel)LOG_DEBUG, __FILE__, __FUNCTION__, __LINE__, 0, args)

void log_log(CmsLogLevel level, const char *file, const char *func, UINT32 lineNum, UINT32 alarmid, const char *pFmt, ... );
void cmsLog_initWithName(CmsEntityId eid, const char *appName);
void cmsLog_init(CmsEntityId eid);
void cmsLog_cleanup(void);
void cmsLog_setLevel(CmsLogLevel level);
CmsLogLevel cmsLog_getLevel(void);
void cmsLog_setDestination(CmsLogDestination dest);
CmsLogDestination cmsLog_getDestination(void);
void cmsLog_setHeaderMask(UINT32 headerMask);
UINT32 cmsLog_getHeaderMask(void);
SINT32 cmsLog_readPartial(SINT32 ptr, char* buffer);


/** cms_ast.h **/
#define cmsAst_assert(expr) cmsAst_assertFunc(__FILE__, __LINE__, __STRING(expr), (expr))

void cmsAst_assertFunc(const char *filename, UINT32 lineNumber, const char *exprString, SINT32 expr);


/** cms_image.h **/
#define  CMS_IMAGE_MIN_LEN    2048
#define CMS_IMAGE_MAX_ALLOC_LEN    32 * 1024 * 1024
#define CMS_IMAGE_REQUIRED_LEN        4 * 1024 * 1024

typedef enum
{
    CMS_IMAGE_FORMAT_INVALID=0,   /**< invalid or unrecognized format */
    CMS_IMAGE_FORMAT_BROADCOM=1,  /**< broadcom image (with our header) */
    CMS_IMAGE_FORMAT_FLASH=2,     /**< raw flash image */
    CMS_IMAGE_FORMAT_XML_CFG=3,   /**< CMS XML config file */
    CMS_IMAGE_FORMAT_MODSW_LINUXPFP=4, /**< modular software Linux Primary Firmware Patch */
    CMS_IMAGE_FORMAT_MODSW_LINUXEE_DU=5, /**< modular software Linux Execution Environment Deployment Unit */
	CMS_IMAGE_FORMAT_PART1=0x10,   /**< Specify to write to partition1 */
    CMS_IMAGE_FORMAT_PART2=0x20,   /**< Specify to write to partition2 */
    CMS_IMAGE_FORMAT_NO_REBOOT=0x80 /**< Do not reboot after flashing  */
} CmsImageFormat;

CmsImageFormat cmsImg_validateImage(const char *imagePtr, UINT32 imageLen, void *msgHandle);
CmsRet cmsImg_writeImage(char *imagePtr, UINT32 imageLen, void *msgHandle);
CmsRet cmsImg_writeValidatedImage(char *imagePtr, UINT32 imageLen,
                                  CmsImageFormat format, void *msgHandle);
CmsRet cmsImg_writeValidatedImageEx(char *imagePtr, UINT32 imageLen,
                                    CmsImageFormat format, void *msgHandle,
                                    UINT32 opts);
UINT32 cmsImg_getImageFlashSize(void);
UINT32 cmsImg_getConfigFlashSize(void);
UINT32 cmsImg_getRealConfigFlashSize(void);
UBOOL8 cmsImg_isBackupConfigFlashAvailable(void);
UINT32 cmsImg_getBroadcomImageTagSize(void);
UBOOL8 cmsImg_willFitInFlash(UINT32 imageLen);
UBOOL8 cmsImg_isConfigFileLikely(const char *buf);
void cmsImg_sendLoadStartingMsg(void *msgHandle, const char *connIfName); 
void cmsImg_sendLoadDoneMsg(void *msgHandle); 
void cmsUtil_sendRequestRebootMsg(void *handle, UBOOL8 bReset);
CmsRet cmsImg_saveIfNameFromSocket(SINT32 socketfd, char *connIfName);
UBOOL8 cmsImg_isBcmTaggedImage(const char *imageBuf, UINT32 *imageSize);

/** cms_lck.h **/
#define CMSLCK_MAX_HOLDTIME    (5*MSECS_IN_SEC)
#define CMSLCK_FUNC_NAME_LENGTH   BUFLEN_64

typedef struct 
{
   UBOOL8         locked;       /**< TRUE if a process has the lock. */
   UINT8          funcCode;     /**< 's' for STL, 'r' for RCL */
   MdmObjectId    oid;          /**< current oid accessed */
   SINT32         lockOwner;    /**< Pid of current lock owner. */
   CmsTimestamp   timeAquired;   /**< Time stamp of when lock was aquired. */
   char           callerFuncName[CMSLCK_FUNC_NAME_LENGTH]; /**< Function name of caller who aquired the lock. */
} CmsLckInfo;

void cmsLck_getInfo(CmsLckInfo *lockInfo);
void cmsLck_dumpInfo(void);
CmsRet cmsLck_acquireLockTraced(const char* callerFuncName);
CmsRet cmsLck_acquireLockWithTimeoutTraced(const char* callerFuncName, UINT32 timeoutMilliSeconds);
void cmsLck_releaseLockTraced(const char* callerFuncName);

#define cmsLck_acquireLock()  cmsLck_acquireLockTraced(__FUNCTION__)
#define cmsLck_acquireLockWithTimeout(x)  cmsLck_acquireLockWithTimeoutTraced(__FUNCTION__, x)
#define cmsLck_releaseLock()  cmsLck_releaseLockTraced(__FUNCTION__)

/** cms_mdm.h **/
#define EMPTY_PATH_DESCRIPTOR {0, {0, {0, 0, 0, 0}}, {0}}

#define NDA_TR69_NO_NOTIFICATION      0
#define NDA_TR69_PASSIVE_NOTIFICATION 1
#define NDA_TR69_ACTIVE_NOTIFICATION  2

typedef enum
{
   MPT_STRING = 0,    /**< string. */
   MPT_INTEGER = 1,   /**< Signed 32 bit integer. */
   MPT_UNSIGNED_INTEGER = 2, /**<Unsigned 32 bit integer. */
   MPT_BOOLEAN = 3,   /**< Either 1 (true) or 0 (false). */
   MPT_DATE_TIME = 4, /**< string, in UTC, in ISO 8601 date-format. */
   MPT_BASE64 = 5,    /**< Base64 string representation of binary data. */
   MPT_HEX_BINARY = 6, /**<Hex string representation of binary data. */
   MPT_LONG64 = 7,     /**< signed 64 bit integer */
   MPT_UNSIGNED_LONG64 = 8  /**< unsigned 64 bit integer */
} MdmParamTypes;

typedef struct
{
   UINT16  accessBitMaskChange:1; /**< Used during set, indicates whether the accessBitMask should be set */
   UINT16  accessBitMask:15;      /**< Bit field containing access bit mask, see NDA_ACCESS_xxx. */
   UINT8   notificationChange:1;  /**< Used during set, indicates whether the notifiction field should be set */
   UINT8   notification:7;        /**< Type of notification requested. */
   UINT8   valueChanged:1;        /**< This param has active or passive notification, and its value has changed.
                                   *   This field is used by MDM internal storage.  Not used during set. */
   UINT8   reserved:7;            /**< Reserved bit fields. */
} MdmNodeAttributes;

typedef struct _MdmPathDescriptor
{
   MdmObjectId       oid;        /**< Object Identifier.
                                  * Just a number used to represent a generic
                                  * object.
                                  */
   InstanceIdStack   iidStack;   /**< Instance Id Stack.
                                  * A stack of instance IDs used together with
                                  * the oid to specify a specific instance of
                                  * an object.
                                  */
   char paramName[MAX_MDM_PARAM_NAME_LENGTH + 1]; /**< Parameter name.
                                  * If paramName is an empty string, then this
                                  * MdmPathDescriptor structure specifies an object.
                                  * Otherwise, this structure specifies a
                                  * full path to a specific parameter.
                                  */
   char fullpath[MAX_MDM_FULL_PATH_LENGTH + 1];/**< we keep the full path, then it will be used in later functions */

   struct _MdmPathDescriptor *pNext;
} MdmPathDescriptor;

CmsRet cmsMdm_initWithAcc(CmsEntityId eid, UINT32 acc, void *msgHandler, SINT32 *shmId);
CmsRet cmsMdm_init(CmsEntityId eid, void *msgHandler, SINT32 *shmId);
void cmsMdm_cleanup(void);
CmsRet cmsMdm_fullPathToPathDescriptor(const char *fullpath, MdmPathDescriptor *pathDesc);
CmsRet cmsMdm_pathDescriptorToFullPath(const MdmPathDescriptor *pathDesc, char **fullpath);
CmsRet cmsMdm_pathDescriptorToFullPathNoEndDot(const MdmPathDescriptor *pathDesc, char **fullpath);
char *cmsMdm_dumpIidStack(const InstanceIdStack *iidStack);
char *cmsMdm_dumpIidStackToBuf(const InstanceIdStack *iidStack,
                               char *buf,
                               UINT32 len);
SINT32 cmsMdm_compareIidStacks(const InstanceIdStack *iidStack1,
                               const InstanceIdStack *iidStack2);
SINT32 cmsMdm_compareIidStacksToDepth(const InstanceIdStack *iidStack1,
                                      const InstanceIdStack *iidStack2,
                                      UINT32 n);
UBOOL8 cmsMdm_isPathDescriptorExist(const MdmPathDescriptor *pathDesc);
UBOOL8 cmsMdm_isDataModelDevice2(void);


/** cms_phl.h **/

/** Set Parameter Value structure
 */
typedef struct
{
   MdmPathDescriptor pathDesc;   /**< Full path name of a parameter. */
   char              *pParamType; /**< pointer to parameter type string */
   char              *pValue;    /**< Pointer to parameter value string. */
   CmsRet            status;     /**< Paremeter value set status. */
                                                                               
} PhlSetParamValue_t;

/** Get Parameter Value structure
 */
typedef struct _PhlGetParamValue_t
{
   MdmPathDescriptor pathDesc;   /**< Full path name of a parameter. */
   char              *pValue;    /**< Pointer to parameter value string. */
   const char        *pParamType;/**< Pointer to Parameter type string. */
   UBOOL8        isTr69Password; /**< Is this parameter considered a password in
                                  * the TR-069 protocol. */ 
   struct _PhlGetParamValue_t *pNext;
} PhlGetParamValue_t;

/** Parameter Name-Property structure
 */
typedef struct
{
   MdmPathDescriptor pathDesc;   /**< Full path name of a parameter or an object. */
   UBOOL8            writable;   /**< Writable property of the parameter or object.
                                  * Whether or not the Parameter value can be
                                  * overwritten using the SetParameterValues function.
                                  * If Name is a child object, this indicates whether
                                  * the DelObjInstance or AddObjInstance functions
                                  * can be used to remove this instance or add other
                                  * instances.
                                  */
   UBOOL8        isTr69Password; /**< Is this parameter considered a password in
                                  * the TR-069 protocol. */                       
} PhlGetParamInfo_t;


/** Parameter Attributes structure
 */
typedef struct
{
   MdmPathDescriptor pathDesc;   /**< Full path name of a parameter or an object. */
   MdmNodeAttributes attributes;	/**< Parameter attribute settings. */
} PhlGetParamAttr_t;


/** Set Parameter Attributes structure
 */
typedef struct
{
   MdmPathDescriptor pathDesc;   /**< Full path name of a parameter or an object. */
   MdmNodeAttributes attributes;	/**< Parameter attribute settings. */
} PhlSetParamAttr_t;

INT8* CmsPhl_GetRootNode(INT8    *pFullPathName);
CmsRet cmsPhl_getParameterNames(MdmPathDescriptor *pNamePath,
                                UBOOL8            nextLevelOnly,
                                PhlGetParamInfo_t **pParamInfoList,
                                SINT32            *pNumEntries);
CmsRet cmsPhl_setParameterValues(PhlSetParamValue_t   *pSetParamValueList,
                              SINT32               numEntries);
CmsRet cmsPhl_getParameterValues(MdmPathDescriptor    *pNamePathList,
                                 SINT32               numNameEntries,
                                 PhlGetParamValue_t   **pParamValueList,
                                 SINT32               *pNumParamValueEntries,
								 UBOOL8               isAcsAction);
CmsRet cmsPhl_setParameterAttributes(PhlSetParamAttr_t *pSetParamAttrList,
                                  SINT32            numEntries);
CmsRet cmsPhl_getParameterAttributes(MdmPathDescriptor *pNamePathList,
                                     SINT32            numNameEntries,
                                     PhlGetParamAttr_t **pParamAttrList,
                                     SINT32            *pNumParamAttrEntries);
CmsRet cmsPhl_addObjInstance(MdmPathDescriptor *pNamePath);
CmsRet cmsPhl_delObjInstance(MdmPathDescriptor *pNamePath);
void cmsPhl_freeGetParamValueBuf(PhlGetParamValue_t *pBuf,
                                SINT32             numEntries);
void cmsPhl_freeGetParamAttrBuf(PhlGetParamAttr_t *pBuf,
                                SINT32             numEntries);
CmsRet cmsPhl_getAllParamValueChanges(PhlGetParamValue_t **pParamValue,
                                SINT32            *numParamValueChanges);
BOOL8 cmsPhl_CheckIfAnyParamValueChangePending(void);
void cmsPhl_clearAllParamValueChanges(void);
CmsRet cmsPhl_getDeviceInfo(Dev2DeviceInfoObject **deviceInfoObj);
CmsRet cmsPhl_getTr69cCfg(Tr69cCfgObject **tr69cCfg);
CmsRet cmsPhl_getGWStateData(void **GWState);
CmsRet cmsPhl_setGWStateData(void *GWState);
CmsRet cmsPhl_getTransferList(void **pSaveList, UINT16 *saveEntryCount);
CmsRet cmsPhl_setTransferList(void *pSaveList, UINT16 saveEntryCount);
CmsRet cmsPhl_getTimeObj(Dev2TimeObject **timeObj);
CmsRet cmsPhl_getAcsCfg(Dev2ManagementServerObject **acsCfg);
CmsRet cmsPhl_setAcsCfg(Dev2ManagementServerObject *acsCfg);
CmsRet cmsPhl_getDUObject(DUObject **pDuObj, SINT32 *numEntries);
void cmsPhl_free(void **mdmObj, SINT32 numEntries);


/** cms_mgm.h **/
void cmsMgm_invalidateConfigFlash(void);
CmsRet cmsMgm_saveConfigToFlash(void);
CmsRet cmsMgm_writeConfigToBuf(char *buf, UINT32 *len);
CmsRet cmsMgm_writeMdmToBuf(char *buf, UINT32 *len);
CmsRet cmsMgm_writeObjectToBuf(const MdmObjectId oid, char *buf, UINT32 *len);
CmsRet cmsMgm_readConfigFlashToBuf(char *buf, UINT32 *len);
CmsRet cmsMgm_validateConfigBuf(const char *buf, UINT32 len);
CmsRet cmsMgm_writeValidatedBufToConfigFlash(const char *buf, UINT32 len);

/** cms_tms.h **/
void cmsTms_get(CmsTimestamp *tms);
void cmsTms_delta(const CmsTimestamp *newTms,
                  const CmsTimestamp *oldTms,
                  CmsTimestamp *deltaTms);
UINT32 cmsTms_deltaInMilliSeconds(const CmsTimestamp *newTms,
                                  const CmsTimestamp *oldTms);
void cmsTms_addMilliSeconds(CmsTimestamp *tms, UINT32 ms);
CmsRet cmsTms_getXSIDateTime(UINT32 t, char *buf, UINT32 bufLen);
CmsRet cmsTms_getXSIDateTimeMicroseconds(UINT32 t, char *buf, UINT32 bufLen);
CmsRet cmsTms_getDaysHoursMinutesSeconds(UINT32 t, char *buf, UINT32 bufLen);
UINT32 cmsTms_getSeconds(void);

/** cms_msg_modsw.h **/
typedef enum 
{
   CMS_MSG_REQUEST_DU_STATE_CHANGE              = 0x10002500, /**< request Deployment Unit state change */
   CMS_MSG_REQUEST_EU_STATE_CHANGE              = 0x10002501, /**< request Execution Unit state change */
   CMS_MSG_GET_FILE_FROM_SERVER                 = 0x10002502, /**< get file from file server */
   CMS_MSG_PUT_FILE_TO_SERVER                   = 0x10002503, /**< put file on file server */
   CMS_MSG_OSGI_EXE_ENV_STATUS_CHANGE           = 0x10002504, /**< OSGI execution environment state change */
   CMS_MSG_REQUEST_BUNDLE_LIST                  = 0x10002505, /**< request for bundle list */
   CMS_MSG_RESPONSE_DU_STATE_CHANGE             = 0x10002507, /**< response Deployment Unit state change */

   CMS_MSG_START_EE    = 0x10002510, /**< start this execution env */
   CMS_MSG_STOP_EE     = 0x10002511, /**< stop this execution env */

   CMS_MSG_OSGID_PRINT                          = 0x10002530, /**< OSGID command to print debug info to console */

   CMS_MSG_LINMOSD_NEWFILE = 0x10002560, /**< tell linmosd to process a new file */

} CmsModSwMsgType;

typedef struct
{
   char operation[BUFLEN_32];   /**< Required: Install, Update, Uninstall, install_at_bootup */
   char URL[BUFLEN_1024]; /**< Optional: Location of DU to be installed/update */
   char UUID[BUFLEN_40];    /**< Required: Unique ID to describe DU,36 bytes */
   char username[BUFLEN_256]; /**< Optional username for file server */
   char password[BUFLEN_256]; /**< Optional password for file server */
   char execEnvFullPath[BUFLEN_256]; /**< Required for install: FullPath to the Exec Env */
   char version[BUFLEN_32]; /**< Optional: Version of DU */
   UINT16 reqId;               /**< Optional: track req number */
} DUrequestStateChangedMsgBody;

#define SW_MODULES_OPERATION_INSTALL              "install"
#define SW_MODULES_OPERATION_UNINSTALL            "uninstall"
#define SW_MODULES_OPERATION_UPDATE               "update"
#define SW_MODULES_OPERATION_START                "start"
#define SW_MODULES_OPERATION_STOP                 "stop"
#define SW_MODULES_OPERATION_INSTALL_AT_BOOTUP    "installBootup"
#define SW_MODULES_OPERATION_START_AT_BOOTUP      "startBootup"
#define SW_MODULES_OPERATION_LB                   "LB"

typedef struct
{
   char operation[BUFLEN_32];   /**< Install, Update, Uninstall, install_at_bootup */
   char URL[BUFLEN_1024]; /**< Location of DU to be installed/update */
   char UUID[BUFLEN_40];    /**< Unique ID to describe DU,36 bytes */
   char version[BUFLEN_32]; /**< Version of DU */
   char EUlist[BUFLEN_1024]; /**< list of comma seperated EU bundle ID */
   char DUlist[BUFLEN_256];  /**< list of comma seperated DU bundle ID */
   int faultCode;
   UINT16 reqId;
   char startTime[BUFLEN_256];
   char completeTime[BUFLEN_256];
   int currentState;
   UBOOL8 resolved;
} DUresponseStateChangedMsgBody;

typedef struct
{
   char operation[BUFLEN_32];   /**< Install, Update, Uninstall */
   char name[BUFLEN_32];        /**< Name of EU */
   char euid[BUFLEN_64];        /**< EUID of EU */
} EUrequestStateChangedMsgBody;


/** cms_strconv.h **/
#define IS_EMPTY_STRING(s)    ((s == NULL) || (*s == '\0'))
CmsRet cmsUtl_atmVpiVciStrToNum(const char *vpiVciStr, SINT32 *vpi, SINT32 *vci);
CmsRet cmsUtl_atmVpiVciNumToStr(const SINT32 vpi, const SINT32 vci, char *vpiVciStr);
CmsRet cmsUtl_macStrToNum(const char *macStr, UINT8 *macNum);
CmsRet cmsUtl_macNumToStr(const UINT8 *macNum, char *macStr);
CmsRet cmsUtl_strtol(const char *str, char **endptr, SINT32 base, SINT32 *val);
CmsRet cmsUtl_strtoul(const char *str, char **endptr, SINT32 base, UINT32 *val);
CmsRet cmsUtl_strtol64(const char *str, char **endptr, SINT32 base, SINT64 *val);
CmsRet cmsUtl_strtoul64(const char *str, char **endptr, SINT32 base, UINT64 *val);
void cmsUtl_strToLower(char *string);
CmsRet cmsUtl_parseUrl(const char *url, UrlProto *proto, char **addr, UINT16 *port, char **path);
CmsRet cmsUtl_getBaseDir(char *pathBuf, UINT32 pathBufLen);
CmsRet cmsUtl_getRunTimeRootDir(char *pathBuf, UINT32 pathBufLen);
CmsRet cmsUtl_getRunTimePath(const char *target, char *pathBuf, UINT32 pathBufLen);
CmsRet cmsUtl_parseDNS(const char *inDsnServers, char *outDnsPrimary, char *outDnsSecondary, UBOOL8 isIPv4);
CmsRet cmsUtl_concatDNS(const char *dns1, const char *dns2, char *serverList, UINT32 bufLen);
SINT32 cmsUtl_syslogModeToNum(const char *modeStr);
char * cmsUtl_numToSyslogModeString(SINT32 mode);
UBOOL8 cmsUtl_isValidSyslogMode(const char *modeStr);
SINT32 cmsUtl_syslogLevelToNum(const char *levelStr);
char * cmsUtl_numToSyslogLevelString(SINT32 level);
UBOOL8 cmsUtl_isValidSyslogLevel(const char *levelStr);
UBOOL8 cmsUtl_isValidSyslogLevelString(const char *level);
CmsRet cmsUtl_standardizeIp6Addr(const char *address, char *stdAddr);
UBOOL8 cmsUtl_isGUAorULA(const char *address);
UBOOL8 cmsUtl_isUlaPrefix(const char *prefix);
CmsRet cmsUtl_replaceEui64(const char *address1, char *address2);
CmsRet cmsUtl_getAddrPrefix(const char *address, UINT32 plen, char *prefix);
CmsRet cmsUtl_parsePrefixAddress(const char *prefixAddr, char *address, UINT32 *plen);
void cmsUtl_truncatePrefixFromIpv6AddrStr(char *ipv6AddrStr);
SINT32 cmsUtl_pppAuthToNum(const char *authStr);
char * cmsUtl_numToPppAuthString(SINT32 authNum);
CmsLogLevel cmsUtl_logLevelStringToEnum(const char *logLevel);
CmsLogDestination cmsUtl_logDestinationStringToEnum(const char *logDest);
UBOOL8 cmsUtl_isValidIpAddress(SINT32 af, const char* address);
UBOOL8 cmsUtl_isValidIpv4Address(const char* address);
UBOOL8 cmsUtl_isZeroIpvxAddress(UINT32 ipvx, const char *address);
UBOOL8 cmsUtl_isValidMacAddress(const char* macAddress);
UBOOL8 cmsUtl_isValidPortNumber(const char* portNumberStr);
SINT32 cmsUtl_strcmp(const char *s1, const char *s2);
SINT32 cmsUtl_strcasecmp(const char *s1, const char *s2);
SINT32 cmsUtl_strncmp(const char *s1, const char *s2, SINT32 n); 
SINT32 cmsUtl_strncasecmp(const char *s1, const char *s2, SINT32 n); 
char *cmsUtl_strstr(const char *s1, const char *s2);
char *cmsUtl_strcpy(char *dest, const char *src);
char *cmsUtl_strcat(char *dest, const char *src);
char *cmsUtl_strncpy(char *dest, const char *src, SINT32 dlen);
CmsRet cmsUtl_strncat(char *prefix, UINT32 prefixLen, const char *suffix);
SINT32 cmsUtl_strlen(const char *src);
UBOOL8 cmsUtl_isSubOptionPresent(const char *fullOptionString, const char *subOption);
void cmsUtl_getWanProtocolName(UINT8 protocol, char *name) ;
char *cmsUtl_getAggregateStringFromDhcpVendorIds(const char *vendorIds);
char *cmsUtl_getDhcpVendorIdsFromAggregateString(const char *aggregateString);
UINT32 cmsUtl_getNumberOfDhcpVendorIds(const char *aggregateString);
ConnectionModeType cmsUtl_connectionModeStrToNum(const char *connModeStr);
void cmsUtl_generateUuidStr(char *str, int len, unsigned char *macAddress);


/** cms_obj.h **/
#define OSF_NORMAL_SET        0x0000
#define OSF_NO_RCL_CALLBACK    0x0001
#define OSF_NO_ACCESSPERM_CHECK    0x0002

typedef struct
{
    UINT32 totalParams;
    char name[MAX_MDM_PARAM_NAME_LENGTH + 1];
    MdmParamTypes type;
    UINT32 minVal;
    UINT32 maxVal;
    void *val;

} MdmObjParamInfo;

CmsRet cmsObj_get(MdmObjectId oid,
                  const InstanceIdStack *iidStack,
                  UINT32 getFlags,
                  void **mdmObj);
CmsRet cmsObj_getNext(MdmObjectId oid,
                      InstanceIdStack *iidStack,
                      void **mdmObj);
CmsRet cmsObj_getNextFlags(MdmObjectId oid,
                           InstanceIdStack *iidStack,
                           UINT32 getFlags,
                           void **mdmObj);
CmsRet cmsObj_getNextInSubTree(MdmObjectId oid,
                               const InstanceIdStack *parentIidStack,
                               InstanceIdStack *iidStack,
                               void **mdmObj);
CmsRet cmsObj_getNextInSubTreeFlags(MdmObjectId oid,
                                    const InstanceIdStack *parentIidStack,
                                    InstanceIdStack *iidStack,
                                    UINT32 getFlags,
                                    void **mdmObj);
CmsRet cmsObj_getAncestor(MdmObjectId ancestorOid,
                          MdmObjectId decendentOid,
                          InstanceIdStack *iidStack,
                          void **mdmObj);
CmsRet cmsObj_getAncestorFlags(MdmObjectId ancestorOid,
                               MdmObjectId decendentOid,
                               InstanceIdStack *iidStack,
                               UINT32 getFlags,
                               void **mdmObj);
CmsRet cmsObj_getNthParam(const void *mdmObj,
                          const UINT32 paramNbr,
                          MdmObjParamInfo *paramInfo);
void cmsObj_free(void **mdmObj);
CmsRet cmsObj_set(const void *mdmObj,
                  const InstanceIdStack *iidStack);
CmsRet cmsObj_setFlags(const void *mdmObj,
                       const InstanceIdStack *iidStack,
                       UINT32 flags);
CmsRet cmsObj_setNthParam(void *mdmObj,
                          const UINT32 paramNbr,
                          const void *paramVal);
CmsRet cmsObj_clearStatistics(MdmObjectId oid,
                              const InstanceIdStack *iidStack);
CmsRet cmsObj_addInstance(MdmObjectId oid,
                          InstanceIdStack *iidStack);
CmsRet cmsObj_deleteInstance(MdmObjectId oid,
                             const InstanceIdStack *iidStack);
CmsRet cmsObj_dumpObject(const void *mdmObj);
CmsRet cmsObj_setNonpersistentInstance(const MdmObjectId oid,
                             const InstanceIdStack *iidStack);
CmsRet cmsObj_clearNonpersistentInstance(const MdmObjectId oid,
                             const InstanceIdStack *iidStack);
UBOOL8 cmsObj_isNonpersistentInstance(const MdmObjectId oid,
                             const InstanceIdStack *iidStack);


/** cms_fil.h **/
typedef struct cms_file_list_entry {
   DlistNode dlist;
   char filename[CMS_MAX_FILENAME_LENGTH];
} CmsFileListEntry;

UBOOL8 cmsFil_isFilePresent(const char *filename);
SINT32 cmsFil_getSize(const char *filename);
CmsRet cmsFil_copyToBuffer(const char *filename, UINT8 *buf, UINT32 *bufSize);
CmsRet cmsFil_writeToProc(const char *procFilename, const char *s);
CmsRet cmsFil_writeBufferToFile(const char *filename, const UINT8 *buf, UINT32 bufLen);
UBOOL8 cmsFil_isDirPresent(const char *dirname);
CmsRet cmsFil_removeDir(const char *dirname);
CmsRet cmsFil_makeDir(const char *dirname);
CmsRet cmsFil_getOrderedFileList(const char *dirname, DlistNode *dirHead);
void cmsFil_freeFileList(DlistNode *dirHead);
UINT32 cmsFil_scaleSizetoKB(long nblks, long blkSize);
CmsRet cmsFil_renameFile(const char *oldName, const char *newName);
CmsRet cmsFil_getNumFilesInDir(const char *dirname, UINT32 *num);
CmsRet cmsFil_getNumericalOrderedFileList(const char *dirname, DlistNode *dirHead);
CmsRet cmsFil_getIntPrefixFromFileName(char *fileName, UINT32 *pNum);

/** osgid.h **/
#define OSGI_NAME   "osgi4_2"
#define OSGI_TYPE   "OSGi Release 4.2"
#define OSGI_FELIX  "Apache Felix"
#define OSGI_FELIX_VERSION "Framework 3.0.1"
#define OSGID_WORK_DIR                "/tmp/osgi"
#define OSGID_FELIX_DIR               "/usr/local/felix"
#define OSGID_FELIX_EXEC              "bin/felix.jar"
#define OSGID_UNIX_SOCK "/tmp/osgi/interfacesock"  /* osgid unix domain name */
#define MAX_MSG_LEN  1024  /*max msg buf length between osgid and felix*/
#define TIMEOUT_RECV_FROM_FELIX   3000 /*timeout value for receive msg from felix*/
#define MAX_LINE_LEN 256
#define TIME_WAIT_FOR_FELIX_UP    60  /* 60 seconds */
#define RETRY_WAIT_FOR_FELIX_UP   4   
#define OSGID_LOCK_TIMEOUT  (60 * MSECS_IN_SEC)
#define MAX_UUID_LEN        36
#define MAX_ERROR_MSG_PRINTOUT 5
#define MAX_EU_LIST_FULL_PATH_LEN     1024
#define OSGID_MAX_LOG_MSG_LEN         128
#define JVM_DEFAULT_MIN_HEAP          4000  /* 4M */
#define JVM_DEFAULT_MAX_HEAP          10000  /* 10M */
#define JVM_DEFAULT_STACK_SIZE        256  /* 256k */
#define OSGID_FELIX_ACTIVATOR         "org.apache.felix.main.Main"
#define OSGID_FELIX_CP                "/usr/local/classpath/lib/classpath:/usr/local/classpath/share/classpath:/lib"
#define REQUEST_ENTRY_PENDING        1
#define REQUEST_ENTRY_PROCESSING     2
#define DU_CHANGE_REQ_ID_WEBUI_BASE           0        /* 1-100000 */
#define DU_CHANGE_REQ_ID_TR69C_BASE           100000   /* 100001-200000 */

#define CERT_PRESET_LOCATION_DIR           "/unitdata/presets/"
#define CERT_DOWNLOADED_LOCATION_DIR       "/unitdata/downloads/"
#define CERT_DOWNLOAD_CERT_PREFIX          "dl_"

typedef enum
{
   OSGID_OPERATION_NO_OP=0,
   OSGID_OPERATION_INSTALL,
   OSGID_OPERATION_UPDATE,
   OSGID_OPERATION_UNINSTALL
} OSGI_OPERATION;

typedef enum
{
   OSGID_UPDATE_ALL_DU=0,
   OSGID_UPDATE_ALL_MATCH_URL=1,
   OSGID_UPDATE_ONLY_UUID=2
} OSGI_UPDATE_ACTION;

typedef enum
{
   OSGID_REQUEST_NOT_START=0,
   OSGID_REQUEST_PROCESSING=1,
   OSGID_REQUEST_DONE=2
} OSGI_REQUEST_STATUS;

typedef struct lb_data
{
   int bundleId;
   char status[BUFLEN_16];
   int startLevel;
   char description[BUFLEN_64];
   char alias[BUFLEN_32];
   char version[BUFLEN_32];
   int faultCode;
   char errMsg[BUFLEN_256];
} *pLB_DATA, LB_DATA;

typedef struct request_data
{
   char operation[BUFLEN_32];   /**< Install, Update, Uninstall, install_at_bootup */
   char URL[BUFLEN_1024]; /**< Location of DU to be installed/update */
   char UUID[BUFLEN_40];    /**< Unique ID to describe DU,36 bytes */
   char username[BUFLEN_256]; /**< Optional username for file server */
   char password[BUFLEN_256]; /**< Optional password for file server */
   char executionEnv[BUFLEN_256]; /**< Environment to execute EU */
   char version[BUFLEN_32]; /**< Version of DU */
   char commandKey[BUFLEN_32]; /**< Command Key of op request */
   UINT16 reqId;
   char name[BUFLEN_32]; /**< Name of EU */
   char alias[BUFLEN_32]; /**< Alias Name of EU */
   char euid[BUFLEN_64]; /**< EUID of EU */
   OSGI_REQUEST_STATUS  requestStatus;
   CmsMsgType  type;  /**< specifies what message this is. */
   CmsEntityId src;  /**< CmsEntityId of the sender; so we can sent reply back to src*/ 
   int FaultCode;   /**< send back to src if exists */
   char ErrMsg[BUFLEN_256];
   char EUlist[BUFLEN_1024]; /**< list of comma seperated EU bundle ID */
   char DUlist[BUFLEN_256];  /**< list of comma seperated DU bundle ID */
} *pREQUEST_DATA, REQUEST_DATA;

/** qdm_modsw_ee.h **/
CmsRet qdmModsw_getExecEnvFullPathByNameLocked(const char *name,
                                        char *fullPathBuf, UINT32 bufLen);
CmsRet qdmModsw_getExecEnvFullPathByUuidLocked(const char *uuid,
                                        char *fullPathBuf, UINT32 bufLen);
CmsRet qdmModsw_getMngrEidByExecEnvFullPathLocked(const char *fullPath,
                                              CmsEntityId *mngrEid);
CmsRet qdmModsw_getExecEnvNameByFullPathLocked(const char *fullPath,
                                        char *nameBuf, UINT32 nameBufLen);
CmsRet qdmModsw_getDeployUnitFullPathByDuidLocked(const char *duid,
                                        char *fullPathBuf, UINT32 bufLen);
CmsRet qdmModsw_getDeployUnitNameByEuFullPathLocked(const char *euFullPath,
                                           char *duName, UINT32 duNameLen);
CmsRet qdmModsw_getExecUnitFullPathByEuidLocked(const char *euid,
                                        char *fullPathBuf, UINT32 bufLen);

/** cms_params_modsw.h **/
#define CMS_MODSW_DIR   "/data/modsw"
#define CMS_MODSW_TMP_DIR   "/data/modsw/tmp"
#define CMS_MODSW_DU_DIR    "/data/modsw/tr157du"
#define CMS_MODSW_OSGIEE_DU_DIR    "/data/modsw/tr157du/osgibundles"
#define CMS_MODSW_LINUXEE_DU_DIR    "/data/modsw/tr157du/linuxpkgs"
#define CMS_MODSW_LINUXPFP_DIR   "/data/modsw/tr157pfp"
#define CMS_MODSW_LINUXPFP_HEADER "BCMTR157:Linux:PrimFirmwrePatch;"
#define CMS_MODSW_LINUXEE_DU_HEADER "BCMTR157:LinuxEE:DeploymentUnit;"
#define CMS_MODSW_LINUX_HEADER1_LEN    32
#define CMS_MODSW_LINUX_HEADER2_LEN    128
#define CMS_MODSW_OUTER_HEADER_NAME    "outer-header.txt"

#define BCMPKG_VERSION                1
#define BCMPKG_KW_VERSION             "version"
#define BCMPKG_TK_VERSION             1001
#define BCMPKG_KW_INNERFILENAME       "innerFilename"
#define BCMPKG_TK_INNERFILENAME       1002
#define BCMPKG_INNERFILENAME_MAX_LEN  32
#define BCMPKG_KW_CHECKSUMALGO        "checksumAlgo"
#define BCMPKG_TK_CHECKSUMALGO        1003
#define BCMPKG_KW_CHECKSUM            "checksum"
#define BCMPKG_TK_CHECKSUM            1004
#define BCMPKG_SHA1_CHECKSUM_LEN      40

#define CMS_MODSW_INNER_HEADER_NAME    "inner-header.txt"
#define BCMPKG_KW_PKGVERSION          "pkgVersion"
#define BCMPKG_TK_PKGVERSION          2001
#define TR157_VERSION_STR_LEN         32
#define BCMPKG_KW_PKGDESCRIPTION      "pkgDescription"
#define BCMPKG_TK_PKGDESCRIPTION      2002
#define TR157_DESCRIPTION_STR_LEN     256
#define BCMPKG_KW_INNERVERSION        "innerVersion"
#define BCMPKG_TK_INNERVERSION        2003
#define BCMPKG_KW_INNERDESCRIPTION    "innerDescription"
#define BCMPKG_TK_INNERDESCRIPTION    2004
#define CMS_MODSW_DEBUG_LOG           "/data/modsw/tmp/DuLog"
#define LINUXEE_NAME     "Linux"
#define LINUXEE_TYPE     "Linux"
#define LINUXEE_VENDOR   "Linux"
#define LINUXEE_VERSION  "3.4"

/** cms_eid.h **/
#define EID_EID_MASK  0x0000ffff
#define EID_PID_MASK  0xffff0000
#define GENERIC_EID(e) (((UINT32) e) & EID_EID_MASK)
#define PID_IN_EID(e) ((((UINT32) e) & EID_PID_MASK) >> 16)
#define MAKE_SPECIFIC_EID(p, e) ((p << 16) | (((UINT32) e) & EID_EID_MASK))


#define TR069_DNS_RESOLV_TIMEOUT_MS     30000


const CmsEntityInfo *cmsEid_getEntityInfo(CmsEntityId eid);
const CmsEntityInfo *cmsEid_getEntityInfoAutoCreate(CmsEntityId eid);
const CmsEntityInfo *cmsEid_getEntityInfoByAccessBit(UINT16 bit);
const CmsEntityInfo *cmsEid_getEntityInfoByStringName(const char *stringName);
CmsRet cmsEid_getStringNamesFromBitMask(UINT16 bitMask, char **buf);
CmsRet cmsEid_getBitMaskFromStringNames(const char *buf, UINT16 *bitMask);
SINT32 cmsEid_getPid(void);
UINT32 cmsEid_getNumberOfEntityInfoEntries(void);
const CmsEntityInfo *cmsEid_getFirstEntityInfo(void);
const CmsEntityInfo *cmsEid_getNextEntityInfo(const CmsEntityInfo *eInfo);
void cmsEid_dumpEntityInfo(const CmsEntityInfo *eInfo);


/** cms_errorcodes.h **/
const char *cmsErr_getStringBuf(CmsRet ret, char *buf, UINT32 buflen);
const char *cmsErr_getString(CmsRet ret);


/** cms_psp.h **/
CmsRet cmsPsp_set(const char *key, const void *buf, UINT32 bufLen);
SINT32 cmsPsp_get(const char *key, void *buf, UINT32 bufLen);
SINT32 cmsPsp_list(char *buf, UINT32 bufLen);
CmsRet cmsPsp_clearAll(void);

/** cms_boardcmds.h **/
CmsRet devCtl_getBaseMacAddress(UINT8 *macAddrNum);
CmsRet devCtl_getMacAddress(UINT8 *macAddrNum, UINT32 ulId);
CmsRet devCtl_getMacAddresses(UINT8 *macAddrNum, UINT32 ulId, UINT32 num_addresses);
CmsRet devCtl_releaseMacAddresses(UINT8 *macAddrNum, UINT32 num_addresses);
CmsRet devCtl_releaseMacAddress(UINT8 *macAddrNum);
UINT32 devCtl_getNumEnetMacs(void);
UINT32 devCtl_getNumEnetPorts(void);
UINT32 devCtl_getSdramSize(void);
CmsRet devCtl_getChipId(UINT32 *chipId);
CmsRet devCtl_setImageState(int state);
int devCtl_getImageState(void);
int devCtl_getImageVersion(int partition, char *verStr, int verStrSize);
int devCtl_getBootedImagePartition(void);
int devCtl_getBootedImageId(void);
UINT32 devCtl_getPortMacType(unsigned short port, unsigned int *mac_type);
UINT32 devCtl_getNumFePorts(unsigned int *fe_ports);
UINT32 devCtl_getNumGePorts(unsigned int *ge_ports);
UINT32 devCtl_getNumVoipPorts(unsigned int *voip_ports);
UINT32 devCtl_getPortMap(unsigned int *port_map);

/** cms_xml.h **/
UBOOL8 cmsXml_isEscapeNeeded(const char *string);
CmsRet cmsXml_escapeString(const char *string, char **escapedString);
UBOOL8 cmsXml_isUnescapeNeeded(const char *string);
CmsRet cmsXml_unescapeString(const char *escapedString, char **string);

/** cms_unicode.h **/
UBOOL8 cmsUnicode_isUnescapeNeeded(const char *string);
CmsRet cmsUnicode_unescapeString(const char *string, char **unicodedString);


CmsRet qdmIpIntf_getDefaultLanIntfNameLocked_dev2(char *ifName);
CmsRet qdmIpIntf_getIpv4AddrAndSubnetByNameLocked_dev2(const char *ipIntfName,
                                                       char *ipAddress,
                                                       char *subnetMask);

CmsRet SendDownloadReqToOam(void *req);
void SendUploadReqToOam(void *req);
UBOOL8 handleTransferCompletePending(UBOOL8 InformNeeded);

typedef enum {
   eFirmwareUpgrade            =1,
   eWebContent                 =2,
   eVendorConfig               =3,
   eVendorLog                  =4,
   eAlarmReport                =11,    /**< Alarm report file, upload only, Vendor-specific identifier */
   ePerformanceReport          =12,     /**< Performance report file, upload only, Vendor-specific identifier */
   eVendorTraces               =13,     /**< flushtraces file, upload only, Vendor-specific identifier */
   eVendorCertReq              =14,     /**< certificate request file, upload only, Vendor-specific identifier */
   eVendorClCert               =15,     /**< client certificate file, download, Vendor-specific identifier */
   eVendorCaCert               =16,     /**< ca certificate file, download, Vendor-specific identifier */
   eVendorDbDefaultFile        =17,     /**< default db file(Device.xml), download only, Vendor-specific identifier */
   eVendorCorefiles            =18,     /**< core-dump file, upload only, Vendor-specific identifier */
   eVendorDbRawFiles           =19,      /**< raw db file(Device.xml), download or upload, Vendor-specific identifier */
   eVendorDbRawInner           =20,       /**< raw db file(Internal.xml), download or upload, Vendor-specific identifier */
   eVendorDbDefaultInner       =21,       /**< default db file(Internal.xml), download only, Vendor-specific identifier */
   eVendorTcpdumpFiles         =22,        /**< tcpdump files, upload only, Vendor-specific identifier */
   eVendorKernelLog            =23,         /**< kernel-log files, upload only, Vendor-specific identifier */
   eVendorFirmware             =24,          /**< vendor firmware, download only, Vendor-specific identifier */  
   eVendorDeviceParams         =25,           /**< vendor device params, download only, Vendor-specific identifier */  
   eVendorDbCnData             =26,           /**< cndata db file(CnData.xml), download only, Vendor-specific identifier */  
   eVendorDbCnDataDefault      =27,           /**< default cndata db file(CnData.xml), download only, Vendor-specific identifier */  
   eVendorTraceMdtFile         =28            /**< trace&mdt file, upload only, Vendor-specific identifier */  
} eFileType;

const char *cmsUtl_getFiletypeString(eFileType filetype);
eFileType cmsUtl_getVendorFiletype(const char *filetype_str);
E_FILE_TYPE_T cmsUtl_ConvertFromFiletype(eFileType fileType);
eFileType cmsUtl_ConvertToFiletype(E_FILE_TYPE_T fileType);
const char *getInformVendorEvtString(UINT32 evt);

void releaseEmeSessionIfAccquired(void);

void sendEmeBootComplete(void);

void updateAcsSocket(int fd);

void cmsPhl_updateLocalIPs(void);
CmsRet CmsUtl_checkUrlIPtype(INT8* Url, INT32* ipType, INT8* ipAddress);

CmsRet cmsUtl_get_certifcate_file(char *certificate_path, char *certificate_file, int certificate_file_length, char *certificate_passwd, int certificate_passwd_length);
void deploy_ssl_certificates(char *prvkeyPath, char *clcertPath, char *caPath);


#endif /* ADAPTOR_H */

