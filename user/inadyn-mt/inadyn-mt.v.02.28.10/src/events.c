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

        Author:	bhoover@wecs.com
        Date:	March 2015

        History:
                - first implemetnation
*/

#include <stdlib.h>
#ifndef _WIN32
#include <pthread.h>
#endif
#include "events.h"
#include "queue.h"
#include "threads_wrapper.h"
#include "dyndns.h"
#include "safe_mem.h"
#include "debug_if.h"
#include "os.h"

#define MODULE_TAG "EVENTS: "

int	is_thread_exit=0;

typedef struct events_reg {

	void		*p_client_context;
	EVENT_HANDLER	p_client_event_handler;
}events_reg;

static gen_list	registrant=NULL;

#ifndef _WIN32
static pthread_t	events_thread_handle=0;
#else
static unsigned long	events_thread_handle=0;
#endif

static semaphore_t	event_q_sem;
static mt_queue_t	event_q;

static mutex_t		thread_handle_mutex=MUTEX_T_INIT;

void events_register_client(EVENT_HANDLER event_handler,void *p_context,events ev)
{

	events_reg	*p_reg;


	if (!(registrant))

		list_create(&registrant);

	p_reg=safe_malloc(sizeof(events_reg));
	p_reg->p_client_context=p_context;
	p_reg->p_client_event_handler=event_handler;

	list_add(registrant,p_reg);
}

