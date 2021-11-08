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
#define MDNS_LOCK_FILE "/var/run/mdns.lock"
#define RC_DELAY_FILE "/tmp/delay_rc_action"
#define APCLI_SITE_FILE_2G	"/tmp/apcli_site_info_2g"
#define APCLI_SITE_FILE_5G	"/tmp/apcli_site_info_5g"
#define DETECT_TIMEOUT 60
#define SUPPORT_ONETOUCH_FLAG	"/tmp/support_onetouch"

#define DEFAULT_SYSLOG_FILE_PATH  "/var/log/messages"
#define ALL_NVRAM 3

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

#define WIRELESS_ROUTER 	"WirelessRouter"
#define WIRELESS_AP 		"WirelessAp"
#define WIRELESS_REPEATER 	"WirelessRepeaterExtender"
#define WIRELESS_EXTENDER 	"WirelessExtender"

#define MAX_RC_SERVICES_LEN	64
#define RC_SERVER_SOCKET_ADDR "127.0.0.1"
#define RC_SOCKET_PORT		6789


typedef struct {
	char rc_services_name[32];
	int wait_time;
}RC_SERVICES_NOTIFY;

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
#define TEST_RESTORE_FLAG "/tmp/test_restore"

enum{
	FACTORY_CONFIG,
	DEFAULT_CONFIG,
	USER_CONFIG
};
#define SIZE_MIX_MEM_DOWNLOAD_UPGRADE_FW (29*1024)  //29M
#define SIZE_MIX_MEM_UPGRADE_FW (15*1024)			//15M
#define UPGRADE_FW_FAIL_NEED_REBOOT "/tmp/upgrade_fail_reboot"
#define DOWNLOAD_FW_FAIL_NEED_UP "/tmp/download_fw_fail"

#define INTERFACE_ETH_IF		"eth3"
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


#ifdef CONFIG_USER_SUPERMESH
#define INTERFACE_2G_NVRAM_ENABLE			"Radio24Vap0En"
#define INTERFACE_5G_NVRAM_ENABLE			"Radio58Vap0En"
#else
#define INTERFACE_2G_NVRAM_ENABLE			"Radio24On"
#define INTERFACE_5G_NVRAM_ENABLE			"Radio58On"
#endif
#define RADIO_2G_NVRAM_ENABLE				"Radio24On"
#define RADIO_5G_NVRAM_ENABLE				"Radio58On"
#define INTERFACE_2G_GUEST_NVRAM_ENABLE		"Radio24Vap1En"
#define INTERFACE_5G_GUEST_NVRAM_ENABLE		"Radio58Vap1En"

#define SCHEDULE_STOP_WLAN_2G				"/tmp/schedule_stop_2g"
#define SCHEDULE_STOP_WLAN_5G				"/tmp/schedule_stop_5g"
#define SCHEDULE_STOP_WLAN_5GH				"/tmp/schedule_stop_5gh"
#define SCHEDULE_STOP_GUEST_2G				"/tmp/schedule_stop_guest_2g"
#define SCHEDULE_STOP_GUEST_5G				"/tmp/schedule_stop_guest_5g"
#define SCHEDULE_STOP_GUEST_5GH				"/tmp/schedule_stop_guest_5gh"

#if defined(WIFI_DBDC_SUPPORT)
#define	WPS_LED_SOLID			"01-00-00-00-00-0-0-0"
#define	WPS_LED_BLINK			"01-00-00-00-00-0-0-4"
#define	WPS_LED_OFF				"01-00-00-00-00-0-0-1"
#else
#define	WPS_LED_SOLID			"00-00-00-00-00-0-0-0"
#define	WPS_LED_BLINK			"00-00-00-00-00-0-0-4"
#define	WPS_LED_OFF				"00-00-00-00-00-0-0-1"
#endif

#define USB_CN1_FLAG_PATH "/var/cn1_usbevent"  //usb3.0
#define USB_CN12_FLAG_PATH "/var/cn12_usbevent"	//usb2.0

//GPIO defined for led and button
#if defined(PRODUCT_DIR853_A2) || defined(PRODUCT_DIR853_A1)
#define LED_POWER_OK 			"14" 
#define LED_POWER_NG 			"13"
#define LED_INTERNET_OK 		"16"
#define LED_INTERNET_NG 		"15"
#define LED_USB3 				"10"
#define LED_WLAN_24G 			"4" 
#define LED_WLAN_5G 			"3"
//button
#define BUTTON_WPS 				18
#define BUTTON_WIFI_ONOFF 		7
#define BUTTON_RESET 			8
#else
#define LED_POWER_OK 			"16" 
#define LED_POWER_NG 			"8"
#define LED_INTERNET_OK 		"3"
#define LED_INTERNET_NG 		"4"
#define LED_USB3 				"14"
#define LED_USB2 				"13"
#define LED_WLAN_24G 			"11" 
#define LED_WLAN_5G 			"9" 
//button
#define BUTTON_WIFI_ONOFF 			7
#define BUTTON_RESET 			15
#define BUTTON_WPS 				18
#endif
#define LED_ON_CMD				"gpio l %s 4000 0 1 0 4000"
#define LED_OFF_CMD				"gpio l %s 0 4000 0 1 4000"
#define LED_BLINK_CMD			"gpio l %s 5 5 4000 1 4000"
#define LED_BLINK_FAST_CMD		"gpio l %s 1 1 1 1 4000"

