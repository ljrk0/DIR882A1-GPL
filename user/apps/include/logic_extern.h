
#ifndef _LOGIC_MODE_EXTERN_H_
#define _LOGIC_MODE_EXTERN_H_

#include "public.h"
/******************************************************************************
*                               GLOBAL VAR                                   *
******************************************************************************/
extern char tw_download_hdr[256];

/******************************************************************************
*                           FUNC DECLEARATION                                 *
******************************************************************************/
/*public*/
extern unsigned short tw_valid_range(const webs_t wp, const char *value, const struct tw_variable *v);
extern unsigned short tw_checkenable (const webs_t wp, const char *value, const struct tw_variable *v, const char *varname);
extern unsigned short tw_match(const char *pszString, const char *pszPattern);
extern unsigned short tw_valid_name(const webs_t wp, const char *value, const struct tw_variable *v);
extern unsigned short tw_validate_name(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname );
extern unsigned short tw_check_hostname(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname);
extern unsigned short tw_validate_range(const webs_t wp, const char *value, const struct tw_variable *v ,const char *varname);
extern unsigned short tw_validate_choice(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname);
extern unsigned short tw_validate_ipaddr_range(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname );
extern unsigned short tw_validate_ipaddr(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname );
extern unsigned short tw_validate_netmask(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname );
extern unsigned short tw_validate_hwaddr(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname );
extern unsigned long tw_CnvtIpFromStr(const char *pcIp);
extern unsigned short tw_valid_port_range(const webs_t wp, const char *value, const struct tw_variable *v,const char *varname);
extern int tw_split_string(char *str, char *substr[], const char delimit, const int max_count);
extern unsigned short tw_ip_range_conflict_check(const char* ip1, const char* mask1, const char* ip2, const char* mask2);
extern char* eatAllSpace(const char* s, char* buf);
extern int tw_NvramListMatch(const char* name, const char* value);
extern int tw_invNvramListMatch(const char* name, const char* value);
extern char *strreplace(char *dest, char *src, const char *oldstr, const char *newstr, size_t len);
extern char* replaceHtmlSpecial(char* src);
extern int tw_check_mac(const char *smac);
extern unsigned short tw_check_domain(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname );

/*logic*/
extern struct tw_variable*get_var_handle(char *varname);
extern int variables_arraysize(void);
extern int internal_init(void);
extern unsigned short tw_validate_cgi(webs_t wp);
extern void tw_apply_cgi(char *url, FILE * stream);
extern void do_dataCenter(char *url, FILE * stream);
extern void do_postCenter(char *url, FILE *stream, int len, char *boundary);
extern void do_applyPost(char *url, FILE *stream);
extern void do_language_set(char *url, FILE *stream);
void do_displayFolder(char *url, FILE *stream);

/*auth*/
extern unsigned short tw_auth_logic(webs_t wp, char *s);
extern int auth_checkhandler(const char* pstAddr);

/*autoupdate*/
extern unsigned short CHECKUPDATE_checkStart(webs_t wp, char *s);
extern unsigned short CHECKUPDATE_Apply(webs_t wp, char *s);

/*portmapping*/
//extern unsigned short tw_port_forwrd_prepare(webs_t wp,char *s);
extern unsigned short tw_validate_forward_port(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname);

/*port trigger*/
extern unsigned short tw_validate_trigger_rules(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname);
extern void upload_trigger_rules();


/* parent control */
extern unsigned short tw_validate_parental_control_rules(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname);
extern unsigned short tw_validate_macaddr(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname );


/*ddns*/
extern unsigned short tw_checkddnsprovider (const webs_t wp, const char *value, const struct tw_variable *v, const char *varname);
extern unsigned short tw_checkddnsusername (const webs_t wp, const char *value, const struct tw_variable *v, const char *varname);
extern unsigned short tw_checkddnspassword (const webs_t wp, const char *value, const struct tw_variable *v, const char *varname);
extern unsigned short tw_checkddnshostname (const webs_t wp, const char *value, const struct tw_variable *v, const char *varname);
extern unsigned short  tw_ddns_logic(webs_t wp, char *s);

