#include "utils.h"
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>
#include "oid.h"
#if defined (CONFIG_RT2860V2_STA_WSC)
#include	"stapriv.h"
#include        <signal.h>
#include        <sys/time.h>
#endif

#define AP_MODE

#include "oid.h"

static void revise_mbss_value(int nvram, int old_num, int new_num)
{
	/* {{{ The parameters that support multiple BSSID is listed as followed,
	 1.) SSID,                 char SSID[33];
	 2.) AuthMode,             char AuthMode[14];
	 3.) EncrypType,           char EncrypType[8];
	 4.) WPAPSK,               char WPAPSK[65];
	 5.) DefaultKeyID,         int  DefaultKeyID;
	 6.) Key1Type,             int  Key1Type;
	 7.) Key1Str,              char Key1Str[27];
	 8.) Key2Type,             int  Key2Type;
	 9.) Key2Str,              char Key2Str[27];
	10.) Key3Type,            int  Key3Type;
	11.) Key3Str,             char Key3Str[27];
	12.) Key4Type,            int  Key4Type;
	13.) Key4Str,             char Key4Str[27];
	14.) AccessPolicy,
	15.) AccessControlList,
	16.) NoForwarding,
	17.) IEEE8021X,           int  IEEE8021X;
	18.) TxRate,              int  TxRate;
	19.) HideSSID,            int  HideSSID;
	20.) PreAuth,             int  PreAuth;
	21.) WmmCapable
	                          int  SecurityMode;
	                          char VlanName[20];
	                          int  VlanId;
	                          int  VlanPriority;
	   }}} */
	char new_value[264], *p;
	const char *old_value;
	int i;

#define MBSS_INIT(field, default_value) \
	do { \
		old_value = nvram_bufget(nvram, #field); \
		snprintf(new_value, 264, "%s", old_value); \
		p = new_value + strlen(old_value); \
		for (i = old_num; i < new_num; i++) { \
			snprintf(p, 264 - (p - new_value), ";%s", default_value); \
			p += 1 + strlen(default_value); \
		} \
		nvram_bufset(nvram, #field, new_value); \
	} while (0)

#define MBSS_REMOVE(field) \
	do { \
		old_value = nvram_bufget(nvram, #field); \
		snprintf(new_value, 264, "%s", old_value); \
		p = new_value; \
		for (i = 0; i < new_num; i++) { \
			if (0 == i) \
			p = strchr(p, ';'); \
			else \
			p = strchr(p+1, ';'); \
			if (NULL == p) \
			break; \
		} \
		if (p) \
		*p = '\0'; \
		nvram_bufset(nvram, #field, new_value); \
	} while (0)

	if (new_num > old_num) {
		//MBSS_INIT(SSID, "ssid");
		MBSS_INIT(AuthMode, "OPEN");
		MBSS_INIT(EncrypType, "NONE");
		//MBSS_INIT(WPAPSK, "12345678");
		MBSS_INIT(DefaultKeyID, "1");
		MBSS_INIT(Key1Type, "0");
		//MBSS_INIT(Key1Str, "");
		MBSS_INIT(Key2Type, "0");
		//MBSS_INIT(Key2Str, "");
		MBSS_INIT(Key3Type, "0");
		//MBSS_INIT(Key3Str, "");
		MBSS_INIT(Key4Type, "0");
		//MBSS_INIT(Key4Str, "");
		/*      MBSS_INIT(AccessPolicy0, "0");
		        MBSS_INIT(AccessControlList0, "");
		        MBSS_INIT(AccessPolicy1, "0");
		        MBSS_INIT(AccessControlList1, "");
		        MBSS_INIT(AccessPolicy2, "0");
		        MBSS_INIT(AccessControlList2, "");
		        MBSS_INIT(AccessPolicy3, "0");
		        MBSS_INIT(AccessControlList3, ""); */
		MBSS_INIT(NoForwarding, "0");
		MBSS_INIT(NoForwardingBTNBSSID, "0");
		MBSS_INIT(IEEE8021X, "0");
		MBSS_INIT(RADIUS_Server, "0");
		MBSS_INIT(RADIUS_Port, "1812");
		MBSS_INIT(TxRate, "0");
		//MBSS_INIT(HideSSID, "0");
		MBSS_INIT(PreAuth, "0");
		MBSS_INIT(WmmCapable, "1");
		for (i = old_num + 1; i <= new_num; i++) {
			nvram_bufset(nvram, racat("WPAPSK", i), "12345678");
			nvram_bufset(nvram, racat("Key1Str", i), "");
			nvram_bufset(nvram, racat("Key2Str", i), "");
			nvram_bufset(nvram, racat("Key3Str", i), "");
			nvram_bufset(nvram, racat("Key4Str", i), "");
			// The index of AccessPolicy & AccessControlList starts at 0.
			nvram_bufset(nvram, racat("AccessPolicy", i-1), "0");
			nvram_bufset(nvram, racat("AccessControlList", i-1), "");
		}
	}
	else if (new_num < old_num) {
		//MBSS_REMOVE(SSID);
		MBSS_REMOVE(AuthMode);
		MBSS_REMOVE(EncrypType);
		//MBSS_REMOVE(WPAPSK);
		MBSS_REMOVE(DefaultKeyID);
		MBSS_REMOVE(Key1Type);
		//MBSS_REMOVE(Key1Str);
		MBSS_REMOVE(Key2Type);
		//MBSS_REMOVE(Key2Str);
		MBSS_REMOVE(Key2Type);
		//MBSS_REMOVE(Key2Str);
		MBSS_REMOVE(Key3Type);
		//MBSS_REMOVE(Key3Str);
		MBSS_REMOVE(Key4Type);
		//MBSS_REMOVE(Key4Str);
		/*      MBSS_REMOVE(AccessPolicy0);
		        MBSS_REMOVE(AccessControlList0);
		        MBSS_REMOVE(AccessPolicy1);
		        MBSS_REMOVE(AccessControlList1);
		        MBSS_REMOVE(AccessPolicy2);
		        MBSS_REMOVE(AccessControlList2);
		        MBSS_REMOVE(AccessPolicy3);
		        MBSS_REMOVE(AccessControlList3); */
		MBSS_REMOVE(NoForwarding);
		MBSS_REMOVE(NoForwardingBTNBSSID);
		MBSS_REMOVE(IEEE8021X);
		MBSS_REMOVE(RADIUS_Server);
		MBSS_REMOVE(RADIUS_Port);
		MBSS_REMOVE(TxRate);
		MBSS_REMOVE(HideSSID);
		MBSS_REMOVE(PreAuth);
		MBSS_REMOVE(WmmCapable);
		for (i = new_num + 1; i <= old_num; i++) {
			nvram_bufset(nvram, racat("SSID", i), "");
			nvram_bufset(nvram, racat("WPAPSK", i), "");
			nvram_bufset(nvram, racat("Key1Str", i), "");
			nvram_bufset(nvram, racat("Key2Str", i), "");
			nvram_bufset(nvram, racat("Key3Str", i), "");
			nvram_bufset(nvram, racat("Key4Str", i), "");
			// The index of AccessPolicy & AccessControlList starts at 0.
			nvram_bufset(nvram, racat("AccessPolicy", i-1), "0");
			nvram_bufset(nvram, racat("AccessControlList", i-1), "");
		}
	}
}

void set_wifi_basic(int nvram, char *input)
{
	char *value;
	char *wirelessmode, *hssid, *isolated_ssid;
#if defined (RT2860_NEW_MBSS_SUPPORT) || defined (RTDEV_NEW_MBSS_SUPPORT)
	char    hidden_ssid[32], noforwarding[32];
#else
	char    hidden_ssid[16], noforwarding[16];
#endif
	char *sz11aChannel, *sz11bChannel, *sz11gChannel;
	char *abg_rate, *n_mcs;
	int i, new_bssid_num = 0, old_bssid_num, is_n = 0, is_ac = 0;
	char bssid_num[3];
	int max_mbssid = 1, old_AutoChannelSelect;
	char ifprefix[4];

#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM) {
		strcpy(ifprefix, "ra");	
#if defined (RT2860_NEW_MBSS_SUPPORT)
		max_mbssid = 16;
#elif defined (RT2860_MBSS_SUPPORT)
		max_mbssid = 8;
#endif
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM) {
		strcpy(ifprefix, "rai");	
#if defined (RTDEV_NEW_MBSS_SUPPORT)
		max_mbssid = 16;
#elif defined (RTDEV_MBSS_SUPPORT)
		max_mbssid = 8;
#endif
	}
#endif

	value = web_get("radiohiddenButton", input, 0);
	if (!strncmp(value, "0", 2)) {
		do_system("iwpriv %s0 set RadioOn=0", ifprefix);
		nvram_set(nvram, "RadioOff", "1");
		nvram_commit(nvram);
		web_redirect(getenv("HTTP_REFERER"));
		return;
	}
	else if (!strncmp(value, "1", 2)) {
		do_system("iwpriv %s0 set RadioOn=1", ifprefix);
		nvram_set(nvram, "RadioOff", "0");
		nvram_commit(nvram);
		web_redirect(getenv("HTTP_REFERER"));
		return;
	}
	value = web_get("wifihiddenButton", input, 0);
	if (!strncmp(value, "0", 2)) {
#if defined (RT2860_MBSS_SUPPORT) || defined (RTDEV_MBSS_SUPPORT)
		int bssid_num = strtol(nvram_bufget(nvram, "BssidNum"), NULL, 10);
		do {
			bssid_num--;
			do_system("ifconfig %s%d down", ifprefix, bssid_num);
		} while (bssid_num > 0);
#else
		do_system("ifconfig %s0 down", ifprefix);
#endif
		//do_system("rmmod rt2860v2_ap");
		nvram_set(nvram, "WiFiOff", "1");
		nvram_commit(nvram);
		web_redirect(getenv("HTTP_REFERER"));
		return;
	}
	else if (!strncmp(value, "1", 2)) {
		do_system("insmod -q rt2860v2_ap");
#if defined (RT2860_MBSS_SUPPORT) || defined (RTDEV_MBSS_SUPPORT)
		int idx = 0;
		int bssid_num = strtol(nvram_bufget(nvram, "BssidNum"), NULL, 10);
		do {
			do_system("ifconfig %s%d up", ifprefix, idx);
			idx++;
		} while (idx < bssid_num);
#else
		do_system("ifconfig %s0 up", ifprefix);
#endif
		nvram_set(nvram, "WiFiOff", "0");
		nvram_commit(nvram);
		web_redirect(getenv("HTTP_REFERER"));
		return;
	}

	wirelessmode = hssid = isolated_ssid = NULL;
	sz11aChannel = sz11bChannel = sz11gChannel = NULL;
	abg_rate = n_mcs = NULL;
	web_debug_header();
	//fetch from web input
	hssid = strdup(web_get("hssid", input, 1));
	isolated_ssid = strdup(web_get("isolated_ssid", input, 1));
	for (i = 0, value = web_get(racat("mssid_", i), input, 1); 
	     i < max_mbssid && strlen(value) > 0; 
	     i++, value = web_get(racat("mssid_", i), input, 1)) {
		new_bssid_num++;
		nvram_bufset(nvram, racat("SSID", new_bssid_num), value);
		if (new_bssid_num > 1) {
			sprintf(hidden_ssid, "%s;", hidden_ssid);
			sprintf(noforwarding, "%s;", hidden_ssid);
		} else {
			sprintf(hidden_ssid, "");
			sprintf(noforwarding, "");
		}
		if (strchr(hssid, i+0x30) != NULL)
			sprintf(hidden_ssid, "%s%s", hidden_ssid, "1");
		else
			sprintf(hidden_ssid, "%s%s", hidden_ssid, "0");
		if (strchr(isolated_ssid, i+0x30) != NULL)
			sprintf(noforwarding, "%s%s", hidden_ssid, "1");
		else
			sprintf(noforwarding, "%s%s", hidden_ssid, "0");
	}
	if (new_bssid_num < 1) {
		DBG_MSG("'SSID' should not be empty!");
		goto leave;
	}
	wirelessmode = strdup(web_get("wirelessmode", input, 1)); //9: bgn mode
	WebTF(input, "mbssidapisolated", nvram, "NoForwardingBTNBSSID", 1);
	sz11aChannel = strdup(web_get("sz11aChannel", input, 1));
	sz11bChannel = strdup(web_get("sz11bChannel", input, 1));
	sz11gChannel = strdup(web_get("sz11gChannel", input, 1));
	abg_rate = strdup(web_get("abg_rate", input, 1));
	WebTF(input, "tx_stream", nvram, "HT_TxStream", 1);
	WebTF(input, "rx_stream", nvram, "HT_RxStream", 1);

	old_bssid_num = strtol(nvram_bufget(nvram, "BssidNum"), NULL, 10);

	nvram_bufset(nvram, "WirelessMode", wirelessmode);
	//BasicRate: bg,bgn,n:15, b:3; g,gn:351
	if (!strncmp(wirelessmode, "4", 2) || !strncmp(wirelessmode, "7", 2)) //g, gn
		nvram_bufset(nvram, "BasicRate", "351");
	else if (!strncmp(wirelessmode, "1", 2)) //b
		nvram_bufset(nvram, "BasicRate", "3");
	else //bg,bgn,n
		nvram_bufset(nvram, "BasicRate", "15");

	if (!strncmp(wirelessmode, "8", 2) || !strncmp(wirelessmode, "9", 2) ||
	    !strncmp(wirelessmode, "6", 2) || !strncmp(wirelessmode, "11", 3))
		is_n = 1;
#if defined (RT2860_AC_SUPPORT) || defined (RTDEV_AC_SUPPORT)
	else if (!strncmp(wirelessmode, "14", 2) || !strncmp(wirelessmode, "15", 2))
		is_ac = 1;
#endif

	//#WPS
	if (!strcmp(hidden_ssid, "1")) {
		nvram_bufset(nvram, "WscModeOption", "0");
		do_system("miniupnpd.sh init");
		do_system("route delete 239.255.255.250");
	} else {
		const char *wordlist= nvram_bufget(nvram, "WscModeOption");
		if(wordlist){
			if (strcmp(wordlist, "0"))
				do_system("iwpriv %s0 set WscConfStatus=1", ifprefix);
			nvram_bufset(nvram, "WscConfigured", "1");
		}
	}

	sprintf(bssid_num, "%d", new_bssid_num);
	nvram_bufset(nvram, "BssidNum", bssid_num);
#if ! defined CONFIG_FIRST_IF_NONE 
#if defined (RT2860_NEW_MBSS_SUPPORT)
	if ((nvram == RT2860_NVRAM) && 
	 	(new_bssid_num < 1 || new_bssid_num > 16)) {
#else
	if ((nvram == RT2860_NVRAM) && 
		(new_bssid_num < 1 || new_bssid_num > 8)) {
#endif
		DBG_MSG("'bssid_num' %d is out of range!", new_bssid_num);
		goto leave;
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
#if defined (RTDEV_NEW_MBSS_SUPPORT)
	if ((nvram == RTDEV_NVRAM) && 
		(new_bssid_num < 1 || new_bssid_num > 16)) {
#else
	if ((nvram == RTDEV_NVRAM) && 
		(new_bssid_num < 1 || new_bssid_num > 8)) {
#endif
		DBG_MSG("'bssid_num' %d is out of range!", new_bssid_num);
		goto leave;
	}
#endif
	revise_mbss_value(nvram, old_bssid_num, new_bssid_num);

	//Broadcast SSID
	nvram_bufset(nvram, "HideSSID", hidden_ssid);

	// NoForwarding and NoForwardingBTNBSSID
	nvram_bufset(nvram, "NoForwarding", noforwarding);

	//11abg Channel or AutoSelect
	if ((0 == strlen(sz11aChannel)) && (0 == strlen(sz11bChannel)) &&
	    (0 == strlen(sz11gChannel))) {
		DBG_MSG("'Channel' should not be empty!");
		goto leave;
	}
	if (!strncmp(sz11aChannel, "0", 2) || !strncmp(sz11bChannel, "0", 2) ||
	    !strncmp(sz11gChannel, "0", 2)) {
		old_AutoChannelSelect = strtol(nvram_bufget(nvram, "AutoChannelSelect"), NULL, 10);
		if (old_AutoChannelSelect == 0)
			nvram_bufset(nvram, "AutoChannelSelect", "3");
	} else {
		nvram_bufset(nvram, "AutoChannelSelect", "0");
	}
	if (0 != strlen(sz11aChannel))
	{
		nvram_bufset(nvram, "Channel", sz11aChannel);
		do_system("iwpriv %s0 set Channel=%s", ifprefix, sz11aChannel);
	}
	if (0 != strlen(sz11bChannel))
	{
		nvram_bufset(nvram, "Channel", sz11bChannel);
		do_system("iwpriv %s0 set Channel=%s", ifprefix, sz11bChannel);
	}
	if (0 != strlen(sz11gChannel))
	{
		nvram_bufset(nvram, "Channel", sz11gChannel);
		do_system("iwpriv %s0 set Channel=%s", ifprefix, sz11gChannel);
	}
	sleep(1);

	//Rate for a, b, g
	if (strncmp(abg_rate, "", 1)) {
		int rate = strtol(abg_rate, NULL, 10);
		if (!strncmp(wirelessmode, "0", 2) || !strncmp(wirelessmode, "2", 2) || !strncmp(wirelessmode, "4", 2)) {
			if (rate == 1 || rate == 2 || rate == 5 || rate == 11)
				nvram_bufset(nvram, "FixedTxMode", "CCK");
			else
				nvram_bufset(nvram, "FixedTxMode", "OFDM");

			if (rate == 1)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "0");
			else if (rate == 2)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "1");
			else if (rate == 5)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "2");
			else if (rate == 6)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "0");
			else if (rate == 9)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "1");
			else if (rate == 11)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "3");
			else if (rate == 12)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "2");
			else if (rate == 18)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "3");
			else if (rate == 24)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "4");
			else if (rate == 36)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "5");
			else if (rate == 48)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "6");
			else if (rate == 54)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "7");
			else
				SetAllValues(new_bssid_num, nvram, HT_MCS, "33");
		}
		else if (!strncmp(wirelessmode, "1", 2)) {
			nvram_bufset(nvram, "FixedTxMode", "CCK");
			if (rate == 1)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "0");
			else if (rate == 2)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "1");
			else if (rate == 5)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "2");
			else if (rate == 11)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "3");
		}
		else //shall not happen
			DBG_MSG("wrong configurations from web UI");
	}
	else
		nvram_bufset(nvram, "FixedTxMode", "HT");

	//HT_OpMode, HT_BW, HT_GI, HT_MCS, HT_RDG, HT_EXTCHA, HT_AMSDU, HT_TxStream, HT_RxStream
	if (is_n | is_ac) {
		WebTF(input, "n_mode", nvram, "HT_OpMode", 1);
		WebTF(input, "n_bandwidth", nvram, "HT_BW", 1);
		WebTF(input, "n_gi", nvram, "HT_GI", 1);
		n_mcs = strdup(web_get("n_mcs", input, 1));
		SetAllValues(new_bssid_num, nvram, HT_MCS, n_mcs);
		WebTF(input, "n_rdg", nvram, "HT_RDG", 1);
		WebTF(input, "n_extcha", nvram, "HT_EXTCHA", 1);
		WebTF(input, "n_stbc", nvram, "HT_STBC", 1);
		WebTF(input, "n_amsdu", nvram, "HT_AMSDU", 1);
		WebTF(input, "n_autoba", nvram, "HT_AutoBA", 1);
		WebTF(input, "n_badecline", nvram, "HT_BADecline", 1);
		WebTF(input, "n_disallow_tkip", nvram, "HT_DisallowTKIP", 1);
		WebTF(input, "n_2040_coexit", nvram, "HT_BSSCoexistence", 1);
		WebTF(input, "n_ldpc", nvram, "HT_LDPC", 1);
	}