#ifndef _WIN32
static void *events_loop_thread(void *p_data)
#else
static unsigned __stdcall events_loop_thread(void *p_data)
#endif
{
	int		*p_event;
	events_reg	*p_reg;
	gen_list_itr	itr=NULL;


	while (!(is_thread_exit)) {

		wait_sem(&event_q_sem);

		if (!(registrant))

			continue;

		itr=list_get_itr(registrant);

		/*test before dequeue, as could be signaled,
		  while as yet servicing/emptying queue, re:
		  create_semaphore sets 1, Windows sem count
		  max.
		*/
		while (queue_size(event_q))
		{

			p_event=queue_dequeue(event_q);

			list_begin(registrant,itr);

			/*search event handler table (keyed on p_event), and call it.
			  implies our client registers a given handler for one or more
			  events (either by list of events, or separate call for each,
			  or indicates ALL events, or a data structure consisting of
			  same, and corresponding event handler).

			  either the above, or client can only register for ALL events,
			  and that's the only handler we call.  for present purposes
			  that's really all that's needed.  what's provided here is
			  some centralization of source event handlers, event handling
			  bottleneck avoidance, readability, maintenance, as the event
			  from the source is handled by our handler (defined here, and
			  pointed to by the source registrant, e.g., os_unix.c, os_windows.c
			  signal handlers; Win callback registrations, etc.), by simply
			  adding the event to our queue.  The main program need maintain
			  but a single event handler - we aggregate all events, program
			  wide, here.
			*/

			while (!(list_end(itr))) {

				p_reg=list_entry_item(itr);

				p_reg->p_client_event_handler(*p_event,p_reg->p_client_context);

				list_next(itr);
			}

			free(p_event);
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

void events_start_loop_thread()
{

/*  parameter p_dyndns redundant here, but I feel so good about it.
*/

	create_mutex(&thread_handle_mutex);
	create_semaphore(&event_q_sem);
	queue_create(&event_q);

	events_thread_handle=begin_thread(events_loop_thread,NULL);
}

void events_stop_loop_thread()
{
	gen_list_itr	itr;


	get_mutex(&thread_handle_mutex);

	if (!(events_thread_handle)) {

		release_mutex(&thread_handle_mutex);

		destroy_mutex(&thread_handle_mutex);

		return;
	}

	is_thread_exit=1;

	signal_sem(&event_q_sem);

	if (events_thread_handle) {

#ifndef _WIN32
		pthread_join(events_thread_handle,NULL);
#else
		WaitForSingleObject((HANDLE) events_thread_handle,INFINITE);
		CloseHandle((HANDLE) events_thread_handle);
#endif
	}

	events_thread_handle=0;

	destroy_semaphore(&event_q_sem);

	queue_destroy(&event_q);

	if (registrant) {

		itr=list_get_itr(registrant);
		list_begin(registrant,itr);

		while (!(list_end(itr)))

			free(list_entry_remove(registrant,itr));

		list_destroy(&registrant);
	}

	release_mutex(&thread_handle_mutex);

	destroy_mutex(&thread_handle_mutex);
}

static void enqueue_event(int int_event)
{
	int	*i_event;

	i_event=safe_malloc(sizeof(int));
	*i_event=int_event;

	queue_enqueue(event_q,i_event);
}

static void signal_event(int int_event)
{

	/*if results in mutex create, event
	  thread, and program exiting*/
	get_mutex(&thread_handle_mutex);

	if (!(events_thread_handle)) {

		release_mutex(&thread_handle_mutex);

		destroy_mutex(&thread_handle_mutex);
	}
	else {

		enqueue_event(int_event);

		signal_sem(&event_q_sem);

		release_mutex(&thread_handle_mutex);
	}
}

#ifndef _WIN32

void unix_signal_handler(int signo)
{

        switch(signo)
        {
        case SIGHUP:
        case SIGINT:
        case SIGQUIT:
        case SIGALRM:
                DBG_PRINTF((LOG_DEBUG,"D:" MODULE_TAG "Signal '0x%x' received. Sending 'Shutdown cmd'.\n", signo));

		signal_event(CMD_STOP);
                break;

        case SIGUSR1:
                DBG_PRINTF((LOG_DEBUG,"D:" MODULE_TAG "Signal '0x%x' received. Sending update cmd.\n", signo));

		signal_event(CMD_UPDT);
                break;

        case SIGPIPE:
                DBG_PRINTF((LOG_DEBUG,"D:" MODULE_TAG "Signal '0x%x' (broken pipe) received...\n", signo));

                break;
        default:
                DBG_PRINTF((LOG_DEBUG,"D:" MODULE_TAG "Signal '0x%x' received. Ignored.\n", signo));
                break;
        }
        return;
}
#endif

#ifdef _WIN32

int dyndns_win32_signal_handler_func(OS_SIGNAL_TYPE signal, void *p_in)
{
	int ret_flag = 0;

	DYN_DNS_CLIENT *p_self = (DYN_DNS_CLIENT *) p_in;
	if (p_self == NULL)
	{
		return 0;
	}

	switch (signal.signal)
	{
	case OS_CTRL_C_SIGNAL :
	case OS_CTRL_CLOSE_SIGNAL :
	case OS_CTRL_BREAK_SIGNAL :
	case OS_CTRL_SHUTDOWN_SIGNAL :
		DBG_PRINTF((LOG_DEBUG,"D:" MODULE_TAG "Signal '0x%x' received. Sending 'Shutdown cmd'.\n", signal));
		ret_flag = 1;
		signal_event(CMD_STOP);
		break;

	case OS_CTRL_LOGOFF_SIGNAL :
	default:
		DBG_PRINTF((LOG_DEBUG,"D:" MODULE_TAG "Signal '0x%x' received. NO ACTION.\n", signal));
	}
	return ret_flag;
}

int service_event_handler(SERVICE_EVENT service_event)
{

	/*trigger dyn_dns_main return*/

	DBG_PRINTF((LOG_DEBUG, "D:" MODULE_TAG "Entered service_event_handler.  Setting return signal...\n"));

	/*
		this is also set via call to threads
		exit routine, in main loop on return
		from updating and/or waiting sleep
		before update.  setting here as well
		could get online status alert threads
		to return sooner.  the main possible
		bottleneck on program quit is if in
		middle of a network comm --	an update,
		or do_dyndns_is_online online status
		check -- socket routines are blocking
		type.
	*/


	DBG_PRINTF((LOG_DEBUG, "D:" MODULE_TAG "Setting thread exit...\n"));

	signal_event(service_event);

	return 1;
}

void dyn_dns_update_ip_handler(RAS_THREAD_DATA *p_ras_thread_data)
{

        DYN_DNS_CLIENT     *p_dyndns=p_ras_thread_data->p_context;

        if (p_ras_thread_data->dwEvent==RAS_CONNECT) {

		signal_event(CMD_UPDT);
        }
        else {

		DBG_PRINTF((LOG_DEBUG,"D:" MODULE_TAG "Not an event of interest...\n"));
        }
}
#endif
