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

/* This Header file contains all declarations corresponding to the functions in form_data_for_token_generation.c
 *
 * Author : Anjana
 *
 * Date : 5th Nov 2015
 */

#ifndef FORM_DATA_FOR_TOKEN_GENERATION_H
#define FORM_DATA_FOR_TOKEN_GENERATION_H

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "csp_params.h"
#include "error_codes.h"
#include "shgw_logger.h"
#include "shgw_utils.h"

STATUS_CODE get_hostname(char **machine_name);

struct tm *get_current_time();

STATUS_CODE get_request_date(char **req_date, char *date_format);

STATUS_CODE get_ip_address(char **ip);

STATUS_CODE get_data(char **data, char *method, char *body, char **machine_name,
		char **ip_address, struct tm *time_ptr);

STATUS_CODE get_request_date_in_header(char **req_date_in_header,
		struct tm *time_ptr);

STATUS_CODE get_request_date_in_token(char **req_date, struct tm *time_ptr);

#endif
