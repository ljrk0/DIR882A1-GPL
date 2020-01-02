/*
Copyright (C) 2003-2004 Narcis Ilisei
Modifications by Steve Horbachuk
Copyright (C) 2006 Steve Horbachuk
Modifications by Bryan Hoover (bhoover@wecs.com)
Copyright (C) 2007-2011 Bryan Hoover (bhoover@wecs.com)

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
        - Nov 2003 - new version
        - April 2004 - freedns.afraid.org system added.
        - October 2004 - Unix syslog capability added.
        - October 2007 - win32 RAS events trap thread, RAS,
          and network online status checking added.  Debug
          level command line parameter added.  Refactored,
          augmented main loop, including moving one time
          initialization outside of loop.  Two files
          changed -- dyndns.c, inadyn_cmd.c.  Two files
          added -- event_trap.c, event_trap.h.
        - November 2007 - multithread safe debug log.  One file
		  changed -- os.c.
        - December 2007 - Windows service routines, parser default
          command, and registry, config file, command line command
          options hierachy.  Parser error handling callback.
          Added files service.c, service.h, service_main.c,
          service_main.h, debug_service.h.  Changed  inadyn_cmd.c,
        - get_cmd.c, os_windows.c, main.c.
        - June/July 2009
	      libao and Windows waveOut interfacing for .wav file
		  audible alerts on net down.
         -Sept. 2009
	      non-blocking socket create, connect
		 -Nov. 2010
		  pending updates mode
		  multiple aliases handled correctly
		  multiple Windows service instances
		  added trolobit.com's inadyn, ipv6tb, dnsomatic, tzo, milkfish support
		  fixed zoneedit rsp_config false positive
		-Jan 2011
		 added IPv6, etc.
*/

/*
Win 95/98/ME

For deprecated Windows functions, such as _vsnprintf, define _WIN_LEG.

UNICODE (Win 95/98/ME)

For Windows 32 non-NT (95/98/ME) unicode, Define UNICOWS.  Then compile and link with
unicows.lib.  Install unicows.dll.

When linking, order is important.  The following (all of it) on the link line,
without the cr/lf's, will do:

/nod:kernel32.lib /nod:advapi32.lib /nod:user32.lib /nod:gdi32.lib /nod:shell32.lib
/nod:comdlg32.lib /nod:version.lib /nod:mpr.lib /nod:rasapi32.lib /nod:winmm.lib
/nod:winspool.lib /nod:vfw32.lib /nod:secur32.lib /nod:oleacc.lib /nod:oledlg.lib
/nod:sensapi.lib

unicows.lib

kernel32.lib advapi32.lib user32.lib gdi32.lib shell32.lib comdlg32.lib version.lib
mpr.lib rasapi32.lib winmm.lib winspool.lib vfw32.lib secur32.lib oleacc.lib
oledlg.lib sensapi.lib Ws2_32.lib libcmt.lib

There's no unicows.lib debug info, so this should be release target compile only.
*/
#define MODULE_TAG      "INADYN: "

#ifndef EXCLUDE_CONFIG_H

#include "config.h"

#endif

#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32

#include <process.h>

#endif

#include "errorcode.h"

#include "path.h"

#include "dyndns.h"
#include "debug_if.h"
#include "base64.h"
#include "get_cmd.h"
#include "unicode_util.h"
#include "os.h"
#include "safe_mem.h"
#include "lang.h"
#include "md5.h"
#include "numbers.h"
#include "ip.h"
#include "queue.h"
#include "events.h"
#include "args_exp.h"
#include "cache_list.h"
#include "threads_wrapper.h"
#include "sha1.h"

typedef struct CB_ALERT_DATA {


	DYN_DNS_CLIENT	*p_dyndns;
#ifndef _WIN32
	void			*p_data;
#else
	RAS_THREAD_DATA *p_data;
#endif

} CB_ALERT_DATA;


/*
	Define this, and link with winmm.lib
	for audible alerts (see inadyn-mt/extra/wav/alarm.wav)
	on network connection loss.
*/
#ifdef USE_SNDFILE

#include "waveout.h"

#endif

static volatile	BOOL	global_is_online=true;
static volatile BOOL	is_online_thread_exit=false;
static volatile BOOL	is_alert_thread_exit=false;
static volatile BOOL	is_global_update_pending=false;
static volatile BOOL	is_global_in_proc_update_pending=false;
static volatile BOOL	is_global_success_updates=false;

#ifdef USE_THREADS

/*use recently developed threads wrapper -- previous conditional compiles predominate in the present module
*/
static mutex_t		mutex_global_is_online=MUTEX_T_INIT;
static my_timer_t	update_timer;

#endif

#ifndef _WIN32

#ifdef USE_THREADS

#include <pthread.h>

static pthread_t thread_online_test=0;
static pthread_t thread_alert=0;

#endif

#else

#include "debug_service.h"
#include "unicode_util.h"

static BOOL		returnSignaled=false;
static unsigned long	thread_online_test=0;
static unsigned	long	thread_alert=0;

#endif

static int do_is_dyndns_online(DYN_DNS_CLIENT *p_self);
static int is_exit_requested(DYN_DNS_CLIENT *p_self);
static int is_exit_requested_void(void *p_self);
static RC_TYPE do_handle_bad_config(DYN_DNS_CLIENT *p_self,int i,char srv_cnt);
static void exit_online_test_threads();
static RC_TYPE dyn_dns_shutdown(DYN_DNS_CLIENT *p_self);
static RC_TYPE do_dyn_dns_init(DYN_DNS_CLIENT *p_self);
static RC_TYPE init_update_loop(DYN_DNS_CLIENT *p_dyndns,int argc, char* argv[],void **p_data,BOOL *init_flag);
static RC_TYPE dyn_dns_set_comm_src_params(HTTP_CLIENT *dest,char *if_name,IPV6_PRIV_EXT ipv6_priv_ext);
static RC_TYPE dyn_dns_set_dyndns_server_params(HTTP_CLIENT *dest,DYNDNS_INFO_TYPE *info);

#ifdef USE_SNDFILE
#ifdef USE_THREADS

static int is_dyndns_online(DYN_DNS_CLIENT *p_self,void *p_data);

#ifndef _WIN32
static void *is_dyndns_online_thread(void *p_data);
static void *alert_if_offline_thread(void *p_data);
#else
static unsigned __stdcall is_dyndns_online_thread(void *p_data);
static unsigned __stdcall alert_if_offline_thread(void *p_data);
#endif

#else

static void alert_if_offline(DYN_DNS_CLIENT *p_dyndns,void *p_data);

#endif
#endif

#define MD5_DIGEST_BYTES (16)

/* DNS systems specific configurations*/

DYNDNS_ORG_SPECIFIC_DATA dyndns_org_dynamic = {"dyndns"};
DYNDNS_ORG_SPECIFIC_DATA dyndns_org_custom = {"custom"};
DYNDNS_ORG_SPECIFIC_DATA dyndns_org_static = {"statdns"};

static int get_req_for_dyndns_server(DYN_DNS_CLIENT *this, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt, DYNDNS_SYSTEM *p_sys_info);
static int get_req_for_freedns_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt,  DYNDNS_SYSTEM *p_sys_info);
static int get_req_for_generic_http_dns_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt,  DYNDNS_SYSTEM *p_sys_info);
static int get_req_for_noip_http_dns_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt,  DYNDNS_SYSTEM *p_sys_info);
static int get_req_for_dlink_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt,  DYNDNS_SYSTEM *p_sys_info);
static int get_req_for_easydns_http_dns_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt,  DYNDNS_SYSTEM *p_sys_info);
static int get_req_for_sitelutions_http_dns_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt,  DYNDNS_SYSTEM *p_sys_info);
static int get_req_for_tzo_http_dns_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt, DYNDNS_SYSTEM *p_sys_info);
static int get_req_for_he_ipv6_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt, DYNDNS_SYSTEM *p_sys_info);
static int get_req_for_he_dns_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt, DYNDNS_SYSTEM *p_sys_info);
static int get_req_for_dhis_http_dns_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt, DYNDNS_SYSTEM *p_sys_info);
static int get_req_for_zerigo_http_dns_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt, DYNDNS_SYSTEM *p_sys_info);
static int get_req_for_twodns_http_dns_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt, DYNDNS_SYSTEM *p_sys_info);

static RC_TYPE is_dyndns_server_rsp_ok( DYN_DNS_CLIENT *p_self, int alias_index, char*p_rsp, char* p_ok_string,char srv_cnt);
static RC_TYPE is_freedns_server_rsp_ok( DYN_DNS_CLIENT *p_self, int alias_index, char*p_rsp, char* p_ok_string,char srv_cnt);
static RC_TYPE is_zoneedit_server_rsp_ok( DYN_DNS_CLIENT *p_self, int alias_index, char*p_rsp, char* p_ok_string,char srv_cnt);
static RC_TYPE is_easydns_server_rsp_ok( DYN_DNS_CLIENT *p_self, int alias_index, char*p_rsp, char* p_ok_string,char srv_cnt);
static RC_TYPE is_sitelutions_server_rsp_ok( DYN_DNS_CLIENT *p_self, int alias_index, char*p_rsp, char* p_ok_string,char srv_cnt);
static RC_TYPE is_tzo_server_rsp_ok( DYN_DNS_CLIENT *p_self, int alias_index, char*p_rsp, char* p_ok_string,char srv_cnt);
static RC_TYPE is_he_ipv6_server_rsp_ok( DYN_DNS_CLIENT *p_self, int alias_index, char*p_rsp, char* p_ok_string,char srv_cnt);
static RC_TYPE is_dhis_server_rsp_ok( DYN_DNS_CLIENT *p_self, int alias_index, char*p_rsp, char* p_ok_string,char srv_cnt);
static RC_TYPE is_zerigo_server_rsp_ok( DYN_DNS_CLIENT *p_self, int alias_index, char*p_rsp, char* p_ok_string,char srv_cnt);

DYNDNS_SYSTEM_INFO dns_system_table[] =
    {
		{DYNDNS_DEFAULT,
			{"default@dyndns.org", &dyndns_org_dynamic,0,0,
			(DNS_SYSTEM_SRV_RESPONSE_OK_FUNC)is_dyndns_server_rsp_ok,
			(DNS_SYSTEM_REQUEST_FUNC)get_req_for_dyndns_server,
			DYNDNS_MY_IP_SERVER, DYNDNS_MY_IP_SERVER_URL,
			DYNDNS_MY_DNS_SERVER, DYNDNS_MY_DNS_SERVER_URL, NULL}},

		{DYNDNS_DYNAMIC,
			{"dyndns@dyndns.org", &dyndns_org_dynamic,0,0,
			(DNS_SYSTEM_SRV_RESPONSE_OK_FUNC)is_dyndns_server_rsp_ok,
			(DNS_SYSTEM_REQUEST_FUNC) get_req_for_dyndns_server,
			DYNDNS_MY_IP_SERVER, DYNDNS_MY_IP_SERVER_URL,
			DYNDNS_MY_DNS_SERVER, DYNDNS_MY_DNS_SERVER_URL, NULL}},

		{DYNDNS_CUSTOM,
			{"custom@dyndns.org", &dyndns_org_custom,0,0,
			(DNS_SYSTEM_SRV_RESPONSE_OK_FUNC)is_dyndns_server_rsp_ok,
			(DNS_SYSTEM_REQUEST_FUNC) get_req_for_dyndns_server,
			DYNDNS_MY_IP_SERVER, DYNDNS_MY_IP_SERVER_URL,
			DYNDNS_MY_DNS_SERVER, DYNDNS_MY_DNS_SERVER_URL, NULL}},

		{DYNDNS_STATIC,
			{"statdns@dyndns.org", &dyndns_org_static,0,0,
			(DNS_SYSTEM_SRV_RESPONSE_OK_FUNC)is_dyndns_server_rsp_ok,
			(DNS_SYSTEM_REQUEST_FUNC) get_req_for_dyndns_server,
			DYNDNS_MY_IP_SERVER, DYNDNS_MY_IP_SERVER_URL,
			DYNDNS_MY_DNS_SERVER, DYNDNS_MY_DNS_SERVER_URL, NULL}},

		{FREEDNS_AFRAID_ORG_DEFAULT,
			{"default@freedns.afraid.org", NULL,0,0,
			(DNS_SYSTEM_SRV_RESPONSE_OK_FUNC)is_freedns_server_rsp_ok,
			(DNS_SYSTEM_REQUEST_FUNC) get_req_for_freedns_server,
			DYNDNS_MY_IP_SERVER, DYNDNS_MY_IP_SERVER_URL,
			"freedns.afraid.org", "/dynamic/update.php?", NULL}},

		{ZONE_EDIT_DEFAULT,
			{"default@zoneedit.com", NULL,0,0,
			(DNS_SYSTEM_SRV_RESPONSE_OK_FUNC)is_zoneedit_server_rsp_ok,
			(DNS_SYSTEM_REQUEST_FUNC) get_req_for_generic_http_dns_server,
			"dynamic.zoneedit.com", "/checkip.html",
			"dynamic.zoneedit.com", "/auth/dynamic.html?host=", ""}},

		{NOIP_DEFAULT,
			{"default@no-ip.com", NULL,0,0,
			(DNS_SYSTEM_SRV_RESPONSE_OK_FUNC)is_dyndns_server_rsp_ok,
			(DNS_SYSTEM_REQUEST_FUNC) get_req_for_noip_http_dns_server,
			"ip1.dynupdate.no-ip.com", "/",
			"dynupdate.no-ip.com", "/nic/update?hostname=", ""}},

		{EASYDNS_DEFAULT,
			{"default@easydns.com", NULL,0,0,
			(DNS_SYSTEM_SRV_RESPONSE_OK_FUNC)is_easydns_server_rsp_ok,
			(DNS_SYSTEM_REQUEST_FUNC) get_req_for_easydns_http_dns_server,
			DYNDNS_MY_IP_SERVER, DYNDNS_MY_IP_SERVER_URL,
			"members.easydns.com", "/dyn/dyndns.php?hostname=", ""}},

		{DYNDNS_3322_DYNAMIC,
			{"dyndns@3322.org", &dyndns_org_dynamic,0,0,
			(DNS_SYSTEM_SRV_RESPONSE_OK_FUNC)is_dyndns_server_rsp_ok,
			(DNS_SYSTEM_REQUEST_FUNC) get_req_for_dyndns_server,
			DYNDNS_3322_MY_IP_SERVER, DYNDNS_3322_MY_IP_SERVER_URL,
			DYNDNS_3322_MY_DNS_SERVER, DYNDNS_3322_MY_DNS_SERVER_URL, NULL}},

		{SITELUTIONS_DOMAIN,
			{"default@sitelutions.com", NULL,0,0,
			(DNS_SYSTEM_SRV_RESPONSE_OK_FUNC)is_sitelutions_server_rsp_ok,
			(DNS_SYSTEM_REQUEST_FUNC) get_req_for_sitelutions_http_dns_server,
			DYNDNS_MY_IP_SERVER, DYNDNS_MY_IP_SERVER_URL,
			"www.sitelutions.com", "/dnsup?", NULL}},

		{TZO_DEFAULT,
			{"default@tzo.com", NULL,0,0,
			(DNS_SYSTEM_SRV_RESPONSE_OK_FUNC)is_tzo_server_rsp_ok,
			(DNS_SYSTEM_REQUEST_FUNC) get_req_for_tzo_http_dns_server,
			DYNDNS_MY_IP_SERVER, DYNDNS_MY_IP_SERVER_URL,
			"cgi.tzo.com", "/webclient/signedon.html?TZOName=", NULL}},

		{DNSOMATIC_DEFAULT,
			{"default@dnsomatic.com", &dyndns_org_dynamic,0,0,
			(DNS_SYSTEM_SRV_RESPONSE_OK_FUNC)is_dyndns_server_rsp_ok,
			(DNS_SYSTEM_REQUEST_FUNC) get_req_for_dyndns_server,
			DYNDNS_MY_IP_SERVER, DYNDNS_MY_IP_SERVER_URL,
			"updates.dnsomatic.com", "/nic/update?", NULL}},

		{HE_IPV6TB,
			{"ipv6tb@he.net", NULL,0,0,
			(DNS_SYSTEM_SRV_RESPONSE_OK_FUNC)is_he_ipv6_server_rsp_ok,
			(DNS_SYSTEM_REQUEST_FUNC) get_req_for_he_ipv6_server,
			DYNDNS_MY_IP_SERVER, DYNDNS_MY_IP_SERVER_URL,
			"ipv4.tunnelbroker.net", "/ipv4_end.php?", NULL}},

		{HE_DEFAULT,
			{"default@he.net", NULL,0,0,
			(DNS_SYSTEM_SRV_RESPONSE_OK_FUNC)is_dyndns_server_rsp_ok,
			(DNS_SYSTEM_REQUEST_FUNC) get_req_for_he_dns_server,
			"checkip.dns.he.net", "/",
			"dyn.dns.he.net", "/nic/update?", NULL}},

		/* Support for dynsip.org by milkfish, from DD-WRT */
		{DYNSIP_DEFAULT,
			{"default@dynsip.org", NULL,0,0,
			(DNS_SYSTEM_SRV_RESPONSE_OK_FUNC)is_dyndns_server_rsp_ok,
			(DNS_SYSTEM_REQUEST_FUNC)get_req_for_generic_http_dns_server,
			DYNDNS_MY_IP_SERVER, DYNDNS_MY_IP_SERVER_URL,
			"dynsip.org", "/nic/update?hostname=", ""}},

		{DHIS_DEFAULT,
			{"default@dhis.org", NULL,0,0,
			(DNS_SYSTEM_SRV_RESPONSE_OK_FUNC)is_dhis_server_rsp_ok,
			(DNS_SYSTEM_REQUEST_FUNC) get_req_for_dhis_http_dns_server,
			DYNDNS_MY_IP_SERVER, DYNDNS_MY_IP_SERVER_URL,
			"is.dhis.org", "/?", ""}},

		{ZERIGO_DEFAULT,
			{"default@zerigo.com", NULL,0,0,
			(DNS_SYSTEM_SRV_RESPONSE_OK_FUNC)is_zerigo_server_rsp_ok,
			(DNS_SYSTEM_REQUEST_FUNC) get_req_for_zerigo_http_dns_server,
			"checkip.zerigo.com", "/",
			"update.zerigo.com", "/dynamic?host=", "OK"}},

		{TWODNS_DEFAULT,
			{"default@two-dns.de", &dyndns_org_dynamic,0,0,
			(DNS_SYSTEM_SRV_RESPONSE_OK_FUNC)is_dyndns_server_rsp_ok,
			(DNS_SYSTEM_REQUEST_FUNC)get_req_for_twodns_http_dns_server,
			"checkip.two-dns.de", "/",
			"update.twodns.de", "/update?", NULL}},

		{CUSTOM_HTTP_BASIC_AUTH,
			{"custom@http_svr_basic_auth", NULL,0,1,
			(DNS_SYSTEM_SRV_RESPONSE_OK_FUNC)is_generic_server_rsp_ok,
			(DNS_SYSTEM_REQUEST_FUNC) get_req_for_generic_custom_http_dns_server,
			GENERIC_DNS_IP_SERVER_NAME, DYNDNS_MY_IP_SERVER_URL,
			"", "", "OK"}},
			
		{DYNDNS_DLINK,
			{"default@dlinkddns.com", NULL,0,0,
			(DNS_SYSTEM_SRV_RESPONSE_OK_FUNC)is_dyndns_server_rsp_ok,
			(DNS_SYSTEM_REQUEST_FUNC)get_req_for_dlink_server,
			DYNDNS_MY_IP_SERVER, DYNDNS_MY_IP_SERVER_URL,
			DYNDNS_MY_DNS_SERVER, DYNDNS_MY_DNS_SERVER_URL, NULL}},

		{LAST_DNS_SYSTEM, {NULL, NULL, 0, 0, NULL, NULL, NULL, NULL}}
    };

