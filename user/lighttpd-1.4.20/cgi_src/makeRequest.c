#include "utils.h"
#include <stdlib.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/wireless.h>
#include "oid.h"

extern void getCurrentWscProfile(char *interface, WSC_CONFIGURED_VALUE *data, int len);
extern int getWscStatus(char *interface);

void arplookup(char *ip, char *arp)
{
	char buf[256];
	FILE *fd = fopen("/proc/net/arp", "r");
	if(!fd){
		strcpy(arp, "");
		return;
	}
	strcpy(arp, "00:00:00:00:00:00");
	while(fgets(buf, 256, fd)){
		char ip_entry[12], hw_address[18];
		sscanf(buf, "%16s %*s %*s %18s %*s %*s", ip_entry, hw_address);
		if(!strcmp(ip, ip_entry)){
			strcpy(arp, hw_address);
			break;
		}
	}
	fclose(fd);
}

void GetCloneMac()
{
	char macAddr[18];
	arplookup(getenv("REMOTE_ADDR"), macAddr);
	web_debug_header();
	printf("%s", macAddr);
}

#if defined (RT2860_WSC_SUPPORT) || defined (RTDEV_WSC_SUPPORT)
void getWPSAuthMode(WSC_CONFIGURED_VALUE *result, char *ret_str)
{
	if(result->WscAuthMode & 0x1)
		strcat(ret_str, "Open");
	if(result->WscAuthMode & 0x2)
		strcat(ret_str, "WPA-PSK");
	if(result->WscAuthMode & 0x4)
		strcat(ret_str, "Shared");
	if(result->WscAuthMode & 0x8)
		strcat(ret_str, "WPA");
	if(result->WscAuthMode & 0x10)
		strcat(ret_str, "WPA2");
	if(result->WscAuthMode & 0x20)
		strcat(ret_str, "WPA2-PSK");
}

void getWPSEncrypType(WSC_CONFIGURED_VALUE *result, char *ret_str)
{
	if(result->WscEncrypType & 0x1)
		strcat(ret_str, "None");
	if(result->WscEncrypType & 0x2)
		strcat(ret_str, "WEP");
	if(result->WscEncrypType & 0x4)
		strcat(ret_str, "TKIP");
	if(result->WscEncrypType & 0x8)
		strcat(ret_str, "AES");
}

/*
 *  * these definitions are from rt2860v2 driver include/wsc.h
 *   */
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

void UpdateAPWPS(char *if_name)
{
	int i, status, WscResult = 0;
	char tmp_str[128];
	WSC_CONFIGURED_VALUE result;

	web_debug_header();
	getCurrentWscProfile(if_name, &result, sizeof(WSC_CONFIGURED_VALUE));

	//1. WPSConfigured
	printf("%d\t", result.WscConfigured);

	//2. WPSSSID
	if(strchr(result.WscSsid, '\n')){
		printf("Invalid SSID character: new line\t");
	}else{
		printf("%s\t", result.WscSsid);
	}

	//3. WPSAuthMode
	tmp_str[0] = '\0';
	getWPSAuthMode(&result, tmp_str);
	printf("%s\t", tmp_str);

	//4. EncrypType
	tmp_str[0] = '\0';
	getWPSEncrypType(&result, tmp_str);
	printf("%s\t", tmp_str);

	//5. DefaultKeyIdx
	printf("%d\t", result.DefaultKeyIdx);

	//6. Key
	for(i=0; i<64 && i<strlen(result.WscWPAKey); i++){                // WPA key default length is 64 (defined & hardcode in driver)
		if(i!=0 && !(i % 32))
			printf("<br>");
		printf("%c", result.WscWPAKey[i]);
	}
	printf("\t");

	//7. WSC Status
	status = getWscStatus(if_name);
	//DBG_MSG("%d", status);
	printf("%s\t", getWscStatusStr(status));

	//8. WSC Result
	if (status == 0x2 || status == 0x109)
		WscResult = -1;
	else if (status == 34)
		WscResult = 1;
	printf("%d\t", WscResult);

	//9. WSC Status Index
	printf("%d", status);
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

int any_wsc_enabled(char* wsc_conf_mode)
{
	if (NULL == wsc_conf_mode)
		return 0;

	if (NULL != strstr(wsc_conf_mode, "7"))
		return 1;

	if (NULL != strstr(wsc_conf_mode, "4"))
		return 1;

	if (NULL != strstr(wsc_conf_mode, "2"))
		return 1;

	if (NULL != strstr(wsc_conf_mode, "1"))
		return 1;

	return 0;
}

//Landen: CP functions from wireless for Ajax, do not reload page for DBDC ssid changed.
static void restart_if_wps(int nvram, int wsc_enable, const char *ifname)
{
	char *wsc_conf_mode;

	do_system("iwpriv %s set WscConfMode=0", ifname);
	do_system("route delete 239.255.255.250");
	if (wsc_enable > 0) {
		do_system("iwpriv %s set WscConfMode=%d", ifname, 7);
		do_system("route add -host 239.255.255.250 dev br0");
	} else {
		wsc_conf_mode = nvram_bufget(nvram, "WscConfMode");
		if(any_wsc_enabled(wsc_conf_mode))
			do_system("route add -host 239.255.255.250 dev br0");
	}
		
	do_system("miniupnpd.sh init");
#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM)
		do_system("killall -SIGXFSZ nvram_daemon");
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM)
		do_system("killall -SIGWINCH nvram_daemon");
#endif
}

static char* get_ifname_by_idx(int nvram, char* input, char* ifname, int ifname_size, int* index)
{ 
	char ssid_index_str[4];
	int  ssid_index;

	get_nth_value(2, input, '&', ssid_index_str, sizeof(ssid_index_str));
	if(ssid_index_str == NULL)
		return NULL;

	ssid_index = (int)strtol(ssid_index_str, NULL, 10);
	if( NULL != index)
		*index = ssid_index;
 
#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM) {
		snprintf(ifname, ifname_size, "ra%s", ssid_index_str);
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM) {
		snprintf(ifname, ifname_size, "rai%s", ssid_index_str);
	}
#endif

	return ifname; 
}

static char* get_apcli_name_by_idx(int nvram, char* input, char* ifname, int ifname_size, int* index)
{ 
	char ssid_index_str[4];
	int  ssid_index;

	get_nth_value(2, input, '&', ssid_index_str, sizeof(ssid_index_str));
	if(ssid_index_str == NULL)
		return NULL;

	ssid_index = (int)strtol(ssid_index_str, NULL, 10);
	if( NULL != index)
		*index = ssid_index;
 
#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM) {
		snprintf(ifname, ifname_size, "apcli%s", ssid_index_str);
	} 
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if  (nvram == RTDEV_NVRAM) {
		snprintf(ifname, ifname_size, "apclii%s", ssid_index_str);
	}
#endif

	return ifname; 
}

