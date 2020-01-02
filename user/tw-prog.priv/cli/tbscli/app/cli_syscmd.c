/****************************************************************************
 文件名  : CLI_Syscmd.c
 作者    : liuzequn
 版本    :
 完成日期:
 文件描述: 注册命令行模块与系统信息的一些基本管理命令，在命令行模块的初始化
           过程中被引用
 修改历史:
        1. 修改者   :
           时间     :
           版本     :
           修改原因 :
 ****************************************************************************/

#include "../cli_pub.h"
#include "../cli/cli_term.h"
#include "../cli/cli_io.h"
#include "../cli/cli_interpret.h"
#include "cli_cmd.inc"

#include "../cli_comm.h"
#include "../include/tbsutil.h"
#include "../include/cfg_api.h"
#include <sys/sysinfo.h>
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
 *                                 MACRO                                      *
 ******************************************************************************/
//WLAN用到的命令

#define CLI_WLAN_CONFIG_ENCRYPTION_WEP "{wep <wepkeylength(64bits|128bits)> <authentication(open|sharekey)> <wepkeyindex(u1~4)> <wepkeyvalue(s26)>}"
#define CLI_WLAN_CONFIG_ENCRYPTION_WPA_ENTERPRISE "{ent <interval(u60~1000)> <server-ip(p)> <port(u1025~65535)> <secret(s64)>}"
#define CLI_WLAN_CONFIG_ENCRYPTION_WPA_PSK "{psk <interval(u60~1000)> <pre-shared-key(s63)>}"
#define CLI_WLAN_CONFIG_ENCRYPTION_WPA "{wpa <wpa-version(u1~2)> <wpa-encryption(TKIP|AES|TKIPAES)> {"CLI_WLAN_CONFIG_ENCRYPTION_WPA_PSK"|"CLI_WLAN_CONFIG_ENCRYPTION_WPA_ENTERPRISE"}}"
#define CLI_WLAN_CONFIG_ENCRYPTION "{encryption {"CLI_WLAN_CONFIG_ENCRYPTION_WEP"|"CLI_WLAN_CONFIG_ENCRYPTION_WPA"}}"
#define CLI_WLAN_CONFIG_BASIC "{basic [ssid <ssid-v(s256)>] [channel <channel-v(u0~13)>] [8211mode <8211mode-v(s)>] }"
#define CLI_WLAN_CMD_FORMAT "wlan: {show <interface(u1~4)> <name(ssid|channel|wep|wpa|8211mode|encrypt|status)>}|{noencryption <interface(u1~4)>}|{enable [interface <interface-value(u1~4)>]}|{disable [interface <interface-value(u1~4)>]}|{config <interface(u1~4)> {" CLI_WLAN_CONFIG_BASIC "|" CLI_WLAN_CONFIG_ENCRYPTION "}}"

//MAC用到的命令
#define CLI_WLAN_MAC_ALLDAYS "{alldays}"
#define CLI_WLAN_MAC_DAYS    "{days <days-v(s27)>}"
#define CLI_WLAN_MAC_CMD_FORMAT "mac: {show <mac-v(c)/all>}|{del <mac-v(c)>}|{add <mac-v(c)> <description(s256)> <starttime(t)> <endtime(t)> {" CLI_WLAN_MAC_DAYS "|" CLI_WLAN_MAC_ALLDAYS "}}"
#define CLI_WIRELESS_MAC_CMD_FORMAT "wmac: {enable}|{disable}|{show}|{del <mac-v(c)>}|{add <mac-v(c)>}"


//DDNS用到的命令
#define CLI_DDNS_ADD_TZO    "{tzo <connection-name(s)> <hostname(s)> <e-mail(s)> <key(s)>}"
#define CLI_DDNS_ADD_DYNDNS "{dyndns <connection-name(s)> <hostname(s)> <username(s)> <password(s)>}"
#define CLI_DDNS_ADD_GNUDIP "{gnudip <connection-name(s)> <hostname(s)> <ddns-server(s)> <username(s)> <password(s)>}"
#define CLI_DDNS_CMD_FORMAT "ddns: {del <hostname(s256)>}|{show}|{commit}|{cancel}|{add {" CLI_DDNS_ADD_TZO "|" CLI_DDNS_ADD_DYNDNS "|" CLI_DDNS_ADD_GNUDIP "}}"

//NATP command
#define CLI_NATP_CMD_FORMAT "natp: {add <server(s256)> <wan(s256)> <protocol(tcp|udp)> <ex-start(u1~65535)> <ex-range(u1~65535)> <internalport(u1~65535)> <local(p)> [schedule <name(Always|Never)>] [remote <IP(p)>]}|{del <server(s256)> <wan(s256)>}|{show}"

//dns command
#define CLI_DNS_CMD_FORMAT "dns: {set <wan(s256)> <server(s256)>}|{show}"

//Routing command
#define CLI_ROUTE_CMD_FORMAT "route: {show}|{dynamic set <wan(s256)> <rip-version(Off|RIPv1|RIPv2)>}|{static {{add <dest-network(p)> <subnet(m)> <gateway-ip(p)> <wan(s256)>}|{del <dest-network(p)> <subnet(m)>}}}"

//WAN command
#define CLI_WAN_CMD_PVC_VLAN  "[vpi <vpi-v(u0~255)>][vci <vci-v(u32~65535)> ][vlan <vlan-v(u0~65535)>][vlanpri <vlanpri-v(u0~7)>]"
#define CLI_WAN_CMD_PPPOE  "[username <username-value(s64)>][password <password-value(s64)>][dialup <dialup-value(AlwaysOn|OnDemand|Manual)>] [pppidletimeout <pppidletimeout-value(u1~3600)>][pppmru <pppmru-value(u128~1540)>][service <service-name(s256)>]"
#define CLI_WAN_CMD_MER    "[ipaddr <ipaddr-value(p)>][netmask <netmask-value(m)>][remoteip <remoteip-value(p)>] [dnsip <dnsip-value(s64)>]"							
#define CLI_WAN_CMD_ADD	 "{add <protocol(bridge|pppoe|pppoa|mer|ipoa)> <mode-v(ATM|PTM|Ethernet)>"\
							CLI_WAN_CMD_PVC_VLAN \
							"[encap <encap-value(LLC|VCMUX)>] [serv <serv-v(UBRPlus|UBR|CBR|VBR-nrt|VBR-rt)>][nat <nat-value(enable|disable)>]"\
							CLI_WAN_CMD_PPPOE CLI_WAN_CMD_MER "}"
							
#define CLI_WAN_CMD_MODIFY	"{modify <pvc(s256)><protocol(bridge|pppoe|pppoa|mer|ipoa)><mode-v(ATM|PTM|Ethernet)>" \
							CLI_WAN_CMD_PVC_VLAN \
							"[encap <encap-value(LLC|VCMUX)>] [nat <nat-value(enable|disable)>]" \
							CLI_WAN_CMD_PPPOE CLI_WAN_CMD_MER "}"
#define CLI_WAN_CMD_SET     "{set [ppp <pvc(s256)>] [vpi <vpi-v(u0~255)>][vci  <vci-v(u32~65535)>][username <username-value(s64)>][password <password-value(s64)>]}"
#define CLI_WAN_CMD_FORMAT "wan: " CLI_WAN_CMD_ADD "|" CLI_WAN_CMD_MODIFY "|" CLI_WAN_CMD_SET "|{del <pvc(s256)>}|{show [conn <pvc(s256)>]}|{get [ppp <pvc(s256)>]}"

//Admin command
#define CLI_ADMIN_CMD_FORMAT "password: {change <userlevel(admin|user|support)> <opassword(s256)> <nusername(s256)> <npassword(s256)>}|{show}"
extern ST_CLI_MODE m_sCliSysModes[MAX_MODE_NUM];    /* 命令树 */

//Modem Information显示用到的命令
#define CLI_MODEM_CMD_FORMAT "modem:{showall}|{brand}|{model}|{version}|{date}"

//Device Information显示用到的命令
#define CLI_DEVICE_CMD_FORMAT "device:{showall}|{soc}|{adsl}|{ram}|{flash}"

//Dhcpserver 用到的命令
#define CLI_DHCPSERVER_LAN_PORT "{lan<interface(u1~4)><enable-disable(enable|disable)>}"
#define CLI_DHCPSERVER_WLAN_PORT "{wlan<interface(u1~4)><enable-disable(enable|disable)>}"

