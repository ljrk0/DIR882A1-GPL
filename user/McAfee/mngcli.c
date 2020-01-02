/****add by zengqinghuang 20171125 for McAfee API
*****mngcli command
*****需要获取或者设置的参数，实际上是通过nvram实现
*****/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include "nvram.h"

#define RT2860_NVRAM	0
#define RTDEV_NVRAM		1
#define WIFI3_NVRAM		2

#define REGION_1_RESPONSE_CODE            1
#define REGION_2_RESPONSE_CODE            2	
#define REGION_3_RESPONSE_CODE            3	
#define REGION_4_RESPONSE_CODE            4	
#define REGION_5_RESPONSE_CODE            5	
#define REGION_6_RESPONSE_CODE            6	
#define REGION_7_RESPONSE_CODE            7	
#define REGION_8_RESPONSE_CODE            8	
#define REGION_9_RESPONSE_CODE            9	
#define REGION_10_RESPONSE_CODE           10
#define REGION_11_RESPONSE_CODE           11
#define REGION_12_RESPONSE_CODE            12
#define REGION_13_RESPONSE_CODE            13
#define REGION_14_RESPONSE_CODE            14
#define REGION_15_RESPONSE_CODE            15
#define REGION_16_RESPONSE_CODE            16
#define REGION_17_RESPONSE_CODE            17
#define REGION_18_RESPONSE_CODE            18
#define REGION_19_RESPONSE_CODE            19
#define REGION_20_RESPONSE_CODE            20

typedef struct {
	const char *name;
	char *nv_name;
} applets_t;

typedef struct {
	unsigned char CountryCode2g[3];
	unsigned char CountryCode5g[3];
	unsigned int FirmwareRegion;
}FIRMWARE_REGION_CODE;

FIRMWARE_REGION_CODE FirmwareRegionCode[]=
{
	{"NA", "NA", REGION_1_RESPONSE_CODE}, /* United States (USA) */
	{"NA", "CA", REGION_2_RESPONSE_CODE}, /* Canada */
	{"DI", "AU", REGION_3_RESPONSE_CODE}, /* South America/Latin America */
	{"DI", "LA", REGION_3_RESPONSE_CODE}, /* South America/Latin America */
	{"AU", "NA", REGION_3_RESPONSE_CODE}, /* South America/Latin America */
	{"AU", "LA", REGION_3_RESPONSE_CODE}, /* South America/Latin America */
	{"AU", "EU", REGION_3_RESPONSE_CODE}, /* South America/Latin America */
	{"EU", "LA", REGION_3_RESPONSE_CODE}, /* South America/Latin America */
	{"DI", "BR", REGION_4_RESPONSE_CODE}, /* Brazil */
	{"AU", "BR", REGION_4_RESPONSE_CODE}, /* Brazil */
	{"EU", "EU", REGION_5_RESPONSE_CODE}, /* Europe ,South Africa*/
	{"FR", "EU", REGION_5_RESPONSE_CODE}, /* Europe ,South Africa*/
	{"AU", "AU", REGION_6_RESPONSE_CODE}, /* Australia */
	{"DI", "SG", REGION_7_RESPONSE_CODE}, /* Singapore */
	{"NA", "SG", REGION_8_RESPONSE_CODE}, /* Singapore,Thailand */
	{"EU", "SG", REGION_9_RESPONSE_CODE}, /* Singapore,Vietnam, Malaysia */
	{"SG", "SG", REGION_9_RESPONSE_CODE}, /* Singapore,Vietnam, Malaysia  */
	{"SG", "EU", REGION_9_RESPONSE_CODE}, /* Singapore,Vietnam, Malaysia  */
	{"EU", "MY", REGION_10_RESPONSE_CODE}, /* Malaysia  */
	{"SG", "MY", REGION_10_RESPONSE_CODE}, /* Malaysia  */
	{"SG", "NA", REGION_11_RESPONSE_CODE}, /* Thailand  */
	{"TH", "NA", REGION_11_RESPONSE_CODE}, /* Thailand  */
	{"NA", "TW", REGION_12_RESPONSE_CODE}, /* Taiwan  */
	{"EU", "KR", REGION_13_RESPONSE_CODE}, /* Korea */
	{"JP", "JP", REGION_14_RESPONSE_CODE}, /* Japan */
	{"AU", "IN", REGION_15_RESPONSE_CODE}, /* India */
	{"AU", "IN", REGION_15_RESPONSE_CODE}, /* India */
	{"EU", "IN", REGION_15_RESPONSE_CODE}, /* India */
	{"DI", "NA", REGION_16_RESPONSE_CODE}, /* Middle East */
	{"DI", "EG", REGION_17_RESPONSE_CODE}, /* Egypt */
	{"EU", "RU", REGION_18_RESPONSE_CODE}, /* Russia */
	{"RU", "RU", REGION_18_RESPONSE_CODE}, /* Russia */
	{"DI", "IL", REGION_19_RESPONSE_CODE}, /* Israel */
	{"EU", "IL", REGION_19_RESPONSE_CODE}, /* Israel */
	{"CN", "CN", REGION_18_RESPONSE_CODE}, /* China */	
	{NULL, NULL}
};

