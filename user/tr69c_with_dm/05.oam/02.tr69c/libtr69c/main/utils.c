/***********************************************************************
 *
 *  Copyright (c) 2005-2010  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2011:proprietary:standard

 This program is the proprietary software of Broadcom Corporation and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>
 *
************************************************************************/

/*----------------------------------------------------------------------*
 * File Name  :utils.c 
 *
 * Description: utility routines for tr69 app. 
 *   
 *  
 * $Revision: 1.21 $
 * $Id: utils.c,v 1.21 2006/02/22 17:26:20 dmounday Exp $
 *----------------------------------------------------------------------*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <linux/if.h>
#include <syslog.h>

#include "cms_util.h"
#include "cms_mem.h"
#include "../inc/appdefs.h"
#include "../inc/utils.h"
#include "md5.h"
#include "net/route.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif
/* forwards */
static void generateBasicAuth( SessionAuth *sa, char *user, char* pwd);
static void generateCnonce(char **cnonceBuf);

//static int  verbose;

size_t b64_encode(const char *inp, size_t insize, char **outptr);

#define SIN_ADDR(addr)  (((struct sockaddr_in *) (&(addr)))->sin_addr.s_addr)

extern int www_ParseUrl(const char *url, char *proto, char *host, int *port, char *uri);
#if 0
/** dns lookup that uses getaddrinfo
 */
int dns_lookup(const char *name, struct sockaddr_storage *addr)
{
	struct addrinfo hints, *res, *res0;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(name, NULL, &hints, &res0) != 0)
	{
		cmsLog_notice("dns_lookup(%s) failed", name);
	}
	else
	{
		for (res=res0;res;res=res->ai_next)
		{
			cmsLog_debug("AF<%d> addr<%s>", res->ai_family, writeIp(res->ai_addr));
		}

		/* pick the first available address */
		if (res0->ai_family == AF_INET)
		{
			memcpy(addr, res0->ai_addr, sizeof(struct sockaddr_in));
		}
		else
		{
			memcpy(addr, res0->ai_addr, sizeof(struct sockaddr_in6));
		}

		freeaddrinfo(res0);
	}

	return 1;
}
#endif

/**********************************************************************
 * Files
 **********************************************************************/
/*--------------------
 * returns 1 if files differ 0 otherwise
 */
int hasChanged(const char* new, const char* old)
{
	int ret = 0;
	FILE* fn = fopen(new, "r");
	FILE* fo = fopen(old, "r");
	char bufn[1024];
	char bufo[1024];

	if (!fn) {
		ret = 0;
	} else if (!fo) {
		ret = 1;
	} else {
		for (;;) {
			int nn = fread(bufn, 1, sizeof bufn, fn);
			int no = fread(bufo, 1, sizeof bufo, fo);
			if (nn != no) {
				ret = 1;
				break;
			}

			if (nn == 0) {
				break;
			}

			if (memcmp(bufn, bufo, nn)) {
				ret = 1;
				break;
			}
		}
	}
	if (fn) {
		fclose(fn);
	}
	if (fo) {
		fclose(fo);
	}
	return ret;
}

/*--------------------*/
int mkdirs(const char *path)
{
	struct stat sb;
	char buf[256];
	char* p;

	if (stat(path, &sb) && errno == ENOENT) {
		strcpy(buf, path);
		p = buf + strlen(buf) - 1;
		while (p > buf && *p=='/') p--;
		while (p >= buf && *p!='/')	p--;
		while (p > buf && *p=='/') p--;

		if (p < buf) {
			p = buf;
			*p = '.';
		}
		p[1] = 0;

		if (mkdirs(buf))
			return -1;

		if (mkdir(path, 0777))
			return -1;
	}
	return 0;
}

/*--------------------*/
static void rmrf1(char** p, int* max, int n)
{
	struct stat sb;

	(*p)[n] = 0;
	if (lstat(*p,&sb))
		return;

	if (S_ISDIR(sb.st_mode)) {
		DIR *dp = opendir(*p);
		if (dp) {
			struct dirent *de;
			int k = n;
			if (strcmp(*p,"/")) {
				(*p)[n] = '/';
				k = n + 1;
			}
			while ((de = readdir(dp))) {
				char* f = de->d_name;
				if (strcmp(f, ".") && strcmp(f, "..")) {
					int m = k + strlen(f);
					while (*max <= m) {
						*max *= 2;
						*p = cmsMem_realloc(*p, *max);
						
						if (*p == NULL)
						{
						   cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"realloc to %d bytes failed", *max);
						   closedir(dp);
						   return;
						}
					}
					strcpy(*p + k, f);
					rmrf1(p, max, m);
				}
			}
			closedir(dp);
		}
		(*p)[n] = 0;
		rmdir(*p);
	} else {
		unlink(*p);
	}
}