/* 命令行模块的命令信息库结构定义 */
ST_CLI_CMD_REG m_CliCmdPool[] =
{
    /*命令串                   对象1帮助信息         对象2帮助信息    对象3帮助信息 */
    {"cls",                   CMDHELP_GLB_CLS,      CLI_ML_NULL,       CLI_ML_NULL,
    /* 执行函数指针            命令等级              命令模式    */
      CLI_CmdClearScreen,      CLI_AL_QUERY,          CTM_GLOBAL },

    /* 退出命令行系统 */
    {"Exit",                  CMDHELP_GLB_EXIT,    CLI_ML_NULL,       CLI_ML_NULL,
      CLI_CmdExit,             CLI_AL_QUERY,          CTM_GLOBAL},
#if 0
    /* 自动/手动滚屏的开关设置命令 */
    {"set scroll: {automatic}|{manual}",
      CMDHELP_GLB_SET,    CMDHELP_GLB_SET_SCROLL,          CLI_ML_NULL,
      CLI_CmdSetScroll,      CLI_AL_QUERY,          CTM_GLOBAL },

		/* ADSL获取bits和snr ,status */
    {"adsl: {bits}|{snr}|{status}",
      CMDHELP_GLB_ADSL,    CMDHELP_GLB_ADSL_CMD,          CLI_ML_NULL,
      CLI_CmdAdsl,      CLI_AL_QUERY,          CTM_GLOBAL },

    /* 显示命令树 */
    {"help",     CMDHELP_GLB_HELP,     CLI_ML_NULL, CLI_ML_NULL,
      CLI_CmdHelp,      CLI_AL_QUERY,          CTM_GLOBAL },


    /* 注册测试命令 */
    {"ifconfig: [set <interface(u1~2)> <ip(p)> <mask(m)> <gateway(p)>]",
      CMDHELP_GLB_IF_CONFIG,   CLI_ML_NULL,   CLI_ML_NULL,
      CLI_IfConfig,          CLI_AL_QUERY,          CTM_GLOBAL},

    /* 注册测试命令 */
    {"debug: {del <path(s)>}|{add <path(s)>}|{get <path(s)>}|{set <path(s)> <value(s)>}",
      CMDHELP_GLB_DEBUG,   CLI_ML_NULL,   CLI_ML_NULL,
      CLI_Debug,          CLI_AL_QUERY,          CTM_GLOBAL},

    /* 注册测试命令 */
    {"save",
      CMDHELP_GLB_SAVE_CFG,   CLI_ML_NULL,   CLI_ML_NULL,
      CLI_SaveCfg,          CLI_AL_QUERY,          CTM_GLOBAL},

	/*注册保存退出*/
	
	    {"save_and_exit",
      HELP_FUN "save config and exit",   CLI_ML_NULL,   CLI_ML_NULL,
      CLI_Save_And_Exit,          CLI_AL_QUERY,          CTM_GLOBAL},

    /* 注册测试命令 */
    {"ipt: {add <path(s)>}|{del <path(s)>}|{get <path(s)>}|{set <path(s)> <value(s)>}|{commit}|{cancel}|{wanconnect}|{wandisconnect}",
          CMDHELP_GLB_IPT,   CLI_ML_NULL,   CLI_ML_NULL,
          CLI_Ipt,          CLI_AL_QUERY,          CTM_GLOBAL},

	{"dmz: {add <name(s)> <ip(p)>}|{del <name(s)> }|{show}",
    	 CMDHELP_GLB_DMZ,   CLI_ML_NULL,     CLI_ML_NULL,
    	 CLI_WAN_DMZ,     CLI_AL_QUERY,    	  CTM_GLOBAL},

    {CLI_DDNS_CMD_FORMAT,
          CMDHELP_GLB_DDNS,   CLI_ML_NULL,   CLI_ML_NULL,
          CLI_Ddns,          CLI_AL_OPERATOR,        CTM_GLOBAL},
    {CLI_DNS_CMD_FORMAT,
          CMDHELP_GLB_DNS,   CLI_ML_NULL,   CLI_ML_NULL,
          CLI_Dns,          CLI_AL_OPERATOR,        CTM_GLOBAL},
    {CLI_NATP_CMD_FORMAT,
          CMDHELP_GLB_NATP,   CLI_ML_NULL,   CLI_ML_NULL,
          CLI_Natp,          CLI_AL_OPERATOR,        CTM_GLOBAL},
    {CLI_ROUTE_CMD_FORMAT,
          CMDHELP_GLB_ROUTE,  CLI_ML_NULL,   CLI_ML_NULL,
          CLI_Route,         CLI_AL_OPERATOR,        CTM_GLOBAL},
    {CLI_WAN_CMD_FORMAT,
          CMDHELP_GLB_WAN,    CLI_ML_NULL,   CLI_ML_NULL,
          CLI_Wan,           CLI_AL_OPERATOR,        CTM_GLOBAL},
    {CLI_ADMIN_CMD_FORMAT,
          CMDHELP_GLB_ADMIN,    CLI_ML_NULL,   CLI_ML_NULL,
          CLI_Admin,           CLI_AL_OPERATOR,        CTM_GLOBAL},

		/*注册lan_ipinterface */
    	{"lan: {config <interface(u1~2)> <ip(p)> <mask(p)>}|{enable <interface(u1~2)/all>}|{disable <interface(u1~2)/all>}|{show <interface(u1~2)/all>}",
    	        CMDHELP_GLB_LAN_IPINTERFACE,   CLI_ML_NULL,     CLI_ML_NULL,
    	        CLI_LAN_IPInterface,     CLI_AL_QUERY,    	  CTM_GLOBAL},

		/*注册dhcpserver命令*/
	 {"dhcpserver: {config <start-IP-address(p)> <end-IP-address(p)> <lease-time(u0~4294967295)> <dnsserver(p)>}|{enable}|{disable}|{show}|{port{"CLI_DHCPSERVER_LAN_PORT"|"CLI_DHCPSERVER_WLAN_PORT"}}",
               CMDHELP_GLB_DHCPSERVER,    CLI_ML_NULL,    CLI_ML_NULL,
               CLI_DHCPServer,     CLI_AL_QUERY,     CTM_GLOBAL},
          
	    /*注册WLAN命令*/
	{CLI_WLAN_CMD_FORMAT,
               CMDHELP_GLB_WLAN,    CLI_ML_NULL,  CLI_ML_NULL,
       	  CLI_WLAN,	  CLI_AL_QUERY,   	  CTM_GLOBAL},


	/*注册MAC命令*/
	{CLI_WLAN_MAC_CMD_FORMAT,
  	  	CMDHELP_GLB_MAC,    CLI_ML_NULL,  CLI_ML_NULL,
      	       CLI_WLAN_MAC,   CLI_AL_QUERY,   CTM_GLOBAL},

	/*注册Wireless MAC命令*/
	{CLI_WIRELESS_MAC_CMD_FORMAT,
  	  	CMDHELP_GLB_WIRELESS_MAC,    CLI_ML_NULL,  CLI_ML_NULL,
      	       CLI_WIRELESS_MAC,   CLI_AL_QUERY,   CTM_GLOBAL},
#endif 
      	/*注册sh命令*/
      	{"sh",
      	      HELP_FUN "go to shell platform",   CLI_ML_NULL,  CLI_ML_NULL,
      	       CLI_Shell,             CLI_AL_QUERY,   CTM_GLOBAL},
#if 0   
    /*注册sh命令*/
         {"sh",
      	      HELP_FUN,   CLI_ML_NULL,  CLI_ML_NULL,
      	       CLI_Shell,             CLI_AL_QUERY,   CTM_GLOBAL},
         {"shell: {mode}|{admin<switch(open|close)>}",
      	      HELP_FUN,   CLI_ML_NULL,  CLI_ML_NULL,
      	       CLI_Shell,             CLI_AL_QUERY,   CTM_GLOBAL},    //mode用于迷惑敌人，本身无用

	/*注册restore_defaults命令*/
	   {"restore_defaults",
      	     HELP_FUN "load default factory config",   CLI_ML_NULL,  CLI_ML_NULL,
      	      CLI_Restore_Defaults,             CLI_AL_QUERY,   CTM_GLOBAL},

	/*注册REBOOT命令*/
	   {"reboot",
      	     HELP_FUN "reboot",   CLI_ML_NULL,  CLI_ML_NULL,
      	      CLI_Reboot,             CLI_AL_QUERY,   CTM_GLOBAL},
      	 /*注册save-and-reboot命令*/
	   {"save_and_reboot",
     	     HELP_FUN "save and reboot",   CLI_ML_NULL,  CLI_ML_NULL,
      	      CLI_Save_And_Reboot,             CLI_AL_QUERY,   CTM_GLOBAL},

         /* 注册modem命令 */
	   {CLI_MODEM_CMD_FORMAT,
     	     CMDHELP_GLB_MODEM,   CLI_ML_NULL,  CLI_ML_NULL,
      	      CLI_Modem,             CLI_AL_QUERY,   CTM_GLOBAL},
#endif
         /* 注册device命令 */
	   {CLI_DEVICE_CMD_FORMAT,
     	     CMDHELP_GLB_DEVICE,   CLI_ML_NULL,  CLI_ML_NULL,
      	      CLI_Device,             CLI_AL_QUERY,   CTM_GLOBAL} ,   
        /* 注册meminfo命令 */
	   {"meminfo",
     	     HELP_FUN "Memory usage",   CLI_ML_NULL,  CLI_ML_NULL,
      	      CLI_Meminfo,             CLI_AL_QUERY,   CTM_GLOBAL} ,   

         /* 注册cpuinfo命令 */
	   {"cpuinfo",
     	     HELP_FUN "CPU usage",   CLI_ML_NULL,  CLI_ML_NULL,
      	      CLI_Cpuinfo,             CLI_AL_QUERY,   CTM_GLOBAL} , 
      	   /* 注册nat session命令 */
	   {"natinfo",
     	     HELP_FUN "Nat usage",   CLI_ML_NULL,  CLI_ML_NULL,
      	      CLI_NATinfo,             CLI_AL_QUERY,   CTM_GLOBAL} , 

#if 0
         /* 注册TR069命令 */
	   {"tr069:{config [status <Enable-value(enable|disable)>] [InfEnable <InfEnable-v(enable|disable)>] [Inform <Inform-Interval(u0~100000)>] [URL <URL-v(s256)>][username <username-v(s256)>] [password <password-v(s256)>]}|{show}|{request [RequestStatus <Enable-value(enable|disable)>] [username <RequestUsername(s256)>] [password <RequestPassword(s256)>][path <RequestPath(s256)>][port <RequestPort(u0~100000)>]}",
     	     CMDHELP_GLB_TR069,   CLI_ML_NULL,  CLI_ML_NULL,
      	      CLI_TR069,             CLI_AL_QUERY,   CTM_GLOBAL} ,   
        
		{"pmirror: [vpi <vpival(u0~255)>] [vci <vcival(u0~255)>] {flags <flagsval(u0~3)>}",
		CMDHELP_GLB_PMIRROR,   CLI_ML_NULL,  CLI_ML_NULL,
      	      CLI_PortMirror,     CLI_AL_QUERY,   CTM_GLOBAL},
      	{"acl: <conname(s)> <contype(s)> <port(s)> <optname(s)>",
			CMDHELP_GLB_ACL,   CLI_ML_NULL,  CLI_ML_NULL,
      	      CLI_Acl,     CLI_AL_QUERY,   CTM_GLOBAL}
#endif
};




