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

#ifndef __SHGW_UTILS_TEST
#define __SHGW_UTILS_TEST


void test_shgw_get_time_since_epoch_time_fails(void **state);
void test_shgw_get_time_since_epoch_time_all_ok(void **state);
void test_shgw_malloc(void **state);
void test_shgw_calloc(void **state);
void test_shgw_realloc(void **state);

void test_shgw_check_ip_for_all_zeros_fail(void **state);
void test_shgw_check_ip_for_all_zeros1_fail(void **state);
void test_shgw_check_ip_for_all_zeros2_fail(void **state);
void test_shgw_check_ip_for_all_zeros3_fail(void **state);
void test_shgw_check_ip_for_all_zeros_success(void **state);
void test_get_mac_without_colon_shgw_malloc_success(void **state);
void test_get_mac_without_colon_shgw_while_mac_id_success(void **state);
void test_get_mac_without_colon_mac_null(void **state);
void test_shgw_check_version_strcmp_fail(void **state);
void test_shgw_check_version_strcmp_success(void **state);
void test_sqlite_callback_argv_success(void **state);
void test_sqlite_callback_argv_failure(void **state);
void test_sqlite_callback_res_failure(void **state);
void test_shgw_calc_hashed_mac_shgw_digest_sha256_success(void **state);
void test_shgw_calc_hashed_mac_shgw_digest_sha256_failure(void **state);
void test_shgw_calc_hashed_mac_hashed_mac_not_null(void **state);
void test_get_csp_clientid_from_db_sql_query_null(void **state);
void test_get_csp_clientid_from_db_sql_query_not_null(void **state);
void test_get_csp_clientid_from_db_shgw_query_sqlite_success(void **state);
void test_on_shp_url_change_if_one_failure(void **state);
void test_on_shp_url_change_if_two_failure(void **state);
void test_on_shp_url_change_if_three_failure(void **state);
void test_on_shp_url_change_success(void **state);
#endif

