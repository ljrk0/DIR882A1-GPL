/* MODULE NAME: apson_sbapi.c
 *
 * PURPOSE:
 *
 *
 *
 * NOTES:
 *
 *
 * Copyright(C) D-Link
 *
 * Revision History
 *    Date        Who         Description of changes
 *    TODO        TODO        Initial created
 */


/* INCLUDE FILE DECLARATIONS */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/ioctl.h>

#include "apson_sb_vendor_api.h"
#include "nvram.h"
#include 	<signal.h>
#include "notify_rc.h"

static char sch_buf[7][2048];
static char G_secu[][32]=
{
	{"OPEN"},	
	{"SHARED"},	
	{"WPAPSK"},	
	{"WPA2PSK"},	
	{"WPAPSKWPA2PSK"},	
};

static char G_enc[][32]=
{
	{"NONE"},	
	{"WEP"},	
	{"TKIP"},	
	{"TKIPAES"},	
	{"AES"},	
};

void serviceApply(int ret,unsigned short usMod)
{
#ifdef OPENWRT
	/* OPENWRT 生效过程 */
	// TO DO ... 待实现
	
#else
	/* SDK 生效过程 */
	// TO DO ... MTK等方案待实现
	if(ret == RESULT_OK || ret == RESULT_REBOOT)
	{
		// QCA SDK生效
		switch(usMod)
		{
			case MODULE_LAN:
			    notify_rc("restart_lan");
				break;

			case MODULE_DNSMASQ:
			    notify_rc("restart_dnsmasq");
				break;			    
				
			case MODULE_WLAN:
				notify_rc_after_wait("restart_wlan");
				break;
			case MODULE_WLAN_GUEST:
				notify_rc("restart_wlan_guest");
				break;
			case MODULE_WAN:
				 notify_rc("restart_wan");
				 break;
			case MODULE_WAN6:
				 notify_rc("restart_wan6");
				 break;				
			case MODULE_DHCPS:
				;
			case MODULE_PLC:
				;
			case MODULE_MLD:
				;
			case MODULE_IGMP:
                 notify_rc("restart_igmp");
				 break;
			case MODULE_ALG:
				;
			case MODULE_UPNP:
                 notify_rc("restart_upnp");
                 break;
			case MODULE_UPNP_AND_IGMP:
                 notify_rc("restart_upnp_and_igmp");
                 break;
			case MODULE_DOS:
				;
			case MODULE_MAC:
				;
			case MODULE_AUTH:
				kill(1, SIGHUP);
				break;
			case MODULE_DNS:
				;
			case MODULE_QOS:
				notify_rc("restart_qos");
				break;
			case MODULE_IPT:
				;
			case MODULE_URLFILTER:
				;
			case MODULE_FIREWALL:
				notify_rc("restart_firewall");
				break;
			case MODULE_SNTP:
				notify_rc("restart_sntp");
                break;
			case MODULE_UPG:
				;
			case MODULE_AUTOUPG:
				;
			case MODULE_REBOOT:
				;
			case MODULE_ACL:
				;
			case MODULE_LOG:
				kill(1, SIGHUP);
				break;
			case MODULE_DSL_ATM:
                ;
            case MODULE_DSL_PTM:
                ;
            case MODULE_DSL_MODULATION:
                ;
            case MODULE_QUICKVPN:
                notify_rc("restart_quickvpn");
                break;
    	    case MODULE_STATICROUTE_IPV4:
    		    notify_rc("start_staticroute_ipv4");
    		    break;
    	    case MODULE_STATICROUTE_IPV6:
    		    notify_rc("restart_staticroute_ipv6");
    		    break;		
    	    case MODULE_SYSTEMLOG:
    		    notify_rc("restart_SystemLog");
    		    break;		
            case MODULE_SCHEDULE:
                notify_rc("restart_schedule");
                break;
    		case MODULE_FTP:
                notify_rc("restart_ftp");
                break;
    		case MODULE_SAMBA:
    			notify_rc("restart_samba");
    			break;
    		case MODULE_DDNS_IPV4:
                notify_rc("restart_ddns_ipv4");
                break;
            case MODULE_DDNS_IPV6:
                notify_rc("restart_ddns_ipv6");
                break;
    		case MODULE_ADMIN:
                notify_rc("restart_admin");
                break;
            case MODULE_DLNA:
                notify_rc("restart_dlna");
                break;
            case MODULE_EMAIL:
                notify_rc("start_email");
                break;
            case MODULE_MDNS:
                notify_rc("restart_mdns");
                break;
			case MODULE_TR69C:
				notify_rc("restart_tr69c");
#ifdef CONFIG_AP_REPEATER_SUPPORT
			 case MODULE_MODE:
                notify_rc("restart_mode");
                break;		
#endif
                break;
    		default:
    			kill(1, SIGHUP);
		}
	}
#endif
}


char *apson_sb_get_radioid(int idx)
{
	if(0 == idx)
		return RADIO_ID_24G;
	else if(1 == idx)
		return RADIO_ID_24G_GUEST;
	else if(2 == idx)
		return RADIO_ID_5G;
	else if(3 == idx)
		return RADIO_ID_5G_GUEST;
	else
		return RADIO_ID_24G;
}


char *apson_sb_get_ifname(int idx)
{
	if(0 == idx)
		return INTERFACE_2G;
	else if(1 == idx)
		return INTERFACE_2G_GUEST1;
	else if(2 == idx)
		return INTERFACE_5G;
	else if(3 == idx)
		return INTERFACE_5G_GUEST1;
	else
		return INTERFACE_5G;
}

int apson_sb_get_nvramidx(int idx)
{
	if(0 == idx)
		return RT2860_NVRAM;
	else if(1 == idx)
		return RT2860_NVRAM;
	else if(2 == idx)
		return RTDEV_NVRAM;
	else if(3 == idx)
		return RTDEV_NVRAM;
	else
		return RT2860_NVRAM;
}


int apson_sb_get_ifname_by_radioid(char *pRadioID)
{
	if(0 == strcmp(pRadioID,RADIO_ID_24G))
		return INTERFACE_2G;
	else if(0 == strcmp(pRadioID,RADIO_ID_24G_GUEST))
		return INTERFACE_2G_GUEST1;
	else if(0 == strcmp(pRadioID,RADIO_ID_5G))
		return INTERFACE_5G;
	else if(0 == strcmp(pRadioID,RADIO_ID_5G_GUEST))
		return INTERFACE_5G_GUEST1;
	else
		return INTERFACE_2G;

}

int apson_sb_get_nvramidx_by_radioid(char *pRadioID)
{
	if(0 == strcmp(pRadioID,RADIO_ID_24G))
		return RT2860_NVRAM;
	else if(0 == strcmp(pRadioID,RADIO_ID_24G_GUEST))
		return RT2860_NVRAM;
	else if(0 == strcmp(pRadioID,RADIO_ID_5G))
		return RTDEV_NVRAM;
	else if(0 == strcmp(pRadioID,RADIO_ID_5G_GUEST))
		return RTDEV_NVRAM;
	else
		return -1;

}


int apson_sb_get_intfIdx(int idx)
{
	if(0 == idx)
		return 0;
	else if(1 == idx)
		return 1;
	else if(2 == idx)
		return 0;
	else if(3 == idx)
		return 1;
	else
		return 0;

}
		
char *racat(char *s, int i)
{
	static char str[32];
	snprintf(str, 32, "%s%1d", s, i);
	return str;
}

static inline char * strcat_r(const char *s1, const char *s2, char *buf)
{
	strcpy(buf, s1);
	strcat(buf, s2);
	return buf;
}

char *apson_sb_get_schnode(int idx)
{
	if(0 == idx)
		return "wifi24RootScheduleName";
	else if(1 == idx)
		return "wifi24Vap1ScheduleName";
	else if(2 == idx)
		return "wifi58RootScheduleName";
	else if(3 == idx)
		return "wifi58Vap1ScheduleName";
	else
	return "wifi24RootScheduleName";
}

RT_802_11_PHY_MODE apson_sb_get_wlmode(char *pMode,char *pRadioID)
{
		if(0 == strcmp(pMode,"802.11a"))
			return PHY_11A;
		else if(0 == strcmp(pMode,"802.11b"))
			return PHY_11B;
		else if(0 == strcmp(pMode,"802.11g"))
			return PHY_11G;
		else if(0 == strcmp(pMode,"802.11n"))
		{
		    if((0 == strcmp(pRadioID,RADIO_ID_24G))||(0 == strcmp(pRadioID,RADIO_ID_24G_GUEST)))
			    return PHY_11N_2_4G;
			else
				return PHY_11N_5G;
		}
		else if(0 == strcmp(pMode,"802.11bg"))
			return PHY_11BG_MIXED;
		else if(0 == strcmp(pMode,"802.11bgn"))
			return PHY_11BGN_MIXED;
		else if(0 == strcmp(pMode,"802.11gn"))
			return PHY_11GN_MIXED;
		else if(0 == strcmp(pMode,"802.11an"))
			return PHY_11AN_MIXED;
		else if(0 == strcmp(pMode,"802.11ac"))
			return PHY_11VHT_AC;
		else if(0 == strcmp(pMode,"802.11anac"))
			return PHY_11VHT_N_A_MIXED;
		else if(0 == strcmp(pMode,"802.11nac"))
			return PHY_11VHT_N_MIXED;
		else
			return PHY_11VHT_N_ABG_MIXED;
}


int get_nth_value(int index, char *value, char delimit, char *result, int len)
{
	int i=0, result_len=0;
	char *begin, *end;

	if(!value || !result || !len)
		return -1;

	begin = value;
	end = strchr(begin, delimit);

	while(i<index && end){
		begin = end+1;
		end = strchr(begin, delimit);
		i++;
	}

	//no delimit
	if(!end){
		if(i == index){
			end = begin + strlen(begin);
			result_len = (len-1) < (end-begin) ? (len-1) : (end-begin);
		}else
			return -1;
	}else
		result_len = (len-1) < (end-begin)? (len-1) : (end-begin);

	memcpy(result, begin, result_len );
	*(result+ result_len ) = '\0';

	return 0;
}

char *set_nth_value(int index, char *old_values, char *new_value)
{
	int i;
	char *p, *q;
	static char ret[2048];
	char buf[8][256];

	memset(ret, 0, 2048);
	for (i = 0; i < 8; i++)
		memset(buf[i], 0, 256);

	//copy original values
	for ( i = 0, p = old_values, q = strchr(p, ';')  ;
			i < 8 && q != NULL                         ;
			i++, p = q + 1, q = strchr(p, ';')         )
	{
		strncpy(buf[i], p, q - p);
	}
	strcpy(buf[i], p); //the last one

	//replace buf[index] with new_value
	strncpy(buf[index], new_value, 256);

	//calculate maximum index
	index = (i > index)? i : index;

	//concatenate into a single string delimited by semicolons
	strcat(ret, buf[0]);
	for (i = 1; i <= index; i++) {
		strncat(ret, ";", 2);
		strncat(ret, buf[i], 256);
	}

	return ret;
}

