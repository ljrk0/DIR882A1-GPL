#include "utils.h"
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/route.h>
#if defined (CONFIG_RT2860V2_STA_WSC)
#include <errno.h>
#include "wps.h"
#endif
#include <linux/wireless.h>
#include "oid.h"
#include "user_conf.h"
#include "busybox_conf.h"
#if defined CONFIG_MTK_VOIP
#include "openssl/pem.h"
#include "openssl/x509.h"
#include "openssl/evp.h"
#endif
#if defined CONFIG_USER_STORAGE
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#include "media.h"
#if defined (RT2860_STA_SUPPORT)
//#include <asm/types.h>
#include <errno.h>
#include "stapriv.h"
#endif

#define CMD_NVRAM_GET		0x1
#define CMD_BUILD_GET		0x2
#define CMD_WIFI_GET		0x3
#define CMD_SYS_GET		0x4
#define CMD_FIREWALL_GET	0x5
#define CMD_VOIP_GET		0x6
#define CMD_TR069_GET		0x7
#define CMD_AUTO_WAN_GET	0x8
#define CMD_MEDIA_GET		0x9
#define CMD_TEST_GET		0xa

#define TXBYTE		0
#define TXPACKET	1
#define RXBYTE		2
#define RXPACKET	3
#define PROTO_UNKNOWN	0
#define PROTO_TCP	1
#define PROTO_UDP	2
#define PROTO_TCP_UDP	3
#define PROTO_ICMP	4
#define PROTO_NONE	5
#define ACTION_DROP	0
#define ACTION_ACCEPT	1

#define WLAN1_CONF		"2860"
#define WLAN2_CONF		"rtdev"
#define PROC_MEM_STATISTIC	"/proc/meminfo"
#define PROC_IF_STATISTIC	"/proc/net/dev"

#define OID_802_11_WIFISPECTRUM_GET_CAPTURE_BW           0x0973
#define OID_802_11_WIFISPECTRUM_GET_CENTRAL_FREQ         0x0974

#define IPPORT_FILTER_CHAIN	"macipport_filter"
struct status media_state;
struct list all_list[1024];

char l7name[8192];			// export it for internet.c qos
					// (The actual string is about 7200 bytes.)

void getCentralFreq(char *argv[]);

void get_usage(char *aout)
{
	int i;

	DBG_MSG("Usage: ");
	for (i = 0; i < getNvramNum(); i++){
		DBG_MSG("\t%s %s nvram lan_ipaddr", aout, getNvramName(i));
	}
}

int ap_oid_query_info(unsigned long OidQueryCode, int socket_id, char *DeviceName, void *ptr, unsigned long PtrLength)
{
	struct iwreq wrq;

	strcpy(wrq.ifr_name, DeviceName);
	wrq.u.data.length = PtrLength;
	wrq.u.data.pointer = (caddr_t) ptr;
	wrq.u.data.flags = OidQueryCode;

	return (ioctl(socket_id, RT_PRIV_IOCTL, &wrq));
}

/* 
 * NVRAM Query
 */
void WebNvramGet(char *argv[])
{
	char *key, tmp[1024], *p, *block_name;
	const char *value;
	int nvram_id;

	key = argv[3];

	if ((!strcmp(argv[1], "voip")) || (!strcmp(argv[1], "tr069")) || (!strcmp(argv[1], "config2")) ){
#if defined CONFIG_CONFIG_SHRINK
		block_name = FB_2860_BLOCK_NAME;
#else
		block_name = FB_CONFIG2_BLOCK_NAME;
#endif
	}
	else{
		block_name = argv[1];
	}
	if ((nvram_id = getNvramIndex(block_name)) == -1) {
		DBG_MSG("%s: Error: \"%s\" flash zone not existed", argv[0], argv[1]);
		get_usage(argv[0]);
		return;
	}

	nvram_init(nvram_id);
/*
	if (!strcmp(argv[3], "Channel")) {
		value = nvram_bufget(nvram_id, "AutoChannelSelect");
		if (!strcmp(value, "0"))
			value = nvram_bufget(nvram_id, key);
	} else
*/
		value = nvram_bufget(nvram_id, key);
	memset(tmp, 0, 129);
	for (; *value != '\0'; value++) {
		switch(*value) {
		case '\\':
		case '\'':
			case '\"':
				sprintf(tmp, "%s\\", tmp);
		}
		sprintf(tmp, "%s%c", tmp, *value);
	}
	printf("%s", tmp);
	nvram_close(nvram_id);
}

void WebBuildGet(char *argv[])
{
	int build = 0;

#if defined CONFIG_RA_NAT_HW
	if (!strcmp(argv[3], "hnat"))
		build = 1;
#endif
#if defined CONFIG_RT2860V2_STA_DPB || defined CONFIG_RT2860V2_STA_ETH_CONVERT
	if (!strcmp(argv[3], "staDPB"))
		build = 1;
#endif
#if defined CONFIG_RTDEV_MII
	if (!strcmp(argv[3], "miiInic"))
		build = 1;
#endif
#if defined CONFIG_LAN_WAN_SUPPORT || defined CONFIG_MAC_TO_MAC_MODE || \
	(defined CONFIG_GE1_RGMII_AN && (defined GE2_RGMII_AN || defined GE2_INTERNAL_GPHY)) || \
	defined CONFIG_GE1_RGMII_FORCE_1000  || defined CONFIG_GE1_TRGMII_FORCE_1200 
	if (!strcmp(argv[3], "gateway"))
		build = 1;
#endif
//#if defined (CONFIG_RT2860V2_STA) || defined (CONFIG_RT2860V2_STA_MODULE)
//	if (!strcmp(argv[3], "sta"))
//		build = 1;
//#endif
#if defined CONFIG_USER_3G
	if (!strcmp(argv[3], "3G"))
		build = 1;
#endif
#if defined CONFIG_HOSTNAME
	if (!strcmp(argv[3], "hostName"))
		build = 1;
#endif
#if defined CONFIG_USER_LLTD
	if (!strcmp(argv[3], "lltd"))
		build = 1;
#endif
#if defined CONFIG_USER_UPNP_IGD || defined CONFIG_USER_MINIUPNPD
	if (!strcmp(argv[3], "upnp"))
		build = 1;
#endif
#if defined CONFIG_USER_RADVD
	if (!strcmp(argv[3], "radvd"))
		build = 1;
#endif
#if defined CONFIG_USER_RPPPPOE_RELAY
	if (!strcmp(argv[3], "pppoeRelay"))
		build = 1;
#endif
#if defined CONFIG_USER_IGMP_PROXY
	if (!strcmp(argv[3], "igmpProxy"))
		build = 1;
#endif
#if defined CONFIG_USER_DNSMASQ
	if (!strcmp(argv[3], "dnsMasq"))
		build = 1;
#endif
#if defined CONFIG_USER_ZEBRA
	if (!strcmp(argv[3], "zebra"))
		build = 1;
#endif
#if defined CONFIG_NF_CONNTRACK_PPTP || defined CONFIG_NF_CONNTRACK_PPTP_MODULE || \
    defined CONFIG_IP_NF_PPTP        || defined CONFIG_IP_NF_PPTP_MODULE
	if (!strcmp(argv[3], "vpnpassthr"))
		build = 1;
#endif
#if defined CONFIG_RALINK_RT3052 || defined CONFIG_RALINK_RT3352 || defined CONFIG_RALINK_RT5350 || defined CONFIG_RALINK_RT6855 || defined CONFIG_RALNK_MT7620 || defined CONFIG_RALINK_MT7621
	if (!strcmp(argv[3], "QoSisPortBasedQoSSupport"))
		build = 1;
#endif

#if defined CONFIG_RALINKAPP_SWQOS || defined CONFIG_RALINKAPP_HWQOS
	if (!strcmp(argv[3], "qos"))
		build = 1;
#endif

#if defined CONFIG_IPV6
	if (!strcmp(argv[3], "ipv6"))
		build = 1;
#endif
#if defined CONFIG_IPV6_SIT_6RD
	if (!strcmp(argv[3], "ipv6rd"))
		build = 1;
#endif
#if defined CONFIG_IPV6_TUNNEL
	if (!strcmp(argv[3], "ipv6dslite"))
		build = 1;
#endif
#if defined (RT2860_MBSS_SUPPORT)
	if (!strcmp(argv[3], "mBssid") && !strcmp(argv[1], WLAN1_CONF))
		build = 1;
#if defined (RT2860_NEW_MBSS_SUPPORT) || defined (RT2860_16BSSID_SUPPORT)
	if (!strcmp(argv[3], "mBssid16") && !strcmp(argv[1], WLAN1_CONF))
		build = 1;
#endif
#endif
#if defined (RTDEV_MBSS_SUPPORT)
	if (!strcmp(argv[3], "mBssid") && !strcmp(argv[1], WLAN2_CONF))
		build = 1;
#if defined (RTDEV_NEW_MBSS_SUPPORT) || defined (RTDEV_16BSSID_SUPPORT)
	if (!strcmp(argv[3], "mBssid16") && !strcmp(argv[1], WLAN2_CONF))
		build = 1;
#endif
#endif
#if defined (RT2860_APCLI_SUPPORT)
	if (!strcmp(argv[3], "apClient") && !strcmp(argv[1], WLAN1_CONF))
		build = 1;
#endif
#if defined (RTDEV_APCLI_SUPPORT)
	if (!strcmp(argv[3], "apClient") && !strcmp(argv[1], WLAN2_CONF))
		build = 1;
#endif
#if defined (RT2860_11NDRAFT3_SUPPORT)
	if (!strcmp(argv[3], "11nDraft3") && !strcmp(argv[1], WLAN1_CONF))
		build = 1;
#endif
#if defined (RTDEV_11NDRAFT3_SUPPORT)
	if (!strcmp(argv[3], "11nDraft3") && !strcmp(argv[1], WLAN2_CONF))
		build = 1;
#endif
#if defined (RT2860_DLS_SUPPORT)
	if (!strcmp(argv[3], "dls") && !strcmp(argv[1], WLAN1_CONF))
		build = 1;
#endif
#if defined (RTDEV_DLS_SUPPORT)
	if (!strcmp(argv[3], "dls") && !strcmp(argv[1], WLAN2_CONF))
		build = 1;
#endif
#if defined (RT2860_IGMPSNOOP_SUPPORT)
	if (!strcmp(argv[3], "igmpSnooping") && !strcmp(argv[1], WLAN1_CONF))
		build = 1;
#endif
#if defined (RTDEV_IGMPSNOOP_SUPPORT)
	if (!strcmp(argv[3], "igmpSnooping") && !strcmp(argv[1], WLAN2_CONF))
		build = 1;
#endif
#if defined (RT2860_WDS_SUPPORT)
	if (!strcmp(argv[3], "wds") && !strcmp(argv[1], WLAN1_CONF))
		build = 1;
#endif
#if defined (RTDEV_WDS_SUPPORT)
	if (!strcmp(argv[3], "wds") && !strcmp(argv[1], WLAN2_CONF))
		build = 1;
#endif
#if defined (RT2860_WSC_SUPPORT)
	if (!strcmp(argv[3], "wsc") && !strcmp(argv[1], WLAN1_CONF))
		build = 1;
#endif
#if defined (RTDEV_WSC_SUPPORT)
	if (!strcmp(argv[3], "wsc") && !strcmp(argv[1], WLAN2_CONF))
		build = 1;
#endif
#if defined (RT2860_ABAND_SUPPORT)
	if (!strcmp(argv[3], "aBand") && !strcmp(argv[1], WLAN1_CONF))
		build = 1;
#endif
#if defined (RTDEV_ABAND_SUPPORT)
	if (!strcmp(argv[3], "aBand") && !strcmp(argv[1], WLAN2_CONF))
		build = 1;
#endif
#if defined (RT2860_GBAND_SUPPORT)
	if (!strcmp(argv[3], "gBand") && !strcmp(argv[1], WLAN1_CONF))
		build = 1;
#endif
#if defined (RTDEV_GBAND_SUPPORT)
	if (!strcmp(argv[3], "gBand") && !strcmp(argv[1], WLAN2_CONF))
		build = 1;
#endif
#if defined (RT2860_AC_SUPPORT)
	if (!strcmp(argv[3], "vht") && !strcmp(argv[1], WLAN1_CONF))
		build = 1;
#endif
#if defined (RTDEV_AC_SUPPORT)
	if (!strcmp(argv[3], "vht") && !strcmp(argv[1], WLAN2_CONF))
		build = 1;
#endif
#if defined (RT2860_WAPI_SUPPORT)
	if (!strcmp(argv[3], "wapi") && !strcmp(argv[1], WLAN1_CONF))
		build = 1;
#endif
#if defined (RTDEV_WAPI_SUPPORT)
	if (!strcmp(argv[3], "wapi") && !strcmp(argv[1], WLAN2_CONF))
		build = 1;
#endif
#if defined (RT2860_DFS_SUPPORT)
	if (!strcmp(argv[3], "dfs") && !strcmp(argv[1], WLAN1_CONF))
		build = 1;
#endif
#if defined (RTDEV_DFS_SUPPORT)
	if (!strcmp(argv[3], "dfs") && !strcmp(argv[1], WLAN2_CONF))
		build = 1;
#endif
#if defined (RT2860_CARRIER_SUPPORT)
	if (!strcmp(argv[3], "carrier") && !strcmp(argv[1], WLAN1_CONF))
		build = 1;
#endif
#if defined (RTDEV_CARRIER_SUPPORT)
	if (!strcmp(argv[3], "carrier") && !strcmp(argv[1], WLAN2_CONF))
		build = 1;
#endif
#if defined (CONFIG_EXT_CHANNEL_LIST)
	if (!strcmp(argv[3], "extchlist") && !strcmp(argv[1], WLAN1_CONF))
		build = 1;
#endif
#if defined (RT2860_HS_SUPPORT)
	if (!strcmp(argv[3], "hotspot") && !strcmp(argv[1], WLAN1_CONF))
		build = 1;
#endif
#if defined (RTDEV_HS_SUPPORT)
	if (!strcmp(argv[3], "hotspot") && !strcmp(argv[1], WLAN2_CONF))
		build = 1;
#endif
#if defined CONFIG_RA_CLASSIFIER_MODULE && \
    defined CONFIG_RT2860V2_AP_VIDEO_TURBINE && \
    defined (RT2860_TXBF_SUPPORT)
	if (!strcmp(argv[3], "rvt") && !strcmp(argv[1], WLAN1_CONF))
		build = 1;
#endif
#if defined (RT2860_TXBF_SUPPORT)
	if (!strcmp(argv[3], "txbf") && !strcmp(argv[1], WLAN1_CONF))
		build = 1;
#endif
#if defined (RTDEV_TXBF_SUPPORT)
	if (!strcmp(argv[3], "txbf") && !strcmp(argv[1], WLAN2_CONF))
		build = 1;
#endif
#if defined CONFIG_RT2860V2_AUTO_PROVISION
	if (!strcmp(argv[3], "autoProvision"))
		build = 1;
#endif
#if defined (RT2860_MESH_SUPPORT)
	if (!strcmp(argv[3], "mesh") && !strcmp(argv[1], WLAN1_CONF))
		build = 1;
#endif
#if defined (RTDEV_MESH_SUPPORT)
	if (!strcmp(argv[3], "mesh") && !strcmp(argv[1], WLAN2_CONF))
		build = 1;
#endif
#if defined (RT2860_DOT11W_PMF_SUPPORT)
	if (!strcmp(argv[3], "pmf") && !strcmp(argv[1], WLAN1_CONF))
		build = 1;
#endif
#if defined (RTDEV_DOT11W_PMF_SUPPORT)
	if (!strcmp(argv[3], "pmf") && !strcmp(argv[1], WLAN2_CONF))
		build = 1;
#endif

#if defined CONFIG_IP_NF_FILTER
	if (!strcmp(argv[3], "pktFilter"))
		build = 1;
#endif
#if defined CONFIG_USER_STORAGE && (defined CONFIG_USB || defined CONFIG_MMC)
	if (!strcmp(argv[3], "nas"))
		build = 1;
#endif
#if defined CONFIG_USER_PROFTPD
	if (!strcmp(argv[3], "ftp"))
		build = 1;
#endif
#if defined CONFIG_USER_SAMBA
	if (!strcmp(argv[3], "samba"))
		build = 1;
#endif
#if defined CONFIG_USER_GREENAP && defined CONFIG_CROND && defined CONFIG_CRONTAB
	if (!strcmp(argv[3], "greenAP"))
		build = 1;
#endif
#if defined CONFIG_DATE
	if (!strcmp(argv[3], "cmd_date"))
		build = 1;
#endif
#if defined CONFIG_USER_INADYN
	if (!strcmp(argv[3], "ddns"))
		build = 1;
#endif
#if (defined CONFIG_RALINK_WATCHDOG || defined CONFIG_RALINK_WATCHDOG_MODULE) && defined CONFIG_USER_WATCHDOG
	if (!strcmp(argv[3], "wdg"))
		build = 1;
#endif
#if defined CONFIG_LOGREAD && defined CONFIG_KLOGD
	if (!strcmp(argv[3], "ethtool"))
		build = 1;
#endif

#if defined CONFIG_RALINKAPP_SUPERDMZ
	if (!strcmp(argv[3], "superDMZ"))
		build = 1;
#endif
#if defined CONFIG_USB && defined CONFIG_USER_UVC_STREAM
	if (!strcmp(argv[3], "webcam"))
		build = 1;
#endif
#if defined CONFIG_USB && defined CONFIG_USER_P910ND
	if (!strcmp(argv[3], "printersrv"))
		build = 1;
#endif
#if defined CONFIG_USB && defined CONFIG_USER_MTDAAPD
	if (!strcmp(argv[3], "itunes"))
		build = 1;
#endif
#if defined CONFIG_USER_STORAGE && defined CONFIG_USER_MINIDLNA
	if (!strcmp(argv[3], "media"))
		build = 1;
#endif
#if defined CONFIG_MTK_VOIP
  if (!strcmp(argv[3], "voip"))
		build = 1;
#endif

#if defined CONFIG_RALINKAPP_AUTO_WAN
  if (!strcmp(argv[3], "auto_wan"))
		build = 1;
#endif


#if defined (CONFIG_RALINK_MT7628)
  if (!strcmp(argv[3], "one_button_through_wall"))
		build = 1;
#endif

#if defined (RT2860_STA_SUPPORT)
	if (!strcmp(argv[3], "sta"))
		build = 1;       

//#if defined (RT2860_STA_TXBF_SUPPORT)
#if defined (RT2860_STA_SUPPORT) && defined (RT2860_TXBF_SUPPORT)
	if (!strcmp(argv[3], "statxbf"))
		build = 1; 
#endif

#if defined (CONFIG_RT2860V2_STA_WSC)
	if (!strcmp(argv[3], "sta_wsc"))
		build = 1;
#endif
#endif

#if defined (CONFIG_RT2860V2_RT3XXX_ANTENNA_DIVERSITY)
	if (!strcmp(argv[3], "AntennaDiversity"))
		build = 1; 
#endif

#if defined (WPA_SUPPLICANT_SUPPORT)
	if (!strcmp(argv[3], "wpa_supplicant"))
		build = 1;       
#endif
#if defined (DBDC_MODE) 
	if (!strcmp(argv[3], "dbdc"))
		build = 1;       
#endif
#if defined (MT_MAC) 
	if (!strcmp(argv[3], "mt_mac"))
		build = 1;       
#endif
	//DBG_MSG("[DBG]%s=%d", argv[3], build);

	printf("%d", build);
}

#if defined (RT2860_STA_SUPPORT)

#define TOKEN	"\t"
#define FALSE	(0==1)
#define TRUE	(1==1)

/*
 * Global Parameters
 */
unsigned int    m_nSigQua[3] = {0,0,0};
unsigned long   m_lTxCount = 0;
unsigned long   m_lRxCount = 0;
unsigned long   m_lChannelQuality = 0;

PAIR_CHANNEL_FREQ_ENTRY ChannelFreqTable[] = {
	//channel Frequency
	{1,     2412000},
	{2,     2417000},
	{3,     2422000},
	{4,     2427000},
	{5,     2432000},
	{6,     2437000},
	{7,     2442000},
	{8,     2447000},
	{9,     2452000},
	{10,    2457000},
	{11,    2462000},
	{12,    2467000},
	{13,    2472000},
	{14,    2484000},
	{34,    5170000},
	{36,    5180000},
	{38,    5190000},
	{40,    5200000},
	{42,    5210000},
	{44,    5220000},
	{46,    5230000},
	{48,    5240000},
	{52,    5260000},
	{56,    5280000},
	{60,    5300000},
	{64,    5320000},
	{100,   5500000},
	{104,   5520000},
	{108,   5540000},
	{112,   5560000},
	{116,   5580000},
	{120,   5600000},
	{124,   5620000},
	{128,   5640000},
	{132,   5660000},
	{136,   5680000},
	{140,   5700000},
	{149,   5745000},
	{153,   5765000},
	{157,   5785000},
	{161,   5805000},
	{165,	5825000},
	{167,	5835000},
	{169,	5845000},
	{171,	5855000},
	{173,	5865000},
	{184,	4920000},
	{188,	4940000},
	{192,	4960000},
	{196,	4980000},
	{208,	5040000},	/* Japan, means J08 */
	{212,	5060000},	/* Japan, means J12 */
	{216,	5080000},	/* Japan, means J16 */
};
int G_nChanFreqCount = sizeof (ChannelFreqTable) / sizeof(PAIR_CHANNEL_FREQ_ENTRY);

PRT_PROFILE_SETTING selectedProfileSetting = NULL, headerProfileSetting = NULL, currentProfileSetting = NULL;

char		G_bRadio = 1; //TRUE
//char		G_bdBm_ischeck = 0; //false
char		G_staProfileNum = 0;
NDIS_802_11_SSID G_SSID;
//unsigned char	G_Bssid[6];
int		G_ConnectStatus = NdisMediaStateDisconnected;
//unsigned char 	WpaSupplicant_flag = FALSE;


/*
 * Function Start
 */
static int myGetSuppAMode(void)
{
	unsigned long lBufLen = sizeof(NDIS_802_11_NETWORK_TYPE_LIST) + sizeof(NDIS_802_11_NETWORK_TYPE)*3 ;
	PNDIS_802_11_NETWORK_TYPE_LIST pNetworkTypeList = (PNDIS_802_11_NETWORK_TYPE_LIST) malloc(lBufLen);
	int i, s, G_bSupportAMode=0;

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (OidQueryInformation(OID_802_11_NETWORK_TYPES_SUPPORTED, s, "ra0", pNetworkTypeList, lBufLen) >= 0)
	{
		for (i = 0 ; i < pNetworkTypeList->NumberOfItems ; i++)
		{
			if ( pNetworkTypeList->NetworkType[i] == Ndis802_11OFDM5 )
			{
				G_bSupportAMode = 1;
				break;
			}
		}
	}
	free(pNetworkTypeList);
	close(s);
	return G_bSupportAMode;
}        

static int GetStaRadioStatus(char *argv[])
{
	unsigned long RadioStatus=0;
	int s, ret;

	s = socket(AF_INET, SOCK_DGRAM, 0);
	ret = OidQueryInformation(RT_OID_802_11_RADIO, s, "ra0", &RadioStatus, sizeof(RadioStatus));
	if (ret < 0)
		fprintf(stderr, "%s: Query RT_OID_802_11_RADIO failed = %d \n", __func__, ret);
		//error(E_L, E_LOG, T("getStaRadioStatus: Query RT_OID_802_11_RADIO failed!"));
	close(s);
	if (RadioStatus == 1)
		printf("%s", "1");
	else
		printf("%s", "0");
	return 0;
}

static int GetLinkingMode(char *argv[])
{
	int s = socket(AF_INET, SOCK_DGRAM, 0);
	HTTRANSMIT_SETTING HTSetting;

	memset(&HTSetting, 0x00, sizeof(HTTRANSMIT_SETTING));
	OidQueryInformation(RT_OID_802_11_QUERY_LAST_TX_RATE, s, "ra0", &HTSetting, sizeof(HTTRANSMIT_SETTING));
	close(s);
	if(HTSetting.field.MODE > 1) {		// 0: CCK, 1:OFDM, 2:Mixedmode, 3:GreenField
		printf("%s", "1");
	} else {
		printf("%s", "0");
	}
}

static int getStaLinkStatus()
{
	int s, ret;
	s = socket(AF_INET, SOCK_DGRAM, 0);

	ret = OidQueryInformation(OID_GEN_MEDIA_CONNECT_STATUS, s, "ra0", &G_ConnectStatus, sizeof(G_ConnectStatus));
	if (ret < 0 ) {
		close(s);
		printf("Disconnected");
		return NdisMediaStateDisconnected;
		//return websWrite(wp, "Disconnected");
	}

	return G_ConnectStatus;
}

/*
 * description: write station link status
 */
static int GetStaLinkStatus(char *argv[])
{
	int s, ret;
	s = socket(AF_INET, SOCK_DGRAM, 0);

	ret = OidQueryInformation(OID_GEN_MEDIA_CONNECT_STATUS, s, "ra0", &G_ConnectStatus, sizeof(G_ConnectStatus));
	if (ret < 0 ) {
		close(s);
		printf("Disconnected");
		return 1;
		//return websWrite(wp, "Disconnected");
	}

	if (G_ConnectStatus == NdisMediaStateConnected) {
		NDIS_802_11_SSID SSID;
		unsigned char Bssid[6];

		memset(&SSID, 0x00, sizeof(NDIS_802_11_SSID));
		OidQueryInformation(OID_802_11_SSID, s, "ra0", &SSID, sizeof(NDIS_802_11_SSID));

		memset(&Bssid, 0x00, sizeof(Bssid));
		OidQueryInformation(OID_802_11_BSSID, s, "ra0", Bssid, 6);

		SSID.Ssid[SSID.SsidLength] = 0;
		//websWrite(wp, "%s <--> %02X-%02X-%02X-%02X-%02X-%02X", SSID.Ssid,
				//Bssid[0], Bssid[1], Bssid[2], Bssid[3], Bssid[4], Bssid[5]);
		printf("%s <--> %02X-%02X-%02X-%02X-%02X-%02X", SSID.Ssid,
				Bssid[0], Bssid[1], Bssid[2], Bssid[3], Bssid[4], Bssid[5]);
	} else {
		if (OidQueryInformation(RT_OID_802_11_RADIO, s, "ra0", &G_bRadio, sizeof(G_bRadio)) < 0) {
			//websWrite(wp, "error!");
			printf("error!");
			close(s);
			return 0;
		}
		if (G_bRadio) {
			RT_802_11_STA_CONFIG configSta;
			OidQueryInformation(RT_OID_802_11_STA_CONFIG, s, "ra0", &configSta, sizeof(RT_802_11_STA_CONFIG));
			if (configSta.HwRadioStatus == 0){ // Hardware radio off
				//websWrite(wp, "RF Off");
				printf("RF Off");
			}else{
				printf("Disconnected");
				//websWrite(wp, "Disconnected");
			}
		}
		else{
			printf("RF Off");
			//websWrite(wp, "RF Off");
		}
	}
	close(s);
	return 0;
}

/*
 * description: write station extra info
 */
static int GetStaExtraInfo(char *argv[])
{
	unsigned long lExtraInfo;
	int s, ret;
	s = socket(AF_INET, SOCK_DGRAM, 0);

	char *ExtraInfoTable[11] = {
		"Link is Up",
		"Link is Down",
		"Hardware radio off",
		"Software radio off",
		"Open authentication fail",
		"Shared authentication fail",
		"Association failed",
		"Deauthencation because MIC failure",
		"Deauthencation on 4-way handshake timeout",
		"Deauthencation on group key handshake timeout",
		"EAP successd"
	};

	ret = OidQueryInformation(RT_OID_802_11_EXTRA_INFO, s, "ra0", &lExtraInfo, 4);
	if (ret < 0 ){
		printf("&nbsp;");
		//websWrite(wp, "&nbsp;");
	}
	else {
		if (lExtraInfo <= 0xa) {
			printf("%s", ExtraInfoTable[lExtraInfo]);
			//websWrite(wp, "%s", ExtraInfoTable[lExtraInfo]);
		}
		else{
			printf("&nbsp;");
			//websWrite(wp, "&nbsp;");
		}
	}
	close(s);
	return 0;
}

static int GetStaLinkChannel(char *argv[])
{
	NDIS_802_11_CONFIGURATION Configuration;
	RT_802_11_LINK_STATUS     LinkStatus;
	HTTRANSMIT_SETTING HTSetting;
	int s, i;
	int nChannel = -1;
	int Japan_channel = 200;

	G_ConnectStatus = getStaLinkStatus();
	if (G_ConnectStatus == NdisMediaStateDisconnected) {
		return printf("&nbsp;");
		//return websWrite(wp, "&nbsp;");

	}

	s = socket(AF_INET, SOCK_DGRAM, 0);

	// Current Channel
	OidQueryInformation(OID_802_11_CONFIGURATION, s, "ra0", &Configuration, sizeof(NDIS_802_11_CONFIGURATION));
	for (i = 0; i < G_nChanFreqCount; i++) {
		if (Configuration.DSConfig == ChannelFreqTable[i].lFreq) {
			nChannel = ChannelFreqTable[i].lChannel;
			break;
		}
	}

	OidQueryInformation(RT_OID_802_11_QUERY_LINK_STATUS, s, "ra0", &LinkStatus, sizeof(&LinkStatus));

	memset(&HTSetting, 0x00, sizeof(HTTRANSMIT_SETTING));
	OidQueryInformation(RT_OID_802_11_QUERY_LAST_TX_RATE, s, "ra0", &HTSetting, sizeof(HTTRANSMIT_SETTING));
	close(s);

	if (nChannel == -1) {
		//websWrite(wp, "error!");
		printf("error!");
	} else if(HTSetting.field.MODE > 1) {		// 0: CCK, 1:OFDM, 2:Mixedmode, 3:GreenField
		if (nChannel == (Japan_channel + 8)){
			printf("J8 <--> %ld KHz ; Central Channel: %ld", Configuration.DSConfig, LinkStatus.CentralChannel);
			//websWrite(wp, "J8 <--> %ld KHz ; Central Channel: %ld", Configuration.DSConfig, LinkStatus.CentralChannel);
		}
		else if (nChannel == (Japan_channel + 12)){
			printf("J12 <--> %ld KHz ; Central Channel: %ld", Configuration.DSConfig, LinkStatus.CentralChannel);
			//websWrite(wp, "J12 <--> %ld KHz ; Central Channel: %ld", Configuration.DSConfig, LinkStatus.CentralChannel);
		}
		else if (nChannel == (Japan_channel + 16)){
			printf("J16 <--> %ld KHz ; Central Channel: %ld", Configuration.DSConfig, LinkStatus.CentralChannel);
			//websWrite(wp, "J16 <--> %ld KHz ; Central Channel: %ld", Configuration.DSConfig, LinkStatus.CentralChannel);
		}
		else{
			printf("%u <--> %ld KHz ; Central Channel: %ld", nChannel, Configuration.DSConfig, LinkStatus.CentralChannel);
			//websWrite(wp, "%u <--> %ld KHz ; Central Channel: %ld", nChannel, Configuration.DSConfig, LinkStatus.CentralChannel);
		}
	} else {
		if (nChannel == (Japan_channel + 8)){
			printf("J8 <--> %ld KHz", Configuration.DSConfig);
			//websWrite(wp, "J8 <--> %ld KHz", Configuration.DSConfig);
		}
		else if (nChannel == (Japan_channel + 12)){
			printf("J12 <--> %ld KHz", Configuration.DSConfig);
			//websWrite(wp, "J12 <--> %ld KHz", Configuration.DSConfig);
		}
		else if (nChannel == (Japan_channel + 16)){
			printf("J16 <--> %ld KHz", Configuration.DSConfig);
			//websWrite(wp, "J16 <--> %ld KHz", Configuration.DSConfig);
		}else{
			printf("%u <--> %ld KHz", nChannel, Configuration.DSConfig);
			//websWrite(wp, "%u <--> %ld KHz", nChannel, Configuration.DSConfig);
		}
	}

	return 0;
}

