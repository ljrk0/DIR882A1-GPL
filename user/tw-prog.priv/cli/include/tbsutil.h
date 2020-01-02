/**********************************************************************
 Copyright (c), 1991-2007, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 文件名称 : tbsutil.h
 文件描述 : TBS 公共工具方法定义


 修订记录 :
          1 创建 : TBS
            日期 : 2007-11-14
            描述 : 从内核中把双向链表搬过来用
          2 修改 : 翟海琛
            日期 : 2008-9-3
            描述 : 原双向链表定义和系统头文件冲突，修改定义解决冲突

**********************************************************************/

#ifndef __TBS_UTIL_H__
#define __TBS_UTIL_H__

#define _GNU_SOURCE
#include "autoconf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "tbstype.h"
#include "common.h"



/* 终端中使用的颜色常量定义 */
#define ECHO_NONE           "\033[0m"

#define ECHO_RED            "\033[0;31m"
#define ECHO_GREEN          "\033[0;32m"
#define ECHO_YELLOW         "\033[0;33m"
#define ECHO_BLUE           "\033[0;34m"
#define ECHO_PURPLE         "\033[0;35m"

/* 闪烁 */
#define ECHO_FLASH_RED      "\033[5;31m"
#define ECHO_FLASH_GREEN    "\033[5;32m"
#define ECHO_FLASH_YELLOW   "\033[5;33m"
#define ECHO_FLASH_BLUE     "\033[5;34m"
#define ECHO_FLASH_PURPLE   "\033[5;35m"



#define MAX_IP_LIST_LEN (16 * MAX_IP_COUNT)
#define MAX_IP_COUNT 30

#define MAX_MAC_LIST_LEN (18 * MAX_MAC_COUNT)
#define MAX_MAC_COUNT 30
#ifdef CONFIG_APPS_LOGIC_DLink_AC750
#define MAX_USER_LENTH    16 
#define MAX_PASSWD_LEN    32 
#endif
#define MAX_USER_LEN    255      /* 用户名/密码长度 */



#define PATH_MAC_ADDRESS  "/proc/llconfig/macaddr"
#define MAX_VALUE_OF_HEX   0xFFFFFF
#define CYCLE_VALUE_OF_HEX  0x1000000
#define URL_STR_URL_MAX_LEN       256       /* URL最多充许的字符数*/
//extern char *strndup(const char *s, size_t n);
enum
{
	MAC_LAN_DEVICE = 0,
	MAC_WAN_DEVICE,
	MAC_WLAN_DEVICE,
	MAC_USB_DEVICE,
};


/******************************************************************************
 *                                 位操作                                   *
 ******************************************************************************/
#define SET_BIT(flag, bit)		(flag)|=(bit)
#define CLEAR_BIT(flag,bit)		(flag)&=~(bit)
#define TEST_BIT(flag,bit)		(flag)&(bit)

/******************************************************************************
 *                                 指针操作                                   *
 ******************************************************************************/
#define	SAFE_FREE(ptr)	free(ptr);ptr=NULL

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define ALIGN(x,a) (((x)+(a)-1)&~((a)-1))
#define FIELD_SIZEOF(t, f) (sizeof(((t*)0)->f))

/******************************************************************************
 *                                 IP操作                                   *
 ******************************************************************************/
#define NET_ADDR(ip, mask)              ((ip) & (mask))
#define HOST_ADDR(ip, mask)             ((ip) & ~(mask))

#define IS_NET_ADDR(ip, mask)           (HOST_ADDR(ip, mask) == 0)
#define IS_BROAD_ADDR(ip, mask)         (HOST_ADDR(ip, mask) == ~(mask))
#define IN_SAME_SUBNET(ip1, ip2, mask)  (NET_ADDR(ip1, mask) == NET_ADDR(ip2, mask))


#ifndef _SYS_QUEUE_H
/*
   TBS原定义的双向链表，和<sys/queue.h> 中的名称有冲突
   现在修改TBS的双向链表相关操作定义，在前面增加TBS前缀
   为了不影响现有模块，在没有引用<sys/queue.h>的时候，做宏的命名转换
   以后新增模块需要使用带TBS前缀的操作函数
*/
#define list_head       TBS_list_head
#define ST_LIST_HEAD    TBS_ST_LIST_HEAD
#define LIST_HEAD_INIT  TBS_LIST_HEAD_INIT
#define LIST_HEAD       TBS_LIST_HEAD
#define INIT_LIST_HEAD  TBS_INIT_LIST_HEAD
#define __list_add      TBS__list_add
#define list_add        TBS_list_add
#define list_add_tail   TBS_list_add_tail
#define __list_del      TBS__list_del
#define list_del        TBS_list_del
#define list_empty      TBS_list_empty
#define list_entry      TBS_list_entry
#define list_for_each   TBS_list_for_each
#define list_for_each_safe   TBS_list_for_each_safe
#define list_splice     TBS_list_splice