static DYNDNS_SYSTEM* get_dns_system_by_id(DYNDNS_SYSTEM_ID id)
{
	{
		DYNDNS_SYSTEM_INFO *it;

		for (it = dns_system_table; it->id != LAST_DNS_SYSTEM; ++it)
		{
			if (it->id == id)
			{
				return &it->system;
			}
		}
	}
	return NULL;
}

DYNDNS_SYSTEM_INFO* get_dyndns_system_table(void)
{
	return dns_system_table;
}

static void init_cmd_timer(int *counter,int *counter_init,int *cmd_check_period_ms,DYN_DNS_CMD *old_cmd,
						   int is_update_pending,DYN_DNS_CLIENT *p_self)
{
	/*pending means server side err but not dyndns2 server, or client side err.
	  pending and retry_later means server side err and dyndns2 server*/
	if (is_update_pending && !(p_self->retry_pending_off) && !(p_self->retry_later))

		*counter=p_self->retry_pending_interval / p_self->cmd_check_period;
	else
		if (!(p_self->retry_later))

			*counter=p_self->sleep_sec / p_self->cmd_check_period;
		else {
			p_self->retry_later=false;
			*counter=1800 / p_self->cmd_check_period;
		}

	*counter_init=*counter;

	*cmd_check_period_ms = p_self->cmd_check_period * 1000;
	*old_cmd = p_self->cmd;
}

#ifdef USE_THREADS

static void dec_forced_update_count(DYN_DNS_CLIENT *p_self)
{

	unsigned	counter_store=p_self->forced_update_counter;
	double		timer_time;


	if (!(p_self->forced_update_counter))

		return;

	timer_time=get_timer(&update_timer);

	timer_time=round_up(timer_time/1000,p_self->forced_update_adjust);

	p_self->forced_update_counter-=((unsigned) (round_up((double) (timer_time/p_self->cmd_check_period),
		p_self->forced_update_adjust)));

	if (counter_store<p_self->forced_update_counter) /*overflow?*/

		p_self->forced_update_counter=0;
}

#endif

static RC_TYPE dyn_dns_wait_for_cmd(DYN_DNS_CLIENT *p_self)
{
	int		counter;
	int		counter_init;
	int		cmd_check_period_ms;
	DYN_DNS_CMD	old_cmd;


	init_cmd_timer(&counter,&counter_init,&cmd_check_period_ms,&old_cmd,is_global_update_pending,p_self);

	if (old_cmd != NO_CMD)
	{
		return RC_OK;
	}

#ifdef USE_THREADS
	/*keeping track of time out of this routine (for forced update period) --
	  a bit experimental, but appears unneccessary.  Easy enough to
	  remove though -- these two functions here, the restart below and in
	  do_update_alias_table, the create in init, and the destroy in main.
	  Compiler option?  :-)
	*/
	stop_timer(&update_timer);

	dec_forced_update_count(p_self);
#endif

	while(1)
	{

#ifdef _WIN32

		if (returnSignaled)

			break;
#endif

		if (p_self->cmd != old_cmd)

			break;

		if (p_self->forced_update_counter) /*unsigned*/

			p_self->forced_update_counter--;

		if (!(p_self->forced_update_counter)) {

			if (!(is_global_update_pending && !(p_self->retry_pending_off))) {

				/*
					If not retrying pendings, forced update retries fallback to update_period.

					So, this setup performs as inadyn if retry pendings off.  But, it will attempt 
					a forced update before update_period expiration if havn't tried since last update 
					and not doing pendings.  A bit overdone perhaps...the idea is to decouple ip 
					change checks (and associated interval) from required account maintenance ip 
					updates.  A failed update prompted by either update interval trigger will cause 
					mode shift to pending interval (15 minute default) if pending updates not explicitly 
					turned off.
				*/

				if (!(is_global_update_pending) || !(p_self->is_forced_update_attempted)) {

					if (!(p_self->is_global_bad_config)) {

						DBG_PRINTF((LOG_INFO,"I:DYNDNS: Command loop breaking for forced update...\n"));

						p_self->is_forced_update_attempted=true;

						break;
					}
				}
			}
		}

		os_sleep_ms(cmd_check_period_ms);

		if (!(--counter)) /*ip check*/

			break;
	}


#ifdef USE_THREADS
	/*keep track of time out of loop (for forced update period)*/

	if (p_self->forced_update_counter)

		restart_timer(&update_timer);
#endif

#ifdef _WIN32

	if (returnSignaled)

		p_self->cmd=CMD_STOP;
#endif

	return RC_OK;
}

/*
        events handler - callbacks from program wide
        events aggregator/sink, events.c.

        events from nix, windows signal handlers,
        windows RAS events, and windows service.
*/
void dyndns_event(int i_event,void  *p_context)
{

        DYN_DNS_CLIENT  *p_self=p_context;

        DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "Entered dyndns_event...\n"));

#ifdef _WIN32

        if (!(p_self->cmd==CMD_UPDT)) {

                /*Windows SCM event*/
                returnSignaled=true;
                is_online_thread_exit=true;
        }
#endif
        p_self->cmd=i_event;
}

static int get_req_for_dyndns_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt,DYNDNS_SYSTEM *p_sys_info)
{

	int bytes_stored=0;


	DYNDNS_ORG_SPECIFIC_DATA *p_dyndns_specific =
	    (DYNDNS_ORG_SPECIFIC_DATA*) p_sys_info->p_specific_data;

	memset(p_self->p_req_buffer,0,DYNDNS_HTTP_REQUEST_BUFFER_SIZE);

	bytes_stored=sprintf(p_self->p_req_buffer, DYNDNS_GET_MY_IP_HTTP_REQUEST_FORMAT,
	                     p_self->info[srv_cnt].dyndns_server_name.url,
	                     p_self->info[srv_cnt].alias_info.names[cnt].name,
	                     p_self->info[srv_cnt].alias_info.p_address[cnt]->my_ip_address.name[ip_store],
	                     DYNDNS_AGENT_NAME,
	                     p_self->info[srv_cnt].dyndns_server_host,
	                     p_self->info[srv_cnt].credentials.p_enc_usr_passwd_buffer);



	return bytes_stored;
}

/*Input afraid_rsp, freedns.afraid.org account's aliases, hashes, etcetera, returned via freedns.afraid.org http api.
  Output aliases, corresponding hashes, detected ipv, to DYNDNS_INFO_TYPE info encapsulated structure.

  Output structure can be subsequently used to find hash corresponding to alias, ipv, for server updates.
*/
static RC_TYPE store_afraid_hashes(DYNDNS_INFO_TYPE *info,char *afraid_rsp)
{
	int	i=0;
	char	*rsp_index;
	char	*field_end;
	int	is_new_line;

	while (1) {

		/*alias*/
		if (!(field_end=strstr(afraid_rsp,"|"))) {

			DBG_PRINTF((LOG_ERR,"E:DYNDNS: Invalid input in parse_afraid_hashes.  No alias field delimiter...\n"));

			return RC_DYNDNS_INVALID_OR_MISSING_PARAMETERS;
		}

		*field_end='\0';
		free(info->alias_info.svr_hashes[i].alias);
		info->alias_info.svr_hashes[i].alias=safe_malloc(strlen(afraid_rsp)+1);
		strcpy(info->alias_info.svr_hashes[i].alias,afraid_rsp);

		afraid_rsp=field_end+1;

		/*ipv*/
		field_end=strstr(afraid_rsp,"|");
		*field_end='\0';

		if (!(NULL==strchr(afraid_rsp,':'))) {

			info->alias_info.svr_hashes[i].ipv_enum=ip_6;
		}
		else
			if (!(NULL==strchr(afraid_rsp,'.')))

				info->alias_info.svr_hashes[i].ipv_enum=ip_4;
			else {
				DBG_PRINTF((LOG_ERR,"E:DYNDNS: Invalid input in parse_afraid_hashes.  Could not detect ip version for alias:"\
						"  %s...\n",info->alias_info.svr_hashes[i].alias));

				return RC_DYNDNS_INVALID_OR_MISSING_PARAMETERS;
			}

		afraid_rsp=field_end+1;

		/*hash*/
		if (!(rsp_index=strstr(afraid_rsp,"?"))) {

			DBG_PRINTF((LOG_ERR,"E:DYNDNS: Invalid input in parse_afraid_hashes.  No hash for alias:  %s:%s...\n",
					info->alias_info.svr_hashes[i].alias,ip_enum_to_str(info->alias_info.svr_hashes[i].ipv_enum)));

			return RC_DYNDNS_INVALID_OR_MISSING_PARAMETERS;
		}

		rsp_index++;

		if (!(field_end=strstr(rsp_index,"\n"))) {

			is_new_line=0;
		}
		else {
			*field_end='\0';

			is_new_line=1;
		}

		free(info->alias_info.svr_hashes[i].str);
		info->alias_info.svr_hashes[i].str=safe_malloc(strlen(rsp_index)+1);
		strcpy(info->alias_info.svr_hashes[i].str,rsp_index);

		DBG_PRINTF((LOG_INFO,"I:DYNDNS: freedns.afraid HASH parse:  %s:%s, %s...\n",info->alias_info.svr_hashes[i].alias,
				ip_enum_to_str(info->alias_info.svr_hashes[i].ipv_enum),info->alias_info.svr_hashes[i].str));

		if (!(is_new_line))

			break;

		afraid_rsp=field_end+1;

		i++;
	}

	info->alias_info.svr_hashes_count=i+1;

	return RC_OK;
}

int is_user_pass(char *username,char *password)
{
	int	userlen;
	int	passlen;


	if (!(username && password))

		return 0;

	userlen=strlen(username);
	passlen=strlen(password);

	if (!(passlen && userlen))

		return 0;

	return (userlen+passlen+1);
}

/*Use freedns.afraid.org's DDNS client api to http get aliases, corresponding hashes, and detect ipv.
*/
static RC_TYPE fetch_afraid_hashes(DYNDNS_INFO_TYPE *info,char **hashes_str)
{
	HTTP_CLIENT		hash_client;
	HTTP_TRANSACTION	hash_trans;
	char			*req_url;
	int			req_buf_len;
	unsigned char		sha1_hash[20];
	char			sha1_hash_str[41];
	int			creds_len;
	char			*creds;
	int			i;
	RC_TYPE			rc=RC_OK;


	*hashes_str=NULL;


	if (!(creds_len=is_user_pass(info->credentials.my_username,info->credentials.my_password)))

		return RC_DYNDNS_INVALID_OR_MISSING_PARAMETERS;

	/*get freedns.afraid.org account username, password sha1 hash*/
	creds=safe_malloc(creds_len+1);

	sprintf(creds,"%s|%s",info->credentials.my_username,info->credentials.my_password);

	sha1((unsigned char *) creds,creds_len,sha1_hash);

	free(creds);

	for (i=0;i<20;i++)

		sprintf(&sha1_hash_str[i*2],"%02x",sha1_hash[i]);

	DBG_PRINTF((LOG_INFO,"I:DYNDNS: SHA1 HASH:  %s...\n",sha1_hash_str));

	/*prepare, and http transact with freedns.afraid.org DDNS client api*/
	memset(&hash_client,0,sizeof(HTTP_CLIENT));

	http_client_construct(&hash_client);

	dyn_dns_set_dyndns_server_params(&hash_client,info);

	req_url=safe_malloc(strlen("/api/?action=getdyndns&v=2&sha=")+strlen(sha1_hash_str)+1);

	req_buf_len=sprintf(req_url,"/api/?action=getdyndns&v=2&sha=%s",sha1_hash_str)+strlen(DYNDNS_HTTP_REQUEST_FORMAT)+
				strlen(info->dyndns_server_host);

	http_transaction_construct(&hash_trans,req_buf_len+1,DYNDNS_HTTP_REQUEST_FORMAT,DYNDNS_HTTP_RESPONSE_BUFFER_SIZE,
					req_url,DYNDNS_AGENT_NAME , info->dyndns_server_host);

	DBG_PRINTF((LOG_INFO,"I:DYNDNS: freedns_afraid.org hashes request:\n%s\n",hash_trans.p_req));

	/*error check init/transaction - may want these in separate function*/
	if ((RC_OK==(rc=http_client_init_and_connect(&hash_client)))) {

		if ((RC_OK==(rc=http_client_transaction(&hash_client,&hash_trans)))) {

			DBG_PRINTF((LOG_INFO,"I:DYNDNS: freedns_afraid.org hashes request response: \n%s\n\n", hash_trans.p_rsp));

			/*copy server response to function output*/
			*hashes_str=safe_malloc(strlen(hash_trans.p_rsp)+1);
			strcpy(*hashes_str,hash_trans.p_rsp);
		}
	}

	/*clean up*/
	free(req_url);

	http_transaction_destruct(&hash_trans);
	http_client_destruct(&hash_client);

	return rc;
}

RC_TYPE fetch_and_store_afraid_hashes(DYNDNS_INFO_TYPE *info)
{
	char 	*hashes;
	RC_TYPE	rc=RC_OK;


	if (RC_OK==(rc=fetch_afraid_hashes(info,&hashes))) {

		if (RC_OK==(rc=store_afraid_hashes(info,hashes)))

			info->alias_info.is_got_svr_hashes=1;

		free(hashes);
	}

	return rc;
}

RC_TYPE get_afraid_hash(char **hash,char *alias,DYNDNS_IPV ip_v,DYNDNS_INFO_TYPE *info)
{
	int     i;


	for (i=0;i<info->alias_info.svr_hashes_count;i++) {

		if (0==strcmp(alias,info->alias_info.svr_hashes[i].alias))

			if (ip_v==info->alias_info.svr_hashes[i].ipv_enum) {

				*hash=info->alias_info.svr_hashes[i].str;

				return RC_OK;
			}
	}

	return RC_ERROR;
}

static int get_req_for_freedns_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt, DYNDNS_SYSTEM *p_sys_info)
{
	(void)p_sys_info;

	if (!(p_self->info[srv_cnt].alias_info.hashes[cnt].str)) {

		DBG_PRINTF((LOG_ERR,"E:" MODULE_TAG "No hash for %s, in get_req_for_freedns_server...\n",
				p_self->info[srv_cnt].alias_info.names[cnt].name));

		return 0;
	}

	return sprintf(p_self->p_req_buffer, FREEDNS_UPDATE_MY_IP_REQUEST_FORMAT,
	               p_self->info[srv_cnt].dyndns_server_name.url,
	               p_self->info[srv_cnt].alias_info.hashes[cnt].str,
	               p_self->info[srv_cnt].alias_info.p_address[cnt]->my_ip_address.name[ip_store],
	               p_self->info[srv_cnt].dyndns_server_host);
}


static int get_req_for_noip_http_dns_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt,  DYNDNS_SYSTEM *p_sys_info)
{
	(void)p_sys_info;
	return sprintf(p_self->p_req_buffer, GENERIC_NOIP_AUTH_MY_IP_REQUEST_FORMAT_NEW,
	               p_self->info[srv_cnt].dyndns_server_name.url,
	               p_self->info[srv_cnt].alias_info.names[cnt].name,
	               p_self->info[srv_cnt].credentials.p_enc_usr_passwd_buffer,
	               p_self->info[srv_cnt].dyndns_server_host);
}

static int get_req_for_dlink_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt,  DYNDNS_SYSTEM *p_sys_info)
{
	(void)p_sys_info;
	return sprintf(p_self->p_req_buffer, DLINKDDNS_GET_MY_IP_HTTP_REQUEST_FORMAT_NEW,
	               p_self->info[srv_cnt].dyndns_server_name.url,
	               p_self->info[srv_cnt].alias_info.names[cnt].name,
					 "ON",
	               p_self->info[srv_cnt].credentials.p_enc_usr_passwd_buffer,
	               p_self->info[srv_cnt].dyndns_server_host);
}

