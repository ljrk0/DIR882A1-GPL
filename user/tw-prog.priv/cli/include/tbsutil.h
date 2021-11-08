/**********************************************************************
 Copyright (c), 1991-2007, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 �ļ����� : tbsutil.h
 �ļ����� : TBS �������߷�������


 �޶���¼ :
          1 ���� : TBS
            ���� : 2007-11-14
            ���� : ���ں��а�˫������������
          2 �޸� : �Ժ��
            ���� : 2008-9-3
            ���� : ԭ˫���������ϵͳͷ�ļ���ͻ���޸Ķ�������ͻ

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



/* �ն���ʹ�õ���ɫ�������� */
#define ECHO_NONE           "\033[0m"

#define ECHO_RED            "\033[0;31m"
#define ECHO_GREEN          "\033[0;32m"
#define ECHO_YELLOW         "\033[0;33m"
#define ECHO_BLUE           "\033[0;34m"
#define ECHO_PURPLE         "\033[0;35m"

/* ��˸ */
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
#define MAX_USER_LEN    255      /* �û���/���볤�� */



#define PATH_MAC_ADDRESS  "/proc/llconfig/macaddr"
#define MAX_VALUE_OF_HEX   0xFFFFFF
#define CYCLE_VALUE_OF_HEX  0x1000000
#define URL_STR_URL_MAX_LEN       256       /* URL��������ַ���*/
//extern char *strndup(const char *s, size_t n);
enum
{
	MAC_LAN_DEVICE = 0,
	MAC_WAN_DEVICE,
	MAC_WLAN_DEVICE,
	MAC_USB_DEVICE,
};


/******************************************************************************
 *                                 λ����                                   *
 ******************************************************************************/
#define SET_BIT(flag, bit)		(flag)|=(bit)
#define CLEAR_BIT(flag,bit)		(flag)&=~(bit)
#define TEST_BIT(flag,bit)		(flag)&(bit)

/******************************************************************************
 *                                 ָ�����                                   *
 ******************************************************************************/
#define	SAFE_FREE(ptr)	free(ptr);ptr=NULL

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define ALIGN(x,a) (((x)+(a)-1)&~((a)-1))
#define FIELD_SIZEOF(t, f) (sizeof(((t*)0)->f))

/******************************************************************************
 *                                 IP����                                   *
 ******************************************************************************/
#define NET_ADDR(ip, mask)              ((ip) & (mask))
#define HOST_ADDR(ip, mask)             ((ip) & ~(mask))

#define IS_NET_ADDR(ip, mask)           (HOST_ADDR(ip, mask) == 0)
#define IS_BROAD_ADDR(ip, mask)         (HOST_ADDR(ip, mask) == ~(mask))
#define IN_SAME_SUBNET(ip1, ip2, mask)  (NET_ADDR(ip1, mask) == NET_ADDR(ip2, mask))


#ifndef _SYS_QUEUE_H
/*
   TBSԭ�����˫��������<sys/queue.h> �е������г�ͻ
   �����޸�TBS��˫��������ز������壬��ǰ������TBSǰ׺
   Ϊ�˲�Ӱ������ģ�飬��û������<sys/queue.h>��ʱ�����������ת��
   �Ժ�����ģ����Ҫʹ�ô�TBSǰ׺�Ĳ�������
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
 *                                 ˫������                                   *
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

typedef struct TBS_list_head TBS_ST_LIST_HEAD;  /* �����ǵ������淶������� */

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
 *                                 �ַ�������                                 *
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
 *                                ͨ�ù��߷���                                *
 ******************************************************************************/

/*�ַ���ƥ�亯��*/
BOOL tbsMatch(const char *pszString, const char *pszPattern);

/* tbsSystem�����ӡ����,CONFIG_TBS_APPS_DEBUG_SYSCMD��make menuconfig���� */
#ifdef CONFIG_TBS_APPS_DEBUG_SYSCMD
#define TBS_PRINT_CMD       1
#else
#define TBS_PRINT_CMD		0
#endif


#ifdef CONFIG_APPS_CMDDAEMON
/* ��ʼ������ģʽ */
void tbsConfigModeInit(void);

