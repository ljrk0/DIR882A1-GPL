/*
Copyright (C) 2003-2004 Narcis Ilisei
Modifications by Steve Horbachuk
Copyright (C) 2006 Steve Horbachuk
Modifications by Bryan Hoover (bhoover@wecs.com)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/*

	History:
		Dec 2007 -- Added get_cmd.h include, for get_config_data_with_error_handling
					prototype.
*/

/*interface for main dydns functions */
#ifndef _DYNDNS_INCLUDED
#define _DYNDNS_INCLUDED

#include "debug_if.h"
#include "get_cmd.h"
#include "wave_util.h"
#include "addr_list.h"

#ifdef _WIN32

#include "ras_events.h"
#include "service_main.h"

#endif

#ifndef USE_SNDFILE

#define DYNDNS_VERSION_SND

#ifndef USE_THREADS

#define DYNDNS_VERSION_THD

#else

#define DYNDNS_VERSION_THD	"_mt"

#endif

#else

#define DYNDNS_VERSION_SND	"_audible"

#ifndef USE_THREADS

#define DYNDNS_VERSION_THD	"_no-mt"

#else

#define DYNDNS_VERSION_THD

#endif

#endif

#define	DYNDNS_EXTRA_VERSION

#define DYNDNS_VERSION_STRING	"02.28.10" DYNDNS_VERSION_SND DYNDNS_VERSION_THD DYNDNS_EXTRA_VERSION

#ifdef PRODUCT_DIR878
#define DYNDNS_AGENT_NAME	"D-Link DIR-878"
#elif defined(PRODUCT_DIR882)
#define DYNDNS_AGENT_NAME	"D-Link DIR-882"
#elif defined(PRODUCT_DIR867)
#define DYNDNS_AGENT_NAME	"D-Link DIR-867"
#elif defined(PRODUCT_DIR853_A2) || defined(PRODUCT_DIR853_A1)
#define DYNDNS_AGENT_NAME	"D-Link DIR-853"
#endif

#define DYNDNS_EMAIL_ADDR	"bhoover@wecs.com"

#define USER_DEFINED_SERVER	"custom@http_svr_basic_auth"

typedef enum
{
    DYNDNS_DYNAMIC,
    DYNDNS_STATIC,
    DYNDNS_CUSTOM,
    DYNDNS_DEFAULT,
    FREEDNS_AFRAID_ORG_DEFAULT,
    ZONE_EDIT_DEFAULT,
    CUSTOM_HTTP_BASIC_AUTH,
    DYNDNS_DLINK,
    NOIP_DEFAULT,
    EASYDNS_DEFAULT,
    DYNDNS_3322_DYNAMIC,
    SITELUTIONS_DOMAIN,
    TZO_DEFAULT,
    DNSOMATIC_DEFAULT,
    HE_IPV6TB,
    HE_DEFAULT,
    DYNSIP_DEFAULT,
    DHIS_DEFAULT,
    MAJIMOTO_DEFAULT,
    ZERIGO_DEFAULT,
    TWODNS_DEFAULT,
    LAST_DNS_SYSTEM = -1
} DYNDNS_SYSTEM_ID;

/*test values*/
#define DYNDNS_DEFAULT_DEBUG_LEVEL		LOG_WARNING
#define DYNDNS_MT_DEFAULT_CONFIG_FILE_OLD	"/etc/inadyn-mt/inadyn-mt.conf"
#define DYNDNS_MT_DEFAULT_CONFIG_FILE		"/etc/inadyn-mt.conf"

/*inadyn backward compatibility*/
#define DYNDNS_DEFAULT_CONFIG_FILE		"/etc/inadyn.conf"

#define DYNDNS_DEFAULT_CACHE_PREFIX		"/tmp/"
#define DYNDNS_DEFAULT_IP_FILE			"inadyn_ip.cache"
#define DYNDNS_DEFAULT_TIME_FILE		"inadyn_time.cache"

#define DYNDNS_MY_USERNAME			"test"
#define DYNDNS_MY_PASSWD			"test"
#define DYNDNS_MY_IP_SERVER			"checkip.dyndns.org"
#define DYNDNS_MY_IP_SERVER_URL			"/"
#define DYNDNS_MY_ONLINE_CHECK			"wrpd.dlink.com"
#define DYNDNS_MY_ONLINE_CHECK_URL		"/"