#define LED_WLAN_ACTION_OFF 		0 
#define LED_WLAN_ACTION_ON 			1 
#define LED_WLAN_ACTION_BLINK 		2

//wlan led action
#if defined(WIFI_WPS_LED_GPIO_GONTROL)
#define LED_WLAN_2G_ON_CMD				"gpio l "LED_WLAN_24G" 4000 0 1 0 4000"
#define LED_WLAN_2G_OFF_CMD				"gpio l "LED_WLAN_24G" 0 4000 0 1 4000"
#define LED_WLAN_5G_ON_CMD				"gpio l "LED_WLAN_5G" 4000 0 1 0 4000"
#define LED_WLAN_5G_OFF_CMD				"gpio l "LED_WLAN_5G" 0 4000 0 1 4000"
#define LED_WLAN_2G_BLINK_CMD			"gpio l "LED_WLAN_24G" 5 5 4000 1 4000"
#define LED_WLAN_5G_BLINK_CMD			"gpio l "LED_WLAN_5G" 5 5 4000 1 4000"
#else
#define LED_WLAN_2G_ON_CMD				"iwpriv "INTERFACE_2G" set led_setting="WPS_LED_SOLID""
#define LED_WLAN_2G_OFF_CMD				"iwpriv "INTERFACE_2G" set led_setting="WPS_LED_OFF""
#define LED_WLAN_2G_BLINK_CMD			"iwpriv "INTERFACE_2G" set led_setting="WPS_LED_BLINK""
#define LED_WLAN_5G_ON_CMD				"iwpriv "INTERFACE_5G" set led_setting="WPS_LED_SOLID""
#define LED_WLAN_5G_OFF_CMD				"iwpriv "INTERFACE_5G" set led_setting="WPS_LED_OFF""
#define LED_WLAN_5G_BLINK_CMD			"iwpriv "INTERFACE_5G" set led_setting="WPS_LED_BLINK""
#endif

/******************************************************************************
*                               GLOBAL VAR                                   *
******************************************************************************/
//copy from wireless driver rtmp_def.h
/* Country Region definition */
#define REGION_MINIMUM_BG_BAND            0
#define REGION_0_BG_BAND                  0	/* 1-11 */
#define REGION_1_BG_BAND                  1	/* 1-13 */
#define REGION_2_BG_BAND                  2	/* 10-11 */
#define REGION_3_BG_BAND                  3	/* 10-13 */
#define REGION_4_BG_BAND                  4	/* 14 */
#define REGION_5_BG_BAND                  5	/* 1-14 */
#define REGION_6_BG_BAND                  6	/* 3-9 */
#define REGION_7_BG_BAND                  7	/* 5-13 */
#define REGION_31_BG_BAND                 31	/* 1-14 (12-14 PASSIVE_SCAN) */ 
#define REGION_32_BG_BAND                 32	/* 1-13 (12-13 PASSIVE_SCAN)*/
#define REGION_33_BG_BAND                 33	/* 1-14 */
#define REGION_MAXIMUM_BG_BAND            7
	
