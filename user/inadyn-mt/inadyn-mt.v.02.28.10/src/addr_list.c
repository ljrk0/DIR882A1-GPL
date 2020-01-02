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
	Linked list of network interfaces containing linked list of corresponding addresses structures
	containing attributes relative to address retrieval and storage, and DDNS server address update.

	Author: bhoover@wecs.com
	Date: Dec 2015

	History:
		- first implemetnation
*/

#define MODULE_TAG	"ADDR_LIST: "

#include "addr_list.h"
#include "errorcode.h"
#include "safe_mem.h"

#include "debug_if.h"

typedef struct
{
        DYNDNS_IPV	ip_enum;
        const char	*ip_str;
} IPV_NAME;

static const IPV_NAME global_ipv_table[]  =
{
	{ip_4,"ip4"},
	{ip_6,"ip6"},
	{ip_store,"auto"},
	{ip_null,NULL}
};

const char *ip_enum_to_str(DYNDNS_IPV ip_enum)
{
	const IPV_NAME	*p_enum=global_ipv_table;

	while (!(ip_null==p_enum->ip_enum)) {

		if (ip_enum==p_enum->ip_enum)

			return p_enum->ip_str;

		p_enum++;
	}

	return NULL;
}

static void mem_safe_var_alloc(void **var,int size)
{
	void	*p_tmp;


	if (!(-1==size)) /*-1 == character string*/

		p_tmp=safe_malloc(size);
	else
		if (!(*var))

			p_tmp=safe_malloc(1);
                else {

                        size=strlen((char *) (*var))+1;

                        p_tmp=safe_malloc(size);
                }

	if (*var)

		memcpy(p_tmp,*var,size);

	*var=p_tmp;
}

static void mem_safe_var_free(void *var)
{

	free(var);
}

/*render vars for pointer safe comparison.  could generalize this with var,
  size pairs, variable params, passing number of variables as first param.
*/
static void mem_safe_alloc(char **iface_name,char **ip_ver,char **ip_server,char **ip_server_url,char **ip_server_host,char **ip_proxy_name)
{

	mem_safe_var_alloc((void *) iface_name,-1);
	mem_safe_var_alloc((void *) ip_ver,-1);
	mem_safe_var_alloc((void *) ip_server,-1);
	mem_safe_var_alloc((void *) ip_server_url,-1);
	mem_safe_var_alloc((void *) ip_server_host,-1);
	mem_safe_var_alloc((void *) ip_proxy_name,-1);
}

static void mem_safe_free(char *iface_name,char *ip_ver,char *ip_server,char *ip_server_url,char *ip_server_host,char *ip_proxy_name)
{

	free(iface_name);
	free(ip_ver);
	free(ip_server_url);
	free(ip_server);
	free(ip_server_host);
	free(ip_proxy_name);
}

void iface_destruct(I_FACE **i_face)
{


}

I_FACE* iface_create(I_FACE **i_face,char *name)
{

	*i_face=safe_malloc(sizeof(I_FACE));

	list_create(&(*i_face)->address);

	if (!(name)) {

		(*i_face)->name=safe_malloc(1);
	}
	else {
		(*i_face)->name=safe_malloc(strlen(name)+1);
		strcpy((*i_face)->name,name);
	}

	return *i_face;
}

static void set_update_requirements(IP_ADDRESS *address,char *ip_ver_str)
{

	address->is_update_auto=(address->is_update_auto || strstr(ip_ver_str,"auto") || !(strlen(ip_ver_str)));

	address->is_update_ip4=(address->is_update_ip4 || strstr(ip_ver_str,"ip4") || strstr(ip_ver_str,DUAL_LIST));

	address->is_update_ip6=(address->is_update_ip6 || strstr(ip_ver_str,"ip6") || strstr(ip_ver_str,DUAL_LIST));
}

