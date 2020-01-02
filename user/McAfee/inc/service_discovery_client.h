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

/* This header file contains all declarations corresponding to the fuinctions in service_discovery_client.c
 *
 * Author : Anjana
 *
 * Date : 5th NOV 2015
 */

#ifndef SERVICE_DISCOVERY_CLIENT_H
#define SERVICE_DISCOVERY_CLIENT_H

#include <stdio.h>
#include "locale.h"
#include "request_csp.h"
#include "csp_sqlite_wrapper.h"
#include "shgw_utils.h"
#include "shgw_json.h"
#include "csp.h"

/*Prepare JSON Request For Service Discovery*/
char *prepare_sd_request_data(char * client_id, SD_FLAGS discovery_flag);

/* Parse and store the response from Service Discovery Api*/
STATUS_CODE parse_and_store_sd_response(char *response, long *sd_csp_ttl);
STATUS_CODE parse_and_store_sd_mgt_cloud_response(char *response, long *sd_mgt_cloud_ttl);
/* Initiates the service discovery request, parse the response */
STATUS_CODE service_discovery(char * client_id, SD_FLAGS discovery_flag);
STATUS_CODE do_csp_async(char* url, char *request_data, shgw_async_http_cb_t user_cb);
char *get_cjson_child_object(shgw_json_node* object, char *child);

#ifdef UNIT_TEST
PRIVATE
STATUS_CODE prepare_sd_query(
		char** pquery,
		const char* primary_url,
		const char* secondary_url,
		const char* ttl,
		const char* url_fragment,
		const char* url_name);
PRIVATE void __csp_cloud_respond_csp_cb(int resp_code, int resp_size, char *resp_data, void* userp, char *header_data);
PRIVATE void __csp_cloud_respond_shp_cb(int resp_code, int resp_size, char *resp_data, void* userp, char *header_data);
#endif

#endif

