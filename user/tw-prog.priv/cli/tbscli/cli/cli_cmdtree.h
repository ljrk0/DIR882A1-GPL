/****************************************************************************
 文件名  : CLI_Io.h
 作者    : liuzequn
 版本    :
 完成日期:
 文件描述:  本文件提供主机软件命令行的注册接口以及注册过程中所需要
            引用的结构定义，另外也包含执行函数中的参数获取接口
 备注   :
 函数列表:
            (1);
            (2);
            (3);
            ......

提供给外部的接口:   本文件提供给外部的接口
            (1) ULONG   CLI_CmdAutoRegist
            (2)
            (3)
            (4)
            (5)
            (6)
            (7)
            (8)
            (9)
            (10)

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
#ifndef _CLI_CMDTREE_INC_
#define _CLI_CMDTREE_INC_

#ifdef  __cplusplus
extern "C" {
#endif
#include "cli_private.h"

/* -------------------------------- 类型定义------------------------------ */


/* -------------------------------- 常量宏、枚举定义------------------------------ */

#define CLI_MAX_MODE_NAME    48          /* 模式名长度       */
#define CLI_MAX_MODE_PROMPT  36          /* 模式提示符的长度 */
#define CLI_MAX_MODE_VERB    256         /* 进入某模式命令字长度 */
#define CLI_MAX_CMDLEN       1024
#define MAX_TOKEN_SIZE      512          /* 单个命令字最大长度*/
#define CLI_MAX_PARAMLEN     MAX_TOKEN_SIZE/* 参数的最大长度*/


/* 参数的属性位定义 */
#define CLI_PRMPROP_NULL        0x00        /* 无属性   */
#define CLI_PRMPROP_NAME        0x01        /* 需要名字 */
#define CLI_PRMPROP_MUTEX       0x02        /* 多值互斥 */
#define CLI_PRMPROP_INDEX       0x04        /* 索引     */
#define CLI_PRMPROP_OPTIONAL    0x08        /* 可选     */

/* 值的属性位定义，自动注册不使用，一律为CLI_VALPROP_CHECK  */
#define CLI_VALPROP_NULL        0x00        /* 无属性       */
#define CLI_VALPROP_OPTIONAL    0x01        /* 可选         */
#define CLI_VALPROP_MUTEX       0x02        /* 互斥(多选一) */
#define CLI_VALPROP_INDEX       0x04        /* 位索引       */
#define CLI_VALPROP_CHECK       0x08        /* 输入类型检查 */
#define CLI_VALPROP_DEFAULT     0x10        /* 缺省         */


/* 模式转换性质 */
typedef enum  CLI_MODE_CONV_E
{
    CLI_MC_NULL = 0      ,   /*非模式转换             */
    CLI_MC_NEW           ,   /*转换到新的模式         */
    CLI_MC_EXIT          ,   /*退出到上一模式         */
    CLI_MC_END               /*退出到CTM_GENL_EXEC模式*/
} CLI_MODE_CHANGE_T;

/* 命令模式枚举定义 */
enum CLI_MODE_ID_E {
    /* 基本模式 */
    CTM_GLOBAL = 0      ,  /* 全局模式，该模式下的命令在任何模式下可以执行    (使用级别: 查询级)*/
    CTM_GENL            ,  /* 特权模式，进行一般的配置     (使用级别:操作者)*/
    CTM_CONFIG          ,  /* 全局配置模式，以业务配置为主 (使用级别:操作者)*/
    CTM_DIAG            ,  /* 诊断模式，系统状态的诊断命令 (使用级别:调试级)*/
    CTM_DBG             ,  /* 调试模式，进供调试使用，release版本不存在该模式 (使用级别: super级)*/
    /* 扩展模式 */
    CTM_IF              ,  /* 接口模式，配置模式的子模式 */

    /* 动态模式 */
    CTM_DYNAMIC_START   ,  /* 动态模式起始标志，此处不是一个具体的模式 */

    CTM_TOTAL              /* 所有模式的结束，不代表一个具体的模式 */
} ;
#define END_MODE        CTM_GENL
#define ROOT_MODE       CTM_GENL
#define MAX_MODE_NUM    (CTM_TOTAL + 1)
#define NULL_MODE       CTM_TOTAL


