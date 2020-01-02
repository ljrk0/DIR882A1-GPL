/*
Copyright (C) 2003-2004 Narcis Ilisei
Modifications by Steve Horbachuk
Copyright (C) 2006 Steve Horbachuk
Modifications by Bryan Hoover (bhoover@wecs.com)
Copyright (C) 2007 Bryan Hoover (bhoover@wecs.com)

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
	Dyn Dns update main implementation file
	Author: narcis Ilisei
	Date: May 2003

	History:
		- first implemetnation
		- 18 May 2003 : cmd line option reading added -
		- many options added
		- january 2005 - new format for the config file =Thanks to Jerome Benoit.
		- january 30 2005 - new parser for config file -
		- october 2007 - debug level command line parameter added
		- dec 2007 - file options handler now provides for command line options precedence
		- June/July 2009 - added audible alert related options
		- dec 2009 - accomodate/change relative to static to heap memory DYN_DNS_CLIENT members
		- dec 2009 - fixed handlers' allocation overflow/range checking
*/
#define MODULE_TAG "CMD_OPTS: "

#ifndef EXCLUDE_CONFIG_H

#include "config.h"

#endif

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>

#include "dyndns.h"
#include "debug_if.h"
#include "base64.h"
#include "get_cmd.h"
#include "unicode_util.h"
#include "safe_mem.h"
#include "path.h"
#include "args_exp.h"
#include "addr_list.h"

/* command line options */
#define DYNDNS_INPUT_FILE_OPT_STRING "--input_file"

static RC_TYPE help_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE enum_interface_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE wildcard_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_username_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_password_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_alias_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_debug_level_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_dns_server_name_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_dns_server_url_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_dns_server_host_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_ip_server_name_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_global_ip_server_name_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_ip_server_host_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_online_check_url_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_dyndns_system_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_update_period_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_update_period_sec_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_forced_update_period_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);

#ifdef USE_THREADS

static RC_TYPE get_forced_update_adjust_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);

#endif

static RC_TYPE get_logfile_name(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_pidfile_name(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE set_silent_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE set_verbose_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_proxy_server_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_options_from_file_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE set_iterations_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE set_syslog_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE set_change_persona_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE print_version_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_exec_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_cache_dir_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_retries_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_retry_interval_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_retry_pending_interval_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_retry_pending_off_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_lang_file_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_addr_pref_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_private_ipv6_addr(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_improb_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_if_name_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_ip_param_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_success_rsp_params_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_config_rsp_params_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_ip_in_svr_rsp_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_auto_dual_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_auto_detect_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_svr_add_cfg_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);

#ifdef USE_SNDFILE

static RC_TYPE get_wave_file_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_audible_on_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_alert_retries_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_alert_interval_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_wave_loops_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_wave_gain_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_wave_buff_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);

#ifdef USE_THREADS

static RC_TYPE get_status_interval_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);
static RC_TYPE get_status_offline_interval_handler(CMD_DATA *p_cmd, int current_nr, void *p_context);

#endif

#endif

/*forwards*/
static void do_get_if_name_handler(char **dest,char *src);
static RC_TYPE set_priv_ext(char *pref_arg,IPV6_PRIV_EXT **ipv6_priv_ext);
static int get_priv_ext_flags(char *pref_arg);
I_FACE* iface_list_add_iface(gen_list *iface_list,char *iface_name);
static RC_TYPE do_set_url_param(CMD_DATA *p_cmd,int *current_nr,char **ip_server_name,char **ip_server_url,int *port);
static RC_TYPE do_get_ip_server_host_handler(CMD_DATA *p_cmd, int current_nr, char **ip_server_host);
static RC_TYPE do_get_proxy_server(CMD_DATA *p_cmd, int current_nr, char **proxy_server_name,int *proxy_port);
static RC_TYPE set_file_param(char **dest, char *dir,char *file_name);
static RC_TYPE set_file_param2(char **dest,char *file_name);
static RC_TYPE do_set_file_param(char **dest, char *dir,char *file_name,int is_create);

/*
	muti-services -- inc or set index based on
	most recent server name option parameter
	appearance.
*/
static int server_index=0;

static CMD_DESCRIPTION_TYPE cmd_options_table[] =
{
        {"--help",	0,	{help_handler, NULL,0},	"help",1},
        {"-h",		0,	{help_handler, NULL,0},	"help\n",1},

        {"--username",	1,	{get_username_handler, NULL,0},	"<name or hash> - your  membername/hash",1},
        {"-u",		1,	{get_username_handler, NULL,0},	"<name or hash> - your membername/hash\n",1},

        {"--password",	1,	{get_password_handler, NULL,0},	"<password> - your password. Optional.",1},
        {"-p",		1,	{get_password_handler, NULL,0},	"<password> - your password\n",1},

        {"--alias",	8,	{get_alias_handler, NULL,7},"<alias host name>[, hash] [ip ver] [if] [addr pref] [ip server[:port] <url> [host]]",1},
        {"-a",		8,	{get_alias_handler, NULL,7},"<alias host name>[, hash] [ip ver] [if] [addr pref] [ip server[:port] <url> [host]]\n",1 },

        {"--debug",     1,	{get_debug_level_handler, NULL,0}, "<#> - debug level 0..7; higher number, more log debug messages.",1},
        {"-d",          1,	{get_debug_level_handler, NULL,0}, "<#> - debug level 0..7; higher number, more log debug messages.\n",1},

		{"--set_private_ipv6",	1,	{get_private_ipv6_addr, NULL,0}, "<#> - Your set private IPV6 addr. It's update to DDNS service.\n",1},

        /*
        for help display only -- service_main takes care of these handlers

        could have a servie_main_display_help() function instead
        */

#ifdef _WIN32
        {"-i",3,{NULL, NULL,3},"[quoted service description] [-n <service name>] - install service\n",1},

        {"-s",2,{NULL, NULL,2},"[-n <service name>] - start service\n",1},

        {"-e",2,{NULL, NULL,2},"[-n <service name>] - exit service\n",1},

        {"-r",2,{NULL, NULL,2},"[-n <service name>] - restart service\n",1},

        {"-x",2,{NULL, NULL,2},"[-n <service name>] - uninstall service\n",1},
#endif


        {DYNDNS_INPUT_FILE_OPT_STRING,	1,	{get_options_from_file_handler, NULL,0}, "<path/file>\n\t\t\tFile containing [further]"
	" inadyn options.  The default config file, \n"
	"\t\t\t " DYNDNS_MT_DEFAULT_CONFIG_FILE " is used if inadyn is called without any cmd line\n"
	"\t\t\toptions.  Input file options are inserted at point of this option's \n"
	"\t\t\tappearance.\n",1},

        {"--ip_server_name",		2,	{get_ip_server_name_handler, NULL,0},
         "<srv_name[:port] url>\n"\
	"\t\t\tLocal IP is detected by parsing the response after returned by this \n"
	"\t\t\tserver and URL.  The first IP in found http response is considered 'my IP'.\n",1},

        {"--ip_server_host",            1,      {get_ip_server_host_handler, NULL,0},
	 "<host name>\n"\
	"\t\t\tHost name used in http request to ip server if differnt from ip server name.\n"
	"\t\t\tDefaults to ip server name.\n",1},

        {"--ip_server_name_global",     2,      {get_global_ip_server_name_handler, NULL,0},
         "<srv_name[:port] local_url> - local IP is detected by parsing the response after returned by this server and URL.  "\
         "The first IP in found http response is considered 'my IP'.  Same as ip_server_name option, but overrides unchanged dyndns_system default ip servers."},

        {"--online_check_url",		2,	{get_online_check_url_handler, NULL,0},
         "<srv_name[:port] url>\n"
	"\t\t\tURL to reach to confirm online status.  Default value:  " DYNDNS_MY_ONLINE_CHECK " /\n",1},

        {"--dyndns_server_name", 	1,	{get_dns_server_name_handler, NULL,0},
         "<NAME>[:port]\n"
	"\t\t\tThe server that receives the update DNS request.  Allows the use of unknown DNS\n"
	"\t\t\tservices that accept HTTP updates.  If no proxy is wanted, then it is enough to\n"
	"\t\t\tset the dyndns system. The default servers will be taken.\n",1},

        {"--dyndns_server_url", 	1, {get_dns_server_url_handler, NULL,0},
         "<name>\n"\
	"\t\t\tFull URL relative to DynDNS server root.  Ex: /some_script.php?hostname=\n",1},

        {"--dyndns_server_host",        1,      {get_dns_server_host_handler, NULL,0},
         "<host name>\n"\
	"\t\t\tHost name used in http update DNS request, if different from dyndns server name.\n"
	"\t\t\tDefaults to dyndns server name.\n",1},

        {"--dyndns_system",		1,	{get_dyndns_system_handler, NULL,0},
         "[name]\n"
	"\t\t\tDynamic DNS service. SHOULD be one of the following: \n"
         "\t\t\t-For dyndns.org:		dyndns@dyndns.org OR statdns@dyndns.org OR customdns@dyndns.org.\n"
         "\t\t\t-For freedns.afraid.org:	default@freedns.afraid.org\n"
         "\t\t\t-For zoneedit.com:		default@zoneedit.com\n"
         "\t\t\t-For no-ip.com:			default@no-ip.com\n"
         "\t\t\t-For easydns.com:		default@easydns.com\n"
         "\t\t\t-For 3322.org:			dyndns@3322.org\n"
         "\t\t\t-For sitelutions.com:		default@sitelutions.com\n"
         "\t\t\t-For dnsomatic.com:		default@dnsomatic.com\n"
         "\t\t\t-For tunnelbroker.net:		ipv6tb@he.net\n"
         "\t\t\t-For tzo.com:			default@tzo.com\n"
         "\t\t\t-For dynsip.org:		default@dynsip.org\n"
         "\t\t\t-For dhis.org:			default@dhis.org\n"
         "\t\t\t-For zerigo.com:		default@zerigo.com\n"
         "\t\t\t-For two-dns.de:		default@two-dns.de\n"
         "\t\t\t-For www.dnsdynamic.org:	default@dnsdynamic.org\n"
         "\t\t\t-For dnspark.com:		default@dnspark.com\n"
         "\t\t\t-For regfish.de:		ipv4@regfish.de\n"
         "\t\t\t-For regfish.de:		ipv6@regfish.de\n"
         "\t\t\t-For www.ovh.com:		default@ovh.com\n"
         "\t\t\t-For joker.com:			default@joker.com\n"
         "\t\t\t-For dyndns.strato.com:		default@strato.com\n"
         "\t\t\t-For system-ns.com:		default@system-ns.com\n"
	 "\t\t\t-For www.dtdns.com:		default@dtdns.com\n"
	 "\t\t\t-For changeip.com:		default@changeip.com\n"
	 "\t\t\t-For dnsexit.com:		default@dnsexit.com\n"
	 "\t\t\t-For nsupdate.info:		ipv4@nsupdate.info\n"
	 "\t\t\t-For nsupdate.info:		ipv6@nsupdate.info\n"
	 "\t\t\t-For loopia.com:		default@loopia.com\n"
	 "\t\t\t-For domains.google.com:	default@domains.google.com (requires SSL bridge, like stunnel)\n"
	 "\t\t\t-For duckdns.org:		default@duckdns.org\n"
	 "\t\t\t-For dy.fi:			default@dy.fi\n"
         "\t\t\t-For generic:			custom@http_svr_basic_auth\n"
         "\t\t\tDEFAULT value:			dyndns@dyndns.org\n",1},

        {"--proxy_server",		1,	{get_proxy_server_handler, NULL,0},
	 "[NAME[:port]]\n"
	 "\t\t\tThe http proxy server name and port. Default is none.\n",1},

        {"--update_period",		1,	{get_update_period_handler, NULL,0},
	 "<#>\n"
	 "\t\t\tHow often the IP is checked. The period is in [ms]. 30000..864000000.  Default\n"
	 "\t\t\tis about 10 min. Max is 10 days\n",1},

        {"--update_period_sec",		1,	{get_update_period_sec_handler, NULL,0},
	 "<#>\n"
	 "\t\t\tHow often the IP is checked. The period is in [sec]. 30..864000.  Default is about \n"
	 "\t\t\t10 min. Max is 10 days.\n",1},

        {"--forced_update_period", 	1,	{get_forced_update_period_handler, NULL,0},
	 "<#>\n"
	 "\t\t\tHow often, in seconds, the IP is updated even if it is not changed. 30 sec..30 \n"
	 "\t\t\tdays, default, 30 days.\n",1},

#ifdef USE_THREADS

	{"--forced_update_adjust",	1,	{get_forced_update_adjust_handler, NULL,0},
	 "<#>\n"
	 "\t\t\tFine timer control.  Slow, or speed timer between -4..5.  Default, 0.\n",1},
#endif

        {"--log_file",			1,	{get_logfile_name, NULL,0},
	 "<path/file>\n"
	 "\t\t\tLog file path and name.\n",1},

        {"--pid_file",			1,	{get_pidfile_name, NULL,0},
	 "<path/file>\n"
	 "\t\t\tPID file path and name.\n",1},

        {"--background",		0,	{set_silent_handler, NULL,0},
	 "\n\t\t\tRun in background. output to log file or to syslog.\n",1},

	{"--verbose",			1,	{set_verbose_handler, NULL,0},
	 "<#>\n"
	 "\t\t\tSet dbg level. 0 to 5.\n",1},

	{"--iterations",		1,	{set_iterations_handler, NULL,0},
	 "<#>\n"
	 "\t\t\tSet the number of DNS updates. Default is 0, which means infinity.\n",1},

        {"--syslog",			0,	{set_syslog_handler, NULL,0},
	 "\n\t\t\tForce logging to syslog . (e.g. /var/log/messages). Works on **NIX systems only.\n",1},

        {"--change_persona",		1,	{set_change_persona_handler, NULL,0},
	 "<uid[:gid]>\n"
	 "\t\t\tAfter init switch to a new user/group. Parameters: <uid[:gid]> to change to. \n"
	 "\t\t\tWorks on **NIX systems only.\n",1},

        {"--version",			0,	{print_version_handler, NULL,0},
	 "\n\t\t\tPrint the version number.\n",1},

        {"--exec",			1,	{get_exec_handler, NULL,0},
	 "<command>\n"
	 "\t\t\tExternal command to exec after an IP update. Include the full path.\n",1},

        {"--cache_dir",			1,	{get_cache_dir_handler, NULL,0},
	 "<path>\n"
	 "\t\t\tCache directory name. (e.g. /tmp/ddns). Defaults to /tmp on **NIX systems.\n",1},

        {"--wildcard",			0,	{wildcard_handler, NULL,0},
	 "\n\t\t\tEnable domain wildcarding for dyndns.org, 3322.org, or easydns.com.\n",1},

        {"--retries",			1,	{get_retries_handler, NULL,0},
	 "<#>\n"
	 "\t\t\tNetwork comm retry attempts.  0 to 100, default 0\n",1},

        {"--retry_interval",		1, 	{get_retry_interval_handler, NULL,0},
	 "<#>\n"
	 "\t\t\tNetwork comm miliseconds retry interval.  0 to 30,000, default 1,000\n",1},

	{"--retry_pending",		0,	{get_retry_pending_off_handler, NULL,0},
	 "<#>\n"
	 "\t\t\tRetry ip update even after network comm retries exhausted,  default on.\n",1},

	{"--retry_pending_interval",	1,	{get_retry_pending_interval_handler, NULL,0},
	 "<#>\n"
	 "\t\t\tNetwork comm seconds update retry interval, after retries exhausted.  5..3600, \n"
	 "\t\t\tdefault 300.\n",1},

        {"--lang_file",			1,	{get_lang_file_handler, NULL,1},
	 "[path/file]\n"
	 "\t\t\tLanguage file path, and file name, defaults to either ../inadyn-mt/lang/en.lng, or \n"
	 "\t\t\tetc/inadyn-mt/en.lng.  Empty parameter option gives hard coded english defaults.\n",1},
#ifdef USE_SNDFILE

	{"--audible",			0,	{get_audible_on_handler, NULL,0},
	 "Audible network status alerts toggle.  default off.\n",1},

	{"--wave_file",			2,	{get_wave_file_handler, NULL,1},
	 "<path/file> [#]\n"
	 "\t\t\tAudible network status alerts wave file path, and file name. defaults\n"
         "\t\t\tto either ../inadyn-mt/extra/wav/alarm.wav, or etc/inadyn-mt/extra/wav/alarm.wav \n"
         "\t\t\t[wave loops: 0..100; -1 for infinite] same as wave_loops below -- optional number of\n"
         "\t\t\ttimes per wave play call to repeat wave file play, default 0.\n",1},

	{"--wave_loops",		1,	{get_wave_loops_handler, NULL,0},
	 "<#>\n"
	 "\t\t\tSame as wave_file parameter optional parameter -- 0..100; -1 for infinite number of \n"
	 "\t\t\ttimes per wave play call to repeat wave file play, default 0.\n",1},

	{"--alert_retries",		1,	{get_alert_retries_handler, NULL,0},
	 "<#>\n"
	 "\t\t\tNetwork retries before audible network status alerts. [0..100], default 0.\n",1},

	{"--alert_interval",		1,	{get_alert_interval_handler, NULL,0},
	 "<#>\n"
	 "\t\t\tTime in miliseconds between consecutive audible network status alerts. [0..3600000], \n"
	 "\t\t\tdefault 0.\n",1},

	{"--wave_gain",			1,	{get_wave_gain_handler, NULL,0},
	 "<#>\n"
	 "\t\t\tGain (amplitude adjust) at which to play audible alert (beware clipping), integer \n"
	 "\t\t\tor float.  [-10..10], default 10 (0db, no attenuation).\n",1},

	{"--wave_buff",			1,	{get_wave_buff_handler, NULL,0},
	 "<#>\n"
	 "\t\t\tWave file output buffer size control -- integer or float multiple of wave file bytes \n"
	 "\t\t\tper sec. [.25..10], default .25.\n",1},

/*possible other options:
		-time before go into rest period
		-time to rest
		-time of day block out period
		-for dial-up, don't test connect
		'til after dail-up
		-for dial-up, don't alert if
		user terminated connection
		-status check at random intervals over
		a specified range
		-alert sounding decay functions options

*/

#ifdef USE_THREADS

	{"--status_interval",		1,	{get_status_interval_handler, NULL,0},
	"<#>\n"
	 "\t\t\tSeconds [30..864000] interval at which to check online status.  defaults to 600.\n",1},

	{"--status_offline_interval",	1,	{get_status_offline_interval_handler, NULL,0},
	 "<#>\n"
	 "\t\t\tSeconds [0..864000] interval at which to check online status, after offline detected. \n"
	 "\t\t\tdefaults to 15\n",1},
#endif
#endif
        {"--svr_ip_param",		3,	{get_ip_param_handler, NULL,2},
	 "<custom DDNS server ip name parameter> [user] [pass]\n"
	 "\t\t\tCustom DDNS url parameter for ip address, e.g., myip.\n",1},

        {"--svr_rsp_success",		1,	{get_success_rsp_params_handler, NULL,0},
	 "<custom DDNS server update success return code>\n"
	 "\t\t\tA quoted comma delimted string,e.g., \"good, nochg\", representing custom DDNS server \n"
	 "\t\t\tsuccessful update return values.\n",1},

        {"--svr_rsp_other",		1,	{get_config_rsp_params_handler, NULL,0},
	 "<custom DDNS server non-fatal failure return codes>\n"
	 "\t\t\tA quoted comma delimited string, e.g., \"dnserr,911,unknown error\", representing custom \n"
	 "\t\t\tDDNS server non-fatal update failure return values.\n",1},

	{"--svr_ip_in_rsp",		0,	{get_ip_in_svr_rsp_handler, NULL,0},
	 "\n\t\t\tFlag that custom DDNS server returns your ip address on update success.  Defaults to false.\n",1},

        {"--svr_auto_dual",             0,      {get_auto_dual_handler, NULL,0},
         "\n\t\t\tFlag that server defaults to dual list (coupled) alias ip type.  Defaults to false.\n",1},

	{"--svr_auto_detect",		0,	{get_auto_detect_handler, NULL,0},
	 "\n\t\t\tFlag server auto detects ip.  Defaults to false.\n",1},

	{"--svr_add_cfg",		1,	{get_svr_add_cfg_handler, NULL,0},
	 "<full path, file name>\n"
	 "\t\t\tAdditional servers file.  Default search for servers_additional.cfg in ./extra, /etc, or \n"
	 "\t\t\t/etc/inadyn-mt.\n",1},

	{"--if",			1,	{get_if_name_handler, NULL,0},
	 "<net interface name>\n"
	 "\t\t\tNetwork interface.  Defaults to system configured default.\n",1},

        {"--addr_pref",			1,	{get_addr_pref_handler, NULL,0},
	 "<addr pref>\n"
	 "\t\t\tIPV6 privacy extensions address preference.  Value is one of:  pub, policy, home, careof\n"
	 "\t\t\tcrypto, nocrypto, tmp, pubtmp.  Logical OR not presently supported.  Defaults to pub.\n",1},

	{"--if_enum",			0,	{enum_interface_handler, NULL,0},
	"\n\t\t\tEnumerate network interfaces.\n",1},

	{"--improb",			0,	{get_improb_handler, NULL,0}, "????",0},
	
	
	

        {NULL,				0,	{0,NULL,0},"",0}
};