static char bGetHTTxRateByBW_GI_MCS(int nBW, int nGI, int nMCS, double* dRate)
{
	//fprintf(stderr, "bGetHTTxRateByBW_GI_MCS()\n");
	double HTTxRate20_800[24]={6.5, 13.0, 19.5, 26.0, 39.0, 52.0, 58.5, 65.0, 13.0, 26.0, 39.0, 52.0, 78.0, 104.0, 117.0, 130.0,
								19.5, 39.0, 58.5, 78.0, 117.0, 156.0, 175.5, 195.0};
	double HTTxRate20_400[24]={7.2, 14.4, 21.7, 28.9, 43.3, 57.8, 65.0, 72.2, 14.444, 28.889, 43.333, 57.778, 86.667, 115.556, 130.000, 144.444,
								21.7, 43.3, 65.0, 86.7, 130.0, 173.3, 195.0, 216.7};
	double HTTxRate40_800[25]={13.5, 27.0, 40.5, 54.0, 81.0, 108.0, 121.5, 135.0, 27.0, 54.0, 81.0, 108.0, 162.0, 216.0, 243.0, 270.0,
								40.5, 81.0, 121.5, 162.0, 243.0, 324.0, 364.5, 405.0, 6.0};
	double HTTxRate40_400[25]={15.0, 30.0, 45.0, 60.0, 90.0, 120.0, 135.0, 150.0, 30.0, 60.0, 90.0, 120.0, 180.0, 240.0, 270.0, 300.0,
								45.0, 90.0, 135.0, 180.0, 270.0, 360.0, 405.0, 450.0, 6.7};

	// no TxRate for (BW = 20, GI = 400, MCS = 32) & (BW = 20, GI = 400, MCS = 32)
	if (((nBW == BW_20) && (nGI == GI_400) && (nMCS == 32)) ||
			((nBW == BW_20) && (nGI == GI_800) && (nMCS == 32)))
	{
		return 0;
	}

	if (nMCS == 32)
		nMCS = 25;

	if (nBW == BW_20 && nGI == GI_800)
		*dRate = HTTxRate20_800[nMCS];
	else if (nBW == BW_20 && nGI == GI_400)
		*dRate = HTTxRate20_400[nMCS];
	else if (nBW == BW_40 && nGI == GI_800)
		*dRate = HTTxRate40_800[nMCS];
	else if (nBW == BW_40 && nGI == GI_400)
		*dRate = HTTxRate40_400[nMCS];
	else
		return 0; //false

	//fprintf(stderr, "dRate=%.1f\n", *dRate);
	return 1; //true
}

static void DisplayLastTxRxRateFor11n(int s, int nID, double* fLastTxRxRate)
{
	unsigned long lHTSetting;
	HTTRANSMIT_SETTING HTSetting;
	double b_mode[] ={1, 2, 5.5, 11};
	float g_Rate[] = { 6,9,12,18,24,36,48,54};

	OidQueryInformation(nID, s, "ra0", &lHTSetting, sizeof(lHTSetting));

	memset(&HTSetting, 0x00, sizeof(HTSetting));
	memcpy(&HTSetting, &lHTSetting, sizeof(HTSetting));

	switch(HTSetting.field.MODE)
	{
		case 0:
			if (HTSetting.field.MCS >= 0 && HTSetting.field.MCS <= 3)
				*fLastTxRxRate = b_mode[HTSetting.field.MCS];
			else if (HTSetting.field.MCS >=8 && HTSetting.field.MCS<=11)
				*fLastTxRxRate = b_mode[HTSetting.field.MCS-8];
			else
				*fLastTxRxRate = 0;

			break;
		case 1:
			if ((HTSetting.field.MCS >= 0) && (HTSetting.field.MCS < 8))
				*fLastTxRxRate = g_Rate[HTSetting.field.MCS];
			else
				*fLastTxRxRate = 0;

			break;
		case 2:
		case 3:
			if (0 == bGetHTTxRateByBW_GI_MCS(HTSetting.field.BW,
						HTSetting.field.ShortGI,
						HTSetting.field.MCS,
						fLastTxRxRate))
			{
				*fLastTxRxRate = 0;
			}
			break;
		default:
			*fLastTxRxRate = 0;
			break;
	}
}

/*
 * description: write station link Rx rate
 */
static int GetStaLinkTxRate(char *argv[])
{
	int s;
	char tmp[8];

	G_ConnectStatus = getStaLinkStatus();
	if (G_ConnectStatus == NdisMediaStateDisconnected) {
		//return websWrite(wp, "0");
		return printf("0");
	}
	s = socket(AF_INET, SOCK_DGRAM, 0);
	double fLastTxRate = 1;
	DisplayLastTxRxRateFor11n(s, RT_OID_802_11_QUERY_LAST_TX_RATE, &fLastTxRate);
	snprintf(tmp, 8, "%.1f", fLastTxRate);
	printf("%s", tmp);
	//websWrite(wp, "%s", tmp);

	close(s);
	return 0;
}

/*
 * description: write station link Rx rate
 */
static int GetStaLinkRxRate(char *argv[])
{
	int s;
	char tmp[8];


	G_ConnectStatus = getStaLinkStatus();
	if (G_ConnectStatus == NdisMediaStateDisconnected) {
		return printf("0");
		//return websWrite(wp, "0");
	}
	
	s = socket(AF_INET, SOCK_DGRAM, 0);
	double fLastRxRate = 1;
	DisplayLastTxRxRateFor11n(s, RT_OID_802_11_QUERY_LAST_RX_RATE, &fLastRxRate);
	snprintf(tmp, 8, "%.1f", fLastRxRate);
	printf("%s", tmp);
	//websWrite(wp, "%s", tmp);

	close(s);
	return 0;
}

/*
 * description: write station link Tx throughput
 */
static int GetStaTxThroughput(char *argv[])
{
	RT_802_11_LINK_STATUS LinkStatus;
	int s;
	char tmp[8];

	G_ConnectStatus = getStaLinkStatus();
	if (G_ConnectStatus == NdisMediaStateDisconnected) {
		return printf("0");
		//return websWrite(wp, "0");
	}

	s = socket(AF_INET, SOCK_DGRAM, 0);
	// Get Link Status Info from driver
	OidQueryInformation(RT_OID_802_11_QUERY_LINK_STATUS, s, "ra0", &LinkStatus, sizeof(RT_802_11_LINK_STATUS));

	// Tx Throughput (KBits/sec) (LinkStatus.TxByteCount - m_lTxCount) * 8(bits) /1000 / 2(secs)
	if (m_lTxCount != 0)
		snprintf(tmp, 8, "%.1f", (double)(LinkStatus.TxByteCount - m_lTxCount) / 250);
	else
		snprintf(tmp, 8, "%.1f", (double)0);
  printf("%s", tmp);
	//websWrite(wp, "%s", tmp);
	m_lTxCount = LinkStatus.TxByteCount;
	close(s);
	return 0;
}

/*
 * description: write station link Rx throughput
 */
static int GetStaRxThroughput(char *argv[])
{
	RT_802_11_LINK_STATUS LinkStatus;
	int s;
	char tmp[8];

	G_ConnectStatus = getStaLinkStatus();
	if (G_ConnectStatus == NdisMediaStateDisconnected) {
		//return websWrite(wp, "0");
		return printf("0");
	}

	s = socket(AF_INET, SOCK_DGRAM, 0);
	// Get Link Status Info from driver
	OidQueryInformation(RT_OID_802_11_QUERY_LINK_STATUS, s, "ra0", &LinkStatus, sizeof(RT_802_11_LINK_STATUS));

	// Rx Throughput (KBits/sec) (LinkStatus.RxByteCount - m_lRxCount) * 8(bits) /1000 / 2(secs)
	if (m_lRxCount != 0)
		snprintf(tmp, 8, "%.1f", (double)(LinkStatus.RxByteCount - m_lRxCount) / 250);
	else
		snprintf(tmp, 8, "%.1f", (double)0);
	printf("%s", tmp);
	//websWrite(wp, "%s", tmp);
	m_lRxCount = LinkStatus.RxByteCount;
	close(s);
	return 0;
}

/*
 * description: write station link quality
 */
static int GetStaLinkQuality(char *argv[])
{
	RT_802_11_LINK_STATUS LinkStatus;
	int s;

	G_ConnectStatus = getStaLinkStatus();
	if (G_ConnectStatus == NdisMediaStateDisconnected) {
		printf("0%%");
		return 1;
		//return websWrite(wp, "0%%");
	}

	s = socket(AF_INET, SOCK_DGRAM, 0);
	// Get Link Status Info from driver
	OidQueryInformation(RT_OID_802_11_QUERY_LINK_STATUS, s, "ra0", &LinkStatus, sizeof(RT_802_11_LINK_STATUS));

	LinkStatus.ChannelQuality = (unsigned long)(LinkStatus.ChannelQuality * 1.2 + 10);
	if (LinkStatus.ChannelQuality > 100)
		LinkStatus.ChannelQuality = 100;
	if (m_lChannelQuality != 0)
		LinkStatus.ChannelQuality = (unsigned long)((m_lChannelQuality + LinkStatus.ChannelQuality) / 2.0 + 0.5);

	m_lChannelQuality = LinkStatus.ChannelQuality;
	close(s);

	if (LinkStatus.ChannelQuality > 70) {
		return printf("Good	%d", LinkStatus.ChannelQuality);
		//return websWrite(wp, replaceSpaces(T("Good     %d%%")), LinkStatus.ChannelQuality);
	}
	else if (LinkStatus.ChannelQuality > 40) {
		return printf("Normal	%d", LinkStatus.ChannelQuality);
		//return websWrite(wp, replaceSpaces(T("Normal     %d%%")), LinkStatus.ChannelQuality);
	}
	else {
		return printf("Weak	%d", LinkStatus.ChannelQuality);
		//return websWrite(wp, replaceSpaces(T("Weak     %d%%")), LinkStatus.ChannelQuality);
	}
}

unsigned int ConvertRssiToSignalQuality(long RSSI)
{
    unsigned int signal_quality;
    if (RSSI >= -50)
        signal_quality = 100;
    else if (RSSI >= -80)    // between -50 ~ -80dbm
        signal_quality = (unsigned int)(24 + (RSSI + 80) * 2.6);
    else if (RSSI >= -90)   // between -80 ~ -90dbm
        signal_quality = (unsigned int)((RSSI + 90) * 2.6);
    else    // < -84 dbm
        signal_quality = 0;

    return signal_quality;
}

static int getRSSI(int antenna)
{
	RT_802_11_LINK_STATUS LinkStatus;
	int s;
	unsigned int nSigQua;
	long RSSI;
	int oid[3] = {RT_OID_802_11_RSSI, RT_OID_802_11_RSSI_1, RT_OID_802_11_RSSI_2};
	const char *G_bdBm_ischeck;

	G_ConnectStatus = getStaLinkStatus();
	if (G_ConnectStatus == NdisMediaStateDisconnected) {
		printf("0%%");
		//websWrite(wp, "0%%");
		return 0;
	}

	s = socket(AF_INET, SOCK_DGRAM, 0);
	// Get Link Status Info from driver
	OidQueryInformation(RT_OID_802_11_QUERY_LINK_STATUS, s, "ra0", &LinkStatus, sizeof(RT_802_11_LINK_STATUS));

	// Signal Strength

	// Get Rssi Value from driver
	OidQueryInformation(oid[antenna], s, "ra0", &RSSI, sizeof(RSSI));

	if (RSSI > 20 || RSSI < -200) {
		close(s);
		return printf("none");
		//return 1;
		//return websWrite(wp, "None");
	}

	// Use convert formula to getSignal Quality
	nSigQua = ConvertRssiToSignalQuality(RSSI);
	if (m_nSigQua[antenna] != 0)
		nSigQua = (unsigned int)((m_nSigQua[antenna] + nSigQua) / 2.0 + 0.5);

	close(s);

	m_nSigQua[antenna] = nSigQua;
	G_bdBm_ischeck = nvram_bufget(RT2860_NVRAM, "G_bdBm_ischeck");
	if (nSigQua > 70) {
		if (strcmp(G_bdBm_ischeck, "1") == 0) { //checked
			return printf("Good     %ld dBm", RSSI);
			
			//return websWrite(wp, replaceSpaces(T("Good     %ld dBm")), RSSI);
		}
		else {
			return printf("Good     %ld dBm", nSigQua);
			//return websWrite(wp, replaceSpaces(T("Good     %d%%")), nSigQua);
		}
	}
	else if (nSigQua > 40) {
		if (strcmp(G_bdBm_ischeck, "1") == 0) { //checked
			//return websWrite(wp, replaceSpaces(T("Normal     %ld dBm")), RSSI);
			return printf("Normal     %ld dBm", RSSI);
		}
		else {
			//return websWrite(wp, replaceSpaces(T("Normal     %d%%")), nSigQua);
			return printf("Normal     %d%%", nSigQua);
		}
	}
	else {
		if (strcmp(G_bdBm_ischeck, "1") == 0) { //checked
			printf("Weak     %ld dBm", RSSI);
			//return websWrite(wp, replaceSpaces(T("Weak     %ld dBm")), RSSI);
		}
		else {
			printf("Weak     %d%%", nSigQua);
			//return websWrite(wp, replaceSpaces(T("Weak     %d%%")), nSigQua);
		}
	}
}

/*
 * description: write station signal strength
 */
static int GetStaSignalStrength(char *argv[])
{
	return getRSSI(0);
}

/*
 * description: write station signal strength
 */
static int GetStaSignalStrength_1(char *argv[])
{
	return getRSSI(1);
}

/*
 * description: write station signal strength
 */
static int GetStaSignalStrength_2(char *argv[])
{
	return getRSSI(2);
}

/*
 * description: write station noise level
 */
static int GetStaNoiseLevel(char *argv[])
{
	unsigned char lNoise; // this value is (ULONG) in Ndis driver (NOTICE!!!)
	int nNoiseDbm;
	int nNoisePercent;
	int s;
	const char *G_bdBm_ischeck;

	G_ConnectStatus = getStaLinkStatus();
	if (G_ConnectStatus == NdisMediaStateDisconnected) {
		return printf("0%%");
		//return websWrite(wp, "0%%");
	}
	G_bdBm_ischeck = nvram_bufget(RT2860_NVRAM, "G_bdBm_ischeck");
	s = socket(AF_INET, SOCK_DGRAM, 0);
	// Noise Level
	// Get Noise Level From Driver
	OidQueryInformation(RT_OID_802_11_QUERY_NOISE_LEVEL, s, "ra0", &lNoise, sizeof(lNoise));

	close(s);

	nNoiseDbm = lNoise;
	nNoiseDbm -= 143;
	nNoisePercent = (nNoiseDbm + 100) * 10 / 3;

	if (nNoisePercent > 100)
		nNoisePercent =100;
	else if (nNoisePercent < 0)
		nNoisePercent =0;

	// Show the NoiseLevel Strength Word & Percentage
	if (nNoisePercent > 90) {
		if (strcmp(G_bdBm_ischeck, "1") == 0){
			//return websWrite(wp, replaceSpaces(T("Strength     %d dBm")), nNoiseDbm);
			return printf("Strength     %d dBm", nNoiseDbm);
		}
		else{
			//return websWrite(wp, replaceSpaces(T("Strength     %d%%")), nNoisePercent);
				return printf("Strength     %d%%", nNoisePercent);
		}
	}
	else if (nNoisePercent > 50) {
		if (strcmp(G_bdBm_ischeck, "1") == 0){
			//return websWrite(wp, replaceSpaces(T("Normal     %d dBm")), nNoiseDbm);
			return printf("Normal     %d dBm", nNoiseDbm);
		}else{
			//return websWrite(wp, replaceSpaces(T("Normal     %d%%")), nNoisePercent);
			return printf("Normal     %d%%", nNoisePercent);
		}
	}
	else {
		if (strcmp(G_bdBm_ischeck, "1") == 0){
			//return websWrite(wp, replaceSpaces(T("Low     %d dBm")), nNoiseDbm);
			return printf("Low     %d dBm", nNoiseDbm);
		}
		else{
			//return websWrite(wp, replaceSpaces(T("Low     %d%%")), nNoisePercent);
			return printf("Low     %d%%", nNoisePercent);
		}
	}
}

/*
 * description: write station HT transmit
 */
static int GetStaHT(char *argv[])
{
	int s;
	HTTRANSMIT_SETTING HTSetting;
#ifdef ENHANCED_STA_STATUS_INFO
	long lastRxRate;
	char tmpTxRate[128];
	char tmpRxRate[128];
	char *phyMode[4] = {"CCK", "OFDM", "MM", "GF"};
#else
	char tmp[8], tmpBW[72], tmpGI[72], tmpSTBC[72], tmpMCS[72];
#endif

	G_ConnectStatus = getStaLinkStatus();
	if (G_ConnectStatus == NdisMediaStateDisconnected)
	{
#ifdef ENHANCED_STA_STATUS_INFO
		snprintf(tmpRxRate, sizeof(tmpRxRate), "<tr><td width=\"35%%\" bgcolor=\"#E8F8FF\" >Last Rx Rate</td><td >n/a</td></tr>");
		snprintf(tmpTxRate, sizeof(tmpTxRate), "<tr><td width=\"35%%\" bgcolor=\"#E8F8FF\" >Tx Rate</td><td >n/a</td></tr>");
		return printf("%s %s", tmpTxRate, tmpRxRat);
		//return websWrite(wp,"%s %s", tmpTxRate, tmpRxRate);
#else
		sprintf((char *)tmpBW, "<tr><td width=\"35%%\" bgcolor=\"#E8F8FF\" >BW</td><td >n/a</td></tr>");
		sprintf((char *)tmpGI, "<tr><td width=\"35%%\" bgcolor=\"#E8F8FF\" >GI</td><td >n/a</td></tr>");
		sprintf((char *)tmpSTBC,"<tr><td width=\"35%%\" bgcolor=\"#E8F8FF\" >STBC</td><td >n/a</td></tr>");
		sprintf((char *)tmpMCS, "<tr><td width=\"35%%\" bgcolor=\"#E8F8FF\" >MCS</td><td >n/a</td></tr>");
		//return websWrite(wp,"%s %s %s %s", tmpBW, tmpGI, tmpSTBC, tmpMCS);
		return printf("%s %s %s %s", tmpBW, tmpGI, tmpSTBC, tmpMCS);
#endif
	}

	s = socket(AF_INET, SOCK_DGRAM, 0);
	memset(&HTSetting, 0x00, sizeof(HTTRANSMIT_SETTING));
	OidQueryInformation(RT_OID_802_11_QUERY_LAST_TX_RATE, s, "ra0", &HTSetting, sizeof(HTTRANSMIT_SETTING));
#ifdef ENHANCED_STA_STATUS_INFO
	OidQueryInformation(RT_OID_802_11_QUERY_LAST_RX_RATE, s, "ra0", &lastRxRate, sizeof(lastRxRate));
#endif
	close(s);

#ifdef ENHANCED_STA_STATUS_INFO
	if (((lastRxRate>>14) & 0x3) >= 1) {		// 0: CCK, 1:OFDM, 2:Mixedmode, 3:GreenField
		snprintf(tmpRxRate, sizeof(tmpRxRate), "<tr><td width=\"35%%\" bgcolor=\"#E8F8FF\" >Last Rx Rate</td><td >MCS %d, %2dMHz, %cGI, %s%s</td></tr>",
					(int)lastRxRate & 0x7F,  ((lastRxRate>>7) & 0x1)? 40: 20,
					((lastRxRate>>8) & 0x1)? 'S': 'L',
					phyMode[(lastRxRate>>14) & 0x3],
					((lastRxRate>>9) & 0x3)? ", STBC": " ");
	}
	else {
		snprintf(tmpRxRate, sizeof(tmpRxRate), "<tr><td width=\"35%%\" bgcolor=\"#E8F8FF\" >Last Rx Rate</td><td >CCK %d</td></tr>",
					(int)lastRxRate & 0x7F);
	}

	if (HTSetting.field.MODE >= 1) {		// 0: CCK, 1:OFDM, 2:Mixedmode, 3:GreenField
		snprintf(tmpTxRate, sizeof(tmpTxRate), "<tr><td width=\"35%%\" bgcolor=\"#E8F8FF\" >Tx Rate</td><td >MCS %d, %2dMHz, %cGI, %s%s%c%c</td></tr>",
					(int)HTSetting.field.MCS,  (HTSetting.field.BW)? 40: 20,
					(HTSetting.field.ShortGI)? 'S': 'L',
					phyMode[HTSetting.field.MODE],
					(HTSetting.field.STBC)? ", STBC ": " ",
					HTSetting.field.iTxBF? 'I': '-',
					HTSetting.field.eTxBF? 'E': '-');
	}
	else {
		snprintf(tmpTxRate, sizeof(tmpTxRate), "<tr><td width=\"35%%\" bgcolor=\"#E8F8FF\" >Tx Rate</td><td >CCK %d</td></tr>",
					(int)HTSetting.field.MCS);
	}

	//return websWrite(wp,"%s %s", tmpTxRate, tmpRxRate);
	return printf("%s %s", tmpTxRate, tmpRxRate);
#else // ENHANCED_STA_STATUS_INFO //
	if(HTSetting.field.MODE > 1) {		// 0: CCK, 1:OFDM, 2:Mixedmode, 3:GreenField
		if (HTSetting.field.BW == 0)
			strcpy(tmp, "20");
		else
			strcpy(tmp, "40");
		snprintf(tmpBW, 72, "<tr><td width=\"35%%\" bgcolor=\"#E8F8FF\" >BW</td><td >%s</td></tr>", tmp);

		if (HTSetting.field.ShortGI == 0)
			strcpy(tmp, "long");
		else
			strcpy(tmp, "short");
		snprintf(tmpGI, 72, "<tr><td width=\"35%%\" bgcolor=\"#E8F8FF\" >GI</td><td >%s</td></tr>", tmp);

		if (HTSetting.field.STBC == 0)
			strcpy(tmp, "none");
		else
			strcpy(tmp, "used");
		snprintf(tmpSTBC, 72, "<tr><td width=\"35%%\" bgcolor=\"#E8F8FF\" >STBC</td><td >%s</td></tr>", tmp);

		snprintf(tmpMCS, 72, "<tr><td width=\"35%%\" bgcolor=\"#E8F8FF\" >MCS</td><td >%d</td></tr>", HTSetting.field.MCS);
	} else {
		sprintf((char *)tmpBW, "<tr><td width=\"35%%\" bgcolor=\"#E8F8FF\" >BW</td><td >n/a</td></tr>");
		sprintf((char *)tmpGI, "<tr><td width=\"35%%\" bgcolor=\"#E8F8FF\" >GI</td><td >n/a</td></tr>");
		sprintf((char *)tmpSTBC,"<tr><td width=\"35%%\" bgcolor=\"#E8F8FF\" >STBC</td><td >n/a</td></tr>");
		sprintf((char *)tmpMCS, "<tr><td width=\"35%%\" bgcolor=\"#E8F8FF\" >MCS</td><td >n/a</td></tr>");
	}

	//return websWrite(wp,"%s %s %s %s", tmpBW, tmpGI, tmpSTBC, tmpMCS);
	return printf("%s %s %s %s", tmpBW, tmpGI, tmpSTBC, tmpMCS);
#endif // ENHANCED_STA_STATUS_INFO //
}

/*
 * description: write station SNR
 */
static int GetStaSNR0(char *argv[])
{
	int s, n, ret;
	unsigned long SNR;
/* //????
	if (ejArgs(argc, argv, T("%d"), &n) < 1) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	*/
	G_ConnectStatus = getStaLinkStatus();
	if (G_ConnectStatus == NdisMediaStateDisconnected) {
		//return  websWrite(wp, "n/a");
		return printf("n/a");
	}

	s = socket(AF_INET, SOCK_DGRAM, 0);

		ret = OidQueryInformation(RT_OID_802_11_SNR_0, s, "ra0", &SNR, sizeof(SNR));

	close(s);

	//fprintf(stderr, "SNR%d = %ld\n", n, SNR);
	if (ret < 0){
		//return websWrite(wp, "n/a");
		return printf("n/a");
	}else{
		//return websWrite(wp, "%ld", SNR);
		return printf("%ld", SNR);
	}
}

/*
 * description: write station SNR
 */
static int GetStaSNR1(char *argv[])
{
	int s, n, ret;
	unsigned long SNR;
/* //????
	if (ejArgs(argc, argv, T("%d"), &n) < 1) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	*/
	G_ConnectStatus = getStaLinkStatus();
	if (G_ConnectStatus == NdisMediaStateDisconnected) {
		//return  websWrite(wp, "n/a");
		return printf("n/a");
	}

	s = socket(AF_INET, SOCK_DGRAM, 0);

		ret = OidQueryInformation(RT_OID_802_11_SNR_1, s, "ra0", &SNR, sizeof(SNR));

	close(s);

	//fprintf(stderr, "SNR%d = %ld\n", n, SNR);
	if (ret < 0){
		//return websWrite(wp, "n/a");
		return printf("n/a");
	}else{
		//return websWrite(wp, "%ld", SNR);
		return printf("%ld", SNR);
	}
}

/*
 * description: write station SNR
 */
static int GetStaSNR2(char *argv[])
{
	int s, n, ret;
	unsigned long SNR;
/* //????
	if (ejArgs(argc, argv, T("%d"), &n) < 1) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	*/
	G_ConnectStatus = getStaLinkStatus();
	if (G_ConnectStatus == NdisMediaStateDisconnected) {
		//return  websWrite(wp, "n/a");
		return printf("n/a");
	}

	s = socket(AF_INET, SOCK_DGRAM, 0);

		ret = OidQueryInformation(RT_OID_802_11_SNR_2, s, "ra0", &SNR, sizeof(SNR));

	close(s);

	//fprintf(stderr, "SNR%d = %ld\n", n, SNR);
	if (ret < 0){
		//return websWrite(wp, "n/a");
		return printf("n/a");
	}else{
		//return websWrite(wp, "%ld", SNR);
		return printf("%ld", SNR);
	}
}

/*
 * description: write station Stream SNR
 */
static int GetStaStreamSNR(char *argv[])
{
	int s, ret;
	long SNR[3];
	char tmpBuffer[32];

	s = socket(AF_INET, SOCK_DGRAM, 0);
	ret = OidQueryInformation(RT_OID_802_11_STREAM_SNR, s, "ra0", &SNR, sizeof(SNR));
	close(s);

	if (ret<0) return printf("n/a");
		//return websWrite(wp, "n/a");

	snprintf(tmpBuffer, sizeof(tmpBuffer), "%0.1f, %0.1f, %0.1f", SNR[0]*.25, SNR[1]*.25, SNR[2]*.25);

	//return websWrite(wp, "%s", tmpBuffer);
	printf("%s", tmpBuffer);
}

/*
 * description: return DLS List
 */
static int GetStaDLSList(char *argv[])
{
	RT_802_11_DLS_INFO dls_info;
	int s, i;
	char tmpmac[20];

	memset(tmpmac, 0x00, sizeof(tmpmac));

	s = socket(AF_INET, SOCK_DGRAM, 0);

	OidQueryInformation(RT_OID_802_11_QUERY_DLS_PARAM, s, "ra0", &dls_info, sizeof(dls_info));
	for (i=0; i<MAX_NUM_OF_DLS_ENTRY; i++) {
		if (dls_info.Entry[i].Valid == 1 && dls_info.Entry[i].Status == DLS_FINISH) {
			sprintf(tmpmac, "%02x-%02x-%02x-%02x-%02x-%02x",
					dls_info.Entry[i].MacAddr[0], dls_info.Entry[i].MacAddr[1], dls_info.Entry[i].MacAddr[2],
					dls_info.Entry[i].MacAddr[3], dls_info.Entry[i].MacAddr[4], dls_info.Entry[i].MacAddr[5]);
			//websWrite(wp, "<tr><td><input type=radio name=selected_dls value=%d>%s</td><td>%d</td></tr>",
					 // i+1, tmpmac, dls_info.Entry[i].TimeOut);
					  printf("<tr><td><input type=radio name=selected_dls value=%d>%s</td><td>%d</td></tr>",
					  i+1, tmpmac, dls_info.Entry[i].TimeOut);
		}
	}
	close(s);
	return 0;
}

/*
 * description: write station statistics Tx all
 */
static int GetStaStatsTx(char *argv[])
{
	NDIS_802_11_STATISTICS  Statistics;
	char  tmpStatisics[16];
	int   s, ret=0;

	s = socket(AF_INET, SOCK_DGRAM, 0);

	memset(&tmpStatisics, 0x00, sizeof(tmpStatisics));

	// Transmit Section
	memset(&Statistics, 0x00, sizeof(Statistics));
	ret = OidQueryInformation(OID_802_11_STATISTICS, s, "ra0", &Statistics, sizeof(Statistics));
	close(s);
	if (ret >= 0) {
		// Frames Transmitted Successfully
		sprintf(tmpStatisics, "%8lld", Statistics.TransmittedFragmentCount.QuadPart);
		//websWrite(wp, "<tr><td class=\"head\">%s</td><td>%s</td></tr>", "Frames Transmitted Successfully", tmpStatisics);
    printf("<tr><td class=\"head\">%s</td><td>%s</td></tr>", "Frames Transmitted Successfully", tmpStatisics);
		// Frames Transmitted Successfully  Without Retry(s)
		sprintf(tmpStatisics, "%8lld", Statistics.TransmittedFragmentCount.QuadPart - Statistics.RetryCount.QuadPart);
		//websWrite(wp, "<tr><td class=\"head\">%s</td><td>%s</td></tr>", "Frames Transmitted Successfully Without Retry", tmpStatisics);
    printf("<tr><td class=\"head\">%s</td><td>%s</td></tr>", "Frames Transmitted Successfully Without Retry", tmpStatisics);
		// Frames Transmitted Successfully After Retry(s)
		sprintf(tmpStatisics, "%8lld", Statistics.RetryCount.QuadPart);
		//websWrite(wp, "<tr><td class=\"head\">%s</td><td>%s</td></tr>", "Frames Transmitted Successfully After Retry(s)", tmpStatisics);
		printf("<tr><td class=\"head\">%s</td><td>%s</td></tr>", "Frames Transmitted Successfully After Retry(s)", tmpStatisics);
		// Frames Fail To Receive ACK After All Retries
		sprintf(tmpStatisics, "%8lld", Statistics.FailedCount.QuadPart);
		//websWrite(wp, "<tr><td class=\"head\">%s</td><td>%s</td></tr>", "Frames Fail To Receive ACK After All Retries", tmpStatisics);
		printf("<tr><td class=\"head\">%s</td><td>%s</td></tr>", "Frames Fail To Receive ACK After All Retries", tmpStatisics);
		// RTS Frames Successfully Receive CTS
		sprintf(tmpStatisics, "%8lld", Statistics.RTSSuccessCount.QuadPart);
		//websWrite(wp, "<tr><td class=\"head\">%s</td><td>%s</td></tr>", "RTS Frames Sucessfully Receive CTS", tmpStatisics);
    printf("<tr><td class=\"head\">%s</td><td>%s</td></tr>", "RTS Frames Sucessfully Receive CTS", tmpStatisics);
		// RTS Frames Fail To Receive CTS
		sprintf(tmpStatisics, "%8lld", Statistics.RTSFailureCount.QuadPart);
		//websWrite(wp, "<tr><td class=\"head\">%s</td><td>%s</td></tr>", "RTS Frames Fail To Receive CTS", tmpStatisics);
		printf("<tr><td class=\"head\">%s</td><td>%s</td></tr>", "RTS Frames Fail To Receive CTS", tmpStatisics);
		// Frames Received Successfully
		sprintf(tmpStatisics, "%8lld", Statistics.ReceivedFragmentCount.QuadPart);
		//websWrite(wp, "<tr><td class=\"title\" colspan=2 id=statisticRx>Receive Statistics</td></tr>");
		printf("<tr><td class=\"title\" colspan=2 id=statisticRx>Receive Statistics</td></tr>");
		//websWrite(wp, "<tr><td class=\"head\">%s</td><td>%s</td></tr>", "Frames Received Successfully", tmpStatisics);
		printf("<tr><td class=\"head\">%s</td><td>%s</td></tr>", "Frames Received Successfully", tmpStatisics);
		return 0;
	}
	else{
		//return websWrite(wp, "<tr><td>no data</td></tr>");
		return printf("<tr><td>no data</td></tr>");
	}
}

/*
 * description: write station statistics Rx CRC error
 */
static int GetStaStatsRxCRCErr(char *argv[])
{
	
	
	NDIS_802_11_STATISTICS  Statistics;

	int s ,ret=0;
	char  tmpStatisics[16];
	memset(&tmpStatisics, 0x00, sizeof(tmpStatisics));
	memset(&Statistics, 0x00, sizeof(Statistics));
	s = socket(AF_INET, SOCK_DGRAM, 0);
	ret = OidQueryInformation(OID_802_11_STATISTICS, s, "ra0", &Statistics, sizeof(Statistics));
	// Frames Received With CRC Error
	if (ret >= 0){
		sprintf(tmpStatisics, "%8lld", Statistics.FCSErrorCount.QuadPart);
		//websWrite(wp, "<tr><td class=\"head\">%s</td><td>%s</td></tr>", "Frames Transmitted Successfully", tmpStatisics);
    //printf("<tr><td class=\"head\">%s</td><td>%s</td></tr>", "Frames Transmitted Successfully", tmpStatisics);
		//websWrite(wp, "%ld", Statistics.FCSErrorCount.QuadPart);
		printf("%s", tmpStatisics);
	}else{
		//websWrite(wp, "0");
		printf("0");
	}
	close(s);
	return 0;
}

