#ifndef _SHG_CSP_EVENT_TIMER_H
#define _SHG_CSP_EVENT_TIMER_H

#include "shgw_logger.h"
#include "shgw_common.h"
#include "csp_security_store.h"
#include <event2/event.h>

#define ONE_MINUTE	60
#define TIME_BEFORE_TTL_EXPIRY 60

void __setup_csp_ttls(struct event_base* base_ev, long ttl_expiry, Execution_Indexes current_state);

#endif
