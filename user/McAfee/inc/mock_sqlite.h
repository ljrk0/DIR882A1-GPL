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
 * mock_sqlite.h
 * */

#pragma once

#include "sqlite3.h"

int __wrap_sqlite3_open(const char *filename, sqlite3 **ppDb);
const char *__wrap_sqlite3_errmsg(sqlite3* db);
int __wrap_sqlite3_close(sqlite3* db);
void __wrap_sqlite3_free(void* data);
char *__wrap_sqlite3_mprintf(const char* data, ...);

int __wrap_sqlite3_exec(sqlite3* db, const char *sql,
		int (*callback)(void* data, int argc, char** argv, char** azColName),
		void *callback_data, char **errmsg);

int sqlite3_busy_handler(sqlite3*, int(*)(void*,int), void*);