/* input is sent from Web Page makeRequest2.
 * input format: WPSConfig&[nvram]&[ssid_index]&WPSEnable=[0|1]
 * input sample: WPSConfig&2860&1&WPSEnable=1 */
void set_wifi_wpsconf(int nvram, char *input)
{
	int  wsc_enable = 0;
	char ifname[IFNAMSIZ];
	int  ssid_index;

	if(NULL == get_ifname_by_idx(nvram, input, ifname, sizeof(ifname), &ssid_index)){
		printf("{\"wpsconf\":\"GET_IFNAME_NG\"}");
		return;
	}

	wsc_enable = (int)strtol(web_get("WPSEnable", input, 0), NULL, 0);
	//resetTimerAll();
	//g_WscResult = 0;
	//LedReset();

	nvram_init(nvram);
	if (wsc_enable == 0){
		set_nth_value_flash(nvram, ssid_index, "WscConfMode", "0");
	}else{
		set_nth_value_flash(nvram, ssid_index, "WscConfMode", "7");
	}
	nvram_commit(nvram);
	restart_if_wps(nvram, wsc_enable, ifname);
	nvram_close(nvram);
	printf("{\"wpsconf\":\"OK\"}");
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

/* input is sent from Web Page makeRequest2.
 * input format: GetPINCode&[nvram]&[ssid_index]
 * input sample: GetPINCode&2860&1 */
int get_pin_code(int nvram, char *input)
{
	char ifname[IFNAMSIZ] = {0};
	
	if (NULL == get_ifname_by_idx(nvram, input, ifname, sizeof(ifname), NULL)) {
		printf("{\"getpincode\":\"GET_IFNAME_NG\"}");
		return 1;
	}

	printf("{\"getpincode\":\"%08d\"}", get_ap_pin(ifname));

	return 0;
}

/* input is sent from Web Page makeRequest2.
 * input format: GenPINCode&[nvram]&[ssid_index]
 * input sample: GenPINCode&2860&1 */
void set_wifi_gen_pin(int nvram, char *input)
{
	char new_pin[9];
	char ifname[6];
	int  ssid_index = 0;

	if(NULL == get_ifname_by_idx(nvram, input, ifname, sizeof(ifname), &ssid_index)){
		printf("{\"genpincode\":\"GET_IFNAME_NG\"}");
		return;
	}

	nvram_init(nvram);
	do_system("iwpriv %s  set WscGenPinCode", ifname);
	snprintf(new_pin, sizeof(new_pin), "%08d", get_ap_pin(ifname));
	//nvram_bufset(nvram, "WscVendorPinCode", new_pin);
	set_nth_value_flash(nvram, ssid_index, "WscVendorPinCode", new_pin);
	nvram_commit(nvram);
	nvram_close(nvram);

	if (nvram == RT2860_NVRAM)
		do_system("ralink_init make_wireless_config rt2860");
	else
		do_system("ralink_init make_wireless_config rtdev");
	printf("{\"genpincode\":\"%s\"}", new_pin);
	//web_redirect(getenv("HTTP_REFERER"));
}

/* input is sent from Web Page makeRequest2.
 * input format: SetPINCode&[nvram]&[ssid_index]&[new_pincode]
 * input sample: SetPINCode&2860&1&12345678 */
void set_wifi_set_pin(int nvram, char *input)
{
	char new_pin[9];
	char ifname[6];
	int  ssid_index = 0;

	if(NULL == get_ifname_by_idx(nvram, input, ifname, sizeof(ifname), &ssid_index)){
		printf("{\"setpincode\":\"GET_IFNAME_NG\"}");
		return;
	}

	get_nth_value(3, input, '&', new_pin, sizeof(new_pin));
	if(NULL == new_pin){
		printf("{\"setpincode\":\"GET_NEW_PIN_NG\"}");
		return;
	}

	nvram_init(nvram);
	do_system("iwpriv %s set WscVendorPinCode=%s", ifname, new_pin);
	set_nth_value_flash(nvram, ssid_index, "WscVendorPinCode", new_pin);
	nvram_commit(nvram);
	nvram_close(nvram);

	if (nvram == RT2860_NVRAM)
		do_system("ralink_init make_wireless_config rt2860");
	else
		do_system("ralink_init make_wireless_config rtdev");
	printf("{\"setpincode\":\"%s\"}", "OK");
	//web_redirect(getenv("HTTP_REFERER"));
}

/* input is sent from Web Page makeRequest2.
 * input format: SubmitOOB&[nvram]&[ssid_index]
 * input sample: SubmitOOB&2860&1 */
void set_wifi_wps_oob(int nvram, char *input)
{
	char SSID[64], *mac;
	char ssid_nvram[8] = {0};
	int  ssid_index = 0;
	char ifname[IFNAMSIZ] = {0};
	
	nvram_init(nvram);
	if(NULL == get_ifname_by_idx(nvram, input, ifname, sizeof(ifname), &ssid_index)){
		printf("{\"wps_oob\":\"GET_IFNAME_NG\"}");
		return;
	}
	
	if ((mac = get_macaddr(ifname)) != NULL)
		snprintf(SSID, sizeof(SSID), "RalinkInitAP%d_%s", ssid_index, mac);
	else
		snprintf(SSID, sizeof(SSID), "RalinkInitAP%d_unknown", ssid_index);

	//nvram_bufset(nvram, "SSID1", SSID);
	nvram_bufset(nvram, racat("SSID", ssid_index + 1), SSID);

	//Landen: Change legacy name.
	//nvram_bufset(nvram, "WscConfigured", "0");
	set_nth_value_flash(nvram, ssid_index, "WscConfStatus", "1");

	set_nth_value_flash(nvram, ssid_index, "AuthMode", "WPA2PSK");
	set_nth_value_flash(nvram, ssid_index, "EncrypType", "AES");
	set_nth_value_flash(nvram, ssid_index, "DefaultKeyID", "2");
	//nvram_bufset(nvram, "WPAPSK1", "12345678");
	nvram_bufset(nvram, racat("WPAPSK", ssid_index + 1), "12345678");

	set_nth_value_flash(nvram, ssid_index, "IEEE8021X", "0");

	/*
	 *   IMPORTANT !!!!!
	 *   5VT doesn't need it cause it will reboot after OOB reset, but RT2880 does.
	 */
	//g_wsc_configured = 0;

	nvram_commit(nvram);

	do_system("iwpriv %s set SSID=%s", ifname,  SSID);
	do_system("iwpriv %s set AuthMode=WPA2PSK", ifname);
	do_system("iwpriv %s set EncrypType=AES", ifname);
	do_system("iwpriv %s set WPAPSK=12345678", ifname);
	do_system("iwpriv %s set SSID=%s", ifname, SSID);

#if ! defined CONFIG_FIRST_IF_NONE || ! defined CONFIG_SECOND_IF_NONE
	restart_8021x(nvram);
#endif
	restart_if_wps(nvram, 1, ifname);
	nvram_close(nvram);
	do_system("iwpriv %s set WscConfStatus=1", ifname);


	printf("{\"wps_oob\":\"OK\"}");
	//web_redirect(getenv("HTTP_REFERER"));
}

static void wps_ap_pbc_start_all(int nvram, char *ifname)
{
	do_system("iwpriv %s set WscMode=2", ifname);
	do_system("iwpriv %s set WscGetConf=1", ifname);
}

static void wps_ap_pin_start_all(int nvram, char *ifname, char* pincode)
{
	do_system("iwpriv %s set WscMode=1", ifname);
	do_system("iwpriv %s set WscPinCode=%s", ifname, pincode);
	do_system("iwpriv %s set WscGetConf=1", ifname);
}

/* input is sent from Web Page makeRequest2.
 * input format: SetPINCode&[nvram]&[ssid_index]&[PIN|PBC]&[pincode]
 * input sample: SetPINCode&2860&1&PBC
 * input sample: SetPINCode&2860&1&PIN&00000000 */
void set_wifi_do_wps(int nvram, char *input)
{
	char wsc_pin_code_w[12] = {0};
	int  ssid_index = 0;
	int  wsc_conf_mode;
	int  wsc_mode;
	char ifname[IFNAMSIZ] = {0};
	char *multi_wsc_conf_mode;

	if (NULL == get_ifname_by_idx(nvram, input, ifname, sizeof(ifname), &ssid_index)) {
		printf("{\"wps_start\":\"GET_IFNAME_NG\"}");
		return;
	}

	nvram_init(nvram);

	multi_wsc_conf_mode = nvram_bufget(nvram, "WscConfMode");
	wsc_conf_mode = get_nth_number(ssid_index, multi_wsc_conf_mode, ';', 10);

	if (wsc_conf_mode == 0) {
		printf("{\"wps_start\":\"WPS_NOT_ENABLED\"}");
		DBG_MSG("WPS is not enabled before do PBC/PIN.\n");
		return;
	}
	
	wsc_mode = get_nth_number(3, input, '&', 10);
	DBG_MSG("[AJAX]wsc_mode=%d\n", wsc_mode);

	// reset wsc result indicator
	//g_WscResult = 0;
	//LedReset();
	if (wsc_mode == 1) {
		// get pin code
		if(get_nth_value(4, input, '&', wsc_pin_code_w, sizeof(wsc_pin_code_w)) != 0) {
			printf("{\"wps_start\":\"GET_PINCODE_NG\"}");
			DBG_MSG("[AJAX]Get PIN code NG. index=%d, input=%s, delimit=%c\n", 4, input, '&');
			return;
		}
		DBG_MSG("[AJAX]wsc_pin_code_w=%s\n", wsc_pin_code_w);
		wps_ap_pin_start_all(nvram, ifname, wsc_pin_code_w);

		//resetTimerAll();
		//setTimer(WPS_AP_CATCH_CONFIGURED_TIMER * 1000, WPSAPTimerHandler);
	} else if (wsc_mode == 2) {
		wps_ap_pbc_start_all(nvram, ifname);
	} else {
		DBG_MSG("ignore unknown WSC method: %d", wsc_mode);
		printf("{\"wps_start\":\"NG\"}");
		return;
	}
	nvram_bufset(nvram, "WscStartIF", ifname);
	nvram_close(nvram); //it will check if commit the changes.
#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM)
		do_system("killall -SIGXFSZ nvram_daemon");
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM)
		do_system("killall -SIGWINCH nvram_daemon");
#endif
	printf("{\"wps_start\":\"OK\"}");
}

