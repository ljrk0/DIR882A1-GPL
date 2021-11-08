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

#ifndef EXCLUDE_CONFIG_H

#include "config.h"

#endif

#include	<stdlib.h>
#include	<errno.h>
#include	"errorcode.h"
#include	"debug_if.h"
#include	"safe_mem.h"

typedef struct
{
	RC_TYPE		rc;
	const char	*p_name;
} ERROR_NAME;

static const ERROR_NAME global_error_table[]  =
{
	{RC_OK,"RC_OK"},
	{RC_ERROR,"RC_ERROR"},
	{RC_INVALID_POINTER,"RC_INVALID_POINTER"},
	{RC_OUT_OF_MEMORY,"RC_OUT_OF_MEMORY"},
	{RC_OUT_BUFFER_OVERFLOW,"RC_OUT_BUFFER_OVERFLOW"},


	{RC_IP_SOCKET_CREATE_ERROR,"RC_IP_SOCKET_CREATE_ERROR"},
	{RC_IP_BAD_PARAMETER,"RC_IP_BAD_PARAMETER"},
	{RC_IP_INVALID_REMOTE_ADDR,"RC_IP_INVALID_REMOTE_ADDR"},
	{RC_IP_CONNECT_FAILED,"RC_IP_CONNECT_FAILED"},
	{RC_IP_SEND_ERROR,"RC_IP_SEND_ERROR"},
	{RC_IP_RECV_ERROR,"RC_IP_RECV_ERROR"},
	{RC_IP_OBJECT_NOT_INITIALIZED,"RC_IP_OBJECT_NOT_INITIALIZED"},
	{RC_IP_OS_SOCKET_INIT_FAILED,"RC_IP_OS_SOCKET_INIT_FAILED"},
	{RC_IP_PARSER_INVALID_IP,"RC_IP_PARSER_INVALID_IP"},
	{RC_IP_INVALID_SOCKET,"RC_IP_INVALID_SOCKET"},

	{RC_TCP_OBJECT_NOT_INITIALIZED,"RC_TCP_OBJECT_NOT_INITIALIZED"},

	{RC_HTTP_OBJECT_NOT_INITIALIZED,"RC_HTTP_OBJECT_NOT_INITIALIZED"},
	{RC_HTTP_BAD_PARAMETER,"RC_HTTP_BAD_PARAMETER"},

	{RC_DYNDNS_INVALID_IP_ADDR,"RC_DYNDNS_INVALID_IP_ADDR"},
	{RC_DYNDNS_BUFFER_TOO_SMALL,"RC_DYNDNS_BUFFER_TOO_SMALL"},
	{RC_DYNDNS_INVALID_IP_ADDR_IN_HTTP_RESPONSE,"RC_DYNDNS_INVALID_IP_ADDR_IN_HTTP_RESPONSE"},
	{RC_DYNDNS_INVALID_RSP_FROM_IP_SERVER,"RC_DYNDNS_INVALID_RSP_FROM_IP_SERVER"},
	{RC_DYNDNS_TOO_MANY_ALIASES,"RC_DYNDNS_TOO_MANY_ALIASES"},
	{RC_DYNDNS_INVALID_OPTION,"RC_DYNDNS_INVALID_OPTION"},
	{RC_DYNDNS_INVALID_OR_MISSING_PARAMETERS,"RC_DYNDNS_INVALID_OR_MISSING_PARAMETERS"},
	{RC_DYNDNS_INVALID_DNS_SYSTEM_DEFAULT,"RC_DYNDNS_INVALID_DNS_SYSTEM_DEFAULT"},
	{RC_DYNDNS_UNRESOLVED_ALIAS,"RC_DYNDNS_UNRESOLVED_ALIAS"},
	{RC_DYNDNS_RSP_NOTOK,"RC_DYNDNS_RSP_NOTOK"},
	{RC_DYNDNS_RSP_CONFIG,"RC_DYNDNS_RSP_CONFIG"},

	{RC_CMD_PARSER_INVALID_OPTION,"RC_CMD_PARSER_INVALID_OPTION"},
	{RC_CMD_PARSER_INVALID_OPTION_ARGUMENT,"RC_CMD_PARSER_INVALID_OPTION_ARGUMENT"},
	{RC_CMD_PARSER_INVALID_FILE_FORMAT,"RC_CMD_PARSER_INVALID_FILE_FORMAT"},

	{RC_OS_ERROR_INSTALLING_SIGNAL_HANDLER,"RC_OS_ERROR_INSTALLING_SIGNAL_HANDLER"},
	{RC_OS_INVALID_IP_ADDRESS,"RC_OS_INVALID_IP_ADDRESS"},
	{RC_OS_FORK_FAILURE,"RC_FORK_FAILURE"},
	{RC_OS_CHANGE_PERSONA_FAILURE,"RC_OS_CHANGE_PERSONA_FAILURE"},

	{RC_FILE_IO_OPEN_ERROR,"RC_FILE_IO_OPEN_ERROR"},
	{RC_FILE_IO_READ_ERROR,"RC_FILE_IO_READ_ERROR"},
	{RC_FILE_IO_OUT_OF_BUFFER,"RC_FILE_IO_OUT_OF_BUFFER"},

	{RC_OK,NULL}
};

static const char* unknown_error = "Unknown error";

const char* errorcode_get_name(RC_TYPE rc)
{
	const ERROR_NAME *it = global_error_table;
	while (it->p_name != NULL)
	{
		if (it->rc == rc)
		{
			return it->p_name;
		}
		++it;
	}
	return unknown_error;
}

/*3/1/2015
  legacy windows wrapper for non-reentrant strerror.

  TODO:  make it reentrant
*/
static int strerror_rr(int err,char* strerr_buf,int strerr_size)
{

#if !(defined(_WIN32) || defined(__SunOS_5_9))

	return strerror_r(err,strerr_buf,strerr_size);
#else
	char*	p_s_err;

	if (!(p_s_err=strerror(err)))

		return errno;
	else {

		strcpy(strerr_buf,p_s_err);

		return 0;
	}

#endif

}

char* str_errno(int err)
{

	char*	strerr_buf=NULL;
	int	strerr_size=0;

	do {

		strerr_size=strerr_size+256;

		strerr_buf=safe_realloc(strerr_buf,strerr_size);
	}
	while (ERANGE==strerror_rr(err,strerr_buf,strerr_size));

	return strerr_buf;
}

void log_errno(int err,int log_level,char* format_str)
{
	char*	strerr_buf=NULL;


	strerr_buf=str_errno(err);

	DBG_PRINTF((log_level,format_str,strerr_buf));

	free(strerr_buf);
}