void set_nth_value_flash(int nvram, int index, char *flash_key, char *value)
{
	char *result;
	char *tmp = (char *) nvram_bufget(nvram, flash_key);
	if(!tmp)
		tmp = "";
	result = set_nth_value(index, tmp, value);
	nvram_bufset(nvram, flash_key, result);
}

static void clear_nvram_sch_rules(int max_rule_num)
{
    int i;
    char prefix[32] = {0};
    char tmp[64] = {0};

    for(i = 0; i < max_rule_num; i++) {
		memset(prefix, 0x0, sizeof(prefix));
		memset(tmp, 0x0, sizeof(tmp));
		snprintf(prefix, sizeof(prefix), "%s%d_", NVRAM_SCHEDULE_ITEM_PREFIX, i);
		nvram_safe_set(strcat_r(prefix, "name", tmp), "");
		nvram_safe_set(strcat_r(prefix, "rule", tmp), "");
    }
    
    return;
}

char* get_str_by_day_index(int day)
{
    char *day_str = NULL;
    
    switch(day) {
    case SCH_MONDAY:
        day_str = "mon_times";
        break;
        
    case SCH_TUESDAY:
        day_str = "tue_times";
        break;

    case SCH_WEDNESDAY:
        day_str = "wed_times";
        break;

    case SCH_THURSDAY:
        day_str = "thu_times";
        break;

    case SCH_FRIDAY:
        day_str = "fri_times";
        break;

    case SCH_SATURDAY:
        day_str = "sat_times";
        break;

    case SCH_SUNDAY:
        day_str = "sun_times";
        break;

    default:
        break;
    }

    return day_str;
}

void clear_sch_buf(void)
{
    int day;

    for(day = SCH_MONDAY; day <= SCH_SUNDAY; day++) {
        memset(sch_buf[day - 1], 0x0, sizeof(sch_buf[day - 1]));
        strcpy(sch_buf[day - 1], get_str_by_day_index(day));
    }

    return;
}

void fill_sch_buf(int day, char *msg) 
{
    int w;
    
    if(day == SCH_ALL_WEEK) {
        for(w = SCH_MONDAY; w <= SCH_SUNDAY; w++) {
            strcat(sch_buf[w - 1], msg);
        }
		printf("sch_buf fill all week msg:%s\n", msg);
    } else {
        strcat(sch_buf[day - 1], msg);
		printf("sch_buf fill day:%s msg:%s\n", sch_buf[day - 1], msg);
    }
    return;
}


void update_sch_buf_to_sch_st(struct sch_rule_s *p_rule)
{
    int w;
    char *buf = NULL;
    int n = 0;

    buf = p_rule->rule;

    if(p_rule->action == SCH_TIME_ALL_WEEK_NULL || p_rule->action == SCH_TIME_ALL_WEEK_ALL_DAY /*|| p_rule->action == SCH_TIME_ALL_WEEK_NOT_ALL_DAY*/) {
        return;
    }

    for(w = SCH_MONDAY; w <= SCH_SUNDAY; w++) {
        //DBG_MSG("sch_buf-->%s\n", sch_buf[w - 1]);
        n += snprintf(buf + n, sizeof(p_rule->rule) - n, "%s;", sch_buf[w - 1]);
    }

    if(buf[n - 1] == ';') {
        buf[n - 1] = '\0';
    }

	//DBG_MSG("buf:%s\n", buf);

    p_rule->action = SCH_TIME_OTHER; 

    return;
}

void update_sch_st_to_nvram(int max_rule_num, struct sch_rule_s *p_rule)
{
    int i, w, n;
    char prefix[32] = {0};
    char tmp[64] = {0};
    char buf[1024] = {0};

    clear_nvram_sch_rules(max_rule_num);

    for(i = 0; i < max_rule_num; i++) {
        n = 0;
   		memset(prefix, 0x0, sizeof(prefix));
    	memset(tmp, 0x0, sizeof(tmp));
    	memset(buf, 0x0, sizeof(buf));
    	snprintf(prefix, sizeof(prefix), "%s%d_", NVRAM_SCHEDULE_ITEM_PREFIX, i);   
    	nvram_safe_set(strcat_r(prefix, "name", tmp), p_rule[i].name);
    	
        if(p_rule[i].action == SCH_TIME_ALL_WEEK_ALL_DAY) {
    		for(w = SCH_MONDAY; w <= SCH_SUNDAY; w++) {
                n += snprintf(buf + n, sizeof(buf) - n, "%s,00:00-24:00;", get_str_by_day_index(w));
            }
            
            if(buf[strlen(buf) - 1] == ';') {
                buf[strlen(buf) - 1] = '\0';
            }
    		
    		nvram_safe_set(strcat_r(prefix, "rule", tmp), buf);    
        }
        #if 0
        else if(p_rule[i].action == SCH_TIME_ALL_WEEK_NOT_ALL_DAY) {
    		for(w = SCH_MONDAY; w <= SCH_SUNDAY; w++) {
                n += snprintf(buf + n, sizeof(buf) - n, "%s%s;", get_str_by_day_index(w), p_rule[i].rule);
            }
            
            if(buf[strlen(buf) - 1] == ';') {
                buf[strlen(buf) - 1] = '\0';
            }

            DBG_MSG("n-->%d\n", n);
    		
    		nvram_safe_set(strcat_r(prefix, "rule", tmp), buf);            
        } 
        #endif
        else if(p_rule[i].action == SCH_TIME_ALL_WEEK_NULL) {
            nvram_safe_set(strcat_r(prefix, "rule", tmp), "");
        } else if(p_rule[i].action == SCH_TIME_OTHER) {
            nvram_safe_set(strcat_r(prefix, "rule", tmp), p_rule[i].rule);
        }
    }

    return;
}

int apson_get_macaddr(char *pszMac)
{
	FILE *file = NULL;
	char buf[18] = { 0 };
	int ret = 1;
	file = popen("eth_mac r lan", "r");
	if(file != NULL)
	{
		if (fgets(buf, sizeof(buf), file) != NULL) 
		{
			if(6 == sscanf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", &pszMac[0],
				&pszMac[1], &pszMac[2], &pszMac[3], &pszMac[4], &pszMac[5]))
				ret = 0;
		}
		pclose(file);

	}	
	return ret;
	
}

int apson_sb_apply_radio(radio_info_s *pSetting_radio)
{
	char cmd[64]={0};
	char ifname[8]={0};

	strcpy(ifname,apson_sb_get_ifname_by_radioid(pSetting_radio->radioid));

	if(strlen(pSetting_radio->mode) == 0)
		return 1;
	sprintf(cmd, "iwpriv %s set WirelessMode=%d", ifname, apson_sb_get_wlmode(pSetting_radio->mode,pSetting_radio->radioid));
	printf("%s\n", cmd);
	system(cmd);

	if(pSetting_radio->ch_width == CHANNEL_WIDTH_AUTO2040M)
	{
		sprintf(cmd, "iwpriv %s set HtBw=1", ifname);
		printf("%s\n", cmd);
		system(cmd);	

		sprintf(cmd, "iwpriv %s set VhtBw=0", ifname);
		printf("%s\n", cmd);
		system(cmd);	

		sprintf(cmd, "iwpriv %s set HtBssCoex=1", ifname);
		printf("%s\n", cmd);
		system(cmd);	
	}
	else if(pSetting_radio->ch_width == CHANNEL_WIDTH_AUTO204080M)
	{
		sprintf(cmd, "iwpriv %s set HtBw=1", ifname);
		printf("%s\n", cmd);
		system(cmd);	

		sprintf(cmd, "iwpriv %s set VhtBw=1", ifname);
		printf("%s\n", cmd);
		system(cmd);	
	}
	else if(pSetting_radio->ch_width == CHANNEL_WIDTH_AUTO204080160M)
	{
		sprintf(cmd, "iwpriv %s set HtBw=1", ifname);
		printf("%s\n", cmd);
		system(cmd);	

		sprintf(cmd, "iwpriv %s set VhtBw=2", ifname);
		printf("%s\n", cmd);
		system(cmd);	

	}
	else if(pSetting_radio->ch_width == CHANNEL_WIDTH_AUTO20408080M)
	{
		sprintf(cmd, "iwpriv %s set HtBw=1", ifname);
		printf("%s\n", cmd);
		system(cmd);	

		sprintf(cmd, "iwpriv %s set VhtBw=3", ifname);
		printf("%s\n", cmd);
		system(cmd);	

	}
	else if(pSetting_radio->ch_width == CHANNEL_WIDTH_20M)
	{
		sprintf(cmd, "iwpriv %s set HtBw=0", ifname);
		printf("%s\n", cmd);
		system(cmd);	

		sprintf(cmd, "iwpriv %s set VhtBw=0", ifname);
		printf("%s\n", cmd);
		system(cmd);	

	}
	else if(pSetting_radio->ch_width == CHANNEL_WIDTH_40M)
	{
		sprintf(cmd, "iwpriv %s set HtBw=1", ifname);
		printf("%s\n", cmd);
		system(cmd);	

		sprintf(cmd, "iwpriv %s set VhtBw=0", ifname);
		printf("%s\n", cmd);
		system(cmd);	

		sprintf(cmd, "iwpriv %s set HtBssCoex=0", ifname);
		printf("%s\n", cmd);
		system(cmd);	
	}
	else if(pSetting_radio->ch_width == CHANNEL_WIDTH_80M)
	{
		sprintf(cmd, "iwpriv %s set HtBw=1", ifname);
		printf("%s\n", cmd);
		system(cmd);	

		sprintf(cmd, "iwpriv %s set VhtBw=1", ifname);
		printf("%s\n", cmd);
		system(cmd);	
	}
	else if(pSetting_radio->ch_width == CHANNEL_WIDTH_160M)
	{
		sprintf(cmd, "iwpriv %s set HtBw=1", ifname);
		printf("%s\n", cmd);
		system(cmd);	

		sprintf(cmd, "iwpriv %s set VhtBw=2", ifname);
		printf("%s\n", cmd);
		system(cmd);	
	}

	sprintf(cmd, "iwpriv %s set Channel=%d", ifname, pSetting_radio->ch);
	printf("%s\n", cmd);
	system(cmd);

	if((pSetting_radio->band_steer_enable == 1) && (0 == strcmp(pSetting_radio->radioid, RADIO_ID_5G)))
	{
		system("bndstrg -d 0");
		sleep(3);
		
		system("iwpriv rax0 set BndStrgCndPriority='3;0;2;1'");
		system("iwpriv rax0 set BndStrgCndChk=0xffff");
		system("iwpriv ra0 set BndStrgCndPriority='3;0;2;1'");
		system("iwpriv ra0 set BndStrgCndChk=0xffff");
	}

	return 0;
}