static int get_req_for_easydns_http_dns_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt,  DYNDNS_SYSTEM *p_sys_info)
{
	(void)p_sys_info;
	return sprintf(p_self->p_req_buffer, GENERIC_EASYDNS_AUTH_MY_IP_REQUEST_FORMAT,
	               p_self->info[srv_cnt].dyndns_server_name.url,
	               p_self->info[srv_cnt].alias_info.names[cnt].name,
	               p_self->info[srv_cnt].alias_info.p_address[cnt]->my_ip_address.name[ip_store],
	               p_self->info[srv_cnt].wildcard ? "ON" : "OFF",
	               p_self->info[srv_cnt].credentials.p_enc_usr_passwd_buffer,
	               p_self->info[srv_cnt].dyndns_server_host);
}

static int get_req_for_tzo_http_dns_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt, DYNDNS_SYSTEM *p_sys_info)
{
	(void)p_sys_info;

	if (p_self == NULL)
	{
		/* 0 == "No characters written" */
		return 0;
	}

	return sprintf(p_self->p_req_buffer, GENERIC_TZO_AUTH_MY_IP_REQUEST_FORMAT,
		       p_self->info[srv_cnt].dyndns_server_name.url,
		       p_self->info[srv_cnt].alias_info.names[cnt].name,
		       p_self->info[srv_cnt].credentials.my_username,
		       p_self->info[srv_cnt].credentials.my_password,
		       p_self->info[srv_cnt].alias_info.p_address[cnt]->my_ip_address.name[ip_store],
		       p_self->info[srv_cnt].dyndns_server_host);
}

static int get_req_for_sitelutions_http_dns_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt,  DYNDNS_SYSTEM *p_sys_info)
{

	(void)p_sys_info;
	return sprintf(p_self->p_req_buffer, SITELUTIONS_GET_MY_IP_HTTP_REQUEST_FORMAT,
	               p_self->info[srv_cnt].dyndns_server_name.url,
	               p_self->info[srv_cnt].credentials.my_username,
	               p_self->info[srv_cnt].credentials.my_password,
	               p_self->info[srv_cnt].alias_info.names[cnt].name,
		       p_self->info[srv_cnt].alias_info.p_address[cnt]->my_ip_address.name[ip_store],
	               p_self->info[srv_cnt].dyndns_server_host);
}

static int get_req_for_he_ipv6_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt, DYNDNS_SYSTEM *p_sys_info)
{
	unsigned char	digestbuf[MD5_DIGEST_BYTES];
	char		digeststr[MD5_DIGEST_BYTES*2+1];
	int		i;

	(void)p_sys_info;


	memset(&digestbuf,0,MD5_DIGEST_BYTES);
	memset(&digeststr,0,MD5_DIGEST_BYTES*2+1);


	if (p_self == NULL)
	{
		/* 0 == "No characters written" */
		return 0;
	}

	md5_buffer(p_self->info[srv_cnt].credentials.my_password,
		   strlen(p_self->info[srv_cnt].credentials.my_password), digestbuf);

	for (i = 0; i < MD5_DIGEST_BYTES; i++)
		sprintf(&digeststr[i*2], "%02x", digestbuf[i]);

	return sprintf(p_self->p_req_buffer, HE_IPV6TB_UPDATE_MY_IP_REQUEST_FORMAT,
				p_self->info[srv_cnt].dyndns_server_name.url,
				p_self->info[srv_cnt].alias_info.p_address[cnt]->my_ip_address.name[ip_store],
				p_self->info[srv_cnt].credentials.my_username,
				digeststr,
				p_self->info[srv_cnt].alias_info.names[cnt].name,
				p_self->info[srv_cnt].dyndns_server_host);
}

static int get_req_for_he_dns_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt,  DYNDNS_SYSTEM *p_sys_info)
{

	(void)p_sys_info;

	return sprintf(p_self->p_req_buffer, HE_DEFAULT_UPDATE_MY_IP_REQUEST_FORMAT,
	               p_self->info[srv_cnt].dyndns_server_name.url,
				   p_self->info[srv_cnt].alias_info.names[cnt].name,
				   p_self->info[srv_cnt].alias_info.p_address[cnt]->my_ip_address.name[ip_store],
				   p_self->info[srv_cnt].credentials.p_enc_usr_passwd_buffer,
				   p_self->info[srv_cnt].dyndns_server_host);
}

static int get_req_for_dhis_http_dns_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt,  DYNDNS_SYSTEM *p_sys_info)
{

	(void)p_sys_info;

	return sprintf(p_self->p_req_buffer, DHIS_MY_IP_UPDATE_REQUEST_FORMAT,
	               p_self->info[srv_cnt].dyndns_server_name.url,
				   p_self->info[srv_cnt].alias_info.names[cnt].name,
				   p_self->info[srv_cnt].credentials.my_password,
				   p_self->info[srv_cnt].alias_info.p_address[cnt]->my_ip_address.name[ip_store],
				   p_self->info[srv_cnt].dyndns_server_host);
}

static int get_req_for_zerigo_http_dns_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt,  DYNDNS_SYSTEM *p_sys_info)
{

	(void)p_sys_info;

	return sprintf(p_self->p_req_buffer, ZERIGO_MY_IP_UPDATE_REQUEST_FORMAT,
	               p_self->info[srv_cnt].dyndns_server_name.url,
				   p_self->info[srv_cnt].alias_info.names[cnt].name,
				   p_self->info[srv_cnt].alias_info.p_address[cnt]->my_ip_address.name[ip_store],
				   p_self->info[srv_cnt].credentials.p_enc_usr_passwd_buffer,
				   p_self->info[srv_cnt].dyndns_server_host);
}

static int get_req_for_twodns_http_dns_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt,  DYNDNS_SYSTEM *p_sys_info)
{

	(void)p_sys_info;

	return sprintf(p_self->p_req_buffer, TWODNS_MY_IP_UPDATE_REQUEST_FORMAT,
	               p_self->info[srv_cnt].dyndns_server_name.url,
				   p_self->info[srv_cnt].alias_info.names[cnt].name,
				   p_self->info[srv_cnt].alias_info.p_address[cnt]->my_ip_address.name[ip_store],
				   p_self->info[srv_cnt].credentials.p_enc_usr_passwd_buffer,
				   p_self->info[srv_cnt].dyndns_server_host);
}

static int get_req_for_ip_server(DYN_DNS_CLIENT *p_self, IP_ADDRESS *p_address,void *p_specific_data)
{
	(void)p_specific_data;

	return sprintf(p_self->p_req_buffer,DYNDNS_HTTP_REQUEST_FORMAT,p_address->ip_server_url,DYNDNS_AGENT_NAME,p_address->ip_server_host);
}

static int get_req_for_generic_http_dns_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int cnt, DYNDNS_SYSTEM *p_sys_info)
{
	(void)p_sys_info;
	return sprintf(p_self->p_req_buffer, GENERIC_DNS_BASIC_AUTH_MY_IP_REQUEST_FORMAT,
	               p_self->info[srv_cnt].dyndns_server_name.url,
	               p_self->info[srv_cnt].alias_info.names[cnt].name,
	               p_self->info[srv_cnt].credentials.p_enc_usr_passwd_buffer,
	               p_self->info[srv_cnt].dyndns_server_host);
}

/*Build a string of url params/values using p_self->ip_param values (ip_param, id_param, pw_param).
*/
static char *ip_param_mapped(struct EXP_MAPPING *p_mapping,char *ip_param,char *id_param,char *pw_param,DYNDNS_CREDENTIALS credentials,
				char *alias_name,DYNDNS_MY_IP my_ip_address)
{

	char	*mapped;	/*return mapped portion of url to be appended to actual url*/
	char	*tmp;
	int	mapped_len=0;

	mapped=safe_malloc(1); /*don't return null*/

	if (!(p_mapping->is_alias)) {

		mapped_len=strlen(alias_name);
		mapped=safe_realloc(mapped,mapped_len+1);
		strcpy(mapped,alias_name);
	}

	if (!(ip_param))

		return mapped;

	mapped_len+=strlen(ip_param)+strlen(my_ip_address.name[ip_store])+2;
	mapped=safe_realloc(mapped,mapped_len+1);

	if (p_mapping->is_alias)

		sprintf(mapped,"&%s=%s",ip_param,my_ip_address.name[ip_store]);
	else {
		tmp=safe_malloc(strlen(mapped)+1);
		strcpy(tmp,mapped);

		sprintf(mapped,"%s&%s=%s",tmp,ip_param,my_ip_address.name[ip_store]);

		free(tmp);
	}


	if (id_param) {

		tmp=safe_malloc(strlen(mapped)+1);
		strcpy(tmp,mapped);

		mapped_len+=strlen(id_param)+strlen(credentials.my_username)+2;
		mapped=safe_realloc(mapped,mapped_len+1);
		sprintf(mapped,"%s&%s=%s",tmp,id_param,credentials.my_username);

		free(tmp);
	}

	if (pw_param) {

		tmp=safe_malloc(strlen(mapped)+1);
		strcpy(tmp,mapped);

		mapped_len+=strlen(pw_param)+strlen(credentials.my_password)+2;
		mapped=safe_realloc(mapped,mapped_len+1);
		sprintf(mapped,"%s&%s=%s",tmp,pw_param,credentials.my_password);

		free(tmp);
	}

	return mapped;
}

int get_req_for_generic_custom_http_dns_server(DYN_DNS_CLIENT *p_self, struct EXP_MAPPING *p_mapping, char srv_cnt, int alias_index, DYNDNS_SYSTEM *p_sys_info)
{
	int	buff_len;
	char	*p_param_mapped;

	(void)	p_sys_info;

	/*Can accomodate url parameter names with either ip_param (overloaded (and subsequently parsed)
	  <ip param name> [user param name] [pass param name]) or with expandable scripting like vars
	  for same and alias name.*/

	/*tag url with alias, and any ip_param values*/
	p_param_mapped=ip_param_mapped(p_mapping,p_self->info[srv_cnt].ip_param,p_self->info[srv_cnt].id_param,p_self->info[srv_cnt].pw_param,
					p_self->info[srv_cnt].credentials,p_self->info[srv_cnt].alias_info.names[alias_index].name,
					p_self->info[srv_cnt].alias_info.p_address[alias_index]->my_ip_address);

	DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "ip_param_mapped, ip_param mapped to:  %s...\n",p_param_mapped));

	/*now pivot in terms of whether basic auth or user, pass params*/

	/*be backward compatible relative to new ip_param option*/
	if (!(p_self->info[srv_cnt].id_param || p_self->info[srv_cnt].pw_param || p_mapping->is_username || p_mapping->is_password))

		buff_len=sprintf(p_self->p_req_buffer, GENERIC_DNS_P_MAPPED_BASIC_AUTH_MY_IP_REQUEST_FORMAT,
				p_mapping->p_exp_url,
				p_param_mapped,
				p_self->info[srv_cnt].credentials.p_enc_usr_passwd_buffer,
				p_self->info[srv_cnt].dyndns_server_host);
	else {

		DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "NOT BASIC AUTH.  url param:  %s%s...\n",p_mapping->p_exp_url,p_param_mapped));

		buff_len=sprintf(p_self->p_req_buffer, GENERIC_DNS_CUSTOM_URL_P_PARAM_MY_IP_REQUEST_FORMAT,
				p_mapping->p_exp_url,
				p_param_mapped,
				p_self->info[srv_cnt].dyndns_server_host);
	}

	free(p_param_mapped);

	return buff_len;
}

static int get_req_for_http_dns_server(DYNDNS_SYSTEM *p_sys_info, struct DYN_DNS_CLIENT *p_self,char srv_cnt, int cnt)
{

	char                    *s_exp_url;
	struct EXP_MAPPING      *p_mapping=safe_malloc(sizeof(struct EXP_MAPPING));
	int			ret;

	p_mapping->p_dyndns=p_self;
	p_mapping->server_index=srv_cnt;
	p_mapping->alias_index=cnt;


	DBG_PRINTF((LOG_NOTICE,"N:" MODULE_TAG "expanding url in get_req_for_http_dns_server:  %s\n",vars_expanded_str(arg_to_val,p_mapping,&s_exp_url,p_self->info[srv_cnt].dyndns_server_name.url)));

	p_mapping->p_exp_url=s_exp_url;

	ret=p_sys_info->p_dns_update_req_func(p_self,p_mapping,srv_cnt,cnt,(struct DYNDNS_SYSTEM *) p_sys_info);

	free(s_exp_url);
	free(p_mapping);

	return ret;
}

/******************************
  Server response handlers.  Many I've added check for try later response, eventhough in many cases the server
  never returns it - guess I'm optimistic, or it's sort of a place holder.

  is_generic_server_rsp_ok, inverts the sense of config list codes, such that, matches there are NOT fatal, and
  so no match in success list or config list IS fatal.  Eventually all will be converted to the inversion - I'm
  not as paranoid (or ignorant) about TCP as I was.
*/
static RC_TYPE is_dyndns_server_rsp_ok( DYN_DNS_CLIENT *p_self, int alias_index, char *p_rsp, char *p_ok_string,char srv_cnt)
{
	/*fail on (badauth, nohost, notfqdn, !yours, etc)*/

	(void) p_ok_string;

	if (strstr(p_rsp,"good") || strstr(p_rsp,"nochg"))

		return RC_OK;

	if (strstr(p_rsp,"dnserr") || strstr(p_rsp,"911"))

		return RC_DYNDNS_SERVER_ERROR;

	return RC_DYNDNS_RSP_CONFIG;
}

/* Freedns afraid.org.specific response validator.
    ok blabla and n.n.n.n
    fail blabla and n.n.n.n
    are the good answers. We search our own IP address in response and that's enough.
*/
static RC_TYPE is_freedns_server_rsp_ok( DYN_DNS_CLIENT *p_self, int alias_index, char *p_rsp, char *p_ok_string,char srv_cnt)
{

	if ((!(strstr(p_rsp, "ERROR")) && strstr(p_rsp, p_self->info[srv_cnt].alias_info.p_address[alias_index]->my_ip_address.name[ip_store]))
		|| strstr(p_rsp, "has not changed"))

		return RC_OK;

	if (strstr(p_rsp,"dnserr") || strstr(p_rsp,"911"))

		return RC_DYNDNS_SERVER_ERROR;

	return RC_DYNDNS_RSP_CONFIG;
}

/**
	the OK codes are:
		CODE=200
		CODE=707, for duplicated updates
*/
static RC_TYPE is_zoneedit_server_rsp_ok( DYN_DNS_CLIENT *p_self, int alias_index, char *p_rsp, char *p_ok_string,char srv_cnt)
{
	(void) p_ok_string;

	if ((strstr(p_rsp, "CODE=\"200\"") != NULL) || (strstr(p_rsp, "CODE=\"707\"") != NULL)
			 || (strstr(p_rsp, "CODE=\"201\"") != NULL))

		return RC_OK;

	if (strstr(p_rsp,"dnserr") || strstr(p_rsp,"911"))

		return RC_DYNDNS_SERVER_ERROR;

	if (strstr(p_rsp, "CODE=\"703\"") || strstr(p_rsp, "CODE=\"707\"")
			|| strstr(p_rsp, "CODE=\"704\"") || strstr(p_rsp, "CODE=\"701\"")
			|| strstr(p_rsp, "CODE=\"705\"") || strstr(p_rsp, "CODE=\"708\""))

		return RC_DYNDNS_RSP_CONFIG;

	return RC_DYNDNS_RSP_NOTOK;
}

/**
	NOERROR is the OK code here
*/
static RC_TYPE is_easydns_server_rsp_ok( DYN_DNS_CLIENT *p_self, int alias_index, char *p_rsp, char *p_ok_string,char srv_cnt)
{
	if (strstr(p_rsp, "NOERROR"))

		return RC_OK;

	if (strstr(p_rsp,"dnserr") || strstr(p_rsp,"911"))

		return RC_DYNDNS_SERVER_ERROR;

	if (strstr(p_rsp, "NOACCESS") || strstr(p_rsp, "NOSERVICE") || strstr(p_rsp, "ILLEGAL INPUT")
                         || strstr(p_rsp, "TOOSOON"))

		return RC_DYNDNS_RSP_CONFIG;

	return RC_DYNDNS_RSP_NOTOK;
}

static RC_TYPE is_sitelutions_server_rsp_ok( DYN_DNS_CLIENT *p_self, int alias_index, char *p_rsp, char* p_ok_string,char srv_cnt)
{

	if (strstr(p_rsp,"success"))

		return RC_OK;

	if (strstr(p_rsp,"dnserr") || strstr(p_rsp,"911"))

		return RC_DYNDNS_SERVER_ERROR;

	if (strstr(p_rsp, "noauth") || strstr(p_rsp, "invalid ip") || strstr(p_rsp, "invalid ttl")
                         || strstr(p_rsp, "no record")  || strstr(p_rsp, "not owner"))

		return RC_DYNDNS_RSP_CONFIG;

	return RC_DYNDNS_RSP_NOTOK;
}

/* HE ipv6 tunnelbroker specific response validator.
   own IP address and 'already in use' are the good answers.
*/
static RC_TYPE is_he_ipv6_server_rsp_ok(DYN_DNS_CLIENT *p_self, int alias_index, char *p_rsp, char *p_ok_string,char srv_cnt)
{
	(void)p_ok_string;

	if (strstr(p_rsp, p_self->info[srv_cnt].alias_info.p_address[alias_index]->my_ip_address.name[ip_store]) || strstr(p_rsp,"already in use"))

		return RC_OK;

	if (strstr(p_rsp,"dnserr") || strstr(p_rsp,"911"))

		return RC_DYNDNS_SERVER_ERROR;

	return RC_DYNDNS_RSP_CONFIG;
}

