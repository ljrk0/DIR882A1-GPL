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

#ifndef HTTPD_PROXY_H
#define HTTPD_PROXY_H

#include "error_codes.h"
#include "shgw_logger.h"
#include "shgw_rpc_client_impl.h"

#define HTTPD_STOP					1
#define HTTPD_UPDATE_LOG_LEVEL		2
#define HTTPD_UPDATE_CONFIG			3

/*
 * This function terminates shgw_httpd
 */
STATUS_CODE shgw_stop_httpd(const char *cmd_data,
	struct event_base* base_event, shgw_rpc_status_cb_t status_cb, void* cb_arg);

/*
 *  This function changes the log level of httpd
 *  The char pointer has to have a valid log level number
 */
STATUS_CODE shgw_change_httpd_log_level(const char *cmd_data,
	struct event_base* base_event, shgw_rpc_status_cb_t status_cb, void* cb_arg);

/*
 * Updates the config within the process memory from the db
 */
STATUS_CODE shgw_update_httpd_config(struct event_base* base_event);

#endif /* HTTPD_PROXY_H */
