/****************************************************************************
 文件名  :  CLI_Io.h
 作者    :  liuzequn
 版本    :
 完成日期:  2003-2-14
 文件描述:  本文件提供主机软件命令行的注册接口以及注册过程中所需要
            引用的结构定义，另外也包含执行函数中的参数获取接口
 备注   :
 函数列表:
            (1);
            (2);
            (3);
            ......

提供给外部的接口:  本文件提供给外部的接口
            (1) CLI_CmdAutoRegist
            (2) CLI_GetParamByName
            (3)

需要外部提供的接口:    本文件需要外部提供的接口
            (1)
            (2)
 修改历史:
        1. 修改者   :
           时间     :
           版本     :
           修改原因 :
        2. ...
 ****************************************************************************/
#ifndef _CLI_REG_H_
#define _CLI_REG_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include "cli_private.h"
#include "cli_cmdtree.h"

/* -------------------------------- 类型定义------------------------------ */


/* -------------------------------- 常量宏、枚举定义------------------------------ */


/* 命令帮助信息的多语种声明*/
/*#define HELP_FUN_E              "\r\n  Function: "
#define HELP_USG_E              "\r\n  Usage   : "
#define HELP_OPT_E              "\r\n  Options : "
#define HELP_EXP_E              "\r\n  e.g.    : "
#define HELP_SPC_E              "\r\n            "
#define HELP_FUN_C              "\r\n  命令功能: "
#define HELP_USG_C              "\r\n  命令格式: "
#define HELP_OPT_C              "\r\n  参数说明: "
#define HELP_EXP_C              "\r\n  例    如: "
#define HELP_SPC_C              "\r\n            "
*/

#define HELP_FUN                "\r\n\r\n  "
#define HELP_FMT                "\r\n\r\n  "
#define HELP_OPT                "\r\n    "
#define HELP_SPC                "\r\n    "
#define HELP_EXP                "\r\n\r\n  "



#define CMD_CM              "cm "
#define CLI_MAX_CMD_FORMAT  1500        /* 定义自动注册的格式化串长度不能超过1000 */
#define CLI_MAX_KEY_NUM     3           /* 定义一条命令中的最多对象个数 */
#define CLI_MAX_PARAM_NUM   40          /* 命令的最大参数个数*/


/* 自动注册的内部使用的错误码定义 */
enum AutoReg_ErrNo
{
    TOKEN_GET_ONLYSPACE = 0, /* 只获取到空格符号 */
    TOKEN_GET_GROUPBEGIN,    /* 参数组开始 */
    TOKEN_GET_MUTEXBEGIN,    /* 互斥组开始 */
    TOKEN_GET_GROUPEND,      /* 参数组结束 */
    TOKEN_GET_MUTEXEND,      /* 互斥组结束 */
    TOKEN_GET_OPTIONBGIN ,   /* 可选组开始 */
    TOKEN_GET_OPTIONEND ,    /* 可选组结束 */
    TOKEN_GET_PROPBEGIN,     /* 参数值属性开始 */
    TOKEN_GET_PROPEND,       /* 参数值属性结束 */
    TOKEN_GET_VALBEGIN,      /* 参数值开始 */
    TOKEN_GET_VALEND,        /* 参数值结束 */
    TOKEN_GET_MUTEXLINE,     /* 互斥分隔符 */
    TOKEN_GET_SPACEEND,      /* 空格分隔符 */
    TOKEN_GET_DEFAULT,       /* 缺省分隔符 */
    TOKEN_GET_LASTOBJ,       /* 最后一个对象 */
    TOKEN_GET_CMDNULL        /* 取空 */
};



/* -------------------------------- 结构、类描述定义 ------------------------------ */



/* 命令注册的基本信息结构定义*/
typedef struct CmdDefine
{
    const char  *szCmd;         /*命令格式化串*/
    PCSTR  strHelp1;       /*第一个对象的帮助信息*/
    PCSTR  strHelp2;       /*第二个对象的帮助信息(若命令对象少于2个，该参数为空)*/
    PCSTR  strHelp3;       /*第三个对象的帮助信息(若命令对象少于3个，该参数为空)*/
    PFUN_CLI_CALLBACK_EXEC pFunc; /*命令执行函数*/
    UCHAR   ucLevel;       /*命令使用等级 */
    UCHAR   ucMode;        /*命令所属模式*/
} ST_CLI_CMD_REG, *PST_CLI_CMD_REG;


/*------------------------------------*/
/*【5】自动注册定义开始               */
/*------------------------------------*/

/* 语法分析过程变量结构定义*/
typedef struct PointStack
{
    PST_CLI_PARAMLINK pParamLink;
    PST_CLI_PARAMETER pParam;
    PST_CLI_VALUE  pValue;
    UCHAR Number;
    UCHAR ucMaxIndex;
    UCHAR ucMutexIndex;
    struct PointStack *pPre;
}ST_POINT_STACK;

/*------------------------------------*/
/*【5】自动注册定义结束               */
/*------------------------------------*/

/* -------------------------------- 对外接口函数外部引用 ------------------------------ */



ULONG  CLI_CmdAutoRegist(const char  *szCmd,  UCHAR  ucMode,
                CLI_OPERAT_LEVEL_T   ucLevel,  PFUN_CLI_CALLBACK_EXEC pFunc,
                UCHAR  ucModeChange, UCHAR  ucNewMode,
                PCSTR  strHelp1, PCSTR  strHelp2, PCSTR  strHelp3);
ULONG  CLI_IsCmdValid(char  * szCmd);
PST_CLI_CMDKEY CLI_GetSubObjByName(PST_CLI_CMDKEY pCmdKey, char  *szName);
PST_CLI_PARAMLINK CLI_CreatGroup(char  * szParmGroup);
ULONG  CLI_GetObj(char  *pCurPos, char  *szObj);
ULONG  CLI_GetRegistToken(char  * szSrc, char  * szDst);
_VOID    CLI_SetProperty(UCHAR *ucState, UCHAR usProp);
ULONG  CLI_IsProp(UCHAR ucState, UCHAR usProp);
char  *CLI_GetEndSignal(char  *pCurPos);
_VOID    CLI_GetValType(char  *szSrc, UCHAR *ucType, int    *piLow,
                   int    *piHigh, int    *piStrLen);
ST_POINT_STACK *CLI_NewPointStack(_VOID   );
PST_CLI_PARAMETER CLI_AutoCreatParam(ST_POINT_STACK *pPointStack, char  *szName, UCHAR *ucStat);
PST_CLI_VALUE CLI_AutoCreatVal(ST_POINT_STACK *pPointStack, char  *szName, UCHAR *ucStat, char  *szType);
ULONG  CLI_GetRangeStr(char  *szSrc, char  *szLow, char  *szHigh);
ULONG  CLI_CheckForValue(PST_CLI_VALUE pValue, char  *szToken);


#ifdef  __cplusplus
}
#endif

#endif /*end of #ifndef _CLI_REG_H_*/
