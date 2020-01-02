/**********************************************************************
 Copyright (c), 1991-2009, T&W ELECTRONICS(SHENTHEN) Co., Ltd.

 文件名称: tbsipv6.h
 文件描述: ipv6相关信息头文件

 修订记录:
        1. 作者: XuXiaodong
           日期: 2009-12-24
           内容: 创建文件
**********************************************************************/
#ifndef _TBS_IPV6_H_
#define _TBS_IPV6_H_

#include <netinet/in.h>

enum {
    IP6_ADDR_UNKNOWNTYPE,
    IP6_ADDR_LINKLOCAL,
    IP6_ADDR_UNIQUELOCAL,
    IP6_ADDR_GLOBALUNIQUE,
    IP6_ADDR_MULTICAST,
    IP6_ADDR_LOOPBACK,
    IP6_ADDR_UNSPECIFIED,
};

#define   CMP_LOGIC_EQUAL  0
#define  CMP_LOGIC_SMALL  -1
#define  CMP_LOGIC_BIG   1
#define MAX_IP6_ADDR_LEN 48

extern int tbsGetIp6AddrTypeByStr(const char *pszIp6Addr);
extern int tbsGetIp6AddrType(const struct in6_addr *pstIp6Addr);
extern int tbsIfIpv6Switch(const char *pszDevName, int iHow);
extern int tbsEnableIfIpv6(const char *pszDevName);
extern int tbsDisableIfIpv6(const char *pszDevName);
extern int tbsGetEui64IfIdByMac(const unsigned char *pucMacAddr,
                                unsigned char *pucIfId, unsigned int uiLen);
extern int tbsGetEui64IfIdByMacStr(const char *pszMacStr, unsigned char *pucIfId, unsigned int uiLen);
extern int tbsGetRandomIfId(unsigned char *pucIfId, unsigned int uiLen);
extern int tbsGenerateIp6Addr(struct in6_addr *pstIp6Prefix,
                              unsigned char *pucIfId, unsigned int uiLen);
extern int tbsIp6AddrStr2Num(struct in6_addr *pstIp6Addr, const char *pcIp6Addr);
extern int tbsIp6AddrNum2Str(struct in6_addr *pstIp6Addr,
                             char *pcIp6Addr, unsigned int uiLen);
extern int tbsCheckGlobalID(char *pszGlobalID);
extern int tbsCheckIpv6Addr(const char *pszIpv6Addr);
extern int tbsCheckIpv6AddrList(const char *pszIp6AddrList, int iMaxCount,
                                int (*CheckFunc)(const char *));
extern int tbsCheckIpv6PrefixLen(const char *pszIpv6PrefixLen);
extern int tbsCheckDUID(char *pszDUID);
extern int tbsCheckIPv6GlobalAddr(const char *pszValue);
extern int tbsIPv6AddrCmp(const char * pszAddr1,const char * pszAddr2);
extern int tbsGetLLIp6Addr(char *DevMac,char *IP6Addr);

#endif /*_TBS_IPV6_H_*/