/* ����system�첽ģʽ */
void tbsSetSystemAsyn(unsigned short usMID);

/* ���system�첽ģʽ */
void tbsSystemAsynComplete(void);
#else
#define tbsConfigModeInit()
#define tbsSetSystemAsyn(usMID)
#define tbsSystemAsynComplete()
#endif

/* ϵͳ����system�ķ�װ */
int tbsSystem(char *command, int printFlag);

#define tbsSystemMute(cmd)  tbsSystem(cmd, 0)

/*���ַ���IPת�����޷��ų�����IP*/
unsigned long tbsCnvtIpFromStr(const char *pcIp);

/*
 * tbsCheckIpRange: ������������IP��ַ�Ƿ���һ����Χ֮��
 * pStartIp: 		��ʼIP��ַ
 * pEndIp: 			����IP��ַ
 * ����ֵ:			���������ͬһ������֮��,����TBS_SUCCESS, ���򷵻�TBS_ERROR
 * ��ע:			�ú�������IP��ַ����Ч�������, �ڵ���֮ǰ���ʹ��
 					tbsCheckIpEx�������м��
*/
int tbsCheckIpRange(const char *pStartIp, const char *pEndIp);
/*������IPת��Ϊ������ַ���IP*/
void tbsCnvtIpToStr(unsigned long ulIp, char *pszIp);

/*���ַ���"1"����"true"�������ִ�Сд����ת����BOOL���ͣ�*/
BOOL tbsStrToBool(const char *pcValue);

/* ����������1�ĸ���,�Ƿ������򷵻ظ��� */
int tbsGetBitsByMask(const char *pcMask);


/* ����ָ���ַ��ָ��ַ��� */
int tbsSplitString(char *str, char *substr[], const char delimit, const int max_count);

/*
�����ƶ��ַ��ָ��б��ַ���
(����: xx,yy,zz, �ָ�֮����������{xx, yy, zz})
*/
int tbsStrListSplit(char *strList, char delimit, char *apcStrArray[], int nMaxCnt);

/*
��һ���б��ַ����в�������һ��Ԫ��
(����: xx,yy,zz, ����yy)
*/
int tbsStrListFind(const char *szList, char delimit, const char *szEntry);

/*
��һ���б��ַ��������һ��Ԫ��
(����: xx,yy, ���zz, ���� xx,yy,zz)
*/
int tbsStrListAdd(char *szList, char delimit, const char *szEntry);

/*
��һ���б��ַ�����ɾ������һ��Ԫ��
(����: xx,yy,zz, ɾ��yy, ���� xx,zz)
*/
int tbsStrListDel(char *szList, char delimit, const char *szDel);

void TW_printf(const void *buf, int len);

/*=========================================================================*/
/*                                                                                                                                                                    */
/*                                     �ϼ�麯����Ϊ�˰汾���ݱ���                                                  */
/*                                                                                                                                                                    */
/*=========================================================================*/

/* ���IP�Ϸ��� */
BOOL tbsCheckIp(const char *pcip);

BOOL tbsCheckPort(const char *pszString);

BOOL tbsCheckPriority(const char *pszString);


/*�����������Ϸ���*/
BOOL tbsCheckMask(const char *pcValue);



/* ���BOOL ֵ�ĺϷ���*/
BOOL tbsCheckEnable(const char *pszValue);

/* ���int ֵ�ĺϷ���*/
BOOL tbsCheckInt(const char *pszValue);

/* ���unsigned int ֵ�ĺϷ���*/
BOOL tbsCheckUInt(const char *pszValue);

/*���MAC��ַ�Ϸ���*/
BOOL tbsCheckMac(const char *pcMac);



/* ���IP�б� */
BOOL tbsCheckIpList(const char *pszIpList, int iMaxCount);

/* ���MAC �б�*/
BOOL tbsCheckMacList(const char *pszMacList, int iMaxCount);

/*=========================================================================*/
/*                                                                                                                                                                    */
/*                                     �¼�麯��������ֵ�ı�Ϊ��������ʽ                                 */
/*                                                                                                                                                                    */
/*=========================================================================*/