/* TZO specific response validator.
   If we have an HTTP 302 the update wasn't good and we're being redirected
*/
static RC_TYPE is_tzo_server_rsp_ok(DYN_DNS_CLIENT *p_self, int alias_index, char *p_rsp, char *p_ok_string,char srv_cnt)
{

	if (strstr(p_rsp,"200") || strstr(p_rsp,"304"))

		return RC_OK;

	if (strstr(p_rsp,"dnserr") || strstr(p_rsp,"911"))

		return RC_DYNDNS_SERVER_ERROR;

	if (strstr(p_rsp, "480") || strstr(p_rsp, "405")  || strstr(p_rsp, "401")
                        || strstr(p_rsp, "403") || strstr(p_rsp, "414") || strstr(p_rsp, "405")
                        || strstr(p_rsp, "407") || strstr(p_rsp, "415"))

		return RC_DYNDNS_RSP_CONFIG;

	return RC_DYNDNS_RSP_NOTOK;
}

static RC_TYPE is_dhis_server_rsp_ok( DYN_DNS_CLIENT *p_self, int alias_index, char *p_rsp, char* p_ok_string,char srv_cnt)
{

	if (strstr(p_rsp,p_self->info[srv_cnt].alias_info.p_address[alias_index]->my_ip_address.name[ip_store]))

		return RC_OK;

	if (strstr(p_rsp,"dnserr") || strstr(p_rsp,"911"))

		return RC_DYNDNS_SERVER_ERROR;

	if (strstr(p_rsp,"Authorization Required"))

		return RC_DYNDNS_RSP_CONFIG;

	return RC_DYNDNS_RSP_NOTOK;
}

static RC_TYPE is_zerigo_server_rsp_ok( DYN_DNS_CLIENT *p_self, int alias_index, char *p_rsp, char* p_ok_string,char srv_cnt)
{
	if (strstr(p_rsp,"Status: 200"))

		return RC_OK;

	if (strstr(p_rsp,"dnserr") || strstr(p_rsp,"911"))

		return RC_DYNDNS_SERVER_ERROR;

	if (strstr(p_rsp,"Status: 400") || strstr(p_rsp,"Status: 403") || strstr(p_rsp,"Status: 404"))

		return RC_DYNDNS_RSP_CONFIG;

	return RC_DYNDNS_RSP_NOTOK;
}

static BOOL is_rsp_in_list(char* haystack,gen_list rsp_list,DYN_DNS_CLIENT *p_dyndns,int srv_index,int alias_index)
{
	gen_list_itr		itr;
	char			*s_exp;
	struct EXP_MAPPING	*p_mapping=safe_malloc(sizeof(struct EXP_MAPPING));

	p_mapping->p_dyndns=p_dyndns;
	p_mapping->server_index=srv_index;
	p_mapping->alias_index=alias_index;

	itr=list_get_itr(rsp_list);
	list_begin(rsp_list,itr);

	do {

		DBG_PRINTF((LOG_NOTICE,"N:" MODULE_TAG "is_rsp_in_list checking list param:  %s\n",vars_expanded_str(arg_to_val,p_mapping,&s_exp,list_entry_item(itr))));

		if (strstr(haystack,s_exp)) {

			free(s_exp);

			free(p_mapping);

			return TRUE;
		}

		list_next(itr);

		free(s_exp);
	}
	while (!(list_end(itr)));

	free(p_mapping);

	return FALSE;
}

/*Evaluate DDNS server's response.  If have corresponding response list, check response for match in success codes list,
  and config list (or rather, not config list).  No match in either list, then it's a fatal update failure - i.e., config.
  That is, match in config list means try later, error other than config.

  Be backward compatible and check p_ok_string, if don't have success codes list, or ip in rsp not indicated.

  Returns RC_TYPE value, one of:

	RC_OK,
	RC_DYNDNS_RSP_NOTOK,
	RC_DYNDNS_SERVER_ERROR,
	RC_DYNDNS_RSP_CONFIG
*/
RC_TYPE is_generic_server_rsp_ok( DYN_DNS_CLIENT *p_self, int alias_index, char *p_rsp, char *p_ok_string, char srv_cnt)
{
	DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "entered is_generic_server_rsp_ok...\n"));

	if (!(p_self->info[srv_cnt].rsp_success_codes)) {

		DBG_PRINTF((LOG_NOTICE,"N:" MODULE_TAG "No svr_rsp_success set in generic server response check...\n"));
	}
	else {
		if (!(p_self->info[srv_cnt].is_ip_in_svr_rsp)) {

			if (is_rsp_in_list(p_rsp,p_self->info[srv_cnt].rsp_success_codes,p_self,srv_cnt,alias_index))

				return RC_OK;
		}
		else {
			if (is_rsp_in_list(p_rsp,p_self->info[srv_cnt].rsp_success_codes,p_self,srv_cnt,alias_index) &&
						strstr(p_rsp, p_self->info[srv_cnt].alias_info.p_address[alias_index]->my_ip_address.name[ip_store]))

				return RC_OK;
		}
	}

	if (p_self->info[srv_cnt].is_ip_in_svr_rsp) {

		if (strstr(p_rsp,p_self->info[srv_cnt].alias_info.p_address[alias_index]->my_ip_address.name[ip_store]))

			return RC_OK;
	}

	DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "Update failed, or no svr_rsp_success set.  Checking server rsp config...\n"));

	/*not sure whether will add separate list for non-config
	  related update errors that are NOT dyndns2/retry later -
	  so, for now, this stays, eventhough should be covered
	  by svr_rsp_other list (rsp_config_codes)*/
	if ((strstr(p_rsp,"dnserr") || strstr(p_rsp,"911") ||   		/*dyndns2*/
				strstr(p_rsp,"STATUS_UNKNOWN_ERROR"))) {	/*regfish.de*/

		DBG_PRINTF((LOG_NOTICE,"N:" MODULE_TAG "Server error.  Pausing a bit before reattempt...\n"));

		return RC_DYNDNS_SERVER_ERROR;
	}

	/*actual failed update?  or no values to check?*/
	if (!(p_self->info[srv_cnt].rsp_success_codes) && !(p_self->info[srv_cnt].is_ip_in_svr_rsp)) {

		DBG_PRINTF((LOG_NOTICE,"N:" MODULE_TAG "No update success check values set in generic server response check...\n"));

		/*custom dyndns_system back compat*/
		if (p_ok_string)

			if (strstr(p_rsp,p_ok_string))

				return RC_OK;
			else
				return RC_DYNDNS_RSP_NOTOK;
	}

	/*svr_rsp_other list may contain codes not meant
	  as 'try later', but only to indicate not config,
	  so may need two separate lists as these could
	  indicate simply RSP_NOTOK - as is, it's a MUST
	  to recognize codes not success and not config,
	  so merely 'pass through', so with only one
	  list, all such are try later.*/
	if (!(p_self->info[srv_cnt].rsp_config_codes)) {

		DBG_PRINTF((LOG_NOTICE,"N:" MODULE_TAG "No svr_rsp_other set in generic server response check...\n"));
	}
	else {
		if (is_rsp_in_list(p_rsp,p_self->info[srv_cnt].rsp_config_codes,p_self,srv_cnt,alias_index)) {

			DBG_PRINTF((LOG_NOTICE,"N:" MODULE_TAG "Server error.  Pausing a bit before reattempt...\n"));

			return RC_DYNDNS_SERVER_ERROR;
		}
	}

	return RC_DYNDNS_RSP_CONFIG;
}

static RC_TYPE update_ip_addr_store(IP_ADDRESS *p_address,char *p_ip_str)
{
	DYNDNS_IPV	ip_enum;
	int		family;
	void		*in_addr;
	int		sock_size;
	unsigned char	ip_str_to_ip[16];


	if (!(strstr(p_ip_str,":"))) {

		ip_enum=ip_4;
		family=AF_INET;
		in_addr=&(((struct sockaddr_in*) &p_address->net_addr[ip_enum])->sin_addr);
		sock_size=sizeof(struct in_addr);
	}
	else {
		ip_enum=ip_6;
		family=AF_INET6;
		in_addr=&(((struct sockaddr_in6*) &p_address->net_addr[ip_enum])->sin6_addr);
		sock_size=sizeof(struct in6_addr);
	}

	inet_pton_w(family,p_ip_str,ip_str_to_ip);

	memcpy(in_addr,ip_str_to_ip,sock_size);
	strcpy(p_address->my_ip_address.name[ip_enum],p_ip_str);

	/*put currently operated upon here -- for auto ip type too*/
	strcpy(p_address->my_ip_address.name[ip_store],p_ip_str);
	memcpy(&p_address->net_addr[ip_store],&p_address->net_addr[ip_enum],sizeof(struct sockaddr_storage));


	return RC_OK;
}

/*
    Note:
        it updates the flag: info->'my_ip_has_changed' if the old address was different
*/
static RC_TYPE do_check_my_ip_address(IP_ADDRESS *p_address,DYN_DNS_CLIENT *p_self)
{

	RC_TYPE		rc=RC_OK;
	char		*p_current_str=p_self->http_tr.p_rsp;
	char		*p_ip_str;



	if (p_self->http_tr.rsp_len <= 0 || p_self->http_tr.p_rsp == NULL) {

		return RC_INVALID_POINTER;
	}

	if (!(RC_OK==(rc=parse_ip_address(&p_ip_str,p_current_str)))) {

		return rc;
	}
	else {

		update_ip_addr_store(p_address,p_ip_str);
		free(p_ip_str);

		return RC_OK;
	}
}

static RC_TYPE check_if_ipv6_addr(IP_ADDRESS *p_address,struct sockaddr_storage *net_addr)
{

	char	ip_str[40];


	inet_ntop_w(AF_INET6,&(((struct sockaddr_in6*) net_addr)->sin6_addr),ip_str,40);

	return update_ip_addr_store(p_address,ip_str);
}

/*
	Send req to IP server and get the response

	12/2010 - bhoover@wecs.com
	extract ip address from response
	iterate over any fallback addresses
	break on first success
*/
static RC_TYPE check_my_ip_address(IP_ADDRESS *p_address,DYN_DNS_CLIENT *p_self)
{
	RC_TYPE			rc;
	struct sockaddr_storage	*if_addr=NULL;


	/*return good if at least one transaction*/

	rc=RC_ERROR;

	do
	{
		{
			int			i=0;
			HTTP_TRANSACTION	*p_tr = &p_self->http_tr;
			struct addrinfo		*addr;
			BOOL			is_got_ip4=p_address->is_got_ip4;
			BOOL			is_got_ip6=p_address->is_got_ip6;
			BOOL			is_got_afinet=false;

			if (p_address->iface_name) {

				DBG_PRINTF((LOG_NOTICE,"N:" MODULE_TAG "Getting interface (\"%s\") address, address pref (\"%s\") "\
						"in check_my_ip_address...\n",p_address->iface_name,p_address->addr_pref.addr_pref));

				if ((if_addr=if_address(p_address->iface_name,AF_INET6,&p_address->addr_pref)))

					if (((p_address->is_update_ip6 || p_address->is_update_auto) && !(p_address->is_got_ip6))) {

						DBG_PRINTF((LOG_NOTICE,"N:" MODULE_TAG "Checking bound interface IPv6 address...\n"));

						if (RC_OK==(rc=check_if_ipv6_addr(p_address,if_addr))) {

							p_address->is_got_ip6=is_got_ip6=true;

							DBG_PRINTF((LOG_NOTICE,"N:DYNDNS: My IP address: %s\n", p_address->my_ip_address.name[ip_store]));

							if (!(p_address->is_update_ip4) || p_address->is_got_ip4)

								break;
						}
					}
			}

			http_client_set_is_ip(&p_address->http_to_ip_server,((p_address->is_update_ip4 && !(is_got_ip4)) ||
						p_address->is_update_auto),((p_address->is_update_ip6 && !(is_got_ip6)) ||
						p_address->is_update_auto));

			if (!(RC_OK==(rc=http_client_init_all(&p_address->http_to_ip_server))))

				break;

			DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "Initialized http_to_ip_server...\n"));

			p_tr->req_len = get_req_for_ip_server(p_self,p_address,NULL);

			DBG_PRINTF((LOG_NOTICE,"N:" MODULE_TAG "The request for IP server:\n%s\n",p_self->p_req_buffer));

			DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "Entering Loop.  Got %d sockets...\n",p_address->http_to_ip_server.super.super.server_socket_count));

			http_transaction_construct(p_tr,p_tr->req_len+1,p_self->p_req_buffer,DYNDNS_HTTP_RESPONSE_BUFFER_SIZE);

			for (i=0;i<p_address->http_to_ip_server.super.super.server_socket_count;i++,p_address->http_to_ip_server.super.super.sock_index++) {

				memset(p_tr->p_rsp,0,DYNDNS_HTTP_RESPONSE_BUFFER_SIZE);

				addr=p_address->http_to_ip_server.super.super.addr_ar[i];

				DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "In It.  SOCKET family:  %s...\n",addr_family_get_name(addr->ai_family)));

				if (!(addr->ai_family==AF_INET || addr->ai_family==AF_INET6))

					continue;

				/*doing any ip4?*/
				/*continue if...
					is ip4					and don't need it				or have it*/
				if ((addr->ai_family==AF_INET && (!(p_address->is_update_ip4 || p_address->is_update_auto) || is_got_ip4)))

					continue;

				/*doing any ip6?*/
				/*continue if...
					is ip6					and don't need it				or have it*/
				if ((addr->ai_family==AF_INET6 && (!(p_address->is_update_ip6 || p_address->is_update_auto) || is_got_ip6)))

					continue;

				if (AF_INET6==addr->ai_family) {

					RC_TYPE rc=RC_OK;

					if (!(0==strcmp(p_address->addr_pref.addr_pref,"policy")))

						rc=ip_address_set_addr_pref(p_address,p_address->http_to_ip_server.super.super.socket[i]);

					if (RC_OK==rc)

						DBG_PRINTF((LOG_NOTICE,"N:" MODULE_TAG "Address pref set (\"%s\") in "\
								"check_my_ip_address...\n",p_address->addr_pref.addr_pref));
				}

				if (!(RC_OK==(rc=http_client_connect_socket(&p_address->http_to_ip_server)))) {

					DBG_PRINTF((LOG_ERR,"E:" MODULE_TAG "connect failed getting %s ip from %s%s in check_my_ip_address...\n",
						addr_family_get_name(addr->ai_family),p_address->ip_server,p_address->ip_server_url));

					continue;
				}
				
				if (!(RC_OK==(rc=http_client_transaction(&p_address->http_to_ip_server,&p_self->http_tr)))) {
					DBG_PRINTF((LOG_ERR,"E:" MODULE_TAG "Failed getting %s ip from %s%s in check_my_ip_address...\n",
						addr_family_get_name(addr->ai_family),p_address->ip_server,p_address->ip_server_url));
				}
				else {
					DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "Transaction %d DONE...\n",i));

					DBG_PRINTF((LOG_INFO,"I:DYNDNS: IP server response: %.500s...\n",p_tr->p_rsp));

					if (!(RC_OK==(rc=do_check_my_ip_address(p_address,p_self)))) {

						DBG_PRINTF((LOG_ERR,"E:" MODULE_TAG "ip parse failed in check_my_ip_address...\n"));
					}
				}

				if (rc==RC_OK) {

					DBG_PRINTF((LOG_NOTICE,"N:DYNDNS: My IP address: %s\n", p_address->my_ip_address.name[ip_store]));

					p_address->is_got_ip4=is_got_ip4=(is_got_ip4 || (addr->ai_family==AF_INET));
					p_address->is_got_ip6=is_got_ip6=(is_got_ip6 || (addr->ai_family==AF_INET6));

					/*detect if just doing auto (based on ip server) and dump out when done*/
					if ((is_got_ip4 && is_got_ip6) || !(p_address->is_update_ip4 && p_address->is_update_ip6))

						break;
				}
			}

			http_transaction_destruct(p_tr);
		}
	}
	while(0);

	/*close*/
	http_client_shutdown(&p_address->http_to_ip_server);

	p_address->is_updated=((((!(p_address->is_update_ip4) || p_address->is_got_ip4)) && ((!(p_address->is_update_ip6) || p_address->is_got_ip6))) 
				|| (p_address->is_update_auto && (p_address->is_got_ip4 || p_address->is_got_ip6)));

	return rc;
}