/*********************************************************************/
/* 函数名称 : CLI_SysCmdReg()                                           */
/* 函数功能 : 命令行模块的命令注册                                   */
/* 输入参数 : 无                                                     */
/* 输出参数 : 无                                                     */
/* 返回     : 成功、失败                                             */
/* 上层函数 : 命令行模块初始化函数                                   */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_SysCmdReg(_VOID   )
{
    ULONG  i;
    ULONG  ulRet = 0;
    for (i = 0; i < CLI_ARRAY_SIZE(m_CliCmdPool); i++)
    {
        ulRet += CLI_CmdRegist(m_CliCmdPool[i].szCmd,     //格式化串
                                  m_CliCmdPool[i].ucMode,    //命令所属模式
                                  (CLI_OPERAT_LEVEL_T)m_CliCmdPool[i].ucLevel,   //命令使用等级
                                  m_CliCmdPool[i].pFunc,     //命令执行函数
                                  m_CliCmdPool[i].strHelp1,   //第一个对象的帮助信息
                                  m_CliCmdPool[i].strHelp2,
                                  m_CliCmdPool[i].strHelp3    );

    }
    if (ulRet != 0)
    {
        STB_Printf("\n Some system command register failed !!");
        return TBS_FAILED;
    }
    return TBS_SUCCESS;
}



/*※※※※※※※※※※※※※※*/
/* 命令行的的命令实现函数     */
/*※※※※※※※※※※※※※※*/
/*********************************************************************/
/* 函数功能 : 屏幕清除功能命令实现                                   */
/*********************************************************************/
ULONG  CLI_CmdClearScreen()
{
#if 0
    ST_CLI_TERM* lpTermStruc;
    int  iSavePos;
    ULONG  ulLine;

    if ( (lpTermStruc = CLI_GetTermData()) == NULL )
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    for (ulLine = 0; ulLine <= LINE_PER_PAGE + 1; ulLine++)
    {
        IO_OutString("\r\n");
        lpTermStruc->ulDispLineCount = 0;
    }

    iSavePos = lpTermStruc->iCurrentPos;
    lpTermStruc->iCurrentPos = 0;
    CLI_MoveToCurrentPos(80*24);
    CLI_SendToTerm();
    lpTermStruc->iCurrentPos = iSavePos;
#endif
    IO_Print("\33[2J");
    return TBS_SUCCESS;

}

ULONG  CLI_CmdExit()
{
    ST_CLI_TERM* lpTermStruc;

    if (IO_GetConfirm(CLI_LOGOUT_CONFIRM, INPUT_NO) != INPUT_YES)
        return CLI_CMD_FAILED_SILENT;

    if ( (lpTermStruc = CLI_GetTermData()) != NULL )
    {
        CLI_TermLogout();
    }

    return TBS_SUCCESS;
}

/*********************************************************************/
/* 函数名称 : CLI_Restore_Defaults()                                           */
/* 函数功能 : 命令行模块的恢复出厂设置                                   */
/* 输入参数 : 无                                                     */
/* 输出参数 : 无                                                     */
/* 返回     : 成功、失败                                             */
/* 上层函数 : 命令行模块初始化函数                                   */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
#if 0
ULONG CLI_Restore_Defaults()
{

        ST_MSG         *pstMsg;

     pstMsg = MSG_CreateMessage(0);
     if (pstMsg == NULL)
    {
        CLI_ComErrorAdd("Out of Memory");
        return TBS_FAILED;
    } 
    
    pstMsg->stMsgHead.ulMsgID   = 0;
    pstMsg->stMsgHead.usSrcMID  = MID_CLI;
    pstMsg->stMsgHead.usDstMID  = MID_CMM;
    pstMsg->stMsgHead.usMsgType = MSG_CMM_RECOVER_CFG;
    
    MSG_SendMessage(pstMsg);
    MSG_ReleaseMessage(pstMsg);

    return TBS_SUCCESS;
}
#endif
/*********************************************************************/
/* 函数功能 : 设置手动滚屏的命令实现                                 */
/*********************************************************************/
ULONG  CLI_CmdSetScroll()
{
    ST_CLI_TERM* lpTermStruct;
    char  *pParam;

    if ( (lpTermStruct = CLI_GetTermData()) == NULL )
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    /* 透传命令的处理任务不支持滚屏方式的修改 */
    if (lpTermStruct->nTermType == CLI_TERMT_FOR_MML)
        return TBS_SUCCESS;

    if ((pParam = CLI_GetParamByName("automatic|manual")) == NULL)
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    if (!STB_StriCmp(pParam, "automatic"))
    {
        lpTermStruct->ucScroll = SCROLL_AUTO;
        IO_Print(CLI_SCROLL_DISABLE);
        return TBS_SUCCESS;
    }
    else if (!STB_StriCmp(pParam, "manual"))
    {
        lpTermStruct->ucScroll = SCROLL_MANUAL;
        IO_Print(CLI_SCROLL_ENABLE);
        return TBS_SUCCESS;
    }

    return TBS_FAILED;
}


/*********************************************************************/
/* 函数功能 : 判断对象是否需要在help中显示                           */
/*********************************************************************/
BOOL CLI_IsObjNeedShow(ULONG  ulMode, PST_CLI_CMDKEY pCmdKey, ULONG  ulLevel)
{
    PST_CLI_CMDKEY pObj;

    pObj = pCmdKey;
    if (pObj == NULL)
        return FALSE;

    if (pObj->action.rightLevel > (CLI_OPERAT_LEVEL_T)ulLevel)
        return FALSE;

    //如果即不是当前模式，又不是全局模式的对象，不予显示
    if (ulMode != pObj->action.ulMode && CTM_GLOBAL != pObj->action.ulMode)
    {
        return FALSE;
    }

    if (ulMode == pObj->action.ulMode)
        return TRUE;
    pObj = pObj->pSubCmdKey;

    while (pObj != NULL)
    {
        if (pObj->action.ulMode == ulMode
         && pObj->action.rightLevel <= (CLI_OPERAT_LEVEL_T)ulLevel)
            return TRUE;

        if (pObj->pSubCmdKey != 0)
            pObj = pObj->pSubCmdKey;
        else if (pObj->pNext != 0)
            pObj = pObj->pNext;
        else
        {
            if (pObj->pParent == pCmdKey)
                break;

            while (pObj->pParent->pNext == NULL)
            {
                pObj = pObj->pParent;
                if (pObj->pParent == pCmdKey)
                    return FALSE;
            }
            pObj = pObj->pParent->pNext;
        }
    }

    return FALSE;
}

