#ifndef __SHARED_H__
#define __SHARED_H__


#include <errno.h>
#include <arpa/inet.h>
#include "../../../config/autoconf.h"

#define WAN_ETH_PREFIX "wan_wan"
#define WAN_USB_PREFIX "usb_wan"
#define WAN_ADSL_PREFIX "adsl_wan"
#define WAN_VDSL_PREFIX "vdsl_wan"

#ifdef CONFIG_MULTI_WAN_SUPPORT

#define WAN_INTERNET_PREFIX "wan_wan0_"
#define WAN_IPTV_PREFIX "wan_wan2_"
#define WAN_VOIP_PREFIX "wan_wan3_"

#define WAN_INTERNET_INTERFACE "WAN"
#define WAN_IPTV_INTERFACE "IPTV VLAN"
#define WAN_VOIP_INTERFACE "VOIP VLAN"

#endif

#define LAN_ETH_PREFIX "lan"

#define INSTANCE_WHIT_SUFFIX_FUNC(index) \
	char ins_suffixbuf[255]; \
	snprintf(ins_suffixbuf, 255, "%d_", index);\

#define LAN_MAX_COUNT 4

#if 1
#define LAN_NAME_IFNAME         "ifname"
#define LAN_NAME_IPADDR         "ipaddr"
#define LAN_NAME_NETMASK        "netmask"
#define LAN_NAME_HWADDR         "mac"
#define LAN_NAME_PORT_MEMBER    "port_member"
#define LAN_NAME_DHCPS_ENABLE   "dhcps_enable"
#define LAN_NAME_DHCPS_IP_START "dhcps_start"
#define LAN_NAME_DHCPS_IP_END   "dhcps_end"
#define LAN_NAME_DHCPS_LEASE_TIME "dhcps_lease"
#define LAN_NAME_DHCPS_BROADCAST_ENABLE  "dhcps_broadcast_enable"
#define LAN_NAME_DOMAIN        "domain"
#define LAN_NAME_DNS_RELAY_ENABLE "dns_relay_enable"
#define LAN_NAME_MGT_LINK  "management_link"


#define WAN_NAME_ENABLE             "enable"
#define WAN_NAME_UNIT               "unit"
#define WAN_NAME_GATEWAY            "gateway"
#define WAN_NAME_IPADDR             "ipaddr"
#define WAN_NAME_NETMASK            "netmask"
#define WAN_NAME_PROTO              "proto"
#define WAN_NAME_PRIMARY            "primary"
#define WAN_NAME_DNS                "dns"
#define WAN_NAME_HOSTNAME           "hostname"
#define WAN_NAME_IFNAME             "ifname"
#define WAN_NAME_IFNAMES            "ifnames"
#define WAN_NAME_MTU                "mtu"
#define WAN_NAME_LEASE              "lease"
#define WAN_NAME_DOMAIN             "domain"
#define WAN_NAME_ROUTE              "route"
#define WAN_NAME_WINS               "wins"
#define WAN_NAME_HWNAME             "hwname"
#define WAN_NAME_DESC               "desc"
#define WAN_NAME_VLANID             "vlanid"

#define WAN_NAME_HWADDR             "hwaddr"
#define WAN_NAME_PPPOE_IFNAME       "pppoe_ifname"
#define WAN_NAME_PPPOE_USERNAME     "pppoe_username"
#define WAN_NAME_PPPOE_PASSWD       "pppoe_passwd"    
#define WAN_NAME_PPPOE_AC           "pppoe_ac"
#define WAN_NAME_PPPOE_KEEPALIVE    "pppoe_keepalive"
#define WAN_NAME_PPPOE_SERVICE      "pppoe_service"
#define WAN_NAME_PPPOE_MTU          "pppoe_mtu"
#define WAN_NAME_PPPOE_MRU          "pppoe_mru"
#define WAN_NAME_PPPOE_DEMAND       "pppoe_demand"    
#define WAN_NAME_PPPOE_IDLETIME     "pppoe_idletime"