static RC_TYPE do_update_alias_table(DYN_DNS_CLIENT *p_self,char *is_forced_update_reset,char srv_index)
{
	DYNDNS_INFO_TYPE	*info;
	int			i;
	int			success_updates=0;
	int			update_ok=false;
	int			config_fails=0;
	int			updates_needed=p_self->info[srv_index].alias_info.count;
	DYNDNS_IPV		ip_auto_to_v;
	RC_TYPE			rc2=RC_OK;
	RC_TYPE 		rc=RC_OK;
	int			is_hashes_fetch;
	int			is_got_hashes=0;


	info=&p_self->info[srv_index];
	


	if (is_hashes_fetch=(0==strcmp(info->p_dns_system->p_key,"default@freedns.afraid.org")))

		is_hashes_fetch=is_user_pass(info->credentials.my_username,info->credentials.my_password);
	do
	{
		for (i = 0; i < info->alias_info.count; ++i) {

			if (!(info->alias_info.update_required[i])) {

				if (info->alias_info.fatal_error[i]) {

					do_handle_bad_config(p_self,i,srv_index);

					config_fails++;
				}

				updates_needed--;
			}
			else {
				
				ip_auto_to_v=actual_alias_ipv(info->alias_info.p_address[i],info->alias_info.names[i].ip_v_enum);

				if (is_hashes_fetch) {

					if (!(is_got_hashes))

						if (!(is_got_hashes=(RC_OK==(rc2=fetch_and_store_afraid_hashes(info)))))

							DBG_PRINTF((LOG_NOTICE,"N:" MODULE_TAG "Failed fetching, storing freedns.afraid.org "\
									"alias update tokens in do_update_alias_table:  %s\n",
									errorcode_get_name(rc2)));
					if (is_got_hashes)

						get_afraid_hash(&info->alias_info.hashes[i].str,info->alias_info.names[i].name,
								ip_auto_to_v,info);
				}

				/*dual stack with ip's updated in comma delimited list fashion?*/
				if (NULL==(strstr(info->alias_info.names[i].ip_v,DUAL_LIST))) {

					strcpy(info->alias_info.p_address[i]->my_ip_address.name[ip_store],
						info->alias_info.p_address[i]->my_ip_address.name[ip_auto_to_v]);
				}
				else {
					sprintf(info->alias_info.p_address[i]->my_ip_address.name[ip_store],"%s,%s",
						info->alias_info.p_address[i]->my_ip_address.name[ip_4],
						info->alias_info.p_address[i]->my_ip_address.name[ip_6]);

					if (!(info->alias_info.p_address[i]->is_got_ip4 && info->alias_info.p_address[i]->is_got_ip6)) {

						info->alias_info.update_succeeded[i]=false;

						DBG_PRINTF((LOG_ERR,"E:" MODULE_TAG "Skipping partially empty address (%s) update of alias "\
							"%s:%s\n",info->alias_info.p_address[i]->my_ip_address.name[ip_store],
							info->alias_info.names[i].name,ip_enum_to_str(ip_auto_to_v)));

						continue;
					}
				}
#if 0
				if (!(strlen(info->alias_info.p_address[i]->my_ip_address.name[ip_store]))) {

					info->alias_info.update_succeeded[i]=false;

					DBG_PRINTF((LOG_ERR,"E:" MODULE_TAG "Skipping empty address update of alias %s\n",info->alias_info.names[i].name));

					continue;
				}
#endif				

				/*bind to dynamic dns server according to address type to be updated*/
				http_client_set_is_ip(&info->http_to_dyndns,((info->alias_info.names[i].ip_v_enum==ip_4) &&
							info->is_auto_detect),((info->alias_info.names[i].ip_v_enum==ip_6) &&
							info->is_auto_detect));

				dyn_dns_set_comm_src_params(&info->http_to_dyndns,info->alias_info.p_address[i]->iface_name,info->alias_info.p_address[i]->addr_pref);

				if (!((rc=http_client_init_and_connect(&info->http_to_dyndns))==RC_OK)) {

					DBG_PRINTF((LOG_ERR,"E:" MODULE_TAG "Init error:  %s updating alias %s\n",errorcode_get_name(rc),info->alias_info.names[i].name));

					info->alias_info.update_succeeded[i]=false;
				}
				else {
					/*build dyndns transaction*/
					{
						HTTP_TRANSACTION http_tr;

						memset(&http_tr,0,sizeof(HTTP_TRANSACTION));

						http_tr.req_len = get_req_for_http_dns_server(info->p_dns_system,(struct DYN_DNS_CLIENT*)
												p_self,srv_index,i);

						if (!(http_tr.req_len)) {

							DBG_PRINTF((LOG_ERR,"E:" MODULE_TAG "Skipping zero request length for alias %s in do_update_alias_table...\n",info->alias_info.names[i].name));

							rc=RC_DYNDNS_INVALID_OR_MISSING_PARAMETERS;
						}
						else {
							http_transaction_construct(&http_tr,http_tr.req_len+1,p_self->p_req_buffer,
											DYNDNS_HTTP_RESPONSE_BUFFER_SIZE);

							/*send it - returns RC_OK, RC_IP_RECV_ERROR, RC_IP_SEND_ERROR, or invalid socket*/
							rc = http_client_transaction(&info->http_to_dyndns, &http_tr);

							DBG_PRINTF((LOG_NOTICE,"N:" MODULE_TAG "DYNDNS my Request:\n%s\n", http_tr.p_req));
						}

						/*error in send?*/
						if (!(rc == RC_OK)) {

							info->alias_info.update_succeeded[i]=false;

							DBG_PRINTF((LOG_ERR,"E:" MODULE_TAG "update request attempt returned: %s in do_update_alias_table...\n",errorcode_get_name(rc)));
						}
						else {
							update_ok = (RC_OK==
								(rc=info->p_dns_system->p_rsp_ok_func((struct DYN_DNS_CLIENT*)p_self,i,
									    http_tr.p_rsp,info->p_dns_system->p_success_string,srv_index)));

							if (update_ok)
							{
								success_updates++;

#ifdef USE_THREADS
								if (!(*is_forced_update_reset)) {

									restart_timer(&update_timer);

									*is_forced_update_reset=true;
								}
#endif
								info->alias_info.update_required[i]=false;
								info->alias_info.update_succeeded[i]=true;

                                /* add by EAP for output state and handle IPv6 DDNS */
								if(p_self->is_private_ipv6_addr != TRUE)
								{
									FILE *pFile;
									char sBuf[32] = {0};
									pFile = fopen( "/tmp/ddnstate","w" );
									sprintf(sBuf, "2"); // connected
									if(pFile)
								    {
	    								fwrite(sBuf, sizeof(sBuf), 1, pFile);
	    								fclose(pFile);
								    }
									
									openlog(LOG_DDNS, LOG_NDELAY, LOG_DAEMON);
									setlogmask( LOG_UPTO(LOG_NOTICE ));
									syslog(LOG_NOTICE , "DDNS client connect success!" );
									closelog();
	                                os_shell_execute("rc upadte_DDNS_ipv6 &");
								}
                                /****************************************************/
								DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "Alias '%s' to IP '%s' updated successfully.\n",
											info->alias_info.names[i].name,info->alias_info.p_address[i]->my_ip_address.name[ip_store]));

								if (p_self->external_command)

									os_shell_execute(p_self->external_command);
							}
							else {

								info->alias_info.update_succeeded[i]=false;

								/*dyndns2 protocol, retry later?*/
								if (!(rc==RC_DYNDNS_SERVER_ERROR)) {

									if (!(rc==RC_DYNDNS_RSP_CONFIG)) {

										DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "Error updating alias %s\n",info->alias_info.names[i].name));
									}
									else {
										if (strstr(http_tr.p_rsp,"badagent"))
									    {
											DBG_PRINTF((LOG_ERR,"E:" MODULE_TAG "Server responded with 'badagent'.  Attempting to update IPv6 to IPv4 only server?  Try ip4 parameter, e.g., --alias youralias ip4.\n"));
									    }
										
										if(p_self->is_private_ipv6_addr != TRUE)
										{

	                                        /* add by EAP for output state */
											FILE *pFile;
											char sBuf[32] = {0};
											pFile = fopen( "/tmp/ddnstate","w" );
											sprintf(sBuf, "0"); // Discnnected
											if (pFile)
										    {
	    										fwrite(sBuf, sizeof(sBuf), 1, pFile);
	    										fclose(pFile);
										    }
											openlog(LOG_DDNS, LOG_NDELAY, LOG_DAEMON);
											setlogmask( LOG_UPTO(LOG_NOTICE ));
											syslog(LOG_NOTICE , "DDNS client connect fault!" );
											closelog();
										}
                                        /*******************************/

										DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "Error validating DYNDNS svr answer. Check usr,pass,hostname!\n"));

										info->alias_info.fatal_error[i]=TRUE;

										ip_address_inc_fatal(info->alias_info.p_address[i]);

										DBG_PRINTF((LOG_CRIT,"C:" MODULE_TAG "\n"\
												ERROR_FLAG \
												"Fatal dyndns server update error for "\
												"alias, %s.\nThis client should be stopped and corrected for "\
												"configuration errors, and restarted...\n" \
												ERROR_FLAG,info->alias_info.names[i].name));

										config_fails++;
										updates_needed--;
									}
								}
							}

							DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "DYNDNS Server response:\n%s\n", http_tr.p_rsp));
						}

						http_transaction_destruct(&http_tr);
					}
				}

				if (!(RC_OK==http_client_shutdown(&info->http_to_dyndns)))

					DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "http client possibly improper shutdown, possible memory leak, system instability...\n"));
			}
							/*even if it's only one alias misconfigured*/
			if (rc==RC_DYNDNS_SERVER_ERROR || rc==RC_DYNDNS_RSP_CONFIG)

				break;

			if (i<info->alias_info.count-1)

				os_sleep_ms(1000);

			/*reset for loop*/
			rc=RC_OK;
		}
	}
	while(0);

	rc2=ip_cache_list_update_state(p_self,updates_needed,success_updates,config_fails,rc,srv_index,&is_global_success_updates,&is_global_in_proc_update_pending);

	if (!(rc==RC_OK))

		return rc;
	else
		return rc2;
}

static int get_is_cache_current(DYN_DNS_CLIENT *p_self,int svr_index,int alias_index,DYNDNS_IPV ip_v,char *ip_v_str)
{
	char	*ip_alias_type;
	int	is_cache_current=0;
	void	*in_addr;
	int	address_size;


	ip_alias_type=safe_malloc(strlen(p_self->info[svr_index].alias_info.p_address[alias_index]->my_ip_address.name[ip_v])+
					strlen(p_self->info[svr_index].alias_info.names[alias_index].name)+
					strlen(p_self->info[svr_index].alias_info.names[alias_index].ip_v)+strlen(ip_v_str)+3);

	sprintf(ip_alias_type,"%s %s:%s",p_self->info[svr_index].alias_info.p_address[alias_index]->my_ip_address.name[ip_v],
		p_self->info[svr_index].alias_info.names[alias_index].name,ip_v_str);

	if (!(strpbrk(p_self->info[svr_index].alias_info.p_address[alias_index]->my_ip_address.name[ip_v],":"))) {

		address_size=sizeof(struct in_addr);

		in_addr=&(((struct sockaddr_in*) &p_self->info[svr_index].alias_info.p_address[alias_index]->net_addr[ip_v])->sin_addr);
	}
	else {
		address_size=sizeof(struct in6_addr);

		in_addr=&(((struct sockaddr_in6*) &p_self->info[svr_index].alias_info.p_address[alias_index]->net_addr[ip_v])->sin6_addr);
	}

	is_cache_current=!(-1==ip_cache_list_index_of(&p_self->ip_cache_list,in_addr,
							p_self->info[svr_index].alias_info.p_address[alias_index]->my_ip_address.name[ip_v],
							p_self->info[svr_index].alias_info.names[alias_index].name,
							p_self->info[svr_index].dyndns_server_host,ip_alias_type));

	free(ip_alias_type);

	return is_cache_current;
}

/*
    Updates for every maintained name the property: 'update_required'.
    The property will be checked in another function and updates performed.

      Action:
        Check if my IP address has changed. -> ALL names have to be updated.
        Note: In the update function the property will set to false if update was successful.
*/
static RC_TYPE check_alias_update_table(DYN_DNS_CLIENT *p_self,char srv_index)
{
	int			i;
	DYNDNS_IPV		ip_v;
	BOOL			is_dyndns_dual;
	BOOL			is_cache_current;
	DYNDNS_INFO_TYPE	*info;

	info=&p_self->info[srv_index];

	for (i = 0; i < info->alias_info.count; ++i)
	{
		/*if ip type is auto (enum ip_store), deduce actual ip
		  type based on p_address structure*/
		ip_v=actual_alias_ipv(info->alias_info.p_address[i],info->alias_info.names[i].ip_v_enum);

		/*ip type DUAL_LIST is dual of form, 1.1.1.1,::1 -- both ip types in one update connection*/
		if (!(is_dyndns_dual=(NULL!=strstr(info->alias_info.names[i].ip_v,DUAL_LIST)))) {

			is_cache_current=get_is_cache_current(p_self,srv_index,i,ip_v,info->alias_info.names[i].ip_v);
		}
		else {

			is_cache_current=get_is_cache_current(p_self,srv_index,i,ip_4,"ip4");

			if (is_cache_current)

				is_cache_current=get_is_cache_current(p_self,srv_index,i,ip_6,"ip6");
		}

		info->alias_info.update_required[i] =

			/*ip address changed, or failed update?*/
			(!is_cache_current

			/*pending failed update?*/
			|| (is_global_update_pending && !(info->alias_info.update_succeeded[i]))

			/*administrative update?*/
			|| (!(p_self->forced_update_counter)));

		if (info->alias_info.update_required[i]) {

			if (is_dyndns_dual)

				DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "IP address for alias '%s:%s' needs update to '%s,%s'...\n",
				        info->alias_info.names[i].name,info->alias_info.names[i].ip_v,
						info->alias_info.p_address[i]->my_ip_address.name[ip_4],
						info->alias_info.p_address[i]->my_ip_address.name[ip_6]));


			else

				DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "IP address for alias '%s:%s' needs update to '%s'...\n",
				        info->alias_info.names[i].name,info->alias_info.names[i].ip_v,
						info->alias_info.p_address[i]->my_ip_address.name[ip_v]));

			info->alias_info.update_required[i]=!(info->alias_info.fatal_error[i]);
		}
	}

	return RC_OK;
}

static RC_TYPE do_handle_bad_config(DYN_DNS_CLIENT *p_self,int i,char srv_cnt)
{

	/*might want to sound an error specific alert too*/

	if (i<p_self->info[srv_cnt].alias_info.count)

		DBG_PRINTF((LOG_ERR,"E:" MODULE_TAG "\n"\
					ERROR_FLAG \
					"Skipping IP update attempt subsequent to "\
					"fatal dyndns server update error for "\
					"alias, %s.\nThis client should be stopped and corrected for "\
					"configuration errors, and restarted...\n" \
					ERROR_FLAG,p_self->info[srv_cnt].alias_info.names[i].name));
	else

		DBG_PRINTF((LOG_ERR,"E:" MODULE_TAG "\n"\
					ERROR_FLAG \
					"Skipping IP update attempt subsequent to "\
					"fatal dyndns server update error.  "\
					"\nThis client should be stopped and corrected for "\
					"configuration errors, and restarted...\n" \
					ERROR_FLAG));

	return RC_ERROR;
}

static RC_TYPE dyn_dns_handle_bad_config(DYN_DNS_CLIENT *p_self,char srv_cnt)
{

	/*ip update subsequent dns server return bad config related error*/

	int	i;

	for (i=0;i<p_self->info[srv_cnt].alias_info.count;i++) {

		if (p_self->info[srv_cnt].alias_info.fatal_error[i])

			break;
	}

	return do_handle_bad_config(p_self,i,srv_cnt);
}

static RC_TYPE update_private_IPV4_address(IP_ADDRESS *p_address,DYN_DNS_CLIENT *p_self)
{
	p_address->my_ip_address.name[ip_4][0] = 0;
	p_address->my_ip_address.name[ip_6][0] = 0;
	p_address->my_ip_address.name[ip_store][0] = 0;

	memset(&p_address->net_addr[ip_4],0,sizeof(struct sockaddr_storage));
	memset(&p_address->net_addr[ip_6],0,sizeof(struct sockaddr_storage));
	memset(&p_address->net_addr[ip_store],0,sizeof(struct sockaddr_storage));
	
	p_address->is_got_ip4 = TRUE;
	p_address->is_updated = TRUE;

	DBG_PRINTF((LOG_NOTICE,"N:DYNDNS: IPV4 address set NULL\n"));

	return RC_OK;
}


static RC_TYPE update_private_IPV6_address(IP_ADDRESS *p_address,DYN_DNS_CLIENT *p_self)
{
	void	*in_addr;
	int 	sock_size;
	unsigned char	ip_str_to_ip[16];


	in_addr=&(((struct sockaddr_in6*) &p_address->net_addr[ip_6])->sin6_addr);
	sock_size=sizeof(struct in6_addr);

	inet_pton_w( AF_INET6 , p_self->private_ipv6_addr , ip_str_to_ip);

	memcpy(in_addr,ip_str_to_ip,sock_size);
	strcpy(p_address->my_ip_address.name[ip_6],p_self->private_ipv6_addr);

	strcpy(p_address->my_ip_address.name[ip_store],p_self->private_ipv6_addr);
	memcpy(&p_address->net_addr[ip_store],&p_address->net_addr[ip_6],sizeof(struct sockaddr_storage));

	p_address->is_got_ip6 = TRUE;
	p_address->is_updated = TRUE;

	DBG_PRINTF((LOG_NOTICE,"N:DYNDNS: My IP address: %s\n", p_address->my_ip_address.name[ip_store]));

	return RC_OK;
}



static RC_TYPE update_address_store(IP_ADDRESS *p_address,DYN_DNS_CLIENT *p_self)
{

	int	is_exit;
	int	net_attempts=1;
	int	ip_attempts=1;
	RC_TYPE	rc=RC_ERROR;


	DBG_PRINTF((LOG_INFO,"I:DYNDNS: update_ip_store entering connect loop...\n"));


	while (!(is_exit=(is_exit_requested(p_self)))) {

		if (!(do_is_dyndns_online(p_self))) {
			

			if ((is_exit=(p_self->net_retries<net_attempts++)))

				break;

			DBG_PRINTF((LOG_WARNING,"W:DYNDNS: Online failed getting ip...iterating after retry interval...\n"));
		}
		else {


			/*ask IP server something so will respond and give me my IP */
			rc = check_my_ip_address(p_address,p_self);

			if (rc==RC_OK)

				break;

			DBG_PRINTF((LOG_WARNING,"W:DYNDNS: Failed checking current ip...\n"));

			if (p_self->net_retries<ip_attempts++)

				break;

			DBG_PRINTF((LOG_WARNING,"W:DYNDNS:  Iterating after retry interval...\n"));
		}

		sleep_lightly_ms(p_self->retry_interval,&is_exit_requested_void,p_self);
	}

	if (is_exit)

		DBG_PRINTF((LOG_INFO,"I:DYNDNS: Exit requested or not online in update_address_store...\n"));

	if (!(RC_OK==rc))
		DBG_PRINTF((LOG_WARNING,"W:DYNDNS: Error '%s' (0x%x) when talking to IP server\n",errorcode_get_name(rc), rc));
	

	return rc;
}