void print_help_page(void)
{
	CMD_DESCRIPTION_TYPE *it = cmd_options_table;

	printf("SYNOPSIS\n\n"
		"inadyn-mt\t[-h | --help] [-u | --username username] [-p | -- password password] [-a | --alias \n"\
		"\t\talias[,hash] [dual | coupled | ip4 | ip6 | auto] [if] [pub | policy | home | careof\n"\
		"\t\t| crypto | nocrypto | tmp | pubtmp] [ip server[:port] <ip server url>> [ip  server  \n"\
		"\t\thost]] [--input_file  path/name]  [--ip_server_name  server[:port]  url]  [--ip_server_host\n"\
		"\t\thostname]  [--dyndns_server_name server[:port]] [--dyndns_server_url url] [--dyndns_system \n"\
		"\t\tservice] [--proxy_server[:port] proxy[:port]]  [--update_period  ms]  [--update_period_sec\n"\
		"\t\tsec]  [--forced_update_period  sec]  [--log_file  path/name]  [--background]  [--verbose  \n"\
		"\t\tlevel] [--iterations iterations] [--syslog] [--change_persona uid[:gid]]\n\n");

	printf("DESCRIPTION\n\n"\
		"\t\tinadyn-mt is a client for \"open\" name servers. That is, it allows the user's machine to have\n"\
		"\t\tan internet recognized name by transferring  some  control  over a name server not managed by \n"\
		"\t\tthe user. Some of the services that are offered by those open name servers are free of charge, \n"\
		"\t\tand work even when the client machine does not have a static IP.  Supports both IPv4, and IPv6 \n"\
		"\t\taddresses.\n\n");

	while( it->p_option != NULL) {

		if (it->is_show_help)

			printf("\t'%s': %s\n\r",it->p_option, it->p_description);

		++it;
	}

	printf("\n\n\n");
}

static RC_TYPE get_pidfile_name(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT *p_self = (DYN_DNS_CLIENT *) p_context;

	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	return set_file_param2(&p_self->p_pidfilename,p_cmd->argv[current_nr]);
}

static char *do_arg_to_val(struct EXP_MAPPING *p_mapping,char *identifier)
{
	char    		*p_ret=safe_malloc(strlen("unknown identifier")+1);
	int			alias_index=p_mapping->alias_index;
	DYNDNS_INFO_TYPE	info=p_mapping->p_dyndns->info[p_mapping->server_index];

	strcpy(p_ret,"unknown identifier");

	if (!(identifier))

		return p_ret;

	DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "mapping from identifier, %s, in do_arg_to_val...\n",identifier));

	if (!(strcmp(identifier,"IP_ADDR"))) {

		if (!(info.alias_info.p_address[alias_index]->my_ip_address.name[ip_store]))

			return p_ret;

		if (!(strlen(info.alias_info.p_address[alias_index]->my_ip_address.name[ip_store])))

			return p_ret;

		p_ret=safe_realloc(p_ret,strlen(info.alias_info.p_address[alias_index]->my_ip_address.name[ip_store])+1);
		strcpy(p_ret,info.alias_info.p_address[alias_index]->my_ip_address.name[ip_store]);

		p_mapping->is_ip_addr=1;
	}

	if (!(strcmp(identifier,"IP_ADDR4"))) {

		if (!(info.alias_info.p_address[alias_index]->my_ip_address.name[ip_4])) {

			free(p_ret);

			return safe_malloc(1);
		}

		if (!(strlen(info.alias_info.p_address[alias_index]->my_ip_address.name[ip_4]))) {

			free(p_ret);

			return safe_malloc(1);
		}

		p_ret=safe_realloc(p_ret,strlen(info.alias_info.p_address[alias_index]->my_ip_address.name[ip_4])+1);
		strcpy(p_ret,info.alias_info.p_address[alias_index]->my_ip_address.name[ip_4]);

		p_mapping->is_ip_addr=1;
	}

	if (!(strcmp(identifier,"IP_ADDR6"))) {

		if (!(info.alias_info.p_address[alias_index]->my_ip_address.name[ip_6])) {

			free(p_ret);

			return safe_malloc(1);
		}

		if (!(strlen(info.alias_info.p_address[alias_index]->my_ip_address.name[ip_6]))) {

			free(p_ret);

			return safe_malloc(1);
		}

		p_ret=safe_realloc(p_ret,strlen(info.alias_info.p_address[alias_index]->my_ip_address.name[ip_6])+1);
		strcpy(p_ret,info.alias_info.p_address[alias_index]->my_ip_address.name[ip_6]);

		p_mapping->is_ip_addr=1;
	}

	if (!(strcmp(identifier,"ALIAS"))) {

		if (!(alias_index<info.alias_info.count))

			return p_ret;

		p_ret=safe_realloc(p_ret,strlen(info.alias_info.names[alias_index].name)+1);
		strcpy(p_ret,info.alias_info.names[alias_index].name);

		p_mapping->is_alias=1;
	}

	if (!(strcmp(identifier,"USERNAME"))) {

		p_ret=safe_realloc(p_ret,strlen(info.credentials.my_username)+1);
		strcpy(p_ret,info.credentials.my_username);

		p_mapping->is_username=1;
	}

	if (!(strcmp(identifier,"PASSWORD"))) {

		p_ret=safe_realloc(p_ret,strlen(info.credentials.my_password)+1);
		strcpy(p_ret,info.credentials.my_password);

		p_mapping->is_password=1;
	}

	return p_ret;
}

char *arg_to_val(void *p_context,char *token)
{
	struct EXP_MAPPING      *p_mapping=p_context;
	char                    *s_ret=do_arg_to_val(p_mapping,token);

	DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "mapping value from identifier %s, to value %s, in arg_to_val...\n",token,s_ret));

	return s_ret;
}

static RC_TYPE help_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT *p_self = (DYN_DNS_CLIENT *) p_context;
	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}
	p_self->abort = TRUE;
	print_help_page();
	return RC_OK;
}

static RC_TYPE wildcard_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT *p_self = (DYN_DNS_CLIENT *) p_context;

	if (p_self == NULL) {

		return RC_INVALID_POINTER;
	}

	p_self->info[server_index].wildcard = TRUE;

	return RC_OK;
}

static RC_TYPE set_verbose_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT *p_self = (DYN_DNS_CLIENT *) p_context;
	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	if (sscanf(p_cmd->argv[current_nr], "%d", &p_self->dbg.level) != 1)
	{
		return RC_DYNDNS_INVALID_OPTION;
	}
	return RC_OK;
}

static RC_TYPE set_iterations_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT *p_self = (DYN_DNS_CLIENT *) p_context;
	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	if (sscanf(p_cmd->argv[current_nr], "%d", &p_self->total_iterations) != 1)
	{
		return RC_DYNDNS_INVALID_OPTION;
	}

	p_self->total_iterations = (p_self->sleep_sec < 0) ?  DYNDNS_DEFAULT_ITERATIONS : p_self->total_iterations;
	return RC_OK;
}

static RC_TYPE set_silent_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT *p_self = (DYN_DNS_CLIENT *) p_context;
	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	p_self->run_in_background = TRUE;
	return RC_OK;
}

/*extract path, filename from file_name, and store in dest*/
static RC_TYPE do_set_file_param2(char **dest,char *file_name,int is_create)
{
	char	*path=NULL;
	char	*name;
	RC_TYPE	rc;

	/*if no path in file_name, set path
	  to cwd, or module path*/
	PATH_and_filename(&path,&name,file_name);

	rc=do_set_file_param(dest,path,name,is_create);

	free(path);
	free(name);

	return rc;
}

static RC_TYPE set_file_param2(char **dest,char *file_name)
{

	return do_set_file_param2(dest,file_name,1);
}

static RC_TYPE get_logfile_name(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT *p_self = (DYN_DNS_CLIENT *) p_context;

	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	free(p_self->dbg.p_logfilename);

	return set_file_param2(&p_self->dbg.p_logfilename,p_cmd->argv[current_nr]);
}

static RC_TYPE get_username_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT	*p_self = (DYN_DNS_CLIENT *) p_context;


	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	if (p_self->info[server_index].credentials.my_username) {

		free(p_self->info[server_index].credentials.my_username);
	}

	/*user*/
	p_self->info[server_index].credentials.my_username=safe_malloc(strlen(p_cmd->argv[current_nr])+1);

	strcpy(p_self->info[server_index].credentials.my_username, p_cmd->argv[current_nr]);

	return RC_OK;
}

static RC_TYPE get_password_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT *p_self = (DYN_DNS_CLIENT *) p_context;
	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	if (p_self->info[server_index].credentials.my_password) {

		free(p_self->info[server_index].credentials.my_password);
	}

	/*password*/
	p_self->info[server_index].credentials.my_password=safe_malloc(strlen(p_cmd->argv[current_nr])+1);
	strcpy(p_self->info[server_index].credentials.my_password, (p_cmd->argv[current_nr]));
	return RC_OK;
}

static int is_alias_ip_type(char *token)
{

	return (0==strcmp("ip4",token) || 0==strcmp("ip6",token) || 0==strcmp("dual",token) || 0==strcmp(DUAL_LIST,token) || 0==strcmp("auto",token));
}

