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
 * shgw_get_router_id.h
 * */

#pragma once

#include "error_codes.h"
#include "shgw_common.h"

STATUS_CODE shgw_get_secret_key_bits(int **bits_arr, int *arr_len);
STATUS_CODE shgw_get_partner_secret(char **partner_secret);
STATUS_CODE shgw_get_router_id(char **router_id);
STATUS_CODE shgw_get_access_token(char **router_id);
STATUS_CODE shgw_get_access_token_expiry(char **access_token);
STATUS_CODE shgw_get_refresh_token(char **refresh_token);
STATUS_CODE shgw_get_base_mac(char **base_mac);
STATUS_CODE shgw_get_potential_edns_ips(char **edns_potential_server_ips);
STATUS_CODE shgw_get_ipv4_addr_from_db(char **ip,char *device_id);
STATUS_CODE shgw_get_is_shgw_stopped(int* is_shgw_stopped);
STATUS_CODE shgw_get_ips_for_dev_id(const char* device_id, char **ipv4_addr, char**ipv6_addr);
STATUS_CODE get_device_mac_from_db(const char *device_id, char **mac);
STATUS_CODE shgw_get_cloud_ttl(const char *field, long *ttl);
STATUS_CODE shgw_get_etag(const char *field_name, char **etag_value);
STATUS_CODE shgw_get_inbound_security_flag(int *inbound_security_enable);

STATUS_CODE shgw_get_cloud_ttl_sample2(long *ttl);
STATUS_CODE shgw_get_cloud_ttl_sample1(long *ttl);

#ifdef UNIT_TEST

PRIVATE int _sqlite_cb(void *data, int argc, char **argv, char **azColName);

#endif