/*
 * description: write station statistics Rx no buffer
 */
static int GetStaStatsRxNoBuf(char *argv[])
{

	unsigned long lRcvNoBuf = 0;
	int s;

	s = socket(AF_INET, SOCK_DGRAM, 0);

	// Frames Dropped Due To Out-of-Resource
	if (OidQueryInformation(OID_GEN_RCV_NO_BUFFER, s, "ra0", &lRcvNoBuf, sizeof(lRcvNoBuf)) >= 0){
		//websWrite(wp, "%ld", lRcvNoBuf);
		printf("%ld", lRcvNoBuf);
	}else{
		printf("0");
		//websWrite(wp, "0");
	}
	close(s);
	return 0;



}

/*
 * description: write station statistics Rx duplicate
 */
static int GetStaStatsRxDup(char *argv[])
{
#if(0)
	NDIS_802_11_STATISTICS  Statistics;

	int s;
	s = socket(AF_INET, SOCK_DGRAM, 0);

	memset(&Statistics, 0x00, sizeof(Statistics));
	// Duplicate Frames Received
	if (OidQueryInformation(OID_802_11_STATISTICS, s, "ra0", &Statistics, sizeof(Statistics)) >= 0){
		//websWrite(wp, "%ld", Statistics.FrameDuplicateCount.QuadPart);
		printf("%ld", Statistics.FrameDuplicateCount.QuadPart);
	}else{
		//websWrite(wp, "0");
		printf("0");
	}
	close(s);
	return 0;
#endif
	NDIS_802_11_STATISTICS  Statistics;

	int s ,ret=0;
	char  tmpStatisics[16];
	memset(&tmpStatisics, 0x00, sizeof(tmpStatisics));
	memset(&Statistics, 0x00, sizeof(Statistics));
	s = socket(AF_INET, SOCK_DGRAM, 0);
	ret = OidQueryInformation(OID_802_11_STATISTICS, s, "ra0", &Statistics, sizeof(Statistics));
	// Frames Received With CRC Error
	if (ret >= 0){
		sprintf(tmpStatisics, "%8lld", Statistics.FrameDuplicateCount.QuadPart);
		//websWrite(wp, "<tr><td class=\"head\">%s</td><td>%s</td></tr>", "Frames Transmitted Successfully", tmpStatisics);
    //printf("<tr><td class=\"head\">%s</td><td>%s</td></tr>", "Frames Transmitted Successfully", tmpStatisics);
		//websWrite(wp, "%ld", Statistics.FCSErrorCount.QuadPart);
		printf("%s", tmpStatisics);
	}else{
		//websWrite(wp, "0");
		printf("0");
	}
	close(s);
	return 0;
}

/*
 * description: write station TxBf statistics
 */
static int GetStaStatsTxBf(char *argv[])
{
//#if defined(RT2860_STA_TXBF_SUPPORT) && defined(ENHANCED_STA_STATUS_INFO)
#if defined (RT2860_STA_SUPPORT) && defined (RT2860_TXBF_SUPPORT) && defined(ENHANCED_STA_STATUS_INFO)
	char  tmpStatisics[256];
	RT_802_11_TXBF_TABLE table = {0};
	int i, s, ret;
	int displayHeader = FALSE;

	s = socket(AF_INET, SOCK_DGRAM, 0);

	memset(&tmpStatisics, 0x00, sizeof(tmpStatisics));

	// Transmit Section
	ret = OidQueryInformation(RT_OID_802_11_QUERY_TXBF_TABLE, s, "ra0", &table, sizeof(table));
	close(s);

	if (ret < 0 ) {
		//return websWrite(wp, T("<tr><td class=\"title\">RT_OID_802_11_QUERY_TXBF_TABLE failed</td></tr>"));
		return printf("<tr><td class=\"title\">RT_OID_802_11_QUERY_TXBF_TABLE failed</td></tr>"));
		return -1;
	}

	for (i=0; i<table.Num; i++) {
		// Format BF Statistics
		RT_COUNTER_TXBF *pCnt = &(table.Entry[i]);
		unsigned long totalNBF, totalEBF, totalIBF, totalTx, totalRetry, totalSuccess;

		totalNBF = pCnt->TxSuccessCount + pCnt->TxFailCount;
		totalEBF = pCnt->ETxSuccessCount + pCnt->ETxFailCount;
		totalIBF = pCnt->ITxSuccessCount + pCnt->ITxFailCount;
		totalTx = totalNBF + totalEBF + totalIBF;
		totalRetry = pCnt->TxRetryCount + pCnt->ETxRetryCount + pCnt->ITxRetryCount;
		totalSuccess = pCnt->TxSuccessCount + pCnt->ETxSuccessCount + pCnt->ITxSuccessCount;

		if (totalTx==0) {
			continue;
		}

		if (!displayHeader) {
			//websWrite(wp, T("<tr><td class=\"title\" colspan=2>Detailed TX Statistics (Retry count is approximate)</td></tr>"));
			printf("<tr><td class=\"title\" colspan=2>Detailed TX Statistics (Retry count is approximate)</td></tr>"));
			displayHeader = TRUE;
		}

		//websWrite(wp, T("<tr><td class=\"head2\">%d</td><td><kbd>"), i);
		printf("<tr><td class=\"head2\">%d</td><td><kbd>"), i);
		//websWrite(wp, T("%s"), replaceSpaces(T("              Success    Retry/PER    Fail/PLR<br>")) );
		printf("              Success    Retry/PER    Fail/PLR<br>");
		if (totalNBF!=0) {
			/*
			sprintf(tmpStatisics, T("NonBF (%3lu%%): %7lu  %7lu (%2lu%%) %5lu (%1lu%%)<br>"),
				100*totalNBF/totalTx, pCnt->TxSuccessCount,
				pCnt->TxRetryCount, 100*pCnt->TxRetryCount/(pCnt->TxSuccessCount+pCnt->TxRetryCount),
				pCnt->TxFailCount, 100*pCnt->TxFailCount/totalNBF);
			websWrite(wp, T("%s"), replaceSpaces(tmpStatisics));*/
				sprintf(tmpStatisics, "NonBF (%3lu%%): %7lu  %7lu (%2lu%%) %5lu (%1lu%%)<br>",
				100*totalNBF/totalTx, pCnt->TxSuccessCount,
				pCnt->TxRetryCount, 100*pCnt->TxRetryCount/(pCnt->TxSuccessCount+pCnt->TxRetryCount),
				pCnt->TxFailCount, 100*pCnt->TxFailCount/totalNBF);
				printf("%s", tmpStatisics);
		}

		if (totalEBF!=0) {
			/*
			sprintf(tmpStatisics, T("ETxBF (%3lu%%): %7lu  %7lu (%2lu%%) %5lu (%1lu%%)<br>"),
				100*totalEBF/totalTx, pCnt->ETxSuccessCount,
				pCnt->ETxRetryCount, 100*pCnt->ETxRetryCount/(pCnt->ETxSuccessCount+pCnt->ETxRetryCount),
				pCnt->ETxFailCount, 100*pCnt->ETxFailCount/totalEBF);
			websWrite(wp, T("%s"), replaceSpaces(tmpStatisics));*/
				sprintf(tmpStatisics, "ETxBF (%3lu%%): %7lu  %7lu (%2lu%%) %5lu (%1lu%%)<br>",
				100*totalEBF/totalTx, pCnt->ETxSuccessCount,
				pCnt->ETxRetryCount, 100*pCnt->ETxRetryCount/(pCnt->ETxSuccessCount+pCnt->ETxRetryCount),
				pCnt->ETxFailCount, 100*pCnt->ETxFailCount/totalEBF);
				printf("%s"), tmpStatisics);
		}

		if (totalIBF!=0) {
			/*
			sprintf(tmpStatisics, T("ITxBF (%3lu%%): %7lu  %7lu (%2lu%%) %5lu (%1lu%%)<br>"),
				100*totalIBF/totalTx, pCnt->ITxSuccessCount,
				pCnt->ITxRetryCount, 100*pCnt->ITxRetryCount/(pCnt->ITxSuccessCount+pCnt->ITxRetryCount),
				pCnt->ITxFailCount, 100*pCnt->ITxFailCount/totalIBF);
			websWrite(wp, T("%s"), replaceSpaces(tmpStatisics));*/
				sprintf(tmpStatisics, "ITxBF (%3lu%%): %7lu  %7lu (%2lu%%) %5lu (%1lu%%)<br>",
				100*totalIBF/totalTx, pCnt->ITxSuccessCount,
				pCnt->ITxRetryCount, 100*pCnt->ITxRetryCount/(pCnt->ITxSuccessCount+pCnt->ITxRetryCount),
				pCnt->ITxFailCount, 100*pCnt->ITxFailCount/totalIBF);
				printf("%s"), tmpStatisics);
			
		}
/*
		sprintf(tmpStatisics, T("Total         %7lu  %7lu (%2lu%%) %5lu (%1lu%%)"),
			totalSuccess, totalRetry, 100*totalRetry/(totalSuccess + totalRetry),
			pCnt->TxFailCount+pCnt->ETxFailCount+pCnt->ITxFailCount,
			100*(pCnt->TxFailCount+pCnt->ETxFailCount+pCnt->ITxFailCount)/totalTx);
		websWrite(wp, T("%s"), replaceSpaces(tmpStatisics));
		websWrite(wp, T("</kbd></td></tr>"));*/
			sprintf(tmpStatisics, "Total         %7lu  %7lu (%2lu%%) %5lu (%1lu%%)",
			totalSuccess, totalRetry, 100*totalRetry/(totalSuccess + totalRetry),
			pCnt->TxFailCount+pCnt->ETxFailCount+pCnt->ITxFailCount,
			100*(pCnt->TxFailCount+pCnt->ETxFailCount+pCnt->ITxFailCount)/totalTx);
			printf("%s", tmpStatisics);
			printf("</kbd></td></tr>");
	}
#endif
	
	return 0;
}

/*
 * description: return station support A band
 */
static int GetStaSuppAMode(char *argv[])
{
	if (myGetSuppAMode() == 1)
		printf("%s", "1");
	else
		printf("%s", "0");
		return 0;
}

static int GetStaWirelessMode(char *argv[])
{
	const char *mode_s = nvram_bufget(RT2860_NVRAM, "WirelessMode");
	int mode;
	int bSuppA = myGetSuppAMode();

	mode = (NULL == mode_s)? 0 : atoi(mode_s);
	printf("<option value=0 %s>802.11 B/G mixed mode</option>", (mode == 0)? "selected" : "");
	printf("<option value=1 %s>802.11 B Only</option>", (mode == 1)? "selected" : "");
	if (bSuppA) {
		printf("<option value=2 %s>802.11 A Only</option>", (mode == 2)? "selected" : "");
		printf("<option value=3 %s>802.11 A/B/G mixed mode</option>", (mode == 3)? "selected" : "");
	}
	printf("<option value=4 %s>802.11 G Only</option>", (mode == 4)? "selected" : "");
	printf("<option value=6 %s>802.11 N Only</option>", (mode == 6)? "selected" : "");
	printf("<option value=7 %s>802.11 GN mixed mode</option>", (mode == 7)? "selected" : "");
	if (bSuppA) {
		printf("<option value=8 %s>802.11 AN mixed mode</option>", (mode == 8)? "selected" : "");
	}
	printf("<option value=9 %s>802.11 B/G/N mixed mode</option>", (mode == 9)? "selected" : "");
	if (bSuppA) {
		printf("<option value=10 %s>802.11 A/G/N mixed mode</option>", (mode == 10)? "selected" : "");
		printf("<option value=5 %s>802.11 A/B/G/N mixed mode</option>", (mode == 5)? "selected" : "");
	}

	return 0;
}        
        
/*
 * description: write station driver version
 */
int GetStaDriverVer(char *argv[])
{
	//RT_VERSION_INFO DriverVersionInfo;
	unsigned char DriverVersionInfo[8];
	int s;

	s = socket(AF_INET, SOCK_DGRAM, 0);

	//Driver
	if (OidQueryInformation(RT_OID_VERSION_INFO, s, "ra0", &DriverVersionInfo, sizeof(DriverVersionInfo)) >= 0) {
		//websWrite(wp, "%d.%d.%d.%d", DriverVersionInfo.DriverVersionW, DriverVersionInfo.DriverVersionX, DriverVersionInfo.DriverVersionY, DriverVersionInfo.DriverVersionZ);
		//sprintf(tmp, "%04d-%02d-%02d", DriverVersionInfo.DriverBuildYear, DriverVersionInfo.DriverBuildMonth, DriverVersionInfo.DriverBuildDay);
		printf("%s", DriverVersionInfo);
	}
	else
		printf("&nbsp;");

	close(s);
	return 0;
}

/*
 * description: write station mac address
 */
static int GetStaMacAddr(char *argv[])
{
	unsigned char CurrentAddress[6];
	int s;

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (OidQueryInformation(OID_802_3_CURRENT_ADDRESS, s, "ra0", &CurrentAddress, sizeof(CurrentAddress)) >= 0) {
		printf( "%02X-%02X-%02X-%02X-%02X-%02X", CurrentAddress[0], CurrentAddress[1],
				CurrentAddress[2], CurrentAddress[3], CurrentAddress[4], CurrentAddress[5]);
	}
	else
		printf("&nbsp;");

	close(s);
	return 0;
}

#if defined (CONFIG_RT2860V2_STA_WSC)
static int GetWPSSTAModeASP(char *argv[])
{
	const char *tmp = nvram_bufget(RT2860_NVRAM, "staWPSMode");
	if(!tmp || !strlen(tmp)){
		//websWrite(wp, T("0"));	//default is "enrollee mode"
		printf("0");
	}else{
		//websWrite(wp, T("%s"), tmp);
		printf("%s", tmp);
	}
	return 0;
}     

static char *DevicePasswordIDTranslate(unsigned short num)
{
	static char result[32];
	if(num == 0x0000)
		strncpy(result, "PIN (default)", 32);
	else if(num == 0x0001)
		strncpy(result, "User-specified", 32);
	else if(num == 0x0002)
		strncpy(result, "Machine-specified", 32);
	else if(num == 0x0003)
		strncpy(result, "Rekey", 32);
	else if(num == 0x0004)
		strncpy(result, "Push Button", 32);
	else if(num == 0x0005)
		strncpy(result, "Registrar-specified", 32);
	else
		strncpy(result, "Reserved or unknown", 32);
	return result;
}

static char *RFBandTranslate(unsigned char rfband)
{
	static char result[16];
	strncpy(result, "Unknown", 16);
	switch(rfband){
	case 1:
		strncpy(result, "2.4G", 16);
		break;
	case 2:
		strncpy(result, "5G", 16);
		break;
	}
	return result;
}

static char *PrimaryDeviceTypeTranslate(unsigned short category, unsigned short sub_cat)
{
	static char result[32];
	snprintf(result, 32, "Unknown:%d,%d", category, sub_cat);
	switch(category){
	case 1:
		if(sub_cat == 1)
			strncpy(result, "PC", 32);
		if(sub_cat == 2)
			strncpy(result, "Server", 32);
		if(sub_cat == 3)
			strncpy(result, "Media Center", 32);
		break;
	case 2:
		strncpy(result, "Input Device", 32);
		break;
	case 3:
		if(sub_cat == 1)
			strncpy(result, "Printer", 32);
		if(sub_cat == 2)
			strncpy(result, "Scanner", 32);
		break;
	case 4:
		strncpy(result, "Digital Still Camera", 32);
	case 5:
		strncpy(result, "Network Storage", 32);
	case 6:
		if(sub_cat == 1)
			strncpy(result, "Access Point", 32);
		if(sub_cat == 2)
			strncpy(result, "Router", 32);
		if(sub_cat == 3)
			strncpy(result, "Switch", 32);
		break;
	case 7:
		if(sub_cat == 1)
			strncpy(result, "TV", 32);
		if(sub_cat == 2)
			strncpy(result, "Eletronic Picture Frame", 32);
		if(sub_cat == 3)
			strncpy(result, "Projector", 32);
		break;
	case 8:
		if(sub_cat == 1)
			strncpy(result, "Access Point", 32);
		if(sub_cat == 2)
			strncpy(result, "Router", 32);
		if(sub_cat == 3)
			strncpy(result, "Switch", 32);
		break;
	case 9:
		if(sub_cat == 1)
			strncpy(result, "DAR", 32);
		if(sub_cat == 2)
			strncpy(result, "PVR", 32);
		if(sub_cat == 3)
			strncpy(result, "MCX", 32);
		if(sub_cat == 4)
			strncpy(result, "DMR", 32);
		break;
	case 10:
		strncpy(result, "Windows Mobile", 32);

	}
	return result;
}

static char *ConfigMethodTranslate(unsigned short num)
{
	static char result[128];
	result[0] = '\0';

	if(num & 0x0001)
		strcat(result, "USBA(Flash Drive),");
	if(num & 0x0002)
		strcat(result, "Ethernet,");
	if(num & 0x0004)
		strcat(result, "Label,");
	if(num & 0x0008)
		strcat(result, "Display,");
	if(num & 0x0010)
		strcat(result, "External NFC Token,");
	if(num & 0x0020)
		strcat(result, "Integrated NFC Token,");
	if(num & 0x0040)
		strcat(result, "NFC Interface,");
	if(num & 0x0080)
		strcat(result, "Push Button,");
	if(num & 0x0100)
		strcat(result, "Keypad");

	return result;
}

#define SITE_SURVEY_APS_MAX		(16*1024)

/*
 * description: STA WPS: write the WPS BSSID list (WPS AP site survey)
 */
static int GetStaWPSBSSIDListASP(char *argv[])
{
	int                         s, ret, retry;
	unsigned int                lBufLen = 4096, we_version = 16; // 64K
	PNDIS_802_11_BSSID_LIST_EX	pBssidList;
	PNDIS_WLAN_BSSID_EX  		pBssid;
	unsigned int                ConnectStatus = NdisMediaStateDisconnected;
	unsigned char               BssidQuery[6];
	int EAGAIN_Count,QueryCount=0;
  	char *g_pAPListData_tmp =NULL;
  	char *g_pAPListData=NULL;
	
	
        //strcpy(g_pAPListData, nvram_bufget(RT2860_NVRAM, "g_pAPListData"));
 	//DBG_MSG("%s",g_pAPListData);
	/*
	 * dont free(g_pAPListData) during goahead life time because we want it persistant
	 */
	if(g_pAPListData == NULL){
		//printf("wps site survey cache created.\n");
		if((g_pAPListData = (char *)malloc(SITE_SURVEY_APS_MAX)) == NULL){
			DBG_MSG("ERROT!");
			return -1;
		}
		memset(g_pAPListData, 0, SITE_SURVEY_APS_MAX);
	}

	if(strlen(g_pAPListData)){
		//printf("%s", g_pAPListData);
		DBG_MSG("[DBG] g_pAPListData =%s ", g_pAPListData); //Harry
		//websLongWrite(wp, g_pAPListData);
		return 0;
	}
	s = socket(AF_INET, SOCK_DGRAM, 0);
	pBssidList = (PNDIS_802_11_BSSID_LIST_EX) malloc(65536);  //64k
	memset(pBssidList, 0x00, sizeof(char)*65536);

	//step 1
	while(ConnectStatus != NdisMediaStateConnected && QueryCount < 3) {
		if (OidQueryInformation(OID_GEN_MEDIA_CONNECT_STATUS, s, "ra0", &ConnectStatus, sizeof(ConnectStatus)) < 0) {
			//websError(wp, 500, "Query OID_GEN_MEDIA_CONNECT_STATUS failed!");
			DBG_MSG("Query OID_GEN_MEDIA_CONNECT_STATUS failed!");
			free(pBssidList); 
			close(s);
			return -1;
		}
		sleep(2);
		QueryCount++;
	}

	//step 2
	if (OidQueryInformation(RT_OID_802_11_RADIO, s, "ra0", &G_bRadio, sizeof(G_bRadio)) < 0) {
		//websError(wp, 500, "Query RT_OID_802_11_RADIO failed!");
		DBG_MSG("Query RT_OID_802_11_RADIO failed!");
		free(pBssidList); 
		close(s);
		return -1;
	}

	if (ConnectStatus == NdisMediaStateConnected && G_bRadio) {
		// for handshake
		memset(&BssidQuery, 0x00, sizeof(BssidQuery));
		OidQueryInformation(OID_802_11_BSSID, s, "ra0", &BssidQuery, sizeof(BssidQuery));
	}

	//step 3
	if (OidSetInformation(OID_802_11_BSSID_LIST_SCAN, s, "ra0", 0, 0) < 0) {
		//websError(wp, 500, "Set OID_802_11_BSSID_LIST_SCAN failed!");
		DBG_MSG("Set OID_802_11_BSSID_LIST_SCAN failed!");
		free(pBssidList); 
		close(s);
		return -1;
	}
	// wait a few seconds to get all AP.
	Sleep(2);

	ret = OidQueryInformation(RT_OID_WE_VERSION_COMPILED, s, "ra0", &we_version, sizeof(we_version));
	if (ret< 0)
	{
		//websError(wp, 500, "Query RT_OID_WE_VERSION_COMPILED error! return=%d", ret);
		DBG_MSG("Query RT_OID_WE_VERSION_COMPILED error! return=%d", ret);
		free(pBssidList);
		close(s);
		return -1;
	}
	if(we_version >= 17)
		lBufLen=8192;
	else
		lBufLen=4096;

	ret=-1;
	retry=1;
	EAGAIN_Count=0;

	while(ret < 0)
	{

		memset(pBssidList, 0x00, sizeof(char)*65536);
		ret = OidQueryInformation(OID_802_11_BSSID_LIST, s, "ra0", pBssidList, lBufLen);
		//int retry=0;
#if 1
		if (errno == EAGAIN)
		{
			sleep(1);
			fprintf(stderr, "errno == EAGAIN\n");
			EAGAIN_Count++;
			if(EAGAIN_Count>25)
			{
				//websError(wp, 500, "Query OID_802_11_BSSID_LIST error! errno == EAGAIN");
				DBG_MSG("Query OID_802_11_BSSID_LIST error! errno == EAGAIN");
				free(pBssidList);
				close(s);
				return -1;
			}
			else
				continue;
		}
		else if (errno == E2BIG)
		{
			fprintf(stderr, "errno == E2BIG\n");
			lBufLen = lBufLen + 4096*retry;
			if (lBufLen < 65536)
			{
				retry++;
				fprintf(stderr,"lBufLen=%d\n",lBufLen);
				continue;
			}
			else
			{
				//websError(wp, 500, "Query OID_802_11_BSSID_LIST error! E2BIG");
				DBG_MSG("Query OID_802_11_BSSID_LIST error! E2BIG");
				free(pBssidList);
				close(s);
				return -1;
			}
		}
		else if( ret != 0 )
		{
			//websError(wp, 500, "Query OID_802_11_BSSID_LIST error! return=%d", ret);
			DBG_MSG("Query OID_802_11_BSSID_LIST error! return=%d", ret);
			free(pBssidList);
			close(s);
			return -1;
		}
#else

		switch(errno)
		{
		case EAGAIN:
			do
			{
				sleep(1);
				ret = OidQueryInformation(OID_802_11_BSSID_LIST, s, "ra0", pBssidList, lBufLen);
				retry++;
			}while((errno == EAGAIN) && (ret <0) && (retry<5));
			//while((errno == EAGAIN) && (ret <0) && (retry<25));
			break;

		case E2BIG:
			{
				retry=1;
				do
				{
					lBufLen = lBufLen + 4096*retry;
					free(pBssidList);
					pBssidList = (PNDIS_802_11_BSSID_LIST_EX) malloc(lBufLen);
					ret = OidQueryInformation(OID_802_11_BSSID_LIST, s, "ra0", pBssidList, lBufLen);
					retry++;
				}while((errno == E2BIG) && (ret <0) && (retry<10));
			}
		default:
			if (ret < 0)
			{
				fprintf(stderr, "Query OID_802_11_BSSID_LIST error! return=%d\n", ret);
				//websError(wp, 500, "Query OID_802_11_BSSID_LIST error! return=%d", ret);
				DBG_MSG("Query OID_802_11_BSSID_LIST error! return=%d", ret);
				close(s);
				return;
			}
			else
				break;
		}
#endif
	}

	if ( pBssidList->NumberOfItems == 0)
	{
		fprintf(stderr, "Bssid List number is 0!\n");
		free(pBssidList);
		close(s);
		return -1;
	}
	else
	{
		unsigned char tmpRadio[188], tmpBSSIDII[20], tmpBSSID[28], tmpSSID[64+NDIS_802_11_LENGTH_SSID], tmpRSSI[16], tmpChannel[16], tmpAuth[32], tmpEncry[20], tmpImg[40];
		unsigned char tmpSSIDII[NDIS_802_11_LENGTH_SSID+1];
		unsigned char version = 0x0, wsc_state=0, ap_setup_lock=0, selected_registrar=0, uuid_e[16], rf_band=0;
		unsigned short device_password_id=0, selected_registrar_config_method=0, primary_device_type_category=0, primary_device_type_subcategory=0;
		unsigned int IEFlags = 0;
		unsigned char extend[1024];

		int i=0, j=0;
		unsigned int nSigQua;
		int nChannel = 1;
		unsigned char radiocheck[8];
		int wps_ap_flag;

		pBssid = (PNDIS_WLAN_BSSID_EX) pBssidList->Bssid;
		for (i = 0; i < pBssidList->NumberOfItems; i++)
		{
			memset(radiocheck, 0x00, sizeof(radiocheck));
			memset(tmpRadio, 0x00, sizeof(tmpRadio));
			memset(tmpBSSID, 0x00, sizeof(tmpBSSID));
			memset(tmpRSSI, 0x00, sizeof(tmpRSSI));
			memset(tmpSSID, 0x00, sizeof(tmpSSID));
			memset(tmpChannel, 0x00, sizeof(tmpChannel));
			memset(tmpAuth, 0x00, sizeof(tmpAuth));
			memset(tmpEncry, 0x00, sizeof(tmpEncry));
			memset(tmpBSSIDII, 0x00, sizeof(tmpBSSIDII));
			memset(tmpImg, 0x00, sizeof(tmpImg));
			memset(tmpSSIDII, 0x00, sizeof(tmpSSIDII));
			memset(extend, 0x00, sizeof(extend));
			wps_ap_flag = 0;

			if (strcmp((char *)pBssid->Ssid.Ssid, "") == 0)
				sprintf((char *)tmpSSID, "%s", "&nbsp;");
			else {
				int i = 0;
				do {
					if (pBssid->Ssid.Ssid[i] < 32 || pBssid->Ssid.Ssid[i] > 126 || pBssid->Ssid.Ssid[i] == 13) // 13 is string end of Dos
					{
						strncpy(tmpSSID, "&nbsp;", sizeof(tmpSSID));
						break;
					}else if(pBssid->Ssid.Ssid[i] == '\\'){
						snprintf(tmpSSID, sizeof(tmpSSID), "%s%s", tmpSSID, "\\\\");
					}else
						snprintf(tmpSSID, sizeof(tmpSSID), "%s%c", tmpSSID, pBssid->Ssid.Ssid[i]);
					i++;
				} while(i < pBssid->Ssid.SsidLength);
			}

			sprintf((char *)tmpBSSID, "<td>%02X-%02X-%02X-%02X-%02X-%02X</td>", pBssid->MacAddress[0], pBssid->MacAddress[1], pBssid->MacAddress[2], pBssid->MacAddress[3], pBssid->MacAddress[4], pBssid->MacAddress[5]);
			sprintf((char *)tmpBSSIDII, "%02X:%02X:%02X:%02X:%02X:%02X", pBssid->MacAddress[0], pBssid->MacAddress[1], pBssid->MacAddress[2], pBssid->MacAddress[3], pBssid->MacAddress[4], pBssid->MacAddress[5]);

			nSigQua = ConvertRssiToSignalQuality(pBssid->Rssi);

			sprintf((char *)tmpRSSI,"%d", nSigQua);
			nChannel = -1;	
			for(j = 0; j < G_nChanFreqCount; j++){
				if (pBssid->Configuration.DSConfig == ChannelFreqTable[j].lFreq) {
					nChannel = ChannelFreqTable[j].lChannel;
					break;
				}
			}

			if (nChannel == -1)
				continue;

			sprintf((char *)tmpChannel, "%u", nChannel);

			// work with NDIS_WLAN_BSSID_EX
			unsigned char bTKIP = FALSE;
			unsigned char bAESWRAP = FALSE;
			unsigned char bAESCCMP = FALSE;
			unsigned char bWPA = FALSE;
			unsigned char bWPAPSK = FALSE;
			unsigned char bWPANONE = FALSE;
			unsigned char bWPA2 = FALSE;
			unsigned char bWPA2PSK = FALSE;
			unsigned char bWPA2NONE = FALSE;
			unsigned char bCCKM = FALSE; // CCKM for Cisco, add by candy 2006.11.24

			if ((pBssid->Length > sizeof(NDIS_WLAN_BSSID)) && (pBssid->IELength > sizeof(NDIS_802_11_FIXED_IEs)))
			{
				unsigned int lIELoc = 0;

				PNDIS_802_11_FIXED_IEs pFixIE = (PNDIS_802_11_FIXED_IEs)pBssid->IEs;
				PNDIS_802_11_VARIABLE_IEs pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pFixIE + sizeof(NDIS_802_11_FIXED_IEs));
				lIELoc += sizeof(NDIS_802_11_FIXED_IEs);

				while (pBssid->IELength > (lIELoc + sizeof(NDIS_802_11_VARIABLE_IEs)))
				{

					/* WPS section  WPS OUI: 00 50 F2 04 , little endian form */
					if ((pVarIE->ElementID == 221) && *((unsigned int*)((char*)pVarIE->data)) == 0x04f25000)
					{
						unsigned char *pos, *data_head;
						unsigned short len_in_hdr;
						unsigned char tot_len =  pVarIE->Length;

						data_head = pos = (unsigned char*)pVarIE->data + 4;

						version = 0x0;
						wsc_state = 0;
						ap_setup_lock = 0xff;
						selected_registrar = 0xff;
						memset(uuid_e, 16, 0);
						rf_band = 0xff;
						device_password_id = 0xffff;
						selected_registrar_config_method = 0xffff;
						IEFlags = 0;

						/* TLV:  T=2bytes, L=2bytes, V=... */
						while(pos < data_head + tot_len - 4)
						{
							len_in_hdr = ntohs(*(unsigned short *)(pos+2));

							if(tot_len - (pos - data_head) < len_in_hdr){
								//printf("Error: crafted WSC packet? tot_len = %d, len_in_hdr = %d\n", tot_len, len_in_hdr);
								DBG_MSG("Error: crafted WSC packet? tot_len = %d, len_in_hdr = %d\n", tot_len, len_in_hdr);
								break;
							}

#define CASE(x)	case x:\
							if(len_in_hdr != x##_LEN){ DBG_MSG("Error: crafted WSC packet? %s.\n", #x); goto WSC_FAILED;}  \
							IEFlags |= x##_BEACON;
#define TESTBIT(x)	(x##_BEACON & IEFlags)

							switch( ntohs(*(unsigned short *)pos)){
								CASE( WSC_ID_VERSION )
									version = *(unsigned char *)(pos + 2 + 2);
								break;

								CASE( WSC_ID_SC_STATE )
									wsc_state = *(unsigned char *)(pos + 2 + 2);
								break;

								CASE( WSC_ID_AP_SETUP_LOCKED )
									ap_setup_lock = *(unsigned char *)(pos + 2 + 2);
								break;

								CASE( WSC_ID_SEL_REGISTRAR )
									selected_registrar = *(unsigned char *)(pos + 2 + 2);
								break;

								CASE( WSC_ID_DEVICE_PWD_ID )
									device_password_id = ntohs(*(unsigned short *)(pos + 2 + 2));
								break;

								CASE( WSC_ID_SEL_REG_CFG_METHODS )
									selected_registrar_config_method = ntohs(*(unsigned short *)(pos + 2 + 2));
								break;

								CASE( WSC_ID_UUID_E )
									memcpy(uuid_e, (unsigned char *)pos + 2 + 2, 16);
								break;

								CASE( WSC_ID_RF_BAND )
									rf_band = *(unsigned char *)(pos + 2 + 2);
								break;

								CASE( WSC_ID_PRIMARY_DEVICE_TYPE )
									primary_device_type_category =  *(unsigned short *)(pos + 2 + 2);
								primary_device_type_subcategory =  *(unsigned short *)(pos + 2 + 2 + 2 + 2);
							default:
								//printf("unknown tlv:%04x\n", ntohs(*(unsigned short *)pos));
								break;

							}
							pos = pos+2+2+ len_in_hdr;
						}

						if( ! TESTBIT(WSC_ID_VERSION) || ! TESTBIT( WSC_ID_SC_STATE)){
							DBG_MSG("No Version and WPS setup state\n.");
							break;
						}

						wps_ap_flag = 1;
					}
WSC_FAILED:

					if ((pVarIE->ElementID == 221) && (pVarIE->Length >= 16))
					{

						//UINT* pOUI = (UINT*)((char*)pVarIE + 2);
						unsigned int* pOUI = (unsigned int*)((char*)pVarIE->data);
						//fprintf(stderr, "pOUI=0x%08x\n", pOUI);
						if (*pOUI != WPA_OUI_TYPE)
						{
							lIELoc += pVarIE->Length;
							lIELoc += 2;
							pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pVarIE + pVarIE->Length + 2);

							if(pVarIE->Length <= 0){

								break;
							}
							continue;
						}

						unsigned int* plGroupKey; 
						unsigned short* pdPairKeyCount;
						unsigned int* plPairwiseKey=NULL;
						unsigned int* plAuthenKey=NULL;
						unsigned short* pdAuthenKeyCount;
						plGroupKey = (unsigned int*)((char*)pVarIE + 8);

						unsigned int lGroupKey = *plGroupKey & 0x00ffffff;
						//fprintf(stderr, "lGroupKey=%d\n", lGroupKey);
						if (lGroupKey == WPA_OUI) {
							lGroupKey = (*plGroupKey & 0xff000000) >> 0x18;
							if (lGroupKey == 2)
								bTKIP = TRUE;
							else if (lGroupKey == 3)
								bAESWRAP = TRUE;
							else if (lGroupKey == 4)
								bAESCCMP = TRUE;
						}
						else{
							lIELoc += pVarIE->Length;
							lIELoc += 2;
							pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pVarIE + pVarIE->Length + 2);

							if(pVarIE->Length <= 0){

								break;
              }
							continue;
						}

						pdPairKeyCount = (unsigned short*)((char*)plGroupKey + 4);
						plPairwiseKey = (unsigned int*) ((char*)pdPairKeyCount + 2);
						unsigned short k = 0;
						for (k = 0; k < *pdPairKeyCount; k++) {
							unsigned int lPairKey = *plPairwiseKey & 0x00ffffff;
							if (lPairKey == WPA_OUI )//|| (lPairKey & 0xffffff00) == WPA_OUI_1)
							{
								lPairKey = (*plPairwiseKey & 0xff000000) >> 0x18;
								if (lPairKey == 2)
									bTKIP = TRUE;
								else if (lPairKey == 3)
									bAESWRAP = TRUE;
								else if (lPairKey == 4)
									bAESCCMP = TRUE;
							}
							else
								break;

							plPairwiseKey++;
						}

						pdAuthenKeyCount = (unsigned short*)((char*)pdPairKeyCount + 2 + 4 * (*pdPairKeyCount));
						plAuthenKey = (unsigned int*)((char*)pdAuthenKeyCount + 2);

						for(k = 0; k < *pdAuthenKeyCount; k++)
						{
							unsigned int lAuthenKey = *plAuthenKey & 0x00ffffff;
							if (lAuthenKey == CISCO_OUI) {
								bCCKM = TRUE; // CCKM for Cisco
							}
							else if (lAuthenKey == WPA_OUI) {
								lAuthenKey = (*plAuthenKey & 0xff000000) >> 0x18;

								if (lAuthenKey == 1)
									bWPA = TRUE;
								else if (lAuthenKey == 0 || lAuthenKey == 2) {
									if (pBssid->InfrastructureMode)
										bWPAPSK = TRUE;
									else
										bWPANONE = TRUE;
								}
							}					
							plAuthenKey++;
						}
						//break;
					}
					else if (pVarIE->ElementID == 48 && pVarIE->Length >= 12)
					{
						unsigned int* plGroupKey; 
						unsigned int* plPairwiseKey; 
						unsigned short* pdPairKeyCount;
						unsigned int* plAuthenKey; 
						unsigned short* pdAuthenKeyCount;
						plGroupKey = (unsigned int*)((char*)pVarIE + 4);

						unsigned int lGroupKey = *plGroupKey & 0x00ffffff;
						if (lGroupKey == WPA2_OUI) {
							lGroupKey = (*plGroupKey & 0xff000000) >> 0x18;
							if (lGroupKey == 2)
								bTKIP = TRUE;
							else if (lGroupKey == 3)
								bAESWRAP = TRUE;
							else if (lGroupKey == 4)
								bAESCCMP = TRUE;
						}
						else{
							lIELoc += pVarIE->Length;
							lIELoc += 2;
							pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pVarIE + pVarIE->Length + 2);

							if(pVarIE->Length <= 0)
								break;

							continue;
						}

						pdPairKeyCount = (unsigned short*)((char*)plGroupKey + 4);
						plPairwiseKey = (unsigned int*)((char*)pdPairKeyCount + 2);
						unsigned short k = 0;

						for (k = 0; k < *pdPairKeyCount; k++)
						{
							unsigned int lPairKey = *plPairwiseKey & 0x00ffffff;
							if (lPairKey == WPA2_OUI) {
								lPairKey = (*plPairwiseKey & 0xff000000) >> 0x18;
								if (lPairKey == 2)
									bTKIP = TRUE;
								else if (lPairKey == 3)
									bAESWRAP = TRUE;
								else if (lPairKey == 4)
									bAESCCMP = TRUE;
							}
							else
								break;
							plPairwiseKey++;
						}

						pdAuthenKeyCount = (unsigned short*)((char*)pdPairKeyCount + 2 + 4 * *pdPairKeyCount);
						plAuthenKey = (unsigned int*)((char*)pdAuthenKeyCount + 2);
						for (k = 0; k < *pdAuthenKeyCount; k++)
						{
							unsigned int lAuthenKey = *plAuthenKey & 0x00ffffff;
							if (lAuthenKey == CISCO_OUI) {
								bCCKM = TRUE; // CCKM for Cisco
							}
							else if (lAuthenKey == WPA2_OUI) {
								lAuthenKey = (*plAuthenKey & 0xff000000) >> 0x18;
								if (lAuthenKey == 1)
									bWPA2 = TRUE;
								else if (lAuthenKey == 0 || lAuthenKey == 2) {
									if (pBssid->InfrastructureMode)
										bWPA2PSK = TRUE;
									else
										bWPA2NONE = TRUE;
								}
							}					
							plAuthenKey++;
						}
					}

					lIELoc += pVarIE->Length;
					lIELoc += 2;
					pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pVarIE + pVarIE->Length + 2);

					if (pVarIE->Length <= 0){

						break;
					}
				}
			}

			char strAuth[32], strEncry[32];
			memset( strAuth, 0x00, sizeof(strAuth) );
			memset( strEncry, 0x00, sizeof(strEncry) );
			if (bCCKM)
				strcpy(strAuth, "CCKM; ");
			if (bWPA)
				strcpy(strAuth, "WPA; ");
			if (bWPAPSK)
				strcat(strAuth, "WPA-PSK; ");
			if (bWPANONE)
				strcat(strAuth, "WPA-NONE; ");
			if (bWPA2)
				strcat(strAuth, "WPA2; ");
			if (bWPA2PSK)
				strcat(strAuth, "WPA2-PSK; ");
			if (bWPA2NONE)
				strcat(strAuth, "WPA2-NONE; ");

			if (strlen(strAuth) > 0) {
				strncpy((char *)tmpAuth, strAuth, strlen(strAuth) - 2);
				strcpy(strAuth, (char *)tmpAuth);
			}else {
				strcpy((char *)strAuth, "Unknown");
			}

			if (bTKIP)
				strcpy(strEncry, "TKIP; ");
			if (bAESWRAP || bAESCCMP)
				strcat(strEncry, "AES; ");

			if (strlen(strEncry) > 0) {
				strncpy((char *)tmpEncry, strEncry, strlen(strEncry) - 2);
				strcpy(strEncry, (char *)tmpEncry);
			}else {
				if (pBssid->Privacy)  // privacy value is on/of
					strcpy(strEncry, "WEP");
				else {
					strcpy(strEncry, "Not Use");
					strcpy(strAuth, "OPEN");
				}
			}

			sprintf((char *)tmpAuth, "%s", strAuth);
			sprintf((char *)tmpEncry, "%s", strEncry);

			strcpy((char *)tmpSSIDII, pBssid->Ssid.Ssid);

			//		ConverterStringToDisplay((char *)tmpSSIDII);
			//		if (strlen(G_SSID.Ssid)>0 && strcmp(pBssid->Ssid.Ssid, G_SSID.Ssid) == 0)
			//			strcpy(radiocheck, "checked");
			//		else
			//			strcpy(radiocheck, "");

			//		sprintf((char *)tmpRadio, "<td><input type=radio name=selectedSSID %s onClick=\"selectedSSIDChange('%s','%s',%d,%d,'%s','%s')\"></td>", radiocheck, tmpSSIDII, tmpBSSIDII, pBssid->InfrastructureMode, nChannel, strEncry, strAuth);
			//		websWrite(wp, "<tr> %s %s %s %s %s %s %s </tr>\n", tmpRadio, tmpSSID, tmpBSSID, tmpRSSI, tmpChannel, tmpEncry, tmpAuth);

			/*
			 *  "extend" will become a literal part of Javascript, so we use "\\n" instead of "\n".
			 */

			if( TESTBIT( WSC_ID_SEL_REG_CFG_METHODS)){
				strncat(extend, "Config Method:", sizeof(extend));
				snprintf(extend, sizeof(extend), "%s%s\\n", extend, ConfigMethodTranslate(selected_registrar_config_method));
			}

			if( TESTBIT( WSC_ID_DEVICE_PWD_ID)){
				strncat(extend, "Device Password:", sizeof(extend));
				snprintf(extend, sizeof(extend), "%s%s\\n", extend, DevicePasswordIDTranslate(device_password_id));
			}

			if( TESTBIT( WSC_ID_SEL_REGISTRAR)){
				strncat(extend, "Seleted Registrar:", sizeof(extend));
				snprintf(extend, sizeof(extend), "%s%d\\n", extend, selected_registrar);
			}

			if( TESTBIT( WSC_ID_AP_SETUP_LOCKED)){
				strncat(extend, "AP Locked:", sizeof(extend));
				snprintf(extend, sizeof(extend), "%s%d\\n", extend, ap_setup_lock);
			}

			if( TESTBIT( WSC_ID_UUID_E)){
				int pos;
				char uuid_str[WSC_ID_UUID_E_LEN * 2 + 1];
				memset(uuid_str, 0, WSC_ID_UUID_E_LEN * 2 + 1);
				strncat(extend, "UUID:", sizeof(extend));
				for(pos=0; pos< WSC_ID_UUID_E_LEN; pos++){
					snprintf(uuid_str, WSC_ID_UUID_E_LEN * 2 + 1, "%s%02x", uuid_str, uuid_e[pos]);
				}
				snprintf(extend, sizeof(extend), "%s%s\\n", extend, uuid_str);
			}

			if( TESTBIT( WSC_ID_RF_BAND)){
				strncat(extend, "RF Band:", sizeof(extend));
				snprintf(extend, sizeof(extend), "%s%s\\n", extend, RFBandTranslate(rf_band));
			}

			if( TESTBIT( WSC_ID_PRIMARY_DEVICE_TYPE )){
				strncat(extend, "Primary Device Type:", sizeof(extend));
				snprintf(extend, sizeof(extend), "%s%s", extend, PrimaryDeviceTypeTranslate(primary_device_type_category, primary_device_type_subcategory));
			}

			/* This space is important for seperating from two possible closed '\n' characters. */
			strncat(extend, " ", sizeof(extend));

			/*
			 * TODO:  test wps_ap_flag early to save time.
			 */ 
			if(wps_ap_flag){
			
				
				// snprintf(g_pAPListData, SITE_SURVEY_APS_MAX, "%s%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%d\\n%d\\n%s\\n\\n", g_pAPListData, tmpSSID, tmpBSSIDII, tmpRSSI, tmpChannel, tmpAuth, tmpEncry, version, wsc_state, extend);
				snprintf(g_pAPListData, SITE_SURVEY_APS_MAX, "%s%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%d\\n%s\\n\\n", g_pAPListData, tmpSSID, tmpBSSIDII, tmpRSSI, tmpChannel, tmpAuth, tmpEncry, wsc_state, extend);


			}

			pBssid = (PNDIS_WLAN_BSSID_EX)((char *)pBssid + pBssid->Length);
		}
	}
	free(pBssidList);
	close(s);

	//websLongWrite(wp, g_pAPListData);
	printf("%s",  g_pAPListData);
	return 0;
}