/*
	- increment the forced update times counter
	- detect current IP
		- connect to an HTTP server
		- parse the response for IP addr

	- for all the names that have to be maintained
		- get the current DYN DNS address from DYN DNS server
		- compare and update if neccessary
*/
static RC_TYPE do_dyn_dns_update_ip(DYN_DNS_CLIENT *p_self,int srv_cnt)
{

	RC_TYPE	rc=RC_ERROR;
	int	is_exit=false;
	int	net_attempts=1; /*use same retries parameter for both of these*/
	int	ip_attempts=1;
	char	is_forced_update_reset=0;



	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	if (p_self->is_global_bad_config)

		return dyn_dns_handle_bad_config(p_self,srv_cnt);

	do
	{
		DBG_PRINTF((LOG_INFO,"I:DYNDNS: dyn_dns_update_ip checking alias table...\n"));

		/*step through aliases list, resolve them and check if they point to my IP*/
		rc = check_alias_update_table(p_self,srv_cnt);
		if (rc != RC_OK)
		{
			break;
		}
		net_attempts=1;
		ip_attempts=1;

		DBG_PRINTF((LOG_INFO,"I:DYNDNS: dyn_dns_update_ip entering update loop...\n"));

		while(!(is_exit_requested(p_self))) {
#if 0
			if (!(do_is_dyndns_online(p_self))) {
#else
			if (0) {
#endif
				if (p_self->net_retries<net_attempts++) {

					p_self->info[srv_cnt].is_update_pending=true;

					break;
				}

				DBG_PRINTF((LOG_WARNING,"W:DYNDNS: Online failed updating alias table...iterating after retry interval...\n"));
			}
			else {

				DBG_PRINTF((LOG_INFO,"D:" MODULE_TAG "dyn_dns_update_ip calling do_update_alias_table...\n"));

				/*update IPs marked as not identical with my IP*/
				rc = do_update_alias_table(p_self,&is_forced_update_reset,srv_cnt);

				if (rc==RC_OK) {

					p_self->info[srv_cnt].is_update_pending=false;

					p_self->info[srv_cnt].is_bad_config=false;

					DBG_PRINTF((LOG_INFO,"D:" MODULE_TAG "aliases update in dyn_dns_update_ip...\n"));

					break;
				}
				

				DBG_PRINTF((LOG_WARNING,"W:DYNDNS: Failed updating alias table...\n"));

				p_self->info[srv_cnt].is_bad_config=(rc==RC_DYNDNS_RSP_CONFIG);

				/*at least one alias config failed*/
				if (!(p_self->info[srv_cnt].is_update_pending=(!(rc==RC_DYNDNS_RSP_CONFIG))))

					break;

				if ((p_self->net_retries<ip_attempts++) || (rc==RC_DYNDNS_SERVER_ERROR))

					break;

				DBG_PRINTF((LOG_WARNING,"W:DYNDNS:  Iterating after retry interval...\n"));
			}

			sleep_lightly_ms(p_self->retry_interval,&is_exit_requested_void,p_self);
		}
	}
	while(0);

	return rc;
}

static void inc_update_times(DYN_DNS_CLIENT *p_self,int *src,int inc)
{

	*src+=inc;
}

int number_of_aliases(DYN_DNS_CLIENT *p_dyndns)
{
	char	i;
	int	alias_count=0;


	for (i=0;i<MAX_DNS_SERVER;i++) {

		alias_count+=p_dyndns->info[i].alias_info.count;
	}

	return alias_count;
}

static int increment_iterations(DYN_DNS_CLIENT *p_dyndns)
{
	int	iterations;


	p_dyndns->iterations++;

	iterations=p_dyndns->iterations;


	return iterations;
}

BOOL is_completed_iterations(DYN_DNS_CLIENT *p_dyndns,int *iteration)
{


	*iteration=p_dyndns->iterations;


	/* check if the user wants us to stop */

	return (*iteration >= p_dyndns->total_iterations && p_dyndns->total_iterations != 0);
}

static RC_TYPE fetch_addresses(gen_list i_face,DYN_DNS_CLIENT *p_self)
{
	gen_list_itr	iface_itr;
	gen_list_itr	addr_list_itr;
	I_FACE		*p_iface;
	IP_ADDRESS	*p_address;
	RC_TYPE		rc=RC_ERROR;
	
	iface_itr=list_get_itr(i_face);
	list_begin(i_face,iface_itr);

	while (!(list_end(iface_itr))) {

		p_iface=list_entry_item(iface_itr);

		addr_list_itr=list_get_itr(p_iface->address);
		list_begin(p_iface->address,addr_list_itr);

		while (!(list_end(addr_list_itr))) {

			p_address=list_entry_item(addr_list_itr);
			
			 				
			if(p_self->is_private_ipv6_addr == TRUE)
			{
				p_address->is_updated=0;
				p_address->is_got_ip4=0;
				p_address->is_got_ip6=0;

				memset(p_address->my_ip_address.name[ip_4],0,DYNDNS_MY_IP_ADDRESS_LENGTH);
				memset(p_address->my_ip_address.name[ip_6],0,DYNDNS_MY_IP_ADDRESS_LENGTH);	
				
				update_private_IPV6_address(p_address,p_self);
				
				rc=RC_OK;
			}	
#if 0
			else if ((is_global_update_pending && p_address->is_updated) || (p_address->dependents_fatal==p_address->dependents))
			{
				rc=RC_OK;
			}	
			else {
				
				/*reset*/
				if (p_address->is_updated) {

					p_address->is_updated=0;
					p_address->is_got_ip4=0;
					p_address->is_got_ip6=0;

					memset(p_address->my_ip_address.name[ip_4],0,DYNDNS_MY_IP_ADDRESS_LENGTH);
					memset(p_address->my_ip_address.name[ip_6],0,DYNDNS_MY_IP_ADDRESS_LENGTH);
				}

				update_address_store(p_address,p_self);

				/*only return not RC_OK if don't fetch any at all*/
				if (p_address->is_got_ip4 || p_address->is_got_ip6)
					rc=RC_OK;
			}
#else
			else 
			{
				p_address->is_updated=0;
				p_address->is_got_ip4=0;
				p_address->is_got_ip6=0;

				memset(p_address->my_ip_address.name[ip_4],0,DYNDNS_MY_IP_ADDRESS_LENGTH);
				memset(p_address->my_ip_address.name[ip_6],0,DYNDNS_MY_IP_ADDRESS_LENGTH);	
				
				update_private_IPV4_address(p_address,p_self);
				
				rc=RC_OK;
			}
#endif 

			is_global_in_proc_update_pending=(is_global_in_proc_update_pending || !(p_address->is_updated));

			list_next(addr_list_itr);
		}

		list_next(iface_itr);
	}

	return rc;
}

static void dyn_dns_update_ip(DYN_DNS_CLIENT *p_dyndns,int *success_count,int *current_iteration,int *bad_config_count)
{
	int	i;
	RC_TYPE	rc;
	
	

	for (i=0;i<p_dyndns->srv_cnt;i++) {

		if (p_dyndns->cmd==CMD_STOP)

			break;

		if ((RC_OK==(rc=do_dyn_dns_update_ip(p_dyndns,i))))

			(*success_count)++;

		else {

			DBG_PRINTF((LOG_WARNING,"W:'%s' (0x%x) updating the IPs. (it %d)\n",errorcode_get_name(rc), rc, *current_iteration));

			if (p_dyndns->info[i].is_bad_config)

				(*bad_config_count)++;
			else
				/*dyndns2 retry later*/
				p_dyndns->retry_later=(p_dyndns->retry_later || (rc==RC_DYNDNS_SERVER_ERROR));
		}

		/*set/maintain pending updates status for this dyn_dns_update_ip instantiation*/
		is_global_in_proc_update_pending=(is_global_in_proc_update_pending || p_dyndns->info[i].is_update_pending);
	}
}

/* MAIN - Dyn DNS update entry point.*/

/*
	Actions:
		- read the configuration options
		- perform various init actions as specified in the options
		- create and init dyn_dns object.
		- launch the IP update action loop
*/
int dyn_dns_main(DYN_DNS_CLIENT *p_dyndns, int argc, char* argv[])
{

	RC_TYPE	rc=RC_OK;
	BOOL	init_flag=FALSE;
	int	current_iteration=0;
	int	bad_config_count;
	int	success_count;

#ifdef _WIN32

	RAS_THREAD_DATA	*p_ras_thread_data=NULL;
#else

	/*used in win32 only*/
	void *p_ras_thread_data=NULL;
#endif

	
	rc=init_update_loop(p_dyndns,argc,argv,&p_ras_thread_data,&init_flag);
    events_register_client(dyndns_event,p_dyndns,NULL);
    events_start_loop_thread();
	
	

	if (p_dyndns->abort)
		return rc;

	if (rc==RC_OK) /*init all okay?*/
	{
		do
		{
			/*state across multiple server updates*/
			bad_config_count=0;
			is_global_success_updates=false; /*flagged in update_update_state*/
			is_global_in_proc_update_pending=false;
			success_count=0;

			if (RC_OK==(rc=fetch_addresses(p_dyndns->address_list,p_dyndns)))
				dyn_dns_update_ip(p_dyndns,&success_count,&current_iteration,&bad_config_count);

			is_global_update_pending=is_global_in_proc_update_pending;

			/*bad config all aliases for all servers?*/
			if (p_dyndns->is_global_bad_config=(bad_config_count==p_dyndns->srv_cnt)) {

				p_dyndns->cmd=CMD_STOP;

				DBG_PRINTF((LOG_ERR,"E:" MODULE_TAG "All server update configurations are bad.  Stopping client...\n"));
			}

			if (success_count==p_dyndns->srv_cnt)

				increment_iterations(p_dyndns);
#ifdef USE_SNDFILE
#ifndef USE_THREADS

			alert_if_offline(p_dyndns,p_ras_thread_data);
#endif
#endif

			if (p_dyndns->cmd==CMD_STOP) {

				DBG_PRINTF((LOG_DEBUG,"D:" MODULE_TAG "STOP command received. Exiting.\n"));

				break;
			}

			if (is_completed_iterations(p_dyndns,&current_iteration)) {

				DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "Iterations (%d) completed.  Exiting program...\n", current_iteration));

				break;
			}

			/* sleep the time set in the ->sleep_sec data memeber*/
			dyn_dns_wait_for_cmd(p_dyndns);

			/*deprecated*/
			inc_update_times(p_dyndns,&(p_dyndns->times_since_last_update),1);

			/*reset the command*/
			if (!(p_dyndns->cmd==CMD_STOP))	{

				p_dyndns->cmd=NO_CMD;
			}
			else {

				DBG_PRINTF((LOG_DEBUG,"D:" MODULE_TAG "STOP command received. Exiting.\n"));

				break;
			}

		}
		while(1);
	}
#ifdef USE_THREADS

	destroy_timer(&update_timer);
#endif


#ifdef _WIN32

	destroy_trap_ras_events(&p_ras_thread_data);
#endif


	if (init_flag) {

		/* dyn_dns_shutdown object */
		rc = dyn_dns_shutdown(p_dyndns);
	}

#ifdef USE_SNDFILE

	waveout_shutdown();
#endif

	events_stop_loop_thread();

	return rc;
}

/*************
 Connection testing, connection status alert, DYN_DNS_CLIENT default config, construct, destruct.
*/
static RC_TYPE dyn_dns_set_proxy_dest(HTTP_CLIENT *dest,char *proxy_server_name,int proxy_server_port)
{
	RC_TYPE	rc;

	if (RC_OK==(rc=http_client_set_remote_name(dest,proxy_server_name)))

		rc=http_client_set_port(dest,proxy_server_port);

	return rc;
}

static RC_TYPE dyn_dns_set_comm_src_params(HTTP_CLIENT *dest,char *if_name,IPV6_PRIV_EXT ipv6_priv_ext)
{
	RC_TYPE	rc;

	if (RC_OK==(rc=http_client_set_priv_ext(dest,ipv6_priv_ext)))

		rc=http_client_set_if_name(dest,if_name);

	return rc;
}

static RC_TYPE dyn_dns_set_online_check_params(HTTP_CLIENT *dest,DYN_DNS_CLIENT *p_self)
{


	if (p_self->info[0].proxy_server_name.name) {

		dyn_dns_set_proxy_dest(dest,p_self->info[0].proxy_server_name.name,p_self->info[0].proxy_server_name.port);
	}
	else {

		http_client_set_remote_name(dest,p_self->info_online_status.name);
		http_client_set_port(dest,p_self->info_online_status.port);
	}

	return RC_OK;
}

static RC_TYPE dyn_dns_set_ip_server_params(HTTP_CLIENT *dest,IP_ADDRESS *ip_address)
{

	if (ip_address->ip_proxy_name) {

		dyn_dns_set_proxy_dest(dest,ip_address->ip_proxy_name,ip_address->ip_proxy_port);
	}
	else {

		http_client_set_remote_name(dest,ip_address->ip_server);
		http_client_set_port(dest,ip_address->port);
	}

	dyn_dns_set_comm_src_params(dest,ip_address->iface_name,ip_address->addr_pref);


	return RC_OK;
}

static RC_TYPE dyn_dns_set_dyndns_server_params(HTTP_CLIENT *dest,DYNDNS_INFO_TYPE *info)
{

	if (info->proxy_server_name.name) {

		dyn_dns_set_proxy_dest(dest,info->proxy_server_name.name,info->proxy_server_name.port);
	}
	else {

		http_client_set_remote_name(dest,info->dyndns_server_name.name);
		http_client_set_port(dest,info->dyndns_server_name.port);
	}

	return RC_OK;
}

static RC_TYPE dyn_dns_set_http_clients(DYN_DNS_CLIENT *p_self)
{
	gen_list_itr	iface_itr;
	gen_list_itr	addr_list_itr;
	I_FACE		*p_iface;
	IP_ADDRESS	*p_address;
	int		i;


	for (i=0;i<p_self->srv_cnt;i++)

		dyn_dns_set_dyndns_server_params(&p_self->info[i].http_to_dyndns,&p_self->info[i]);

	iface_itr=list_get_itr(p_self->address_list);
	list_begin(p_self->address_list,iface_itr);

	while (!(list_end(iface_itr))) {

		p_iface=list_entry_item(iface_itr);

		addr_list_itr=list_get_itr(p_iface->address);
		list_begin(p_iface->address,addr_list_itr);

		while (!(list_end(addr_list_itr))) {

			p_address=list_entry_item(addr_list_itr);

			dyn_dns_set_ip_server_params(&p_address->http_to_ip_server,p_address);

			list_next(addr_list_itr);
		}

		list_next(iface_itr);
	}

	return RC_OK;
}

static RC_TYPE test_connect(DYN_DNS_CLIENT *p_self)
{
	RC_TYPE		rc=RC_OK;
	HTTP_CLIENT	*http_to_ip_server;


	http_to_ip_server=safe_malloc(sizeof(HTTP_CLIENT));

	rc=http_client_construct(http_to_ip_server);


	dyn_dns_set_online_check_params(http_to_ip_server,p_self);


#ifndef USE_THREADS
	rc=http_client_init_and_connect(http_to_ip_server);
#else
	rc=http_client_test_connect(http_to_ip_server,is_exit_requested_void,p_self);
#endif

	http_client_shutdown(http_to_ip_server);
	http_client_destruct(http_to_ip_server);

	free(http_to_ip_server);

	return rc;
}

static int do_is_dyndns_online(DYN_DNS_CLIENT *p_self)
{

	return (test_connect(p_self)==RC_OK);
}

static int is_exit_requested(DYN_DNS_CLIENT *p_self)
{

	/*
		Check whether Windows service requested exit or
		user signaled.
	*/

#ifndef _WIN32

	return (p_self->cmd==CMD_STOP);
#else
	return ((returnSignaled) || (p_self->cmd==CMD_STOP));
#endif

}

static int is_exit_requested_void(void *p_self)
{

	return is_exit_requested((DYN_DNS_CLIENT *) p_self);
}

static int is_dyndns_online(DYN_DNS_CLIENT *p_self,void *p_data)
{

#ifdef USE_THREADS

	BOOL	is_online;


	get_mutex(&mutex_global_is_online);

	is_online=global_is_online;

	release_mutex(&mutex_global_is_online);

	return is_online;
#else

	return do_is_dyndns_online(p_self);
#endif

}

static int alert_abort_cond(void *p_data)
{

	CB_ALERT_DATA	*p_online_data=(CB_ALERT_DATA *) p_data;


	return is_dyndns_online(p_online_data->p_dyndns,p_online_data->p_data) || is_exit_requested(p_online_data->p_dyndns);
}

#ifdef USE_SNDFILE

static int sound_abort_cond(WAVE_PARAMS *p_data)
{

	CB_ALERT_DATA	*p_this_data=(CB_ALERT_DATA *) (p_data->cb_client_data);


	return (alert_abort_cond(p_data->cb_client_data) || is_exit_requested(p_this_data->p_dyndns));
}

