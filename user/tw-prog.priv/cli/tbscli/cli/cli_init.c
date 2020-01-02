/****************************************************************************
 文件名  : CLI_Init.c
 作者    : liuzequn
 版本    :
 完成日期:
 文件描述:  本文件实现命令行模块主要部分的初始化操作
 修改历史:
        1. 修改者   :
           时间     :
           版本     :
           修改原因 :
 ****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include "cli_private.h"
#include "cli_term.h"
#include "cli_interpret.h"
#include "cli_io.h"
#include "../app/cli_cmd.inc"
#include "cli_cmdreg.h"
#include "../cli_comm.h"
#include <signal.h>

char               m_szHostName[32] = "TBS";             /* 主机设备名     */

// 模式注册信息
ST_CMD_MODE_INFO m_stCmdModeInfo[] =
  /* 模式       父模式       权限           模式名      提示符       进入命令执行操作与帮助信息 */
{{CTM_GLOBAL,  NULL_MODE,  CLI_AL_QUERY,   "Global",   ">>"       , NULL, CLI_ML_NULL},
 {CTM_GENL,    NULL_MODE,  CLI_AL_QUERY,   "Root",     ">>"       , NULL, CLI_ML_NULL},
 {CTM_CONFIG,  CTM_GENL,   CLI_AL_QUERY,   "Config",   ">>config>", NULL, CMDHELP_GENL_CM_CONFIG},

 // TODO: 此处添加更多命令模式

 {NULL_MODE}
};

/*********************************************************************/
/* 函数名称 : CLI_ProcessInit()                                      */
/* 函数功能 : 模块分段初始化函数                                     */
/* 输入参数 :                                                        */
/* 输出参数 :                                                        */
/* 返回     :                                                        */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_ProcessInit()
{
    ULONG  ulRet = TBS_SUCCESS;

    signal(SIGINT, CLI_ProcForCtrlC);

    //ulRet += CLI_CommInit();

    /* 模式结构初始化 */
    CLI_InitSysModes();
    /* 终端任务表初始化*/
    CLI_TermDataInit();
    /* 解析环境初始化  */
    ulRet += CLI_ResetEnviroment();

    /* 注册命令模式与模式对象 */
    ulRet += CLI_InitCmdTree(m_stCmdModeInfo);

    /* --------各模块命令注册开始位置--------{                  */

	/* 注册本模块命令 */
    ulRet += CLI_SysCmdReg();

    // TODO: 各模块命令注册

    /* --------各模块命令注册结束位置--------}                  */


    /* 将全局命令链接到所有模式 */
    ulRet += CLI_GlobalCmdLink();

    return ulRet;
}


void CLI_ProcessDestory()
{
    CLI_TermDestroy();
}

/*********************************************************************/
/* 函数名称 : CLI_GetHostName()                                      */
/* 函数功能 : 对外提供的主机名获取函数                               */
/* 输入参数 :                                                        */
/* 输出参数 :                                                        */
/* 返回     : 主机名字符串                                           */
/*********************************************************************/
char  *CLI_GetHostName(_VOID   )
{
    return m_szHostName;
}


#ifdef __cplusplus
}
#endif