int apson_sb_apply_wlan(wlan_info_s *pSetting_wlan, int32_t enable)
{
	char cmd[64]={0};
	char ifname[8]={0};

	printf("apson_sb_apply_wlan new !\n");
	
	strcpy(ifname,apson_sb_get_ifname_by_radioid(pSetting_wlan->radioid));

	if(1 == enable) 
		sprintf(cmd, "ifconfig %s up", ifname);
	else
		sprintf(cmd, "ifconfig %s down", ifname);

	printf("%s \n", cmd);
	system(cmd);


	if(0 == pSetting_wlan->ssid_bcast )
		sprintf(cmd, "iwpriv %s set HideSSID=1", ifname);
	else	
		sprintf(cmd, "iwpriv %s set HideSSID=0", ifname);
	
	printf("%s \n", cmd);
	system(cmd);


	if(1 == pSetting_wlan->secu_enable)
	{
		if((0 == strcmp("WPAORWPA2-PSK", pSetting_wlan->type)) || (0 == strcmp("TKIPORAES", pSetting_wlan->encryption)))
		{
			sprintf(cmd, "iwpriv %s set AuthMode=WPAPSKWPA2PSK", ifname);
			printf("%s \n", cmd);
			system(cmd);

			sprintf(cmd, "iwpriv %s set EncrypType=AES", ifname);
			printf("%s \n", cmd);
			system(cmd);

			sprintf(cmd, "iwpriv %s set WPAPSK=%s", ifname, pSetting_wlan->key);
			printf("%s \n", cmd);
			system(cmd);
		}
	}
	else //only consider open 
	{
		sprintf(cmd, "iwpriv %s set AuthMode=OPEN", ifname);
		printf("%s \n", cmd);
		system(cmd);

		sprintf(cmd, "iwpriv %s set EncrypType=NONE", ifname);
		printf("%s \n", cmd);
		system(cmd);
	}
	
	sprintf(cmd, "iwpriv %s set SSID=%s", ifname, pSetting_wlan->ssid);
	printf("%s \n", cmd);
	system(cmd);

	if(1 == pSetting_wlan->isolate)
	{
		if(0 ==strcmp(nvram_safe_get("wan_wan0_proto"), "Bridge"))
		{
			snprintf(cmd, sizeof(cmd), "echo \"%s\" >/proc/guestzoneisolation/gateway", nvram_safe_get("lan0_gateway"));
		}
		else
		{
			snprintf(cmd, sizeof(cmd), "echo \"%s\" >/proc/guestzoneisolation/gateway", nvram_safe_get("lan0_ipaddr"));
		}
		printf("%s \n", cmd);
		system(cmd);		
	
		snprintf(cmd, sizeof(cmd), "echo \"%s\">/proc/guestzoneisolation/ifaceList", "ra1,rax1,");
		printf("%s \n", cmd);
		system(cmd);		
		
	}
	else
	{
		snprintf(cmd, sizeof(cmd), "echo >/proc/guestzoneisolation/gateway");
		printf("%s \n", cmd);
		system(cmd);
	
		snprintf(cmd, sizeof(cmd), "; echo >/proc/guestzoneisolation/ifaceList");
		printf("%s \n", cmd);
		system(cmd);
	}

	return 0;
}


/* NAMING CONSTANT DECLARATIONS */

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_reboot
 *
 * PURPOSE: reboot this device
 *
 * INPUT:   none
 *
 * OUTPUT:
 *          none
 * RETURN:
 *          int         --  0:  SUCCESS
 *                         -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_reboot()
{
    printf("Reboot!\n");

    // reboot device
    system("gpio l 16 0 4000 0 1 4000");
    system("echo \"Reboot\" > /dev/console");
    system("sleep 3; reboot;");

    return 0;
}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_reset_to_default
 *
 * PURPOSE: reset this device to default setting
 *
 * INPUT:   none
 *
 * OUTPUT:
 *          none
 * RETURN:
 *          int         --  0:  SUCCESS
 *                         -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_reset_to_default()
{
    printf("Reset to default!\n");
    system("killall -USR2 nvram_daemon");
    return 0;
}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_image_check
 *
 * PURPOSE: check newest image version, maybe on internet.
 *
 * INPUT:   pMsg        -- set newest image version in this.
 *
 * OUTPUT:
 *          none
 * RETURN:
 *          int         --  0:  SUCCESS
 *                         -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_image_check(char *pMsg)
{
    printf("Image check!\n");
    return 0;
}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_image_upgrade
 *
 * PURPOSE: upgrade to newest image version, maybe on internet.
 *
 * INPUT:   none
 *
 * OUTPUT:
 *          none
 * RETURN:
 *          int         --  0:  SUCCESS
 *                         -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_image_upgrade()
{
    printf("Firmware upgrade!\n");
    return 0;
}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_set_led
 *
 * PURPOSE: set led to display wireless connection quality
 *
 * INPUT:   status      --  0: loss
 *                          1: unqualified
 *                          2: qualified
 * OUTPUT:
 *          none
 * RETURN:
 *          int         --  0:  SUCCESS
 *                         -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_set_led(int status)
{
    printf("Set LED! Val = %d\n", status);
    return 0;
}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_set_led_provision
 *
 * PURPOSE: turn on or blinking the provision led
 *
 * INPUT:   status      --  0:  unprovision, blink the led
 *                      --  1;  provision, turn on the led
 * OUTPUT:
 *          none
 * RETURN:
 *          int         --  0:  SUCCESS
 *                         -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_set_led_provision(int status)
{
    printf("Set provision LED! Val = %d\n", status);
    return 0;
}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_set_schedule_rule
 *
 * PURPOSE: set schedule rule configuration in device
 *
 * INPUT:   pSetting_data   -- data structure of time schedule rule
 *
 * OUTPUT:
 *          none
 * RETURN:
 *          int             --  0:  SUCCESS
 *                             -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_set_schedule_rule(schedule_setting_s *pSetting_data)
{
	int i, j;
    int sch_rule_count, sch_per_rule_count;
    int max_rule_num;
    char xmlPath[512] = {0};
    char tmp[64] = {0};
    char *sch_rule_name = NULL;
    struct sch_rule_s *p_sch_rule_s = NULL;

    char *p_sch_date = NULL;
    char *p_sch_all_day = NULL;
    char *p_sch_time_fmt = NULL;
    char *p_sch_start_time_hour_val = NULL;
    char *p_sch_start_time_min_val = NULL;
    char *p_sch_end_time_hour_val = NULL;
    char *p_sch_end_time_min_val = NULL;
    int sch_date = 0;
    int w, n;	
	int ret = 0;
    printf("Set schedule rule!\n");
	
	for(i = 0; i < pSetting_data->sche_list_count; i++)
	{
		printf("sche_list[%d] name(%s) sche_info_count(%d) \n", i, pSetting_data->sche_list[i].name, pSetting_data->sche_list[i].sche_info_count);
	
		for(j = 0; j < pSetting_data->sche_list[i].sche_info_count; j++)
		{
			printf("sche_info(%d): D%d_(%d)(%d)_%02d:%02d-%02d:%02d\n", j, pSetting_data->sche_list[i].sche_info[j].date, pSetting_data->sche_list[i].sche_info[j].allday, pSetting_data->sche_list[i].sche_info[j].time_format, pSetting_data->sche_list[i].sche_info[j].start_time.hour, pSetting_data->sche_list[i].sche_info[j].start_time.minute, pSetting_data->sche_list[i].sche_info[j].end_time.hour, pSetting_data->sche_list[i].sche_info[j].end_time.minute);

		}
		
	}

    max_rule_num = nvram_get_int("schedule_max_num");
    p_sch_rule_s = (struct sch_rule_s *)malloc(sizeof(struct sch_rule_s) * max_rule_num);

    if(p_sch_rule_s == NULL) {
		printf("maclloc fail \n");
		return 1;
    }
    
    memset(p_sch_rule_s, 0x0, sizeof(struct sch_rule_s) * max_rule_num);

	for(i = 0; i < pSetting_data->sche_list_count; i++)
	{
		if(!pSetting_data->sche_list[i].name[0]) {
			clear_nvram_sch_rules(10);
			ret = 1;
			goto response;
		}

		strcpy(p_sch_rule_s[i].name, pSetting_data->sche_list[i].name);
		sch_per_rule_count = pSetting_data->sche_list[i].sche_info_count;
		if(sch_per_rule_count == 0) {
			p_sch_rule_s[i].action = SCH_TIME_ALL_WEEK_NULL;
			continue;
		}		 
		clear_sch_buf();
		n = 0;
		for(j = 0; j < sch_per_rule_count; j++) {
			sch_date = pSetting_data->sche_list[i].sche_info[j].date;

			if(!sch_date) {
				p_sch_rule_s[i].action = SCH_TIME_ALL_WEEK_NULL;

				break;
			}

			if(sch_date < 0 || sch_date > SCH_SUNDAY) {
				printf("error date \n");
				ret = 1;
				goto response;
			}

			if(sch_date == 0) {
				printf("all week schedule!\n");

				p_sch_all_day = pSetting_data->sche_list[i].sche_info[j].allday; 

				if(1 == p_sch_all_day) {
					p_sch_rule_s[i].action = SCH_TIME_ALL_WEEK_ALL_DAY;
					break;
				} 
				else 
				{
					printf("time_format:%d %02d:%02d-%02d:%02d\n", 
							pSetting_data->sche_list[i].sche_info[j].time_format, pSetting_data->sche_list[i].sche_info[j].start_time.hour, pSetting_data->sche_list[i].sche_info[j].start_time.minute, pSetting_data->sche_list[i].sche_info[j].end_time.hour, pSetting_data->sche_list[i].sche_info[j].end_time.minute);


					snprintf(tmp, 
							 sizeof(tmp), 
							 ",%02d:%02d-%02d:%02d", 
							 pSetting_data->sche_list[i].sche_info[j].start_time.hour, 
							 pSetting_data->sche_list[i].sche_info[j].start_time.minute, 
							 pSetting_data->sche_list[i].sche_info[j].end_time.hour,
							 pSetting_data->sche_list[i].sche_info[j].end_time.minute
							 );
					fill_sch_buf(SCH_ALL_WEEK, tmp);			  
					continue;
				}
				
			}

			for(w = SCH_MONDAY; w <= SCH_SUNDAY; w++) {
				if(w != sch_date) {
					continue;
				}
					
				if(1 == pSetting_data->sche_list[i].sche_info[j].allday) {
					fill_sch_buf(w, ",00:00-24:00");
				} else {
					printf("time_format:%d %02d:%02d-%02d:%02d\n", 
							pSetting_data->sche_list[i].sche_info[j].time_format, pSetting_data->sche_list[i].sche_info[j].start_time.hour, pSetting_data->sche_list[i].sche_info[j].start_time.minute, pSetting_data->sche_list[i].sche_info[j].end_time.hour, pSetting_data->sche_list[i].sche_info[j].end_time.minute);


					snprintf(tmp, 
							 sizeof(tmp), 
							 ",%02d:%02d-%02d:%02d", 
							 pSetting_data->sche_list[i].sche_info[j].start_time.hour, 
							 pSetting_data->sche_list[i].sche_info[j].start_time.minute, 
							 pSetting_data->sche_list[i].sche_info[j].end_time.hour,
							 pSetting_data->sche_list[i].sche_info[j].end_time.minute
							 );

						fill_sch_buf(w, tmp);
					} 
				}
			}
		
		update_sch_buf_to_sch_st(&p_sch_rule_s[i]);
	}

    update_sch_st_to_nvram(max_rule_num, p_sch_rule_s);
	

	notify_rc("restart_schedule");
response:
	if(p_sch_rule_s) {
		free(p_sch_rule_s);
	}
	
	return ret;


}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_set_managed_device
 *
 * PURPOSE: set managed device configuration in device
 *
 * INPUT:   pSetting_filter -- data structure of filter mac
 *          pSetting_del    -- data structure of delete filter mac if pSetting_del.mac = "", donothing
 * OUTPUT:
 *          none
 * RETURN:
 *          int             --  0:  SUCCESS
 *                             -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_set_managed_device(mac_filter_s *pSetting_filter, mac_del_filter_s *pSetting_del)
{
    printf("Set managed device! but the device do not suppport this function\n");
    return 0;
}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_set_wps
 *
 * PURPOSE: set wps configuration in device
 *
 * INPUT:   pSetting_data   -- data structure of wps
 *
 * OUTPUT:
 *          none
 * RETURN:
 *          int             --  0:  SUCCESS
 *                             -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_set_wps(wps_s *pSetting_data)
{
    printf("Set wps!\n");
	if(1 == pSetting_data->wpspbc)
	{
		nvram_bufset(RT2860_NVRAM, "WscCusPBCEnable", "1");
		nvram_bufset(RTDEV_NVRAM, "WscCusPBCEnable", "1");
	}
	else
	{
		nvram_bufset(RT2860_NVRAM, "WscCusPBCEnable", "0");
		nvram_bufset(RTDEV_NVRAM, "WscCusPBCEnable", "0");
	}

	if(1 == pSetting_data->wpspin)
	{
		nvram_bufset(RT2860_NVRAM, "WscCusPINEnable", "1");
		nvram_bufset(RTDEV_NVRAM, "WscCusPINEnable", "1");
		nvram_bufset(RT2860_NVRAM, "WscSetupLock", "0");
		nvram_bufset(RTDEV_NVRAM, "WscSetupLock", "0");
	}
	else
	{
		nvram_bufset(RT2860_NVRAM, "WscCusPINEnable", "0");
		nvram_bufset(RTDEV_NVRAM, "WscCusPINEnable", "0");
		nvram_bufset(RT2860_NVRAM, "WscSetupLock", "1");
		nvram_bufset(RTDEV_NVRAM, "WscSetupLock", "1");

	}

	nvram_commit(RT2860_NVRAM);//RT2860_NVRAM && RTDEV_NVRAM
	nvram_commit(RTDEV_NVRAM);//RT2860_NVRAM && RTDEV_NVRAM

    return 0;
}