#define DYNDNS_MY_DNS_SERVER			"members.dyndns.org"
#define DYNDNS_MY_DNS_SERVER_URL		"/nic/update?"
#define DYNDNS_MY_HOST_NAME_1			"test.homeip.net"
#define DYNDNS_MY_HOST_NAME_2			"test2.homeip.net"
#define DYNDNS_HTTP_PORT			80

/* botho 30/07/06 : add www.3322.org */
#define DYNDNS_3322_MY_IP_SERVER	"bliao.com"
#define DYNDNS_3322_MY_IP_SERVER_URL	"/ip.phtml"
#define DYNDNS_3322_MY_DNS_SERVER	"members.3322.org"
#define DYNDNS_3322_MY_DNS_SERVER_URL	"/dyndns/update?"

/*REQ/RSP definitions*/

#define DYNDNS_IP_SERVER_RESPONSE_BEGIN	"Current IP Address: "
#define DYNDNS_IP_ADDR_FORMAT		"%d.%d.%d.%d"
#define DYNDNS_ALL_DIGITS		"0123456789"


#define DYNDNS_SYSTEM_CUSTOM	"custom"
#define DYNDNS_SYSTEM_DYNAMIC	"dyndns"
#define DYNDNS_SYSTEM_STATIC	"statdns"

#define GENERIC_DNS_IP_SERVER_NAME	DYNDNS_MY_IP_SERVER
#define DYNDNS_MY_DNS_SYSTEM		DYNDNS_DEFAULT

#define MAX_IP6_LEN         46

#define DYNDNS_HTTP_REQUEST_FORMAT \
	"GET %s\r\n" \
	"HTTP/1.0\r\n" \
	"User-Agent: %s\r\n" \
	"Host: %s\r\n\r\n"

/* Conversation with the IP server */
#define DYNDNS_GET_MY_IP_HTTP_REQUEST  \
	"GET %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: " DYNDNS_AGENT_NAME " " DYNDNS_EMAIL_ADDR "\r\n\r\n"

/* dyndns.org specific update address format */
/* 3322.org has the same parameters ...*/
#if 0
#define DYNDNS_GET_MY_IP_HTTP_REQUEST_FORMAT \
	"GET %s" \
	"system=%s&" \
	"hostname=%s&" \
	"myip=%s&" \
	"wildcard=%s&" \
	"mx=%s&" \
	"backmx=NO&" \
	"offline=NO " \
	"HTTP/1.0\r\n" \
	"Host: %s\r\n" \
	"Authorization: Basic %s\r\n" \
	"User-Agent: "DYNDNS_AGENT_NAME " " DYNDNS_EMAIL_ADDR"\r\n\r\n"
#else

#define DYNDNS_GET_MY_IP_HTTP_REQUEST_FORMAT \
	"GET %s" \
	"hostname=%s&" \
	"myip=%s&" \
	"wildcard=NOCHG&" \
	"mx=NOCHG&" \
	"backmx=NOCHG  " \
	"HTTP/1.0\r\n" \
	"User-Agent: %s\r\n" \
	"Host: %s\r\n" \
	"Authorization: Basic %s\r\n\r\n"

#endif
/*freedns.afraid.org specific update request format */
#define FREEDNS_UPDATE_MY_IP_REQUEST_FORMAT \
	"GET %s" \
	"%s&address=%s " \
	"HTTP/1.0\r\n" \
	"Host: %s\r\n" \
	"User-Agent: "DYNDNS_AGENT_NAME " " DYNDNS_EMAIL_ADDR"\r\n\r\n"

/** generic update format for sites that perform the update
	with:
	http://some.address.domain/somesubdir
	?some_param_name=MY_ALIAS
	and then the normal http stuff and basic base64 encoded auth.
	The parameter here is the entire request but NOT including the alias.
*/
#define GENERIC_DNS_BASIC_AUTH_MY_IP_REQUEST_FORMAT \
	"GET %s%s " \
	"HTTP/1.0\r\n" \
	"Authorization: Basic %s\r\n" \
	"Host: %s\r\n" \
	"User-Agent: "DYNDNS_AGENT_NAME " " DYNDNS_EMAIL_ADDR"\r\n\r\n"

