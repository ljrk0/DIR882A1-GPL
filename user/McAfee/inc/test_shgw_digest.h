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
 * test_shgw_digest.h
 * */

#pragma once

void test_generate_sha256_digest_sha_bad_arg(void **state);
void test_shgw_digest_sha256_indata_is_null_fail(void **state);
void test_shgw_digest_sha256_indata_size_is_null_fail(void **state);
void test_generate_sha256_digest_sha_init_failed(void **state);
void test_generate_sha256_digest_sha_update_failed(void **state);
void test_generate_sha256_digest_sha_final_failed(void **state);
void test_generate_sha256_digest_success(void **state);
void test_shgw_encode_base16(void **state);
void test_shgw_encode_base16_malloc_fail(void **state);
void test_calc_key_for_hmac_BAD_ARGUMENT_c1_fail(void **state);
void test_calc_key_for_hmac_BAD_ARGUMENT_c2_fail(void **state);
void test_calc_key_for_hmac_BAD_ARGUMENT_c3_fail(void **state);
void test_calc_key_for_hmac_MEMORY_ALLOC_FAILED_c1_fail(void **state);
void test_calc_key_for_hmac_if3_c1_fail(void **state);
void test_calc_key_for_hmac_if3_c2_fail(void **state);
void test_calc_key_for_hmac_success(void **state);
void test_string_to_bit_array_BAD_ARGUMENT_fail(void **state);
void test_string_to_bit_array_data_bits_alloc_fail(void **state);
void test_string_to_bit_array_success(void **state);
void test_shgw_calc_partner_secret_nonce_is_null_router_id_fail(void **state);
void test_shgw_calc_partner_secret_nonce_is_non_null_router_id_is_null_fail(void **state);
void test_shgw_calc_partner_secret_nonce_is_non_null_router_id_fail(void **state);
void test_shgw_calc_partner_secret_get_secret_is_non_null_nonce_is_null_fail(void **state);
void test_shgw_calc_partner_secret_get_secret_secret_is_null_nonce_is_null_fail(void **state);
void test_shgw_calc_partner_secret_get_secret_asprintf_data_is_null_nonce_is_null_fail(void **state);
void test_shgw_calc_partner_secret_get_secret_asprintf_data_is_non_null_nonce_is_null_fail(void **state);
void test_shgw_calc_partner_secret_shgw_get_secret_key_bits_null_nonce_is_null_fail(void **state);
void test_shgw_calc_partner_secret_calc_key_for_hmac_nonce_is_null_fail(void **state);
void test_shgw_calc_partner_secret_HMAC_digest_is_non_null_shgw_encode_base16_hmac_nonce_is_null_fail(void **state);
void test_shgw_calc_partner_secret_shgw_encode_base16_hmac_nonce_is_null_success(void **state);
