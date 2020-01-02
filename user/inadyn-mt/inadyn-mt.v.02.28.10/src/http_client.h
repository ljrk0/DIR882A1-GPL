/*
Copyright (C) 2003-2004 Narcis Ilisei

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

/*interface for http_client */
/* interface for tcp functions */

#ifndef _HTTP_CLIENT_H_INCLUDED
#define _HTTP_CLIENT_H_INCLUDED

#include "os.h"
#include "errorcode.h"
#include "tcp.h"


/* SOME DEFAULT CONFIGURATIONS */
#define HTTP_DEFAULT_TIMEOUT	10000 /*ms*/
#define	HTTP_DEFAULT_PORT       80


typedef struct
{
	TCP_SOCKET super;
	BOOL is_constructed;
	BOOL initialized;
} HTTP_CLIENT;

typedef struct
{
	char	*p_req;/*get信息暂时保存在这里面*/
	int	req_len;

	char	*p_rsp;/*待存储返回的信息*/
	int	max_rsp_len;
	int	rsp_len;

	int	status;
} HTTP_TRANSACTION;


/*public functions*/

/*
	 basic resource allocations for the tcp object
*/
RC_TYPE http_client_construct(HTTP_CLIENT *p_self);

/*
	Resource free.
*/
RC_TYPE http_client_destruct(HTTP_CLIENT *p_self);

/*
	Sets up the object.

	- ...
*/

RC_TYPE http_transaction_construct(HTTP_TRANSACTION *http_trans,int req_buf_size,char *fmt,int rsp_max_size,...);
RC_TYPE http_transaction_destruct(HTTP_TRANSACTION *http_trans);

RC_TYPE http_client_init(HTTP_CLIENT *p_self);
RC_TYPE http_client_init_all(HTTP_CLIENT *p_self);
/*return connections to all given by getaddrinfo*/
RC_TYPE http_client_init_and_connect(HTTP_CLIENT *p_self);
RC_TYPE http_client_init_and_connect_all(HTTP_CLIENT *p_self);
RC_TYPE http_client_connect(HTTP_CLIENT *p_self);
RC_TYPE http_client_connect_all(HTTP_CLIENT *p_self);
RC_TYPE http_client_connect_socket(HTTP_CLIENT *p_self);

#ifdef USE_THREADS

/*do client init with client exit callbacks*/
RC_TYPE http_client_init_async(HTTP_CLIENT *p_self,CB_EXIT_COND p_exit_func,void *p_cb_data);
RC_TYPE http_client_test_connect(HTTP_CLIENT *p_self,CB_EXIT_COND p_exit_func,void *p_cb_data);

#endif

/*
	Disconnect and some other clean up.
*/
RC_TYPE http_client_shutdown(HTTP_CLIENT *p_self);

/* Send req and get response */
RC_TYPE http_client_transaction(HTTP_CLIENT *p_self, HTTP_TRANSACTION *p_tr );

/* Accessors */
RC_TYPE http_client_set_priv_ext(HTTP_CLIENT *p_self,IPV6_PRIV_EXT addr_pref);
RC_TYPE http_client_set_if_name(HTTP_CLIENT *p_self,char *if_name);
IPV6_PRIV_EXT* http_client_get_priv_ext(HTTP_CLIENT *p_self,IPV6_PRIV_EXT *addr_pref);
char* http_client_get_if_name(HTTP_CLIENT *p_self,char **if_name);

RC_TYPE http_client_set_port(HTTP_CLIENT *p_self, int p);
RC_TYPE http_client_set_remote_name(HTTP_CLIENT *p_self, const char* p);
RC_TYPE http_client_set_remote_addr(HTTP_CLIENT *p_self, const char* p);
RC_TYPE http_client_set_remote_timeout(HTTP_CLIENT *p_self, int t);
RC_TYPE http_client_set_is_ipv4(HTTP_CLIENT *p_self, BOOL is_ipv4);
RC_TYPE http_client_set_is_ipv6(HTTP_CLIENT *p_self, BOOL is_ipv6);
RC_TYPE http_client_set_is_ip(HTTP_CLIENT *p_self, BOOL is_ipv4, BOOL is_ipv6);

RC_TYPE http_client_get_port(HTTP_CLIENT *p_self, int *p_port);
RC_TYPE http_client_get_remote_name(HTTP_CLIENT *p_self, const char* *p);
RC_TYPE http_client_get_remote_timeout(HTTP_CLIENT *p_self, int *p);
RC_TYPE http_client_get_is_ipv4(HTTP_CLIENT *p_self, BOOL *is_ipv4);
RC_TYPE http_client_get_is_ipv6(HTTP_CLIENT *p_self, BOOL *is_ipv6);

#endif /*_HTTP_CLIENT_H_INCLUDED*/

