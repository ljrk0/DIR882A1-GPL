
#ifndef __COMMON_H__
#define __COMMON_H__


#ifdef _cplusplus
    #if _cplusplus
        extern "C" {
    #endif
#endif




#include <stdio.h>
#include <string.h>
#include <time.h>
#include "tbsmsg.h"
#include "tbserror.h"
#include "tbsipv6.h"
#include "tbsutil.h"
#include "cfg_api.h"
#include "pc_api.h"
#include "tbstype.h"
#include "warnlog.h"



/* �ڵ���Ϣ*/
typedef struct tag_ST_NodeInfo {
    const char * pszName;                        /* �ڵ���*/
    BOOL (* pCheckFunc)(const char * pszValue);    /* �ڵ�ֵ��麯��*/
    struct tag_ST_NodeInfo * pstNext;
    BOOL bReadOnly;                           /* �Ƿ��д*/
}ST_NodeInfo;


/* ͨ��ʵ��*/
typedef struct tag_ST_Instance {
    struct tag_ST_Instance *pstNext;            /* ��һ��ʵ��*/
    char **ppcIdxTable;                               /* ���û���������*/
    unsigned int nNodeCount;                        /* �ڵ����*/
    const ST_NodeInfo *pstNodeInfoArray;   /* �ڵ���Ϣ����*/
    ST_NodeInfo **ppstNodeInfoList;   /* �ڵ���Ϣ��ϣ��*/
}ST_Instance;


/* �������ڵ�*/
typedef struct tag_ST_TreeNode {
    const char *pszName;
    const char *pszType;
    const char *pszValue;
    unsigned char ucIsStandard;
    unsigned char ucNeedSave;
    unsigned char ucWriteable;
}ST_TreeNode;


/* �ַ�������*/
typedef struct tag_ST_Str {
    char *pszStr;
    struct tag_ST_Str *pstNext;
}ST_Str;

typedef ST_Str ST_ErrInfo;
typedef ST_Str ST_ResultInfo;


/* ���ƥ����Ϣ*/
typedef struct tag_ST_ProcOutMatchInfo {
    const char * pszMatchCond;                                    /* ƥ������ */
    int (* pMatchFunc)(void * pInstance, const char * pszOutStr);        /* ƥ�䴦���� */
}ST_ProcOutMatchInfo;


/* ����״̬�����¼� */
typedef enum tagEN_SMEvent {
    RESTART,        /* �������� */
    START_SUCC,    /* ���������ɹ� */
    START_FAIL,        /* ��������ʧ�� */
    STOP_SUCC,        /* ������ֹ�ɹ� */
    STOP_FAIL,        /* ������ֹʧ�� */
}EN_SMEvent;

/* ����״̬����״̬*/
typedef enum tag_EN_SMState {
     SM_WAIT_START,    /* �ȴ��������� */
     SM_WAIT_STOP,        /* �ȴ�������ֹ */
     SM_RESTART,        /* ������ */
     SM_STARTED,        /* ���������� */
     SM_STOPPED,        /* ��������ֹ */
}EN_SMState;


/***********************�ڲ���Ϣ�ؼ��ֶζ���********************************/
#define KEY_PATH   "Path"
#define KEY_NAME   "Name"
#define KEY_ENABLE   "Enable"
#define KEY_INTERFACE_NAME   "InterfaceName"
#define KEY_REMOTE_IP   "RemoteIPAddress"
#define KEY_EXTERNAL_IP   "ExternalIPAddress"
#define KEY_NETMASK   "SubnetMask"
#define KEY_STATUS   "Status"
#define KEY_DNS_SERVER   "DNSServers"
#define KEY_DEST_IP             "DestIp"

//Tr069�������ConnectionStatusֵ
#define VALUE_UNCONFIGURED    "Unconfigured"
#define VALUE_CONNECTING       "Connecting"
#define VALUE_CONNECTED       "Connected"
#define VALUE_DISCONNECTING    "Disconnecting"
#define VALUE_DISCONNECTED    "Disconnected"
#define VALUE_GLOBALCONNECTED       "GlobalConnected"
#define VALUE_LINKLOCALCONNECTED    "LinkLocalConnected"
#define STR_PROTO_TYPE_IPV4   "IPv4"
#define STR_PROTO_TYPE_IPV6   "IPv6"
#define STR_PROTO_TYPE_IPV4_6 "IPv4_6"

#define VALUE_ENABLE  "Enabled"
#define VALUE_DISABLE "Disabled"
#define VALUE_ERROR     "Error"

#define VALUE_UP     "Up"                        //WANģ�鷢�͵��ڲ���Ϣstatusֵ
#define VALUE_DOWN   "Down"                      //WANģ�鷢�͵��ڲ���Ϣstatusֵ
#define BUTTON_CONNECTED   "Connect"             //pppoe�ֶ��������Ӱ�ť�Ľڵ�ֵ
#define BUTTON_DISCONNETED   "Disconnect"        //pppoe�ֶ����ŶϿ���ť�Ľڵ�ֵ

#define MODE_ALLOWED   "Allowed"
#define MODE_BLOCKED   "Blocked"

#define VALUE_BLACK  "Black"
#define VALUE_WHILE  "White"

#define KEY_MAC             "Mac"
#define KEY_DEVICEIP        "DeviceIp"
#define KEY_DEVICETYPE      "DeviceType"
#define KEY_CONNECT_NAME    "ConnectName"

#ifndef CPE_PREFIX
#define CPE_PREFIX  "X_TWSZ-COM"
#endif

/* ���ڵ��ֵ���浽���û�����*/
int SetToConfBuf(void *pInstance, const char *pszName, const char * pszValue, const char *pszPath);

/* �����û����л�ȡ�ڵ��ֵ */
const char * GetFromConfBuf(void * pInstance, const char *pszName);

/* �ͷ�ʵ�������û��� */
int FreeConfBuf(void *pInstance);

/* �ͷ�ģ�������ʵ�������û��� */
int FreeAllConfBuf(void *pInstanceList);

/* Ӧ������ʵ������������ */
int ApplyAllConfig(void *pInstanceList, int (* pApplyFunc)(void * pInstance));


/* ���ݽڵ����ҵ���Ӧ�� NodeInfo */
const ST_NodeInfo * FindNodeInfo(ST_NodeInfo **ppstNodeInfoList, const char * pszName);

/* ���ڵ���Ϣ��ӵ��ڵ���Ϣ�б��� */
int AddNodeInfo(ST_NodeInfo **ppstNodeInfoList, ST_NodeInfo * pstNodeInfo);


/* ��Ӵ�����Ϣ��������Ϣ������*/
int AddErrInfo(ST_ErrInfo **ppstErrInfoList, int iErrCode, const char *pszPath);

/* ��ӽ����Ϣ�������Ϣ������*/
int AddResultInfo(ST_ResultInfo **ppstResultInfoList, const char *pszPath, const char *pszType, const char *pszValue);

/* �ͷŴ����������д�����Ϣ�ռ�*/
int FreeAllStr(ST_Str **ppstStrList);

/* �ͷŴ����������д�����Ϣ�ռ�*/
#define FreeAllErrInfo FreeAllStr

/* �ͷŽ���������н����Ϣ�ռ�*/
#define FreeAllResultInfo FreeAllStr


/* ����ӽ��*/
int AddChildNode(const char *pszPath, const ST_TreeNode astTreeNodeList[], unsigned int nNodeCount);


/* ��SET �����л�ȡ�ڵ�·��*/
int GetPathFromCmd(const char *pszCmd, char *pszPath, unsigned long ulLen);

/* ��SET�����н������ڵ������ڵ�ֵ��ʵ���� */
int ParseSetCmd(const char * pszSetCmd, char * pszName, char * pszValue, unsigned long aulIdxList[], int *piIdxCount);
int ParseSetCmdExact(const char *pszSetCmd, unsigned int nNameLen, unsigned int nValueLen, const char *pszFmt, ...);


/* ������Ϣ*/
int RespErrMsg(const ST_MSG *pstOrgMsg, unsigned long ulErrNo, ST_ErrInfo *pstErrInfoList);

/* ��Ӧ���ʵ����Ϣ */
int RespAddMsg(const ST_MSG *pstOrgMsg, unsigned long ulObjNo, unsigned long ulStatus);

/* ��Ӧɾ��ʵ����Ϣ */
int RespDelMsg(const ST_MSG *pstOrgMsg, unsigned long ulStatus);

/* ��Ӧ������Ϣ*/
int RespSetMsg(const ST_MSG *pstOrgMsg, unsigned long ulStatus);

/* ��Ӧ��ѯ��Ϣ*/
int RespGetMsg(const ST_MSG *pstOrgMsg, ST_ResultInfo *pstResultInfoList);

/* ��Ӧ���ø�����Ϣ*/
int RespUpdateMsg(const ST_MSG *pstOrgMsg, unsigned long ulResult);

/* ��ȡ����ֵ��Ϣ������ */
int GetMsgHandler(ST_MSG *pstMsg);




/* ��ʼ��ʵ��*/
#define INIT_INSTANCE(instance, node_info_array, node_info_list)   \
    memset(instance, 0, sizeof(*instance));   \
    ((ST_Instance *)instance)->nNodeCount = sizeof(node_info_array) / sizeof(ST_NodeInfo);    \
    ((ST_Instance *)instance)->pstNodeInfoArray = node_info_array; \
    ((ST_Instance *)instance)->ppstNodeInfoList = node_info_list;


/* �ֶβ����� */

#define GET_UCHAR(p, value) \
    value = *( (unsigned char *)p ); \
    p += sizeof(unsigned char);

#define SET_UCHAR(p, value) \
    *( (unsigned char *)p ) = value; \
    p += sizeof(unsigned char);

#define GET_ULONG(p, value) \
    value = *( (unsigned long *)p ); \
    p += sizeof(unsigned long);

#define SET_ULONG(p, value) \
    *( (unsigned long *)p ) = value; \
    p += sizeof(unsigned long);

#define GET_STRN(p, value, n) \
    safe_strncpy(value, p, n); \
    (p) += strlen(p) + 1;

#define SET_STRN(p, value, n) \
    safe_strncpy(p, value, n); \
    (p) += strlen(p) + 1;

#define GET_STR(p, value) \
    value = (char *)(p); \
    (p) += strlen(p) + 1;

#define SET_STR(p, value) \
    strcpy(p, value); \
    (p) += strlen(p) + 1;


/* ʵ���������Ƿ����ı�*/
#define CONF_HAS_CHANGED(instance) (NULL != ((ST_Instance *)instance)->ppcIdxTable)

/* ��BOOL ��ֵת�����ַ���*/
#define BooltoStr(b)  (b ? "1" : "0")

/* ��ȫ�ͷ��ڴ�*/
#define safe_free(pointer) \
    { \
        if ( pointer ) \
        { \
            free(pointer); \
            pointer = NULL; \
        } \
    }

/* ��ȫ���ͷ�CFG ���ص�����*/
#define safe_free_cfg(pointer) \
    { \
        if ( pointer ) \
        { \
            CFG_MemFree(pointer); \
            pointer = NULL; \
        } \
    }

/* ��ȫ�ķ�����Ϣ*/
#define safe_send_msg(pointer) \
    { \
        if ( pointer ) \
        { \
            MSG_SendMessage(pointer); \
            MSG_ReleaseMessage(pointer); \
            pointer = NULL; \
        } \
    }

/* ��ȫ���ͷ���Ϣ*/
#define safe_free_msg(pointer) \
    { \
        if ( pointer ) \
        { \
            MSG_ReleaseMessage(pointer); \
            pointer = NULL; \
        } \
    }

/* ��ȫ�Ĺر��ļ�*/
#define safe_close_file(pointer) \
    { \
        if ( pointer ) \
        { \
            fclose(pointer); \
            pointer = NULL; \
        } \
    }

/* �ж�CFG ��API �Ƿ��سɹ���ʧ��*/
#define CFG_RET_SUCCEED(ret)    ((ret) == CFG_OK)
#define CFG_RET_FAILED(ret)        ((ret) != CFG_OK)

/* ��־��ʾ��Ϣ*/
#define INFO_SUCCESS            "Success! "
#define INFO_ERROR                "Error! "

/* ���·������*/
#define MAX_PATH_LEN        257

/* �ڵ�����󳤶� */
#define MAX_NODE_NAME_LEN   128

/* �ڵ�������󳤶� */
#define MAX_NODE_TYPE_LEN   30

/* �ڵ�ֵ��󳤶� */
#define MAX_NODE_VALUE_LEN  517

/* ·�������ڵ����*/
#define MAX_PATH_NODE_COUNT 30

/* ��������Ϣ����*/
#define MAX_ERROR_INFO_LEN  128

/* Ӣ����ĸ�ĸ���*/
#define LETTER_COUNT        26

/* �е���󳤶�*/
#define MAX_LINE_LEN        81

/* ϵͳִ��������󳤶� */
#define SYS_CMD_LEN         256
#define MAX_CMD_LEN         256

/* max items of mac addr whitelist*/
#define MAX_MAC_LIST_COUNT  20

/* һЩ�������ݳ��� */
#define MAX_IP_LEN          16
#define MAX_IP6_LEN         46
#define MAX_NETMASK_LEN     16
#define MAX_PORT_LEN        6
#define MAX_MAC_LEN         18
#define MAX_DNS_LEN         68
#define MAX_IF_NAME_LEN     16

/* ���Mac��ַ���� */
#define MAX_MAC_ADDR_LEN  20


/* IP��ַ */
#define NIPQUAD(addr) \
    ((unsigned char *)&addr)[0], \
    ((unsigned char *)&addr)[1], \
    ((unsigned char *)&addr)[2], \
    ((unsigned char *)&addr)[3]
#define NIPQUAD_FMT "%u.%u.%u.%u"

/* Mac��ַ */
#define NMACQUAD(addr) \
    ((unsigned char *)addr)[0], \
    ((unsigned char *)addr)[1], \
    ((unsigned char *)addr)[2], \
    ((unsigned char *)addr)[3], \
    ((unsigned char *)addr)[4], \
    ((unsigned char *)addr)[5]
#define NMACQUAD_FMT "%02x:%02x:%02x:%02x:%02x:%02x"

#ifdef _cplusplus
    #if _cplusplus
        }
    #endif
#endif

/************************************************************
Function:      int pid_poll_set(char *pName, char *pFile, char *pValue)
Description:   ���ý������ȼ�
Input:         char *pName, char *pFile, char *pValue
Return:        ��
Others:        pid_poll_set("/usr/bin/pc", "oom_adj", "-17");
************************************************************/
int pid_poll_set(char *pName, char *pFile, char *pValue);


/************************************************************
Function:      bool ModuleExist(const char *pszName)
Description:   �ж�kenel module�Ƿ��Ѿ�����
Input:         const char *pszName      ģ������
Return:        ��
Others:        ModuleExist("NetUsb");
************************************************************/
int ModuleExist(const char *pszName);


#endif /*__COMMON_H__*/

