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
 * query_sqlite.h
 * */

#pragma once

#include <sqlite3.h>
#include "error_codes.h"
#include "shgw_common.h"

typedef enum
{
	SHGW_QT_CREATE,
	SHGW_QT_DELETE,
	SHGW_QT_INSERT,
	SHGW_QT_SELECT,
	SHGW_QT_UPDATE
} query_type;

/* sqlite callback */
typedef int (*shgw_sqlite_cb_t)(void *data, int argc, char **argv, char **azColName);

/* public methods */
STATUS_CODE shgw_query_sqlite(const char* db_path, char* query, query_type type, shgw_sqlite_cb_t callback, void* cb_data);

STATUS_CODE update_config_db(const char *key, const char *value);

#ifdef UNIT_TEST

PRIVATE int _sqlite_busy(void* pv, int nattempt);

#endif
