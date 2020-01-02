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
 * Author : Eswar
 */
#include "error_codes.h"

#ifndef LOGGER_LOGGER_H
#define LOGGER_LOGGER_H

/* Trusting the client to provide legal format strings */

enum log_levels{
    CRITICAL,
    ERROR,
    WARNING,
    INFO,
    DEBUG,
    /* Max number of log levels. don't add a level after this line */
    MAX_LOG_LEVEL
};

enum conf_level{
    HIGH,
    MEDIUM,
    LOW,
    /* Max number of conf levels. dont add a level after this line */
    MAX_CONF_LEVEL
};

typedef int LOG_LEVEL;

/*
 * Call log_init to set log file name, module name
 * call log_destroy and log_init to modify one of those
 */
STATUS_CODE shgw_log_init(const char *log_file_name, const char *module_name, int log_level);

/*
 * update the log level to provided value. use above enum constants
 */
void shgw_update_log_level(LOG_LEVEL _level);

/*
 * Destroys the open handles of files
 */
STATUS_CODE shgw_log_destroy();

/* Logging functions */
void shgw_critical(int id, int confidence_level, const char* fmt, ...);
void    shgw_error(int id, int confidence_level, const char* fmt, ...);
void  shgw_warning(int id, int confidence_level, const char* fmt, ...);
void     shgw_info(int id, int confidence_level, const char* fmt, ...);
void    shgw_debug(int id, int confidence_level, const char* fmt, ...);

#endif //LOGGER_LOGGER_H
