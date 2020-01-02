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
 * convenient function for reading CSP Security parameters
 * */

#pragma once

#include "error_codes.h"
#include "shgw_utils.h"

typedef struct _csp_data
{
	char* app_key;
	char* shared_key;
	int csp_log_level;
	char* locale;
	char* affid;
	char* country_code;
} csp_data_t;

STATUS_CODE csp_security_store_init(const char* filename, csp_data_t* pdata);
STATUS_CODE csp_security_store_deinit(csp_data_t* pdata);
