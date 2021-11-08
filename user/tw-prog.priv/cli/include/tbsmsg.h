
/******************************************************************************
  Copyright (c), 1991-2007, T&W ELECTRONICS(SHENTHEN) Co., Ltd.

  �ļ�����: tbsmsg.h
  �ļ�����: ������Ϣ����
  �����б�:
  �޶���¼:
         1. ����: ����
            ����: 2007-9-5
            ����: �����ļ�
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





/* ͨ��mid�õ�pid */
#define MID2PID(mid) ((mid) >> 8)




/* ��Ϣͷ */
typedef struct tag_ST_MSGHEAD
{
    unsigned short usSrcMID;       /* Դģ��MID */
    unsigned short usDstMID;       /* Ŀ��ģ��MID */
    unsigned long  ulMsgID;        /* ��ϢID----Ӧ�ó����Զ��� */
    unsigned short usMsgType;      /* ��Ϣ����----Ӧ�ó����Զ��� */
    unsigned short usFirstMID;     /* ��Ϣ�����ȷ����� */
    unsigned long  ulBodyLength;   /* ��Ϣ�峤�� */
}ST_MSGHEAD;

/* ��Ϣ */
typedef struct tag_ST_MSG
{
    ST_MSGHEAD  stMsgHead;
    char        szMsgBody[0];
}ST_MSG;


/* ģ��ӿڷ���ֵ���� */
typedef int  MSG_RET;

/* �ص���Ϣ������ָ�� */
typedef void (*FUN_RECEIVE)(ST_MSG *pstMsg);

extern int s_iBtnSock;
extern pid_t btn_pid;

/* ģ��ע��, ע���ӿ� */
MSG_RET MSG_RegModule(unsigned short usMID, FUN_RECEIVE funReceive);
MSG_RET MSG_UnregModule(unsigned short usMID, FUN_RECEIVE funReceive);

void MSG_AllModStartOK(void);
void MSG_AllModDone(void);

/* ��Ϣ���������ͷŽӿ� */
ST_MSG *MSG_CreateMessage(unsigned long ulSize);
void MSG_ReleaseMessage(ST_MSG *pstMsg);


/* ��Ϣ���ͣ����սӿ� */
MSG_RET MSG_SendMessage(ST_MSG *pstMsg);
MSG_RET MSG_ReceiveMessage(ST_MSG **ppstMsg, unsigned long ulSecond);
extern int s_iMsgSocket;


/* ��Ϣ���� */
void MSG_Poll(void);

void MSG_PollToIdle(void);
MSG_RET MSG_RegModuleGroup(unsigned short usMID, ...);
MSG_RET MSG_RegExternalModule(unsigned short usInterGID, unsigned short usExtMID);

#ifdef CONFIG_REPLACE_NODE_PREFIX

/*******************************************************************************
* Function    : MSG_ReplaceNodePrefix
* Description : �滻��Ϣ�еĽڵ�ǰ׺��
*               ֻ���������Ϣ���ͽ����滻��
*                   MSG_CMM_GET_VAL
*                   MSG_CMM_SET_VAL
*                   MSG_CMM_GET_ATTR
*                   MSG_CMM_SET_ATTR
*                   MSG_CMM_GET_VAL_ACK
*                   MSG_CMM_GET_NAME_ACK
*                   MSG_CMM_ADD_NODE
*                   MSG_CMM_DEL_NODE
*                   MSG_CMM_GET_NAME
*               �����÷���
*                   �յ�TR069������Ϣʱ��pstMsg = MSG_ReplaceNodePrefix(pstMsg, "X_TWSZ-COM_", "ZTE_");
*                   ����TR069Ӧ����Ϣʱ��pstMsg = MSG_ReplaceNodePrefix(pstMsg, "ZTE_", "X_TWSZ-COM_");
*
�������Ϣ����Ҫ�������ͷţ����ص���Ϣ�����Ϊ�գ���Ҫ�������ͷţ�
* Parameters  : pstOldMsg   -- ��Ϣָ��
*               szOldPrefix -- �滻ǰ�Ľڵ�ǰ׺
*               szNewPrefix -- �滻��Ľڵ�ǰ׺
* Return      : �µ���Ϣ��ָ��
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