#define GENERIC_DNS_P_MAPPED_BASIC_AUTH_MY_IP_REQUEST_FORMAT \
        "GET %s%s " \
        "HTTP/1.0\r\n" \
        "Authorization: Basic %s\r\n" \
        "Host: %s\r\n" \
        "User-Agent: "DYNDNS_AGENT_NAME " " DYNDNS_EMAIL_ADDR"\r\n\r\n"

#define GENERIC_DNS_CUSTOM_URL_P_PARAM_MY_IP_REQUEST_FORMAT \
        "GET %s%s " \
        "HTTP/1.0\r\n" \
        "Host: %s\r\n" \
        "User-Agent: "DYNDNS_AGENT_NAME " " DYNDNS_EMAIL_ADDR"\r\n\r\n"


#define GENERIC_NOIP_AUTH_MY_IP_REQUEST_FORMAT_NEW \   
	"GET %s%s " \	 
	"HTTP/1.0\r\n" \	
	"Authorization: Basic %s\r\n" \	
	"Host: %s\r\n" \	
	"User-Agent: "DYNDNS_AGENT_NAME " " DYNDNS_EMAIL_ADDR"\r\n\r\n"	

/* dlinkddns.com specific update address format */
#define DLINKDDNS_GET_MY_IP_HTTP_REQUEST_FORMAT_NEW \		
	"GET %s" \		
	"hostname=%s&" \		
	"wildcard=%s " \		
	"HTTP/1.0\r\n" \		
	"Authorization: Basic %s\r\n" \		
	"User-Agent: "DYNDNS_AGENT_NAME " " DYNDNS_EMAIL_ADDR"\r\n\r\n"\		
	"Host: %s\r\n" 

/** dont ask me why easydns is so picky
*/
#define GENERIC_EASYDNS_AUTH_MY_IP_REQUEST_FORMAT \
	"GET %s%s&" \
	"myip=%s&" \
	"wildcard=%s "\
	"HTTP/1.0\r\n" \
	"Authorization: Basic %s\r\n" \
	"Host: %s\r\n" \
	"User-Agent: "DYNDNS_AGENT_NAME " " DYNDNS_EMAIL_ADDR"\r\n\r\n"

/* sitelutions.com specific update address format */
#define SITELUTIONS_GET_MY_IP_HTTP_REQUEST_FORMAT \
	"GET %s" \
	"user=%s&" \
	"pass=%s&" \
	"id=%s&" \
	"ip=%s&" \
	"HTTP/1.0\r\n" \
	"Host: %s\r\n" \
	"User-Agent: "DYNDNS_AGENT_NAME " " DYNDNS_EMAIL_ADDR"\r\n\r\n"

/* tzo doesnt encode password */
#define GENERIC_TZO_AUTH_MY_IP_REQUEST_FORMAT \
	"GET %s%s&" \
	"Email=%s&" \
	"TZOKey=%s&" \
	"IPAddress=%s "	\
	"HTTP/1.0\r\n" \
	"Host: %s\r\n" \
	"User-Agent: "DYNDNS_AGENT_NAME " " DYNDNS_EMAIL_ADDR"\r\n\r\n"

/* HE tunnelbroker.com specific update request format */
#define HE_IPV6TB_UPDATE_MY_IP_REQUEST_FORMAT \
	"GET %s" \
	"ipv4b=%s&" \
	"user_id=%s&" \
	"pass=%s&" \
	"tunnel_id=%s "	\
	"HTTP/1.0\r\n" \
	"Host: %s\r\n"	\
	"User-Agent: "DYNDNS_AGENT_NAME " " DYNDNS_EMAIL_ADDR"\r\n\r\n"

/* HE DynDNS specific update request format */
#define HE_DEFAULT_UPDATE_MY_IP_REQUEST_FORMAT	\
	"GET %s" \
	"hostname=%s&"	\
	"myip=%s " \
	"HTTP/1.0\r\n"	\
	"Authorization: Basic %s\r\n"	\
	"Host: %s\r\n" \
	"User-Agent: "DYNDNS_AGENT_NAME " " DYNDNS_EMAIL_ADDR"\r\n\r\n"