/*  参数值类型定义 */
#define CLI_DATA_MAX_TYPE    32
typedef enum  CLI_DATATYPES_E
{
    CLI_DATA_NULL,                       /* 空                   */
    CLI_DATA_VI,                         /* 整数                 */
    CLI_DATA_VIK,                        /* 整数 + 串列          */
    CLI_DATA_VU,                         /* 无符号整数           */
    CLI_DATA_VUK,                        /* 无符号整数 + 串列    */
    CLI_DATA_VH,                         /* 16进制整数           */
    CLI_DATA_VHK,                        /* 16进制整数 +串列     */
    CLI_DATA_VS,                         /* 字符串               */
    CLI_DATA_K,                          /* 串列                 */
    CLI_DATA_IP,                         /* 主机ip               */
    CLI_DATA_IPK,                        /* 主机ip + 串列        */
    CLI_DATA_MASK,                       /* ip掩码               */
    CLI_DATA_MAC,                        /* MAC地址              */
    CLI_DATA_MACK,                       /* MAC地址 + 串列       */
    CLI_DATA_DATE,                       /* 日期类型 y-m-d       */
    CLI_DATA_TIME,                       /* 时间类型 h:m:s       */

    CLI_DATA_USER,                       /* 用户数据自定义界限，该
                                           类型及以后的均为用户自定义类型   */
    CLI_DATA_BOTTOM = CLI_DATA_MAX_TYPE

} CLI_DATATYPES_T;



/* -------------------------------- 宏操作定义 ------------------------------ */

/* 判断两个对象是否同名 */
#define CLI_IS_SAME_KEY(p1,p2) ( strcmp((p1)->szName,(p2)->szName) == 0 )
/* 判断排序上对象1是否优于对象2*/
#define CLI_KEY_BIGGER_THAN(p1,p2)  ( strcmp((p1)->szName,(p2)->szName) > 0  )


/* -------------------------------- 结构、类描述定义 ------------------------------ */
/*------------------------------------*/
/* 【1】参数值的定义开始              */
/*------------------------------------*/
typedef struct tag_ST_CLI_VALTYPE
{
    UCHAR ucAttrib;    /* 参数值属性    */
    UCHAR ucDataType;  /* 参数值类型    */
    ULONG ulDataLen;   /* 参数值长度(针对字符串而言)*/
    UCHAR ucIndex;     /* 保留          */
}ST_CLI_VALTYPE, *PST_CLI_VALTYPE;


/* 为关键字定义的数据结构  {{*/
typedef struct tag_ST_CLI_ENUM
{
    char    *pK;                   /* enum枚举串        */
    struct tag_ST_CLI_ENUM *pPrevious;   /* 前一个枚举 */
    struct tag_ST_CLI_ENUM *pNext;       /* 后一个枚举 */
}ST_CLI_ENUM,*PST_CLI_ENUM;

typedef struct tag_ST_CLI_ENUMS
{
    ULONG    ulNumber;             /* 枚举序列的个数 */
    PST_CLI_ENUM pKeyword;            /* 枚举序列的内容 */
}ST_CLI_ENUMS;
/*                             }}*/

/* 自定义的参数值检查函数*/
typedef ULONG  (*FUN_USER_CHECK)(char  *szValue,char  *szShow);
typedef struct tag_ST_CLI_VALUE
{
    char  *szName;                      /* 参数值名           */
    ST_CLI_VALTYPE valueType;           /* 参数值类型         */
    ST_CLI_ENUMS keywords;              /* 多选一关键字列表   */
    int    lIntFilter[2];             /* 参数数值的上下限   */
    struct tag_ST_CLI_VALUE *pPrevious; /* 参数值前驱         */
    struct tag_ST_CLI_VALUE *pNext;     /* 参数值的后继       */
    ULONG    ulAbsoluteIndex;           /* 绝对位置索引       */
    char    *szDefault;                 /* 参数值的缺省值     */
    FUN_USER_CHECK pCheckFunc;          /* 用户提供的检查函数 */
}ST_CLI_VALUE, *PST_CLI_VALUE;

/* 参数值链表的定义 */
typedef struct tag_ST_CLI_VALUELINK
{
    ULONG  ulNumber;           /* Total number of Values in the Link   */
    PST_CLI_VALUE pValue;      /* Head Pointer to the Value Link       */
}ST_CLI_VALUELINK, *PST_CLI_VALUELINK;

/*用户自定义参数值类型*/
#define CLI_MAX_USERTYPES    ((ULONG )CLI_DATA_BOTTOM - (ULONG )CLI_DATA_USER)

typedef ULONG  (*CLI_TYPECHECK_FUNC_T) (char  *szValue,char  *szShow);
typedef struct  tag_ST_CLI_USERTYPE
{
    UCHAR ucTypeID;                        /* 数据类型     */
    char  szTypeName[24];                  /* 数据类型名字 */
    CLI_TYPECHECK_FUNC_T pCheckFunc;      /* 检查函数     */
} ST_CLI_USERTYPE;
/*------------------------------------*/
/* 【1】参数值的定义结束              */
/*------------------------------------*/