/* input is sent from Web Page makeRequest2.
 * input format: wps_cancel&[nvram]&[ssid_index]
 * input sample: wps_cancel&2860&1 */
void set_wifi_cancel_wps(int nvram, char *input)
{
	int  ssid_index = 0;
	char ifname[IFNAMSIZ] = {0};

	if(NULL == get_ifname_by_idx(nvram, input, ifname, sizeof(ifname), &ssid_index)){
		printf("{\"wps_cancel\":\"NG\"}");
		return;
	}

	//resetTimerAll();
	do_system("iwpriv %s set WscStop=1", ifname);
	do_system("miniupnpd.sh init");
	printf("{\"wps_cancel\":\"OK\"}");
}

#if defined (RT2860_APCLI_SUPPORT)
void apcli_scan(int nvram, char *input)
{
	FILE *pp;
	char cmd[CMDLEN], *ptr, wif[IFNAMSIZ];
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
	char special_if[IFNAMSIZ];
	char filter[4];
	int total_ssid_num, get_ssid_times, round;

	get_nth_value(2, input, '&', special_if, sizeof(special_if));
	get_nth_value(3, input, '&', filter, sizeof(filter));

	if (nvram == RTDEV_NVRAM)
		strncpy(wif, "rai0", sizeof(wif));
	else
		strncpy(wif, "ra0", sizeof(wif));

	// special interface
	if (strlen(special_if) > 0)
		strncpy(wif, special_if, sizeof(wif));

	do_system("iwpriv %s set SiteSurvey=1", wif);
	
	sleep(6); // for get the SCAN result. (2G + 5G may enough for 5 seconds)
	
	sprintf(cmd, "iwpriv %s get_site_survey", wif);
	if(!(pp = popen(cmd, "r"))) {
		DBG_MSG("execute get_site_survey fail!");
		return;
	}

	memset(cmd, 0, sizeof(cmd));

	web_debug_header();
#if defined (FIRST_MT7615_SUPPORT) || defined (SECOND_MT7615_SUPPORT)
	fgets(cmd, sizeof(cmd), pp);
	fgets(cmd, sizeof(cmd), pp);
	ptr = cmd;
	total_ssid_num = 0;
	sscanf(ptr, "Total=%d", &total_ssid_num);
	get_ssid_times = (total_ssid_num / 32) + 1;
	DBG_MSG("total_ssid_num=%d, get_ssid_times=%d", total_ssid_num, get_ssid_times);

	fgets(cmd, sizeof(cmd), pp);
	//TODO: Get more than 1 page of site survey result.
	int index = 0;
	printf("{");
	for (round = 0; round < get_ssid_times; round++) {
		if(round != 0) {
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "iwpriv %s get_site_survey %d", wif, round*32);

			if(pp != NULL)
				pclose(pp);

			if(!(pp = popen(cmd, "r"))) {
				DBG_MSG("execute get_site_survey fail!");
				return;
			}

			fgets(cmd, sizeof(cmd), pp);
			fgets(cmd, sizeof(cmd), pp);
			fgets(cmd, sizeof(cmd), pp);
		}

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

			if (!strcmp(filter, "WPS")) {
				if (!strcmp(wps, "YES")) {
					printf("\"%d\":{", index++);
					printf("\"channeld\":\"%d\",", channeld);
					printf("\"ssid\":\"%s\",", ssid);
					printf("\"bssid\":\"%s\",", bssid);
					printf("\"security\":\"%s\",", security);
					printf("\"signald\":\"%d\",", signald);
					printf("\"mode\":\"%s\",", mode);
					printf("\"ext_ch\":\"%s\",", ext_ch);
					printf("\"net_type\":\"%s\",", net_type);
					printf("\"wps\":\"%s\"},", wps);
				}
			} else {
				printf("\"%d\":{", no);
				printf("\"channeld\":\"%d\",", channeld);
				printf("\"ssid\":\"%s\",", ssid);
				printf("\"bssid\":\"%s\",", bssid);
				printf("\"security\":\"%s\",", security);
				printf("\"signald\":\"%d\",", signald);
				printf("\"mode\":\"%s\",", mode);
				printf("\"ext_ch\":\"%s\",", ext_ch);
				printf("\"net_type\":\"%s\",", net_type);
				printf("\"wps\":\"%s\"},", wps);
			}
		}
	}

	if (!strcmp(filter, "WPS"))
		printf("\"%d\":\"NULL\"}", index);
	else
		printf("\"%d\":\"NULL\"}", ++no);
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

		if (!strcmp(filter, "WPS")) {
			if (!strcmp(wps, "YES")) {
				printf("<tr>\n");
				printf("<td>%d</td>\n", index++);
				printf("<td>%s</td><td>%s</td>\n", channel, ssid);
				printf("<td>%s</td><td>%s</td>\n", bssid, security);
				printf("<td>%s</td><td>%s</td>\n", signal, mode);
				printf("<td>%s</td><td>%s</td>\n", ext_ch, net_type);
				printf("<td>%s</td>\n", wps);
				printf("</tr>\n");
			}
		} else {
			printf("<tr>\n");
			printf("<td>%d</td>\n", index++);
			printf("<td>%s</td><td>%s</td>\n", channel, ssid);
			printf("<td>%s</td><td>%s</td>\n", bssid, security);
			printf("<td>%s</td><td>%s</td>\n", signal, mode);
			printf("<td>%s</td><td>%s</td>\n", ext_ch, net_type);
			printf("<td>%s</td>\n", wps);
			printf("</tr>\n");
		}
	}