/* is.dhis.org specific update request format */
#define DHIS_MY_IP_UPDATE_REQUEST_FORMAT \
	"GET %s" \
	"hostname=%s&" \
	"password=%s&" \
	"ipaddr=%s&" \
	"updatetimeout=0 " \
	"HTTP/1.0\r\n" \
	"Host: %s\r\n" \
	"User-Agent: "DYNDNS_AGENT_NAME " " DYNDNS_EMAIL_ADDR"\r\n\r\n"

/* majimoto.net specific update request format */
#define MAJIMOTO_MY_IP_UPDATE_REQUEST_FORMAT \
	"GET %s" \
	"hostname=%s&" \
	"myip=%s " \
	"HTTP/1.0\r\n" \
	"Authorization: Basic %s\r\n" \
	"Host: %s\r\n" \
	"User-Agent: "DYNDNS_AGENT_NAME " " DYNDNS_EMAIL_ADDR"\r\n\r\n"

/* zerigo.net specific update request format */
#define ZERIGO_MY_IP_UPDATE_REQUEST_FORMAT \
	"GET %s%s&" \
	"ip=%s " \
	"HTTP/1.0\r\n" \
	"Authorization: Basic %s\r\n" \
	"Host: %s\r\n" \
	"User-Agent: "DYNDNS_AGENT_NAME " " DYNDNS_EMAIL_ADDR"\r\n\r\n"

/* two-dns.com specific update request format */
#define TWODNS_MY_IP_UPDATE_REQUEST_FORMAT \
	"GET %s" \
	"hostname=%s&" \
	"ip=%s " \
	"HTTP/1.0\r\n" \
	"Authorization: Basic %s\r\n" \
	"Host: %s\r\n" \
	"User-Agent: "DYNDNS_AGENT_NAME " " DYNDNS_EMAIL_ADDR"\r\n\r\n"

#define DYNDNS_OK_RESPONSE	"good"
#define DYNDNS_OK_NOCHANGE	"nochg"
#define DYNDNS_DNS_ERROR	"dnserr"
#define DYNDNS_911		"911"


/* SOME DEFAULT CONFIGURATIONS */
#define DYNDNS_DEFAULT_SLEEP			(600)	/*s*/
#define DYNDNS_MIN_SLEEP			(30)	/*s*/
#define DYNDNS_MAX_SLEEP			(10 * 24 * 3600)	/*10 days in s*/
#define DYNDNS_MIN_FORCED_UPDATE_PERIOD_S	DYNDNS_MIN_SLEEP
#define DYNDNS_MAX_FORCED_UPDATE_PERIOD_S	(30 * 24 * 3600)	/* 30 days in sec*/
#define DYNDNS_MY_FORCED_UPDATE_PERIOD_S	DYNDNS_MAX_FORCED_UPDATE_PERIOD_S	/* 30 days in sec*/
#define DYNDNS_DEFAULT_UPDATE_ADJUST		.5
#define DYNDNS_MIN_FORCED_UPDATE_ADJUST		-4
#define DYNDNS_MAX_FORCED_UPDATE_ADJUST		5
#define DYNDNS_DEFAULT_CMD_CHECK_PERIOD		(1)	/*s*/
#define DYNDNS_DEFAULT_ITERATIONS		0	/*forever*/
#define DYNDNS_DEFAULT_NET_RETRIES		0
#define DYNDNS_DEFAULT_RETRY_INTERVAL		1000
#define DYNDNS_DEFAULT_RETRY_PENDING_INTERVAL	10
#define DYNDNS_HTTP_RESPONSE_BUFFER_SIZE	(4524)	/*Bytes*/
#define DYNDNS_HTTP_REQUEST_BUFFER_SIZE		(2524)	/*Bytes*/
#define DYNDNS_MAX_ALIAS_NUMBER			20	/*maximum number of aliases that can be maintained*/
#define DYNDNS_MAX_ALERT_INTERVAL		3600000	/*maximum time between audible alert soundings*/
#define DYNDNS_MIN_ALERT_INTERVAL		0	/*min time between audible alert soundings*/
#define DYNDNS_MAX_ALERT_RETRIES		100	/*max retries before sounding alert*/
#define DYNDNS_MIN_ALERT_RETRIES		0	/*min retries before sounding alert*/
#define DYNDNS_MIN_STATUS_INTERVAL		30	/*min online status check interval*/
#define DYNDNS_MAX_STATUS_INTERVAL		864000	/*max online status check interval*/
#define DYNDNS_MIN_STATUS_OFFLINE_INTERVAL	0	/*min online status check interval, after offline detect*/
#define DYNDNS_MAX_STATUS_OFFLINE_INTERVAL	864000	/*max online status check interval, after offline detect*/
#define DYNDNS_MIN_RETRY_INTERVAL		0
#define DYNDNS_MAX_RETRY_INTERVAL		30000
#define DYNDNS_MIN_RETRY_PENDING_INTERVAL	5
#define DYNDNS_MAX_RETRY_PENDING_INTERVAL	3600
#define DYNDNS_MIN_WAVE_LOOPS			0	/*min number of times per wave play call to repeat wave file*/
#define DYNDNS_MAX_WAVE_LOOPS			100	/*max number of times per wave play call to repeat wave file*/
#define DYNDNS_MIN_WAVE_BUFF_FACTOR		.25	/*min by which to divide or multiply wave file bytes per second for buffer size*/
#define DYNDNS_MAX_WAVE_BUFF_FACTOR		10	/*max by which to divide or multiply wave file bytes per second for buffer size*/

