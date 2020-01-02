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

#ifndef DNSPROXY_RPC_PROXY_H
#define DNSPROXY_RPC_PROXY_H

#include "error_codes.h"
#include "shgw_logger.h"
#include "shgw_rpc_client_impl.h"
#include "shgw_common.h"

#define DNSPROXY_STOP					1
#define DNSPROXY_UPDATE_LOG_LEVEL			2
#define UPDATE_DNSPROXY_CONFIG				3
#define REFRESH_DEVICE_DATA				4
#define INVALIDATE_CACHE				5
#define CONTINUE_TO_SITE	            		6
#define SHGW_BLOCK_DEVICE				7
#define	SHGW_UNBLOCK_DEVICE				8
#define SHGW_WHITELISTED_DOMAIN_CHECK			9
#define	SHGW_NETWORK_UNPAUSE				10
#define SHGW_TLD_CHECK					11

/*
 * This function terminates dns-proxy
 */
STATUS_CODE shgw_stop_dnsproxy(const char *cmd_data,
	struct event_base* base_event, shgw_rpc_status_cb_t status_cb, void* cb_arg);

/*
 *  This function changes the log level of dnsproxy
 *  The char pointer has to have a valid log level number
 */
STATUS_CODE shgw_change_dnsproxy_log_level(const char *cmd_data,
	struct event_base* base_event, shgw_rpc_status_cb_t status_cb, void* cb_arg);

/*
 * Updates the config within the process memory from the db
 */
STATUS_CODE shgw_update_dnsproxy_config(struct event_base* base_event);

/*
 * refresh the device id cache
 */
STATUS_CODE shgw_refresh_device_data(struct event_base* base_event);

/*
 * Invalidates the policy caches for devices.
 * Data has to be an array of valid devids for which the cache needs to be invalidated
 * An empty array means 'All Devices'
 */
STATUS_CODE shgw_invalidate_policy_cache(const char *_cmd_data,
	struct event_base* base_event, shgw_rpc_status_cb_t status_cb, void* cb_arg);

STATUS_CODE shgw_continue_to_site_data(const char *_cmd_data,
        struct event_base* base_event, shgw_rpc_status_cb_t status_cb, void* cb_arg);

STATUS_CODE shgw_block_device(const char *_cmd_data,
        struct event_base* base_event, shgw_rpc_status_cb_t status_cb, void* cb_arg);

STATUS_CODE shgw_unblock_device(const char *_cmd_data,
        struct event_base* base_event, shgw_rpc_status_cb_t status_cb, void* cb_arg);

STATUS_CODE shgw_update_whitelisted_domains(struct event_base* base_event);

STATUS_CODE shgw_network_unpause(const char *_cmd_data,
        struct event_base* base_event, shgw_rpc_status_cb_t status_cb, void* cb_arg);

STATUS_CODE shgw_update_tld(struct event_base* base_event);
#endif /* DNSPROXY_RPC_PROXY_H */