static unsigned char *getStaMacAddr(void)
{
	static unsigned char CurrentAddress[6];
	int s;
	memset(CurrentAddress, 0, 6);
	s = socket(AF_INET, SOCK_DGRAM, 0);
	OidQueryInformation(OID_802_3_CURRENT_ADDRESS, s, "ra0", &CurrentAddress, sizeof(CurrentAddress));
	close(s);
	return CurrentAddress;
}

static char *getSTAEnrolleePIN(void *result)
{
	unsigned int pin;
	char *str;
	char long_buf[4096];
	FILE *fp;
	memset(long_buf, 0, 4096);
	if(!(fp = popen("iwpriv ra0 stat", "r")))
		return NULL;
	fread(long_buf, 1, 4096, fp);
	pclose(fp);

	if(!(str = strstr(long_buf, "RT2860 Linux STA PinCode")))
		return NULL;

	str = str + strlen("RT2860 Linux STA PinCode");
	pin = atoi(str);
	sprintf(result, "%08d", pin);
	return result;	
}

static int GetWPSSTAPINCodeASP(char *argv[])
{
	char pin[16];
	if(!getSTAEnrolleePIN(pin)){
		printf("");
		//websWrite(wp, T(""));
	}else{
		printf("%s", pin);
		//websWrite(wp, T("%s"), pin);
	}
	return 0;
}

static int GetWPSSTARegSSIDASP(char *argv[])
{
	const char *regSSID = nvram_bufget(RT2860_NVRAM, "staRegSSID");
	if(!regSSID || !strlen(regSSID)){
		unsigned char mac[6];
		char defaultSSID[33];
		memcpy(mac, getStaMacAddr(), 6);
		snprintf(defaultSSID, 32, "STARegistrar%02X%02X%02X", mac[3], mac[4], mac[5]);
		printf("%s", defaultSSID);
		//websWrite(wp, T("%s"), defaultSSID);
	}else{
		printf("%s", regSSID);
		//websWrite(wp, T("%s"), regSSID);
	}
	return 0;
}

static int GetWPSSTARegAuthASP(char *argv[])
{
	const char *regAuth = nvram_bufget(RT2860_NVRAM, "staRegAuth");
	if(!regAuth || !strlen(regAuth)){
		printf("");
		//websWrite(wp, T(""));
	}else{
		printf("%s", regAuth);
		//websWrite(wp, T("%s"), regAuth);
	}
	return 0;
}

static int GetWPSSTARegEncryASP(char *argv[])
{
	const char *tmp = nvram_bufget(RT2860_NVRAM, "staRegEncry");
	if(!tmp || !strlen(tmp)){
		//websWrite(wp, T(""));
		printf("");
	}else{
		//websWrite(wp, T("%s"), tmp);
		printf("%s", tmp);
	}
	return 0;
}

static int GetWPSSTARegKeyTypeASP(char *argv[])
{
	const char *tmp = nvram_bufget(RT2860_NVRAM, "staRegKeyType");
	if(!tmp || !strlen(tmp)){
		printf("");
		//websWrite(wp, T(""));
	}else{
		printf("%s", tmp);
		//websWrite(wp, T("%s"), tmp);
	}
	return 0;
}	

static int GetWPSSTARegKeyIndexASP(char *argv[])
{
	const char *tmp = nvram_bufget(RT2860_NVRAM, "staRegKeyIndex");
	if(!tmp || !strlen(tmp)){
		//websWrite(wp, T(""));
		printf("");
	}else{
		printf("%s", tmp);
		//websWrite(wp, T("%s"), tmp);
	}
	return 0;
}

static int GetWPSSTARegKeyASP(char *argv[])
{
	const char *tmp = nvram_bufget(RT2860_NVRAM, "staRegKey");
	if(!tmp || !strlen(tmp)){
		//websWrite(wp, T(""));
		printf("");
	}else{
		//websWrite(wp, T("%s"), tmp);
		printf("%s", tmp);
	}
	return 0;
}
#endif
#if 0 //Landen
int OidQueryInformation(unsigned long OidQueryCode, int socket_id, char *DeviceName, void *ptr, unsigned long PtrLength)
{
	struct iwreq wrq;

	strcpy(wrq.ifr_name, DeviceName);
	wrq.u.data.length = PtrLength;
	wrq.u.data.pointer = (caddr_t) ptr;
	wrq.u.data.flags = OidQueryCode;

	return (ioctl(socket_id, RT_PRIV_IOCTL, &wrq));
}

int OidSetInformation(unsigned long OidQueryCode, int socket_id, char *DeviceName, void *ptr, unsigned long PtrLength)
{
	struct iwreq wrq;

	strcpy(wrq.ifr_name, DeviceName);
	wrq.u.data.length = PtrLength;
	wrq.u.data.pointer = (caddr_t) ptr;
	wrq.u.data.flags = OidQueryCode | OID_GET_SET_TOGGLE;

	return (ioctl(socket_id, RT_PRIV_IOCTL, &wrq));
}
#endif
/*
 * description: write the BSSID list (site survey)
 */
static int getStaBSSIDList(char *argv[])
{
	int                         s, ret, retry=1;
	unsigned int                lBufLen = 4096, we_version=16; // 64K
	PNDIS_802_11_BSSID_LIST_EX	pBssidList;
	PNDIS_WLAN_BSSID_EX  		pBssid;
	unsigned int                ConnectStatus = NdisMediaStateDisconnected;
	unsigned char               BssidQuery[6];
	NDIS_802_11_SSID            SSIDQuery;
	int							QueryCount=0, EAGAIN_Count=0;

	s = socket(AF_INET, SOCK_DGRAM, 0);
	pBssidList = (PNDIS_802_11_BSSID_LIST_EX) malloc(65536*2);  //64k
	memset(pBssidList, 0x00, sizeof(char)*65536*2);

	//step 1
	while(ConnectStatus != NdisMediaStateConnected && QueryCount < 3) {
		if (OidQueryInformation(OID_GEN_MEDIA_CONNECT_STATUS, s, "ra0", &ConnectStatus, sizeof(ConnectStatus)) < 0) {
			printf("Query OID_GEN_MEDIA_CONNECT_STATUS failed!");
			free(pBssidList); close(s);
			return -1;
		}
		sleep(2);
		QueryCount++;
	}

	//step 2
	if (OidQueryInformation(RT_OID_802_11_RADIO, s, "ra0", &G_bRadio, sizeof(G_bRadio)) < 0) {
		printf("Query RT_OID_802_11_RADIO failed!");
		free(pBssidList); close(s);
		return -1;
	}

	if (ConnectStatus == NdisMediaStateConnected && G_bRadio) {
		memset(&BssidQuery, 0x00, sizeof(BssidQuery));
		OidQueryInformation(OID_802_11_BSSID, s, "ra0", &BssidQuery, sizeof(BssidQuery));
		memset(&SSIDQuery, 0x00, sizeof(SSIDQuery));
		OidQueryInformation(OID_802_11_SSID, s, "ra0", &SSIDQuery, sizeof(SSIDQuery));
	}

	//step 3
	if (OidSetInformation(OID_802_11_BSSID_LIST_SCAN, s, "ra0", 0, 0) < 0) {
		printf("Set OID_802_11_BSSID_LIST_SCAN failed!");
		free(pBssidList); close(s);
		return -1;
	}
	// wait a few seconds to get all AP.
	//Sleep(2);
	sleep(2);

	//step 4
	ret = OidQueryInformation(RT_OID_WE_VERSION_COMPILED, s, "ra0", &we_version, sizeof(we_version));
	if (ret< 0)
	{
		printf("Query RT_OID_WE_VERSION_COMPILED error! return=%d", ret);
		free(pBssidList);
		close(s);
		return -1;
	}
	if(we_version >= 17)
		lBufLen=8192;
	else
		lBufLen=4096;

	ret = -1;
	retry = 1;
	while (ret < 0) {
		memset(pBssidList, 0x00, sizeof(char)*65536*2);
		ret = OidQueryInformation(OID_802_11_BSSID_LIST, s, "ra0", pBssidList, lBufLen);
		if (errno == EAGAIN) {
			sleep(1);
			// fprintf(stderr, "errno == EAGAIN\n");
			EAGAIN_Count++;
			if(EAGAIN_Count>25) {
				printf("Query OID_802_11_BSSID_LIST error! errno == EAGAIN");
				free(pBssidList);
				close(s);
				return -1;
			}
			else
				continue;
		} else if (errno == E2BIG) {
			// fprintf(stderr, "errno == E2BIG\n");
			// lBufLen = lBufLen + 4096*retry;
			if ((lBufLen * 2) > 65535)
				lBufLen = 65535;
			else
				lBufLen *= 2;
			if (lBufLen < 65536 && retry <= 20) {
				retry++;
				// fprintf(stderr,"lBufLen=%d\n",lBufLen);
				continue;
			} else {
				printf("Query OID_802_11_BSSID_LIST error! E2BIG");
				free(pBssidList);
				close(s);
				return -1;
			}
		}
		else if( ret != 0 ) {
			printf("Query OID_802_11_BSSID_LIST error! return=%d", ret);
			free(pBssidList);
			close(s);
			return -1;
		}
	}
	if ( pBssidList->NumberOfItems == 0) {
		free(pBssidList);
		close(s);
		printf("Bssid List number is 0!\n");
		return 0;
	} else {
		unsigned char tmpRadio[272], tmpBSSIDII[16], tmpBSSID[28], tmpSSID[64+NDIS_802_11_LENGTH_SSID*4], tmpRSSI[60], tmpChannel[16], tmpAuth[32], tmpEncry[52], tmpNetworkType[24], tmpImg[40];
		unsigned char tmpSSIDII[(NDIS_802_11_LENGTH_SSID+1)*5];
		int i=0, j=0;
		unsigned int nSigQua;
		int nChannel = 1;
		unsigned char radiocheck[8];

		pBssid = (PNDIS_WLAN_BSSID_EX) pBssidList->Bssid;
		G_ConnectStatus = NdisMediaStateDisconnected;
		for (i = 0; i < pBssidList->NumberOfItems; i++)
		{
			memset(radiocheck, 0x00, sizeof(radiocheck));
			memset(tmpRadio, 0x00, sizeof(tmpRadio));
			memset(tmpBSSID, 0x00, sizeof(tmpBSSID));
			memset(tmpRSSI, 0x00, sizeof(tmpRSSI));
			memset(tmpSSID, 0x00, sizeof(tmpSSID));
			memset(tmpChannel, 0x00, sizeof(tmpChannel));
			memset(tmpAuth, 0x00, sizeof(tmpAuth));
			memset(tmpEncry, 0x00, sizeof(tmpEncry));
			memset(tmpNetworkType, 0x00, sizeof(tmpNetworkType));
			memset(tmpBSSIDII, 0x00, sizeof(tmpBSSIDII));
			memset(tmpImg, 0x00, sizeof(tmpImg));
			memset(tmpSSIDII, 0x00, sizeof(tmpSSIDII));

			// compare BSSID with connected bssid
			if (memcmp(BssidQuery, pBssid->MacAddress, 6) == 0)
				sprintf((char *)tmpImg, "<img src=\"/graphics/handshake.gif\"> ");
			else
				sprintf((char *)tmpImg, " ");

			if (strcmp((char *)pBssid->Ssid.Ssid, "") == 0)
				sprintf((char *)tmpSSID, "<td>%s%s</td>", tmpImg, "&nbsp;");
			else {
				int i = 0;
				do {
					if (pBssid->Ssid.Ssid[i] < 32 || pBssid->Ssid.Ssid[i] > 126 || pBssid->Ssid.Ssid[i] == 13) // 13 is string end of Dos
					{
						strcpy((char *)pBssid->Ssid.Ssid, "&nbsp;");
						break;
					}
					i++;
				} while(i < pBssid->Ssid.SsidLength-1);
				strncpy((char *)tmpSSIDII, (char *)pBssid->Ssid.Ssid, pBssid->Ssid.SsidLength);
				//fprintf(stderr, "%s: sid->Ssid.Ssid=%s\n", __func__, pBssid->Ssid.Ssid);
				//fprintf(stderr, "%s: before convert, SSID=%s\n", __func__,  tmpSSIDII);
				convert_string_display((char *)tmpSSIDII);
				//fprintf(stderr, "%s: after convert, SSID=%s\n", __func__, tmpSSIDII);
				sprintf((char *)tmpSSID, "<td>%s%s</td>", tmpImg, tmpSSIDII);
			}

			sprintf((char *)tmpBSSID, "<td>%02X-%02X-%02X-%02X-%02X-%02X</td>",
				pBssid->MacAddress[0], pBssid->MacAddress[1], pBssid->MacAddress[2],
				pBssid->MacAddress[3], pBssid->MacAddress[4], pBssid->MacAddress[5]);

			sprintf((char *)tmpBSSIDII, "%02X%02X%02X%02X%02X%02X",
				pBssid->MacAddress[0], pBssid->MacAddress[1], pBssid->MacAddress[2],
				pBssid->MacAddress[3], pBssid->MacAddress[4], pBssid->MacAddress[5]);

			nSigQua = convert_rssi_signal_quality(pBssid->Rssi);
       		sprintf((char *)tmpRSSI,"<td>%d%%</td>", nSigQua);

			nChannel = -1;	
			for(j = 0; j < G_nChanFreqCount; j++)
			{
				//fprintf(stderr, "pBssid->Configuration.DSConfig = %d, ChannelFreqTable[j].lFreq=%d\n ", pBssid->Configuration.DSConfig, ChannelFreqTable[j].lFreq);
				if (pBssid->Configuration.DSConfig == ChannelFreqTable[j].lFreq) {
					nChannel = ChannelFreqTable[j].lChannel;
					break;
				}
			}

			if (nChannel == -1)
				continue;

			sprintf((char *)tmpChannel, "<td>%u</td>", nChannel);
			if (pBssid->InfrastructureMode == Ndis802_11Infrastructure)
				sprintf((char *)tmpNetworkType, "<td>%s</td>", "Infrastructure");
			else
				sprintf((char *)tmpNetworkType, "<td>%s</td>", "Ad Hoc");
			
			// work with NDIS_WLAN_BSSID_EX
			unsigned char bTKIP = FALSE;
			unsigned char bAESWRAP = FALSE;
			unsigned char bAESCCMP = FALSE;
			unsigned char bWPA = FALSE;
			unsigned char bWPAPSK = FALSE;
			unsigned char bWPANONE = FALSE;
			unsigned char bWPA2 = FALSE;
			unsigned char bWPA2PSK = FALSE;
			unsigned char bWPA2NONE = FALSE;
			unsigned char bCCKM = FALSE; // CCKM for Cisco, add by candy 2006.11.24


			if ((pBssid->Length > sizeof(NDIS_WLAN_BSSID)) && (pBssid->IELength > sizeof(NDIS_802_11_FIXED_IEs)))
			{
				unsigned int lIELoc = 0;
				PNDIS_802_11_FIXED_IEs pFixIE = (PNDIS_802_11_FIXED_IEs)pBssid->IEs;
				PNDIS_802_11_VARIABLE_IEs pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pFixIE + sizeof(NDIS_802_11_FIXED_IEs));
				lIELoc += sizeof(NDIS_802_11_FIXED_IEs);

				while (pBssid->IELength > (lIELoc + sizeof(NDIS_802_11_VARIABLE_IEs)))
				{
					if ((pVarIE->ElementID == 221) && (pVarIE->Length >= 16))
					{
						unsigned int* pOUI = (unsigned int*)((char*)pVarIE->data);
						if (*pOUI != WPA_OUI_TYPE)
						{
							lIELoc += pVarIE->Length;
							lIELoc += 2;
							pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pVarIE + pVarIE->Length + 2);

							if(pVarIE->Length <= 0)
								break;

							continue;
						}

						unsigned int* plGroupKey; 
						unsigned short* pdPairKeyCount;
						unsigned int* plPairwiseKey=NULL;
						unsigned int* plAuthenKey=NULL;
						unsigned short* pdAuthenKeyCount;
						plGroupKey = (unsigned int*)((char*)pVarIE + 8);
				
						unsigned int lGroupKey = *plGroupKey & 0x00ffffff;
						//fprintf(stderr, "lGroupKey=%d\n", lGroupKey);
						if (lGroupKey == WPA_OUI) {
							lGroupKey = (*plGroupKey & 0xff000000) >> 0x18;
							if (lGroupKey == 2)
								bTKIP = TRUE;
							else if (lGroupKey == 3)
								bAESWRAP = TRUE;
							else if (lGroupKey == 4)
								bAESCCMP = TRUE;
						}
						else
						{
							lIELoc += pVarIE->Length;
							lIELoc += 2;
							pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pVarIE + pVarIE->Length + 2);

							if(pVarIE->Length <= 0)
								break;
							
							continue;
						}
				
						pdPairKeyCount = (unsigned short*)((char*)plGroupKey + 4);
						plPairwiseKey = (unsigned int*) ((char*)pdPairKeyCount + 2);
						unsigned short k = 0;
						for (k = 0; k < *pdPairKeyCount; k++) {
							unsigned int lPairKey = *plPairwiseKey & 0x00ffffff;
							if (lPairKey == WPA_OUI )//|| (lPairKey & 0xffffff00) == WPA_OUI_1)
							{
								lPairKey = (*plPairwiseKey & 0xff000000) >> 0x18;
								if (lPairKey == 2)
									bTKIP = TRUE;
								else if (lPairKey == 3)
									bAESWRAP = TRUE;
								else if (lPairKey == 4)
									bAESCCMP = TRUE;
							}
							else
								break;
							
							plPairwiseKey++;
						}
				
						pdAuthenKeyCount = (unsigned short*)((char*)pdPairKeyCount + 2 + 4 * (*pdPairKeyCount));
						plAuthenKey = (unsigned int*)((char*)pdAuthenKeyCount + 2);

						for(k = 0; k < *pdAuthenKeyCount; k++)
						{
							unsigned int lAuthenKey = *plAuthenKey & 0x00ffffff;
							if (lAuthenKey == CISCO_OUI) {
								bCCKM = TRUE; // CCKM for Cisco
							}
							else if (lAuthenKey == WPA_OUI) {
								lAuthenKey = (*plAuthenKey & 0xff000000) >> 0x18;

								if (lAuthenKey == 1)
									bWPA = TRUE;
								else if (lAuthenKey == 0 || lAuthenKey == 2) {
									if (pBssid->InfrastructureMode)
										bWPAPSK = TRUE;
									else
										bWPANONE = TRUE;
								}
							}					
							plAuthenKey++;
						}
					//break;
					}
					else if (pVarIE->ElementID == 48 && pVarIE->Length >= 12)
					{
						unsigned int* plGroupKey; 
						unsigned int* plPairwiseKey; 
						unsigned short* pdPairKeyCount;
						unsigned int* plAuthenKey; 
						unsigned short* pdAuthenKeyCount;
						plGroupKey = (unsigned int*)((char*)pVarIE + 4);

						unsigned int lGroupKey = *plGroupKey & 0x00ffffff;
						if (lGroupKey == WPA2_OUI) {
							lGroupKey = (*plGroupKey & 0xff000000) >> 0x18;
							if (lGroupKey == 2)
								bTKIP = TRUE;
							else if (lGroupKey == 3)
								bAESWRAP = TRUE;
							else if (lGroupKey == 4)
								bAESCCMP = TRUE;
						}
						else
						{
							lIELoc += pVarIE->Length;
							lIELoc += 2;
							pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pVarIE + pVarIE->Length + 2);

							if(pVarIE->Length <= 0)
								break;
							
							continue;
						}

						pdPairKeyCount = (unsigned short*)((char*)plGroupKey + 4);
						plPairwiseKey = (unsigned int*)((char*)pdPairKeyCount + 2);
						unsigned short k = 0;

						for (k = 0; k < *pdPairKeyCount; k++)
						{
							unsigned int lPairKey = *plPairwiseKey & 0x00ffffff;
							if (lPairKey == WPA2_OUI) {
								lPairKey = (*plPairwiseKey & 0xff000000) >> 0x18;
								if (lPairKey == 2)
									bTKIP = TRUE;
								else if (lPairKey == 3)
									bAESWRAP = TRUE;
								else if (lPairKey == 4)
									bAESCCMP = TRUE;
							}
							else
								break;
							plPairwiseKey++;
						}
			
						pdAuthenKeyCount = (unsigned short*)((char*)pdPairKeyCount + 2 + 4 * *pdPairKeyCount);
						plAuthenKey = (unsigned int*)((char*)pdAuthenKeyCount + 2);
						for (k = 0; k < *pdAuthenKeyCount; k++)
						{
							unsigned int lAuthenKey = *plAuthenKey & 0x00ffffff;
							if (lAuthenKey == CISCO_OUI) {
								bCCKM = TRUE; // CCKM for Cisco
							}
	                        else if (lAuthenKey == WPA2_OUI) {
								lAuthenKey = (*plAuthenKey & 0xff000000) >> 0x18;
								if (lAuthenKey == 1)
									bWPA2 = TRUE;
								else if (lAuthenKey == 0 || lAuthenKey == 2) {
									if (pBssid->InfrastructureMode)
										bWPA2PSK = TRUE;
									else
										bWPA2NONE = TRUE;
								}
							}					
							plAuthenKey++;
						}
					}
					lIELoc += pVarIE->Length;
					lIELoc += 2;
					pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pVarIE + pVarIE->Length + 2);

					if (pVarIE->Length <= 0)
						break;
				}
			}
			
			char strAuth[32], strEncry[32];
			memset( strAuth, 0x00, sizeof(strAuth) );
			memset( strEncry, 0x00, sizeof(strEncry) );
			if (bCCKM)
				strcpy(strAuth, "CCKM; ");
			if (bWPA)
				strcpy(strAuth, "WPA; ");
			if (bWPAPSK)
				strcat(strAuth, "WPA-PSK; ");
			if (bWPANONE)
				strcat(strAuth, "WPA-NONE; ");
			if (bWPA2)
				strcat(strAuth, "WPA2; ");
			if (bWPA2PSK)
				strcat(strAuth, "WPA2-PSK; ");
			if (bWPA2NONE)
				strcat(strAuth, "WPA2-NONE; ");
			
			if (strlen(strAuth) > 0) {
				strncpy((char *)tmpAuth, strAuth, strlen(strAuth) - 2);
				strcpy(strAuth, (char *)tmpAuth);
			}
			else {
				strcpy((char *)strAuth, "Unknown");
			}

			if (bTKIP)
				strcpy(strEncry, "TKIP; ");
			if (bAESWRAP || bAESCCMP)
				strcat(strEncry, "AES; ");

			if (strlen(strEncry) > 0) {
				strncpy((char *)tmpEncry, strEncry, strlen(strEncry) - 2);
				strcpy(strEncry, (char *)tmpEncry);
			}
			else {
				if (pBssid->Privacy)  // privacy value is on/of
					strcpy(strEncry, "WEP");
				else {
					strcpy(strEncry, "Not Use");
					strcpy(strAuth, "OPEN");
				}
			}

			sprintf((char *)tmpAuth, "<td>%s</td>", strAuth);
			sprintf((char *)tmpEncry, "<td>%s</td>", strEncry);

			if (strlen(G_SSID.Ssid)>0 && strcmp(pBssid->Ssid.Ssid, G_SSID.Ssid) == 0)
				strcpy(radiocheck, "checked");
			else
				strcpy(radiocheck, "");

			sprintf((char *)tmpRadio, "<td><input type=radio name=selectedSSID %s value=\"%s\" onClick=\"selectedSSIDChange(this.value,'%s',%d,%d,'%s','%s')\"></td>", radiocheck, tmpSSIDII, tmpBSSIDII, pBssid->InfrastructureMode, nChannel, strEncry, strAuth);
			printf("<tr> %s %s %s %s %s %s %s %s </tr>\n", tmpRadio, tmpSSID, tmpBSSID, tmpRSSI, tmpChannel, tmpEncry, tmpAuth, tmpNetworkType);
			pBssid = (PNDIS_WLAN_BSSID_EX)((char *)pBssid + pBssid->Length);
		}
	}
	free(pBssidList);
	close(s);
	return 0;
}