#define REGION_MINIMUM_A_BAND             0
#define REGION_0_A_BAND                   0	/* 36,40,44,48, 52,56,60,64, 149,153,157,161,165 */
#define REGION_1_A_BAND                   1	/* 36,40,44,48, 52,56,60,64, 100,104,108,112,116,120,124,128,132,136,140 */
#define REGION_2_A_BAND                   2	/* 36,40,44,48, 52,56,60,64 */
#define REGION_3_A_BAND                   3	/* 52,56,60,64, 149,153,157,161 */
#define REGION_4_A_BAND                   4	/* 149,153,157,161,165 */
#define REGION_5_A_BAND                   5	/* 149,153,157,161 */
#define REGION_6_A_BAND                   6	/* 36,40,44,48 */
#define REGION_7_A_BAND                   7	/* 36,40,44,48, 52,56,60,64, 100,104,108,112,116,120,124,128,132,136,140, 149,153,157,161,165 */
#define REGION_8_A_BAND                   8	/* 52,56,60,64 */
#define REGION_9_A_BAND                   9	/* 36,40,44,48, 52,56,60,64, 100,104,108,112,116, 132,136,140, 149,153,157,161,165 */
#define REGION_10_A_BAND                  10	/* 36,40,44,48, 149,153,157,161,165 */
#define REGION_11_A_BAND                  11	/* 36,40,44,48, 52,56,60,64, 100,104,108,112,116,120, 149,153,157,161 */
#define REGION_12_A_BAND                  12	/* 36,40,44,48, 52,56,60,64, 100,104,108,112,116,120,124,128,132,136,140,144 */
#define REGION_13_A_BAND                  13	/* 36,40,44,48, 52,56,60,64, 100,104,108,112,116,120,124,128,132,136,140,144, 149,153,157,161,165 */
#define REGION_14_A_BAND                  14	/* 36,40,44,48, 52,56,60,64, 100,104,108,112,116, 132,136,140,144, 149,153,157,161,165 */
#define REGION_15_A_BAND                  15	/* 149,153,157,161,165,169,173 */
#define REGION_16_A_BAND                  16	/* 52,56,60,64, 149,153,157,161,165 */
#define REGION_17_A_BAND                  17	/* 36,40,44,48, 149,153,157,161 */
#define REGION_18_A_BAND                  18	/* 36,40,44,48, 52,56,60,64, 100,104,108,112,116, 132,136,140 */
#define REGION_19_A_BAND                  19	/* 52,56,60, 100,104,108,112,116,120,124,128,132,136,140, 149,153,157,161 */
#define REGION_20_A_BAND                  20	/* 36,40,44,48, 52,56,60,64, 100,104,108,112,116,120,124, 149,153,157,161 */
#define REGION_21_A_BAND                  21	/* 36,40,44,48, 52,56,60,64, 100,104,108,112,116,120,124,128,132,136,140, 149,153,157,161 */
#define REGION_22_A_BAND                  22	/* 100,104,108,112,116,120,124,128,132,136,140 */
#define REGION_23_A_BAND                  23	/* 52,56,60, 100,104,108,112,116,120,124,128, 149,153,157,161,165 */
#define REGION_24_A_BAND                  24	/* 36,40,44,48, 52,56,60,64, 136,140,144, 149,153,157,161,165 */
#define REGION_25_A_BAND                  25	/* 36,40,44,48, 52,56,60,64, 100,104,108,112,116, 132,136,140, 149,153,157,161,165 */
#define REGION_26_A_BAND                  26	/* 36,40,44,48, 52,56,60, 100,104,108,112,116, 132,136, 149,153,157,161,165 */
#define REGION_27_A_BAND                  27	/* 36,40,44,48, 52,56,60,64, 100,104,108,112,116,120,124,128,132,136,140 */
#define REGION_28_A_BAND                  28	/* 36,40,44,48, 52,56,60,64 */
#define REGION_29_A_BAND                  29	/* 36,40,44,48, 52,56,60,64, 100,104,108,112,116,120,124, 149,153,157,161 */
#define REGION_30_A_BAND                  30	/* 36,40,44,48, 52,56,60,64, 100,104,108,112,116,120,124,128,132,136,140, 149,153,157,161,165 */
#define REGION_31_A_BAND                  31	/* 36,40,44,48, 52,56,60,64, 100,104,108,112, 132,136,140 */
#define REGION_32_A_BAND                  32	/* 36,40,44,48, 149,153,157,161, 165, 169, 173 */
#define REGION_33_A_BAND                  33	/* 100,104,108,112, 132,136,140 */
#define REGION_34_A_BAND                  34	/* 36,40,44,48, 52,56,60,64, 100,104,108,112, 132,136,140, 149,153,157,161, 165, 169, 173 */
#define REGION_MAXIMUM_A_BAND             37

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
typedef struct {
	unsigned char customCountRegion[3];
	unsigned int CountRegion;
	unsigned char CountCode[3];
	unsigned int CountRegionDfs;
}COUNTRY_REGION_CODE;	

extern COUNTRY_REGION_CODE CountryRegionCode2g[];
extern COUNTRY_REGION_CODE CountryRegionCode5g[];

extern void str_binit(struct strbuf *b, char *buf, uint size);
extern int str_bprintf(struct strbuf *b, const char *fmt, ...);
extern int get_meminfo_item(const char *name);
extern void set_action(int a);
extern int check_action(void);
extern int wait_action_idle(int n);
extern char *get_process_name_by_pid(const int pid);
// 下面这两接口已经废弃
//extern int ppp_ifunit(char *ifname);
//extern int wan_ifunit(char *wan_ifname);
extern int TW_StrReplace(char strRes[],char from[], char to[]);
extern int TW_GetFactoryMode();
extern int TW_LoadFactoryConfig();
extern int TW_LoadDefaultConfig();
extern void loadDefault(int chip_id);
extern void loadCountryRegion2g();
extern void loadCountryRegion5g();
extern void loadCountryRegion5g2();
extern void loadDefaultSSID(int idx);
extern void loadDefaultEncrypt(int idx);
extern void loadDefaultWpsPin(int idx);
extern void loadDefaultConsolePwd();
extern int PTI_GetMacAddr(char *pszMac);
extern int PTI_nvram_get(const char *name);

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

/* 用来判断返回值是否正常*/
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