/*
ʹ�ô����������ʽƥ��Ҫ����ֵ,��ƥ���򷵻�ָ���Ĵ�����
*/
int tbsCheckRegex(const char *pszValue, const char *pszPattern, int iErrNo);


/* ���IP�Ϸ��� */
int tbsCheckIpEx(const char *pszValue);
int tbsCheckHostIpEx(const char *pszValue);

/* ���ip6����ĺϷ���*/
int tbsCheckIpv6Net(const char *pszIpv6Net);

/*���DNS IP�Ϸ���,����0.0.0.0��IP����*/
int tbsCheckDNSHostIpEx(const char *pszValue);

int tbsCheckPortEx(const char *pszString);

#if (defined(CONFIG_DSL_3785_TALKTALK_BT) || (defined CONFIG_CUSTOMER_dlink_EU))
int tbsCheckPortGroupEx(const char *pszString);
#endif

int tbsCheckPriorityEx(const char *pszString);

/*�����������Ϸ���*/
int tbsCheckMaskEx(const char *pcValue);

/* ���BOOL ֵ�ĺϷ���*/
int tbsCheckEnableEx(const char *pszValue);

/* ���int ֵ�ĺϷ���*/
int tbsCheckIntEx(const char *pszValue);

/* ���unsigned int ֵ�ĺϷ���*/
int tbsCheckUIntEx(const char *pszValue);

/* ���intֵ�Ƿ���ָ����Χ */
int tbsCheckIntRangeEx(const char *pszValue, int nMin, int nMax);

/* ���uintֵ�Ƿ���ָ����Χ */
int tbsCheckUIntRangeEx(const char *pszValue, unsigned int nMin, unsigned int nMax);

/*���MAC��ַ�Ϸ���*/
int tbsCheckMacEx(const char *pcMac);

/* ���IP�б� */
int tbsCheckIpListEx(const char *pszIpList, int iMaxCount);
int tbsCheckHostIpListEx(const char *pszIpList, int iMaxCount);

/*���DNS IP�б�*/
int tbsCheckDNSHostIpListEx(const char *pszIpList, int iMaxCount);


/* ���MAC �б�*/
int tbsCheckMacListEx(const char *pszMacList, int iMaxCount);

/* ����û���/���� */
int tbsCheckUserEx(const char *pszValue);
int tbsCheckUserExWithLength(const char *pszValue, const int iLen);
int tbsCheckPasswdEx(const char *pszValue);
int tbsCheckPasswdExWithLength(const char *pszValue, const int iLen);

/* ���������ʽ�Ƿ�Ϸ� */
int tbsCheckDomainName(const char *pszValue);

/* �����������IP�ĺϷ��� */
int tbsCheckHost(const char *pszValue);

/* ���url�Ϸ���*/
int tbsCheckUrl(const char * pValue);

/*=========================================================================
*
*    Mem Pool function
*
*=========================================================================*/

/*********************************************************************
*  ����: �����ڴ�,����¼
*********************************************************************/
void *Mem_Malloc(size_t nSize, const char *szFile, const char *szFunc, unsigned long ulLine);

/*********************************************************************
*  ����: Ϊ��������ڴ�(���������ڴ���0),����¼
*********************************************************************/
void *Mem_Calloc(size_t nMemb, size_t nSize, const char *szFile, const char *szFunc, unsigned long ulLine);

/*********************************************************************
*  ����: ���·����ڴ�( �ͷ�/��չԭ��������ڴ�),����¼
*********************************************************************/
void *Mem_Realloc(void *pAddr, size_t nSize, const char *szFile, const char *szFunc, unsigned long ulLine);

/*********************************************************************
*  ����: �ͷ��ڴ�,��ɾ����¼
*********************************************************************/
void Mem_Free(void *pAddr, const char *szFile, const char *szFunc, unsigned long ulLine);

/*********************************************************************
*  ����: �ͷ������ڴ�, ��ɾ�����м�¼
*********************************************************************/
void Mem_FreeAll(void);