#endif
	pclose(pp);
}

//Landen: CP functions from wireless for Ajax, do not reload page for DBDC ssid changed.

//TODO: Not yet test.
void apcli_get_wps_reg_info(int nvram, char *input)
{
	int i, status, WscResult = 0;
	char tmp_str[128];
	WSC_CONFIGURED_VALUE result;
	char ifname[IFNAMSIZ];
	int  ssid_index;

	web_debug_header();

	if(NULL == get_apcli_name_by_idx(nvram, input, ifname, sizeof(ifname), &ssid_index)){
		printf("{\"apcli_get_wps_reg_info\":\"GET_IFNAME_NG\"}");
		return;
	}

	getCurrentWscProfile(ifname, &result, sizeof(WSC_CONFIGURED_VALUE));

/*TODO: Error handling
	if (result == NULL) {
		printf("{\"apcli_get_wps_reg_info\":\"NG\"}");
		return;
	}
*/

	printf("{\"apcli_get_wps_reg_info\":\"OK\",");
	//1. WPSConfigured
	printf("\"WscConfigured\":\"%d\",", result.WscConfigured);

	//2. WPSSSID
	if(strchr(result.WscSsid, '\n')){
		printf("Invalid SSID character: new line\t");
	}else{
		printf("\"reg_WscSsid\":\"%s\",", result.WscSsid);
	}

	//3. WPSAuthMode
	tmp_str[0] = '\0';
	getWPSAuthMode(&result, tmp_str);
	printf("\"reg_AuthMode\":\"%s\",", tmp_str);

	//4. EncrypType
	tmp_str[0] = '\0';
	getWPSEncrypType(&result, tmp_str);
	printf("\"reg_EncrypType\":\"%s\",", tmp_str);

	//5. DefaultKeyIdx
	printf("\"reg_DefaultKeyIdx\":\"%d\",", result.DefaultKeyIdx);

	//6. Key
	printf("\"reg_Key1\":\"");
	for(i=0; i<64 && i<strlen(result.WscWPAKey); i++){                // WPA key default length is 64 (defined & hardcode in driver)
		if (i!=0 && !(i % 32))
			printf("\",\"reg_Key2\":\"");

		printf("%c", result.WscWPAKey[i]);
	}
	printf("\"}");

}