#if defined (RT2860_AC_SUPPORT) || defined (RTDEV_AC_SUPPORT)
	if (is_ac) {
		WebTF(input, "vht_bandwidth", nvram, "VHT_BW", 1);
		WebTF(input, "vht2nd11aChannel", nvram, "VHT_Sec80_Channel", 1);
		WebTF(input, "vht_stbc", nvram, "VHT_STBC", 1);
		WebTF(input, "vht_sgi", nvram, "VHT_SGI", 1);
		WebTF(input, "vht_bw_signal", nvram, "VHT_BW_SIGNAL", 1);
		WebTF(input, "vht_ldpc", nvram, "VHT_LDPC", 1);
	}
#endif

	if (!strncmp(wirelessmode, "6", 2) || !strncmp(wirelessmode, "11", 2))
	{
		int mbssid_num = strtol(nvram_bufget(nvram, "BssidNum"), NULL, 10);
		int i = 0;

		while (i < mbssid_num)
		{
			char tmp[128];
			char *EncrypType = (char *) nvram_bufget(nvram, "EncrypType");
			get_nth_value(i, EncrypType, ';', tmp, 128);
			if (!strcmp(tmp, "NONE") || !strcmp(tmp, "AES"))
			{
				memset(tmp, 0, 128);
				char *IEEE8021X = (char *) nvram_bufget(nvram, "IEEE8021X");
				get_nth_value(i, IEEE8021X, ';', tmp, 128);
				if (!strcmp(tmp, "0"))
				{
					i++;
					continue;
				}
			}
			set_nth_value_flash(nvram, i, "AuthMode", "OPEN");
			set_nth_value_flash(nvram, i, "EncrypType", "NONE");
			set_nth_value_flash(nvram, i, "IEEE8021X", "0");
			i++;
		}
	}
	nvram_commit(nvram);
	/*  this is a workaround:
	 *  when AP is built as kernel
	 *  if more ssids are created, driver won't exe RT28xx_MBSS_Init again
	 *  therefore, we reboot to make it available
	 *  (PS. CONFIG_MIPS would be "y")
	 */
	if (new_bssid_num != old_bssid_num)
		do_system("reboot");

	do_system("init_system restart");
leave:
	free_all(8, hssid, isolated_ssid, wirelessmode, 
		    sz11aChannel, sz11bChannel, sz11gChannel,
		    abg_rate, n_mcs);
}

void set_wifi_advance(int nvram, char *input)
{
	char 	*wmm_capable, *countrycode, *countryregion;
	char 	wmm_enable[16];
#if defined CONFIG_RA_CLASSIFIER_MODULE && defined CONFIG_RT2860V2_AP_VIDEO_TURBINE && defined (RT2860_TXBF_SUPPORT)
	char  	*rvt = NULL;
	int     oldrvt;
	const char 	*oldrx, *oldtx;
#endif // getRVTBuilt //
#if defined (RT2860_TXBF_SUPPORT) || defined (RTDEV_TXBF_SUPPORT)
	char  	*txbf = NULL;
#endif // TXBF //
	int     i, ssid_num, wlan_mode;

	wmm_capable = countrycode = NULL;
	web_debug_header();
	//fetch from web input
	WebTF(input, "bg_protection", nvram, "BGProtection", 1);
	// WebTF(input, "basic_rate", nvram, "BasicRate", 1);
	WebTF(input, "beacon", nvram, "BeaconPeriod", 1);
	WebTF(input, "dtim", nvram, "DtimPeriod", 1);
	WebTF(input, "fragment", nvram, "FragThreshold", 1);
	WebTF(input, "rts", nvram, "RTSThreshold", 1);
	WebTF(input, "tx_power", nvram, "TxPower", 1);
	WebTF(input, "short_preamble", nvram, "TxPreamble", 1);
	WebTF(input, "short_slot", nvram, "ShortSlot", 1);
	WebTF(input, "tx_burst", nvram, "TxBurst", 1);
	WebTF(input, "pkt_aggregate", nvram, "PktAggregate", 1);
	WebTF(input, "ieee_80211h", nvram, "IEEE80211H", 1);
	wmm_capable = strdup(web_get("wmm_capable", input, 1));
	WebTF(input, "apsd_capable", nvram, "APSDCapable", 1);
	WebTF(input, "dls_capable", nvram, "DLSCapable", 1);
	countrycode = strdup(web_get("country_code", input, 1));
	if(!strncmp(countrycode, "NONE", 5)) countrycode = "";
	countryregion = strdup(web_get("country_region", input, 1));
	WebTF(input, "m2u_enable", nvram, "M2UEnabled", 1);
	WebTF(input, "rd_region", nvram, "RDRegion", 1);
	WebTF(input, "carrier_detect", nvram, "CarrierDetect", 1);
	WebTF(input, "ieee_80211h", nvram, "IEEE80211H", 1);
	WebTF(input, "ieee_80211h", nvram, "IEEE80211H", 1);
#ifdef CONFIG_RT2860V2_AUTO_PROVISION
	WebTF(input, "auto_provision", nvram, "AutoProvisionEn", 1);
#endif
#if defined CONFIG_RA_CLASSIFIER_MODULE && defined CONFIG_RT2860V2_AP_VIDEO_TURBINE && defined (RT2860_TXBF_SUPPORT)
	rvt = strdup(web_get("rvt", input, 1));
#endif // getRVTBuilt //
#if defined (RT2860_TXBF_SUPPORT) || defined (RTDEV_TXBF_SUPPORT)
	txbf = strdup(web_get("txbf", input, 1));
#endif // TXBF //

	if (NULL != nvram_bufget(nvram, "BssidNum"))
		ssid_num = strtol(nvram_bufget(nvram, "BssidNum"), NULL, 10);
	else
		ssid_num = 1;
	wlan_mode = strtol(nvram_bufget(nvram, "WirelessMode"), NULL, 10);
#if defined CONFIG_RA_CLASSIFIER_MODULE && defined CONFIG_RT2860V2_AP_VIDEO_TURBINE && defined (RT2860_TXBF_SUPPORT)
	if (NULL != nvram_bufget(nvram, "RVT"))
		oldrvt = strtol(nvram_bufget(nvram, "RVT"), NULL, 10);
	else
		oldrvt = 0;
#endif // getRVTBuilt //

	//set to nvram
#if defined CONFIG_RA_CLASSIFIER_MODULE && defined CONFIG_RT2860V2_AP_VIDEO_TURBINE && defined (RT2860_TXBF_SUPPORT)
	if (!strncmp(rvt, "1", 2) && oldrvt != 1) {
		//backup the previous configurations for Rx/TxStream
		oldrx = nvram_bufget(nvram, "HT_RxStream");
		oldtx = nvram_bufget(nvram, "HT_TxStream");
		nvram_bufset(nvram, "HT_RxStream_old", oldrx);
		nvram_bufset(nvram, "HT_TxStream_old", oldtx);

		nvram_bufset(nvram, "RVT", "1");
		nvram_bufset(nvram, "HT_RxStream", "1");
		nvram_bufset(nvram, "HT_TxStream", "1");
		nvram_bufset(nvram, "HT_GI", "0");
		nvram_bufset(nvram, "VideoTurbine", "1");
	}
	else if (!strncmp(rvt, "0", 2) && oldrvt != 0) {
		//restore the previous configurations for Rx/TxStream
		oldrx = nvram_bufget(nvram, "HT_RxStream_old");
		oldtx = nvram_bufget(nvram, "HT_TxStream_old");

		nvram_bufset(nvram, "RVT", "0");
		nvram_bufset(nvram, "HT_RxStream", oldrx);
		nvram_bufset(nvram, "HT_TxStream", oldtx);
		nvram_bufset(nvram, "HT_GI", "1");
		nvram_bufset(nvram, "VideoTurbine", "0");
	}
#endif // getRVTBuilt //
#if defined (RT2860_TXBF_SUPPORT) || defined (RTDEV_TXBF_SUPPORT)
	if (!strncmp(txbf, "3", 2)) {
		nvram_bufset(nvram, "ETxBfEnCond", "1");
		nvram_bufset(nvram, "ETxBfIncapable", "0");
		nvram_bufset(nvram, "ITxBfEn", "1");
	}
	else if (!strncmp(txbf, "2", 2)) {
		nvram_bufset(nvram, "ETxBfEnCond", "1");
		nvram_bufset(nvram, "ETxBfIncapable", "0");
		nvram_bufset(nvram, "ITxBfEn", "0");
	}
	else if (!strncmp(txbf, "1", 2)) {
		nvram_bufset(nvram, "ETxBfEnCond", "0");
		nvram_bufset(nvram, "ETxBfIncapable", "1");
		nvram_bufset(nvram, "ITxBfEn", "1");
	}
	else {
		nvram_bufset(nvram, "ETxBfEnCond", "0");
		nvram_bufset(nvram, "ETxBfIncapable", "1");
		nvram_bufset(nvram, "ITxBfEn", "0");
	}
#endif // TXBF //

	bzero(wmm_enable, sizeof(char)*16);
	for (i = 0; i < ssid_num; i++)
	{
		sprintf(wmm_enable+strlen(wmm_enable), "%d", strtol(wmm_capable, NULL, 10));
		sprintf(wmm_enable+strlen(wmm_enable), "%c", ';');
	}
	wmm_enable[strlen(wmm_enable) - 1] = '\0';
	nvram_bufset(nvram, "WmmCapable", wmm_enable);

	if (!strncmp(wmm_capable, "1", 2)) {
		if (wlan_mode < 5)
			nvram_bufset(nvram, "TxBurst", "0");
	}

#if ! defined CONFIG_EXT_CHANNEL_LIST
	if (wlan_mode == 0 || wlan_mode == 1 || wlan_mode == 4 ||
	    wlan_mode == 9 || wlan_mode == 6) {
		if (!strcmp(countryregion, "8"))
			nvram_bufset(nvram, "CountryRegion", "31");
		else if (!strcmp(countryregion, "9"))
			nvram_bufset(nvram, "CountryRegion", "32");
		else if (!strcmp(countryregion, "10"))
			nvram_bufset(nvram, "CountryRegion", "33");
		else
			nvram_bufset(nvram, "CountryRegion", countryregion);
	} else if (wlan_mode == 2 || wlan_mode == 8 || wlan_mode == 11 ||
		   wlan_mode == 14 || wlan_mode == 15) {
		nvram_bufset(nvram, "CountryRegionABand", countryregion);
	}
#endif
	nvram_bufset(nvram, "CountryCode", countrycode);

	nvram_commit(nvram);
	do_system("init_system restart");

	free_all(3, wmm_capable, countrycode, countryregion);
#if defined CONFIG_RA_CLASSIFIER_MODULE && defined CONFIG_RT2860V2_AP_VIDEO_TURBINE && defined (RT2860_TXBF_SUPPORT)
	free(rvt);
#endif
#if defined (RT2860_TXBF_SUPPORT) || defined (RTDEV_TXBF_SUPPORT)
	free(txbf);
#endif
}

void set_wifi_wmm(int nvram, char *input)
{
	WebTF(input, "ap_aifsn_all", nvram, "APAifsn", 0);
	WebTF(input, "ap_cwmin_all", nvram, "APCwmin", 0);
	WebTF(input, "ap_cwmax_all", nvram, "APCwmax", 0);
	WebTF(input, "ap_txop_all", nvram, "APTxop", 0);
	WebTF(input, "ap_acm_all", nvram, "APACM", 0);
	WebTF(input, "ap_ackpolicy_all", nvram, "AckPolicy", 0);
	WebTF(input, "sta_aifsn_all", nvram, "BSSAifsn", 0);
	WebTF(input, "sta_cwmin_all", nvram, "BSSCwmin", 0);
	WebTF(input, "sta_cwmax_all", nvram, "BSSCwmax", 0);
	WebTF(input, "sta_txop_all", nvram, "BSSTxop", 0);
	WebTF(input, "sta_acm_all", nvram, "BSSACM", 0);

	 nvram_commit(nvram);
	 do_system("init_system restart");

	 web_back_parentpage();
}

static void clear_radius_setting(int nvram, int mbssid)
{
	set_nth_value_flash(nvram, mbssid, "IEEE8021X", "0");
	set_nth_value_flash(nvram, mbssid, "RADIUS_Server", "0");
	set_nth_value_flash(nvram, mbssid, "RADIUS_Port", "1812");
	set_nth_value_flash(nvram, mbssid, racat("RADIUS_Key", mbssid+1), "ralink");
	// set_nth_value_flash(nvram, mbssid, "session_timeout_interval", "");
}

void conf_wep(char *input, int nvram, int mbssid)
{
	WebNthTF(input, "wep_default_key", nvram, mbssid, "DefaultKeyID", 1);
	WebTF(input, "wep_key_1", nvram, racat("Key1Str", mbssid+1), 1);
	WebTF(input, "wep_key_2", nvram, racat("Key2Str", mbssid+1), 1);
	WebTF(input, "wep_key_3", nvram, racat("Key3Str", mbssid+1), 1);
	WebTF(input, "wep_key_4", nvram, racat("Key4Str", mbssid+1), 1);
	WebNthTF(input, "WEP1Select", nvram, mbssid, "Key1Type", 1);
	WebNthTF(input, "WEP2Select", nvram, mbssid, "Key2Type", 1);
	WebNthTF(input, "WEP3Select", nvram, mbssid, "Key3Type", 1);
	WebNthTF(input, "WEP4Select", nvram, mbssid, "Key4Type", 1);
}

void conf_8021x(char *input, int nvram, int mbssid)
{
	char	*rs_session_to;

	WebNthTF(input, "RadiusServerIP", nvram, mbssid, "RADIUS_Server", 1);
	WebNthTF(input, "RadiusServerPort", nvram, mbssid, "RADIUS_Port", 1);
	WebTF(input, "RadiusServerSecret", nvram, racat("RADIUS_Key", mbssid+1), 1);
	rs_session_to = web_get("RadiusServerSessionTimeout", input, 1);
	if(!strlen(rs_session_to))
		rs_session_to = "0";
	set_nth_value_flash(nvram, mbssid, "session_timeout_interval", rs_session_to);

	update_flash_8021x(nvram);
}

void conf_wapi_general(char *input, int nvram, int mbssid)
{
	char *cipher_str = web_get("cipher", input, 1);

	switch(cipher_str[0]){
	case '0':
		set_nth_value_flash(nvram, mbssid, "EncrypType", "TKIP");
		break;
	case '1':
		set_nth_value_flash(nvram, mbssid, "EncrypType", "AES");
		break;
	case '2':
		set_nth_value_flash(nvram, mbssid, "EncrypType", "TKIPAES");
	}
	set_nth_value_flash(nvram, mbssid, "DefaultKeyID", "2");	// DefaultKeyID is 2
	WebNthTF(input, "keyRenewalInterval", nvram, mbssid, "RekeyInterval", 1);
	set_nth_value_flash(nvram, mbssid, "RekeyMethod", "TIME");
	set_nth_value_flash(nvram, mbssid, "IEEE8021X", "0");
}

