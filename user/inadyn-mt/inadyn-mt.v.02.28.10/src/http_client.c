/*
Copyright (C) 2003-2004 Narcis Ilisei
Modifications by Bryan Hoover (bhoover@wecs.com)
Copyright (C) 2009 Bryan Hoover (bhoover@wecs.com)

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
        Sept. 2009
	  - Non-blocking socket create, and connect
*/

#ifndef EXCLUDE_CONFIG_H

#include "config.h"

#endif

#include <string.h>
#include <stdarg.h>
#include "http_client.h"
#include "errorcode.h"
#include "safe_mem.h"
#include "debug_if.h"

#define super_construct(p)			tcp_construct(p)
#define super_destruct(p)			tcp_destruct(p)
#define super_initialize_and_connect(p)		tcp_initialize_and_connect(p)
#define super_initialize_and_connect_all(p)	tcp_initialize_and_connect_all(p)
#define super_initialize(p)			tcp_initialize(p)
#define super_initialize_all(p)			tcp_initialize_all(p)
#define super_init_async(p)			tcp_initialize_async(p,p_exit_func,p_cb_data)
#define super_connect(p)			tcp_connect(p)
#define super_connect_all(p)			tcp_connect_all(p)
#define super_connect_socket(p)			tcp_connect_socket(p)
#define super_test_connect(p)			tcp_test_connect(p,p_exit_func,p_cb_data)
#define super_shutdown(p)			tcp_shutdown(p)
#define super_set_port(p_self,p)		tcp_set_port(p_self,p)
#define super_get_port(p_self,p)		tcp_get_port(p_self,p)
#define super_set_remote_name(p_self,p)		tcp_set_remote_name(p_self,p)
#define super_get_remote_name(p_self,p)		tcp_get_remote_name(p_self,p)
#define super_set_remote_timeout(p_self,p)	tcp_set_remote_timeout(p_self,p)
#define super_get_remote_timeout(p_self,p)	tcp_get_remote_timeout(p_self,p)
#define super_get_is_ipv4(p_self,is_ipv4)	tcp_get_is_ipv4(p_self,is_ipv4)
#define super_set_is_ipv4(p_self,is_ipv4)	tcp_set_is_ipv4(p_self,is_ipv4)
#define super_get_is_ipv6(p_self,is_ipv6)       tcp_get_is_ipv6(p_self,is_ipv6)
#define super_set_is_ipv6(p_self,is_ipv6)       tcp_set_is_ipv6(p_self,is_ipv6)
#define super_set_priv_ext(p_self,addr_pref)	tcp_set_priv_ext(p_self,addr_pref)
#define super_get_priv_ext(p_self,addr_pref)	tcp_get_priv_ext(p_self,addr_pref) 
#define super_set_if_name(p_self,if_name)       tcp_set_if_name(p_self,if_name)
#define super_get_if_name(p_self,if_name)       tcp_get_if_name(p_self,if_name)

/*public functions*/

/*
	 basic resource allocations for the tcp object
*/
RC_TYPE http_client_construct(HTTP_CLIENT *p_self)
{
	RC_TYPE rc;

	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	if (p_self->is_constructed)
	{
		return RC_OK;
	}

	rc = super_construct(&p_self->super);

	if (rc != RC_OK)
	{
		return rc;
	}

	/*init*/
	memset( (char*)p_self + sizeof(p_self->super), 0 , sizeof(*p_self) - sizeof(p_self->super));
	p_self->initialized = FALSE;
	p_self->is_constructed=TRUE;

	return RC_OK;
}

/*
	Resource free.
*/
RC_TYPE http_client_destruct(HTTP_CLIENT *p_self)
{
	RC_TYPE	rc;


	if (p_self==NULL)
	{
		return RC_OK;
	}

	rc=super_destruct(&p_self->super);

	if (!(p_self->is_constructed))
	{
		return RC_OK;
	}

	http_client_shutdown(p_self);

	memset(p_self,0,sizeof(HTTP_CLIENT));


	/*free*/
	return rc;
}

