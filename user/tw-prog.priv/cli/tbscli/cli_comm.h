/**********************************************************************
 Copyright (c), 1991-2007, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 �ļ����� : cli_comm.h
 �ļ����� : ������ͨ�Ŵ����ͷ�ļ�


 �����б� :


 �޶���¼ :
          1 ���� : liuzequn
            ���� : 2007-11-6
            ���� :

**********************************************************************/


#ifndef __CLI_COMM_H__
#define __CLI_COMM_H__

#include "cli_pub.h"
#include "cli/cli_private.h"

/******************************************************************************
 *                                 MACRO                                      *
 ******************************************************************************/


#define CLI_NODE__LEN       512     // �ڵ㳤�ȷ�Χ
#define CLI_LEAF__LEN       512      // Ҷ�ӳ��ȷ�Χ

#define CLI_MAX_OBJ_NUM     64      // ÿ��OBJECT������

#define CLI_MAX_RWOBJ_NUM  16       // ÿ�ζ�д�����������Ŀ


#define CLI_COMM_TIMEOUT    15

#define MAX_FAIL_TIMES 3            //�û���֤ʧ�ܵ�����������
#define MAX_USER_COUNT 6            //���������û���

#define AUTH_KEY            "$1$TW" /* ����������ܵ���Կ ��Ҫ
                                     * ��apps/logic/auth/auth.h
                                     * �����Ҫһ��
                                     */
/* ��������AUTH ��Ϣ��result ֵ��ͬ��logic�е�AUTHģ�� */
#define CLI_AUTH_SUCCESS         0       /* �ɹ� */
/*  ���������֤��Ϣ */
#define CLI_AUTH_WRONG_NAME      1       /* �û����� */
#define CLI_AUTH_WRONG_PASSWORD  2       /* ����� */
#define CLI_AUTH_SESSION_FULL    3       /* ��¼�û����� */
/* ���ڼ�Ȩ��Ϣ */
#define CLI_AUTH_CONTROL_FAILED  4       /* δ��¼ */
#define CLI_AUTH_SESSION_TIMEOUT 5       /* session��ʱ*/
#define CLI_AUTH_LOGOUT_SUCCESS  6       /*�û�ע���ɹ�*/
#define CLI_AUTH_LOGIN_FREQUENT  7       /*��¼����Ƶ��*/

 /*
 *�ͷŽṹ��ָ���Ӧ���ڴ棬����ָ�븳ֵΪ��
 *pstMsg������Ҫ�ͷŵĽṹ��ָ��
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

//�û���Ϣ����Ӧ��authģ���еĵ����û���Ϣ
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
    int    iUserCount;        //������¼�û�����
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
/*  ��������: CLI_CommInit                                                 */
/*  ��������: ������ͨ�ų�ʼ��                                             */
/*  �������:                                                              */
/*  ��  ��  : �ɹ���ʧ��                                                   */
/*  ��  ��  : liuzequn / 2007-11-6                                         */
/*=========================================================================*/
ULONG CLI_CommInit();

// ����Ҷ�ӽڵ���Ϣ�Ļ�ȡ
ULONG CLI_GetLeaf(char *szLeafName, char *szLeafVal);

//����MSG_CMM_GET_VAL������GET�Ľ��(��Ϣ�ṹָ��)
ST_MSG *CLI_GetNode(char *pszNodePath);

// ������һ��ڵ����ƵĻ�ȡ
int CLI_GetNextLevelName(char *szPathName, char aszNextLevel[CLI_MAX_OBJ_NUM][CLI_LEAF__LEN]);

// ����ڵ���Ϣ�Ļ�ȡ
// ע�Ȿ�������ص��ַ���������Ҫ������ɾ��
ST_CLISTR_LINK *CLI_GetObject(char *szPathName, int nValNum, char aszLeafName[][CLI_LEAF__LEN]);

// Ҷ�ӽڵ���Ϣ������
int CLI_SetLeaf(int nValNum, char aszName[][CLI_NODE__LEN], char aszVal[][CLI_NODE__LEN]);

// ����ڵ���Ϣ�����
int CLI_AddObject(char *szPathName);

//���һ������ڵ�
int CLI_AddObjectEx(char *szPathName, ULONG *pulInstIndex);

// ����ڵ���Ϣ��ɾ��
int CLI_DelObject(char *szPathName);

void CLI_CommErrorPrint();









/******************************************************************************
 *                                 END                                        *
 ******************************************************************************/


#endif /* __CLI_COMM_H__ */