static IP_ADDRESS* address_list_add(gen_list *address_list,char *i_face,char *ip_ver,IPV6_PRIV_EXT *addr_pref,char *ip_server,char *ip_server_url,
				    int port,char *ip_proxy_name,int ip_proxy_port,char *ip_server_host)
{

	return iface_list_add_address(address_list,i_face,ip_ver,addr_pref,ip_server,ip_server_url,port,ip_server_host,ip_proxy_name,ip_proxy_port);
}

static int is_interface_name(char *iface,struct ifaddrs_w *ifaddrs)
{
	struct ifaddrs_w	*p_ifaddrs;
	int			ret=0;


	DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "testing \"%s\" in is_interface_name...\n",iface));

	for (p_ifaddrs=ifaddrs;p_ifaddrs;p_ifaddrs=p_ifaddrs->ifa_next) {

		if (0==(strcmp(iface,p_ifaddrs->ifa_name))) {

			ret=1;

			DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "matched \"%s\" as interface name in is_interface_name...\n",p_ifaddrs->ifa_name));

			break;
		}
	}

	return ret;
}

static RC_TYPE enum_interface_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT		*p_self=p_context;
	struct ifaddrs_w	*p_ifaddrs;

	printf("%sS:" MODULE_TAG "Network interfaces:\n",print_time());

	for (p_ifaddrs=p_self->ifaddrs;p_ifaddrs;p_ifaddrs=p_ifaddrs->ifa_next)

		printf("%s\n",p_ifaddrs->ifa_name);

	p_self->abort=TRUE;

	return RC_OK;
}

/*get command line alias option parameters.  set defaults -
  at minimum, set priv_ext (startup default), and ip server.

  if no ip server, interface, address pref parameter, use that
  given by option --ip_server_name --if, --addr_pref respectively.

  if still no ip server, use default for DDNS system.
*/
static RC_TYPE get_alias_params(DYN_DNS_CLIENT *p_self,CMD_DATA *p_cmd,int current_nr,ALIAS_PARAMS *p_params)
{
	RC_TYPE		rc=RC_OK;
	int		port_param=HTTP_DEFAULT_PORT;

	do {

		if (!(current_nr<p_cmd->argc))

			break;

		if ((0==strncmp(p_cmd->argv[current_nr],"--",2)))

			break;

		/*interface*/
		if (!(is_interface_name(p_cmd->argv[current_nr],p_self->ifaddrs))) {

			DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "\"%s\" not a network interface.  Assuming alias interface parameter for alias %s, omitted in get_alias_params...\n",
					p_cmd->argv[current_nr],p_self->info[server_index].alias_info.names[p_self->info[server_index].alias_info.count-1].name));
		}
		else {
			do_get_if_name_handler(&p_params->iface,p_cmd->argv[current_nr]);

			current_nr++;
		}

		if (!(current_nr<p_cmd->argc))

			break;

		if ((0==strncmp(p_cmd->argv[current_nr],"--",2)))

			break;

		/*addr_pref*/
		if (RC_OK==set_priv_ext(p_cmd->argv[current_nr],&p_params->priv_ext))

			current_nr++;

		if (!(current_nr<p_cmd->argc))

			break;

		if ((0==strncmp(p_cmd->argv[current_nr],"--",2)))

			break;

		/*ip server name, url, port*/
		if (RC_OK==do_set_url_param(p_cmd,&current_nr,&p_params->ip_server_name,&p_params->ip_server_url,&port_param)) {

			if (!(-1==port_param))

				p_params->ip_server_port=port_param;

			current_nr++;
		}

		if (!(current_nr<p_cmd->argc))

			break;

		if ((0==strncmp(p_cmd->argv[current_nr],"--",2)))

			break;

		/*ip server host name*/
		do_get_ip_server_host_handler(p_cmd,current_nr,&p_params->ip_server_host);

	} while (0);


	return rc;
}

static void free_alias_params(ALIAS_PARAMS **p_alias_params)
{

	free((*p_alias_params)->iface);
	free((*p_alias_params)->ip_server_host);
	free((*p_alias_params)->ip_server_name);
	free((*p_alias_params)->ip_server_url);
	free((*p_alias_params)->ip_proxy_name);
	free((*p_alias_params)->ip_ver_str);
	free((*p_alias_params)->priv_ext);

	free(*p_alias_params);
	*p_alias_params=NULL;
}

static void free_alias_params_ar(ALIAS_PARAMS ***p_alias_params,int alias_params_count)
{

	while (alias_params_count) {

		free_alias_params(&(*p_alias_params)[alias_params_count-1]);

		alias_params_count--;

		*p_alias_params=safe_realloc(*p_alias_params,sizeof(ALIAS_PARAMS* )*(alias_params_count));
	}
}

static void free_alias_params_ars(DYNDNS_INFO_TYPE *info,int dyndns_info_count)
{
	int	i;


	for (i=0;i<dyndns_info_count;i++)

		free_alias_params_ar(&info[i].alias_info.p_params,info[i].alias_info.count);
}

static int alias_short_index_of(DYNDNS_ALIAS_INFO alias_info,char *alias)
{
	int	i;

	for (i=0;i<alias_info.count;i++)

		if (0==strcmp(alias,alias_info.names[i].name))

			return i;

	return -1;
}

static int alias_index_of(DYNDNS_ALIAS_INFO alias_info,char *alias,char *ip_ver_str)
{
	int	i;

	for (i=0;i<alias_info.count;i++)

		if (0==strcmp(alias,alias_info.names[i].name))

			if (0==strcmp(ip_ver_str,alias_info.names[i].ip_v))

				return i;

	return -1;
}

static void delete_alias(DYNDNS_ALIAS_INFO *alias,int alias_index,int count)
{
	int	i;


	free(alias->names[alias_index].name);
	alias->names[alias_index].name=NULL;

	free_alias_params(&alias->p_params[alias_index]);

	if (!(alias_index==(count-1)))

		alias->p_params[alias_index]=safe_malloc(sizeof(ALIAS_PARAMS));

	for (i=alias_index;i<count-1;i++) {

		memcpy(&alias->names[i],&alias->names[i+1],sizeof(DYNDNS_ALIAS_NAME));
		memcpy(alias->p_params[i],alias->p_params[i+1],sizeof(ALIAS_PARAMS));
	}

	alias->p_params=safe_realloc(alias->p_params,sizeof(ALIAS_PARAMS *)*(count-1));
}

/**
    Parses alias,hash ip_update_type.
    Example: blabla.domain.com,hashahashshahah
    Action:
	-search by ',' and replace the ',' with 0
	-read hash and alias, and ip update type
	01/23/11 -- added ip update type facility (bhoover@wecs.com)
				and side effect of setting related DYN_DNS_CLIENT,
				DYN_DNS_INFO address type properties.
*/
static RC_TYPE do_get_alias_handler(CMD_DATA *p_cmd, int current_nr, void *p_context,char *ip_ver_str,int ip_ver_offset,int is_dual)
{
	DYN_DNS_CLIENT		*p_self=(DYN_DNS_CLIENT *) p_context;
	char			*p_hash=NULL;
	int			alias_len=0;
	ALIAS_PARAMS		***p_params;
	int			alias_index;

	if (p_self == NULL)	{

		return RC_INVALID_POINTER;
	}

	if (p_self->info[server_index].alias_info.count >= DYNDNS_MAX_ALIAS_NUMBER) {

		return RC_DYNDNS_TOO_MANY_ALIASES;
	}

	/*hash?*/
	p_hash = strstr(p_cmd->argv[current_nr],",");

	if (p_hash) {

		alias_len=strlen(p_hash);

		/*hash space delimited?*/
		if (!(0==strcmp(",",p_hash)))

			p_hash++;
		else
			p_hash=p_cmd->argv[current_nr+1];

		if (p_self->info[server_index].alias_info.hashes[p_self->info[server_index].alias_info.count].str)

			free(p_self->info[server_index].alias_info.hashes[p_self->info[server_index].alias_info.count].str);

		/*allocate and copy hash len minus comma*/
		p_self->info[server_index].alias_info.hashes[p_self->info[server_index].alias_info.count].str
			=safe_malloc(strlen(p_hash)+1);

		strcpy(p_self->info[server_index].alias_info.hashes[p_self->info[server_index].alias_info.count].str,(p_hash));
	}

	/*alias*/

	alias_len=strlen(p_cmd->argv[current_nr])-alias_len;
	p_cmd->argv[current_nr][alias_len]='\0';

	p_params=&p_self->info[server_index].alias_info.p_params;

/*Dual stack aliases can be of type 'dual', 'coupled', or simply entered twice - once each qualified with ip type, ip4, ip6.
  The logic to accomodate command line (or config file, or windows registry parameters) "recalling" an alias (to change its
  type or some other of its parameters), is a tad knarly, but not too terribly bad.

  Type 'dual' is like twice singularly entered, ip type (ip4, ip6) qaualified aliases, but does not require user enter the alias
  once for each ip type.

  Type 'coupled' is like 'dual', but constitues a single alias entry as the update is done in a single DDNS server call, with
  ip4, ip6 addresses given as a comma delimited list.
*/
	/*recall the alias, if it's already been entered*/
	alias_index=alias_short_index_of(p_self->info[server_index].alias_info,p_cmd->argv[current_nr]);

	/*if type changing from "dual", need to delete one of them*/
	if (!(-1==alias_index) && (!(is_dual) && p_self->info[server_index].alias_info.names[alias_index].is_dual)) {

		/*dual alias type is given us in two consecutive calls - type ip4, ip6 respecively - delete one*/

		delete_alias(&p_self->info[server_index].alias_info,alias_index+1,p_self->info[server_index].alias_info.count);

		p_self->info[server_index].alias_info.count--;
	}
	else {
		int	is_new_alias=1;

		if (!(-1==alias_index))

			/*changing to coupled or auto?*/
			if ((is_new_alias=(!((0==strcmp("coupled",ip_ver_str)) || (0==strcmp("auto",ip_ver_str))))))

				/*changing from coupled or auto?*/
				if ((is_new_alias=(!((0==strcmp("auto",p_self->info[server_index].alias_info.names[alias_index].ip_v)) || 
						     (0==strcmp("coupled",p_self->info[server_index].alias_info.names[alias_index].ip_v))))))

					/*adding ip4, or ip6 (singularly, or as part of type dual)?*/
					if ((is_new_alias=(-1==(alias_index=alias_index_of(p_self->info[server_index].alias_info,p_cmd->argv[current_nr],ip_ver_str))))) {

						int		other_dual_index;

						/*coerce separately entered ip4, ip6 pairs to dual*/
						is_dual=1;

						/*find the other one, and set it's is_dual flag*/
						if (0==strcmp("ip4",ip_ver_str))

							other_dual_index=alias_index_of(p_self->info[server_index].alias_info,p_cmd->argv[current_nr],"ip6");
						else
							other_dual_index=alias_index_of(p_self->info[server_index].alias_info,p_cmd->argv[current_nr],"ip4");

						p_self->info[server_index].alias_info.names[other_dual_index].is_dual=1;
					}

		if (is_new_alias) {

			alias_index=p_self->info[server_index].alias_info.count;

			*p_params=safe_realloc(*p_params,sizeof(ALIAS_PARAMS *)*(p_self->info[server_index].alias_info.count+1));
			(*p_params)[alias_index]=safe_malloc(sizeof(ALIAS_PARAMS));

			/*alias*/
			p_self->info[server_index].alias_info.names[alias_index].name
				=safe_malloc(alias_len+1);
			strcpy(p_self->info[server_index].alias_info.names[alias_index].name,p_cmd->argv[current_nr]);

			p_self->info[server_index].alias_info.count++;
		}
	}

	p_self->info[server_index].alias_info.names[alias_index].is_dual=is_dual;

	(*p_params)[alias_index]->ip_ver_str=safe_realloc((*p_params)[alias_index]->ip_ver_str,strlen(ip_ver_str)+1);
	strcpy((*p_params)[alias_index]->ip_ver_str,ip_ver_str);

	/*default to letting ip server determine update type*/
	strcpy(p_self->info[server_index].alias_info.names[alias_index].ip_v,"auto");

	p_self->info[server_index].alias_info.names[alias_index].ip_v_enum=ip_store;

	current_nr+=ip_ver_offset;

	/*IPv/type*/
	if (current_nr<p_cmd->argc)

		if (!(0==strncmp(p_cmd->argv[current_nr],"--",2)))

			if (is_alias_ip_type(p_cmd->argv[current_nr])) /*command line entered*/

				current_nr++;

	strcpy(p_self->info[server_index].alias_info.names[alias_index].ip_v,ip_ver_str);

	if (!(0==strcmp(ip_ver_str,"auto"))) {

		if (!(0==strcmp(ip_ver_str,DUAL_LIST))) {

			if (!(0==strcmp(ip_ver_str,"ip4")))

				p_self->info[server_index].alias_info.names[alias_index].ip_v_enum=ip_6;
			else
				p_self->info[server_index].alias_info.names[alias_index].ip_v_enum=ip_4;
		}
	}

	/*defaults, additional alias parameters*/
	return get_alias_params(p_self,p_cmd,current_nr,(*p_params)[alias_index]);
}

static RC_TYPE get_alias_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT	*p_self=(DYN_DNS_CLIENT *) p_context;
	char		ip_ver_str[8]={"auto"};
	RC_TYPE		rc=RC_OK;
	int		ip_ver_offset=1;

	/*user entered space before hash following comma?  (original inadyn back compat)*/
	if (','==p_cmd->argv[current_nr][strlen(p_cmd->argv[current_nr])-1])

		ip_ver_offset=2;

	if (current_nr+ip_ver_offset<p_cmd->argc) {

		if (!(0==strncmp(p_cmd->argv[current_nr+ip_ver_offset],"--",2))) {

			/*for ip type parameter backward compat, we require it appears first after
			  host, and (if given), hash*/
			if (is_alias_ip_type(p_cmd->argv[current_nr+ip_ver_offset])) {
#ifndef _WIN32
				if (snprintf(ip_ver_str, 8, "%s", p_cmd->argv[current_nr+ip_ver_offset]) < 0) {
#else
				if (_snprintf(ip_ver_str, 8, "%s", p_cmd->argv[current_nr+ip_ver_offset]) < 0)	{
#endif
					return RC_DYNDNS_INVALID_OPTION;
				}
			}
		}
	}

	/*server defaults to DUAL_LIST?*/
	if ((0==strcmp(ip_ver_str,"auto")) && p_self->info[server_index].is_auto_dual_list)

		strcpy(ip_ver_str,DUAL_LIST);

	if (!(0==strcmp(ip_ver_str,"dual"))) {

		return do_get_alias_handler(p_cmd,current_nr,p_context,ip_ver_str,ip_ver_offset,0);
	}
	else {

		if (!(RC_OK==(rc=do_get_alias_handler(p_cmd,current_nr,p_context,"ip4",ip_ver_offset,1))))

			return rc;

		rc=do_get_alias_handler(p_cmd,current_nr,p_context,"ip6",ip_ver_offset,1);
	}

	return rc;
}

