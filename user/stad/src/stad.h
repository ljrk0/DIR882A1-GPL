/********************************************************************** 
Copyright (c), 2016-2023, T&W ELECTRONICS(SHENTHEN) Co., Ltd. 
文件名称: stad.h 
文件描述: stad头文件 
修订记录:        
	1. 作者: 陶橄         
	    日期: 2016-10-28           
	    内容: 创建文件       
**********************************************************************/

#ifndef __stad_h__
#define __stad_h__

#if defined(PRODUCT_DIR853_A1)||defined(PRODUCT_DIR853_A2) ||defined(PRODUCT_DIR1360)
#define INTERFACE_2G			"rax0"
#define INTERFACE_5G			"ra0"
#define APCLI_IF_2G 			"apclix0"
#define APCLI_IF_5G 			"apcli0"
#define INTERFACE_2G_GUEST1		"rax1"
#define	INTERFACE_5G_GUEST1		"ra1"
#else
#define INTERFACE_2G			"ra0"
#define INTERFACE_5G			"rai0"
#define APCLI_IF_2G 			"apcli0"
#define APCLI_IF_5G 			"apclii0"
#define INTERFACE_2G_GUEST1		"ra1"
#define	INTERFACE_5G_GUEST1		"rai1"
#endif

#define TW_USER_DEVICE_TYPE_24G 	1
#define TW_USER_DEVICE_TYPE_5G 		2
#define TW_USER_DEVICE_TYPE_24G_GUEST 	3
#define TW_USER_DEVICE_TYPE_5G_GUEST 	4

#define DHCP_LEASES_FILE "/tmp/dnsmasq.leases"
#define MACLIST_INFO_PATH "/tmp/mac.lists"
#define MACLIST_INFO_TMP_PATH "/tmp/mac.lists.tmp"
#define CLIENT_INFO_PATH "/tmp/client.lists"
#define CLIENT_INFO_TMP1_PATH "/tmp/client.lists.tmp1"
#define CLIENT_INFO_TMP_PATH "/tmp/client.lists.tmp"
#define IPV6_LEASES_FILE "/tmp/dhcp6s.lease"
#define NBNS_HOSTNAME_FILE "/tmp/nbns.hostinfo"
#define DHCP_MON_LIST_PATH "/tmp/dhcp_mon_list"
#define DHCP_MON_LIST_TMP_PATH "/tmp/dhcp_mon_list.tmp"
#define MACLIST_FILE_LOCK "maclist"
#define CLIENT_FILE_LOCK "clientlist"
#define NETLINK_STA_MSG 29
#define MAX_PAYLOAD 1024

int g_netlink_2g_fd = 0;
struct nlmsghdr *g_nlh = NULL;

#if 0
#define DEBUGP                                          printf
#else
#define DEBUGP(format, args...)
#endif

typedef struct sta_msg{
	unsigned char  type;
	unsigned char mac_addr[6];
	unsigned int access_time;
};

typedef struct client_info
{
	char MacAddress[18];
	char IPv4Address[16];
	char IPv6Address[32];
	char Type[16];
	char DeviceName[64];
	char NickName[64];
	char ReserveIP[16];
	char Band[16];
	char SignalLevel[16];
	char DeviceOS[16];
	char DeviceType[32];
	char DeviceFamily[32];
}ST_CLIENT_INFO;

typedef struct arp_info
{
	char IPAddress[17];
	char szHWtype[12];
	char szFlag[12];
	char szHWaddress[22];
	char szMask[9];
	char szDevice[16];
}ST_ARP_INFO;

#endif /* #ifndef __stad_h__ */

