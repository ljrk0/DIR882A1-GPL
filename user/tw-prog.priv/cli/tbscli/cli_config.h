/****************************************************************************
 文件名  : CLI_Config.h
 作者    : liuzequn
 版本    :
 完成日期:
 文件描述: 命令行模块基本配置选项
 修改历史:
        1. 修改者   :
           时间     :
           版本     :
           修改原因 :
 ****************************************************************************/
#ifndef _CLI_CONFIG_H_
#define _CLI_CONFIG_H_
//#include "cli_private.h"

#define CLI_YES     1
#define CLI_NO      0

#define CLI_DEBUG_ON            CLI_YES   //是否调试版本
//#define CLI_DEBUG_ON            CLI_NO

#define LOG_SAVE_FLASH          CLI_NO    //是否使用FLASH进行日志存储

#define DBG_USER_PASSWORD       "root"            //调试用户密码

#define UNLOGON_USER_NAME       "unlogon user"

/*针对超时开关设置的两种超时时间 单位:秒*/
#define DEADLINE_LONG           7200
#define DEADLINE_SHORT          300

#endif