#endif
/******************************************************************************
 *                                 双向链表                                   *
 ******************************************************************************/

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

struct TBS_list_head {
    struct TBS_list_head *next, *prev;
};

typedef struct TBS_list_head TBS_ST_LIST_HEAD;  /* 按咱们的命名规范，用这个 */

#define TBS_LIST_HEAD_INIT(name) { &(name), &(name) }

#define TBS_LIST_HEAD(name) \
    struct TBS_list_head name = TBS_LIST_HEAD_INIT(name)

static inline void TBS_INIT_LIST_HEAD(TBS_ST_LIST_HEAD *list)
{
    list->next = list;
    list->prev = list;
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void TBS__list_add(TBS_ST_LIST_HEAD *new,
                              TBS_ST_LIST_HEAD *prev, TBS_ST_LIST_HEAD *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void TBS_list_add(TBS_ST_LIST_HEAD *new, TBS_ST_LIST_HEAD *head)
{
    TBS__list_add(new, head, head->next);
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void TBS_list_add_tail(TBS_ST_LIST_HEAD *new, TBS_ST_LIST_HEAD *head)
{
    TBS__list_add(new, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void TBS__list_del(TBS_ST_LIST_HEAD *prev, TBS_ST_LIST_HEAD *next)
{
    next->prev = prev;
    prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty on entry does not return true after this, the entry is
 * in an undefined state.
 */
static inline void TBS_list_del(TBS_ST_LIST_HEAD *entry)
{
    TBS__list_del(entry->prev, entry->next);
    TBS_INIT_LIST_HEAD(entry);
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int TBS_list_empty(const TBS_ST_LIST_HEAD *head)
{
    return head->next == head;
}

/**
 * TBS_list_one_node - tests whether a list has only one node
 * @head: the list to test
 */
static inline int TBS_list_one_node(const TBS_ST_LIST_HEAD *head)
{
    return head->next->next == head;
}

/**
 * list_entry - get the struct for this entry
 * @ptr:	the &TBS_ST_LIST_HEAD pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define TBS_list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/**
 * list_for_each	-	iterate over a list
 * @pos:	the &TBS_ST_LIST_HEAD to use as a loop cursor.
 * @head:	the head for your list.
 */
#define TBS_list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)


/**
 * list_for_each_safe    -    iterate over a list safe against removal of list entry
 * @pos:    the &struct list_head to use as a loop counter.
 * @n:        another &struct list_head to use as temporary storage
 * @head:    the head for your list.
 */
#define TBS_list_for_each_safe(pos, n, head) \
        for (pos = (head)->next, n = pos->next; pos != (head); \
            pos = n, n = pos->next)

/**
 * list_splice - join two lists
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void TBS_list_splice(TBS_ST_LIST_HEAD *list, TBS_ST_LIST_HEAD *head)
{
    TBS_ST_LIST_HEAD *first = list->next;

    if (first != list) {
        TBS_ST_LIST_HEAD *last = list->prev;
        TBS_ST_LIST_HEAD *at = head->next;

        first->prev = head;
        head->next = first;

        last->next = at;
        at->prev = last;
    }
}


static inline int TBS_list_node_count(const TBS_ST_LIST_HEAD *head)
{
    TBS_ST_LIST_HEAD *plhPos = NULL;
    int i = 0;

    TBS_list_for_each(plhPos, head)
    {
        i++;
    }

    return i;
}

/******************************************************************************
 *                                 字符串处理                                 *
 ******************************************************************************/
static inline int safe_strcmp(const char *s1, const char *s2)
{
    if (!s1 && !s2)
        return 0;
    if (!s1)
        return -1;
    if (!s2)
        return 1;
    return strcmp(s1, s2);
}

static inline int safe_strncmp(const char *s1, const char *s2, int n)
{
    if (!s1 && !s2)
        return 0;
    if (!s1)
        return -1;
    if (!s2)
        return 1;
    return strncmp(s1, s2, n);
}

static inline char *safe_strstr(const char *s1, const char *s2)
{
    if (!s1 || !s2)
        return NULL;
    return strstr(s1, s2);
}

static inline int safe_strlen(const char *s)
{
    if (!s)
        return 0;
    return strlen(s);
}

static inline int safe_atoi(const char *s)
{
    if (!s)
        return 0;
    return atoi(s);
}

static inline long safe_atol(const char *s)
{
    if (!s)
        return 0;
    return atol(s);
}

static inline unsigned long safe_atoul(const char *s)
{
    if (!s)
        return 0;
    return strtoul(s, NULL, 10);
}

static inline char * safe_strncpy(char *dst, const char *src, size_t size)
{
	if (!size) return dst;
	dst[--size] = '\0';
	return strncpy(dst, src, size);
}

size_t strlcpy(char *dst, const char *src, size_t siz);

size_t strlcat(char* dst, const char* src, size_t siz);



/******************************************************************************
 *                                通用工具方法                                *
 ******************************************************************************/

/*字符串匹配函数*/
BOOL tbsMatch(const char *pszString, const char *pszPattern);

/* tbsSystem命令打印开关,CONFIG_TBS_APPS_DEBUG_SYSCMD由make menuconfig控制 */
#ifdef CONFIG_TBS_APPS_DEBUG_SYSCMD
#define TBS_PRINT_CMD       1
#else
#define TBS_PRINT_CMD		0
#endif


#ifdef CONFIG_APPS_CMDDAEMON
/* 初始化配置模式 */
void tbsConfigModeInit(void);

/* 设置system异步模式 */
void tbsSetSystemAsyn(unsigned short usMID);

/* 完成system异步模式 */
void tbsSystemAsynComplete(void);
#else
#define tbsConfigModeInit()
#define tbsSetSystemAsyn(usMID)
#define tbsSystemAsynComplete()
#endif

/* 系统调用system的封装 */
int tbsSystem(char *command, int printFlag);

#define tbsSystemMute(cmd)  tbsSystem(cmd, 0)

/*将字符串IP转换成无符号长整型IP*/
unsigned long tbsCnvtIpFromStr(const char *pcIp);

/*
 * tbsCheckIpRange: 检查输入的两个IP地址是否在一个范围之内
 * pStartIp: 		起始IP地址
 * pEndIp: 			结束IP地址
 * 返回值:			如果两者在同一个返回之内,返回TBS_SUCCESS, 否则返回TBS_ERROR
 * 备注:			该函数不对IP地址的有效性做检查, 在调用之前最好使用
 					tbsCheckIpEx函数进行检查
*/
int tbsCheckIpRange(const char *pStartIp, const char *pEndIp);
/*将整型IP转换为带点的字符串IP*/
void tbsCnvtIpToStr(unsigned long ulIp, char *pszIp);

/*将字符串"1"或者"true"（不区分大小写），转换成BOOL类型，*/
BOOL tbsStrToBool(const char *pcValue);

/* 计算掩码中1的个数,非法掩码则返回负数 */
int tbsGetBitsByMask(const char *pcMask);


/* 根据指定字符分割字符串 */
int tbsSplitString(char *str, char *substr[], const char delimit, const int max_count);

/*
根据制定字符分割列表字符串
(形如: xx,yy,zz, 分割之后生成数组{xx, yy, zz})
*/
int tbsStrListSplit(char *strList, char delimit, char *apcStrArray[], int nMaxCnt);

/*
从一个列表字符串中查找其中一个元素
(形如: xx,yy,zz, 查找yy)
*/
int tbsStrListFind(const char *szList, char delimit, const char *szEntry);

/*
往一个列表字符串中添加一个元素
(形如: xx,yy, 添加zz, 则变成 xx,yy,zz)
*/
int tbsStrListAdd(char *szList, char delimit, const char *szEntry);

/*
从一个列表字符串中删除其中一个元素
(形如: xx,yy,zz, 删除yy, 则变成 xx,zz)
*/
int tbsStrListDel(char *szList, char delimit, const char *szDel);

void TW_printf(const void *buf, int len);

/*=========================================================================*/
/*                                                                                                                                                                    */
/*                                     老检查函数，为了版本兼容保留                                                  */
/*                                                                                                                                                                    */
/*=========================================================================*/

/* 检查IP合法性 */
BOOL tbsCheckIp(const char *pcip);

BOOL tbsCheckPort(const char *pszString);

BOOL tbsCheckPriority(const char *pszString);


/*检查子网掩码合法性*/
BOOL tbsCheckMask(const char *pcValue);



/* 检查BOOL 值的合法性*/
BOOL tbsCheckEnable(const char *pszValue);

/* 检查int 值的合法性*/
BOOL tbsCheckInt(const char *pszValue);

/* 检查unsigned int 值的合法性*/
BOOL tbsCheckUInt(const char *pszValue);

/*检查MAC地址合法性*/
BOOL tbsCheckMac(const char *pcMac);



/* 检查IP列表 */
BOOL tbsCheckIpList(const char *pszIpList, int iMaxCount);

/* 检查MAC 列表*/
BOOL tbsCheckMacList(const char *pszMacList, int iMaxCount);

/*=========================================================================*/
/*                                                                                                                                                                    */
/*                                     新检查函数，返回值改变为错误码新式                                 */
/*                                                                                                                                                                    */
/*=========================================================================*/

/*
使用传入的正则表达式匹配要检查的值,不匹配则返回指定的错误码
*/
int tbsCheckRegex(const char *pszValue, const char *pszPattern, int iErrNo);


/* 检查IP合法性 */
int tbsCheckIpEx(const char *pszValue);
int tbsCheckHostIpEx(const char *pszValue);

/* 检查ip6网络的合法性*/
int tbsCheckIpv6Net(const char *pszIpv6Net);

/*检查DNS IP合法性,允许0.0.0.0的IP存在*/
int tbsCheckDNSHostIpEx(const char *pszValue);

int tbsCheckPortEx(const char *pszString);

#if (defined(CONFIG_DSL_3785_TALKTALK_BT) || (defined CONFIG_CUSTOMER_dlink_EU))
int tbsCheckPortGroupEx(const char *pszString);
#endif

int tbsCheckPriorityEx(const char *pszString);

/*检查子网掩码合法性*/
int tbsCheckMaskEx(const char *pcValue);

/* 检查BOOL 值的合法性*/
int tbsCheckEnableEx(const char *pszValue);

/* 检查int 值的合法性*/
int tbsCheckIntEx(const char *pszValue);

/* 检查unsigned int 值的合法性*/
int tbsCheckUIntEx(const char *pszValue);

/* 检查int值是否在指定范围 */
int tbsCheckIntRangeEx(const char *pszValue, int nMin, int nMax);

/* 检查uint值是否在指定范围 */
int tbsCheckUIntRangeEx(const char *pszValue, unsigned int nMin, unsigned int nMax);

/*检查MAC地址合法性*/
int tbsCheckMacEx(const char *pcMac);

/* 检查IP列表 */
int tbsCheckIpListEx(const char *pszIpList, int iMaxCount);
int tbsCheckHostIpListEx(const char *pszIpList, int iMaxCount);

/*检查DNS IP列表*/
int tbsCheckDNSHostIpListEx(const char *pszIpList, int iMaxCount);


/* 检查MAC 列表*/
int tbsCheckMacListEx(const char *pszMacList, int iMaxCount);

/* 检查用户名/密码 */
int tbsCheckUserEx(const char *pszValue);
int tbsCheckUserExWithLength(const char *pszValue, const int iLen);
int tbsCheckPasswdEx(const char *pszValue);
int tbsCheckPasswdExWithLength(const char *pszValue, const int iLen);

/* 检查域名格式是否合法 */
int tbsCheckDomainName(const char *pszValue);

/* 检查域名或者IP的合法性 */
int tbsCheckHost(const char *pszValue);

/* 检查url合法性*/
int tbsCheckUrl(const char * pValue);

/*=========================================================================
*
*    Mem Pool function
*
*=========================================================================*/

/*********************************************************************
*  功能: 分配内存,并记录
*********************************************************************/
void *Mem_Malloc(size_t nSize, const char *szFile, const char *szFunc, unsigned long ulLine);

/*********************************************************************
*  功能: 为数组分配内存(会给分配的内存置0),并记录
*********************************************************************/
void *Mem_Calloc(size_t nMemb, size_t nSize, const char *szFile, const char *szFunc, unsigned long ulLine);

/*********************************************************************
*  功能: 重新分配内存( 释放/扩展原来分配的内存),并记录
*********************************************************************/
void *Mem_Realloc(void *pAddr, size_t nSize, const char *szFile, const char *szFunc, unsigned long ulLine);

/*********************************************************************
*  功能: 释放内存,并删除记录
*********************************************************************/
void Mem_Free(void *pAddr, const char *szFile, const char *szFunc, unsigned long ulLine);

/*********************************************************************
*  功能: 释放所有内存, 并删除所有记录
*********************************************************************/
void Mem_FreeAll(void);

/*********************************************************************
*  功能: 显示分配之后尚未释放的内存情况
*********************************************************************/
void Mem_PrintStatus(void);

/*********************************************************************
*  功能: 显示分配之后尚未释放的内存情况(已经调用此函数显示过的内存块不再显示)
*********************************************************************/
void Mem_PrintStatusOnce(void);

char *Mem_Strdup(const char *pcStr, const char *szFile, const char *szFunc, unsigned long ulLine);
char *Mem_Strndup(const char *pcStr, size_t nSize, const char *szFile, const char *szFunc, unsigned long ulLine);


/*********************************************************************
*  名称: tbsAddMac

*  功能: 增加mac地址，支持LAN和WAN设备的修改

*  参数: pIfName为设备名,如eth0
		 iInterfaceType为MAC_LAN_DEVICE或者MAC_WAN_DEVICE
		 szMacVal  返回给调用者的mac地址, 内存由调用者申请

*  返回: TBS_SUCCESS/TBS_FAILED

*  创建: zhujianwen / 2008-12-25
*********************************************************************/
int tbsAddMac(const char *pIfName, int iInterfaceType, char *szMacVal);


/* DNS解析相关的API及结构体,宏的定义 */
#define IP_LEN  46

/* 存放解析之后IP地址的链表元素 */
typedef struct ST_DNS_INFO
{
	char szIp[IP_LEN];
	struct ST_DNS_INFO* pNext;
}ST_DNS_INFO;

enum
{
	SPLIT_ERROR		= -1,		/* 解析失败 */
	SPLIT_SUCCESS	= 0,		/* 解析成功 */
	SPLIT_IP		= 1,		/* 传入的域名是IP地址 */
	SPLIT_IP6		= 2,		/* 传入的域名是IPv6地址 */
};


/*********************************************************************
*  名称: tbsResolveIpByName

*  功能: 对传入的域名进行解析, 解析结果放在ST_DNS_INFO链表中

*  参数:
		pHostName: 需要解析的域名
		ppDnsInfo: 存放解析结果的链表元素指针, 使用完毕之后使用者要
                   使用tbsFreeDnsInfo函数释放
		ppPort:    存放解析之后的端口号, 由tbsResolveIpByName函数内部分配
                    在使用完毕之后需要调用者自己释放内存
		nMsecond:	超时用的毫秒数
		pServer:    解析DNS的server地址, 可以为空, 为空则使用系统的地址

*  返回: TBS_SUCCESS/TBS_FAILED

*  创建: lichuang (2009-04-01)
*********************************************************************/
int	tbsResolveIpByName(const char *pszUrl, ST_DNS_INFO** ppDnsInfo,
						char* pPort, int nMsecond, const char* pServer);

/*********************************************************************
*  名称: tbsFreeDnsInfo

*  功能: 遍历ST_DNS_INFO链表, 释放里面的所有元素

*  参数:
		pDnsInfo:  需要释放的链表元素指针

*  返回:

*  创建: lichuang (2009-04-01)
*********************************************************************/
void	tbsFreeDnsInfo(ST_DNS_INFO* pDnsInfo);

/*********************************************************************
*  名称: Strtok_r

*  功能: 线程安全的strtok函数的实现, 代码来自net-snmp-5.4.1项目

*  参数:
	pString: 需要分析的字符串, 第一次调用的时候需要传入需要解析的字符串
			 后续传入NULL即可
	pDelim:  分隔字符串
	ppLast:  保存上一次解析结果的指针

*  返回:
			如果是NULL则找不到所分隔的字符串了, 否则是已经分隔好的字符串

*  创建: lichuang (2009-04-01)
*********************************************************************/
char * Strtok_r(char *pString, const char *pDelim, char **pLast);


/* 对传入的域名使用指定的接口和dns进行解析, 并输出解析结果 */
int tbsResolveURL(const char *pszUrl, const char *pszIfName, const char *pszGateway, const char *pszDnsServers,
    char *pcIpAddr, char *pcPort);


/* 解析传入的域名, 分离出真正需要解析的域名以及端口号 */
int tbsSplitUrl(const char *pUrl, char *pHostName, char *pPort, char *pPath);




/* 字符串替换功能函数(重新分配内存) */
char* tbsStringReplaceWithMalloc(const char *pcSrc, const char *pcOldStr, const char *pcNewStr);

/* 字符串替换功能函数(替换原有字符串) */
char* tbsStringReplace(char *pcSrc, const char *pcOldStr, const char *pcNewStr);



/*
将整个文件读取到一个缓冲区, 返回的字符串需要用户自行释放
*/
char *tbsReadFileToBufWithMalloc(const char *pcFileName, int *piLen);

/*
从指定的文件中查找字符串
*/
int tbsFindStrInFile(const char *pszFileName, const char *pszStr);

/*
将字符串写入到指定的字符串中
*/
int tbsWriteStrToFile(const char *pszFileName, const char *pszStr);


/**************************************************************************
功  能: 根据ethernet WAN接口名称获取接口的协商速率(单位Mbps)

参  数: pszDevName -- ethernet WAN接口名称(如eth0)

返回值: ethernet WAN接口的协商速率

备  注: created by XuXiaodong 2009-3-18
***************************************************************************/
int tbsGetEthWanIfLinkSpeed(const char *pszDevName);

/* 根据ppp接口名(pppoe)获取与该ppp接口相关联的以太网接口名 */
int tbsGetPppPhyDev(const char *pszPppDevName, char *pszPhyDevName, int iLen);

/*在升级过程中清除内存*/
void tbsClearRamForUpgrade(int iUpgradeFlag);

/* 让进城变为守护进程 */
int tbsDaemonize(int closefd);

/* 允许/禁用当前进程崩溃时生成core文件 */
int tbsEnableCoreDump(int iFlag);

/* 字符串替换函数,需要调用者释放返回*/
char* tbsStringReplaceWithMalloc(const char *pcSrc, const char *pcOldStr, const char *pcNewStr);


/* 将普通字符串转换成16进制字符串 */
int tbsSting2Hex(char *szSrcString, char *szDecHex, unsigned int i2Num);

/* 通过接口名获取接口对应的MAC地址 */
int tbsGetMacAddr(const char *pszIfName, char *pszMac);


inline long GetSysUpTime(void);

/* 检查协议类型是否合法*/
int tbsCheckProtoType(const char * pszValue);


enum
{
	TR069_UPGRADE,
	WEB_UPGRADE
};

/* 重定义了malloc等内存分配函数,方便检查内存泄漏 */
#ifdef CONFIG_TBS_APPS_DEBUG_MALLOC
#define tbsMalloc(size)            Mem_Malloc(size, __FILE__, __func__, __LINE__)
#define tbsCalloc(nmemb, size)     Mem_Calloc(nmemb, size, __FILE__, __func__, __LINE__)
#define tbsRealloc(p, size)        Mem_Realloc(p, size, __FILE__, __func__, __LINE__)
#define tbsFree(p)                 Mem_Free(p, __FILE__, __func__, __LINE__)
#define tbsStrdup(str)             Mem_Strdup(str, __FILE__, __func__, __LINE__)
#define tbsStrndup(str, size)      Mem_Strndup(str, size, __FILE__, __func__, __LINE__)

#define malloc  tbsMalloc
#define calloc  tbsCalloc
#define realloc tbsRealloc
#define free    tbsFree
#define strdup      tbsStrdup
#define strndup     tbsStrndup
#endif

#ifdef CONFIG_VX180
/* 启用 IKANOS AP 加速功能 */
int tbsApfeatureEnable(unsigned short ucMID);

/* 禁用 IKANOS AP 加速功能 */
int tbsApfeatureDisable(unsigned short ucMID);
#endif

/******************************************************************
功能: 基于64位加密方式的字符串加密函数
参数: *value:加密字符串
        vlen:加密长度
返回: result: 加密后的字符串
备注: 调用此函数请注意在外部free返回指针
******************************************************************/
char *tbsBase64Encode(const unsigned char *value, int vlen);

/******************************************************************
功能: 基于64位加密方式的字符串解密函数
参数: *value:加密字符串
        vlen:加密长度
返回: result: 加密后的字符串
备注: 调用此函数请注意在外部free返回指针
******************************************************************/
unsigned char *tbsBase64Decode(const char *value, int *rlen);


#endif                          /* __TBS_UTIL_H__ */