/*attenuation max, min constants -- translate to [-40..0], [-120db..0db]*/
#define DYNDNS_MAX_WAVE_RANGE			120
#define DYNDNS_MIN_WAVE_GAIN			-10	/*minimum wave play volume*/
#define DYNDNS_MAX_WAVE_GAIN			10	/*maximum wave play volume*/

#define DYNDNS_INFINITE_WAVE			-1	/*infinite times per wave play call to play wave file*/
#define DYNDNS_DEFAULT_INAUDIBLE		1	/*audible alerts default as off*/
#define DYNDNS_DEFAULT_ALERT_RETRIES		0	/*network retries before sounding alert*/
#define DYNDNS_DEFAULT_ALERT_INTERVAL		0	/*ms time between audible alerts*/
#define DYNDNS_DEFAULT_WAVE_FILE		"alarm.wav"	/*default wave file, and path ("current working dir")*/
#define DYNDNS_DEFAULT_WAVE_BUFF_FACTOR		.25		/*max by which to divide or multiply wave file bytes per second for buffer size*/

#define DYNDNS_DEFAULT_LANG_FILE		"en.lng"	/*default debug output message language strings file*/

/*increments in [DYNDNS_MIN_WAVE_GAIN..DYNDNS_MAX_WAVE_GAIN]*/
#define DYNDNS_WAVE_RANGE			(-1*DYNDNS_MIN_WAVE_GAIN+DYNDNS_MAX_WAVE_GAIN)

/*wave play mid point gain*/
#define DYNDNS_DEFAULT_WAVE_GAIN		-1*DYNDNS_WAVE_RANGE/2

/*default to no attenuation (0db)*/
#define DYNDNS_DEFAULT_USER_WAVE_GAIN		10

/*allow attenuate to 120db in DYNDNS_MAX_WAVE_RANGE increments*/
#define DYNDNS_DECIBEL_STEP		DYNDNS_MAX_WAVE_RANGE/(-1*DYNDNS_MIN_WAVE_GAIN+DYNDNS_MAX_WAVE_GAIN)

#define DYNDNS_DEFAULT_WAVE_LOOPS		0	/*default number of times per wave play call to play wave file*/

#define DYNDNS_DEFAULT_STATUS_INTERVAL		600	/*seconds online status check interval default*/
#define DYNDNS_DEFAULT_STATUS_OFFLINE_INTERVAL	15	/*seconds online status check interval default, after offline detect*/

/*local configs*/
#define DYNDNS_MY_USERNAME_LENGTH		50	/*chars*/
#define DYNDNS_MY_PASSWORD_LENGTH		50	/*chars*/
#define DYNDNS_SERVER_NAME_LENGTH		256	/*chars*/
#define DYNDNS_SERVER_URL_LENGTH		256	/*chars*/
#define DYNDNS_HASH_STRING_MAX_LENGTH		256	/*chars*/
#define IP_V4_MAX_LENGTH			16	/*chars: nnn.nnn.nnn.nnn\0*/