static RC_TYPE get_name_and_port(char *p_src, char **p_dest_name, int *p_dest_port)
{
	char *p_port = NULL;


	if (*p_dest_name)

		free(*p_dest_name);

	p_port = strstr(p_src,":");


	if (p_port) {

		/*ipv6 localhost (i.e., ::1)?*/
		if (p_port==p_src) {

			p_port++;

			if (!(p_port[0]))

				return RC_DYNDNS_INVALID_OPTION;

			if (!(strstr(p_port,":")))

				return RC_DYNDNS_INVALID_OPTION;

			p_port++;

			if (!(p_port[0]))

				return RC_DYNDNS_INVALID_OPTION;

			p_port=strstr(p_port,":");
		}
	}

	if (!(p_port)) {

		*p_dest_name=safe_malloc(strlen(p_src)+1);

		strcpy(*p_dest_name, p_src);
	}
	else {

		int     port_nr, len;
		int     port_ok;

		port_ok=sscanf(p_port+1,"%d",&port_nr);

		if (port_ok != 1) {

			return RC_DYNDNS_INVALID_OPTION;
		}

		*p_dest_port = port_nr;
		len = p_port - p_src;

		*p_dest_name=safe_malloc(len+1);
		memcpy(*p_dest_name, p_src, len);

		*(*p_dest_name+len) = '\0';
	}

	return RC_OK;
}

/** Returns the svr name and port if the format is :
 * name[:port] url.
 */
static RC_TYPE do_set_url_param(CMD_DATA *p_cmd,int *current_nr,char **ip_server_name,char **ip_server_url,int *port)
{
	RC_TYPE	rc;

	/*user*/
	*port=HTTP_DEFAULT_PORT;

	if (!(RC_OK==(rc=get_name_and_port(p_cmd->argv[*current_nr],ip_server_name,port))))

		return rc;

	if (!((*current_nr)+1<p_cmd->argc))

		return RC_OK;

        if (0==(strncmp(p_cmd->argv[(*current_nr)+1],"-",1)))

		return RC_OK;

	if (p_cmd->argv[(*current_nr)+1][0]=='/') {

		(*current_nr)++;

		free(*ip_server_url);

		*ip_server_url=safe_malloc(strlen(p_cmd->argv[*current_nr])+1);

		strcpy(*ip_server_url, p_cmd->argv[*current_nr]);
	}

	return RC_OK;
}

static RC_TYPE set_url_param(CMD_DATA *p_cmd, int current_nr,DYNDNS_SERVER_NAME *info)
{
	int	port=-1;
	RC_TYPE	rc;


	if (!(info))

		return RC_INVALID_POINTER;

	if (!(RC_OK==(rc=do_set_url_param(p_cmd,&current_nr,&info->name,&info->url,&port))))

		return rc;

	info->port=HTTP_DEFAULT_PORT;

	if (RC_OK==rc && !(-1==port))

		info->port=port;

	return RC_OK;
}

static RC_TYPE get_ip_server_name_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{

    return set_url_param(p_cmd,current_nr,&(((DYN_DNS_CLIENT *) (p_context))->info[server_index].ip_server_name));
}

static RC_TYPE get_global_ip_server_name_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	RC_TYPE		rc;
	DYN_DNS_CLIENT	*p_self=(DYN_DNS_CLIENT *) p_context;
	int		i;


    if ((RC_OK==(rc=set_url_param(p_cmd,current_nr,&p_self->ip_server_name_global)))) {

		for (i=0;i<MAX_DNS_SERVER;i++) {

			if (!(p_self->info[i].ip_server_name.name)) {

				p_self->info[i].ip_server_name.name=safe_malloc(strlen(p_self->ip_server_name_global.name)+1);
				strcpy(p_self->info[i].ip_server_name.name,p_self->ip_server_name_global.name);

				p_self->info[i].ip_server_name.url=safe_malloc(strlen(p_self->ip_server_name_global.url)+1);
				strcpy(p_self->info[i].ip_server_name.url,p_self->ip_server_name_global.url);

				p_self->info[i].ip_server_name.port=p_self->ip_server_name_global.port;
			}
		}
	}

	return rc;
}

static RC_TYPE do_get_ip_server_host_handler(CMD_DATA *p_cmd, int current_nr, char **ip_server_host)
{

	if (current_nr>=p_cmd->argc)

		return RC_DYNDNS_INVALID_OPTION;

	if (!(strncmp(p_cmd->argv[current_nr],"-",1)))

		return RC_DYNDNS_INVALID_OPTION;

	free(*ip_server_host);

	*ip_server_host=safe_malloc(strlen(p_cmd->argv[current_nr])+1);
	strcpy(*ip_server_host,p_cmd->argv[current_nr]);


	return RC_OK;
}

static RC_TYPE get_ip_server_host_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT	*p_self=p_context;

	if (!(p_self))

		return RC_INVALID_POINTER;

	return do_get_ip_server_host_handler(p_cmd,current_nr,&p_self->info[server_index].ip_server_host);
}

static RC_TYPE get_online_check_url_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{

    return set_url_param(p_cmd,current_nr,&(((DYN_DNS_CLIENT *) (p_context))->info_online_status));
}

static signed char dns_system_index_of(char *p_key,DYN_DNS_CLIENT *p_self)
{

	signed char i;

	DBG_PRINTF((LOG_DEBUG,"D:" MODULE_TAG "Searching among servers, for key:  %s, in dns_system_index_of...\n",p_key));

	for (i=0;i<p_self->srv_cnt;i++) {

		DBG_PRINTF((LOG_DEBUG,"D:" MODULE_TAG "Compairing index:  %s, in dns_system_index_of...\n",p_self->info[i].p_dns_system->p_key));

		if (0==strcmp(p_key,p_self->info[i].p_dns_system->p_key)) {

			DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "Returning index of already entered server:  %s, in dns_system_index_of...\n",p_self->info[i].p_dns_system->p_key));

			return i;
		}
	}

	return -1;
}

static DYNDNS_SYSTEM *get_dns_system(char *src_key)
{
	DYNDNS_SYSTEM_INFO *it = get_dyndns_system_table();


	for (; it != NULL && it->id != LAST_DNS_SYSTEM; ++it) {

		if (strcmp(it->system.p_key, src_key) == 0) {

			return &it->system;
		}
	}

	return NULL;
}

static RC_TYPE get_dns_server_name_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT	*p_self = (DYN_DNS_CLIENT *) p_context;
	RC_TYPE		rc;
	int		port = -1;


	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	p_self->info[server_index].dyndns_server_name.port = HTTP_DEFAULT_PORT;
	rc = get_name_and_port(p_cmd->argv[current_nr], &p_self->info[server_index].dyndns_server_name.name, &port);
	if (rc==RC_OK) {

		if (port != -1) {

			p_self->info[server_index].dyndns_server_name.port = port;
		}
    }


	return rc;
}

RC_TYPE get_dns_server_url_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT	*p_self = (DYN_DNS_CLIENT *) p_context;
	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	if (p_self->info[server_index].dyndns_server_name.url)

		free(p_self->info[server_index].dyndns_server_name.url);

	p_self->info[server_index].dyndns_server_name.url=safe_malloc(strlen(p_cmd->argv[current_nr])+1);

	/*url*/
	strcpy(p_self->info[server_index].dyndns_server_name.url, p_cmd->argv[current_nr]);

	return RC_OK;
}

static RC_TYPE get_dns_server_host_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT  *p_self=p_context;


	if (p_self==NULL)

		return RC_INVALID_POINTER;

	if (current_nr>=p_cmd->argc)

		return RC_DYNDNS_INVALID_OPTION;

	if (!(strncmp(p_cmd->argv[current_nr],"-",1)))

		return RC_DYNDNS_INVALID_OPTION;

	p_self->info[server_index].dyndns_server_host=safe_realloc(p_self->info[server_index].dyndns_server_host,strlen(p_cmd->argv[current_nr])+1);
	strcpy(p_self->info[server_index].dyndns_server_host,p_cmd->argv[current_nr]);

	return RC_OK;
}

/* returns the proxy server name and port
*/
static RC_TYPE do_get_proxy_server(CMD_DATA *p_cmd, int current_nr, char **proxy_server_name,int *proxy_port)
{
	RC_TYPE		rc;
	int		port = -1;


	/*user*/
	*proxy_port=port = HTTP_DEFAULT_PORT;

	rc = get_name_and_port(p_cmd->argv[current_nr], proxy_server_name, &port);

	if (rc == RC_OK && port != -1) {

		*proxy_port = port;
	}

	return rc;
}

static RC_TYPE get_proxy_server_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT	*p_self=p_context;

	return do_get_proxy_server(p_cmd,current_nr,&p_self->info[server_index].proxy_server_name.name,
					&p_self->info[server_index].proxy_server_name.port);
}

/* Read the dyndnds name update period.
   and impose the max and min limits
*/
static RC_TYPE get_update_period_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT *p_self = (DYN_DNS_CLIENT *) p_context;


	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	if (sscanf(p_cmd->argv[current_nr], "%d", &p_self->sleep_sec) != 1)
	{
		return RC_DYNDNS_INVALID_OPTION;
	}

	p_self->sleep_sec /= 1000;
	p_self->sleep_sec = (p_self->sleep_sec < DYNDNS_MIN_SLEEP) ?  DYNDNS_MIN_SLEEP: p_self->sleep_sec;
	(p_self->sleep_sec > DYNDNS_MAX_SLEEP) ?  p_self->sleep_sec = DYNDNS_MAX_SLEEP: 1;

	return RC_OK;
}

static RC_TYPE get_update_period_sec_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT *p_self = (DYN_DNS_CLIENT *) p_context;
	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	if (sscanf(p_cmd->argv[current_nr], "%d", &p_self->sleep_sec) != 1)
	{
		return RC_DYNDNS_INVALID_OPTION;
	}

	p_self->sleep_sec = (p_self->sleep_sec < DYNDNS_MIN_SLEEP) ?  DYNDNS_MIN_SLEEP: p_self->sleep_sec;
	(p_self->sleep_sec > DYNDNS_MAX_SLEEP) ?  p_self->sleep_sec = DYNDNS_MAX_SLEEP: 1;

	return RC_OK;
}

static RC_TYPE get_forced_update_period_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT *p_self = (DYN_DNS_CLIENT *) p_context;
	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	if (sscanf(p_cmd->argv[current_nr], "%u", &p_self->forced_update_period_sec) != 1 ||
	        sscanf(p_cmd->argv[current_nr], "%u", &p_self->forced_update_period_sec_orig) != 1)
	{
		return RC_DYNDNS_INVALID_OPTION;
	}

	p_self->forced_update_period_sec = (p_self->forced_update_period_sec < DYNDNS_MIN_FORCED_UPDATE_PERIOD_S) ?  DYNDNS_MIN_FORCED_UPDATE_PERIOD_S: p_self->forced_update_period_sec;
	(p_self->forced_update_period_sec > DYNDNS_MAX_FORCED_UPDATE_PERIOD_S) ?  p_self->forced_update_period_sec = DYNDNS_MAX_FORCED_UPDATE_PERIOD_S: 1;

	p_self->forced_update_period_sec_orig=p_self->forced_update_period_sec;


	return RC_OK;
}

#ifdef USE_THREADS

/*round up cutoff for forced update period expiration calc.  map [-4 .. 5] to [0 .. .9]*/
static RC_TYPE get_forced_update_adjust_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	int		adjust;
	DYN_DNS_CLIENT	*p_self = (DYN_DNS_CLIENT *) p_context;

	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	if (sscanf(p_cmd->argv[current_nr], "%d", &adjust) != 1)
	{
		return RC_DYNDNS_INVALID_OPTION;
	}

	if ((adjust < DYNDNS_MIN_FORCED_UPDATE_ADJUST) || (adjust > DYNDNS_MAX_FORCED_UPDATE_ADJUST)) {

		DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "forced_update_adjust, %d, not [%d..%d]...\n",adjust,DYNDNS_MIN_FORCED_UPDATE_ADJUST,DYNDNS_MAX_FORCED_UPDATE_ADJUST));

		return RC_DYNDNS_INVALID_OPTION;
	}
	else {

		p_self->forced_update_adjust=(float) adjust*-1;

		p_self->forced_update_adjust=p_self->forced_update_adjust/10+.5f;
	}

	return RC_OK;
}

#endif

static RC_TYPE set_syslog_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT *p_self = (DYN_DNS_CLIENT *) p_context;
	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}
	p_self->debug_to_syslog = TRUE;
	return RC_OK;
}

/**
 * Reads the params for change persona. Format:
 * <uid[:gid]>
 */
static RC_TYPE set_change_persona_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT *p_self = (DYN_DNS_CLIENT *) p_context;
	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	{
		int gid = -1;
		int uid = -1;

		char *p_gid = strstr(p_cmd->argv[current_nr],":");
		if (p_gid)
		{
			if ((strlen(p_gid + 1) > 0) &&  /* if something is present after :*/
			        sscanf(p_gid + 1, "%d",&gid) != 1)
			{
				return RC_DYNDNS_INVALID_OPTION;
			}
		}
		if (sscanf(p_cmd->argv[current_nr], "%d",&uid) != 1)
		{
			return RC_DYNDNS_INVALID_OPTION;
		}

		p_self->change_persona = TRUE;
		p_self->sys_usr_info.gid = gid;
		p_self->sys_usr_info.uid = uid;
	}
	return RC_OK;
}

RC_TYPE print_version_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT *p_self = (DYN_DNS_CLIENT *) p_context;
	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	DBG_PRINTF((LOG_SYSTEM,"Version: %s\n", DYNDNS_VERSION_STRING));
	p_self->abort = TRUE;
	return RC_OK;
}

static RC_TYPE get_exec_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT *p_self = (DYN_DNS_CLIENT *) p_context;
	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	if (p_self->external_command)

		free(p_self->external_command);

	p_self->external_command=safe_malloc(strlen(p_cmd->argv[current_nr])+1);

	strcpy(p_self->external_command, p_cmd->argv[current_nr]);
	return RC_OK;
}

static RC_TYPE do_set_file_param(char **dest, char *dir,char *file_name,int is_create)
{
        RC_TYPE rc=RC_FILE_IO_OPEN_ERROR;
        char    *src_ptr=NULL;
        int     is_file_exists;
        char    *p_cwd=NULL;
        int     p_trailing;

	*dest=NULL;

        if (dir[0]==DIR_DELIM_STR[0] || dir[1]==':') {

                p_cwd=dir;
        }
        else {

		p_cwd=default_relative_path(&p_cwd);

                if (dir[0]=='.' && !(dir[1]=='.'))

                        dir++;

                if(dir[0]==DIR_DELIM_STR[0])

                        dir++;
        }

        p_trailing=strlen(dir)-1;

        if (p_trailing>=0)

                if (dir[p_trailing]==DIR_DELIM_STR[0])

                        dir[p_trailing]='\0';

        if (p_cwd==dir || !(strlen(dir))) {

                src_ptr=safe_malloc(strlen(p_cwd)+strlen(file_name)+2);
                sprintf(src_ptr,"%s" DIR_DELIM_STR "%s",p_cwd,file_name);
        }
        else {

                src_ptr=safe_malloc(strlen(p_cwd)+strlen(dir)+strlen(file_name)+3);

                sprintf(src_ptr,"%s" DIR_DELIM_STR "%s" DIR_DELIM_STR "%s",p_cwd,dir,file_name);
        }

        if (!(is_file_exists=is_file(src_ptr)))

		if (is_create)

	                is_file_exists=create_file2(src_ptr);

        if (is_file_exists) {

                rc=RC_OK;

#ifdef _WIN32
                nt_console_name2(dest,src_ptr);
#else

                *dest=safe_malloc(strlen(src_ptr)+1);

                strcpy(*dest,src_ptr);
#endif

        }

        free(src_ptr);

        if (!(p_cwd==dir))

                free(p_cwd);

        return rc;
}

