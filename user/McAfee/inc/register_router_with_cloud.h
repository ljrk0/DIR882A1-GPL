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

#ifndef REGISTER_ROUTER_WITH_CLOUD_H
#define REGISTER_ROUTER_WITH_CLOUD_H

#include "router_registration.h"
#include "shgw_json.h"

/* Temp Key Used By Cloud - Needs Change */
#define SHGW_PARTNER_SECRET_KEY "abc"

STATUS_CODE prepare_request_data(char *router_id, char *param_name, char **request_data);

STATUS_CODE form_request_headers(struct curl_slist **headers);

STATUS_CODE form_and_send_rr_request_to_cloud(char *data, char *param_name, char *url);

STATUS_CODE generate_and_store_routerid();


#ifdef UNIT_TEST
PRIVATE STATUS_CODE get_string_value(shgw_json_node * cloud_resp, const char *key, char **dest);
PRIVATE STATUS_CODE parse_resp_update_db(char *resp_data);
PRIVATE void __rr_cloud_resp_cb(int resp_code, int resp_size, char *resp_data, void* userp, char *header_data);
PRIVATE STATUS_CODE check_nonce_and_retry(char *resp_data);
PRIVATE STATUS_CODE fill_additional_regestraion_details(shgw_json_node *json_request_object);
#endif

#endif