/*********************************************************************/
/* 函数功能 : 在help命令中显示指定的对象信息                         */
/*********************************************************************/
ULONG  CLI_ObjectShow(ULONG  ulMode, ULONG  ulLevel, PST_CLI_CMDKEY pCmdKey)
{
    ULONG  ulBlank[5] = {0, 0, 0, 0, 0};
    ULONG  ulBlankTotal = 0, i, j;
    ULONG  ulObjNum = 0;
    PST_CLI_CMDKEY pObj, pObjTmp;
    char  szInfo[256];
    char  szHelp[128];
    ULONG  ulPos = 0;
    char  *szOutput;
    ULONG  ulRet = TBS_SUCCESS;


    if (!CLI_IsObjNeedShow(ulMode, pCmdKey, ulLevel))
    {
        return TBS_FAILED;
    }

    if ((szOutput = CLI_Malloc(4096))
         == NULL)
         return TBS_FAILED;

    szOutput[0] = '\0';
    szInfo[0] = '\0';

    pObj = pCmdKey;

    while (pObj != NULL)
    {
        if (pObj->action.rightLevel <= (CLI_OPERAT_LEVEL_T)ulLevel
            && (pObj->action.ulMode == ulMode || pObj->action.ulMode == CTM_GLOBAL)
            && (CLI_IsObjNeedShow(ulMode, pObj, ulLevel)))
        {
            if (pObj == pCmdKey
             || (pObj->pPrevious == NULL && pObj->pNext == NULL))
            {
                STB_StrCat(szOutput, "  ");
            }
            else if (pObj->pPrevious == NULL)
            {
                STB_StrCat(szOutput, "+-");
            }
            else
            {
                i = 0, j = 0;
                //计算该对象的兄长对象
                pObjTmp = pObj->pPrevious;
                while (pObjTmp != NULL)
                {
                    if (CLI_IsObjNeedShow(ulMode, pObjTmp, ulLevel))
                        i++;
                    pObjTmp = pObjTmp->pPrevious;
                }

                //计算该对象的弟弟对象
                pObjTmp = pObj->pNext;
                while (pObjTmp != NULL)
                {
                    if (CLI_IsObjNeedShow(ulMode, pObjTmp, ulLevel))
                        j++;
                    pObjTmp = pObjTmp->pNext;
                }
                if ((i + j) == 0) //如果前后没有可显示的对象
                    STB_StrCat(szOutput, "  ");
                else if (i == 0) //如果该对象前的可显示对象个数为0，则该对象为第一个
                    STB_StrCat(szOutput, "+-");
                else
                    STB_StrCat(szOutput, "|-");
            }
            ulPos += 2;

            STB_Sprintf(szInfo, "%-7s", pObj->szName);
            STB_StrCat(szOutput, szInfo);
            ulPos += STB_StrLen(szInfo);

            if (!CLI_IsObjectExecutable(pObj, HAVEACTIONS))
            {
//                CLI_DELAY(1);
                CLI_GetObjectHelpString(pObj->action.strObjHelp, szHelp);

                if (ulPos <  38)
                {
                    for (i = 0; i < 38 - ulPos; i++)
                        STB_StrCat(szOutput, " ");
                }

                if (STB_StrLen(szHelp) > 40)
                {
                    ulBlankTotal = 0;
                    for (i = 0; i < 37; i++)
                    {
                        if ((UCHAR)szHelp[i] > 127)
                            ulBlankTotal ++;
                    }
                    if (ulBlankTotal % 2 == 0)
                        STB_StrCpy(szHelp + 37, "...");
                    else
                        STB_StrCpy(szHelp + 38, "...");
                }
                STB_StrCat(szOutput, szHelp);
            }
        }
        else if (pObj->action.ulMode != ulMode && ulMode == CTM_GLOBAL)
        {
            ulRet = 2;
        }

        if (pObj->pSubCmdKey != NULL)
        {
            ulBlank[ulObjNum++] = STB_StrLen(szInfo) + 2;
            pObj = pObj->pSubCmdKey;
        }
        else if (pObj == pCmdKey)
        {
            break;
        }
        else if (pObj->pNext != NULL)
        {
            if (pObj->action.rightLevel <= (CLI_OPERAT_LEVEL_T)ulLevel
                && (pObj->action.ulMode == ulMode || pObj->action.ulMode == CTM_GLOBAL)
                && (CLI_IsObjNeedShow(ulMode, pObj, ulLevel)))
            {
                STB_StrCat(szOutput, "\r\n");
                ulPos = 0;
            }
            else if (pObj->action.ulMode != ulMode && ulMode == CTM_GLOBAL)
            {
                ulRet = 2;
            }

            ulBlankTotal = 0;
            for (i = 0; i < ulObjNum; i++)
            {
                ulBlankTotal += ulBlank[i];
            }
            for (i = 0; i < ulBlankTotal; i++)
            {

                if (pObj->action.rightLevel <= (CLI_OPERAT_LEVEL_T)ulLevel
                    && (pObj->action.ulMode == ulMode || pObj->action.ulMode == CTM_GLOBAL)
                    && (CLI_IsObjNeedShow(ulMode, pObj, ulLevel)))
                {
                    if (ulObjNum == 2 && i == ulBlank[0] && pObj->pParent->pNext != NULL)
                        STB_StrCat(szOutput, "|");
                    else
                        STB_StrCat(szOutput, " ");
                    ulPos++;
                }
                else if (pObj->action.ulMode != ulMode && ulMode == CTM_GLOBAL)
                {
                    ulRet = 2;
                }
            }
            pObj = pObj->pNext;
        }
        else
        {
            while (pObj->pParent != pCmdKey
                && pObj->pParent->pNext == NULL)
            {
                pObj = pObj->pParent;
                --ulObjNum;

            }
            if (pObj->pParent == pCmdKey)
            {
                break;
            }

            if (pObj->pParent->pNext != NULL)
            {
                --ulObjNum;

                if (pObj->action.rightLevel <= (CLI_OPERAT_LEVEL_T)ulLevel
                    && (pObj->action.ulMode == ulMode || pObj->action.ulMode == CTM_GLOBAL)
                    && (CLI_IsObjNeedShow(ulMode, pObj, ulLevel)))
                {
                    STB_StrCat(szOutput, "\r\n");
                    ulPos = 0;
                }
                else if (pObj->action.ulMode != ulMode && ulMode == CTM_GLOBAL)
                {
                    ulRet = 2;
                }
                ulBlankTotal = 0;

                for (i = 0; i < ulObjNum; i++)
                {
                    ulBlankTotal += ulBlank[i];
                }
                for (i = 0; i < ulBlankTotal; i++)
                {
                    if (pObj->action.rightLevel <= (CLI_OPERAT_LEVEL_T)ulLevel
                        && (pObj->action.ulMode == ulMode || pObj->action.ulMode == CTM_GLOBAL)
                        && (CLI_IsObjNeedShow(ulMode, pObj, ulLevel)))
                    {
                        STB_StrCat(szOutput, " ");
                        ulPos++;
                    }
                    else if (pObj->action.ulMode != ulMode && ulMode == CTM_GLOBAL)
                    {
                        ulRet = 2;
                    }
                }
                pObj = pObj->pParent->pNext;
            }
            else
            {
                break;
            }
        }

    }
    STB_StrCat(szOutput, "\r\n");
    IO_OutString(szOutput);

    CLI_Free(szOutput);
    return ulRet;
}

/*********************************************************************/
/* 函数功能 : 命令树的列表显示                                       */
/*********************************************************************/
ULONG  CLI_CmdHelp()
{
    ULONG  ulMode;
    ULONG  ulObjIndex = 0;
    PST_CLI_CMDKEY pObj, pObjSave[5] = {NULL, NULL, NULL, NULL, NULL};
    ST_CLI_TERM* pTermStruct;
    ULONG  ulRet;

    pTermStruct = &m_stCliTerm;

    IO_Print(CLI_HELP_INFO);

    for (ulMode = 0; ulMode < CTM_TOTAL; ulMode ++)
    {
        if (m_sCliSysModes[ulMode].pCmdKey == NULL)
            continue;

        if (m_sCliSysModes[ulMode].rightLevel > pTermStruct->ucUserLevel)
            continue;

        pObj = m_sCliSysModes[ulMode].pCmdKey->pSubCmdKey;

        IO_OutString("\r\n\r\n --------------------------------\r\n");
        IO_Print(" Mode:%-15s  ", m_sCliSysModes[ulMode].szModeName);
        IO_OutString("\r\n --------------------------------\r\n");

        ulObjIndex = 0;

        while (pObj != NULL
            && (ulMode == CTM_GLOBAL || pObj != m_sCliSysModes[CTM_GLOBAL].pCmdKey->pSubCmdKey))
        {
            if (!STB_StriCmp(pObj->szName, "cm"))
            {
                pObj = pObj->pNext;
                continue;
            }

            if (IO_IsOutputBroken())
                return TBS_SUCCESS;

            //CLI_DELAY(1);

            //如果非全局模式下的命令大于全局模式的命令，
            //则先显示可能需要显示的全局模式命令
            if (ulMode != CTM_GLOBAL )
            {
                while (pObjSave[ulObjIndex] != NULL && STB_StriCmp(pObj->szName, pObjSave[ulObjIndex]->szName) > 0)
                {
                    CLI_ObjectShow(ulMode, (ULONG )pTermStruct->ucUserLevel, pObjSave[ulObjIndex]);
                    ulObjIndex++;
                }
            }

            //显示当前分支
            //help信息中隐藏掉sh 和shell命令提示
            if(strcmp(pObj->szName, "sh") == 0 || strcmp(pObj->szName, "shell") == 0)
            {
                pObj = pObj->pNext;
                continue;
            }
            ulRet = CLI_ObjectShow(ulMode, (ULONG )pTermStruct->ucUserLevel, pObj);
            if (2 == ulRet && ulMode == CTM_GLOBAL)
            {
                if (ulObjIndex < 5)
                {
                    pObjSave[ulObjIndex++] = pObj;
                }
            }
            pObj = pObj->pNext;

            if (pObj == NULL && ulObjIndex < 5 && pObjSave[ulObjIndex] != 0)
            {
                //如果显示完全了，但还有全局模式的遗留命令没有输出，尝试输出
                while (ulObjIndex < 5 && pObjSave[ulObjIndex] != 0)
                {
                    CLI_ObjectShow(ulMode, (ULONG )pTermStruct->ucUserLevel, pObjSave[ulObjIndex]);
                    ulObjIndex++;
                }
            }
        }
    }
    return TBS_SUCCESS;
}