static RC_TYPE set_file_param(char **dest, char *dir,char *file_name)
{

	return do_set_file_param(dest,dir,file_name,1);
}

static RC_TYPE get_cache_dir_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{

	DYN_DNS_CLIENT	*p_self = (DYN_DNS_CLIENT *) p_context;

	char		*ip_cache=NULL;
	char		*time_cache=NULL;


	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	if (RC_OK==set_file_param(&ip_cache,p_cmd->argv[current_nr],DYNDNS_DEFAULT_IP_FILE)) {

		if (p_self->ip_cache) {

			free(p_self->ip_cache);
		}

		p_self->ip_cache=ip_cache;
	}

	if (RC_OK==set_file_param(&time_cache,p_cmd->argv[current_nr],DYNDNS_DEFAULT_TIME_FILE)) {

		if (p_self->time_cache) {

			free(p_self->time_cache);
		}

		p_self->time_cache=time_cache;
	}


	return RC_OK;
}

DYNDNS_SYSTEM *create_custom_ddns(char *p_key)
{
	DYNDNS_SYSTEM	*p_ret=safe_malloc(sizeof(DYNDNS_SYSTEM));


	p_ret->p_rsp_ok_func=is_generic_server_rsp_ok;
	p_ret->p_dns_update_req_func=(DNS_SYSTEM_REQUEST_FUNC) get_req_for_generic_custom_http_dns_server;

	p_ret->p_key=safe_malloc(strlen(p_key)+1);
	strcpy(p_ret->p_key,p_key);

	p_ret->p_ip_server_name=safe_malloc(strlen(GENERIC_DNS_IP_SERVER_NAME)+1);
	strcpy(p_ret->p_ip_server_name,GENERIC_DNS_IP_SERVER_NAME);

	p_ret->p_ip_server_url=safe_malloc(strlen(DYNDNS_MY_IP_SERVER_URL)+1);
	strcpy(p_ret->p_ip_server_url,DYNDNS_MY_IP_SERVER_URL);

	p_ret->p_dyndns_server_name=safe_malloc(1);
	p_ret->p_dyndns_server_url=safe_malloc(1);

	/*back compat*/
	p_ret->p_success_string=safe_malloc(strlen("OK")+1);
	strcpy(p_ret->p_success_string,"OK");

	return p_ret;
}

static char *read_line(FILE *f_handle)
{
	char	c_buff[7];
	int	c_bytes;
	char	*in_line;
	int	in_chars=0;

	in_line=safe_malloc(561);

	while (utf_read_utf_8(c_buff,f_handle)) {

 		if (c_buff[0]=='\n')

			break;

		c_bytes=utf_8_encoded_size(c_buff[0]);

		strncat(in_line,c_buff,c_bytes);

		in_chars+=c_bytes;

		if (!(in_chars % 560))

			in_line=safe_realloc(in_line,in_chars+560);
	}

	if (in_chars)

		return in_line;

	free(in_line);

	return NULL;
}

static char *get_additional_server(char *file_name,char *server_key)
{

	FILE	*f_handle;
	char	*in_line=NULL;


	f_handle=fopen(file_name,"r");

	if (!(f_handle))

		DBG_PRINTF((LOG_ERR,"E:" MODULE_TAG "Could not open servers additional cfg:  %s, in get_additional_server...\n",file_name));
	else {

		while ((!(feof(f_handle)))) {

			if ((in_line=read_line(f_handle))) {

				if (!(*in_line=='#')) {

					if (strstr(in_line,server_key)) {

						break;
					}
				}

				free(in_line);
			}
		}

		fclose(f_handle);
	}

	return in_line;
}

static char *ltrim(char *s_in)
{

	while (*s_in==' ')

		s_in++;

	return s_in;
}

static RC_TYPE add_server_params_to_cmd(CMD_DATA *p_cmd,char *p_server,int current_nr)
{

	char	*p_arg_end;


        if (!(p_server=strpbrk(p_server," "))) {

                DBG_PRINTF((LOG_CRIT,"C:" MODULE_TAG "Invalid servers additional file format in add_server_params_to_cmd...\n"));

                return RC_CMD_PARSER_INVALID_OPTION_ARGUMENT;
        }


	while (*p_server) {

		p_server=ltrim(p_server);

		/*quotation state*/
		if (!(*p_server=='"')) {

			if ((p_arg_end=strpbrk(p_server," ")))

				*p_arg_end='\0';
		}
		else {

			p_server++;

			p_arg_end=strpbrk(p_server,"\"");

			*p_arg_end='\0';
		}

		cmd_insert_val_after(p_cmd,p_server,current_nr);

		current_nr++;

		if (!(p_arg_end))

			break;

		p_server=p_arg_end+1;
	}

	return RC_OK;
}

/*input is a space delimited string of server parameter options,
  the first of which must be the unique (relative to system hard
  coded servers, and those in system additional servers file)
  identifier for the given server.

  return the unique server identifiier.
*/
static char *svr_add_cfg_key(char **s_ret,char *p_server)
{
	char	*p_delim;
	char	*in_str=safe_malloc(strlen(p_server)+1);

	strcpy(in_str,p_server);

	p_delim=strpbrk(in_str," ");

	*(p_delim)='\0';

	*s_ret=safe_malloc(p_delim-in_str+1);

	strcpy(*s_ret,in_str);

	free(in_str);

	return *s_ret;
}

/**
	Searches the DYNDNS system by the argument.
	Input is like: system@server.name
	system=statdns|custom|dyndns|default
	server name = dyndns.org | freedns.afraid.org
	The result is a pointer in the table of DNS systems.

	Argument may also be searched within additional
	preconfigured servers program file.  So if miss
	on DYNDNS system, check the file.

	custom@blah_blah (USER_DEFINED_SERVER) may appear
	more than once in multi-services version of the
	program.

	Both custom, and additional servers are allocated
	based on DYNDNS system custom server type.  Ad-
	ditional type are qualified relative to pre-
	configuration defined in file.
*/
static RC_TYPE get_dyndns_system_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYNDNS_SYSTEM	*p_dns_system;
	DYN_DNS_CLIENT	*p_self = (DYN_DNS_CLIENT *) p_context;
	char		*p_server=NULL;
	int		ndx;


	if (p_self == NULL)

		return RC_INVALID_POINTER;

	/*new or existing server name can appear anywhere in config or command line.  if new, increment the global
	  index but, first check if it's already in list of servers and set index (or counter) correspondingly
	  to it.  all "info" (server info structure) related handlers will use this index (or counter) until
	  a new, or other existing name appears.*/

	if (!(p_self->srv_cnt<MAX_DNS_SERVER))

		return RC_DYNDNS_TOO_MANY_SERVERS;

	/*search system table first, then additional preconfigured servers file*/
	if (!(p_dns_system=get_dns_system(p_cmd->argv[current_nr]))) {

		if (!(p_self->servers_additional))

			return RC_DYNDNS_INVALID_OPTION;

		/*p_server gets a string like:  default@server.com --cmd_option param --cmd_option2 param...*/
		if (!(p_server=get_additional_server(p_self->servers_additional,p_cmd->argv[current_nr])))

			return RC_DYNDNS_INVALID_OPTION;
	}

	/*if default server has no aliases*/
	if (0==server_index)

		if (!(p_self->info[0].alias_info.count))

			p_self->srv_cnt--;

	if (p_dns_system) {

		/*note if more than one 'custom' no way to 'recall' it in config
		  so we create a new one every time with dyndns.c system table
		  custom entry as template.
		*/
		if ((0==strcmp(USER_DEFINED_SERVER,p_dns_system->p_key))) {

			p_dns_system=create_custom_ddns(USER_DEFINED_SERVER);

			server_index=p_self->srv_cnt;

			p_self->srv_cnt++;
		}
		else {
			if(p_self->srv_cnt == 1 && p_self->info[0].is_defaults_config == TRUE)
			{
				server_index = 0 ;
				p_self->info[0].is_defaults_config == FALSE;
			}
			else if (!(-1==(ndx=dns_system_index_of(p_dns_system->p_key,p_self))))

				server_index=ndx;
			else {
				server_index=p_self->srv_cnt;

				p_self->srv_cnt++;
			}
		}
	}
	else {
		if (p_server) {

			char	*p_key;

			if (!(-1==(ndx=dns_system_index_of(svr_add_cfg_key(&p_key,p_server),p_self)))) {

				DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "found existing configured server:  %s\n",p_key));

				server_index=ndx;

				p_dns_system=p_self->info[server_index].p_dns_system;
			}
			else {
				DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "Additional server:  %s\n",p_server));

				add_server_params_to_cmd(p_cmd,p_server,current_nr);

				free(p_server);

				p_dns_system=create_custom_ddns(p_key);

				server_index=p_self->srv_cnt;

				p_self->srv_cnt++;
			}

			free(p_key);
		}
	}
	

	p_self->info[server_index].p_dns_system = p_dns_system;

        p_self->info[server_index].is_auto_dual_list=p_dns_system->is_auto_dual_list;
	/* setting svr_ip_param, will turn off auto detect*/
	p_self->info[server_index].is_auto_detect=p_dns_system->is_auto_detect;

	/*if not reconfiguring already entered server*/
	if (!(strlen(p_self->info[server_index].ipv6_priv_ext.addr_pref))) {

		strcpy(p_self->info[server_index].ipv6_priv_ext.addr_pref,"pub");
		p_self->info[server_index].ipv6_priv_ext.flags=IPV6_PREFER_SRC_PUBLIC;
	}

	return RC_OK;
}

static RC_TYPE get_debug_level_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{

	#define ASCII_ZERO     48

	int                    dwLevel=0;


	DYN_DNS_CLIENT *p_self = (DYN_DNS_CLIENT *) p_context;

	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	dwLevel=*(p_cmd->argv[current_nr])-ASCII_ZERO;

	if (dwLevel<LOG_EMERG || dwLevel>LOG_DEBUG)

		return RC_DYNDNS_INVALID_OPTION;

	p_self->dbg.level=dwLevel;

	set_debug_level(dwLevel);

	return RC_OK;
}

static RC_TYPE get_retries_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	int	retries=0;


	DYN_DNS_CLIENT *p_self=(DYN_DNS_CLIENT *) p_context;


	if (p_self==NULL)

		return RC_INVALID_POINTER;


	if (sscanf(p_cmd->argv[current_nr], "%d",&retries) != 1)

		return RC_DYNDNS_INVALID_OPTION;


	if (retries<0 || retries>100)

		return RC_DYNDNS_INVALID_OPTION;


	p_self->net_retries=retries;


	return RC_OK;
}

static RC_TYPE get_retry_interval_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	int	retry_interval=0;


	DYN_DNS_CLIENT *p_self=(DYN_DNS_CLIENT *) p_context;


	if (p_self==NULL)

		return RC_INVALID_POINTER;


	if (sscanf(p_cmd->argv[current_nr], "%d",&retry_interval) != 1)

		return RC_DYNDNS_INVALID_OPTION;


	if (retry_interval<DYNDNS_MIN_RETRY_INTERVAL || retry_interval>DYNDNS_MAX_RETRY_INTERVAL)

		return RC_DYNDNS_INVALID_OPTION;


	p_self->retry_interval=retry_interval;


	return RC_OK;
}

static RC_TYPE get_retry_pending_interval_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	int	retry_interval=0;


	DYN_DNS_CLIENT *p_self=(DYN_DNS_CLIENT *) p_context;


	if (p_self==NULL)

		return RC_INVALID_POINTER;


	if (sscanf(p_cmd->argv[current_nr], "%d",&retry_interval) != 1)

		return RC_DYNDNS_INVALID_OPTION;


	if (retry_interval<DYNDNS_MIN_RETRY_PENDING_INTERVAL || retry_interval>DYNDNS_MAX_RETRY_PENDING_INTERVAL)

		return RC_DYNDNS_INVALID_OPTION;


	p_self->retry_pending_interval=retry_interval;


	return RC_OK;
}

static RC_TYPE get_retry_pending_off_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{

	DYN_DNS_CLIENT *p_self=(DYN_DNS_CLIENT *) p_context;


	if (p_self==NULL)

		return RC_INVALID_POINTER;

	p_self->retry_pending_off=(!(p_self->retry_pending_off));


	return RC_OK;
}

static RC_TYPE get_lang_file_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{

	DYN_DNS_CLIENT	*p_self=(DYN_DNS_CLIENT *) p_context;
	char		*filename;


	/*use harded coded defaults if have
	  --lang_file parameter with no option --
	  otherwise, use option indicated language
	  strings file, and if not there, program
	  will automatically use locale indicated
	  language strings file, defaulting to
	  en.lng if locale indicated file not
	  present.
	*/

	if (p_self==NULL)

		return RC_INVALID_POINTER;

	p_self->lang_hard_coded=1;


	if (current_nr>=p_cmd->argc) /*no parameter -- means use hard coded*/

		return RC_OK;

	if (!(strncmp(p_cmd->argv[current_nr],"-",1))) /*no parameter -- means use hard coded*/

		return RC_OK;


	p_self->lang_hard_coded=0;

	do_set_file_param2(&filename,p_cmd->argv[current_nr],0);

	if ((is_file(filename))) { /*ignore if invalid*/

		DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "Using default override language strings file, %s...\n",filename));
	}
	else {

		DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "Could not open default override language strings file, %s...\n",p_cmd->argv[current_nr]));

		return RC_OK;
	}

	if (p_self->lang_file)

		free(p_self->lang_file);

	p_self->lang_file=filename;


	return RC_OK;
}

/*3/1/2015
  legacy windows wrapper for non-reentrant strtok.

  present inadyn-mt does not care this is not
  reentrant, but this is not future proof.

  TODO:  make it reentrant
	 move somewhere more appropriate.
*/
static char* strtok_rr(char* s_in,const char* c_delim,char **p_saved)
{

#ifdef _WIN32

	return strtok(s_in,c_delim);
#else
	return strtok_r(s_in,c_delim,p_saved);
#endif
}

/*input:  could be a single string, or 2 or more comma delimited strings.  Add each to a list.
*/
static RC_TYPE tokenize_to_list(gen_list list,char* s_src)
{
	int		arg_len;
	char		*s_in;
	char		*p_s;
	char		*s_list_in;
	char		*p_saved;
	char		c_delim[2];


	arg_len=strlen(s_src);

	s_in=safe_malloc(arg_len+1);
	strcpy(s_in,s_src);

	DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "tokenize_to_list input string:  %s...\n",s_in));

	/*a single string*/
	if (!(strpbrk(s_in,","))) {

		DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "tokenize_to_list adding to list, exiting  %s...\n",s_in));

		list_add(list,s_in);

		return RC_OK;
	}

	strcpy(c_delim,",");

	/*only comma(s)*/
	if (!(p_s=strtok_rr(s_in,c_delim,&p_saved))) {

		free(s_in);

		return RC_DYNDNS_INVALID_OPTION;
	}

	do {

		s_list_in=safe_malloc(strlen(p_s)+1);
		strcpy(s_list_in,p_s);

		DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "tokenize_to_list adding to list:  %s\n",s_list_in));

		list_add(list,s_list_in);
	}
	while ((p_s=strtok_rr(NULL,c_delim,&p_saved)));

	free(s_in);

	return RC_OK;
}