int access_policy_handle(char *input, int nvram, int mbssid)
{
	char *apselect, *newap_list;
	char ap_list[2048];

	if(mbssid > 8 || mbssid < 0)
		return -1;

	apselect = web_get(racat("apselect_", mbssid), input, 1);
	if(!strlen(apselect)){
		DBG_MSG("cant find %s", apselect);
		return -1;
	}
	nvram_bufset(nvram, racat("AccessPolicy", mbssid), apselect);

	newap_list = web_get(racat("newap_text_", mbssid), input, 1);
	if(!newap_list)
		return -1;
	if(!strlen(newap_list))
		return 0;
	strcpy(ap_list, nvram_bufget(nvram, racat("AccessControlList", mbssid)));
	if(strlen(ap_list))
		sprintf(ap_list, "%s;%s", ap_list, newap_list);
	else
		sprintf(ap_list, "%s", newap_list);

	//DBG_MSG("%s", ap_list);
	nvram_bufset(nvram, racat("AccessControlList", mbssid), ap_list);
	return 0;
}

#ifdef CONFIG_USER_HOTSPOT
void restart_hotspot_daemon()
{
    do_system("killall hs");
    sleep(3);
    //do_system("hs -d 3");
    do_system("hs -d 1 -v 2");
    do_system("hs -d 1 -v 2 -f/etc_ro/hotspot_ap2.conf -Irai0");
    sleep(3);
}

#endif /* CONFIG_USER_HOTSPOT */
void set_wifi_security(int nvram, char *input)
{
	char 	*value;
	int 	mbssid;
	char 	*security_mode;
	char 	ifprefix[4];
#if defined (RT2860_DOT11W_PMF_SUPPORT) || defined (RTDEV_DOT11W_PMF_SUPPORT)
	char    *PMFMFPC;
	char    *PMFMFPR;
	char    *PMFSHA256;
#endif

#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM) {
		strcpy(ifprefix, "ra");	
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM) {
		strcpy(ifprefix, "rai");	
	}
#endif
	security_mode = NULL;
	web_debug_header();
	WebTF(input, "auto_provision", nvram, "AutoProvisionEn", 1);
	value = web_get("ssidIndex", input, 1);
	if(!strlen(value))
		return;

	mbssid = strtol(value, NULL, 10);
	security_mode = strdup(web_get("security_mode", input, 1));

	//clear Radius settings
	clear_radius_setting(nvram, mbssid);

	if ((!strcmp(security_mode, "OPEN")) || 	// OPEN WEP Mode
	    (!strcmp(security_mode, "SHARED")) ||	// SHARE WEP Mode
	    (!strcmp(security_mode, "WEPAUTO"))) {      // AUTO WEP Mode
		conf_wep(input, nvram,  mbssid);
		set_nth_value_flash(nvram, mbssid, "AuthMode", security_mode);
		set_nth_value_flash(nvram, mbssid, "EncrypType", "WEP");
		nvram_bufset(nvram, "WscModeOption", "0");
		do_system("miniupnpd.sh init");
		do_system("route delete 239.255.255.250");
	} else if ((!strcmp(security_mode, "WPAPSK")) ||		// WPA Personal Mode
		   (!strcmp(security_mode, "WPAPSKWPA2PSK"))) {      	// WPA/WPA2 mixed Personal Mode
		conf_wapi_general(input, nvram, mbssid);
		set_nth_value_flash(nvram, mbssid, "AuthMode", security_mode);
		WebTF(input, "passphrase", nvram, racat("WPAPSK", mbssid+1), 1);
	} else if (!strcmp(security_mode, "WPA2PSK")) {			// WPA2 Personal Mode
		conf_wapi_general(input, nvram, mbssid);
		set_nth_value_flash(nvram, mbssid, "AuthMode", security_mode);
		WebTF(input, "passphrase", nvram, racat("WPAPSK", mbssid+1), 1);
#if defined (RT2860_DOT11W_PMF_SUPPORT) || defined (RTDEV_DOT11W_PMF_SUPPORT)
		PMFMFPC = strdup(web_get("PMFMFPC", input, 1));
		PMFMFPR = strdup(web_get("PMFMFPR", input, 1));
		PMFSHA256 = strdup(web_get("PMFSHA256", input, 1));
		set_nth_value_flash(nvram, mbssid, "PMFMFPC", PMFMFPC);
		set_nth_value_flash(nvram, mbssid, "PMFMFPR", PMFMFPR);
		set_nth_value_flash(nvram, mbssid, "PMFSHA256", PMFSHA256);
#endif
	} else if (!strcmp(security_mode, "WPA")) {             // WPA Enterprise Mode
		conf_8021x(input, nvram, mbssid);
		conf_wapi_general(input, nvram, mbssid);
		set_nth_value_flash(nvram, mbssid, "AuthMode", security_mode);
	} else if (!strcmp(security_mode, "WPA2")) {            // WPA2 Enterprise Mode
		conf_8021x(input, nvram, mbssid);
		conf_wapi_general(input, nvram, mbssid);
		set_nth_value_flash(nvram, mbssid, "AuthMode", security_mode);
		WebNthTF(input, "PMKCachePeriod", nvram, mbssid, "PMKCachePeriod", 1);
		WebNthTF(input, "PreAuthentication", nvram, mbssid, "PreAuth", 1);
#if defined (RT2860_DOT11W_PMF_SUPPORT) || defined (RTDEV_DOT11W_PMF_SUPPORT)
		PMFMFPC = strdup(web_get("PMFMFPC", input, 1));
		PMFMFPR = strdup(web_get("PMFMFPR", input, 1));
		PMFSHA256 = strdup(web_get("PMFSHA256", input, 1));
		set_nth_value_flash(nvram, mbssid, "PMFMFPC", PMFMFPC);
		set_nth_value_flash(nvram, mbssid, "PMFMFPR", PMFMFPR);
		set_nth_value_flash(nvram, mbssid, "PMFSHA256", PMFSHA256);
#endif
	} else if (!strcmp(security_mode, "WPA1WPA2")) {        // WPA Enterprise Mode
		conf_8021x(input, nvram, mbssid);
		conf_wapi_general(input, nvram, mbssid);
		set_nth_value_flash(nvram, mbssid, "AuthMode", security_mode);
		WebNthTF(input, "PMKCachePeriod", nvram, mbssid, "PMKCachePeriod", 1);
		WebNthTF(input, "PreAuthentication", nvram, mbssid, "PreAuth", 1);
	} else if (!strcmp(security_mode, "IEEE8021X")) {       // 802.1X WEP Mode
		char *ieee8021x_wep = web_get("ieee8021x_wep", input, 1);

		set_nth_value_flash(nvram, mbssid, "IEEE8021X", "1");
		set_nth_value_flash(nvram, mbssid, "AuthMode", "OPEN");
		if(ieee8021x_wep[0] == '0')
			set_nth_value_flash(nvram, mbssid, "EncrypType", "NONE");
		else
			set_nth_value_flash(nvram, mbssid, "EncrypType", "WEP");
		conf_8021x(input, nvram, mbssid);
	}
#if defined (RT2860_WAPI_SUPPORT) || defined (RTDEV_WAPI_SUPPORT)
	else if (!strcmp(security_mode, "WAICERT")) {           // WAPI-CERT Mode
		set_nth_value_flash(nvram, mbssid, "AuthMode", security_mode);
		set_nth_value_flash(nvram, mbssid, "EncrypType", "SMS4");
		nvram_bufset(nvram, "Wapiifname", "br0");
		WebTF(input, "wapicert_asipaddr", nvram, "WapiAsIpAddr", 1);
		nvram_bufset(nvram, "WapiAsPort", "3810");
		WebTF(input, "wapicert_ascert", nvram, "WapiAsCertPath", 1);
		WebTF(input, "wapicert_usercert", nvram, "WapiUserCertPath", 1);
	} else if (!strcmp(security_mode, "WAIPSK")) {          // WAPI-PSK Mode
		set_nth_value_flash(nvram, mbssid, "AuthMode", security_mode);
		set_nth_value_flash(nvram, mbssid, "EncrypType", "SMS4");
		WebNthTF(input, "wapipsk_keytype", nvram, mbssid, "WapiPskType", 1);
		WebTF(input, "wapipsk_prekey", nvram, racat("WapiPsk", mbssid+1), 1);
	}
#endif
	else {                                                  // Open-None Mode
		set_nth_value_flash(nvram, mbssid, "AuthMode", "OPEN");
		set_nth_value_flash(nvram, mbssid, "EncrypType", "NONE");
	}

	//# Access Policy
	if(access_policy_handle(input, nvram, mbssid) == -1)
		DBG_MSG("error");

	//# WPS
	const char *wordlist= nvram_bufget(nvram, "WscModeOption");
	if (strcmp(wordlist, "0")) {
		if (nvram == RT2860_NVRAM)
			do_system("iwpriv %s%d set WscConfStatus=1", ifprefix, 0);
	}
	nvram_bufset(nvram, "WscConfigured", "1");
	nvram_commit(nvram);
	do_system("init_system restart");
	free(security_mode);
#if defined (RT2860_DOT11W_PMF_SUPPORT) || defined (RTDEV_DOT11W_PMF_SUPPORT)
	free_all(3, PMFMFPC, PMFMFPR, PMFSHA256);
#endif
#ifdef CONFIG_USER_HOTSPOT
    restart_hotspot_daemon();
#endif /* CONFIG_USER_HOTSPOT */
}

#if defined (RT2860_WDS_SUPPORT)
void set_wifi_wds(int nvram, char *input)
{
	char	*wds_mode, *wds_list;

	wds_mode = NULL;
	web_debug_header();
	wds_mode = strdup(web_get("wds_mode", input, 1));
	nvram_bufset(nvram, "WdsEnable", wds_mode);
	if (strncmp(wds_mode, "0", 2)) {
		WebTF(input, "wds_phy_mode", nvram, "WdsPhyMode", 1);
		WebNthTF(input, "wds_encryp_type0", nvram, 0, "WdsEncrypType", 1);
		WebNthTF(input, "wds_encryp_type1", nvram, 1, "WdsEncrypType", 1);
		WebNthTF(input, "wds_encryp_type2", nvram, 2, "WdsEncrypType", 1);
		WebNthTF(input, "wds_encryp_type3", nvram, 3, "WdsEncrypType", 1);
		WebTF(input, "wds_encryp_key0", nvram, "Wds0Key", 1);
		WebTF(input, "wds_encryp_key1", nvram, "Wds1Key", 1);
		WebTF(input, "wds_encryp_key2", nvram, "Wds2Key", 1);
		WebTF(input, "wds_encryp_key3", nvram, "Wds3Key", 1);
		if (!strncmp(wds_mode, "2", 2) || !strncmp(wds_mode, "3", 2)) {
			wds_list = web_get("wds_list", input, 1);
			if (0 != strlen(wds_list))
				nvram_bufset(nvram, "WdsList", wds_list);
		}
	}
	nvram_commit(nvram);
	do_system("init_system restart");
	free(wds_mode);
}
#endif

#if defined (RT2860_APCLI_SUPPORT)
void set_wifi_apclient(int nvram, char *input)
{
	char  	*ssid; 

	web_debug_header();
	ssid = web_get("apcli_ssid", input, 1);
	if (strlen(ssid) == 0) {
		DBG_MSG("SSID is empty");
		return;
	}
	nvram_bufset(nvram, "ApCliSsid", ssid);
	nvram_bufset(RT2860_NVRAM, "ApCliEnable", "1");
	WebTF(input, "apcli_bssid", nvram, "ApCliBssid", 1);
	WebTF(input, "apcli_mode", nvram, "ApCliAuthMode", 1);
	WebTF(input, "apcli_enc", nvram, "ApCliEncrypType", 1);
	WebTF(input, "apcli_wpapsk", nvram, "ApCliWPAPSK", 1);
	WebTF(input, "apcli_default_key", nvram, "ApCliDefaultKeyID", 1);
	WebTF(input, "apcli_key1type", nvram, "ApCliKey1Type", 1);
	WebTF(input, "apcli_key2type", nvram, "ApCliKey2Type", 1);
	WebTF(input, "apcli_key3type", nvram, "ApCliKey3Type", 1);
	WebTF(input, "apcli_key4type", nvram, "ApCliKey4Type", 1);
	WebTF(input, "apcli_key1", nvram, "ApCliKey1Str", 1);
	WebTF(input, "apcli_key2", nvram, "ApCliKey2Str", 1);
	WebTF(input, "apcli_key3", nvram, "ApCliKey3Str", 1);
	WebTF(input, "apcli_key4", nvram, "ApCliKey4Str", 1);
	nvram_commit(RT2860_NVRAM);
	do_system("init_system restart");
}
#endif

#if defined (RT2860_HS_SUPPORT) || defined (RTDEV_HS_SUPPORT)
void set_wifi_hotspot(int nvram, char *input)
{
	int index, wmode;
	char *hs_enabled, *internet, *hessid, *roaming_consortium_oi; 
	char *proxy_arp, *dgaf_disabled, *l2_filter, *icmpv4_deny, *nai_enabled, *plmn_enabled;
	char *realm1, *eap1, *realm2, *eap2, *realm3, *eap3, *realm4, *eap4;
	char *mcc1, *mnc1, *mcc2, *mnc2, *mcc3, *mnc3, *mcc4, *mnc4, *mcc5, *mnc5, *mcc6, *mnc6;
	FILE *pp;
	char reg[20];

	web_debug_header();
	index = strtol(web_get("wifiIndex", input, 1), NULL, 10);
	wmode = strtol(nvram_bufget(nvram, "WirelessMode"), NULL, 10);
	hs_enabled = web_get("hs_enabled", input, 1);
	set_nth_value_flash(nvram, index, "HS_enabled", hs_enabled);
	internet = web_get("internet", input, 1);
	set_nth_value_flash(nvram, index, "HS_internet", internet);
	hessid = web_get("hessid", input, 1);
	set_nth_value_flash(nvram, index, "HS_hessid", hessid);
	roaming_consortium_oi = web_get("roaming_consortium_oi", input, 1);
	set_nth_value_flash(nvram, index, "HS_roaming_consortium_oi", roaming_consortium_oi);
	//set_nth_value_flash(nvram, index, "HS_roaming_consortium_oi", "50-6F-9A,00-1B-C5-04-BD");
	if (wmode == 10) {
		set_nth_value_flash(nvram, index, "HS_operating_class", "81,115");
	} else if (wmode == 2 || wmode == 8 || wmode == 11) {
		set_nth_value_flash(nvram, index, "HS_operating_class", "115");
	} else {
		set_nth_value_flash(nvram, index, "HS_operating_class", "81");
	}

	proxy_arp = web_get("proxy_arp", input, 1);
	set_nth_value_flash(nvram, index, "HS_proxy_arp", proxy_arp);
	dgaf_disabled = web_get("dgaf_disabled", input, 1);
	set_nth_value_flash(nvram, index, "HS_dgaf_disabled", dgaf_disabled);
	l2_filter = web_get("l2_filter", input, 1);
	set_nth_value_flash(nvram, index, "HS_l2_filter", l2_filter);
	icmpv4_deny = web_get("icmpv4_deny", input, 1);
	set_nth_value_flash(nvram, index, "HS_icmpv4_deny", icmpv4_deny);
	nai_enabled = web_get("nai_enabled", input, 1);
	set_nth_value_flash(nvram, index, "HS_nai", nai_enabled);
	plmn_enabled = web_get("plmn_enabled", input, 1);
	set_nth_value_flash(nvram, index, "HS_plmn", plmn_enabled);
	realm1 = web_get("realm1", input, 1);
	set_nth_value_flash(nvram, index, "HS_nai1_realm", realm1);
	eap1 = web_get("eap_method1", input, 1);
	set_nth_value_flash(nvram, index, "HS_nai1_eap_method", eap1);
	realm2 = web_get("realm2", input, 1);
	set_nth_value_flash(nvram, index, "HS_nai2_realm", realm2);
	eap2 = web_get("eap_method2", input, 1);
	set_nth_value_flash(nvram, index, "HS_nai2_eap_method", eap2);
	realm3 = web_get("realm3", input, 1);
	set_nth_value_flash(nvram, index, "HS_nai3_realm", realm3);
	eap3 = web_get("eap_method3", input, 1);
	set_nth_value_flash(nvram, index, "HS_nai3_eap_method", eap3);
	realm4 = web_get("realm4", input, 1);
	set_nth_value_flash(nvram, index, "HS_nai4_realm", realm4);
	eap4 = web_get("eap_method4", input, 1);
	set_nth_value_flash(nvram, index, "HS_nai4_eap_method", eap4);
	set_nth_value_flash(nvram, index, "HS_capacity", "0");
#if defined CONFIG_MAC_TO_MAC_MODE
	set_nth_value_flash(nvram, index, "HS_link_status", (get_if_live(get_wanif_name()) == 1)? "1" : "0");
	set_nth_value_flash(nvram, index, "HS_dl_speed", "1000000");
	set_nth_value_flash(nvram, index, "HS_ul_speed", "1000000");
#else
#if defined (CONFIG_RALINK_RT3052) || defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_MT7628)
#ifdef CONFIG_WAN_AT_P0
	int bitmask_link_status = 25;
	int bitmask_speed = 0;
#else
	int bitmask_link_status = 29;
	int bitmask_speed = 4;
#endif
	pp = popen("reg s b0110000; reg p 80", "r");
	fscanf(pp, "%s\n", reg);
	pclose(pp);
	if (((strtoul(reg, NULL, 16)>>bitmask_link_status)&(0x1)) == 0) {
		set_nth_value_flash(nvram, index, "HS_link_status", "0");
		set_nth_value_flash(nvram, index, "HS_dl_speed", "0");
		set_nth_value_flash(nvram, index, "HS_ul_speed", "0");
	} else {
		int link_speed = (strtoul(reg, NULL, 16)>>bitmask_speed)&(0x1);
		set_nth_value_flash(nvram, index, "HS_link_status", "1");
		if (link_speed == 0x1) {
			set_nth_value_flash(nvram, index, "HS_dl_speed", "100000");
			set_nth_value_flash(nvram, index, "HS_ul_speed", "100000");
		} else {
			set_nth_value_flash(nvram, index, "HS_dl_speed", "10000");
			set_nth_value_flash(nvram, index, "HS_ul_speed", "10000");
		}
	}
#else
#ifdef CONFIG_WAN_AT_P0
	pp = popen("switch reg r 3008 | sed s/^.*value=//", "r");
#else
#if defined (CONFIG_ARCH_MT7623)
	do_system("reg s fb110000");
	pp = popen("reg p 208 | sed s/^.*0x//", "r");
#elif defined (CONFIG_RALINK_MT7621)
	do_system("reg s be110000");
	pp = popen("reg p 208 | sed s/^.*0x//", "r");
#else
	pp = popen("switch reg r 3408 | sed s/^.*value=//", "r");
#endif
#endif
	fscanf(pp, "%s\n", reg);
	pclose(pp);
	if ((strtoul(reg, NULL, 16)&(0x1)) == 0) {
		set_nth_value_flash(nvram, index, "HS_link_status", "0");
		set_nth_value_flash(nvram, index, "HS_dl_speed", "0");
		set_nth_value_flash(nvram, index, "HS_ul_speed", "0");
	} else { 
		int link_speed = (strtoul(reg, NULL, 16)>>2)&(0x3);
		set_nth_value_flash(nvram, index, "HS_link_status", "1");
		if (link_speed == 0x2) {
			set_nth_value_flash(nvram, index, "HS_dl_speed", "1000000");
			set_nth_value_flash(nvram, index, "HS_ul_speed", "1000000");

		} else if (link_speed == 0x1) {
			set_nth_value_flash(nvram, index, "HS_dl_speed", "100000");
			set_nth_value_flash(nvram, index, "HS_ul_speed", "100000");
		} else {
			set_nth_value_flash(nvram, index, "HS_dl_speed", "10000");
			set_nth_value_flash(nvram, index, "HS_ul_speed", "10000");
		}
	}
#endif
#endif
	mcc1 = web_get("mcc1", input, 1);
	set_nth_value_flash(nvram, index, "HS_plmn1_mcc", mcc1);
	mnc1 = web_get("mnc1", input, 1);
	set_nth_value_flash(nvram, index, "HS_plmn1_mnc", mnc1);
	mcc2 = web_get("mcc2", input, 1);
	set_nth_value_flash(nvram, index, "HS_plmn2_mcc", mcc2);
	mnc2 = web_get("mnc2", input, 1);
	set_nth_value_flash(nvram, index, "HS_plmn2_mnc", mnc2);
	mcc3 = web_get("mcc3", input, 1);
	set_nth_value_flash(nvram, index, "HS_plmn3_mcc", mcc3);
	mnc3 = web_get("mnc3", input, 1);
	set_nth_value_flash(nvram, index, "HS_plmn3_mnc", mnc3);
	mcc4 = web_get("mcc4", input, 1);
	set_nth_value_flash(nvram, index, "HS_plmn4_mcc", mcc4);
	mnc4 = web_get("mnc4", input, 1);
	set_nth_value_flash(nvram, index, "HS_plmn4_mnc", mnc4);
	mcc5 = web_get("mcc5", input, 1);
	set_nth_value_flash(nvram, index, "HS_plmn5_mcc", mcc5);
	mnc5 = web_get("mnc5", input, 1);
	set_nth_value_flash(nvram, index, "HS_plmn5_mnc", mnc5);
	mcc6 = web_get("mcc6", input, 1);
	set_nth_value_flash(nvram, index, "HS_plmn6_mcc", mcc6);
	mnc6 = web_get("mnc6", input, 1);
	set_nth_value_flash(nvram, index, "HS_plmn6_mnc", mnc6);
	nvram_commit(nvram);

	do_system("init_system restart");
	do_system("sleep 1");
	do_system("hotspot.sh");
}
#endif

