/*
Copyright (C) 2015 Bryan Hoover (bhoover@wecs.com)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3
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
		Author:	bhoover@wecs.com
		Date:	May 2015

		History:
				- created
*/

#include <stdio.h>
#include <time.h>
#include <wctype.h>

#include "unicode_util.h"
#include "cache_list.h"
#include "safe_mem.h"
#include "debug_if.h"
#include "ip.h"
#include "path.h"

#define MODULE_TAG	"CACHE_LIST "

#define false	0
#define true	1

typedef enum
{
    GET_IP,
    GET_AFTER_IP,
    GET_ALIAS,
    GET_AFTER_ALIAS,
    GET_TYPE,
    GET_SERVER
} PARSER_STATE;

typedef struct
{
	FILE		*p_file;
	PARSER_STATE	state;
} OPTION_FILE_PARSER;

/*find the alias given by parameter alias in list of p_dyndns structure alias names and set
  correponding update state boolean to parameter is_updated
*/
static void set_update_state(DYN_DNS_CLIENT *p_dyndns,char *alias,char *alias_type,char *server,int is_updated)
{

	int	i;
	int	ii;
	char	*alias_ip_v;
	char	*alias_cmp_src;
	int	is_auto=false;
	int	is_found=false;


	is_auto=!(strcmp((alias_type+1),"auto"));

	alias_ip_v=safe_malloc(strlen(alias)+strlen(alias_type)+2);
	sprintf(alias_ip_v,"%s:%s",alias,alias_type);

	for (ii=0;ii<p_dyndns->srv_cnt;ii++) {

		for (i=0;i<p_dyndns->info[ii].alias_info.count;i++) {

			/*add ip type [auto,coupled,ip6,ip4] to name[i].name to do compare*/

			alias_cmp_src=safe_malloc(strlen(p_dyndns->info[ii].alias_info.names[i].name)+9);
			sprintf(alias_cmp_src,"%s:%s",p_dyndns->info[ii].alias_info.names[i].name,p_dyndns->info[ii].alias_info.names[i].ip_v);

			if ((!(strcmp(alias_cmp_src,alias_ip_v)) || (strstr(p_dyndns->info[ii].alias_info.names[i].ip_v,DUAL_LIST)))
				&& !(strcmp(p_dyndns->info[ii].alias_info.names[i].name,alias))
				&& !(strcmp(p_dyndns->info[ii].dyndns_server_host,server))) {

				p_dyndns->info[ii].alias_info.update_succeeded[i]=is_updated;

				is_found=true;
			}
			else {

				/*for aliases of type auto, set update state for either ip6, ip4*/

				if (is_auto)

					if (!(strcmp(p_dyndns->info[ii].alias_info.names[i].name,alias))
						&& !(strcmp(p_dyndns->info[ii].dyndns_server_host,server))) {

						p_dyndns->info[ii].alias_info.update_succeeded[i]=is_updated;

						is_found=true;
					}
			}

			free(alias_cmp_src);

			if (is_found)

				break;
		}
	}

	free(alias_ip_v);
}

static RC_TYPE push_in_buffer(char* p_src, int src_len, char *p_buffer, int *p_act_len, int max_len)
{
	if (*p_act_len + src_len > max_len)

		return RC_FILE_IO_OUT_OF_BUFFER;

	memcpy(p_buffer + *p_act_len, p_src, src_len);

	*p_act_len += src_len;

	return RC_OK;
}

static RC_TYPE parser_init(OPTION_FILE_PARSER *p_cfg, FILE *p_file)
{
	memset(p_cfg, 0, sizeof(*p_cfg));
	p_cfg->state = GET_IP;
	p_cfg->p_file = p_file;
	return RC_OK;
}

static void set_token(char **token,char *p_buffer,int *count,int maxlen)
{

	*token=safe_malloc(strlen(p_buffer)+1);
	strcpy(*token,p_buffer);
	memset(p_buffer,0,maxlen);
	*count=0;
}