#define MAX_DNS_SERVER		10

/* forwards */
struct DYN_DNS_CLIENT;
struct DYNDNS_SYSTEM;
struct DYNDNS_INFO_TYPE;
struct EXP_MAPPING;

/** Function to prepare DNS system specific server requests
*/
typedef int (*DNS_SYSTEM_REQUEST_FUNC) (struct DYN_DNS_CLIENT *this, struct EXP_MAPPING *p_mapping, char srv_cnt, int nr, struct DYNDNS_SYSTEM *p_sys_info);
typedef RC_TYPE (*DNS_SYSTEM_SRV_RESPONSE_OK_FUNC) (struct DYN_DNS_CLIENT *this, int alias_index, char *p_rsp, char *p_ok_str, char srv_cnt);

/** Types used for DNS system specific configuration
*/
typedef struct
{
	char*				p_key;
	void*				p_specific_data;
	int				is_auto_dual_list;
	int				is_auto_detect;
	DNS_SYSTEM_SRV_RESPONSE_OK_FUNC	p_rsp_ok_func;
	DNS_SYSTEM_REQUEST_FUNC		p_dns_update_req_func;
	char				*p_ip_server_name;
	char				*p_ip_server_url;
	char				*p_dyndns_server_name;
	char				*p_dyndns_server_url;
	char				*p_success_string;
} DYNDNS_SYSTEM;

typedef struct
{
	DYNDNS_SYSTEM_ID id;
	DYNDNS_SYSTEM system;
} DYNDNS_SYSTEM_INFO;

typedef struct
{
	const char *p_system;
} DYNDNS_ORG_SPECIFIC_DATA;

typedef enum
{
	NO_CMD		= 0,
	CMD_STOP	= 1,
	CMD_UPDT	= 2,
	CMD_UPDTED	= 3
} DYN_DNS_CMD;

typedef struct
{
	char		*alias;
	char *str;
	DYNDNS_IPV	ipv_enum;
} DYNDNS_HASH_TYPE;

typedef struct
{
	char	*my_username;
	char	*my_password;
	char	*p_enc_usr_passwd_buffer;
	int	size;
	BOOL	encoded;
} DYNDNS_CREDENTIALS;

/*a given alias (dyn domain names) can
  be associated with one "ip type", of
  'dual', 'coupled', 'ip4', 'ip6', 'auto',
  mapping to IPv4, IPv6, or both (coupled)
  address types.  store ip type as a string.
  enum maps to corresponding actual IPv4 or
  IPv6 address in a string array,
  my_ip_address.
*/
typedef struct
{
	char		*name;
	DYNDNS_IPV	ip_v_enum;
	char		ip_v[MAX_IP_TYPE_SIZE];

	/*one of two as part of alias type "dual"?*/
	int		is_dual;
} DYNDNS_ALIAS_NAME;

typedef struct
{
	char			*iface;
	char			*ip_ver_str;
	IPV6_PRIV_EXT		*priv_ext;
	char			*ip_server_name;
	char			*ip_server_url;
	int			ip_server_port;
	char			*ip_server_host;
	char			*ip_proxy_name;
	int			ip_proxy_port;
} ALIAS_PARAMS;

typedef struct
{
	/*multiple of ip types (2 -- IPv4, IPv6)*/
	DYNDNS_ALIAS_NAME	names[DYNDNS_MAX_ALIAS_NUMBER];

	/*tmp params per alias, for IP_ADDRESS creates, at user
	  input parse end*/
	ALIAS_PARAMS		**p_params;
	int			update_required[DYNDNS_MAX_ALIAS_NUMBER];
	int			update_succeeded[DYNDNS_MAX_ALIAS_NUMBER];
	int			fatal_error[DYNDNS_MAX_ALIAS_NUMBER];
	DYNDNS_HASH_TYPE	hashes[DYNDNS_MAX_ALIAS_NUMBER];
	int			is_got_svr_hashes;
	DYNDNS_HASH_TYPE	svr_hashes[DYNDNS_MAX_ALIAS_NUMBER];
	int			count;
	int			svr_hashes_count;
	IP_ADDRESS		*p_address[DYNDNS_MAX_ALIAS_NUMBER];
	int			server_index[DYNDNS_MAX_ALIAS_NUMBER];
} DYNDNS_ALIAS_INFO;

