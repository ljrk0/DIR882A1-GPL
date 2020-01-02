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

/* This header file contains all declarations corresponding to the fuinctions in csp_sqlite_wrapper.c
 *
 * Author : Anjana
 *
 * Date : 5th NOV 2015
 */

#ifndef CSP_SQLITE_WRAPPER_H
#define CSP_SQLITE_WRAPPER_H

#include "sqlite3.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "shgw_logger.h"
#include "error_codes.h"
#include "csp_params.h"
#include "shgw_utils.h"
#include "shgw_query_sqlite.h"

#define CSP_SERVICE_DISCOVERY "csp_service_discovery"
#define CSP_ENROLLMENT_DATA "csp_enrollment_data"

STATUS_CODE delete_csp_data_from_db(char *table);

STATUS_CODE get_sd_ttl_from_db(const char* service, long* ttl);

char *get_server_url_from_db(const char* server_name);

char *get_enrollment_nonce_from_db();

char *get_client_id_from_db();

STATUS_CODE get_enrollment_ttl_from_db(long* ttl);

int sqlite_callback(void *data, int argc, char **argv, char **azColName);

STATUS_CODE update_shp_ttl_in_db(const char *service, const char *ttl);

#endif

