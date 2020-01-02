
#ifndef _LOGIC_RCM_EXTERN_H_
#define _LOGIC_RCM_EXTERN_H_

#include <stdio.h>

/*public*/

extern int getRCModeType(void);
extern int cleanRCModeType(void);
extern int setAllRCModetype(void);
extern void setRCWANModeType(int wanType);

/*antiattack*/
extern int TW_Anti_DOS_Config();
extern int TW_Anti_DOS_ReConfig();
extern int TW_Spi_Fw_Config();
extern int Get_The_Interface_By_Proto(char *ifname);

/*dns*/
extern int tw_add_ns(char *wan_ifname);

/*sntp*/
extern int tw_set_localtime_to_nvram(void);
extern int tw_check_Iscurrtime_Indsrange(void);
extern int tw_set_dstoffset(void);
extern int tw_set_timezone(void);

/*lan*/
extern int LAN_WANIPConflictHandler(char * chWanIfIP , char * chWanIfMask);
extern int tw_add_reserve_ip_to_file(FILE *fp);

/*devinfo*/
extern int tw_add_device_mac(void);
extern int mac_check_and_change(const char *smac, char *dmac);
extern int restore_do_unset(void);

/*acl*/
extern int load_iptalbes();
extern int stop_iptalbes();
extern int tw_wl_guest_cafestyle(void);
extern char * tw_get_wan_interface(char *wan_name);

int load_IPv6_iptables(void);
int IPv6_REMOTEMANGE_run(void);

/*static router*/
extern int tw_sroute_init(char *ifname);
/*ftpd*/
extern int tw_start_ftpd(void);
extern int tw_stop_ftpd(void);

/*alg*/
extern int start_alg_service(void);

/* miniupnp */

void miniupnpd( const char * pcWanIfnName);
void start_miniupnpd_finish( const char * pcWanIfnName);
void stop_miniupnpd(void);
void start_miniupnpd( void );

/*other services*/
extern int start_timer(void);
extern int stop_timer(void);
extern int start_logmonitor(void);
extern int stop_logmonitor(void);
extern int start_klogd(void);
extern int stop_klogd(void);
extern int start_syslogd(void);
extern int stop_syslogd(void);
extern int start_btn(void);
extern int stop_btn(void);
extern int start_downloader();
extern int stop_downloader();

extern int USBWAN_device_init();
extern int USBWAN_remove_card(unsigned int ui_vid, unsigned int ui_pid);
extern int USBWAN_switch_card_common(unsigned int ui_vid, unsigned int ui_pid);
extern int USBWAN_check_card_support(unsigned int ui_vid, unsigned int ui_pid);
extern int USBWAN_generate_3G_ppp_conf(int unit);
extern char *Gobi_SimCard(char *line, int size);	//get sim card status line
extern int Gobi_SimCardReady(const char *line);		//check status via status line
extern char *Gobi_IMEI(char *line, int size);		//get device IMEI number
extern char *Gobi_ConnectISP(char *line, int size);	//get current connect ISP name
extern int Gobi_ConnectStatus_Int(void);		//get current connect status number (GSM/…/CDMA/LTE)
extern char *Gobi_ConnectStatus_Str(int status);	//convert status number to string
extern int Gobi_SignalQuality_Int(void);		//get signal quality value (0~31)
extern int Gobi_SignalQuality_Percent(int value);	//convert quality value (0~31) to percentage
extern int Gobi_SignalLevel_Int(void);			//get signal quality (level) in dBm
extern char * Gobi_FwVersion(char *line, int size);
extern char * Gobi_QcnVersion(char *line, int size);
extern char * Gobi_BandChannel(char *line, int size);

extern int tw_get_usb_wan_interface(char *wan_name);
extern int tw_get_cur_wan_type(char *wan_type);

#ifdef CONFIG_TZ_LOCATION

int schedule_iptables_start(const char * pRule_name);
int schedule_iptables_stop(const char * pRule_name);

#endif

#endif /*_LOGIC_RCM_EXTERN_H_*/