static RC_TYPE do_get_rsp_params_handler(CMD_DATA *p_cmd, int current_nr, void *p_context,gen_list* params_list)
{
	DYN_DNS_CLIENT* p_self=p_context;
	gen_list_itr    itr;
	RC_TYPE         ret;


	if (p_self==NULL)

		return RC_INVALID_POINTER;

	if (current_nr>=p_cmd->argc)

		return RC_DYNDNS_INVALID_OPTION;

	if (!(strncmp(p_cmd->argv[current_nr],"-",1)))

		return RC_DYNDNS_INVALID_OPTION;

	if (!(strlen(p_cmd->argv[current_nr])))

		return RC_DYNDNS_INVALID_OPTION;

	/*overwriting?*/
	if (*params_list) {

		itr=list_get_itr(*params_list);
		list_begin(*params_list,itr);

		while (!list_end(itr)) {

			free(list_entry_item(itr));

			list_next(itr);
		}

		list_destroy(params_list);
	}

	list_create(params_list);

	if (!(RC_OK==(ret=tokenize_to_list(*params_list,p_cmd->argv[current_nr])))) {

		list_destroy(params_list);
	}
	else {

		list_itr_create(*params_list,&itr);

		DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "do_get_rsp_params_handler, entering list print loop...\n"));

		while(1) {

			DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "do_get_rsp_params_handler:  %s\n",list_entry_item(itr)));

			list_next(itr);

			if (list_end(itr))

				break;
		}

		list_itr_destroy(&itr);
	}

	return ret;
}

static RC_TYPE get_config_rsp_params_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{

	return do_get_rsp_params_handler(p_cmd,current_nr,p_context,&((DYN_DNS_CLIENT*) p_context)->info[server_index].rsp_config_codes);
}

static RC_TYPE get_success_rsp_params_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{

	return do_get_rsp_params_handler(p_cmd,current_nr,p_context,&((DYN_DNS_CLIENT*) p_context)->info[server_index].rsp_success_codes);
}

static RC_TYPE get_ip_param_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT	*p_self=(DYN_DNS_CLIENT *) p_context;


	if (p_self==NULL)

		return RC_INVALID_POINTER;

	if (current_nr>=p_cmd->argc)

		return RC_DYNDNS_INVALID_OPTION;

	if (!(strncmp(p_cmd->argv[current_nr],"-",1)))

		return RC_DYNDNS_INVALID_OPTION;

	p_self->info[server_index].ip_param=safe_realloc(p_self->info[server_index].ip_param,strlen(p_cmd->argv[current_nr])+1);
	strcpy(p_self->info[server_index].ip_param,p_cmd->argv[current_nr]);

	p_self->info[server_index].is_auto_detect=0;

	if (current_nr+1<p_cmd->argc) {

		if (strncmp(p_cmd->argv[current_nr+1],"-",1)) {

			p_self->info[server_index].id_param=safe_realloc(p_self->info[server_index].id_param,strlen(p_cmd->argv[current_nr+1])+1);
			strcpy(p_self->info[server_index].id_param,p_cmd->argv[current_nr+1]);

			if (current_nr+2<p_cmd->argc) {

				if (strncmp(p_cmd->argv[current_nr+2],"-",1)) {

					p_self->info[server_index].pw_param=safe_realloc(p_self->info[server_index].pw_param,strlen(p_cmd->argv[current_nr+2])+1);
					strcpy(p_self->info[server_index].pw_param,p_cmd->argv[current_nr+2]);
				}
			}
		}
	}

	return RC_OK;
}

static RC_TYPE get_ip_in_svr_rsp_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT	*p_self=(DYN_DNS_CLIENT *) p_context;

	if (!(p_self))

		return RC_INVALID_POINTER;

	p_self->info[server_index].is_ip_in_svr_rsp=1;

	return RC_OK;
}

static RC_TYPE get_auto_dual_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
        DYN_DNS_CLIENT  *p_self=(DYN_DNS_CLIENT *) p_context;

        if (!(p_self))

                return RC_INVALID_POINTER;

	p_self->info[server_index].is_auto_dual_list=1;

	return RC_OK;
}

static RC_TYPE get_auto_detect_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
        DYN_DNS_CLIENT  *p_self=(DYN_DNS_CLIENT *) p_context;

        if (!(p_self))

                return RC_INVALID_POINTER;

        p_self->info[server_index].is_auto_detect=1;

        return RC_OK;
}

static RC_TYPE get_svr_add_cfg_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT	*p_self=p_context;


	if (p_self==NULL)

		return RC_INVALID_POINTER;

	if (current_nr>=p_cmd->argc)

		return RC_DYNDNS_INVALID_OPTION;

	if (!(strncmp(p_cmd->argv[current_nr],"-",1)))

		return RC_DYNDNS_INVALID_OPTION;

	free(p_self->servers_additional);
	p_self->servers_additional=safe_malloc(strlen(p_cmd->argv[current_nr])+1);
	strcpy(p_self->servers_additional,p_cmd->argv[current_nr]);

	return RC_OK;
}

static void do_get_if_name_handler(char **dest,char *src)
{

	*dest=safe_realloc(*dest,strlen(src)+1);
	strcpy(*dest,src);
}

static RC_TYPE get_if_name_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT	*p_self=(DYN_DNS_CLIENT *) p_context;


	if (p_self==NULL)

		return RC_INVALID_POINTER;

	if (current_nr>=p_cmd->argc)

		return RC_DYNDNS_INVALID_OPTION;

	if (!(strncmp(p_cmd->argv[current_nr],"-",1)))

		return RC_DYNDNS_INVALID_OPTION;

	do_get_if_name_handler(&p_self->info[server_index].if_name,p_cmd->argv[current_nr]);

	return RC_OK;
}

/*map inadyn-mt user interface ipv6 privacy extensions address preference specifier to corresponding rfc constant.
addr_pref string member for specifier, not really needed, but we'll hang on to it - may come in handy for something*/
static int get_priv_ext_flags(char *pref_arg)
{
	if (!strcmp(pref_arg,"pub")) {

		return IPV6_PREFER_SRC_PUBLIC;
	}

	if (!strcmp(pref_arg,"policy")) {

		return 0;
	}

	if (!strcmp(pref_arg,"tmp")) {

		return IPV6_PREFER_SRC_TMP;
	}

	if (!strcmp(pref_arg,"careof")) {

		return IPV6_PREFER_SRC_COA;
	}

	if (!strcmp(pref_arg,"crypto")) {

		return IPV6_PREFER_SRC_CGA;
	}

	if (!strcmp(pref_arg,"nocrypto")) {

		return IPV6_PREFER_SRC_NONCGA;
	}

	if (!strcmp(pref_arg,"home")) {

		return IPV6_PREFER_SRC_HOME;
	}

	if (!strcmp(pref_arg,"pubtmp")) {

		return IPV6_PREFER_SRC_PUBTMP_DEFAULT;
	}

	return -1;
}

static RC_TYPE do_set_priv_ext(char *pref_arg,IPV6_PRIV_EXT *ipv6_priv_ext)
{
	int	flags;


	if ((-1==(flags=get_priv_ext_flags(pref_arg))))

		return RC_DYNDNS_INVALID_OPTION;
	else {
		ipv6_priv_ext->flags=flags;

		strcpy(ipv6_priv_ext->addr_pref,pref_arg);
	}

	return RC_OK;
}

static RC_TYPE set_priv_ext(char *pref_arg,IPV6_PRIV_EXT **ipv6_priv_ext)
{
	RC_TYPE		rc;
	IPV6_PRIV_EXT	this_ipv6_priv_ext;

	if (RC_OK==(rc=do_set_priv_ext(pref_arg,&this_ipv6_priv_ext))) {

		*ipv6_priv_ext=safe_malloc(sizeof(IPV6_PRIV_EXT));
		memcpy(*ipv6_priv_ext,&this_ipv6_priv_ext,sizeof(IPV6_PRIV_EXT));
	}

	return rc;
}

static RC_TYPE get_addr_pref_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{

	DYN_DNS_CLIENT *p_self=(DYN_DNS_CLIENT *) p_context;


	if (p_self==NULL)

		return RC_INVALID_POINTER;

	if (current_nr>=p_cmd->argc)

		return RC_DYNDNS_INVALID_OPTION;

	if (!(strncmp(p_cmd->argv[current_nr],"-",1)))

		return RC_DYNDNS_INVALID_OPTION;

	return do_set_priv_ext(p_cmd->argv[current_nr],&p_self->info[server_index].ipv6_priv_ext);
}

static RC_TYPE get_private_ipv6_addr(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT	*p_self = (DYN_DNS_CLIENT *) p_context;
	
	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	/*user*/
	p_self->is_private_ipv6_addr = TRUE	;

	strcpy(p_self->private_ipv6_addr, p_cmd->argv[current_nr]);

	return RC_OK;
}


static RC_TYPE get_improb_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{
	DYN_DNS_CLIENT	*p_self=p_context;

	p_self->is_improbability=TRUE;

	return RC_OK;
}

#ifdef USE_SNDFILE

static RC_TYPE get_audible_on_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{

	DYN_DNS_CLIENT *p_self=(DYN_DNS_CLIENT *) p_context;


	if (p_self==NULL)

		return RC_INVALID_POINTER;


	p_self->audible_off=(!(p_self->audible_off));


	return RC_OK;
}

static RC_TYPE get_wave_loops_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{

	DYN_DNS_CLIENT	*p_self=(DYN_DNS_CLIENT *) p_context;
	int		loops;

	if (p_self==NULL)

		return RC_INVALID_POINTER;


	if (sscanf(p_cmd->argv[current_nr], "%d",&loops) != 1)

		return RC_DYNDNS_INVALID_OPTION;


	if (((loops < DYNDNS_MIN_WAVE_LOOPS || loops > DYNDNS_MAX_WAVE_LOOPS) && loops!=DYNDNS_INFINITE_WAVE)) { /*ignore if invalid*/

		DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "wave_loops, %s, not [%d or %d..%d]...\n",p_cmd->argv[current_nr],DYNDNS_INFINITE_WAVE,DYNDNS_MIN_WAVE_LOOPS,DYNDNS_MAX_WAVE_LOOPS));

		return RC_DYNDNS_INVALID_OPTION;
	}


	p_self->wave_loops=loops;


	return RC_OK;
}

static RC_TYPE get_wave_file_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{

	DYN_DNS_CLIENT	*p_self=(DYN_DNS_CLIENT *) p_context;
	char		*filename;

	if (p_self==NULL)

		return RC_INVALID_POINTER;

	do_set_file_param2(&filename,p_cmd->argv[current_nr],0);

	if ((is_file(filename))) { /*ignore if invalid*/

		DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "Using default override wave file, %s...\n",filename));
	}
	else {

		DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "Could not open default override wave file, %s...\n",p_cmd->argv[current_nr]));

		return RC_OK;
	}

	/*in case already allocated for default*/
	free(p_self->wave_file);

	p_self->wave_file=filename;

	if (current_nr+1<p_cmd->argc)

		if (strncmp(p_cmd->argv[current_nr+1],"-",1))

			return get_wave_loops_handler(p_cmd,current_nr+1,p_context);

	return RC_OK;
}

static RC_TYPE get_alert_retries_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{

	DYN_DNS_CLIENT	*p_self=(DYN_DNS_CLIENT *) p_context;
	int		retries=0;


	if (p_self==NULL)

		return RC_INVALID_POINTER;


	if (sscanf(p_cmd->argv[current_nr], "%d",&retries) != 1)

		return RC_DYNDNS_INVALID_OPTION;


	if (retries < DYNDNS_MIN_ALERT_RETRIES || retries > DYNDNS_MAX_ALERT_RETRIES) { /*ignore if invalid*/

		DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "alert_retries, %s, not [%d..%d]...\n",p_cmd->argv[current_nr],DYNDNS_MIN_ALERT_RETRIES,DYNDNS_MAX_ALERT_RETRIES));

		return RC_DYNDNS_INVALID_OPTION;
	}


	p_self->alert_retries=retries;


	return RC_OK;
}

static RC_TYPE get_alert_interval_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{

	DYN_DNS_CLIENT	*p_self=(DYN_DNS_CLIENT *) p_context;
	int		interval=0;


	if (p_self==NULL)

		return RC_INVALID_POINTER;


	if (sscanf(p_cmd->argv[current_nr], "%i",&interval) != 1)

		return RC_DYNDNS_INVALID_OPTION;


	if (interval < DYNDNS_MIN_ALERT_INTERVAL || interval > DYNDNS_MAX_ALERT_INTERVAL) {

		DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "alert_retries, %s, not [%d..%d]...\n",p_cmd->argv[current_nr],DYNDNS_MIN_ALERT_INTERVAL,DYNDNS_MAX_ALERT_INTERVAL));

		return RC_DYNDNS_INVALID_OPTION;
	}

	p_self->alert_interval=interval;


	return RC_OK;
}

/*
	Allow wave attenuation to -120db

	User interface gain [-10..10] (present values of related constants)
	is translated to [-20..0]

	This in turn is translated according to waveout.c's WAVE_DECIBEL_STEP
*/
RC_TYPE set_wave_gain(DYN_DNS_CLIENT *p_self,float gain)
{

	if (p_self==NULL)

		return RC_INVALID_POINTER;


	if (gain < DYNDNS_MIN_WAVE_GAIN || gain > DYNDNS_MAX_WAVE_GAIN) {

		DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "wave_gain, %d, not [%d..%d]...\n",gain,DYNDNS_MIN_WAVE_GAIN,DYNDNS_MAX_WAVE_GAIN));

		return RC_DYNDNS_INVALID_OPTION;
	}

	if (!(gain))

		gain=DYNDNS_DEFAULT_WAVE_GAIN;

	else
		gain=DYNDNS_MIN_WAVE_GAIN+gain;


	p_self->wave_gain=gain*DYNDNS_DECIBEL_STEP/WAVE_DECIBEL_STEP;


	return RC_OK;
}

static RC_TYPE get_wave_gain_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{

	DYN_DNS_CLIENT	*p_self=(DYN_DNS_CLIENT *) p_context;
	float		gain=0;


	if (p_self==NULL)

		return RC_INVALID_POINTER;


	if (sscanf(p_cmd->argv[current_nr], "%f",&gain) != 1)

		return RC_DYNDNS_INVALID_OPTION;


	return set_wave_gain(p_self,gain);
}

