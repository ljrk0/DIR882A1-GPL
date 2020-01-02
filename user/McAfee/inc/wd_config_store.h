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

#ifndef __COMMAND_MANAGER_STORE_H
#define __COMMAND_MANAGER_STORE_H

#include "error_codes.h"

typedef struct _command_manager_data{
	char *comm_mgr_api_endpoint;
	char *comm_mgr_status_endpoint;
	char *wd_stats_url;
	char *wd_critical_url;
	char *wd_version_url;
	char *potential_edns_ports;
	char *edns_server_ip;
	char *potential_edns_server_ips;
	int   log_level;
	int   stats_timer_interval;
	int   edns_cm_query_interval;
	int   shgw_cloud_stop;
	int   edns_server_port;
	char *version;
	char *edns_ip_disc_endpoint;
	char *subscription_check_url;
	char *inbound_security_url;
	char *whitelisted_domain_url;
	char *tld_url;
	int is_subscription_valid;
	int inbound_security_enable;
	int device_block_enable;
	char *log_upload_url;
	char *file_name;
	char *rotate_strategy;
	int   file_size;
	int   rotate_limit;
	int   sensitivity_level;
	int upload_debug_duration;
}wd_config_data_t;

typedef struct _wd_config_in_mem_data{
	int   shgw_wd_curr_stopped;
}wd_config_in_mem_data_t;

STATUS_CODE shgw_wd_config_init(const char* filename, wd_config_data_t* pdata);
void shgw_wd_config_deinit(wd_config_data_t* pdata);

#endif // __COMMAND_MANAGER_STORE_H