/*
 * description: write the SSID that station connected to
 */
static int getStaConnectionSSID(char *argv[])
{
	int  ConnectStatus = NdisMediaStateDisconnected;
	NDIS_802_11_SSID  SsidQuery;
	unsigned char     BssidQuery[6];
	char              strSSID[NDIS_802_11_LENGTH_SSID + 1];
	int  s, ret;

	s = socket(AF_INET, SOCK_DGRAM, 0);

	//step 1
	if (OidQueryInformation(OID_GEN_MEDIA_CONNECT_STATUS, s, "ra0", &ConnectStatus, sizeof(ConnectStatus)) < 0) {
		printf("Query OID_GEN_MEDIA_CONNECT_STATUS error!");
		close(s);
		return -1;
	}

	//step 2
	if (OidQueryInformation(RT_OID_802_11_RADIO, s, "ra0", &G_bRadio, sizeof(G_bRadio)) < 0) {
		printf("Query RT_OID_802_11_RADIO error!");
		close(s);
		return -1;
	}

	if (ConnectStatus == NdisMediaStateConnected && G_bRadio) {
		memset(&SsidQuery, 0x00, sizeof(SsidQuery));
		OidQueryInformation(OID_802_11_SSID, s, "ra0", &SsidQuery, sizeof(SsidQuery));

		if (SsidQuery.SsidLength == 0) {
			memset(&BssidQuery, 0x00, sizeof(BssidQuery));
			ret = OidQueryInformation(OID_802_11_BSSID, s, "ra0", &BssidQuery, sizeof(BssidQuery));
			//websWrite(wp, "Connected <--> [%02X:%02X:%02X:%02X:%02X:%02X]",
			printf("Connected <--> [%02X:%02X:%02X:%02X:%02X:%02X]",
					BssidQuery[0], BssidQuery[1], BssidQuery[2],
					BssidQuery[3], BssidQuery[4], BssidQuery[5]);
		}
		else {
			memset(strSSID, 0x00, NDIS_802_11_LENGTH_SSID + 1);
			memcpy(strSSID, SsidQuery.Ssid, SsidQuery.SsidLength);
			//websWrite(wp, "Connected <--> %s", strSSID);
			printf("Connected <--> %s", strSSID);
		}
		G_ConnectStatus = NdisMediaStateConnected;
	}
	else if (G_bRadio) {
		//websWrite(wp, "Disconnected");
		printf("Disconnected");
		G_ConnectStatus = NdisMediaStateDisconnected;
	}
	else {
		G_ConnectStatus = NdisMediaStateDisconnected;
		//websWrite(wp, "Radio Off");
		printf("Radio Off");
	}
	close(s);
	return 0;
}

static int getCACLCertList(char *argv[])
{
#if defined (WPA_SUPPLICANT_SUPPORT)
	FILE *p = popen("nvram_get cert CACLCertFile", "r");
	char path[128];

	memset(path, 0, 128);
	fscanf(p, "%s", path);
	pclose(p);

	if (strlen(path) > 0)
		printf("<option value=\"%s\">%s</option>", path, path);
	else
#endif
		printf("<option value=\"\"></option>");
	
	return 0;
}

static int getKeyCertList(char *argv[])
{
#if defined (WPA_SUPPLICANT_SUPPORT)
	FILE *p = popen("nvram_get cert KeyCertFile", "r");
	char path[128];

	memset(path, 0, 128);
	fscanf(p, "%s", path);
	pclose(p);


	if (strlen(path) > 0)
		printf("<option value=\"%s\">%s</option>"), path, path;
	else
#endif
		printf("<option value=\"\"></option>");
	
	return 0;
}

/*
 * description: write station new profile name
 */
static int getStaNewProfileName(char *argv[])
{
	char profilename[32+1]; //refer to _RT_PROFILE_SETTING.
	FILE *fd = popen("nvram_get staProfile","r");
	char value[512], *tok;
	int  count;

	strcpy(profilename, "PROF001");

	if(fd == NULL)
		return -1;

	fgets(value, sizeof(value), fd);
	if(strlen(value) == 0){
		printf("%s", profilename);
		return 0;
	}

	count = 0;
	tok = strtok(value, TOKEN);
	while(tok){
		if (strncmp(tok, "PROF", 4) == 0) {
			count = strtol(tok+4, NULL, 10);
			if (++count < 1000) {
				memset(profilename, 0x00, 32);
				strncpy(profilename, "PROF", 4);
				if (count < 100) {
					strcat(profilename, "0");
					if (count < 10) {
						strcat(profilename, "0");
					}
				}
				sprintf(profilename, "%s%d", profilename, count);
			}
		}
		count++;
		tok = strtok(NULL, TOKEN);
	}

//	for (count = 0, tok = strtok(value, TOKEN) ; tok ; tok = strtok(NULL, TOKEN), count++) {
//	}


#if 0
	if (headerProfileSetting != NULL)
	{
		int count;

		currentProfileSetting = headerProfileSetting;
		do {
			if (strncmp(currentProfileSetting->Profile, "PROF", 4) == 0) {
				count = strtol(currentProfileSetting->Profile+4, NULL, 10);
				if (++count < 1000) {
					memset(profilename, 0x00, 32);
					strncpy(profilename, "PROF", 4);
					if (count < 100) {
						strcat(profilename, "0");
						if (count < 10) {
							strcat(profilename, "0");
						}
					}
					sprintf(profilename, "%s%d", profilename, count);
				}

			}
			currentProfileSetting = currentProfileSetting->Next;
		} while (currentProfileSetting != NULL );
	}
#endif

	//return websWrite(wp, "%s", profilename);
	return printf("%s", profilename);
}

/*
 * description: write station Adhoc Channel (a << 8 | bg)
 */
static int getStaAdhocChannel(char *argv[])
{
	const char *p = NULL;
	unsigned int country_region_bg = 0, country_region_a = 0;
	long country_region = 0;

	p = nvram_bufget(RT2860_NVRAM, "CountryRegion");
	if (p)
		country_region_bg = atoi(p);
	p = nvram_bufget(RT2860_NVRAM, "CountryRegionABand");
	if (p)
		country_region_a = atoi(p);

	country_region = country_region_bg | ( country_region_a << 8);
	//return websWrite(wp, "%ld", country_region);
	return printf("%ld", country_region);
}

#if 0
/*
 * description: station profile initialization
 */
