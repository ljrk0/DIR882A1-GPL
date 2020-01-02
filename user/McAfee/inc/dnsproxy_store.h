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

//#pragma once
#ifndef DNSPROXY_STORE_H
#define DNSPROXY_STORE_H

#include "error_codes.h"
#include "shgw_utils.h"
#include "config_store.h"
#include <sys/time.h>

#define SHGW_IPv4_ADDR_LEN						4        /* Length of IPv4 addr */
#define DEFAULT_POLICY_RESPONSE_TIMEOUT			2000 		 /* Milliseconds */
#define DEFAULT_POLICY_CACHE_LOAD_FACTOR		10

typedef struct _dnsproxy_data_t {
		char* version;
        int shg_ttl;
        int ttl_for_local_domain;
        int dns_server_port;
        int edns_server_port;
        int dnsproxy_log_level;
        struct timeval edns_policy_response_timeout;
        struct timeval dns_isp_response_timeout;
        int policy_control_enable;
        int policy_cache_enable;
        int dns_cache_enable;
        int policy_cache_max_domains;
        int policy_cache_load_factor;
        int stats_timer_interval;
        int devcache_timeout;
        char* dns_server_ip;
        char* edns_server_ip;
        char* dns_request_port;
        char* local_http_domains;
        char* whitelisted_domains;
        char* redirector_map;
        char* lan_interfaces;
        unsigned char resolved_local_ip[SHGW_IPv4_ADDR_LEN];
        int local_http_domains_size;
        char **local_http_domain;
        int shgw_dns_retry_time;
} dnsproxy_data_t;

STATUS_CODE dnsproxy_store_init(const char* filename, dnsproxy_data_t* pdata);
STATUS_CODE dnsproxy_store_deinit(dnsproxy_data_t* pdata);

#endif