/*wlan*/
extern unsigned short tw_wlan_logic_check(webs_t wp,  char *s);
extern unsigned short tw_wiz_flag_set(webs_t wp,  char *s);
extern unsigned short tw_cafestyle_rules_add(webs_t wp,  char *s);
extern unsigned short tw_2g_chanspec_check(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname);
extern unsigned short tw_5g_chanspec_check(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname);
extern unsigned short tw_validate_ssid(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname);
extern unsigned short tw_validate_wl_key(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname);
extern unsigned short tw_validate_wl_wpa_psk(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname);
extern unsigned short tw_wps_check_prepare(webs_t wp, char *s);
extern unsigned short tw_wps_cgi(webs_t wp, char *s);
extern unsigned short tw_wlan_time_logic(webs_t wp, char *s);
extern unsigned short tw_validate_wl_wds_hwaddrs(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname);

/*syslog*/
extern unsigned short delete_syslog_cgi(webs_t wp, char *s);
extern int syslog_download_sysevt_logfile(FILE *stream);
extern int creat_syslog_jsonfile(char * fpath);

/* miniupnp */
extern void creat_upnpinfo_jsonfile();

/*btdown*/
extern unsigned short tw_downloader_logic(webs_t wp, char *s);
extern void do_bt_post(char *url, FILE *stream, int len, char *boundary);
extern void tw_do_bt_cgi(char *url, FILE *stream);

/*qos*/
extern unsigned short tw_valid_qos_var(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname );
extern unsigned short tw_validate_ip_mac(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname );
extern unsigned short tw_qos_logic(webs_t wp, char *s);

/*schedules*/
extern unsigned short tw_checktime(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname);
extern unsigned short tw_checkscheduledays(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname);

/*mail*/
extern unsigned short tw_mail_checkhost(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname);
extern unsigned short tw_mail_checkemail(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname);
extern unsigned short tw_uploadCenter_cgi(webs_t wp, char *s);

/*wan*/
extern unsigned short tw_validate_dnsservers(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname );
extern unsigned short tw_wan_unit(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname);
extern unsigned short WAN_MAC_logicCheck(webs_t wp,  char *s);
extern unsigned short tw_wandhcp_logic(webs_t wp, char *s);
extern unsigned short tw_pppoe_prepare(webs_t wp, char *s);
extern unsigned short tw_pppoe_logicCheck(webs_t wp,  char *s);
extern unsigned short tw_vpn_logicCheck(webs_t wp,  char *s);
extern unsigned short tw_wanStatic_prepare(webs_t wp, char *s);
extern unsigned short tw_wanStatic_logic(webs_t wp,  char *s);
extern char* tw_wan_mac_multiconn_set(webs_t wp,  char *s, unsigned short reslut);

/*alc*/
extern unsigned short tw_validate_ping(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname);
extern unsigned short ut_system_set_apply(webs_t wp, char * s);

/*diagnastics*/
extern unsigned short tw_diag_prepare(webs_t wp, char *s);
extern unsigned short tw_diag_logic(webs_t wp, char *s);

/*urlfilter*/
extern unsigned short tw_url_filter_rule(webs_t wp, char *s);

/*lan*/
extern unsigned short tw_lan_prepare(webs_t wp, char *s);
extern unsigned short tw_lan_logic_check(webs_t wp, char *s);

/*dhcps*/
extern int Creat_DhcpList_JsonFile(void);

/*vlan*/
extern unsigned short tw_apmode_logic(webs_t wp,char *s);

/*upgload*/
extern void tw_downloadCenter_cgi(char *url, FILE *stream);
//extern void tw_do_upgrade_cgi(char *url, FILE *stream);

/*static route*/
extern unsigned short tw_sroute_logic(webs_t wp,  char *s);
extern unsigned short tw_validate_sroute_choice(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname );


/*ftp*/
extern unsigned short ftpd_check_port(const webs_t wp, const char *value, const struct tw_variable *v);
extern unsigned short tw_validate_ftp_config(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname );

/*reserve ip*/
extern unsigned short tw_validate_ip_reserve_check(const webs_t wp, const char *value, const struct tw_variable *v, const char *varname );


/*STA*/
unsigned short tw_staAssoc_logic(webs_t wp, char *s);
/*repeater*/
unsigned short tw_repeater_logic(webs_t wp, char *s);
/*client scan*/
int tw_wl_EnrScan(char *filename);
/*client status*/
int tw_staAssoc_status(void);
/*statistics*/
extern int create_trafficStatistics_jsonFile(char * filePath);

/*restore*/
extern unsigned short tw_restore_logic(webs_t wp, char *s);


#endif /*_LOGIC_MODE_EXTERN_H_*/