static void reset_vars(char **ip,char **alias,char **ip_type,char **server)
{

	free(*ip); free(*alias); free(*ip_type); free(*server);
	*ip=NULL; *alias=NULL; *ip_type=NULL; *server=NULL;
}

static RC_TYPE parser_read(OPTION_FILE_PARSER *p_cfg,IP_FILE_PARSER *p_ip_parser,char *p_buffer,int maxlen,char *ch,int *count,int *parse_end,DYN_DNS_CLIENT *p_dyndns)
{
	static char	*ip=NULL;
	static char	*alias=NULL;
	static char	*ip_type=NULL;
	static char	*server=NULL;

	int		ip_parse_end=0;
	RC_TYPE		rc;

	switch (p_cfg->state)
	{

		case GET_IP:

			if (!(RC_OK==(rc=ip_parser_read(p_ip_parser,p_buffer,maxlen,ch,count,&ip_parse_end)))) {

				DBG_PRINTF((LOG_DEBUG, "D:" MODULE_TAG "ip_parser_read returned not rc_ok\n"));

				return rc;
			}

			if (ip_parse_end) {

				set_token(&ip,p_buffer,count,maxlen);

				p_cfg->state = GET_AFTER_IP;

				return RC_OK;
			}

			return rc;

		case GET_AFTER_IP:

			/*may use alias for other than host name, so
			  don't worry about spaces*/

			if (!(strcmp(ch,"\n")) || !(strcmp(ch,":"))) {

				reset_vars(&ip,&alias,&ip_type,&server);

				return RC_ERROR;
			}

			push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);

			p_cfg->state = GET_ALIAS;

			return RC_OK;

		case GET_ALIAS:

			if (!(strcmp(ch,":"))) {

				set_token(&alias,p_buffer,count,maxlen);

				p_cfg->state = GET_AFTER_ALIAS;

				return RC_OK;
			}

			if (!(strcmp(ch,"\n"))) {

				reset_vars(&ip,&alias,&ip_type,&server);

				return RC_ERROR;
			}

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);

		case GET_AFTER_ALIAS:

			if (!(iswalnum(utf_8_code_point(ch)))) {

				reset_vars(&ip,&alias,&ip_type,&server);

				return RC_ERROR;
			}

			/*alias type is at least 3 chars long*/
			if (strlen(p_buffer)>1)

				p_cfg->state = GET_TYPE;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);

		case GET_TYPE:

			if ((iswspace(utf_8_code_point(ch)))) {

				if (!(!(strcmp(p_buffer,"auto")) || !(strcmp(p_buffer,"ip4")) || !(strcmp(p_buffer,"ip6")) ||
					!(strcmp(p_buffer,"dual")) || !(strcmp(p_buffer,"coupled")))) {

					reset_vars(&ip,&alias,&ip_type,&server);

					return RC_ERROR;
				}

				set_token(&ip_type,p_buffer,count,maxlen);

				p_cfg->state = GET_SERVER;

				return RC_OK;
			}

			if (!(iswalnum(utf_8_code_point(ch)))) {

				reset_vars(&ip,&alias,&ip_type,&server);

				return RC_ERROR;
			}

			return push_in_buffer(ch,strlen(ch),p_buffer,count,maxlen);

		case GET_SERVER:

			if (!(iswalnum((utf_8_code_point(ch)))) && strcmp(ch,".") && strcmp(ch,"-") & strcmp(ch,"\n")) {

				reset_vars(&ip,&alias,&ip_type,&server);

				return RC_ERROR;
			}

			if (*parse_end=(!(strcmp(ch,"\n")))) {

				set_token(&server,p_buffer,count,maxlen);

				ip_cache_list_add(&p_dyndns->ip_cache_list,ip,alias,ip_type,server);

				set_update_state(p_dyndns,alias,ip_type,server,1);

				DBG_PRINTF((LOG_WARNING, "W:" MODULE_TAG "%s %s:%s %s...\n",ip,alias,ip_type,server));

				reset_vars(&ip,&alias,&ip_type,&server);

				p_cfg->state = GET_IP;

				return RC_OK;
			}

			return push_in_buffer(ch,strlen(ch),p_buffer,count,maxlen);

		default:

			return RC_OK;
	}
}