#define WAN_NAME_PPTP_IFNAME        "pptp_ifname"               
#define WAN_NAME_PPTP_USERNAME      "pptp_username"            
#define WAN_NAME_PPTP_PASSWD        "pptp_passwd"               
#define WAN_NAME_PPTP_DEMAND        "pptp_demand"               
#define WAN_NAME_PPTP_IDLETIME      "pptp_idletime"             
#define WAN_NAME_PPTP_CLIENT        "pptp_client"               
#define WAN_NAME_PPTP_NETMASK       "pptp_netmask"              
#define WAN_NAME_PPTP_SERVER        "pptp_server"               
#define WAN_NAME_PPTP_GATEWAY       "pptp_gateway"              
#define WAN_NAME_PPTP_MRU           "pptp_mru"                  
#define WAN_NAME_PPTP_MTU           "pptp_mtu"                  
#define WAN_NAME_L2TP_IFNAME        "l2tp_ifname"               
#define WAN_NAME_L2TP_USERNAME      "l2tp_username"            
#define WAN_NAME_L2TP_PASSWD        "l2tp_passwd"               
#define WAN_NAME_L2TP_DEMAND        "l2tp_demand"               
#define WAN_NAME_L2TP_IDLETIME      "l2tp_idletime"             
#define WAN_NAME_L2TP_CLIENT        "l2tp_client"               
#define WAN_NAME_L2TP_NETMASK       "l2tp_netmask"              
#define WAN_NAME_L2TP_SERVER        "l2tp_server"               
#define WAN_NAME_L2TP_GATEWAY       "l2tp_gateway"              
#define WAN_NAME_L2TP_MRU           "l2tp_mru"                  
#define WAN_NAME_L2TP_MTU           "l2tp_mtu"                  

#define WAN_NAME_USB_MODEM_ISP            "modem_isp"
#define WAN_NAME_USB_MODEM_CARD_MODE      "modem_card_mode"
#define WAN_NAME_USB_MODEM_APN            "modem_apn"
#define WAN_NAME_USB_MODEM_DIALNUM        "modem_dialnum"
#define WAN_NAME_USB_MODEM_BAND           "modem_baud"
#define WAN_NAME_USB_MODEM_NET_MODE       "modem_net_mode"
#define WAN_NAME_USB_MODEM_PINCODE        "modem_pincode"
#define WAN_NAME_USB_MODEM_DATA_ITF       "modem_data_itf"
#define WAN_NAME_USB_MODEM_VENDOR_NAME    "modem_vendor_name"
#define WAN_NAME_USB_MODEM_PRODUCT_NAME   "modem_product_name"
#define WAN_NAME_USB_MODEM_CONTROL_ITF    "modem_control_itf"
#define WAN_NAME_USB_MODEM_DEFAULT_VID    "modem_default_vid"
#define WAN_NAME_USB_MODEM_DEFAULT_PID    "modem_default_pid"
#define WAN_NAME_USB_MODEM_TARGET_VID     "modem_target_vid"
#define WAN_NAME_USB_MODEM_TARGET_PID     "modem_target_pid"
#define WAN_NAME_USB_MODEM_PPPOU_IDLETIME "pppou_idletime"
#define WAN_NAME_USB_MODEM_PPPOU_MTU      "pppou_mtu"
#define WAN_NAME_USB_MODEM_PPPOU_USER     "pppou_user"
#define WAN_NAME_USB_MODEM_PPPOU_PASS     "pppou_pass"
#define WAN_NAME_USB_MODEM_PPPOU_IFNAMES  "pppou_ifname"

#define WAN_NAME_ADSL_PVC            "pvc"
#define WAN_NAME_ADSL_LATENCY        "latency"
#define WAN_NAME_ADSL_ENCAPS         "atmencaps"
#define WAN_NAME_ADSL_QOS            "atmqos"
#define WAN_NAME_ADSL_QOS_PCR        "atmpcr"
#define WAN_NAME_ADSL_QOS_SCR        "atmscr"
#define WAN_NAME_ADSL_QOS_MBS        "atmmbs"
#define WAN_NAME_ADSL_PVC_ENABLE     "pvc_enable"

