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
 * shgw_digest.h
 * */

#pragma once

#include "error_codes.h"

STATUS_CODE shgw_encode_base16(const unsigned char* indata, int indata_size, unsigned char** outdata);
STATUS_CODE shgw_digest_sha256(const char *data, int size, unsigned char** hash);
STATUS_CODE shgw_calc_partner_secret(char **partner_secret, char **nonce);
STATUS_CODE _calc_key_for_hmac(int *secret_indices, char *data_bits, int len, char **hmac_key);
STATUS_CODE _string_to_bit_array(char *data, char **data_bits);
STATUS_CODE shgw_calc_partner_secret(char **partner_secret, char **nonce);

