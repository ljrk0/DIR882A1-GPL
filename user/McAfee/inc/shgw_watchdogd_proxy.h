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

/*
 * shgw_watchdogd_proxy.h
 * */

#pragma once

#include <event2/event.h>
#include "error_codes.h"

#define SHGW_WATCHDOGD_RPC_PORT 	"/var/watchdogd_rpc_port"

#define SHGW_WATCHDOGD_REPORT_ERROR 	0
#define SHGW_CLOUD_AUTH_ERROR_HANDLER	1
#define SHGW_POD_FAILOVER_HANDLER		2
#define SHGW_TRIGGER_CMD_MGR			3
#define SHGW_DEVICE_PROP_CHANGED		4

/*
 * Router's critical error codes starts from 200
 * Please check with the cloud team for latest range
 * Notify the cloud team when adding a new code to this
 */
typedef enum {
	/* Device Discovery related codes */
	SHGW_DD_DHCP_FAILED = 200,		/* 200 */
	SHGW_DD_DEFAULT_FEATURE_FLAG = 201,

	/* Port Config */
	SHGW_PORT_IDENTIFICATION_FAILED = 220,	/* 220 */
	SHGW_EDNS_IP_IDENTIFICATION_FAILED,

	/* WD Related */
	SHGW_PROCESS_TERMINATED	= 240,
	SHGW_PROCESS_RESTART_LIMIT_REACHED,
} SHGW_CRITICAL_EVENT;

STATUS_CODE shgw_report_error(SHGW_CRITICAL_EVENT err_code, struct event_base* base_event);
STATUS_CODE shgw_cloud_auth_error_handler(struct event_base* base_event);
STATUS_CODE shgw_pod_failover_handler(struct event_base* base_event);
STATUS_CODE shgw_trigger_comm_mgr(struct event_base* base_event);
STATUS_CODE shgw_device_property_changed(char *dev_id, struct event_base* base_event);
