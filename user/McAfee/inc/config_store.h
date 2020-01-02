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
 * config_store.h
 * */

#pragma once

#include <sqlite3.h>
#include "error_codes.h"
#include "shgw_utils.h"

typedef struct _config_store_t {
	char* table;
	sqlite3* connection;
} config_store_t;

STATUS_CODE config_store_init(const char* filename, const char* table,
		config_store_t* pcnf_store);
STATUS_CODE config_store_deinit(config_store_t* cnf_store);
STATUS_CODE shgw_cfg_get_str(config_store_t* cnf_store, const char* key,
		char** ppvalue);
STATUS_CODE config_store_get_int(config_store_t* pcnf_store, const char* key,
		int *ppvalue);
STATUS_CODE shgw_cfg_init(const char *filename, void *pdata,
		config_store_t *cstore, const char *g_tablename);
STATUS_CODE shgw_cfg_get_url_str(config_store_t* cnf_store, const char* key,
		char** ppvalue);

#ifdef UNIT_TEST
PRIVATE int _sqlite_busy(void* pv, int nattempt);
PRIVATE int _sqlite_callback(void* ptoken, int argc, char** argv, char** errmsg);
#endif

