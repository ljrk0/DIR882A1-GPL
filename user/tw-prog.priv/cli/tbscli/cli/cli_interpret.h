/****************************************************************************
 文件名  : cli_interpret.h
 作者    : liuzequn
 版本    :
 完成日期:
 文件描述:  声明命令解释的各种状态定义、数据结构、宏与函数
 修改历史:
        1. 修改者   :
           时间     :
           版本     :
           修改原因 :
 ****************************************************************************/
#ifndef _CLI_INTP_H_
#define _CLI_INTP_H_
#include "cli_cmdtree.h"
#ifdef  __cplusplus
extern "C" {
#endif

/* -------------------------------- 类型定义------------------------------ */


/* -------------------------------- 常量宏、枚举定义------------------------------ */

#define CLI_INTP_RET_HELP       44
#define CLI_INTP_RET_NEWLINE    55
#define CLI_MAX_MODE_STACK                    16  /* 最多允许进入的模式*/
#define CLI_CMD_FAILED_SILENT   (ULONG )(-2)    /* 命令执行函数的返回值，表示错误但要求不输出错误信息 */

/* 解释过程中的总体状态字*/
#define CLI_STAT_ENV_SILENT     0x00040000
#define CLI_STAT_ENV_SYSTEM     0x00080000   /* 该标志位已经不用， 留待扩展 */
#define CLI_STAT_ENV_ENTER      0x00100000
#define CLI_STAT_ENV_RUNCFG     0x00200000
#define CLI_STAT_ENV_NEWLINE    0x00400000

#define CTMF_PARTIAL    0xf0
#define CTMF_FULL       0x0f

#define CLI_IS_ENV_SILENT(s)    ((s) & CLI_STAT_ENV_SILENT)
#define CLI_IS_ENV_SYSTEM(s)    ((s) & CLI_STAT_ENV_SYSTEM)
#define CLI_IS_ENV_ENTER(s)     ((s) & CLI_STAT_ENV_ENTER)
#define CLI_IS_ENV_RUNCFG(s)    ((s) & CLI_STAT_ENV_RUNCFG)
#define CLI_IS_ENV_NEWLINE(s)   ((s) & CLI_STAT_ENV_NEWLINE)

/* 解释解析的状态机状态位定义 */
#define WAIT_NULL           0x00    /* Initial state            */
#define WAIT_KEY            0x01    /* expecting a object       */
#define WAIT_PARAM          0x02    /* expecting a parameter    */
#define WAIT_VALUE          0x04    /* expecting a value        */

#define NO_FORM             0x20    /* a no form command        */
#define INTERACTIVE         0x40    /* interactive command mode */
#define SKIP_ELEMENT        0x80    /* skip an optional element */

#define RES_COMPLETE        0x100   /* Research Completed       */
#define LAST_TOKEN          0x200   /* Last token to be matched */
#define MUTEX_PARAM         0x400   /* Now Parameter Mutex      */
#define VIRTUAL_P           0x800   /* Current element is       */

#define MUTEX_VALUE         0x1000  /* Now Value Mutex          */
#define BIG_TOKEN           0x8000  /* Token too big error      */

#define _UNWANTED           0x10000 /* Unwanted token exists.   */
#define MATCH_SUCC          0x20000 /* Match Success            */

#define MASK_STATUS         0x0f    /* Mask Bits                */
#define MASK_SKIPFLAG       0x80    /* Mask Bits                */
#define MASK_INTERACTIVE    0x40    /* Mask Bits                */
#define MASK_NOFORM         0x20    /* Mask Bits                */
#define MASK_COMPLETE       0x100   /* Mask Bits                */
#define MASK_LASTTOKEN      0x200   /* Mask Bits                */
#define MASK_MUTEXPARAM     0x400   /* Mask Bits                */
#define MASK_VIRTUAL_P      0x800   /* Mask Bits                */
#define MASK_MUTEXVALUE     0x1000  /* Mask Bits                */
#define MASK_BIGTOKEN       0x8000  /* Mask Bits                */
#define MASK_UNWANTED       0x10000 /* Mask Bits                */
#define MASK_MATCHSUCC      0x20000 /* Mask Bits                */

enum
{
   HAVEACTIONS,
   HAVENOACTIONS
};

/* -------------------------------- 宏操作定义 ------------------------------ */

/* 解析状态位的判断、设置与清除                                 */
/* 状态位判断 */
#define IS_WAIT_NULL(s)     (((s) & MASK_STATUS) == WAIT_NULL  )
#define IS_WAIT_KEY(s)      (((s) & MASK_STATUS) == WAIT_KEY   )
#define IS_WAIT_PARAM(s)    (((s) & MASK_STATUS) == WAIT_PARAM )
#define IS_WAIT_VALUE(s)    (((s) & MASK_STATUS) == WAIT_VALUE )
#define IS_SKIP_FLAG(s)     (((s) & MASK_SKIPFLAG)    != 0 )
#define IS_INTERACTIVE(s)   (((s) & MASK_INTERACTIVE) != 0 )
#define IS_NOFORM(s)        (((s) & MASK_NOFORM)      != 0 )
#define IS_COMPLETE(s)      (((s) & MASK_COMPLETE)    != 0 )
#define IS_LASTTOKEN(s)     (((s) & MASK_LASTTOKEN)   != 0 )
#define IS_MUTEXPARAM(s)    (((s) & MASK_MUTEXPARAM)  != 0 )
#define IS_VIRTUAL_P(s)     (((s) & MASK_VIRTUAL_P)   != 0 )
#define IS_MUTEXVALUE(s)    (((s) & MASK_MUTEXVALUE)  != 0 )
#define IS_BIGTOKEN(s)      (((s) & MASK_BIGTOKEN)    != 0 )
#define IS_UNWANTED(s)      (((s) & MASK_UNWANTED)    != 0 )
#define IS_MATCHSUCC(s)     (((s) & MASK_MATCHSUCC)   != 0 )

/* 状态位的设置与清除 */
#define SET_WAIT_NULL(s)    ((s) = ((s) & 0xfffffff0) | WAIT_NULL  )
#define SET_WAIT_KEY(s)     ((s) = ((s) & 0xfffffff0) | WAIT_KEY   )
#define SET_WAIT_PARAM(s)   ((s) = ((s) & 0xfffffff0) | WAIT_PARAM )
#define SET_WAIT_VALUE(s)   ((s) = ((s) & 0xfffffff0) | WAIT_VALUE )

#define SET_SKIP_FLAG(s)    ((s) = (s) | SKIP_ELEMENT    )
#define CLR_SKIP_FLAG(s)    ((s) = (s) & (~SKIP_ELEMENT) )
#define SET_INTERACTIVE(s)  ((s) = (s) | INTERACTIVE     )
#define CLR_INTERACTIVE(s)  ((s) = (s) & (~INTERACTIVE)  )
#define SET_NOFORM(s)       ((s) = (s) | NO_FORM         )
#define SET_COMPLETE(s)     ((s) = (s) | RES_COMPLETE    )
#define SET_LASTTOKEN(s)    ((s) = (s) | LAST_TOKEN      )
#define SET_MUTEXPARAM(s)   ((s) = (s) | MUTEX_PARAM     )
#define CLR_MUTEXPARAM(s)   ((s) = (s) & (~MUTEX_PARAM)  )
#define SET_VIRTUAL_P(s)    ((s) = (s) | VIRTUAL_P       )
#define CLR_VIRTUAL_P(s)    ((s) = (s) & (~VIRTUAL_P)    )
#define SET_MUTEXVALUE(s)   ((s) = (s) | MUTEX_VALUE     )
#define CLR_MUTEXVALUE(s)   ((s) = (s) & (~MUTEX_VALUE)  )
#define SET_BIGTOKEN(s)     ((s) = (s) | BIG_TOKEN       )
#define SET_UNWANTED(s)     ((s) = (s) | _UNWANTED       )
#define SET_MATCHSUCC(s)    ((s) = (s) | MASK_MATCHSUCC  )
#define CLR_MATCHSUCC(s)    ((s) = (s) & (~MASK_MATCHSUCC))


/* 命令解释过程中的状态变迁的宏定义开始                        {
*/
/*【1】对象*/
/* (1.1) 检查对象是否可执行 */
#define CLI_KEY_HAVE_ACTION(pObj)                \
( ((pObj)->action.pFunction != NULL) ||      \
  ((pObj)->noAction.pFunction != NULL) )

/* (1.2) 获取前一个对象 */
#define CLI_KEY_ELDER_BROTHER(pObj)      ((pObj)->pPrevious)

/* (1.3) 获取后一个对象 */
#define CLI_KEY_YOUNGER_BROTHER(pObj)    ((pObj)->pNext)

/* (1.4) 获取对象名 */
#define CLI_KEY_NAME(pObj)   ((pObj)->szName)

/* (1.5) 判断是否为模式转换对象 */
#define CLI_KEY_CHANGE_MODE(pObj)    ((pObj)->action.modeConv != CLI_MC_NULL)

/* (1.6) 判断对象是否有参数 */
#define CLI_KEY_HAVE_PARAMETER0(pObj)            \
( ((pObj)->action.pParamLink != NULL) &&        \
  ((pObj)->action.pParamLink->ulNumber > 0) )
#define CLI_KEY_HAVE_PARAMETER1(pObj)            \
( ((pObj)->noAction.pParamLink != NULL) &&      \
  ((pObj)->noAction.pParamLink->ulNumber > 0) )

/* (1.7) 判断对象是否有子对象 */
#define CLI_KEY_HAVE_SUBOBJECTS(pObj)                \
( (pObj)->pSubCmdKey != NULL )

/* (1.8) 对象是否超大 */
#define CLI_IS_TOOBIG_KEY(p1,p2)                         \
( strcmp((p1)->szName,(p2)->szName) > 0 )

/* (1.9) 获取对象模式 */
#define CLI_GET_KEY_MODE(pObj) ((pObj)->action.ulMode)

/* (1.9) 是否可被当前用户与模式执行*/
#define CLI_IS_CURRENT_KNOWN_KEY(pObj)  \
(((ULONG )(pObj->action.rightLevel) <= m_stCliWS.ulLevel)\
&& ((pObj->action.ulMode == m_stCliWS.ulCurrentMode)\
|| (pObj->action.ulMode == CTM_GLOBAL)))


/*【2】参数*/
/* (2.1) 判断参数是否可选 */
#define CLI_PARAM_IS_OPTIONAL(pParam)                \
( (pParam)->stParamType.ucAttrib & CLI_PRMPROP_OPTIONAL )

/* (2.2) 判断参数是否互斥属性    */
#define CLI_PARAM_IS_MUTEX(pParam)                   \
( (pParam)->stParamType.ucAttrib & CLI_PRMPROP_MUTEX )

/* (2.3) 判断是否需要参数名    */
#define CLI_PARAM_NEED_NAME(pParam)                  \
( ((pParam)->stParamType.ucAttrib & CLI_PRMPROP_NAME )    \
 || ((pParam)->stParamType.ucAttrib & CLI_PRMPROP_MUTEX) )

/* (2.4) 判断参数是否带值   */
#define CLI_PARAM_NEED_VALUE(pParam)                 \
( (pParam)->valueList.ulNumber > 0 )

/* (2.5) 判断参数是否有子参数   */
#define CLI_PARAM_NEED_SUB(pParam)                   \
( (pParam)->subParam != NULL )

/* (2.6) 判断参数是否有后继 */
#define CLI_PARAM_YOUNGER_BROTHER(pParam)    ( (pParam)->pNext != NULL )

/* (2.7) 判断参数是否有父参数 */
#define CLI_PARAM_PARENT(pParam)     ( (pParam)->pParent != NULL )

/* (2.8) 判断参数是否同名 */
#define CLI_IS_SAME_PARAM(p1,p2)                     \
( strcmp((p1)->szName,(p2)->szName)== 0 )

/* (2.9) 判断参数是否具有索引调整属性    */
#define CLI_PARAM_IS_INDEX(pParam)                   \
( (pParam)->stParamType.ucAttrib & CLI_PRMPROP_INDEX )


/*【3】参数值*/
/* (3.1) 判断参数值是否有后继 */
#define CLI_VALUE_YOUNGER_BROTHER(pValue)    ( (pValue)->pNext != NULL )

/* (3.2) 判断参数是否需要自动检测 */
#define CLI_VALUE_NEED_CHECK(pValue)                \
( (pValue)->valueType.ucAttrib & CLI_VALPROP_CHECK )

/* (3.3) 判断参数是否用户自行检测 */
#define CLI_VALUE_USER_CHECK(pValue)                \
( (pValue)->pCheckFunc != NULL )

/* (3.4) 判断参数是否有缺省值 */
#define CLI_VALUE_HAVE_DEFAULT(pValue)                \
( (pValue)->valueType.ucAttrib & CLI_VALPROP_DEFAULT )

/* (3.5) 值是否互斥 (预留)*/
#define CLI_VALUE_IS_MUTEX(pInputValue)                  \
( (pInputValue)->valueType.ucAttrib & CLI_VALPROP_MUTEX )

/* (3.6) 值是否可选 (预留)*/
#define CLI_VALUE_IS_OPTIONAL(pInputValue)               \
( (pInputValue)->valueType.ucAttrib & CLI_VALPROP_OPTIONAL )

/*【3】解释状态结构变迁 */
/* (3.1) 保存解析成功的状态 */
#define COPY_POINTER_TO_CUR(p)                          \
{                                                       \
    ((p)->curP).pObj   = ((p)->expP).pObj;              \
    ((p)->curP).pParam = ((p)->expP).pParam;            \
    ((p)->curP).pValue = ((p)->expP).pValue;            \
}

/* (3.2) 获取上次解析成功的状态 */
#define COPY_POINTER_TO_EXP(p)                          \
{                                                       \
    ((p)->expP).pObj   = ((p)->curP).pObj;              \
    ((p)->expP).pParam = ((p)->curP).pParam;            \
    ((p)->expP).pValue = ((p)->curP).pValue;            \
}

/* (3.3) 当前状态是否允许命令交互 */
#define CLI_IS_SMARTON(state) (state == SMART_ON)


/* -------------------------------- 结构、类描述定义 ------------------------------ */
/* 命令解释环境状态变量定义开始                                   { */
typedef struct CLI_CMD_STACK_S
{
    UCHAR ucTop;                          /* Stack Top                */
    ULONG  ulMode[CLI_MAX_MODE_STACK];          /* Mode hierarchy           */
}CLI_CMD_STACK_T;
typedef CLI_CMD_STACK_T *CLI_CMD_STACK_P;

typedef struct tag_ST_CLI_WS
{
    char   szCmdString[CLI_MAX_CMDLEN];  /* 已解释的命令串      */
    ULONG  ulLevel;                     /* 命令等级            */
    ULONG  ulCurrentMode;               /* 当前命令模式        */
    CLI_CMD_STACK_T cmdStack;            /* 命令模式栈          */
    ULONG  aulIndexTable[CLI_MAX_PARAM_NUM];       /* 参数缓冲中的索引    */
    char   aszParamBuff[CLI_MAX_PARAM_NUM][CLI_MAX_PARAMLEN];               /* 参数缓冲            */
    char   aszNameBuff[CLI_MAX_PARAM_NUM][CLI_MAX_PARAMLEN];              /* 参数名缓冲          */
    ULONG  ulParamIndex;              /* 最大参数索引        */
    ULONG  ulValidParams;             /* 有效参数个数        */
    ULONG  ulStatus;                  /* 环境变量的状态      */
    ULONG  ulExpectEle;
}ST_CLI_WS, *PST_CLI_WS;

/* 命令解释环境状态变量定义结束                                   } */

typedef struct tag_ST_CLI_INTP_POINTER
{
    PST_CLI_CMDKEY    pObj;
    PST_CLI_PARAMETER pParam;
    PST_CLI_VALUE     pValue;
}ST_CLI_INTP_POINTER, *PST_CLI_INTP_POINTER;


/* Pointers data structure */
typedef struct tag_ST_CLI_POINTER
{
    ST_CLI_INTP_POINTER curP;
    ST_CLI_INTP_POINTER expP;
    PST_CLI_PARAMETER pIndexParam;
    ULONG  ulValueIndex;
    ULONG  *ulStatus;           /* 解释状态 */
    char   szPatch[MAX_TOKEN_SIZE + 4];
    UCHAR  ucMatchFlag;         /* 匹配标志, 取值: CTMF_PARTIAL / CTMF_FULL */
    UCHAR  ucSystemFlag;        /* 当前是否全局模式     */
    char   *szToken;            /* 当前所获取的token    */
}ST_CLI_POINTER, *PST_CLI_POINTER;


typedef struct CLI_MODE_RECORD_S
{
    char  szName[256] ;
    ULONG  ulCount ;
} CLI_MODE_RECORD_T;



/* -------------------------------- 对外接口函数外部引用 ------------------------------ */

/* 命令解析的函数声明 */
ULONG   CLI_ResetEnviroment();
ULONG  CLI_IntpKernelProc();
ST_CLI_USERTYPE * CLI_GetUserType(ULONG  ulType);
ULONG  CLI_ValidateUserType(ULONG  ulType);
ULONG  CLI_GetCmdToken(char  **szCmdString, char  *szToken,
                        ULONG  *ulStatus, PST_CLI_POINTER pPointer);
char  *CLI_GetInteractiveObject(PST_CLI_POINTER pPointer, ULONG  ulStatus, char  *szSelect);
char  *CLI_GetInteractiveParameter(PST_CLI_POINTER pPointer, char  *szSelect);
char  *CLI_GetInteractiveValue(PST_CLI_POINTER pPointer, char  *szSelect);
ULONG  CLI_GetCmdTokenFromUser(char  *szToken, ULONG  ulStatus, PST_CLI_POINTER pPointer );
ULONG  CLI_GetCmdTokenFromBuffer(char  **szCmdString, char  *szToken, ULONG  *ulStatus );
ULONG  CLI_CheckElementObject(char  *szToken, PST_CLI_POINTER pPointer,ULONG  *pulStatus );
ULONG  CLI_CheckElementParameter(char  *szToken, PST_CLI_POINTER pPointer, ULONG  *pulStatus );
ULONG  CLI_CheckElementValue(char  *szToken, PST_CLI_POINTER pPointer, ULONG  *pulStatus );
ULONG  CLI_CheckElement(char  *szToken, PST_CLI_POINTER pPointer, ULONG  *pulStatus);
ULONG  CLI_UserPermitted(ULONG  ulLevel, PST_CLI_CMDKEY pCmdKey, ULONG  ulStatus);
ULONG  CLI_GetUserRightLevel();
ULONG  CLI_IsObjectExecutable(PST_CLI_CMDKEY pCmdKey, ULONG  ulStatus);
ULONG  CLI_BackTopVal(PST_CLI_POINTER pPointer, ULONG  * pulStatus );
ULONG  CLI_BackTopParam(PST_CLI_POINTER pPointer, ULONG  * pulStatus );
ULONG  CLI_BackTopKey(PST_CLI_POINTER pPointer, ULONG  * pulStatus );
ULONG  CLI_AdjustParam(PST_CLI_POINTER pPointer);
ULONG  CLI_GetNextElement_O(PST_CLI_POINTER pPointer, ULONG  * pulStatus );
ULONG  CLI_GetNextElement_P(PST_CLI_POINTER pPointer, ULONG  * pulStatus );
ULONG  CLI_GetNextElement_V(PST_CLI_POINTER pPointer, ULONG  * pulStatus );
ULONG  CLI_GetExpectedElement(PST_CLI_POINTER pPointer, ULONG  * pulStatus );
ULONG  CLI_TakeAction(PST_CLI_CMDKEY pCmdKey, ULONG  ulStatus);
ULONG  CLI_GetNewMode(PST_CLI_CMDKEY pCmdKey);
ULONG  CLI_GetExitMode(ULONG  ulMode);
ULONG  CLI_GetEndMode(_VOID);
ULONG  CLI_GetModeTotalParams(ULONG  ulMode);
ULONG  CLI_CmdPush(CLI_CMD_STACK_P pStk, ULONG  ulMode);
ULONG  CLI_CmdPop(CLI_CMD_STACK_P pStk, char  *s);
ULONG  CLI_ConvertMode( PST_CLI_CMDKEY pCmdKey);
ST_CLI_MODE *CLI_GetMode(ULONG  ulMode);
char  *CLI_GetModePrompt(ULONG  ulMode);
_VOID   CLI_MatchMore(char  *szResult,char  *szNew);
char  *CLI_SetPatch(char  *szBase,char  *szResult);
ULONG  CLI_MatchKeywords(char  *szToken,PST_CLI_ENUM pKeyword,PST_CLI_POINTER pPointer);
ULONG  CLI_IfCommandHelp();
ULONG  CLI_PrintHelpString(char  *szString);
ULONG  CLI_GetObjectHelpString(PCSTR szString,char  helpStr[128]);
ULONG  CLI_ShowParamHelp(char  *pName, const char  *szCmdHelp);
ULONG  CLI_ShowCommandHelp( PST_CLI_CMDKEY pCmdKey, PST_CLI_POINTER pPointer, ULONG  ulStatus);
ULONG  CLI_GetSubObjectsNumber(PST_CLI_CMDKEY pCmdKey, PST_CLI_POINTER pPointer, ULONG  ulStatus);
ULONG  CLI_ShowObjectHelp( PST_CLI_CMDKEY pCmdKey, PST_CLI_POINTER pPointer,
                           ULONG  ulStatus, char  *pszToken);
ULONG  CLI_ShowHelp( PST_CLI_POINTER pPointer, ULONG  ulCurMode, ULONG  ulStatus);
ULONG  CLI_GetString(char  *szString, ULONG  ulLength);
ULONG  CLI_BrotherParameterOptional(PST_CLI_PARAMETER *ppParam);
ULONG  CLI_BrotherValueOptional(PST_CLI_VALUE *ppValue);
ULONG  CLI_SetCommandString( char  *szCmd);
char  *CLI_GetCommandPatch(ULONG  ulCmdLen);
ULONG  CLI_SaveParameter( char  *szToken, char  *szName, ULONG  ulIndex);
ULONG  CLI_GetCmdWord(ULONG  ulIndex, char  *Outp);
ULONG  CLI_GetCurrentMode(_VOID);
char  * CLI_GetParameter( ULONG  ulIndex);
char  * CLI_GetParamByName(const char  *szName);
ULONG  CLI_GetLongByName(const char  *szName, int    *lResult);
ULONG  CLI_GetUlongByName(const char  * szName, ULONG  * ulResult);
ULONG  CLI_GetHexByName(const char  *szName, ULONG  *lResult);
char  * CLI_GetModeParameter( ULONG  ulMode, ULONG  ulIndex);
char  *CLI_GetCurrentPrompt();
ULONG  CLI_ResetIndexTable();
ULONG  CLI_SetWSStatus(ULONG  ulStatus);
ULONG  CLI_FindModeIdx(ULONG  ulMode, char  * szModeName);
_VOID   CLI_EnterMode(ULONG  ulMode, char  *szModeNme);
ULONG  CLI_ExitMode(ULONG  ulMode, char  *szModeName);
ULONG  CLI_DelMode(ULONG  ulMode, char  * szModeName);
ULONG  CLI_CmdExecute ( );

#ifdef  __cplusplus
}
#endif

#endif /*end of #ifndef _CLI_INTP_H_*/

