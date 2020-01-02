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

#ifndef _ADDR_LIST_H_INCLUDED
#define _ADDR_LIST_H_INCLUDED

#include "http_client.h"
#include "list.h"

/*can't decide what to call dual stack
  updates in list fashion (single update
  request, one or more ip's) so leave
  it variable -- may change*/
#define DUAL_LIST		"coupled"
#define MAX_IP_TYPE_SIZE	8

/*address str may contain both ip4,ip6 separated by semi colon*/
#define DYNDNS_MY_IP_ADDRESS_LENGTH	56	/*chars*/

typedef struct
{
        char    *name;
        char    *url;
        int     port;
} DYNDNS_SERVER_NAME;

/*alias name property used as index for alias
  ip address names array (DYNDNS_SERVER_NAME
  name field) and DYNDNS_INFO_TYPE field,
  my_ip_has_changed boolean array*/
typedef enum
{
	ip_4,
	ip_6,
	ip_store,
	ip_null
} DYNDNS_IPV;

typedef struct
{
        char    *name[3];
        int     port;
} DYNDNS_MY_IP;

typedef struct {

	/*address fetch logic is a little complex so
	  for convenience, set is_update vars to indicate
	  what's needed.  is_got vars used by fetch
	  routine*/
	BOOL 		is_update_ip4;
	BOOL 		is_update_ip6;
	BOOL 		is_update_auto;
	BOOL 		is_got_ip4;
	BOOL 		is_got_ip6;
	BOOL		is_updated;

	/*key fields*/
	IPV6_PRIV_EXT	addr_pref;
	char		*ip_server;
	char		*ip_server_url;
	int		port;
	char		*ip_server_host;
	char		*ip_proxy_name;
	int		ip_proxy_port;
	char		*iface_name;

	HTTP_CLIENT	http_to_ip_server;

	/*char* [3] for ip4, ip6, and
	  current operated upon*/
	DYNDNS_MY_IP		my_ip_address;
	struct sockaddr_storage	net_addr[3];

	int	dependents;
	int	dependents_fatal;
} IP_ADDRESS;

typedef struct {

	char		*name;

	/*list of pointer to IP_ADDRESS*/
	gen_list	address;
} I_FACE;

const char *ip_enum_to_str(DYNDNS_IPV ip_enum);
RC_TYPE ip_address_set_addr_pref(IP_ADDRESS *p_address,SOCKET socket);
I_FACE* iface_create(I_FACE **i_face,char *name);
IP_ADDRESS* ip_address_create(IP_ADDRESS **address,char *iface_name,char *ip_ver,IPV6_PRIV_EXT *addr_pref,char *ip_server,char *ip_server_url,
			      int port,char *ip_server_host,char *ip_proxy_name,int ip_proxy_port);
IP_ADDRESS* ip_address_list_get(gen_list address_list,char *iface_name,char *ip_ver,IPV6_PRIV_EXT *addr_pref,char *ip_server,char *ip_server_url,
				int port,char *ip_proxy_name,int ip_proxy_port);
IP_ADDRESS* ip_address_list_add(gen_list *address_list,char *iface_name,char *ip_ver,IPV6_PRIV_EXT *addr_pref,char *ip_server,char *ip_server_url,
				int port,char *ip_server_host,char *ip_proxy_name,int ip_proxy_port);
I_FACE* iface_list_get(gen_list iface_list,char *iface_name);
IP_ADDRESS* iface_list_add_address(gen_list *iface_list,char *iface_name,char *ip_ver,IPV6_PRIV_EXT *addr_pref,char *ip_server,char *ip_server_url,
				   int port,char *ip_server_host,char *ip_proxy_name,int ip_proxy_port);
I_FACE* iface_list_add_iface(gen_list *iface_list,char *iface_name);
void iface_enum(gen_list address_list);
void ip_address_inc_dependents(IP_ADDRESS *p_address);
void ip_address_inc_fatal(IP_ADDRESS *p_address);

#endif