void apcli_get_wps_status(int nvram, char *input)
{
	int i, status, WscResult = 0;
	int profile_idx;
	char tmp_str[68] = {0};
	WSC_CONFIGURED_VALUE result;
	char ifname[IFNAMSIZ];
	int  ssid_index;

	web_debug_header();

	if(NULL == get_apcli_name_by_idx(nvram, input, ifname, sizeof(ifname), &ssid_index)){
		printf("{\"apcli_get_wps_status\":\"GET_IFNAME_NG\"}");
		return;
	}

	//7. WSC Status
	status = getWscStatus(ifname);
	//DBG_MSG("%d", status);


	//8. WSC Result
	if (status == 0x2 || status == 0x109) {
		WscResult = -1;
		printf("{\"wps_result\":\"Failed\",");
	} else if (status == 34) {
		WscResult = 1;
		printf("{\"wps_result\":\"Success\",");
	} else {
		printf("{\"wps_result\":\"Continuing\",");
	}

	//9. WSC Status Index
	printf("\"wps_status_code\":\"%d\",", status);
	printf("\"wps_status\":\"%s\",", getWscStatusStr(status));

	// driver will try all Profiles until connect to Root AP successfully,
	// so get the Profile after connection established.
	if (port_secured(ifname) <= 0) {
		printf("\"wps_port_secured\":\"NO\",");
		printf("\"apcli_get_wps_status\":\"OK\"}");
		return;
	}

	printf("\"wps_port_secured\":\"YES\",");

	if (strstr(ifname,"apcli") != NULL) {
		//WSC_CONFIGURED_VALUE wsc_value;
		//getCurrentWscProfile(ifname, wsc_value, sizeof(WSC_CONFIGURED_VALUE));
		//getWscProfile()
		WSC_PROFILE *wsc_profile;

		if ((wsc_profile = malloc(sizeof(WSC_PROFILE))) == NULL) {
			printf("\"apcli_get_wps_status\":\"NG\"}");
			perror("malloc wsc_profile");
			return;
		}

		getWscProfile(ifname, wsc_profile);
		if (wsc_profile != NULL) {
			DBG_MSG("%s get the valid AP Profile.\n");
			printf("\"enr_profile_cnt\":\"%d\",", wsc_profile->ProfileCnt);
			printf("\"enr_profile_idx\":\"%d\",", wsc_profile->ApplyProfileIdx);
			printf("\"enr_SSID\":\"%s\",", wsc_profile->Profile[profile_idx].SSID.Ssid);
			nvram_init(nvram);
			set_nth_value_flash(nvram, ssid_index, "ApCliSsid", wsc_profile->Profile[profile_idx].SSID.Ssid);
			set_nth_value_flash(nvram, ssid_index, "ApCliEnable", "1"); //Is it necessary?

			profile_idx = wsc_profile->ApplyProfileIdx;
			switch (wsc_profile->Profile[profile_idx].AuthType) {
				case 0x0002:
					printf("\"enr_AuthMode\":\"%s\",", "WPAPSK");
					set_nth_value_flash(nvram, ssid_index, "ApCliAuthMode", "WPAPSK");
					break;
				case 0x0004:
					printf("\"enr_AuthMode\":\"%s\",", "SHARED");
					set_nth_value_flash(nvram, ssid_index, "ApCliAuthMode", "SHARED");
					break;
				case 0x0008:
					printf("\"enr_AuthMode\":\"%s\",", "WPA");
					set_nth_value_flash(nvram, ssid_index, "ApCliAuthMode", "WPA");
					break;
				case 0x0010:
					printf("\"enr_AuthMode\":\"%s\",", "WPA2");
					set_nth_value_flash(nvram, ssid_index, "ApCliAuthMode", "WPA2");
					break;
				case 0x0020:
					printf("\"enr_AuthMode\":\"%s\",", "WPA2PSK");
					set_nth_value_flash(nvram, ssid_index, "ApCliAuthMode", "WPA2PSK");
					break;
				case 0x0022:
					printf("\"enr_AuthMode\":\"%s\",", "WPAPSKWPA2PSK");
					set_nth_value_flash(nvram, ssid_index, "ApCliAuthMode", "WPAPSKWPA2PSK");
					break;
				case 0x0001:
				default:
					printf("\"enr_AuthMode\":\"%s\",", "OPEN");
					set_nth_value_flash(nvram, ssid_index, "ApCliAuthMode", "OPEN");
			}
			switch (wsc_profile->Profile[profile_idx].EncrType) {
				case 0x0002:
					printf("\"enr_EncrypType\":\"%s\",", "WEP");
					set_nth_value_flash(nvram, ssid_index, "ApCliEncrypType", "WEP");
					if ((wsc_profile->Profile[profile_idx].KeyLength == 10) ||
							(wsc_profile->Profile[profile_idx].KeyLength == 26)) {
						/* Key Entry Method == HEX */
						printf("\"enr_%s\":\"%s\",", "Key1Type", "0");
						printf("\"enr_%s\":\"%s\",", "Key2Type", "0");
						printf("\"enr_%s\":\"%s\",", "Key3Type", "0");
						printf("\"enr_%s\":\"%s\",", "Key4Type", "0");
						set_nth_value_flash(nvram, ssid_index, "ApCliKey1Type", "0");
						set_nth_value_flash(nvram, ssid_index, "ApCliKey2Type", "0");
						set_nth_value_flash(nvram, ssid_index, "ApCliKey3Type", "0");
						set_nth_value_flash(nvram, ssid_index, "ApCliKey4Type", "0");
					} else {
						/* Key Entry Method == ASCII */
						printf("\"enr_%s\":\"%s\",", "Key1Type", "1");
						printf("\"enr_%s\":\"%s\",", "Key2Type", "1");
						printf("\"enr_%s\":\"%s\",", "Key3Type", "1");
						printf("\"enr_%s\":\"%s\",", "Key4Type", "1");
						set_nth_value_flash(nvram, ssid_index, "ApCliKey1Type", "1");
						set_nth_value_flash(nvram, ssid_index, "ApCliKey2Type", "1");
						set_nth_value_flash(nvram, ssid_index, "ApCliKey3Type", "1");
						set_nth_value_flash(nvram, ssid_index, "ApCliKey4Type", "1");
					}
					if (wsc_profile->Profile[profile_idx].KeyIndex == 1) {
						printf("\"enr_%s\":\"%s\",", "KeyStr", wsc_profile->Profile[profile_idx].Key);
						printf("\"enr_%s\":\"%s\",", "DefaultKeyID", "1");
						set_nth_value_flash(nvram, ssid_index, "ApCliDefaultKeyID", "1");
						if(ssid_index == 0)
							nvram_bufset(nvram, "ApCliKey1Str",
									wsc_profile->Profile[profile_idx].Key);
						else
							nvram_bufset(nvram, racat("ApCliKey1Str", ssid_index),
									wsc_profile->Profile[profile_idx].Key);
					} else if (wsc_profile->Profile[profile_idx].KeyIndex == 2) {
						printf("\"enr_%s\":\"%s\",", "KeyStr", wsc_profile->Profile[profile_idx].Key);
						printf("\"enr_%s\":\"%s\",", "DefaultKeyID", "2");
						set_nth_value_flash(nvram, ssid_index, "ApCliDefaultKeyID", "2");
						if(ssid_index == 0)
							nvram_bufset(nvram, "ApCliKey2Str",
									wsc_profile->Profile[profile_idx].Key);
						else
							nvram_bufset(nvram, racat("ApCliKey2Str", ssid_index),
									wsc_profile->Profile[profile_idx].Key);
					} else if (wsc_profile->Profile[profile_idx].KeyIndex == 3) {
						printf("\"enr_%s\":\"%s\",", "KeyStr", wsc_profile->Profile[profile_idx].Key);
						printf("\"enr_%s\":\"%s\",", "DefaultKeyID", "3");
						set_nth_value_flash(nvram, ssid_index, "ApCliDefaultKeyID", "3");
						if(ssid_index == 0)
							nvram_bufset(nvram, "ApCliKey3Str",
									wsc_profile->Profile[profile_idx].Key);
						else
							nvram_bufset(nvram, racat("ApCliKey3Str", ssid_index),
									wsc_profile->Profile[profile_idx].Key);
					} else if (wsc_profile->Profile[profile_idx].KeyIndex == 4) {
						printf("\"enr_%s\":\"%s\",", "KeyStr", wsc_profile->Profile[profile_idx].Key);
						printf("\"enr_%s\":\"%s\",", "DefaultKeyID", "4");
						set_nth_value_flash(nvram, ssid_index, "ApCliDefaultKeyID", "4");
						if(ssid_index == 0)
							nvram_bufset(nvram, "ApCliKey4Str",
									wsc_profile->Profile[profile_idx].Key);
						else
							nvram_bufset(nvram, racat("ApCliKey4Str", ssid_index),
									wsc_profile->Profile[profile_idx].Key);
					}
					break;
				case 0x0004:
					printf("\"enr_%s\":\"%s\",", "EncrypType", "TKIP");
					printf("\"enr_%s\":\"%s\",", "DefaultKeyID", "2");
					memset(tmp_str, 0, 65);
					memcpy(tmp_str, wsc_profile->Profile[profile_idx].Key, wsc_profile->Profile[profile_idx].KeyLength);
					printf("\"enr_%s\":\"%s\",", "WPAPSK", tmp_str);
					set_nth_value_flash(nvram, ssid_index, "ApCliEncrypType", "TKIP");
					set_nth_value_flash(nvram, ssid_index, "ApCliDefaultKeyID", "2");
					if(ssid_index == 0)
						nvram_bufset(nvram, "ApCliWPAPSK", tmp_str);
					else
						nvram_bufset(nvram, racat("ApCliWPAPSK", ssid_index), tmp_str);
					break;
				case 0x0008:
					printf("\"enr_%s\":\"%s\",", "EncrypType", "AES");
					printf("\"enr_%s\":\"%s\",", "DefaultKeyID", "2");
					memset(tmp_str, 0, 65);
					memcpy(tmp_str, wsc_profile->Profile[profile_idx].Key, wsc_profile->Profile[profile_idx].KeyLength);
					printf("\"enr_%s\":\"%s\",", "WPAPSK", tmp_str);
					set_nth_value_flash(nvram, ssid_index, "ApCliEncrypType", "AES");
					set_nth_value_flash(nvram, ssid_index, "ApCliDefaultKeyID", "2");
					if(ssid_index == 0)
						nvram_bufset(nvram, "ApCliWPAPSK", tmp_str);
					else
						nvram_bufset(nvram, racat("ApCliWPAPSK", ssid_index), tmp_str);
					break;
				case 0x000C:
					printf("\"enr_%s\":\"%s\",", "EncrypType", "TKIPAES");
					printf("\"enr_%s\":\"%s\",", "DefaultKeyID", "2");
					memset(tmp_str, 0, 65);
					memcpy(tmp_str, wsc_profile->Profile[profile_idx].Key, wsc_profile->Profile[profile_idx].KeyLength);
					printf("\"enr_%s\":\"%s\",", "WPAPSK", tmp_str);
					set_nth_value_flash(nvram, ssid_index, "ApCliEncrypType", "TKIPAES");
					set_nth_value_flash(nvram, ssid_index, "ApCliDefaultKeyID", "2");
					if(ssid_index == 0)
						nvram_bufset(nvram, "ApCliWPAPSK", tmp_str);
					else
						nvram_bufset(nvram, racat("ApCliWPAPSK", ssid_index), tmp_str);
					break;
				case 0x0001:
				default:
					printf("\"enr_%s\":\"%s\",", "EncrypType", "NONE");
					printf("\"enr_%s\":\"%s\",", "DefaultKeyID", "1");
					set_nth_value_flash(nvram, ssid_index, "ApCliEncrypType", "NONE");
					set_nth_value_flash(nvram, ssid_index, "ApCliDefaultKeyID", "1");
			}

			if (wsc_profile->Profile[profile_idx].AuthType == 0x0002 &&
					wsc_profile->Profile[profile_idx].EncrType == 0x0004) {
				printf("\"enr_%s\":\"%s\",", "AuthMode", "WPAPSKWPA2PSK");
				printf("\"enr_%s\":\"%s\",", "EncrypType", "TKIPAES");
				set_nth_value_flash(nvram, ssid_index, "ApCliAuthMode", "WPAPSKWPA2PSK");
				set_nth_value_flash(nvram, ssid_index, "ApCliEncrypType", "TKIPAES");
			}
			nvram_commit(nvram);
			nvram_close(nvram);
		}
		printf("\"apcli_get_wps_status\":\"OK\"}");
	}
}

