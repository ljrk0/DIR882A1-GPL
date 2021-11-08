#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
//#include <bcmnvram.h>
#include "shutils.h"
#include <signal.h>
#include <string.h>
//#include <bcmnvram.h>
#include <time.h>
#include "bcmtimer.h"
#include "common.h"
#include <stdarg.h>
#include <syslog.h>
#include <fcntl.h>

#include "nvram.h"
#include <shared.h>
#include <shutils.h>
#include "../../../../autoconf.h"
//#include "Notify_rc.h"

#define BCM_TIMER_ID_INIT 						0
#define BCM_TIMER_INTERVAR_PPP_DETECT			8
#define BCM_TIMER_INTERVAR_SYSLOG_MAIL			60
#define BCM_TIMER_INTERVAR_SYSLOG_MAIL_PERIOD	60
#define BCM_TIMER_INTERVAR_WAN_LEDCTRL			5		
#define BCM_TIMER_INTERVAR_GET_ONLINE_FWVER		3600 * 24 * 7
#define BCM_TIMER_INTERVAR_WLAN_LEDCTRL			5
#define BCM_TIMER_INTERVAR_APCLI_STATUS			3
#define BCM_TIMER_INTERVAR_APCLI_CONNECT_STATUS	30
#define BCM_TIMER_INTERVAR_WLAN					5
#define BCM_TIMER_INTERVAR_FOTA_AUTO_UPGRADE	59
#define BCM_TIMER_INTERVAR_APCLI_STATUS_DETECT	3
#define BCM_TIMER_INTERVAR_APCLI_WPS			5
#define BCM_TIMER_INTERVAR_AP_WPS				5

enum {
	HTTP_TIMER_PPP_DETECT=1,   //0
	HTTP_TIMER_SYSLOG_MAIL,
	HTTP_TIMER_SYSLOG_MAIL_PERIOD,
	HTTP_TIMER_WAN_LEDCTRL,
	HTTP_TIMER_GET_ONLINE_FWVER,
	HTTP_TIMER_WLAN_LEDCTRL,
	HTTP_TIMER_FOTA_MONITOR,
	HTTP_TIMER_APCLI_STATUS,
	HTTP_TIMER_APCLI_CONNECT_STATUS,
	HTTP_TIMER_WLAN,
	HTTP_TIMER_DETECT_APCLI_STATUS,
	HTTP_TIMER_DETECT_APCLI_WPS,
	HTTP_TIMER_DETECT_AP_WPS,
	
	HTTP_TIMER_DETECT_MAX_NUM
};

#ifdef CONFIG_AP_REPEATER_SUPPORT

#include <linux/wireless.h>

/* Wsc status code */
#define	STATUS_WSC_NOTUSED						0
#define	STATUS_WSC_IDLE							1
#define STATUS_WSC_FAIL        			        2		/* WSC Process Fail */
#define	STATUS_WSC_LINK_UP						3		/* Start WSC Process */
#define	STATUS_WSC_EAPOL_START_RECEIVED			4		/* Received EAPOL-Start */
#define	STATUS_WSC_EAP_REQ_ID_SENT				5		/* Sending EAP-Req(ID) */
#define	STATUS_WSC_EAP_RSP_ID_RECEIVED			6		/* Receive EAP-Rsp(ID) */
#define	STATUS_WSC_EAP_RSP_WRONG_SMI			7		/* Receive EAP-Req with wrong WSC SMI Vendor Id */
#define	STATUS_WSC_EAP_RSP_WRONG_VENDOR_TYPE	8		/* Receive EAPReq with wrong WSC Vendor Type */
#define	STATUS_WSC_EAP_REQ_WSC_START			9		/* Sending EAP-Req(WSC_START) */
#define	STATUS_WSC_EAP_M1_SENT					10		/* Send M1 */
#define	STATUS_WSC_EAP_M1_RECEIVED				11		/* Received M1 */
#define	STATUS_WSC_EAP_M2_SENT					12		/* Send M2 */
#define	STATUS_WSC_EAP_M2_RECEIVED				13		/* Received M2 */
#define	STATUS_WSC_EAP_M2D_RECEIVED				14		/* Received M2D */
#define	STATUS_WSC_EAP_M3_SENT					15		/* Send M3 */
#define	STATUS_WSC_EAP_M3_RECEIVED				16		/* Received M3 */
#define	STATUS_WSC_EAP_M4_SENT					17		/* Send M4 */
#define	STATUS_WSC_EAP_M4_RECEIVED				18		/* Received M4 */
#define	STATUS_WSC_EAP_M5_SENT					19		/* Send M5 */
#define	STATUS_WSC_EAP_M5_RECEIVED				20		/* Received M5 */
#define	STATUS_WSC_EAP_M6_SENT					21		/* Send M6 */
#define	STATUS_WSC_EAP_M6_RECEIVED				22		/* Received M6 */
#define	STATUS_WSC_EAP_M7_SENT					23		/* Send M7 */
#define	STATUS_WSC_EAP_M7_RECEIVED				24		/* Received M7 */
#define	STATUS_WSC_EAP_M8_SENT					25		/* Send M8 */
#define	STATUS_WSC_EAP_M8_RECEIVED				26		/* Received M8 */
#define	STATUS_WSC_EAP_RAP_RSP_ACK				27		/* Processing EAP Response (ACK) */
#define	STATUS_WSC_EAP_RAP_REQ_DONE_SENT		28		/* Processing EAP Request (Done) */
#define	STATUS_WSC_EAP_RAP_RSP_DONE_SENT		29		/* Processing EAP Response (Done) */
#define STATUS_WSC_EAP_FAIL_SENT                30      /* Sending EAP-Fail */
#define STATUS_WSC_ERROR_HASH_FAIL              31      /* WSC_ERROR_HASH_FAIL */
#define STATUS_WSC_ERROR_HMAC_FAIL              32      /* WSC_ERROR_HMAC_FAIL */
#define STATUS_WSC_ERROR_DEV_PWD_AUTH_FAIL      33      /* WSC_ERROR_DEV_PWD_AUTH_FAIL */
#define STATUS_WSC_CONFIGURED					34
#define	STATUS_WSC_SCAN_AP						35		/* Scanning AP */
#define	STATUS_WSC_EAPOL_START_SENT				36
#define STATUS_WSC_EAP_RSP_DONE_SENT			37
#define STATUS_WSC_WAIT_PIN_CODE                38
#define STATUS_WSC_START_ASSOC					39
#define STATUS_WSC_IBSS_WAIT_NEXT_SMPBC_ENROLLEE	40
#define STATUS_WSC_IBSS_NEW_RANDOM_PIN			41
#define STATUS_WSC_IBSS_FIXED_PIN				42

/* All error message dtarting from 0x0100 */
#define	STATUS_WSC_PBC_TOO_MANY_AP				0x0101		/* Too many PBC AP avaliable */
#define	STATUS_WSC_PBC_NO_AP					0x0102		/* No PBC AP avaliable */
#define	STATUS_WSC_EAP_FAIL_RECEIVED			0x0103		/* Received EAP-FAIL */
#define	STATUS_WSC_EAP_NONCE_MISMATCH			0x0104		/* Receive EAP with wrong NONCE */
#define	STATUS_WSC_EAP_INVALID_DATA				0x0105		/* Receive EAP without integrity (Hmac mismatch) */
#define STATUS_WSC_PASSWORD_MISMATCH			0x0106		/* Error PIN Code (R-Hash mismatch) */
#define	STATUS_WSC_EAP_REQ_WRONG_SMI			0x0107		/* Receive EAP-Req with wrong WPS SMI Vendor Id */
#define	STATUS_WSC_EAP_REQ_WRONG_VENDOR_TYPE	0x0108		/* Receive EAPReq with wrong WPS Vendor Type */
#define	STATUS_WSC_PBC_SESSION_OVERLAP			0x0109		/* AP PBC session overlap */
#define	STATUS_WSC_SMPBC_TOO_MANY_REGISTRAR		0x010a		/* Too many SMPBC Registrars avaliable */
#define STATUS_WSC_EMPTY_IPV4_SUBMASK_LIST		0x010b		/* Empty available IPv4 Submask list */
#define	STATUS_WSC_SMPBC_NO_AP					0x010c	

#ifndef SIOCDEVPRIVATE
#define SIOCDEVPRIVATE                              0x8BE0
#endif
#define SIOCIWFIRSTPRIV								SIOCDEVPRIVATE

#define RT_OID_WSC_QUERY_STATUS						0x0751

#define RT_PRIV_IOCTL							(SIOCIWFIRSTPRIV + 0x01)
#define RTPRIV_IOCTL_SET					    (SIOCIWFIRSTPRIV + 0x02)

typedef struct _NDIS_802_11_SSID {
	unsigned int SsidLength;	/* length of SSID field below, in bytes; */
	/* this can be zero. */
	char Ssid[32];	/* SSID information field */
} NDIS_802_11_SSID, *PNDIS_802_11_SSID;
				
typedef struct _WSC_CREDENTIAL {
	NDIS_802_11_SSID SSID;	/* mandatory */
	unsigned short AuthType;	/* mandatory, 1: open, 2: wpa-psk, 4: shared, 8:wpa, 0x10: wpa2, 0x20: wpa2-psk */
	unsigned short EncrType;	/* mandatory, 1: none, 2: wep, 4: tkip, 8: aes */
	unsigned char Key[64 + 1];		/* mandatory, Maximum 64 byte */
	unsigned short KeyLength;
	unsigned char MacAddr[6];	/* mandatory, AP MAC address */
	unsigned char KeyIndex;		/* optional, default is 1 */
	unsigned char bFromUPnP;	/* TRUE: This credential is from external UPnP registrar */
	unsigned char Rsvd[2];		/* Make alignment */
} WSC_CREDENTIAL, *PWSC_CREDENTIAL;
#define APCLI_AUTH_TYPE_OPEN		1
#define APCLI_AUTH_TYPE_WPAPSK		2
#define APCLI_AUTH_TYPE_SHARED		4
#define APCLI_AUTH_TYPE_WPA			8
#define APCLI_AUTH_TYPE_WPA2		16
#define APCLI_AUTH_TYPE_WPA2PAK		32

#define APCLI_ENCRYP_TYPE_NONE		1
#define APCLI_ENCRYP_TYPE_WEP		2
#define APCLI_ENCRYP_TYPE_TKIP		4
#define APCLI_ENCRYP_TYPE_AES		8

/* WSC configured profiles */
typedef struct _WSC_PROFILE {
	unsigned int ProfileCnt;
	unsigned int ApplyProfileIdx;	/* add by johnli, fix WPS test plan 5.1.1 */
	WSC_CREDENTIAL Profile[8];	/* Support up to 8 profiles */
} WSC_PROFILE, *PWSC_PROFILE;

typedef struct _UPLINKAP_INFO {
    unsigned char Channel; /*上行ap的信道*/
    WSC_PROFILE WscProfile;/*ssid,加密方式，认证方式，密钥等*/
} UPLINKAP_INFO, *PUPLINKAP_INFO;

/*当用wps和上行ap协商成功后，repeater的下行接口要同步上行的ssid,加密方式以及密钥*/
#define RT_OID_WSC_UPLINK_AP_INFO                   0x764
#define RT_OID_WSC_UPLINK_CONN_STATUS				0x767

static bcm_timer_id timerId_apcli_wps = BCM_TIMER_ID_INIT;


#endif /* end ifdef CONFIG_AP_REPEATER_SUPPORT */

typedef int bool;

#define TRUE (1)
#define FALSE (0)



static int sig_quit = -1;
static int pc_monitor_count = 0;
bool rc_restart_flag = FALSE;//flag use for if rc restart
static bcm_timer_id ppp_id = BCM_TIMER_ID_INIT;//detect定时
static bcm_timer_module_id http_timer = 0;
static bcm_timer_id timerId_wlan = BCM_TIMER_ID_INIT;
static bcm_timer_id timerId_ap_wps = BCM_TIMER_ID_INIT;
static bcm_timer_id timerId_wireless_detect = BCM_TIMER_ID_INIT;
static int wpsTimeStart = 0;

