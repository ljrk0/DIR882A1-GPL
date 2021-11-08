
/**********************************************************************
 Copyright (c), 1991-2012, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 文件名称: protest.h
 文件描述: 
 修订记录:
        1. 作者: 朱健文
           日期: 
           内容: 创建文件
**********************************************************************/

#ifndef _PROTEST_H_
#define _PROTEST_H_

//#include <bcmconfig.h>

/******************************************************************************
*                                 MACRO                                      *
******************************************************************************/
/*
add configuration item for compilate the rssi modle
zhengmingming 
20131012
*/

/**********************其他宏定义***********************/
#define MODULE_NAME "pti_module: "
#define PTI_PRINTF(fmt, args...)  printf(MODULE_NAME "pti_module: ""%s: %d: " fmt, __FUNCTION__, __LINE__, ##args)
#define TAB_PRINT "	"
#define PTI_System(cmd) tbsSystem(cmd, 0)
#define TelnetPrint(fmt, args...)  printf("~!"fmt"!~\n\r", ##args)
#define TelnetGetPrint(fmt, args...)  printf("~~~~~~!"fmt"!~~~~~~\n\r", ##args)
#define HelpPrint(fmt, args...)  printf(fmt, ##args)


#define HELP_MSG_LENGTH 100 
#define FUNC_FLAG_LENGTH 20
#define MAX_NODE_LENGTH 256
#define MSG_BODY_LENGTH   64*1024
#define IP_ADDR_LENGTH  20
#define MAX_LENGTH_PATH 100
#define MAX_LENGTH_CMD 80
#define MAX_LENGTH_BUF 50
#define MAX_NAME_LEN  32


#define RETURN_SUCCSS   "success"
#define RETURN_FAIL     "false"
#define RETURN_ERROR    "error"
//led
#define LED_NAME_MAX	32
#define TRIG_NAME_MAX	32
//button
#define MAX_MSGSIZE 1024
#define BLP_CMM_GROUP 0
#define PID_BUF_LEN 32
#define BUTTON_MSG_LEN 128
#define BUTTON_BAN "ban"
#define BUTTON_RESUME "resume"

//help MESSAGE
#define HELP_START_MSG  "usage: protest --option1 [-option2] [option3]...\n\r"
#define HELP_LED_MSG    "--led -on/off/ban/resume -green/-red	    led all on/off\n\r"
#define HELP_BUTTON_MSG "--button -ban/resume/r		ban/resume button normal function; read button message!\n\r"
#define HELP_MAC_MSG    "--mac -r/-w mac-addr		read mac addr/set mac addr\n\r"
#define HELP_MYDLINKNUMBER_MSG    "--mynum -r/-w 44441234 read/write mydlink number\n\r"
#define HELP_MYDLINKUID_MSG    "--myuid -r/-w auto/CO2KsIup1zCmgmzm read/write mydlink UID\n\r"
#define HELP_MYDLINKCERT_MSG    "--mycert -r/-w key/pem key/pem-str; read/write mydlink cert key\n\r"
#define HELP_FOTA_MSG  "--fota -client_id -w xxxxxxxx\n\r"\
			TAB_PRINT"--fota -client_id -r\n\r"\
			TAB_PRINT"--fota -client_secret -w xxxxxxxx\n\r"\
			TAB_PRINT"--fota -client_secret -r\n\r"\
			TAB_PRINT"--fota -device_id -generate	Must set factory mac and client secret first\n\r"\
			TAB_PRINT"--fota -device_id -r\n\r"