/* input is sent from Web Page makeRequest2.
 * input format: apcli_wps_config&[nvram]&[ssid_idx]&WPSEnable=[0|1]
 * input sample: apcli_wps_config&2860&1&WPSEnable=1 */
void apcli_wps_config(int nvram, char *input)
{
	int  wsc_enable = 0;
	char ifname[IFNAMSIZ];
	int  ssid_index;

	web_debug_header();

	if(NULL == get_apcli_name_by_idx(nvram, input, ifname, sizeof(ifname), &ssid_index)){
		printf("{\"apcli_wps_config\":\"GET_IFNAME_NG\"}");
		return;
	}

	wsc_enable = (int)strtol(web_get("WPSEnable", input, 0), NULL, 0);
/*
	//No nvram setting requirement.
	nvram_init(nvram);
	if (wsc_enable == 0){
		set_nth_value_flash(nvram, ssid_index, "ApcliWscConfMode", "0");
	}else{
		set_nth_value_flash(nvram, ssid_index, "ApcliWscConfMode", "1");
	}
	nvram_commit(nvram);
*/
//	apcli_restart_wps(nvram, wsc_enable, ifname);
	nvram_close(nvram);
	printf("{\"apcli_wps_config\":\"OK\"}");
}

static unsigned int apcli_get_pincode_ioctl(char *interface)
{ 
	int socket_id;
	struct iwreq wrq;
	unsigned int data = 0;
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	strcpy(wrq.ifr_name, interface);
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = (caddr_t) &data;
	wrq.u.data.flags = RT_OID_APCLI_WSC_PIN_CODE;
	if( ioctl(socket_id, RT_PRIV_IOCTL, &wrq) == -1)
		DBG_MSG("ioctl error");
	close(socket_id);

	return data;
}

/* input is sent from Web Page makeRequest2.
 * input format: get_apcli_pincode&[nvram]&[ssid_index]
 * input sample: get_apcli_pincode&2860&1 */
int apcli_wps_get_pincode(int nvram, char *input)
{
	char ifname[IFNAMSIZ] = {0};

	web_debug_header();
	
	if (NULL == get_apcli_name_by_idx(nvram, input, ifname, sizeof(ifname), NULL)) {
		printf("{\"getpincode\":\"GET_IFNAME_NG\"}");
		return 1;
	}

	printf("{\"getpincode\":\"%08d\"}", apcli_get_pincode_ioctl(ifname));

	return 0;
}

void apcli_wps_gen_pincode(int nvram, char *input)
{
	char ifname[IFNAMSIZ] = {0};
	int  ssid_index;
	FILE *pp;
	char cmd[CMDLEN] = {0};
	char pincode[12] = {0};

	web_debug_header();

	if(NULL == get_apcli_name_by_idx(nvram, input, ifname, sizeof(ifname), &ssid_index)){
		printf("{\"apcli_wps_gen_pincode\":\"GET_IFNAME_NG\"}");
		return;
	}

	do_system("iwpriv %s set WscGenPinCode", ifname);
/*TODO: Check dirver why 'show WscPin' is NG, but 'get_site_survye' is OK.
	sprintf(cmd, "iwpriv %s show WscPin", ifname);
	if(!(pp = popen(cmd, "r"))) {
		printf("{\"apcli_wps_gen_pincode\":\"SHOW_WSCPIN_NG\"}");
		DBG_MSG("execute iwpriv apcli# show WscPin fail!");
		return;
	}

	memset(cmd, 0, sizeof(cmd));

	fgets(cmd, sizeof(cmd), pp); //skip 1st line: IF(ifname) WPS Information:
	//2nd line is like : Enrollee PinCode(ApCli0)        01557367
	fgets(cmd, sizeof(cmd), pp);
	DBG_MSG("pincode_raw=[%s]\n", cmd);
 	sscanf(cmd, "%*s %*s(%*s) %s", pincode);
	DBG_MSG("pincode=[%s]\n", pincode);

	pclose(pp);

	if (strlen(pincode) == 8)
		printf("{\"apcli_wps_gen_pincode\":\"%s\"}", pincode);
	else
		printf("{\"apcli_wps_gen_pincode\":\"GET_PINCODE_NG\"}");
*/
	printf("{\"apcli_wps_gen_pincode\":\"OK\"}");

}