static void alert_if_offline(DYN_DNS_CLIENT *p_dyndns,void *p_data)
{

	CB_ALERT_DATA	alert_cond_param;
	char		*waveout=NULL;
	WAVE_PARAMS	*wave_params;

#ifndef USE_THREADS
	int		attempts=0;
#endif


	memset(&alert_cond_param,0,sizeof(CB_ALERT_DATA));


	if (p_dyndns->audible_off)

		return;


	alert_cond_param.p_data=p_data;
	alert_cond_param.p_dyndns=p_dyndns;


	while (1) {

		if (is_alert_thread_exit) {

			DBG_PRINTF((LOG_INFO, "I:" MODULE_TAG "is_alert_thread_exit "\
						"true in alert_if_offline.  Exiting alert loop...\n"));

			break;
		}

		if (is_exit_requested(p_dyndns)) {

			DBG_PRINTF((LOG_INFO, "I:" MODULE_TAG "is_exit_requested returned true in alert_if_offline.  "\
						"Exiting alert loop...\n"));

			break;
		}

		if (is_dyndns_online(p_dyndns,p_data)) {

			DBG_PRINTF((LOG_INFO, "I:" MODULE_TAG "is_dyndns_online returned true in alert_if_offline.  "\
						"Exiting alert loop...\n"));

			break;
		}

#ifndef USE_THREADS

		if (p_dyndns->alert_retries<++attempts)
#endif

		{
			/*future implementation may not preclude parameters changing*/

			waveout=safe_malloc(strlen(p_dyndns->wave_file)+1);
			strcpy(waveout,p_dyndns->wave_file);

			wave_params=safe_malloc(sizeof(WAVE_PARAMS));

			wave_params->buff_factor=p_dyndns->wave_buff_factor;
			wave_params->loops=p_dyndns->wave_loops;
			wave_params->gain=p_dyndns->wave_gain;
			wave_params->wave_file=waveout;
			wave_params->cb_client_data=&alert_cond_param;
			wave_params->p_func=sound_abort_cond;


			do {

				DBG_PRINTF((LOG_INFO, "I:" MODULE_TAG "Entering sound_play...\n"));

				play_wave1(wave_params);


				DBG_PRINTF((LOG_INFO, "I:" MODULE_TAG "Returned from sound_play...\n"));

				if (is_alert_thread_exit) {

					break;
				}

				if (is_exit_requested(p_dyndns)) {

					DBG_PRINTF((LOG_INFO, "I:" MODULE_TAG "is_exit_requested returned true in alert_if_offline.  "\
								"Exiting alert loop...\n"));

					break;
				}

				if (is_dyndns_online(p_dyndns,p_data)) {

					DBG_PRINTF((LOG_INFO, "I:" MODULE_TAG "is_dyndns_online returned true in alert_if_offline.  "\
								"Exiting alert loop...\n"));

					break;
				}

				sleep_lightly_ms(p_dyndns->alert_interval,&alert_abort_cond,&alert_cond_param);

			} while (1);

			free(wave_params);
			free(waveout);
		}
	}
}

#endif

#ifdef USE_THREADS

static void exit_alert_thread()
{

	DBG_PRINTF((LOG_INFO, "I:" MODULE_TAG "Entered exit_alert_thread.  Setting return flag...\n"));

	is_alert_thread_exit=TRUE;

	DBG_PRINTF((LOG_INFO, "I:" MODULE_TAG "Joining alert thread:  %d...\n",thread_alert));

#ifndef _WIN32

	if (thread_alert)

		pthread_join(thread_alert,NULL);
#else
	WaitForSingleObject((HANDLE) thread_alert,INFINITE);
	CloseHandle((HANDLE) thread_alert);
#endif

	thread_alert=0;
}

static void exit_online_thread()
{


	DBG_PRINTF((LOG_INFO, "I:" MODULE_TAG "Entered exit_online_thread.  Setting return flag...\n"));

	is_online_thread_exit=TRUE;

	DBG_PRINTF((LOG_INFO, "I:" MODULE_TAG "Joining online test thread:  %d...\n",thread_online_test));

#ifndef _WIN32

	if (thread_online_test)

		pthread_join(thread_online_test,NULL);
#else
	CloseHandle((HANDLE) thread_online_test);
#endif

	thread_online_test=0;
}

static void exit_online_test_threads()
{

	exit_online_thread();
	exit_alert_thread();
}

static int thread_exit_cond(void *p_data)
{

	return is_online_thread_exit || is_exit_requested((DYN_DNS_CLIENT *) p_data);
}

#ifdef USE_SNDFILE

#ifndef _WIN32
static void *alert_if_offline_thread(void *p_data)
#else
static unsigned __stdcall alert_if_offline_thread(void *p_data)
#endif
{
	DYN_DNS_CLIENT	*p_dyndns;

	if (p_data) {

		p_dyndns=((CB_ALERT_DATA *) p_data)->p_dyndns;

		if (p_dyndns) {

			is_alert_thread_exit=false;

			alert_if_offline(p_dyndns,((CB_ALERT_DATA *) p_data)->p_data);

			DBG_PRINTF((LOG_INFO, "I:" MODULE_TAG "Exiting alert_if_offline_thread...\n"));
		}
	}

#ifdef _WIN32

	return 0;
#else
	pthread_exit(p_data);

	/*compiler complaints (pthread_exit does not return)*/
	return p_data;
#endif
}

#endif

static void start_online_alert_thread(CB_ALERT_DATA *p_online_data)
{

	/*
		launch alert_if_offline_thread to sound audible alert until
		back online
	*/

#ifdef USE_SNDFILE

	thread_alert=begin_thread(alert_if_offline_thread,(void *) p_online_data);
#endif

}

#ifndef _WIN32
static void *is_dyndns_online_thread(void *p_data)
#else
static unsigned __stdcall is_dyndns_online_thread(void *p_data)
#endif
{

	CB_ALERT_DATA	*online_data;

	DYN_DNS_CLIENT	*p_self;
	int		attempts;
	int		is_online=false;
	int		is_was_offline=false;


	if (p_data) {

		if (((CB_ALERT_DATA *) p_data)->p_dyndns) {

			p_self=((CB_ALERT_DATA *) p_data)->p_dyndns;

			online_data=safe_malloc(sizeof(CB_ALERT_DATA));

			online_data->p_data=p_data;
			online_data->p_dyndns=p_self;


			while(!(is_online_thread_exit) && !(is_exit_requested(p_self))) {

				attempts=0;

				do {

					DBG_PRINTF((LOG_INFO, "I:" MODULE_TAG "online thread calling do_is_dyndns_online...\n"));

					is_online=do_is_dyndns_online(p_self);


					DBG_PRINTF((LOG_INFO, "I:" MODULE_TAG "online thread returned from do_is_dyndns_online...\n"));

					if (is_online)

						break;

					if ((++attempts>p_self->alert_retries))

						break;

					DBG_PRINTF((LOG_INFO, "I:" MODULE_TAG "online thread pausing 5 seconds before retry...\n"));

					/*pause before retry -- need an option*/
					sleep_lightly_ms(5000,&thread_exit_cond,p_self);

				} while (!(is_online_thread_exit) && !(is_exit_requested(p_self)));


				get_mutex(&mutex_global_is_online);

				global_is_online=is_online;

				release_mutex(&mutex_global_is_online);

				if (global_is_online) {

					if (is_was_offline) {

						is_was_offline=false;

						p_self->cmd=CMD_UPDT;
					}

					exit_alert_thread();

					DBG_PRINTF((LOG_INFO, "I:" MODULE_TAG "Status thread entering status interval...\n"));

					sleep_lightly_ms(p_self->status_interval,&thread_exit_cond,p_self);
				}
				else {

					DBG_PRINTF((LOG_INFO, "I:" MODULE_TAG "\n" \
								ERROR_FLAG \
								"Status thread detected offline...\n" \
								ERROR_FLAG));

					is_was_offline=true;

					if (!(thread_alert))

						start_online_alert_thread(online_data);

					/*sleep less, check status more, if offline*/
					sleep_lightly_ms(p_self->status_offline_interval,&thread_exit_cond,p_self);
				}
			}

			exit_alert_thread();

			free(online_data);

			DBG_PRINTF((LOG_INFO, "I:" MODULE_TAG "Exiting is_dyndns_online_thread...\n"));
		}
	}

#ifdef _WIN32

	return 0;
#else
	pthread_exit(p_data);

	/*compiler complaints (pthread_exit does not return)*/
	return p_data;
#endif
}

static void start_online_test_thread(DYN_DNS_CLIENT *p_dyndns,void *p_data)
{

	/*
		launch is_dyndns_online_thread to periodically check online status and set
		a global flag, global_is_online, accordingly.
	*/

	CB_ALERT_DATA	*p_online_data=safe_malloc(sizeof(CB_ALERT_DATA));


	p_online_data->p_data=p_data;
	p_online_data->p_dyndns=p_dyndns;

	thread_online_test=begin_thread(is_dyndns_online_thread,(void *) p_online_data);
}

#endif /*USE_THREADS*/

/******************Windows RAS thread******************/

#ifdef _WIN32

static void start_ras_thread(DYN_DNS_CLIENT *p_dyndns,RAS_THREAD_DATA **p_ras_thread_data)
{

	if (p_dyndns->dbg.level>=LOG_INFO)

		DBG_PRINTF((LOG_INFO, "I:" MODULE_TAG "Creating, launching ras thread...\n"));


	*p_ras_thread_data=construct_and_launch_trap_ras_events(dyn_dns_update_ip_handler,p_dyndns,p_dyndns->\
		                   info[0].http_to_ip_server.super.super.p_remote_host_name,\
		                   p_dyndns->dbg.level);

	if (*p_ras_thread_data) {

		if (p_dyndns->dbg.level>=LOG_INFO)

			DBG_PRINTF((LOG_INFO, "I:" MODULE_TAG "Launched RAS events trapping...\n"));

	}
	else {

		if (p_dyndns->dbg.level>=LOG_ERR)

			DBG_PRINTF((LOG_ERR, "E:" MODULE_TAG "RAS events trapping constructor returned NULL.  Continuing without RAS events trapping...\n"));
	}
}

#endif /*_WIN32*/

static RC_TYPE get_default_config_data(DYN_DNS_CLIENT *p_self)
{
	RC_TYPE	rc = RC_OK;
	int	prefix_len;
	int	ip_len;
	int	time_len;
	int	rel_len;
	int	delim_len;
	char	*rel_path;
	do
	{
		p_self->is_private_ipv6_addr = FALSE;
		memset(p_self->private_ipv6_addr , 0 , sizeof(p_self->private_ipv6_addr));
		
		rel_path=default_relative_path(&rel_path);
		rel_len=strlen(rel_path);
		delim_len=strlen(DIR_DELIM_STR);

		/*set up default ddns system*/
		p_self->info[0].p_dns_system = get_dns_system_by_id(DYNDNS_MY_DNS_SYSTEM);
		p_self->info[0].is_auto_dual_list=p_self->info[0].p_dns_system->is_auto_dual_list;
		p_self->info[0].is_defaults_config = TRUE;
		p_self->srv_cnt++;

		if (p_self->info[0].p_dns_system == NULL)
		{
			rc = RC_DYNDNS_INVALID_DNS_SYSTEM_DEFAULT;
			break;
		}

		strcpy(p_self->info[0].ipv6_priv_ext.addr_pref,"pub");
		p_self->info[0].ipv6_priv_ext.flags=IPV6_PREFER_SRC_PUBLIC;

		/*forced update period*/
		p_self->forced_update_period_sec = DYNDNS_MY_FORCED_UPDATE_PERIOD_S;
		p_self->forced_update_period_sec_orig = DYNDNS_MY_FORCED_UPDATE_PERIOD_S;

		p_self->forced_update_adjust = DYNDNS_DEFAULT_UPDATE_ADJUST;

		/*network comm retries*/
		p_self->net_retries = DYNDNS_DEFAULT_NET_RETRIES;
		p_self->retry_interval = DYNDNS_DEFAULT_RETRY_INTERVAL;
		p_self->retry_pending_interval = DYNDNS_DEFAULT_RETRY_PENDING_INTERVAL;

		prefix_len=strlen(DYNDNS_DEFAULT_CACHE_PREFIX);
		ip_len=strlen(DYNDNS_DEFAULT_IP_FILE)+1;
		time_len=strlen(DYNDNS_DEFAULT_TIME_FILE)+1;

		p_self->ip_cache=safe_malloc(prefix_len+ip_len);
		sprintf(p_self->ip_cache, "%s%s", DYNDNS_DEFAULT_CACHE_PREFIX, DYNDNS_DEFAULT_IP_FILE);

		p_self->time_cache=safe_malloc(prefix_len+time_len);
		sprintf(p_self->time_cache, "%s%s", DYNDNS_DEFAULT_CACHE_PREFIX, DYNDNS_DEFAULT_TIME_FILE);
		/*update period*/
		p_self->sleep_sec = DYNDNS_DEFAULT_SLEEP;

		p_self->audible_off = DYNDNS_DEFAULT_INAUDIBLE;
		p_self->alert_retries = DYNDNS_DEFAULT_ALERT_RETRIES;
		p_self->alert_interval = DYNDNS_DEFAULT_ALERT_INTERVAL;
		p_self->status_interval = DYNDNS_DEFAULT_STATUS_INTERVAL*1000;
		p_self->status_offline_interval = DYNDNS_DEFAULT_STATUS_OFFLINE_INTERVAL*1000;
		p_self->wave_loops = DYNDNS_DEFAULT_WAVE_LOOPS;

#ifdef USE_SNDFILE

		set_wave_gain(p_self,DYNDNS_DEFAULT_USER_WAVE_GAIN);

		searchedProgFile(NULL,&(p_self->wave_file),"inadyn-mt","extra" DIR_DELIM_STR "wav",4,DYNDNS_DEFAULT_WAVE_FILE);


		if (!(p_self->wave_file)) {

			DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "Could not open default wave file, extra" DIR_DELIM_STR "wav" \
						DIR_DELIM_STR "alarm.wav.  Using default, ." DIR_DELIM_STR DYNDNS_DEFAULT_WAVE_FILE "...\n"));

			p_self->wave_file=safe_malloc(strlen(DYNDNS_DEFAULT_WAVE_FILE) + 1);
			strcpy(p_self->wave_file,DYNDNS_DEFAULT_WAVE_FILE);
		}

		/*
			default:  tell wave routines
			use quarter bytes/sec buffer
		*/
		p_self->wave_buff_factor=DYNDNS_DEFAULT_WAVE_BUFF_FACTOR;
#endif
		p_self->dbg.level=DYNDNS_DEFAULT_DEBUG_LEVEL;
	}
	while(0);

	return rc;
}

static RC_TYPE get_encoded_user_passwd(DYN_DNS_CLIENT *p_self,char srv_cnt)
{
	RC_TYPE		rc=RC_OK;
	char		*str_uri_encoded;
	const char	*format="%s:%s";
	char		*p_tmp_buff=NULL;
	int		size;
	int		actual_len;


	size=strlen(format)+1;

	if (!(p_self->info[srv_cnt].credentials.my_password))

		p_self->info[srv_cnt].credentials.my_password=safe_malloc(1);

	size+=strlen(p_self->info[srv_cnt].credentials.my_password);

	if (!(p_self->info[srv_cnt].credentials.my_username))

		p_self->info[srv_cnt].credentials.my_username=safe_malloc(1);

	size+=strlen(p_self->info[srv_cnt].credentials.my_username);

	do
	{
		p_tmp_buff = (char *) safe_malloc(size);
		if (p_tmp_buff == NULL)
		{
			rc = RC_OUT_OF_MEMORY;
			break;
		}

		actual_len = sprintf(p_tmp_buff, format,
		                     p_self->info[srv_cnt].credentials.my_username,
		                     p_self->info[srv_cnt].credentials.my_password);

		if (actual_len >= size)
		{
			rc = RC_OUT_BUFFER_OVERFLOW;
			break;
		}

		/*encode*/

		p_self->info[srv_cnt].credentials.p_enc_usr_passwd_buffer =
		    b64encode(utf_8_uri_encoded(&str_uri_encoded,p_tmp_buff,"&#",";"));

		free(str_uri_encoded);

		p_self->info[srv_cnt].credentials.encoded =
		    (p_self->info[srv_cnt].credentials.p_enc_usr_passwd_buffer != NULL);

		p_self->info[srv_cnt].credentials.size = strlen(p_self->info[srv_cnt].credentials.p_enc_usr_passwd_buffer);
	}
	while(0);

	if (p_tmp_buff != NULL)
	{
		free(p_tmp_buff);
	}

	return rc;
}

static void dyn_dns_print_hello(void*p)
{
	(void) p;

	DBG_PRINTF((LOG_SYSTEM, "S:" MODULE_TAG "Started 'inadyn-mt version %s' - dynamic DNS updater.\n", DYNDNS_VERSION_STRING));
}

static void destroy_custom_params_list(gen_list* params_list)
{
	gen_list_itr	itr=list_itr_create(*params_list,&itr);


	do {

		free(list_entry_remove(*params_list,itr));
	}
	while (!(list_end(itr)));

	list_destroy(params_list);
	list_itr_destroy(&itr);
}

static void destroy_custom_params(DYN_DNS_CLIENT *p_self)
{

	int		i;
	DYNDNS_SYSTEM	*p_static_user_defined=get_dns_system_by_id(CUSTOM_HTTP_BASIC_AUTH);


	for (i=0;i<p_self->srv_cnt;i++) {

		free(p_self->info[i].ip_param);
		p_self->info[i].ip_param=NULL;
		free(p_self->info[i].id_param);
		p_self->info[i].id_param=NULL;
		free(p_self->info[i].pw_param);
		p_self->info[i].pw_param=NULL;

		free(p_self->info[i].if_name);
		p_self->info[i].if_name=NULL;

		if (p_self->info[i].rsp_config_codes)

			destroy_custom_params_list(&p_self->info[i].rsp_config_codes);

		if (p_self->info[i].rsp_success_codes)

			destroy_custom_params_list(&p_self->info[i].rsp_success_codes);

		if (0==strcmp(USER_DEFINED_SERVER,p_self->info[i].p_dns_system->p_key)) {

			if (!(p_self->info[i].p_dns_system==p_static_user_defined)) {

				free(p_self->info[i].p_dns_system->p_key);
				p_self->info[i].p_dns_system->p_key=NULL;

				free(p_self->info[i].p_dns_system);
				p_self->info[i].p_dns_system=NULL;
			}
		}
	}
}