static int number_of_aliases(DYN_DNS_CLIENT *p_dyndns)
{
	char    i;
	int     alias_count=0;


	for (i=0;i<MAX_DNS_SERVER;i++) {

		alias_count+=p_dyndns->info[i].alias_info.count;
	}

	return alias_count;
}

RC_TYPE ip_cache_list_read(DYN_DNS_CLIENT *p_dyndns,volatile int *is_need_update)
{
	FILE			*fp=NULL;
	RC_TYPE			rc=RC_OK;
	int			parse_end=0;
	int			count=0;
	char			ch[7];
	OPTION_FILE_PARSER	p_cfg;
	char			*p_buffer;
	int			max_len=1023;
	int			alias_count=0;
	IP_FILE_PARSER		ip_parser;
	char			*cache_file=p_dyndns->ip_cache;

#ifndef _WIN32
	fp=utf_fopen(cache_file,"r");
#else
	cache_file=short_name(p_dyndns->ip_cache);

	if (!(strlen(cache_file))) {

		free(cache_file);
	}
	else {

		p_dyndns->ip_cache=cache_file;

		fp=utf_fopen(cache_file,"r");
	}
#endif

	if (!(fp)) {

		DBG_PRINTF((LOG_ERR,"E:" MODULE_TAG "Failed opening cache file in ip_cache_list_read...\n"));

		rc=RC_ERROR;
	}
	else {

		p_buffer=safe_malloc(max_len+1);

		parser_init(&p_cfg,NULL);
		ip_parser_init(&ip_parser,NULL);

		memset(&p_dyndns->ip_cache_list,0,sizeof(IP_CACHE_LIST));

		DBG_PRINTF((LOG_WARNING, "W:" MODULE_TAG "Address, alias pairs read from cache file (%s):\n",cache_file));

		while (1) {

			memset(ch,0,7);

			if (!(utf_read_utf_8(ch,fp)))

				break;

			if (RC_OK==(rc=parser_read(&p_cfg,&ip_parser,p_buffer,max_len,ch,&count,&parse_end,p_dyndns))) {

				if (parse_end) {

					alias_count++;

					parser_init(&p_cfg,NULL);
					ip_parser_init(&ip_parser,NULL);
					memset(p_buffer,0,max_len);
					parse_end=0;
					count=0;
				}
			}
			else {
				DBG_PRINTF((LOG_CRIT, "C:" MODULE_TAG "Error parsing cache file.  Overwritting...\n"));

				fclose(fp);

				fp=utf_fopen(cache_file,"w");

				ip_cache_list_destruct(&p_dyndns->ip_cache_list);

				alias_count=0;

				break;
			}
		}

		if (fp)
			fclose(fp);
	}

	*is_need_update=(alias_count<number_of_aliases(p_dyndns));

	return rc;
}

/*for auto type aliases, ip server determines ip type (effectively, first socket type returned by getaddrinfo) - if ip
  server is dual stack, ip type is ip6, ip4 otherwise.  So our logic is, if check_my_ip_address got an ip6 address for
  this p_address, type is ip6, ip4 otherwise.
*/
DYNDNS_IPV actual_alias_ipv(IP_ADDRESS *p_address,DYNDNS_IPV ip_v)
{

        /*type ip_store==auto*/
        if (!(ip_store==ip_v))

                return ip_v;

        if (strlen(p_address->my_ip_address.name[ip_6]))

                return ip_6;

        return ip_4;
}