void reset_wifi_state(int nvram, char *input)
{
	char 	ifprefix[4];

#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM) {
		strcpy(ifprefix, "ra");	
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM) {
		strcpy(ifprefix, "rai");	
	}
#endif
	do_system("iwpriv %s0 set ResetCounter=0", ifprefix);
	web_redirect(getenv("HTTP_REFERER"));
}

#if ! defined CONFIG_FIRST_IF_NONE || ! defined CONFIG_SECOND_IF_NONE
static void restart_8021x(int nvram)
{
	int i, num, apd_flag = 0;
	const char *auth_mode = (char *) nvram_bufget(nvram, "AuthMode");
	const char *ieee8021x = (char *)nvram_bufget(nvram, "IEEE8021X");
	const char *num_s = nvram_bufget(nvram, "BssidNum");
	if(!num_s)
		return;
	num = strtol(num_s, NULL, 10);

#if ! defined CONFIG_FIRST_IF_NONE 
	if(nvram == RT2860_NVRAM)
		do_system("killall rt2860apd");
#endif
#if ! defined CONFIG_SECOND_IF_NONE
	if(nvram == RTDEV_NVRAM)
		do_system("killall rtinicapd");
#endif

	/*
	 * In fact we only support mbssid[0] to use 802.1x radius settings.
	 */
	for(i=0; i<num; i++){
		char tmp_auth[128];
		if (get_nth_value(i, (char *)auth_mode, ';', tmp_auth, 128) != -1){
			if(!strcmp(tmp_auth, "WPA") || !strcmp(tmp_auth, "WPA2") || !strcmp(tmp_auth, "WPA1WPA2")){
				apd_flag = 1;
				break;
			}
		}

		if (get_nth_value(i, (char *)ieee8021x, ';', tmp_auth, 128) != -1){
			if(!strcmp(tmp_auth, "1")){
				apd_flag = 1;
				break;
			}
		}
	}

	if (apd_flag) {
#if ! defined CONFIG_FIRST_IF_NONE 
		if(nvram == RT2860_NVRAM)
			do_system("rt2860apd");
#endif
#if ! defined CONFIG_SECOND_IF_NONE
		if(nvram == RTDEV_NVRAM)
			do_system("rtinicapd");
#endif
	}
#ifdef CONFIG_USER_HOTSPOT
    restart_hotspot_daemon();
#endif /* CONFIG_USER_HOTSPOT */
}
#endif

#if defined (RT2860_WSC_SUPPORT) || defined (RTDEV_WSC_SUPPORT)
static void restart_wps(int nvram, int wsc_enable)
{
	char 	ifprefix[4];

#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM) {
		strcpy(ifprefix, "ra");	
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM) {
		strcpy(ifprefix, "rai");	
	}
#endif
	do_system("iwpriv %s0 set WscConfMode=0", ifprefix);
	do_system("route delete 239.255.255.250");
	if (wsc_enable > 0) {
		do_system("iwpriv %s0 set WscConfMode=%d", ifprefix, 7);
		do_system("route add -host 239.255.255.250 dev br0");
	}
	do_system("miniupnpd.sh init");
}

void set_wifi_wpsconf(int nvram, char *input)
{
	int	wsc_enable = 0;
	char 	ifprefix[4];

#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM) {
		strcpy(ifprefix, "ra");	
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM) {
		strcpy(ifprefix, "rai");	
	}
#endif
	wsc_enable = strtol(web_get("WPSEnable", input, 0), NULL, 0);

	//resetTimerAll();
	//g_WscResult = 0;
	//LedReset();

	if (wsc_enable == 0){
		nvram_bufset(nvram, "WscModeOption", "0");
		do_system("iwpriv %s0 set WscConfMode=0", ifprefix);
		do_system("route delete 239.255.255.250");
	}else{
		nvram_bufset(nvram, "WscModeOption", "7");
		do_system("iwpriv %s0 set WscConfMode=7", ifprefix);
		do_system("route add -host 239.255.255.250 dev br0");
	}
	nvram_commit(nvram);
	restart_wps(nvram, wsc_enable);
	web_redirect(getenv("HTTP_REFERER"));
}

static unsigned int get_ap_pin(char *interface)
{
	int socket_id;
	struct iwreq wrq;
	unsigned int data = 0;
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	strcpy(wrq.ifr_name, interface);
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = (caddr_t) &data;
	wrq.u.data.flags = RT_OID_WSC_PIN_CODE;
	if( ioctl(socket_id, RT_PRIV_IOCTL, &wrq) == -1)
		DBG_MSG("ioctl error");
	close(socket_id);
	return data;
}

void set_wifi_gen_pin(int nvram, char *input)
{
	char new_pin[9];
	char iface[6];

#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM) {
		strcpy(iface, "ra0");	
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM) {
		strcpy(iface, "rai0");	
	}
#endif

	do_system("iwpriv %s  set WscGenPinCode", iface);
	sprintf(new_pin, "%08d", get_ap_pin(iface));
	nvram_bufset(nvram, "WscVendorPinCode", new_pin);
	nvram_commit(nvram);

	if (nvram == RT2860_NVRAM)
		do_system("ralink_init make_wireless_config rt2860");
	else
		do_system("ralink_init make_wireless_config rtdev");
	web_redirect(getenv("HTTP_REFERER"));
}

void set_wifi_wps_oob(int nvram, char *input)
{
	char SSID[64], *mac;
	char iface[6];

#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM) {
		strcpy(iface, "ra0");	
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM) {
		strcpy(iface, "rai0");	
	}
#endif

	//resetTimerAll();
	//g_WscResult = 0;
	//LedReset();
	
	if ((mac = get_macaddr(iface)) != NULL)
		sprintf(SSID, "RalinkInitAP_%s", mac);
	else
		sprintf(SSID, "RalinkInitAP_unknown");
	nvram_bufset(nvram, "SSID1", SSID);

	nvram_bufset(nvram, "WscConfigured", "0");

	set_nth_value_flash(nvram, 0, "AuthMode", "WPA2PSK");
	set_nth_value_flash(nvram, 0, "EncrypType", "AES");
	set_nth_value_flash(nvram, 0, "DefaultKeyID", "2");
	nvram_bufset(nvram, "WPAPSK1", "12345678");

	set_nth_value_flash(nvram, 0, "IEEE8021X", "0");

	/*
	 *   IMPORTANT !!!!!
	 *   5VT doesn't need it cause it will reboot after OOB reset, but RT2880 does.
	 */
	//g_wsc_configured = 0;

	nvram_commit(nvram);

	do_system("iwpriv %s set SSID=%s", iface,  SSID);
	do_system("iwpriv %s set AuthMode=WPA2PSK", iface);
	do_system("iwpriv %s set EncrypType=AES", iface);
	do_system("iwpriv %s set WPAPSK=12345678", iface);
	do_system("iwpriv %s set SSID=%s", iface, SSID);

#if ! defined CONFIG_FIRST_IF_NONE || ! defined CONFIG_SECOND_IF_NONE
	restart_8021x(nvram);
#endif
	restart_wps(nvram, 1);
	do_system("iwpriv %s set WscConfStatus=1", iface);
	web_redirect(getenv("HTTP_REFERER"));
}

static void wps_ap_pbc_start_all(int nvram)
{
	const char *wsc_enable = nvram_bufget(nvram, "WscModeOption");
	char iface[6];

#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM) {
		strcpy(iface, "ra0");	
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM) {
		strcpy(iface, "rai0");	
	}
#endif

	// It is possible user press PBC button when WPS is disabled.
	if (!strcmp(wsc_enable, "0")) {
		DBG_MSG("The PBC button is pressed but WPS is disabled now.");
		return;
	}

	do_system("iwpriv %s set WscMode=2", iface);
	do_system("iwpriv %s set WscGetConf=1", iface);

	//resetTimerAll();
	//setTimer(WPS_AP_CATCH_CONFIGURED_TIMER * 1000, WPSAPTimerHandler);
}

void set_wifi_do_wps(int nvram, char *input)
{
	char *wsc_config_option, *wsc_pin_code_w;
	char iface[6];

#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM) {
		strcpy(iface, "ra0");	
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM) {
		strcpy(iface, "rai0");	
	}
#endif

	wsc_config_option = wsc_pin_code_w = NULL;
	wsc_config_option = web_get("PINPBCRadio", input, 0);

	// reset wsc result indicator
	//g_WscResult = 0;
	//LedReset();
	if (!strcmp(wsc_config_option, "1")) {
		do_system("iwpriv %s set WscMode=1", iface);

		// get pin code
		wsc_pin_code_w = web_get("PIN", input, 0);

		do_system("iwpriv %s set WscPinCode=%s", iface, wsc_pin_code_w);
		do_system("iwpriv %s set WscGetConf=1", iface);
		//DBG_MSG("%s ; %s", iface, wsc_pin_code_w);

		//resetTimerAll();
		//setTimer(WPS_AP_CATCH_CONFIGURED_TIMER * 1000, WPSAPTimerHandler);
	} else if (! strcmp(wsc_config_option, "2")) {
		wps_ap_pbc_start_all(nvram);
	} else {
		DBG_MSG("ignore unknown WSC method: %s", wsc_config_option);
	}
#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM)
		do_system("killall -SIGXFSZ nvram_daemon");
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM)
		do_system("killall -SIGWINCH nvram_daemon");
#endif
	web_redirect(getenv("HTTP_REFERER"));
}

void set_wifi_cancel_wps(int nvram, char *input)
{
	char iface[6];

#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM) {
		strcpy(iface, "ra0");	
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM) {
		strcpy(iface, "rai0");	
	}
#endif
	//resetTimerAll();
	do_system("iwpriv %s set WscStop=1", iface);
	do_system("miniupnpd.sh init");
	web_redirect(getenv("HTTP_REFERER"));
}
#endif

