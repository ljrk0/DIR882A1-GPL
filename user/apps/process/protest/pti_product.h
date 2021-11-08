
/**********************************************************************
 Copyright (c), 1991-2012, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 文件名称: pti_product.h
 文件描述: 产测功能配置文件
 修订记录:
        1. 作者: 苏邓海
           日期: 
           内容: 创建文件
**********************************************************************/

#ifndef _PTI_PRODUCT_H
#define _PTI_PRODUCT_H

//#include <bcmconfig.h>

/******************************************************************************
*                                STRUCT                                      *
******************************************************************************/

/* 产测支持的功能 */
struct funcTable funcNode[] = 
{
/***********************************非CMM节点****************************************/
//1. ######产测必须部分#######  
	{"--restore",   PTI_RestoreFunc,    HELP_RESTORE_MSG,   FUNC_ENABLE,    NOT_NVRAM_NODE},
	{"--mac",		PTI_MacAddrFunc,	HELP_MAC_MSG,		FUNC_ENABLE,	NOT_NVRAM_NODE},
    {"--led",       PTI_LedFunc,       HELP_LED_MSG,       FUNC_ENABLE,    NOT_NVRAM_NODE},
	{"--button",    PTI_ButtonFunc,    HELP_BUTTON_MSG,    FUNC_ENABLE,    NOT_NVRAM_NODE},
	{"--usb1",   PTI_Usb_Cn1_Func,        HELP_USB_MSG1,       FUNC_ENABLE,    NOT_NVRAM_NODE},
	{"--usb2",   PTI_Usb_Cn12_Func,        HELP_USB_MSG2,       FUNC_ENABLE,    NOT_NVRAM_NODE},
#if 0
	{"--wan",       NULL,    			HELP_WAN_MSG,       FUNC_ENABLE,    NOT_NVRAM_NODE},
#endif
	//add for check eth port
	{"--eth",		PTI_EthFunc,		HELP_ETH_TEST,		FUNC_ENABLE,	NOT_NVRAM_NODE},
	{"--sn",        PTI_SnFunc,    	HELP_SN_MSG,        FUNC_ENABLE,    NOT_NVRAM_NODE},
	{"--help",		PTI_HelpFunc,		HELP_HELP_MSG,		FUNC_ENABLE,	NOT_NVRAM_NODE},