static RC_TYPE get_wave_buff_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{

	DYN_DNS_CLIENT	*p_self=(DYN_DNS_CLIENT *) p_context;
	float		buff_fact=0;


	if (p_self==NULL)

		return RC_INVALID_POINTER;


	if (sscanf(p_cmd->argv[current_nr], "%f",&buff_fact) != 1)

		return RC_DYNDNS_INVALID_OPTION;


	if (buff_fact>=DYNDNS_MIN_WAVE_BUFF_FACTOR && buff_fact<=DYNDNS_MAX_WAVE_BUFF_FACTOR)

		p_self->wave_buff_factor=buff_fact;
	else

		return RC_DYNDNS_INVALID_OPTION;


	return RC_OK;
}

#ifdef USE_THREADS

static RC_TYPE get_status_interval_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{

	DYN_DNS_CLIENT	*p_self=(DYN_DNS_CLIENT *) p_context;
	int		interval=0;


	if (p_self==NULL)

		return RC_INVALID_POINTER;


	if (sscanf(p_cmd->argv[current_nr], "%i",&interval) != 1)

		return RC_DYNDNS_INVALID_OPTION;


	if (interval < DYNDNS_MIN_STATUS_INTERVAL || interval > DYNDNS_MAX_STATUS_INTERVAL) {

		DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "status_interval, %s, not [%i..%i]...\n",p_cmd->argv[current_nr],
					DYNDNS_MIN_STATUS_INTERVAL,DYNDNS_MAX_STATUS_INTERVAL));

		return RC_DYNDNS_INVALID_OPTION;
	}

	p_self->status_interval=interval*1000;


	return RC_OK;

}

static RC_TYPE get_status_offline_interval_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{

	DYN_DNS_CLIENT	*p_self=(DYN_DNS_CLIENT *) p_context;
	int		interval=0;


	if (p_self==NULL)

		return RC_INVALID_POINTER;


	if (sscanf(p_cmd->argv[current_nr], "%i",&interval) != 1)

		return RC_DYNDNS_INVALID_OPTION;


	if (interval < DYNDNS_MIN_STATUS_OFFLINE_INTERVAL || interval > DYNDNS_MAX_STATUS_OFFLINE_INTERVAL) {

		DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "status_offline_interval, %s, not [%i..%i]...\n",p_cmd->argv[current_nr],
					DYNDNS_MIN_STATUS_OFFLINE_INTERVAL,DYNDNS_MAX_STATUS_OFFLINE_INTERVAL));

		return RC_DYNDNS_INVALID_OPTION;
	}

	p_self->status_offline_interval=interval*1000;


	return RC_OK;

}

#endif /*USE_THREADS*/
#endif /*USE_SNDFILE*/

static RC_TYPE push_in_buffer(char* p_src, int src_len, char *p_buffer, int* p_act_len, int max_len)
{
	if (*p_act_len + src_len > max_len)
	{
		return RC_FILE_IO_OUT_OF_BUFFER;
	}
	memcpy(p_buffer + *p_act_len,p_src, src_len);
	*p_act_len += src_len;
	return RC_OK;
}

typedef enum
{
    NEW_LINE,
    COMMENT,
    DATA,
    SPACE,
    ESCAPE,
    QUOTED_DATA,
    QUOTE_END
} PARSER_STATE;

typedef struct
{
	FILE 		*p_file;
	PARSER_STATE	state;
} OPTION_FILE_PARSER;

typedef RC_TYPE (*UTF_PARSE_FUNC)(OPTION_FILE_PARSER *p_cfg, char *p_buffer, int maxlen);

static RC_TYPE parser_init(OPTION_FILE_PARSER *p_cfg, FILE *p_file)
{
	memset(p_cfg, 0, sizeof(*p_cfg));
	p_cfg->state = NEW_LINE;
	p_cfg->p_file = p_file;

	return RC_OK;
}

static RC_TYPE do_parser_read_option(OPTION_FILE_PARSER *p_cfg,
                                     char *p_buffer,
                                     int maxlen,
                                     char *ch,
                                     int *count,
                                     BOOL *parse_end)
{

	RC_TYPE rc = RC_OK;


	switch (p_cfg->state)
	{
	case NEW_LINE:

		if (!(strcmp(ch,"\n")) || !(strcmp(ch,"\r")))
		{
			return rc;
		}

		if (!(strcmp(ch,"\\")))
		{
			p_cfg->state = ESCAPE;

			return rc;
		}

		if (!(strcmp(ch,"#"))) /*comment*/
		{
			p_cfg->state = COMMENT;

			return rc;
		}

		if ((strcmp(ch," ")) && (strcmp(ch,"	")))
		{
			if (strcmp(ch,"-"))   /*add '--' to first word in line*/
			{
				if ((rc = push_in_buffer("--", 2, p_buffer, count, maxlen)) != RC_OK)
				{
					return rc;
				}
			}
			if ((rc = push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen)) != RC_OK)
			{
				return rc;
			}

			p_cfg->state = DATA;

			return rc;
		}

		/*skip actual leading (before option) spaces*/
		return rc;

	case SPACE:

		if (!(strcmp(ch,"\"")))
		{
			p_cfg->state = QUOTED_DATA;

			return rc;
		}

		if (!(strcmp(ch,"\\")))
		{
			p_cfg->state = ESCAPE;

			return rc;
		}

		if (!(strcmp(ch,"#"))) /*comment*/
		{
			p_cfg->state = COMMENT;

			return rc;
		}

		if (!(strcmp(ch,"\n")) || !(strcmp(ch,"\r")))
		{
			p_cfg->state = NEW_LINE;

			return rc;
		}

		if ((strcmp(ch," ")) && (strcmp(ch,"	")))
		{
			if ((rc = push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen)) != RC_OK)
			{
				return rc;
			}
			p_cfg->state = DATA;
		}

		return rc;

	case COMMENT:

		if (!(strcmp(ch,"\n")) || !(strcmp(ch,"\r")))
		{
			p_cfg->state = NEW_LINE;
		}

		/*skip comments*/
		return rc;

	case DATA:

		if (!(strcmp(ch,"\\")))
		{
			p_cfg->state = ESCAPE;

			return rc;
		}

		if (!(strcmp(ch,"#")))
		{
			p_cfg->state = COMMENT;

			return rc;
		}

		if (!(strcmp(ch,"\n")) || !(strcmp(ch,"\r")))
		{

			p_cfg->state = NEW_LINE;
			*parse_end = TRUE;

			return rc;
		}

		if (!(strcmp(ch," ")) || !(strcmp(ch,"	")))
		{
			p_cfg->state = SPACE;
			*parse_end = TRUE;

			return rc;
		}

		/*actual data*/

		return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);

	case QUOTED_DATA:

		if (!(strcmp(ch,"\"")))
		{

			p_cfg->state=QUOTE_END;

			return rc;
		}

		if (strcmp(ch,"\n") && strcmp(ch,"\r")) {

			/*actual data*/

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		return RC_CMD_PARSER_INVALID_OPTION;

	case QUOTE_END:

		if (!(strcmp(ch,"\n")) || !(strcmp(ch,"\r")))
		{

			p_cfg->state=NEW_LINE;

			*parse_end=1;

			return rc;
		}

		if (!(strcmp(ch," ")) || !(strcmp(ch,"	")))
		{

			p_cfg->state=SPACE;

			*parse_end=1;

			return rc;
		}

		return RC_CMD_PARSER_INVALID_OPTION;

	case ESCAPE:

		/*don't require escaping '\' on Windows,
		  and be backward compatible with original
		  inadyn, and previous inadyn-mt.
		*/
		if (strcmp(ch,"\\")) {

			char	back_slash[2]="\\";

			push_in_buffer(back_slash, 1, p_buffer, count, maxlen);
		}

		p_cfg->state=DATA;

		return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);

	default:

		return RC_CMD_PARSER_INVALID_OPTION;
	}
}

/** Read one single option from utf-8 file into the given buffer.
	When the first separator is encountered it returns.
	Actions:
		- read chars while not eof
		- skip comments (parts beginning with '#' and ending with '\n')
		- switch to DATA STATE if non space char is encountered
		- assume first name in lines to be a long option name by adding '--' if necesssary
		- add data to buffer
		- do not forget a 0 at the end
 * States:
 * NEW_LINE - wait here until some option. Add '--' if not already there
 * SPACE - between options. Like NEW_LINE but no additions
 * DATA - real data. Stop on space.
 * COMMENT - everything beginning with # until EOLine
 * ESCAPE - everything that is otherwise (incl. spaces). Next char is raw copied.
*/
static RC_TYPE parser_utf_8_read_option(OPTION_FILE_PARSER *p_cfg, char *p_buffer, int maxlen)
{
	RC_TYPE	rc = RC_OK;
	BOOL	parse_end = FALSE;
	int	count = 0;
	char	c_buff[7];


	*p_buffer = 0;


	while(!parse_end)
	{

		memset(c_buff,0,7);

		if (!(utf_read_utf_8(c_buff,p_cfg->p_file)))
		{
			if (feof(p_cfg->p_file))
			{
				break;
			}

			rc = RC_FILE_IO_READ_ERROR;

			break;
		}

		rc=do_parser_read_option(p_cfg,p_buffer,maxlen,c_buff,&count,&parse_end);

		if (!(rc == RC_OK))

			return rc;
	}


	rc = push_in_buffer("\0",1,p_buffer,&count,maxlen);

	if (p_cfg->state==QUOTED_DATA) /*didn't get an end quote*/

		return RC_CMD_PARSER_INVALID_OPTION;


	return rc;
}

#ifdef _WIN32

/** Read one single option from utf-16 file into the given buffer.
	When the first separator is encountered it returns.
	Actions:
		- read chars while not eof
		- skip comments (parts beginning with '#' and ending with '\n')
		- switch to DATA STATE if non space char is encountered
		- assume first name in lines to be a long option name by adding '--' if necesssary
		- add data to buffer
		- do not forget a 0 at the end
 * States:
 * NEW_LINE	- wait here until some option. Add '--' if not already there
 * SPACE	- between options. Like NEW_LINE but no additions
 * DATA		- real data. Stop on space.
 * COMMENT	- everything beginning with # until EOLine
 * ESCAPE	- everything that is otherwise (incl. spaces). Next char is raw copied.
*/
static RC_TYPE parser_utf_16_read_option(OPTION_FILE_PARSER *p_cfg, char *p_buffer, int maxlen)
{
	RC_TYPE	rc = RC_OK;
	BOOL	parse_end = FALSE;
	int	count = 0;
	char	utf_8[18];
	char	c_buff[4]={0,0,0,0};

	*p_buffer = 0;


	while(!parse_end)

	{
		if (feof(p_cfg->p_file))
		{
			break;
		}

		c_buff[0]=fgetc(p_cfg->p_file);

		if ((c_buff[0]==EOF))
		{
			rc = RC_FILE_IO_READ_ERROR;

			break;
		}

		if (feof(p_cfg->p_file))
		{
			rc = RC_FILE_IO_READ_ERROR;

			break;
		}

		c_buff[1]=fgetc(p_cfg->p_file);

		if ((c_buff[1]==EOF))
		{
			rc = RC_FILE_IO_READ_ERROR;

			break;
		}

		/*convert to utf-8, and pass to parser*/

		rc=do_parser_read_option(p_cfg,p_buffer,maxlen,utf_16_to_8(utf_8,(wchar_t *) c_buff),&count,&parse_end);

		if (!(rc == RC_OK))

			return rc;
	}


	{
		char ch = 0;
		rc = push_in_buffer(&ch, 1, p_buffer, &count, maxlen);
	}

	if (p_cfg->state==QUOTED_DATA) /*didn't get an end quote*/

		return RC_CMD_PARSER_INVALID_OPTION;


	return rc;
}

#endif

static RC_TYPE do_get_options_from_file(CMD_DATA *p_cmd,int current_nr,void *p_context)
{

	RC_TYPE			rc = RC_OK;
	FILE			*p_file = NULL;
	char			*path_filename;
	char			*p_tmp_buffer = NULL;
	const int		buffer_size = DYNDNS_SERVER_NAME_LENGTH;
	OPTION_FILE_PARSER	parser;

	UTF_PARSE_FUNC parse_func=parser_utf_8_read_option;

#ifdef _WIN32

	int	is_bom=0;	/*win32 utf byte order mark?*/
	int	is_bom_8=0;
#endif


	do
	{
		p_tmp_buffer = safe_malloc(buffer_size);
		if (!p_tmp_buffer)
		{
			rc = RC_OUT_OF_MEMORY;
			break;
		}

		do_set_file_param2(&path_filename,p_cmd->argv[current_nr],0);

		if (path_filename)

			p_file = utf_fopen(path_filename, "r");

		if (!(p_file))
		{
			DBG_PRINTF((LOG_ERR,"W:" MODULE_TAG "Cannot open cfg file:  %s\n", p_cmd->argv[current_nr]));
			rc = RC_FILE_IO_OPEN_ERROR;
			break;
		}
#ifdef _WIN32
		{
			wchar_t *utf_16;


			utf_8_to_16(utf_malloc_8_to_16(&utf_16,path_filename),path_filename);

			if (!(utf_is_win_utf_file(utf_16,&is_bom)))

				is_bom_8=is_bom;
			else
				parse_func=parser_utf_16_read_option;

			free(utf_16);
		}

		if (is_bom_8)

			fseek(p_file,3,0);
		else
			if (is_bom)

				fseek(p_file,2,0);
#endif
		if ((rc = parser_init(&parser, p_file)) != RC_OK)
		{
			break;
		}

		while(!feof(p_file))
		{
			rc = parse_func(&parser,p_tmp_buffer, buffer_size);

			if (rc != RC_OK)
			{
				break;
			}

			if (!strlen(p_tmp_buffer))
			{
				break;
			}

			rc = cmd_add_val(p_cmd, p_tmp_buffer,cmd_options_table);
			if (rc != RC_OK)
			{
				break;
			}
		}

	}
	while(0);


	if (p_file)
	{
		fclose(p_file);
	}
	if (p_tmp_buffer)
	{
		free(p_tmp_buffer);
	}

	free(path_filename);

	return rc;
}

/**
	This handler reads the data in the passed file name.
	Then appends the words in the table (cutting all spaces) to the existing cmd line options.
	It adds to the CMD_DATA struct.
	Actions:
		- open file
		- read characters and cut spaces away
		- add values one by one to the existing p_cmd data
*/
static RC_TYPE get_options_from_file_handler(CMD_DATA *p_cmd, int current_nr, void *p_context)
{

	RC_TYPE	rc=RC_OK;
	int	curr_argc=0;
	int	i;
	int	destIndex;

	char	**arg_list;


	DYN_DNS_CLIENT *p_self = (DYN_DNS_CLIENT *) p_context;

	if (!p_self || !p_cmd)
	{
		return RC_INVALID_POINTER;
	}

	curr_argc=p_cmd->argc;

	rc=do_get_options_from_file(p_cmd,current_nr,p_context);

	if (!(rc==RC_OK))

		return rc;

	/*fix up so file arguments have been inserted after current_nr*/

	arg_list=safe_malloc(p_cmd->argc*sizeof(char *));

	current_nr++;

	for (i=0;i<current_nr;i++) {

		arg_list[i]=safe_malloc(strlen(p_cmd->argv[i])+1);

		strcpy(arg_list[i],p_cmd->argv[i]);

		free(p_cmd->argv[i]);
	}

	for (i=curr_argc,destIndex=current_nr;i<p_cmd->argc;i++,destIndex++) {

		arg_list[destIndex]=safe_malloc(strlen(p_cmd->argv[i])+1);

		strcpy(arg_list[destIndex],p_cmd->argv[i]);

		free(p_cmd->argv[i]);
	}

	for (i=current_nr;i<curr_argc;i++,destIndex++) {

		arg_list[destIndex]=safe_malloc(strlen(p_cmd->argv[i])+1);

		strcpy(arg_list[destIndex],p_cmd->argv[i]);

		free(p_cmd->argv[i]);
	}

	free(p_cmd->argv);

	p_cmd->argv=arg_list;


	return rc;
}