#define WAN_NAME_VDSL_LATENCY        "latency"

#define WAN_NAME_DSL_CFG_MODULATION           "dsl_modulation"
#define WAN_NAME_DSL_CFG_PROFILES             "dsl_profiles"
#define WAN_NAME_DSL_CFG_US0_ENABLE           "dsl_us0_enable"
#define WAN_NAME_DSL_CFG_BITSWAP_ENABLE       "dsl_bitswap_enable"
#define WAN_NAME_DSL_CFG_SRA_ENABLE           "dsl_sra_enable"

#define WAN_NAME_DSL_ST_PHY_VERSION           "dsl_phy_version"
#define WAN_NAME_DSL_ST_STATUS                "dsl_status"
#define WAN_NAME_DSL_ST_CURR_MODULATION       "dsl_curr_modulation"
#define WAN_NAME_DSL_ST_LINK_POWER_STATE      "dsl_link_power_state"
#define WAN_NAME_DSL_ST_US_NOISE_MARGIN       "dsl_upstream_noise_margin"
#define WAN_NAME_DSL_ST_DS_NOISE_MARGIN       "dsl_downstream_noise_margin"
#define WAN_NAME_DSL_ST_US_ATTENUATION        "dsl_upstream_attenuation"
#define WAN_NAME_DSL_ST_DS_ATTENUATION        "dsl_downstream_attenuation"
#define WAN_NAME_DSL_ST_US_POWER              "dsl_upstream_power"
#define WAN_NAME_DSL_ST_DS_POWER              "dsl_downstream_power"
#define WAN_NAME_DSL_ST_US_MAX_RATE           "dsl_upstream_max_rate"
#define WAN_NAME_DSL_ST_DS_MAX_RATE           "dsl_downstream_max_rate"
#define WAN_NAME_DSL_ST_US_CURR_RATE          "dsl_upstream_curr_rate"
#define WAN_NAME_DSL_ST_DS_CURR_RATE          "dsl_downstream_curr_rate"

#define WAN_NAME_ETH_ST_LINK              "eth_wan_link"
#define WAN_NAME_USB_ST_LINK              "usb_wan_link"
#define WAN_NAME_DSL_ST_LINK              "dsl_wan_link"

#define WAN_NAME_DSLITE_CONF        		"dslite_conf"
#define WAN_NAME_DSLITE_AFTRIPV6ADDR       	"dslite_aftripv6addr"
#define WAN_NAME_DSLITE_B4IPV4ADDDR        	"dslite_b4ipv4addr"

#define WAN_NAME_IPV6_TYPE          		"ipv6_type"
#define WAN_NAME_IPV6_TYPE_DETECT         	"ipv6_type_detect" // WAN连接类型检测的类型状态值
#define WAN_NAME_IPV6_NETWORK_STATUS    	"ipv6_network_status"
#define WAN_NAME_IPV6_CONN_TIME       		"ipv6_conn_time" // 暂时未写到配置模板中 给页面读取用
#define WAN_NAME_IPV6_UP_TIME       		"ipv6_up_time" // 私有配置
#define WAN_NAME_IPV6_PPPOE_TYPE			"ipv6_pppoe_type"
#define WAN_NAME_IPV6_PPPOE_STATICIP		"ipv6_pppoe_staticip"
#define WAN_NAME_IPV6_PPPOE_MANUAL			"ipv6_pppoe_manual"

#define WAN_NAME_IPV6_LLADDR_ENABLE     	"ipv6_lladdr_enable"

#define WAN_NAME_IPV6_PREFIX        		"ipv6_prefix"
#define WAN_NAME_IPV6_IPADDR        		"ipv6_ipaddr"
#define WAN_NAME_IPV6_PREFIX_LENGTH 		"ipv6_prefix_length"

#define WAN_NAME_IPV6_DHCPPD_ENABLE 		"ipv6_dhcppd_enable"
#define WAN_NAME_IPV6_DHCPPD       			"ipv6_dhcppd"
#define WAN_NAME_IPV6_DHCPPD_LENGTH 		"ipv6_dhcppd_length"

