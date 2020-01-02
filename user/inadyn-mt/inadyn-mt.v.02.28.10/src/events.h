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
        inadyn-mt events agrregation.

        Author: bhoover@wecs.com
        Date:   March 2015

        History:
                - first implemetnation
*/

#include "dyndns.h"

typedef void*	events;

typedef void (*EVENT_HANDLER)(int i_event,void *p_context);

void events_register_client(EVENT_HANDLER event_handler,void *p_context,events ev);
void events_start_loop_thread();
void events_stop_loop_thread();

void unix_signal_handler(int signo);
#ifdef _WIN32
int dyndns_win32_signal_handler_func(OS_SIGNAL_TYPE signal, void *p_in);
int service_event_handler(SERVICE_EVENT service_event);
void dyn_dns_update_ip_handler(RAS_THREAD_DATA *p_ras_thread_data);
#endif