/*--------------------*/
void rmrf(const char* path)
{
	int n = strlen(path);
	int len = n > 254 ? 2*n : 256;
	char* buf=cmsMem_alloc(len, 0);

	strcpy(buf, path);
	while ( n > 1 && buf[n-1] == '/') {
		n--;
	}
	rmrf1(&buf, &len, n);
	CMSMEM_FREE_BUF_AND_NULL_PTR(buf);
}

/**********************************************************************
 * Time
 **********************************************************************/
/*--------------------*/
static int DSTflag;
static int  timeoffset;

time_t getCurrentTime()
{
	time_t now;
    struct tm tmp;
	struct tm *t=&tmp;
	now = time(NULL);
	localtime_r(&now,t);
	DSTflag = t->tm_isdst;
	timeoffset=timezone;
	return now-timeoffset+(DSTflag==1?3600:0); 
}

/**********************************************************************
 * hex
 **********************************************************************/
static char hex[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

const char *util_StringToHex(const char *s)
{
	static char buf[256];
	char *p;

	if (strlen(s) > 254) return NULL;
	p = buf;
	while (*s) {
		*p++ = hex[((*s & 0xF0) >> 4)];
		*p++ = hex[(*s & 0x0F)];
		s++;
	}
	*p = '\0';
	return buf;
}

/**********************************************************************
 * Addresses
 **********************************************************************/
/*--------------------*/
static int hexChar(int c)
{
	if (c >= 'A') {
		if (c >= 'a') {
			return c <= 'f' ? c - 'a' + 10 : -1;
		} else {
			return c <= 'F' ? c - 'A' + 10 : -1;
		}
	} else {
		return c >= '0' && c <= '9' ? c - '0' : -1;
	}
}

/*--------------------*/
void readMac(u_char* mac, const char* val)
{
	u_char* emac = mac + 6;
	int flag = 0;
	int b = 0;

	while (*val && mac < emac) {
		int c = hexChar(*val++);
		if (c >= 0) {
			b = (b << 4) | c;
			if (flag) {
				*mac++ = b;
			}
			flag = !flag;
		}
	}
}

/*--------------------*/
int readIp(const char* ip)
{
	int n = 0;
	int res = 0;

	while (n < 4 && *ip) {
		if (isdigit(*ip)) {
			res = (res << 8) | atoi(ip);
			n++;
			while (isdigit(*ip)) {
				ip++;
			}
		} else {
			ip++;
		}
	}
	return res;
}

/*--------------------*/
int readProto(const char* val)
{
	if (!strcmp(val,"udp") || !strcmp(val, "UDP")) {
		return IPPROTO_UDP;
	}

	if (!strcmp(val,"icmp") || !strcmp(val, "ICMP")) {
		return IPPROTO_ICMP;
	}

	return IPPROTO_TCP;
}

/*--------------------*/
int readMask(const char *mask)
{
	int ip = readIp(mask);
	int count = 1;
	int bit = 1;

	while ((bit = (ip << 1))) {
		if (bit) count++;
		ip = (ip << 1);
	}
	return count;
}

/*--------------------*/
char* writeMac(const u_char* mac)
{
	static char buf[24];

	sprintf(buf,"%02x:%02x:%02x:%02x:%02x:%02x",
			mac[0], mac[1], mac[2] ,mac[3], mac[4], mac[5]);
	return buf;
}

/*--------------------
 * writes MAC in canonical form (mosquito definition),
 * 12 characters denoting macaddress, no colon or other delimiters, all lower case
 */
char* writeCanonicalMac(const u_char* mac)
{
	static char buf[24];
	// char *p;

	sprintf(buf,"%02x%02x%02x%02x%02x%02x",
			mac[0], mac[1], mac[2] ,mac[3], mac[4], mac[5]);
	// p = buf;
	/* while ((*p = tolower(*p++)));  The %x is lower case abcdef This is causing an error with BRCMSW */

	return buf;
}

/*--------------------*/
char* writeQMac(const u_char* mac)
{
	static char buf[24];

	sprintf(buf,"%02x\\:%02x\\:%02x\\:%02x\\:%02x\\:%02x",
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return buf;
}

/*--------------------*/
void writeIp_b(int ip, char *buf)
{
	sprintf(buf,"%d.%d.%d.%d",
			0xff&(ip>>24), 0xff&(ip>>16), 0xff&(ip>>8), 0xff&ip);
}

/*--------------------*/
char* writeIp(struct sockaddr *ip)
{
	static char buf[CMS_IPADDR_LENGTH];

	if (ip->sa_family == AF_INET6)
	{
		inet_ntop(ip->sa_family, &(((struct sockaddr_in6 *)ip)->sin6_addr), buf, CMS_IPADDR_LENGTH);
	}
	else
	{
		inet_ntop(ip->sa_family, &(((struct sockaddr_in *)ip)->sin_addr), buf, CMS_IPADDR_LENGTH);
	}

	return buf;
}

/*--------------------*/
char* writeNet(int ip, int bits)
{
	static char buf[24];

	ip &= INADDR_BROADCAST << (32 - bits);
	sprintf(buf,"%d.%d.%d.%d",
			0xff&(ip>>24), 0xff&(ip>>16), 0xff&(ip>>8), 0xff&ip);
	return buf;
}

/*--------------------*/
char* writeBcast(int ip, int bits)
{
	static char buf[24];

	ip |= INADDR_BROADCAST >> bits;
	sprintf(buf,"%d.%d.%d.%d",
			0xff&(ip>>24), 0xff&(ip>>16), 0xff&(ip>>8), 0xff&ip);
	return buf;
}

/*--------------------*/
char* writeMask(int bits)
{
	static char buf[24];
	int ip = INADDR_BROADCAST << (32 - bits);

	sprintf(buf,"%d.%d.%d.%d",
			0xff&(ip>>24), 0xff&(ip>>16), 0xff&(ip>>8), 0xff&ip);
	return buf;
}

#if 0 // These two functions are not used in tr69c
/*--------------------*/
char* writeRevNet(int ip, int bits)
{
	static char buf[24];
	char* p = buf;

	if (bits % 8) {
		cmsLog_error("Cannot split FQDN %s/%d", writeIp(ip), bits);
	}

	ip >>= 32 - bits;
	while (bits > 0) {
		if (p > buf) {
			*p++ = '.';
		}
		sprintf(p, "%d", 0xff&ip);
		while (*p) {
			p++;
		}
		ip >>= 8;
		bits -= 8;
	}
	return buf;
}

/*--------------------*/
char* writeRevHost(int ip, int bits)
{
	static char buf[24];
	char* p = buf;

	if (bits % 8) {
		cmsLog_error("Cannot split FQDN %s/%d", writeIp(ip), bits);
	}

	bits = 32 - bits;
	while (bits > 0) {
		if (p > buf) {
			*p++ = '.';
		}
		sprintf(p, "%d", 0xff&ip);
		while (*p) p++;
		ip >>= 8;
		bits -= 8;
	}
	return buf;
}
#endif

/*--------------------*/
char* writeProto(int proto)
{
	switch (proto) {
	case IPPROTO_UDP:
		return "udp";
	case IPPROTO_ICMP:
		return "icmp";
	case IPPROTO_TCP:
	default:
		return "tcp";
	}
}

/**********************************************************************
 * Text formatting
 *********************************************************************/
/*--------------------*/
void readHash(u_char* hash, const char* val)
{
	u_char* ehash = hash + MD5_DIGEST_LEN;
	int flag = 0;
	int b = 0;

	while (*val && hash < ehash) {
		int c = hexChar(*val++);
		if (c >= 0) {
			b = (b << 4) | c;
			if (flag) {
				*hash++ = b;
			}
			flag = !flag;
		}
	}
}

/*--------------------*/
char* writeQHash(const u_char* hash)
{
	int i;
	static char buf[128];
	char* p = buf;

	for (i=0; i < MD5_DIGEST_LEN; i++) {
		if (i > 0) {
			*p++ = '\\';
			*p++ = ':';
		}
		sprintf(p, "%02x", hash[i]);
		p += 2;
	}
	return buf;
}

/*--------------------*/
char* unquoteText(const char* t)
{
	int len = 0;
	char* t1;
	const char* p;
	char* p1;

	for (p = t; *p; p++) {
		if (*p != '\\') {
			len++;
		}
	}
	t1 = cmsMem_alloc(len + 1, 0);
    if(t1==NULL)
    {
        return NULL;
    }

	for (p = t, p1 = t1; *p; p++) {
		if (*p != '\\') {
			*p1++ = *p;
		}
	}
	*p1 = 0;

	return t1;
}

/*--------------------*/
char* quoteText(const char* t)
{
	int len = 0;
	char* t1;
	const char* p;
	char* p1;

	for (p = t; *p; p++) {
		len += *p == ':' ? 2 : 1;
	}
	t1 = cmsMem_alloc(len + 1, 0);
    if(t1==NULL)
    {
        return NULL;
    }

	for (p = t, p1 = t1; *p; p++) {
		if (*p == ':') {
			*p1++ = '\\';
		}
		*p1++ = *p;
	}
	*p1 = 0;

	return t1;
}

/**********************************************************************
 * Processes
 **********************************************************************/
void do_cmd(int logon, const char *cmd, char *fmt, ...)
{
	va_list ap;
	char msg[1024];
	char *p;
	int ret __attribute__ ((unused));

	strcpy(msg, cmd);
	p = msg + strlen(cmd);
	*p++ = ' ';

	va_start(ap, fmt);
	vsnprintf(p, sizeof msg, fmt, ap);
	va_end(ap);
	ret = system(msg);
	if (logon)
	{
		cmsLog_notice("system(\"%s\"), ret=%d", msg, ret);
	}
}

/*--------------------*/
void runScript(const char* name)
{
	char buf[1024];
	int ret __attribute__ ((unused));
	sprintf(buf, "%s %s", SHELL, name);
#ifdef BOX_TBOXxxxx
	printf("TBOX: system(\"%s\"\n", buf);
#else
	ret=system(buf);
#endif
	cmsLog_debug("runScript %s, ret=%d", name, ret);
}

/*--------------------*
 * returns
 *   <pid> for str
 *   -1    if process not found
 */
int findProc(const char* str)
{
	DIR* d;
	struct dirent *de;
	char path[64];
	char cmd[256];
	int n;

	if ((d = opendir("/proc")) != NULL) {
		while ((de = readdir(d))) {
			if (! isdigit(*de->d_name)) {
				continue;
			}
			sprintf(path, "/proc/%s/exe", de->d_name);
			n = readlink(path, cmd, 255);
			if(n < 0) {
				closedir(d);
				return -1;
			}
			cmd[n] = '\0';

//	  cmsLog_debug("findProc() pid=%s cmd=\"%s\" str=\"%s\" match=%d", 
//		  de->d_name, cmd, str, strcmp(cmd,str) == 0);
			if (strcmp(cmd, str) == 0) {
				int pid = atoi(de->d_name);
				closedir(d);
				cmsLog_debug("findProc(%s) pid=%d", str, pid);
				return pid;
			}
		}
		closedir(d);
	}
	cmsLog_debug("findProc(%s) not found", str);
	return -1;
}

/* caseless strcmp */
int stricmp( const char *s1, const char *s2 )
{
	for (;(*s1 && *s2) && (tolower(*s1))==(tolower(*s2)); ++s1, ++s2);
	return(tolower(*s1))-(tolower(*s2));
}

int streq(const char *s0, const char *s1)
{
	if (s0 == NULL && s1 == NULL)
		return 1;
	else if (s0 == NULL || s1 == NULL)
		return 0;
	else
		return strcmp(s0, s1) == 0;
}

const char *itoa(int i)
{
	static char buf[256];
	sprintf(buf, "%d", i);
	return buf;
}


int testBoolean(const char *s)
{
	int r;

	if (strcasecmp(s,"true")==0)
		return 1;

		if (strcasecmp(s,"false")==0)
			return 0;

	/* try to be a little more flexible and allow "0" to mean false */
	if (strcmp(s,"0")==0)
		r = 0;
	else
		r = 1;
	cmsLog_notice("unexpected string ==>%s<==, returning %d", s, r);
	return r;
}

/*
* digest authentication routines
*/


void  resetSessionAuth( SessionAuth *s)
{
	CMSMEM_FREE_BUF_AND_NULL_PTR(s->nc);
	CMSMEM_FREE_BUF_AND_NULL_PTR(s->nonce);
	CMSMEM_FREE_BUF_AND_NULL_PTR(s->orignonce);
	CMSMEM_FREE_BUF_AND_NULL_PTR(s->realm);
	CMSMEM_FREE_BUF_AND_NULL_PTR(s->domain);
	CMSMEM_FREE_BUF_AND_NULL_PTR(s->method);
	CMSMEM_FREE_BUF_AND_NULL_PTR(s->cnonce);
	CMSMEM_FREE_BUF_AND_NULL_PTR(s->opaque);
	CMSMEM_FREE_BUF_AND_NULL_PTR(s->qop);
	CMSMEM_FREE_BUF_AND_NULL_PTR(s->user);
	CMSMEM_FREE_BUF_AND_NULL_PTR(s->uri);
	CMSMEM_FREE_BUF_AND_NULL_PTR(s->algorithm);
	CMSMEM_FREE_BUF_AND_NULL_PTR(s->response);
	CMSMEM_FREE_BUF_AND_NULL_PTR(s->basic);
	memset(s,0,sizeof(struct SessionAuth));
	s->nonceCnt = 0;
}
/*
*  Scan for "argname=arval" 
 * and return strdup pointer to argval;
 * Return NULL is not found or form error;
*/

char *getArg(char *p, char *argname, char **argval)
{
	int		nameLth = strlen(argname);
	int		lth;
	char    *s = p;

	*argval = NULL;
	do {
		if ((s=strcasestr(s, argname)) ) {
			if ( isalpha(*(s-1))) {
				s += nameLth;
				continue;
			}
			s += nameLth;
			while (*s && isblank(*s)) ++s;
			if ( *s == '='){
				++s;
				while (*s && isblank(*s)) ++s;
				if (*s!='\"') {
					/* no quotes around value assume blank delimited or trailing , */
					char *e;
					if ( (e=strchr(s, ',')) || (e=strchr(s,' ')))
						lth = e-s;
					else /* assume hit \0 at end */
						lth = strlen(s);
				} else { /* s at opening quote of string enclosed in quotes */
					char *e;
					++s;
					if ((e=strchr(s,'\"')))
						lth = e-s;
					else
						lth = 0;		/* no closing quote-- ignore */
				}
				if (lth)
					*argval = (char *)cmsMem_strndup(s,lth);
				return *argval;
			}
		}  else	 /* no char sequence found -- return */
            return NULL;

	} while (*s);
	return NULL;
}

void md5ToAscii( unsigned char *s /*16bytes */, unsigned char *d /*33bytes*/)
{
	int i;

	for (i = 0; i < 16; i++)
		snprintf((char *)&d[i*2],3,"%02x", s[i]);
}
/*
* return dynamic memory buffer containing a nonce
*/
static char *makeNonce(void)
{
	struct timeval	tv;
	char	buf[100];
	char	*np;

	gettimeofday( &tv, NULL);
	/* start with something odd but hardly random */
	srand(tv.tv_usec*17);
	snprintf(buf, sizeof(buf), "%8x:%8x:%8x", 
		(unsigned)tv.tv_usec*rand(), (unsigned)tv.tv_usec*rand(), (unsigned)tv.tv_usec*1551*rand());
	b64_encode(buf, 0, &np);
	return np;
}

#define DEFAULT_OPAQUE "5ccc09c403ebaf9f0171e9517f40e41"
char *generateWWWAuthenticateHdr(SessionAuth *sa, char *realm, char *domain, char *method )
{
	char buf[256];

	resetSessionAuth(sa);
	
	sa->nonce = makeNonce(); 
	sa->orignonce = cmsMem_strdup(sa->nonce); /* make copy for later test */
	sa->realm = cmsMem_strdup(realm);
	sa->domain = cmsMem_strdup(domain);
	sa->method = cmsMem_strdup(method);
	sa->qopType = eAuth;
	sa->opaque = cmsMem_strdup(DEFAULT_OPAQUE);
	snprintf(buf, sizeof(buf), 
			 "WWW-Authenticate: Digest realm=\"%s\", domain=\"%s\", nonce=\"%s\", qop=\"auth\","
			 " algorithm=MD5, opaque=\"%s\" ",
			 sa->realm, sa->domain, sa->nonce, sa->opaque);
	return cmsMem_strdup(buf);
}
/*
* Returns value of calculated digest in sa->requestDigest
 * *
*/
static void generateRequestDigest( SessionAuth *sa, char *user, char* pwd)
{
	char md5inbuf[612];
	unsigned char md5buf[16];
	unsigned char HA1[33];
	unsigned char HA2[33];

	snprintf(md5inbuf, sizeof(md5inbuf), "%s:%s:%s", user, sa->realm, pwd);

	tr69_md5it(md5buf, (unsigned char*)md5inbuf);
	md5ToAscii(md5buf,HA1);
	/*if ( sa->algorithm && strcmp(sa->algorithm, "MD5-sess"))
		snprintf(tmpbuf, sizeof(tmpbuf), "%s:%s:%s", HA1, sa->nonce, cnonceBuf); */
	/* don't know how to do auth-int */
	snprintf(md5inbuf, sizeof(md5inbuf),"%s:%s", sa->method, sa->uri);
	tr69_md5it(md5buf,(unsigned char *)md5inbuf);
	md5ToAscii(md5buf,HA2);

	if (sa->qopType == eNoQop )
		snprintf(md5inbuf, sizeof(md5inbuf), "%s:%s:%s", HA1, sa->nonce, HA2);
    else
		snprintf(md5inbuf, sizeof(md5inbuf), "%s:%s:%08x:%s:%s:%s", HA1, sa->nonce,
				 sa->nonceCnt, sa->cnonce, sa->qop, HA2);
	tr69_md5it(md5buf, (unsigned char*)md5inbuf);
	md5ToAscii(md5buf, sa->requestDigest);
}

/*
* parse Authorization: header and test response against  requestDigest
 * returns : 1 passed authorization
 *         : 0 failed authorization
*/
int parseAuthorizationHdr(char *ahdr, SessionAuth *sa, char *username, char *password)
{
	char    *p;

	if ( ahdr && (p=strcasestr(ahdr, "digest"))) {
		CMSMEM_FREE_BUF_AND_NULL_PTR(sa->realm);
		getArg(p,"realm", &sa->realm);
      
   		if (streq(sa->realm, ACSREALM) == 0)
   		{
   			cmsLog_notice("Mismatch authorization header realm value =%s", sa->realm);
   			return 0;
  		}

		CMSMEM_FREE_BUF_AND_NULL_PTR(sa->user);
		getArg(p,"username", &sa->user);
		CMSMEM_FREE_BUF_AND_NULL_PTR(sa->opaque);
		getArg(p,"opaque", &sa->opaque);

 		if (streq(sa->opaque, DEFAULT_OPAQUE) == 0)
 		{
 			cmsLog_notice("Mismatch authorization header opaque value =%s", sa->opaque);
 			return 0;
 		}

		CMSMEM_FREE_BUF_AND_NULL_PTR(sa->nonce);
		getArg(p,"nonce", &sa->nonce);
		CMSMEM_FREE_BUF_AND_NULL_PTR(sa->uri);
		getArg(p,"uri", &sa->uri);
		CMSMEM_FREE_BUF_AND_NULL_PTR(sa->response);
		getArg(p,"response", &sa->response);
		CMSMEM_FREE_BUF_AND_NULL_PTR(sa->cnonce);
		getArg(p,"cnonce", &sa->cnonce);
		CMSMEM_FREE_BUF_AND_NULL_PTR(sa->qop);
		getArg(p,"qop", &sa->qop);

		/* we only support "auth" now.  When we support a list, we need to compare against a list */
		if (streq(sa->qop, "auth") == 0)
		{
			cmsLog_notice("Mismatch authorization header qop value =%s", sa->qop);
			return 0;
		}

		CMSMEM_FREE_BUF_AND_NULL_PTR(sa->nc);
		getArg(p,"nc", &sa->nc);
		if (sa->nc)
			sa->nonceCnt = atoi(sa->nc);
		
		if ( streq(sa->user, username) && streq(sa->nonce, sa->orignonce)) {
			generateRequestDigest(sa, sa->user, password);
			if (!memcmp(sa->response, sa->requestDigest, 32))
				return 1;
		}
	}
	/* else "basic" is not allowed on connectionRequest" */
	return 0;
}

/*
* parse the WWW-Authenticate header and generate response digest in SessionAuth
*/
eAuthentication parseWWWAuthenticate(char *ahdr, SessionAuth *sa)
{
	char    *p;
	eAuthentication auth = eNone;

	resetSessionAuth(sa);
	if ( ahdr ) {
		if ((p=strcasestr(ahdr, "digest"))) {
			getArg(p,"realm", &sa->realm);
			getArg(p,"nonce", &sa->nonce);
			getArg(p,"domain", &sa->domain);
			getArg(p,"opaque", &sa->opaque);
			getArg(p,"cnonce", &sa->cnonce);
			getArg(p,"algorithm", &sa->algorithm);
			getArg(p,"qop", &sa->qop);
			auth = eDigest;
			if(sa->qop)	{
				// sa->qop can be either "auth", "auth-int", "auth,auth-int"                        
				if (strcasestr(sa->qop, "auth"))
					sa->qopType = eAuth;
				else if (strcmp(sa->qop, "auth-int"))
					sa->qopType = eAuthInt;
				else
					sa->qopType = eNoQop;
			} else
				sa->qopType = eNoQop;
		} else if ((p=strcasestr(ahdr, "basic"))) {
			getArg(p,"realm", &sa->realm);
			auth = eBasic;
		} else
			auth = eNone;
	}
	return auth;
}
/* 
* create formated digest string for Authorization header
*/

#define	HDRVALUESZ 512

static char *formatDigestParamStr( SessionAuth *sa, char *user)
{
	char xhdrbuf[256];
	char opaquebuf[256];
	char *hdrvalue;

	if (!(hdrvalue = (char *)cmsMem_alloc(HDRVALUESZ, 0)))
		return NULL;
	if (sa->cnonce)
		snprintf(xhdrbuf, sizeof(xhdrbuf), "cnonce=\"%s\", nc=%08x, ",
				 sa->cnonce, sa->nonceCnt);
	else
		xhdrbuf[0]='\0';
	if (sa->opaque)
		snprintf(opaquebuf, sizeof(opaquebuf), "opaque=\"%s\", ", sa->opaque);
	else
		opaquebuf[0]='\0';
	snprintf(hdrvalue, HDRVALUESZ,
	  "Digest username=\"%s\", realm=\"%s\", algorithm=\"MD5\",%s"
			 " uri=\"%s\", nonce=\"%s\", %s%sresponse=\"%s\"",
			 user, sa->realm, sa->qop? " qop=\"auth\",":"",
			 sa->uri, sa->nonce, xhdrbuf, opaquebuf, sa->requestDigest);
	cmsLog_debug("Authorization header value =%s", hdrvalue);
	return hdrvalue;
}
/* 
* generate the Authorization header value for Digest
*       SessionAuth *sa: Session authorization struct to use.
* 		wwwAuth: pointer to WWWAuthenticate header value
*		method: "GET" or "POST" const string.
*		uri:	pointer to with uri.
*		user:  username for authentication
*       pwd:   password for authentication
* Return is pointer to malloced value: oK,
	        NULL: Parse error on WWWAuthenticate header or malloc failure.
*/

#define CNONCELTH	7
char *generateAuthorizationHdrValue( SessionAuth *sa, char *wwwAuth, char *method,
									  char *uri, char *user, char *pwd)
{
	eAuthentication auth;
	char	*hdrvalue = NULL;

	if ( (auth = parseWWWAuthenticate(wwwAuth, sa))== eDigest){
		sa->method = cmsMem_strdup(method);
		sa->uri = cmsMem_strdup(uri);
		CMSMEM_FREE_BUF_AND_NULL_PTR(sa->cnonce);
		generateCnonce(&sa->cnonce);
		generateRequestDigest(sa, user, pwd);
		hdrvalue = formatDigestParamStr(sa, user);
	} else if (auth == eBasic) {
		if ((hdrvalue = (char *)cmsMem_alloc(HDRVALUESZ, 0))){
			generateBasicAuth(sa, user, pwd);
			snprintf(hdrvalue, HDRVALUESZ, "Basic %s", sa->basic); 
		}
	}
	return hdrvalue;
}

/*
* regenerate the Authorization header digest with the next nonce-count (nc)
*/
char *generateNextAuthorizationHdrValue( SessionAuth *sa, char *user, char *pwd)
{
	char	*hdrvalue = NULL;

	++sa->nonceCnt;
	//CMSMEM_FREE_BUF_AND_NULL_PTR(sa->cnonce);
	//generateCnonce(&sa->cnonce);
	generateRequestDigest(sa, user, pwd);
	hdrvalue = formatDigestParamStr(sa, user);
	return hdrvalue;
}

char *generateBasicAuthorizationHdrValue( SessionAuth *sa, char *user, char *pwd)
{
    char *hdrvalue = NULL;

    if ((hdrvalue = (char *)cmsMem_alloc(HDRVALUESZ, 0))){
	generateBasicAuth(sa, user, pwd);
        snprintf(hdrvalue, HDRVALUESZ, "Basic %s", sa->basic); 
    }
	
    return hdrvalue;
}

/* ---- Base64 Encoding --- */
static const char table64[]=
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
 * Curl_base64_encode()
 *
 * Returns the length of the newly created base64 string. The third argument
 * is a pointer to an allocated area holding the base64 data. If something
 * went wrong, -1 is returned.
 *
 */
size_t b64_encode(const char *inp, size_t insize, char **outptr)
{
	unsigned char ibuf[3];
	unsigned char obuf[4];
	int i;
	int inputparts;
	char *output;
	char *base64data;

	char *indata = (char *)inp;

	*outptr = NULL;	/* set to NULL in case of failure before we reach the end */

	if (0 == insize)
		insize = strlen(indata);

	base64data = output = (char*)cmsMem_alloc(insize*4/3+4, 0);
	if (NULL == output)
		return 0;

	while (insize > 0) {
		for (i = inputparts = 0; i < 3; i++) {
			if (insize > 0) {
				inputparts++;
				ibuf[i] = *indata;
				indata++;
				insize--;
			} else
				ibuf[i]	= 0;
		}

		obuf [0] = (ibuf [0] & 0xFC) >> 2;
		obuf [1] = ((ibuf [0] & 0x03) << 4) | ((ibuf [1] & 0xF0) >> 4);
		obuf [2] = ((ibuf [1] & 0x0F) << 2) | ((ibuf [2] & 0xC0) >> 6);
		obuf [3] = ibuf [2] & 0x3F;

		switch (inputparts) {
		case 1:	/* only one byte read */
			snprintf(output, 5, "%c%c==",
					 table64[obuf[0]],
					 table64[obuf[1]]);
			break;
		case 2:	/* two bytes read */
			snprintf(output, 5, "%c%c%c=",
					 table64[obuf[0]],
					 table64[obuf[1]],
					 table64[obuf[2]]);
			break;
		default:
			snprintf(output, 5, "%c%c%c%c",
					 table64[obuf[0]],
					 table64[obuf[1]],
					 table64[obuf[2]],
					 table64[obuf[3]] );
			break;
		}
		output += 4;
	}
	*output=0;
	*outptr = base64data; /* make it return the actual data memory */

	return strlen(base64data); /* return the length of the new data */
}
/* ---- End of Base64 Encoding ---- */
static void generateCnonce(char **cnonceBuf)
{
	char 	buf[12];
	time_t	now;
	now= time(NULL);
	snprintf(buf, 12, "%011ld", now);
	b64_encode(buf+(12-CNONCELTH), CNONCELTH, cnonceBuf);
}

static void generateBasicAuth(SessionAuth *sa, char *user, char *pwd)
{
	char    raw[256];
	size_t  dataLen;
	char    *b64Buf;
	size_t  b64Len __attribute__ ((unused));

	strcpy(raw, user);
	strcat(raw, ":");
	strcat(raw, pwd);
	dataLen=strlen(raw);
	b64Len = b64_encode(raw, dataLen, &b64Buf);
	sa->basic = b64Buf;
	cmsLog_debug("generateBasicAuth b64Len=%d", b64Len);
}