#if 0
/*********************************************************************/
/* 函数功能 : 从任何模式进入调试模式的命令实现                       */
/*********************************************************************/
ULONG  CLI_IfConfig()
{
    int nIfNum, i;
    char *szPathName = "InternetGatewayDevice.WANDevice._0.WANConnectionDevice._0.WANIPConnection.";

    // 没有参数表示打印
    if (CLI_GetParamByName("set") == NULL)
    {
        char aszNextLevel[CLI_MAX_OBJ_NUM][CLI_LEAF__LEN];
        nIfNum = CLI_GetNextLevelName(szPathName, aszNextLevel);
        if (nIfNum < 0)
        {
            IO_Print("\n No configured wan interface.");
            return TBS_SUCCESS;
        }
        for (i = 0; i < nIfNum; i++)
        {
            char aszLeafName[3][CLI_LEAF__LEN] = {"ExternalIPAddress", "SubnetMask", "DefaultGateway"};
            char szObjName[256] ;
            ST_CLISTR_LINK *pStrList;

            sprintf(szObjName, "%s%s.", szPathName, aszNextLevel[i]);
            pStrList = CLI_GetObject(szObjName, 3, aszLeafName);

            if (pStrList != NULL)
            {
                IO_Print("\n ifwan%s:"
                         "\n     IP      : %s"
                         "\n     Mask    : %s"
                         "\n     Gateway : %s",aszNextLevel[i], pStrList->pstrValue,
                         pStrList->pNext->pstrValue, pStrList->pNext->pNext->pstrValue);
                CLI_DestroyStrLink(pStrList);
            }
        }
    }
    else
    {
        char   *szIfName, *szIP,*szMask, *szGateway;

        char aszName[4][CLI_NODE__LEN];
        char aszVal[4][CLI_NODE__LEN];

        szIfName = CLI_GetParamByName("interface");
        szIP     = CLI_GetParamByName("ip");
        szMask   = CLI_GetParamByName("mask");
        szGateway= CLI_GetParamByName("gateway");

        if (szIfName == NULL || szIP == NULL || szMask == NULL || szGateway == NULL)
        {
            IO_Print("Interface Parameter error.");
            return TBS_FAILED;
        }

        sprintf(aszName[0], "%s_%s.%s", szPathName, szIfName, "ExternalIPAddress");
        sprintf(aszName[1], "%s_%s.%s", szPathName, szIfName, "SubnetMask");
        sprintf(aszName[2], "%s_%s.%s", szPathName, szIfName, "DefaultGateway");
        sprintf(aszName[3], "%s_%s.%s", szPathName, szIfName, "AddressingType");

        strcpy(aszVal[0], szIP);
        strcpy(aszVal[1], szMask);
        strcpy(aszVal[2], szGateway);
        strcpy(aszVal[3], "static");

        if (CLI_SetLeaf(4, aszName, aszVal))
        {
            CLI_CommErrorPrint();
            return TBS_FAILED;
        }
    }

    return TBS_SUCCESS;
}


/*********************************************************************/
/* 函数功能 : 从任何模式进入调试模式的命令实现                       */
/*********************************************************************/
ULONG  CLI_EnterDiagMode()
{
    PST_CLI_WS pWS;
    char  *pParamNO;

    pWS = &m_stCliWS;

    IO_Print("Enter Diagnose Mode.");
    CLI_DELAY(50);
    if (pWS->ulCurrentMode == CTM_GENL)
        return TBS_SUCCESS;

    /* Write to pWS       */
    if(CLI_CmdPush(&(pWS->cmdStack), CTM_DIAG) != TBS_SUCCESS )
    {
        IO_Print(CLI_PUSH_CMD_FAILED);
        return TBS_FAILED;
    }
    pWS->ulCurrentMode = CTM_DIAG;
    pWS->ulParamIndex += m_sCliSysModes[CTM_DIAG].ulParamCount;

    pParamNO = CLI_GetModeParameter(CTM_DIAG, 2) ;
    CLI_EnterMode(CTM_DIAG, pParamNO) ;

    return TBS_SUCCESS;
}


/*********************************************************************/
/* 函数功能 : 获取或设置某个节点，调试用                             */
/*********************************************************************/
ULONG  CLI_Debug()
{
  char   *szPath, *szValue;

  // 没有参数表示打印
  if (CLI_GetParamByName("get") != NULL)
  {
    char aszValue[CLI_LEAF__LEN];
    szPath = CLI_GetParamByName("path");
    if (szPath == NULL)
    {
      IO_Print("Interface Parameter error.");
      return TBS_FAILED;
    }

    if (TBS_SUCCESS!=CLI_GetLeaf(szPath,aszValue))
    {
      CLI_CommErrorPrint();
      return TBS_FAILED;
    }
    IO_Print("\nValue=%s\n",aszValue);

  }
  else if (CLI_GetParamByName("set") != NULL)
  {
    char aszName[1][CLI_NODE__LEN];
    char aszVal[1][CLI_NODE__LEN];

    szPath = CLI_GetParamByName("path");
    szValue     = CLI_GetParamByName("value");

    if (szPath == NULL || szValue == NULL)
    {
      IO_Print("Interface Parameter error.");
      return TBS_FAILED;
    }

    strcpy(aszName[0],szPath);
    strcpy(aszVal[0],szValue);
    if (CLI_SetLeaf(1, aszName, aszVal))
    {
      CLI_CommErrorPrint();
      return TBS_FAILED;
    }
  }
  else if (CLI_GetParamByName("add") != NULL)
  {
    ULONG ulObjIndex = 0;

    szPath = CLI_GetParamByName("path");

    if (szPath == NULL)
    {
      IO_Print("path error.\n");
      return TBS_FAILED;
    }
    
    if (CLI_AddObjectEx(szPath, &ulObjIndex))
    {
      CLI_CommErrorPrint();
      return TBS_FAILED;
    }

    IO_Print("\n\t\tnew index:%lu\n", ulObjIndex);
  }
  else if (CLI_GetParamByName("del") != NULL)
  {
    szPath = CLI_GetParamByName("path");

    if (szPath == NULL)
    {
      IO_Print("path error.\n");
      return TBS_FAILED;
    }
    
    if (CLI_DelObject(szPath))
    {
      CLI_CommErrorPrint();
      return TBS_FAILED;
    }

    IO_Print("\n\t\tdelete %s success\n", szPath);
  }
  else
    return TBS_FAILED;
  return TBS_SUCCESS;
}

/*********************************************************************/
/* 函数功能 : 调试 IPT 模块用                                        */
/*********************************************************************/
ULONG  CLI_Ipt()
{
    char   *szPath, *szValue;

    // 没有参数表示打印
    if (CLI_GetParamByName("add") != NULL)
    {
        char aszValue[CLI_LEAF__LEN];
        szPath = CLI_GetParamByName("path");

        if (szPath == NULL)
        {
            IO_Print("Interface Parameter error.");
            return TBS_FAILED;
        }

        if (TBS_SUCCESS != CLI_AddObject(szPath))
        {
            CLI_CommErrorPrint();
            return TBS_FAILED;
        }

        IO_Print("Value=%s\n",aszValue);
    }
    else if (CLI_GetParamByName("del") != NULL)
    {
        char aszValue[CLI_LEAF__LEN];
        szPath = CLI_GetParamByName("path");

        if (szPath == NULL)
        {
            IO_Print("Interface Parameter error.");
            return TBS_FAILED;
        }

        if (TBS_SUCCESS != CLI_DelObject(szPath))
        {
            CLI_CommErrorPrint();
            return TBS_FAILED;
        }

        IO_Print("Value=%s\n",aszValue);
    }
    else if (CLI_GetParamByName("get") != NULL)
    {
        char aszValue[CLI_LEAF__LEN];
        szPath = CLI_GetParamByName("path");

        if (szPath == NULL)
        {
            IO_Print("Interface Parameter error.");
            return TBS_FAILED;
        }

        if (TBS_SUCCESS != CLI_GetLeaf(szPath, aszValue))
        {
            CLI_CommErrorPrint();
            return TBS_FAILED;
        }

        IO_Print("Value=%s\n",aszValue);
    }
    else if (CLI_GetParamByName("set") != NULL)
    {
        char aszName[1][CLI_NODE__LEN];
        char aszVal[1][CLI_NODE__LEN];

        szPath = CLI_GetParamByName("path");
        szValue     = CLI_GetParamByName("value");

        if (szPath == NULL || szValue == NULL)
        {
            IO_Print("Interface Parameter error.");
            return TBS_FAILED;
        }

        strcpy(aszName[0],szPath);
        strcpy(aszVal[0],szValue);

        if (TBS_SUCCESS != CLI_SetLeaf(1, aszName, aszVal))
        {
            CLI_CommErrorPrint();
            return TBS_FAILED;
        }
    }
    else if (CLI_GetParamByName("commit") != NULL)
    {
        IO_Print("Error: ipt commit /* Not available */");
        return TBS_FAILED;
    }
    else if (CLI_GetParamByName("cancel") != NULL)
    {
        IO_Print("Error: ipt cancel /* Not available */");
        return TBS_FAILED;
    }
    else if (CLI_GetParamByName("wanconnect") != NULL)
    {
        IO_Print("Error: ipt wanconnect /* Not available */");
        return TBS_FAILED;
    }
    else if (CLI_GetParamByName("wandisconnect") != NULL)
    {
        IO_Print("Error: ipt wandisconnect /* Not available */");
        return TBS_FAILED;
    }
    else if (CLI_GetParamByName("") != NULL)
    {
        IO_Print("Error: ipt /* Not available */");
        return TBS_FAILED;
    }
    else
    {
        IO_Print("Error: ipt <Unknown command> /* Unknown command */");
        return TBS_FAILED;
    }

    return TBS_SUCCESS;
}