IP_ADDRESS* ip_address_create(IP_ADDRESS **address,char *iface_name,char *ip_ver,IPV6_PRIV_EXT *addr_pref,char *ip_server,char *ip_server_url,
			      int port,char *ip_server_host,char *ip_proxy_name,int ip_proxy_port)
{
	int	len;

	*address=safe_malloc(sizeof(IP_ADDRESS));

	memcpy(&(*address)->addr_pref,addr_pref,sizeof(IPV6_PRIV_EXT));

	if ((len=strlen(ip_server))) {

		(*address)->ip_server=safe_malloc(len+1);
		strcpy((*address)->ip_server,ip_server);
	}

	if ((len=strlen(ip_server_url))) {

		(*address)->ip_server_url=safe_malloc(len+1);
		strcpy((*address)->ip_server_url,ip_server_url);
	}

	(*address)->port=port;

	if ((len=strlen(ip_server_host))) {

		(*address)->ip_server_host=safe_malloc(len+1);
		strcpy((*address)->ip_server_host,ip_server_host);
	}

	set_update_requirements(*address,ip_ver);

	if ((len=strlen(iface_name))) {

		(*address)->iface_name=safe_malloc(len+1);
		strcpy((*address)->iface_name,iface_name);
	}

	if ((len=strlen(ip_proxy_name))) {

		(*address)->ip_proxy_name=safe_malloc(len+1);
		strcpy((*address)->ip_proxy_name,ip_proxy_name);
	}

	(*address)->ip_proxy_port=ip_proxy_port;

	(*address)->my_ip_address.name[ip_4]=safe_malloc(DYNDNS_MY_IP_ADDRESS_LENGTH);
	(*address)->my_ip_address.name[ip_6]=safe_malloc(DYNDNS_MY_IP_ADDRESS_LENGTH);
	(*address)->my_ip_address.name[ip_store]=safe_malloc(DYNDNS_MY_IP_ADDRESS_LENGTH);

	http_client_construct(&(*address)->http_to_ip_server);

	return *address;
}

/*return the entry that meets, or at least does not contradict (because if the entry does not meet the
  critera, but doesn't contradict it, the criteria will be added to the entry) the parameters specified
  criteria.
*/
static IP_ADDRESS* do_ip_address_list_get(gen_list address_list,char *iface_name,char *ip_ver,IPV6_PRIV_EXT *addr_pref,char *ip_server,
					  char *ip_server_url,int port,char *ip_proxy_name,int ip_proxy_port)
{
	IP_ADDRESS	*ip_address=NULL;
	IP_ADDRESS	*p_ip_address;
	char		*cur_iface_name=NULL;
	gen_list_itr	itr=list_get_itr(address_list);
	int		is_net_fetch_match;
	int		is_proxy_match;
	int		is_priv_ext_match;
	int		is_ip_ver_auto;

	list_begin(address_list,itr);

	while (!(list_end(itr))) {

		p_ip_address=list_entry_item(itr);

		free(cur_iface_name);

		/*upcoming string compare can't be null*/
		if (!(p_ip_address->iface_name))

			cur_iface_name=safe_malloc(1);
		else {
			cur_iface_name=safe_malloc(strlen(p_ip_address->iface_name)+1);

			strcpy(cur_iface_name,p_ip_address->iface_name);
		}

		is_ip_ver_auto=(0==strcmp("auto",ip_ver) || !(strlen(ip_ver)));

		is_priv_ext_match=(addr_pref->flags==p_ip_address->addr_pref.flags);

		if (!(p_ip_address->ip_proxy_name))

			is_proxy_match=!(strlen(ip_proxy_name));
		else
			is_proxy_match=(0==strcmp(ip_proxy_name,p_ip_address->ip_proxy_name));

		is_net_fetch_match=((0==strcmp(ip_server,p_ip_address->ip_server)) && (0==strcmp(ip_server_url,p_ip_address->ip_server_url)) &&
			(port==p_ip_address->port) && is_proxy_match);

		if (is_net_fetch_match && ((0==strcmp(ip_ver,"ip4")))) {

			ip_address=p_ip_address;

			break;
		}
					   /*only named interface(s) used for ip6 interface address fetch*/
		if ((is_net_fetch_match || ((0==strcmp(iface_name,cur_iface_name) && strlen(iface_name)))) &&
				((0==strcmp(ip_ver,"ip6")) || is_ip_ver_auto || (is_net_fetch_match && (0==strcmp(ip_ver,"coupled"))))) {

			if (is_priv_ext_match) {

				ip_address=p_ip_address;

				break;
			}
		}

		list_next(itr);
	}

	free(cur_iface_name);

	return ip_address;
}