int initStaProfile(void)
{
	PRT_PROFILE_SETTING nextProfileSetting;
	char tmp_buffer[512];
	const char *wordlist = NULL;
	char *tok = NULL;
	int i;
	char *value = (char *) nvram_bufget(RT2860_NVRAM, "RadioOff");
	int s = socket(AF_INET, SOCK_DGRAM, 0);

	if (strcmp((char *)value, "1") == 0) {
		G_bRadio = 0;
		OidSetInformation(RT_OID_802_11_RADIO, s, "ra0", &G_bRadio, 1);
	} else {
		G_bRadio = 1;
		OidSetInformation(RT_OID_802_11_RADIO, s, "ra0", &G_bRadio, 1);
	}
	close(s);

	// fprintf(stderr, "kathy -- init_StaProfile()\n");
	G_ConnectStatus = NdisMediaStateDisconnected;

	//staProfile
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staProfile");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("no previous profiles defined");
		return 0;
	}

	if (headerProfileSetting == NULL ) {
		headerProfileSetting = malloc(sizeof(RT_PROFILE_SETTING));
		memset(headerProfileSetting, 0x00, sizeof(RT_PROFILE_SETTING));
		headerProfileSetting->Next = NULL;
	}
	currentProfileSetting = headerProfileSetting;

	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN) ; tok ;  i++) {
		//profile
		sprintf((char *)currentProfileSetting->Profile, "%s", tok);
		// fprintf(stderr, "i=%d, Profile=%s, tok=%s\n", i,currentProfileSetting->Profile, tok);
		tok = strtok(NULL, TOKEN);

		if (tok != NULL && currentProfileSetting->Next == NULL) {
			nextProfileSetting = malloc(sizeof(RT_PROFILE_SETTING));
			memset(nextProfileSetting, 0x00, sizeof(RT_PROFILE_SETTING));
			nextProfileSetting->Next = NULL;
			currentProfileSetting->Next = nextProfileSetting;
			currentProfileSetting = nextProfileSetting;
		}
		else
			currentProfileSetting = currentProfileSetting->Next;
	}
	G_staProfileNum = i;

	// SSID
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staSSID");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta SSID has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->SSID, "%s", tok);
		currentProfileSetting->SsidLen = strlen(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// NetworkType
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staNetworkType");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta NetworkType has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->NetworkType = atoi(tok);
		// fprintf(stderr, "i=%d, NetworkType=%d\n", i,currentProfileSetting->NetworkType);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// PSMode
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staPSMode");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0)	{
		DBG_MSG("Sta PSMode has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->PSmode= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// AdhocMode
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staAdhocMode");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0)	{
		DBG_MSG("Sta AdhocMode has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->AdhocMode= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Channel
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staChannel");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0)	{
		DBG_MSG("Sta Channel has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Channel= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// PreamType
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staPreamType");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0)	{
		DBG_MSG("Sta PreamType has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->PreamType= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// RTSCheck
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staRTSCheck");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta RTSCheck has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->RTSCheck= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// FragmentCheck
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staFragmentCheck");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0)	{
		DBG_MSG("Sta FragmentCheck has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->FragmentCheck= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// RTS
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staRTS");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta RTS has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->RTS= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Fragment
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staFragment");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Fragment has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Fragment= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Auth
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staAuth");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Auth has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Authentication= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Encryption
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staEncrypt");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Encryption has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Encryption= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key1	
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey1");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key1 has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->Key1, "%s", tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key2
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey2");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key2 has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->Key2, "%s", tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key3
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey3");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key3 has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->Key3, "%s", tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key4
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey4");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key4 has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->Key4, "%s", tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key1Type
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey1Type");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key1Type has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Key1Type= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key2Type
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey2Type");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key2Type has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Key2Type= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key3Type
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey3Type");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key3Type has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Key3Type= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key4Type
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey4Type");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key4Type has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Key4Type= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key1Length
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey1Length");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key1Lenght has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Key1Length= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key2Length
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey2Length");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key2Lenght has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Key2Length= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key3Length
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey3Length");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key3Lenght has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Key3Length= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key4Length
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey4Length");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key4Length has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Key4Length= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// DefaultKeyID
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKeyDefaultId");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta DefaultKeyID has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->KeyDefaultId = atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// WPAPSK
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staWpaPsk");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta WPAPSK has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->WpaPsk, "%s", tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

#ifdef WPA_SUPPLICANT_SUPPORT
	//keymgmt
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xKeyMgmt");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta 802.1x Key Mgmt has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->KeyMgmt = atoi(tok);

		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// EAP
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xEAP");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta 802.1x EAP has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->EAP = atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	//Cert ID
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xIdentity");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta 802.1x Identity has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->Identity, "%s", tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	//Cert Password
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xPassword");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta 802.1x Password has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->Password, "%s", tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	//Cert Client Cert Path
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xClientCert");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta 802.1x Client Cert has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->ClientCert, "%s", tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	//Cert Private Key Path
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xPrivateKey");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta 802.1x Private Key has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->PrivateKey, "%s", tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	//Cert Private Key Password
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xPrivateKeyPassword");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta 802.1x Private Key Password has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->PrivateKeyPassword, "%s", tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	//Cert CA Cert
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xCACert");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta 802.1x CA Cert has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->CACert, "%s", tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	//Tunnel
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xTunnel");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta 802.1x Tunnel has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Tunnel = atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}
#endif

	//Active
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staActive");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0)	{
		DBG_MSG("Sta Active has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Active = atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}
	return 0;
}

/*
 * description: return station radio status
 */
static int getStaProfile(char *argv[])
{
	char tmpImg[40];
	int i = 0, s;

	NDIS_802_11_SSID                SsidQuery;
	unsigned int                    ConnectStatus = 0;
	NDIS_802_11_WEP_STATUS          Encryp = Ndis802_11WEPDisabled;
	NDIS_802_11_AUTHENTICATION_MODE AuthenType = Ndis802_11AuthModeOpen;
	NDIS_802_11_NETWORK_INFRASTRUCTURE      NetworkType = Ndis802_11Infrastructure;

	initStaProfile();
	if (G_staProfileNum == 0)
		return 0;
	if (headerProfileSetting == NULL)
		return 0;

	currentProfileSetting = headerProfileSetting;
	do {
		memset(tmpImg, 0x00, sizeof(tmpImg));
		// check activate function for the profile
		if (currentProfileSetting->Active)
		{
			// get connected SSID
			s = socket(AF_INET, SOCK_DGRAM, 0);

			//step 1
			if (OidQueryInformation(OID_GEN_MEDIA_CONNECT_STATUS, s, "ra0", &ConnectStatus, sizeof(ConnectStatus)) < 0) {
				DBG_MSG("Query OID_GEN_MEDIA_CONNECT_STATUS error!");
				close(s);
				return 0;
			}

			//step 2
			if (OidQueryInformation(RT_OID_802_11_RADIO, s, "ra0", &G_bRadio, sizeof(G_bRadio)) < 0) {
				DBG_MSG("Query RT_OID_802_11_RADIO error!");
				close(s);
				return 0;
			}
			if (ConnectStatus == 1 && G_bRadio)
			{
				OidQueryInformation(OID_802_11_WEP_STATUS, s, "ra0", &Encryp, sizeof(Encryp) );
				OidQueryInformation(OID_802_11_AUTHENTICATION_MODE, s, "ra0", &AuthenType, sizeof(AuthenType));
				OidQueryInformation(OID_802_11_INFRASTRUCTURE_MODE, s, "ra0", &NetworkType, sizeof(NetworkType));

				memset(&SsidQuery, 0x00, sizeof(SsidQuery));
				OidQueryInformation(OID_802_11_SSID, s, "ra0", &SsidQuery, sizeof(SsidQuery));

				int tmp_auth = currentProfileSetting->Authentication;
				if (tmp_auth == Ndis802_11AuthModeMax)
					tmp_auth = Ndis802_11AuthModeOpen;
				if (strcmp((char *)SsidQuery.Ssid, (char *)currentProfileSetting->SSID) == 0 &&
						currentProfileSetting->Encryption == Encryp &&
						tmp_auth == AuthenType &&
						currentProfileSetting->NetworkType == NetworkType)
				{
					memcpy(&G_SSID, &SsidQuery, sizeof(NDIS_802_11_SSID));
					snprintf(tmpImg, sizeof(tmpImg), "<img src=\"/graphics/checkmrk.gif\">");
				}
				else
					snprintf(tmpImg, sizeof(tmpImg), "<img src=\"/graphics/uncheckmrk.gif\">");
				close(s);
			}
			else if (G_bRadio)
			{
				int tmp_auth, tmp_encry, tmp_defaultkeyid, tmp_networktype, tmp_preamtype, tmp_channel; //tmp_adhocmode,
				char tmp_wpapsk[65], tmp_key1[27], tmp_key2[27], tmp_key3[27], tmp_key4[27], tmp_bssid[13];
				char tmp_rtscheck=0, tmp_fragmentcheck=0;
				NDIS_802_11_RTS_THRESHOLD	tmp_rts;
				NDIS_802_11_FRAGMENTATION_THRESHOLD	tmp_fragment;
				NDIS_802_11_SSID			SSID;
				NDIS_802_11_POWER_MODE		tmp_psmode;

				memset(&SSID, 0x00, sizeof(SSID));
				bzero(tmp_bssid, sizeof(tmp_bssid));
				bzero(tmp_wpapsk, sizeof(tmp_wpapsk));
				bzero(tmp_key1, sizeof(tmp_key1));
				bzero(tmp_key2, sizeof(tmp_key2));
				bzero(tmp_key3, sizeof(tmp_key3));
				bzero(tmp_key4, sizeof(tmp_key4));
				memset(tmp_wpapsk, 0x00, sizeof(tmp_wpapsk));

				SSID.SsidLength = currentProfileSetting->SsidLen;
				memcpy(SSID.Ssid, (const void *)currentProfileSetting->SSID, currentProfileSetting->SsidLen);

				tmp_networktype = currentProfileSetting->NetworkType;
				tmp_auth  = currentProfileSetting->Authentication;
				tmp_encry = currentProfileSetting->Encryption;
				tmp_preamtype = currentProfileSetting->PreamType;
				tmp_rts = currentProfileSetting->RTS;
				tmp_rtscheck = currentProfileSetting->RTSCheck;
				tmp_fragment = currentProfileSetting->Fragment;
				tmp_fragmentcheck = currentProfileSetting->FragmentCheck;
				tmp_psmode = currentProfileSetting->PSmode;
				tmp_channel = currentProfileSetting->Channel;
				tmp_defaultkeyid = currentProfileSetting->KeyDefaultId;

				//strncpy(tmp_wpapsk, selectedProfileSetting->WpaPsk, 63);
				sprintf((char *)tmp_wpapsk, "%s", currentProfileSetting->WpaPsk);
				strcpy(tmp_key1, (char *)currentProfileSetting->Key1);
				strcpy(tmp_key2, (char *)currentProfileSetting->Key2);
				strcpy(tmp_key3, (char *)currentProfileSetting->Key3);
				strcpy(tmp_key4, (char *)currentProfileSetting->Key4);

				unsigned char Bssid[6];
#ifdef WPA_SUPPLICANT_SUPPORT
				if (currentProfileSetting->Authentication == Ndis802_11AuthModeWPA ||
						currentProfileSetting->Authentication == Ndis802_11AuthModeWPA2 ||
						currentProfileSetting->Authentication == Ndis802_11AuthModeMax )//802.1x
				{
					char tmp_key[27];
					if (tmp_defaultkeyid == 1) // 1~4
						strcpy(tmp_key, tmp_key1);
					else if (tmp_defaultkeyid == 2)
						strcpy(tmp_key, tmp_key2);
					else if (tmp_defaultkeyid == 3)
						strcpy(tmp_key, tmp_key3);
					else if (tmp_defaultkeyid == 4)
						strcpy(tmp_key, tmp_key4);

					conf_WPASupplicant((char *)currentProfileSetting->SSID, currentProfileSetting->KeyMgmt, currentProfileSetting->EAP, (char *)currentProfileSetting->Identity, (char *)currentProfileSetting->Password, (char *)currentProfileSetting->CACert, (char *)currentProfileSetting->ClientCert, (char *)currentProfileSetting->PrivateKey, (char *)currentProfileSetting->PrivateKeyPassword, tmp_key, currentProfileSetting->KeyDefaultId-1, currentProfileSetting->Encryption, currentProfileSetting->Tunnel, currentProfileSetting->Authentication);
				}
				else
#endif
					sta_connection(tmp_networktype, tmp_auth, tmp_encry, tmp_defaultkeyid, &SSID, Bssid, tmp_wpapsk, tmp_key1, tmp_key2, tmp_key3, tmp_key4, tmp_preamtype, tmp_rtscheck, tmp_rts, tmp_fragmentcheck, tmp_fragment, tmp_psmode, tmp_channel);

				/*NDIS_802_11_SSID SSID;
				  memset(&SSID, 0x00, sizeof(SSID));
				  strcpy((char *)SSID.Ssid ,(char *)currentProfileSetting->SSID);
				  SSID.SsidLength = strlen((char *)currentProfileSetting->SSID);
				  OidSetInformation(OID_802_11_SSID, s, "ra0", &SSID, sizeof(NDIS_802_11_SSID));*/
				snprintf(tmpImg, sizeof(tmpImg), "<img src=\"/graphics/uncheckmrk.gif\">");
			}
			else
				snprintf(tmpImg, sizeof(tmpImg), "<img src=\"/graphics/uncheckmrk.gif\">");
			close(s);
		}

		//web_debug_header(); //this function is called in html bt lighty SSI
		printf("<tr>");

		// Radio
		printf("<td><input type=radio name=selectedProfile value=%d onClick=\"selectedProfileChange()\">%s</td>",
				i+1, tmpImg);

		// Profile 
		printf("<td>%s</td>", currentProfileSetting->Profile);
		printf("<td>%s</td>", currentProfileSetting->SSID);

		// Channel
		if (currentProfileSetting->Channel <= 0)
			printf("<td>%s</td>", "Auto");
		else
			printf("<td>%d</td>", currentProfileSetting->Channel);

		// Auth
		if (currentProfileSetting->Authentication == Ndis802_11AuthModeOpen)
			printf("<td>%s</td>","OPEN");
		else if (currentProfileSetting->Authentication == Ndis802_11AuthModeShared)
			printf("<td>%s</td>", "SHARED");
		else if (currentProfileSetting->Authentication == Ndis802_11AuthModeWPAPSK)
			printf("<td>%s</td>", "WPA-PSK");
		else if (currentProfileSetting->Authentication == Ndis802_11AuthModeWPA2PSK)
			printf("<td>%s</td>", "WPA2-PSK");
		else if (currentProfileSetting->Authentication == Ndis802_11AuthModeWPANone)
			printf("<td>%s</td>", "WPA-NONE");
		else if (currentProfileSetting->Authentication == Ndis802_11AuthModeWPA)
			printf("<td>%s</td>", "WPA");
		else if (currentProfileSetting->Authentication == Ndis802_11AuthModeWPA2)
			printf("<td>%s</td>", "WPA2");
		else if (currentProfileSetting->Authentication == Ndis802_11AuthModeMax) //802.1x
			printf("<td>%s</td>", "OPEN");
		else
			printf("<td>%s</td>", "unknown");

		// Encryption
		if (currentProfileSetting->Encryption == Ndis802_11WEPEnabled)
			printf("<td>%s</td>", "WEP");
		else if (currentProfileSetting->Encryption == Ndis802_11WEPDisabled)
			printf("<td>%s</td>", "NONE");
		else if (currentProfileSetting->Encryption == Ndis802_11Encryption2Enabled)
			printf("<td>%s</td>", "TKIP");
		else if (currentProfileSetting->Encryption == Ndis802_11Encryption3Enabled)
			printf("<td>%s</td>", "AES");
		else
			printf("<td>%s</td>", "unknown");

		// NetworkType
		if (currentProfileSetting->NetworkType == Ndis802_11Infrastructure)
			printf("<td>%s</td>", "Infrastructure");
		else
			printf("<td>%s</td>", "Ad Hoc");

		printf("</tr>\n");
		currentProfileSetting = currentProfileSetting->Next;
		i++;
	} while (currentProfileSetting != NULL );
	return 0;
}
#endif
#endif

/*
 * WiFi Query
 */
void GetWifiVersion(char *argv[])
{
	unsigned char DriverVersionInfo[16];
	int s;
	char wif[5];

	memset(DriverVersionInfo, 0, 16);
	if (!strcmp(argv[1], WLAN2_CONF))
		strcpy(wif, "rai0");
	else
		strcpy(wif, "ra0");
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (ap_oid_query_info(RT_OID_VERSION_INFO, s, wif, &DriverVersionInfo, sizeof(DriverVersionInfo)) >= 0)
		printf("%s", DriverVersionInfo);
	else
		printf("&nbsp;");
	close(s);
}

/*
 *  * description: write 802.11a channels in <select> tag
 */
void GetAChannels(char *argv[])
{
	int  nvram_id, idx = 0, channel;
	const char *value, *channel_s;

	if ((nvram_id = getNvramIndex(argv[1])) == -1) {
		DBG_MSG("%s: Error: \"%s\" flash zone not existed", argv[0], argv[1]);
		get_usage(argv[0]);
		return;
	}
	nvram_init(nvram_id);
	value = nvram_bufget(nvram_id,"CountryRegionABand");
	channel_s = nvram_bufget(nvram_id, "Channel");

	channel = (channel_s == NULL)? 0 : strtoul(channel_s, NULL, 10);
#if ! defined CONFIG_EXT_CHANNEL_LIST
	if ((value == NULL) || (strcmp(value, "") == 0) ||
			(strcmp(value, "7") == 0)) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~140 */
		for (idx = 16; idx < 27; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~173 */
		for (idx = 28; idx < 35; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "0") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "1") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~140 */
		for (idx = 16; idx < 27; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
	} else if (strcmp(value, "2") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
	} else if (strcmp(value, "3") == 0) {
		/* 52~64 */
		for (idx = 4; idx < 8; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "4") == 0) {
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "5") == 0) {
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "6") == 0) {
		/* 36~48 */
		for (idx = 0; idx < 4; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
	} else if (strcmp(value, "8") == 0) {
		/* 52~64 */
		for (idx = 4; idx < 8; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
	} else if (strcmp(value, "9") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~116 */
		for (idx = 16; idx < 21; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 132~140 */
		for (idx = 24; idx < 27; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "10") == 0) {
		/* 36~48 */
		for (idx = 0; idx < 4; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "11") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~120 */
		for (idx = 16; idx < 22; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "12") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~140 */
		for (idx = 16; idx < 27; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
	} else if (strcmp(value, "13") == 0) {
		/* 52~64 */
		for (idx = 4; idx < 8; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~140 */
		for (idx = 16; idx < 27; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "14") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~116 */
		for (idx = 16; idx < 21; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 136~140 */
		for (idx = 25; idx < 27; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "15") == 0) {
		/* 149~173 */
		for (idx = 28; idx < 35; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "16") == 0) {
		/* 52~64 */
		for (idx = 4; idx < 8; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "17") == 0) {
		/* 36~48 */
		for (idx = 0; idx < 4; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "18") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~116 */
		for (idx = 16; idx < 21; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 132~140 */
		for (idx = 24; idx < 27; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
	} else if (strcmp(value, "19") == 0) {
		/* 56~64 */
		for (idx = 5; idx < 8; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~140 */
		for (idx = 16; idx < 27; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "20") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~124 */
		for (idx = 16; idx < 23; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "21") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~140 */
		for (idx = 16; idx < 27; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	}
#else
	/* 36~64 */
	for (idx = 0; idx < 8; idx++)
		printf("%s%d %s>%d%s%d%s", "<option value=", 36+4*idx,
				(36+4*idx == channel)? "selected" : "", 5180+20*idx,
				"MHz (Channel ", 36+4*idx, ")</option>");
	/* 100~140 */
	for (idx = 16; idx < 27; idx++)
		printf("%s%d %s>%d%s%d%s", "<option value=", 36+4*idx,
				(36+4*idx == channel)? "selected" : "", 5180+20*idx,
				"MHz (Channel ", 36+4*idx, ")</option>");
	/* 149~173 */
	for (idx = 28; idx < 35; idx++)
		printf("%s%d %s>%d%s%d%s", "<option value=",
				36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
				5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
#endif
	nvram_close(nvram_id);
}

void GetGChannels(char *argv[])
{
	int  nvram_id, idx = 0, channel;
	const char *value, *channel_s;

	if ((nvram_id = getNvramIndex(argv[1])) == -1) {
		DBG_MSG("%s: Error: \"%s\" flash zone not existed", argv[0], argv[1]);
		get_usage(argv[0]);
		return;
	}
	nvram_init(nvram_id);
	value = nvram_bufget(nvram_id,"CountryRegion");
	channel_s = nvram_bufget(nvram_id, "Channel");

	channel = (channel_s == NULL)? 0 : strtoul(channel_s, NULL, 10);
#if ! defined CONFIG_EXT_CHANNEL_LIST
	if ((value == NULL) || (strcmp(value, "") == 0) ||
			(strcmp(value, "5") == 0)) {
		/* ch1 ~ ch14 */
		for (idx = 0; idx < 14; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "0") == 0) {
		/* ch1 ~ ch11 */
		for (idx = 0; idx < 11; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "1") == 0) {
		/* ch1 ~ ch13 */
		for (idx = 0; idx < 13; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "2") == 0) {
		/* ch10 ~ ch11 */
		for (idx = 9; idx < 11; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "3") == 0) {
		/* ch10 ~ ch13 */
		for (idx = 9; idx < 13; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "4") == 0) {
		/* ch14 */
		printf("<option value=14 %s>2484MHz (Channel 14)</option>\n", 
				(14 == channel)? "selected" : "");
	} else if (strcmp(value, "6") == 0) {
		/* ch3 ~ ch9 */
		for (idx = 2; idx < 9; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "7") == 0) {
		/* ch5 ~ ch13 */
		for (idx = 4; idx < 13; idx++)
			printf("%s%d %s>%d%s%d%s", 
					"<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "31") == 0) {
		/* ch1 ~ ch11 */
		for (idx = 0; idx < 11; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
#if 0
		/* ch12 ~ ch14 */
		for (idx = 11; idx < 14; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
#endif
	} else if (strcmp(value, "32") == 0) {
		/* ch1 ~ ch11 */
		for (idx = 0; idx < 11; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
#if 0
		/* ch12 ~ ch13 */
		for (idx = 11; idx < 13; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
#endif
	} else if (strcmp(value, "33") == 0) {
		/* ch1 ~ ch14 */
		for (idx = 0; idx < 14; idx++)
			printf("%s%d %s>%d%s%d%s", "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	}
#else
	/* ch1 ~ ch14 */
	for (idx = 0; idx < 14; idx++)
		printf("%s%d %s>%d%s%d%s", "<option value=", idx+1,
				(idx+1 == channel)? "selected" : "", 2412+5*idx,
				"MHz (Channel ", idx+1, ")</option>");
#endif
	nvram_close(nvram_id);
}

void StaInfo(char *argv[])
{
	int i, s;
	struct iwreq iwr;
	RT_802_11_MAC_TABLE table = {0};
#if defined (RT2860_TXBF_SUPPORT) || defined (RTDEV_TXBF_SUPPORT)
	char tmpBuff[32];
	char *phyMode[4] = {"CCK", "OFDM", "MM", "GF"};
#endif
	char ifname[5];

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (!strcmp(argv[1], WLAN2_CONF))
		strncpy(iwr.ifr_name, "rai0", IFNAMSIZ);
	else
		strncpy(iwr.ifr_name, "ra0", IFNAMSIZ);
	iwr.u.data.pointer = (caddr_t) &table;

	if (s < 0) {
		DBG_MSG("open socket fail!");
		return;
	}

	if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT, &iwr) < 0) {
		DBG_MSG("ioctl -> RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT fail!");
		close(s);
		return;
	}

	close(s);
#if defined (RT2860_TXBF_SUPPORT)
	if (!strcmp(argv[1], WLAN1_CONF))
		goto advance;
#endif
#if defined (RT2860_TXBF_SUPPORT)
	if (!strcmp(argv[1], WLAN2_CONF))
		goto advance;
#endif
#if ! defined (RT2860_TXBF_SUPPORT) || ! defined (RTDEV_TXBF_SUPPORT)
	for (i = 0; i < table.Num; i++) {
		printf("<tr><td>%02X:%02X:%02X:%02X:%02X:%02X</td>",
				table.Entry[i].Addr[0], table.Entry[i].Addr[1],
				table.Entry[i].Addr[2], table.Entry[i].Addr[3],
				table.Entry[i].Addr[4], table.Entry[i].Addr[5]);
		printf("<td>%d</td><td>%d</td><td>%d</td>",
				table.Entry[i].Aid, table.Entry[i].Psm, table.Entry[i].MimoPs);
		printf("<td>%d</td><td>%s</td><td>%d</td><td>%d</td></tr>",
				table.Entry[i].TxRate.field.MCS,
				(table.Entry[i].TxRate.field.BW == 0)? "20M":"40M",
				table.Entry[i].TxRate.field.ShortGI, table.Entry[i].TxRate.field.STBC);
	}
	return;
#endif
#if defined (RT2860_TXBF_SUPPORT) || defined (RTDEV_TXBF_SUPPORT)
advance:
	for (i = 0; i < table.Num; i++) {
		RT_802_11_MAC_ENTRY *pe = &(table.Entry[i]);
		unsigned int lastRxRate = pe->LastRxRate;
		unsigned int mcs = pe->LastRxRate & 0x7F;
		int hr, min, sec;

		hr = pe->ConnectedTime/3600;
		min = (pe->ConnectedTime % 3600)/60;
		sec = pe->ConnectedTime - hr*3600 - min*60;

		// MAC Address
		printf("<tr><td>%02X:%02X:%02X:<br>%02X:%02X:%02X</td>",
				pe->Addr[0], pe->Addr[1], pe->Addr[2], pe->Addr[3],
				pe->Addr[4], pe->Addr[5]);

		// AID, Power Save mode, MIMO Power Save
		printf("<td align=\"center\">%d</td><td align=\"center\">%d</td><td align=\"center\">%d</td>",
				pe->Aid, pe->Psm, pe->MimoPs);

		// TX Rate
		printf("<td>MCS %d<br>%2dM, %cGI<br>%s%s</td>",
				pe->TxRate.field.MCS,
				pe->TxRate.field.BW? 40: 20,
				pe->TxRate.field.ShortGI? 'S': 'L',
				phyMode[pe->TxRate.field.MODE],
				pe->TxRate.field.STBC? ", STBC": " ");

		// TxBF configuration
		printf("<td align=\"center\">%c %c</td>",
				pe->TxRate.field.iTxBF? 'I': '-',
				pe->TxRate.field.eTxBF? 'E': '-');

		// RSSI
		printf("<td align=\"center\">%d<br>%d<br>%d</td>",
				(int)(pe->AvgRssi0), (int)(pe->AvgRssi1), (int)(pe->AvgRssi2));

		// Per Stream SNR
		snprintf(tmpBuff, sizeof(tmpBuff), "%0.1f", pe->StreamSnr[0]*0.25);
		printf("<td>%s<br>", tmpBuff);
		snprintf(tmpBuff, sizeof(tmpBuff), "%0.1f", pe->StreamSnr[1]*0.25); //mcs>7? pe->StreamSnr[1]*0.25: 0.0);
		printf("%s<br>", tmpBuff);
		snprintf(tmpBuff, sizeof(tmpBuff), "%0.1f", pe->StreamSnr[2]*0.25); //mcs>15? pe->StreamSnr[2]*0.25: 0.0);
		printf("%s</td>", tmpBuff);

		// Sounding Response SNR
		if (pe->TxRate.field.eTxBF) {
			snprintf(tmpBuff, sizeof(tmpBuff), "%0.1f", pe->SoundingRespSnr[0]*0.25);
			printf("<td>%s<br>", tmpBuff);
			snprintf(tmpBuff, sizeof(tmpBuff), "%0.1f", pe->SoundingRespSnr[1]*0.25);
			printf("%s<br>", tmpBuff);
			snprintf(tmpBuff, sizeof(tmpBuff), "%0.1f", pe->SoundingRespSnr[2]*0.25);
			printf("%s</td>", tmpBuff);
		}
		else {
			printf("<td align=\"center\">-<br>-<br>-</td>");
		}

		// Last RX Rate
		printf("<td>MCS %d<br>%2dM, %cGI<br>%s%s</td>",
				mcs,  ((lastRxRate>>7) & 0x1)? 40: 20,
				((lastRxRate>>8) & 0x1)? 'S': 'L',
				phyMode[(lastRxRate>>14) & 0x3],
				((lastRxRate>>9) & 0x3)? ", STBC": " ");

		// Connect time
		//printf("<td align=\"center\">%02d:%02d:%02d<br>PER=%d%%</td>", hr, min, sec, pe->TxPER);
		printf("<td align=\"center\">%02d:%02d:%02d</td>", hr, min, sec);
		printf("</tr>");
	}
#endif
}

void ApStats(char *argv[])
{
	NDIS_802_11_STATISTICS stat;
	int s, ret;
	float txCount;
#ifdef ENHANCED_AP_STATUS_INFO
	char  tmpStatistics[256];
#endif

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (!strcmp(argv[1], WLAN2_CONF))
		ret = ap_oid_query_info(OID_802_11_STATISTICS, s, "rai0", &stat, sizeof(stat));
	else
		ret = ap_oid_query_info(OID_802_11_STATISTICS, s, "ra0", &stat, sizeof(stat));
	close(s);
	if (ret >= 0) {
		txCount = stat.TransmittedFragmentCount.QuadPart;

		if (!strncmp(argv[4], "TxSucc", 7)) {
			printf("%lld", stat.TransmittedFragmentCount.QuadPart);
#ifdef ENHANCED_AP_STATUS_INFO
		} else if (!strncmp(argv[4], "TxRetry", 8)) {
			sprintf(tmpStatistics, "%lld,  PER=%0.1f%%", stat.RetryCount.QuadPart,
					txCount==0? 0.0: 100.0f*(stat.RetryCount.QuadPart+stat.FailedCount.QuadPart)/(stat.RetryCount.QuadPart+stat.FailedCount.QuadPart+txCount));
			printf("%s", tmpStatistics);
		} else if (!strncmp(argv[4], "TxFail", 7)) {
			sprintf(tmpStatistics, "%lld,  PLR=%0.1e", stat.FailedCount.QuadPart,
					txCount==0? 0.0: (float)stat.FailedCount.QuadPart/(stat.FailedCount.QuadPart+txCount));
			printf("%s", tmpStatistics);
#else
		} else if (!strncmp(argv[4], "TxRetry", 8)) {
			printf("%lld", stat.RetryCount.QuadPart);
		} else if (!strncmp(argv[4], "TxFail", 7)) {
			printf("%lld", stat.FailedCount.QuadPart);
#endif
		} else if (!strncmp(argv[4], "RTSSucc", 8)) {
			printf("%lld", stat.RTSSuccessCount.QuadPart);
		} else if (!strncmp(argv[4], "RTSFail", 8)) {
			printf("%lld", stat.RTSFailureCount.QuadPart);
		} else if (!strncmp(argv[4], "RxSucc", 7)) {
			printf("%lld", stat.ReceivedFragmentCount.QuadPart);
		} else if (!strncmp(argv[4], "FCSError", 9)) {
#ifdef ENHANCED_AP_STATUS_INFO
			sprintf(tmpStatistics, "%lld, PER=%0.1f%%", stat.FCSErrorCount.QuadPart,
					stat.ReceivedFragmentCount.QuadPart==0?
					0.0: 100.0f*stat.FCSErrorCount.QuadPart/(stat.FCSErrorCount.QuadPart+stat.ReceivedFragmentCount.QuadPart));
			printf("%s", tmpStatistics);
#else
			printf("%lld", stat.FCSErrorCount.QuadPart);
#endif
		} else {
			printf("type not supported");
		}
	} else {
		printf("ioctl %d", ret);
	}
}

void ApSNR(char *argv[])
{
	int s, n, ret;
	unsigned long SNR;
	//char wif[5];
	char wif[16];
	if (!strcmp(argv[1], WLAN2_CONF))
		strncpy(wif, "rai0", IFNAMSIZ);
	else
		strncpy(wif, "ra0", IFNAMSIZ);
	n = strtoul(argv[4], NULL, 10);
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (n == 0)
		ret = ap_oid_query_info(RT_OID_802_11_SNR_0, s, wif, &SNR, sizeof(SNR));
	else if (n == 1)
		ret = ap_oid_query_info(RT_OID_802_11_SNR_1, s, wif, &SNR, sizeof(SNR));
	else if (n == 2)
		ret = ap_oid_query_info(RT_OID_802_11_SNR_2, s, wif, &SNR, sizeof(SNR));
	else
		ret = -1;
	close(s);

	if (ret < 0) {
		printf("n/a");
	} else {
		printf("%ld", SNR);
	}

}

#if defined (RT2860_TXBF_SUPPORT) || defined (RTDEV_TXBF_SUPPORT)
void ApTxBfStats(char *argv[])
{
	int s, ret, i;
	RT_802_11_TXBF_TABLE table = {0};
	RT_COUNTER_TXBF *pCnt;
	int hdr = 0;
	char  tmpStatistics[256];
	unsigned long totalNBF, totalEBF, totalIBF, totalTx, totalRetry, totalSuccess;

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (!strcmp(argv[1], WLAN2_CONF))
		ret = ap_oid_query_info(RT_OID_802_11_QUERY_TXBF_TABLE, s, "rai0", &table, sizeof(table));
	else
		ret = ap_oid_query_info(RT_OID_802_11_QUERY_TXBF_TABLE, s, "ra0", &table, sizeof(table));
	close(s);
	if (ret < 0) {
		DBG_MSG("ioctl error %d", ret);
		return;
	}

	for (i = 0; i < table.Num; i++) {
		pCnt = &(table.Entry[i]);
		totalNBF = pCnt->TxSuccessCount + pCnt->TxFailCount;
		totalEBF = pCnt->ETxSuccessCount + pCnt->ETxFailCount;
		totalIBF = pCnt->ITxSuccessCount + pCnt->ITxFailCount;
		totalTx = totalNBF + totalEBF + totalIBF;
		totalRetry = pCnt->TxRetryCount + pCnt->ETxRetryCount + pCnt->ITxRetryCount;
		totalSuccess = pCnt->TxSuccessCount + pCnt->ETxSuccessCount + pCnt->ITxSuccessCount;

		if (totalTx == 0)
			continue;
		if (hdr == 0) {
			printf("<tr><td class=\"title\" colspan=\"2\">Detailed TX Statistics (Retry count is approximate)</td></tr>");
			hdr = 1;
		}

		printf("<tr><td class=\"head2\">%d</td><td><kbd>", i);
		strcpy(tmpStatistics, "              Success    Retry/PER    Fail/PLR");
		convert_string_display(tmpStatistics);
		printf("%s<br>", tmpStatistics);
		if (totalNBF!=0) {
			sprintf(tmpStatistics, "NonBF (%3lu%%): %7lu  %7lu (%2lu%%) %5lu (%1lu%%)",
					100*totalNBF/totalTx, pCnt->TxSuccessCount,
					pCnt->TxRetryCount, 100*pCnt->TxRetryCount/(pCnt->TxSuccessCount+pCnt->TxRetryCount),
					pCnt->TxFailCount, 100*pCnt->TxFailCount/totalNBF);
			convert_string_display(tmpStatistics);
			printf("%s<<br>", tmpStatistics);
		}
		if (totalEBF!=0) {
			sprintf(tmpStatistics, "ETxBF (%3lu%%): %7lu  %7lu (%2lu%%) %5lu (%1lu%%)",
					100*totalEBF/totalTx, pCnt->ETxSuccessCount,
					pCnt->ETxRetryCount, 100*pCnt->ETxRetryCount/(pCnt->ETxSuccessCount+pCnt->ETxRetryCount),
					pCnt->ETxFailCount, 100*pCnt->ETxFailCount/totalEBF);
			convert_string_display(tmpStatistics);
			printf("%s<br>", tmpStatistics);
		}

		if (totalIBF!=0) {
			sprintf(tmpStatistics, "ITxBF (%3lu%%): %7lu  %7lu (%2lu%%) %5lu (%1lu%%)",
					100*totalIBF/totalTx, pCnt->ITxSuccessCount,
					pCnt->ITxRetryCount, 100*pCnt->ITxRetryCount/(pCnt->ITxSuccessCount+pCnt->ITxRetryCount),
					pCnt->ITxFailCount, 100*pCnt->ITxFailCount/totalIBF);
			convert_string_display(tmpStatistics);
			printf("%s<br>", tmpStatistics);
		}

		sprintf(tmpStatistics, "Total         %7lu  %7lu (%2lu%%) %5lu (%1lu%%)",
				totalSuccess, totalRetry, 100*totalRetry/(totalSuccess + totalRetry),
				pCnt->TxFailCount+pCnt->ETxFailCount+pCnt->ITxFailCount,
				100*(pCnt->TxFailCount+pCnt->ETxFailCount+pCnt->ITxFailCount)/totalTx);
		convert_string_display(tmpStatistics);
		printf("%s", tmpStatistics);
		printf("</kbd></td></tr>");
	}
}
#endif

#if defined (RT2860_APCLI_SUPPORT)
void ApcliScan(char *argv[])
{
	FILE *pp;
	char cmd[CMDLEN], *ptr, wif[5];
	char channel[4], ssid[186], bssid[20], security[23];
	char signal[9], mode[9], ext_ch[7], net_type[3];
	char wps[4];
#if defined (FIRST_MT7615_SUPPORT) || defined (SECOND_MT7615_SUPPORT)
//	char no[4];
	int  no = 0; 
	int  channeld = 0;
	int  signald = 0;
#endif
	int i, space_start;

	if (!strcmp(argv[1], WLAN2_CONF)){
		strcpy(wif, "rai0");
	} else {
		if(!strcmp(argv[4], "0"))
			strcpy(wif, "apcli0");
		else if(!strcmp(argv[4], "1"))
			strcpy(wif, "apcli1");
		else
			strcpy(wif, "ra0");
	}
	do_system("iwpriv %s set SiteSurvey=1", wif);
	
	sleep(5); // for get the SCAN result. (2G + 5G may enough for 5 seconds)
	
	sprintf(cmd, "iwpriv %s get_site_survey", wif);
	if(!(pp = popen(cmd, "r"))) {
		DBG_MSG("execute get_site_survey fail!");
		return;
	}

	memset(cmd, 0, sizeof(cmd));
#if defined (FIRST_MT7615_SUPPORT) || defined (SECOND_MT7615_SUPPORT)
	fgets(cmd, sizeof(cmd), pp);
	fgets(cmd, sizeof(cmd), pp);
	fgets(cmd, sizeof(cmd), pp);
	//TODO: Get more than 1 page of site survey result.
	while (fgets(cmd, sizeof(cmd), pp)) {
		if (strlen(cmd) < 4)
			break;
		ptr = cmd;
		sscanf(ptr, "%d %d ", &no, &channeld);
		ptr += 41;
		sscanf(ptr, "%20s %23s %d %9s %7s %3s %4s", bssid, security, &signald, mode, ext_ch, net_type, wps);
		ptr = cmd+8;
		i = 0;
		while (i < 33) {
			if ((ptr[i] == 0x20) && (i == 0 || ptr[i-1] != 0x20))
				space_start = i;
			i++;
		}
		ptr[space_start] = '\0';
		strcpy(ssid, cmd+8);
		convert_string_display(ssid);

		printf("<tr>\n");
		printf("<td>%d</td>\n", no);
		printf("<td>%d</td><td>%s</td>\n", channeld, ssid);
		printf("<td>%s</td><td>%s</td>\n", bssid, security);
		printf("<td>%d</td><td>%s</td>\n", signald, mode);
		printf("<td>%s</td><td>%s</td>\n", ext_ch, net_type);
		printf("<td>%s</td>\n", wps);
		printf("</tr>\n");
	}
#else
	fgets(cmd, sizeof(cmd), pp);
	fgets(cmd, sizeof(cmd), pp);
	int index = 0;
	while (fgets(cmd, sizeof(cmd), pp)) {
		if (strlen(cmd) < 4)
			break;
		ptr = cmd;
		sscanf(ptr, "%s ", channel);
		ptr += 37;
		sscanf(ptr, "%s %s %s %s %s %s %s", bssid, security, signal, mode, ext_ch, net_type, wps);
		ptr = cmd+4;
		i = 0;
		while (i < 33) {
			if ((ptr[i] == 0x20) && (i == 0 || ptr[i-1] != 0x20))
				space_start = i;
			i++;
		}
		ptr[space_start] = '\0';
		strcpy(ssid, cmd+4);
		convert_string_display(ssid);

		printf("<tr>\n");
		printf("<td>%d</td>\n", index++);
		printf("<td>%s</td><td>%s</td>\n", channel, ssid);
		printf("<td>%s</td><td>%s</td>\n", bssid, security);
		printf("<td>%s</td><td>%s</td>\n", signal, mode);
		printf("<td>%s</td><td>%s</td>\n", ext_ch, net_type);
		printf("<td>%s</td>\n", wps);
		printf("</tr>\n");
	}
#endif
	pclose(pp);
}
#endif

void GetPINCode(char *argv[])
{
	int socket_id;
	struct iwreq wrq;
	unsigned int data = 0;

	//DBG_MSG();
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	//DBG_MSG("socket id = %d", socket_id);
	if (!strcmp(argv[1], WLAN2_CONF)) {
		sprintf(wrq.ifr_name, "rai0");
	} else {
		sprintf(wrq.ifr_name, "ra0");
	}
	//DBG_MSG("if: %s",wrq.ifr_name);
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = (caddr_t) &data;
	wrq.u.data.flags = RT_OID_WSC_PIN_CODE;
	//DBG_MSG();
	if( ioctl(socket_id, RT_PRIV_IOCTL, &wrq) == -1)
		DBG_MSG("ioctl error");
	close(socket_id);
	//DBG_MSG();
	printf("%08d", data);
}

void WebWifiGet(char *argv[])
{
	if (!strcmp(argv[3], "version")) {
		GetWifiVersion(argv);
	} else if (!strcmp(argv[3], "getAChannels")) {
		GetAChannels(argv);
	} else if (!strcmp(argv[3], "getGChannels")) {
		GetGChannels(argv);
	} else if (!strcmp(argv[3], "wpsPINCode")) {
		GetPINCode(argv);
	} else if (!strcmp(argv[3], "staInfo")) {
		StaInfo(argv);
    } else if (!strcmp(argv[3], "getCentralFreq")) {
		getCentralFreq(argv);
	} else if (!strcmp(argv[3], "apStats")) {
		ApStats(argv);
	} else if (!strcmp(argv[3], "apSNR")) {
		ApSNR(argv);
#if defined (RT2860_TXBF_SUPPORT)
	} else if (!strcmp(argv[3], "apTxBfStats") && !strcmp(argv[1], WLAN1_CONF)) {
		ApTxBfStats(argv);
#endif
#if defined (RTDEV_TXBF_SUPPORT)
	} else if (!strcmp(argv[3], "apTxBfStats") && !strcmp(argv[1], WLAN2_CONF)) {
		ApTxBfStats(argv);
#endif
	} else if (!strcmp(argv[3], "maxStream") && !strcmp(argv[1], WLAN1_CONF)) {
#if defined (RT2860_4T4R_SUPPORT)
		printf("%d", 4);
#elif defined (RT2860_3T3R_SUPPORT)
		printf("%d", 3);
#elif defined (RT2860_1T1R_SUPPORT)
		printf("%d", 1);
#else
		printf("%d", 2);
#endif
	} else if (!strcmp(argv[3], "maxStream") && !strcmp(argv[1], WLAN2_CONF)) {
#if defined (RTDEV_4T4R_SUPPORT)
		printf("%d", 4);
#elif defined (RTDEV_3T3R_SUPPORT)
		printf("%d", 3);
#elif defined (RTDEV_1T1R_SUPPORT)
		printf("%d", 1);
#else
		printf("%d", 2);
#endif
	} else if (!strcmp(argv[3], "maxBssNum") && !strcmp(argv[1], WLAN1_CONF)) {
#if defined (CONFIG_NEW_MBSSID_MODE) || defined (RT2860_16BSSID_SUPPORT)
		printf("%d", 16);
#elif defined (RT2860_MBSS_SUPPORT)
		printf("%d", 8);
#else
		printf("%d", 1);
#endif
	} else if (!strcmp(argv[3], "maxBssNum") && !strcmp(argv[1], WLAN2_CONF)) {
#if defined (CONFIG_NEW_MBSSID_MODE) || defined (RTDEV_16BSSID_SUPPORT)
		printf("%d", 16);
#elif defined (RT2860_MBSS_SUPPORT)
		printf("%d", 8);
#else
		printf("%d", 1);
#endif
#if defined (RT2860_APCLI_SUPPORT)
	} else if (!strcmp(argv[3], "ApcliScan")) {
		ApcliScan(argv);
#endif
#if defined (RT2860_STA_SUPPORT)
	} else if (!strcmp(argv[3], "StaDriverVer")) {
		GetStaDriverVer(argv);
	} else if (!strcmp(argv[3], "StaMac")) {
		GetStaMacAddr(argv);
	} else if (!strcmp(argv[3], "StaWirelessMode")) {
		GetStaWirelessMode(argv);			
	} else if (!strcmp(argv[3], "StaSuppAMode")) {
		GetStaSuppAMode(argv);
	} else if (!strcmp(argv[3], "StaRadioStatus")) {
		GetStaRadioStatus(argv);	
	} else if (!strcmp(argv[3], "StaLinkingMode")) {
		GetLinkingMode(argv);						
	} else if (!strcmp(argv[3], "StaLinkStatus")) {
		GetStaLinkStatus(argv);					
	} else if (!strcmp(argv[3], "StaExtraInfo")) {
		GetStaExtraInfo(argv);	
	} else if (!strcmp(argv[3], "StaLinkChannel")) {
		GetStaLinkChannel(argv);			
	} else if (!strcmp(argv[3], "StaLinkTxRate")) {
		GetStaLinkTxRate(argv);			
	} else if (!strcmp(argv[3], "StaLinkRxRate")) {
		GetStaLinkRxRate(argv);			
	} else if (!strcmp(argv[3], "StaTxThroughput")) {
		GetStaTxThroughput(argv);				
	} else if (!strcmp(argv[3], "StaRxThroughput")) {
		GetStaRxThroughput(argv);	
	} else if (!strcmp(argv[3], "StaLinkQuality")) {
		GetStaLinkQuality(argv);				
	} else if (!strcmp(argv[3], "SignalStrength")) {
		GetStaSignalStrength(argv);			
	} else if (!strcmp(argv[3], "SignalStrength_1")) {
		GetStaSignalStrength_1(argv);			
	} else if (!strcmp(argv[3], "SignalStrength_2")) {
		GetStaSignalStrength_2(argv);		
	} else if (!strcmp(argv[3], "StaNoiseLevel")) {
		GetStaNoiseLevel(argv);	
	} else if (!strcmp(argv[3], "StaHT")) {
		GetStaHT(argv);	
	} else if (!strcmp(argv[3], "StaSNR0")) {
		GetStaSNR0(argv);				
	} else if (!strcmp(argv[3], "StaSNR1")) {
		GetStaSNR1(argv);					
	} else if (!strcmp(argv[3], "StaSNR2")) {
		GetStaSNR2(argv);		
	} else if (!strcmp(argv[3], "StaStreamSNR")) {
		GetStaStreamSNR(argv);				
	} else if (!strcmp(argv[3], "StaDLSList")) {
		GetStaDLSList(argv);		
	} else if (!strcmp(argv[3], "StaStatsTx")) {
		GetStaStatsTx(argv);			
	} else if (!strcmp(argv[3], "StaStatsRxCRCErr")) {
		GetStaStatsRxCRCErr(argv);		
	} else if (!strcmp(argv[3], "StaStatsRxNoBuf")) {
		GetStaStatsRxNoBuf(argv);	
	} else if (!strcmp(argv[3], "StaStatsRxDup")) {
		GetStaStatsRxDup(argv);	
	} else if (!strcmp(argv[3], "StaStatsTxBf")) {
		GetStaStatsTxBf(argv);			
	} else if (!strcmp(argv[3], "getStaConnectionSSID")){
		getStaConnectionSSID(argv);
	} else if (!strcmp(argv[3], "getStaBSSIDList")){
		getStaBSSIDList(argv);
	} else if (!strcmp(argv[3], "getCACLCertList")){
		getCACLCertList(argv);
	} else if (!strcmp(argv[3], "getKeyCertList")){
		getKeyCertList(argv);
	} else if (!strcmp(argv[3], "getStaNewProfileName")){
		getStaNewProfileName(argv);
	} else if (!strcmp(argv[3], "getStaAdhocChannel")){
		getStaAdhocChannel(argv);
#endif
#if defined (CONFIG_RT2860V2_STA_WSC)

	} else if (!strcmp(argv[3], "WPSSTAModeASP")) {
		GetWPSSTAModeASP(argv);
	} else if (!strcmp(argv[3], "WPSBSSIDListASP")) {
		GetStaWPSBSSIDListASP(argv);
	} else if (!strcmp(argv[3], "WPSSTAPINCodeASP")) {
		GetWPSSTAPINCodeASP(argv);	
	} else if (!strcmp(argv[3], "WPSSTARegSSIDASP")) {
		GetWPSSTARegSSIDASP(argv);	
	} else if (!strcmp(argv[3], "WPSSTARegAuthASP")) {
		GetWPSSTARegAuthASP(argv);				
	} else if (!strcmp(argv[3], "WPSSTARegKeyTypeASP")) {
		GetWPSSTARegKeyTypeASP(argv);							
	} else if (!strcmp(argv[3], "WPSSTARegKeyIndexASP")) {
		GetWPSSTARegKeyIndexASP(argv);		
#endif
	}
}

void GetGateway(void)
{
	char   buff[256];
	int    nl = 0 ;
	struct in_addr dest;
	struct in_addr gw;
	int    flgs, ref, use, metric;
	unsigned long int d,g,m;
	int    find_default_flag = 0;
	char sgw[16];
	FILE *fp = fopen("/proc/net/route", "r");
	if (fp == NULL) fp = "";
	while (fgets(buff, sizeof(buff), fp) != NULL) {
		if (nl) {
			int ifl = 0;
			while (buff[ifl]!=' ' && buff[ifl]!='\t' && buff[ifl]!='\0')
				ifl++;
			buff[ifl]=0;    /* interface */
			if (sscanf(buff+ifl+1, "%lx%lx%X%d%d%d%lx",
						&d, &g, &flgs, &ref, &use, &metric, &m)!=7) {
				fclose(fp);
				DBG_MSG("format error");
				return;
			}

			if (flgs&RTF_UP) {
				dest.s_addr = d;
				gw.s_addr   = g;
				strcpy(sgw, (gw.s_addr==0 ? "" : inet_ntoa(gw)));

				if (dest.s_addr == 0) {
					find_default_flag = 1;
					break;
				}
			}
		}
		nl++;
	}
	fclose(fp);

	if (find_default_flag == 1)
		printf("%s", sgw);
}

void GetDns(int index)
{
	FILE *fp;
	char buf[80] = {0}, ns_str[11], dns[16] = {0};
	int type, i = 0;

	fp = fopen("/etc/resolv.conf", "r");
	if (NULL == fp) 
		return;
	while (fgets(buf, sizeof(buf), fp) != NULL) {
		if (strncmp(buf, "nameserver", 10) != 0)
			continue;
		sscanf(buf, "%11s%16s", ns_str, dns);
		i++;
		if (i == index)
			break;
	}
	fclose(fp);

	printf("%s", dns);
}

/*
 * description: write DHCP client list
 */
#define BYOD_SUPPORT //by chhung
void GetDhcpClientList(void)
{
	FILE *fp;
	struct dhcpOfferedAddr {
		unsigned char hostname[16];
		unsigned char mac[16];
		unsigned long ip;
		unsigned long expires;
	} lease;
	int i;
	struct in_addr addr;
	unsigned long expires;
	unsigned d, h, m;
	char tmpValue[256];

	do_system("killall -q -USR1 udhcpd");
	fp = fopen("/var/udhcpd.leases", "r");
	if (NULL == fp)
		return;
	while (fread(&lease, 1, sizeof(lease), fp) == sizeof(lease)) {
		if (strlen(lease.hostname) > 0) {
			sprintf(tmpValue, "%s", lease.hostname);
			convert_string_display(tmpValue);			
			printf("<tr><td>%-16s</td>", tmpValue);
		} else {
			printf("<tr><td><br /></td>");
		}
		printf("<td>%02X", lease.mac[0]);
		for (i = 1; i < 6; i++)
			printf(":%02X", lease.mac[i]);
		addr.s_addr = lease.ip;
		expires = ntohl(lease.expires);
		printf("</td><td>%s</td><td>", inet_ntoa(addr));
		d = expires / (24*60*60); expires %= (24*60*60);
		h = expires / (60*60); expires %= (60*60);
		m = expires / 60; expires %= 60;
		if (d) printf("%u days ", d);
		printf("%02u:%02u:%02u</td>", h, m, (unsigned)expires);
#ifdef BYOD_SUPPORT
		FILE *pp;
		char cmd[128], msg_os[64];
		sprintf(cmd, "fingerprint.sh query %s", inet_ntoa(addr));
		pp = popen(cmd, "r");
		if(pp == NULL) pp = "";
		memset(msg_os, 0, 64);
		fread(msg_os, 64, 1, pp);
		pclose(pp);
		printf("<td>%s</td>", msg_os);
#endif
		printf("</tr>");
	}
	fclose(fp);
}

char *getLanWanNamebyIf(char *ifname)
{
	const char *mode = nvram_bufget(RT2860_NVRAM, "OperationMode");

	if (NULL == mode)
		return "Unknown";

	if (!strcmp(mode, "0")){    // bridge mode
		if(!strcmp(ifname, "br0"))
			return "LAN";
		return ifname;
	}

	if (!strcmp(mode, "1")) {   // gateway mode
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW
		if(!strcmp(ifname, "br0"))
			return "LAN";
#if defined (CONFIG_RAETH_SPECIAL_TAG)
#if defined (CONFIG_WAN_AT_P0)
		if(!strcmp(ifname, "eth2.1") || !strcmp(ifname, "ppp0"))
#else
			if(!strcmp(ifname, "eth2.5") || !strcmp(ifname, "ppp0"))
#endif
#else
				if(!strcmp(ifname, "eth2.2") || !strcmp(ifname, "ppp0"))
#endif
					return "WAN";
		return ifname;
#elif defined  CONFIG_ICPLUS_PHY && CONFIG_RT2860V2_AP_MBSS
		char *num_s = nvram_bufget(RT2860_NVRAM, "BssidNum");
		if(strtol(num_s, NULL, 10) > 1 && !strcmp(ifname, "br0") )  // multiple ssid
			return "LAN";
		if(strtol(num_s, NULL, 10) == 1 && !strcmp(ifname, "ra0"))
			return "LAN";
		if (!strcmp(ifname, "eth2") || !strcmp(ifname, "ppp0"))
			return "WAN";
		return ifname;
#elif defined (CONFIG_GE1_RGMII_AN) && defined (CONFIG_GE2_RGMII_AN)
		if(!strcmp(ifname, "br0"))
			return "LAN";
		if(!strcmp(ifname, "eth3"))
			return "WAN";
#else
		if(!strcmp(ifname, "ra0"))
			return "LAN";
		return ifname;
#endif

	}else if (!strncmp(mode, "2", 2)) { // ethernet convertor
		if(!strcmp("eth2", ifname))
			return "LAN";
		if(!strcmp("ra0", ifname))
			return "WAN";
		return ifname;
	}else if (!strncmp(mode, "3", 2)) { // apcli mode
		if(!strcmp("br0", ifname))
			return "LAN";
		if(!strcmp("apcli0", ifname))
			return "WAN";
		return ifname;
	}

	return ifname;
}

void GetRoutingTable(void)
{
	char   result[4096] = {0};
	char   buff[512];
	int    nl = 0, index;
	char   ifname[32], interface[128];
	struct in_addr dest, gw, netmask;
	char   dest_str[32], gw_str[32], netmask_str[32], comment[32];
	int    flgs, ref, use, metric;
	int    *running_rules = NULL;
	unsigned long int d,g,m;
	char *rrs;
	int  rule_count;
	FILE *fp = fopen("/proc/net/route", "r");
	if(!fp)
		return;

	rrs = (char *) nvram_bufget(RT2860_NVRAM, "RoutingRules");
	rule_count = get_nums(rrs, ';');
	if(rule_count){
		running_rules = calloc(1, sizeof(int) * rule_count);
		if(!running_rules) {
			fclose(fp);
			return;
		}
	}

	while (fgets(buff, sizeof(buff), fp) != NULL) {
		if (nl) {
			if (sscanf(buff, "%32s%lx%lx%X%d%d%d%lx",
						ifname, &d, &g, &flgs, &ref, &use, &metric, &m) != 8) {
				DBG_MSG("format error");
				free(running_rules);
				fclose(fp);
				printf("");
				return;
			}
			dest.s_addr = d;
			gw.s_addr = g;
			netmask.s_addr = m;

			if(! (flgs & 0x1) ) // skip not usable
				continue;

			strncpy(dest_str, inet_ntoa(dest), sizeof(dest_str));
			strncpy(gw_str, inet_ntoa(gw), sizeof(gw_str));
			strncpy(netmask_str, inet_ntoa(netmask), sizeof(netmask_str));

			if(nl > 1){
				//strncat(result, ";", sizeof(result));
				snprintf(result, sizeof(result), "%s%s", result, ";");
			}
			//strncat(result, ifname, sizeof(result));
			snprintf(result, sizeof(result), "%s%s", result, ifname);       
			//strncat(result, ",", sizeof(result));
			snprintf(result, sizeof(result), "%s%s", result, ","); 
			//strncat(result, dest_str, sizeof(result));
			snprintf(result, sizeof(result), "%s%s", result, dest_str);       
			//strncat(result, ",", sizeof(result));
			snprintf(result, sizeof(result), "%s%s", result, ",");     
			//strncat(result, gw_str, sizeof(result));
			snprintf(result, sizeof(result), "%s%s", result, gw_str);              
			//strncat(result, ",", sizeof(result));
			snprintf(result, sizeof(result), "%s%s", result, ",");
			//strncat(result, netmask_str, sizeof(result) );
			snprintf(result, sizeof(result), "%s%s", result, netmask_str);  
			//strncat(result, ",", sizeof(result));
			snprintf(result, sizeof(result), "%s%s", result, ","); 
			snprintf(result, sizeof(result), "%s%d,%d,%d,%d,", result, flgs, ref, use, metric);

			// check if internal routing rules
			strcpy(comment, " ");
			if( (index=get_index_routingrule(rrs, dest_str, netmask_str, ifname)) != -1){
				char one_rule[256];

				if(index < rule_count)
					running_rules[index] = 1;
				else
					DBG_MSG("fatal error");

				snprintf(result, sizeof(result), "%s%d,", result, index);
				if(rrs && strlen(rrs)){
					if( get_nth_value(index, (char *)rrs, ';', one_rule, sizeof(one_rule)) != -1){

						if( get_nth_value(3, one_rule, ',', interface, sizeof(interface)) != -1){
							strncat(result, interface, sizeof(result));
							strncat(result, ",", sizeof(result));
						}
						if( get_nth_value(6, one_rule, ',', comment, sizeof(comment)) != -1){
							// do nothing;
						}
					}
				}
			}else{
				strncat(result, "-1,", sizeof(result));
				//strncat(result, getLanWanNamebyIf(ifname), sizeof(result));
				snprintf(result, sizeof(result), "%s%s", result, getLanWanNamebyIf(ifname)); 
				//strncat(result, ",", sizeof(result));
				snprintf(result, sizeof(result), "%s%s", result, ","); 
			}
			//strncat(result, "0,", sizeof(result));  // used rule
			snprintf(result, sizeof(result), "%s%s", result, "0,"); 
			strncat(result, comment, sizeof(result));
		}
		nl++;
	}

	for(index=0; index < rule_count; index++){
		char one_rule[256];

		if(running_rules[index])
			continue;

		if(get_nth_value(index, (char *)rrs, ';', one_rule, sizeof(one_rule)) == -1)
			continue;

		if(get_nth_value(0, one_rule, ',', dest_str, sizeof(dest_str)) == -1)
			continue;

		if(get_nth_value(1, one_rule, ',', netmask_str, sizeof(netmask_str)) == -1)
			continue;

		if(get_nth_value(2, one_rule, ',', gw_str, sizeof(gw_str)) == -1)
			continue;

		if(get_nth_value(3, one_rule, ',', interface, sizeof(interface)) == -1)
			continue;

		if(get_nth_value(4, one_rule, ',', ifname, sizeof(ifname)) == -1)
			continue;

		if(get_nth_value(6, one_rule, ',', comment, sizeof(comment)) == -1)
			continue;

		if(strlen(result))
			strncat(result, ";", sizeof(result));

		snprintf(result, sizeof(result), "%s%s,%s,%s,%s,0,0,0,0,%d,%s,1,%s", result, ifname, dest_str, gw_str, netmask_str, index, interface, comment);
	}

	printf("%s", result);
	fclose(fp);
	if(running_rules)
		free(running_rules);
}

void GetCurrentTime(void)
{
	char buf[64];
	FILE *pp = popen("date", "r");
	char *temp;

	if (!pp) {
		printf("none");
		return;
	}
	fgets(buf, 64, pp);
	pclose(pp);
	temp =  strchr(buf, '\n');
	if(temp == NULL) temp = "";
	*temp = '\0';

	printf("%s", buf);
}

void GetMemTotal(void)
{
	char buf[1024], *semiColon, *key, *value;
	FILE *fp = fopen(PROC_MEM_STATISTIC, "r");

	if(!fp){
		DBG_MSG("no proc?");
		return;
	}

	while(fgets(buf, 1024, fp)){
		if(! (semiColon = strchr(buf, ':'))  )
			continue;
		*semiColon = '\0';
		key = buf;
		value = semiColon + 1;
		if(!strcmp(key, "MemTotal")){
			value = strip_space(value);
			printf("%s", value);
			break;
		}
	}
	fclose(fp);
}

void GetMemFree(void)
{
	char buf[1024], *semiColon, *key, *value;
	FILE *fp = fopen(PROC_MEM_STATISTIC, "r");

	if(!fp){
		DBG_MSG("no proc?");
		return;
	}

	while(fgets(buf, 1024, fp)){
		if(! (semiColon = strchr(buf, ':'))  )
			continue;
		*semiColon = '\0';
		key = buf;
		value = semiColon + 1;
		if(!strcmp(key, "MemFree")){
			value = strip_space(value);
			printf("%s", value);
			break;
		}
	}
	fclose(fp);
}

static long long getIfStatistic(char *interface, int type)
{
	int found_flag = 0;
	int skip_line = 2;
	char buf[1024], *field, *semiColon = NULL;
	FILE *fp = fopen(PROC_IF_STATISTIC, "r");
	if(!fp){
		DBG_MSG("no proc?");
		return -1;
	}

	while(fgets(buf, 1024, fp)){
		char *ifname;
		if(skip_line != 0){
			skip_line--;
			continue;
		}
		if(! (semiColon = strchr(buf, ':'))  )
			continue;
		*semiColon = '\0';
		ifname = buf;
		ifname = strip_space(ifname);

		if(!strcmp(ifname, interface)){
			found_flag = 1;
			break;
		}
	}
	fclose(fp);

	semiColon++;

	switch(type){
	case TXBYTE:
		if(  (field = get_field(semiColon, " ", 8))  ){
			return strtoll(field, NULL, 10);
		}
		break;
	case TXPACKET:
		if(  (field = get_field(semiColon, " ", 9))  ){
			return strtoll(field, NULL, 10);
		}
		break;
	case RXBYTE:
		if(  (field = get_field(semiColon, " ", 0))  ){
			return strtoll(field, NULL, 10);
		}
		break;
	case RXPACKET:
		if(  (field = get_field(semiColon, " ", 1))  ){
			return strtoll(field, NULL, 10);
		}
		break;
	}

	return -1;
}

void GetAllNICStatisticASP(void)
{
	char result[1024];
	char buf[1024];
	int rc = 0, pos = 0, skip_line = 2;
	int first_time_flag = 1;
	FILE *fp = fopen(PROC_IF_STATISTIC, "r");
	if (!fp) {
		DBG_MSG("no proc?");
		return;
	}

	while(fgets(buf, 1024, fp)){
		char *ifname, *semiColon;
		long long rxpkt, rxbyte, txpkt, txbyte;
		if(skip_line != 0){
			skip_line--;
			continue;
		}
		if(! (semiColon = strchr(buf, ':'))  )
			continue;
		*semiColon = '\0';

		ifname = buf;
		ifname = strip_space(ifname);

		rxpkt = getIfStatistic(ifname, RXPACKET);
		rxbyte = getIfStatistic(ifname, RXBYTE);
		txpkt = getIfStatistic(ifname, TXPACKET);
		txbyte = getIfStatistic(ifname, TXBYTE);
		if (rxpkt <= 0 && rxbyte <= 0 && txpkt <= 0 && txbyte <= 0)
			continue;
		/* try to get statistics data */
		if(getIfStatistic(ifname, RXPACKET) >= 0){
			/* a success try */
			if(first_time_flag){
				pos = snprintf(result+rc, 1024-rc, "\"%s\"", ifname);
				rc += pos;
				first_time_flag = 0;
			}else{
				pos = snprintf(result+rc, 1024-rc, ",\"%s\"", ifname);
				rc += pos;
			}

		}else   /* failed and just skip */
			continue;

		pos = snprintf(result+rc, 1024-rc, ",\"%lld\"", rxpkt);
		rc += pos;
		pos = snprintf(result+rc, 1024-rc, ",\"%lld\"", rxbyte);
		rc += pos;
		pos = snprintf(result+rc, 1024-rc, ",\"%lld\"", txpkt);
		rc += pos;
		pos = snprintf(result+rc, 1024-rc, ",\"%lld\"", txbyte);
		rc += pos;
	}
	fclose(fp);

	printf("%s", result);
}


void GetPortStatus(void)
{
#if (defined (CONFIG_RAETH_ROUTER) || defined CONFIG_RT_3052_ESW) && defined (CONFIG_USER_ETHTOOL)
	int port, rc;
	FILE *pp;
	char buf[1024];

	for(port=0; port<5; port++){
		char *pos;
		char link = '0';
		int speed = 100;
		char duplex = 'F';
		FILE *proc_file = fopen("/proc/rt2880/gmac", "w");
		if(!proc_file){
			DBG_MSG("open /proc/rt2880/gmac fail!");     // indicate error
			return;
		}
		fprintf(proc_file, "%d", port);
		fclose(proc_file);

		if((pp = popen("ethtool eth2", "r")) == NULL){
			DBG_MSG("open ethtool command fail!");     // indicate error
			return;
		}
		rc = fread(buf, 1, 1024, pp);
		pclose(pp);
		if(rc == -1){
			DBG_MSG("read fail!");     // indicate error
			return;
		}else{
			// get Link status
			if((pos = strstr(buf, "Link detected: ")) != NULL){
				pos += strlen("Link detected: ");
				if(*pos == 'y')
					link = '1';
			}
			// get speed
			if((pos = strstr(buf, "Speed: ")) != NULL){
				pos += strlen("Speed: ");
				if(*pos == '1' && *(pos+1) == '0' && *(pos+2) == 'M')
					speed = 10;
				if(*pos == '1' && *(pos+1) == '0' && *(pos+2) == '0' && *(pos+3) == '0' && *(pos+4) == 'M')
					speed = 1000;
			}
			// get duplex
			if((pos = strstr(buf, "Duplex: ")) != NULL){
				pos += strlen("Duplex: ");
				if(*pos == 'H')
					duplex = 'H';
			}

			printf("%c,%d,%c,", link, speed, duplex);
		}
	}
#else
	DBG_MSG("not support!");
#endif
}

void GetSdkVersion(void)
{
	FILE *fp = fopen("/etc_ro/web/cgi-bin/History", "r");
	char version[16] = "";

	if (fp != NULL)
	{
		char buf[30];
		while (fgets(buf, sizeof(buf), fp) != NULL) {
			if (strncasecmp(buf, "Version", 7) != 0)
				continue;
			sscanf(buf, "%*s%16s", version);
			break;
		}
		fclose(fp);
	}
	printf("%s", version);
}

void GetUptime(void)
{
	struct tm *utime;
	time_t usecs;

	time(&usecs);
	utime = localtime(&usecs);
	if(utime == NULL) utime = "";
	if (utime->tm_hour > 0)
		printf("%d hour%s, %d min%s, %d sec%s",
			utime->tm_hour, (utime->tm_hour == 1)? "" : "s",
			utime->tm_min, (utime->tm_min == 1)? "" : "s",
			utime->tm_sec, (utime->tm_sec == 1)? "" : "s");
	else if (utime->tm_min > 0)
		printf("%d min%s, %d sec%s",
			utime->tm_min, (utime->tm_min == 1)? "" : "s",
			utime->tm_sec, (utime->tm_sec == 1)? "" : "s");
	else
		printf("%d sec%s",
			utime->tm_sec, (utime->tm_sec == 1)? "" : "s");
}

void GetPlatform(void)
{
	char platform[9];
#if defined CONFIG_RALINK_RT2883
	strcpy(platform, "RT2883");
#elif defined CONFIG_RALINK_RT3052
	strcpy(platform, "RT3052");
#elif defined CONFIG_RALINK_RT3883
	strcpy(platform, "RT3883");
#elif defined CONFIG_RALINK_RT3352
	strcpy(platform, "RT3352");
#elif defined CONFIG_RALINK_RT5350
	strcpy(platform, "RT5350");
#elif defined CONFIG_RALINK_RT6855
	strcpy(platform, "RT6855");
#elif defined CONFIG_RALINK_RT6855A
	strcpy(platform, "RT6855/6");
#elif defined CONFIG_RALINK_MT7620
	strcpy(platform, "MT7620");
#elif defined CONFIG_RALINK_MT7621
	strcpy(platform, "MT7621");
#else
	strcpy(platform, "RT2880");
#endif

#ifdef CONFIG_RAETH_ROUTER
	printf("%s with IC+ MACPHY", platform);
#endif
#ifdef CONFIG_ICPLUS_PHY
	printf("%s with IC+ PHY", platform);
#endif
#ifdef CONFIG_RT_MARVELL
	printf("%s with MARVELL", platform);
#endif
#ifdef CONFIG_MAC_TO_MAC_MODE
	printf("%s with Vitesse", platform);
#endif
#ifdef CONFIG_RT_3052_ESW
	printf("%s embedded switch", platform);
#endif
#if defined (CONFIG_GE1_RGMII_AN) && defined (CONFIG_GE2_RGMII_AN)
	printf("%s with Two Giga PHY", platform);
#endif
}

void GetCmdShow(void)
{
	FILE *fp;
	char buf[1024];

	fp = fopen(SYSTEM_COMMAND_LOG, "r");
	if (!fp) {
		DBG_MSG("opne fail!");
		return;
	}

	while(fgets(buf, 1024, fp)){
		printf("%s", buf);
	}
	fclose(fp);
}

void GetSysLog(void)
{
	FILE *pp = NULL;
	char *log;

	pp = popen("logread", "r");
	if (!pp) {
		DBG_MSG("open logread command fail!");
		printf("-1");
		goto error;
	}

	log = malloc(LOG_MAX * sizeof(char));
	if (!log) {
		DBG_MSG("memory allocate fail!");
		printf("-1");
		goto error;
	}
	memset(log, 0, LOG_MAX);
	fread(log, 1, LOG_MAX - 1, pp);
	*(log + LOG_MAX - 1) = '\0';
	printf("%s", log);

	free(log);
error:
	if(pp)
		pclose(pp);
}

#if defined CONFIG_USER_STORAGE
int GetDiskDir(int show)
{
	FILE *fp_mount = fopen(MOUNT_INFO, "r");
	char part[30], path[50];
	//char dir_name[30];
	int dir_len = 0;
	int dir_count = 0;

	if (NULL == fp_mount) {
		DBG_MSG("opne %s fail!", MOUNT_INFO);
		goto error;
	}

	while(EOF != fscanf(fp_mount, "%30s %50s %*s %*s %*s %*s\n", part, path))
	{
		DIR *dp;
		struct dirent *dirp;
		struct stat statbuf;

		if (0 != strncmp(path, "/media/", 7))
		{
			continue;
		}
		if (NULL == (dp = opendir(path)))
		{
			DBG_MSG("open %s error\n", path);
			goto error;
		}
		chdir(path);
		while(NULL != (dirp = readdir(dp)))
		{
			lstat(dirp->d_name, &statbuf);
			if(S_ISDIR(statbuf.st_mode))
			{
				//if (0 == strncmp(dirp->d_name, ".", 1))
				if ((0 == strncmp(dirp->d_name, ".", 1)) ||
				    (strlen(dirp->d_name) <= 0))
					continue;
#if 0
				strcpy(dir_name, dirp->d_name);
				dir_len = strlen(dirp->d_name);
				if (dir_len <= 0)
					continue;
#endif
				if (show) {
					printf("<tr>");
					printf("<td><input type=\"radio\" name=\"dir_path\" value=\"%s/%s\"></td>", 
						path, dirp->d_name);
					printf("<td>%s/%s</td>", path, dirp->d_name);
					printf("<td>%s</td>", part);
					printf("</tr>");
				}
				dir_count++;
				/*
				if (dir_len < 30 && dir_len > 0)
				{
					if (show) {
						printf("<tr>");
						printf("<td>%s/%s</td>", path, dirp->d_name);
						printf("<td>%s</td>", part);
						printf("</tr>");
					} else {
						dir_count++;
					}
				}
				*/
			}
		}
		chdir("/");
		closedir(dp);
	}
error:
	if(fp_mount)
		fclose(fp_mount);

	return dir_count;
}

int GetDiskPartition(int show)
{
	FILE *fp = fopen(MOUNT_INFO, "r");
	char part[30], path[50];
	int part_count = 0;

	if (NULL == fp) {
		DBG_MSG("fopen %s fail!", MOUNT_INFO);
		goto error;
	}

	while(EOF != fscanf(fp, "%30s %50s %*s %*s %*s %*s\n", part, path))
	{
		// if (strncmp(path, "/var", 4) != 0)
		if (0 != strncmp(path, "/media/", 7))
			continue;
		if (show)
		{
			printf("<tr align=center>");
			printf("<td><input type=\"radio\" name=\"disk_part\" value=\"%s\"></td>",
					path);
			printf("<td>%s</td>", part);
			printf("<td>%s</td>", path);
			printf("</tr>");
		}
		part_count++;
	}
error:
	if (fp)
		fclose(fp);

	return part_count;
}

#if defined CONFIG_USER_STORAGE && defined CONFIG_USER_MINIDLNA
int GetMediaDir(int show)
{
	char path[50];
	int index, media_dir_count = 0;
	struct media_config cfg[4];

	memset(cfg, 0, sizeof(cfg));
	fetch_media_cfg(cfg, 0);
	for(index=0;index<4;index++)
	{
		if (0 != strlen(cfg[index].path))
		{
			if (show)
			{
				printf("<tr align=\"center\">");
				printf("<td><input type=\"radio\" name=\"media_dir\" value=\"%d\"></td>",
						index);
				printf("<td>%s</td>", cfg[index].path);
				printf("</tr>");
			}
			media_dir_count++;
		}
	}

	return media_dir_count;
}
#endif

void GetCounter(char *field)
{
	if (0 == strcmp(field, "n_AllDir")) {
		printf("%d", GetDiskDir(0));
		// fprintf(stderr,"AllDir: %s\n", count);
	} else if (0 == strcmp(field, "n_AllPart")) {
		printf("%d", GetDiskPartition(0));
		// fprintf(stderr,"AllPart: %s\n", count);
#if defined CONFIG_USER_STORAGE && defined CONFIG_USER_MINIDLNA
	} else if (0 == strcmp(field, "n_AllMediaDir")) {
		printf("%d", GetMediaDir(0));
		// fprintf(stderr,"AllPart: %s\n", count);
#endif
	}
}
#endif

void WebSysGet(char *argv[])
{
	char *tmp;
	if (!strcmp(argv[3], "wif2_live") && !strcmp(argv[1], "rtdev")) {
		printf("%d", get_if_live("rai0"));
	} else if (!strcmp(argv[3], "onePortOnly")) {
#if defined (CONFIG_ICPLUS_PHY)
		printf("true");
#else
		printf("false");
#endif
	} else if (!strcmp(argv[3], "wanIpAddr")) {
		printf("%s", get_ipaddr(get_wanif_name()));
	} else if (!strcmp(argv[3], "wanNetmask")) {
		printf("%s", get_netmask(get_wanif_name()));
	} else if (!strcmp(argv[3], "wanGateway")) {
		GetGateway();
	} else if (!strcmp(argv[3], "dns1")) {
		GetDns(1);
	} else if (!strcmp(argv[3], "dns2")) {
		GetDns(2);
	} else if (!strcmp(argv[3], "lanIpAddr")) {
		printf("%s", get_ipaddr(get_lanif_name()));
	} else if (!strcmp(argv[3], "lanNetmask")) {
		printf("%s", get_netmask(get_lanif_name()));
	} else if (!strcmp(argv[3], "dhcpClientList")) {
		GetDhcpClientList();
	} else if (!strcmp(argv[3], "routingtable")) {
		GetRoutingTable();
	} else if (!strcmp(argv[3], "lanMacAddr")) {
		nvram_init(RT2860_NVRAM);
		tmp = get_macaddr(get_lanif_name());
		if(tmp == NULL) tmp = "";
		printf("%s", tmp);
		nvram_close(RT2860_NVRAM);
	} else if (!strcmp(argv[3], "wifiMacAddr")) {
		if (!strcmp(argv[1], WLAN2_CONF)){
			tmp = get_macaddr("rai0");
			if(tmp == NULL) tmp = "";
			printf("%s", tmp);
		}else{
			tmp = get_macaddr("ra0");
			if(tmp == NULL) tmp = "";
			printf("%s", tmp);
		}
	} else if (!strcmp(argv[3], "wanMacAddr")) {
		nvram_init(RT2860_NVRAM);
		tmp = get_macaddr(get_wanif_name());
		if(tmp == NULL) tmp = "";
		printf("%s", tmp);
		nvram_close(RT2860_NVRAM);
	} else if (!strcmp(argv[3], "currentTime")) {
		GetCurrentTime();
	} else if (!strcmp(argv[3], "memTotal")) {
		GetMemTotal();
	} else if (!strcmp(argv[3], "memFree")) {
		GetMemFree();
	} else if (!strcmp(argv[3], "wanRxPkt")) {
		long long data = getIfStatistic(get_wanif_name(), RXPACKET);
		printf("%lld", data);
	} else if (!strcmp(argv[3], "wanRxByte")) {
		long long data = getIfStatistic(get_wanif_name(), RXBYTE);
		printf("%lld", data);
	} else if (!strcmp(argv[3], "wanTxPkt")) {
		long long data = getIfStatistic(get_wanif_name(), TXPACKET);
		printf("%lld", data);
	} else if (!strcmp(argv[3], "wanTxByte")) {
		long long data = getIfStatistic(get_wanif_name(), TXBYTE);
		printf("%lld", data);
	} else if (!strcmp(argv[3], "lanRxPkt")) {
		long long data = getIfStatistic(get_lanif_name(), RXPACKET);
		printf("%lld", data);
	} else if (!strcmp(argv[3], "lanRxByte")) {
		long long data = getIfStatistic(get_lanif_name(), RXBYTE);
		printf("%lld", data);
	} else if (!strcmp(argv[3], "lanTxPkt")) {
		long long data = getIfStatistic(get_lanif_name(), TXPACKET);
		printf("%lld", data);
	} else if (!strcmp(argv[3], "lanTxByte")) {
		long long data = getIfStatistic(get_lanif_name(), TXBYTE);
		printf("%lld", data);
	} else if (!strcmp(argv[3], "allStatistic")) {
		GetAllNICStatisticASP();
	} else if (!strcmp(argv[3], "portStatus")) {
		GetPortStatus();
	} else if (!strcmp(argv[3], "sdkVersion")) {
		GetSdkVersion();
	} else if (!strcmp(argv[3], "buildTime")) {
		printf("%s", __DATE__);
	} else if (!strcmp(argv[3], "uptime")) {
		GetUptime();
	} else if (!strcmp(argv[3], "platform")) {
		GetPlatform();
	} else if (!strcmp(argv[3], "cmdShow")) {
		GetCmdShow();
	} else if (!strcmp(argv[3], "syslog")) {
		GetSysLog();
#if defined CONFIG_USER_STORAGE
	} else if (!strcmp(argv[3], "DiskDir")) {
		GetDiskDir(1);
	} else if (!strcmp(argv[3], "DiskPartition")) {
		GetDiskPartition(1);
#if defined CONFIG_USER_STORAGE && defined CONFIG_USER_MINIDLNA
	} else if (!strcmp(argv[3], "MediaDir")) {
		GetMediaDir(1);
#endif
	} else if (!strcmp(argv[3], "n_AllDir") || 
		   !strcmp(argv[3], "n_AllPart") || 
		   !strcmp(argv[3], "n_AllMediaDir")) {
		GetCounter(argv[3]);
#endif
	}
}

void GetIPPortRuleNum(void)
{
	char *rules;

	nvram_init(RT2860_NVRAM);
	rules = (char *) nvram_bufget(RT2860_NVRAM, "IPPortFilterRules");
	if(!rules || !strlen(rules) ) {
		printf("0");
		return;
	}
	printf("%d", get_nums(rules, ';'));
	nvram_close(RT2860_NVRAM);
}

void GetRulesPacketCount(void)
{
	FILE *pp;
	int i, step_in_chains=0;
	char buf[1024], *default_policy;
	int default_drop_flag;
	int index=0, pkt_count;
	int *result;

	// check if the default policy is "drop"
	nvram_init(RT2860_NVRAM);
	default_policy = (char *)nvram_bufget(RT2860_NVRAM, "DefaultFirewallPolicy");
	if(!default_policy)
		default_policy = "0";
	default_drop_flag = strtol(default_policy, NULL, 10);
	nvram_close(RT2860_NVRAM);

	result = (int *)malloc(sizeof(int) * 128);
	if (!result)
		return;

	pp = popen("iptables -t filter -L -v", "r");
	if (!pp) {
		free(result);
		return;
	}

	while (fgets(buf, 1024, pp) && index < 128) {
		if(step_in_chains) {
			if(buf[0] == '\n')
				break;
			if(buf[0] == ' ' && buf[1] == 'p' && buf[2] == 'k' && buf[3] == 't' )
				continue;
			// Skip the first one rule if default policy is drop.
			if (default_drop_flag) {
				default_drop_flag = 0;
				continue;
			}
			sscanf(buf, "%d ", &pkt_count);
			result[index++] = pkt_count;
		}

		if(strstr(buf, "Chain " IPPORT_FILTER_CHAIN))
			step_in_chains = 1;
	}
	pclose(pp);

	if(index > 0)
		printf("%d", result[0]);
	for(i=1; i<index; i++)
		printf(" %d", result[i]);

	free(result);
}

void GetIPPortFilterRule(void)
{
	int i;
	int sprf_int, sprt_int, proto;
	char mac_address[32];
	char sip_1[32], sip_2[32], sprf[8], sprt[8], comment[16], protocol[8], action[4];
	char dip_1[32], dip_2[32], dprf[8], dprt[8];
	int dprf_int, dprt_int;
	char rec[256];
	char *default_policy, *rules;

	nvram_init(RT2860_NVRAM);
	rules = (char *) nvram_bufget(RT2860_NVRAM, "IPPortFilterRules");
	default_policy = (char *) nvram_bufget(RT2860_NVRAM, "DefaultFirewallPolicy");
	// add the default policy to the end of FORWARD chain
	if(!rules)
		return;
	if (!default_policy)
		return;
	if (!strlen(default_policy))
		return;

	i=0;
	while (get_nth_value(i, rules, ';', rec, sizeof(rec)) != -1 && strlen(rec)) {
		DBG_MSG("i=%d, rec=%s, strlen(rec)=%d", i, rec, strlen(rec));
		// get ip 1
		if((get_nth_value(0, rec, ',', sip_1, sizeof(sip_1)) == -1)){
			i++;
			continue;
		}
		if(!is_ipnetmask_valid(sip_1)){
			i++;
			continue;
		}
		// translate "any/0" to "any" for readable reason
		if( !strcmp(sip_1, "any/0"))
			strcpy(sip_1, "-");

		// get ip 2
		// get ip address
		if((get_nth_value(1, rec, ',', sip_2, sizeof(sip_2)) == -1)){
			i++;
			continue;
		}
		// dont verify cause we dont have ip range support
		//if(!is_ip_valid(sip_2))
		//    continue;
		// get port range "from"
		if((get_nth_value(2, rec, ',', sprf, sizeof(sprf)) == -1)){
			i++;
			continue;
		}
		if( (sprf_int = strtol(sprf, NULL, 10)) > 65535){
			i++;
			continue;
		}

		// get port range "to"
		if((get_nth_value(3, rec, ',', sprt, sizeof(sprt)) == -1)){
			i++;
			continue;
		}
		if( (sprt_int = strtol(sprt, NULL, 10)) > 65535){
			i++;
			continue;
		}

		// get ip 1
		if((get_nth_value(4, rec, ',', dip_1, sizeof(dip_1)) == -1)){
			i++;
			continue;
		}
		if(!is_ipnetmask_valid(dip_1)){
			i++;
			continue;
		}
		// translate "any/0" to "any" for readable reason
		if( !strcmp(dip_1, "any/0"))
			strcpy(dip_1, "-");

		// get ip 2
		if((get_nth_value(5, rec, ',', dip_2, sizeof(dip_2)) == -1)){
			i++;
			continue;
		}
		// dont verify cause we dont have ip range support
		//if(!is_ip_valid(dip_2))
		//    continue;
		// get protocol
		if((get_nth_value(8, rec, ',', protocol, sizeof(protocol)) == -1)){
			i++;
			continue;
		}
		proto = strtol(protocol, NULL, 10);
		switch(proto){
		case PROTO_TCP:
		case PROTO_UDP:
		case PROTO_NONE:
		case PROTO_ICMP:
			break;
		default:
			continue;
		}

		// get port range "from"
		if((get_nth_value(6, rec, ',', dprf, sizeof(dprf)) == -1)){
			i++;
			continue;
		}
		if( (dprf_int = strtol(dprf, NULL, 10)) > 65535){
			i++;
			continue;
		}

		// get port range "to"
		if((get_nth_value(7, rec, ',', dprt, sizeof(dprt)) == -1)){
			i++;
			continue;
		}
		if( (dprt_int = strtol(dprt, NULL, 10)) > 65535){
			i++;
			continue;
		}

		// get action
		if((get_nth_value(9, rec, ',', action, sizeof(action)) == -1)){
			i++;
			continue;
		}

		// get comment
		if((get_nth_value(10, rec, ',', comment, sizeof(comment)) == -1)){
			i++;
			continue;
		}

		// get mac address
		if((get_nth_value(11, rec, ',', mac_address, sizeof(mac_address)) == -1)){
			i++;
			continue;
		}
		if(!strlen(mac_address))
			strcpy(mac_address, "-");

		printf("<tr>");
		// output No.
		printf("<td> %d&nbsp; <input type=\"checkbox\" name=\"delRule%d\"> </td>", i+1, i );
		// output Mac address
		printf("<td align=center> %s </td>", mac_address);

		// output DIP
		printf("<td align=center> %s </td>", dip_1);
		// we dont support ip range
		// printf("<td align=center> %s-%s </td>", ip_1, ip_2);
		// output SIP
		printf("<td align=center> %s </td>", sip_1);
		// we dont support ip range
		// printf("<td align=center> %s-%s </td>", ip_1, ip_2);
		// output Protocol
		switch(proto){
		case PROTO_TCP:
			printf("<td align=center> TCP </td>");
			break;
		case PROTO_UDP:
			printf("<td align=center> UDP </td>");
			break;
		case PROTO_ICMP:
			printf("<td align=center> ICMP </td>");
			break;
		case PROTO_NONE:
			printf("<td align=center> - </td>");
			break;
		}

		// output dest Port Range
		if(dprt_int)
			printf("<td align=center> %d - %d </td>", dprf_int, dprt_int);
		else {
			// we re-descript the port number here because
			// "any" word is more meanful than "0"
			if (!dprf_int)
				printf("<td align=center> - </td>", dprf_int);
			else
				printf("<td align=center> %d </td>", dprf_int);
		}

		// output Source Port Range
		if(sprt_int)
			printf("<td align=center> %d - %d </td>", sprf_int, sprt_int);
		else{
			// we re-descript the port number here because
			// "any" word is more meanful than "0"
			if(!sprf_int){
				printf("<td align=center> - </td>", sprf_int);
			}else{
				printf("<td align=center> %d </td>", sprf_int);
			}
		}


		// output action
		switch(strtol(action, NULL, 10)){
		case ACTION_DROP:
			printf("<td align=center id=portFilterActionDrop%d> Drop </td>", i);
			break;
		case ACTION_ACCEPT:
			printf("<td align=center id=portFilterActionAccept%d> Accept </td>", i);
			break;
		}

		// output Comment
		if(strlen(comment))
			printf("<td align=center> %s</td>", comment);
		else
			printf("<td align=center> &nbsp; </td>");

		// output the id of "packet count"
		printf("<td align=center id=pktCnt%d>-</td>", i);

		printf("</tr>\n");

		i++;
	}

	switch (strtol(default_policy, NULL, 10)) {
	case 0:
		printf("<tr><td align=center colspan=9 id=portCurrentFilterDefaultAccept> Others would be accepted.</td><td align=center id=pktCnt%d>-</td></tr>", i);
		break;
	case 1:
		printf("<tr><td align=center colspan=9 id=portCurrentFilterDefaultDrop> Others would be dropped.</td><td align=center id=pktCnt%d>-</td></tr>", i);
		break;
	}
	nvram_close(RT2860_NVRAM);
}
void GetPortTriggerRuleNum(void)
{
  char *rules; 
  nvram_init(RT2860_NVRAM);
  rules = (char *)nvram_bufget(RT2860_NVRAM, "PortTriggerRules");
	if(!rules || !strlen(rules) ){
		printf("0");
		return;
	}
	printf("%d", get_nums(rules, ';'));
	nvram_close(RT2860_NVRAM);
}
void GetPortForwardRuleNum(void)
{
	char *rules;

	nvram_init(RT2860_NVRAM);
	rules = (char *) nvram_bufget(RT2860_NVRAM, "PortForwardRules");
	if(!rules || !strlen(rules) ){
		printf("0");
		return;
	}
	printf("%d", get_nums(rules, ';'));
	nvram_close(RT2860_NVRAM);
}

void GetSinglePortForwardRuleNum(void)
{
	char *rules;

	nvram_init(RT2860_NVRAM);
	rules = (char *) nvram_bufget(RT2860_NVRAM, "SinglePortForwardRules");
	if(!rules || !strlen(rules) ){
		printf("0");
		return;
	}
	printf("%d", get_nums(rules, ';'));
	nvram_close(RT2860_NVRAM);
}

void GetPortForwardRule(void)
{
	int i=0;
	int prf_int, prt_int, proto;
	char ip_address[32], prf[8], prt[8], comment[16], protocol[8];
	char rec[128];
	char *rules;

	nvram_init(RT2860_NVRAM);
	rules = (char *) nvram_bufget(RT2860_NVRAM, "PortForwardRules");
	if (!rules)
		return;
	if (!strlen(rules))
		return;

	/* format is :
	 *          * [ip],[port_from],[port_to],[protocol],[comment],;
	 *                   */
	while (get_nth_value(i++, rules, ';', rec, sizeof(rec)) != -1 ) {
		//printf("i=%d : \n",i);
		// get ip address
		if((get_nth_value(0, rec, ',', ip_address, sizeof(ip_address)) == -1)){
			//printf("ip fail!!\n");
			continue;
		}
		if(!is_ip_valid(ip_address)){
			continue;
		}

		// get port range "from"
		if((get_nth_value(1, rec, ',', prf, sizeof(prf)) == -1)){
			//printf("prf fail!!\n");
			continue;
		}
		if( (prf_int = strtol(prf, NULL, 10)) == 0 || prf_int > 65535){
			continue;
		}

		// get port range "to"
		if((get_nth_value(2, rec, ',', prt, sizeof(prt)) == -1)){
			//printf("prt fail!!\n");
			continue;
		}

		if( (prt_int = strtol(prt, NULL, 10)) > 65535){
			continue;
		}
		// get protocol
		if((get_nth_value(3, rec, ',', protocol, sizeof(protocol)) == -1)){
			//printf("proto fail!!\n");
			continue;
		}
		proto = strtol(protocol, NULL, 10);
		switch(proto){
		case PROTO_TCP:
		case PROTO_UDP:
		case PROTO_TCP_UDP:
			break;
		default:
			continue;
		}

		if((get_nth_value(4, rec, ',', comment, sizeof(comment)) == -1)){
			continue;
		}
		printf("<tr>\n");
		// output No.
		printf("<td> %d&nbsp; <input type=\"checkbox\" name=\"delRule%d\"> </td>", i, i-1 );

		// output IP address
		printf("<td align=center> %s </td>", ip_address);

		// output Port Range
		if(prt_int)
			printf("<td align=center> %d - %d </td>", prf_int, prt_int);
		else
			printf("<td align=center> %d </td>", prf_int);

		// output Protocol
		switch(proto){
		case PROTO_TCP:
			printf("<td align=center> TCP </td>");
			break;
		case PROTO_UDP:
			printf("<td align=center> UDP </td>");
			break;
		case PROTO_TCP_UDP:
			printf("<td align=center> TCP + UDP </td>");
			break;
		}

		// output Comment
		if(strlen(comment))
			printf("<td align=center> %s</td>", comment);
		else
			printf("<td align=center> &nbsp; </td>");
		printf("</tr>\n");
	}
	nvram_close(RT2860_NVRAM);
}
#if defined CONFIG_MTK_VOIP
void GetVoipDialRule(void)
{
	int i=0;
	char rec[256];
	int shortNumber_int, realNumber_int, dialActive_int;
	char shortNumber[10], realNumber[10], dialActive[6], note[16];
		nvram_init(VOIP_NVRAM);
	char *rules = (char *)nvram_bufget(VOIP_NVRAM, "VoipDialRules");
	if(!rules)
		return;
	if(!strlen(rules))
		return;
		
	while( (get_nth_value(i++, rules, ';', rec, sizeof(rec)) != -1) ){
		//get active
		if((get_nth_value(0, rec, ',', dialActive, sizeof(dialActive)) == -1)){
			continue;
		}
	  dialActive_int= atoi(dialActive);
		// get shortnumber
		if((get_nth_value(1, rec, ',', shortNumber, sizeof(shortNumber)) == -1)){
			continue;
		}
		shortNumber_int=atoi(shortNumber);
    if((get_nth_value(2, rec, ',', realNumber, sizeof(realNumber)) == -1))
			continue;
		realNumber_int=atoi(realNumber);	
		if((get_nth_value(3, rec, ',', note, sizeof(note)) == -1)){
			continue;
		}
		//printf(wp, T("<tr>\n"));
		printf("<tr> \n");
		// output No.
		//printf(wp, T("<td> %d&nbsp; <input type=\"checkbox\" name=\"delRule%d\"> </td>"), i, i-1 );
    printf("<td> %d&nbsp; <input type=\"checkbox\" name=\"delRule%d\"> </td>", i, i-1 );
	
	  if(dialActive_int == 1)
	    printf("<td align=center> Y </td>");
	  else
	  	printf("<td align=center> N </td>");

			printf("<td align=center> %d </td>", shortNumber_int);

      printf("<td align=center> %d </td>", realNumber_int);
		// output Comment
		if(strlen(note)){
			//printf(wp, T("<td align=center> %s</td>"), comment);
			printf("<td align=center> %s</td>", note);
		}	
		else{
			//printf(wp, T("<td align=center> &nbsp; </td>"));
   		//printf(wp, T("</tr>\n"));
   		printf("<td align=center> &nbsp; </td>");
   		printf("</tr>\n");
   	}
  }	
  	nvram_close(VOIP_NVRAM);	
}
void GetVoipDialRule2(void)
{
	int i=0;
	char rec[256];
	int shortNumber_int, realNumber_int, dialActive_int;
	char shortNumber[10], realNumber[10], dialActive[6], note[16];
			nvram_init(VOIP_NVRAM);
	char *rules = (char *)nvram_bufget(VOIP_NVRAM, "VoipDialRules2");
	if(!rules)
		return;
	if(!strlen(rules))
		return;
		
	while( (get_nth_value(i++, rules, ';', rec, sizeof(rec)) != -1) ){
		//get active
		if((get_nth_value(0, rec, ',', dialActive, sizeof(dialActive)) == -1)){
			continue;
		}
	  dialActive_int= atoi(dialActive);
		// get shortnumber
		if((get_nth_value(1, rec, ',', shortNumber, sizeof(shortNumber)) == -1)){
			continue;
		}
		shortNumber_int=atoi(shortNumber);
    if((get_nth_value(2, rec, ',', realNumber, sizeof(realNumber)) == -1))
			continue;
		realNumber_int=atoi(realNumber);	
		if((get_nth_value(3, rec, ',', note, sizeof(note)) == -1)){
			continue;
		}
		//printf(wp, T("<tr>\n"));
		printf("<tr> \n");
		// output No.
		//printf(wp, T("<td> %d&nbsp; <input type=\"checkbox\" name=\"delRule%d\"> </td>"), i, i-1 );
    printf("<td> %d&nbsp; <input type=\"checkbox\" name=\"delRule%d\"> </td>", i, i-1 );
	
	  if(dialActive_int == 1)
	    printf("<td align=center> Y </td>");
	  else
	  	printf("<td align=center> N </td>");

			printf("<td align=center> %d </td>", shortNumber_int);

      printf("<td align=center> %d </td>", realNumber_int);
		// output Comment
		if(strlen(note)){
			//printf(wp, T("<td align=center> %s</td>"), comment);
			printf("<td align=center> %s</td>", note);
		}	
		else{
			//printf(wp, T("<td align=center> &nbsp; </td>"));
   		//printf(wp, T("</tr>\n"));
   		printf("<td align=center> &nbsp; </td>");
   		printf("</tr>\n");
   	}
  }		
  nvram_close(VOIP_NVRAM);
}
#endif
void GetPortTriggerRule(void)
{
	int i=0;
	char rec[256];
	int triggerPort_int, incomingPort_int, triggerProto, incomingProto;
	char triggerPort[8], incomingPort[8], triggerProtocol[8], incomingProtocol[8], comment[16];
	char *rules = (char *)nvram_bufget(RT2860_NVRAM, "PortTriggerRules");
	
	if(!rules)
		return;
	if(!strlen(rules))
		return;
		
	while( (get_nth_value(i++, rules, ';', rec, sizeof(rec)) != -1) ){
		//get trigger protocol
		if((get_nth_value(0, rec, ',', triggerProtocol, sizeof(triggerProtocol)) == -1))
			continue;
		triggerProto = atoi(triggerProtocol);
		// get trigger port
		if((get_nth_value(1, rec, ',', triggerPort, sizeof(triggerPort)) == -1)){
			printf("triggerPort = %s\n", triggerPort);	
			continue;
		}
		if( (triggerPort_int = atoi(triggerPort)) == 0 || triggerPort_int > 65535)
			continue;
        // get incoming portoocol
    if((get_nth_value(2, rec, ',', incomingProtocol, sizeof(incomingProtocol)) == -1))
			continue;
		incomingProto = atoi(incomingProtocol);
		// get incoming port
    if((get_nth_value(3, rec, ',', incomingPort, sizeof(incomingPort)) == -1)){
			printf("incomingPort = %s\n", incomingPort);	
			continue;
		}
		if( (incomingPort_int = atoi(incomingPort)) == 0 || incomingPort_int > 65535)
			continue;
		
		if((get_nth_value(4, rec, ',', comment, sizeof(comment)) == -1)){
			continue;
		}
		//printf(wp, T("<tr>\n"));
		printf("<tr> \n");
		// output No.
		//printf(wp, T("<td> %d&nbsp; <input type=\"checkbox\" name=\"delRule%d\"> </td>"), i, i-1 );
    printf("<td> %d&nbsp; <input type=\"checkbox\" name=\"delRule%d\"> </td>", i, i-1 );
		// output Trigger Protocol
     switch(triggerProto){
        case PROTO_TCP:
					//printf(wp, T("<td align=center> TCP </td>"));
					printf("<td align=center> TCP </td>");
					break;
        case PROTO_UDP:
					//printf(wp, T("<td align=center> UDP </td>"));
					printf("<td align=center> UDP </td>");
				break;
		}

		// output trigger Port 
			//printf(wp, T("<td align=center> %d </td>"), triggerPort_int);
			printf("<td align=center> %d </td>", triggerPort_int);
		// output Incoming Protocol
        switch(incomingProto){
            case PROTO_TCP:
							//printf(wp, T("<td align=center> TCP </td>"));
							printf("<td align=center> TCP </td>");
							break;
            case PROTO_UDP:
							//printf(wp, T("<td align=center> UDP </td>"));
							printf("<td align=center> UDP </td>");
							break;
				}

		// output incoming Port 
			//printf(wp, T("<td align=center> %d </td>"), incomingPort_int);
    printf("<td align=center> %d </td>", incomingPort_int);
		// output Comment
		if(strlen(comment)){
			//printf(wp, T("<td align=center> %s</td>"), comment);
			printf("<td align=center> %s</td>", comment);
		}	
		else{
			//printf(wp, T("<td align=center> &nbsp; </td>"));
   		//printf(wp, T("</tr>\n"));
   		printf("<td align=center> &nbsp; </td>");
   		printf("</tr>\n");
   	}
  }	
}
void GetSinglePortForwardRule(void)
{
	int i=0;
	int publicPort_int, privatePort_int, proto;
	char ip_address[32], publicPort[8], privatePort[8], comment[16], protocol[8];
	char rec[128];
	char *rules;

	nvram_init(RT2860_NVRAM);
	rules = (char *) nvram_bufget(RT2860_NVRAM, "SinglePortForwardRules");
	if (!rules)
		return;
	if (!strlen(rules))
		return;

	/* format is :
	 *          * [ip],[port_public],[port_private],[protocol],[comment],;
	 *                   */
	while(get_nth_value(i++, rules, ';', rec, sizeof(rec)) != -1 ){
		// get ip address
		if((get_nth_value(0, rec, ',', ip_address, sizeof(ip_address)) == -1)){
			continue;
		}
		if(!is_ip_valid(ip_address)){
			continue;
		}

		// get public port
		if((get_nth_value(1, rec, ',', publicPort, sizeof(publicPort)) == -1)){
			continue;
		}
		if( (publicPort_int = strtol(publicPort, NULL, 10)) == 0 || publicPort_int > 65535){
			continue;
		}

		// get private port
		if((get_nth_value(2, rec, ',', privatePort, sizeof(privatePort)) == -1)){
			continue;
		}
		if( (privatePort_int = strtol(privatePort, NULL, 10)) == 0 || privatePort_int > 65535){
			continue;
		}

		// get protocol
		if((get_nth_value(3, rec, ',', protocol, sizeof(protocol)) == -1)){
			continue;
		}
		proto = strtol(protocol, NULL, 10);
		switch(proto){
		case PROTO_TCP:
		case PROTO_UDP:
		case PROTO_TCP_UDP:
			break;
		default:
			continue;
		}

		if((get_nth_value(4, rec, ',', comment, sizeof(comment)) == -1)){
			continue;
		}

		printf("<tr>\n");
		// output No.
		printf("<td> %d&nbsp; <input type=\"checkbox\" name=\"delRule%d\"> </td>", i, i-1 );

		// output IP address
		printf("<td align=center> %s </td>", ip_address);

		// output Public port
		printf("<td align=center> %d </td>", publicPort_int);

		// output Private port
		printf("<td align=center> %d </td>", privatePort_int);

		// output Protocol
		switch(proto){
		case PROTO_TCP:
			printf("<td align=center> TCP </td>");
			break;
		case PROTO_UDP:
			printf("<td align=center> UDP </td>");
			break;
		case PROTO_TCP_UDP:
			printf("<td align=center> TCP + UDP </td>");
			break;
		}

		// output Comment
		if(strlen(comment))
			printf("<td align=center> %s</td>", comment);
		else
			printf("<td align=center> &nbsp; </td>");
		printf("</tr>\n");
	}
	nvram_close(RT2860_NVRAM);
}
char *getNameIntroFromPat(char *filename)
{
	static char result[512];
	char buf[512], *begin, *end, *desh;
	char path_filename[512];
	char *rc;
	FILE *fp;

	sprintf(path_filename, "%s/%s", "/etc_ro/l7-protocols", filename);
	if(! (fp = fopen(path_filename, "r")))
		return NULL;
	result[0] = '\0';
	rc = fgets(buf, sizeof(buf), fp);
	if(rc){
		// find name
		begin = buf + 2;
		if(! ( desh = strchr(buf, '-'))){
			printf("warning: can't find %s name.\n", filename);
			fclose(fp);
			return "N/A#N/A";
		}
		end = desh;
		if(*(end-1) == ' ')
			end--;
		*end = '\0';
		strncat(result, begin, sizeof(result));
		strncat(result, "#", sizeof(result));

		// find intro
		if(!(end = strchr(desh+1, '\n'))){
			printf("warning: can't find %s intro.\n", filename);
			fclose(fp);
			return "N/A#N/A";
		}
		*end = '\0';
		//strncat(result, desh + 2 , sizeof(result));
		//strncat(tmp_str, protX[i], sizeof(tmp_str));
                snprintf(result, sizeof(result), "%s%s", result, desh + 2);
	}else{
		printf("warning: can't read %s intro.\n", filename);
		fclose(fp);
		return "N/A#N/A";
	}

	fclose(fp);
	return result;	
}
void GetLayer7FilterName(void)
{
	char *delim;
	struct dirent *dir;
	DIR *d;
	char *intro;

	l7name[0] = '\0';
	if(!(d = opendir("/etc_ro/l7-protocols")))
		return;
	
	while((dir = readdir(d))){
		if(dir->d_name[0] == '.')
			continue;
		if(!(delim = strstr(dir->d_name, ".pat")) )
			continue;
		
		intro = getNameIntroFromPat(dir->d_name);
		if(intro == NULL) intro = "";
		*delim = '\0';
		if(l7name[0] == '\0'){
			strncat(l7name, dir->d_name, sizeof(l7name));
			strncat(l7name, "#", sizeof(l7name));
			strncat(l7name, intro, sizeof(l7name));
		}else{
			strncat(l7name, ";", sizeof(l7name));
			strncat(l7name, dir->d_name, sizeof(l7name));
			strncat(l7name, "#", sizeof(l7name));
			strncat(l7name, intro, sizeof(l7name));
		}
	}
	closedir(d);
	//websLongWrite(wp, l7name);
	printf("%s",l7name);
}
#if defined CONFIG_MTK_VOIP
void GetStunStatus(void){
		char *value;
		nvram_init(VOIP_NVRAM);
	
		value = nvram_bufget(VOIP_NVRAM, "SC_ACCT_1_NAT_SRV_ADDR");

		if(strcmp(value, "")!=0){
			printf("Enable");
		}else{
	    printf("Disable");
		}
	nvram_close(VOIP_NVRAM);
	
}
void GetStunStatus2(void){
		char *value;
		nvram_init(VOIP_NVRAM);
	
		value = nvram_bufget(VOIP_NVRAM, "SC_ACCT_2_NAT_SRV_ADDR");

		if(strcmp(value, "")!=0){
			printf("Enable");
		}else{
	    printf("Disable");
		}
	nvram_close(VOIP_NVRAM);
	
}

void WebVoipGet(char *argv[])
{
	if (!strcmp(argv[3], "voipDialRule")) {
		GetVoipDialRule();
	}else if(!strcmp(argv[3], "voipDialRule2")) {
		GetVoipDialRule2();
  }else if(!strcmp(argv[3], "stun_status")) {
		GetStunStatus();
  }else if(!strcmp(argv[3], "stun_status2")) {
		GetStunStatus2();
  }
	
}
void getCertInfo(char *name)
{

	#if (1)
	char *buf;
	char filename[256];
	int flag=0;
	unsigned is_load = 0;
	unsigned size;
	X509 *pCert=NULL;
	memset(filename, 0, 256);
	sprintf(filename,"/etc/%s.pem", name);
	FILE *fp=fopen(filename,"r");

  nvram_init(VOIP_NVRAM);
  size = 256;
	buf = malloc(size);

	/**
	 * 20100127 Alex Huang 
	 * Read Cert from MOM
	 */
/*
	if (!fp) {
		unsigned type = 0xFFFFFFFF, *is_load;
		unsigned size;
		char *buf2 = NULL;
		//int ret = -1;

		if (strcmp("wmx_root_ca", argv[0]) == 0) 
			type = WMXAPI_SERVER_CERT;
		else if (strcmp("wmx_client_ca", argv[0]) == 0)
			type = WMXAPI_DEVICE_CERT;

		if (type != 0xFFFFFFFF) {
			size = 256;
			ret = wmxapi_get_cert(type, buf, &size);
			if (ret != 0 && size != 0 && size < 128*1024) {
				buf2 = malloc(size);
				if (buf2) 
					ret = wmxapi_get_cert(type, buf2, &size);
			}
		}

		if (ret == 0) {
			if (size != 0) {
				int i;
				sprintf(filename,"/tmp/%s.pem", name);
				fp = fopen(filename, "w");
				fwrite(buf, size, 1, fp);
				fclose(fp);
				fp = fopen(filename, "r");
			}
			is_load = 1;
		}
	}
*/
	if(!fp)
	{
		sprintf(buf,"No certificate file found");
		printf("%s", buf);
		return;
	}

	pCert=PEM_read_X509(fp,NULL,0,NULL);
	if(pCert != NULL)  //certificate is PEM format
		flag = 1;
	else
	{
		fseek(fp, 0, SEEK_SET);
		pCert=d2i_X509_fp(fp,NULL);
		if(pCert != NULL)  //certificate is DER format
			flag = 2;
	}
	
	if(flag > 0)
	{
		while(pCert != NULL)
		{
			memset(buf,0,256);
			X509_NAME_oneline(X509_get_subject_name(pCert), buf, 256);
			//web_putsEscape(conn_fp, buf, "'\\");
			printf("%s", buf);
			pCert=NULL;
			if(flag == 1)
				pCert=PEM_read_X509(fp,NULL,0,NULL);
			else if(flag == 2)
				pCert=d2i_X509_fp(fp,NULL);
			if(pCert != NULL)
				printf("\\n\\n");
		}
	}
	else
	{
		sprintf(buf,"Unknown certificate file format");
		printf("%s", buf);
	}
 
	fclose(fp);
	if (is_load) 
		unlink(filename);
	
	nvram_commit(VOIP_NVRAM);
	nvram_close(VOIP_NVRAM);
	#endif
}


void WebCertInfoGet(char *argv[])
{
	if (!strcmp(argv[3], "cwmp_cacert")) {
		getCertInfo("cwmp_cacert");
	}else if(!strcmp(argv[3], "cwmp_client")) {
		getCertInfo("cwmp_client");
  }

}
#endif
void WebFirewallGet(char *argv[])
{
	if (!strcmp(argv[3], "ipPortRuleNum")) {
		GetIPPortRuleNum();
	} else if (!strcmp(argv[3], "rulePktCount")) {
		GetRulesPacketCount();
	} else if (!strcmp(argv[3], "ipPortFilterRule")) {
		GetIPPortFilterRule();
	} else if (!strcmp(argv[3], "portForwardRuleNum")) {
		GetPortForwardRuleNum();
	} else if (!strcmp(argv[3], "singlePortForwardRuleNum")) {
		GetSinglePortForwardRuleNum();
	} else if (!strcmp(argv[3], "portTriggerRuleNum")) {
		GetPortTriggerRuleNum();
	} else if (!strcmp(argv[3], "portTriggerRule")) {
		GetPortTriggerRule();			
	} else if (!strcmp(argv[3], "portForwardRule")) {
		GetPortForwardRule();
	} else if (!strcmp(argv[3], "singlePortForwardRule")) {
		GetSinglePortForwardRule();
	} else if (!strcmp(argv[3], "getLayer7")) {
		GetLayer7FilterName();
	} 
}

void do_auto_wan_detect(void)
{
#if	defined CONFIG_RALINK_MT7620 || CONFIG_RALINK_MT7628
	do_system("auto_wan_pt 1 eth2.2");
#endif

#if	defined (CONFIG_RALNK_MT7621)
	do_system("auto_wan_pt 1 eth3");
#endif
	printf("0");
}

void GetMediaState(void)
{
	printf("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n");
	printf("%s;%s", media_state.operate, media_state.connected);
	//websDone(wp, 200);
}

void GetMediaOpt(void)
{
	//DBG_MSG("harry media_state.operate =%s\n", media_state.operate);
	printf("%s", nvram_bufget(RT2860_NVRAM, "media_state_operate"));
}

void GetMediaMode(void)
{
	//DBG_MSG("harry media_state.service_mode =%s\n", media_state.service_mode);
	//printf("%s", media_state.service_mode);
	printf("%s", nvram_bufget(RT2860_NVRAM, "media_state_service_mode"));
}

void GetMediaOption(void)
{
	DBG_MSG("[DBG] media_state.option =%s", media_state.option); //Harry
	printf("%s", media_state.option);
}

void GetMediaTarget(void)
{
	DBG_MSG("[DBG] media_state.target =%s\n", media_state.target); //Harry
	printf("%s", media_state.target);
}

/*
static int getMediaVolume(int eid, webs_t wp, int argc, char_t **argv)
{
	return websWrite(wp, T("%s"), media_state.volume);
}

static int getMediaMute(int eid, webs_t wp, int argc, char_t **argv)
{
	return websWrite(wp, T("%s"), media_state.mute);
}
*/

void GetMediaConnected(void)
{
	DBG_MSG("[DBG] media_state.connected =%s\n", media_state.connected); //Harry
	printf("%s", media_state.connected);
}




int ShowUnSelectedList(void)
{
	int i = 0;

	if (0 == strlen(all_list[0].name))
	{
		memset(all_list, 0, sizeof(all_list));
		lookupAllList("/media");
		lookupSelectList();
	}

	while(0 != strlen(all_list[i].name) && i < 1024)
	{
		if (0 == all_list[i].selected)
		{
			//websWrite(wp, T("<option value=\"%d\">%s</option>"), i, all_list[i].name);
			  printf("<option value=\"%d\">%s</option>", i, all_list[i].name);
		}
		i++;
		if(i >= 1024) break;
	}

	return 0;
}
int ShowSelectedList(void)
{
	int i = 0;
	if (0 == strlen(all_list[0].name))
	{
		memset(all_list, 0, sizeof(all_list));
		lookupAllList("/media");
		lookupSelectList();
	}
	while(0 != strlen(all_list[i].name) && i < 1024)
	{
		if (1 == all_list[i].selected)
		{
			//websWrite(wp, T("<option value=\"%d\">%s</option>"), i, all_list[i].name);
			  printf("<option value=\"%d\">%s</option>", i, all_list[i].name);
		}
		i++;
		if(i >= 1024) break;
	}

	return 0;
}

void WebMediaGet(char *argv[])
{
		if (!strcmp(argv[3], "getMediaState")) {
		GetMediaState();
	} else if (!strcmp(argv[3], "getMediaOpt")) {
		GetMediaOpt();
	} else if (!strcmp(argv[3], "getMediaMode")) {
		GetMediaMode();
	} else if (!strcmp(argv[3], "getMediaTarget")) {
		GetMediaTarget();
	} else if (!strcmp(argv[3], "getMediaConnected")) {
		GetMediaConnected();
	} else if (!strcmp(argv[3], "showUnSelectedList")) {
		ShowUnSelectedList();
	} else if (!strcmp(argv[3], "showSelectedList")) {
		ShowSelectedList();
	} 
}

void usage(char *aout)
{
	//set_usage("STF");
	get_usage(aout);

	return;
}

void getCentralFreq(char *argv[]) 
{ 

	int s, ret, CentralFreq = 0, CaptureBw = 0; 

	s = socket(AF_INET, SOCK_DGRAM, 0); 
	if (!strcmp(argv[1], WLAN1_CONF)) 
	{ 
		ret = ap_oid_query_info(OID_802_11_WIFISPECTRUM_GET_CENTRAL_FREQ, s, "ra0", &CentralFreq, sizeof(CentralFreq)); 
		ret = ap_oid_query_info(OID_802_11_WIFISPECTRUM_GET_CAPTURE_BW, s, "ra0", &CaptureBw, sizeof(CaptureBw)); 

		printf("%d:%d", CentralFreq,CaptureBw); 
	} 
	else if (!strcmp(argv[1], WLAN2_CONF)) 
	{ 
		ret = ap_oid_query_info(OID_802_11_WIFISPECTRUM_GET_CENTRAL_FREQ, s, "rai0", &CentralFreq, sizeof(CentralFreq)); 

		ret = ap_oid_query_info(OID_802_11_WIFISPECTRUM_GET_CAPTURE_BW, s, "rai0", &CaptureBw, sizeof(CaptureBw)); 

		printf("%d:%d", CentralFreq,CaptureBw); 
	} 
	else 
	{ 
		printf("Interface doesn't match\n\n"); 
		close(s);
		exit(1); 
	} 
	close(s);
}

int main(int argc, char *argv[])
{
	int cmd;

	if (argc < 4) {
		get_usage(argv[0]);
		return;
	}

	if (!strncmp(argv[2], "nvram", 6))
		cmd = CMD_NVRAM_GET;
	else if (!strncmp(argv[2], "build", 6))
		cmd = CMD_BUILD_GET;
	else if (!strncmp(argv[2], "wifi", 5))
		cmd = CMD_WIFI_GET;
	else if (!strncmp(argv[2], "sys", 4))
		cmd = CMD_SYS_GET;
	else if (!strncmp(argv[2], "firewall", 5))
		cmd = CMD_FIREWALL_GET;
	else if (!strncmp(argv[2], "media", 6))
		cmd = CMD_MEDIA_GET;
	else if (!strncmp(argv[2], "test", 6))
		cmd = CMD_TEST_GET;
#if defined CONFIG_MTK_VOIP
	else if (!strncmp(argv[2], "voip", 5))
		cmd = CMD_VOIP_GET;
	else if (!strncmp(argv[2], "tr069", 5))
	  cmd = CMD_TR069_GET;
#endif	  
#if defined CONFIG_RALINKAPP_AUTO_WAN
	else if (!strncmp(argv[2], "auto_wan", 5))
		cmd = CMD_AUTO_WAN_GET;	
#endif
	else
		cmd = 0;

	switch(cmd) {
	case CMD_NVRAM_GET:
		WebNvramGet(argv);
		break;
	case CMD_BUILD_GET:
		WebBuildGet(argv);
		break;
	case CMD_WIFI_GET:
		WebWifiGet(argv);
		break;
	case CMD_SYS_GET:
		WebSysGet(argv);
		break;
	case CMD_FIREWALL_GET:
		WebFirewallGet(argv);
		break;
#if defined CONFIG_MTK_VOIP
	case CMD_VOIP_GET:
		WebVoipGet(argv);
		break;
	case CMD_TR069_GET:

		WebCertInfoGet(argv);
		break;
#endif
#if defined CONFIG_RALINKAPP_AUTO_WAN
	case CMD_AUTO_WAN_GET:
		do_auto_wan_detect();
		break;
#endif
	case CMD_TEST_GET:
		printf("SSID1\tSSID2\tSSID3\t\tSSID4\tSSID5\tSSID6\t\tSSID7\tSSID8\tSSID9");
		break;
	}
}