ULONG CLI_SaveCfg()
{
    ST_MSG         *pstMsg;

     pstMsg = MSG_CreateMessage(0);
     if (pstMsg == NULL)
    {
        CLI_ComErrorAdd("Out of Memory");
        return TBS_FAILED;
    } 

    if (OM_FAILED(CLI_ComMsgInteract(pstMsg, MSG_CMM_SAVE_CFG)))
   {
        CLI_ComErrorAdd("Communcate failed");
        //在离开前，先释放已经申请的内存
        CLI_MSG_RELEASE(pstMsg); 

        return TBS_FAILED;
    }
	CLI_MSG_RELEASE(pstMsg); 
    return TBS_SUCCESS;
}

ULONG CLI_Save_And_Exit()
{
	 ULONG  ulRet = 0;
	ulRet = CLI_SaveCfg();
	if(ulRet == TBS_FAILED)
		IO_Print("save config error!\n");
	return CLI_CmdExit();
	
}

ULONG CLI_Reboot()
{
     ST_MSG         *pstMsg;

     pstMsg = MSG_CreateMessage(0);
     if (pstMsg == NULL)
    {
        CLI_ComErrorAdd("Out of Memory");
        return TBS_FAILED;
    } 
    
    pstMsg->stMsgHead.ulMsgID   = 0;
    pstMsg->stMsgHead.usSrcMID  = MID_CLI;
    pstMsg->stMsgHead.usDstMID  = MID_CMM;
    pstMsg->stMsgHead.usMsgType = MSG_CMM_REBOOT;
    
    MSG_SendMessage(pstMsg);
    MSG_ReleaseMessage(pstMsg);

    return TBS_SUCCESS;
}

ULONG CLI_Save_And_Reboot()
{
	 ULONG  ulRet = 0;
	ulRet = CLI_SaveCfg();
	if(ulRet == TBS_FAILED)
		IO_Print("save config error!\n");
	return CLI_Reboot();
}
#endif 
ULONG CLI_Shell()
{
    /* 开启终端回显 */
    CLI_TermEchoOn();

    printf("\n");
    system("sh");

    /* 关闭终端回显 */
    CLI_TermEchoOff();

    return TBS_SUCCESS;
}

#if 0
#define NODE_ShellEnable "InternetGatewayDevice.X_TWSZ-COM_ACL.ShellEnable"
ULONG CLI_Shell()
{
    char szShellEnable[10] = {0};
    int ValCount = 0;
    char* szSwitch = CLI_GetParamByName("switch");
    if (CLI_GetLeaf(NODE_ShellEnable, szShellEnable) != TBS_SUCCESS)
    {
        IO_Print("\n Bad command");
        return TBS_SUCCESS;
    }
    if(CLI_GetParamByName("admin") != NULL)
    {
        if(strcmp(szSwitch, "open") == 0)
        {
            ValCount++;
            strcpy(szShellEnable, "1");
            if(CLI_SetLeaf(ValCount, NODE_ShellEnable, szShellEnable))
            {
                IO_Print("\nShell mode open failed.");
                return TBS_FAILED;
            }
            IO_Print("\nShell mode open success.");
            return TBS_SUCCESS;
        }
        else if(strcmp(szSwitch, "close") == 0)
        {
            ValCount++;
            strcpy(szShellEnable, "0");
            if(CLI_SetLeaf(ValCount, NODE_ShellEnable, szShellEnable))
            {
                IO_Print("\nShell mode close failed.");
                return TBS_FAILED;
            }
            IO_Print("\nShell mode close success.");
            return TBS_SUCCESS;
        }
        else
        {
            IO_Print("\n Bad command");
            return TBS_FAILED;
        }
    }
    if (strcmp(szShellEnable, "0") == 0 || strcmp(szShellEnable, "false") == 0)
    {
        //limited command: shell admin open / shell admin close
        IO_Print("\n Bad command");
        return TBS_SUCCESS;
    }
    if (strcmp(szShellEnable, "1") == 0 || strcmp(szShellEnable, "true") == 0)
    {
        /* 开启终端回显 */
        CLI_TermEchoOn();

        IO_Print("\n");
        tbsSystem("sh", 0);

        /* 关闭终端回显 */
        CLI_TermEchoOff();

        return TBS_SUCCESS;
    }
    return TBS_FAILED;
}