#define HELP_SSID_MSG   "--2g_ssid -r/-w ssid	--5g_ssid -r/-w ssid	read ssid/set ssid\n\r"
#define HELP_SWITCH_MSG   "--2g_switch -on/-off  --5g_swtich -on/-off	turn on/off the wireless function\n\r"
#define HELP_ADMINPWD_MSG   "--adminpwd -r/-w adminpwd		read adminpwd/set adminpwd\n\r"
#define HELP_PIN_MSG   "--pin -r/-w pin	 read pin/set pin\n\r"
#define HELP_WLANPWD_MSG   "--wlanpasswd -r/-w wlanpwd		read wlanpwd/set wlanpwd, the password should be 8-63 characters or 64 hex digits.\n\r"
#define HELP_DEVICEID_MSG   "--deviceid -r/-w deviceid		read deviceid/set deviceid\n\r"
#define HELP_RESERVE1_MSG   "--regioncode -r/-w/-help Regioncode		read Regioncode/Regioncode\n\r"
#define HELP_RESERVE2_MSG   "--countrycode -2G/-5G -r/-w/-help countrycode	read/write countrycode\n\r"
#define HELP_WCODE_MSG   "--wcode -r/-w country-code		read country code/set country code\n\r"
#define HELP_CHANNAL_MSG    "--channal -r/-w channal		read channal/set channal\n\r"
#define HELP_WRATE_MSG   "--wrate -r/-w rate  get/set the wireless rate\n\r"
#define HELP_WAN_MSG    "--wan WanPortName ip_addr netmask		build static link\n\r"
#define HELP_ETH_TEST  "--eth -port[n]\n\r"
#define HELP_SN_MSG    "--sn -r/-w serial number		read /write serial number\n\r"
#define HELP_USB_MSG1    "--usb1		get message if there are some usb devices\n\r"
#define HELP_USB_MSG2    "--usb2		get message if there are some usb devices\n\r"
#define HELP_HARDV_MSG    "--hardware -r/-w <hardware version>		get the hardware version\r\n"
#define HELP_SOFTV_MSG    "--software -r		get the software version\r\n"
#define HELP_RESTORE_MSG    "--restore  -r/-f restore factory setting -r read the restore mode -f recover to factory mode\n\r"
#define HELP_HELP_MSG   "--help		protest command help\n\r"
#define HELP_FIRMWARE_MSG "--firmware -r    get the CPE firmware\n\r"
#define HELP_WMM_MSG    "--wmm -w 1/0   shutdown/open the wireless Qos function\n\r"
#define HELP_PPPOEUSR_MSG   "--pppoeusr -r/-w usrname   get/set the pppoe username\n\r"
#define HELP_PPPOEPW_MSG    "--pppoepw -r/-w passwd     get/set the pppoe password\n\r"
#define HELP_HIDE_SSID_MSG  "--hssid -w 1/0  hide the ssid or show the ssid \n\r"
#define HELP_CONSOLEPWD_MSG	"--consolepwd -r/-w consolepwd		read consolepwd/set consolepwd\n\r"
#define HELP_RESTORE_TEST_MSG "--testrestore -on/resume		product test restore default function; resume test restore normal function!\n\r"

/******************************************************************************
*                                 ENUM                                       *
******************************************************************************/
#define NOT_NVRAM_NODE -1

typedef struct btn_s {
	int gpio;
	int assertlvl;
	struct timespec start_ts;
	unsigned long prev_assertval;
	btn_press_t status;
	char name[16];
} btn_t;

enum ret_result
{
	PTI_RET_OK = 0,
	PTI_RET_FALSE,
};

enum
{
    WRITE_ENABLE = 0,
    WRITE_DISABLE,
};

enum
{
    FUNC_ENABLE = 0,
    FUNC_DISABLE,
};

enum
{
	NV_2G_SSID_OPTION = 0,
	NV_2G_WLANPWD_OPTION,
	NV_2G_CHANNAL_OPTION,
	NV_2G_WMM_OPTION,
	NV_2G_WCODE_OPTION,
	NV_2G_WRATE_OPTION,
	NV_2G_HIDE_SSID_OPTION,

	NV_5G_SSID_OPTION,
	NV_5G_WLANPWD_OPTION,
	NV_5G_CHANNAL_OPTION,
	NV_5G_WMM_OPTION,
	NV_5G_WCODE_OPTION,
	NV_5G_WRATE_OPTION,
	NV_5G_HIDE_SSID_OPTION,

	NV_PIN_OPTION,//pin

	NV_REGION_CODE_OPTION,
	NV_MAC_OPTION,
	NV_ADMINPWD_OPTION,
	NV_DEVICEID_OPTION,
    NV_HARDVERSION_OPTION,
	NV_SOFTVERSION_OPTION,
	NV_FIRMWARE_OPTION,

	NV_PPPOE_USRNAME_OPTION,
	NV_PPPOE_PASSWORD_OPTION,
	NV_CONSOLE_PASSWORD_OPTION,
};

enum
{
	PTI_WRITE_OPERATE = 0,
	PTI_READ_OPERATE,
};