IP_ADDRESS* ip_address_list_get(gen_list address_list,char *iface_name,char *ip_ver,IPV6_PRIV_EXT *addr_pref,char *ip_server,
				char *ip_server_url,int port,char *ip_proxy_name,int ip_proxy_port)
{
	IP_ADDRESS	*ip_address=NULL;
	char		*ip_server_host_dummy;

	mem_safe_alloc(&iface_name,&ip_ver,&ip_server,&ip_server_url,&ip_server_host_dummy,&ip_proxy_name);

	ip_address=do_ip_address_list_get(address_list,iface_name,ip_ver,addr_pref,ip_server,ip_server_url,port,ip_proxy_name,ip_proxy_port);

	mem_safe_free(iface_name,ip_ver,ip_server,ip_server_url,ip_server_host_dummy,ip_proxy_name);

	return ip_address;
}

static IP_ADDRESS* do_do_ip_address_list_add(gen_list address_list,char *iface_name,char *ip_ver,IPV6_PRIV_EXT *addr_pref,char *ip_server,char *ip_server_url,
					     int port,char *ip_server_host,char *ip_proxy_name,int ip_proxy_port)
{
	IP_ADDRESS	*address;

	/*if address not already in list, create it*/
	if ((address=do_ip_address_list_get(address_list,iface_name,ip_ver,addr_pref,ip_server,ip_server_url,port,ip_proxy_name,ip_proxy_port))) {

		set_update_requirements(address,ip_ver);
	}
	else { /*update address fields to reflect additional non-contradictory attributes*/

		address=ip_address_create(&address,iface_name,ip_ver,addr_pref,ip_server,ip_server_url,port,ip_server_host,ip_proxy_name,ip_proxy_port);

		list_add(address_list,address);
	}

	return address;
}

static IP_ADDRESS* do_ip_address_list_add(gen_list address_list,char *iface_name,char *ip_ver,IPV6_PRIV_EXT *addr_pref,char *ip_server,char *ip_server_url,
					  int port,char *ip_server_host,char *ip_proxy_name,int ip_proxy_port)
{
	IP_ADDRESS	*address;


	mem_safe_alloc(&iface_name,&ip_ver,&ip_server,&ip_server_url,&ip_server_host,&ip_proxy_name);

	address=do_do_ip_address_list_add(address_list,iface_name,ip_ver,addr_pref,ip_server,ip_server_url,port,ip_server_host,ip_proxy_name,ip_proxy_port);

	mem_safe_free(iface_name,ip_ver,ip_server,ip_server_url,ip_server_host,ip_proxy_name);

	return address;
}

IP_ADDRESS* ip_address_list_add(gen_list *address_list,char *iface_name,char *ip_ver,IPV6_PRIV_EXT *addr_pref,char *ip_server,char *ip_server_url,
				int port,char *ip_server_host,char *ip_proxy_name,int ip_proxy_port)
{

	if (!(*address_list))

		list_create(address_list);

	return do_ip_address_list_add(*address_list,iface_name,ip_ver,addr_pref,ip_server,ip_server_url,port,ip_server_host,ip_proxy_name,ip_proxy_port);
}

void iface_enum(gen_list address_list)
{
	gen_list_itr    addr_list_itr;
	gen_list_itr    iface_list_itr;
	I_FACE          *iface;
	IP_ADDRESS      *address;

	iface_list_itr=list_get_itr(address_list);
	list_begin(address_list,iface_list_itr);

	DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "Enumerating interfaces, and corresponding addresses\n"));

	while(!(list_end(iface_list_itr))) {

		iface=list_entry_item(iface_list_itr);
		addr_list_itr=list_get_itr(iface->address);
		list_begin(iface->address,addr_list_itr);

		DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "Interface name:  %s\n",iface->name));

		while (!(list_end(addr_list_itr))) {

			address=list_entry_item(addr_list_itr);

			DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "Address struct:  ip server:  %s, iface name:  %s, addr_pref:  %s, is_update_ip4:  %s "\
					"is_update_ip6:  %s, is_update_auto:  %s\n",(NULL==address->ip_server ? "NULL":address->ip_server),
					(NULL==address->iface_name ? "NULL":address->iface_name),(0==strlen(address->addr_pref.addr_pref) ? "NULL":
					address->addr_pref.addr_pref),((address->is_update_ip4) ? "true":"false"),((address->is_update_ip6) ? "true":
					"false"),((address->is_update_auto) ? "true":"false")));

			list_next(addr_list_itr);
		}

		list_next(iface_list_itr);
	}
}

