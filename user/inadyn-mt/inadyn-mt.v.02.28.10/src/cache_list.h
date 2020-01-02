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

#include "dyndns.h"

RC_TYPE ip_cache_cache_list(DYN_DNS_CLIENT *p_dyndns);
void ip_cache_list_add(IP_CACHE_LIST *ip_cache_list,char *ip,char *alias,char *ip_type,char *server);
int ip_cache_list_index_of(IP_CACHE_LIST *s_list,void *in_addr,char *ip,char *alias,char *server,char *ip_alias_type);
int ip_cache_list_short_index_of(IP_CACHE_LIST *s_list,char *alias,char *server,char *ip_type);
RC_TYPE ip_cache_list_update_state(DYN_DNS_CLIENT *p_self,int updates_needed,int success_updates,int config_fails,RC_TYPE rc,char srv_index,volatile int *is_global_success_updates,volatile int *is_global_in_proc_update_pending);
RC_TYPE ip_cache_list_read(DYN_DNS_CLIENT *p_dyndns,volatile int *is_need_update);
RC_TYPE ip_cache_list_update(DYN_DNS_CLIENT *p_dyndns,int *is_need_update);
void ip_cache_list_destruct(IP_CACHE_LIST *ip_cache_list);
DYNDNS_IPV actual_alias_ipv(IP_ADDRESS *p_address,DYNDNS_IPV ip_v);