/*per update server*/
typedef struct DYNDNS_INFO_TYPE
{

	HTTP_CLIENT	http_to_ip_server;
	HTTP_CLIENT	http_to_dyndns;

	DYNDNS_CREDENTIALS	credentials;
	DYNDNS_SYSTEM		*p_dns_system;
	DYNDNS_SERVER_NAME	dyndns_server_name;
        char                    *dyndns_server_host;
        DYNDNS_SERVER_NAME      ip_server_name;
        char                    *ip_server_host;
	DYNDNS_SERVER_NAME	proxy_server_name;

	DYNDNS_ALIAS_INFO	alias_info;
        BOOL                    wildcard;

	BOOL			is_bad_config;
	BOOL			is_update_pending;

	/*additional custom DDNS server type options*/
	gen_list		rsp_success_codes;
	gen_list		rsp_config_codes;
	char			*ip_param;
	char			*id_param;
	char			*pw_param;
	int			is_ip_in_svr_rsp;
	int			is_auto_dual_list;
	int			is_auto_detect;

	IPV6_PRIV_EXT		ipv6_priv_ext;
	char			*if_name;

	BOOL			is_defaults_config;
} DYNDNS_INFO_TYPE;

typedef struct
{
	int uid;
	int gid;
} USER_INFO;

typedef struct IP_CACHE
{
	char		*ip;
	char		*alias;
	char		*type;
	char		*server;
	char		*ip_alias_type;

	unsigned char	ip_n[16];
} IP_CACHE;

typedef struct IP_CACHE_LIST {

	IP_CACHE	*saved_ip_cache;

	int		count;
} IP_CACHE_LIST;

typedef struct DYN_DNS_CLIENT
{
	DYN_DNS_CMD	cmd;
	int		sleep_sec;			/* time between 2 updates*/
	unsigned	forced_update_period_sec;
	float		forced_update_adjust;		/* how to round forced update calculation */
	unsigned	forced_update_period_sec_orig;	/* original read from cmd line */
	int		times_since_last_update;
	int		forced_update_times;		/* the same forced update period counted in sleep periods*/
	unsigned	cmd_check_period;		/*time to wait for a command*/
	unsigned	forced_update_counter;		/*cmd_check_periods tracking forced_update_period expiration*/	
	int		is_forced_update_attempted;
	int		total_iterations;
	int		iterations;
	int		net_retries;			/*failed comm retries*/
	long		retry_interval;			/*miliseconds wait interval between retries*/
	long		retry_pending_interval;
	int		retry_pending_off;
	int		retry_later;			/*dyndns2 network error - retry in 5 minutes*/
	char		*lang_file;			/*language (locale) file full path, and file name*/
	int		lang_hard_coded;		/*flagged when --lang_file parameter with empty option*/
	int		audible_off;			/*compiled with USE_SNDFILE defined, but want run without*/
	char		*wave_file;			/*network status alerts wave file full path, and file name*/
	int		wave_loops;			/*number of times per wave play call to play wave file*/
	float		wave_gain;			/*volume at which to play wave file*/
	int		alert_retries;			/*network retries before sounding alert*/
	int		alert_interval;			/*time betweens sounding network status alarm*/
	int		status_interval;		/*ms between online status checks*/
	int		status_offline_interval;	/*ms between online status checks, after offline detect*/
	float		wave_buff_factor;		/*multiple of wave file bytes per second*/

	BOOL		initialized;
	BOOL		run_in_background;
	BOOL		debug_to_syslog;
	BOOL		change_persona;

	HTTP_TRANSACTION	http_tr;
	char			*p_req_buffer;		/* for HTTP requests*/
	int			req_buffer_size;
	char			*external_command;
	char			*time_cache;
	char			*ip_cache;

	USER_INFO		sys_usr_info;		/*info about the current account running inadyn*/


	DYNDNS_INFO_TYPE	info[MAX_DNS_SERVER];	/*servers, names, passwd*/
	char			srv_cnt;

	gen_list		address_list;		/*list of type I_FACE structures*/

	struct ifaddrs_w	*ifaddrs;		/*interface enumeration*/

	IP_CACHE_LIST		ip_cache_list;		/*read in from ip cache file written on ddns update*/

	DYNDNS_SERVER_NAME	info_online_status;	/*info about url to reach to confirm online status -- defaults to info*/

	BOOL			abort_on_network_errors;
	BOOL			force_addr_update;
	BOOL			use_proxy;
	BOOL			abort;
	BOOL			is_global_bad_config;	/*all servers*/

	DYNDNS_SERVER_NAME	ip_server_name_global;
	/*should add a global for proxy too*/

	char 			*p_pidfilename;

	char			*servers_additional;

	/*dbg*/
	DBG_TYPE	dbg;

	int		is_improbability;

	
	BOOL			is_private_ipv6_addr;
	char			private_ipv6_addr[MAX_IP6_LEN];
} DYN_DNS_CLIENT;