RC_TYPE http_transaction_construct(HTTP_TRANSACTION *http_trans,int req_buf_size,char *fmt,int rsp_max_size,...)
{
	char	*req_buf;
	va_list args;


	req_buf=safe_malloc(req_buf_size);

	va_start(args,rsp_max_size);
	http_trans->req_len=vsnprintf(req_buf,req_buf_size,fmt,args);
	va_end(args);

	http_trans->p_req=req_buf;
	http_trans->p_rsp=safe_malloc(rsp_max_size);
	http_trans->max_rsp_len=rsp_max_size-1;
	http_trans->rsp_len=0;

	return RC_OK;
}

RC_TYPE http_transaction_destruct(HTTP_TRANSACTION *http_trans)
{

	free(http_trans->p_req);
	free(http_trans->p_rsp);

	memset(http_trans,0,sizeof(HTTP_TRANSACTION));

	return RC_OK;
}

static RC_TYPE local_set_params(HTTP_CLIENT *p_self)
{
	{
		int timeout;
		/*set default TCP specififc params*/
		http_client_get_remote_timeout(p_self, &timeout);

		if (timeout == 0)
		{
			http_client_set_remote_timeout(p_self, HTTP_DEFAULT_TIMEOUT);
		}
	}

	{
		int port;
		http_client_get_port(p_self, &port);
		if ( port == 0)
		{
			http_client_set_port(p_self, HTTP_DEFAULT_PORT);
		}
	}

	return RC_OK;
}

RC_TYPE http_client_do_connect(HTTP_CLIENT *p_self,int is_init_all)
{
	RC_TYPE rc;

	do
	{
		rc = local_set_params(p_self);

		if (rc != RC_OK)
		{
			break;
		}


		if (is_init_all) {

			rc = super_connect_all(&p_self->super);
		}
		else {

			rc = super_connect(&p_self->super);
		}

		if (rc != RC_OK)
		{
			break;
		}

		/*local init*/

	}
	while(0);

	if (rc != RC_OK)
	{
		http_client_shutdown(p_self);
	}
	else
	{
		p_self->initialized = TRUE;
	}

	return rc;
}

RC_TYPE http_client_do_init(HTTP_CLIENT *p_self,int is_init_all)
{
	RC_TYPE rc;

	do
	{
		rc = local_set_params(p_self);

		if (rc != RC_OK)
		{
			break;
		}


		if (is_init_all) {

			rc = super_initialize_all(&p_self->super);
		}
		else {

			rc = super_initialize(&p_self->super);
		}

		if (rc != RC_OK)
		{
			break;
		}

		/*local init*/

	}
	while(0);

	if (rc != RC_OK)
	{
		http_client_shutdown(p_self);
	}
	else
	{
		p_self->initialized = TRUE;
	}

	return rc;
}

RC_TYPE http_client_init_and_connect(HTTP_CLIENT *p_self)
{

	RC_TYPE	rc;

	if (RC_OK==(rc=http_client_do_init(p_self,0))) {

		rc=http_client_do_connect(p_self,0);
	}

	return rc;
}

RC_TYPE http_client_init_and_connect_all(HTTP_CLIENT *p_self)
{
	RC_TYPE	rc;

	if (RC_OK==(rc=http_client_do_init(p_self,1))) {

		rc=http_client_do_connect(p_self,1);
	}

	return rc;
}

RC_TYPE http_client_init(HTTP_CLIENT *p_self)
{

	return http_client_do_init(p_self,0);
}

RC_TYPE http_client_init_all(HTTP_CLIENT *p_self)
{

	return http_client_do_init(p_self,1);
}

RC_TYPE http_client_connect_all(HTTP_CLIENT *p_self)
{

	return http_client_do_connect(p_self,1);
}

RC_TYPE http_client_connect(HTTP_CLIENT *p_self)
{

	return http_client_do_connect(p_self,0);
}

RC_TYPE http_client_connect_socket(HTTP_CLIENT *p_self)
{

	return super_connect_socket(&p_self->super);
}

/*
	Disconnect and some other clean up.
*/
RC_TYPE http_client_shutdown(HTTP_CLIENT *p_self)
{
	RC_TYPE	rc;


	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	rc=super_shutdown(&p_self->super);

	p_self->initialized=FALSE;

	return rc;
}

