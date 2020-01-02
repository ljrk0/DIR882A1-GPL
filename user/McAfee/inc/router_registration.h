/* router_registration.h */

#ifndef ROUTER_REGISTRATION
#define ROUTER_REGISTRATION

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "event2/event.h"
#include "sqlite3.h"
#include "error_codes.h"
#include "shgw_logger.h"
#include "shgw_common.h"
#include "router_registration_store.h"

#define RR_RETRY_INTERVAL 60
#define TIME_BEFORE_TOKEN_EXPIRY 300

typedef enum __RR_RUN_MODE {
	RUN_RR_FRESH,
	RUN_RR_REFRESH,
}RR_RUN_MODE;

typedef struct _rr_details
{
	char	*access_token;
	char	*refresh_token;
	long	expiry;
}rr_details_t;

router_registration_data_t g_config;

void state_run_rr(state_machine_data_t *state_machine_data);
void _rr_cb(evutil_socket_t sock_fd, short what, void *data);


STATUS_CODE do_rr(RR_RUN_MODE run_mode);
STATUS_CODE init_rr_timer(state_machine_data_t *state_machine_data, long *rerun_time); 


#ifdef UNIT_TEST
PRIVATE STATUS_CODE run_router_registration(long *rerun_duration);
PRIVATE void _timer_cb(evutil_socket_t fd, short event, void *arg);
#endif

#endif /* ROUTER_REGISTRATION */