/*
	Disconnect and some other clean up.
*/
static RC_TYPE dyn_dns_shutdown(DYN_DNS_CLIENT *p_self)
{
	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	if (p_self->initialized == FALSE)
	{
		return RC_OK;
	}

	return RC_OK;
}

/*
	Sets up the object.
	- sets the IPs of the DYN DNS server
    - if proxy server is set use it!
	- ...
*/
static RC_TYPE do_dyn_dns_init(DYN_DNS_CLIENT *p_self)
{
	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	if (p_self->initialized == TRUE)
	{
		return RC_OK;
	}

	p_self->abort_on_network_errors = FALSE;
	p_self->force_addr_update = FALSE;

	dyn_dns_set_http_clients(p_self);

	p_self->cmd = NO_CMD;
	if (p_self->cmd_check_period == 0)
	{
		p_self->cmd_check_period = DYNDNS_DEFAULT_CMD_CHECK_PERIOD;
	}

	p_self->initialized = TRUE;
	return RC_OK;
}

static RC_TYPE dyn_dns_init(DYN_DNS_CLIENT *p_self)
{
	RC_TYPE	rc;

	if (!((rc=do_dyn_dns_init(p_self))==RC_OK))

		return rc;

	p_self->forced_update_counter=p_self->forced_update_period_sec/p_self->cmd_check_period;

	return RC_OK;
}

static RC_TYPE init_update_loop(DYN_DNS_CLIENT *p_dyndns,int argc, char* argv[],void **p_data,BOOL *init_flag)
{
	RC_TYPE	rc = RC_OK;
	RC_TYPE	rc_cmd_line = RC_OK;


#ifndef _WIN32

#ifdef USE_THREADS

	void	**p_ras_thread_data=p_data;
#endif

#else
	RAS_THREAD_DATA	**p_ras_thread_data= (RAS_THREAD_DATA **) p_data;
	int             regParamsC=1;
	wchar_t         *regArgs[50];
	char            *utf_8_argv[50];
	int             i=0;

	RC_TYPE         rc_reg=RC_ERROR;
#endif

#ifdef USE_THREADS

	threads_wrapper_init();

	memset(&update_timer,0,sizeof(my_timer_t));
#endif


	if (p_dyndns == NULL)

		return RC_INVALID_POINTER;

	/*load default data */
	rc = get_default_config_data(p_dyndns);
	if (rc != RC_OK)
	{
		return rc;
	}

#ifdef _WIN32

	SetLastError(0);

	/*set up to any registry parameters first -- input file,
	  and command args override registry*/

	utf_8_argv[0]=safe_malloc(strlen(argv[0])+1);
	strcpy(utf_8_argv[0],argv[0]);

	/*if we're a service, service_main will have these*/
	regParamsC+=get_registry_parameters(regArgs);

	if (regParamsC>1) {

		utf_16_to_8_ar((utf_8_argv+1),regArgs,regParamsC-1);

		rc_reg=do_get_config_data(p_dyndns,regParamsC,utf_8_argv);

		for (i=0;i<regParamsC;i++) free(regArgs[i]);

		utf_free_ar(utf_8_argv,regParamsC);

		if (!(rc_reg == RC_OK))

			return rc_reg;
	}

#endif

	/* read cmd line options and set object properties*/
	rc_cmd_line = get_config_data(p_dyndns, argc, argv);

#ifndef _WIN32

	if (rc_cmd_line != RC_OK || p_dyndns->abort)

#else

	if (!(rc_cmd_line == RC_OK || rc_reg == RC_OK) || p_dyndns->abort)

#endif

		return rc_cmd_line;

	if (p_dyndns->lang_hard_coded) {

		/*use hard coded defaults -- don't use default locale file*/
		dealloc_lang_strings();

		DBG_PRINTF((LOG_INFO, "I:" MODULE_TAG "Empty --lang_file parameter.  Deallocated language strings, using hard coded english defaults...\n"));
	}
	else {

		/*if opt around default language strings, use that*/
		if (p_dyndns->lang_file) {

			if (!(re_init_lang_strings(p_dyndns->lang_file)==RC_OK)) {

				DBG_PRINTF((LOG_WARNING, "W:" MODULE_TAG "Failed using default override language strings file, %s...\n",p_dyndns->lang_file));
			}
		}
	}

	/*if logfile provided, redirect output to log file*/
	if (!(p_dyndns->dbg.p_logfilename)) {

		/*set log level to default*/
		set_debug_level(p_dyndns->dbg.level);
	}
	else {

		DBG_PRINTF((LOG_INFO, "I:" MODULE_TAG "Attempting to open log file: %s, log level:  %i...\n",p_dyndns->dbg.p_logfilename,
				p_dyndns->dbg.level));

		if (!(RC_OK==(rc=os_open_dbg_output(DBG_FILE_LOG, "", p_dyndns->dbg.p_logfilename,p_dyndns->dbg.level))))

			return rc;
	}

	if (p_dyndns->debug_to_syslog == TRUE || (p_dyndns->run_in_background == TRUE))
	{
		if (get_dbg_dest() == DBG_STD_LOG) /*avoid file and syslog output */
		{
			rc = os_open_dbg_output(DBG_SYS_LOG, "INADYN", NULL,p_dyndns->dbg.level);
			if (rc != RC_OK)

				return rc;
		}
	}

	if (p_dyndns->change_persona)
	{
		OS_USER_INFO os_usr_info;
		memset(&os_usr_info, 0, sizeof(os_usr_info));
		os_usr_info.gid = p_dyndns->sys_usr_info.gid;
		os_usr_info.uid = p_dyndns->sys_usr_info.uid;
		rc = os_change_persona(&os_usr_info);
		if (rc != RC_OK)
		{
			return rc;
		}
	}

	/*if silent required, close console window*/
	if (p_dyndns->run_in_background == TRUE)
	{
		rc = close_console_window();
		if (rc != RC_OK)
		{
			return rc;
		}
		if (get_dbg_dest() == DBG_SYS_LOG)
		{
			fclose(stdout);
		}
	}

	/*if pid-file wanted, create it*/
	if (p_dyndns->p_pidfilename) {

		if (strlen(p_dyndns->p_pidfilename) != 0)
		{
			FILE *fp=NULL;
			int obj=0;
			char pid[7];


			if ((fp = utf_fopen(p_dyndns->p_pidfilename, "w")))
			{

#ifndef _WIN32
				if (0<snprintf(pid, 7, "%d\n", getpid()))
#else
				if (0<_snprintf(pid, 7, "%d\n", _getpid()))
#endif
					obj = fwrite(pid, strlen(pid), 1, fp);

				fclose(fp);
			}
			if (!fp || obj != 1)
			{
				return RC_FILE_IO_OPEN_ERROR;
			}
		}
	}

	dyn_dns_print_hello(NULL);

	if (p_dyndns->is_improbability)

		DBG_PRINTF((LOG_WARNING, "W:" MODULE_TAG "Improbability status:  In effect...\n"));

	/*  now that log is open, report any command line errors eventhough registry params made up for them --
	    if argc is 1, parser returns error if can't open default config file -- ignore that condition
	*/
	if (rc_cmd_line != RC_OK && argc > 1) {

		DBG_PRINTF((LOG_WARNING, "W:" MODULE_TAG "%s error returned getting command line "\
		            "parameters.  One or more command line parameters ignored...\n",errorcode_get_name(rc_cmd_line)));

	}

	ip_cache_list_read(p_dyndns,&is_global_update_pending);

	rc = dyn_dns_init(p_dyndns);

	if (rc==RC_OK) {

		*init_flag=true;

		rc = os_install_signal_handler(p_dyndns);

		if (rc != RC_OK)

			DBG_PRINTF((LOG_WARNING,"DYNDNS: Error '%s' (0x%x) installing OS signal handler\n",errorcode_get_name(rc), rc));
	}

	if (rc==RC_OK) {

		char	i;
		RC_TYPE	pw_rc;


		for (i=0;i<p_dyndns->srv_cnt;i++) {

			if (!(RC_OK==(pw_rc=get_encoded_user_passwd(p_dyndns,i))))

				if (rc==RC_OK) /*return first of any error(s)*/

					rc=pw_rc;
		}

#ifdef _WIN32

		/*
			Windows RAS/connect/disconnect events threads
		*/
		start_ras_thread(p_dyndns,p_ras_thread_data);
#endif

#ifdef USE_THREADS

		/*
			online status thread
		*/
		start_online_test_thread(p_dyndns,p_ras_thread_data);

		/*use update_timer when out of cmd wait loop servicing commands --
		  update/net comm can take some time, especially with retries
		  parameter(s) set.  Makes forced_update_period more accurate.*/
		create_timer(&update_timer,25);
#endif

#ifdef USE_SNDFILE

		waveout_init();
#endif

	}

	return rc;
}

/*
	Resource free.
*/
RC_TYPE dyn_dns_destruct(DYN_DNS_CLIENT *p_self)
{
	char	i;
	char	ii;


	if (p_self == NULL)
	{
		return RC_OK;
	}

	if (p_self->initialized == TRUE)
	{
		dyn_dns_shutdown(p_self);
	}

	free(p_self->ip_server_name_global.name);
	p_self->ip_server_name_global.name=NULL;
	free(p_self->ip_server_name_global.url);
	p_self->ip_server_name_global.url=NULL;

	for (i=0;i<MAX_DNS_SERVER;i++) {

		http_client_destruct(&p_self->info[i].http_to_ip_server);
		http_client_destruct(&p_self->info[i].http_to_dyndns);

		free(p_self->info[i].credentials.p_enc_usr_passwd_buffer);
		p_self->info[i].credentials.p_enc_usr_passwd_buffer=NULL;

		free(p_self->info[i].credentials.my_username);
		p_self->info[i].credentials.my_username=NULL;

		free(p_self->info[i].credentials.my_password);
		p_self->info[i].credentials.my_password=NULL;

		free(p_self->info[i].dyndns_server_name.url);
		p_self->info[i].dyndns_server_name.url=NULL;

		free(p_self->info[i].proxy_server_name.name);
		p_self->info[i].proxy_server_name.name=NULL;

		free(p_self->info[i].ip_server_name.name);
		p_self->info[i].ip_server_name.name=NULL;

		free(p_self->info[i].ip_server_name.url);
		p_self->info[i].ip_server_name.url=NULL;

		free(p_self->info[i].ip_server_host);
		p_self->info[i].ip_server_host=NULL;

		free(p_self->info[i].dyndns_server_host);
		p_self->info[i].dyndns_server_host=NULL;

		for (ii=0;ii<p_self->info[i].alias_info.count;ii++) {

			free(p_self->info[i].alias_info.names[ii].name);
			p_self->info[i].alias_info.names[ii].name=NULL;

			free(p_self->info[i].alias_info.svr_hashes[i].alias);
			p_self->info[i].alias_info.svr_hashes[i].alias=NULL;
			free(p_self->info[i].alias_info.svr_hashes[i].str);
			p_self->info[i].alias_info.svr_hashes[i].str=NULL;
		}

	}

	if (p_self->p_req_buffer != NULL)
	{
		free(p_self->p_req_buffer);
		p_self->p_req_buffer = NULL;
	}


	if (p_self->lang_file != NULL)
	{

		free(p_self->lang_file);
		p_self->lang_file = NULL;
	}

	if (p_self->dbg.p_logfilename != NULL)
	{

		free(p_self->dbg.p_logfilename);
		p_self->dbg.p_logfilename = NULL;
	}

	if (p_self->ip_cache != NULL)
	{
		free(p_self->ip_cache);
		p_self->ip_cache=NULL;
	}

	if (p_self->time_cache != NULL)
	{
		free(p_self->time_cache);
		p_self->time_cache=NULL;
	}


	if (p_self->external_command != NULL)
	{

		free(p_self->external_command);
		p_self->external_command=NULL;
	}

	free(p_self->p_pidfilename);
	p_self->p_pidfilename = NULL;

	ip_cache_list_destruct(&p_self->ip_cache_list);

	destroy_custom_params(p_self);

	free(p_self);
	p_self = NULL;

	return RC_OK;
}

/*
        See if we can find servers_additional.cfg.
*/
static void get_servers_additional(DYN_DNS_CLIENT *p_self)
{
	char	path[]=".";
	char	file_name[]="servers_additional.cfg";
	char	*a_path=NULL;

	do {

#ifdef _WIN32
		/*module dir*/
		if (p_self->servers_additional=get_file(a_path=dir_delimited(modulePath(NULL,&a_path)),file_name)) {

			free(a_path);

			break;
		}

		free(a_path);

		{
			char    path[]="extra";

			/*extra dir 2 above module dir*/
			if ((p_self->servers_additional=searchedProgFile(NULL,&p_self->servers_additional,a_path=dir_delimited(path_ancestor(a_path,modulePath(NULL,&a_path),2)),path,3,file_name))) {

				free(a_path);

				break;
			}

			free(a_path);
		}

#endif

		{
			/*cwd*/
			if ((p_self->servers_additional=get_file(a_path=dir_delimited(cross_platform_cwd(&a_path)),file_name))) {

				free(a_path);

				break;
			}

			free(a_path);
		}

		{
			char	path[]="." DIR_DELIM_STR "extra" DIR_DELIM_STR;

			cross_platform_cwd(&a_path);
			a_path=safe_realloc(a_path,strlen(a_path)+strlen("extra")+strlen(DIR_DELIM_STR)+2);
			sprintf(a_path,"%s%sextra%s",a_path,DIR_DELIM_STR,DIR_DELIM_STR);

			/*extra dir off of cwd*/
			if ((p_self->servers_additional=get_file(a_path,file_name))) {

				free(a_path);

				break;
			}

			free(a_path);
		}

#ifndef _WIN32

		{
			char	path[]="/etc/";

			if ((p_self->servers_additional=get_file(path,file_name)))

				break;
		}

		{
			char	path[]="/etc/inadyn-mt/";

			if ((p_self->servers_additional=get_file(path,file_name)))

				break;
		}

		{
			char    path[]="/etc/inadyn-mt/extra/";

			if ((p_self->servers_additional=get_file(path,file_name)))

				break;
		}

		{
			char	path[]="/usr/share/inadyn-mt/extra/";

			if ((p_self->servers_additional=get_file(path,file_name)))

				break;
		}
#endif
	}
	while(0);

	if (p_self->servers_additional)

		DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "Additional servers at:  %s...\n",p_self->servers_additional));
	else
		DBG_PRINTF((LOG_SYSTEM,"S:" MODULE_TAG "Could not open default servers_additional.cfg.  Will try svr_add_cfg option, if set...\n"));
}

/*
	 basic resource allocations for the dyn_dns object
*/
RC_TYPE dyn_dns_construct(DYN_DNS_CLIENT **pp_self)
{
	RC_TYPE		rc;
	DYN_DNS_CLIENT	*p_self;
	BOOL		http_to_dyndns_constructed = FALSE;
	BOOL		http_to_ip_constructed = FALSE;
	char i;


	if (pp_self == NULL)
	{
		return RC_INVALID_POINTER;
	}
	/*alloc space for me*/
	*pp_self = (DYN_DNS_CLIENT *) safe_malloc(sizeof(DYN_DNS_CLIENT));
	if (*pp_self == NULL)
	{
		return RC_OUT_OF_MEMORY;
	}

	do
	{
		p_self = *pp_self;
		memset(p_self, 0, sizeof(DYN_DNS_CLIENT));

		/*alloc space for request data*/
		p_self->req_buffer_size = DYNDNS_HTTP_REQUEST_BUFFER_SIZE;
		p_self->p_req_buffer = (char*) safe_malloc(p_self->req_buffer_size);
		if (p_self->p_req_buffer == NULL)
		{
			rc = RC_OUT_OF_MEMORY;
			break;
		}


		for (i=0;i<MAX_DNS_SERVER;i++) {

			rc = http_client_construct(&p_self->info[i].http_to_ip_server);
			if (rc != RC_OK)
			{
				rc = RC_OUT_OF_MEMORY;
				break;
			}

			rc = http_client_construct(&p_self->info[i].http_to_dyndns);
			if (rc != RC_OK)
			{
				rc = RC_OUT_OF_MEMORY;
				break;
			}

			p_self->info[i].credentials.p_enc_usr_passwd_buffer = NULL;
		}

		http_to_ip_constructed = TRUE;
		http_to_dyndns_constructed = TRUE;

		(p_self)->cmd = NO_CMD;
		(p_self)->sleep_sec = DYNDNS_DEFAULT_SLEEP;
		(p_self)->total_iterations = DYNDNS_DEFAULT_ITERATIONS;
		(p_self)->initialized = FALSE;

		p_self->lang_file = NULL;

		get_servers_additional(p_self);
	}
	while(0);

	if (rc != RC_OK)
	{
		if (*pp_self)
		{
			free(*pp_self);
		}

		if (p_self->p_req_buffer != NULL)
		{
			free (p_self->p_req_buffer);
		}

		{
			for (i=0;i<MAX_DNS_SERVER;i++) {

				http_client_destruct(&p_self->info[i].http_to_ip_server);
				http_client_destruct(&p_self->info[i].http_to_dyndns);
			}
		}
	}

	return RC_OK;
}