#define WAN_NAME_IPV6_GATEWAY				"ipv6_gateway"
#define WAN_NAME_IPV6_DNS_MANUAL			"ipv6_dns_manual"
#define WAN_NAME_IPV6_DNS					"ipv6_dns"
#define WAN_NAME_IPV6_DNS_ENABLE			"ipv6_dns_enable"
#define WAN_NAME_IPV6_DOMAIN				"ipv6_domain" //暂时未写到配置模板中

#define WAN_NAME_IPV6_6RD_OPTENABLE			"ipv6_6rd_optenable"
#define WAN_NAME_IPV6_6RD_IP4MASKLEN		"ipv6_6rd_ip4masklen"
#define WAN_NAME_IPV6_6RD_PEEFIXLEN			"ipv6_6rd_prefixlen"
#define WAN_NAME_IPV6_6RD_PREFIX			"ipv6_6rd_prefix"
#define WAN_NAME_IPV6_6RD_ROUTE				"ipv6_6rd_router"
#define WAN_NAME_IPV6_6RD_SPOKEENABLE		"ipv6_6rd_spokeenable"
#define WAN_NAME_IPV6_6RD_DLGTPREFIX 		"ipv6_6rd_dlgtprefix" //暂时未写到配置模板中 给页面读取用
#define WAN_NAME_IPV6_6RD_DLGTPREFIXLEN		"ipv6_6rd_dlgtprefix_len" //暂时未写到配置模板中 给页面读取用

#define WAN_NAME_IPV6_6TO4_ROUTE			"ipv6_6to4_router"
#define WAN_NAME_IPV6_6TO4_SLAID			"ipv6_6to4_slaid"
//#define WAN_NAME_IPV6_6TO4_ADDR 			"ipv6_6to4_addr" //暂时未写到配置模板中 给页面读取用


#define WAN_NAME_IPV6_6IN4_V4END            "ipv6_6in4_v4end"

#define WAN_NAME_IPV6_TUN_IFNAME			"ipv6_tun_ifname"
#define WAN_NAME_IPV6_TUN_TTL				"ipv6_tun_ttl"
#define WAN_NAME_IPV6_TUN_MTU				"ipv6_tun_mtu"
#define WAN_NAME_IPV6_DEFAULT_ROUTER_EXIST	"ipv6_default_router_exist"

#define LAN_NAME_IPV6_AUTOCONF_ENABLE		"lan0_ipv6_autoconf_enable"
#define LAN_NAME_IPV6_AUTOCONF_TYPE			"lan0_ipv6_autoconf_type"
#define LAN_NAME_IPV6_RADVD					"lan0_ipv6_radvd"
#define LAN_NAME_IPV6_DHCP_LIFETIME			"lan0_ipv6_dhcp_lifetime"
#define LAN_NAME_IPV6_DHCP_START			"lan0_ipv6_dhcp_start"
#define LAN_NAME_IPV6_DHCP_END				"lan0_ipv6_dhcp_end"
#define LAN_NAME_IPV6_PREFIX				"lan0_ipv6_prefix"
#define LAN_NAME_IPV6_PREFIX_LENGTH			"lan0_ipv6_prefix_length"

#define LAN_NAME_IPV6_ROUTER_LIFETIME		"lan0_ipv6_route_lifetime" // 调整为分钟
#define LAN_NAME_IPV6_DHCPPD_VLDTIME		"lan0_ipv6_dhcppd_vldtime"
#define LAN_NAME_IPV6_DHCPPD_PREFTIME		"lan0_ipv6_dhcppd_preftime"
#define LAN_NAME_IPV6_DHCPPD_REFER_TIME		"lan0_ipv6_dhcppd_refertime"