/*------------------------------------*/
/* 【2】参数定义开始                  */
/*------------------------------------*/
typedef struct tag_ST_CLI_PARAM_TYPE
{
    UCHAR ucAttrib;     /* 属性位   */
    UCHAR ucIndex;      /* 位置索引 */
}ST_CLI_PARAM_TYPE, *PST_CLI_PARAM_TYPE;

typedef struct tag_ST_CLI_PARAMETER
{
    char  *szName;                      /* 参数名            */
    ST_CLI_PARAM_TYPE stParamType;           /* 参数类型          */
    UCHAR ucAttrib;                     /* 参数属性          */
    struct tag_ST_CLI_PARAMETER *subParam;    /* 第一个子参数的指针*/
    struct tag_ST_CLI_PARAMETER *pPrevious;   /* 参数的前驱        */
    struct tag_ST_CLI_PARAMETER *pNext;       /* 参数的后继        */
    struct tag_ST_CLI_PARAMETER *pParent;     /* 父参数指针        */
    ULONG  ulAbsoluteIndex;             /* 绝对索引          */
    ST_CLI_VALUELINK valueList;          /* 参数值列表        */
}ST_CLI_PARAMETER, *PST_CLI_PARAMETER;

/*参数链表结构定义*/
typedef struct tag_ST_CLI_PARAMLINK
{
    ULONG  ulNumber;          /* 参数链的内容数量   */
    PST_CLI_PARAMETER pParam;    /* 参数链的参数头部 */
}ST_CLI_PARAMLINK, *PST_CLI_PARAMLINK;
/*------------------------------------*/
/* 【2】参数定义结束                  */
/*------------------------------------*/




/*------------------------------------*/
/* 【3】关键字的结构定义开始          */
/*------------------------------------*/

/* 命令执行操作的属性结构*/
typedef struct tag_ST_CLI_ACTION
{
    ULONG  ulMode;                    /* 命令操作模式       */
    CLI_OPERAT_LEVEL_T rightLevel;   /* 命令操作权限       */
    PFUN_CLI_CALLBACK_EXEC pFunction; /* 命令的执行函数地址 */
    PCSTR   strObjHelp;                  /* 帮助信息ID         */
    CLI_MODE_CHANGE_T modeConv;        /* 模式转换方式       */
    PST_CLI_PARAMLINK pParamLink;      /* 操作参数链         */
}ST_CLI_ACTION;

typedef struct tag_ST_CLI_CMDKEY
{
    char  *szName;                    /* 对象名               */
    ULONG  ulObjectCount;             /* 子对象个数           */
    struct tag_ST_CLI_CMDKEY *pSubCmdKey; /* 第一个子对象的指针   */
    struct tag_ST_CLI_CMDKEY *pPrevious;  /* 对象的前驱           */
    struct tag_ST_CLI_CMDKEY *pNext;      /* 对象的后继           */
    struct tag_ST_CLI_CMDKEY *pParent;    /* 父对象指针           */
    ST_CLI_ACTION action;             /* 对象的操作属性结构   */
    ST_CLI_ACTION noAction;           /* 对象的no操作属性结构 */
    ULONG   ulNoFlag;                 /* 对象是否有否定形式   */
}ST_CLI_CMDKEY, *PST_CLI_CMDKEY;

/*------------------------------------*/
/* 【3】对象的结构定义结束            */
/*------------------------------------*/



/*------------------------------------*/
/* 【4】命令模式定义开始              */
/*------------------------------------*/
typedef struct tag_ST_CLI_MODE
{
    ULONG   ulMode;                           /* 模式ID         */
    ULONG   ulParent;                         /* 父模式ID       */
    CLI_OPERAT_LEVEL_T rightLevel;            /* 模式权限       */
    char    szModeName[CLI_MAX_MODE_NAME];    /* 模式名称       */
    char    szModePrompt[CLI_MAX_MODE_PROMPT];/* 模式提示符     */

    ULONG   ulHierarchy;                      /* 模式层次       */
    ULONG   ulParamCount;                     /* 参数个数       */
    PST_CLI_CMDKEY pCmdKey;                   /* 模式对象       */
}ST_CLI_MODE;

typedef struct tag_ST_CMD_MODE_INFO
{
    ULONG   ulMode;                           /* 模式ID         */
    ULONG   ulParent;                         /* 父模式ID       */
    CLI_OPERAT_LEVEL_T rightLevel;            /* 模式权限       */
    char    szModeName[CLI_MAX_MODE_NAME];    /* 模式名称       */
    char    szModePrompt[CLI_MAX_MODE_PROMPT];/* 模式提示符     */
    PFUN_CLI_CALLBACK_EXEC pFunction;         /* 命令的执行函数地址 */
    PCSTR   strObjHelp;                       /* 帮助信息ID         */
}ST_CMD_MODE_INFO;



/*------------------------------------*/
/* 【4】命令模式定义结束              */
/*------------------------------------*/