static const applets_t applets[] = {
	{ "ARC_SYS_SerialNum",		"lan0_mac" },
	{ "ARC_SYS_FWVersion",      "sw_ex_version"   },
	{ "ARC_SYS_FWSubVersion",   "sw_in_version" },	
	{ "ARC_SYS_ModelName",      "model_name"},
	{ "ARC_LAN_0_Ifname",       "lan0_ifname"},
	{ "ARC_LAN_1_Ifname",      "lan1_ifname"},
	{ "ARC_WLAN_24G_SSID_0_ESSID",      "SSID1"},
	{ "ARC_WLAN_24G_SSID_0_WPA_Passphrase",      "WPAPSK1"},
	{ "ARC_WLAN_24G_SSID_2_ESSID",      "SSID2"},
	{ "ARC_WLAN_24G_SSID_2_WPA_Passphrase",      "WPAPSK2"},
	{ "ARC_WLAN_5G_SSID_0_ESSID",      "SSID1"},
	{ "ARC_WLAN_5G_SSID_0_WPA_Passphrase",      "WPAPSK1"},
	{ "ARC_WLAN_5G_SSID_2_ESSID",      "SSID2"},
	{ "ARC_WLAN_5G_SSID_2_WPA_Passphrase",      "WPAPSK2"},
	{ "ARC_WLAN_5G_SSID_3_ESSID",      "SSID1"},
	{ "ARC_WLAN_5G_SSID_3_WPA_Passphrase",      "WPAPSK1"},
	{ "ARC_WLAN_Smartconnect",      "BandSteering"},
	{ "ARC_WAN_100_Ifname",         "wan_wan0_ifname"},
	{ "ARC_WAN_100_IP4_Proto",      "wan_wan0_proto"},
	{ "ARC_WAN_100_IP6_Proto",      "wan_wan0_ipv6_type"},
	{ "ARC_SYS_FirmwareRegion",     "FirmwareRegion"},
	
	{NULL, NULL}
};

int
main(int argc, char **argv)
{
	char *base = NULL;
	char *value = NULL;
	int wifi_num = RT2860_NVRAM;

	if(argc < 3)
	{
		printf("format is err\n");
		return -1;
	}
	base = strrchr(argv[2], '/');
	base = base ? base + 1 : argv[2];
	
	const applets_t *a;
	for (a = applets; a->name; ++a) 
	{
		if (strcmp(base, a->name) == 0) 
		{
			if(strcmp(argv[1], "get") == 0 && argc == 3)
			{
				if(strstr(base, "5G_SSID_3"))
					wifi_num = WIFI3_NVRAM;
				else if(strstr(base, "5G_SSID"))
					wifi_num = RTDEV_NVRAM;
				else
					wifi_num = RT2860_NVRAM;
				
				if(strcmp(base, "ARC_SYS_FirmwareRegion") == 0)
				{
					int i = 0;
					char *pszRegion2g = NULL;
					char *pszRegion5g = NULL;
					pszRegion2g = nvram_get(RT2860_NVRAM, "CountryCode");
					pszRegion5g = nvram_get(RTDEV_NVRAM, "CountryCode");
					for(i = 0; i < sizeof(FirmwareRegionCode)/sizeof(FirmwareRegionCode[0]); i++)
					{
						if(!strncmp(FirmwareRegionCode[i].CountryCode2g, pszRegion2g, 2) && 
							!strncmp(FirmwareRegionCode[i].CountryCode5g, pszRegion5g, 2))
						{
							printf("%d\n", FirmwareRegionCode[i].FirmwareRegion);
						}
					}
				}
				else
				{
					value = nvram_get(wifi_num, a->nv_name);
					printf("%s\n", value);
				}
			}
			else if(strcmp(argv[1], "set") == 0 && argc == 4)
			{
				if(strlen(argv[3]) > 0)
				{
					if(strstr(base, "5G_SSID_3"))
						wifi_num = WIFI3_NVRAM;
					else if(strstr(base, "5G_SSID"))
						wifi_num = RTDEV_NVRAM;
					else
						wifi_num = RT2860_NVRAM;
					
					nvram_set(wifi_num, a->nv_name, argv[3]);
				}
			}
			else
				printf("hehe, format is error\n");
		}
   	}
	return 0;
}