RC_TYPE do_get_default_infile_config(char *in_file,DYN_DNS_CLIENT *p_self,CMD_OPTION_ERR_HANDLER_FUNC pf_err_handler)
{

	RC_TYPE	rc = RC_OK;
	int	custom_argc = 3;


	char	**custom_argv = safe_malloc(sizeof(char **)*3);


	custom_argv[0] = safe_malloc(2);
	strcpy(custom_argv[0],"");

	custom_argv[1] = safe_malloc(strlen(DYNDNS_INPUT_FILE_OPT_STRING)+1);
	strcpy(custom_argv[1],DYNDNS_INPUT_FILE_OPT_STRING);

	custom_argv[2] = safe_malloc(strlen(in_file)+1);
	strcpy(custom_argv[2],in_file);


	DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "Attempting using default config file %s\n", in_file));


	rc = get_cmd_parse_data_with_error_handling(custom_argv, custom_argc, cmd_options_table,pf_err_handler);

	free(custom_argv[0]);
	free(custom_argv[1]);
	free(custom_argv[2]);

	free(custom_argv);


	return rc;
}

/*
	try inadyn-mt config file first
	and if it's not there, try mt
	config file
*/
RC_TYPE get_default_infile_config(DYN_DNS_CLIENT *p_self,CMD_OPTION_ERR_HANDLER_FUNC pf_err_handler)
{

	RC_TYPE rc = RC_OK;


	rc = do_get_default_infile_config(DYNDNS_MT_DEFAULT_CONFIG_FILE,p_self,pf_err_handler);

	if (rc==RC_FILE_IO_OPEN_ERROR) {

		rc = do_get_default_infile_config(DYNDNS_MT_DEFAULT_CONFIG_FILE_OLD,p_self,pf_err_handler);

		if (rc==RC_FILE_IO_OPEN_ERROR) {

			rc = do_get_default_infile_config(DYNDNS_DEFAULT_CONFIG_FILE,p_self,pf_err_handler);
		}
	}


	return rc;
}

static RC_TYPE get_alias_param_defaults(DYN_DNS_CLIENT *p_self)
{
	int			i;
	int			server_index;
	ALIAS_PARAMS		*p_params;
	DYNDNS_ALIAS_INFO	*alias_info;
	RC_TYPE			rc=RC_OK;
	int			is_afraid;

	/*get ip server from previously set --ip_server_name option, or from DDNS server default ip server*/

	for (server_index=0;server_index<p_self->srv_cnt;server_index++) {

		if (is_afraid=(0==strcmp(p_self->info[server_index].p_dns_system->p_key,"default@freedns.afraid.org")))

			is_afraid=is_user_pass(p_self->info[server_index].credentials.my_username,
						p_self->info[server_index].credentials.my_password);

		alias_info=&p_self->info[server_index].alias_info;

		for (i=0;i<alias_info->count;i++) {

			p_params=alias_info->p_params[i];

			/*freedns.afraid.org - using sha1 hashed username, and password
			  to fetch update tokens so to be tidy, free any config inlined.
			*/
			if (is_afraid) {

				free(p_self->info[server_index].alias_info.hashes[i].str);
				p_self->info[server_index].alias_info.hashes[i].str=NULL;
			}

			if (!(p_params->ip_server_name)) {

				if ((p_self->ip_server_name_global.name)) {

					p_params->ip_server_name=safe_malloc(strlen(p_self->ip_server_name_global.name)+1);
					strcpy(p_params->ip_server_name,p_self->ip_server_name_global.name);

					p_params->ip_server_url=safe_malloc(strlen(p_self->ip_server_name_global.url)+1);
					strcpy(p_params->ip_server_url,p_self->ip_server_name_global.url);
				}
				else {

					if (!(p_self->info[server_index].ip_server_name.name)) {

						p_params->ip_server_name=safe_malloc(strlen(p_self->info[server_index].p_dns_system->p_ip_server_name)+1);
						strcpy(p_params->ip_server_name,p_self->info[server_index].p_dns_system->p_ip_server_name);

						p_params->ip_server_url=safe_malloc(strlen(p_self->info[server_index].p_dns_system->p_ip_server_url)+1);
						strcpy(p_params->ip_server_url,p_self->info[server_index].p_dns_system->p_ip_server_url);
					}
					else {

						p_params->ip_server_name=safe_malloc(strlen(p_self->info[server_index].ip_server_name.name)+1);
						strcpy(p_params->ip_server_name,p_self->info[server_index].ip_server_name.name);

						p_params->ip_server_url=safe_malloc(strlen(p_self->info[server_index].ip_server_name.url)+1);
						strcpy(p_params->ip_server_url,p_self->info[server_index].ip_server_name.url);

						p_params->ip_server_port=p_self->info[server_index].ip_server_name.port;
					}
				}
			}

			if (!(p_params->ip_server_url)) {

				DBG_PRINTF((LOG_ERR,"E:" MODULE_TAG "No ip server url for alias %s, ip server name %s, in get_alias_params_defaults...\n",
						alias_info->names[i].name,p_params->ip_server_name));

				return RC_DYNDNS_INVALID_OR_MISSING_PARAMETERS;
			}

			if (!(p_params->ip_server_host)) {

				p_params->ip_server_host=safe_malloc(strlen(p_params->ip_server_name)+1);
				strcpy(p_params->ip_server_host,p_params->ip_server_name);
			}

			if (!(p_params->ip_server_port) || (-1==p_params->ip_server_port))

				p_params->ip_server_port=HTTP_DEFAULT_PORT;

			if (!(p_params->iface)) {

				if (p_self->info[server_index].if_name) {

					p_params->iface=safe_malloc(strlen(p_self->info[server_index].if_name)+1);
					strcpy(p_params->iface,p_self->info[server_index].if_name);

					DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "Using DDNS server %s, global interface parameter, %s for alias %s:%s, in get_alias_params_defaults...\n",
							p_self->info[server_index].p_dns_system->p_key,p_params->iface,alias_info->names[i].name,alias_info->names[i].ip_v));
				}
			}

			if (!(p_params->priv_ext))

				if ((strlen(p_self->info[server_index].ipv6_priv_ext.addr_pref))) {

					p_params->priv_ext=safe_malloc(sizeof(IPV6_PRIV_EXT));
					memcpy(p_params->priv_ext,&p_self->info[server_index].ipv6_priv_ext,sizeof(IPV6_PRIV_EXT));
				}

			if (!(p_params->ip_proxy_name))

				if (p_self->info[server_index].proxy_server_name.name) {

					p_params->ip_proxy_name=safe_malloc(strlen(p_self->info[server_index].proxy_server_name.name)+1);
					strcpy(p_params->ip_proxy_name,p_self->info[server_index].proxy_server_name.name);
					p_params->ip_proxy_port=p_self->info[server_index].proxy_server_name.port;
				}
		}
	}

	return RC_OK;
}

static void create_address_structs(DYN_DNS_CLIENT *p_self)
{
	int		i;
	int		svr_index;
	ALIAS_PARAMS	*p_this_params;


	for (svr_index=0;svr_index<p_self->srv_cnt;svr_index++) {

		for (i=0;i<p_self->info[svr_index].alias_info.count;i++) {

			p_this_params=p_self->info[svr_index].alias_info.p_params[i];

			p_self->info[svr_index].alias_info.p_address[i]=address_list_add(&p_self->address_list,
											 p_this_params->iface,
											 p_this_params->ip_ver_str,
											 p_this_params->priv_ext,
											 p_this_params->ip_server_name,
											 p_this_params->ip_server_url,
											 p_this_params->ip_server_port,
											 p_this_params->ip_proxy_name,
											 p_this_params->ip_proxy_port,
											 p_this_params->ip_server_host);

			ip_address_inc_dependents(p_self->info[svr_index].alias_info.p_address[i]);
		}

	}

	iface_enum(p_self->address_list);
}

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
	Note:
		- if no argument is specified tries to call the cmd line parser
	with the default cfg file path.
*/
static RC_TYPE do_get_config_data_with_error_handling(DYN_DNS_CLIENT *p_self, int argc, char** argv,CMD_OPTION_ERR_HANDLER_FUNC pf_err_handler)
{
	RC_TYPE	rc = RC_OK;
	FILE	*fp;
	char	cached_time[80];


	if (!(0==getifaddrs_w(&p_self->ifaddrs)))

		DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "net interface enumeration failed in do_get_config_data... ...\n"));

	do
	{
		/*set up the context pointers */
		{
			CMD_DESCRIPTION_TYPE *it = cmd_options_table;
			while( it->p_option != NULL)
			{
				it->p_handler.p_context = (void*) p_self;
				++it;
			}
		}
		/* in case of no options, assume the default cfg file may be present */
		if (argc == 1)
		{

			rc = get_default_infile_config(p_self,pf_err_handler);
		}
		else
		{
			rc = get_cmd_parse_data_with_error_handling(argv, argc, cmd_options_table,pf_err_handler);
		}

		if (rc != RC_OK || p_self->abort)
		{
			break;
		}

		/*settings that may change due to cmd line options*/
		{
			int	len=0;
			int	i;

			if (!(p_self->info_online_status.name))
			{
				p_self->info_online_status.name=safe_malloc(strlen(DYNDNS_MY_ONLINE_CHECK)+1);
				strcpy(p_self->info_online_status.name,DYNDNS_MY_ONLINE_CHECK);

				p_self->info_online_status.url=safe_malloc(strlen(DYNDNS_MY_ONLINE_CHECK_URL)+1);
				strcpy(p_self->info_online_status.url,DYNDNS_MY_ONLINE_CHECK_URL);

				p_self->info_online_status.port=80;
			}

			if (!(p_self->srv_cnt))

				p_self->srv_cnt=1; /*default server set in main*/

			for (i=0;i<p_self->srv_cnt;i++) {

    				/*dyndns server*/
				if (!(p_self->info[i].dyndns_server_name.name))
				{
					if (p_self->info[i].p_dns_system->p_dyndns_server_name)

						if (!(0==(len=strlen(p_self->info[i].p_dns_system->p_dyndns_server_name)))) {

							p_self->info[i].dyndns_server_name.name=safe_malloc(len+1);
        						strcpy(p_self->info[i].dyndns_server_name.name, p_self->info[i].p_dns_system->p_dyndns_server_name);
						}
				}

 				/*ip server*/
				if (!(p_self->info[i].ip_server_name.name))
				{
					p_self->info[i].ip_server_name.name=safe_malloc(strlen(p_self->info[i].p_dns_system->p_ip_server_name)+1);
					strcpy(p_self->info[i].ip_server_name.name, p_self->info[i].p_dns_system->p_ip_server_name);

					p_self->info[i].ip_server_name.url=safe_malloc(strlen(p_self->info[i].p_dns_system->p_ip_server_url)+1);
					strcpy(p_self->info[i].ip_server_name.url, p_self->info[i].p_dns_system->p_ip_server_url);
				}


				if (!(p_self->info[i].dyndns_server_name.url))
				{
					if (p_self->info[i].p_dns_system->p_dyndns_server_url)

						if (!(0==(len=strlen(p_self->info[i].p_dns_system->p_dyndns_server_url)))) {

							p_self->info[i].dyndns_server_name.url=safe_malloc(len+1);
    							strcpy(p_self->info[i].dyndns_server_name.url, p_self->info[i].p_dns_system->p_dyndns_server_url);
						}
				}

				if (!(p_self->info[i].dyndns_server_host)) {

					if (p_self->info[i].dyndns_server_name.name) {

						p_self->info[i].dyndns_server_host=safe_malloc(strlen(p_self->info[i].dyndns_server_name.name)+1);

						strcpy(p_self->info[i].dyndns_server_host,p_self->info[i].dyndns_server_name.name);
					}
				}

				if (!(p_self->info[i].ip_server_host)) {

					p_self->info[i].ip_server_host=safe_malloc(strlen(p_self->info[i].ip_server_name.name)+1);
					strcpy(p_self->info[i].ip_server_host,p_self->info[i].ip_server_name.name);
				}
			}
		}

		/*check if the neccessary params have been provided*/
		if ((!(p_self->info[0].dyndns_server_name.name)) || (!(p_self->info[0].ip_server_name.name))
			|| (!(p_self->info[0].dyndns_server_name.url)) || (p_self->info[0].alias_info.count == 0))

		{
			rc = RC_DYNDNS_INVALID_OR_MISSING_PARAMETERS;
			break;
		}

		/*be back compat with global options corresponding to
		  new alias option optional parameters - use globals
		  if no corresponding alias parameter*/
		if (!(RC_OK==(rc=get_alias_param_defaults(p_self))))

			break;

		/*finally, create addresses based on alias params, and added
		  missing corresponding global defaults regardless of order
		  entered relative to alias*/
		create_address_structs(p_self);

		/*forced update*/
		if ((fp=utf_fopen(p_self->time_cache, "r")))
		{
			if (fgets (cached_time, sizeof (cached_time), fp)) {

				time_t		current;
				unsigned long	cached;
				unsigned long	dif;
				unsigned long	fup_store=p_self->forced_update_period_sec;

				sscanf(cached_time,"%lu",&cached);
				current=time(NULL);


				if ((unsigned long) current<=cached) /*clock turned back?*/

					p_self->forced_update_period_sec=0;

				else {

					dif=current-cached;

					p_self->forced_update_period_sec-=dif;

					/*unsigned wrap around?*/

					if (fup_store<p_self->forced_update_period_sec)

						p_self->forced_update_period_sec=0;
				}
			}

			fclose(fp);
		}

		/*deprecated out, but may make a comeback, w/a compiler option*/
		p_self->times_since_last_update = 0;
		p_self->forced_update_times = p_self->forced_update_period_sec / p_self->sleep_sec;

		p_self->is_forced_update_attempted=(p_self->forced_update_period_sec==0);
	}
	while(0);

	freeifaddrs_w(p_self->ifaddrs);

	return rc;
}

RC_TYPE get_config_data_with_error_handling(DYN_DNS_CLIENT *p_self, int argc, char** argv,CMD_OPTION_ERR_HANDLER_FUNC pf_err_handler)
{
	RC_TYPE	rc;

	rc=do_get_config_data_with_error_handling(p_self,argc,argv,pf_err_handler);

	free_alias_params_ars(p_self->info,p_self->srv_cnt);

	return rc;
}

RC_TYPE do_get_config_data(DYN_DNS_CLIENT *p_self, int argc, char** argv)
{

	return do_get_config_data_with_error_handling(p_self,argc,argv,NULL);
}

RC_TYPE get_config_data(DYN_DNS_CLIENT *p_self, int argc, char** argv)
{

	return get_config_data_with_error_handling(p_self,argc,argv,NULL);
}
