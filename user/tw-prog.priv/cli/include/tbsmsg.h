
/******************************************************************************
  Copyright (c), 1991-2007, T&W ELECTRONICS(SHENTHEN) Co., Ltd.

  文件名称: tbsmsg.h
  文件描述: 公共消息定义
  函数列表:
  修订记录:
         1. 作者: 武萌
            日期: 2007-9-5
            内容: 创建文件
 ******************************************************************************/

#ifndef __TBS_MSG_H__
#define __TBS_MSG_H__

#include <stdio.h>
#include <sys/types.h>
#include "tbstype.h"
#include "autoconf.h"

#ifdef _cplusplus
    #if _cplusplus
        extern "C" {
    #endif
#endif


#define MSG_OK               TBS_SUCCESS
#define ERR_MSG_MALLOC_FAIL  ERR_MALLOC_FAILED
#define ERR_MSG_PARA_INVALID ERR_PARA_INVALID
#define MSG_TIMEOUT           TBS_TIME_OUT





/* 通过mid得到pid */
#define MID2PID(mid) ((mid) >> 8)




/* 消息头 */
typedef struct tag_ST_MSGHEAD
{
    unsigned short usSrcMID;       /* 源模块MID */
    unsigned short usDstMID;       /* 目的模块MID */
    unsigned long  ulMsgID;        /* 消息ID----应用程序自定义 */
    unsigned short usMsgType;      /* 消息类型----应用程序自定义 */
    unsigned short usFirstMID;     /* 消息的最先发出者 */
    unsigned long  ulBodyLength;   /* 消息体长度 */
}ST_MSGHEAD;

/* 消息 */
typedef struct tag_ST_MSG
{
    ST_MSGHEAD  stMsgHead;
    char        szMsgBody[0];
}ST_MSG;


/* 模块接口返回值类型 */
typedef int  MSG_RET;

/* 回调消息处理函数指针 */
typedef void (*FUN_RECEIVE)(ST_MSG *pstMsg);

extern int s_iBtnSock;
extern pid_t btn_pid;

/* 模块注册, 注销接口 */
MSG_RET MSG_RegModule(unsigned short usMID, FUN_RECEIVE funReceive);
MSG_RET MSG_UnregModule(unsigned short usMID, FUN_RECEIVE funReceive);

void MSG_AllModStartOK(void);
void MSG_AllModDone(void);

/* 消息包构建，释放接口 */
ST_MSG *MSG_CreateMessage(unsigned long ulSize);
void MSG_ReleaseMessage(ST_MSG *pstMsg);


/* 消息发送，接收接口 */
MSG_RET MSG_SendMessage(ST_MSG *pstMsg);
MSG_RET MSG_ReceiveMessage(ST_MSG **ppstMsg, unsigned long ulSecond);
extern int s_iMsgSocket;


/* 消息调度 */
void MSG_Poll(void);

void MSG_PollToIdle(void);
MSG_RET MSG_RegModuleGroup(unsigned short usMID, ...);
MSG_RET MSG_RegExternalModule(unsigned short usInterGID, unsigned short usExtMID);

#ifdef CONFIG_REPLACE_NODE_PREFIX

/*******************************************************************************
* Function    : MSG_ReplaceNodePrefix
* Description : 替换消息中的节点前缀。
*               只针对以下消息类型进行替换：
*                   MSG_CMM_GET_VAL
*                   MSG_CMM_SET_VAL
*                   MSG_CMM_GET_ATTR
*                   MSG_CMM_SET_ATTR
*                   MSG_CMM_GET_VAL_ACK
*                   MSG_CMM_GET_NAME_ACK
*                   MSG_CMM_ADD_NODE
*                   MSG_CMM_DEL_NODE
*                   MSG_CMM_GET_NAME
*               典型用法：
*                   收到TR069请求消息时：pstMsg = MSG_ReplaceNodePrefix(pstMsg, "X_TWSZ-COM_", "ZTE_");
*                   返回TR069应答消息时：pstMsg = MSG_ReplaceNodePrefix(pstMsg, "ZTE_", "X_TWSZ-COM_");
*
传入的消息不需要调用者释放；返回的消息如果不为空，需要调用者释放；
* Parameters  : pstOldMsg   -- 消息指针
*               szOldPrefix -- 替换前的节点前缀
*               szNewPrefix -- 替换后的节点前缀
* Return      : 新的消息的指针
* Author      : wuyun / 2009-02-14
* History     :
*******************************************************************************/

ST_MSG *MSG_ReplaceNodePrefix(ST_MSG *pstOldMsg, char *szOldPrefix, char *szNewPrefix);
#endif

#define ACS_PREFIX  "X_DLINK-COM"
#define CPE_PREFIX  "X_TWSZ-COM"


#ifdef _cplusplus
    #if _cplusplus
        }
    #endif
#endif


#endif /*__TBS_MSG_H__*/

