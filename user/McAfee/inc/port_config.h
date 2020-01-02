/*****************************************************************************
 *
 * INTEL CONFIDENTIAL
 * Copyright (c) 2016 Intel Corporation All Rights Reserved
 *
 * The source code contained or described herein and all documents related
 * to the source code ("Material") are owned by Intel Corporation or its
 * suppliers or licensors. Title to the Material remains with Intel
 * Corporation or its suppliers and licensors. The Material contains trade
 * secrets and proprietary and confidential information of Intel or its
 * suppliers and licensors. The Material is protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Material may
 * be used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Intel's prior
 * express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or
 * delivery of the Materials, either expressly, by implication, inducement,
 * estoppel or otherwise. Any license under such intellectual property rights
 * must be express and approved by Intel in writing.
 *
****************************************************************************/

#ifndef PORT_CONFIG_H_
#define PORT_CONFIG_H_

#include <event2/event.h>
#include "error_codes.h"
#include "shgw_watchdogd_proxy.h"
#include "shgw_common.h"

#define EDNS_PORT_KEY_STRING	"edns_server_port"

/* microseconds */
#define RESPONSE_TIMEOUT		2

#define MAX_PORT_STRING_SIZE	6

typedef struct _ev_result_t{
	short gp_result;
	short bp_result;
}ev_result_t;

typedef struct _ev_data{
	int         expected_result;
	struct event*	io;
	int			port;
	ev_result_t	*ev_result;
}ev_data_t;

typedef struct _port_config{
	int 		identified_port;
	int 		num_of_ports;
}port_config_t;

STATUS_CODE init_events(int good_packet_fd, int bad_packet_fd, int port);
STATUS_CODE probe_edns_port(const char *edns_server_ip, int port);
void state_run_edns_port_discovery(state_machine_data_t *state_machine_data);
void init_port_retry_event();
STATUS_CODE update_port_in_db(int port);
STATUS_CODE iterate_and_identify_port(const char *potential_edns_ports, const char *edns_server_ip);
STATUS_CODE check_edns_response(evutil_socket_t fd, ev_data_t* ev_data);
void edns_response_cb(evutil_socket_t fd, short event, void *arg);
void close_fds(int good_packet_fd, int bad_packet_fd);
void retry_port_discovery_state();

#endif /* PORT_CONFIG_H_ */