//#define TIMER_DEBUG 1
#ifdef TIMER_DEBUG
#define TIMER_TRACE(fmt, args...) \
	do {  \
	     printf("%s:%4d %20s: " fmt, __FILE__, __LINE__, __FUNCTION__, ##args); \
	} while(0)
	
#else
#define TIMER_TRACE(fmt, args...)
#endif

#ifdef CONFIG_FOTA

static void fota_monitor(bcm_timer_id id, int flag);

#endif

extern int _vstrsep(char *buf, const char *sep, ...);
#define vstrsep(buf, sep, args...) _vstrsep(buf, sep, args, NULL)

/*************************************************************************
功能: set RC WAN ModeType
参数: 
返回: 
备注:
*************************************************************************/
void setRCWANModeType(int wanType)
{
    char ptmp[64] = {0};
    int ModeTypeData = 0;
    
    memcpy(ptmp,nvram_safe_get("RC_ModeType"),63);
    if(strlen(ptmp))
    {
        ModeTypeData = atoi(ptmp);
    }
    ModeTypeData = ModeTypeData | (1 << wanType);
    sprintf(ptmp, "%d", ModeTypeData);
    nvram_safe_set("RC_ModeType", ptmp);
}


/*************************************************************************
功能: 
参数: 
返回: 
备注:
*************************************************************************/
int tw_self_healing(int id, int flag)
{
	char CurrentDay[ 50 ] ={0}; 
	char *e;
    const char *szTime = NULL;
	unsigned int szTimeHour = 0, szTimeMin = 0;
	struct tm *t;
	time_t tt;
	
	if( !strcmp(nvram_safe_get("self_healing_enable") ,"1")) // test
	//if( !strcmp(websGetVar(wp, "self_healing_enable", ""),"1" ))
	{
		//alarm(60);//设置定时器
		time(&tt);
		t = localtime(&tt);
		TIMER_TRACE("localtime:%d %02d:%02d\n",t->tm_wday,t->tm_hour,t->tm_min); 
		//获取当前时间和星期
		if(t->tm_wday == 1)
		{strcpy(CurrentDay,"Mon");}
		else if(t->tm_wday == 2)
		{strcpy(CurrentDay,"Tue");}
		else if(t->tm_wday == 3)
		{strcpy(CurrentDay,"Wed");}
		else if(t->tm_wday == 4)
		{strcpy(CurrentDay,"Thu");}
		else if(t->tm_wday == 5)
		{strcpy(CurrentDay,"Fri");}
		else if(t->tm_wday == 6)
		{strcpy(CurrentDay,"Sat");}
		else if(t->tm_wday == 7)
		{strcpy(CurrentDay,"Sun");}  

		szTime=nvram_safe_get("self_healing_time");// test
		//szTime=websGetVar(wp,"self_healing_time","");
		szTimeHour=strtoul(szTime, &e, 10);
		szTimeMin=atoi(e+1);
		TIMER_TRACE("szTimeHour = %02d szTimeMin=%02d \n",szTimeHour,szTimeMin);
		//(12:00 -1)-> 12:00-> (12:00 + 1)精度为1分钟
		if( strstr(nvram_safe_get("self_healing_day"),CurrentDay) && ((szTimeHour*60+szTimeMin - 1<= t->tm_hour*60+t->tm_min ) && (t->tm_hour*60+t->tm_min <= szTimeHour*60+szTimeMin + 1) ) ) //test
		//if( strstr(websGetVar(wp, "self_healing_day", ""),CurrentDay) && ((szTimeHour*60+szTimeMin - 1<= t->tm_hour*60+t->tm_min ) && (t->tm_hour*60+t->tm_min <= szTimeHour*60+szTimeMin + 1) ) )
		{
			printf("self healing reboot......\n");
			TIMER_TRACE("now time:tm_hour = %02d tm_min=%02d self_healingtime:szTimeHour = %02d szTimeMin=%02d\n",t->tm_hour,t->tm_min,szTimeHour,szTimeMin);
			kill(1, SIGTERM);//reboot
		}
		
 	}
	
	return 0;
}
/*************************************************************************
功能: 
参数: 
返回: 
备注:
*************************************************************************/
int tw_wlan_time(int id, int flag)
{
	char CurrentDay[ 50 ] ={0}; 
	char *e;
    const char *szStartTime = NULL;
    const char *szEndTime = NULL;
    unsigned int szStartTimeHour = 0,szStartTimeMin = 0;
	unsigned int szEndTimeHour = 0,szEndTimeMin = 0;
	struct tm *t;
	time_t tt;
	
	if( !strcmp(nvram_safe_get("wlan_time_enable") ,"1")) // test
	//if( !strcmp(websGetVar(wp, "self_healing_enable", ""),"1" ))
	{
		//alarm(60);//设置定时器
		time(&tt);
		t = localtime(&tt);
		TIMER_TRACE("localtime:%d %02d:%02d\n",t->tm_wday,t->tm_hour,t->tm_min); 
		//获取当前时间和星期
		if(t->tm_wday == 1)
		{strcpy(CurrentDay,"Mon");}
		else if(t->tm_wday == 2)
		{strcpy(CurrentDay,"Tue");}
		else if(t->tm_wday == 3)
		{strcpy(CurrentDay,"Wed");}
		else if(t->tm_wday == 4)
		{strcpy(CurrentDay,"Thu");}
		else if(t->tm_wday == 5)
		{strcpy(CurrentDay,"Fri");}
		else if(t->tm_wday == 6)
		{strcpy(CurrentDay,"Sat");}
		else if(t->tm_wday == 7)
		{strcpy(CurrentDay,"Sun");}  

		szStartTime=nvram_safe_get("wlan_start_time");// test
		//szTime=websGetVar(wp,"self_healing_time","");
		szStartTimeHour=strtoul(szStartTime, &e, 10);
		szStartTimeMin=atoi(e+1);
		TIMER_TRACE("szStartTimeHour = %02d szStartTimeMin=%02d \n",szStartTimeHour,szStartTimeMin);

		szEndTime=nvram_safe_get("wlan_end_time");// test
		//szTime=websGetVar(wp,"self_healing_time","");
		szEndTimeHour=strtoul(szEndTime, &e, 10);
		szEndTimeMin=atoi(e+1);
		TIMER_TRACE("szEndTimeHour = %02d szEndTimeMin=%02d \n",szEndTimeHour,szEndTimeMin);
		if( strstr(nvram_safe_get("wlan_day_time"),CurrentDay) && ((szStartTimeHour*60+szStartTimeMin) <= (t->tm_hour*60+t->tm_min) && (t->tm_hour*60+t->tm_min)<(szEndTimeHour*60+szEndTimeMin)) ) //test
		//if(  strstr(websGetVar(wp, "self_healing_day", ""),CurrentDay) && ((szStartTimeHour*60+szStartTimeMin) <= (t->tm_hour*60+t->tm_min) && (t->tm_hour*60+t->tm_min)<(szEndTimeHour*60+szEndTimeMin))  )
		{
			TIMER_TRACE("Now time:%02d:%02d\n",t->tm_hour,t->tm_min);
			if( !strcmp(nvram_safe_get("wl0_radio") ,"1") || !strcmp(nvram_safe_get("wl1_radio") ,"1")) // 2G,5G
			{
				TIMER_TRACE("wifi down......\n");
				nvram_safe_set("wl0_radio","0");
				nvram_safe_set("wl1_radio","0");
				nvram_commit(RT2860_NVRAM);				
				kill(1, SIGHUP);//rc restart
			}
		}
		else
		{
			if( !strcmp(nvram_safe_get("wl0_radio") ,"0") || !strcmp(nvram_safe_get("wl1_radio") ,"0")) // 2G,5G
			{
				TIMER_TRACE("wifi up......\n");
				nvram_safe_set("wl0_radio","1");
				nvram_safe_set("wl1_radio","1");
				nvram_commit(RT2860_NVRAM);
				kill(1, SIGHUP);//rc restart
			}
		}

 	}
	
	return 0;
}
/*************************************************************************
功能: 注册的超时处理函数
参数: 无
返回: 状态码
*************************************************************************/
static void tw_self_healing_callback(bcm_timer_id id, int flag)
{	
	TIMER_TRACE("self_healing timer come:id=%d flag=%d\n",id,flag);
	if( strcmp(nvram_safe_get("self_healing_enable") ,"1") ) 
	{
		TIMER_TRACE("stop self_healing timer:id=%d flag=%d\n",id,flag);
		bcm_timer_delete(id);
	}
	else
	{
		tw_self_healing(id,flag);
	}
}
/*************************************************************************
功能: 注册的超时处理函数
参数: 无
返回: 状态码
*************************************************************************/
static void tw_wlan_callback(bcm_timer_id id, int flag)
{	
	TIMER_TRACE("wlan timer come:id=%d flag=%d\n",id,flag);
	if( strcmp(nvram_safe_get("wlan_time_enable") ,"1") ) // test
	{
		TIMER_TRACE("stop wlan timer:id=%d flag=%d\n",id,flag);
		bcm_timer_delete(id);
	}
	else
	{
		tw_wlan_time(id,flag);
	}
}
/*************************************************************************
功能: 注册的超时处理函数
参数: 无
返回: 状态码
*************************************************************************/
static void eth_wan_detect_callback(bcm_timer_id id, int flag)
{	
	char cmd[100] = {0};
	char result[]= "0x0000";
	int portregstate = 0;
	FILE   *stream;
	int portNum = -1;

	//根据wan PORT定义，动态判断需要监测的网口
	if(0 != strchr(nvram_safe_get("vlan2ports"),'5')&& strlen(nvram_safe_get("vlan2ports")) > 2){
		sscanf(nvram_safe_get("vlan2ports"),"%d 5",&portNum);
		//printf("get wan Port num %d\n",portNum);
		sprintf(cmd , "et robord 0x1%d 0x22", portNum);//read port4(wan) status		
		
		
	}else{//vlanports 异常
		//printf("not router...\n");
		return;
	}
	
	stream = popen( cmd, "r" );
	if (!stream)
	{
		pclose( stream );
		return;
	}
	fgets(result, sizeof(result), stream);
	pclose(stream);
	portregstate = strtol( result+2,NULL,16 );	
	//第一次启动时,不重启rc。这里只是获取wan状态。
	//1:加快启动速度2:防止dhcp获取不到地址
	if(strcmp("2",nvram_safe_get("wan_phylink")) == 0)
	{
		 if((portregstate & 0x0100) == 0x0000)
		 	nvram_safe_set("wan_phylink","0");
		 else if((portregstate & 0x0100) == 0x0100)
		 	nvram_safe_set("wan_phylink","1");

		 return;
	}
	//8bit LINK_STS : 1=link pass  0=link fail
	if( (portregstate & 0x0100) == 0x0000 && !strcmp( nvram_safe_get("wan_phylink"),"1" )) // status change(link pass)
	{
		printf("wan phylink change: 1->0......\n");//直接输出到串口提示
		/*断线后清除配置参数*/
		/*rc stop 时pppoe会发送SIGHUP清除配置,而dhcp发送的SIGUSR2信号本来会
		清除配置的，但是确执行失败.原因还是不明确*/
		//注释一下做法，是因为不能紧是简单的清楚页面参数
	
		nvram_safe_set("wan_phylink","0");//中间参数,不用commit到flash
		setRCWANModeType(ETH_WAN_TYPE);
		kill(1, SIGHUP);//rc restart,重新生效配置
		
	}
	else if( (portregstate & 0x0100) == 0x0100 && !strcmp( nvram_safe_get("wan_phylink"),"0" ) ) // status change(link fail)
	{
		printf("wan phylink change: 0->1......\n");
		nvram_safe_set("wan_phylink","1");
		setRCWANModeType(ETH_WAN_TYPE);
		kill(1, SIGHUP);
	}
}


/*************************************************************************
功能: 注册的超时处理函数
参数: 无
返回: 状态码
*************************************************************************/
static void usb_wan_detect_callback(bcm_timer_id id, int flag)
{
	char *usb_wan_port = NULL;

	usb_wan_port = nvram_safe_get("usb_wan0_modem_data_itf");

	if(strcmp("2",nvram_safe_get("usb_wan_link")) == 0)
	{
		if(NULL != strstr(usb_wan_port, "tty"))
		{
			nvram_safe_set("usb_wan_link", "1");
		}
		else
		{
			nvram_safe_set("usb_wan_link", "0");
		}
		setRCWANModeType(USB_WAN_TYPE);
		kill(1, SIGHUP);
	}

	if(strlen(usb_wan_port) > 0)
	{
		if( NULL != strstr(usb_wan_port, "tty") &&  !strcmp( nvram_safe_get("usb_wan_link"),"0" ))
		{
			printf("usb wan change: 0->1......\n");
			nvram_safe_set("usb_wan_link", "1");
			setRCWANModeType(USB_WAN_TYPE);
			kill(1, SIGHUP);
		}

	}
	else if(strlen(usb_wan_port) == 0)
	{
		if( !strcmp( nvram_safe_get("usb_wan_link"),"1" ) )
		{
			printf("usb wan change: 1->0......\n");
			nvram_safe_set("usb_wan_link", "0");
			setRCWANModeType(USB_WAN_TYPE);
			kill(1, SIGHUP);
		}
	}
	
	return;
}

/*************************************************************************
功能: 注册的超时处理函数
参数: 无
返回: 状态码
*************************************************************************/
static void wan_detect_callback(bcm_timer_id id, int flag)
{	
	char *wan_count = NULL;
	char *wan_en = NULL;
	char tmp[100], prefix[] = "wanXXXXXXXXXX_";
	int unit = 0;
	
	//liuchao. 用于smartbits测试时使用. 将此值置为0， 将退出WAN口状态检测
	if(strcmp("0",nvram_safe_get("wan_detect")) == 0)
	{
		return;
	}

//eth wan detect
	wan_count = nvram_safe_get("wan_wan_count");
	if(strlen(wan_count) < 0 || atoi(wan_count) > 8)
	{
		printf("invalid eth wan count %s\n", wan_count);
		return ;
	}

	for(unit = 0; unit < atoi(wan_count); unit++)	
	{
		snprintf(prefix, sizeof(prefix), "wan%d_", unit);
		if(!strcmp(nvram_safe_get(strcat_r(prefix, "enable", tmp)), "1"))
		{
			eth_wan_detect_callback(id, flag);
			break;
		}
	}
//usb wan detect
	wan_count = nvram_safe_get("usb_wan_count");
	if(strlen(wan_count) < 0 || atoi(wan_count) > 8)
	{
		printf("invalid usb wan count %s\n", wan_count);
		return ;
	}

	for(unit = 0; unit < atoi(wan_count); unit++)	
	{
		snprintf(prefix, sizeof(prefix), "usb_wan%d_", unit);
		if(!strcmp(nvram_safe_get(strcat_r(prefix, "enable", tmp)), "1"))
		{
			usb_wan_detect_callback(id, flag);
			break;
		}
	}
//adsl wan detect

	return;
	
}

static void pcmonitor_callback(bcm_timer_id id, int flag)
{	
	 pc_monitor_count++; 
	 if ((!strcmp(nvram_safe_get("wan_proto"), "pppoe") && !strcmp(nvram_safe_get("pppoe"), "start"))
	 	||(!strcmp(nvram_safe_get("wan_proto"), "pptp") && !strcmp(nvram_safe_get("pptp"), "start"))
		||(!strcmp(nvram_safe_get("wan_proto"), "l2tp") && !strcmp(nvram_safe_get("l2tp"), "start")))
	 {
		/* launch pc_monitor(shell srcipt) daemon monitor the pc_daemon process */
		if(pc_monitor_count >= 3 ) /*pc_monitor_count*3 = 6,6秒检查一次*/
		{
    		system("/sbin/pc_monitor pc_daemon");
			pc_monitor_count = 0;
		}	

     }

}

//添加扩展检测功能
static void detect_callback(bcm_timer_id id, int flag)
{
	//每次rc重启完成后，再进行下次检测动作。防止拔插网线进程异常
	//引起rc退出
	//if(strcmp("start_finish",nvram_safe_get("rc_status")) == 0)
	if(wait_action_idle(1) == 1)
	{
		//wan端口检测
		wan_detect_callback(id,flag);
	}
	//pc_daemon监控进程检测
	pcmonitor_callback(id,flag);

}
#if 0
int detect_process(char * process_name)   
{   
	FILE *ptr;
	char buff[512];
	char ps[128];
	char *pGrep=NULL;
	sprintf(ps,"ps | grep %s",process_name);
	if((ptr=popen(ps, "r")) != NULL)
	{
		while (fgets(buff, 512, ptr) != NULL)
		{
			//printf("buff=[%s] atoi(buff) = %d\n", buff, atoi(buff));
			if(atoi(buff)!=0)
			{
				pGrep=NULL;
				pGrep = strstr(buff, "grep");
				//printf("pGrep=0x%p pGrep=[%s]\n", pGrep, pGrep);
				if(pGrep==NULL){
					pclose(ptr);
					return 1;
				}
			}
		}
	}
	
	pclose(ptr);   
	return 0;
}
#endif
/*
手动拨号思路:
在触发按钮的时候，连接按钮，要将它wan_wan0_xxx_manual_action置位
而在disconnect的时候要清0，在wan口拔掉的时候要清0，在wan_up的时候
要清零。
*/
static void ppp_callback(bcm_timer_id id, int flag)
{
	//每次rc重启完成后，再进行下次检测动作。防止拔插网线进程异常
	//引起rc退出
	char *szProto = NULL;
	int manual = 0;
	
	//if(strcmp("start_finish",nvram_safe_get("rc_status")) != 0 || nvram_invmatch("wan_phylink","1" ))
	if(0 == wait_action_idle(1) || nvram_invmatch("wan_phylink","1" ))
	{
		return;
	}
#ifdef CONFIG_AP_REPEATER_SUPPORT
	char *workmode = nvram_safe_get("WorkMode");
	if(strcmp(workmode,WIRELESS_REPEATER) == 0 || strcmp(workmode,WIRELESS_AP) == 0 )
	{
		return;
	}
#endif

	if (0 == strcmp(nvram_safe_get("IsInConfiguring"), "1"))
	{
		/*
		  * WAN口配置采用先stop, 再start, 如果timer的检测
		  * 动作在两者中间发生,则会多了一个restart.
		  * 从而多一个进程,因此配置期间不检测
		  * 2017-03-23 --liushenghui
		*/
		cprintf("Configuring is underway, so skip the detection for the moment.\n");
		return;
	}

	if((!strcmp(nvram_safe_get("wan_wan0_proto"), "pppoe"))||((!strcmp(nvram_safe_get("wan_wan0_pppoe_DualAccessEn"), "1"))&&(!strcmp(nvram_safe_get("wan_wan0_proto"), "rupppoe"))))//
	{
		if((!strcmp(nvram_safe_get("wan_wan0_pppoe_demand"),"0"))&&(!strcmp(nvram_safe_get("wan_wan0_pppoe_autoreconn"),"false"))&&(!strcmp(nvram_safe_get("wan_wan0_pppoe_manual_action"),"0")))
			return;
		if(detect_process("/usr/sbin/pppd") == 0)
		{
			cprintf("====>pppoe");
			goto WAN_RESTART;
		}
		else
		{
			return;
		}
	}
	else if (!strcmp(nvram_safe_get("wan_wan0_proto"), "pptp"))//
	{
		//printf("pptp:%s %s %d %d\n",nvram_safe_get("rc_status"),nvram_safe_get("wan_phylink"),detect_process("/usr/sbin/pptp"),detect_process("/usr/sbin/pppd"));
		if((!strcmp(nvram_safe_get("wan_wan0_vpn_demand"),"0"))&&(!strcmp(nvram_safe_get("wan_wan0_vpn_autoreconn"),"false"))&&(!strcmp(nvram_safe_get("wan_wan0_vpn_manual_action"),"0")))
			return;
		if(/*detect_process("/usr/sbin/pptp") == 0 || */detect_process("pppd") == 0)//进程退出监控
		{
			cprintf("=====>pptp");
			goto WAN_RESTART;
		}
		else
		{
			return;
		}
 	}
	else if (!strcmp(nvram_safe_get("wan_wan0_proto"), "l2tp"))
	{
			//手动拨号时，当没有触发时，不要监控
		if((!strcmp(nvram_safe_get("wan_wan0_vpn_demand"),"0"))&&(!strcmp(nvram_safe_get("wan_wan0_vpn_autoreconn"),"false"))&&(!strcmp(nvram_safe_get("wan_wan0_vpn_manual_action"),"0")))
			return;
		if(detect_process("openl2tpd") == 0 || detect_process("pppd") == 0)
		{
			 cprintf("====>l2tp");
			 goto WAN_RESTART;
 		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}
WAN_RESTART:
	cprintf("->wan restart...\n");//XXX->wan 组合起来输出,调试参数以及代码逻辑错误导致rc不断重启
	setRCWANModeType(ETH_WAN_TYPE);
	//kill(1, SIGHUP);//rc restart,重新生效配置
	notify_rc("restart_wan");
	
}


/* 家长控制之日程计划 */
int tw_check_parental_control_time(char *day,char *start_time ,char *end_time)
{
	char CurrentDay[ 50 ] ={0}; 
	char *e;
    const char *szStartTime = NULL;
    const char *szEndTime = NULL;
    unsigned int szStartTimeHour = 0,szStartTimeMin = 0;
	unsigned int szEndTimeHour = 0,szEndTimeMin = 0;
	struct tm *t;
	time_t tt;
	
	if( !strcmp(nvram_safe_get("parent_control_enable") ,"1")) 
	{
		time(&tt);
		t = localtime(&tt);
		TIMER_TRACE("localtime:%d %02d:%02d\n",t->tm_wday,t->tm_hour,t->tm_min); 
		//获取当前时间和星期
		if(t->tm_wday == 1)
		{strcpy(CurrentDay,"Mon");}
		else if(t->tm_wday == 2)
		{strcpy(CurrentDay,"Tue");}
		else if(t->tm_wday == 3)
		{strcpy(CurrentDay,"Wed");}
		else if(t->tm_wday == 4)
		{strcpy(CurrentDay,"Thu");}
		else if(t->tm_wday == 5)
		{strcpy(CurrentDay,"Fri");}
		else if(t->tm_wday == 6)
		{strcpy(CurrentDay,"Sat");}
		else if(t->tm_wday == 7)
		{strcpy(CurrentDay,"Sun");}  

		//szStartTime=nvram_safe_get("parental_start_time");
		szStartTime = start_time;
		szStartTimeHour = strtoul(szStartTime, &e, 10);
		szStartTimeMin = atoi(e+1);
		TIMER_TRACE("szStartTimeHour = %02d szStartTimeMin=%02d \n",szStartTimeHour,szStartTimeMin);

		//szEndTime=nvram_safe_get("parental_end_time");
		szEndTime = end_time;
		szEndTimeHour = strtoul(szEndTime, &e, 10);
		szEndTimeMin = atoi(e+1);
		TIMER_TRACE("szEndTimeHour = %02d szEndTimeMin=%02d \n",szEndTimeHour,szEndTimeMin);
		if( strstr(day,CurrentDay) && ((szStartTimeHour*60+szStartTimeMin) <= (t->tm_hour*60+t->tm_min) && (t->tm_hour*60+t->tm_min)<(szEndTimeHour*60+szEndTimeMin)) ) 
		{
			TIMER_TRACE("start parent control schedule ...\n");
			if(!strcmp(nvram_safe_get("schedule_enable"),"0"))
			{
				nvram_safe_set("schedule_enable","1");
				nvram_commit(RT2860_NVRAM);
				rc_restart_flag = TRUE;
			}	
		}
		else
		{
			TIMER_TRACE("end parent control schedule ...\n");
			if(!strcmp(nvram_safe_get("schedule_enable"),"1"))
			{
				nvram_safe_set("schedule_enable","0");
				nvram_commit(RT2860_NVRAM);
				rc_restart_flag = TRUE;
			}	
		}
		



 	}
	
	return 0;
}




void parse_parental_control_rules()
{
	char *buf;
	char *g;
	char *p;
	int i;
	char *mac = NULL;
	char *web_disc = NULL,*schedule_disc = NULL;
	char *key1 = NULL,*key2 = NULL,*key3 = NULL,*key4 = NULL;
	char *key5 = NULL,*key6 = NULL,*key7 = NULL,*key8 = NULL;
	char *parental_day = NULL,*parental_start_time = NULL,*parental_end_time = NULL;
	char *parental_enable = NULL;
	char *parent_rules = NULL;
  
    /*
    rules格式合法性检查
    parent_rules="mac<disc1<key1<key2<key3<key4<key5<key6<key7<key8<disc2<Sun,Mon,Tue,Wed,Thu,Fri,Sat<hh:mm<hh:mm<enable/disable"
    */
	parent_rules = nvram_safe_get("parent_rules");
	if(!parent_rules)
		return;
	
	g = buf = strdup(parent_rules);
	while (g) {
		
		if ((p = strsep(&g, ">")) == NULL)
		{
			break;
		}	
		i = vstrsep(p, "<", &mac, &web_disc, &key1, &key2, &key3, &key4,\
			 &key5, &key6, &key7, &key8, &schedule_disc, &parental_day, &parental_start_time ,&parental_end_time ,&parental_enable ,&p);
				
		if(i != 15)
			continue;


		tw_check_parental_control_time(parental_day,parental_start_time,parental_end_time);

#if 0  //由于ap的时间未同步正确，为方便调试从nvram读值判断
	   char *tmp = NULL;	
	   tmp = nvram_safe_get("restart_flag");
	   if(!strcmp(tmp,"true"))	
	   		rc_restart_flag = TRUE;
	   else 
	   		rc_restart_flag = FALSE;
#endif		
		if(rc_restart_flag)
			break;
	
	}
	free(buf);

	
	//重启rc执行日程计划
	if(rc_restart_flag)
	{
		rc_restart_flag = FALSE;
		kill(1, SIGHUP);
	}	
	
 
}




static void tw_parental_control_callback(bcm_timer_id id, int flag)
{
	
	if( strcmp(nvram_safe_get("parent_control_enable") ,"1") )
	{
		TIMER_TRACE("stop parental control timer:id=%d flag=%d\n",id,flag);
		bcm_timer_delete(id);
	}
	else
	{
		parse_parental_control_rules();
	}	
}

#ifndef	CONFIG_DEFAULTS_MEDIATEK_DIR_878_CA
static void tw_GetOnlineFWVer_callback(bcm_timer_id id, int flag)
{
    notify_rc("start_get_online_fw_version");
}
#endif

static int doWanLedControl = 1;
static int wpsJustOk = 0;


static void tw_WanLedControl_callback(bcm_timer_id id, int flag)
{
    static int g_dCnt = 0;
#ifdef __CONFIG_IPV6__	
	char tmp[100], prefix[] = "XXXXXXXXXX_wanXXXXXXXXXX_";
	int service;
	char *wan6_conn_status = NULL;	
	char *szfp6 = NULL;
	char szIpv6Buf[64] = {0};
#endif
	char *wan4_conn_proto = NULL;
	char *wan4_demand_dial = NULL;
	char *wan4_manual_dial = NULL;
	char *wan4_conn_status = NULL;
	char *szfp = NULL;
	char szIpv4Buf[64] = {0};
	int internet_access = 0;
	char *szLedEnable = NULL;
	char szCmd[128] = {0};
	szLedEnable = nvram_safe_get("LEDStatusEnable");

	if(0 == access("/tmp/ban_led", F_OK))//factory test mode
		return;

    if (doWanLedControl)
    {
#ifdef CONFIG_AP_REPEATER_SUPPORT	
		char *workmode = nvram_safe_get("WorkMode");
		if(strcmp(workmode,WIRELESS_REPEATER) == 0 || strcmp(workmode,WIRELESS_AP) == 0 )
		{
			/*正在WPS,不改变灯位状态*/
			if(nvram_get_int("WscStatus") == 1)
				return;

			if( strcmp(nvram_safe_get("wan_phylink"), "1") == 0 || strcmp(workmode,WIRELESS_REPEATER) == 0)//repeater or phy link up
			{
#ifdef __CONFIG_IPV6__    
				service = ipv6_wan_getconntype();
				if (service != IPV6_DISABLED && (ipv6_wan_getpathprefix(prefix) >= 0))
					wan6_conn_status = nvram_safe_get(ipv6_nvname(WAN_NAME_IPV6_NETWORK_STATUS));
				else
					wan6_conn_status = NETWORK_STATUS_DISCONNECTED;
#endif	
				wan4_conn_proto = nvram_safe_get("lan0_ap_proto");
				wan4_conn_status = nvram_safe_get("lan0_ap_status");
				if(!strcmp(wan4_conn_status, NETWORK_STATUS_CONNECTED)
#ifdef __CONFIG_IPV6__				
					|| !strcmp(wan6_conn_status, NETWORK_STATUS_CONNECTED) 
#endif
				)
				{
					//Green led Solid
					system("led internet high off -s");
					if(strcmp(szLedEnable, "0"))
						system("led internet on -s");
				}
				else
				{
					if((strlen(nvram_bufget(RT2860_NVRAM, "ApCliSsid")) == 0) &&(strlen(nvram_bufget(RTDEV_NVRAM, "ApCliSsid")) == 0))
					{
						system("led internet high off -s");
						system("led internet off -s");
					}
					else{
						//Orange led Solid
						system("led internet off -s");
						system("led internet high on -s");
					}
				}
			}
			else
			{
				//wan phylink disconnect,close wan led
				system("led internet high off -s");
				system("led internet off -s");
			}
			return ;
		}
		else if(strcmp(workmode,WIRELESS_ROUTER) == 0)
		{
			goto RouterLED;
		}
		else
			return;
#endif //end ifdef CONFIG_AP_REPEATER_SUPPORT
		
RouterLED:
    	if(!strcmp(nvram_safe_get("wan_phylink"), "1"))//phy link up
    	{
    	
#ifdef __CONFIG_IPV6__    
			service = ipv6_wan_getconntype();
			if (service != IPV6_DISABLED && (ipv6_wan_getpathprefix(prefix) >= 0))
				wan6_conn_status = nvram_safe_get(ipv6_nvname(WAN_NAME_IPV6_NETWORK_STATUS));
			else
				wan6_conn_status = NETWORK_STATUS_DISCONNECTED;
			if(service == IPV6_MANUAL)
			{
				szfp6 = fopen(INTERNET_IPV6_FILE,"r");
				if(NULL==szfp6)	return;
				fgets(szIpv6Buf,sizeof(szIpv6Buf),szfp6);
				fclose(szfp6);
			}

#endif				
    		wan4_conn_proto = nvram_safe_get("wan_wan0_proto");
			wan4_conn_status = nvram_safe_get("wan_wan0_status");
			if(!strcmp(wan4_conn_proto,"static"))
			{
				szfp = fopen(INTERNET_IPV4_FILE,"r");
				if(NULL==szfp)	return;
				fgets(szIpv4Buf,sizeof(szIpv4Buf),szfp);
				fclose(szfp);
			}
			if((!strcmp(wan4_conn_proto,"pptp"))||(!strcmp(wan4_conn_proto,"l2tp")))
			{
				wan4_demand_dial = nvram_safe_get("wan_wan0_vpn_demand");
				wan4_manual_dial = nvram_safe_get("wan_wan0_vpn_manual_action");
			}
			else if(!strcmp(wan4_conn_proto,"pppoe"))
			{				
				wan4_demand_dial = nvram_safe_get("wan_wan0_pppoe_demand");
				wan4_manual_dial = nvram_safe_get("wan_wan0_pppoe_manual_action");
			}
			else
			{
				wan4_demand_dial = "0";
				wan4_manual_dial = "0";
			}

			if(!strcmp(wan4_demand_dial, "1") || !strcmp(wan4_manual_dial, "1"))//demand manual dial
			{
				if(!strcmp(wan4_conn_status, "CONNECTED")
#ifdef __CONFIG_IPV6__				
				|| !strcmp(wan6_conn_status, NETWORK_STATUS_CONNECTED) 
#endif
				)
				{
					//Green led Solid
					system("led internet high off -s");					
	    			if(strcmp(szLedEnable, "0"))
	            		system("led internet on -s");
				}
				else
				{
					//Orange led Blinking
					system("led internet off -s");
        			system("led internet high blink -s");
				}
			}
			else
			{
				if(!strcmp(wan4_conn_status, "CONNECTED")
#ifdef __CONFIG_IPV6__				
				|| (!strcmp(wan6_conn_status, NETWORK_STATUS_CONNECTED)) 
#endif
				)
				{
					internet_access = 1;
					if((!strcmp(wan4_conn_status,"CONNECTED"))
#ifdef __CONFIG_IPV6__  
						&&(!strcmp(wan6_conn_status, NETWORK_STATUS_CONNECTED))
#endif
						)
					{
						if((strstr(szIpv4Buf,"NoInternet"))
#ifdef __CONFIG_IPV6__  
							&&(strstr(szIpv6Buf,"NoInternet"))
#endif
							&&(!strcmp(wan4_conn_proto,"static"))
#ifdef __CONFIG_IPV6__  
							&&(service == IPV6_MANUAL)
#endif
							)
							internet_access = 0;
						
					}
					else if(!strcmp(wan4_conn_status,"CONNECTED"))
					{
						if((strstr(szIpv4Buf,"NoInternet"))&&(!strcmp(wan4_conn_proto,"static")))
							internet_access = 0;
					}
#ifdef __CONFIG_IPV6__  
					else if(!strcmp(wan6_conn_status, NETWORK_STATUS_CONNECTED))
					{
						if((strstr(szIpv6Buf,"NoInternet"))&&(service == IPV6_MANUAL))
							internet_access = 0;	
					}
#endif
					if(internet_access)
					{
						//Green led Solid
						//Green led Solid for wan on
						system("led internet high off -s");
						if(strcmp(szLedEnable, "0"))
						{
							system("led internet on -s");
						}
					}
					else
					{
						//Orange led Solid for wan off
						system("led internet off -s");
						system("led internet high on -s");						
					}
				}				
				else
				{
					//Orange led Solid
					system("led internet off -s");
					system("led internet high on -s");
				}

			}
		}
		else//phy link down
		{
			system("led internet off -s");
			system("led internet high off -s");
		}
    }
}

static void tw_WlanLedControl_callback(bcm_timer_id id, int flag)
{
	char szCmd[128] = {0};
	//检测2.4g和5g所有接口的开关，只有有一个开，则打开灯位，否则关闭；
	char *szG2_enable = NULL, *szG2_guest_enable = NULL;
	char *szG5_enable = NULL, *szG5_guest_enable = NULL;
	char *szLedEnable = nvram_safe_get("LEDStatusEnable");

	szG2_enable = nvram_get(RT2860_NVRAM, "Radio24On");
	szG2_guest_enable = nvram_get(RT2860_NVRAM, "Radio24Vap1En");
	szG5_enable = nvram_get(RTDEV_NVRAM, "Radio58On");
	szG5_guest_enable = nvram_get(RTDEV_NVRAM, "Radio58Vap1En");

	if(access("/tmp/ban_led", 0)!=0 && access("/tmp/wps_going", 0)!=0)
	{
		if(strcmp(szLedEnable, "0"))
		{
			if(strcmp(szG2_enable, "1") == 0 || strcmp(szG2_guest_enable, "1") == 0)
			{
				if(strcmp(szG2_enable, "1") == 0 && strcmp(szG2_guest_enable, "1") == 0)
				{
					if(access(SCHEDULE_STOP_WLAN_2G, 0)==0 && access(SCHEDULE_STOP_GUEST_2G, 0)==0)
						system("led wifi low off -s");
					else
						system("led wifi low on -s");
				}
				else
				{
					if(access(SCHEDULE_STOP_WLAN_2G, 0)==0 || access(SCHEDULE_STOP_GUEST_2G, 0)==0)
						system("led wifi low off -s");
					else
						system("led wifi low on -s");
				}
			}
			else
			{
				system("led wifi low off -s");
			}
			
			if(strcmp(szG5_enable, "1") == 0 || strcmp(szG5_guest_enable, "1") == 0 )
			{
				if(strcmp(szG5_enable, "1") == 0 && strcmp(szG5_guest_enable, "1") == 0)
				{
					if(access(SCHEDULE_STOP_WLAN_5G, 0)==0 && access(SCHEDULE_STOP_GUEST_5G, 0)==0)
						system("led wifi high off -s");
					else
						system("led wifi high on -s");
				}
				else
				{
					if(access(SCHEDULE_STOP_WLAN_5G, 0)==0 || access(SCHEDULE_STOP_GUEST_5G, 0)==0)
						system("led wifi high off -s");
					else
						system("led wifi high on -s");
				}
			}		
			else
			{
				system("led wifi high off -s");
			}
		}
	}

	return;
}


static int tw_SysLogMail_FullLog()
{
    int iSrcFD = -1;
    char *pszBuf = NULL;
    char *pszPos = NULL;
    int send_log = 0;
    char *p = NULL;

    unsigned long ulRead = 0, ulWriten = 0;
    unsigned long n = 0;
    int logfile_size = 0;
    int max_logfile_size = 0;
    unsigned long erase_len = 0;

    logfile_size = tbsGetFileSize(DEFAULT_SYSLOG_FILE_PATH);
    max_logfile_size = nvram_get_int("SysLog_FILE_MAX_LEN");

    //dprintf("check log=========now:%d   max:%d===========\n", logfile_size, max_logfile_size);

    if(logfile_size < max_logfile_size) {
        return 0;
    }

    //dprintf("====syslog file is full====\n");

    if((iSrcFD = open(DEFAULT_SYSLOG_FILE_PATH, O_RDWR)) < 0) {
        dprintf("Failed to open file(%s), %s\n", DEFAULT_SYSLOG_FILE_PATH, strerror(errno));
        return -1;
    }

    if(tbsFileLock(iSrcFD) < 0) {
        dprintf("Failed to lock file\n");
        goto ERR_LOCK;
    }    

    pszBuf = (char *)malloc(logfile_size + 1);
    if(pszBuf == NULL) {
        dprintf("malloc fail\n");
        goto OUT;
    }

    memset(pszBuf, 0x0, logfile_size + 1);

    if(lseek(iSrcFD, max_logfile_size * (-1), SEEK_END) < 0) {
        dprintf("Failed to lseek file\n");
        goto OUT;
    }    

    pszPos = pszBuf;
    while((n = read(iSrcFD, pszPos, max_logfile_size - ulRead)) > 0) {
        ulRead += n;
        pszPos += n;

        if(ulRead >= max_logfile_size)
            break;
    }

    if(n < 0) {
        dprintf("Failed to read file to buffer\n");
        goto OUT;
    }   

    ftruncate(iSrcFD, 0);
    if(lseek(iSrcFD, 0, SEEK_SET) < 0) {
        dprintf("lseek\n");
        goto OUT;
    }

    if(p = strchr(pszBuf, '\n')) {
        erase_len = (unsigned long)(p - pszBuf) + 1; 
        ulWriten = tbsFileFullWrite(iSrcFD, p + 1, ulRead - erase_len);
        if((ulRead - erase_len) != ulWriten) {
            dprintf("Failed to write back file.\n");
            goto OUT;
        }    
    } else {
        ulWriten = tbsFileFullWrite(iSrcFD, pszBuf, ulRead);
        if(ulRead != ulWriten) {
            dprintf("Failed to write back file.\n");
            goto OUT;
        }    
    }
    
    if(nvram_get_int("SysLogMail_Enable") && nvram_get_int("SysLogMail_LogFull_Enable")) {
        send_log = 1;
        twsystem("echo > /tmp/clear_syslog_file", TRUE);
    }

OUT:    
    if(pszBuf) free(pszBuf);

    tbsFileUnLock(iSrcFD);

ERR_LOCK:
    if(iSrcFD >= 0) {
        close(iSrcFD);
    }

    if(send_log) {
        notify_rc("start_email");
        return 1;
    }

    //dprintf("======syslog file is full and process done======\n");

    return 0;
}

static void tw_SysLogMail_callback()
{

    int send_log = 0;
    static int cnt = 0;

    static int check_log_cnt = 0;

    if(++check_log_cnt % 120 == 0) {
    
        send_log = tw_SysLogMail_FullLog();

        if(++cnt % 60 == 0 
          && send_log != 1
          && nvram_get_int("SysLogMail_Enable")
          && nvram_get_int("SysLogMail_Schedule_Enable") 
          && strcmp(nvram_safe_get("SysLogMail_Schedule_Name"), "") == 0) {
            notify_rc("start_email");
        }

    }

	return;
}





/*************************************************************************
功能: 
参数: 无
返回: 状态码
*************************************************************************/
int tw_timer_start(bcm_timer_module_id module,bcm_timer_id *id, int time, bcm_timer_cb handler, int flag)
{
	struct itimerspec timer;
	int status;
	
	timer.it_interval.tv_sec = time;
	timer.it_interval.tv_nsec = 0;
	timer.it_value.tv_sec = time;
	timer.it_value.tv_nsec = 0;
	
	status = bcm_timer_create(module, (bcm_timer_id *)id);
	if (status) {
		TIMER_TRACE("bcm_timer_create() failed with error %d\n", status);
		return -1;
	}
	status = bcm_timer_connect((bcm_timer_id)*id, handler, flag);
	if (status) {
		printf("bcm_timer_create() failed with error %d\n", status);
		return -1;
	}
	status = bcm_timer_settime((bcm_timer_id)*id, &timer);
	if (status) {
		TIMER_TRACE("bcm_timer_create() failed with error %d\n", status);
		return -1;
	}
	TIMER_TRACE("create timer ok:module=%d id=%d time=%d flag=%d\n",module, *id ,time,flag);
	return 0;
}

/* Signal handling */
static void
sig_handler(int sig)
{
	if (sig == SIGTERM) {
		sig_quit = 1;
	}
}

int writeTimerPid(void)
{
	FILE *fp;
	fp = fopen("/var/run/timer.pid", "wr+");
	if (NULL == fp) {
		//error(E_L, E_LOG, T("timer.c: cannot open pid file"));
		return (-1);
	}
	fprintf(fp, "%d", getpid());
    fclose(fp);
	return 0;
}

static void wan_registerTimer(int sig)
{
	int ppp_time = BCM_TIMER_INTERVAR_PPP_DETECT;//定时4s


	if(sig == PPTP_L2TP_MON_SIG_NUM)
	{
		bcm_timer_delete(ppp_id);
		ppp_id = BCM_TIMER_ID_INIT;
		if( nvram_invmatch("wan_phylink","0" ) )
		{		
			tw_timer_start(http_timer,&ppp_id, ppp_time, (bcm_timer_cb)ppp_callback, HTTP_TIMER_PPP_DETECT);
		}
	}
}

static void wlan_callback(void)
{
	notify_rc("restart_wlan");//restart_wlan
	bcm_timer_delete(timerId_wlan);
}

static void wlan_registerTimer(int sig)
{
	int time_wlan = BCM_TIMER_INTERVAR_WLAN;
	bcm_timer_delete(timerId_wlan);
	tw_timer_start(http_timer,&timerId_wlan, time_wlan, (bcm_timer_cb)wlan_callback, HTTP_TIMER_WLAN);
}

static void wan_handler(int sig)
{
	FILE *szfp = NULL;
	char szbuf[64] = {0};
	char *szProto = NULL;
	int manual = 0;
	int ret = 0;
	int ucount=0;
	
	if (sig == SIGUSR2) {
		szfp = fopen("/proc/mt7621/port_status","r");
		if(!szfp)
		{
			TIMER_TRACE("open file failed\n");
			return;
		}
		while(NULL!=fgets(szbuf,sizeof(szbuf),szfp))
		{
			if(strstr(szbuf,"Port4:"))
			{
				if(strstr(szbuf,"LinkUp"))
				{
#ifdef CONFIG_AP_REPEATER_SUPPORT
					if(strcmp(nvram_safe_get("WorkMode"),WIRELESS_ROUTER))
						twsystem("brctl addif br0 eth3",TRUE);
#endif
				/*
				因为插拔太快，导致写wan_phylink参数太快，信号量被占用，可能会导致失败
				因此在这里，要多尝试几次写操作。
				*/
					ucount = 0;
					while(ucount<3)
					{
						ucount++;
						ret = nvram_safe_set("wan_phylink","1");
						if(ret) {usleep(500);continue;}
						ret = nvram_safe_set("wan_wan0_status","LIMITED_CONNECTION");
						nvram_set_int("wan_conn_uptime", 0);
						if(ret) {usleep(500);continue;}
						break;
						
					}
#ifdef CONFIG_AP_REPEATER_SUPPORT
					if(strcmp(nvram_safe_get("WorkMode"),WIRELESS_ROUTER))
					{
						dprintf("wan_handle==>start_wanall\n");
						notify_rc("start_wanall");
						break;
					}
#endif
					szProto = nvram_safe_get("wan_wan0_proto");
					if((!strcmp(szProto,"l2tp"))||(!strcmp(szProto,"pptp")))
					{
						if((!strcmp(nvram_safe_get("wan_wan0_vpn_idletime"),"0"))&&(!strcmp(nvram_safe_get("wan_wan0_vpn_autoreconn"),"false")))
							manual = 1;
					}
					else if(!strcmp(szProto,"pppoe"))
					{
						if((!strcmp(nvram_safe_get("wan_wan0_pppoe_idletime"),"0"))&&(!strcmp(nvram_safe_get("wan_wan0_pppoe_autoreconn"),"false")))
							manual = 1;
					}
					if(!manual)
					{
						notify_rc("restart_wanall");
					}
				}
				else
				{
					ucount = 0;
					while(ucount<3)
					{
						ucount++;
						nvram_safe_set("wan_wan0_status","DISCONNECTED");
						nvram_set_int("wan_conn_uptime", 0);
						if(ret) {usleep(500);continue;}
						nvram_safe_set("wan_phylink","0");
						if(ret) {usleep(500);continue;}
						break;
						
					}
#ifdef CONFIG_AP_REPEATER_SUPPORT
					if(strcmp(nvram_safe_get("WorkMode"),WIRELESS_ROUTER))
						break;
#endif
					nvram_safe_set("wan_wan0_pppoe_manual_action","0");//手动拨号触发按钮清0
					nvram_safe_set("wan_wan0_vpn_manual_action","0");
					dprintf("wan_handle==>stop_wanall\n");
					notify_rc("stop_wanall");
					//notify_rc("stop_wan6only");
				}
				break;
			}
		}
		fclose(szfp);
	}
	
}

static void
WanLedControl_handler(int sig)
{
	if (sig == SIGUSR1)
    {
        char sCMD[128] = {0};
		doWanLedControl = !doWanLedControl;
        sprintf(sCMD, "echo \"%s WanLedControl in timer\n\" > /dev/console", doWanLedControl ? "Enable":"Disable");
        system(sCMD);
        // use CMD: killall -USR1 timer
	}
}
#ifdef CONFIG_AP_REPEATER_SUPPORT

static void br_fdb_loop_handler(int sig)
{
	dprintf("------br_fdb_loop_handler!-------\n");
	if (sig == LOOPBACK_START)
	{
		twsystem("brctl delif br0 eth3",TRUE);
	}
}

/*************************************************************************
功能:获取上行接口的状态
参数:接口名称
返回:0 --->未连接	1 ---> 已连接
*************************************************************************/
static int RT6352_apclient_get_conn_status(char *interface)
{
	int status = 0;
	int get_status = -1;
	int socket_id = 0;
	struct iwreq wrq;

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	strcpy(wrq.ifr_name, interface);
	wrq.u.data.length = sizeof(status);
	wrq.u.data.pointer = (caddr_t) &status;
	wrq.u.data.flags = RT_OID_WSC_UPLINK_CONN_STATUS;
	if( ioctl(socket_id, RT_PRIV_IOCTL, &wrq) == -1)
	{
		//dprintf("ioctl error\n");
	}
	close(socket_id);
	
	//dprintf("Ralink wps status = %d \n", status);
	if(status >= 0)
	{
		if(status == 0)
			return 0;
		else if(status == 1)
			return 1;
	}
	return -1; 
}
static void MON_CheckAssociate(const char * pszInterfaceName,int * eLinkState)
{
	FILE * fp_if = NULL;
	FILE *fp_ext = NULL;
	char buf[256] = {0};
	char szCmd[256] = {0};

	sprintf(szCmd, "iwconfig %s", pszInterfaceName);
	fp_if = popen(szCmd, "r");
	if(NULL != fp_if)
	{
	    if(fread(buf, 1, 256, fp_if))
		{
	        /* if not link return 0, else return 1*/
		  //if(strstr(buf, "Not-Associated") || strstr(buf, "Invalid") || strstr(buf, "None"))
		    if(strstr(buf, "RTWIFI SoftAP  ESSID:\"\"") && strstr(buf, "Access Point: Not-Associated"))
		    {
			    *eLinkState = 0;
		    }    
		    else 
		    {
		     	/*是否假连接判断*/
			    sprintf(szCmd, "iwpriv %s show assoc", pszInterfaceName);
			    system(szCmd);
				if(strcmp(APCLI_IF_2G,pszInterfaceName) == 0)
					sprintf(szCmd,"/proc/apcli_assoc/iwconfig_assoc_flag_2g");
				else if(strcmp(APCLI_IF_5G,pszInterfaceName) == 0)
		       		sprintf(szCmd,"/proc/apcli_assoc/iwconfig_assoc_flag_5g");
			
				fp_ext = fopen(szCmd, "r");
				if(NULL != fp_ext)
				{
					fread(buf, 1, 256, fp_ext);
					if(0 == atoi(buf)) 
					{
						//如果关联状态为0说明是假连接
						*eLinkState = 0;
					}else if(1 == atoi(buf)){
						//如果关联状态为1说明已连接成功
		      			*eLinkState = 1;
					}
		            fclose(fp_ext);
				}else{
		        	//打开proc目录失败
		            *eLinkState = 0;
		        } 
		    }    
		}
	    pclose(fp_if);
	}
	else
	{
	    *eLinkState = 0;
	}
	return;

}

static void tw_CheckApCliStatus_callback(bcm_timer_id id, int flag)
{
	int statu2 = 0;
	int statu5 = 0;
	int eth_status = 0;
	char *laststatus = NULL;
	FILE *fp_ext = NULL;
	char buf[256] = {0};
	char szCmd[256] = {0};
	int start_flag = 0;
	char *ApCliConnStatus = nvram_safe_get("ApCliConnStatus");
	/*
	连接状态为有线连接和无线连接的综合，只要有其中一个连接上，就需要启动udhcp/odhcp6c
	连接状态变化时，修改连接状态的同时
	*/
	MON_CheckAssociate(APCLI_IF_2G,&statu2);
	MON_CheckAssociate(APCLI_IF_5G,&statu5);
	
	eth_status = nvram_get_int("wan_phylink");
	int newstatu = statu2 | statu5 | eth_status;
	
	laststatus = nvram_get_int("lan0_apcli_eth_link");
	//dprintf("now status2=%d,statu5=%d,newstatu=%d,eth_status=%d\n",statu2,statu5,newstatu,eth_status);
	//dprintf("laststatus=%d. last ApCliConnStatus=%s\n",laststatus,ApCliConnStatus);
	if(newstatu == 0)/*当前为断开状态*/
	{
		/*从连接到断开*/
		if(laststatus == 1)
		{
			/*当进行TriggerAPValidate动作时，不管连接状态*/
			if(strcmp(nvram_safe_get("wireless_detect_action"), "2") == 0 || strcmp(nvram_safe_get("wireless_detect_action"), "5") == 0)
			{
				//nvram_safe_set("wireless_detect_status", "DETECTING");
			}
			else
			{
				twsystem("killall udhcpc",TRUE);
				nvram_safe_set("lan0_ap_status",NETWORK_STATUS_CONNECTING);
				nvram_set_int("lan0_apcli_eth_link",0);
				
				notify_rc("stop_wanall");
				//notify_rc("stop_wan6");
				nvram_safe_set("lan0_ap_status",NETWORK_STATUS_DISCONNECTED);
				if(strcmp(nvram_safe_get("LEDStatusEnable"), "1") == 0)
				{
					/*为了让拔掉有线口时灯位显示更及时一点，在这里立即修改power灯位状态*/
					//Orange led Blinking
					system("led internet off -s");
        			system("led internet high blink -s");
				}
				twsystem("echo 0 >> /proc/apcli_disconnect/wlstatus_disconnect_flag_2g",TRUE);
				twsystem("echo 0 >> /proc/apcli_disconnect/wlstatus_disconnect_flag_5g",TRUE);
				
			}
		}
		else
		{
			/*断开到断开*/
			;
		}
	}
	else if(newstatu == 1)/*当前为连接状态*/
	{
		//
		if(strcmp(nvram_safe_get("wireless_detect_action"), "2") == 0 || strcmp(nvram_safe_get("wireless_detect_action"), "5") == 0)
		{
			//nvram_safe_set("wireless_detect_status", "OK_VALIDATED");
		}
		else
		{
			/*断开到连接*/
			if(laststatus == 0 )
			{
				start_flag = 1;
			}
			/*有线保持着连接的情况下，无线连接状态从连接到断开*/
			else if(strcmp(ApCliConnStatus,NETWORK_STATUS_CONNECTED)==0
				&& (statu2==0 && statu5==0))
			{
				start_flag = 1;
			}
			/*前后两次都为连接上的状态*/
			else/*由于监控有间隔且不能间隔太小，一直处于连接时，要检测中间是否有短暂的断开过*/
			{
				if(statu2 == 1)
					sprintf(szCmd,"/proc/apcli_disconnect/wlstatus_disconnect_flag_2g");
				else if(statu5 == 1)
		       		sprintf(szCmd,"/proc/apcli_disconnect/wlstatus_disconnect_flag_5g");
				fp_ext = fopen(szCmd, "r");
				if(NULL != fp_ext)
				{
					fread(buf, 1, 256, fp_ext);
					if(1 == atoi(buf)) /* 0 --> 在监控间隔内曾经断开过又连上了。*/
					{
						dprintf("------disconnect and connected!-------\n");
						
						start_flag = 1;
					}else if(0 == atoi(buf)){
						;/*一直连着没有断开过*/
					}
		            fclose(fp_ext);
				}else{
		        	//打开proc目录失败
		           //dprintf("proc file don't exist!\n");
		        } 
			}
			if(start_flag == 1)/*表示从断开到连接，或者在监控周期曾经断开过*/
			{
				twsystem("echo 0 >> /proc/apcli_disconnect/wlstatus_disconnect_flag_2g",TRUE);
				twsystem("echo 0 >> /proc/apcli_disconnect/wlstatus_disconnect_flag_5g",TRUE);
				
				nvram_safe_set("lan0_ap_status",NETWORK_STATUS_CONNECTING);
				nvram_set_int("lan0_apcli_eth_link",1);
				dprintf("restart_wanall---!\n");
				notify_rc("restart_wanall");
				//notify_rc("start_wan");
				//notify_rc("start_wan6");
			}
			else
			{
				;/*连上到连上*/
			}
				
		}
	}
	/*记录下无线的连接状态*/
	if(statu2 || statu5)
		nvram_safe_set("ApCliConnStatus",NETWORK_STATUS_CONNECTED);
	else
		nvram_safe_set("ApCliConnStatus",NETWORK_STATUS_DISCONNECTED);

	//setup_br_filter();
	return;
}

void setup_br_filter(void)
{
	char *pApCliStatus = NULL;
	char *pWanConStatus = NULL;
	char *lan_port_member;
	char wan_ifnames[64],lan_ifnames[32],lan_interfaces[32];
	char prefix[32];
	char tmp[128];
	char szCmd[256];
	int iEnable = 1,iStatus =1;
	static int isBrStatus = 1;

	snprintf(prefix, sizeof(prefix), "%s%d_", LAN_ETH_PREFIX, 0);
	memset(tmp, 0x0, sizeof(tmp));
	lan_port_member = nvram_safe_get(strcat_r(prefix, "port_member", tmp));	

	char *pi = strstr(lan_port_member,INTERFACE_ETH_IF);
	if(pi){
		pi += strlen(INTERFACE_ETH_IF);
		if(*pi == ' ')
			pi++;
		snprintf(lan_interfaces,sizeof(lan_interfaces),"%s",pi);
	}else{
		snprintf(lan_interfaces,sizeof(lan_interfaces),"%s",lan_port_member);
	}
	
	pApCliStatus = nvram_safe_get("ApCliConnStatus");
	pWanConStatus = nvram_safe_get("wan_phylink");

	if(!strcmp(pApCliStatus,NETWORK_STATUS_CONNECTED))
	{
		if(!strcmp(pWanConStatus,"1"))
		{
			snprintf(wan_ifnames,sizeof(wan_ifnames),"%s_%s",APCLI_IF_2G,APCLI_IF_5G);
			snprintf(lan_ifnames,sizeof(lan_ifnames),"%s",lan_port_member);
			iStatus = 1;
		}else{
			snprintf(wan_ifnames,sizeof(wan_ifnames),"%s_%s",APCLI_IF_2G,APCLI_IF_5G);
			snprintf(lan_ifnames,sizeof(lan_ifnames),"%s",lan_interfaces);
			iStatus = 2;
		}
	}else{
		if(!strcmp(pWanConStatus,"1"))
		{
			snprintf(wan_ifnames,sizeof(wan_ifnames),"%s",INTERFACE_ETH_IF);
			snprintf(lan_ifnames,sizeof(lan_ifnames),"%s",lan_interfaces);
			iStatus = 3;
		}else{
			iEnable = 0;
			snprintf(wan_ifnames,sizeof(wan_ifnames),"%s",INTERFACE_ETH_IF);
			snprintf(lan_ifnames,sizeof(lan_ifnames),"%s",lan_interfaces);
			iStatus = 4;
		}
	}
	if(iStatus != isBrStatus)
	{
		memset(szCmd, 0x0, sizeof(szCmd));
		snprintf(szCmd,sizeof(szCmd),"echo \"%d %s %s\" > /proc/br_filter_dhcp",iEnable,wan_ifnames,lan_ifnames);
		twsystem(szCmd,TRUE);
	}
	isBrStatus = iStatus;
}

/***
	监控上行连接的状态，断开将会触发重连。
***/
static void tw_CheckApCliConectStatus_callback(bcm_timer_id id, int flag)
{

	int statu2 = 0;
	int statu5 = 0;

	/*正在WPS,不重连*/
	if(nvram_get_int("WscStatus") == 1)
		return;
	TIMER_TRACE("\n");
	if(strcmp(nvram_bufget(RT2860_NVRAM,"ApCliEnable"),"1") == 0 )
	{   
	    TIMER_TRACE("\n");
		MON_CheckAssociate(APCLI_IF_2G,&statu2);
		if(statu2 == 0)//断开状态
		{
			if(strcmp(nvram_get(RT2860_NVRAM,"ApCliSsid"),"") != 0){
				twsystem("iwpriv apcli0 set ApCliEnable=0",TRUE);
				twsystem("iwpriv apcli0 set ApCliAutoConnect=2",TRUE);
				twsystem("iwpriv apcli0 set ApCliEnable=1",TRUE);
				twsystem("iwpriv apcli0 set PartialScan=1",TRUE);
				TIMER_TRACE("\n");
			}
		}
	}
	else
	{
		twsystem("iwpriv apcli0 set ApCliEnable=0",FALSE);
		twsystem("iwpriv apcli0 set ApCliSsid=",FALSE);
		twsystem("iwpriv apcli0 set ApCliBssid=",FALSE);
		TIMER_TRACE("\n");
	}

	if(strcmp(nvram_bufget(RTDEV_NVRAM,"ApCliEnable"),"1") == 0 )
	{
	    TIMER_TRACE("\n");
		MON_CheckAssociate(APCLI_IF_5G,&statu5);
		if(statu5 == 0)//断开状态
		{

			if(strcmp(nvram_get(RTDEV_NVRAM,"ApCliSsid"),"") != 0){
				twsystem("iwpriv apclii0 set ApCliEnable=0",TRUE);
				twsystem("iwpriv apclii0 set ApCliAutoConnect=2",TRUE);
				twsystem("iwpriv apclii0 set ApCliEnable=1",TRUE);
				twsystem("iwpriv apclii0 set PartialScan=1",TRUE);
				TIMER_TRACE("\n");
			}
		}
	}
	else
	{
		twsystem("iwpriv apclii0 set ApCliEnable=0",FALSE);
		twsystem("iwpriv apclii0 set ApCliSsid=",FALSE);
		twsystem("iwpriv apclii0 set ApCliBssid=",FALSE);
		TIMER_TRACE("\n");
	}
	return;

}


/*
	功能:在探测结束时将配置恢复到原来的
	
	参数:iNVRAM_Idx--RT2860_NVRAM/RTDEV_NVRAM
*/
static void tw_Recovery_Wireless_config(int iNVRAM_Idx)
{
	char cmd[512]={0};
	char *interface_apcli = NULL;
	
	if(iNVRAM_Idx == RT2860_NVRAM)
		interface_apcli = APCLI_IF_2G;
	else if(iNVRAM_Idx == RTDEV_NVRAM)
		interface_apcli = APCLI_IF_5G;
	
	/*探测到结果之后恢复原来的配置*/
	char * ApCliSsid = (char * )nvram_bufget(iNVRAM_Idx, "ApCliSsid");
	if(strchr(ApCliSsid, '\"'))
	{
		char dst[64] = {0};
		TW_addTransferchar(dst, ApCliSsid, '"');
		snprintf(cmd, sizeof(cmd),"iwpriv %s set ApCliSsid=\"%s\"", interface_apcli, dst);
	}
	else
		snprintf(cmd, sizeof(cmd),"iwpriv %s set ApCliSsid=\"%s\"", interface_apcli,ApCliSsid);

	twsystem(cmd, TRUE);
	
	char * ApCliAuthMode =  (char *)nvram_bufget(iNVRAM_Idx, "ApCliAuthMode");
	snprintf(cmd, sizeof(cmd),"iwpriv %s set ApCliAuthMode=%s", interface_apcli, ApCliAuthMode);
	twsystem(cmd, TRUE);
	char *ApCliEncrypType = (char *)nvram_bufget(iNVRAM_Idx, "ApCliEncrypType");
	snprintf(cmd, sizeof(cmd),"iwpriv %s set ApCliEncrypType=%s", interface_apcli, ApCliEncrypType);
	twsystem(cmd, TRUE);
	
	char *ApCliWPAPSK = (char *)nvram_bufget(iNVRAM_Idx, "ApCliWPAPSK");
	if(strchr(ApCliWPAPSK, '\"'))
	{
		char dst[64] = {0};
		TW_addTransferchar(dst, ApCliWPAPSK, '"');
		snprintf(cmd, sizeof(cmd),"iwpriv %s set ApCliWPAPSK=\"%s\"", interface_apcli, dst);
	}
	else
		snprintf(cmd, sizeof(cmd),"iwpriv %s set ApCliWPAPSK=\"%s\"", interface_apcli,ApCliWPAPSK);
	twsystem(cmd, TRUE);
	
	if(strcmp(nvram_get(iNVRAM_Idx,"ApCliEnable") ,"1") == 0)
	{
		snprintf(cmd, sizeof(cmd),"iwpriv %s set ApCliAutoConnect=2",interface_apcli);
		twsystem(cmd, TRUE);
		snprintf(cmd, sizeof(cmd),"iwpriv %s set ApCliEnable=1", interface_apcli);
		twsystem(cmd, TRUE);
	}
	else
	{
		snprintf(cmd, sizeof(cmd),"iwpriv %s set ApCliEnable=0", interface_apcli);
		twsystem(cmd, TRUE);
	}
}

/***
	检测由UI配置的参数是否正确
***/
static void tw_DetectApCliStatus_callback(bcm_timer_id id, int flag)
{
	int statu2 = 0;
	int statu5 = 0;
	char * action = NULL;
	char cmd[512]={0};
	static int g_iDetectTimes = 0;

	/*正在WPS,不探测*/
	if(nvram_get_int("WscStatus") == 1)
		return;

	/*这里检测超时的原因是避免用户在探测的过程中异常结束探测时定时器结束不了(比如关闭浏览器)*/
	unsigned long time_start= (unsigned long)nvram_get_int("wireless_detect_time");
	unsigned long time_now= (unsigned long)getSysUpTime();
	unsigned long time_tmp = time_now - time_start;
	dprintf("time_tmp:%ld\n", time_tmp);

	action = nvram_safe_get("wireless_detect_action");
	dprintf("action:%s\n",action);
	if(strcmp("5",action) == 0)
	{
		MON_CheckAssociate(APCLI_IF_5G,&statu5);
		dprintf("statu5:%d\n",statu5);
		if(statu5 ==1 || time_tmp > DETECT_TIMEOUT)//5G detect success or timeout
		{
			tw_Recovery_Wireless_config(RTDEV_NVRAM);
			bcm_timer_delete(timerId_wireless_detect);
			if( statu5 ==1)//5G detect success
				//写入探测结果
				nvram_safe_set("wireless_detect_status", "OK_VALIDATED");
			
			if(time_tmp > DETECT_TIMEOUT)
				nvram_safe_set("wireless_detect_action", "0");
			nvram_commit(RT2860_NVRAM);
		}
		else
		{
		    g_iDetectTimes = g_iDetectTimes + 1;
			if(g_iDetectTimes >= 2)
			{
				twsystem("iwpriv apclii0 set ApCliEnable=0", TRUE);
				twsystem("iwpriv apclii0 set ApCliAutoConnect=0", TRUE);
				twsystem("iwpriv apclii0 set ApCliEnable=1", TRUE);
				twsystem("iwpriv apclii0 set ApCliAutoConnect=2", TRUE);
				g_iDetectTimes = 0;
			}
		}
		
	}
	else if(strcmp("2",action) == 0)
	{
		MON_CheckAssociate(APCLI_IF_2G,&statu2);
		dprintf("statu2:%d\n",statu2);
		if(statu2 ==1 || time_tmp > DETECT_TIMEOUT)//2.4G detect success or timeout
		{
			/*探测到结果之后恢复原来的配置*/
			tw_Recovery_Wireless_config(RT2860_NVRAM);
			bcm_timer_delete(timerId_wireless_detect);
			if( statu2 ==1)//2.4G detect success
				//写入探测结果
				nvram_safe_set("wireless_detect_status", "OK_VALIDATED");
			
			if(time_tmp > DETECT_TIMEOUT)
				nvram_safe_set("wireless_detect_action", "0");
			nvram_commit(RT2860_NVRAM);
		}
		else
		{
		    g_iDetectTimes = g_iDetectTimes + 1;
			if(g_iDetectTimes >= 2)
			{
				twsystem("iwpriv apcli0 set ApCliEnable=0", TRUE);
				twsystem("iwpriv apcli0 set ApCliAutoConnect=0", TRUE);
				twsystem("iwpriv apcli0 set ApCliEnable=1", TRUE);
				twsystem("iwpriv apcli0 set ApCliAutoConnect=2", TRUE);
				g_iDetectTimes = 0;
			}
		}

	}
	else if(strcmp("0",action) == 0)/*cgi将wireless_detect_action置0时，将会在这里超时*/
	{
		dprintf(

"**********wireless detect stop**********\n");
		tw_Recovery_Wireless_config(RT2860_NVRAM);
		bcm_timer_delete(timerId_wireless_detect);
	}
	
}
static void detect_registerTimer(int sig)
{
	dprintf(

"**********wireless detect timer**********\n");
	int time_detect = BCM_TIMER_INTERVAR_APCLI_STATUS_DETECT;
	bcm_timer_delete(timerId_wireless_detect);
	tw_timer_start(http_timer,&timerId_wireless_detect, time_detect, (bcm_timer_cb)tw_DetectApCliStatus_callback, HTTP_TIMER_DETECT_APCLI_STATUS);
}

/*************************************************************************
功能:上行WPS连接上之后，获取上行的参数 ssid,authmode,encryptype
参数:接口名称,UPLINKAP_INFO结构体指针
返回:0 --->获取成功，-1 --->获取失败
*************************************************************************/
static int RT6352_apclient_wps_get_uplink_ap_info(char *interface, UPLINKAP_INFO *pUplinkApInfo)
{
	int socket_id = 0;
	struct iwreq wrq;
    
    socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	strcpy(wrq.ifr_name, interface);
	wrq.u.data.length = sizeof(UPLINKAP_INFO);
	wrq.u.data.pointer = (caddr_t)pUplinkApInfo;
	wrq.u.data.flags = RT_OID_WSC_UPLINK_AP_INFO;
	if( ioctl(socket_id, RT_PRIV_IOCTL, &wrq) == -1)
	{
		//dprintf("ioctl error\n");
        close(socket_id);
        return -1;
	}
	close(socket_id);
    
    return 0;
}


/*************************************************************************
功能:获取对应接口的上行WPS状态
参数:接口名称
返回:状态码
*************************************************************************/
static int RT6352_apclient_wps_get_status(char *interface)
{
	int status = 0;
	int get_status = -1;
	int socket_id = 0;
	struct iwreq wrq;

    socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	strcpy(wrq.ifr_name, interface);
	wrq.u.data.length = sizeof(status);
	wrq.u.data.pointer = (caddr_t) &status;
	wrq.u.data.flags = RT_OID_WSC_QUERY_STATUS;
	if( ioctl(socket_id, RT_PRIV_IOCTL, &wrq) == -1)
	{
		dprintf("ioctl error\n");
	}
	close(socket_id);

	if(status >= 0)
	{
		dprintf("Ralink wps status = %d \n", status);
		switch( status )
	    {
	        case STATUS_WSC_LINK_UP:
	        	get_status = 0; //WPS_PROTOCOL_START
	            break;
	        case STATUS_WSC_CONFIGURED:
	        	get_status = 3; //WPS_PROTOCOL_SUCCESS
	            break;
	        case STATUS_WSC_FAIL:
	        	get_status = 2; //WPS_PROTOCOL_TIMEOUT
	        	break;
	        case STATUS_WSC_IDLE: /*这说明在WPS协商过程中，又对AP配置进行修改, WPS 被关闭了*/
                dprintf("*****STATUS_WSC_IDLE %s %d ****\n",__FUNCTION__,__LINE__);
				get_status = -1; //WPS_PROTOCOL_FAIL
	            break;
	        case STATUS_WSC_PBC_SESSION_OVERLAP:
	        	get_status = 1; //WPS_PROTOCOL_PBC_OVERLAPPING
	            break;
            case STATUS_WSC_PBC_TOO_MANY_AP:
                get_status = 4;
                break;
            case STATUS_WSC_PBC_NO_AP:
                get_status = 0; /*只时暂时没有发现 ap, 如果在两分钟内没有发现ap,驱动将会设置time out状态*/
                break;
	        default:            
	        	if(status < 0x0100) //RALINK 错误status是从0x0100开始
				{
					get_status = 0; //WPS_PROTOCOL_START
				}
				else
				{
                    printf("*****status: %d %s %d ****\n",status, __FUNCTION__,__LINE__);
					get_status = -1; //WPS_PROTOCOL_FAIL
				}
	            break;
	    }		
		return get_status;
	}
	return -1; //WPS_PROTOCOL_FAIL
}

static char *getAuthMode(int n)
{
	switch(n)
	{
		case APCLI_AUTH_TYPE_OPEN:
			return "OPEN";
		case APCLI_AUTH_TYPE_WPAPSK:
			return "WPAPSK";
		case APCLI_AUTH_TYPE_SHARED:
			return "SHARED";
		case APCLI_AUTH_TYPE_WPA:
			return "WPA";
		case APCLI_AUTH_TYPE_WPA2:
			return "WPA2";
		case APCLI_AUTH_TYPE_WPA2PAK:
			return "WPA2PSK";
		default:
			return "";
	}
}
static char *getEncrytType(int n)
{
	switch(n)
	{
		case APCLI_ENCRYP_TYPE_NONE:
			return "NONE";
		case APCLI_ENCRYP_TYPE_WEP:
			return "WEP";
		case APCLI_ENCRYP_TYPE_TKIP:
			return "TKIP";
		case APCLI_ENCRYP_TYPE_AES:
			return "AES";
		default:
			return "";
	}
}

static void wps_status_handle(void)
{	
	int status2, status5;
	int ret = 0;
	char interfac2[] = "apcli0";
	char interfac5[] = "apclii0";
	char *pSSID = NULL;
	char *pAuthMode = NULL;
	char *pEncrType = NULL;
	char *pKey = NULL;
	char *SSID = NULL;
	char tmp_SSID[128] = {0};
	int iNVRAM_Idx = RT2860_NVRAM;
	const char *pEnableWsc=NULL;
	pEnableWsc = nvram_safe_get("WscConfMode");
	char cmd_buf[256]={0};
	UPLINKAP_INFO UplinkApInfo;
	
	long timeout = getSysUpTime() - wpsTimeStart;
	dprintf("ap apcli0 timeout= %d\n",timeout);
	/*timeout check*/
	if(timeout >= 125 )
		goto endwps;
	
	status2 = RT6352_apclient_wps_get_status(interfac2);
	status5 = RT6352_apclient_wps_get_status(interfac5);
	dprintf("Client WPS status2:%d status5:%d\n", status2,status5);
	
	if((status2 == 3) || (status5 == 3)){//有一个连接成功
		dprintf("ApCli WPS success\n");
		
		wpsJustOk = 4;/*设定标识，给tw_WanLedControl_callback用,这个定时器4s一次， 4 * 4s=16s */
		TIMER_TRACE("---------------------OOOO--->wpsJustOk=%d\n",wpsJustOk);
		wpsTimeStart = 0;
		bcm_timer_delete(timerId_apcli_wps);//连接成功，删除定时器
		bcm_timer_delete(timerId_ap_wps);
		nvram_safe_set("WscStatus","0");
		system("iwpriv apcli0 set WscStop=1");
		system("iwpriv apclii0 set WscStop=1");

		system("iwpriv ra0 set WscStop=1");
		system("iwpriv rai0 set WscStop=1");
		sprintf(cmd_buf, "iwpriv ra0 set WscConfMode=%s\n",pEnableWsc);
		sprintf(cmd_buf, "%siwpriv rai0 set WscConfMode=%s\n",cmd_buf,pEnableWsc);
		system(cmd_buf);

	//获取上行信息，存到nvram配置里
		if(status2==3){
			ret=RT6352_apclient_wps_get_uplink_ap_info(interfac2,&UplinkApInfo);
			iNVRAM_Idx = RT2860_NVRAM;
		}
		if(status5==3){
			ret=RT6352_apclient_wps_get_uplink_ap_info(interfac5,&UplinkApInfo);
			iNVRAM_Idx = RTDEV_NVRAM;
		}
		if(ret<0)
			return ;
		if(status2==3){
			nvram_bufset(RTDEV_NVRAM, "ApCliSsid", "");
			nvram_bufset(RTDEV_NVRAM, "ApCliBssid", "");
			nvram_bufset(RTDEV_NVRAM, "ApCliAuthMode", "");
			nvram_bufset(RTDEV_NVRAM, "ApCliEncrypType", "");
			nvram_bufset(RTDEV_NVRAM, "ApCliWPAPSK", "");
		}
		if(status5==3){
			nvram_bufset(RT2860_NVRAM, "ApCliSsid", "");
			nvram_bufset(RT2860_NVRAM, "ApCliBssid", "");
			nvram_bufset(RT2860_NVRAM, "ApCliAuthMode", "");
			nvram_bufset(RT2860_NVRAM, "ApCliEncrypType", "");
			nvram_bufset(RT2860_NVRAM, "ApCliWPAPSK", "");
		}
		dprintf("---ret=%d,Channel=%d-----\n", ret,UplinkApInfo.Channel);
		dprintf("---SSID=%s------\n", UplinkApInfo.WscProfile.Profile[0].SSID.Ssid);
		dprintf("---AuthType=%d-----\n", UplinkApInfo.WscProfile.Profile[0].AuthType);
		dprintf("---EncrType=%d-----\n", UplinkApInfo.WscProfile.Profile[0].EncrType);
		dprintf("---Key=%s-----\n", UplinkApInfo.WscProfile.Profile[0].Key);
		

		pSSID = UplinkApInfo.WscProfile.Profile[0].SSID.Ssid;
		if(pSSID != NULL)
		{
			strcpy(tmp_SSID,pSSID);
			dprintf("---strlen SSID=%d------\n", strlen(tmp_SSID));
			if(strlen(pSSID)>26){
				memset(tmp_SSID+23, 0, sizeof(tmp_SSID)-23);
				SSID=strcat(tmp_SSID,"...-EXT");
			}
			else
				SSID=strcat(tmp_SSID,"-EXT");
			
			dprintf("---Extender SSID=%s-----\n", SSID);
			nvram_bufset(RT2860_NVRAM, "SSID1",SSID);
			nvram_bufset(RTDEV_NVRAM, "SSID1",SSID);
			nvram_bufset(RT2860_NVRAM, "IsDefaultLogin","0");/*must be configured mode*/
				
			//nvram_bufset(RT2860_NVRAM, "ApCliSsid",UplinkApInfo.WscProfile.Profile[0].SSID.Ssid);
			nvram_bufset(iNVRAM_Idx, "ApCliSsid",UplinkApInfo.WscProfile.Profile[0].SSID.Ssid);
		}
		pAuthMode=getAuthMode(UplinkApInfo.WscProfile.Profile[0].AuthType);
		if( pAuthMode != NULL)
		{
			//nvram_bufset(RT2860_NVRAM, "ApCliAuthMode",pAuthMode);
			nvram_bufset(iNVRAM_Idx, "ApCliAuthMode",pAuthMode);
			if(strcmp("OPEN",pAuthMode) == 0)
			{
				set_nth_value_flash(RT2860_NVRAM, 0, "AuthMode", "OPEN");
				set_nth_value_flash(RTDEV_NVRAM, 0, "AuthMode", "OPEN");
			}
		}
		pEncrType =getEncrytType(UplinkApInfo.WscProfile.Profile[0].EncrType);
		if( pEncrType != NULL)
		{
			//nvram_bufset(RT2860_NVRAM, "ApCliEncrypType",pEncrType);
			nvram_bufset(iNVRAM_Idx, "ApCliEncrypType",pEncrType);
			if(strcmp("NONE",pEncrType) == 0)
			{
				set_nth_value_flash(RT2860_NVRAM, 0, "EncrypType", "NONE");
				set_nth_value_flash(RTDEV_NVRAM, 0, "EncrypType", "NONE");
			}
		}
		UplinkApInfo.WscProfile.Profile[0].Key[64] = 0;
		pKey = UplinkApInfo.WscProfile.Profile[0].Key;
		if( pKey != NULL)
		{
			
			nvram_bufset(RT2860_NVRAM, "WPAPSK1",pKey);
			nvram_bufset(RTDEV_NVRAM, "WPAPSK1",pKey);
			
			//nvram_bufset(RT2860_NVRAM, "ApCliWPAPSK",pKey);//不支持WEP
			nvram_bufset(iNVRAM_Idx, "ApCliWPAPSK",pKey);//不支持WEP
		}
		
		nvram_bufset(RT2860_NVRAM, "ApCliEnable","0");
		nvram_bufset(RTDEV_NVRAM, "ApCliEnable","0");
		if(status5 == 3)
		{
			nvram_bufset(RTDEV_NVRAM, "ApCliEnable","1");
			system("iwpriv apcli0 set ApCliEnable=0");
		}
		else if(status2 == 3)
		{
			nvram_bufset(RT2860_NVRAM, "ApCliEnable","1");
			system("iwpriv apclii0 set ApCliEnable=0");
		}
		
		nvram_commit(RT2860_NVRAM);
		nvram_commit(RTDEV_NVRAM);
		notify_rc("restart_mdns");
		notify_rc("restart_wlan");
		
	}
	else if((status2 == 0) && (status5 == 0)){//两个都仍在连接中
		;
	}
	else{//出现异常/超时

		goto endwps;
	}
	
	return ;

endwps:

	dprintf("ApCli WPS Fail\n");
	wpsTimeStart = 0;
	bcm_timer_delete(timerId_apcli_wps);
	bcm_timer_delete(timerId_ap_wps);
	
	system("iwpriv apcli0 set WscStop=1");
	system("iwpriv apclii0 set WscStop=1");
	nvram_safe_set("WscStatus","0");
	
	system("iwpriv ra0 set WscStop=1");
	system("iwpriv rai0 set WscStop=1");
	sprintf(cmd_buf, "iwpriv ra0 set WscConfMode=%s\n",pEnableWsc);
	sprintf(cmd_buf, "%siwpriv rai0 set WscConfMode=%s\n",cmd_buf,pEnableWsc);
	system(cmd_buf);
}

static void apcli_wps_callback(void)
{
	wps_status_handle();
}

static void
ApCliWpsStart_registerTimer(int sig)
{
	if (sig == APCLI_WPS_START)
    {   
    	dprintf(

"**********start apcli wps timer**********\n");
		int time_apcli_wps = BCM_TIMER_INTERVAR_APCLI_WPS;
		if(!wpsTimeStart)
			wpsTimeStart = getSysUpTime();
		bcm_timer_delete(timerId_apcli_wps);
		tw_timer_start(http_timer,&timerId_apcli_wps, time_apcli_wps, (bcm_timer_cb)apcli_wps_callback, HTTP_TIMER_DETECT_APCLI_WPS);
	}
}

static void ap_wps_handle(void)
{	
	int status2, status5;
	int ret = 0;
	char interfac2[] = "ra0";
	char interfac5[] = "rai0";
	const char *pEnableWsc=NULL;
	char cmd_buf[256]={0};
	char *workmode = nvram_safe_get("WorkMode");

	long timeout = getSysUpTime() - wpsTimeStart;
	dprintf("ap wps timeout= %d\n",timeout);
	/*timeout check*/
	if(timeout >= 125 )
		goto endwps;
	
	//cprintf("########################ap wps_status############\n");
	status2 = RT6352_apclient_wps_get_status(interfac2);
	status5 = RT6352_apclient_wps_get_status(interfac5);
	dprintf("Ap WPS status2:%d,status5:%d\n", status2,status5);
	
	if((status2 == 3) || (status5 == 3)){//有一个连接成功
		bcm_timer_delete(timerId_ap_wps);//连接成功，删除定时器
		wpsTimeStart = 0;
		pEnableWsc = nvram_safe_get("WscConfMode");
		system("iwpriv ra0 set WscStop=1");
		system("iwpriv rai0 set WscStop=1");
		sprintf(cmd_buf, "iwpriv ra0 set WscConfMode=%s\n",pEnableWsc);
		sprintf(cmd_buf, "%siwpriv ra0 set WscConfMode=%s\n",cmd_buf,pEnableWsc);
		system(cmd_buf);
		
#ifdef CONFIG_AP_REPEATER_SUPPORT
		if(strcmp(workmode,WIRELESS_REPEATER) == 0){
			system("iwpriv apcli0 set WscStop=1");
			system("iwpriv apclii0 set WscStop=1");
			bcm_timer_delete(timerId_apcli_wps);
		}
		

#endif
		system("iwpriv ra0 set led_setting=00-00-00-00-00-00-00-00");
    	system("iwpriv rai0 set led_setting=00-00-00-00-00-00-00-00");
	}
	else if((status2 == 0) || (status5 == 0)){//两个都仍在连接中
		;
	}
	else
	{
		goto endwps;
	}
	return ;
	
endwps:
	dprintf("Ap WPS Fail\n");
	system("iwpriv ra0 set led_setting=00-00-00-00-00-00-00-00");
    system("iwpriv rai0 set led_setting=00-00-00-00-00-00-00-00");
	wpsTimeStart = 0;
	bcm_timer_delete(timerId_ap_wps);//连接异常/超时
	if(strcmp(workmode,WIRELESS_REPEATER) == 0){
		system("iwpriv apcli0 set WscStop=1");
		system("iwpriv apclii0 set WscStop=1");
		bcm_timer_delete(timerId_apcli_wps);
	}
	
	nvram_safe_set("WscStatus","0");
	system("iwpriv ra0 set WscStop=1");
	system("iwpriv rai0 set WscStop=1");
	sprintf(cmd_buf, "iwpriv ra0 set WscConfMode=%s\n",pEnableWsc);
	sprintf(cmd_buf, "%siwpriv rai0 set WscConfMode=%s\n",cmd_buf,pEnableWsc);
	system(cmd_buf);	

	return ;
}

Ap_WpsStart_registerTimer(int sig)
{
	if (sig == WPS_START)
    {   
    	dprintf("**********start ap wps timer**********\n");
		int time_wps = BCM_TIMER_INTERVAR_AP_WPS;
		if(!wpsTimeStart)
			wpsTimeStart = getSysUpTime();
		bcm_timer_delete(timerId_ap_wps);
		tw_timer_start(http_timer,&timerId_ap_wps, time_wps, (bcm_timer_cb)ap_wps_handle, HTTP_TIMER_DETECT_AP_WPS);
	}
}

#endif

void main(int argc, char **argv)
{	
	int ppp_time = BCM_TIMER_INTERVAR_PPP_DETECT;//定时4s
	
	bcm_timer_id timerId_SysLogMail = BCM_TIMER_ID_INIT; //  timer for SysLogMail
	int time_SysLogMail = BCM_TIMER_INTERVAR_SYSLOG_MAIL; // time period for SysLogMail	
	
	bcm_timer_id timerId_WanLedControl = BCM_TIMER_ID_INIT; //  timer for WanLedControl
	int time_WanLedControl = BCM_TIMER_INTERVAR_WAN_LEDCTRL; // time period for WanLedControl	
	
	bcm_timer_id timerId_GetOnlineFWVer = BCM_TIMER_ID_INIT; //  timer for GetOnlineFWVer
	int time_GetOnlineFWVer = BCM_TIMER_INTERVAR_GET_ONLINE_FWVER; // time period for GetOnlineFWVer	
	
	bcm_timer_id timerId_WlanLedControl = BCM_TIMER_ID_INIT; //  timer for wlan
	int time_WlanLedControl = BCM_TIMER_INTERVAR_WLAN_LEDCTRL; // time period for wlan	
	
#ifdef CONFIG_AP_REPEATER_SUPPORT	
	char *workmode = nvram_safe_get("WorkMode");
	bcm_timer_id timerId_CheckApCliStatus = BCM_TIMER_ID_INIT; //  timer for checkApCliStatus	
	int time_CheckApCliStatus = BCM_TIMER_INTERVAR_APCLI_STATUS; // time period for checkApCliStatus
	
	bcm_timer_id timerId_CheckApCliConnectStatus = BCM_TIMER_ID_INIT;
	int time_CheckApCliConnectStatus = BCM_TIMER_INTERVAR_APCLI_CONNECT_STATUS; // time period for checkApCliConnectStatus
#endif

#ifdef CONFIG_FOTA
	bcm_timer_id timerId_fota_auto_upgrade = BCM_TIMER_ID_INIT;
	/*
	  * 1. The timer is delayed by one second every few minutes.
	  * 2. Therefore, to ensure that the timer times out every minute,
	  *   we set the interval to 59 seconds.
	  * 3. The timer sometimes times out twice in a minute. This is acceptable.
	  * 2018-03-01 --liushenghui
	*/
	int time_fota_auto_upgrade = BCM_TIMER_INTERVAR_FOTA_AUTO_UPGRADE;
#endif
	sigset_t sigset;
	/* Show usage */
	if (argc > 1) {
		fprintf(stderr, "Usage: timer\n");
		return;
	}
	writeTimerPid();
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGTERM);
	sigaddset(&sigset, SIGUSR2);
	sigaddset(&sigset, SIGUSR1);
    sigaddset(&sigset, SIGALRM);
    sigaddset(&sigset, PPTP_L2TP_MON_SIG_NUM);
	sigaddset(&sigset, WLAN_RESTART_TYPE);	
	sigaddset(&sigset, WPS_START);
#ifdef CONFIG_AP_REPEATER_SUPPORT
	if(strcmp(workmode,WIRELESS_REPEATER) == 0){
		sigaddset(&sigset, APCLI_WPS_START);
	}
	sigaddset(&sigset, LOOPBACK_START);
	sigaddset(&sigset, DETECT_START);
#endif
	sigprocmask(SIG_UNBLOCK, &sigset, NULL);
    
	signal(PPTP_L2TP_MON_SIG_NUM, wan_registerTimer);
	signal(WLAN_RESTART_TYPE, wlan_registerTimer);
	signal(SIGTERM, sig_handler);
	signal(SIGUSR2, wan_handler);
    signal(SIGUSR1, WanLedControl_handler);
#ifdef CONFIG_AP_REPEATER_SUPPORT
	signal(LOOPBACK_START, br_fdb_loop_handler);
	signal(DETECT_START, detect_registerTimer);
	signal(WPS_START, Ap_WpsStart_registerTimer);
	// for CheckApCliWpsStatus
	if(strcmp(workmode,WIRELESS_REPEATER) == 0){
		dprintf("**********timer wps register***********\n");
		signal(APCLI_WPS_START, ApCliWpsStart_registerTimer);
	}
	else if(!workmode)
		return;
#endif	
	bcm_timer_module_init(HTTP_TIMER_DETECT_MAX_NUM, &http_timer);//http模块注册3个timer.
//	TIMER_TRACE("http timer enable:%s %s\n", nvram_safe_get("self_healing_enable"),nvram_safe_get("wlan_time_enable"));
	
	if( nvram_invmatch("wan_phylink","0" ) )
	{		
		tw_timer_start(http_timer, &ppp_id, ppp_time, (bcm_timer_cb)ppp_callback, HTTP_TIMER_PPP_DETECT);
	}

    // for WanLedControl
    tw_timer_start(http_timer, &timerId_WanLedControl, time_WanLedControl, (bcm_timer_cb)tw_WanLedControl_callback, HTTP_TIMER_WAN_LEDCTRL);

    // for GetOnlineFWVer
#ifndef	CONFIG_DEFAULTS_MEDIATEK_DIR_878_CA
    tw_timer_start(http_timer, &timerId_GetOnlineFWVer, time_GetOnlineFWVer, (bcm_timer_cb)tw_GetOnlineFWVer_callback, HTTP_TIMER_GET_ONLINE_FWVER);
#endif
 	tw_timer_start(http_timer,&timerId_WlanLedControl, time_WlanLedControl, (bcm_timer_cb)tw_WlanLedControl_callback, HTTP_TIMER_WLAN_LEDCTRL);
 
#ifdef CONFIG_FOTA
	if (tw_timer_start(http_timer, &timerId_fota_auto_upgrade,
		time_fota_auto_upgrade, fota_monitor, HTTP_TIMER_FOTA_MONITOR) < 0
	)
	{
		cprintf("Create fota timer fail.\n");
	}
#endif
#ifdef CONFIG_AP_REPEATER_SUPPORT
	// for CheckApCliStatus
	if(strcmp(workmode,WIRELESS_REPEATER) == 0){
		tw_timer_start(http_timer, &timerId_CheckApCliStatus, time_CheckApCliStatus, (bcm_timer_cb)tw_CheckApCliStatus_callback, HTTP_TIMER_APCLI_STATUS);
		tw_timer_start(http_timer, &timerId_CheckApCliConnectStatus, time_CheckApCliConnectStatus, (bcm_timer_cb)tw_CheckApCliConectStatus_callback, HTTP_TIMER_APCLI_CONNECT_STATUS);
	}
#endif

	while(1) {
		
		/* check for quit */
		if (sig_quit > 0) {
#if 0
			bcm_timer_delete(self_heling_id);
			bcm_timer_delete(wlan_id);
			bcm_timer_delete(parental_control_id);
			//bcm_timer_delete(detect_id);
#endif			
			bcm_timer_delete(ppp_id);
			bcm_timer_delete(timerId_SysLogMail);
            bcm_timer_delete(timerId_WanLedControl);
            bcm_timer_delete(timerId_GetOnlineFWVer);
			return;
		}

		usleep(500*1000);
		tw_SysLogMail_callback();
	}
}


/* va_list is part of stdarg.h */	
int _vstrsep(char *buf, const char *sep, ...)
{
	va_list ap;
	char **p;
	int n;
	n = 0;
	va_start(ap, sep);
	while ((p = va_arg(ap, char **)) != NULL) {
		if ((*p = strsep(&buf, sep)) == NULL) break;
		++n;
	}
	va_end(ap);
	return n;
}

#ifdef CONFIG_FOTA

#define  TMP_FW "/tmp/fota_firmware.img"

int iRand_time_minute = 0;
int iRand_has_generate = 0;

#if 0
static int fota_has_been_updated_today(const struct tm * pNow)
{
	const char * pLast_time = NULL;
	time_t last;
	struct tm * pTm_last = NULL;

	if (!pNow)
	{
		cprintf("Null pointer parameter.\n");
		return -1;
	}

	pLast_time = nvram_get(CONFIG2_NVRAM, "fota_last_update_time");
	if (!pLast_time || !strcmp(pLast_time, ""))
	{
		return 0;
	}

	last = time(NULL);
	if (last < 0)
	{
		cprintf("time fail, %s, errno=%d.\n", strerror(errno), errno);
		return -1;
	}

	pTm_last = localtime(&last);
	if (!pTm_last)
	{
		cprintf("localtime fail, %s, errno=%d.\n", strerror(errno), errno);
		return -1;
	}

	if (pNow->tm_yday == pTm_last->tm_yday)
	{
		return 1;
	}

	return 0;
}
#endif

static int fota_auto_upgrade_monitor(void)
{
	char sTime[32] = { 0 };
	char sCmd_buf[128] = { 0 };
	char szBuf[128] = { 0 };
	int  mac_addr[6] = { 0 };	
	int iFota_time_hour = 0;
	int iFota_time_minute = 0;
	time_t now;
	struct tm * pTm_now = NULL;

#ifdef CONFIG_TZ_LOCATION

	if (nvram_invmatch("time_sync_done", "1"))
	{
		return 0;
	}

#endif

	if (nvram_invmatch("fota_auto_query_enable", "1"))
	{
		return 0;
	}

	iFota_time_hour = nvram_get_int("fota_time_hour");
	iFota_time_minute = nvram_get_int("fota_time_minute");

	now = time(NULL);
	if (now < 0)
	{
		cprintf("time fail, %s, errno=%d.\n", strerror(errno), errno);
		return -1;
	}

	pTm_now = localtime(&now);
	if (!pTm_now)
	{
		cprintf("localtime fail, %s, errno=%d.\n", strerror(errno), errno);
		return -1;
	}

	if (nvram_match("fota_time_hour", "")
		|| nvram_match("fota_time_minute", ""))
	{
		if (0 == iRand_has_generate)
		{
			strcpy(szBuf, nvram_safe_get("lan0_mac"));
			sscanf(szBuf, "%02x:%02x:%02x:%02x:%02x:%02x", &(mac_addr[0]), &(mac_addr[1]), &(mac_addr[2]), &(mac_addr[3]), &(mac_addr[4]), &(mac_addr[5]));
			
			iRand_time_minute = ( mac_addr[0] + mac_addr[1] + mac_addr[2] + 	
								  mac_addr[3] + mac_addr[4] + mac_addr[5] ) % 30;
			iRand_has_generate = 1;

			cprintf("Fota time is not set, select random time %d:%d.\n",
				3, 30 + iRand_time_minute);
		}

		iFota_time_hour = 3;
		iFota_time_minute = 30 + iRand_time_minute;
	}

	if (pTm_now->tm_hour == iFota_time_hour
		&& pTm_now->tm_min == iFota_time_minute
	)
	{
	#ifdef __SUPPORT_IMG_CRYPT__
		snprintf(sCmd_buf, sizeof (sCmd_buf),
			"fota --action auto --fw-path %s --fw-encrypt &",
			TMP_FW);
	#else
		snprintf(sCmd_buf, sizeof (sCmd_buf),
			"fota --action auto --fw-path %s &",
			TMP_FW);
	#endif

		twsystem(sCmd_buf, TRUE);

		iRand_has_generate = 0;
	}

	return 1;
}

static int fota_event_notify_monitor(void)
{
	const char * pEvent_notify_flag = NULL;

	pEvent_notify_flag = nvram_safe_get("fota_update_done_notify");
	if (!pEvent_notify_flag || 0 != strcmp(pEvent_notify_flag, "1"))
	{
		return 0;
	}

	twsystem("fota --action notify --fw-encrypt &", TRUE);

	return 1;
}

static void fota_monitor(bcm_timer_id id, int flag)
{
	fota_auto_upgrade_monitor();

#ifdef __SUPPORT_IMG_CRYPT__
	fota_event_notify_monitor();
#endif
}

#endif