static I_FACE* do_iface_list_get(gen_list iface_list,char *iface_name)
{
	I_FACE		*p_iface;
	I_FACE		*iface=NULL;
	gen_list_itr	itr=list_get_itr(iface_list);

	list_begin(iface_list,itr);

	while (!(list_end(itr))) {

		p_iface=list_entry_item(itr);

		if (0==strcmp(iface_name,p_iface->name)) {

			iface=p_iface;

			break;
		}

		list_next(itr);
	}

	return iface;
}

I_FACE* iface_list_get(gen_list iface_list,char *iface_name)
{
	I_FACE		*iface;

	mem_safe_var_alloc((void *) &iface_name,-1);

	iface=do_iface_list_get(iface_list,iface_name);

	mem_safe_var_free((void *) iface_name);

	return iface;
}

static IP_ADDRESS* do_do_iface_list_add_address(gen_list iface_list,char *iface_name,char *ip_ver,IPV6_PRIV_EXT *addr_pref,char *ip_server,char *ip_server_url,
						int port,char *ip_server_host,char *ip_proxy_name,int ip_proxy_port)
{
	I_FACE	*p_iface;

	/*iface_list should have been initialized to the "default" (no name) interface*/

	p_iface=do_iface_list_get(iface_list,iface_name);

	return ip_address_list_add(&p_iface->address,iface_name,ip_ver,addr_pref,ip_server,ip_server_url,port,ip_server_host,ip_proxy_name,ip_proxy_port);
}

static IP_ADDRESS* do_iface_list_add_address(gen_list iface_list,char *iface_name,char *ip_ver,IPV6_PRIV_EXT *addr_pref,char *ip_server,char *ip_server_url,
					     int port,char *ip_server_host,char *ip_proxy_name,int ip_proxy_port)
{
	IP_ADDRESS	*p_address;

	mem_safe_var_alloc((void *) &iface_name,-1);

	p_address=do_do_iface_list_add_address(iface_list,iface_name,ip_ver,addr_pref,ip_server,ip_server_url,port,ip_server_host,ip_proxy_name,ip_proxy_port);

	mem_safe_var_free((void *) iface_name);

	return p_address;
}

IP_ADDRESS* iface_list_add_address(gen_list *iface_list,char *iface_name,char *ip_ver,IPV6_PRIV_EXT *addr_pref,char *ip_server,char *ip_server_url,
				   int port,char *ip_server_host,char *ip_proxy_name,int ip_proxy_port)
{
	I_FACE	*p_iface;

	p_iface=iface_list_add_iface(iface_list,iface_name);

	return do_ip_address_list_add(p_iface->address,iface_name,ip_ver,addr_pref,ip_server,ip_server_url,port,ip_server_host,ip_proxy_name,ip_proxy_port);
}

static I_FACE* do_do_iface_list_add_iface(gen_list iface_list,char *iface_name)
{
	I_FACE	*i_face;

	/*check if already have it*/
	if (!(i_face=do_iface_list_get(iface_list,iface_name))) {

		iface_create(&i_face,iface_name);

		list_add(iface_list,i_face);
	}

	return i_face;
}

static I_FACE* do_iface_list_add_iface(gen_list iface_list,char *iface_name)
{
	I_FACE	*i_face;

	mem_safe_var_alloc((void *) &iface_name,-1);

	i_face=do_do_iface_list_add_iface(iface_list,iface_name);

	mem_safe_var_free((void *) iface_name);

	return i_face;
}

I_FACE* iface_list_add_iface(gen_list *iface_list,char *iface_name)
{
	if (!(*iface_list))

		list_create(iface_list);

	return do_iface_list_add_iface(*iface_list,iface_name);
}

RC_TYPE ip_address_set_addr_pref(IP_ADDRESS *p_address,SOCKET socket)
{

	return do_ip_set_addr_pref(socket,p_address->iface_name,p_address->addr_pref);
}

void ip_address_inc_dependents(IP_ADDRESS *p_address)
{

	p_address->dependents++;
}

void ip_address_inc_fatal(IP_ADDRESS *p_address)
{

	p_address->dependents_fatal++;
}