/*
 * input is sent from Web Page makeRequest2.
 * input format: apcli_do_enr_pin_wps&[nvram]&[ssid_idx]&SSID
 * input sample: apcli_do_enr_pin_wps&2860&1&root_ap_ssid_5G
 */
void apcli_do_enr_pin_wps(int nvram, char *input)
{
	char ifname[IFNAMSIZ] = {0};
	char raw_ssid[36] = {0}; //max ssid length = 32
	char target_ap_ssid[36] = {0}; //max ssid length = 32
	int  ssid_index;

	web_debug_header();

	if(NULL == get_apcli_name_by_idx(nvram, input, ifname, sizeof(ifname), &ssid_index)){
		printf("{\"apcli_do_enr_pin_wps\":\"GET_IFNAME_NG\"}");
		return;
	}

	get_nth_value(3, input, '&', raw_ssid, sizeof(raw_ssid));
	if ( strlen(raw_ssid) == 0) {
		printf("{\"apcli_do_enr_pin_wps\":\"GET_SSID_NG\"}");
		return;
	}
	unencode(raw_ssid, raw_ssid + strlen(raw_ssid), target_ap_ssid);
	DBG_MSG("target_ap_ssid=[%s]\n", target_ap_ssid);

	do_system("iwpriv %s set ApCliAutoConnect=1", ifname);
	do_system("iwpriv %s set ApCliEnable=1", ifname);
	do_system("iwpriv %s set WscConfMode=0", ifname);
	do_system("iwpriv %s set WscConfMode=1", ifname);
	do_system("iwpriv %s set WscMode=1", ifname);
	do_system("iwpriv %s set ApCliWscSsid=%s", ifname, target_ap_ssid);
	do_system("iwpriv %s set WscGetConf=1", ifname);

	printf("{\"apcli_do_enr_pin_wps\":\"OK\"}");

#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM)
		do_system("killall -SIGXFSZ nvram_daemon");
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM)
		do_system("killall -SIGWINCH nvram_daemon");
#endif
}

/*
 * input is sent from Web Page makeRequest2.
 * input format: apcli_do_enr_pbc_wps&[nvram]&[ssid_idx]
 * input sample: apcli_do_enr_pbc_wps&2860&1
 */
void apcli_do_enr_pbc_wps(int nvram, char *input)
{
	char ifname[IFNAMSIZ] = {0};
	int  ssid_index;

	web_debug_header();

	if(NULL == get_apcli_name_by_idx(nvram, input, ifname, sizeof(ifname), &ssid_index)){
		printf("{\"apcli_do_enr_pbc_wps\":\"GET_IFNAME_NG\"}");
		return;
	}

	do_system("iwpriv %s set ApCliAutoConnect=1", ifname);
	do_system("iwpriv %s set ApCliEnable=1", ifname);
	do_system("iwpriv %s set WscConfMode=0", ifname);
	do_system("iwpriv %s set WscConfMode=1", ifname);
	do_system("iwpriv %s set WscMode=2", ifname);
	do_system("iwpriv %s set WscGetConf=1", ifname);

	printf("{\"apcli_do_enr_pbc_wps\":\"OK\"}");

#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM)
		do_system("killall -SIGXFSZ nvram_daemon");
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM)
		do_system("killall -SIGWINCH nvram_daemon");
#endif
}

/*
 * input is sent from Web Page makeRequest2.
 * input format: apcli_cancel_wps&[nvram]&[ssid_idx]
 * input sample: apcli_cancel_wps&2860&1
 */
void apcli_cancel_wps(int nvram, char *input)
{
	char ifname[IFNAMSIZ] = {0};
	int  ssid_index;

	web_debug_header();

	if(NULL == get_apcli_name_by_idx(nvram, input, ifname, sizeof(ifname), &ssid_index)){
		printf("{\"apcli_cancel_wps\":\"GET_IFNAME_NG\"}");
		return;
	}
	
	do_system("iwpriv %s set WscStop=1", ifname);

	printf("{\"apcli_cancel_wps\":\"OK\"}");

#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM)
		do_system("killall -SIGXFSZ nvram_daemon");
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM)
		do_system("killall -SIGWINCH nvram_daemon");
#endif
}
#endif
#endif

#if defined (RT2860_WAPI_SUPPORT) || defined (RTDEV_WAPI_SUPPORT)
void UpdateWapiCert(void)
{
	nvram_init(WAPI_NVRAM);
	web_debug_header();
	printf("%s\t", nvram_bufget(WAPI_NVRAM, "ASCertFile"));
	printf("%s\t", nvram_bufget(WAPI_NVRAM, "UserCertFile"));
	nvram_close(WAPI_NVRAM);
}
#endif

void StartSniffer(char *channel)
{
	char path[512];
	FILE *pp = popen("cat /proc/mounts | grep mmc", "r");
        if (pp == NULL) 
		return;

	memset(path, 0, 512);
	while (EOF != fscanf(pp, "%*s %512s %*s %*s %*s %*s\n", path)) {
		if (strlen(path) != 0) {
			do_system("ifconfig ra0 down; ifconfig ra0 up");
			do_system("iwconfig ra0 channel %s;iwconfig ra0 mode monitor", channel);
			do_system("tcpdump -i ra0 -s 0 -w %s/wifi_sniffer.pcap &", path);
			break;
		}
	}
}
void StopSniffer(void)
{
	do_system("killall -9 tcpdump");
	do_system("ifconfig ra0 down; ifconfig ra0 up");
}
void APStartSniffer(void)
{
	do_system("chmod 777 /etc_ro/sniffer_on.sh");
	do_system("/etc_ro/sniffer_on.sh");
}
void APStopSniffer(void)
{
	do_system("/etc_ro/sniffer_off.sh");
}

void DeleteAccessPolicyList(char *input)
{
	int nvram;
	int mbssid, aplist_num;
	char str[32], apl[64*20];
	char tok[8];
	const char *tmp;

	get_nth_value(1, input, '&', tok, 8);
	if (!strcmp(tok, "rai0"))
		nvram = RTDEV_NVRAM;
	else
		nvram = RT2860_NVRAM;

	get_nth_value(2, input, '&', tok, 8);
	sscanf(tok, "%d,%d", &mbssid, &aplist_num);
	sprintf(str, "AccessControlList%d", mbssid);
	if(!(tmp = nvram_bufget(nvram, str))) {
		if( tmp == NULL) tmp="";
		DBG_MSG("%s is empty!!!", tmp);
		return;
	}
	strcpy(apl, tmp);

	delete_nth_value(&aplist_num, 1, apl, ';');

	nvram_set(nvram, str, apl);
	sprintf(str, "%d", mbssid);
	nvram_set(nvram, "CurrentSSIDIndx", str);

	return;
}

