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

/* This file contains all the csp related data
 *
 * TODO : This data should be moved to config file
 *
 * Author : Anjana
 * Date : 5th Nov 2015
 */

#ifndef CSP_PARAMS_H
#define CSP_PARAMS_H

#include "csp_security_store.h"

#define RANDOM_DIGITS 		6
#define MAC_ADDRESS_SIZE 	18
#define SALT_LEN			 12
#define ITERATIONS 			1000
#define KEY_LEN 			32
#define IV_LEN 				16
#define RANDOM_LENGTH 		6
#define DATE_FORMAT_FOR_HEADER "%04d-%02d-%02dT%02d:%02d:%02d.%03d"
#define DATE_FORMAT_FOR_TOKEN "%04d%03d$%02d#,%02d.%02d%02d-%02d"

csp_data_t	g_csp_config;

#endif