#define LAN_NAME_IPV6_DHCPPD_ENABLE			"lan0_ipv6_dhcppd_enable"
#define LAN_NAME_IPV6_ULA_ENABLE			"lan0_ipv6_ula_enable"
#define LAN_NAME_IPV6_ULA_DEFPREFIX_ENABLE	"lan0_ipv6_ula_defprefix_enable"
#define LAN_NAME_IPV6_ULA_DEFPREFIX			"lan0_ipv6_ula_defprefix"
#define LAN_NAME_IPV6_ULA_PREFIX			"lan0_ipv6_ula_prefix"
#define LAN_NAME_IPV6_ULA_PREFIX_LENGTH		"lan0_ipv6_ula_prefix_length"
#define	LAN_NAME_IPV6_IPADDR				"lan0_ipv6_ipaddr"
#define LAN_NAME_IPV6_DNS					"lan0_ipv6_dns"  // 暂时在配置模型和功能中未用到           

#endif

#define CONF_VALUE_AUTO									"Automatic"
#define CONF_VALUE_MANUAL								"Manual"

#define CONF_VALUE_TRUE									"true"
#define CONF_VALUE_FALSE								"false"

#define CONF_VALUE_ENABLE								"Enable"
#define CONF_VALUE_DISABLE								"Disable"

#define NETWORK_STATUS_CONNECTED						"Connected"
#define NETWORK_STATUS_DISCONNECTED						"Disconnected"
#define NETWORK_STATUS_CONNECTING						"Connecting"

#define CONF_VALUE_DYNAMIC								"Dynamic"
#define CONF_VALUE_STATIC								"Static"


#ifdef __CONFIG_IPV6__

#define	IPT_V4	0x01
#define	IPT_V6	0x02

enum {
	IPV6_DISABLED = 0,
	IPV6_AUTO_DETECTION,
	IPV6_MANUAL,
	IPV6_NATIVE_DHCP,
	IPV6_PPPOE,
	IPV6_6IN4,
	IPV6_6TO4,
	IPV6_6RD,
	IPV6_MAX_TYPE
};

enum {
	IPV6_LAN_STATELESS = 0,
	IPV6_LAN_STATEFUL,
	IPV6_LAN_SLAAC_STAELESSDHCP,
	IPV6_LAN_SLAAC_RDNSS,
};
#ifndef RTF_UP
/* Keep this in sync with /usr/src/linux/include/linux/route.h */
#define RTF_UP		0x0001  /* route usable			*/
#define RTF_GATEWAY     0x0002  /* destination is a gateway	*/
#define RTF_HOST	0x0004  /* host entry (net otherwise)	*/
#define RTF_REINSTATE   0x0008  /* reinstate route after tmout	*/
#define RTF_DYNAMIC     0x0010  /* created dyn. (by redirect)	*/
#define RTF_MODIFIED    0x0020  /* modified dyn. (by redirect)	*/
#endif

#define GIF_LINKLOCAL  0x0001  /* return link-local addr */
#define GIF_PREFIXLEN  0x0002  /* return prefix length */
#define GIF_PREFIX     0x0004  /* return prefix, not addr */


#define DEFAULT_LAN_PREFIX_LENGTH    		64
#define DEFAULT_LAN_DHCPPD_VLDTIME    		2592000
#define DEFAULT_LAN_DHCPPD_PREFTIME    		604800

#define DEFAULT_ULA_PREFIX  		"fd08:26b9:2481:1::"


// 需要通过ipv6_wan_getpathprefix先获取prefix，且调用者需要定义prefix和tmp
#define ipv6_nvname(name) (strcat_r(prefix, name, tmp))

extern int ipv6_wan_conntype2enum(char *wan6_proto);
extern int ipv6_wan_getconntype(void);
#ifdef SUPPORT_IPV6_AUTO_DETECTION	
extern int ipv6_wan_getdetectconntype(void);
#endif
extern int ipv6_wan_getpathprefix(char *fullprefix);

extern const char *ipv6_router_address(struct in6_addr *in6addr);
extern const char *ipv6_gateway_address(void);

extern const char *ipv6_wan_prefix(struct in6_addr *in6addr);
extern const char *ipv6_lan_prefix(struct in6_addr *in6addr);

