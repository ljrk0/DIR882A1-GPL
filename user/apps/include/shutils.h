/*
 * Shell-like utility functions
 *
 * Copyright (C) 2016, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *
 * <<Broadcom-WL-IPTag/Open:>>
 *
 * $Id: shutils.h 554956 2015-05-07 05:36:55Z $
 */

#ifndef _shutils_h_
#define _shutils_h_

#define ACTION_LOCK		"/var/lock/action"

#define DEFAULT_SYSLOG_FILE_PATH  "/var/log/messages"

enum {
	ACT_IDLE,   //0
	ACT_TFTP_UPGRADE_UNUSED,
	ACT_WEB_UPGRADE,
	ACT_WEBS_UPGRADE_UNUSED,
	ACT_SW_RESTORE,
	ACT_HW_RESTORE,     //5
	ACT_ERASE_NVRAM,
	ACT_NVRAM_COMMIT,
	ACT_REBOOT,
	ACT_PROTEST,
	ACT_RC_SIGUSR2_RUNING,  //10
	ACT_RC_SIGUSR1_RUNING,
	ACT_RC_TERM_RUNING,
	ACT_UNKNOWN
};

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

//private config
#define PRIVATE_RANDROOTCA_PATH "/var/private/.randRootCA"
#define PRIVATE_SERVER_KEY_PRIVATE_PATH "/private/server.key.private"
#define PRIVATE_SERVER_KEY_PATH "/var/private/server.key"
#define PRIVATE_SERVER_REQ_PATH "/private/server.req"
#define LIGHTTPD_PEM_PATH "/var/private/lighttpd.pem"
#define PRIVATE_PASSWORD "QaZWsX741852"
//factory config
#define FACTORY_MODE "factory_mode"
#define FACTORY_MAC  "factory_mac"
#define FACTORY_MODE_FLAG "/private/factory_mode"
enum{
	FACTORY_CONFIG,
	DEFAULT_CONFIG,
	USER_CONFIG
};

#if defined(PRODUCT_DIR853_A1)||defined(PRODUCT_DIR853_A2) ||defined(PRODUCT_DIR1360)
#define INTERFACE_2G			"rax0"
#define INTERFACE_5G			"ra0"
#define APCLI_IF_2G 			"apclix0"
#define APCLI_IF_5G 			"apcli0"
#define INTERFACE_2G_GUEST1		"rax1"
#define	INTERFACE_5G_GUEST1		"ra1"
#else
#define INTERFACE_2G			"ra0"
#define INTERFACE_5G			"rai0"
#define APCLI_IF_2G 			"apcli0"
#define APCLI_IF_5G 			"apclii0"
#define INTERFACE_2G_GUEST1		"ra1"
#define	INTERFACE_5G_GUEST1		"rai1"

#endif
#if defined(__THIRD_WIFI_CARD_SUPPORT__)
#define MACNUM_WAN_LAN_INTERVAL 	4
#else
#define MACNUM_WAN_LAN_INTERVAL 	3
#endif

#define INTERNET_STATUS "/tmp/internet_status"
#define INTERNET_STATUS_RETRY_COUNT 8
#define INTERNET_STATUS_OLD "/tmp/internet_status_lastTime"

#define INTERNET_IPV4_TYPE 0
#define INTERNET_IPV6_TYPE 1

#define INTERNET_IPV6_FILE "/tmp/Ipv6InternetStatus"
#define INTERNET_IPV4_FILE "/tmp/InternetStatus"


//#ifndef BOOL
//#define BOOL unsigned int
//#endif
/*
 * Reads file and returns contents
 * @param	fd	file descriptor
 * @return	contents of file or NULL if an error occurred
 */
extern char * fd2str(int fd);

/*
 * Reads file and returns contents
 * @param	path	path to file
 * @return	contents of file or NULL if an error occurred
 */
extern char * file2str(const char *path);

/*
 * Waits for a file descriptor to become available for reading or unblocked signal
 * @param	fd	file descriptor
 * @param	timeout	seconds to wait before timing out or 0 for no timeout
 * @return	1 if descriptor changed status or 0 if timed out or -1 on error
 */
extern int waitfor(int fd, int timeout);
extern long getSysUpTime(void);

#if defined(linux) || defined(__NetBSD__)
/*
 * Concatenates NULL-terminated list of arguments into a single
 * commmand and executes it
 * @param	argv	argument list
 * @param	path	NULL, ">output", or ">>output"
 * @param	timeout	seconds to wait before timing out or 0 for no timeout
 * @param	ppid	NULL to wait for child termination or pointer to pid
 * @return	return value of executed command or errno
 */
extern int _eval(char *const argv[], char *path, int timeout, pid_t *ppid);
#endif /* defined(linux) */

/*
 * Concatenates NULL-terminated list of arguments into a single
 * commmand and executes it
 * @param	argv	argument list
 * @return	stdout of executed command or NULL if an error occurred
 */