#define TOKEN_LEN 32
int main(unsigned int argc, char *argv[]) 
{
	char *value, inStr[512], *tmp;
	long inLen;
	char token[TOKEN_LEN];
#if defined (RT2860_WSC_SUPPORT) || defined (RTDEV_WSC_SUPPORT)
	char nvram_id_str[8];
	int  nvram_id;
#endif

	//DBG_MSG("%s", inStr);
	
	memset(inStr, 0, sizeof(inStr));
	if( argc > 1){
		strncpy(inStr, argv[1], sizeof(inStr));
		DBG_MSG("Get cmd from console:%s\n", inStr);
	} else {
		tmp = getenv("CONTENT_LENGTH");
		if(tmp == NULL) tmp = "";
		inLen = strtol(tmp, NULL, 10) + 1;
		if (inLen <= 1) {
			DBG_MSG("get no data!");
			goto leave;
		}
		fgets(inStr, inLen, stdin);
	}
	//inStr = malloc(inLen);
	//if(inStr == NULL) goto leave;
	get_nth_value(0, inStr, '&', token, TOKEN_LEN);
	if (!strcmp(token, "clone")) {
		GetCloneMac();
	} else if (!strcmp(token, "delAccessPolicyList")) {
		DeleteAccessPolicyList(inStr);
	} else if (!strcmp(token, "sniffer_start")) {
		get_nth_value(1, inStr, '&', token, IFNAMSIZ);
		StartSniffer(token);
	} else if (!strcmp(token, "sniffer_stop")) {
		StopSniffer();
#if defined (RT2860_WSC_SUPPORT) || defined (RTDEV_WSC_SUPPORT)
	} else if (!strcmp(token, "updateAPWPSStatus")) {
		get_nth_value(1, inStr, '&', token, IFNAMSIZ);
		UpdateAPWPS(token);
	} else if (!strcmp(token, "WPSConfig")){
		get_nth_value(1, inStr, '&', nvram_id_str, sizeof(nvram_id_str));
		nvram_id = getNvramIndex(nvram_id_str);
		set_wifi_wpsconf(nvram_id, inStr);
	} else if (!strcmp(token, "GetPINCode")){
		get_nth_value(1, inStr, '&', nvram_id_str, sizeof(nvram_id_str));
		nvram_id = getNvramIndex(nvram_id_str);
		get_pin_code(nvram_id, inStr);
	} else if (!strcmp(token, "GenPINCode")){
		get_nth_value(1, inStr, '&', nvram_id_str, sizeof(nvram_id_str));
		nvram_id = getNvramIndex(nvram_id_str);
		set_wifi_gen_pin(nvram_id, inStr);
	} else if (!strcmp(token, "SetPINCode")){
		get_nth_value(1, inStr, '&', nvram_id_str, sizeof(nvram_id_str));
		nvram_id = getNvramIndex(nvram_id_str);
		set_wifi_set_pin(nvram_id, inStr);
	} else if (!strcmp(token, "SubmitOOB")){
		get_nth_value(1, inStr, '&', nvram_id_str, sizeof(nvram_id_str));
		nvram_id = getNvramIndex(nvram_id_str);
		set_wifi_wps_oob(nvram_id, inStr);
	} else if (!strcmp(token, "WPS")){
		get_nth_value(1, inStr, '&', nvram_id_str, sizeof(nvram_id_str));
		nvram_id = getNvramIndex(nvram_id_str);
		set_wifi_do_wps(nvram_id, inStr);
	} else if (!strcmp(token, "wps_cancel")){
		get_nth_value(1, inStr, '&', nvram_id_str, sizeof(nvram_id_str));
		nvram_id = getNvramIndex(nvram_id_str);
		set_wifi_cancel_wps(nvram_id, inStr);
#if defined (RT2860_APCLI_SUPPORT)
	} else if (!strcmp(token, "apcli_scan")){
		get_nth_value(1, inStr, '&', nvram_id_str, sizeof(nvram_id_str));
		nvram_id = getNvramIndex(nvram_id_str);
		apcli_scan(nvram_id, inStr);
	} else if (!strcmp(token, "apcli_get_wps_status")){
		get_nth_value(1, inStr, '&', nvram_id_str, sizeof(nvram_id_str));
		nvram_id = getNvramIndex(nvram_id_str);
		apcli_get_wps_status(nvram_id, inStr);
	} else if (!strcmp(token, "apcli_get_wps_reg_info")){
		get_nth_value(1, inStr, '&', nvram_id_str, sizeof(nvram_id_str));
		nvram_id = getNvramIndex(nvram_id_str);
		apcli_get_wps_reg_info(nvram_id, inStr);
	} else if (!strcmp(token, "apcli_do_enr_pin_wps")){
		get_nth_value(1, inStr, '&', nvram_id_str, sizeof(nvram_id_str));
		nvram_id = getNvramIndex(nvram_id_str);
		apcli_do_enr_pin_wps(nvram_id, inStr);
	} else if (!strcmp(token, "apcli_do_enr_pbc_wps")){
		get_nth_value(1, inStr, '&', nvram_id_str, sizeof(nvram_id_str));
		nvram_id = getNvramIndex(nvram_id_str);
		apcli_do_enr_pbc_wps(nvram_id, inStr);
	} else if (!strcmp(token, "apcli_cancel_wps")){
		get_nth_value(1, inStr, '&', nvram_id_str, sizeof(nvram_id_str));
		nvram_id = getNvramIndex(nvram_id_str);
		apcli_cancel_wps(nvram_id, inStr);
	} else if (!strcmp(token, "apcli_wps_get_pincode")){
		get_nth_value(1, inStr, '&', nvram_id_str, sizeof(nvram_id_str));
		nvram_id = getNvramIndex(nvram_id_str);
		apcli_wps_get_pincode(nvram_id, inStr);
	} else if (!strcmp(token, "apcli_wps_gen_pincode")){
		get_nth_value(1, inStr, '&', nvram_id_str, sizeof(nvram_id_str));
		nvram_id = getNvramIndex(nvram_id_str);
		apcli_wps_gen_pincode(nvram_id, inStr);
#endif
#endif
#if defined (RT2860_WAPI_SUPPORT) || defined (RTDEV_WAPI_SUPPORT)
	} else if (!strcmp(token, "updateWapiCert")) {
		UpdateWapiCert();
#endif
	} else if (!strcmp(token, "ap_sniffer_start")) {		
		APStartSniffer();
	} else if (!strcmp(token, "ap_sniffer_stop")) {
		APStopSniffer();
	} else {
		DBG_MSG("Nothing Runs\n");
	}
	//free(inStr);
leave:
	return 0;
}
