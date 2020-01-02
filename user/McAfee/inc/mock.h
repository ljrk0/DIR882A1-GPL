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

#ifndef _UTILS_MOCK_H
#define _UTILS_MOCK_H

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "error_codes.h"
#include "shgw_http_request.h"

time_t __wrap_time(time_t *timer);
int __wrap_utimes(const char *filename, const struct timeval times[2]);
FILE* __wrap_fopen(const char* file_name, const char* mode);
int __wrap_fclose(FILE* fd);
int __wrap_event_del(struct event *);
STATUS_CODE __wrap_update_config_db(const char *key, const char *value);
#endif // _UTILS_MOCK_H