/*If alias marked as successful update, set ip(s) to new one(s), and return true.

  If alias not marked as successfull update, look up cache entry & set ip(s) to that/those and return true.
  Otherwise if not in cache, return false, with ip's set NULL.
*/
static int get_update_addresses(DYN_DNS_CLIENT *p_self,int svr_index,int alias_index,char **p_ip_addr,char **p_ip4_addr,char **p_ip6_addr,
				int is_dyndns_dual,DYNDNS_IPV ip_enum)
{
	int	cache_index;

	*p_ip_addr=NULL;
	*p_ip4_addr=NULL;
	*p_ip6_addr=NULL;

	if (p_self->info[svr_index].alias_info.update_succeeded[alias_index]) {

		if (!(is_dyndns_dual))

			*p_ip_addr=p_self->info[svr_index].alias_info.p_address[alias_index]->my_ip_address.name[ip_enum];
		else {
			*p_ip4_addr=p_self->info[svr_index].alias_info.p_address[alias_index]->my_ip_address.name[ip_4];
			*p_ip6_addr=p_self->info[svr_index].alias_info.p_address[alias_index]->my_ip_address.name[ip_6];
		}
	}
	else {
		if (!(is_dyndns_dual)) {

			cache_index=ip_cache_list_short_index_of(&p_self->ip_cache_list,
						p_self->info[svr_index].alias_info.names[alias_index].name,
						p_self->info[svr_index].dyndns_server_host,
						p_self->info[svr_index].alias_info.names[alias_index].ip_v);

			if (-1==cache_index)

				return 0;
			else
				*p_ip_addr=p_self->ip_cache_list.saved_ip_cache[cache_index].ip;
		}
		else {
			cache_index=ip_cache_list_short_index_of(&p_self->ip_cache_list,
						p_self->info[svr_index].alias_info.names[alias_index].name,
						p_self->info[svr_index].dyndns_server_host,
						"ipv4");

			if (!(-1==cache_index))

				*p_ip4_addr=p_self->ip_cache_list.saved_ip_cache[cache_index].ip;

			cache_index=ip_cache_list_short_index_of(&p_self->ip_cache_list,
					p_self->info[svr_index].alias_info.names[alias_index].name,
					p_self->info[svr_index].dyndns_server_host,
					"ipv6");

			if (!(-1==cache_index))

				*p_ip6_addr=p_self->ip_cache_list.saved_ip_cache[cache_index].ip;

			if (!(*p_ip4_addr || *p_ip6_addr))

				return 0;
		}
	}

	return 1;
}