char* tz_num_to_time(char *tznum)
{
    char *time_zong = NULL;
    switch(atoi(tznum)){
        case tz_International_Date_Line_West:
            time_zong="-12:00";
        break;
        case tz_Midway:
            time_zong="-11:00";
        break;
        case tz_Hawaii:
            time_zong="-10:00";
        break;
        case tz_Alaska:
            time_zong="-09:00";
        break;
        case tz_Pacific:
            time_zong="-08:00";
        break;
        case tz_Arizona:
        case tz_Chihuahua:
        case tz_Mountain_Canada:
            time_zong="-07:00";
        break;
        case tz_Central_America:
        case tz_Central:
        case tz_Guadalajara:
        case tz_Saskatchewan:
            time_zong="-06:00";
        break;
        case tz_Bogota:
        case tz_Eastern:
            time_zong="-05:00";
        break;
        case tz_Caracas:
            time_zong="-04:30";
        break;
        case tz_Georgetown:
        case tz_Atlantic:
        case tz_Santiago:
            time_zong="-04:00";
        break;
        case tz_Newfoundland:
            time_zong="-03:30";  
        break;
        case tz_Brasilia:
        case tz_Buenos:
        case tz_Greenland:
            time_zong="-03:00";
        break;
        case tz_Mid_Atlantic:
            time_zong="-02:00";
        break;
        case tz_Azores:
        case tz_Cape:
            time_zong="-01:00";
        break;
        default:
        case tz_Casablanca:
        case tz_Greenwich:
            time_zong="+00:00";
        break;
        case tz_Amsterdam:
        case tz_Belgrade:
        case tz_Brussels:
        case tz_Sarajevo:
        case tz_West:
            time_zong="+01:00";
        break;
        case tz_Athens:
        case tz_Bucharest:
        case tz_Cairo:
        case tz_Harare:
        case tz_Helsinki: 
        case tz_Jerusalem:
            time_zong="+02:00";
        break;
        case tz_Baghdad:
        case tz_Kuwait:
        case tz_Nairobi:
            time_zong="+03:00";
        break;
        case tz_Tehran:
            time_zong="+03:30";
        break;
        case tz_Abu:
        case tz_Baku:
        case tz_Moscow:
            time_zong="+04:00";
        break;
        case tz_Kabul:
            time_zong="+04:30";
        break;
        case tz_Islamabad:
            time_zong="+05:00";
        break;
        case tz_Chennai:
        case tz_Sri:
            time_zong="+05:30";
        break;
        case tz_Kathmandu:
            time_zong="+05:45";
        break;
        case tz_Astana:
        case tz_Ekaterinburg:
            time_zong="+06:00";
        break;
        case tz_Rangoon:
            time_zong="+06:30";
        break;
        case tz_Almaty:
        case tz_Bangkok:
            time_zong="+07:00";
        break;
        case tz_Krasnoyarsk:
        case tz_Beijing:
        case tz_Kuala:
        case tz_Perth:
        case tz_Taipei:
        case tz_Ulaan:
            time_zong="+08:00";
        break;
        case tz_Irkutsk:
        case tz_Osaka:
        case tz_Seoul:
            time_zong="+09:00";
        break;
        case tz_Adelaide:
        case tz_Darwin:
            time_zong="+09:30";
        break;
        case tz_Brisbane:
        case tz_Canberra:
        case tz_Guam:
        case tz_Hobart:
        case tz_Yakutsk:
            time_zong="+10:00";
        break;
        case tz_Solomon:
        case tz_Vladivostok:
            time_zong="+11:00";
        break;
        case tz_Magadan:
        case tz_Auckland:
        case tz_Fiji:
            time_zong="+12:00";
        break;
        case tz_Nuku:
        case tz_Samoa:
            time_zong="+13:00";
        break;
    }

    //nvram_safe_set("ntp_timezone", time_zong);
    return time_zong ? time_zong : "";
}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_set_ntp
 *
 * PURPOSE: set ntp configuration in device
 *
 * INPUT:   pSetting_data   -- data structure of ntp
 *
 * OUTPUT:
 *          none
 * RETURN:
 *          int             --  0:  SUCCESS
 *                             -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */

int apson_sb_set_ntp(time_setting_s *pSetting_data)
{
    char *pszTZOffset = NULL;
    int Hours = 0, Minutes = 0;
    char pCurrTime[21] = {0};
    int iyear = 0;
    int iMon = 0;
    int iDay = 0;
    int iHours = 0;
    int iMinutes = 0;

    printf("Set ntp!\n");

	if(1 == pSetting_data->ntp) //ntp enable
	{
        struct timeval tv;
        char *pTimeZoneStr = NULL;
        time_t currTime = time(NULL);
        pszTZOffset = nvram_safe_get("ntp_timezone");
        // shift time by token
    	if(strchr(pszTZOffset, '-')) {
    		sscanf(pszTZOffset, "-%d:%d", &Hours, &Minutes);
            currTime += (Hours * 60 + Minutes) * 60;
    	} else {
    		sscanf(pszTZOffset, "+%d:%d", &Hours, &Minutes);
    		currTime -= (Hours * 60 + Minutes) * 60;
    	}

    	if(nvram_get_int("ntp_DaylightSavingsUsed")) 
		{
            currTime -= 3600;
	    }

    	pTimeZoneStr = tz_num_to_time(pSetting_data->tz);

    	if(strchr(pTimeZoneStr, '-')) {
    		sscanf(pTimeZoneStr, "-%d:%d", &Hours, &Minutes);
            currTime -= (Hours * 60 + Minutes) * 60;
    	} else {
    		sscanf(pTimeZoneStr, "+%d:%d", &Hours, &Minutes);
    		currTime += (Hours * 60 + Minutes) * 60;
    	}    	

        if(1 == pSetting_data->daylight_saving) //daylightSaveing
		{
	        currTime += 3600;
        }

        tv.tv_sec = currTime;
        tv.tv_usec = 0;

        if(settimeofday(&tv, NULL) < 0) {
            printf("settimeofday fail\n");
			return -1; 
        }
       
	}
	else
	{
		struct tm stTm;
		struct tm *pstTm;
		time_t tmpTime = 0;
		
		sscanf(pSetting_data->cur_date, "%4d/%2d/%2d",&iyear,&iMon,&iDay);
		sscanf(pSetting_data->cur_time, "%2d:%2d:%*d",&iHours,&iMinutes);
		
		stTm.tm_year= iyear - 1900;
		stTm.tm_mon= iMon - 1;
		stTm.tm_mday= iDay;
		stTm.tm_hour= iHours;
		stTm.tm_min= iMinutes;
		stTm.tm_sec= 0;
		
		tmpTime = mktime(&stTm);
		
		// check timezone token*3600
		//pszTZOffset = nvram_safe_get("ntp_timezone");
		pszTZOffset = tz_num_to_time(pSetting_data->tz);
		// shift time by token
		if (strchr(pszTZOffset, '-'))
		{
			sscanf(pszTZOffset, "-%d:%d", &Hours, &Minutes);
			tmpTime -= (Hours * 60 + Minutes) * 60;
		}
		else
		{
			sscanf(pszTZOffset, "+%d:%d", &Hours, &Minutes);
			tmpTime += (Hours * 60 + Minutes) * 60;
		}

        if(1 == pSetting_data->daylight_saving) //daylightSaveing
		{
	        tmpTime += 3600;
        }
				
		pstTm =localtime(&tmpTime);
		sprintf(pCurrTime, "%04d-%02d-%02dT%02d:%02d:%02d",pstTm->tm_year + 1900, pstTm->tm_mon + 1, pstTm->tm_mday, pstTm->tm_hour, pstTm->tm_min, 0);
		nvram_safe_set("ntp_localtime", pCurrTime);

	}

	if(1 == pSetting_data->ntp) //ntp enable
	{
        nvram_safe_set("ntpEnabled", "1");
        nvram_safe_set("ntp_server", pSetting_data->ntp_server);
    } else {
        nvram_safe_set("ntpEnabled", "0");
        nvram_safe_set("ntp_server", "");
    }

    nvram_safe_set("ntp_timezone_num", pSetting_data->tz);
    nvram_safe_set("ntp_timezone", tz_num_to_time(pSetting_data->tz));
    
    nvram_safe_set("ntp_DaylightSavingsUsed", pSetting_data->daylight_saving? "1" : "0");

    nvram_commit(RT2860_NVRAM);

    //serviceApply(ret, MODULE_SNTP);
	notify_rc("restart_sntp");

    return 0;
}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_set_administration
 *
 * PURPOSE: set administration configuration in device
 *
 * INPUT:   pSetting_data   -- data structure of administration
 *
 * OUTPUT:
 *          none
 * RETURN:
 *          int             --  0:  SUCCESS
 *                             -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_set_administration(dev_setting_s *pSetting_data)
{
    printf("Set administration!\n");

	nvram_safe_set("PresentURL", pSetting_data->presentation_url);

	if(1 == pSetting_data->change_pwd)
	{
		nvram_safe_set("http_passwd", pSetting_data->admin_pwd);
	}
	
	if (1 == pSetting_data->captcha)
		nvram_safe_set("CAPTCHA", "1");
	else
		nvram_safe_set("CAPTCHA", "0");

	nvram_commit(RT2860_NVRAM);
	//serviceApply(ret, MODULE_ADMIN);
	notify_rc("restart_admin");

    return 0;
}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_set_service_wlan
 *
 * PURPOSE: set service wlan configuration in device
 *
 * INPUT:   pSetting_wlan   -- data structure of service wlan
 *          pSetting_radio  -- data structure of service wlan radio
 * OUTPUT:
 *          none
 * RETURN:
 *          int             --  0:  SUCCESS
 *                             -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_set_service_wlan(wlan_info_s *pSetting_wlan, radio_info_s *pSetting_radio)
{
	int iNVRAM_Idx = RT2860_NVRAM;
	int scheduleChanged = 0;
	char szValue[65] = {0};
	const char * pScheduleNameOld=NULL;
	RT_802_11_PHY_MODE enphMode = PHY_11VHT_N_ABG_MIXED;


    printf("Set service wlan !\n");
	
	printf("service wlan_info_s: device(%s) ifname(%s) ssid(%s) encryption(%s) key(%s) sche_name(%s) secu_enable(%d) enable(%d) isolate(%d) radioid(%s)  ssid_bcast(%d) qos(%d) type(%s)\n", 
									      pSetting_wlan->device,
								  		  pSetting_wlan->ifname,
	                                      pSetting_wlan->ssid,
	                                      pSetting_wlan->encryption,
	                                      pSetting_wlan->key,
										pSetting_wlan->sche_name,
								 		  pSetting_wlan->secu_enable,
										  pSetting_wlan->enable,
										  pSetting_wlan->isolate,
  	  									  pSetting_wlan->radioid,
											pSetting_wlan->ssid_bcast,
										pSetting_wlan->qos,
										pSetting_wlan->type);

	printf("service radio_info_s: radioid(%s) enable(%d) mode(%s) ch_width(%d) ch(%d) second_ch(%d) tx_power(%d) coexistence(%d) band_steer_enable(%d)\n", 
									  pSetting_radio->radioid,
									  pSetting_radio->enable,
									  pSetting_radio->mode,
									  pSetting_radio->ch_width,
									  pSetting_radio->ch,
									pSetting_radio->second_ch,
									  pSetting_radio->tx_power,
									  pSetting_radio->coexistence,
									  pSetting_radio->band_steer_enable);


	iNVRAM_Idx = apson_sb_get_nvramidx_by_radioid(pSetting_wlan->radioid);
	if(iNVRAM_Idx < 0)
	{
		printf("radioid should not be empty!");
		return 1;	
	}

	//radio
	if(1 == pSetting_radio->enable)
	{
		if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_24G))
			set_nth_value_flash(iNVRAM_Idx, 0, "Radio24On", "1");
		else if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_24G_GUEST))
			set_nth_value_flash(iNVRAM_Idx, 0, "Radio24Vap1En", "1");
		else if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_5G))
			set_nth_value_flash(iNVRAM_Idx, 0, "Radio58On", "1");
		else if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_5G_GUEST))
			set_nth_value_flash(iNVRAM_Idx, 0, "Radio58Vap1En", "1");
	}
	else
	{
		if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_24G))
			set_nth_value_flash(iNVRAM_Idx, 0, "Radio24On", "0");
		else if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_24G_GUEST))
			set_nth_value_flash(iNVRAM_Idx, 0, "Radio24Vap1En", "0");
		else if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_5G))
			set_nth_value_flash(iNVRAM_Idx, 0, "Radio58On", "0");
		else if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_5G_GUEST))
			set_nth_value_flash(iNVRAM_Idx, 0, "Radio58Vap1En", "0");
	}

	snprintf(szValue,64, "%d", apson_sb_get_wlmode(pSetting_radio->mode,pSetting_radio->radioid));
	set_nth_value_flash(iNVRAM_Idx,0, "WirelessMode", szValue);


	snprintf(szValue,64,"%d", pSetting_radio->ch);
	set_nth_value_flash(iNVRAM_Idx, 0, "Channel",  szValue);
	set_nth_value_flash(iNVRAM_Idx, 0, "CusChannel",  szValue);

	if(pSetting_radio->ch > 0 && pSetting_radio->ch < 7)
	{
		set_nth_value_flash(iNVRAM_Idx, 0, "HT_EXTCHA", "1");
	}
	else if(pSetting_radio->ch >6 && pSetting_radio->ch < 15)
	{
		set_nth_value_flash(iNVRAM_Idx, 0, "HT_EXTCHA", "0");
	}

	snprintf(szValue,64,"%d", pSetting_radio->tx_power);
	set_nth_value_flash(iNVRAM_Idx, 0, "TxPower", szValue);

	if(100 == pSetting_radio->tx_power)
	{
		//set_nth_value_flash(iNVRAM_Idx, 0, "SKUenable", "0");
		set_nth_value_flash(iNVRAM_Idx, 0, "PERCENTAGEenable", "0");
	}
	else
	{
		set_nth_value_flash(iNVRAM_Idx, 0, "PERCENTAGEenable", "1");
		//set_nth_value_flash(iNVRAM_Idx, 0, "SKUenable", "1");
	}


	if(pSetting_radio->ch_width == CHANNEL_WIDTH_AUTO2040M)
	{
		set_nth_value_flash(iNVRAM_Idx, 0, "HT_BW", "1");
		set_nth_value_flash(iNVRAM_Idx, 0, "VHT_BW", "0");
		set_nth_value_flash(iNVRAM_Idx, 0, "HT_BSSCoexistence", "1");
	}
	else if(pSetting_radio->ch_width == CHANNEL_WIDTH_AUTO204080M)
	{
		set_nth_value_flash(iNVRAM_Idx, 0, "VHT_BW", "1");
	}
	else if(pSetting_radio->ch_width == CHANNEL_WIDTH_AUTO204080160M)
	{
		set_nth_value_flash(iNVRAM_Idx, 0, "VHT_BW", "2");
	}
	else if(pSetting_radio->ch_width == CHANNEL_WIDTH_AUTO20408080M)
	{
		set_nth_value_flash(iNVRAM_Idx, 0, "VHT_BW", "3");
	}
	else if(pSetting_radio->ch_width == CHANNEL_WIDTH_20M)
	{
		set_nth_value_flash(iNVRAM_Idx, 0, "HT_BW", "0");
		set_nth_value_flash(iNVRAM_Idx, 0, "VHT_BW", "0");
		set_nth_value_flash(iNVRAM_Idx, 0, "HT_BSSCoexistence", "0");
	}
	else if(pSetting_radio->ch_width == CHANNEL_WIDTH_40M)
	{
		set_nth_value_flash(iNVRAM_Idx, 0, "HT_BW", "1");
		set_nth_value_flash(iNVRAM_Idx, 0, "VHT_BW", "0");
		set_nth_value_flash(iNVRAM_Idx, 0, "HT_BSSCoexistence", "0");
	}
	else if(pSetting_radio->ch_width == CHANNEL_WIDTH_80M)
	{
		set_nth_value_flash(iNVRAM_Idx, 0, "VHT_BW", "1");
	}
	else if(pSetting_radio->ch_width == CHANNEL_WIDTH_160M)
	{
		set_nth_value_flash(iNVRAM_Idx, 0, "VHT_BW", "2");
	}

	if(1 == pSetting_radio->band_steer_enable)
		set_nth_value_flash(iNVRAM_Idx, 0, "BandSteering", "1");
	else
		set_nth_value_flash(iNVRAM_Idx, 0, "BandSteering", "0");

	if(1 == pSetting_wlan->ssid_bcast)
	{
		if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_24G))
			nvram_bufset(RT2860_NVRAM,"HideSSID","0");
		else if (0 == strcmp(pSetting_wlan->radioid,RADIO_ID_5G))
			nvram_bufset(RTDEV_NVRAM,"HideSSID_5","0");
		else if((0 == strcmp(pSetting_wlan->radioid,RADIO_ID_24G_GUEST)) || (0 == strcmp(pSetting_wlan->radioid,RADIO_ID_5G_GUEST)))
			set_nth_value_flash(iNVRAM_Idx,1,"HideSSID","0");
	}
	else
	{
		if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_24G))
			nvram_bufset(RT2860_NVRAM,"HideSSID","1");
		else if (0 == strcmp(pSetting_wlan->radioid,RADIO_ID_5G))
			nvram_bufset(RTDEV_NVRAM,"HideSSID_5","1");
		else if((0 == strcmp(pSetting_wlan->radioid,RADIO_ID_24G_GUEST)) || (0 == strcmp(pSetting_wlan->radioid,RADIO_ID_5G_GUEST)))
			set_nth_value_flash(iNVRAM_Idx,1,"HideSSID","1");
	}

	nvram_bufset(iNVRAM_Idx, "SSID1", pSetting_wlan->ssid);
	if(1 == pSetting_wlan->secu_enable)
	{
		if((0 == strcmp("WPAORWPA2-PSK", pSetting_wlan->type)) || (0 == strcmp("TKIPORAES", pSetting_wlan->encryption)))
		{
			set_nth_value_flash(iNVRAM_Idx, 0, "AuthMode", "WPAPSKWPA2PSK");
			set_nth_value_flash(iNVRAM_Idx, 0, "EncrypType", "AES");
			set_nth_value_flash(iNVRAM_Idx, 0, "IEEE8021X", "0");
			nvram_bufset(iNVRAM_Idx, "WPAPSK1", pSetting_wlan->key);
		}
	}
	else //only consider open 
	{
		set_nth_value_flash(iNVRAM_Idx, 0, "AuthMode", "OPEN");
		set_nth_value_flash(iNVRAM_Idx, 0, "EncrypType", "NONE");
		set_nth_value_flash(iNVRAM_Idx, 0, "IEEE8021X", "0");
	}

	// 读取旧的无线调度列表名称
	if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_24G))
	{
		pScheduleNameOld = nvram_get(RT2860_NVRAM,"wifi24RootScheduleName"); // for rc
		pScheduleNameOld = (0 == strcmp(pScheduleNameOld,"")) ? "Always" : pScheduleNameOld;
		scheduleChanged = (scheduleChanged == 1) ? 1 : ((0 != strcmp(pScheduleNameOld, pSetting_wlan->sche_name)) ? 1 : 0);
	}
	else if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_5G))
	{
		pScheduleNameOld = nvram_get(RTDEV_NVRAM,"wifi58RootScheduleName"); // for rc
		pScheduleNameOld = (0 == strcmp(pScheduleNameOld,"")) ? "Always" : pScheduleNameOld;
		scheduleChanged = (scheduleChanged == 1) ? 1 :((0 != strcmp(pScheduleNameOld, pSetting_wlan->sche_name)) ? 1 : 0);
	}	

	snprintf(szValue,64,"%s",pSetting_wlan->sche_name);

	if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_24G))
		set_nth_value_flash(iNVRAM_Idx, 0, "wifi24RootScheduleName", szValue);
	else if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_24G_GUEST))
		set_nth_value_flash(iNVRAM_Idx, 0, "wifi24Vap1ScheduleName", szValue);
	else if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_5G))
		nvram_safe_set("wifi58RootScheduleName",szValue); // for rc
	else if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_5G_GUEST))
		nvram_safe_set("wifi58Vap1ScheduleName",szValue); // for rc
	// copy 2.4G Schedule to 5G if SmartConnect is enabled


	//apson_sb_apply_radio(pSetting_radio);
	//apson_sb_apply_wlan(pSetting_wlan, pSetting_radio->enable);

	notify_rc("restart_mdns");
	notify_rc("restart_wlan");
	if (scheduleChanged){// 生效无线调度
		printf("*****scheduleChanged ***** \n");
		FILE *pFile;
		pFile = fopen( "/tmp/schedulereload","w" );
		fclose(pFile);
		}

	//serviceApply(ret, MODULE_WLAN);
		
   return 0;
}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_set_guest_wlan
 *
 * PURPOSE: set guest wlan configuration in device
 *
 * INPUT:   pSetting_wlan   -- data structure of guest wlan
 *          pSetting_radio  -- data structure of guest wlan radio
 * OUTPUT:
 *          none
 * RETURN:
 *          int             --  0:  SUCCESS
 *                             -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_set_guest_wlan(wlan_info_s *pSetting_wlan, radio_info_s *pSetting_radio)
{
	int iNVRAM_Idx = RT2860_NVRAM;
    printf("Set guest wlan!\n");


	printf("guest wlan_info_s: device(%s) ifname(%s) ssid(%s) encryption(%s) key(%s) sche_name(%s) secu_enable(%d) enable(%d) isolate(%d) radioid(%s)  ssid_bcast(%d) qos(%d) type(%s)\n", 
									      pSetting_wlan->device,
								  		  pSetting_wlan->ifname,
	                                      pSetting_wlan->ssid,
	                                      pSetting_wlan->encryption,
	                                      pSetting_wlan->key,
										pSetting_wlan->sche_name,
								 		  pSetting_wlan->secu_enable,
										  pSetting_wlan->enable,
										  pSetting_wlan->isolate,
  	  									  pSetting_wlan->radioid,
											pSetting_wlan->ssid_bcast,
										pSetting_wlan->qos,
										pSetting_wlan->type);

	printf("guest radio_info_s: radioid(%s) enable(%d) mode(%s) ch_width(%d) ch(%d) second_ch(%d) tx_power(%d) coexistence(%d) band_steer_enable(%d)\n", 
									  pSetting_radio->radioid,
									  pSetting_radio->enable,
									  pSetting_radio->mode,
									  pSetting_radio->ch_width,
									  pSetting_radio->ch,
									pSetting_radio->second_ch,
									  pSetting_radio->tx_power,
									  pSetting_radio->coexistence,
									  pSetting_radio->band_steer_enable);


	iNVRAM_Idx = apson_sb_get_nvramidx_by_radioid(pSetting_wlan->radioid);

	if(iNVRAM_Idx < 0)
	{
		printf("radioid should not be empty!");
		return 1;	
	}

	if(1 == pSetting_radio->enable)
	{
		if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_24G))
			set_nth_value_flash(iNVRAM_Idx, 0, "Radio24On", "1");
		else if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_24G_GUEST))
			set_nth_value_flash(iNVRAM_Idx, 0, "Radio24Vap1En", "1");
		else if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_5G))
			set_nth_value_flash(iNVRAM_Idx, 0, "Radio58On", "1");
		else if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_5G_GUEST))
			set_nth_value_flash(iNVRAM_Idx, 0, "Radio58Vap1En", "1");
	}
	else
	{
		if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_24G))
			set_nth_value_flash(iNVRAM_Idx, 0, "Radio24On", "0");
		else if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_24G_GUEST))
			set_nth_value_flash(iNVRAM_Idx, 0, "Radio24Vap1En", "0");
		else if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_5G))
			set_nth_value_flash(iNVRAM_Idx, 0, "Radio58On", "0");
		else if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_5G_GUEST))
			set_nth_value_flash(iNVRAM_Idx, 0, "Radio58Vap1En", "0");
	}

	if(1 == pSetting_wlan->ssid_bcast)
	{
		if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_24G))
			nvram_bufset(RT2860_NVRAM,"HideSSID","0");
		else if (0 == strcmp(pSetting_wlan->radioid,RADIO_ID_5G))
			nvram_bufset(RTDEV_NVRAM,"HideSSID_5","0");
		else if((0 == strcmp(pSetting_wlan->radioid,RADIO_ID_24G_GUEST)) || (0 == strcmp(pSetting_wlan->radioid,RADIO_ID_5G_GUEST)))
			set_nth_value_flash(iNVRAM_Idx,1,"HideSSID","0");
	}
	else
	{
		if(0 == strcmp(pSetting_wlan->radioid,RADIO_ID_24G))
			nvram_bufset(RT2860_NVRAM,"HideSSID","1");
		else if (0 == strcmp(pSetting_wlan->radioid,RADIO_ID_5G))
			nvram_bufset(RTDEV_NVRAM,"HideSSID_5","1");
		else if((0 == strcmp(pSetting_wlan->radioid,RADIO_ID_24G_GUEST)) || (0 == strcmp(pSetting_wlan->radioid,RADIO_ID_5G_GUEST)))
			set_nth_value_flash(iNVRAM_Idx,1,"HideSSID","1");
	}

	nvram_bufset(iNVRAM_Idx, "SSID2", pSetting_wlan->ssid);
	if(1 == pSetting_wlan->secu_enable)
	{
		if((0 == strcmp("WPAORWPA2-PSK", pSetting_wlan->type)) || (0 == strcmp("TKIPORAES", pSetting_wlan->encryption)))
		{
			set_nth_value_flash(iNVRAM_Idx, 1, "AuthMode", "WPAPSKWPA2PSK");
			set_nth_value_flash(iNVRAM_Idx, 1, "EncrypType", "AES");
			set_nth_value_flash(iNVRAM_Idx, 1, "IEEE8021X", "0");
			nvram_bufset(iNVRAM_Idx, "WPAPSK2", pSetting_wlan->key);
		}
	}
	else //only consider open 
	{
		set_nth_value_flash(iNVRAM_Idx, 1, "AuthMode", "OPEN");
		set_nth_value_flash(iNVRAM_Idx, 1, "EncrypType", "NONE");
		set_nth_value_flash(iNVRAM_Idx, 1, "IEEE8021X", "0");
	}

	nvram_safe_set("guest_login_blocked", pSetting_wlan->isolate == 1? "1" : "0");
	
	//apson_sb_apply_wlan(pSetting_wlan, pSetting_radio->enable);
	notify_rc("restart_wlan_guest");

	//serviceApply(ret, MODULE_WLAN_GUEST);

    return 0;
}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_get_schedule_rule
 *
 * PURPOSE: get time schedule rule configuration in device
 *
 * INPUT:   none
 *
 * OUTPUT:
 *          pDevice_data    -- data structure of time schedule rule
 * RETURN:
 *          int             --  0:  SUCCESS
 *                             -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_get_schedule_rule(schedule_setting_s *pDevice_data)
{
    printf("Get schedule rule!\n");
    return 0;
}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_get_managed_device
 *
 * PURPOSE: get managed device configuration in device
 *
 * INPUT:   none
 *
 * OUTPUT:
 *          pDevice_data    -- data structure of managed device
 * RETURN:
 *          int             --  0:  SUCCESS
 *                             -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_get_managed_device(mac_filter_s *pDevice_data)
{
    printf("Get managed device! but the device do not suppport this function\n");
    return 0;
}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_get_wps
 *
 * PURPOSE: get wps configuration in device
 *
 * INPUT:   none
 *
 * OUTPUT:
 *          pDevice_data    -- data structure of wps
 * RETURN:
 *          int             --  0:  SUCCESS
 *                             -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_get_wps(wps_s *pDevice_data)
{
    printf("Get wps!\n");
	char *pEnableWsc=NULL;
	char *pEnableWpsPin=NULL;

	/* WPS Get */ //TODO: Consider 2.4G or 5G
	pEnableWsc = nvram_safe_get("WscCusPBCEnable");
	pEnableWpsPin = nvram_safe_get("WscCusPINEnable");


	if(pEnableWsc && strlen(pEnableWsc) != 0 && 0 == strcmp("0",pEnableWsc))
		pDevice_data->wpspbc = 0;
	else
		pDevice_data->wpspbc = 1;

	if(pEnableWpsPin && strlen(pEnableWpsPin) != 0 && 0 == strcmp("0",pEnableWpsPin)) //unlock
		pDevice_data->wpspin= 0;
	else //locked(default)
		pDevice_data->wpspin= 1;

    return 0;
}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_get_ntp
 *
 * PURPOSE: get ntp configuration in device
 *
 * INPUT:   none
 *
 * OUTPUT:
 *          pDevice_data    -- data structure of ntp
 * RETURN:
 *          int             --  0:  SUCCESS
 *                             -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_get_ntp(time_setting_s *pDevice_data)
{
    char CurrentDate[12] = {0};
    char CurrentTime[12] = {0};
    time_t tCurrTime = 0;
    struct tm *pstTm;
    tCurrTime = time(NULL);
    int Hours = 0, Minutes =0;
    char *pszTZOffset = NULL;

    printf("Get ntp!\n");

    tCurrTime = time(NULL);
    // check timezone token
    pszTZOffset = nvram_safe_get("ntp_timezone");
    // shift time by token*3600
	if(strchr(pszTZOffset, '-')) {
		sscanf(pszTZOffset, "-%d:%d", &Hours, &Minutes);
        tCurrTime += (Hours * 60 + Minutes) * 60;
	} else {
		sscanf(pszTZOffset, "+%d:%d", &Hours, &Minutes);
		tCurrTime -= (Hours * 60 + Minutes) * 60;
	}

	if(nvram_get_int("ntp_DaylightSavingsUsed")) {
        tCurrTime -= 3600;
	}
	
    pstTm = localtime(&tCurrTime);
    
    // bypass daylight since web will not add daylight

    sprintf(CurrentDate, "%04d/%02d/%02d", pstTm->tm_year + 1900, pstTm->tm_mon + 1, pstTm->tm_mday);
    sprintf(CurrentTime, "%02d:%02d:%02d", pstTm->tm_hour, pstTm->tm_min, pstTm->tm_sec);
	
	strncpy(pDevice_data->cur_date, CurrentDate, sizeof(pDevice_data->cur_date));
	strncpy(pDevice_data->cur_time, CurrentTime, sizeof(pDevice_data->cur_time));

	pDevice_data->ntp = atoi(nvram_safe_get("ntpEnabled")) ;
	strncpy(pDevice_data->ntp_server, nvram_safe_get("ntp_server"), sizeof(pDevice_data->ntp_server));	
	strncpy(pDevice_data->tz, (strlen(nvram_safe_get("ntp_timezone_num"))) ? nvram_safe_get("ntp_timezone_num") : "27", sizeof(pDevice_data->ntp_server));	
	//pDevice_data->tz = (strlen(nvram_safe_get("ntp_timezone_num"))) ? nvram_safe_get("ntp_timezone_num") : "27"/*TZ*1,2,3...*/,
	pDevice_data->daylight_saving = atoi(nvram_safe_get("ntp_DaylightSavingsUsed"));
	
    return 0;
}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_get_administration
 *
 * PURPOSE: get administration configuration in device
 *
 * INPUT:   none
 *
 * OUTPUT:
 *          pDevice_data    -- data structure of administration
 * RETURN:
 *          int             --  0:  SUCCESS
 *                             -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_get_administration(dev_setting_s *pDevice_data)
{
    printf("Get administration!\n");

	strncpy(pDevice_data->admin_pwd, nvram_safe_get("http_passwd"), sizeof(pDevice_data->admin_pwd));	
	strncpy(pDevice_data->presentation_url, nvram_safe_get("PresentURL"), sizeof(pDevice_data->presentation_url));	
	pDevice_data->captcha = atoi(nvram_safe_get("CAPTCHA"));
	pDevice_data->change_pwd = 0;

	return 0;

}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_get_wlan_all
 *
 * PURPOSE: get all wlan configuration in device
 *
 * INPUT:   none
 *
 * OUTPUT:
 *          pDevice_data    -- data structure of administration
 * RETURN:
 *          int             --  0:  SUCCESS
 *                             -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_get_wlan_all(wlan_s *pDevice_wlan, radio_s *pDevice_radio)
{
	int i = 0;
    wlan_s wlan;
    radio_s radio;
	int iNVRAM_Idx = RT2860_NVRAM;
    int iIntfIdx = 0;

    printf("Get wlan!\n");

	wlan.wlan_sum = 4;
	wlan.wlan_sub_count = 4;

	radio.radio_sum = 4;
	radio.radio_sub_count = 4;

	for(i = 0; i < wlan.wlan_sum; i++)
	{
		char tmpAuthMode[128];
		char tmpEncrypType[128];
		char tmpBroadcast[128];
		char *AuthMode, *EncrypType, *Broadcast;
		const char * pKey=NULL;
		const char * pSchNode=NULL;
		const char * pSsid=NULL;
		const char * pMode=NULL;
		const char * pHT_BW=NULL;
		const char * pHT_BSSCoexistence=NULL;
		const char * pVHT_BW=NULL;
		const char * pTxPower=NULL;

		iNVRAM_Idx = apson_sb_get_nvramidx(i);
		iIntfIdx = apson_sb_get_intfIdx(i);

        pMode = nvram_get(iNVRAM_Idx,"WirelessMode");
		strncpy(radio.radio_sub[i].radioid, apson_sb_get_radioid(i), sizeof(radio.radio_sub[i].radioid));
		strncpy(radio.radio_sub[i].mode, G_wlMode[atoi(pMode)], sizeof(radio.radio_sub[i].mode));

		if(0 == i)
		{
			radio.radio_sub[i].enable = strcmp(nvram_get(iNVRAM_Idx, "Radio24On"), "1") ? 0 : 1;
			radio.radio_sub[i].band_steer_enable = strcmp(nvram_safe_get("BandSteering"), "1")? 0 : 1;
		}
		else if(1 == i)
		{
			radio.radio_sub[i].enable = strcmp(nvram_get(iNVRAM_Idx, "Radio24Vap1En"), "1") ? 0 : 1;
			wlan.wlan_sub[i].isolate = strcmp(nvram_safe_get("guest_login_blocked"), "1")? 0 : 1;
		}
		else if(2 == i)
		{
			radio.radio_sub[i].enable = strcmp(nvram_get(iNVRAM_Idx, "Radio58On"), "1") ? 0 : 1;
			radio.radio_sub[i].band_steer_enable = strcmp(nvram_safe_get("BandSteering"), "1")? 0 : 1;
		}
		else if(3 == i)
		{
			radio.radio_sub[i].enable = strcmp(nvram_get(iNVRAM_Idx, "Radio58Vap1En"), "1") ? 0 : 1;
			wlan.wlan_sub[i].isolate = strcmp(nvram_safe_get("guest_login_blocked"), "1")? 0 : 1;
		}
		
		if(1 >= i)
		{
            pHT_BW = nvram_get(iNVRAM_Idx,"HT_BW");
			pHT_BSSCoexistence = nvram_get(iNVRAM_Idx,"HT_BSSCoexistence");
			
			if(0 == strcmp(pHT_BW,"1"))
			{
			    if(0 == strcmp(pHT_BSSCoexistence,"1"))
			    {
					radio.radio_sub[i].ch_width = CHANNEL_WIDTH_AUTO2040M;
					radio.radio_sub[i].coexistence = 1;
			    }
				else
					radio.radio_sub[i].ch_width = CHANNEL_WIDTH_40M;

			}
			else
				radio.radio_sub[i].ch_width = CHANNEL_WIDTH_20M;

			radio.radio_sub[i].second_ch = atoi(nvram_get(iNVRAM_Idx,"HT_EXTCHA"));
		}
		else
		{
            pHT_BW = nvram_get(iNVRAM_Idx,"HT_BW");
			pVHT_BW = nvram_get(iNVRAM_Idx,"VHT_BW");
			
			if(0 == strcmp(pVHT_BW,"0"))
			{
			    radio.radio_sub[i].ch_width = CHANNEL_WIDTH_AUTO2040M;
			}
			else if(0 == strcmp(pVHT_BW,"1"))
				radio.radio_sub[i].ch_width = CHANNEL_WIDTH_AUTO204080M;
			else if(0 == strcmp(pVHT_BW,"2"))
				radio.radio_sub[i].ch_width = CHANNEL_WIDTH_AUTO204080160M;
			else if(0 == strcmp(pVHT_BW,"3"))
				radio.radio_sub[i].ch_width = CHANNEL_WIDTH_AUTO20408080M;
		}

		radio.radio_sub[i].ch = atoi(nvram_get(iNVRAM_Idx,"CusChannel"));

		pTxPower = nvram_get(iNVRAM_Idx,"TxPower");
		if(!((0 == strcmp(pTxPower,"100"))||(0 == strcmp(pTxPower,"50"))||(0 == strcmp(pTxPower,"25"))))
		    pTxPower = TX_POWER_MID;	
		radio.radio_sub[i].tx_power = atoi(pTxPower);

		if(0 == strcmp(nvram_get(iNVRAM_Idx,"BandSteering"), "1"))
			radio.radio_sub[i].band_steer_enable = 1;
		else			
			radio.radio_sub[i].band_steer_enable = 0;

		if (i == 2) //only for  5g service
		{			
			Broadcast = (char *) nvram_bufget(iNVRAM_Idx, "HideSSID_5");
			if(0 == strcmp(Broadcast, "1"))
				wlan.wlan_sub[i].ssid_bcast = 0;
			else			
				wlan.wlan_sub[i].ssid_bcast = 1;

		}
		else
		{
			Broadcast = (char *) nvram_bufget(iNVRAM_Idx, "HideSSID");
			get_nth_value(iIntfIdx, Broadcast, ';', tmpBroadcast, 128);

			if(0 == strcmp(tmpBroadcast, "1"))
				wlan.wlan_sub[i].ssid_bcast = 0;
			else			
				wlan.wlan_sub[i].ssid_bcast = 1;

		}
	
		printf("get bcast: %d \n", wlan.wlan_sub[i].ssid_bcast);
		//nvram_bufget(i_nvram_2g_index, "CountryRegion");			
		strncpy(wlan.wlan_sub[i].radioid, apson_sb_get_radioid(i), sizeof(wlan.wlan_sub[i].radioid));
		strncpy(wlan.wlan_sub[i].ifname, apson_sb_get_ifname(i), sizeof(wlan.wlan_sub[i].ifname));
		pSsid = nvram_bufget(iNVRAM_Idx,racat("SSID", iIntfIdx+1));

		strncpy(wlan.wlan_sub[i].ssid, pSsid, sizeof(wlan.wlan_sub[i].ssid));
	    AuthMode = (char *) nvram_bufget(iNVRAM_Idx, "AuthMode");
		EncrypType = (char *) nvram_bufget(iNVRAM_Idx, "EncrypType");
		
		get_nth_value(iIntfIdx, AuthMode, ';', tmpAuthMode, 128);
		get_nth_value(iIntfIdx, EncrypType, ';', tmpEncrypType, 128);
		if(0 == strcmp(tmpAuthMode, "WPAPSKWPA2PSK"))
		{
			wlan.wlan_sub[i].secu_enable = 1;
			strncpy(wlan.wlan_sub[i].type, "WPAORWPA2-PSK", sizeof(wlan.wlan_sub[i].type));
		}
		else if(0 == strcmp(tmpAuthMode, "WPA2PSK"))
		{
			wlan.wlan_sub[i].secu_enable = 1;
			strncpy(wlan.wlan_sub[i].type, "WPA2-PSK", sizeof(wlan.wlan_sub[i].type));
		}
		else //0 == strcmp(tmpAuthMode, "OPEN") or WEP(TODO)
		{
			wlan.wlan_sub[i].secu_enable = 0;
			strncpy(wlan.wlan_sub[i].type, "", sizeof(wlan.wlan_sub[i].type));
		}

		if(0 == strcmp(tmpEncrypType, "TKIPAES") || 0 == strcmp(tmpEncrypType, "AES"))
		{
			strncpy(wlan.wlan_sub[i].encryption, "TKIPORAES", sizeof(wlan.wlan_sub[i].encryption));
			pKey = nvram_bufget(iNVRAM_Idx,racat("WPAPSK", iIntfIdx+1));
			strncpy(wlan.wlan_sub[i].key, pKey, sizeof(wlan.wlan_sub[i].key));
		}
		else //0 == strcmp(tmpAuthMode, "OPEN") or WEP(TODO)
		{
			strncpy(wlan.wlan_sub[i].encryption, "", sizeof(wlan.wlan_sub[i].encryption));
			strncpy(wlan.wlan_sub[i].key, "", sizeof(wlan.wlan_sub[i].key));
		}

		pSchNode = apson_sb_get_schnode(i);
		strncpy(wlan.wlan_sub[i].sche_name, nvram_bufget(iNVRAM_Idx, pSchNode), sizeof(wlan.wlan_sub[i].sche_name));

		wlan.wlan_sub[i].isolate = iIntfIdx;




	}

	memcpy(pDevice_wlan, &wlan, sizeof(wlan_s));
	memcpy(pDevice_radio, &radio, sizeof(radio_s));
	
    return 0;
}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_get_firmware_version
 *
 * PURPOSE: get current firmware version in device
 *
 * INPUT:   none
 *
 * OUTPUT:
 *          pDevice_data    -- current firmware version
 * RETURN:
 *          int             --  0:  SUCCESS
 *                             -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_get_firmware_version(char *pDevice_data)
{
	char version[128]={0};
    printf("Get firmware version!\n");

	sprintf(version, "%s%s", nvram_safe_get("sw_ex_version"), nvram_safe_get("sw_in_version"));	
	
	strcpy(pDevice_data, version);

    return 0;
}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_set_dlinkmaster
 *
 * PURPOSE: set apson master in device
 *
 * INPUT:   domain          -- dns domain in apson
 *          ip              -- master ip of apson
 * OUTPUT:  none
 *
 * RETURN:
 *          int             --  0:  SUCCESS
 *                             -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_set_dlinkmaster(char *domain, char *ip)
{
    printf("Set dlinkmaster!\n");
    printf("domain = %s, master ip = %s\n", domain, ip);
    return 0;
}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_init_apson
 *
 * PURPOSE: get apson configuration to make /etc/config/apson configuration in device, format is:
 *
 *          interface=br-lan
 *          prefer_master=0
 *          election=1
 *          domain=none
 *          model=DIR-878
 *          router=1
 *          ipv6=0
 *          fwver=none
 *          ap_name=none
 *          mesh_ssid=none
 *          mesh_security_mode=WPA2PSK
 *          mesh_encryption_type=AES
 *          mesh_psk=none
 *          eth_list=eth0
 *          eth_speed=1000
 *          br_name=br-lan
 *          mesh_module_antenna_num=4
 *          mesh_module_uplink_if_name=apclii0
 *          mesh_module_downlink_if_name=rai3
 *          isgateway=1
 *          wan_speed=1000
 *          wlan_base_if_name_2g=ra0
 *          wlan_base_if_name_5g=rai0
 *
 * INPUT:   none
 *
 * OUTPUT:  none
 *
 * RETURN:
 *          int             --  0:  SUCCESS
 *                             -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_init_apson(void)
{
    FILE *fp = NULL;
	char *domain = NULL;
	char *ssid = NULL;
	char *secmode = NULL;
	char *encmode = NULL;
	char *psk = NULL;
	unsigned char mac_addr[6] = { 0 };
	int ret = 0;

    printf(" apson_sb_init_apson init apson!\n");

    fp = fopen(APSON_CONF_FILE_PATH, "w+");
    if(fp)
	{
		fprintf(fp, "interface=br0\n");	
		fprintf(fp, "prefer_master=0\n");	
		fprintf(fp, "election=1\n");	

		domain = nvram_safe_get("mesh_domain");
		ssid = nvram_safe_get("mesh_ssid");
		secmode = nvram_safe_get("mesh_security_mode");
		encmode = nvram_safe_get("mesh_encryption_type");
		psk = nvram_safe_get("mesh_psk");

        fprintf(fp, "domain=%s\n", strlen(domain)==0? "none" :domain);
        fprintf(fp, "mesh_ssid=%s\n", strlen(ssid)==0?  "none" : ssid);
        fprintf(fp, "mesh_psk=%s\n", strlen(psk)==0? "none" : psk);
        fprintf(fp, "mesh_security_mode=%s\n", strlen(secmode)==0? "WPA2PSK" : G_secu[atoi(secmode)]);
        fprintf(fp, "mesh_encryption_type=%s\n", strlen(encmode)==0? "AES" : G_enc[atoi(encmode)]);

		fprintf(fp, "ipv6=0\n");	
		ret = apson_get_macaddr(mac_addr);
		if (ret == 1)	
			fprintf(fp, "ap_name=%s\n", nvram_safe_get("model_name"));	
		else
			fprintf(fp, "ap_name=%s_%02X%02X\n", nvram_safe_get("model_name"), mac_addr[4], mac_addr[5]);	

		fprintf(fp, "fwver=%s%s\n", nvram_safe_get("sw_ex_version"), nvram_safe_get("sw_in_version"));	
		fprintf(fp, "br_name=br0\n");	

		if(0 ==strcmp(nvram_safe_get("wan_wan0_proto"), "Bridge"))
		{
			fprintf(fp, "model=%s\n", nvram_safe_get("model_name"));	
			fprintf(fp, "router=0\n");	
			fprintf(fp, "isgateway=0\n");	
			fprintf(fp, "eth_list=eth2.1,eth2.2,eth2.3,eth2.4,eth3\n");	
			fprintf(fp, "eth_speed=1000,1000,1000,1000,1000\n");	

		}
		else
		{
			fprintf(fp, "model=%s\n", nvram_safe_get("model_name"));	
			fprintf(fp, "router=1\n");	
			fprintf(fp, "isgateway=1\n");	
			fprintf(fp, "eth_list=eth2.1,eth2.2,eth2.3,eth2.4\n");	
			fprintf(fp, "eth_speed=1000,1000,1000,1000\n");	
		}

		fprintf(fp, "mesh_module_antenna_num=2\n");			
		fprintf(fp, "mesh_module_uplink_if_name=%s\n", INTERFACE_5G_MESH_UPLINK);	
		fprintf(fp, "mesh_module_downlink_if_name=%s\n", INTERFACE_5G_MESH_DOWNLINK);	
		fprintf(fp, "wan_speed=1000\n");	
		fprintf(fp, "wlan_base_if_name_2g=%s\n", INTERFACE_2G);	
		fprintf(fp, "wlan_base_if_name_5g=%s\n", INTERFACE_5G);			
        fflush(fp);
        fclose(fp);
    }
	else
		printf("open conf fail \n");

    return 0;//FAILED
}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_set_ap_name
 *
 * PURPOSE: set ap name in NVRAM.
 *
 * INPUT:   pMsg        -- ap name
 *
 * OUTPUT:
 *          none
 * RETURN:
 *          int         --  0:  SUCCESS
 *                         -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_set_ap_name(char* pMsg)
{
    printf("Set ap name: %s\n", pMsg);
    return 0;
}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_set_apson
 *
 * PURPOSE: set apson configuration in NVRAM
 *
 * INPUT:   pSetting_apson  -- data structure of apson configuration
 *
 * OUTPUT:  none
 *
 * RETURN:
 *          int             --  0:  SUCCESS
 *                             -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_set_apson(vendor_apson_s *pSetting_apson)
{
	char secu[32]={0};
	char enc[32]={0};
    printf("apson_sb_set_apson start \n");

    printf("apson conf domain(%s) ssid(%s) secu(%d) enc(%d) psk(%s)\n", pSetting_apson->domain, pSetting_apson->ssid, pSetting_apson->security_mode, pSetting_apson->encryption_mode, pSetting_apson->psk);

	sprintf(secu,"%d",pSetting_apson->security_mode);
	sprintf(enc,"%d",pSetting_apson->encryption_mode);

    nvram_safe_set("mesh_domain", pSetting_apson->domain);
    nvram_safe_set("mesh_ssid", pSetting_apson->ssid);
    nvram_safe_set("mesh_security_mode", secu);
    nvram_safe_set("mesh_encryption_type", enc);
    nvram_safe_set("mesh_psk", pSetting_apson->psk);
	
	nvram_commit(RT2860_NVRAM);

    return 0;
}

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_sb_set_channel
 *
 * PURPOSE: If channel is auto now, take effect radio channel, maybe by iwpriv(), but don't save in NVRAM. 
 *          If channel is not auto, do nothing and return success.
 *
 * INPUT:   radioid  -- radio id to identify set in which radio
 *          ch       -- the value of channel need to take effect in radio configuration
 * OUTPUT:  none
 *
 * RETURN:
 *          int             --  0:  SUCCESS
 *                             -1:  FAIL
 * NOTE:
 *-----------------------------------------------------------
 */
int apson_sb_set_channel(char *radioid, int ch)
{
	char cmd[64]={0};
	char ifname[8]={0};
	int iNVRAM_Idx = RT2860_NVRAM;

    printf("set channel!\n");
    printf("radioid = %s!\n", radioid);
    printf("ch = %d!\n", ch);

	iNVRAM_Idx = apson_sb_get_nvramidx_by_radioid(radioid);
	if(iNVRAM_Idx < 0)
	{
		printf("radioid should not be empty!");
		return 1;	
	}

	if(atoi(nvram_get(iNVRAM_Idx,"CusChannel")) != 0)
		return 1;

	if (ch == 0)
		return 1;

	strcpy(ifname, apson_sb_get_ifname_by_radioid(radioid));
	sprintf(cmd, "iwpriv %s set Channel=%d", ifname, ch);
	system(cmd);

    return 0;
}