typedef enum  {
	led_power	= 0,
	led_power_green,	/* 1 */
	led_power_red,
	led_internet,
	led_internet_green,
	led_internet_red,	/* 5 */
	led_usb,
	led_wlan,
	led_wps,
	led_wps_green,
	led_wps_red,		/* 10 */
	led_wps_yellow,
	led_dsl,
	led_register,
	led_phone_1,
	led_phone_2,		/* 15 */
	led_line,
	led_prov_alm,
	led_end,		 	/* 数组结束 */
}led_name;
enum
{
	PTI_LED_ON = 0,
	PTI_LED_OFF,
};
typedef enum  {
	led_off_trig	= 0,
	led_on_trig,
	led_flash_trig,
	led_flash_4hz_trig,
	led_dataflash_trig,
	led_wps_inprogress_trig,
	led_wps_error_trig,
	led_wps_overlap_trig,
	led_wps_success_trig,
	led_end_trig,
}trig_name;
#if 0
enum
{
	PTI_LED_ON = 0,
	PTI_LED_OFF,
};

enum
{
	PTI_BUTTON_BAN = 0,
	PTI_BUTTON_RESUME,
};

enum BUTTON_RECEIVE_STATE
{
    BTN_RECEIVE_MSG = 0,
    BTN_NOT_RECEIVE_MSG,
    BTN_RECEIVE_ERROR,
};
#endif

/******************************************************************************
*                                STRUCT                                      *
******************************************************************************/

/* 读写缓存数据结构 */
struct para_buf
{
	char readBuf[MAX_NODE_LENGTH];
	char writeBuf[MAX_NODE_LENGTH];
};

/* 功能列表 */
struct funcTable
{
    char  function[FUNC_FLAG_LENGTH];        //功能标识，如--led，--mac
    void (*func)(int arg, char **argc);      //功能函数，对某个标识调用相应的处理函数，如果是nvram变量读写操作，则可以作为nvram变量的检查函数
	char  *helpMsg;                          //help打印信息
	int   enable;                             //使能开关，对某个产品可以设置此标识，用来开启或者关闭此功能点
    int   NvPosition;                        //如果不是nvram变量操作，那么设置为0，如果是，则设置为1
};

/* nvram变量属性 */
struct nvTable
{
    int (*node_check)(char *nodeValue);  	//节点检测函数
    char nodePath[MAX_NODE_LENGTH];        	//值保存所在的nvram变量名
	int  enWrite;                           //nvram变量是否可写
	int  nodeNum; 
};



/* 建立WAN连接数据结构 */
struct wan_para
{
	char wanName[MAX_NAME_LEN];
	char ip_addr[IP_ADDR_LENGTH];
	char netmask[IP_ADDR_LENGTH];
};

/******************************************************************************
*                        FUNCTION DECLARE                                     *
******************************************************************************/
void PTI_RestoreFunc(int argc, char **argv);
void PTI_MacAddrFunc(int argc, char **argv);
void PTI_HelpFunc(int argc, char **argv);
void PTI_LedFunc(int argc, char **argv);
void PTI_EthFunc(int argc, char **argv);
void PTI_ButtonFunc(int argc, char **argv);
void PTI_SnFunc(int argc, char **argv);
void PTI_Usb_Cn1_Func(int argc, char **argv);
void PTI_Usb_Cn12_Func(int argc, char **argv);
void PTI_WlanPwdFunc(int argc, char **argv);
void PTI_RegionFunc(int argc, char **argv);
void PTI_CountryCodeFunc(int argc, char **argv);
void PTI_PinCodeFunc(int argc, char **argv);
void PTI_HardwareFunc(int argc, char ** argv);
void PTI_SoftwareFunc(int argc, char **argv);
void PTI_SoftwareExtFunc(int argc, char **argv);
void PTI_MyDlinkNumberFunc(int argc, char **argv);
void PTI_MyDlinkUIDFunc(int argc, char **argv);
void PTI_MyDlinkCertFunc(int argc, char **argv);
void PTI_fota_func(int argc, char * * argv);
void PTI_ConsolePwdFunc(int argc, char **argv);
void PTI_MyDlinkCheckFunc(int argc, char **argv);
void PTI_TestRestoreFunc(int argc, char **argv);

//void PTI_2g_Switch_FUNC(int argc, char **argv);
//void PTI_5g_Switch_FUNC(int argc, char **argv);

#endif