RC_TYPE ip_cache_list_update_state(DYN_DNS_CLIENT *p_self,int updates_needed,int success_updates,
						int config_fails,RC_TYPE rc,char srv_index,volatile int *is_global_success_updates,
						volatile int *is_global_in_proc_update_pending)
{
	FILE			*fp;
	DYNDNS_INFO_TYPE	*info;


	info=&p_self->info[srv_index];

	/*global successes maintained across server iterations in main loop*/
	*is_global_success_updates=(*is_global_success_updates || success_updates);

	if (config_fails==info->alias_info.count) {

		rc=RC_DYNDNS_RSP_CONFIG;

		if (!(srv_index==p_self->srv_cnt-1))

			return rc;
	}

	/*
		reset on last server if any server successes
	*/
	if (*is_global_success_updates && srv_index==p_self->srv_cnt-1) {

		/*reset forced update period*/
		p_self->forced_update_counter=p_self->forced_update_period_sec_orig/p_self->cmd_check_period;
		p_self->forced_update_period_sec=p_self->forced_update_period_sec_orig;

		if ((fp=utf_fopen(p_self->time_cache, "w"))) {

			fprintf(fp,"%ld",time(NULL));

			fclose(fp);
		}

		fp=utf_fopen(p_self->ip_cache, "w");

		{
			IP_CACHE_LIST	this_cache_list;
			char		*p_ip_addr;
			char		*p_ip4_addr;
			char		*p_ip6_addr;
			int		i;
			int		ii;
			DYNDNS_IPV	ip_enum;
			BOOL		is_dyndns_dual;

			memset(&this_cache_list,0,sizeof(IP_CACHE_LIST));

			for (ii=0;ii<p_self->srv_cnt;ii++) {

				for (i=0;i<p_self->info[ii].alias_info.count;i++) {

					is_dyndns_dual=(NULL!=strstr(p_self->info[ii].alias_info.names[i].ip_v,DUAL_LIST));

					ip_enum=actual_alias_ipv(p_self->info[ii].alias_info.p_address[i],
								p_self->info[ii].alias_info.names[i].ip_v_enum);

					/*figure out whether updated to new, or failed and need rewrite cache to that read last*/
					if (!(get_update_addresses(p_self,ii,i,&p_ip_addr,&p_ip4_addr,&p_ip6_addr,is_dyndns_dual,ip_enum)))

						continue;

					if (!(is_dyndns_dual)) {

						ip_cache_list_add(&this_cache_list,p_ip_addr,
								p_self->info[ii].alias_info.names[i].name,p_self->info[ii].
								alias_info.names[i].ip_v,p_self->info[ii].dyndns_server_host);
						if (fp)
							fprintf(fp,"%s %s:%s %s\n",p_ip_addr,
								p_self->info[ii].alias_info.names[i].name,p_self->info[ii].
								alias_info.names[i].ip_v,p_self->info[ii].dyndns_server_host);
					}
					else {
						if (p_ip4_addr)

							ip_cache_list_add(&this_cache_list,p_ip4_addr,p_self->info[ii].alias_info.names[i].name,
								"ip4",p_self->info[ii].dyndns_server_host);

						if (p_ip6_addr)

							ip_cache_list_add(&this_cache_list,p_ip6_addr,p_self->info[ii].alias_info.names[i].name,
								"ip6",p_self->info[ii].dyndns_server_host);

						if (fp) {

							if (p_ip4_addr)

								fprintf(fp,"%s %s:ip4 %s\n",p_ip4_addr,p_self->info[ii].alias_info.names[i].name,
									p_self->info[ii].dyndns_server_host);

							if (p_ip6_addr)

								fprintf(fp,"%s %s:ip6 %s\n",p_ip6_addr,p_self->info[ii].alias_info.names[i].name,
									p_self->info[ii].dyndns_server_host);
						}
					}
				}
			}

			if (fp)
				fclose(fp);

			ip_cache_list_destruct(&p_self->ip_cache_list);
			p_self->ip_cache_list.saved_ip_cache=this_cache_list.saved_ip_cache;
			p_self->ip_cache_list.count=this_cache_list.count;
		}
	}

	/*any pending?*/
	if ((updates_needed && !(success_updates==updates_needed))) {

		if (rc==RC_OK) /*not break after http client init*/

			rc=RC_DYNDNS_RSP_NOTOK;

		DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "One or more (%d) alias updates failed...\n",
						updates_needed-success_updates));
	}
	else {

		/*no pendings this server*/
		if (success_updates) {

			/*no pendings any server*/
			if (!(*is_global_in_proc_update_pending) && srv_index==p_self->srv_cnt-1)

				p_self->is_forced_update_attempted=false;

			rc=RC_OK;
		}
	}

	return rc;
}

