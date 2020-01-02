
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



/* 节点信息*/
typedef struct tag_ST_NodeInfo {
    const char * pszName;                        /* 节点名*/
    BOOL (* pCheckFunc)(const char * pszValue);    /* 节点值检查函数*/
    struct tag_ST_NodeInfo * pstNext;
    BOOL bReadOnly;                           /* 是否可写*/
}ST_NodeInfo;


/* 通用实例*/
typedef struct tag_ST_Instance {
    struct tag_ST_Instance *pstNext;            /* 下一个实例*/
    char **ppcIdxTable;                               /* 配置缓存索引表*/
    unsigned int nNodeCount;                        /* 节点个数*/
    const ST_NodeInfo *pstNodeInfoArray;   /* 节点信息数组*/
    ST_NodeInfo **ppstNodeInfoList;   /* 节点信息哈希表*/
}ST_Instance;


/* 配置树节点*/
typedef struct tag_ST_TreeNode {
    const char *pszName;
    const char *pszType;
    const char *pszValue;
    unsigned char ucIsStandard;
    unsigned char ucNeedSave;
    unsigned char ucWriteable;
}ST_TreeNode;


/* 字符串链表*/
typedef struct tag_ST_Str {
    char *pszStr;
    struct tag_ST_Str *pstNext;
}ST_Str;

typedef ST_Str ST_ErrInfo;
typedef ST_Str ST_ResultInfo;


/* 输出匹配信息*/
typedef struct tag_ST_ProcOutMatchInfo {
    const char * pszMatchCond;                                    /* 匹配条件 */
    int (* pMatchFunc)(void * pInstance, const char * pszOutStr);        /* 匹配处理函数 */
}ST_ProcOutMatchInfo;


/* 重启状态机的事件 */
typedef enum tagEN_SMEvent {
    RESTART,        /* 重启进程 */
    START_SUCC,    /* 进程启动成功 */
    START_FAIL,        /* 进程启动失败 */
    STOP_SUCC,        /* 进程终止成功 */
    STOP_FAIL,        /* 进程终止失败 */
}EN_SMEvent;

/* 重启状态机的状态*/
typedef enum tag_EN_SMState {
     SM_WAIT_START,    /* 等待进程启动 */
     SM_WAIT_STOP,        /* 等待进程终止 */
     SM_RESTART,        /* 重启中 */
     SM_STARTED,        /* 进程已启动 */
     SM_STOPPED,        /* 进程已终止 */
}EN_SMState;


/***********************内部消息关键字段定义********************************/
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

//Tr069定义可用ConnectionStatus值
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

#define VALUE_UP     "Up"                        //WAN模块发送的内部消息status值
#define VALUE_DOWN   "Down"                      //WAN模块发送的内部消息status值
#define BUTTON_CONNECTED   "Connect"             //pppoe手动拨号连接按钮的节点值
#define BUTTON_DISCONNETED   "Disconnect"        //pppoe手动拨号断开按钮的节点值

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

/* 将节点的值保存到配置缓存中*/
int SetToConfBuf(void *pInstance, const char *pszName, const char * pszValue, const char *pszPath);

/* 从配置缓存中获取节点的值 */
const char * GetFromConfBuf(void * pInstance, const char *pszName);

/* 释放实例的配置缓存 */
int FreeConfBuf(void *pInstance);

/* 释放模块的所有实例的配置缓存 */
int FreeAllConfBuf(void *pInstanceList);

/* 应用所有实例的最新配置 */
int ApplyAllConfig(void *pInstanceList, int (* pApplyFunc)(void * pInstance));


/* 根据节点名找到对应的 NodeInfo */
const ST_NodeInfo * FindNodeInfo(ST_NodeInfo **ppstNodeInfoList, const char * pszName);

/* 将节点信息添加到节点信息列表中 */
int AddNodeInfo(ST_NodeInfo **ppstNodeInfoList, ST_NodeInfo * pstNodeInfo);


/* 添加错误信息到错误信息链表中*/
int AddErrInfo(ST_ErrInfo **ppstErrInfoList, int iErrCode, const char *pszPath);

/* 添加结果信息到结果信息链表中*/
int AddResultInfo(ST_ResultInfo **ppstResultInfoList, const char *pszPath, const char *pszType, const char *pszValue);

/* 释放错误链表所有错误信息空间*/
int FreeAllStr(ST_Str **ppstStrList);

/* 释放错误链表所有错误信息空间*/
#define FreeAllErrInfo FreeAllStr

/* 释放结果链表所有结果信息空间*/
#define FreeAllResultInfo FreeAllStr


/* 添加子结点*/
int AddChildNode(const char *pszPath, const ST_TreeNode astTreeNodeList[], unsigned int nNodeCount);


/* 从SET 命令中获取节点路径*/
int GetPathFromCmd(const char *pszCmd, char *pszPath, unsigned long ulLen);

/* 从SET命令中解析出节点名、节点值和实例号 */
int ParseSetCmd(const char * pszSetCmd, char * pszName, char * pszValue, unsigned long aulIdxList[], int *piIdxCount);
int ParseSetCmdExact(const char *pszSetCmd, unsigned int nNameLen, unsigned int nValueLen, const char *pszFmt, ...);