ULONG CLI_Modem()
{
    char szModemBrand[CLI_MAX_MODE_NAME] = {0};
    char szModemModel[CLI_MAX_MODE_NAME] = {0};
    char szFirmwareVersion[CLI_MAX_MODE_NAME] = {0};

    if (CLI_GetLeaf(NODE_MODEM_BRAND, szModemBrand) != TBS_SUCCESS)
    {
        strcpy(szModemBrand, " Not available ");
    }

    if (CLI_GetLeaf(NODE_MODEM_MODEL, szModemModel) != TBS_SUCCESS)
    {
        strcpy(szModemModel, " Not available ");
    }

    if (CLI_GetLeaf(NODE_FIRMWARE_VERSION, szFirmwareVersion) != TBS_SUCCESS)
    {
        strcpy(szFirmwareVersion, " Not available");
    }

    if (CLI_GetParamByName("brand") != NULL)
    {
        IO_Print("\nModem brand:      %s", szModemBrand);
        return TBS_SUCCESS;
    } 
    else if(CLI_GetParamByName("model") != NULL)
    {
        IO_Print("\nModem model:      %s", szModemModel);
        return TBS_SUCCESS;
    } 
    else if(CLI_GetParamByName("version") != NULL)
    {
        IO_Print("\nFirmware version:    %s", szFirmwareVersion);
        return TBS_SUCCESS;
    }
    else if(CLI_GetParamByName("date") != NULL)
    {
        IO_Print("\nFirmware date: %s", __DATE__);
        return TBS_SUCCESS;
    }
    else if(CLI_GetParamByName("showall") != NULL)
    {
        IO_Print("\nModem brand:      %s", szModemBrand);
        IO_Print("\nModem model:      %s", szModemModel);
		IO_Print("\nNAT session:      %s", "4096");
		IO_Print("\nModem CPU:        %s", "MT7513");
		IO_Print("\nModem memory:     %s", "128MB");
        IO_Print("\nFirmware version: %s", szFirmwareVersion);
        IO_Print("\nFirmware date:    %s", __DATE__);
        return TBS_SUCCESS;
    }
    return TBS_FAILED;
}
#endif
ULONG CLI_Device()
{
    if (CLI_GetParamByName("showall") != NULL)
    {
        IO_Print("\n\t\tbrand\t\tmodel");
        IO_Print("\nProcessor\t%s\t%s", DEVICE_SOC_BRAND, DEVICE_SOC_MODEL);
        IO_Print("\nADSL\t\t%s\t%s", DEVICE_ADSL_BRAND, DEVICE_ADSL_MODEL);
        IO_Print("\nRAM memory\t%s\t\t%s", DEVICE_RAM_BRAND, DEVICE_RAM_MODEL);
        IO_Print("\nFlash memory\t%s\t\t%s", DEVICE_FLASH_BRAND, DEVICE_FLASH_MODEL);
        return TBS_SUCCESS;
    } 
    else if(CLI_GetParamByName("soc") != NULL)
    {
        IO_Print("\n\t\tbrand\t\tmodel");
        IO_Print("\nProcessor\t%s\t\t%s", DEVICE_SOC_BRAND, DEVICE_SOC_MODEL);
        return TBS_SUCCESS;
    } 
    else if(CLI_GetParamByName("adsl") != NULL)
    {
        IO_Print("\n\t\tbrand\t\tmodel");
        IO_Print("\nADSL\t\t%s\t\t%s", DEVICE_ADSL_BRAND, DEVICE_ADSL_MODEL);
        return TBS_SUCCESS;
    } 
    else if(CLI_GetParamByName("ram") != NULL)
    {
        IO_Print("\n\t\tbrand\t\tmodel");
        IO_Print("\nRAM memory\t%s\t\t%s", DEVICE_RAM_BRAND, DEVICE_RAM_MODEL);
        return TBS_SUCCESS;
    } 
    else if(CLI_GetParamByName("flash") != NULL)
    {
        IO_Print("\n\t\tbrand\t\tmodel");
        IO_Print("\nFlash memory\t%s\t\t%s", DEVICE_FLASH_BRAND, DEVICE_FLASH_MODEL);
        return TBS_SUCCESS;
    }
    return TBS_FAILED;    
}
#define NODE_UpstreamNoiseMargin "InternetGatewayDevice.WANDevice.1.WANDSLInterfaceConfig.UpstreamNoiseMargin"
#define NODE_DownstreamNoiseMargin "InternetGatewayDevice.WANDevice.1.WANDSLInterfaceConfig.DownstreamNoiseMargin"
#define NODE_UpstreamBlocks "InternetGatewayDevice.WANDevice.1.WANDSLInterfaceConfig.UpstreamBlocks"
#define NODE_DownstreamBlocks "InternetGatewayDevice.WANDevice.1.WANDSLInterfaceConfig.DownstreamBlocks"
#define NODE_UpstreamFlag "InternetGatewayDevice.WANDevice.1.WANDSLInterfaceConfig.UpstreamFlag"
#define NODE_DownstreamFlag "InternetGatewayDevice.WANDevice.1.WANDSLInterfaceConfig.DownstreamFlag"
#define NODE_Status "InternetGatewayDevice.WANDevice.1.WANDSLInterfaceConfig.Status"
#if 0
ULONG CLI_CmdAdsl()
{
	char updata[10],downdata[10],upflag[10],downflag[10];
    long lupdata,ldowndata,lupflag,ldownflag;
	
    char  *pParam;
    char status[100];
    if ((pParam = CLI_GetParamByName("bits|snr|status")) == NULL)
    {
        IO_Print("\nError: please input \"bits\" or \"snr\"");
        return TBS_FAILED;
    }

    if (!STB_StriCmp(pParam, "snr"))
    {
        if (CLI_GetLeaf(NODE_UpstreamNoiseMargin, updata) != TBS_SUCCESS)
        {
            IO_Print("\nError: can not get leaf \"%s\" value\n",NODE_UpstreamNoiseMargin);
			return TBS_FAILED;
        }
		 if (CLI_GetLeaf(NODE_DownstreamNoiseMargin, downdata) != TBS_SUCCESS)
        {
            IO_Print("\nError: can not get leaf \"%s\" value\n",NODE_DownstreamNoiseMargin);
			return TBS_FAILED;
        }
         IO_Print("\n------the table SNR per tones(carriers) of the interface ADSL2+-----\n");
         IO_Print("\n%15s%15s\n","Downstream","Upstream");
         IO_Print("%15s%15s\n",downdata,updata);
		 return TBS_SUCCESS;
    }
    else if (!STB_StriCmp(pParam, "bits"))
    {
        if (CLI_GetLeaf(NODE_UpstreamBlocks, updata) != TBS_SUCCESS)
        {
            IO_Print("\nError: can not get leaf \"%s\" value\n",NODE_UpstreamBlocks);
			return TBS_FAILED;
        }
		 if (CLI_GetLeaf(NODE_DownstreamBlocks, downdata) != TBS_SUCCESS)
        {
            IO_Print("\nError: can not get leaf \"%s\" value\n",NODE_DownstreamBlocks);
			return TBS_FAILED;
        }
		if (CLI_GetLeaf(NODE_UpstreamFlag, upflag) != TBS_SUCCESS)
        {
            IO_Print("\nError: can not get leaf \"%s\" value\n",NODE_UpstreamFlag);
			return TBS_FAILED;
        }
		 if (CLI_GetLeaf(NODE_DownstreamFlag, downflag) != TBS_SUCCESS)
        {
            IO_Print("\nError: can not get leaf \"%s\" value\n",NODE_DownstreamFlag);
			return TBS_FAILED;
        }
        lupdata=strtol(updata,NULL,10);
		ldowndata=strtol(downdata,NULL,10);
		lupflag=strtol(upflag,NULL,10);
		ldownflag=strtol(downflag,NULL,10);
        IO_Print("\n------the table Bits per tones(carriers) of the interface ADSL2+-----\n");
        IO_Print("\n%15s%15s\n","Downstream","Upstream");
        IO_Print("%15d%15d\n",ldowndata-ldownflag,lupdata-ldownflag);
		return TBS_SUCCESS;
    }
	else if (!STB_StriCmp(pParam, "status"))
		{
		  memset(status,0,sizeof(status));
          if(CLI_GetLeaf(NODE_Status,status) != TBS_SUCCESS)
          	{
            IO_Print("\nError: can not get leaf \"%s\" value\n",NODE_Status);
			return TBS_FAILED;          	   
          	}
            IO_Print("\n------the status of the interface ADSL2+-----\n");
            IO_Print("\n%s\n",status);
		    return TBS_SUCCESS;		  	  
		}

    return TBS_FAILED;
}
#endif
ULONG CLI_NATinfo()
{
   FILE * pFile = NULL;
   int icount = 0;
   int iNum = 0;
   float percent = 0.0;
   char temp_line[256]={0};
   
    pFile=fopen("/proc/sys/net/netfilter/nf_conntrack_max","r");
    if(!pFile)
    {
        return TBS_FAILED;
    }
     fgets(temp_line,sizeof(temp_line), pFile);
     icount = strtol(temp_line,NULL,10);
     fclose(pFile);
     
    pFile=fopen("/proc/sys/net/netfilter/nf_conntrack_count","r");
    if(!pFile)
    {
        return TBS_FAILED;
    }
    memset(temp_line,0,sizeof(temp_line));
    fgets(temp_line,sizeof(temp_line), pFile);
    iNum = strtol(temp_line,NULL,10);
    fclose(pFile);
    percent =  (iNum*100.00)/icount;
	IO_Print("\nNat Session Current: %d", iNum);
	IO_Print("\nNat Session Total: %d", icount);
    IO_Print("\nNat Session usage: %f%%", percent);
    return TBS_SUCCESS;
    
}
ULONG CLI_Meminfo()
{
    struct sysinfo info;
    memset(&info, 0, sizeof(sysinfo));

    if(0 == sysinfo(&info))
    {
        IO_Print("\nMemory usage: %d%%", ((info.totalram - info.freeram) * 100) / info.totalram);
        return TBS_SUCCESS;
    }
    return TBS_FAILED;
}

ULONG CLI_Cpuinfo()
{
    int i,j,count,control;
    long total,total1,number4_1=0,number4_2=0,percent=0;
    FILE * pFile = NULL;
    char temp_line[256]={0};
    char temp_line_number[20]={0};

    pFile=fopen("/proc/stat","r");
    if(!pFile)
    {
        return TBS_FAILED;
    }
    
    control=0;
    count = 0;
    total = 0;

    fgets(temp_line,sizeof(temp_line), pFile);
    
    for(i=0, j=0; i<256; i++)
    {
        if(temp_line[i] == '\0')
            break;
            
        if(temp_line[i] >= '0' && temp_line[i] <= '9')
        {
            temp_line_number[j] = temp_line[i];
            j++;
            control = 1;
        }
        else
        {
            if(control == 1)
            {
                j = 0;
                control = 0;
                count++;
                total += atoi(temp_line_number);
                
                if(count == 4)
                {
                    number4_1 = atoi(temp_line_number);
                }

                memset(temp_line_number, 0, sizeof(temp_line_number));								   
            }
        }
    }
    
    fclose(pFile);
    sleep(2);
    
    memset(temp_line_number, 0, sizeof(temp_line_number));
    pFile=fopen("/proc/stat","r");
    if(!pFile)
    {
        return TBS_FAILED;
    }
    
    control=0;
    count = 0;
    total1 = 0;
    
    fgets(temp_line,sizeof(temp_line), pFile);
    
    for(i=0,j=0;i<256;i++)
    {
        if(temp_line[i] == '\0')
            break;
            
        if(temp_line[i] >= '0' && temp_line[i] <= '9')
        {
            temp_line_number[j] = temp_line[i];
            j++;
            control = 1;
        }
        else
        {
            if(control == 1)
            {
                j = 0;
                control = 0;
                count++;
                total1 += atoi(temp_line_number);
                
                if(count == 4)
                {
                    number4_2 = atoi(temp_line_number);
                    //printf("number4_2 = %ld\n",number4_2);
                }
                
                memset(temp_line_number, 0, sizeof(temp_line_number));				   
            }
        }
    }	 
     
    percent = number4_2 - number4_1;	 
    percent *=  100;
    percent /= total1 - total;
    percent = 100 - percent;
  
    fclose(pFile);
    IO_Print("\nCPU usage: %ld%%", percent);
    return TBS_SUCCESS;
}