void ip_cache_list_add(IP_CACHE_LIST *ip_cache_list,char *ip,char *alias,char *ip_type,char *server)
{
	int	cache_index;
	char	*ip_alias_type;


	ip_alias_type=safe_malloc(strlen(ip)+strlen(alias)+strlen(ip_type)+3);
	sprintf(ip_alias_type,"%s %s:%s",ip,alias,ip_type);

	cache_index=ip_cache_list->count;
	ip_cache_list->count+=1;

	ip_cache_list->saved_ip_cache=safe_realloc(ip_cache_list->saved_ip_cache,sizeof(IP_CACHE)*ip_cache_list->count);

	ip_cache_list->saved_ip_cache[cache_index].ip=safe_malloc(strlen(ip)+1);
	ip_cache_list->saved_ip_cache[cache_index].alias=safe_malloc(strlen(alias)+1);
	ip_cache_list->saved_ip_cache[cache_index].type=safe_malloc(strlen(ip_type)+1);
	ip_cache_list->saved_ip_cache[cache_index].ip_alias_type=safe_malloc(strlen(ip_alias_type)+1);
	ip_cache_list->saved_ip_cache[cache_index].server=safe_malloc(strlen(server)+1);

	strcpy(ip_cache_list->saved_ip_cache[cache_index].ip,ip);
	strcpy(ip_cache_list->saved_ip_cache[cache_index].alias,alias);
	strcpy(ip_cache_list->saved_ip_cache[cache_index].type,ip_type);
	strcpy(ip_cache_list->saved_ip_cache[cache_index].ip_alias_type,ip_alias_type);
	strcpy(ip_cache_list->saved_ip_cache[cache_index].server,server);

	inet_pton_w(strpbrk(ip,":") ? AF_INET6: AF_INET,ip,ip_cache_list->saved_ip_cache[cache_index].ip_n);


	free(ip_alias_type);
}

RC_TYPE ip_cache_list_update(DYN_DNS_CLIENT *p_dyndns,int *is_need_update)
{

	ip_cache_list_destruct(&p_dyndns->ip_cache_list);

	return ip_cache_list_read(p_dyndns,is_need_update);
}

int ip_cache_list_index_of(IP_CACHE_LIST *s_list,void *in_addr,char *ip,char *alias,char *server,char *ip_alias_type)
{
	int	i;
	int	sock_size;


	DBG_PRINTF((LOG_INFO, "I:DYNDNS: Searching for ip, alias %s %s, among %i ip, alias pairs...\n",ip,alias,s_list->count));

	if (strpbrk(ip,":"))

		sock_size=sizeof(struct in6_addr);
	else
		sock_size=sizeof(struct in_addr);

	for (i=0;i<s_list->count;i++) {

		DBG_PRINTF((LOG_INFO,"I:DYNDNS: %s, COMPARING TO %s\n",s_list->saved_ip_cache[i].ip_alias_type,ip_alias_type));

		if (0==strcmp(s_list->saved_ip_cache[i].alias,alias))

			if (0==memcmp(s_list->saved_ip_cache[i].ip_n,in_addr,sock_size))

				if (0==strcmp(s_list->saved_ip_cache[i].server,server))

					return i;
	}

	return -1;
}

int ip_cache_list_short_index_of(IP_CACHE_LIST *s_list,char *alias,char *server,char *ip_type)
{
	int	i;

	for (i=0;i<s_list->count;i++) {

		if (0==strcmp(s_list->saved_ip_cache[i].alias,alias))

			if (0==strcmp(s_list->saved_ip_cache[i].server,server))

				if (0==strcmp(s_list->saved_ip_cache[i].type,ip_type))

					return i;
	}

	return -1;
}

void ip_cache_list_destruct(IP_CACHE_LIST *ip_cache_list)
{

	int i;

	for (i=0;i<ip_cache_list->count;i++) {

 		free(ip_cache_list->saved_ip_cache[i].ip);
		free(ip_cache_list->saved_ip_cache[i].alias);
		free(ip_cache_list->saved_ip_cache[i].type);
		free(ip_cache_list->saved_ip_cache[i].ip_alias_type);
		free(ip_cache_list->saved_ip_cache[i].server);
	}

	ip_cache_list->saved_ip_cache=NULL;

	ip_cache_list->count=0;
}