/*********************************************************************
*  ����: ��ʾ����֮����δ�ͷŵ��ڴ����
*********************************************************************/
void Mem_PrintStatus(void);

/*********************************************************************
*  ����: ��ʾ����֮����δ�ͷŵ��ڴ����(�Ѿ����ô˺�����ʾ�����ڴ�鲻����ʾ)
*********************************************************************/
void Mem_PrintStatusOnce(void);

char *Mem_Strdup(const char *pcStr, const char *szFile, const char *szFunc, unsigned long ulLine);
char *Mem_Strndup(const char *pcStr, size_t nSize, const char *szFile, const char *szFunc, unsigned long ulLine);


/*********************************************************************
*  ����: tbsAddMac

*  ����: ����mac��ַ��֧��LAN��WAN�豸���޸�

*  ����: pIfNameΪ�豸��,��eth0
		 iInterfaceTypeΪMAC_LAN_DEVICE����MAC_WAN_DEVICE
		 szMacVal  ���ظ������ߵ�mac��ַ, �ڴ��ɵ���������

*  ����: TBS_SUCCESS/TBS_FAILED

*  ����: zhujianwen / 2008-12-25
*********************************************************************/
int tbsAddMac(const char *pIfName, int iInterfaceType, char *szMacVal);


/* DNS������ص�API���ṹ��,��Ķ��� */
#define IP_LEN  46

/* ��Ž���֮��IP��ַ������Ԫ�� */
typedef struct ST_DNS_INFO
{
	char szIp[IP_LEN];
	struct ST_DNS_INFO* pNext;
}ST_DNS_INFO;

enum
{
	SPLIT_ERROR		= -1,		/* ����ʧ�� */
	SPLIT_SUCCESS	= 0,		/* �����ɹ� */
	SPLIT_IP		= 1,		/* �����������IP��ַ */
	SPLIT_IP6		= 2,		/* �����������IPv6��ַ */
};


/*********************************************************************
*  ����: tbsResolveIpByName

*  ����: �Դ�����������н���, �����������ST_DNS_INFO������

*  ����:
		pHostName: ��Ҫ����������
		ppDnsInfo: ��Ž������������Ԫ��ָ��, ʹ�����֮��ʹ����Ҫ
                   ʹ��tbsFreeDnsInfo�����ͷ�
		ppPort:    ��Ž���֮��Ķ˿ں�, ��tbsResolveIpByName�����ڲ�����
                    ��ʹ�����֮����Ҫ�������Լ��ͷ��ڴ�
		nMsecond:	��ʱ�õĺ�����
		pServer:    ����DNS��server��ַ, ����Ϊ��, Ϊ����ʹ��ϵͳ�ĵ�ַ

*  ����: TBS_SUCCESS/TBS_FAILED

*  ����: lichuang (2009-04-01)
*********************************************************************/
int	tbsResolveIpByName(const char *pszUrl, ST_DNS_INFO** ppDnsInfo,
						char* pPort, int nMsecond, const char* pServer);

/*********************************************************************
*  ����: tbsFreeDnsInfo

*  ����: ����ST_DNS_INFO����, �ͷ����������Ԫ��

*  ����:
		pDnsInfo:  ��Ҫ�ͷŵ�����Ԫ��ָ��

*  ����:

*  ����: lichuang (2009-04-01)
*********************************************************************/
void	tbsFreeDnsInfo(ST_DNS_INFO* pDnsInfo);

/*********************************************************************
*  ����: Strtok_r

*  ����: �̰߳�ȫ��strtok������ʵ��, ��������net-snmp-5.4.1��Ŀ

*  ����:
	pString: ��Ҫ�������ַ���, ��һ�ε��õ�ʱ����Ҫ������Ҫ�������ַ���
			 ��������NULL����
	pDelim:  �ָ��ַ���
	ppLast:  ������һ�ν��������ָ��

*  ����:
			�����NULL���Ҳ������ָ����ַ�����, �������Ѿ��ָ��õ��ַ���

*  ����: lichuang (2009-04-01)
*********************************************************************/
char * Strtok_r(char *pString, const char *pDelim, char **pLast);