extern int ipv6_mapaddr4(struct in6_addr *addr6, int ip6len, struct in_addr *addr4, int ip4mask);
extern int ipv6_lan_genulaprefix(char *ulaprefix);
extern int get_wan6_mac(unsigned char *mac);
extern int getIfIpv6addr(const char * pIfName, char * pIpv6AddrStr, int * pPrefixLen);

#endif
extern int get_lan_mac(unsigned char *mac);
extern int host_addr_info(const char *name, int af, char *addrbuf);
extern int ifconfig_get_status(char *name, int *flags, unsigned long *addr, unsigned long *netmask);


enum {
    CONN_TYPE_BRIDGE = 0,
    CONN_TYPE_STATIC,
    CONN_TYPE_DHCP,
    CONN_TYPE_PPPOE,
    CONN_TYPE_PPTP,
    CONN_TYPE_L2TP,
};

#if 0
extern int nvram_get_int(const char *key);
extern int nvram_set_int(const char *key, int value);
extern double nvram_get_double(const char *key);
extern int nvram_set_double(const char *key, double value);
#endif

// process.c
extern char *psname(int pid, char *buffer, int maxlen);
extern int pidof(const char *name);
extern int killall(const char *name, int sig);
extern int process_exists(pid_t pid);
extern int module_loaded(const char *module);

// files.c
extern int check_if_dir_empty(const char *dirpath);
extern int file_lock(char *tag);
extern void file_unlock(int lockfd);


#define FW_CREATE	0
#define FW_APPEND	1
#define FW_NEWLINE	2

#define	IFF_UP		0x1		/* interface is up		*/
#define	IFF_BROADCAST	0x2		/* broadcast address valid	*/
#define	IFF_RUNNING	0x40		/* interface RFC2863 OPER_UP	*/
#define IFF_MULTICAST	0x1000		/* Supports multicast		*/

#define IFUP (IFF_UP | IFF_RUNNING | IFF_BROADCAST | IFF_MULTICAST)

#define ACTION_LOCK_FILE "/var/lock/a_w_l" // action write lock

#define SYS_RESOLV_FILE_PATH  "/etc/resolv.conf"
#define SYS_HOSTS_FILE_PATH   "/etc/hosts"

#define REAL_RESOLV_FILE_PATH "/tmp/resolv.conf"
#define REAL_HOSTS_FILE_PATH "/tmp/hosts"

#define PPTP_L2TP_MON_SIG_NUM      (SIGRTMIN + 0)
#define WLAN_RESTART_TYPE			(SIGRTMIN + 1)
#define WLAN_START_TYPE				(SIGRTMIN + 2)
#define WLAN_STOP_TYPE				(SIGRTMIN + 3)
#ifdef CONFIG_AP_REPEATER_SUPPORT
#define APCLI_WPS_START				(SIGRTMIN + 4)
#endif
#define WPS_START					(SIGRTMIN + 5)


enum {
	WAN_CONN_0 = 0,
	WAN_CONN_1,
	WAN_CONN_2,
	WAN_CONN_3,
	WAN_MAX_CONN
};

enum {
	LAN_CONN_0 = 0,
	LAN_CONN_1,
	LAN_CONN_2,
	LAN_CONN_3,
	LAN_MAX_CONN
};

extern unsigned long f_size(const char *path);
extern int f_exists(const char *file);
extern int d_exists(const char *path);
extern int f_read_excl(const char *path, void *buffer, int max);
extern int f_read(const char *file, void *buffer, int max);												// returns bytes read
extern int f_write_excl(const char *path, const void *buffer, int len, unsigned flags, unsigned cmode);
extern int f_write(const char *file, const void *buffer, int len, unsigned flags, unsigned cmode);		//
extern int f_read_string(const char *file, char *buffer, int max);										// returns bytes read, not including term; max includes term
extern int f_write_string(const char *file, const char *buffer, unsigned flags, unsigned cmode);		//
extern int f_read_alloc(const char *path, char **buffer, int max);
extern int f_read_alloc_string(const char *path, char **buffer, int max);
extern int f_wait_exists(const char *name, int max);
extern int f_wait_notexists(const char *name, int max);

#endif	/* !__SHARED_H__ */
