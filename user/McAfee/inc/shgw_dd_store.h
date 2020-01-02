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
 * convenient function for reading dnsproxy params
 * */

#pragma once

#include "error_codes.h"
#include "shgw_utils.h"
#include "config_store.h"

typedef struct _shgw_dd_data_t {
		char* version;
		int device_discovery_time_interval;
		int device_inactivity_timeout;
		char* cloud_url_device_discovery;
		int device_discovery_log_level;
		struct timeval device_upload_interval;
		char* lan_interfaces;
		char* device_discovery_protocols;
		char *potential_upload_triggers;
		int limit_ipv6_address;

} shgw_cfg_dd_data_t;

STATUS_CODE shgw_cfg_dd_store_init(const char* filename,
	shgw_cfg_dd_data_t* pdata);
STATUS_CODE shgw_cfg_dd_store_deinit(shgw_cfg_dd_data_t* pdata);
