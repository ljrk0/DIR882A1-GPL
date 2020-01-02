/****************************************************************************
 文件名  : cli_cmd.inc
 作者    : liuzequn
 版本    :
 完成日期:
 文件描述:  对命令行模块的命令注册函数与实现函数的声明
 修改历史:
        1. 修改者   :
           时间     :
           版本     :
           修改原因 :
 ****************************************************************************/

#ifndef _CLI_CMD_INC_
#define _CLI_CMD_INC_

#include "../cli/cli_private.h"

ULONG  CLI_SysCmdReg(_VOID   );

ULONG  CLI_CmdClearScreen();
ULONG  CLI_CmdExit();
ULONG  CLI_CmdSetMonitorOn();
ULONG  CLI_CmdSetMonitorOff();
ULONG  CLI_CmdSetScroll();
ULONG  CLI_CmdShowLog();
ULONG  CLI_CmdHelp();
ULONG  CLI_IfConfig();
ULONG  CLI_Debug();
ULONG  CLI_Ipt();
ULONG  CLI_Ddns();
ULONG  CLI_Dns();
ULONG  CLI_SaveCfg();
ULONG  CLI_Natp();
ULONG  CLI_Route();
ULONG  CLI_Wan();
ULONG  CLI_Admin();
ULONG CLI_LAN_IPInterface();
ULONG CLI_DHCPServer();

ULONG CLI_CmdAdsl();
ULONG CLI_WLAN();
ULONG CLI_WLAN_MAC();
ULONG CLI_WIRELESS_MAC();
ULONG CLI_WAN_DMZ();

ULONG CLI_Shell();

ULONG CLI_Restore_Defaults();
ULONG CLI_Save_And_Exit();
ULONG CLI_Reboot();
ULONG CLI_Save_And_Reboot();
ULONG CLI_Modem();
ULONG CLI_Device();
ULONG CLI_PortMirror();
ULONG CLI_Acl();
ULONG CLI_TR069();
ULONG CLI_Meminfo();
ULONG CLI_Cpuinfo();
ULONG CLI_NATinfo();
#endif  /*end of #ifndf _CLI_CMD_INC_*/


