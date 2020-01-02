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

/* This header file contains all declarations corresponding to the fuinctions in generate_security_tokens.c
 *
 * Author : Anjana
 *
 * Date : 5th NOV 2015
 */

#ifndef GENERATE_SECURITY_TOKENS_H
#define GENERATE_SECURITY_TOKENS_H

#include "shgw_digest.h"
#include "form_data_for_token_generation.h"
#include "shgw_utils.h"


STATUS_CODE generate_security_token(char *method, char *body,
                char **machine_name, char **ip_address, char **verification_token,struct tm *time_ptr);
STATUS_CODE get_aes_keys(unsigned char *key, unsigned char *IV);
unsigned char *generate_sha256_digest(char *data);
STATUS_CODE append_random_number(unsigned char *hash, unsigned char **plain_text);
STATUS_CODE encrypt_data(unsigned char *plaintext, int plaintext_len, unsigned char *key,
		unsigned char *iv,unsigned char *ciphertext, int *ciphertext_len);
void generate_random_number(char *random_string);
char *encode_to_base64(const unsigned char *data, int input_length);


#endif
