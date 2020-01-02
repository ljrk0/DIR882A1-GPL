#ifndef _SHG_CSP_H
#define _SHG_CSP_H

#include "shgw_logger.h"
#include "shgw_common.h"
#include "csp_security_store.h"
#include <event2/event.h>

#define CSP_RUN_INTERVAL 60
#define CSP_CLOUD_URL_DEVICE_DISC		"cloud_url_device_discovery"
#define CSP_ENROLLMENT					"CSP_Enrollment"


typedef enum {
	SERVICE_DISCOVER_CSP,
	SERVICE_DISCOVER_SHP
} SD_FLAGS;


void state_run_client_enroll(state_machine_data_t *state_machine_data);
void state_run_csp_sd(state_machine_data_t *state_machine_data);
void state_run_shp_sd(state_machine_data_t *state_machine_data);
STATUS_CODE csp_init(state_machine_data_t *state_machine_data);
void csp_deinit();
#endif