#define DMZ_WANDEVICENUM             "InternetGatewayDevice.WANDeviceNumberOfEntries"
#define DMZ_WANCONNECTIONUM          "WANConnectionNumberOfEntries"
#define DMZ_WANCONNECTIONPPPNUM      "WANPPPConnectionNumberOfEntries"
#define DMZ_WANCONNECTIONIPNUM       "WANIPConnectionNumberOfEntries"
#define DMZ_WANDEV                   "InternetGatewayDevice.WANDevice."
#define DMZ_WANCONNDEV               "WANConnectionDevice"
#define DMZ_WANPPPCONN               "WANPPPConnection"
#define DMZ_WANIPCONN                "WANIPConnection"
#define PATH_LENGTH                  512
#if 0
// add by lbw
ULONG CLI_WAN_DMZ()
{  
    char szConnectionType[32],szConnectionName[100],szPath[PATH_LENGTH];
    char *szName,*szIp;
    char aszLeafValue[2][512];
    char aszLeafName[2][512];
    int iConnectionCount = 0;
    int ilevel1,ilevel2,ilevel3;
    char  szWanDeviceNum[5];
    char  szWanConnectionDeviceNum[5];
    char szWanConnectionPPPNum[5];
    char szWanConnectionIPNum[5];
    char szWanConnectionPPPPath[CLI_LEAF__LEN];
    char szWanConnectionIPPath[CLI_LEAF__LEN];
    char szPathEnd[CLI_LEAF__LEN]={0};
    int iFlag=-1;
	int count=0,totalnum;
    int iPPPCount = 1, PPPIdex = 1, IPIdex = 1;
    char szDMZHost[32]={0};
    ULONG ulFlag = 0;

    if (CLI_GetLeaf(DMZ_WANDEVICENUM,  szWanDeviceNum) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }

    for(ilevel1=1;ilevel1<=strtol(szWanDeviceNum,NULL, 10);ilevel1++)
    {
		memset(szPath,0,sizeof(szPath));
        sprintf(szPath, "%s%d.%s", DMZ_WANDEV,ilevel1,DMZ_WANCONNECTIONUM);

        if (CLI_GetLeaf(szPath,  szWanConnectionDeviceNum) != TBS_SUCCESS)
        {
            return TBS_FAILED;
         }

	    totalnum=strtol(szWanConnectionDeviceNum,NULL,10);
        ilevel2=1;
        while(count<totalnum)
		{
			memset(szPath, 0, PATH_LENGTH);
             sprintf(szPath, "%s%d.%s.%d.",DMZ_WANDEV ,ilevel1, DMZ_WANCONNDEV,ilevel2);
			 
			 memset(szWanConnectionPPPNum,0,sizeof(szWanConnectionPPPNum));
			 memset(szWanConnectionPPPPath,0,sizeof(szWanConnectionPPPPath));	  
             sprintf(szWanConnectionPPPPath,"%s%s",szPath,DMZ_WANCONNECTIONPPPNUM);

             if (CLI_GetLeaf(szWanConnectionPPPPath, szWanConnectionPPPNum) != TBS_SUCCESS)
             {
				 ilevel2++;
				 continue;
                // return TBS_FAILED;
             }
             
			 memset(szWanConnectionIPNum,0,sizeof(szWanConnectionIPNum));
			 memset(szWanConnectionIPPath,0,sizeof(szWanConnectionIPPath));
             sprintf(szWanConnectionIPPath,"%s%s",szPath,DMZ_WANCONNECTIONIPNUM);

             if (CLI_GetLeaf(szWanConnectionIPPath, szWanConnectionIPNum) != TBS_SUCCESS)
             {
                 return TBS_FAILED;
             }
					  
             iFlag=-1;
			 if( strtol(szWanConnectionPPPNum,NULL,10)!=0 )
			 {
			      iFlag=0;
			 }
			 else if ( strtol(szWanConnectionIPNum,NULL,10)!=0 )
			 {
                  iFlag=1;			  
  			 }

             if(iFlag!=0 && iFlag!=1) continue;

             for(ilevel3=1;ilevel3<=((iFlag==0)?strtol(szWanConnectionPPPNum,NULL,10):strtol(szWanConnectionIPNum,NULL,10));ilevel3++)
             {
			     memset(szPath,0,sizeof(szPath));
                 if(iFlag==0)
                 {
                       for(iPPPCount = PPPIdex; iPPPCount < 99; iPPPCount++)
                       {
                             sprintf(szPath, "%s%d.%s.%d.%s.%d.",DMZ_WANDEV ,ilevel1, DMZ_WANCONNDEV,ilevel2,"WANPPPConnection",iPPPCount);
                             sprintf(szPathEnd, "%s%s",szPath, "Name");
                             if (CLI_GetLeaf(szPathEnd, szConnectionName) == TBS_SUCCESS)
                             {
                                   PPPIdex = iPPPCount + 1;
                                   break;
                             }
                       }
                 }
                 else if(iFlag==1)
                 {
                       
                       for(iPPPCount = IPIdex; iPPPCount < 99; iPPPCount++)
                       {
                             sprintf(szPath, "%s%d.%s.%d.%s.%d.",DMZ_WANDEV ,ilevel1, DMZ_WANCONNDEV,ilevel2,"WANIPConnection",iPPPCount);
                             sprintf(szPathEnd, "%s%s",szPath, "Name");
                             if (CLI_GetLeaf(szPathEnd, szConnectionName) == TBS_SUCCESS)
                             {
                                   IPIdex = iPPPCount + 1;
                                   break;
                             }
                       }
                 }

				 memset(szPathEnd, 0, PATH_LENGTH);
                 sprintf(szPathEnd, "%s%s", szPath, "ConnectionType");                 
				 if (CLI_GetLeaf(szPathEnd, szConnectionType) != TBS_SUCCESS)
                 {
                      return TBS_FAILED;
                 }

				 memset(szPathEnd, 0, PATH_LENGTH);
                 sprintf(szPathEnd, "%s%s", szPath, "X_TWSZ-COM_DMZHost");                 
				 if (CLI_GetLeaf(szPathEnd, szDMZHost) != TBS_SUCCESS)
                 {
                      return TBS_FAILED;
                 }
 
			     if (CLI_GetParamByName("show") != NULL)
				 {
                    /* 如果命令是show，更新标记 */
                    ulFlag = 1;
				     if( iConnectionCount == 0 )
					 {
					      IO_Print("\n---The Connection List Table----\n");
					      IO_Print("\n%2s%30s%18s%18s\n","No","Name","Type","DMZ Host");
					 }
					 IO_Print("%2d%30s%18s%18s\n",++iConnectionCount,szConnectionName,szConnectionType,szDMZHost);
				 }
				 else
				 {				  
                     szName = CLI_GetParamByName("name");
                     if ( szName == NULL )
                     {
                                 IO_Print("WanConnection Name Parameter error.");
                                 return TBS_FAILED;
                     }
				     if ( strcmp(szConnectionType,"IP_Routed") == 0 && strcmp(szConnectionName,szName) == 0 )
				     {
                        /* 输入的WAN连接名匹配，匹配到就更新标记 */
                        ulFlag = 1;
                         if (CLI_GetParamByName("add") != NULL)
					     {
						     memset(aszLeafName[0],0,256);
							 memset(aszLeafName[1],0,256);
							 memset(aszLeafValue[0],0,256);
							 memset(aszLeafValue[0],0,256);
							 szIp = CLI_GetParamByName("ip");
							 if (szIp == NULL)
							 {
								  IO_Print("DMZ Host IP Address Parameter error.");
								  return TBS_FAILED;
							 }
							 sprintf(aszLeafName[0], "%s%s", szPath, "X_TWSZ-COM_DMZEnabled");
							 sprintf(aszLeafName[1],"%s%s", szPath, "X_TWSZ-COM_DMZHost");
							 sprintf(aszLeafValue[0], "%s","1");
							 sprintf(aszLeafValue[1], "%s",szIp);

							 if (CLI_SetLeaf(2, aszLeafName, aszLeafValue) != TBS_SUCCESS)
							 {
							    IO_Print("\nDMZ added failed\n");
								   return TBS_FAILED;
							 }
                             IO_Print("\nDMZ added successfully\n");
							 return TBS_SUCCESS;
				         }
					     if ( CLI_GetParamByName("del") != NULL )
					     {
					         sprintf(aszLeafName[0], "%s%s", szPath, "X_TWSZ-COM_DMZEnabled");
                             sprintf(aszLeafName[1], "%s%s", szPath, "X_TWSZ-COM_DMZHost");
							 sprintf(aszLeafValue[0], "%s", "0");
                             sprintf(aszLeafValue[1], "%s", "");
							 if ( CLI_SetLeaf(2, aszLeafName, aszLeafValue) != TBS_SUCCESS )
							 {
							    IO_Print("\nDMZ deleted failed\n");
									return TBS_FAILED;
						     }
                             IO_Print("\nDMZ deleted successfully\n");
							 return TBS_SUCCESS;
					     }
			         }
			     }
			  }//for 3
			 PPPIdex = 1;
			 IPIdex = 1;
			 count++;
			 ilevel2++;
		 }
	  }
    /* 遍历WAN连接，如果没找到返回TBS_FAILED */
    if (ulFlag == 0)
    {
        IO_Print("\nInvalid wan connection name\n");
        return TBS_FAILED;
    }
	  return TBS_SUCCESS;
}
#endif
#ifdef __cplusplus
}
#endif