/* -------------------------------- 对外接口函数外部引用 ------------------------------ */

_VOID CLI_InitUserType(_VOID);
_VOID CLI_InitSysModes(_VOID);
ULONG  CLI_InitModeObject(_VOID);
ULONG   CLI_InitCmdTree(ST_CMD_MODE_INFO *pModeInfo);
ULONG  CLI_RegisterUserType(ULONG  *ulID,char  *szName,CLI_TYPECHECK_FUNC_T pFunc);
ULONG  CLI_FreeValue( PST_CLI_VALUE pValue );
ULONG  CLI_FreeKeyword(PST_CLI_ENUM pKW);
PST_CLI_ENUM CLI_NewKeywords(char  *szKeys, ULONG  *ulCount);
ULONG  CLI_FreeParameter(PST_CLI_PARAMETER pParam);
ULONG  CLI_FreeParamlink(PST_CLI_PARAMLINK pParamLink);
ULONG  CLI_FreeObject(PST_CLI_CMDKEY pCmdKey);
PST_CLI_VALUE CLI_NewValue(_VOID);
PST_CLI_VALUELINK CLI_NewValuelink(_VOID);
PST_CLI_ENUM CLI_NewKeyword(_VOID);
PST_CLI_CMDKEY CLI_NewObject(_VOID);
PST_CLI_PARAMETER CLI_NewParameter(_VOID);
PST_CLI_PARAMLINK CLI_NewParamlink(_VOID);
PST_CLI_CMDKEY CLI_CreateObject(char  *szName, ULONG  ulMode,
            CLI_OPERAT_LEVEL_T level, PFUN_CLI_CALLBACK_EXEC pFunc,
            BOOL bChangeMode, CLI_MODE_CHANGE_T modeConv,
            PFUN_CLI_CALLBACK_EXEC pNoFunc);
PST_CLI_PARAMETER CLI_CreateParameter( char  *szNameStr, UCHAR ucTypeAttrib,
                                   UCHAR ucIndex, ULONG  ulIndex );
PST_CLI_PARAMLINK CLI_CreateParamLink(_VOID);
PST_CLI_VALUE CLI_CreateValue(char  *szName, UCHAR ucDataType,
                          ULONG ulDataLen, char  *szKeys,  int    iLow, int    iHigh,
                          ULONG  ulIndex);
PST_CLI_VALUE CLI_CreateValueWithCheck(char  *szName,
                                UCHAR ucAttrib, UCHAR ucDataType, ULONG ulDataLen,
                                char  *szKeys,  int    iLow, int    iHigh,
                                ULONG  ulIndex,
                                FUN_USER_CHECK pFunc);
ULONG  CLI_DefineValueCheck(PST_CLI_VALUE pValue,FUN_USER_CHECK pFunc);
ULONG  CLI_SetValueDefault(PST_CLI_VALUE pInValue, char  *szDefault);
ULONG  CLI_SetMode(ULONG  ulMode, char  *szName, char  *szPrompt,
              CLI_OPERAT_LEVEL_T level,
              ULONG  ulParentMode, PST_CLI_CMDKEY pCmdKey);
ULONG  CLI_RegMode(ST_CLI_MODE *pMode);
ULONG  CLI_RegModeObject(ULONG  ulMode,PST_CLI_CMDKEY pCmdKey, ULONG  ulParamCount);
ULONG  CLI_RegModeObjectExt( ULONG  ulMode, PST_CLI_CMDKEY pCmdKey,
                        ULONG  ulParamCount, PFUN_CLI_CALLBACK_EXEC pFunc);
ULONG  CLI_RegObject(PST_CLI_CMDKEY pParent, PST_CLI_CMDKEY pCmdKey);
ULONG  CLI_RegObjToMode(ULONG  ulMode, PST_CLI_CMDKEY pCmdKey);
ULONG  CLI_RegCmdHelp(PST_CLI_CMDKEY pCmdKey, PCSTR szHelpInfoID);
ULONG  CLI_RegParamlink(PST_CLI_CMDKEY pCmdKey, BOOL bNoform,
                         PST_CLI_PARAMLINK pParamLink);
ULONG  CLI_RegValueToParam(PST_CLI_PARAMETER pParam,
                                PST_CLI_VALUE pValue);
ULONG  CLI_RegParamAsHeir(PST_CLI_PARAMETER pOldParam,
                     PST_CLI_PARAMETER pNewParam);
ULONG  CLI_RegParamToLink(PST_CLI_PARAMLINK pParamLink,
                                PST_CLI_PARAMETER pNewParam);
ULONG  CLI_GlobalCmdLink(_VOID);


#ifdef  __cplusplus
}
#endif

#endif /*end of #ifndef _CLI_CMDTREE_H_*/