/* �Դ��������ʹ��ָ���Ľӿں�dns���н���, ������������ */
int tbsResolveURL(const char *pszUrl, const char *pszIfName, const char *pszGateway, const char *pszDnsServers,
    char *pcIpAddr, char *pcPort);


/* �������������, �����������Ҫ�����������Լ��˿ں� */
int tbsSplitUrl(const char *pUrl, char *pHostName, char *pPort, char *pPath);




/* �ַ����滻���ܺ���(���·����ڴ�) */
char* tbsStringReplaceWithMalloc(const char *pcSrc, const char *pcOldStr, const char *pcNewStr);

/* �ַ����滻���ܺ���(�滻ԭ���ַ���) */
char* tbsStringReplace(char *pcSrc, const char *pcOldStr, const char *pcNewStr);



/*
�������ļ���ȡ��һ��������, ���ص��ַ�����Ҫ�û������ͷ�
*/
char *tbsReadFileToBufWithMalloc(const char *pcFileName, int *piLen);

/*
��ָ�����ļ��в����ַ���
*/
int tbsFindStrInFile(const char *pszFileName, const char *pszStr);

/*
���ַ���д�뵽ָ�����ַ�����
*/
int tbsWriteStrToFile(const char *pszFileName, const char *pszStr);


/**************************************************************************
��  ��: ����ethernet WAN�ӿ����ƻ�ȡ�ӿڵ�Э������(��λMbps)

��  ��: pszDevName -- ethernet WAN�ӿ�����(��eth0)

����ֵ: ethernet WAN�ӿڵ�Э������

��  ע: created by XuXiaodong 2009-3-18
***************************************************************************/
int tbsGetEthWanIfLinkSpeed(const char *pszDevName);

/* ����ppp�ӿ���(pppoe)��ȡ���ppp�ӿ����������̫���ӿ��� */
int tbsGetPppPhyDev(const char *pszPppDevName, char *pszPhyDevName, int iLen);

/*����������������ڴ�*/
void tbsClearRamForUpgrade(int iUpgradeFlag);

/* �ý��Ǳ�Ϊ�ػ����� */
int tbsDaemonize(int closefd);

/* ����/���õ�ǰ���̱���ʱ����core�ļ� */
int tbsEnableCoreDump(int iFlag);

/* �ַ����滻����,��Ҫ�������ͷŷ���*/
char* tbsStringReplaceWithMalloc(const char *pcSrc, const char *pcOldStr, const char *pcNewStr);


/* ����ͨ�ַ���ת����16�����ַ��� */
int tbsSting2Hex(char *szSrcString, char *szDecHex, unsigned int i2Num);

/* ͨ���ӿ�����ȡ�ӿڶ�Ӧ��MAC��ַ */
int tbsGetMacAddr(const char *pszIfName, char *pszMac);


inline long GetSysUpTime(void);

/* ���Э�������Ƿ�Ϸ�*/
int tbsCheckProtoType(const char * pszValue);


enum
{
	TR069_UPGRADE,
	WEB_UPGRADE
};

/* �ض�����malloc���ڴ���亯��,�������ڴ�й© */
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
/* ���� IKANOS AP ���ٹ��� */
int tbsApfeatureEnable(unsigned short ucMID);

/* ���� IKANOS AP ���ٹ��� */
int tbsApfeatureDisable(unsigned short ucMID);
#endif

/******************************************************************
����: ����64λ���ܷ�ʽ���ַ������ܺ���
����: *value:�����ַ���
        vlen:���ܳ���
����: result: ���ܺ���ַ���
��ע: ���ô˺�����ע�����ⲿfree����ָ��
******************************************************************/
char *tbsBase64Encode(const unsigned char *value, int vlen);

/******************************************************************
����: ����64λ���ܷ�ʽ���ַ������ܺ���
����: *value:�����ַ���
        vlen:���ܳ���
����: result: ���ܺ���ַ���
��ע: ���ô˺�����ע�����ⲿfree����ָ��
******************************************************************/
unsigned char *tbsBase64Decode(const char *value, int *rlen);


#endif                          /* __TBS_UTIL_H__ */