	{"--mynum",		PTI_MyDlinkNumberFunc,	HELP_MYDLINKNUMBER_MSG,		FUNC_ENABLE,	NOT_NVRAM_NODE}, //wwzh add for mydlink number
	{"--myuid",		PTI_MyDlinkUIDFunc,	HELP_MYDLINKUID_MSG,		FUNC_ENABLE,	NOT_NVRAM_NODE},
	{"--mycert",	PTI_MyDlinkCertFunc,	HELP_MYDLINKCERT_MSG,		FUNC_ENABLE,	NOT_NVRAM_NODE},
	{"--fota",	PTI_fota_func,	HELP_FOTA_MSG,		FUNC_ENABLE,	NOT_NVRAM_NODE},
	
/***********************************CMM节点********************************************/	
//2. ######产测必须部分#######  
    {"--hardware", 	   PTI_HardwareFunc,     HELP_HARDV_MSG,     	FUNC_ENABLE,    NV_HARDVERSION_OPTION},
	{"--software",     PTI_SoftwareFunc,     HELP_SOFTV_MSG,     	FUNC_ENABLE,    NV_SOFTVERSION_OPTION},
	{"--software_ext", PTI_SoftwareExtFunc,  HELP_SOFTV_MSG,     	FUNC_ENABLE,    NV_SOFTVERSION_OPTION},
#if 0
	{"--firmware",     NULL,     HELP_FIRMWARE_MSG,  	FUNC_ENABLE,    NV_FIRMWARE_OPTION},
#endif
//3. ######与产品相关部分######
	{"--wlanpasswd",	  PTI_WlanPwdFunc, 	HELP_WLANPWD_MSG,		FUNC_ENABLE,	NV_2G_WLANPWD_OPTION},
#if 0
	//2.4G
	{"--2g_ssid",         NULL,     HELP_SSID_MSG,          FUNC_ENABLE,    NV_2G_SSID_OPTION},
	//{"--2g_switch",       PTI_2g_Switch_FUNC,     HELP_SWITCH_MSG,          FUNC_ENABLE,    NOT_NVRAM_NODE},
	//{"--2g_wlanpwd",      NULL,     HELP_WLANPWD_MSG,   	FUNC_ENABLE,    NV_2G_WLANPWD_OPTION},
	{"--2g_channal",      NULL,     HELP_CHANNAL_MSG,       FUNC_ENABLE,    NV_2G_CHANNAL_OPTION},	
	{"--2g_wmm",          NULL,     HELP_WMM_MSG,           FUNC_ENABLE,    NV_2G_WMM_OPTION},
	{"--2g_wrate",        NULL,     HELP_WRATE_MSG,   		FUNC_ENABLE,    NV_2G_WRATE_OPTION},
	{"--2g_wcode",        NULL,     HELP_WCODE_MSG,       	FUNC_ENABLE,    NV_2G_WCODE_OPTION},
	{"--2g_hssid",        NULL,     HELP_HIDE_SSID_MSG,     FUNC_ENABLE,    NV_2G_HIDE_SSID_OPTION},
	//5.8G
	{"--5g_ssid",         NULL,     HELP_SSID_MSG,          FUNC_ENABLE,    NV_5G_SSID_OPTION},
	//{"--5g_switch",       PTI_5g_Switch_FUNC,     HELP_SWITCH_MSG,          FUNC_ENABLE,    NOT_NVRAM_NODE},
	//{"--5g_wlanpwd",      NULL,     HELP_WLANPWD_MSG,   	FUNC_ENABLE,    NV_5G_WLANPWD_OPTION},
	{"--5g_channal",      NULL,     HELP_CHANNAL_MSG,       FUNC_ENABLE,    NV_5G_CHANNAL_OPTION},	
	{"--5g_wmm",          NULL,     HELP_WMM_MSG,           FUNC_ENABLE,    NV_5G_WMM_OPTION},
	{"--5g_wrate",        NULL,     HELP_WRATE_MSG,   		FUNC_ENABLE,    NV_5G_WRATE_OPTION},
	{"--5g_wcode",        NULL,     HELP_WCODE_MSG,       	FUNC_ENABLE,    NV_5G_WCODE_OPTION},
	{"--5g_hssid",        NULL,     HELP_HIDE_SSID_MSG,     FUNC_ENABLE,    NV_5G_HIDE_SSID_OPTION},
    //
#endif
	{"--pin",             PTI_PinCodeFunc,     HELP_PIN_MSG,     		FUNC_ENABLE,    NV_PIN_OPTION},
    //no wireless
	{"--countrycode",     PTI_CountryCodeFunc,     HELP_RESERVE2_MSG,  	FUNC_ENABLE,    NV_REGION_CODE_OPTION},
	{"--regioncode",	  PTI_RegionFunc,	  HELP_RESERVE1_MSG,	FUNC_ENABLE,	NV_REGION_CODE_OPTION},
	{"--consolepwd",	  PTI_ConsolePwdFunc, 	HELP_CONSOLEPWD_MSG,		FUNC_ENABLE,	NV_CONSOLE_PASSWORD_OPTION},
	#if 0
	{"--adminpwd",     	  NULL,     HELP_ADMINPWD_MSG,  	FUNC_ENABLE,    NV_ADMINPWD_OPTION},	
	{"--deviceid",     	  NULL,     HELP_DEVICEID_MSG,  	FUNC_ENABLE,    NV_DEVICEID_OPTION},
	{"--pppoeusr",     	  NULL,     HELP_PPPOEUSR_MSG,      FUNC_ENABLE,    NV_PPPOE_USRNAME_OPTION},
	{"--pppoepw",      	  NULL,     HELP_PPPOEPW_MSG,       FUNC_ENABLE,    NV_PPPOE_PASSWORD_OPTION},
#endif
};

#endif /*_PTI_PRODUCT_H*/