extern char * _backtick(char *const argv[]);

/*
 * Kills process whose PID is stored in plaintext in pidfile
 * @param	pidfile	PID file
 * @return	0 on success and errno on failure
 */
extern int kill_pidfile(char *pidfile);
extern int kill_pidfile_s(char *pidfile, int sig);

#if defined(linux)
/*
 * Returns the process ID.
 *
 * @param	name	pathname used to start the process.  Do not include the
 *                      arguments.
 * @return	pid
 */
extern pid_t get_pid_by_name(char *name);

/*
 * fread() with automatic retry on syscall interrupt
 * @param	ptr	location to store to
 * @param	size	size of each element of data
 * @param	nmemb	number of elements
 * @param	stream	file stream
 * @return	number of items successfully read
 */
extern int safe_fread(void *ptr, size_t size, size_t nmemb, FILE *stream);

/*
 * fwrite() with automatic retry on syscall interrupt
 * @param	ptr	location to read from
 * @param	size	size of each element of data
 * @param	nmemb	number of elements
 * @param	stream	file stream
 * @return	number of items successfully written
 */
extern int safe_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
#endif /* defined(linux) */

/*
 * Convert Ethernet address string representation to binary data
 * @param	a	string in xx:xx:xx:xx:xx:xx notation
 * @param	e	binary data
 * @return	TRUE if conversion was successful and FALSE otherwise
 */
extern int ether_atoe(const char *a, unsigned char *e);

/*
 * Convert Ethernet address binary data to string representation
 * @param	e	binary data
 * @param	a	string in xx:xx:xx:xx:xx:xx notation
 * @return	a
 */
extern char * ether_etoa(const unsigned char *e, char *a);

/*
 * Concatenate two strings together into a caller supplied buffer
 * @param	s1	first string
 * @param	s2	second string
 * @param	buf	buffer large enough to hold both strings
 * @return	buf
 */
static inline char * strcat_r(const char *s1, const char *s2, char *buf)
{
	strcpy(buf, s1);
	strcat(buf, s2);
	return buf;
}

/*
 * Parse the unit and subunit from an interface string such as wlXX or wlXX.YY
 *
 * @param	ifname	interface string to parse
 * @param	unit	pointer to return the unit number, may pass NULL
 * @param	subunit	pointer to return the subunit number, may pass NULL
 * @return	Returns 0 if the string ends with digits or digits.digits, -1 otherwise.
 *		If ifname ends in digits.digits, then unit and subuint are set
 *		to the first and second values respectively. If ifname ends
 *		in just digits, unit is set to the value, and subunit is set
 *		to -1. On error both unit and subunit are -1. NULL may be passed
 *		for unit and/or subuint to ignore the value.
 */
extern int get_ifname_unit(const char* ifname, int *unit, int *subunit);

/*
 * Get interfaces belonging to a specific bridge.
 *
 * @param	bridge_name 	pointer to bridge interface name
 * @return	list on interfaces beloging to the bridge
 */
extern char *
get_bridged_interfaces(char *bridge_name);

/*
		remove_from_list
		Remove the specified word from the list.

		@param name word to be removed from the list
		@param list List to modify
		@param listsize Max size the list can occupy

		@return	error code
*/
extern int remove_from_list(const char *name, char *list, int listsize);

/*
		add_to_list
		Add the specified interface(string) to the list as long as
		it will fit in the space left in the list.

		@param name Name of interface to be added to the list
		@param list List to modify
		@param listsize Max size the list can occupy

		@return	error code
*/
extern int add_to_list(const char *name, char *list, int listsize);

extern char *find_in_list(const char *haystack, const char *needle);

extern int nvifname_to_osifname(const char *nvifname, char *osifname_buf,
                                int osifname_buf_len);
extern int osifname_to_nvifname(const char *osifname, char *nvifname_buf,
                                int nvifname_buf_len);

extern char *remove_dups(char *inlist, int inlist_size);

int ure_any_enabled(void);

#if !(defined(__USE_ISOC99)) && !(_XOPEN_SOURCE >= 600)
/* Check for a blank character; that is, a space or a tab */
#define isblank(c) ((c) == ' ' || (c) == '\t')
#endif

/* Strip trailing CR/NL from string <s> */
#define chomp(s) ({ \
	char *c = (s) + strlen((s)) - 1; \
	while ((c > (s)) && (*c == '\n' || *c == '\r')) \
		*c-- = '\0'; \
	s; \
})

/* Simple version of _backtick() */
#define backtick(cmd, args...) ({ \
	char *argv[] = { cmd , ## args, NULL }; \
	_backtick(argv); \
})

/* Simple version of _eval() (no timeout and wait for child termination) */
#define eval(cmd, args...) ({ \
	char *argv[] = { cmd , ## args, NULL }; \
	_eval(argv, ">/dev/console", 0, NULL); \
})