/* 错误消息*/
int RespErrMsg(const ST_MSG *pstOrgMsg, unsigned long ulErrNo, ST_ErrInfo *pstErrInfoList);

/* 响应添加实例消息 */
int RespAddMsg(const ST_MSG *pstOrgMsg, unsigned long ulObjNo, unsigned long ulStatus);

/* 响应删除实例消息 */
int RespDelMsg(const ST_MSG *pstOrgMsg, unsigned long ulStatus);

/* 响应设置消息*/
int RespSetMsg(const ST_MSG *pstOrgMsg, unsigned long ulStatus);

/* 响应查询消息*/
int RespGetMsg(const ST_MSG *pstOrgMsg, ST_ResultInfo *pstResultInfoList);

/* 响应配置更新消息*/
int RespUpdateMsg(const ST_MSG *pstOrgMsg, unsigned long ulResult);

/* 获取配置值消息处理函数 */
int GetMsgHandler(ST_MSG *pstMsg);




/* 初始化实例*/
#define INIT_INSTANCE(instance, node_info_array, node_info_list)   \
    memset(instance, 0, sizeof(*instance));   \
    ((ST_Instance *)instance)->nNodeCount = sizeof(node_info_array) / sizeof(ST_NodeInfo);    \
    ((ST_Instance *)instance)->pstNodeInfoArray = node_info_array; \
    ((ST_Instance *)instance)->ppstNodeInfoList = node_info_list;


/* 字段操作宏 */

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


/* 实例的配置是否发生改变*/
#define CONF_HAS_CHANGED(instance) (NULL != ((ST_Instance *)instance)->ppcIdxTable)

/* 将BOOL 型值转换成字符串*/
#define BooltoStr(b)  (b ? "1" : "0")

/* 安全释放内存*/
#define safe_free(pointer) \
    { \
        if ( pointer ) \
        { \
            free(pointer); \
            pointer = NULL; \
        } \
    }

/* 安全的释放CFG 返回的链表*/
#define safe_free_cfg(pointer) \
    { \
        if ( pointer ) \
        { \
            CFG_MemFree(pointer); \
            pointer = NULL; \
        } \
    }

/* 安全的发送消息*/
#define safe_send_msg(pointer) \
    { \
        if ( pointer ) \
        { \
            MSG_SendMessage(pointer); \
            MSG_ReleaseMessage(pointer); \
            pointer = NULL; \
        } \
    }

/* 安全的释放消息*/
#define safe_free_msg(pointer) \
    { \
        if ( pointer ) \
        { \
            MSG_ReleaseMessage(pointer); \
            pointer = NULL; \
        } \
    }

/* 安全的关闭文件*/
#define safe_close_file(pointer) \
    { \
        if ( pointer ) \
        { \
            fclose(pointer); \
            pointer = NULL; \
        } \
    }

/* 判断CFG 的API 是返回成功或失败*/
#define CFG_RET_SUCCEED(ret)    ((ret) == CFG_OK)
#define CFG_RET_FAILED(ret)        ((ret) != CFG_OK)

/* 日志提示信息*/
#define INFO_SUCCESS            "Success! "
#define INFO_ERROR                "Error! "

/* 最大路径长度*/
#define MAX_PATH_LEN        257

/* 节点名最大长度 */
#define MAX_NODE_NAME_LEN   128

/* 节点类型最大长度 */
#define MAX_NODE_TYPE_LEN   30

/* 节点值最大长度 */
#define MAX_NODE_VALUE_LEN  517

/* 路径中最大节点个数*/
#define MAX_PATH_NODE_COUNT 30

/* 最大错误信息长度*/
#define MAX_ERROR_INFO_LEN  128

/* 英文字母的个数*/
#define LETTER_COUNT        26

/* 行的最大长度*/
#define MAX_LINE_LEN        81

/* 系统执行命令最大长度 */
#define SYS_CMD_LEN         256
#define MAX_CMD_LEN         256

/* max items of mac addr whitelist*/
#define MAX_MAC_LIST_COUNT  20

/* 一些常用数据长度 */
#define MAX_IP_LEN          16
#define MAX_IP6_LEN         46
#define MAX_NETMASK_LEN     16
#define MAX_PORT_LEN        6
#define MAX_MAC_LEN         18
#define MAX_DNS_LEN         68
#define MAX_IF_NAME_LEN     16

/* 最大Mac地址长度 */
#define MAX_MAC_ADDR_LEN  20


/* IP地址 */
#define NIPQUAD(addr) \
    ((unsigned char *)&addr)[0], \
    ((unsigned char *)&addr)[1], \
    ((unsigned char *)&addr)[2], \
    ((unsigned char *)&addr)[3]
#define NIPQUAD_FMT "%u.%u.%u.%u"

/* Mac地址 */
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
Description:   设置进程优先级
Input:         char *pName, char *pFile, char *pValue
Return:        无
Others:        pid_poll_set("/usr/bin/pc", "oom_adj", "-17");
************************************************************/
int pid_poll_set(char *pName, char *pFile, char *pValue);


/************************************************************
Function:      bool ModuleExist(const char *pszName)
Description:   判断kenel module是否已经加载
Input:         const char *pszName      模块名称
Return:        无
Others:        ModuleExist("NetUsb");
************************************************************/
int ModuleExist(const char *pszName);


#endif /*__COMMON_H__*/