/*for mapping servers_additional.cfg variable identifiers
  to values in arg_to_val (in inadyn_cmd.c)*/
struct EXP_MAPPING {

	DYN_DNS_CLIENT  *p_dyndns;
	int		server_index;
	int             alias_index;

	char		*p_exp_url;
	int		is_username;
	int		is_password;
	int		is_alias;
	int		is_ip_addr;
}EXP_MAPPING;

/*public functions*/

/** Returns the table of supported dyndns systems
*/
DYNDNS_SYSTEM_INFO* get_dyndns_system_table(void);

/*
	Set up all details:
		- ip server name
		- dns server name
		- username, passwd
		- ...
	Implementation:
		- load defaults
		- parse cmd line
        - assign settings that may change due to cmd line options
		- check data
*/
RC_TYPE get_config_data(DYN_DNS_CLIENT *p_self, int argc, char** argv);
RC_TYPE do_get_config_data(DYN_DNS_CLIENT *p_self, int argc, char** argv);

RC_TYPE get_config_data_with_error_handling(DYN_DNS_CLIENT *p_self, int argc, char** argv,CMD_OPTION_ERR_HANDLER_FUNC pf_err_handler);
RC_TYPE set_wave_gain(DYN_DNS_CLIENT *p_self,float gain);

RC_TYPE os_shell_execute(char *p_cmd);

char *arg_to_val(void *p_context,char *token);

char *print_time(void);

/*
	 basic resource allocations for the dyn_dns object
*/
RC_TYPE dyn_dns_construct(DYN_DNS_CLIENT **pp_self);

/*
	Resource free.
*/
RC_TYPE dyn_dns_destruct(DYN_DNS_CLIENT *p_self);

void dyndns_event(int i_event,void  *p_context);

/*
	Disconnect and some other clean up.
*/

/* MAIN - Dyn DNS update entry point.*/

/*
	Actions:
		- read the configuration options
		- create and init dyn_dns object.
		- launch the IP update action
*/
int dyn_dns_main(DYN_DNS_CLIENT *p_self, int argc, char* argv[]);


/*
	help.
*/
void print_help_page(void);

/*
    main entry point
*/
#ifndef _WIN32

int inadyn_main(int argc, char* argv[]);

#else

int service_event_handler(SERVICE_EVENT service_event);

int inadyn_main(int argc, char* argv[],int is_service_in,FILE *pLOG_FILE);

#endif

RC_TYPE is_generic_server_rsp_ok(struct DYN_DNS_CLIENT *p_self, int alias_index, char *p_rsp, char *p_ok_string, char srv_cnt);
int get_req_for_generic_custom_http_dns_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int alias_index, DYNDNS_SYSTEM *p_sys_info);

RC_TYPE fetch_and_store_afraid_hashes(DYNDNS_INFO_TYPE *info);
RC_TYPE get_afraid_hash(char **hash,char *alias,DYNDNS_IPV ip_v,DYNDNS_INFO_TYPE *info);
int is_user_pass(char *username,char *password);

#endif /*_DYNDNS_INCLUDED*/