/* Copy each token in wordlist delimited by space into word */
#define foreach(word, wordlist, next) \
	for (next = &wordlist[strspn(wordlist, " ")], \
	     strncpy(word, next, sizeof(word)), \
	     word[strcspn(word, " ")] = '\0', \
	     word[sizeof(word) - 1] = '\0', \
	     next = strchr(next, ' '); \
	     strlen(word); \
	     next = next ? &next[strspn(next, " ")] : "", \
	     strncpy(word, next, sizeof(word)), \
	     word[strcspn(word, " ")] = '\0', \
	     word[sizeof(word) - 1] = '\0', \
	     next = strchr(next, ' '))

/* Return NUL instead of NULL if undefined */
#define safe_getenv(s) (getenv(s) ? : "")

#if defined(linux) || defined(__NetBSD__)
/* Print directly to the console */

#define cprintf(fmt, args...) do { \
	FILE *fp = fopen("/dev/console", "w"); \
	if (fp) { \
		fprintf(fp, fmt , ## args); \
		fclose(fp); \
	} \
} while (0)
#endif

#define DEBUG
/* Debug print */
#ifdef DEBUG
#define dprintf(fmt, args...) cprintf("%s: " fmt, __FUNCTION__ , ## args)
#else
#define dprintf(fmt, args...)
#endif /* DEBUG */

/* Buffer structure for collecting string-formatted data
 * using str_bprintf() API.
 * Use str_binit() to initialize before use
 */
struct strbuf {
	char *buf;		/* pointer to current position in origbuf */
	unsigned int size;	/* current (residual) size in bytes */
	char *origbuf;		/* unmodified pointer to orignal buffer */
	unsigned int origsize;	/* unmodified orignal buffer size in bytes */
};

extern void str_binit(struct strbuf *b, char *buf, uint size);
extern int str_bprintf(struct strbuf *b, const char *fmt, ...);
extern int get_meminfo_item(const char *name);
extern void set_action(int a);
extern int check_action(void);
extern int wait_action_idle(int n);
extern char *get_process_name_by_pid(const int pid);
// ���������ӿ��Ѿ�����
//extern int ppp_ifunit(char *ifname);
//extern int wan_ifunit(char *wan_ifname);
extern int TW_StrReplace(char strRes[],char from[], char to[]);
extern int TW_GetFactoryMode();
extern int TW_LoadFactoryConfig();
extern int TW_LoadDefaultConfig();
//extern int tw_wan_prefix(char *ifname, char *prefix);
extern char *racat(char *s, int i);
extern void set_nth_value_flash(int nvram, int index, char *flash_key, char *value);
extern int detect_process(char * process_name); 
#if defined(__SUPPORT_WIFI_CERT__)
extern int detect_process_num(char * process_name,int num); 
#endif
extern int getIfMac(char *ifname, char *if_hw);
extern int setIfMac(char *ifname, char *if_hw);
extern int getFlashContentByOffset(unsigned int offset, unsigned char *pValue);
extern int tbsMatch(const char *pszString, const char *pszPattern);
extern int isdigitstr(char *str);
extern int tbsCheckIp(const char *pcip);
extern int tbsCheckPort(const char *pszString);
extern unsigned long tbsCnvtIpFromStr(const char *pcIp);
extern int tbsCheckHostIp(const char *pszValue);
extern int tbsCheckHostIpEx(const char *pszValue);
extern int tbsCheckMaskEx(const char *pcValue);
extern int TWCheckMacAddr(const char *pszMac);
extern int TWTranslatLowerToUpper(char *oriString, char *destString);
extern int tbsGetFileSize(const char* pFile);
extern int tbsFileLock(int iFD);
extern int tbsFileUnLock(int iFD);
extern int tbsFileSafeWrite(int iFD, const void *pszBuf, size_t ulCount);
extern int tbsFileFullWrite(int iFD, const void *pszBuf, size_t ulLen);
extern int  send_signal_to_pid(char *name,int type);
extern int pingAccessible(const char * pHost);
extern int pingDualStackAccessible(const char * pHost,int type);


enum {
    IP6_ADDR_UNKNOWNTYPE,
    IP6_ADDR_LINKLOCAL,
    IP6_ADDR_UNIQUELOCAL,
    IP6_ADDR_GLOBALUNIQUE,
    IP6_ADDR_MULTICAST,
    IP6_ADDR_LOOPBACK,
    IP6_ADDR_UNSPECIFIED,
};

#define MAX_IF_NAME_LEN     16
#define MAX_IP6_LEN         46
#define MAX_IF_NAME_LEN     16

#define   CMP_LOGIC_EQUAL  0
#define  CMP_LOGIC_SMALL  -1
#define  CMP_LOGIC_BIG   1
#define MAX_IP6_ADDR_LEN 48

/* �����жϷ���ֵ�Ƿ�����*/
#define RET_SUCCEED(ret) (ret == TRUE)
#define RET_FAILED(ret)  (ret != TRUE)

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

#endif /* _shutils_h_ */