#if defined (RT2860_WAPI_SUPPORT) || defined (RTDEV_WAPI_SUPPORT)
static void restart_wapi(void)
{
	const char *auth_mode;

#if defined (RT2860_WAPI_SUPPORT)
	auth_mode = nvram_bufget(RT2860_NVRAM, "AuthMode");
	do_system("killall wapid");
	if (NULL !=strstr(auth_mode, "WAIPSK") || NULL !=strstr(auth_mode, "WAICERT")) {
		do_system("ralink_init gen wapi");
		do_system("wapid");
	}
#endif
#if defined (RTDEV_WAPI_SUPPORT)
	auth_mode = nvram_bufget(RTDEV_NVRAM, "AuthMode");
	if (NULL !=strstr(auth_mode, "WAIPSK") || NULL !=strstr(auth_mode, "WAICERT")) {
		do_system("ralink_init gen wapi");
		do_system("wapid -i rai0");
	}
#endif
}
#endif
#if defined (CONFIG_RALINK_MT7628)	
void obtw(int nvram, char *input)
{
	char *obtw_enable, *CCK_1M, *CCK_5M, *OFDM_6M, *OFDM_12M, *HT20_MCS_0, *HT20_MCS_32, *HT20_MCS_1_2, *HT40_MCS_0, *HT40_MCS_32, *HT40_MCS_1_2;
	web_debug_header();
	obtw_enable = strdup(web_get("obtw_enable", input, 1));
	CCK_1M = strdup(web_get("CCK_1M", input, 1));
	CCK_5M = strdup(web_get("CCK_5M", input, 1));
	OFDM_6M = strdup(web_get("OFDM_6M", input, 1));
	OFDM_12M = strdup(web_get("OFDM_12M", input, 1));
	HT20_MCS_0 = strdup(web_get("HT20_MCS_0", input, 1));
	//HT20_MCS_32 = strdup(web_get("HT20_MCS_32", input, 1));
	HT20_MCS_1_2 = strdup(web_get("HT20_MCS_1_2", input, 1));
	HT40_MCS_0 = strdup(web_get("HT40_MCS_0", input, 1));
	HT40_MCS_32 = strdup(web_get("HT40_MCS_32", input, 1));
	HT40_MCS_1_2 = strdup(web_get("HT40_MCS_1_2", input, 1));
	
	
	
	nvram_bufset(RT2860_NVRAM , "obtw", obtw_enable);
	nvram_bufset(RT2860_NVRAM , "CCK_1M", CCK_1M);
	nvram_bufset(RT2860_NVRAM , "CCK_5M", CCK_5M);
	nvram_bufset(RT2860_NVRAM , "OFDM_6M", OFDM_6M);
	nvram_bufset(RT2860_NVRAM , "OFDM_12M", OFDM_12M);
	nvram_bufset(RT2860_NVRAM , "HT20_MCS_0", HT20_MCS_0);
	//nvram_bufset(RT2860_NVRAM , "HT20_MCS_32", HT20_MCS_32);
	nvram_bufset(RT2860_NVRAM , "HT20_MCS_1_2", HT20_MCS_1_2);
	nvram_bufset(RT2860_NVRAM , "HT40_MCS_0", HT40_MCS_0);
	nvram_bufset(RT2860_NVRAM , "HT40_MCS_32", HT40_MCS_32);
	nvram_bufset(RT2860_NVRAM , "HT40_MCS_1_2", HT40_MCS_1_2);	
	
		if(!strcmp(obtw_enable, "0")){
			do_system("iwpriv ra0 set obtw=disable");
		}else{
			do_system("iwpriv ra0 set obtw=cck1m_%s_cck5m_%s_ofdm6m_%s_ofdm12m_%s_ht20mcs0_%s_ht20mcs1_%s_ht40mcs0_%s_ht40mcs32_%s_ht40mcs1_%s", CCK_1M, CCK_5M, OFDM_6M, OFDM_12M, HT20_MCS_0, HT20_MCS_1_2, HT40_MCS_0, HT40_MCS_32, HT40_MCS_1_2);
		}
	/*
	reip = strdup(web_get("stunIp", input, 1));
	reip = strdup(web_get("stunPort", input, 1));
	*/
	nvram_commit(RT2860_NVRAM );
	free_all(11, obtw_enable, CCK_1M, CCK_5M, OFDM_6M, OFDM_12M, HT20_MCS_0, HT20_MCS_32, HT20_MCS_1_2, HT40_MCS_0, HT40_MCS_32, HT40_MCS_1_2);

}
#endif
#if defined (CONFIG_SECOND_IF_MT7612E)	&& defined(CONFIG_FIRST_IF_MT7628)
void obtw_mt7612(int nvram, char *input)
{
	char *obtw_enable, *OFDM_6M, *OFDM_9M,*OFDM_12M, *OFDM_18M,*HT20_MCS_0, *HT20_MCS_1, *HT20_MCS_2, *HT40_MCS_0, *HT40_MCS_1, *HT40_MCS_2, *VHT80_MCS_0, *VHT80_MCS_1, *VHT80_MCS_2;
	int HT40_MCS_32 = 0;
	web_debug_header();
	obtw_enable = strdup(web_get("obtw_enable", input, 1));

	OFDM_6M = strdup(web_get("OFDM_6M", input, 1));
	OFDM_9M = strdup(web_get("OFDM_9M", input, 1));
	OFDM_12M = strdup(web_get("OFDM_12M", input, 1));
	OFDM_18M = strdup(web_get("OFDM_18M", input, 1));
	HT20_MCS_0 = strdup(web_get("HT20_MCS_0", input, 1));
  HT20_MCS_1 = strdup(web_get("HT20_MCS_1", input, 1));
  HT20_MCS_2 = strdup(web_get("HT20_MCS_2", input, 1));
  
	HT40_MCS_0 = strdup(web_get("HT40_MCS_0", input, 1));
	HT40_MCS_1 = strdup(web_get("HT40_MCS_1", input, 1));
	HT40_MCS_2 = strdup(web_get("HT40_MCS_2", input, 1));
	//HT40_MCS_32 = strdup(web_get("HT40_MCS_32", input, 1));
	
	VHT80_MCS_0 = strdup(web_get("VHT80_MCS_0", input, 1));
	VHT80_MCS_1 = strdup(web_get("VHT80_MCS_1", input, 1));
	VHT80_MCS_2	= strdup(web_get("VHT80_MCS_2", input, 1));
	
	
	nvram_bufset(RT2860_NVRAM , "obtw_mt7612", obtw_enable);
	nvram_bufset(RT2860_NVRAM , "OFDM_6M", OFDM_6M);
	nvram_bufset(RT2860_NVRAM , "OFDM_9M", OFDM_9M);	
	nvram_bufset(RT2860_NVRAM , "OFDM_12M", OFDM_12M);
	nvram_bufset(RT2860_NVRAM , "OFDM_18M", OFDM_18M);
	nvram_bufset(RT2860_NVRAM , "HT20_MCS_0", HT20_MCS_0);
	nvram_bufset(RT2860_NVRAM , "HT20_MCS_1", HT20_MCS_1);
	nvram_bufset(RT2860_NVRAM , "HT20_MCS_2", HT20_MCS_2);	
	nvram_bufset(RT2860_NVRAM , "HT40_MCS_0", HT40_MCS_0);
	//nvram_bufset(RT2860_NVRAM , "HT40_MCS_32", HT40_MCS_32);
	nvram_bufset(RT2860_NVRAM , "HT40_MCS_1", HT40_MCS_1);	
	nvram_bufset(RT2860_NVRAM , "HT40_MCS_2", HT40_MCS_2);
	
	nvram_bufset(RT2860_NVRAM , "VHT80_MCS_0", VHT80_MCS_0);	
	nvram_bufset(RT2860_NVRAM , "VHT80_MCS_1", VHT80_MCS_1);
	nvram_bufset(RT2860_NVRAM , "VHT80_MCS_2", VHT80_MCS_2);	

		if(!strcmp(obtw_enable, "0")){
			do_system("iwpriv rai0 set obtw=disable");
		}else{
			do_system("iwpriv rai0 set obtw=ofdm6m_%s_ofdm9m_%s_ofdm12m_%s_ofdm18m_%s_ht20mcs0_%s_ht20mcs1_%s_ht20mcs2_%s_ht40mcs0_%s_ht40mcs1_%s_ht40mcs2_%s_ht40mcs32_%d_vht80mcs0_%s_vht80mcs1_%s_vht80mcs2_%s", OFDM_6M, OFDM_9M, OFDM_12M, OFDM_18M, HT20_MCS_0, HT20_MCS_1, HT20_MCS_2, HT40_MCS_0, HT40_MCS_1, HT40_MCS_2, HT40_MCS_32, VHT80_MCS_0, VHT80_MCS_1, VHT80_MCS_2);
		}
	/*
	reip = strdup(web_get("stunIp", input, 1));
	reip = strdup(web_get("stunPort", input, 1));
	*/
	nvram_commit(RT2860_NVRAM );
	free_all(14, obtw_enable, OFDM_6M, OFDM_9M, OFDM_12M, OFDM_18M, HT20_MCS_0, HT20_MCS_1, HT20_MCS_2, HT40_MCS_0, HT40_MCS_1, HT40_MCS_2,VHT80_MCS_0, VHT80_MCS_1, VHT80_MCS_2);

}
#endif

#if defined (CONFIG_RT2860V2_STA_WSC)
#define WPS_STA_TIMEOUT_SECS			120000				// 120 seconds
static int g_wps_timer_state = 0;
static int g_wps_finished = 1;
#define WPS_STA_CATCH_CONFIGURED_TIMER	10					// 10 * 1000 microsecond = every 0.010 sec
#define REGISTRAR_TIMER_MODE			0xdeadbeef			// okay, this is a magic number
#define TOKEN "\t"

static char	*g_pAPListData = NULL;
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
PRT_PROFILE_SETTING headerProfileSetting, currentProfileSetting;


