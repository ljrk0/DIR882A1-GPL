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

/* This file is the header file for the CSP Enrollment
 * It lists all the headers and the function declarations used
 *
 * Author : Anjana
 * Date : 4th Nov 2015
 */

#ifndef CSP_ENROLLMENT_H
#define CSP_ENROLLMENT_H

#include "time.h"
#include "service_discovery_client.h"
#include "shgw_utils.h"

#define CLIENT_ENROLL_SIZE 12

char *prepare_enrollment_request_data(char *hw_id);

STATUS_CODE store_enrollment_data(char *data, long *get_enrollment_ttl);
STATUS_CODE csp_enroll(char *client_id);

STATUS_CODE get_device_id();
STATUS_CODE get_csp_service_discovery_urls(const char *service, SD_FLAGS discovery_flag, Execution_Indexes current_state, char *client_id);
STATUS_CODE get_enrolled_client_id(char **client_id);

#ifdef UNIT_TEST
PRIVATE void __csp_cloud_respond_client_enroll_cb(int resp_code, int resp_size, char *resp_data, void* userp, char *header_data);
#endif
#endif
