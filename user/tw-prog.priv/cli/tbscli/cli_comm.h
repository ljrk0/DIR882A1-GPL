/**********************************************************************
 Copyright (c), 1991-2007, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 文件名称 : cli_comm.h
 文件描述 : 命令行通信处理的头文件


 函数列表 :


 修订记录 :
          1 创建 : liuzequn
            日期 : 2007-11-6
            描述 :

**********************************************************************/


#ifndef __CLI_COMM_H__
#define __CLI_COMM_H__

#include "cli_pub.h"
#include "cli/cli_private.h"

/******************************************************************************
 *                                 MACRO                                      *
 ******************************************************************************/


#define CLI_NODE__LEN       512     // 节点长度范围
#define CLI_LEAF__LEN       512      // 叶子长度范围

#define CLI_MAX_OBJ_NUM     64      // 每种OBJECT最大个数

#define CLI_MAX_RWOBJ_NUM  16       // 每次读写操作的最大数目


#define CLI_COMM_TIMEOUT    15

#define MAX_FAIL_TIMES 3            //用户验证失败的最大允许次数
#define MAX_USER_COUNT 6            //最多允许的用户数

#define AUTH_KEY            "$1$TW" /* 用于密码加密的密钥 ，要
                                     * 跟apps/logic/auth/auth.h
                                     * 定义的要一致
                                     */
/* 以下用于AUTH 消息的result 值，同步logic中的AUTH模块 */
#define CLI_AUTH_SUCCESS         0       /* 成功 */
/*  用于身份认证消息 */
#define CLI_AUTH_WRONG_NAME      1       /* 用户名错 */
#define CLI_AUTH_WRONG_PASSWORD  2       /* 密码错 */
#define CLI_AUTH_SESSION_FULL    3       /* 登录用户过多 */
/* 用于鉴权消息 */
#define CLI_AUTH_CONTROL_FAILED  4       /* 未登录 */
#define CLI_AUTH_SESSION_TIMEOUT 5       /* session超时*/
#define CLI_AUTH_LOGOUT_SUCCESS  6       /*用户注销成功*/
#define CLI_AUTH_LOGIN_FREQUENT  7       /*登录出错频繁*/

 /*
 *释放结构体指针对应的内存，并把指针赋值为空
 *pstMsg就是需要释放的结构体指针
 */
#define CLI_MSG_RELEASE(pstMsg) do \
{ \
    if ((pstMsg) != NULL) \
    { \
        MSG_ReleaseMessage((pstMsg)); \
        (pstMsg) = NULL; \
    } \
}while(0);


/******************************************************************************
 *                                 ENUM                                       *
 ******************************************************************************/


/******************************************************************************
 *                                STRUCT                                      *
 ******************************************************************************/
typedef struct tag_ST_CLISTR_LINK
{
    char *pstrValue;
    struct tag_ST_CLISTR_LINK *pNext;
} ST_CLISTR_LINK;

//用户信息，对应于auth模块中的单个用户信息
struct stUserInfo
{
    struct stUserInfo *pNext;
    char  szUsername[32];
    char  szPassword[32];
    ULONG ulLevel;
};

struct stUserInfoList
{
    struct stUserInfo arrayUserInfo[MAX_USER_COUNT];
    int    iUserCount;        //用来记录用户个数
};





/******************************************************************************
 *                               FUNCTION                                     *
 ******************************************************************************/

void CLI_ComStatusStart();
void CLI_ComStatusEnd();

void CLI_ComErrorAdd(PCSTR szErrorInfo);

ULONG CLI_ComMsgInteract(ST_MSG *pstMsg, USHORT usType);

int CLI_AddStrToLink(ST_CLISTR_LINK **ppHead, char *szStr);

void CLI_DestroyStrLink(ST_CLISTR_LINK *pHead);


int CLI_GetUserInfo(void);
ULONG CLI_Authenticate(char *szUsername, char *szPassword, ULONG *ulIndex);

/*=========================================================================*/
/*  函数名称: CLI_CommInit                                                 */
/*  函数功能: 命令行通信初始化                                             */
/*  输入参数:                                                              */
/*  返  回  : 成功、失败                                                   */
/*  创  建  : liuzequn / 2007-11-6                                         */
/*=========================================================================*/
ULONG CLI_CommInit();

// 单个叶子节点信息的获取
ULONG CLI_GetLeaf(char *szLeafName, char *szLeafVal);

//发送MSG_CMM_GET_VAL并返回GET的结果(消息结构指针)
ST_MSG *CLI_GetNode(char *pszNodePath);

// 对象下一层节点名称的获取
int CLI_GetNextLevelName(char *szPathName, char aszNextLevel[CLI_MAX_OBJ_NUM][CLI_LEAF__LEN]);

// 对象节点信息的获取
// 注意本函数返回的字符串链表需要调用者删除
ST_CLISTR_LINK *CLI_GetObject(char *szPathName, int nValNum, char aszLeafName[][CLI_LEAF__LEN]);

// 叶子节点信息的设置
int CLI_SetLeaf(int nValNum, char aszName[][CLI_NODE__LEN], char aszVal[][CLI_NODE__LEN]);

// 对象节点信息的添加
int CLI_AddObject(char *szPathName);

//添加一个对象节点
int CLI_AddObjectEx(char *szPathName, ULONG *pulInstIndex);

// 对象节点信息的删除
int CLI_DelObject(char *szPathName);

void CLI_CommErrorPrint();









/******************************************************************************
 *                                 END                                        *
 ******************************************************************************/


#endif /* __CLI_COMM_H__ */