static void WaitWPSSTAFinish()
{
	while(!g_wps_finished)
		sleep(1);
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
/*
 * The setitimer() is Linux-specified.
 */
int setTimer(int microsec, void ((*sigroutine)(int)))
{
        struct itimerval value, ovalue;

        signal(SIGALRM, sigroutine);
        value.it_value.tv_sec = 0;
        value.it_value.tv_usec = microsec;
        value.it_interval.tv_sec = 0;
        value.it_interval.tv_usec = microsec;
        return setitimer(ITIMER_REAL, &value, &ovalue);
}

void stopTimer(void)
{
        struct itimerval value, ovalue;

        value.it_value.tv_sec = 0;
        value.it_value.tv_usec = 0;
        value.it_interval.tv_sec = 0;
        value.it_interval.tv_usec = 0;
        setitimer(ITIMER_REAL, &value, &ovalue);
}

/*
 * description: 
 *	station profile initialization
 * 	Initial the Profile List from values in NVRAM named staXXX.
 * 	Each Profile is seperated by '\t' in NVRAM values.
 */
int initStaProfile(void)
{
	PRT_PROFILE_SETTING nextProfileSetting;
	char tmp_buffer[512];
	const char *wordlist = NULL;
	char *tok = NULL;
	int i;
/*
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
*/

	//staProfile
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staProfile");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("no previous profiles defined");
		return 0;
	}

	//It is always NULL in Lighty, because cgi is not a daemon.
	if (headerProfileSetting == NULL ) { //It is always NULL in Lighty
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
	//G_staProfileNum = i;

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
	//bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staWpaPsk");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta WPAPSK has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	//snprintf(tmp_buffer, sizeof(tmp_buffer), "%s", wordlist);
	char *strp = strdup(wordlist);
	for (i = 0, tok = strsep(&strp, TOKEN); tok; tok = strsep(&strp, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->WpaPsk, "%s", tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}
	free(strp);
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
 * description: get a station new/uniq profile name
 */
#define WSC_PROFILE_NAME_POSTFIX_LEN 3
#if 1
static char *getStaNewProfileName(char *prefix)
{
	PRT_PROFILE_SETTING currentProfileSetting;
	int prefix_len = strlen(prefix);
	int total_len;
	int postfix_candidate = 1;
	static char result[32+1]; //refer to _RT_PROFILE_SETTING.

	if(strlen(prefix) > (32 - WSC_PROFILE_NAME_POSTFIX_LEN) ){
		// we force the prefix length can't exceed 29.
		prefix[32-WSC_PROFILE_NAME_POSTFIX_LEN] = '\0';
	}

	result[0] = '\0';

	initStaProfile();

	if(headerProfileSetting == NULL){
		strncpy(result,  prefix, 32);
		strcat(result, "001");
		return result;
	}

	currentProfileSetting = headerProfileSetting;
	while(currentProfileSetting){
		total_len = strlen(currentProfileSetting->Profile);
		if(!strncmp(currentProfileSetting->Profile, prefix, prefix_len)) {
			char *profile_num = &currentProfileSetting->Profile[prefix_len];	// AAA001, BBB001 
			int profile_num_int = atoi(profile_num);

			if(postfix_candidate == profile_num_int){
				postfix_candidate++;

				if(postfix_candidate > 999)
					return NULL;

				// loop whole chain again
				currentProfileSetting = headerProfileSetting;
				continue;
			}
		}
		currentProfileSetting = currentProfileSetting->Next;
	}

	snprintf(result, 32, "%s%03d", prefix, postfix_candidate);
	return result;
}
#else
static char *getStaNewProfileName(char *prefix)
{
	PRT_PROFILE_SETTING currentProfileSetting;
	int prefix_len = strlen(prefix);
	int total_len;
	int postfix_candidate = 1;
	static char result[32+1]; //refer to _RT_PROFILE_SETTING.

	if(strlen(prefix) > (32 - WSC_PROFILE_NAME_POSTFIX_LEN) ){
		// we force the prefix length can't exceed 29.
		prefix[32-WSC_PROFILE_NAME_POSTFIX_LEN] = '\0';
	}

	result[0] = '\0';
	if(headerProfileSetting == NULL){
		strncpy(result,  prefix, 32);
		strcat(result, "001");
		return result;
	}

	currentProfileSetting = headerProfileSetting;
	while(currentProfileSetting){
		total_len = strlen(currentProfileSetting->Profile);
		if(!strncmp(currentProfileSetting->Profile, prefix, prefix_len)) {
			char *profile_num = &currentProfileSetting->Profile[prefix_len];	// AAA001, BBB001 
			int profile_num_int = atoi(profile_num);

			if(postfix_candidate == profile_num_int){
				postfix_candidate++;

				if(postfix_candidate > 999)
					return NULL;

				// loop whole chain again
				currentProfileSetting = headerProfileSetting;
				continue;
			}
		}
		currentProfileSetting = currentProfileSetting->Next;
	}

	snprintf(result, 32, "%s%03d", prefix, postfix_candidate);
	return result;
}
#endif

static int getStaChannel(char *interface)
{
	unsigned int ConnectStatus = 0;

	NDIS_802_11_CONFIGURATION Configuration;
//	RT_802_11_LINK_STATUS     LinkStatus;
	int s, ret, i;
	int nChannel = -1;
//	int Japan_channel = 200;

	s = socket(AF_INET, SOCK_DGRAM, 0);

	ret = OidQueryInformation(OID_GEN_MEDIA_CONNECT_STATUS, s, interface, &ConnectStatus, sizeof(ConnectStatus));
	if (ret < 0 || ConnectStatus == 0) {
		close(s);
		return 0;
	}

	// Current Channel
	OidQueryInformation(OID_802_11_CONFIGURATION, s, interface, &Configuration, sizeof(NDIS_802_11_CONFIGURATION));
	for (i = 0; i < G_nChanFreqCount; i++) {
		if (Configuration.DSConfig == ChannelFreqTable[i].lFreq) {
			nChannel = ChannelFreqTable[i].lChannel;
			break;
		}
	}

/*
	if (nChannel == -1)
		return 0;
	else if (nChannel == (Japan_channel + 8))
		websWrite(wp, "J8 <--> %ld KHz ; Central Channel: %ld", Configuration.DSConfig, LinkStatus.CentralChannel);
	else if (nChannel == (Japan_channel + 12))
		websWrite(wp, "J12 <--> %ld KHz ; Central Channel: %ld", Configuration.DSConfig, LinkStatus.CentralChannel);
	else if (nChannel == (Japan_channel + 16))
		websWrite(wp, "J16 <--> %ld KHz ; Central Channel: %ld", Configuration.DSConfig, LinkStatus.CentralChannel);
	else
		websWrite(wp, "%u <--> %ld KHz ; Central Channel: %ld", nChannel, Configuration.DSConfig, LinkStatus.CentralChannel);
*/
	close(s);
	return nChannel;
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
static void WPSSTAMode0(int nvram, char *input)
{
	DBG_MSG("");
	//if(!gstrcmp(query, "0")) {
		if (strcmp(nvram_bufget(RT2860_NVRAM, "staWPSMode"), "1") == 0) {
			unsigned char mac[6];
			char defaultSSID[33];
			memcpy(mac, getStaMacAddr(), 6);
			snprintf(defaultSSID, 32, "STARegistrar%02X%02X%02X", mac[3], mac[4], mac[5]);
			nvram_bufset(RT2860_NVRAM, "staRegSSID", defaultSSID);
			nvram_bufset(RT2860_NVRAM, "staRegAuth", "WPA2PSK");
			nvram_bufset(RT2860_NVRAM, "staRegEncry", "AES");
			nvram_bufset(RT2860_NVRAM, "staRegKeyType", "1");
			nvram_bufset(RT2860_NVRAM, "staRegKeyIndex", "1");
			nvram_bufset(RT2860_NVRAM, "staRegKey", "12345678");
		}
		nvram_bufset(RT2860_NVRAM, "staWPSMode", "0");
	//} else if(!gstrcmp(query, "1")) {
		//nvram_bufset(RT2860_NVRAM, "staWPSMode", "1");
	//} else
		//return;
	nvram_commit(RT2860_NVRAM);
	//websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	//websWrite(wp, T("WPS STA mode setting done\n"));
	//websDone(wp, 200);
	//printf("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n");
	web_debug_header();
	printf("WPS STA mode setting done\n");
}
static void WPSSTAMode1(int nvram, char *input)
{
DBG_MSG("");
		nvram_bufset(RT2860_NVRAM, "staWPSMode", "1");

	nvram_commit(RT2860_NVRAM);
	//websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	//websWrite(wp, T("WPS STA mode setting done\n"));
	//websDone(wp, 200);
	//printf("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n");
	web_debug_header();
	printf("WPS STA mode setting done\n");
	
}
static void resetTimerAll(void)
{
	stopTimer();
	g_wps_timer_state = 0;
}
void SaveToFlashStr(char *key, char *value)
{
	char tmp_buffer[512];
	const char *wordlist = nvram_bufget(RT2860_NVRAM, key);
	if (wordlist && strcmp(wordlist,"") != 0)
		snprintf(tmp_buffer, 512, "%s\t%s", wordlist, value);
	else
		snprintf(tmp_buffer, 512, "%s", value);
	nvram_bufset(RT2860_NVRAM, key, tmp_buffer);
}

void SaveToFlashInt(char *key, int value)
{
	char tmp_buffer[512];
	const char *wordlist = nvram_bufget(RT2860_NVRAM, key);
	if (wordlist && strcmp(wordlist,"") != 0)
		snprintf(tmp_buffer, 512, "%s\t%d", wordlist, value);
	else
		snprintf(tmp_buffer, 512, "%d", value);
	nvram_bufset(RT2860_NVRAM, key, tmp_buffer);
}
void freeHeaderProfileSettings(void)
{
	PRT_PROFILE_SETTING list = headerProfileSetting;
	PRT_PROFILE_SETTING next;
	while(list){
		next = list->Next;
		free(list);
		list = next;
		next = list->Next;
	}
}
static int isWPSSuccess(void)
{
	char *str;
	char long_buf[4096];
	FILE *fp;
	memset(long_buf, 0, 4096);
	if(!(fp = popen("iwpriv ra0 stat", "r")))
		return 0;
	fread(long_buf, 1, 4096, fp);
	pclose(fp);

	if(!(str = strstr(long_buf, "WPS Profile Count")))
		return 0;
	if(!(str = strchr(str, '=')))
		return 0;
	if(atoi(str+1))
		return 1;
	return 0;
}
/*
 * for WiFi STA Test Plan Case 5.1.1
 */
static char *addWPSSTAProfile2(WSC_CREDENTIAL *wsc_cre)
{
	RT_PROFILE_SETTING  tmpProfileSetting;

	char			tmp_value[512];
	USHORT              AuthType	= wsc_cre->AuthType;           // mandatory, 1: open, 2: wpa-psk, 4: shared, 8:wpa, 0x10: wpa2, 0x20: wpa
	USHORT              EncrType	= wsc_cre->EncrType;           // mandatory, 1: none, 2: wep, 4: tkip, 8: aes
	UCHAR               *Key		= &wsc_cre->Key[0];            // mandatory, Maximum 64 byte
	USHORT              KeyLength	= wsc_cre->KeyLength;
	UCHAR               KeyIndex	= wsc_cre->KeyIndex;           // optional, default is 1
                        
	char buffer[512] = {0}, *p;
	const char *wordlist = nvram_bufget(RT2860_NVRAM, "staActive");
	strcpy(buffer, wordlist);
	p = buffer;
	while (*p != '\0') {
		if (*p == '1')
			*p = '0';
		p++;
	}
	nvram_bufset(RT2860_NVRAM, "staActive", buffer);
	nvram_commit(RT2860_NVRAM);

	memset(&tmpProfileSetting, 0x00, sizeof(RT_PROFILE_SETTING));
	tmpProfileSetting.Next = NULL;

	strncpy(tmpProfileSetting.SSID, wsc_cre->SSID.Ssid, 32);
	printf("SSID1=%s\n", tmpProfileSetting.SSID);
	SaveToFlashStr("staSSID", tmpProfileSetting.SSID);

	//profile name, gen a uniq name
	snprintf(tmp_value, 512, "WPS_%s", tmpProfileSetting.SSID);
	strncpy(tmp_value, getStaNewProfileName(tmp_value), 512);
	if (!tmp_value || !strlen(tmp_value)) {
		fprintf(stderr, "Error profile name !\n");
		return NULL;
	}
	strncpy((char *)tmpProfileSetting.Profile, tmp_value, 32);
	SaveToFlashStr("staProfile", tmpProfileSetting.Profile);

	//network type
	tmpProfileSetting.NetworkType = 1;
	SaveToFlashInt("staNetworkType", 1);

	//Adhoc mode
	tmpProfileSetting.AdhocMode = 0;
	SaveToFlashInt("staAdhocMode", 0);

	//power saving mode
	tmpProfileSetting.PSmode = Ndis802_11PowerModeCAM;
	SaveToFlashInt("staPSMode", Ndis802_11PowerModeCAM);

	//channel
	tmpProfileSetting.Channel = getStaChannel("ra0");
	SaveToFlashInt("staChannel", tmpProfileSetting.Channel);

	//b preamble type
	tmpProfileSetting.PreamType = Rt802_11PreambleAuto;
	SaveToFlashInt("staPreamType", tmpProfileSetting.PreamType);

	//rts threshold value
	tmpProfileSetting.RTSCheck = 0;
	SaveToFlashInt("staRTSCheck", tmpProfileSetting.RTSCheck);
	tmpProfileSetting.RTS = 2347;
	SaveToFlashInt("staRTS", tmpProfileSetting.RTS);

	//fragment threshold value
	tmpProfileSetting.FragmentCheck = 0;
	SaveToFlashInt("staFragmentCheck", tmpProfileSetting.FragmentCheck);
	tmpProfileSetting.Fragment = 2346;
	SaveToFlashInt("staFragment", tmpProfileSetting.Fragment);

	// AuthMode
	//security policy (security_infra_mode or security_adhoc_mode)
	// get Security from .dat
	fprintf(stderr, "WPS 2.0: auth:%x\n", AuthType);
	switch(AuthType){
	case 0x1:
		tmpProfileSetting.Authentication = Ndis802_11AuthModeOpen;
		break;
	case 0x2:
		tmpProfileSetting.Authentication = Ndis802_11AuthModeWPAPSK;
		if(EncrType != 0x4 && EncrType != 0x8)
			return NULL;
		break;
	case 0x4:
		tmpProfileSetting.Authentication = Ndis802_11AuthModeShared;
		if(EncrType != 0x1 && EncrType != 0x2)
			return NULL;
		break;
	case 0x8:
		tmpProfileSetting.Authentication = Ndis802_11AuthModeWPA;
		break;
	case 0x10:
		tmpProfileSetting.Authentication = Ndis802_11AuthModeWPA2;
		break;
	case 0x20:
	case 0x22:
		tmpProfileSetting.Authentication = Ndis802_11AuthModeWPA2PSK;		
		if(EncrType != 0x4 && EncrType != 0x8 && EncrType != 0xC)
			return NULL;
		break;
	default:
		return NULL;
	}
	SaveToFlashInt("staAuth", tmpProfileSetting.Authentication);

	// Encrypt mode
	//Encrypt
	fprintf(stderr, "WPS 2.0: encry:%x\n", EncrType);
	switch(EncrType){
	case 0x1:	/* None */
		tmpProfileSetting.Encryption = Ndis802_11WEPDisabled;
		break;
	case 0x2:	/* WEP */
		if(KeyLength && (KeyLength != 5 && KeyLength != 13) && (KeyLength != 10 && KeyLength != 26))
			return NULL;
		tmpProfileSetting.Encryption = Ndis802_11WEPEnabled;
		break;
	case 0x8:	/* AES */
	case 0xC:	/* TKIP or AES */
		tmpProfileSetting.Encryption = Ndis802_11Encryption3Enabled;
		break;
	case 0x4:	/* TKIP */
		;
	default:	/* default: TKIP */
		tmpProfileSetting.Encryption = Ndis802_11Encryption2Enabled;
		break;
	}
	SaveToFlashInt("staEncrypt", tmpProfileSetting.Encryption);
	
	//wep default key
	tmpProfileSetting.KeyDefaultId = KeyIndex;
	SaveToFlashInt("staKeyDefaultId", tmpProfileSetting.KeyDefaultId);

#ifdef WPA_SUPPLICANT_SUPPORT
	if(tmpProfileSetting.Authentication  == Ndis802_11AuthModeWPA ||
		tmpProfileSetting.Authentication == Ndis802_11AuthModeWPA2){
		tmpProfileSetting.KeyMgmt = Rtwpa_supplicantKeyMgmtWPAEAP;
	}else if(tmpProfileSetting.Authentication == Ndis802_11AuthModeMax){
		tmpProfileSetting.KeyMgmt = Rtwpa_supplicantKeyMgmtIEEE8021X;
	}else
		tmpProfileSetting.KeyMgmt = Rtwpa_supplicantKeyMgmtNONE;
	SaveToFlashInt("sta8021xKeyMgmt", tmpProfileSetting.KeyMgmt);
#endif

	/*
	 *	Deal with Key
	 */
	switch(AuthType){
	case 0x1:	/* Open */
	case 0x4:	/* Shared */
		//tmpProfileSetting.Authentication = Ndis802_11AuthModeOpen;
		if(EncrType == 2 /* WEP */){
			char hex_wep[128];
			if(KeyLength == 5)
				sprintf(hex_wep, "%02X%02X%02X%02X%02X", Key[0], Key[1], Key[2], Key[3], Key[4]);
			else if (KeyLength == 13)
				sprintf(hex_wep, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", Key[0], Key[1], Key[2], Key[3], Key[4], Key[5], Key[6], Key[7], Key[8], Key[9], Key[10], Key[11], Key[12]);
			if(KeyLength == 10)
				sprintf(hex_wep, "%c%c%c%c%c%c%c%c%c%c", Key[0], Key[1], Key[2], Key[3], Key[4], Key[5], Key[6], Key[7], Key[8], Key[9]);
			else if (KeyLength == 26)
				sprintf(hex_wep, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 
						Key[0], Key[1], Key[2], Key[3], Key[4], Key[5], Key[6], Key[7], Key[8], Key[9], Key[10], Key[11], Key[12], Key[13], Key[14], Key[15], Key[16], Key[17], Key[18], Key[19], Key[20], Key[21], Key[22], Key[23], Key[24], Key[25]);
			switch(KeyIndex){
			case 1:
				SaveToFlashStr("staKey1", hex_wep);
				SaveToFlashInt("staKey1Type", 0);
				break;
			case 2:
				SaveToFlashStr("staKey2", hex_wep);
				SaveToFlashInt("staKey2Type", 0);
				break;
			case 3:
				SaveToFlashStr("staKey3", 		hex_wep);
				SaveToFlashInt("staKey3Type", 0);
				break;
			case 4:
				SaveToFlashStr("staKey4", 		hex_wep);
				SaveToFlashInt("staKey4Type", 0);
				break;
			}
		}else{
			// clear WEP Keys
			SaveToFlashStr("staKey1", "0");
			SaveToFlashInt("staKey1Type", 0);
			SaveToFlashStr("staKey2", "0");
			SaveToFlashInt("staKey2Type", 0);
			SaveToFlashStr("staKey3", "0");
			SaveToFlashInt("staKey3Type", 0);
			SaveToFlashStr("staKey4", "0");
			SaveToFlashInt("staKey4Type", 0);
		}

		// clear WPAPSK
		SaveToFlashStr("staWpaPsk", "0");
		break;
	case 0x2:	/* WPAPSK */
	case 0x20:	/* WPA2PSK */
	case 0x22:	/* WPAPSK or WPA2PSK */
		// set WPAPSK Key
		strncpy(tmpProfileSetting.WpaPsk, Key, 64);
		SaveToFlashStr("staWpaPsk", tmpProfileSetting.WpaPsk);

		// clear WEP Keys
		SaveToFlashStr("staKey1", "0");
		SaveToFlashInt("staKey1Type", 0);
		SaveToFlashStr("staKey2", "0");
		SaveToFlashInt("staKey2Type", 0);
		SaveToFlashStr("staKey3", "0");
		SaveToFlashInt("staKey3Type", 0);
		SaveToFlashStr("staKey4", "0");
		SaveToFlashInt("staKey4Type", 0);
		break;
	case 0x8:	/* WPA */
	case 0x10:	/* WPA2 */
		printf("Warning,WPS WPA/WPA\n");
		break;
	default:
		return NULL;
    }

	// can't find "key length" in .dat and ioctl()...
	tmpProfileSetting.Key1Length = tmpProfileSetting.Key2Length = 
		tmpProfileSetting.Key3Length = tmpProfileSetting.Key4Length = 0;
	SaveToFlashInt("staKey1Length", 0);
	SaveToFlashInt("staKey2Length", 0);
	SaveToFlashInt("staKey3Length", 0);
	SaveToFlashInt("staKey4Length", 0);

#ifdef WPA_SUPPLICANT_SUPPORT
	tmpProfileSetting.EAP = Rtwpa_supplicantEAPNONE;
	SaveToFlashInt("sta8021xEAP", tmpProfileSetting.EAP);

	tmpProfileSetting.Tunnel = Rtwpa_supplicantTUNNENONE;
	SaveToFlashInt("sta8021xTunnel", tmpProfileSetting.Tunnel);

	strncpy(tmpProfileSetting.Identity, "0", IDENTITY_LENGTH);
	SaveToFlashStr("sta8021xIdentity", "0");

	strncpy(tmpProfileSetting.Password, "0", 32);
	SaveToFlashStr("sta8021xPassword", "0");

	strncpy(tmpProfileSetting.ClientCert, "0", CERT_PATH_LENGTH);
	SaveToFlashStr("sta8021xClientCert", "0");

	strncpy(tmpProfileSetting.PrivateKey, "0", PRIVATE_KEY_PATH_LENGTH);
	SaveToFlashStr("sta8021xPrivateKey", "0");

	strncpy(tmpProfileSetting.PrivateKeyPassword, "0", 32);
	SaveToFlashStr("sta8021xPrivateKeyPassword", "0");

	strncpy(tmpProfileSetting.CACert, "0", CERT_PATH_LENGTH);
	SaveToFlashStr("sta8021xCACert", "0");
#else /* WPA_SUPPLICANT_SUPPORT */
	SaveToFlashStr("sta8021xEAP", "7");
	SaveToFlashStr("sta8021xTunnel", "3");
	SaveToFlashStr("sta8021xKeyMgmt", "3");
	SaveToFlashStr("sta8021xIdentity", "0");
	SaveToFlashStr("sta8021xPassword", "0");
	SaveToFlashStr("sta8021xClientCert", "0");
	SaveToFlashStr("sta8021xPrivateKey", "0");
	SaveToFlashStr("sta8021xPrivateKeyPassword", "0");
	SaveToFlashStr("sta8021xCACert", "0");
#endif /* WPA_SUPPLICANT_SUPPORT */

	//write into /etc/rt61sta.ui
	//writeProfileToFile(&tmpProfileSetting);

	tmpProfileSetting.Active = 1;
	SaveToFlashInt("staActive", tmpProfileSetting.Active);

	nvram_commit(RT2860_NVRAM);

	freeHeaderProfileSettings();
	headerProfileSetting = NULL;

	return NULL;
}
char *getValueFromDat(char *key, char *result, int len)
{
	char a_line[512];
	FILE *file = fopen("/etc/Wireless/RT2860/RT2860.dat", "r");
	if(!file){
		//error(E_L, E_LOG, T("FATAL: open file failed!!! %s\n"), "/etc/Wireless/RT2860/RT2860.dat");
		DBG_MSG("FATAL: open file failed!!! %s\n", "/etc/Wireless/RT2860/RT2860.dat");

		return NULL;
	}

	while(fgets(a_line, 512, file)){
		char *nl1, *nl2;
		char *eq = strchr(a_line, '=');
		if(!eq)
			continue;
		*eq = '\0';
		if(!strcmp(key, a_line)){
			nl1 = strchr(eq+1, '\r');
			nl2 = strchr(eq+1, '\n');
			if(nl1)	*nl1 = '\0';
			if(nl2)	*nl2 = '\0';
			strncpy(result, eq+1, len);
			break;
		}
	}
	fclose(file);
	return result;
}
static char *addWPSSTAProfile(char *result)
{
	RT_PROFILE_SETTING  tmpProfileSetting;

	char value[512];
	
	char buffer[512] = {0}, *p;
	const char *wordlist = nvram_bufget(RT2860_NVRAM, "staActive");
	strcpy(buffer, wordlist);
	p = buffer;
	while (*p != '\0') {
		if (*p == '1')
			*p = '0';
		p++;
	}
	nvram_bufset(RT2860_NVRAM, "staActive", buffer);
	nvram_commit(RT2860_NVRAM);


	memset(&tmpProfileSetting, 0x00, sizeof(RT_PROFILE_SETTING));
	tmpProfileSetting.Next = NULL;

	//SSID, get SSID from Dat
	if(!getValueFromDat("SSID1", tmpProfileSetting.SSID, NDIS_802_11_LENGTH_SSID+1))			// get SSID from .DAT
		return NULL;
	printf("SSID1=%s\n", tmpProfileSetting.SSID);
	SaveToFlashStr("staSSID", tmpProfileSetting.SSID);

	//profile name, gen a uniq name
	snprintf(value, 512, "WPS_%s", tmpProfileSetting.SSID);
	strncpy(value, getStaNewProfileName(value), 512);
	if (!value || !strlen(value)) {
		fprintf(stderr, "Error profile name !\n");
		return NULL;
	}
	strncpy((char *)tmpProfileSetting.Profile, value, 32);
	SaveToFlashStr("staProfile", tmpProfileSetting.Profile);

	//network type
	tmpProfileSetting.NetworkType = 1;
	SaveToFlashInt("staNetworkType", 1);

	//Adhoc mode
	tmpProfileSetting.AdhocMode = 0;
	SaveToFlashInt("staAdhocMode", 0);

	//power saving mode
	tmpProfileSetting.PSmode = Ndis802_11PowerModeCAM;
	SaveToFlashInt("staPSMode", Ndis802_11PowerModeCAM);

	//channel
	tmpProfileSetting.Channel = getStaChannel("ra0");
	SaveToFlashInt("staChannel", tmpProfileSetting.Channel);

	//b preamble type
	tmpProfileSetting.PreamType = Rt802_11PreambleAuto;
	SaveToFlashInt("staPreamType", tmpProfileSetting.PreamType);

	//rts threshold value
	tmpProfileSetting.RTSCheck = 0;
	SaveToFlashInt("staRTSCheck", tmpProfileSetting.RTSCheck);
	tmpProfileSetting.RTS = 2347;
	SaveToFlashInt("staRTS", tmpProfileSetting.RTS);

	//fragment threshold value
	tmpProfileSetting.FragmentCheck = 0;
	SaveToFlashInt("staFragmentCheck", tmpProfileSetting.FragmentCheck);
	tmpProfileSetting.Fragment = 2346;
	SaveToFlashInt("staFragment", tmpProfileSetting.Fragment);

	//security policy (security_infra_mode or security_adhoc_mode)
	// get Security from .dat
	if(!getValueFromDat("AuthMode", value, 512))			// get Auth from .DAT
		return NULL;
	if(!strlen(value))
		return NULL;

	if(!strcmp(value, "OPEN")){
		tmpProfileSetting.Authentication = Ndis802_11AuthModeOpen;
	}else if(!strcmp(value, "SHARED")){
		tmpProfileSetting.Authentication = Ndis802_11AuthModeShared;
	}else if(!strcmp(value, "WEPAUTO")){
		tmpProfileSetting.Authentication = Ndis802_11AuthModeAutoSwitch;
	}else if(!strcmp(value, "WPAPSK")){
		tmpProfileSetting.Authentication = Ndis802_11AuthModeWPAPSK;
	}else if(!strcmp(value, "WPA2PSK")){
		tmpProfileSetting.Authentication = Ndis802_11AuthModeWPA2PSK;
	}else if(!strcmp(value, "WPANONE")){
		return NULL;										//no WPANONE in WPS.
	}else if(!strcmp(value, "WPA")){
		tmpProfileSetting.Authentication = Ndis802_11AuthModeWPA;
	}else if(!strcmp(value, "WPA2")){
		tmpProfileSetting.Authentication = Ndis802_11AuthModeWPA2;
	}else{
		tmpProfileSetting.Authentication = Ndis802_11AuthModeWPAPSK;
	}
	SaveToFlashInt("staAuth", tmpProfileSetting.Authentication);

#ifdef WPA_SUPPLICANT_SUPPORT
	if(tmpProfileSetting.Authentication  == Ndis802_11AuthModeWPA ||
		tmpProfileSetting.Authentication == Ndis802_11AuthModeWPA2){
		tmpProfileSetting.KeyMgmt = Rtwpa_supplicantKeyMgmtWPAEAP;
	}else if(tmpProfileSetting.Authentication == Ndis802_11AuthModeMax){
		tmpProfileSetting.KeyMgmt = Rtwpa_supplicantKeyMgmtIEEE8021X;
	}else
		tmpProfileSetting.KeyMgmt = Rtwpa_supplicantKeyMgmtNONE;
	SaveToFlashInt("sta8021xKeyMgmt", tmpProfileSetting.KeyMgmt);
#endif

	if(!getValueFromDat("Key1Str", value, 512))			
		return NULL;
	if(!strlen(value))
		strcpy(value, "0");
	strcpy(tmpProfileSetting.Key1, value);
	SaveToFlashStr("staKey1", tmpProfileSetting.Key1);

	if(!getValueFromDat("Key2Str", value, 512))			
		return NULL;
	if(!strlen(value))
		strcpy(value, "0");
	strcpy(tmpProfileSetting.Key2, value);
	SaveToFlashStr("staKey2", tmpProfileSetting.Key2);

	if(!getValueFromDat("Key3Str", value, 512))			
		return NULL;
	if(!strlen(value))
		strcpy(value, "0");
	strcpy(tmpProfileSetting.Key3, value);
	SaveToFlashStr("staKey3", tmpProfileSetting.Key3);

	if(!getValueFromDat("Key4Str", value, 512))			
		return NULL;
	if(!strlen(value))
		strcpy(value, "0");
	strcpy(tmpProfileSetting.Key4, value);
	SaveToFlashStr("staKey4", tmpProfileSetting.Key4);

	//wep key entry method
	if(!getValueFromDat("Key1Type", value, 512))			
		return NULL;
	if(!strlen(value))
		strcpy(value, "0");
	tmpProfileSetting.Key1Type = atoi(value);
	SaveToFlashInt("staKey1Type", tmpProfileSetting.Key1Type);

	if(!getValueFromDat("Key2Type", value, 512))			
		return NULL;
	if(!strlen(value))
		strcpy(value, "0");
	tmpProfileSetting.Key2Type = atoi(value);
	SaveToFlashInt("staKey2Type", tmpProfileSetting.Key2Type);

	if(!getValueFromDat("Key3Type", value, 512))			
		return NULL;
	if(!strlen(value))
		strcpy(value, "0");
	tmpProfileSetting.Key3Type = atoi(value);
	SaveToFlashInt("staKey3Type", tmpProfileSetting.Key3Type);

	if(!getValueFromDat("Key4Type", value, 512))			
		return NULL;
	if(!strlen(value))
		strcpy(value, "0");
	tmpProfileSetting.Key4Type = atoi(value);
	SaveToFlashInt("staKey4Type", tmpProfileSetting.Key4Type);

	// can't find "key length" in .dat and ioctl()...
	tmpProfileSetting.Key1Length = tmpProfileSetting.Key2Length = 
		tmpProfileSetting.Key3Length = tmpProfileSetting.Key4Length = 0;
	SaveToFlashInt("staKey1Length", 0);
	SaveToFlashInt("staKey2Length", 0);
	SaveToFlashInt("staKey3Length", 0);
	SaveToFlashInt("staKey4Length", 0);

	//wep default key
	if(!getValueFromDat("DefaultKeyID", value, 512))
		return NULL;
	if(!strlen(value))
		strcpy(value, "1");
	tmpProfileSetting.KeyDefaultId = atoi(value);
	SaveToFlashInt("staKeyDefaultId", tmpProfileSetting.KeyDefaultId);

	//Encrypt
	if(!getValueFromDat("EncrypType", value, 512))
		return NULL;
	if(!strlen(value))
		strcpy(value, "TKIP");
	if(!strcmp(value, "TKIP")){
		tmpProfileSetting.Encryption = Ndis802_11Encryption2Enabled;
	}else if(!strcmp(value, "AES")){
		tmpProfileSetting.Encryption = Ndis802_11Encryption3Enabled;
	}else if(!strcmp(value, "WEP")){
		tmpProfileSetting.Encryption = Ndis802_11WEPEnabled;
	}else if(!strcmp(value, "NONE")){
		tmpProfileSetting.Encryption = Ndis802_11WEPDisabled;
	}else
		tmpProfileSetting.Encryption = Ndis802_11Encryption2Enabled;
	SaveToFlashInt("staEncrypt", tmpProfileSetting.Encryption);

	//passphrase
	if(!getValueFromDat("WPAPSK", value, 512))
		return NULL;
	strncpy(tmpProfileSetting.WpaPsk, value, 64);
	SaveToFlashStr("staWpaPsk", tmpProfileSetting.WpaPsk);

#ifdef WPA_SUPPLICANT_SUPPORT
	tmpProfileSetting.EAP = Rtwpa_supplicantEAPNONE;
	SaveToFlashInt("sta8021xEAP", tmpProfileSetting.EAP);

	tmpProfileSetting.Tunnel = Rtwpa_supplicantTUNNENONE;
	SaveToFlashInt("sta8021xTunnel", tmpProfileSetting.Tunnel);

	strncpy(tmpProfileSetting.Identity, "0", IDENTITY_LENGTH);
	SaveToFlashStr("sta8021xIdentity", "0");

	strncpy(tmpProfileSetting.Password, "0", 32);
	SaveToFlashStr("sta8021xPassword", "0");

	strncpy(tmpProfileSetting.ClientCert, "0", CERT_PATH_LENGTH);
	SaveToFlashStr("sta8021xClientCert", "0");

	strncpy(tmpProfileSetting.PrivateKey, "0", PRIVATE_KEY_PATH_LENGTH);
	SaveToFlashStr("sta8021xPrivateKey", "0");

	strncpy(tmpProfileSetting.PrivateKeyPassword, "0", 32);
	SaveToFlashStr("sta8021xPrivateKeyPassword", "0");

	strncpy(tmpProfileSetting.CACert, "0", CERT_PATH_LENGTH);
	SaveToFlashStr("sta8021xCACert", "0");
#else /* WPA_SUPPLICANT_SUPPORT */
	SaveToFlashStr("sta8021xEAP", "7");
	SaveToFlashStr("sta8021xTunnel", "3");
	SaveToFlashStr("sta8021xKeyMgmt", "3");
	SaveToFlashStr("sta8021xIdentity", "0");
	SaveToFlashStr("sta8021xPassword", "0");
	SaveToFlashStr("sta8021xClientCert", "0");
	SaveToFlashStr("sta8021xPrivateKey", "0");
	SaveToFlashStr("sta8021xPrivateKeyPassword", "0");
	SaveToFlashStr("sta8021xCACert", "0");
#endif /* WPA_SUPPLICANT_SUPPORT */

	//write into /etc/rt61sta.ui
	//writeProfileToFile(&tmpProfileSetting);

	tmpProfileSetting.Active = 1;
	SaveToFlashInt("staActive", tmpProfileSetting.Active);

	nvram_commit(RT2860_NVRAM);

	freeHeaderProfileSettings();
	headerProfileSetting = NULL;

	if(result)
		gstrcpy(result, tmpProfileSetting.Profile);
	return result;
}

void WPSSTAEnrolleeTimerHandler(int signo)
{
	int status;
	char interface[] = "ra0";
	static int wsc_timeout_counter = 0;
	status =  getWscStatus(interface);

	if( (status == 3 || status == 35) && g_wps_timer_state == 0){	// 3 == "Start WSC Process",  35 == "SCAN_AP"
		printf("goahead: Start to monitor WSC Status...\n");
		g_wps_timer_state = 1;
		wsc_timeout_counter = 0;
	}

	if(g_wps_timer_state == 1){
//		printf("%s\n", getWscStatusStr(status));

		/* check if timeout is happened */
		wsc_timeout_counter += WPS_STA_CATCH_CONFIGURED_TIMER;
		if(wsc_timeout_counter > WPS_STA_TIMEOUT_SECS){				// 110 second
			wsc_timeout_counter = 0;
			resetTimerAll();

			do_system("iwpriv %s wsc_stop", interface);
			//trace(0, T("-- WSC failed, timeout\n"));
			DBG_MSG("-- WSC failed, timeout\n");
			g_wps_finished = TRUE;

			return;
		}

		switch(status){
			case 34 /* WSC Configured */ :
				wsc_timeout_counter = 0;
				resetTimerAll();

				/*
				 * WPS STA Enrollee mode is strange here that driver still
				 * acknowledge us the success of WPS procedure even failed actually, so we use isWPSSuceess() to
				 * get truth. 
				 */
				if(isWPSSuccess() || signo == REGISTRAR_TIMER_MODE){
					WSC_PROFILE wsc_profile;
					//trace(0, T("++ WSC success\n"));

					/*
					 * For WiFi STA WPS test plan case 5.1.1.
					 *
					 * We use ioctl(WSC_QUERY_PROFILE) to get possible multiple credentials,
					 * and the addWPSSTAProfile() should be replaced with new addWPSSTAProfile2() in the future.
					 */
					if( getWscProfile("ra0", &wsc_profile) != -1){
//						if(wsc_profile.ProfileCnt != 1){
							int i;
							DBG_MSG("%u credentials found.", wsc_profile.ProfileCnt);
							for(i=0; i< wsc_profile.ProfileCnt; i++)
								addWPSSTAProfile2(&wsc_profile.Profile[i]);
//						}else
//							addWPSSTAProfile(NULL);													
					}else{
						// add current link to station profile
						printf("Warning, can't get wsc profile!\n");
						addWPSSTAProfile(NULL);
					}
				}else{
					do_system("iwpriv %s wsc_stop", interface);
				}
			case 2:
			case 1:
				g_wps_finished = TRUE;

				return;
		/*	
			case 2: // WSC failed 
				wsc_timeout_counter = 0;
				resetTimerAll();
				do_system("iwpriv %s wsc_stop", interface);
				trace(0, T("-- WSC failed, PIN incorrect\n"));
				break;
		*/
		}
	}
}
int isSafeForShell(char *str)
{
	if(strchr(str, ';')) return 0;
	if(strchr(str, '\'')) return 0;
	if(strchr(str, '\n')) return 0;
	if(strchr(str, '`')) return 0;
	if(strchr(str, '\"')) return 0;
	return 1;
}
void WPSSTARegistrarTimerHandler(int signo)
{
	WPSSTAEnrolleeTimerHandler(REGISTRAR_TIMER_MODE);
}
static void WPSSTAPINStartEnr(char *bssid)
{
	g_wps_finished = FALSE;

	resetTimerAll();
	setTimer(WPS_STA_CATCH_CONFIGURED_TIMER * 1000, WPSSTAEnrolleeTimerHandler);

	if(!isSafeForShell(bssid))
		return;

	do_system("iwpriv ra0 wsc_cred_count 0");			// reset  creditial count
	do_system("iwpriv ra0 wsc_conf_mode 1");				// Enrollee
	do_system("iwpriv ra0 wsc_mode 1");					// PIN
	do_system("iwpriv ra0 wsc_bssid %s\n", bssid);
	do_system("iwpriv ra0 wsc_start");

	WaitWPSSTAFinish();
}
/*
 *  STA Enrollee
 *  The Browser would trigger PIN by this function.
 */
static void WPSSTAPINEnr(int nvram, char *input)
{
	char *query = NULL;

	query = strdup(web_get("query", input, 0));
	DBG_MSG("[DBG]%s", query);
	//websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	//printf("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n");
	printf("Query = %s\n", query);
	WPSSTAPINStartEnr(query);
	//websWrite(wp, T("Enrollee PIN..."));
	web_debug_header();
	printf("Enrollee PIN...");
	//websDone(wp, 200);
	free_all(1, query);	
}
static void WPSSTAPINStartReg(char *bssid, char *pin, char *force)
{
	char *wsc_cred_ssid, *wsc_cred_key;
	const char *wsc_cred_auth, *wsc_cred_encr, *wsc_cred_keyIdx;

	wsc_cred_ssid = (char *) nvram_bufget(RT2860_NVRAM, "staRegSSID");
	wsc_cred_auth = nvram_bufget(RT2860_NVRAM, "staRegAuth");
	wsc_cred_encr = nvram_bufget(RT2860_NVRAM, "staRegEncry");
	wsc_cred_keyIdx = nvram_bufget(RT2860_NVRAM, "staRegKeyIndex");
	wsc_cred_key = (char *) nvram_bufget(RT2860_NVRAM, "staRegKey");
	// The strange driver has no wep key type here

	g_wps_finished = FALSE;

	resetTimerAll();
	setTimer(WPS_STA_CATCH_CONFIGURED_TIMER * 1000, WPSSTARegistrarTimerHandler);

	if(!isSafeForShell(wsc_cred_ssid) || !isSafeForShell(wsc_cred_key) || !isSafeForShell(pin) || !isSafeForShell(bssid))
		return ;

	do_system("iwpriv ra0 set WscForceSetAP=%s", force);

	do_system("iwpriv ra0 wsc_cred_ssid \"0 %s\"", wsc_cred_ssid);
	do_system("iwpriv ra0 wsc_cred_auth \"0 %s\"", wsc_cred_auth);
	do_system("iwpriv ra0 wsc_cred_encr \"0 %s\"", wsc_cred_encr);
	do_system("iwpriv ra0 wsc_cred_keyIdx \"0 %s\"", wsc_cred_keyIdx);
	do_system("iwpriv ra0 wsc_cred_key \"0 %s\"", wsc_cred_key);
	do_system("iwpriv ra0 wsc_cred_count 1");

	do_system("iwpriv ra0 wsc_conn_by_idx 0");
	do_system("iwpriv ra0 wsc_auto_conn 2");
	do_system("iwpriv ra0 wsc_conf_mode 2");			// We are the Registrar.
	do_system("iwpriv ra0 wsc_mode 1");				// PIN
	do_system("iwpriv ra0 wsc_pin %s", pin);
	do_system("iwpriv ra0 wsc_bssid \"%s\"", bssid);	
	do_system("iwpriv ra0 wsc_start");

	WaitWPSSTAFinish();
}
/*
 *  STA Registrar
 *  The Browser would trigger PIN by this function.
 */
static void WPSSTAPINReg(int nvram, char *input)
{
	char bssid[18], pin[16] , force[2];
	char *query = NULL;
	char *sp;

	query = strdup(web_get("query", input, 0));
	DBG_MSG("[DBG]query=%s", query);
	//websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	//printf("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n");
	memset(bssid, 0, 18);
	memset(pin, 0, 16);
	memset(force, 0, 2);
	if(!query)
		return;
	sp = strchr(query, ' ');
	strncpy(pin, query, sp - query);	
	query = sp + 1;
	sp = strchr(query, ' ');
	strncpy(bssid, query, sp - query);	
	query = sp + 1;
	strcpy(force, query);	
	fprintf(stderr, "!!! bssid=%s, pin_code=%s, force=%s !!!\n", bssid, pin, force);

	WPSSTAPINStartReg(bssid, pin, force);
	//websWrite(wp, T("Registrar PIN..."));
	web_debug_header();
	printf("Registrar PIN...");
    //websDone(wp, 200);	
}
static void WPSSTAPBCStartEnr(void)
{
	g_wps_finished = FALSE;

	resetTimerAll();
	setTimer(WPS_STA_CATCH_CONFIGURED_TIMER * 1000, WPSSTAEnrolleeTimerHandler);

	do_system("iwpriv ra0 wsc_cred_count 0");			// reset  creditial count
	do_system("iwpriv ra0 wsc_conf_mode 1");				// Enrollee
	do_system("iwpriv ra0 wsc_mode 2");					// PBC
	do_system("iwpriv ra0 wsc_start");

	WaitWPSSTAFinish();
}
/*
 *  STA Enrollee
 *  The Browser would trigger PBC by this function.
 */
static void WPSSTAPBCEnr(int nvram, char *input)
{
	char *query = NULL;

	query = strdup(web_get("query", input, 0));
	DBG_MSG("[DBG]%s", query);
	//websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	// printf("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n");
	printf("Query = %s\n", query);
	WPSSTAPBCStartEnr();
	//websWrite(wp, T("Enrollee PBC..."));
	web_debug_header();
	printf("Enrollee PBC...");
	//websDone(wp, 200);	
}
static void WPSSTAPBCStartReg(void)
{
	char *wsc_cred_ssid, *wsc_cred_key;
	const char *wsc_cred_auth, *wsc_cred_encr, *wsc_cred_keyIdx;

	wsc_cred_ssid = (char *) nvram_bufget(RT2860_NVRAM, "staRegSSID");
	wsc_cred_auth = nvram_bufget(RT2860_NVRAM, "staRegAuth");
	wsc_cred_encr = nvram_bufget(RT2860_NVRAM, "staRegEncry");
	wsc_cred_keyIdx = nvram_bufget(RT2860_NVRAM, "staRegKeyIndex");
	wsc_cred_key = (char *) nvram_bufget(RT2860_NVRAM, "staRegKey");
	// The strange driver has no wep key type here

	if(!isSafeForShell(wsc_cred_ssid) || !isSafeForShell(wsc_cred_key))
		return ;

	g_wps_finished = FALSE;

	resetTimerAll();
	setTimer(WPS_STA_CATCH_CONFIGURED_TIMER * 1000, WPSSTARegistrarTimerHandler);

	do_system("iwpriv ra0 wsc_cred_ssid \"0 %s\"", wsc_cred_ssid);
	do_system("iwpriv ra0 wsc_cred_auth \"0 %s\"", wsc_cred_auth);
	do_system("iwpriv ra0 wsc_cred_encr \"0 %s\"", wsc_cred_encr);
	do_system("iwpriv ra0 wsc_cred_keyIdx \"0 %s\"", wsc_cred_keyIdx);
	do_system("iwpriv ra0 wsc_cred_key \"0 %s\"", wsc_cred_key);
	do_system("iwpriv ra0 wsc_cred_count 1");

	do_system("iwpriv ra0 wsc_conn_by_idx 0");
	do_system("iwpriv ra0 wsc_auto_conn 2");
	do_system("iwpriv ra0 wsc_conf_mode 2");			// Registrar.
	do_system("iwpriv ra0 wsc_mode 2");
	do_system("iwpriv ra0 wsc_start");

	WaitWPSSTAFinish();
}
/*
 *  STA Registrar
 *  The Browser would trigger PBC by this function.
 */
static void WPSSTAPBCReg(int nvram, char *input)
{
	char *query = NULL;

	query = strdup(web_get("query", input, 0));
	DBG_MSG("[DBG]%s", query);
	//websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	//printf("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n");
	//printf("Query = %s\n", query);
	WPSSTAPBCStartReg();
	web_debug_header();
	printf("Registrar PBC...");
	//websWrite(wp, T("Registrar PBC..."));
  //websDone(wp, 200);	
}
/*
 * these definitions are from rt2860v2 driver include/wsc.h 
 */
char *getWscStatusStr(int status)
{
	switch(status){
	case 0:
		return "Not used";
	case 1:
		return "Idle";
	case 2:
		return "WSC Fail(Ignore this if Intel/Marvell registrar used)";
	case 3:
		return "Start WSC Process";
	case 4:
		return "Received EAPOL-Start";
	case 5:
		return "Sending EAP-Req(ID)";
	case 6:
		return "Receive EAP-Rsp(ID)";
	case 7:
		return "Receive EAP-Req with wrong WSC SMI Vendor Id";
	case 8:
		return "Receive EAPReq with wrong WSC Vendor Type";
	case 9:
		return "Sending EAP-Req(WSC_START)";
	case 10:
		return "Send M1";
	case 11:
		return "Received M1";
	case 12:
		return "Send M2";
	case 13:
		return "Received M2";
	case 14:
		return "Received M2D";
	case 15:
		return "Send M3";
	case 16:
		return "Received M3";
	case 17:
		return "Send M4";
	case 18:
		return "Received M4";
	case 19:
		return "Send M5";
	case 20:
		return "Received M5";
	case 21:
		return "Send M6";
	case 22:
		return "Received M6";
	case 23:
		return "Send M7";
	case 24:
		return "Received M7";
	case 25:
		return "Send M8";
	case 26:
		return "Received M8";
	case 27:
		return "Processing EAP Response (ACK)";
	case 28:
		return "Processing EAP Request (Done)";
	case 29:
		return "Processing EAP Response (Done)";
	case 30:
		return "Sending EAP-Fail";
	case 31:
		return "WSC_ERROR_HASH_FAIL";
	case 32:
		return "WSC_ERROR_HMAC_FAIL";
	case 33:
		return "WSC_ERROR_DEV_PWD_AUTH_FAIL";
	case 34:
		return "Configured";
	case 35:
		return "SCAN AP";
	case 36:
		return "EAPOL START SENT";
	case 37:
		return "WSC_EAP_RSP_DONE_SENT";
	case 38:
		return "WAIT PINCODE";
	case 39:
		return "WSC_START_ASSOC";
	case 0x101:
		return "PBC:TOO MANY AP";
	case 0x102:
		return "PBC:NO AP";
	case 0x103:
		return "EAP_FAIL_RECEIVED";
	case 0x104:
		return "EAP_NONCE_MISMATCH";
	case 0x105:
		return "EAP_INVALID_DATA";
	case 0x106:
		return "PASSWORD_MISMATCH";
	case 0x107:
		return "EAP_REQ_WRONG_SMI";
	case 0x108:
		return "EAP_REQ_WRONG_VENDOR_TYPE";
	case 0x109:
		return "PBC_SESSION_OVERLAP";
	default:
		return "Unknown";
	}
}
int getWscStatus(char *interface)
{
	int socket_id;
	struct iwreq wrq;
	int data = 0;
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	strcpy(wrq.ifr_name, interface);
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = (caddr_t) &data;
	wrq.u.data.flags = RT_OID_WSC_QUERY_STATUS;
	if( ioctl(socket_id, RT_PRIV_IOCTL, &wrq) == -1)
		printf("ioctl error\n");
	close(socket_id);
	//DBG_MSG("[DBG]data=%d", data); //Landen
	return data;
}
static void WPSSTAStop(int nvram, char *input)
{
	char interface[] = "ra0";
	char *query = NULL;
	query = strdup(web_get("query", input, 0));
	DBG_MSG("[DBG]%s", query);
	resetTimerAll();
	do_system("iwpriv ra0 wsc_stop");

	//websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	//printf("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n");
	web_debug_header();
	//websWrite(wp, T("%s"), getWscStatusStr(getWscStatus(interface)));
	printf("%s", getWscStatusStr(getWscStatus(interface)));
	//websDone(wp, 200);	
	return;
}

static void WPSSTAGenNewPIN(int nvram, char *input)
{
	char pin[16];
		char *query = NULL;
	query = strdup(web_get("query", input, 0));
	DBG_MSG("%s", query);
	do_system("iwpriv ra0 wsc_gen_pincode");
	//websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	//printf("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n");
	if(!getSTAEnrolleePIN(pin)){
		//websWrite(wp, T("error"));
		DBG_MSG("error");
		web_debug_header();
		printf("error");
	}else{
		//websWrite(wp, T("%s"), pin);
		DBG_MSG("%s", pin);
		web_debug_header();
		printf("%s", pin);
	}
    //websDone(wp, 200);
}
static void WPSSTARegistrarSetupSSID(int nvram, char *input)
{
	char *query = NULL;
	query = strdup(web_get("query", input, 0));
	DBG_MSG("%s", query);
	nvram_bufset(RT2860_NVRAM, "staRegSSID", query);
	nvram_commit(RT2860_NVRAM);
	//websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	//websWrite(wp, T("WPS STA Registrar settings: SSID done\n"));
	//websDone(wp, 200);
	//printf("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n");
	web_debug_header();
	printf("WPS STA Registrar settings: SSID done\n");
	

}
static void WPSSTARegistrarSetupRest(int nvram, char *input)
{
  char *query = NULL;
	char auth[32], encrypt[32], keytype[2], keyindex[2];
	query = strdup(web_get("query", input, 0));
	DBG_MSG("query = %s\n", query);
	sscanf(query, "%32s %32s %2s %2s", auth, encrypt, keytype, keyindex);
	printf("auth = %s\n", auth);
	nvram_bufset(RT2860_NVRAM, "staRegAuth", auth);
	nvram_bufset(RT2860_NVRAM, "staRegEncry", encrypt);
	nvram_bufset(RT2860_NVRAM, "staRegKeyType", keytype);
	nvram_bufset(RT2860_NVRAM, "staRegKeyIndex", keyindex);
	nvram_commit(RT2860_NVRAM);
	//websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	//websWrite(wp, T("WPS STA Registrar settings: rest all done\n"));
	//websDone(wp, 200);
	//printf("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n");
	web_debug_header();
	printf("WPS STA Registrar settings: rest all done\n");
}

static void WPSSTARegistrarSetupKey(int nvram, char *input)
{
	char *query = NULL;
	query = strdup(web_get("query", input, 0));
	DBG_MSG("query = %s\n", query);
	nvram_bufset(RT2860_NVRAM, "staRegKey", query);
	nvram_commit(RT2860_NVRAM);
	//websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	//websWrite(wp, T("WPS STA Registrar settings: Key done\n"));
	//websDone(wp, 200);
	//printf("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n");
	web_debug_header();
	printf("WPS STA Registrar settings: Key done\n");
}

/*
 *  The Browser will pull STA WPS info. from this function.
 */
static void updateWPSStaStatus(int nvram, char *input)
{
	char interface[] = "ra0";
	//websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	//websWrite(wp, T("%s"), getWscStatusStr(getWscStatus(interface)));
	//printf("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n");
	web_debug_header();
	printf("%s", getWscStatusStr(getWscStatus(interface)));
	//websDone(wp, 200);
	return;
}
static void WPSSTABSSIDListReset(int nvram, char *input)
{
	DBG_MSG("WPSSTABSSIDListReset");
	//if (nvram_bufget(RT2860_NVRAM, "g_pAPListData") != NULL) {
	if (strcmp(nvram_bufget(RT2860_NVRAM, "g_pAPListData"), "") != 0) {
		nvram_bufset(RT2860_NVRAM, "g_pAPListData", "");
	}
	nvram_commit(RT2860_NVRAM);
	/*
	if(g_pAPListData)
		*g_pAPListData = '\0';
		*/
	//websHeader(wp);
	//websWrite(wp, T("BSSIDList reset\n"));
	//websFooter(wp);
	//websDone(wp, 200);
	web_debug_header();
	printf("BSSIDList reset\n");
	return;
}
#endif

#if defined (CONFIG_RT2860V2_HW_STA_ANTENNA_DIVERSITY)
void AntennaDiversityInit(void)
{
	const char *mode = nvram_bufget(RT2860_NVRAM, "AntennaDiversity");

	if(!strcmp(mode, "Disable")){						// Disable
		do_system("echo 0 > /proc/AntDiv/AD_RUN");
	}else if(!strcmp(mode, "Enable_Algorithm1")){
		do_system("echo 1 > /proc/AntDiv/AD_ALGORITHM"); // Algorithm1
		do_system("echo 1 > /proc/AntDiv/AD_RUN");
	}else if(!strcmp(mode, "Enable_Algorithm2")){
		do_system("echo 2 > /proc/AntDiv/AD_ALGORITHM"); // Algorithm2
		do_system("echo 1 > /proc/AntDiv/AD_RUN");
	}else if(!strcmp(mode, "Antenna0")){				// fix Ant0
		do_system("echo 0 > /proc/AntDiv/AD_RUN");
		do_system("echo 0 > /proc/AntDiv/AD_FORCE_ANTENNA");
	}else if(!strcmp(mode, "Antenna2")){				// fix Ant2
		do_system("echo 0 > /proc/AntDiv/AD_RUN");
		do_system("echo 2 > /proc/AntDiv/AD_FORCE_ANTENNA");
	}else{
		do_system("echo 0 > /proc/AntDiv/AD_RUN");
	return;
	}

	return;
}
static void GetAntenna(int nvram, char *input)
{
	char buf[32];
	DBG_MSG("");
	FILE *fp = fopen("/proc/AntDiv/AD_CHOSEN_ANTENNA", "r");
	if(!fp){
		strcpy(buf, "not support\n");
	}else{
		fgets(buf, 32, fp);
		fclose(fp);
	}
	//websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	//websWrite(wp, "%s", buf);
	//websDone(wp, 200);
	web_debug_header();
	printf("%s", buf);
}

static void AntennaDiversity(int nvram, char *input)
{
	char	*mode;
	web_debug_header();
	mode = strdup(web_get("ADSelect", input, 1));
	DBG_MSG("mode=%s", mode);
	//mode = websGetVar(wp, T("ADSelect"), T(""));
	if(!mode || !strlen(mode))
		return;
	
	nvram_bufset(RT2860_NVRAM, "AntennaDiversity", mode);
	nvram_commit(RT2860_NVRAM);

	// re-init
	AntennaDiversityInit();

	//debug print
	//websHeader(wp);
	//websWrite(wp, T("mode:%s"), mode);
	//websFooter(wp);
	//websDone(wp, 200);	
	
	printf("mode:%s", mode);
}
#endif

int main(int argc, char *argv[]) 
{
	char *page, *inStr;
	int wps_enable, nvram_id;
	long inLen;

	if ((argc > 1) && (!strcmp(argv[1], "init"))) {
#if ! defined CONFIG_FIRST_IF_NONE 
		nvram_init(RT2860_NVRAM);
#if defined (RT2860_WSC_SUPPORT)
		restart_wps(RT2860_NVRAM, 
				strtol(nvram_bufget(RT2860_NVRAM, "WscModeOption"), NULL, 10));
#endif
		update_flash_8021x(RT2860_NVRAM);
		restart_8021x(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
#endif
#if ! defined CONFIG_SECOND_IF_NONE
		nvram_init(RTDEV_NVRAM);
#if defined (RTDEV_WSC_SUPPORT)
		restart_wps(RTDEV_NVRAM, 
				strtol(nvram_bufget(RTDEV_NVRAM, "WscModeOption"), NULL, 10));
#endif
		update_flash_8021x(RTDEV_NVRAM);
		restart_8021x(RTDEV_NVRAM);
		nvram_close(RTDEV_NVRAM);
#endif
#if defined (RT2860_WAPI_SUPPORT) || defined (RTDEV_WAPI_SUPPORT)
		restart_wapi();
#endif
		return;
	} else if ((argc > 1) && (!strcmp(argv[1], "wps_pbc"))) {
#if defined (RT2860_WSC_SUPPORT) || defined (RTDEV_WSC_SUPPORT)
		wps_ap_pbc_start_all(strtol(argv[2], NULL, 10));
#endif
		return;
	}
	inLen = strtol(getenv("CONTENT_LENGTH"), NULL, 10) + 1;
	if (inLen <= 1) {
		DBG_MSG("get no data!");
		return;
	}
	inStr = malloc(inLen);
	memset(inStr, 0, inLen);
	fgets(inStr, inLen, stdin);
	//DBG_MSG("%s\n", inStr);
	if ((nvram_id = getNvramIndex(web_get("wlan_conf", inStr, 0))) == -1) {
		DBG_MSG("get config zone fail");
		return;
	}
	nvram_init(nvram_id);
	page = web_get("page", inStr, 0);
	if (!strcmp(page, "basic")) {
		set_wifi_basic(nvram_id, inStr);
	} else if (!strcmp(page, "advance")) {
		set_wifi_advance(nvram_id, inStr);
	} else if (!strcmp(page, "wmm")) {
		set_wifi_wmm(nvram_id, inStr);
	} else if (!strcmp(page, "security")) {
		set_wifi_security(nvram_id, inStr);
#if defined (RT2860_WDS_SUPPORT) || defined (RTDEV_WDS_SUPPORT)
	} else if (!strcmp(page, "wds")) {
		set_wifi_wds(nvram_id, inStr);
#endif
#if defined (RT2860_APCLI_SUPPORT)
	} else if (!strcmp(page, "apclient")) {
		set_wifi_apclient(nvram_id, inStr);
#endif
	} else if (!strcmp(page, "apstatistics")) {
		reset_wifi_state(nvram_id, inStr);
#if defined (RT2860_WSC_SUPPORT) || defined (RTDEV_WSC_SUPPORT)
	} else if (!strcmp(page, "WPSConfig")) {
		set_wifi_wpsconf(nvram_id, inStr);
	} else if (!strcmp(page, "GenPINCode")) {
		set_wifi_gen_pin(nvram_id, inStr);
	} else if (!strcmp(page, "SubmitOOB")) {
		set_wifi_wps_oob(nvram_id, inStr);
	} else if (!strcmp(page, "WPS")) {
		set_wifi_do_wps(nvram_id, inStr);
	} else if (!strcmp(page, "wps_cancel")) {
		set_wifi_cancel_wps(nvram_id, inStr);
#endif	 
#if defined (RT2860_HS_SUPPORT) || defined (RTDEV_HS_SUPPORT)
	} else if (!strcmp(page, "hotspot")) {
		set_wifi_hotspot(nvram_id, inStr);
#endif
#if defined (CONFIG_RALINK_MT7628)	
	}else if (!strcmp(page, "obtw")) {
     obtw(nvram_id, inStr);
#endif
#if defined (CONFIG_SECOND_IF_MT7612E)	&& defined(CONFIG_FIRST_IF_MT7628)	
	}else if (!strcmp(page, "obtw_mt7612")) {
     obtw_mt7612(nvram_id, inStr);
#endif
#if defined (CONFIG_RT2860V2_STA_WSC)
	}else if (!strcmp(page, "WPSSTAMode0")) {
     		WPSSTAMode0(nvram_id, inStr);
 	}else if (!strcmp(page, "WPSSTAMode1")) {
     		WPSSTAMode1(nvram_id, inStr);    
 	}else if (!strcmp(page, "WPSSTAPINReg")) {
     		WPSSTAPINReg(nvram_id, inStr);        
	}else if (!strcmp(page, "WPSSTAPINEnr")) {
		WPSSTAPINEnr(nvram_id, inStr);  
	}else if (!strcmp(page, "WPSSTAPBCEnr")) {
   		WPSSTAPBCEnr(nvram_id, inStr);  
	}else if (!strcmp(page, "WPSSTAPBCReg")) {
   		WPSSTAPBCReg(nvram_id, inStr);  
	}else if (!strcmp(page, "WPSSTAStop")) {
   		WPSSTAStop(nvram_id, inStr); 
	}else if (!strcmp(page, "WPSSTAGenNewPIN")) {
   		WPSSTAGenNewPIN(nvram_id, inStr); 
	}else if (!strcmp(page, "WPSSTARegistrarSetupSSID")) {
   		WPSSTARegistrarSetupSSID(nvram_id, inStr);     
	}else if (!strcmp(page, "WPSSTARegistrarSetupRest")) {
   		WPSSTARegistrarSetupRest(nvram_id, inStr);       
	}else if (!strcmp(page, "WPSSTARegistrarSetupKey")) {
   		WPSSTARegistrarSetupKey(nvram_id, inStr);       
	}else if (!strcmp(page, "updateWPSStaStatus")) {
   		updateWPSStaStatus(nvram_id, inStr);      
	}else if (!strcmp(page, "WPSSTABSSIDListReset")) {
   		WPSSTABSSIDListReset(nvram_id, inStr);          
#endif
#if defined (CONFIG_RT2860V2_HW_STA_ANTENNA_DIVERSITY)
	}else if (!strcmp(page, "GetAntenna")) {
		GetAntenna(nvram_id, inStr);   
	}else if (!strcmp(page, "AntennaDiversity")) {
		AntennaDiversity(nvram_id, inStr);   		
#endif
	}
	free(inStr);
	nvram_close(nvram_id);
	
	return 0;
}