/* Send req and get response */
RC_TYPE http_client_transaction(HTTP_CLIENT *p_self, HTTP_TRANSACTION *p_tr )
{
	RC_TYPE rc;
	if (p_self == NULL || p_tr == NULL)
	{
		return RC_INVALID_POINTER;
	}

	if (!p_self->initialized)
	{
		return RC_HTTP_OBJECT_NOT_INITIALIZED;
	}

	do
	{
		rc = tcp_send(&p_self->super, p_tr->p_req, p_tr->req_len);
		if (rc != RC_OK)
		{
			break;
		}

		rc = tcp_recv(&p_self->super, p_tr->p_rsp, p_tr->max_rsp_len, &p_tr->rsp_len);
	}
	while(0);

	return rc;
}

/* Accessors */

RC_TYPE http_client_set_if_name(HTTP_CLIENT *p_self,char *if_name)
{

	return super_set_if_name(&p_self->super,if_name);
}

char* http_client_get_if_name(HTTP_CLIENT *p_self,char **if_name)
{

	return super_get_if_name(&p_self->super,if_name);
}

RC_TYPE http_client_set_priv_ext(HTTP_CLIENT *p_self,IPV6_PRIV_EXT addr_pref)
{

	return super_set_priv_ext(&p_self->super,addr_pref);
}

IPV6_PRIV_EXT* http_client_get_addr_pref(HTTP_CLIENT *p_self,IPV6_PRIV_EXT *addr_pref)
{

	return super_get_priv_ext(&p_self->super,addr_pref);
}

RC_TYPE http_client_set_port(HTTP_CLIENT *p_self, int p)
{
	return super_set_port(&p_self->super, p);
}

RC_TYPE http_client_set_remote_name(HTTP_CLIENT *p_self, const char* p)
{
	return super_set_remote_name(&p_self->super, p);
}

RC_TYPE http_client_set_remote_timeout(HTTP_CLIENT *p_self, int p)
{
	return super_set_remote_timeout(&p_self->super, p);
}

RC_TYPE http_client_set_is_ipv4(HTTP_CLIENT *p_self, BOOL is_ipv4)
{
	return super_set_is_ipv4(&p_self->super,is_ipv4);
}

RC_TYPE http_client_set_is_ipv6(HTTP_CLIENT *p_self, BOOL is_ipv6)
{
        return super_set_is_ipv6(&p_self->super,is_ipv6);
}

RC_TYPE http_client_set_is_ip(HTTP_CLIENT *p_self, BOOL is_ipv4, BOOL is_ipv6)
{
	RC_TYPE	rc;


	if (!(RC_OK==(rc=http_client_set_is_ipv4(p_self,is_ipv4))))

		return rc;

	return http_client_set_is_ipv6(p_self,is_ipv6);
}

RC_TYPE http_client_get_port(HTTP_CLIENT *p_self, int *p)
{
	return super_get_port(&p_self->super, p);
}
RC_TYPE http_client_get_remote_name(HTTP_CLIENT *p_self, const char* *p)
{
	return super_get_remote_name(&p_self->super, p);
}
RC_TYPE http_client_get_remote_timeout(HTTP_CLIENT *p_self, int *p)
{
	return super_get_remote_timeout(&p_self->super, p);
}

RC_TYPE http_client_get_is_ipv4(HTTP_CLIENT *p_self, BOOL *is_ipv4)
{
	return super_get_is_ipv4(&p_self->super,is_ipv4);
}

RC_TYPE http_client_get_is_ipv6(HTTP_CLIENT *p_self, BOOL *is_ipv6)
{
        return super_get_is_ipv6(&p_self->super,is_ipv6);
}

#ifdef USE_THREADS

RC_TYPE http_client_init_async(HTTP_CLIENT *p_self,CB_EXIT_COND p_exit_func,void *p_cb_data)
{
	RC_TYPE rc;
	do
	{
		/*set local params*/
		rc = local_set_params(p_self);
		if (rc != RC_OK)
		{
			break;
		}


		/*call super*/
		rc = super_init_async(&p_self->super);
		if (rc != RC_OK)
		{
			break;
		}

		/*local init*/

	}
	while(0);

	if (rc != RC_OK)
	{
		http_client_shutdown(p_self);
	}
	else
	{
		p_self->initialized = TRUE;
	}

	return rc;
}

RC_TYPE http_client_test_connect(HTTP_CLIENT *p_self,CB_EXIT_COND p_exit_func,void *p_cb_data)
{
	RC_TYPE	rc;


	if (!((rc=local_set_params(p_self))==RC_OK))

		return rc;

	return super_test_connect(&p_self->super);
}

#endif /*USE_THREADS*/
